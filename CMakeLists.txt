cmake_minimum_required(VERSION 3.15)
project(simple_tftp)

set(CMAKE_CXX_STANDARD 14)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(simple_tftp_server src/tftp_server.cpp src/tftp_packet.cpp)
add_executable(simple_tftp_client src/tftp_client.cpp src/tftp_packet.cpp)