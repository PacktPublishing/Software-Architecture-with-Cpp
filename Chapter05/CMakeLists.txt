cmake_minimum_required(VERSION 3.15)

project(
  ch05
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
list(APPEND CMAKE_PREFIX_PATH "${PROJECT_BINARY_DIR}")

find_package(ms-gsl CONFIG REQUIRED)

include(CommonCompileFlags)

add_library(apis INTERFACE)
target_include_directories(apis INTERFACE src/apis)

add_library(declarative src/apis/declarative.cpp)
target_compile_features(declarative PUBLIC cxx_std_20)

add_executable(constexpr src/constexpr.cpp)
target_compile_features(constexpr PRIVATE cxx_std_20)
target_compile_options(constexpr PRIVATE ${BASE_COMPILE_FLAGS})

add_executable(base src/1_base.cpp)
target_link_libraries(base PRIVATE Microsoft.GSL::GSL)
target_compile_features(base PRIVATE cxx_std_20)
target_compile_options(base PRIVATE ${BASE_COMPILE_FLAGS})

add_executable(ranges1 src/2_ranges_part_1.cpp)
target_link_libraries(ranges1 PRIVATE Microsoft.GSL::GSL)
target_compile_features(ranges1 PRIVATE cxx_std_20)
target_compile_options(ranges1 PRIVATE ${BASE_COMPILE_FLAGS})

add_executable(ranges2 src/3_ranges_part_2.cpp)
target_link_libraries(ranges2 PRIVATE Microsoft.GSL::GSL)
target_compile_features(ranges2 PRIVATE cxx_std_20)
target_compile_options(ranges2 PRIVATE ${BASE_COMPILE_FLAGS})

add_executable(concepts src/4_concepts.cpp)
target_link_libraries(concepts PRIVATE Microsoft.GSL::GSL)
target_compile_features(concepts PRIVATE cxx_std_20)
target_compile_options(concepts PRIVATE ${BASE_COMPILE_FLAGS})

option(ENABLE_MODULES "Compile the modules example" OFF)

if(ENABLE_MODULES)
  # this is workaround until CMake supports Modules properly
  function(add_module_executable name) # then sources in specific order
    add_custom_target(
      ${name} ALL
      COMMAND
        ${CMAKE_CXX_COMPILER} -std=c++2a -fmodules-ts
        ${CMAKE_CXX_FLAGS} # TODO: append build-type specific compile flags and
                           # ${BASE_COMPILE_FLAGS}
        ${ARGN} -o ${CMAKE_CURRENT_BINARY_DIR}/${name} -isystem
        ${PROJECT_SOURCE_DIR}/3rd-parties/ # TODO: proper support for
                                           # target_link_libraries
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  endfunction()

  add_module_executable(
    5_modules src/5_modules/merchant.cpp src/5_modules/customer.cpp
    src/5_modules/store.cpp src/5_modules/main.cpp)
endif()
