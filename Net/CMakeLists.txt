cmake_minimum_required(VERSION 2.8.12.2)
project(NetTool)

file(GLOB SRCS src/*.cpp)

add_library(net_tool SHARED ${SRCS})
target_include_directories(net_tool PRIVATE include/ ../Log/)
target_link_libraries(net_tool PRIVATE sqlite3)

add_executable(test ${SRCS} test/main.cpp)
target_include_directories(test PRIVATE  include/ ../Log/)
target_link_libraries(test PRIVATE sqlite3)