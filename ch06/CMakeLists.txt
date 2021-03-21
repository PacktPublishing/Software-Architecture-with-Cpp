cmake_minimum_required(VERSION 3.15)

project(chapter_6 LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}")

find_package(ms-gsl CONFIG REQUIRED)

add_executable(01_raii_guards 01_raii_guards.cpp)
target_compile_features(01_raii_guards PRIVATE cxx_std_11)
target_link_libraries(01_raii_guards PRIVATE Microsoft.GSL::GSL)

add_executable(02_copy_move 02_copy_move.cpp)
target_compile_features(02_copy_move PRIVATE cxx_std_17)

add_executable(03_rules_of_5_and_0 03_rules_of_5_and_0.cpp)
target_compile_features(03_rules_of_5_and_0 PRIVATE cxx_std_17)

add_executable(04_hidden_friend__copy_and_swap
               04_hidden_friend__copy_and_swap.cpp)
target_compile_features(04_hidden_friend__copy_and_swap PRIVATE cxx_std_17)

add_executable(05_niebloids 05_niebloids.cpp)
target_compile_features(05_niebloids PRIVATE cxx_std_20)
target_link_libraries(05_niebloids PRIVATE Microsoft.GSL::GSL)

add_executable(06_policy-based_design 06_policy-based_design.cpp)
target_compile_features(06_policy-based_design PRIVATE cxx_std_17)

add_executable(07_static_polymorphism 07_static_polymorphism.cpp)
target_compile_features(07_static_polymorphism PRIVATE cxx_std_20)

add_executable(08_type_erasure 08_type_erasure.cpp)
target_compile_features(08_type_erasure PRIVATE cxx_std_20)

add_executable(09_factory_functions 09_factory_functions.cpp)
target_compile_features(09_factory_functions PRIVATE cxx_std_11)

add_executable(10_factory_class 10_factory_class.cpp)
target_compile_features(10_factory_class PRIVATE cxx_std_20)

add_executable(11_builder 11_builder.cpp)
target_compile_features(11_builder PRIVATE cxx_std_20)

add_executable(12_state_and_visitation 12_state_and_visitation.cpp)
target_compile_features(12_state_and_visitation PRIVATE cxx_std_20)

add_executable(13_memory_arenas 13_memory_arenas.cpp)
target_compile_features(13_memory_arenas PRIVATE cxx_std_20)
