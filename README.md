# v5dbg
v5dbg is an experimental real-time debugger for V5 C++ programs

## Authors
- Hunter (8568T)

## About
v5dbg aims to create a powerful debugger for V5 C++ programs running on the brain. While vexOS and PROS offer a limited debugging environment v5dbg is able to reconstruct the callstack, expose memory, and implement basic breakpoints without modification of PROS.

## Terms
- Debug Server
    * This is the program which runs with your bots program on the brain and handles all debug operations

- Debugger
    * This is the program which runs on your host computer and communicates with the debugger over serial. You interact with the debugger to halt the program and inspect state

- Thread
    * Same as `pros::rtos::Task`

- Remote
    * The brain

- Local
    * Your computer

- Breakpoint
    * Location where the program halts execution for state inspection

- vstack
    * Virtual callstack reconstruction with `$function`

- vmem
    * Virtual memory reconstruction with `$expose`
