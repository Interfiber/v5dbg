from enum import IntFlag, auto

from prompt_toolkit import print_formatted_text
from prompt_toolkit.formatted_text import FormattedText
from server.comms import DebugServer
from client.breakpoint import DebuggerBreakpoint
from cli.colors import Colors
from client.stack import StackFrame
from client.thread import DebuggerThread, RawVariableData
from server.protocol import DebuggerMessage, DebuggerMessageType, DebuggerVariableSetMode, DebuggerVariableSetResult


# Debugger state bit flags
class DebuggerState(IntFlag):
    RUN = auto()
    SUSPEND = auto()

"""
Wrapper over a DebugServer to provide an easy way to perform common operations without dealing with message data
"""
class DebuggerClient:
    state: DebuggerState
    active_thread: DebuggerThread
    active_break: DebuggerBreakpoint
    server: DebugServer

    def __init__(self, server: DebugServer):
        self.server = server
        self.state = DebuggerState.RUN
        self.active_break = None

        self.active_thread = DebuggerThread(0, self.server)

    # Send a message to the remote server
    def send_msg(self, message: DebuggerMessage):
        self.server.write(message.serialize())

    """
    Callback used when a breakpoint is tripped
    """

    def break_tripped_handler(self, msg: DebuggerMessage):
        b = DebuggerBreakpoint(msg)

        b.print_tripped()

        # Mark program to be in the suspend state
        self.state |= DebuggerState.SUSPEND
        self.state = self.state & ~DebuggerState.RUN

        # Find current stack frame
        bt = self.get_stacktrace()

        # Plus sign is used to indicate extra data
        backtrace_func = b.function.split("+")

        # Select the given frame in the backtrace that this breakpoint belongs too
        for frame in bt:
            if frame.name == backtrace_func[0]:
                print_formatted_text(
                    FormattedText(
                        [
                            ("", "Moving to frame "),
                            (Colors.CYAN, backtrace_func[0]),
                            ("", " in "),
                            (Colors.ORANGE, str(b.location)),
                        ]
                    )
                )

                self.active_thread.frame_index = frame.id

                break

        print_formatted_text(
            "Program suspended for breakpoint, use 'continue' to resume execution"
        )

        self.active_break = b

    """
    Enable/disable a breakpoint by numerical ID

    Args:
        id (int): ID of the breakpoint to enable/disable
        enabled (bool): True to enable the breakpoint, false if otherwise
    """

    def enable_breakpoint(self, id: int, enabled: bool = True):
        if id < 0:
            print_formatted_text(f"Breakpoint ID must be positive!")
            return None

        msg = DebuggerMessage(DebuggerMessageType.BREAKPOINT_SET_STATUS)
        msg.data = f"{str(id)}:{int(enabled)}"

        self.send_msg(msg)

        return True

    """
    Return the active threads memory object
    """
    def get_memory(self) -> RawVariableData:
        return self.active_thread.get_memory()

    """
    Return the stacktrace for the active thread

    Args:
        inject_breaks (bool): If True any stack frames which match an active breakpoint will be modified to be the breakpoint location
    """
    def get_stacktrace(self, inject_breaks: bool = False) -> list[StackFrame]:
        # Ask debugger for virtual callstack

        b = self.active_thread.get_backtrace()

        if not inject_breaks or self.active_break == None:
            return b

        # Inject breakpoints by changing the frame information where the breakpoint is located
        for frame in b:
            if (
                frame.name
                == self.active_break.function.split("+")[
                    0
                ]  # Get name before the + which denotes extra data
                and frame.file == self.active_break.location.file
            ):
                frame.line = self.active_break.location.line
                break

        return b

    """
    Suspend program execution
    """
    def suspend(self):
        self.state |= DebuggerState.SUSPEND
        self.state = self.state & ~DebuggerState.RUN

        suspend = DebuggerMessage(DebuggerMessageType.SUSPEND)
        self.send_msg(suspend)

    """
    Resume program execution
    """
    def resume(self):
        self.state |= DebuggerState.RUN
        self.state = self.state & ~DebuggerState.SUSPEND
        self.active_break = None

        resume = DebuggerMessage(DebuggerMessageType.RESUME)
        self.send_msg(resume)

    """
    Print out the list of all active threads
    """
    def print_threads(self):
        # Ask for thread listing

        get_threads = DebuggerMessage(DebuggerMessageType.THREADS)
        self.send_msg(get_threads)

        # Wait for thread data
        thread_data = self.server.wait_for(DebuggerMessageType.RTHREADS)
        if len(thread_data) == 0:
            print("Invalid thread ID for threads()")
        else:
            print("Supervised thread listing:")
            print("-----------------------")

            thread_blob = str()

            for x in thread_data:
                if x.msg_type == DebuggerMessageType.RTHREADS:
                    # Store thread data blob
                    thread_blob = x.data
                    break

            thread_blob_split = thread_blob.split(",")
            thread_blob_split.reverse()

            i = 0
            c_thread = False
            for blob in thread_blob_split:
                if i % 2 == 0:
                    if int(blob) == self.active_thread.id:
                        c_thread = True

                    print(f"({blob}) ", end="")
                else:
                    print(f"{blob} {"[Current]" if c_thread else ""}")
                    c_thread = False

                i += 1

    """
    Return the active stack frame
    """

    def get_stack_frame(self) -> StackFrame:
        return self.get_stacktrace()[self.active_thread.frame_index]

    """
    List all breakpoints found so far in the program

    Args:
      list_hidden (bool): If enabled hidden breakpoints are also listed
    """
    def get_breakpoints(self, list_hidden: bool = False):
        list_breaks = DebuggerMessage(DebuggerMessageType.LBREAKPOINTS)
        list_breaks.data = "1" if list_hidden else "0"

        self.send_msg(list_breaks)

        msgs = self.server.get_msg_range(
            DebuggerMessageType.RBREAKPOINT, DebuggerMessageType.END_BREAKPOINTS
        )

        for msg in msgs:
            if msg.msg_type != DebuggerMessageType.RBREAKPOINT:
                continue

            yield DebuggerBreakpoint(msg)

    """
    Set the value of a variable within the current stack frame

    Args:
        mode (DebuggerVariableSetMode): Variable set mode to use
        name (str): Name of the variable to set
        buffer (str): Value to set the variable too, this will be converted into the proper data type by the debug server
    """
    def set_variable(self, mode: DebuggerVariableSetMode, name: str, buffer: str) -> DebuggerVariableSetResult:
        set_msg = DebuggerMessage(DebuggerMessageType.MEMORY_SET)
        set_msg.data = f"[{name}]:[{buffer}]:{self.active_thread.frame_index}:{self.active_thread.id}:{mode}"

        self.send_msg(set_msg)

        # Wait for the debug server response
        response = self.server.wait_for(DebuggerMessageType.RMEMORY_SET)

        return DebuggerVariableSetResult(response[0].data)
