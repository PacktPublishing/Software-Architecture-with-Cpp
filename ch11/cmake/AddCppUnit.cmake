include(ExternalProject)

ExternalProject_Add(
        cppunit_project
        GIT_REPOSITORY    git://anongit.freedesktop.org/git/libreoffice/cppunit/
        GIT_TAG           master
        SOURCE_DIR        "${CMAKE_BINARY_DIR}/cppunit-src"
        # BINARY_DIR        "${CMAKE_BINARY_DIR}/cppunit-build"
        INSTALL_DIR       "${CMAKE_CURRENT_BINARY_DIR}/cppunit"
        CONFIGURE_COMMAND autoreconf -i && automake --add-missing && ./configure --prefix "${CMAKE_CURRENT_BINARY_DIR}/cppunit"
        BUILD_COMMAND     make
        INSTALL_COMMAND   make install
        # TEST_COMMAND      make check
        LOG_DOWNLOAD      1
        LOG_BUILD         1
        BUILD_IN_SOURCE   1
)

add_library(cppunit STATIC IMPORTED)
add_dependencies(cppunit cppunit_project)

ExternalProject_Get_Property(cppunit_project INSTALL_DIR)
set_target_properties(cppunit PROPERTIES IMPORTED_LOCATION "${INSTALL_DIR}/lib/libcppunit.a")
set_target_properties(cppunit PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${INSTALL_DIR}/include")
unset(INSTALL_DIR)
