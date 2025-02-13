cmake_minimum_required(VERSION 2.8.12.2)
project(Logic)

file(GLOB SRCS src/*.cpp)

link_directories(../Net/build)
add_library(logic SHARED ${SRCS})
target_link_libraries(logic PUBLIC net_tool)
target_include_directories(logic PUBLIC include ../Log ../Net/include)

add_executable(test test/main.cpp)
target_link_libraries(test PRIVATE logic)