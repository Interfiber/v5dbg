from client.client import DebuggerClient, DebuggerState
from cli.colors import Colors
from cli.debug import CommandExecutor, Debugger
from prompt_toolkit import print_formatted_text
from prompt_toolkit.formatted_text import FormattedText
from server.protocol import DebuggerVariableSetMode, DebuggerVariableSetResult

"""
Base class for all set-based commands
"""


class SetCommandBase:
    """
    Handle a return code from the debug server

    Return True if the return code is a failure, False if otherwise
    """

    def handle_return_code(exit_code: DebuggerVariableSetResult) -> bool:
        if exit_code == DebuggerVariableSetResult.ALLOCATION_FAILURE:
            print_formatted_text(
                FormattedText(
                    [
                        (
                            Colors.RED,
                            "The debug server buffer allocator failed to allocate your input data",
                        )
                    ]
                )
            )
        elif exit_code == DebuggerVariableSetResult.CONVERSION_FAILURE:
            print_formatted_text(
                FormattedText(
                    [
                        (
                            Colors.RED,
                            "The debug server buffer allocator failed to convert your input string",
                        )
                    ]
                )
            )
        elif exit_code == DebuggerVariableSetResult.NO_ALLOCATOR:
            print_formatted_text(
                FormattedText(
                    [
                        (
                            Colors.RED,
                            "The debug server could not find a suitable allocator registered with $pretty_printer_allocator",
                        )
                    ]
                )
            )
        elif exit_code == DebuggerVariableSetResult.NO_VARIABLE:
            print_formatted_text(
                FormattedText(
                    [
                        (
                            Colors.RED,
                            f"No variable in the current scope with that name, try changing your scope with the `frame` command",
                        )
                    ]
                )
            )
        else:
            return False

        return True

    """
    Generic variable set handling
    """

    def run_set(
        self,
        mode: DebuggerVariableSetMode,
        client: DebuggerClient,
        name: str,
        buffer: str,
    ):
        if client.state != DebuggerState.SUSPEND:
            print_formatted_text(FormattedText([
              (Colors.RED, 'The program must be in the SUSPEND state to set variables')
            ]))

            return
        response_code = client.set_variable(mode, name, buffer)

        if SetCommandBase.handle_return_code(response_code):
            return

        if response_code == DebuggerVariableSetResult.RESET_COPY_BUFFER:
            print_formatted_text(
                FormattedText(
                    [
                        ("", "Disabled constant-copy for "),
                        (Colors.ORANGE, name),
                    ]
                )
            )
        else:
            print_formatted_text(
                FormattedText(
                    [
                        ("", "Set value of variable "),
                        (Colors.ORANGE, name),
                        ("", " to "),
                        (Colors.STEEL, buffer),
                    ]
                )
            )


"""
Handles setting memory within the local frame context (Single)
"""


class SetCommand(CommandExecutor, SetCommandBase):
    def __init__(self):
        pass

    def get_name(self):
        return "set"

    def register(self, parser):
        p = parser.add_parser("set", help="Set the value of a variable")

        p.add_argument(
            "variable_id",
            help="Name of the local variable to set the value of",
            type=str,
            action="store",
        )

        p.add_argument(
            "value_buffer",
            help="Value to set the variable to",
            type=str,
            action="store",
        )

    def next_completion(
        self,
        command: str,
        current_arg: int,
        current_text: str,
        c_index: int,
        client: DebuggerClient,
    ) -> str:
        return None

    def execute(self, client: DebuggerClient, debugger: Debugger, command):
        if command.debugger != "set":
            return
        
        self.run_set(
            DebuggerVariableSetMode.SINGLE,
            client,
            command.variable_id,
            command.value_buffer,
        )


"""
Handles setting memory within the local frame context (Constant)
"""


class SetConstantCommand(CommandExecutor, SetCommandBase):
    def __init__(self):
        pass

    def get_name(self):
        return "setc"

    def register(self, parser):
        p = parser.add_parser(
            "setc", help="Set the value of a variable across reallocations"
        )

        p.add_argument(
            "variable_id",
            help="Name of the local variable to set the value of",
            type=str,
            action="store",
        )

        p.add_argument(
            "value_buffer",
            help="Value to set the variable to",
            type=str,
            action="store",
        )

    def next_completion(
        self,
        command: str,
        current_arg: int,
        current_text: str,
        c_index: int,
        client: DebuggerClient,
    ) -> str:
        return None

    def execute(self, client: DebuggerClient, debugger: Debugger, command):
        if command.debugger != "setc":
            return

        self.run_set(
            DebuggerVariableSetMode.CONST,
            client,
            command.variable_id,
            command.value_buffer,
        )


"""
Disables the constant set operation performed by a 'setc' command
"""


class SetConstantDisableCommand(CommandExecutor, SetCommandBase):
    def __init__(self):
        pass

    def get_name(self):
        return "setcd"

    def register(self, parser):
        p = parser.add_parser(
            "setcd",
            help="Disable the constant set operation performed by a setc command",
        )

        p.add_argument(
            "variable_id",
            help="Name of the local variable to disable a setc on",
            type=str,
            action="store",
        )

    def next_completion(
        self,
        command: str,
        current_arg: int,
        current_text: str,
        c_index: int,
        client: DebuggerClient,
    ) -> str:
        return None

    def execute(self, client: DebuggerClient, debugger: Debugger, command):
        if command.debugger != "setcd":
            return

        # The debug server will see 0x0 and set the reallocation buffer to NULL which disables recopies
        self.run_set(DebuggerVariableSetMode.CONST, client, command.variable_id, "0x0")
