cmake_minimum_required(VERSION 2.8.12.2)
project(NetTool)

file(GLOB SRCS ${CMAKE_SOURCE_DIR}/src/*.cpp)

add_library(net_tool SHARED ${SRCS})
target_include_directories(net_tool PRIVATE ${CMAKE_SOURCE_DIR}/include/ ../Log/)

add_executable(test ${SRCS} test/main.cpp)
target_include_directories(test PRIVATE  ${CMAKE_SOURCE_DIR}/include/ ../Log/)