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
    - [ClangTidy](https://clang.llvm.org/extra/clang-tidy/)
    - [Include What You Use](https://github.com/include-what-you-use/include-what-you-use)
- Formatters
    - [ClangFormat](https://clang.llvm.org/docs/ClangFormat.html)
    - [Prettier](https://prettier.io/)

## How to build and install

### Requirements

You need to install the [development toolchain mentioned above](DEVELOPER.md#basic-development-toolchain).

### Step 1 - Clone project and `cd` into it

First, you must get a copy of the project's source from this GitHub repository:

```sh
git clone https://github.com/Jayden876212/hyprland-workspace-indicator
cd hyprland-workspace-indicator
```

### Step 2 - Generate the build files

Then you need to generate the build files in the `build` directory.

```sh
./configure
```

### Step 3 - Run `make` from `build` directory

Finally, run `make` from the `build` directory:

```sh
make -C build install
```

This should install `hyprland-workspaces` to your `$PATH` so you can run it.

This may need root privileges depending on your environment:

Using `sudo`:

```sh
sudo make -C build install
```

Using `doas`:

```sh
doas make -C build install
```

Using `su`:

```sh
su
make -C build install
```
