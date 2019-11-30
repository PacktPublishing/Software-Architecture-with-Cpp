include (FetchGTest)
include(GoogleTest)

fetch_gtest()

include(AddCppUnit)

add_subdirectory(customer)
