# Hands-On-Software-Architecture-with-Cpp
Hands-On Software Architecture with C++ by Packt Publishing

## Chapter 7: Building and Packaging

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

### Testing

To run tests from each of the projects, cd into their respective build directory, and then simply run `ctest`.

### Installing

In the build directory, run `cmake --install .` to install the software into `${CMAKE_PREFIX_PATH}`. If no prefix is
given, it will install system-wide. To change this, add `-DCMAKE_INSTALL_PREFIX=/path/to/install/to` to your cmake
invocation.

### Packaging

In the build directory, run `cpack`. Simple as that. Assuming you're running on a system supporting DEB packages,
you'll get a .tar.gz file, a .zip file, and a .deb package.

### Building a Conan package

In the build directory, run `cmake --build . --target conan`.

### Building a Docker container

In the build directory, run `cmake --build . --target docker`.
