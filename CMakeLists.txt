cmake_minimum_required(VERSION 3.8.1)
project(cpp_tcp)

find_package(Threads)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++21")

add_library(${PROJECT_NAME}
 src/main.cpp
)

option(SERVER)