# Hands-On-Software-Architecture-with-Cpp
Hands-On Software Architecture with C++ by Packt Publishing

## Chapter 11: [Writing Testable Code](ch11)

### Prerequisites

Install the following software:
- CMake
- Conan
- GCC 9

Assuming you're on Linux, configure a default Conan profile and remotes by running:

```bash
conan profile new default || true
conan profile update settings.compiler=gcc default
conan profile update settings.compiler.libcxx=libstdc++11 default
conan profile update settings.compiler.version=9 default
conan profile update settings.arch=x86_64 default
conan profile update settings.os=Linux default
conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan || true
```

### Building

The customer and mobile app components must be built separately.

To build each of them out of source, first cd to their directory, and then run:

```bash
mkdir build
cd build
conan install .. --build=missing -pr=default # only run this for customer
cmake ..
cmake --build .
```
