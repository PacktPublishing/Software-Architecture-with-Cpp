# Hands-On-Software-Architecture-with-Cpp
Hands-On Software Architecture with C++ by Packt Publishing

## Chapter 11: Performance

### Prerequisites

Install the following software:
- CMake 3.15
- Conan 1.34
- GCC 10

Assuming you're on Linux, configure a hosacpp Conan profile by running:

```bash
conan profile new hosacpp || true
conan profile update settings.compiler=gcc hosacpp
conan profile update settings.compiler.libcxx=libstdc++11 hosacpp
conan profile update settings.compiler.version=10 hosacpp
conan profile update settings.arch=x86_64 hosacpp
conan profile update settings.os=Linux hosacpp
```

### Building

To build the project run:

```bash
mkdir build
cd build
conan install .. --build=missing -s build_type=Release -pr=hosacpp
cmake .. -DCMAKE_BUILD_TYPE=Release # build type must match Conan's
cmake --build .
```
