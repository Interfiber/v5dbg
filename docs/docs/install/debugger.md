---
description: Install the debugger component of v5dbg
icon: octicons/download-24
---

# Install the v5dbg debugger

## Downloading the latest archive

Download the latest `v5dbg_debugger.zip` file for your platform from the [releases page](https://github.com/Interfiber/v5dbg/releases)

* For **Windows** users download `v5dbg_debugger.windows-AMD64.zip`
* For **GNU/Linux** users download `v5dbg_debugger.linux-x86_64.zip`
* **MacOS** is not currently built by the CI/CD system and archives are not published

??? tip "Verify checksums"
    If you want to verify that your file has not been tampered with you may check its checksum again the computed ones available on the versions specific release page.

## Extract and install

Extract the downloaded zip file into an **easily accessible location**. If you wish to make the `v5dbg` command available in your shell without using the **full path** you can add the extracted location to your ```PATH``` variable.

## Commons errors

??? danger "Waiting for user program execution on target to begin...."
    If you get **stuck** at this message it means we are still waiting for the **user program** to start on the brain.

    Make sure that:

    * You're **wired** to the brain over MicroUSB
    * You have **started** the user program on the brain

??? danger "Warning! DebugServer (local) has not gotten any OPEN messages from the debug server in over 5s, did the debug server freeze?"
    This means that the debug server running on the brain **hasn't** sent us any messages in over 5 seconds.

    This usually means:
    
    * The user program & debug server have **crashed**
    * The serial connection to the debug server has been **dropped**, try restarting the debugger
    * The user program is **hogging CPU resources** and not allowing the debug server to execute