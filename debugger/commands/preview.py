from cli.colors import Colors
from cli.debug import CommandExecutor, Debugger
from cli.preview import FilePreview
from client.client import DebuggerClient, DebuggerState
from prompt_toolkit import print_formatted_text
from prompt_toolkit.formatted_text import FormattedText
import os

"""
Preview the code around the current frames file and line
"""


class PreviewCommand(CommandExecutor):
    def __init__(self):
        pass

    def register(self, parser):
        parser.add_parser(
            "preview",
            help="Preview the contents of the file around the current frame",
            aliases=["pr"],
        )

    def get_name(self):
        return "preview"

    def execute(self, client: DebuggerClient, debugger: Debugger, command):
        if command.debugger == "preview" or command.debugger == "pr":
            # Only allow preview during suspend

            if client.state & DebuggerState.RUN:
                print_formatted_text(
                    FormattedText(
                        [
                            ("", "Program is not in the "),
                            (Colors.RED, "SUSPEND "),
                            ("", "state"),
                        ]
                    )
                )
                return

            st = client.get_stacktrace(True)
            if len(st) == 0:
                return

            # Obtain last stack frame
            last_frame = st[client.active_thread.frame_index]

            # Check if the file exists
            if not os.path.exists(last_frame.file):
                print_formatted_text(FormattedText([
                    (Colors.RED, f'File {last_frame.file} does not exist in the current directory? Are you in the project folder?')
                ]))

                return

            # Print file preview
            preview = FilePreview(last_frame.file)
            preview.print_region(last_frame.line, 8)
