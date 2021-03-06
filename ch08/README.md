# Hands-On-Software-Architecture-with-Cpp
Hands-On Software Architecture with C++ by Packt Publishing

## Chapter 8: Writing Testable Code

### Prerequisites

Install the following software:
- CMake 3.15
- Conan 1.34
- GCC 10

Assuming you're on Linux, configure a hosacpp Conan profile and remotes by running:

```bash
conan profile new hosacpp || true
conan profile update settings.compiler=gcc hosacpp
conan profile update settings.compiler.libcxx=libstdc++11 hosacpp
conan profile update settings.compiler.version=10 hosacpp
conan profile update settings.arch=x86_64 hosacpp
conan profile update settings.os=Linux hosacpp
conan remote add trompeloeil https://api.bintray.com/conan/trompeloeil/trompeloeil || true
```

### Building

The customer and mobile app components must be built separately.

To build each of them, first cd to their source directory, and then run:

```bash
mkdir build
cd build
conan install .. --build=missing -s build_type=Release -pr=hosacpp
cmake .. -DCMAKE_BUILD_TYPE=Release # build type must match Conan's
cmake --build .
```

### Testing

To run tests from each of the projects, cd into their respective build directory, and then simply run `ctest`.
