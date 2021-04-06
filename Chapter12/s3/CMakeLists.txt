cmake_minimum_required(VERSION 3.15)

project(
  ch12
  VERSION 0.0.1
  LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}")

find_package(aws-sdk-cpp REQUIRED)
find_package(spdlog REQUIRED)

add_executable(s3 s3.cpp)
target_link_libraries(s3 PRIVATE aws-sdk-cpp::aws-sdk-cpp spdlog::spdlog)
