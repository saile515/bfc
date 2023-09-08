cmake_minimum_required(VERSION 3.25)

project(bfc)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)

add_executable(bfc src/main.cpp)
