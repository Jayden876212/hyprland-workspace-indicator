# Hyprland Workspace Indicator - Developer Handbook 

## Overview

This is a developer handbook that will guide developers with:

- Building
- Architecture
- Designs
- Testing

## Contributing

To contribute, there is a [*contributing guide*](CONTRIBUTING.md).

## Dependencies

### Runtime Dependencies

- [Hyprland](https://github.com/hyprwm/Hyprland)

### Basic Development Toolchain

- [C Programming Language](https://www.c-language.org/)
- Basic Toolchain
	- Compiler - [Clang](https://clang.llvm.org/)
	- Build System - [CMake](https://cmake.org/)
	- Debugger - [Valgrind](https://valgrind.org/)
	- Libraries:
		- Standard Library - [The GNU C Library](https://www.gnu.org/software/libc/)
		- Other Libraries - [cJSON](https://github.com/DaveGamble/cJSON)

### Recommended Developer Tools/Environment

Some of these (i.e., linters and formatters) are enforced through CI checks.

- Editor
    - [Neovim](https://neovim.io/)
- Language Server
    - [clangd](https://clangd.llvm.org/)
- Linters
    - [ClangTidy](https://prettier.io/)
    - [Include What You Use](https://github.com/include-what-you-use/include-what-you-use)
- Formatters
    - [ClangFormat](https://clang.llvm.org/docs/ClangFormat.html)
    - [Prettier](https://prettier.io/)

## How to build and install (eww)

Build requirements:

- cJSON
- Make

1. Run `make`
2. Edit the workspaces_script in `bin/` to the path of the binary (which is produced in the `bin/` directory by default).
3. Set a `deflisten` variable in eww with the workspaces_script as the input.
4. Create a `defwidget` that uses that variable for `:reveal` or `:visible` as seen by the example code in `example/`.
5. Implement the widget in a `defwindow`, like your eww bar for example.
