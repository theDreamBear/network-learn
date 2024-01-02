cmake_minimum_required(VERSION 3.21)
project(select)

set(CMAKE_CXX_STANDARD 17)

add_executable(select main.cpp)

add_executable(client client.cpp)
