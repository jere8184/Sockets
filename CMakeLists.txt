# CMake version
cmake_minimum_required(VERSION 3.10)

#C++ STD
set(CMAKE_CXX_STANDARD 23)

# Project name
project(sockets)

# Executables
add_executable(client src/client.cpp src/socket.cpp)
target_link_libraries(client Ws2_32)

add_executable(server src/server.cpp src/socket.cpp)
target_link_libraries(server Ws2_32)
