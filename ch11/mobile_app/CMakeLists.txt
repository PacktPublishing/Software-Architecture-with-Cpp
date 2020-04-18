cmake_minimum_required(VERSION 3.15)

project(MobileApp VERSION 0.0.1 LANGUAGES CXX)

# TODO: make sure this works for Xcode and MSVC
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

include(CommonCompileFlags)

include(${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

list(APPEND CMAKE_MODULE_PATH "${CONAN_LIB_DIRS_CATCH2}/cmake/Catch2")

add_subdirectory(src)

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
    if (BUILD_TESTING)
        add_subdirectory(test)
    endif()
endif ()
