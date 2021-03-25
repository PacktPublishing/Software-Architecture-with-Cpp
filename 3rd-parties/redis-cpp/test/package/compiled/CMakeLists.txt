#[[
redis-cpp package test
----------------------

This is an example usage of the installed package.
It requires 'ExternalProject' to build/install.

Doesn't use the download step since it uses this repo as
a SOURCE_DIR.

]]
cmake_minimum_required(VERSION 3.11)

project(TestInstalledPackage)

include(ExternalProject)

message(STATUS "Installing ${PROJECT}")

if(NOT REDISCPP_EXTERNAL_DIR)
    set(REDISCPP_EXTERNAL_DIR "${CMAKE_CURRENT_LIST_DIR}/external" CACHE STRING "External base")
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../This.cmake.in
    ${REDISCPP_EXTERNAL_DIR}/${PROJECT}-download/CMakeLists.txt)

execute_process(COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" .
    WORKING_DIRECTORY "${REDISCPP_EXTERNAL_DIR}/${PROJECT}-download"
)

execute_process(COMMAND "${CMAKE_COMMAND}" --build .
    WORKING_DIRECTORY "${REDISCPP_EXTERNAL_DIR}/${PROJECT}-download"
)

list(APPEND CMAKE_PREFIX_PATH ${REDISCPP_EXTERNAL_DIR}/${PROJECT}-install)

find_package(${PROJECT} REQUIRED)

add_executable (${PROJECT}-package-test ../main.cpp)

target_link_libraries(${PROJECT}-package-test PRIVATE redis-cpp::redis-cpp)

message(STATUS "Test passed!")
