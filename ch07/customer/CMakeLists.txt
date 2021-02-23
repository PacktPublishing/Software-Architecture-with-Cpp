cmake_minimum_required(VERSION 3.15)

project(
  Customer
  VERSION 0.0.1
  LANGUAGES CXX)

# TODO: make sure this works for Xcode and MSVC
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

include(${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

include(CommonCompileFlags)

add_subdirectory(src/customer)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  include(CTest)
  if(BUILD_TESTING)
    add_subdirectory(test)
  endif()
endif()

# Container creation

find_program(Docker_EXECUTABLE docker)
if(Docker_EXECUTABLE)
  add_custom_target(
    docker
    COMMENT "Preparing Docker image"
    COMMAND ${Docker_EXECUTABLE} build ${CMAKE_CURRENT_LIST_DIR} -t
            customer:${PROJECT_VERSION} -t customer:latest
    VERBATIM)
endif()
