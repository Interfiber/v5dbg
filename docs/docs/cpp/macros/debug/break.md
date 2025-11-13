---
description: Documentation for the $break macro which is used to place disabled-by-default breakpoints in code
icon: octicons/pause-24
---

# `$break`

!!! abstract "Version information"
    Introduced in version **v0.2** :octicons-tag-24:

    This macro is considered **stable** :octicons-check-24:

## About

The `$break` macro allows the programmer to insert **breakpoint locations**. Once breakpoints are placed they be **enabled** by their file path or ID later in the debugger using the [`break`](../../../debugger/cmds/break.md) command.

For **conditional breakpoints** see the [`$cbreak`](./cbreak.md) macro.

## Notes

!!! danger "Required environment"
    - This macro **requires** the debug server to have been initialized before being called.
    - This macro **requires** the current thread to be supervised with [$ntask](./ntask.md) or `V5Dbg_Init`
    - This macro **requires** the current function to be [debuggable](../debug/function.md4)

## Example

```c++ hl_lines="18" linenums="1"
#include "v5dbg/debug.h"

void
opcontrol()
{
    $ntask
    $function

    int i = 0; // Incrementor

    float randomData = 0;
    $expose(randomData);

    while (true)
    {
        pros::lcd::print("%i", i);

        $break; // (1)

        i++;
    }
}
```

1. Our breakpoint will be placed here and can be enabled using the `break` command in the debugger