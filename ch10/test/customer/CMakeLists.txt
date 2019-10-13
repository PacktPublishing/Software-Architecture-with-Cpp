add_executable(unittests unit.cpp)
target_link_libraries(unittests domifair::libcustomer gtest gtest_main)
gtest_discover_tests(unittests)
