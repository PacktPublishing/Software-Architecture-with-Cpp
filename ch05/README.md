# Hands-On-Software-Architecture-with-Cpp
Hands-On Software Architecture with C++ by Packt Publishing

## Chapter 5: Leveraging C++ Language Features

### Prerequisites

Install the following software:
- CMake 3.15
- GCC 11

Install the following software:

- CMake 3.15
- Conan 1.34.1
- GCC 11

Assuming you're on Linux or using WSL, configure a hosacpp Conan profile and remotes by running:

```bash
conan profile new hosacpp || true
conan profile update settings.compiler=gcc hosacpp
conan profile update settings.compiler.libcxx=libstdc++11 hosacpp
conan profile update settings.compiler.version=11 hosacpp
conan profile update settings.arch=x86_64 hosacpp
conan profile update settings.os=Linux hosacpp
```

If GCC 11 is not your default compiler, you also need to add:

```bash
conan profile update env.CXX=`which g++-11` hosacpp
conan profile update env.CC=`which gcc-11` hosacpp
```

### Building

To build the project, first cd to its directory, and then run:

```bash
mkdir build
cd build
conan install .. --build=missing -s build_type=Release -pr=hosacpp
cmake .. -DCMAKE_BUILD_TYPE=Release # build type must match Conan's
cmake --build .
```

If GCC is not your default compiler, you can tell CMake to use it with the `CMAKE_CXX_COMPILER` flag.
Replace the first invocation above with:

```bash
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=`which g++-11`
```
