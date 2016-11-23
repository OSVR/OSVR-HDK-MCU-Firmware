# OSVR HDK Microcontroller Firmware
> Maintained at <https://github.com/OSVR/OSVR-HDK-MCU-Firmware>

This repository contains the source code to the main microcontroller firmware for a family of HMDs related to the OSVR HDKs, originally and primarily developed by [Sensics](http://sensics.com).

The firmware is developed using a "product-line engineering" approach, with each variant sharing the bulk of the code but differing mainly in an include path that contains primarily/only the `VariantOptions.h` configuration header. Each variant can be selected by choosing a corresponding configuration in Atmel Studio or building the corresponding target using the Makefile build.

The variants primarily supported in this repository, listed by their Atmel Studio configuration names, are:

- `HDK_Sharp_LCD` - OSVR HDK 1.1 and derived custom devices containing a 5.5" Sharp LCD. (Makefile target: `hdk_lcd`)
- `HDK_OLED` - OSVR HDK 1.2, 1.3/1.4 and derived custom devices containing a single AUO low-persistence OLED panel. (Makefile target: `hdk_oled`)
- `HDK_20_SVR` - OSVR HDK 2 and derived custom devices containing dual AUO OLED panels and the Toshiba TC358870 HDMI-dual MIPI converter. (Makefile target: `hdk2svr` for the Sensics-defined EDID)
    - `HDK_20` (Makefile target `hdk2`) is similar but uses legacy Razer-dictated EDID data.

**Other variants are present**, but this repository is not necessarily the authoritative source for those variants. Any changes **must not break other variants** present in this codebase.

## Building
The canonical build environment is a fully-updated install of Atmel Studio 7.0 on Windows, though the build is portable to non-Windows.
There are two methods of building, both of which must work and must be updated for new code.
**Releases are built with the Makefile method, along with any other builds that may be distributed** - inconsistent/faulty support for pre/post build steps in Atmel Studio means that Atmel Studio IDE builds cannot carry detailed git commit version metadata accessible via `#?v` and `#?f`, and Makefile builds of the variants are easier to maintain in sync with one another with regards to build flags, etc.

### Atmel Studio project
Open the solution file in the `Source Code` folder, and build one of the variants, selected using the configuration menu (where you might see "Release" or "Debug" in Visual Studio).

### Makefile
Open a command prompt: to make it easier (to avoid needing to provide a full path to `make` in the `shelltools` subdirectory of the Atmel Studio install directory or install your own copy of `make` using [chocolatey](https://chocolatey.org) ) you can choose Tools, Command Prompt in Atmel Studio to open one with the PATH already extended. You also need git on your path, for automatic version stamping of builds based on tags - you may instead pass `GIT=c:/path/to/git.exe` as an argument to `make` if git isn't on your PATH.

Change directory to the source tree, subdirectory `Source Code/Embedded/makefile-build`. Here, you can run `make` (or `make all`) to build all variants (not just the main variants listed above) and copy their hex files, renamed after their Makefile target, to the current directory. Of course, `make hdk2svr` or any other single target name will just make that one target. `make help` will list some information - but open the Makefile in a text editor for more help.

This build system is portable to non-Windows installs of `avr-gcc` with `avr-libc`, but has not been rigorously tested on other platforms. The Atmel Studio bundled toolchain reports the following:

```
> avr-gcc -v
...
gcc version 4.9.2 (AVR_8_bit_GNU_Toolchain_3.5.3_1700)
```

## Development/Contribution

### Code Formatting
A clang-format file is present, and `/Source code/Embedded/format-code.sh` is a script (usable both on Unix-like systems as well as under "git bash") to automatically apply clang-format to a subset of the source tree (the "non-vendor" portions).
*Changes shall have clang-format applied to "non-vendor" code at every commit* (not in one commit at the end of a branch) for clarity and ease of review, so that diffs are minimal and contain just the "meat" of the changes, not frivolous whitespace changes due to badly-behaving editors.
The ["git clang-format" utility](https://llvm.org/svn/llvm-project/cfe/trunk/tools/clang-format/git-clang-format), which formats just patched/staged code, can be a useful assistant in this.

Use a recent version of clang/clang-format: [LLVM Windows snapshots](http://llvm.org/builds/) are the canonical source for Windows clang-format for this project.

### Build System and Atmel Studio Project/Files
Atmel Studio frequently modifies the `.cproj`, `.componentinfo.xml`, and `.atsln` file but with no substantial cross-developer content.
Until this upstream design flaw is fixed by Atmel and user preferences (such as debug tool and component versions) are split out from project description files, be very cautious before staging and committing any changes to those files - *do not commit changes to this file unless required, in which case stage only the portions required for your update.*
Typically, the only time to do so would be if you add a new file to the build, or need to change the build flags for a variant or add a variant, and then you can stage just the part of the `.cproj` file diff that matters.
**Do not** stage changes that just add/change the debug tool (JTAG/ICE) or mention `PackRepoDir`, since they vary from system to system and are automatically performed by each user's installation of Atmel Studio.

Please note, also, that if you actually have a need to modify these files, you'll typically have a need to modify the Makefile as well.
This can also be used as a test: if you didn't need to modify the Makefile, unless all you did was add a header file to the repo/IDE project, then you probably don't need to modify the Atmel Studio files.

If you modified only one of the two build systems, please say why in your pull request - and if it's just "I'm not good at Makefiles", that's OK, we can suggest what to do, but this information is important.

### Code review and testing
Pull requests are used for code review. Fork or branch from the latest master, and make a branch with just a single logical set of changes.

The minimal testing required is that you run a Makefile build of all variants in all supported flag sets - `make clean` followed by `make complete` - and that it complete successfully, however, this is just an initial smoketest and not comprehensive.

In your pull request, please state which devices you've installed your patched firmware on and how you've tested the change.

## License and Vendored Projects

See individual files for licenses. If not marked, files should be assumed to be:

> Copyright 2008-2016 Sensics, Inc. All rights reserved.

A goal is to eventually be able to open source as much of this firmware as possible under the Apache License, version 2.0. A number of vendored files have already been relicensed, with permission, under the Apache License, version 2.0. As such, **new non-Sensics contributions shall be licensed under the Apache License, version 2.0**.

- `Source Code/Embedded/src/ASF` - the Atmel Software Framework, with some minimal modifications (see version control), under its own license:

> Copyright (c) 2008-2016 Atmel Corporation. All rights reserved.
>
> Redistribution and use in source and binary forms, with or without
> modification, are permitted provided that the following conditions are met:
>
> 1. Redistributions of source code must retain the above copyright notice,
>    this list of conditions and the following disclaimer.
>
> 2. Redistributions in binary form must reproduce the above copyright notice,
>    this list of conditions and the following disclaimer in the documentation
>    and/or other materials provided with the distribution.
>
> 3. The name of Atmel may not be used to endorse or promote products derived
>    from this software without specific prior written permission.
>
> 4. This software may only be redistributed and used in connection with an
>    Atmel microcontroller product.
>
> THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
> WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
> MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
> EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
> ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
> DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
> OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
> HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
> STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
> ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
> POSSIBILITY OF SUCH DAMAGE.
