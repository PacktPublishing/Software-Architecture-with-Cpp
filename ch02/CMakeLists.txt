cmake_minimum_required(VERSION 3.15)

project(
  ch02
  VERSION 0.0.1
  LANGUAGES CXX)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(CommonCompileFlags)

find_package(Protobuf REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_search_module(GRPCPP REQUIRED grpc++)
find_program(GRPC_CPP_PLUGIN grpc_cpp_plugin REQUIRED)

get_filename_component(srv_proto "grpc/service.proto" ABSOLUTE)
get_filename_component(srv_proto_path "${srv_proto}" PATH)

set(srv_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/service.pb.cc")
set(srv_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/service.pb.h")
set(srv_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/service.grpc.pb.cc")
set(srv_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/service.grpc.pb.h")

add_custom_command(
  OUTPUT "${srv_proto_srcs}" "${srv_proto_hdrs}" "${srv_grpc_srcs}"
         "${srv_grpc_hdrs}"
  COMMAND
    ${Protobuf_PROTOC_EXECUTABLE} ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
    --cpp_out "${CMAKE_CURRENT_BINARY_DIR}" -I "${srv_proto_path}"
    --plugin=protoc-gen-grpc="${GRPC_CPP_PLUGIN}" "${srv_proto}"
  DEPENDS "${srv_proto}")

include_directories("${CMAKE_CURRENT_BINARY_DIR}")

add_executable(grpc_client grpc/AsyncClient.cpp ${srv_grpc_srcs}
                           ${srv_grpc_hdrs} ${srv_proto_srcs} ${srv_proto_hdrs})
target_link_libraries(grpc_client ${Protobuf_LIBRARIES} ${GRPCPP_LIBRARIES})

add_executable(grpc_server grpc/AsyncServer.cpp ${srv_grpc_srcs}
                           ${srv_grpc_hdrs} ${srv_proto_srcs} ${srv_proto_hdrs})
target_link_libraries(grpc_server ${Protobuf_LIBRARIES} ${GRPCPP_LIBRARIES})

find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(cpprestsdk REQUIRED)

add_executable(rest_client rest/CppRestSdkReadPage.cpp)
target_link_libraries(
  rest_client PRIVATE cpprestsdk::cpprest ${CMAKE_THREAD_LIBS_INIT}
                      ${OPENSSL_LIBRARIES})

add_executable(rest_server rest/CppRestSdkSimpleService.cpp)
target_link_libraries(
  rest_server PRIVATE cpprestsdk::cpprest ${CMAKE_THREAD_LIBS_INIT}
                      ${OPENSSL_LIBRARIES})
