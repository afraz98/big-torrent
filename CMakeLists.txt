cmake_minimum_required(VERSION 3.16)
project(bigtorrent)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(bencode)
add_subdirectory(bigtorrent)

