add_library(merchants STATIC visited_merchant_history.cpp reviews.cpp)
target_compile_features(merchants PUBLIC cxx_std_20)
target_include_directories(merchants PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_include_directories(
  merchants PUBLIC "${PROJECT_SOURCE_DIR}/../../3rd-parties/SaferCPlusPlus")
target_link_libraries(merchants PRIVATE Microsoft.GSL::GSL)
