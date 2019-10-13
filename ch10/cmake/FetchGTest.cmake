include(FetchContent)

macro(fetch_gtest)
    FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            # GIT_TAG        release-1.10.0
            GIT_TAG master # live at head
    )

    FetchContent_GetProperties(googletest)
    if (NOT googletest_POPULATED)
        FetchContent_Populate(googletest)
        add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)
    endif ()

    message(STATUS "GTest binaries are present at ${googletest_BINARY_DIR}")
endmacro()