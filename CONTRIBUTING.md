# Contributing to icon-changer

Thanks for your interest in contributing! Contributions are very welcome, whether it's through code, issues, or suggestions. Feel free to open pull requests, look into existing issues, or reach out to me directly if you have any questions or ideas.

## Dependencies

Make sure your environment meets the following requirements:

- **Operating System:** Windows 11
- **CMake:** 4.1.1
- **Ninja:** 1.13.1
- **LLVM:** 21.1.1

## Building the Project

To build the project in Release mode:

```sh
mkdir build
cmake -G "Ninja" -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Release -S . -B build
cmake --build build
```

The compiled executable will be located at:

```
build/bin/icon-changer.exe
```

## Running Unit Tests

To build and run unit tests, use the following commands:

```sh
mkdir build
cmake -G "Ninja" -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON -S . -B build
cmake --build build
cmake --build build --target coverage_report
```

## Code Formatting

Before committing, make sure Git is configured to use the repository's hooks for formatting:

```sh
git config core.hooksPath .githooks
```

## Inspecting Built Executables

A useful tool for inspecting the resources embedded in Windows executables is **Resource Hacker**.
