cmake_minimum_required(VERSION 3.15)
project(fileTransfer)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -pthread -g")
set(CMAKE_BUILD_TYPE Debug)
add_executable(fileTransfer main.cpp)
add_executable(client client.cpp)