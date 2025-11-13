---
description: Documentation for the $function macro which is used to mark the entrance of functions
icon: octicons/package-24
---

# `$function`

!!! abstract "Version information"
    Introduced in version **v0.1** :octicons-tag-24:

    This macro is considered **stable** :octicons-check-24:

## About

The `$function` macro allows v5dbg to generate a **backtrace** to determine the current point of execution. When placed at the **top of a function** it adds debug information to the current backtrace, when the function exits its then removed.

??? question "Why do I need to use this macro?"
    Unlike traditional debuggers which can ask the operating system for a backtrace and can then convert the returned addresses into symbol names, v5dbg runs on an environment where this is not possible **(yet)**.

    As a workaround `$function` traces where it was called from to determine the proper line of execution.

## Notes

!!! danger "Required environment"
    - This macro **requires** the debug server to have been initialized before being called.
    - This macro **requires** the current thread to be supervised with [$ntask](./ntask.md) or `V5Dbg_Init`

## Examples

=== "Single function"

    ```c++ hl_lines="7" linenums="1"
    #include "v5dbg/debug.h"

    void
    opcontrol()
    {
        $ntask // (1)
        $function // (2)

        while (true)
        {
            // Print "Hello World" to the screen via pros::lcd
            pros::lcd::print(1, "Hello World");

            pros::delay(300);
        }
    }
    ```

    1. For more information on `$ntask` look [here](./ntask.md)
    2. Now **opcontrol** will be the first item in the callstack since it's the first time `$function` was called on this thread/task.

=== "Multiple functions"

    ```c++ hl_lines="6" linenums="1"
    #include "v5dbg/debug.h"

    void
    printAndSleep(const std::string &p)
    {
        $function // (2)
        pros::lcd::print(1, "%s", p.c_str()); // Print our data to the screen

        pros::delay(300); // Wait 300ms
    }

    void
    opcontrol()
    {
        $ntask // (1)
        $function

        while (true)
        {
            printAndSleep("Hello World");
        }
    }
    ```


    1. For more information on `$ntask` look [here](./ntask.md)
    2. Now our `printAndSleep` function will be the second item in the callstack, it should now look like this:
        - `void printAndSleep(const std::string &p)`
        - `void opcontrol()`