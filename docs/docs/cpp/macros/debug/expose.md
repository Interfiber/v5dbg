---
description: Documentation for the $expose macro which is used to expose memory to thd debugger
icon: octicons/arrow-switch-24
---

# `$expose(T)`

!!! abstract "Version information"
    Introduced in version **v0.1** :octicons-tag-24:

    This macro is considered **stable** :octicons-check-24:

## About

The `$expose` macro allows v5dbg to **expose variables** to the debugger. The exposed variables can be **viewed and modified** later using debugger commands such as [`print`](../../../debugger/cmds/print.md) and [`set`](../../../debugger/cmds/set.md)

??? question "Why do I need to use this macro?"
    V5dbg **cannot see** where local variables are **in memory**, see [the engineering page](../../../debug_server/exposed_memory.md) for more information.

## Notes

!!! danger "Required environment"
    - This macro **requires** the debug server to have been initialized before being called.
    - This macro **requires** the current thread to be supervised with [$ntask](./ntask.md) or `V5Dbg_Init`
    - This macro **requires** the current function to be [debuggable](../debug/function.md4)

??? memory-error "Potential memory error"
    The `$expose` macro allows you to expose memory which **should be marked as const**. For example the following code is valid:

    ```c++
    #include "v5dbg/debug.h"

    void funcWithParams(const std::string &data)
    {
        $function

        $expose(data);
    }

    void opcontrol()
    {
        $ntask
        $function

        funcWithParams("Hello this usually cannot be changed!");
    }
    ```

    Using the debugger you can **set the value** of the `data` parameter even though in standard C++ this would result in a **compiler error**.

    Setting the value will **usually** be fine but it should be classified as **undefined behavior**.

## Examples

=== "Single scope"

    ```c++ hl_lines="11 12 17" linenums="1"
    #include "v5dbg/debug.h"

    void
    opcontrol()
    {
        $ntask
        $function

        int i = 0; // Incrementor

        float randomData = 0;
        $expose(randomData); // (1)

        while (true)
        {
            // Add some junk to our variable
            randomData += (i * 5) / 3.3333;

            pros::delay(300);

            i++;
        }
    }
    ```

    1. Here we allocate a new `float` variable on the stack and **expose** it with `$expose`. We only need to do this when a variable **is allocated** and any changes made to the variable afterwards will be **automatically reflected** in the debugger.

=== "Multiple scopes"

    ```c++ hl_lines="8 22" linenums="1"
    #include "v5dbg/debug.h"

    void
    printAndSleep(const std::string &p)
    {
        $function

        $expose(p); // (1)

        pros::lcd::print(1, "%s", p.c_str()); // Print our data to the screen

        pros::delay(300); // Wait 300ms
    }

    void
    opcontrol()
    {
        $ntask
        $function

        int i = 0;
        $expose(i); // (2)
        while (true)
        {
            printAndSleep("Hello World");

            i++;
        }
    }
    ```


    1. We can expose arguments, even references! This value with show up in a different stack frame from `opcontrol`
    2. Expose our variable `i` here, this will show up in a different stack frame from the `$expose` in `printAndSleep`