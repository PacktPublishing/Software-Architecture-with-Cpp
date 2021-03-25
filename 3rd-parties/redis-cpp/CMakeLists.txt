cmake_minimum_required(VERSION 3.12.0)

#-----------------------Options--------------------------------------
option (REDISCPP_PURE_CORE "[REDISCPP] Only pure core" OFF)
option (REDISCPP_HEADER_ONLY "[REDISCPP] Header only" OFF)
option (REDISCPP_EASY_ADDRESS_RESOLVE "[REDISCPP] Use easy address resolving" OFF)
option (REDISCPP_PACKAGE_TEST "[REDISCPP] Test installation" OFF)
#--------------------------------------------------------------------

mark_as_advanced(REDISCPP_PACKAGE_TEST)

project(redis-cpp
    VERSION 1.0.0
    LANGUAGES CXX
    DESCRIPTION "redis-cpp - lightweight C++ client library for Redis"
)
set(PROJECT ${PROJECT_NAME})
string(TOLOWER "${PROJECT}" PROJECT_LC)

if (REDISCPP_PURE_CORE)
    set (REDISCPP_DEFINES "${REDISCPP_DEFINES} -DREDISCPP_PURE_CORE")
endif()

if (REDISCPP_EASY_ADDRESS_RESOLVE)
    set(REDISCPP_DEFINES, "${REDISCPP_DEFINES} -DREDISCPP_EASY_ADDRESS_RESOLVE")
endif()

set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

set (CMAKE_CXX_EXTENSIONS OFF)

#---------------------------------------------------------

add_library (${PROJECT_LC}-ho INTERFACE)
add_library (${PROJECT_LC}::${PROJECT_LC}-ho ALIAS ${PROJECT_LC}-ho)

target_include_directories (${PROJECT_LC}-ho INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_definitions(${PROJECT_LC}-ho INTERFACE ${REDISCPP_DEFINES} REDISCPP_HEADER_ONLY)
target_compile_features(${PROJECT_LC}-ho INTERFACE cxx_std_17)

if (NOT REDISCPP_HEADER_ONLY)
    add_library (${PROJECT_LC} STATIC src/redis-cpp/stream.cpp)
    add_library (${PROJECT_LC}::${PROJECT_LC} ALIAS ${PROJECT_LC})
    target_include_directories (${PROJECT_LC} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
        $<INSTALL_INTERFACE:include>
    )
    target_compile_definitions(${PROJECT_LC} INTERFACE ${REDISCPP_DEFINES})
    target_compile_features(${PROJECT_LC} PUBLIC cxx_std_17)
    set_target_properties(${PROJECT_LC} PROPERTIES PUBLIC_CMAKE_POSITION_INDEPENDENT_CODE ON)

    if (UNIX)
        target_compile_options(${PROJECT_LC} PRIVATE -Wall -Wextra -W)
    endif()
endif()

if (NOT REDISCPP_PURE_CORE)
    find_package (Boost 1.67.0 REQUIRED COMPONENTS thread system iostreams)

    target_link_libraries (${PROJECT_LC}-ho INTERFACE Boost::boost Boost::system Boost::thread Boost::iostreams)

    if(NOT REDISCPP_HEADER_ONLY)
        target_link_libraries (${PROJECT_LC} PUBLIC Boost::boost Boost::system Boost::thread Boost::iostreams)
    endif()
endif()



# install
include (GNUInstallDirs)
include (CMakePackageConfigHelpers)

write_basic_package_version_file (
    ${CMAKE_CURRENT_BINARY_DIR}/generated/${PROJECT_LC}-config-version.cmake
    COMPATIBILITY
        SameMajorVersion
)

configure_package_config_file (
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_LC}-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/generated/${PROJECT_LC}-config.cmake
    INSTALL_DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_LC}
)

install (
    DIRECTORY
        ${CMAKE_CURRENT_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING
    PATTERN "*.hpp"
    PATTERN "*.h"
)

install (
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/generated/${PROJECT_LC}-config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/generated/${PROJECT_LC}-config-version.cmake
    DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_LC}
)

install (
    TARGETS
        ${PROJECT_LC}-ho
    EXPORT ${PROJECT_LC}-targets
)

if (NOT REDISCPP_HEADER_ONLY)
    install (
        TARGETS
            ${PROJECT_LC}
        EXPORT ${PROJECT_LC}-targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
endif()

install(
    EXPORT ${PROJECT_LC}-targets
    NAMESPACE ${PROJECT_LC}::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_LC}
)

# testing
if (REDISCPP_PACKAGE_TEST)
    add_subdirectory(test/package/header_only)
    if (NOT REDISCPP_HEADER_ONLY)
        add_subdirectory(test/package/compiled)
    endif()
endif()
