# Hands-On-Software-Architecture-with-Cpp
Hands-On Software Architecture with C++ by Packt Publishing

## Chapter 7: Building and Packaging

### Prerequisites

Install the following software:
- CMake 3.15
- Conan 1.34.1
- GCC 10

Assuming you're on Linux or using WSL, configure a hosacpp Conan profile and remotes by running:

```bash
conan profile new hosacpp || true
conan profile update settings.compiler=gcc hosacpp
conan profile update settings.compiler.libcxx=libstdc++11 hosacpp
conan profile update settings.compiler.version=10 hosacpp
conan profile update settings.arch=x86_64 hosacpp
conan profile update settings.os=Linux hosacpp
```

If GCC 10 is not your default compiler, you also need to add:

```bash
conan profile update env.CXX=`which g++-10` hosacpp
conan profile update env.CC=`which gcc-10` hosacpp
```

### Building

To build the customer project, first cd to its source directory, and then run:

```bash
mkdir build
cd build
conan install .. --build=missing -s build_type=Release -pr=hosacpp
cmake .. -DCMAKE_BUILD_TYPE=Release # build type must match Conan's
cmake --build .
```

If GCC 10 is not your default compiler, you can tell CMake to use it with the `CMAKE_CXX_COMPILER` flag.
Replace the first invocation above with:

```bash
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=`which g++-10`
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
