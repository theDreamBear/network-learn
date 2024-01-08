cmake_minimum_required(VERSION 3.21)
project(reflection)

set(CMAKE_CXX_STANDARD 17)

add_executable(reflection main.cpp net.h)

add_executable(client client.cpp net.h)
