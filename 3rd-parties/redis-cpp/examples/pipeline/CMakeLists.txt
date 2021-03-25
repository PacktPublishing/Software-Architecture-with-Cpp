cmake_minimum_required(VERSION 3.12.0)
set(PROJECT pipeline)
string(TOLOWER "${PROJECT}" PROJECT_LC)

set (STD_CXX "c++17")
set (REDISCPP_FLAGS "-DREDISCPP_HEADER_ONLY=ON")

set (CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/MyCMakeScripts)
set (EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -std=${STD_CXX} ${REDISCPP_FLAGS}")
set (CMAKE_CXX_FLAGS_RELEASE "-O3 -g0 -std=${STD_CXX} -Wall -DNDEBUG ${REDISCPP_FLAGS}")
set (CMAKE_POSITION_INDEPENDENT_CODE ON)

#---------------------------------------------------------

#---------------------- Dependencies ---------------------

find_package(Boost 1.67.0 REQUIRED COMPONENTS thread system iostreams)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

set (LIBRARIES
    ${LIBRARIES}
    ${Boost_LIBRARIES}
)


include_directories(../../include/)

#---------------------------------------------------------

include_directories (include)

add_executable(${PROJECT_LC} src/main.cpp)
target_link_libraries(${PROJECT_LC} ${LIBRARIES})