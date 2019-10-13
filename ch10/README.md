# Building and Packaging

## Building

To build this project out of source, run:

```bash
mkdir build
cd build
conan install .. --build=missing
cmake ..
cmake --build .
```

## Installing

Run `cmake --install .` to install the software into `${CMAKE_PREFIX_PATH}`. If no prefix is given, it will install system-wide. To change this, add `-DCMAKE_PREFIX_PATH=/path/to/install/to` to your cmake invocation.

## Packaging

Run `cpack`. Simple as that. You'll get a .tar.gz file, a .zip file, and a .deb package, assuming you're running on Ubuntu.  

## Building a Conan package



## Building a Docker container


