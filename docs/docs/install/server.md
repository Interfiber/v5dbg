---
description: Install the v5dbg debug server into your PROs project
icon: octicons/download-24
---

# Installation

???+ warning "Limitations of v5dbg"
    Please keep the following limitations in mind while using v5dbg.

    - Every function must begin with `$function`
    - Variables accessible by the debugger must be fed into `$expose`
    - If your program has a fatal crash v5dbg cannot suspend the program.
        - Try using [Symbolizer for VEX V5](https://marketplace.visualstudio.com/items?itemName=vexide.symbolizer-for-vex-v5) in combination with v5dbg.
    - Line-by-line stepping is not possible

## Requirements

Please read through the requirements carefully

- Your user program is developed using [PROS](https://pros.cs.purdue.edu/)
- PROS toolchain is **installed** and **up-to-date**
    - The easiest way to do this is to use the [VSCode extension](https://marketplace.visualstudio.com/items?itemName=sigbots.pros)

??? question "Debugger vs Debug Server"
    The debug server is the program that executes **on the brain** and performs all of the internal debug logic.

    The debugger is the program that executes **on your computer** and serves as a frontend to the debug server over USB serial.

## Downloading and installing with PROS conductor

`v5dbg` has a PROS conductor depot avalible which prevents you from having to build and keep the source code up-to-date.

```shell
pros c add-depot v5dbg https://raw.githubusercontent.com/LemLib/LemLib/depot/stable.json
```

You can then install the `v5dbg` server with `pros c apply v5dbg`

!!! tip "Updating"
    If you ever need to **update** v5dbg you can run `pros c upgrade` to upgrade all your dependencies.
