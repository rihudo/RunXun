cmake_minimum_required(VERSION 2.8.12.2)
project(NetTool)

file(GLOB SRCS src/*.cpp)

add_library(net_tool SHARED ${SRCS})
target_include_directories(net_tool PUBLIC include/ ../Log/)
target_link_libraries(net_tool PUBLIC sqlite3)

add_executable(test test/main.cpp)
target_link_libraries(test PRIVATE net_tool)