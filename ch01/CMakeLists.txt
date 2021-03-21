cmake_minimum_required(VERSION 3.15)

project(
  ch01
  VERSION 0.0.1
  LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

add_library(cohesion_high cc/cohesion/high.cpp)
add_library(cohesion_low cc/cohesion/low.cpp)
add_executable(coupling_loose cc/coupling/loose.cpp)
target_compile_features(coupling_loose PRIVATE cxx_std_17)
add_executable(coupling_tight cc/coupling/tight.cpp)

add_library(philosophy_algo_bad philosophy/algorithms/bad_dots.cpp)
add_library(philosophy_algo_good philosophy/algorithms/good_dots.cpp)
target_compile_features(philosophy_algo_good PRIVATE cxx_std_17)
add_library(philosophy_time_bad philosophy/time/bad_time.cpp)
add_library(philosophy_time_good philosophy/time/good_time.cpp)

add_executable(solid_dip_bad solid/dip/bad.cpp)
target_compile_features(solid_dip_bad PRIVATE cxx_std_17)

add_executable(solid_dip_dynamic_di solid/dip/dynamic_di.cpp)
target_compile_features(solid_dip_dynamic_di PRIVATE cxx_std_17)

add_executable(solid_dip_static_dip solid/dip/static_dip.cpp)
target_compile_features(solid_dip_static_dip PRIVATE cxx_std_17)

add_library(solid_isp_bad solid/isp/bad.cpp)
add_library(solid_isp_base solid/isp/base.cpp)
add_library(solid_isp_good solid/isp/good.cpp)

add_library(solid_lsp_bad_substitution solid/lsp/bad_substitution.cpp)

add_executable(solid_ocp_ostream_op_preferred
               solid/ocp/ostream_op_preferred.cpp)
add_executable(solid_ocp_ostream_op_private_access_needed
               solid/ocp/ostream_op_private_access_needed.cpp)
