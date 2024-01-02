cmake_minimum_required(VERSION 3.21)
project(hello_world)

set(CMAKE_CXX_STANDARD 20)

add_executable(hello_server tcp_hello_server.cpp)

add_executable(hello_client tcp_hello_client.cpp)