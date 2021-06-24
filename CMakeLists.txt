
cmake_minimum_required(VERSION 3.10)

project(libeventL)

add_definitions(-std=c++11)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory(src/epollDemo)
add_subdirectory(src/reactor)
add_subdirectory(src/eventBaseDemo)
