# .rst: FindMyDep
# ----------
#
# Find my favourite external dependency (MyDep).
#
# Imported targets
# ^^^^^^^^^^^^^^^^
#
# This module defines the following :prop_tgt:`IMPORTED` target:
#
# ``MyDep::MyDep`` The MyDep library, if found.
#
# Result variables
# ^^^^^^^^^^^^^^^^
#
# This module will set the following variables in your project:
#
# ``MyDep_FOUND`` whether MyDep was found or not ``MyDep_VERSION`` the found
# version of MyDep

find_package(Boost REQUIRED)

find_package(PkgConfig)
pkg_check_modules(PC_MyDep QUIET MyDep)

find_path(
  MyDep_INCLUDE_DIR
  NAMES MyDep.h
  PATHS "${MyDep_ROOT_DIR}/include" "${PC_MyDep_INCLUDE_DIRS}"
  PATH_SUFFIXES MyDep)

find_library(
  MyDep_LIBRARY
  NAMES mydep
  PATHS "${MyDep_ROOT_DIR}/lib" "${PC_MyDep_LIBRARY_DIRS}")

set(MyDep_VERSION ${PC_MyDep_VERSION})

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(
  MyDep
  FOUND_VAR
  MyDep_FOUND
  REQUIRED_VARS
  MyDep_LIBRARY
  MyDep_INCLUDE_DIR
  VERSION_VAR
  MyDep_VERSION)

if(MyDep_FOUND AND NOT TARGET MyDep::MyDep)
  add_library(MyDep::MyDep UNKNOWN IMPORTED)
  set_target_properties(
    MyDep::MyDep
    PROPERTIES IMPORTED_LOCATION "${MyDep_LIBRARY}" INTERFACE_COMPILE_OPTIONS
                                                    "${PC_MyDep_CFLAGS_OTHER}"
               INTERFACE_INCLUDE_DIRECTORIES "${MyDep_INCLUDE_DIR}")
endif()

mark_as_advanced(MyDep_INCLUDE_DIR MyDep_LIBRARY)
