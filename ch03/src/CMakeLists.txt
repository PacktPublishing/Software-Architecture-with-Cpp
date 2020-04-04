add_library(BreatheDemo demo.cpp)
target_include_directories(BreatheDemo PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_features(BreatheDemo PUBLIC cxx_std_11)
