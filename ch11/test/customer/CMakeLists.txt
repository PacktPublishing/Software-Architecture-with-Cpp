add_executable(catch2tests catch.cpp)
target_link_libraries(catch2tests PRIVATE domifair::libcustomer CONAN_PKG::Catch2)

add_executable(unittests unit.cpp)
target_link_libraries(unittests PRIVATE domifair::libcustomer gtest gtest_main)
gtest_discover_tests(unittests)

add_executable(cppunittests cppunit.cpp)
target_link_libraries(cppunittests PRIVATE domifair::libcustomer cppunit)
