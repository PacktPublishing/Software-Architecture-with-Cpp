add_executable(merchanttests_gmock reviewtests_gmock.cpp)
target_link_libraries(merchanttests_gmock PRIVATE merchants gmock gmock_main)
gtest_discover_tests(merchanttests_gmock)

add_executable(merchanttests_trompeloeil reviewtests_trompeloeil.cpp)
target_compile_features(merchanttests_trompeloeil PRIVATE cxx_std_17)
target_link_libraries(merchanttests_trompeloeil
                      PRIVATE merchants trompeloeil::trompeloeil Catch2::Catch2)
catch_discover_tests(merchanttests_trompeloeil)
