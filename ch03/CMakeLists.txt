cmake_minimum_required(VERSION 3.10)

project(
  "Breathe Demo"
  VERSION 0.0.1
  LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
add_subdirectory(src)
add_subdirectory(doc)
