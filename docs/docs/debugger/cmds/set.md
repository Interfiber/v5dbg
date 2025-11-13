# `set`

!!! abstract "Command information"
    Regular name **`set`** :octicons-unlink-24:

    Introduced in version **v0.2** :octicons-tag-24:

## About

The `set` command is used to **set the value** of variables [exposed](../../cpp/macros/debug/expose.md) within the current scope.

## Notes

!!! warning
    This command will only work when the program is detected to be in the `SUSPEND` state.

## Syntax

The `set` command accepts the `String` **name** of the variable and the `String` **value** of the variable.

| Argument position      | Description |
| ----------- | ----------- |
| Position 0  | Name of the variable to set |
| Position 1  | New value of the variable |