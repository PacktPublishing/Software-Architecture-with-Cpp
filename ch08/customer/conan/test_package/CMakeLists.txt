cmake_minimum_required(VERSION 3.12)
project(PackageTest CXX)

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}")

find_package(customer CONFIG REQUIRED)

add_executable(example example.cpp)
target_link_libraries(example customer::customer)

# CTest tests can be added here
