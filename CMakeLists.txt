# CMake version
cmake_minimum_required(VERSION 3.10)

# Project name
project(sockets)

# Set C++ standard
#set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Executable
add_executable(client client.cpp socket.cpp)
target_link_libraries(client Ws2_32)

add_executable(server server.cpp socket.cpp)
target_link_libraries(server Ws2_32)
