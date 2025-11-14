# `break`

!!! abstract "Command information"
    Regular name **`break`** :octicons-unlink-24:

    Introduced in version **v0.2** :octicons-tag-24:

    This command is considered **stable** :octicons-stop-24:

## About

The `break` command is used to list, and enable/disable debugger breakpoints.

## Syntax

The `break` command has **two commands**, when invoked without a command all **public breakpoints** are listed to the terminal.

### Breakpoint locations

Breakpoints can be address by either their

- Numeric ID obtained from running `break`
- File and line number: `src/main.cpp:45`
- Relative breakpoints
    * When a breakpoint location **begins** with a `+` then the **current function** is combined with the breakpoint location to form the **new breakpoint name**.

    * Example: `+alloc=x` becomes `void opcontrol()+alloc=x`

### enable

Interpret and enable the given breakpoint location.
