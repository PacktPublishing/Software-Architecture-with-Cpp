add_executable(unittests unit.cpp)
target_compile_options(unittests PRIVATE ${BASE_COMPILE_FLAGS})
target_link_libraries(unittests PRIVATE domifair::libcustomer gtest_main)
gtest_discover_tests(unittests)
