cmake_minimum_required(VERSION 3.21)
project(client_io_split)

set(CMAKE_CXX_STANDARD 17)

add_executable(client_io_split main.cpp)
add_executable(server server.cpp)
