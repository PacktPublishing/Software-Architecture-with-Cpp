add_library(libcustomer lib.cpp)
add_library(domifair::libcustomer ALIAS libcustomer)
set_target_properties(libcustomer PROPERTIES OUTPUT_NAME customer)
target_compile_features(libcustomer PUBLIC cxx_std_17)
target_include_directories(
  libcustomer PUBLIC $<INSTALL_INTERFACE:include>
                     $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>)
target_compile_definitions(libcustomer PUBLIC _TURN_OFF_PLATFORM_STRING
)# see https://github.com/Microsoft/cpprestsdk/issues/230
target_link_libraries(libcustomer PUBLIC cpprestsdk::cpprest)
set_target_properties(libcustomer PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(libcustomer PRIVATE jaegertracing-static)

add_executable(customer main.cpp)
target_compile_features(customer PRIVATE cxx_std_17)
target_link_libraries(customer PRIVATE libcustomer jaegertracing-static)
set_target_properties(customer PROPERTIES CXX_EXTENSIONS OFF)
