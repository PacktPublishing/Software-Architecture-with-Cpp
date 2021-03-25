cmake_minimum_required(VERSION 3.15)

project(
  ch11
  VERSION 0.0.1
  LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}")
find_package(benchmark REQUIRED)
find_package(fmt REQUIRED)
find_package(Threads REQUIRED)

function(add_benchmark NAME SOURCE)
  add_executable(${NAME} ${SOURCE})
  target_compile_features(${NAME} PRIVATE cxx_std_20)
  target_link_libraries(${NAME} PRIVATE benchmark::benchmark pthread)
endfunction()

add_benchmark(microbenchmark_1 microbenchmarking/main_1.cpp)
add_benchmark(microbenchmark_2 microbenchmarking/main_2.cpp)
add_benchmark(microbenchmark_3 microbenchmarking/main_3.cpp)
add_benchmark(microbenchmark_4 microbenchmarking/main_4.cpp)

include(FetchContent)
FetchContent_Declare(
  cppcoro
  GIT_REPOSITORY https://github.com/andreasbuhr/cppcoro
  GIT_TAG master
  GIT_SHALLOW ON)
FetchContent_MakeAvailable(cppcoro)

add_executable(coroutines_1 coroutines/main_1.cpp)
target_link_libraries(coroutines_1 PRIVATE cppcoro fmt::fmt Threads::Threads)
target_compile_features(coroutines_1 PRIVATE cxx_std_20)
