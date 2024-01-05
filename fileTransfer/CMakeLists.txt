cmake_minimum_required(VERSION 3.15)
project(fileTransfer)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

add_executable(fileTransfer main.cpp)
add_executable(client client.cpp)