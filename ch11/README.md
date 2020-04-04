# Hands-On-Software-Architecture-with-Cpp
Hands-On Software Architecture with C++ by Packt Publishing 

## Chapter 11: [Writing Testable Code](ch11)

### Building

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
