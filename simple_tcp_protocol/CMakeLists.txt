cmake_minimum_required(VERSION 3.21)
project(simple_tcp_protocol)

set(CMAKE_CXX_STANDARD 14)

add_executable(server server.cpp simple_protocol.h)

add_executable(client client.cpp simple_protocol.h)
