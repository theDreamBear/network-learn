cmake_minimum_required(VERSION 3.21)
project(calculator)

set(CMAKE_CXX_STANDARD 17)

add_executable(calculator server.cpp protocal.h)

add_executable(client client.cpp protocal.h)
