add_executable(catch2tests catch.cpp)
target_link_libraries(catch2tests PRIVATE domifair::libcustomer Catch2::Catch2)
target_compile_options(catch2tests PRIVATE ${BASE_COMPILE_FLAGS})
catch_discover_tests(catch2tests)

add_executable(gtests gtest.cpp)
target_link_libraries(gtests PRIVATE domifair::libcustomer gtest_main)
target_compile_options(gtests PRIVATE ${BASE_COMPILE_FLAGS})
gtest_discover_tests(gtests)

add_executable(gmocks gmock.cpp)
target_link_libraries(gmocks PRIVATE domifair::libcustomer gmock_main)
target_compile_options(gmocks PRIVATE ${BASE_COMPILE_FLAGS})
target_compile_features(gmocks PRIVATE cxx_std_17)
gtest_discover_tests(gmocks)

add_executable(cppunittests cppunit.cpp)
target_link_libraries(cppunittests PRIVATE domifair::libcustomer
                                           cppunit::cppunit)
target_compile_options(cppunittests PRIVATE ${BASE_COMPILE_FLAGS})
add_test(NAME cppunittests COMMAND cppunittests)

include(doctest)

add_executable(doctests doctest.cpp)
target_link_libraries(doctests PRIVATE domifair::libcustomer doctest::doctest)
target_compile_options(doctests PRIVATE ${BASE_COMPILE_FLAGS})
doctest_discover_tests(doctests)

add_library(compiletimetests OBJECT compiletime.cpp)
target_compile_features(compiletimetests PRIVATE cxx_std_17)
target_compile_options(compiletimetests PRIVATE ${BASE_COMPILE_FLAGS})
target_link_libraries(compiletimetests PRIVATE domifair::libcustomer)
