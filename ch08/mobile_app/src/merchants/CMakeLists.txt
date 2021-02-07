add_library(merchants STATIC visited_merchant_history.cpp reviews.cpp)
target_compile_features(merchants PUBLIC cxx_std_17)
target_include_directories(merchants PUBLIC "${PROJECT_SOURCE_DIR}/include")
