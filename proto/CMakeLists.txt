if(WIN32)
    find_package(Protobuf CONFIG REQUIRED)
    find_package(gRPC CONFIG REQUIRED)

    set(ALEXANDRIA_PROTOBUF_TARGET protobuf::libprotobuf)
    set(ALEXANDRIA_GRPCPP_TARGET gRPC::grpc++)
    set(ALEXANDRIA_PROTOC_EXECUTABLE $<TARGET_FILE:protobuf::protoc>)
    set(ALEXANDRIA_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(PROTOBUF REQUIRED IMPORTED_TARGET protobuf)
    pkg_check_modules(GRPCPP REQUIRED IMPORTED_TARGET grpc++)

    find_program(PROTOC_EXECUTABLE protoc REQUIRED)
    find_program(GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin REQUIRED)

    set(ALEXANDRIA_PROTOBUF_TARGET PkgConfig::PROTOBUF)
    set(ALEXANDRIA_GRPCPP_TARGET PkgConfig::GRPCPP)
    set(ALEXANDRIA_PROTOC_EXECUTABLE ${PROTOC_EXECUTABLE})
    set(ALEXANDRIA_GRPC_CPP_PLUGIN_EXECUTABLE ${GRPC_CPP_PLUGIN_EXECUTABLE})
endif()

set(PROTO_FILES
    common.proto
    book.proto
    user.proto
    auth.proto
)

set(PROTO_SRCS)
set(PROTO_HDRS)
set(GRPC_SRCS)
set(GRPC_HDRS)

foreach(proto_file ${PROTO_FILES})
    get_filename_component(proto_name ${proto_file} NAME_WE)

    set(pb_cc "${CMAKE_CURRENT_BINARY_DIR}/${proto_name}.pb.cc")
    set(pb_h "${CMAKE_CURRENT_BINARY_DIR}/${proto_name}.pb.h")
    set(grpc_cc "${CMAKE_CURRENT_BINARY_DIR}/${proto_name}.grpc.pb.cc")
    set(grpc_h "${CMAKE_CURRENT_BINARY_DIR}/${proto_name}.grpc.pb.h")

    add_custom_command(
        OUTPUT ${pb_cc} ${pb_h} ${grpc_cc} ${grpc_h}
        COMMAND ${ALEXANDRIA_PROTOC_EXECUTABLE}
        ARGS
            --cpp_out=${CMAKE_CURRENT_BINARY_DIR}
            --grpc_out=${CMAKE_CURRENT_BINARY_DIR}
            --plugin=protoc-gen-grpc=${ALEXANDRIA_GRPC_CPP_PLUGIN_EXECUTABLE}
            -I ${CMAKE_CURRENT_SOURCE_DIR}
            ${CMAKE_CURRENT_SOURCE_DIR}/${proto_file}
        DEPENDS ${proto_file}
    )

    list(APPEND PROTO_SRCS ${pb_cc})
    list(APPEND PROTO_HDRS ${pb_h})
    list(APPEND GRPC_SRCS ${grpc_cc})
    list(APPEND GRPC_HDRS ${grpc_h})
endforeach()

add_library(alexandria_proto STATIC
    ${PROTO_SRCS}
    ${GRPC_SRCS}
)

target_include_directories(alexandria_proto PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(alexandria_proto
    PUBLIC
        ${ALEXANDRIA_PROTOBUF_TARGET}
        ${ALEXANDRIA_GRPCPP_TARGET}
)