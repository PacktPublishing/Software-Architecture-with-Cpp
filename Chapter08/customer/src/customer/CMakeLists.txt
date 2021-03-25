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
target_compile_options(libcustomer PRIVATE ${BASE_COMPILE_FLAGS})

add_executable(customer main.cpp)
target_compile_features(customer PRIVATE cxx_std_17)
target_link_libraries(customer PRIVATE libcustomer)
set_target_properties(customer PROPERTIES CXX_EXTENSIONS OFF)
target_compile_options(customer PRIVATE ${BASE_COMPILE_FLAGS})

# Installing

include(GNUInstallDirs) # TODO: check if works on Windows

install(
  TARGETS libcustomer customer
  EXPORT CustomerTargets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ # last slash is important here
        DESTINATION include)

if(NOT CONAN_EXPORTED)
  install(
    EXPORT CustomerTargets
    FILE CustomerTargets.cmake
    NAMESPACE domifair::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Customer)

  configure_file(${PROJECT_SOURCE_DIR}/cmake/CustomerConfig.cmake.in
                 CustomerConfig.cmake @ONLY)

  include(CMakePackageConfigHelpers)
  write_basic_package_version_file(
    CustomerConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY AnyNewerVersion)

  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/CustomerConfig.cmake
                ${CMAKE_CURRENT_BINARY_DIR}/CustomerConfigVersion.cmake
          DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Customer)
endif()

install(
  FILES ${PROJECT_SOURCE_DIR}/LICENSE
  DESTINATION $<IF:$<BOOL:${CONAN_EXPORTED}>,licenses,${CMAKE_INSTALL_DOCDIR}>)

# Exporting

export(
  TARGETS libcustomer customer
  NAMESPACE domifair::
  FILE CustomerTargets.cmake)

set(CMAKE_EXPORT_PACKAGE_REGISTRY ON)
export(PACKAGE domifair)

# Packaging support

set(CPACK_PACKAGE_VENDOR "Authors")
set(CPACK_PACKAGE_CONTACT "author@example.com")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY
    "Library and app for the Customer microservice")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
# used by installers like NSIS
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${PROJECT_SOURCE_DIR}/README.md")

list(APPEND CPACK_GENERATOR TGZ ZIP)
if(UNIX)
  find_program(DPKG_PROGRAM dpkg)
  if(DPKG_PROGRAM)
    list(APPEND CPACK_GENERATOR DEB)
    set(CPACK_DEBIAN_PACKAGE_DEPENDS
        "${CPACK_DEBIAN_PACKAGE_DEPENDS} libcpprest2.10 (>= 2.10.2-6)")
    set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
  else()
    message(STATUS "dpkg not found - won't be able to create DEB packages")
  endif()

  find_program(RPMBUILD_PROGRAM rpmbuild)
  if(RPMBUILD_PROGRAM)
    list(APPEND CPACK_GENERATOR RPM)
    set(CPACK_RPM_PACKAGE_REQUIRES
        "${CPACK_RPM_PACKAGE_REQUIRES} cpprest >= 2.10.2-6")
  else()
    message(STATUS "rpmbuild not found - won't be able to create RPM packages")
  endif()
endif()

set(CPACK_SOURCE_IGNORE_FILES /.git /dist /.*build.* /\\\\.DS_Store)

include(CPack)

# Conan support

configure_file(${PROJECT_SOURCE_DIR}/conan/conanfile.py.in
               ${CMAKE_CURRENT_BINARY_DIR}/conan/conanfile.py @ONLY)

add_custom_target(
  conan
  COMMAND
    ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/conan/test_package/
    ${CMAKE_CURRENT_BINARY_DIR}/conan/test_package
  COMMAND conan create . customer/testing -s build_type=$<CONFIG> -pr hosacpp
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/conan
  VERBATIM)
