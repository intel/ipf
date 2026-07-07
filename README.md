# IPF-EF ClientApi

This repository contains the source code needed to build both the static and dynamic versions of the IPF-EF client API.

## Prerequisites

- Microsoft Windows 11 23H2 or later operating system.
- [CMake 3.15 or later](https://cmake.org/download/)
- Visual Studio 2022 with C++ development support and the latest Spectre-mitigated libraries

## How to build

1. Open the `IPF_EF` directory using the VS2022 Developer PowerShell
2. Run `cmake -S . -B build -G "Visual Studio 17 2022" -A x64` to generate the build folder
3. Run `cmake --build build --config Release` to build the libraries

## License

Source code in this repository is provided under the Apache 2.0 license (see LICENSE.txt).
Third-party licenses are contained in the `licenses` folder.
