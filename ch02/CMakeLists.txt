cmake_minimum_required(VERSION 3.15)

project(
  ch02
  VERSION 0.0.1
  LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}")

find_package(ms-gsl CONFIG REQUIRED)

add_library(state INTERFACE)
target_include_directories(state INTERFACE state/)
target_link_libraries(state INTERFACE Microsoft.GSL::GSL)
