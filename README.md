# Assemblia®

Dungeon-Crawler where you replace your limbs with guns that you tore off of your enemies. 
Battle System like Dragon Quest where you can target different weapons, limbs and body parts of enemies and shoot them off.

# Building

The game is currently targeted at 64-bit x86 architecture. ARM64 support may be added in the future. Due to usage of Vulkan, we do not (currently) support macOS. This is a Linux-first system, but is extensively tested on both Linux and Windows.

**Supported Platforms:** Linux x86_64 (X11, via `-platform x11`, or by default if Wayland isn't available), Linux x86_64 (Wayland), Windows x86_64

## Prerequisites

**In general (to play the game):**

- GPU which has a Vulkan-compatible driver, which supports at least Vulkan 1.1
- At least 384MB of VRAM (512MB or more of total VRAM recommended)
- At least 512MB of free RAM (4GB or more of system RAM recommended)
- For best results, CPU with at least 4 logical cores (threads)
- 16MB of free disk space for save files (may increase if you make a ton of saves)
- Whatever this repository takes on the hard drive for game files

**On Linux:**

- gcc9 or newer (must support C++20 and C11 standards)
- git
- cmake
- Vulkan-compatible GPU driver which provides `libvulkan.so.1` and a Vulkan implementation
- Headers and libraries for X11 *and* Wayland

**On Windows:**

- Windows 10 (1607, Anniversary Update) or newer
- Visual Studio 2019 or newer with C++ development features selected (which must provide MSVC compiler with vc140 / MSVC 2015)
- cmake commandline tools
- git commandline tools
- Vulkan-compatible GPU driver which provides `vulkan-1.dll` and a Vulkan implementation
- Visual C++ Redistributables 2015

## Compiling

1. Pull all submodules (`git submodule update --init --recursive`)
2. On Linux, `make`. On Windows, run `build.bat`

## Compiling a release version

**Linux:**

`make dist` should create a dist/ folder, build the project and copy all of the assets to it. The folder should be ready for distribution.

**Windows:**

`build_release.bat` should create a dist/ folder, build the project and copy all of the assets to it. The folder should be ready for distribution.

## Recompiling

On Linux, run `make clean`. On Windows, delete `build/` and `dist/` folders.

## Compiling shaders

Compiling shaders is currently only supported of Linux. To do so, you must have Ruby 2.7 or newer installed on your system and available in $PATH.

`make shaders` will compile the shaders and pack them into the `shaders.h` file.

## Linting

Linting is only officially supported on Linux. However, on Windows you could get away with just running `clang-format -i *.cpp` within the relevant directory. To lint, LLVM tools must be installed, providing the `clang-format` commandline tool.

`make lint` will format all source files of the project to be compliant with our coding standard.

## See also

* [The Wiki Pages](https://github.com/infomediadesign/projektarbeit-ii-team-7/wiki)
* [Basic Git Guide](https://github.com/infomediadesign/projektarbeit-ii-team-7/wiki/Git-Basics)
* [C++ Tutorial](https://learncpp.com)
* [LLVM Tools, such as clang-format](https://clang.llvm.org/)
* [The Spec](https://github.com/infomediadesign/projektarbeit-ii-team-7/wiki/Game-Spec)
