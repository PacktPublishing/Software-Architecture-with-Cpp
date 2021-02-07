include(FetchContent)

FetchContent_Declare(
  cppunit
  GIT_REPOSITORY https://github.com/aostrowski/CppUnit.git
  GIT_TAG master
  GIT_SHALLOW ON)
FetchContent_MakeAvailable(cppunit)
message(STATUS "CppUnit fetched successfully")
