# V5dbg protocol
The v5dbg protocol is a single stream protocol. Once machine listens for data while the other writes.

## Example capture
The client writes a message over serial as a string with the debug server waiting for data. As the IO thread processes in the incoming data its handed off to the debug server thread which performs all program logic.
When data needs to be written back its written over serial to the stdin stream