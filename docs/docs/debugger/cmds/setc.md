# `setc`

!!! abstract "Command information"
    Regular name **`setc`** :octicons-unlink-24:

    Introduced in version **v0.2** :octicons-tag-24:

## About

The `setc` command is used to **set the value** of variables [exposed](../../cpp/macros/debug/expose.md) within the current scope **even when reallocated**.

Whenever a variable is **reallocated** its value is **overwritten** to the value specified by this command. If you wish to **disable** the overwriting of the variable after a call to `setc` you may pass the **variable name** to `setcd`

## Notes

!!! warning
    * This command will only work when the program is detected to be in the `SUSPEND` state.
    * **Reallocation** is not the same as **modification**

## Syntax

The `set` command accepts the `String` **name** of the variable and the `String` **value** of the variable.

| Argument position      | Description |
| ----------- | ----------- |
| Position 0  | Name of the variable to set |
| Position 1  | New value of the variable |