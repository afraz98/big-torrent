cmake_minimum_required(VERSION 3.16)
project(bigtorrent)

add_subdirectory(bencode)

set(SRC src/main.cpp)
set(HDR include/main.h)

add_executable(bigtorrent ${SRC} ${HDR})

# Find installed headers
find_path(BENCODE_INCLUDE_DIR bencode.h PATH_SUFFIXES include)
if(BENCODE_INCLUDE_DIR)
    target_include_directories(bigtorrent PRIVATE ${BENCODE_INCLUDE_DIR})
else()
    message(FATAL_ERROR "bencode.h header not found! Make sure to build the 'bencode' library first.")
endif()

# Link with the shared library
find_library(BENCODE_LIBRARY NAMES bencode PATH_SUFFIXES lib)
if(BENCODE_LIBRARY)
    target_link_libraries(bigtorrent PRIVATE ${BENCODE_LIBRARY})
else()
    message(FATAL_ERROR "libbencode.so library not found! Make sure to build the 'bencode' library first.")
endif()