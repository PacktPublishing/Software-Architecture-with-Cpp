cmake_minimum_required(VERSION 3.3)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/toolchain.cmake"
    CACHE FILEPATH
    "Toolchain to use for building this package and dependencies")

set(HUNTER_CONFIGURATION_TYPES "Release;Debug" CACHE STRING
    "Configuration types used when building Hunter dependencies")

include(CMakeDependentOption)
include(HunterGate)

option(HUNTER_BUILD_SHARED_LIBS "Build Shared Library" ON)

HunterGate(
    URL "https://github.com/cpp-pm/hunter/archive/v0.23.249.tar.gz"
    SHA1 "d45d77d8bba9da13e9290a180e0477e90accd89b"
    LOCAL # load `${CMAKE_CURRENT_LIST_DIR}/cmake/Hunter/config.cmake`
)

project(jaegertracing VERSION 0.7.0)

option(JAEGERTRACING_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR
    CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(cxx_flags -Wall -pedantic)
  if(JAEGERTRACING_WARNINGS_AS_ERRORS)
    list(APPEND cxx_flags -Werror)
  endif()
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  list(APPEND cxx_flags -Wno-unused-private-field)
endif()

if(MSVC)
  add_definitions(-D_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING)

  # https://studiofreya.com/2018/01/06/visual-studio-2017-with-cpp17-and-boost/
  add_definitions(-DBOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE)
  add_definitions(-D_SCL_SECURE_NO_WARNINGS)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(HUNTER_ENABLED)
  set(hunter_config "CONFIG")
else()
  set(hunter_config "")
endif()

set(package_deps)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND
   CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.9")
  message(FATAL_ERROR "Must use gcc >= 4.9")
endif()

hunter_add_package(thrift)
find_package(thrift ${hunter_config} REQUIRED)
if(HUNTER_ENABLED)
  list(APPEND LIBS thrift::thrift_static)
else()
  list(APPEND LIBS ${THRIFT_LIBRARIES})
endif()
list(APPEND package_deps thrift)


hunter_add_package(opentracing-cpp)
# Not `${hunter_config}` because OpenTracing provides its own
# OpenTracingConfig.cmake file
find_package(OpenTracing CONFIG REQUIRED)
# Under Windows, link dynamically with OpenTracing
if (WIN32)
  list(APPEND LIBS OpenTracing::opentracing)
  set(OPENTRACING_LIB OpenTracing::opentracing)
else()
  list(APPEND LIBS OpenTracing::opentracing-static)
  set(OPENTRACING_LIB OpenTracing::opentracing-static)
endif()
list(APPEND package_deps OpenTracing)

hunter_add_package(nlohmann_json)
find_package(nlohmann_json CONFIG REQUIRED)
list(APPEND LIBS nlohmann_json::nlohmann_json)
list(APPEND package_deps nlohmann_json)

option(JAEGERTRACING_BUILD_EXAMPLES "Build examples" ON)

option(JAEGERTRACING_COVERAGE "Build with coverage" $ENV{COVERAGE})
option(JAEGERTRACING_BUILD_CROSSDOCK "Build crossdock" $ENV{CROSSDOCK})
cmake_dependent_option(
  JAEGERTRACING_WITH_YAML_CPP "Use yaml-cpp to parse config files" ON
  "NOT JAEGERTRACING_BUILD_CROSSDOCK" ON)

if(JAEGERTRACING_WITH_YAML_CPP)
  hunter_add_package(yaml-cpp)
  # Not `${hunter_config}` because yaml-cpp provides its own
  # yaml-cpp-config.cmake file
  find_package(yaml-cpp CONFIG REQUIRED)
  if(HUNTER_ENABLED)
      list(APPEND LIBS yaml-cpp::yaml-cpp)
  else()
      list(APPEND LIBS yaml-cpp)
  endif()
  list(APPEND package_deps yaml-cpp)
endif()

include(CTest)
if(BUILD_TESTING)
  hunter_add_package(GTest)
  find_package(GTest ${hunter_config} REQUIRED)

  if(JAEGERTRACING_COVERAGE)
      include(CodeCoverage)
      append_coverage_compiler_flags(cxx_flags)
      append_coverage_compiler_flags(link_flags)
      set(COVERAGE_EXCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/src/jaegertracing/thrift-gen/*"
                            "*Test.cpp")
  endif()
endif()

set(SRC
    src/jaegertracing/Config.cpp
    src/jaegertracing/DynamicLoad.cpp
    src/jaegertracing/LogRecord.cpp
    src/jaegertracing/Logging.cpp
    src/jaegertracing/Reference.cpp
    src/jaegertracing/Span.cpp
    src/jaegertracing/SpanContext.cpp
    src/jaegertracing/Tag.cpp
    src/jaegertracing/TraceID.cpp
    src/jaegertracing/Tracer.cpp
    src/jaegertracing/TracerFactory.cpp
    src/jaegertracing/Sender.cpp
    src/jaegertracing/ThriftSender.cpp
    src/jaegertracing/baggage/BaggageSetter.cpp
    src/jaegertracing/baggage/RemoteRestrictionJSON.cpp
    src/jaegertracing/baggage/RemoteRestrictionManager.cpp
    src/jaegertracing/baggage/Restriction.cpp
    src/jaegertracing/baggage/RestrictionManager.cpp
    src/jaegertracing/baggage/RestrictionsConfig.cpp
    src/jaegertracing/metrics/Counter.cpp
    src/jaegertracing/metrics/Gauge.cpp
    src/jaegertracing/metrics/InMemoryStatsReporter.cpp
    src/jaegertracing/metrics/Metric.cpp
    src/jaegertracing/metrics/Metrics.cpp
    src/jaegertracing/metrics/NullCounter.cpp
    src/jaegertracing/metrics/NullGauge.cpp
    src/jaegertracing/metrics/NullStatsFactory.cpp
    src/jaegertracing/metrics/NullStatsReporter.cpp
    src/jaegertracing/metrics/NullTimer.cpp
    src/jaegertracing/metrics/StatsFactory.cpp
    src/jaegertracing/metrics/StatsFactoryImpl.cpp
    src/jaegertracing/metrics/StatsReporter.cpp
    src/jaegertracing/metrics/Timer.cpp
    src/jaegertracing/net/IPAddress.cpp
    src/jaegertracing/net/Socket.cpp
    src/jaegertracing/net/URI.cpp
    src/jaegertracing/net/http/Error.cpp
    src/jaegertracing/net/http/Header.cpp
    src/jaegertracing/net/http/Method.cpp
    src/jaegertracing/net/http/Request.cpp
    src/jaegertracing/net/http/Response.cpp
    src/jaegertracing/platform/Endian.cpp
    src/jaegertracing/platform/Hostname.cpp
    src/jaegertracing/propagation/Extractor.cpp
    src/jaegertracing/propagation/HeadersConfig.cpp
    src/jaegertracing/propagation/Injector.cpp
    src/jaegertracing/propagation/Propagator.cpp
    src/jaegertracing/propagation/JaegerPropagator.cpp
    src/jaegertracing/propagation/W3CPropagator.cpp
    src/jaegertracing/reporters/CompositeReporter.cpp
    src/jaegertracing/reporters/Config.cpp
    src/jaegertracing/reporters/InMemoryReporter.cpp
    src/jaegertracing/reporters/LoggingReporter.cpp
    src/jaegertracing/reporters/NullReporter.cpp
    src/jaegertracing/reporters/RemoteReporter.cpp
    src/jaegertracing/reporters/Reporter.cpp
    src/jaegertracing/samplers/AdaptiveSampler.cpp
    src/jaegertracing/samplers/Config.cpp
    src/jaegertracing/samplers/ConstSampler.cpp
    src/jaegertracing/samplers/GuaranteedThroughputProbabilisticSampler.cpp
    src/jaegertracing/samplers/ProbabilisticSampler.cpp
    src/jaegertracing/samplers/RateLimitingSampler.cpp
    src/jaegertracing/samplers/RemoteSamplingJSON.cpp
    src/jaegertracing/samplers/RemotelyControlledSampler.cpp
    src/jaegertracing/samplers/Sampler.cpp
    src/jaegertracing/samplers/SamplingStatus.cpp
    src/jaegertracing/thrift-gen/Agent.cpp
    src/jaegertracing/thrift-gen/AggregationValidator.cpp
    src/jaegertracing/thrift-gen/BaggageRestrictionManager.cpp
    src/jaegertracing/thrift-gen/Collector.cpp
    src/jaegertracing/thrift-gen/Dependency.cpp
    src/jaegertracing/thrift-gen/SamplingManager.cpp
    src/jaegertracing/thrift-gen/ZipkinCollector.cpp
    src/jaegertracing/thrift-gen/agent_constants.cpp
    src/jaegertracing/thrift-gen/agent_types.cpp
    src/jaegertracing/thrift-gen/aggregation_validator_constants.cpp
    src/jaegertracing/thrift-gen/aggregation_validator_types.cpp
    src/jaegertracing/thrift-gen/baggage_constants.cpp
    src/jaegertracing/thrift-gen/baggage_types.cpp
    src/jaegertracing/thrift-gen/dependency_constants.cpp
    src/jaegertracing/thrift-gen/dependency_types.cpp
    src/jaegertracing/thrift-gen/jaeger_constants.cpp
    src/jaegertracing/thrift-gen/jaeger_types.cpp
    src/jaegertracing/thrift-gen/sampling_constants.cpp
    src/jaegertracing/thrift-gen/sampling_types.cpp
    src/jaegertracing/thrift-gen/zipkincore_constants.cpp
    src/jaegertracing/thrift-gen/zipkincore_types.cpp
    src/jaegertracing/utils/ErrorUtil.cpp
    src/jaegertracing/utils/HexParsing.cpp
    src/jaegertracing/utils/EnvVariable.cpp
    src/jaegertracing/utils/RateLimiter.cpp
    src/jaegertracing/utils/UDPTransporter.cpp
    src/jaegertracing/utils/HTTPTransporter.cpp
    src/jaegertracing/utils/YAML.cpp
    src/jaegertracing/ThriftMethods.cpp)

if(JAEGERTRACING_BUILD_CROSSDOCK)
  list(APPEND SRC
    src/jaegertracing/thrift-gen/TracedService.cpp
    src/jaegertracing/thrift-gen/tracetest_constants.cpp
    src/jaegertracing/thrift-gen/tracetest_types.cpp)
endif()

function(add_lib_deps lib)
  target_include_directories(${lib} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/src>)
  target_link_libraries(${lib} PUBLIC ${link_flags} ${LIBS})
  target_compile_options(${lib} PUBLIC ${cxx_flags})
endfunction()

function(update_path_for_test atest)
  if(WIN32)
    # If dependent library is a DLL we have to add location of DLL to PATH
    set(new_path "")

    foreach(LIB OpenTracing::opentracing yaml-cpp::yaml-cpp GTest::main)
      list(APPEND new_path $<TARGET_FILE_DIR:${LIB}>)
    endforeach()
    list(APPEND new_path $ENV{PATH})
    string(REPLACE ";" "\\;" new_path "${new_path}")

    set_tests_properties(${atest} PROPERTIES ENVIRONMENT "PATH=${new_path}")
  endif()
endfunction()

option(JAEGERTRACING_PLUGIN "Build dynamic plugin" OFF)
cmake_dependent_option(BUILD_SHARED_LIBS "Build shared libraries" ON
  "NOT JAEGERTRACING_PLUGIN" OFF)

if(BUILD_SHARED_LIBS)
  add_library(jaegertracing SHARED ${SRC})
  if (WIN32)
    target_compile_definitions(jaegertracing PUBLIC YAML_CPP_DLL)
    target_link_libraries(jaegertracing PUBLIC Iphlpapi Ws2_32)
  endif()
  add_lib_deps(jaegertracing)
  set_target_properties(jaegertracing PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR})
  set(JAEGERTRACING_LIB jaegertracing)
  list(APPEND JAEGERTRACING_LIBS jaegertracing)
endif()


if(JAEGERTRACING_PLUGIN)
  file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/export.map
    "{ global: OpenTracingMakeTracerFactory; local: *; };")
  add_library(jaegertracing_plugin MODULE ${SRC})
  add_lib_deps(jaegertracing_plugin)
  target_link_libraries(jaegertracing_plugin PUBLIC
    -static-libgcc
    -static-libstdc++
    -Wl,--version-script=${CMAKE_CURRENT_BINARY_DIR}/export.map)
endif()

add_library(jaegertracing-static STATIC ${SRC})
if (NOT WIN32)
  #on windows, the shared library generate also a .lib file
  set_target_properties(jaegertracing-static PROPERTIES OUTPUT_NAME jaegertracing)
endif()

add_lib_deps(jaegertracing-static)
if (WIN32)
  target_link_libraries(jaegertracing-static PUBLIC Iphlpapi Ws2_32)
endif()

if(NOT JAEGERTRACING_LIB)
  set(JAEGERTRACING_LIB jaegertracing-static)
endif()
list(APPEND JAEGERTRACING_LIBS jaegertracing-static)

configure_file(
  src/jaegertracing/Constants.h.in
  src/jaegertracing/Constants.h
  @ONLY)

if(JAEGERTRACING_BUILD_EXAMPLES)
  add_executable(app examples/App.cpp)
  target_link_libraries(app PUBLIC ${JAEGERTRACING_LIB})
endif()

if(BUILD_TESTING)
  add_library(testutils
      src/jaegertracing/testutils/TUDPTransport.cpp
      src/jaegertracing/testutils/SamplingManager.cpp
      src/jaegertracing/testutils/MockAgent.cpp
      src/jaegertracing/testutils/TracerUtil.cpp)
  target_link_libraries(testutils PUBLIC ${JAEGERTRACING_LIB})

  add_executable(UnitTest
      src/jaegertracing/ConfigTest.cpp
      src/jaegertracing/ReferenceTest.cpp
      src/jaegertracing/SpanContextTest.cpp
      src/jaegertracing/SpanTest.cpp
      src/jaegertracing/TagTest.cpp
      src/jaegertracing/TraceIDTest.cpp
      src/jaegertracing/TracerFactoryTest.cpp
      src/jaegertracing/TracerTest.cpp
      src/jaegertracing/ThriftSenderTest.cpp
      src/jaegertracing/baggage/BaggageTest.cpp
      src/jaegertracing/metrics/MetricsTest.cpp
      src/jaegertracing/metrics/NullStatsFactoryTest.cpp
      src/jaegertracing/net/IPAddressTest.cpp
      src/jaegertracing/net/SocketTest.cpp
      src/jaegertracing/net/URITest.cpp
      src/jaegertracing/net/http/HeaderTest.cpp
      src/jaegertracing/net/http/MethodTest.cpp
      src/jaegertracing/net/http/ResponseTest.cpp
      src/jaegertracing/propagation/PropagatorTest.cpp
      src/jaegertracing/propagation/W3CPropagatorTest.cpp
      src/jaegertracing/reporters/ConfigTest.cpp
      src/jaegertracing/reporters/ReporterTest.cpp
      src/jaegertracing/samplers/SamplerTest.cpp
      src/jaegertracing/testutils/MockAgentTest.cpp
      src/jaegertracing/testutils/TUDPTransportTest.cpp
      src/jaegertracing/utils/ErrorUtilTest.cpp
      src/jaegertracing/utils/RateLimiterTest.cpp
      src/jaegertracing/utils/UDPSenderTest.cpp
      src/jaegertracing/utils/HTTPTransporterTest.cpp)
  target_link_libraries(
      UnitTest PRIVATE testutils PUBLIC GTest::main ${JAEGERTRACING_LIB})
  add_test(NAME UnitTest COMMAND UnitTest)

  update_path_for_test(UnitTest)

  if(TARGET jaegertracing)
    add_executable(DynamicallyLoadTracerTest
      src/jaegertracing/DynamicallyLoadTracerTest.cpp)

    target_include_directories(DynamicallyLoadTracerTest PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/src>)
    target_link_libraries(
      DynamicallyLoadTracerTest ${OPENTRACING_LIB} GTest::main)
    add_test(NAME DynamicallyLoadTracerTest
             COMMAND DynamicallyLoadTracerTest $<TARGET_FILE:jaegertracing>)
    update_path_for_test(DynamicallyLoadTracerTest)
    if(JAEGERTRACING_COVERAGE)
        setup_target_for_coverage(NAME UnitTestCoverage
                                  EXECUTABLE UnitTest
                                  DEPENDENCIES UnitTest)
    endif()
  endif()
endif()



if(JAEGERTRACING_BUILD_CROSSDOCK)
  set(CROSSDOCK_SRC crossdock/Server.cpp)
  add_executable(crossdock ${CROSSDOCK_SRC})
  target_include_directories(crossdock PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/crossdock>)
  target_link_libraries(crossdock PUBLIC ${JAEGERTRACING_LIB})

  string(CONCAT JAEGER_CROSSDOCK_URL
         "https://raw.githubusercontent.com/"
         "jaegertracing/jaeger/master/docker-compose/jaeger-docker-compose.yml")
  file(DOWNLOAD ${JAEGER_CROSSDOCK_URL}
       "${CMAKE_CURRENT_SOURCE_DIR}/crossdock/jaeger-docker-compose.yml")
  find_program(DOCKER_COMPOSE_EXE docker-compose REQUIRED)
  set(DOCKER_COMPOSE_CMD ${DOCKER_COMPOSE_EXE}
      -f ${CMAKE_CURRENT_SOURCE_DIR}/crossdock/docker-compose.yml
      -f ${CMAKE_CURRENT_SOURCE_DIR}/crossdock/jaeger-docker-compose.yml)
  add_custom_target(crossdock-kill
    COMMAND ${DOCKER_COMPOSE_CMD} down)
  add_custom_target(crossdock-run
    COMMAND ${DOCKER_COMPOSE_CMD} build
    COMMAND ${DOCKER_COMPOSE_CMD} run crossdock
    DEPENDS crossdock-kill
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  add_custom_target(crossdock-fresh
    COMMAND ${DOCKER_COMPOSE_CMD} build --pull --no-cache
    COMMAND ${DOCKER_COMPOSE_CMD} run crossdock
    DEPENDS crossdock-kill
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  add_custom_target(crossdock-logs
    COMMAND ${DOCKER_COMPOSE_CMD} logs)
endif()



# Installation (https://github.com/forexample/package-example)

# Introduce variables:
# * CMAKE_INSTALL_LIBDIR
# * CMAKE_INSTALL_BINDIR
# * CMAKE_INSTALL_INCLUDEDIR
include(GNUInstallDirs)

# Layout. This works for all platforms:
#   * <prefix>/lib*/cmake/<PROJECT-NAME>
#   * <prefix>/lib*/
#   * <prefix>/include/
set(config_install_dir "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")

set(generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")

# Configuration
set(version_config "${generated_dir}/${PROJECT_NAME}ConfigVersion.cmake")
set(project_config "${generated_dir}/${PROJECT_NAME}Config.cmake")
set(TARGETS_EXPORT_NAME "${PROJECT_NAME}Targets")
set(namespace "${PROJECT_NAME}::")

# Include module with fuction 'write_basic_package_version_file'
include(CMakePackageConfigHelpers)

# Configure '<PROJECT-NAME>ConfigVersion.cmake'
# Use:
#   * PROJECT_VERSION
write_basic_package_version_file(
  "${version_config}" COMPATIBILITY SameMajorVersion
)

# Configure '<PROJECT-NAME>Config.cmake'
# Use variables:
#   * TARGETS_EXPORT_NAME
#   * PROJECT_NAME
configure_package_config_file(
  "cmake/Config.cmake.in"
  "${project_config}"
  INSTALL_DESTINATION "${config_install_dir}"
)

# Targets:
#   * <prefix>/lib*/libjaegertracing.a
#   * <prefix>/lib*/libjaegertracing.so
#   * header location after install: <prefix>/include/jaegertracing/Tracer.h
#   * headers can be included by C++ code `#include <jaegertracing/Tracer.h>`
install(
  TARGETS ${JAEGERTRACING_LIBS}
  EXPORT "${TARGETS_EXPORT_NAME}"
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
  INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

# Headers:
#   * src/jaegertracing/Tracer.h -> <prefix>/include/jaegertracing/Tracer.h
install(DIRECTORY "src/jaegertracing"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
        FILES_MATCHING
        PATTERN "*.h"
        PATTERN "testutils/*.h" EXCLUDE)

#   * build/src/jaegertracing/Constants.h ->
#     <prefix>/include/jaegertracing/Constants.h
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/src/jaegertracing/Constants.h"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/jaegertracing")

# Config
#   * <prefix>/lib*/cmake/jaegertracing/jaegertracingConfig.cmake
#   * <prefix>/lib*/cmake/jaegertracing/jaegertracingConfigVersion.cmake
install(
  FILES "${project_config}" "${version_config}"
  DESTINATION "${config_install_dir}"
)

# Config
#   * <prefix>/lib*/cmake/jaegertracing/jaegertracingTargets.cmake
install(
  EXPORT "${TARGETS_EXPORT_NAME}"
  NAMESPACE "${namespace}"
  DESTINATION "${config_install_dir}"
)
