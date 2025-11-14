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

## Downloading the release archive

Download the latest `v5dbg_server.zip` from the [releases page](https://github.com/Interfiber/v5dbg/releases) on GitHub, now extract this archive into your project folder.

!!! note "Pick the correct file!"
    Make sure to download **`v5dbg_server.zip`**, and **not** `v5dbg_debugger.zip`. 

??? question "Debugger vs Debug Server"
    The debug server is the program that executes **on the brain** and performs all of the internal debug logic.

    The debugger is the program that executes **on your computer** and serves as a frontend to the debug server over USB serial.

## Copy server source code

Extract the downloaded zip file and copy the following directories:

- [x] `include/v5dbg` into `include`
- [x] `src/v5dbg` into `src`

## Build your program

Using either the VSCode extension, or by calling `pros make` from the terminal make sure that your program builds properly, all v5dbg source code files should automatically be detected and compiled in.