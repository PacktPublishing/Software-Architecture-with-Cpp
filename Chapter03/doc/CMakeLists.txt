find_package(Doxygen)

if(NOT DOXYGEN_FOUND)
  return()
endif()

# just output XML since this is what Breathe consumes
set(DOXYGEN_GENERATE_HTML NO)
set(DOXYGEN_GENERATE_XML YES)

# don't generate documentation from implementation details
set(DOXYGEN_EXCLUDE_PATTERNS "*/detail/*")

# print paths relative to include directory
set(DOXYGEN_STRIP_FROM_PATH ${PROJECT_SOURCE_DIR}/include)

# Note: use doxygen_add_docs(doxygen-doc ALL ...) if you want your documentation
# to be created by default each time you build. Without the keyword you need to
# explicitly invoke building of the 'doc' target.
doxygen_add_docs(doxygen-doc ${PROJECT_SOURCE_DIR}/include COMMENT
                 "Generating API documentation with Doxygen")

find_package(Sphinx REQUIRED)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/conf.py.in
               ${CMAKE_CURRENT_BINARY_DIR}/conf.py @ONLY)
add_custom_target(
  sphinx-doc ALL
  COMMAND ${SPHINX_EXECUTABLE} -b html -c ${CMAKE_CURRENT_BINARY_DIR}
          ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Generating API documentation with Sphinx"
  VERBATIM)

add_dependencies(sphinx-doc doxygen-doc)
