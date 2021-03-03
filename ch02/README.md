# Hands-On-Software-Architecture-with-Cpp
Hands-On Software Architecture with C++ by Packt Publishing

## Chapter 2: Architectural Styles

### Prerequisites

Install the following software:
- CMake
- Conan
- GCC 10

Assuming you're on Linux, configure a hosacpp Conan profile and remotes by running:

```bash
conan profile new hosacpp || true
conan profile update settings.compiler=gcc hosacpp
conan profile update settings.compiler.libcxx=libstdc++11 hosacpp
conan profile update settings.compiler.version=10 hosacpp
conan profile update settings.arch=x86_64 hosacpp
conan profile update settings.os=Linux hosacpp
conan profile update settings.build_type=Release hosacpp
```

### Building

To build the customer project out of source, first cd to its directory, and then run:

```bash
mkdir build
cd build
conan install .. --build=missing -pr=hosacpp
cmake ..
cmake --build .
```
