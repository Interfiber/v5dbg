# `evalpy`

!!! abstract "Command information"
    Regular name **`evalpy`** :octicons-unlink-24:

    Introduced in version **v0.2** :octicons-tag-24:

    This command is considered **unstable** :octicons-stop-24:

## About

The `evalpy` command is used to return the result of a **Python expression**. When `evalpy` is used all variables from the **current scope** are injected.

## Notes

!!! danger
    Python code executed by `evalpy` has **full operating system access**, do not paste random code without reading it first!

## Syntax

This command accepts a single `String` argument representing the **Python expression** to execute.