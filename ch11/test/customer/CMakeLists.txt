add_executable(catch2tests catch.cpp)
target_link_libraries(catch2tests PRIVATE domifair::libcustomer CONAN_PKG::Catch2)

add_executable(gtests gtest.cpp)
target_link_libraries(gtests PRIVATE domifair::libcustomer gtest gtest_main)
gtest_discover_tests(gtests)

add_executable(cppunittests cppunit.cpp)
target_link_libraries(cppunittests PRIVATE domifair::libcustomer cppunit)

include(doctest)

add_executable(doctests doctest.cpp)
target_link_libraries(doctests PRIVATE domifair::libcustomer CONAN_PKG::doctest)
doctest_discover_tests(doctests)

add_library(compiletimetests OBJECT compiletime.cpp)
target_compile_features(compiletimetests PRIVATE cxx_std_17)
target_link_libraries(doctests PRIVATE domifair::libcustomer)
