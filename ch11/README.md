# Building and Packaging

## Building

To build this project out of source, run:

```bash
mkdir build
cd build
conan profile new default || true
conan profile update settings.compiler=gcc default
conan profile update settings.compiler.libcxx=libstdc++11 default
conan profile update settings.compiler.version=9 default
conan profile update settings.arch=x86_64 default
conan profile update settings.os=Linux default
conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan || true
conan install .. --build=missing -pr=default
cmake ..
cmake --build .
```

## Installing

Run `cmake --install .` to install the software into `${CMAKE_PREFIX_PATH}`. If no prefix is given, it will install
 system-wide. To change this, add `-DCMAKE_PREFIX_PATH=/path/to/install/to` to your cmake invocation.

## Packaging

Run `cpack`. Simple as that. Assuming you're running on a system supporting DEB packages, you'll get a .tar.gz file
, a .zip file, and a .deb package.

## Building a Conan package

In the build directory, run `cmake --build . --target conan`.

## Building a Docker container

In the build directory, run `cmake --build . --target docker`.
