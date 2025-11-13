# Handles debugger <-> debug server communication and various utils around messaging

import time

import sys
from prompt_toolkit import ANSI, print_formatted_text
from prompt_toolkit.formatted_text import FormattedText
from utils import find_server
from server.protocol import DebuggerMessage,DebuggerMessageType
from cli.colors import Colors
import threading as thread
import subprocess


class DebugServer:
    waits: dict[DebuggerMessageType, thread.Condition]
    wait_results: dict[DebuggerMessageType, list[DebuggerMessage]]

    # Parse incoming messages
    io: thread.Thread

    # Check if the connection to the remote server hung up
    hang: thread.Thread

    # Have we received an OPEN message
    is_open: bool

    # Timestamp of the last OPEN message
    last_open: int

    MESSAGE_TRACE_MAX = 100

    # Messages analyzed by get_msg_range, which automatically clears this.
    # NOTE: If get_msg_range is not called enough it will be auto cleaned by the message IO thread
    message_trace: list[DebuggerMessage]

    def __init__(self, wait_open: bool = True):
        self.server_path = find_server()
        self.breakpoint_tripped = None
        self.is_open = False

        self.last_open = 0

        self.proc = subprocess.Popen([self.server_path], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        self.waits = dict()
        self.wait_results = dict()
        self.message_trace = list()

        if self.proc.poll() != None:
            print("Failed to start and communicate with v5dbg comms server!")
            print("Exit data: " + self.proc.stderr.readline().decode())
            sys.exit(-1)

        self.io = thread.Thread(target=self.io_thread, name="DebugServer IO thread")
        self.io.start()

        self.hang = thread.Thread(target=self.hang_thread, name="DebugServer hang detector")
        self.hang.start()

        print(">> Waiting for signal from user program...")

        if wait_open:
            self.wait_for(DebuggerMessageType.OPEN)

        print(">> Connected to remote user program!")

    # Return True if we are connected to the remote debug server
    def connected(self):
        return self.proc.poll() is None

    """
    Set the handler which executes when a breakpoint is tripped
    """
    def set_breakpoint_trip(self, handle):
        self.breakpoint_tripped = handle

    # Write `buffer` to the remote servers incoming data stream and flush
    def write(self, buffer: str):
        try:
            self.proc.stdin.write(buffer.encode())
            self.proc.stdin.flush()
        except:
            print_formatted_text(FormattedText([
                (Colors.RED, 'Failed to write commands to debug server pipe!')
            ]))

    # Stockpile messages until msg_type is found
    # NOTE: This function blocks until completion, use carefully
    def get_msg_range(self, front: DebuggerMessageType, end: DebuggerMessageType):
        self.wait_for(end)

        # Copy all messages from message history

        trace_reversed = self.message_trace
        trace_reversed.reverse()

        end_index = 0
        front_index = 0

        # Find end index
        x = 0

        for t in trace_reversed:
            if t.msg_type == end:
                end_index = x # Keep improving until we find the latest

            x += 1

        x = 0

        for t in trace_reversed:
            if t.msg_type == front:
                front_index = x # Keep improving until we find the latest

            x += 1

        splice = trace_reversed[end_index:front_index + 1]
        splice.reverse()

        self.message_trace.clear() # Auto clear our message trace so we don't get confused with old messages when we get called again

        return splice


    # Check and see if the remote server has hung up if we don't get an OPEN message for over 5 seconds
    def hang_thread(self):
        self.last_open = int(time.time())

        while self.proc.poll() == None:
            c_time = int(time.time())

            if c_time - self.last_open >= 5:
                print("Warning! DebugServer (local) has not gotten any OPEN messages from the debug server in over 5s, did the debug server freeze?")

            time.sleep(2)

        print_formatted_text(FormattedText([
            (Colors.RED, 'Disconnected from communications server, process exited!')
        ]))

        sys.exit(0)

    # Wait for the next message of the given type to arrive
    def wait_for(self, msg_type: DebuggerMessageType, count: int = -1):
        if msg_type in self.waits:
            raise Exception("Thread already waiting for that message!")

        self.waits[msg_type] = thread.Condition()

        with self.waits[msg_type]:
            self.waits[msg_type].wait(300)

        self.waits.pop(msg_type)

        if count == -1:
            results = self.wait_results[msg_type]
            self.wait_results.pop(msg_type)

            return results
        elif count != len(self.wait_results[msg_type]):
            print("re-run wait_for to meet count")
            return self.wait_for(msg_type, count)
        elif count == len(self.wait_results[msg_type]):
            results = self.wait_results[msg_type]
            self.wait_results.pop(msg_type)

            return results

    # IO thread for reading messages from the debug server (remote)
    def io_thread(self):
        while self.proc.poll() == None:
            b = self.proc.stdout.readline()

            # All messages should be written without COBS & stream multiplexing, so we can just read it raw here
            data = b.decode()
            if len(data) == 0:
                continue

            if data[0] != '%':
                # Ignore v5dbg log messages (for now...)
                if not data.startswith("(v5dbg)") and not data.startswith("(v5dbg-sys-init)") and self.is_open:
                  print_formatted_text(ANSI(f"stdout >> {data}"), end="")

                continue

            msg = DebuggerMessage.deserialize(data)

            if len(self.message_trace) + 1 > self.MESSAGE_TRACE_MAX:
                self.message_trace.clear()

            self.message_trace.append(msg)

            if msg.msg_type == DebuggerMessageType.OPEN:
                self.last_open = int(time.time())
                self.is_open = True
            elif msg.msg_type == DebuggerMessageType.BREAK_INVOKED and self.breakpoint_tripped != None:
                t = thread.Thread(target=self.breakpoint_tripped, args=(msg,), name="Breakpoint trip handler")
                t.start()

            if msg.msg_type in self.waits:
                if not msg.msg_type in self.wait_results:
                    self.wait_results[msg.msg_type] = list()

                self.wait_results[msg.msg_type].append(msg)

                with self.waits[msg.msg_type]:
                    self.waits[msg.msg_type].notify_all()
