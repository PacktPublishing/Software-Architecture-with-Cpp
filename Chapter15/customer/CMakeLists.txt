cmake_minimum_required(VERSION 3.15...3.19)

include(FetchContent)

option(BUILD_TESTING OFF)

FetchContent_Declare(
  jaeger-client-cpp
  GIT_REPOSITORY https://github.com/jaegertracing/jaeger-client-cpp.git
  GIT_TAG v0.7.0)

FetchContent_MakeAvailable(jaeger-client-cpp)

project(
  Customer
  VERSION 0.0.2
  LANGUAGES CXX)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

find_package(Boost CONFIG REQUIRED EXACT 1.75.0)
find_package(cpprestsdk CONFIG REQUIRED)

add_subdirectory(src/customer)
