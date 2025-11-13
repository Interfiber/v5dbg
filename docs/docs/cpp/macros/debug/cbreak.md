---
description: Documentation for the $cbreak macro which is used to place conditional breakpoints in code
icon: octicons/pause-24
---

# `$cbreak(E)`

!!! abstract "Version information"
    Introduced in version **v0.2** :octicons-tag-24:

    This macro is considered **stable** :octicons-check-24:

## About

The `$cbreak` macro is used to place **conditional breakpoints** in code. When the expression `E` **evaluates to `true`** then the breakpoint trips, otherwise nothing will happen.

## Notes

!!! note
    Unlike [`$break`](./break.md) breakpoints, these breakpoints are **enabled by default**

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

        $cbreak(i % 4 == 0); // (1)

        i++;
    }
}
```

1. This breakpoint will be enabled by default and will only trip if the value of `i` is divisible by `4` with no remainder