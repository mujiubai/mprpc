aux_source_directory(. SRC_LIST)

add_library(mprpc SHARED ${SRC_LIST} ../protobuf/rpcheader.pb.cc)

target_link_libraries(mprpc mymuduo protobuf pthread)
