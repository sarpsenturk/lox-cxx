# lox-cxx
A modern C++ (20) implementation for the Lox programming language as show in [Crafting Interpreters](https://craftinginterpreters.com) by Robert Nystrom

## Getting Started
Simply clone the repository and build with cmake
```
git clone https://github.com/sarpsenturk/lox-cxx --recurse-submodules
cd lox-cxx
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build
./build/lox-cxx
```

## Lox Language Features
- [x] Dynamic, ref counted lox objects (numbers, strings, booleans)
- [x] Arithmetic expressions
- [x] Logic expressions
- [x] Global & scoped local variables
- [x] if/else control flow
- [x] while/for loops
- [ ] Functions & Closures
- [ ] Classes & Methods

## Examples
Lox scripts demonstrating implemented features are available in the [lox folder](lox)
