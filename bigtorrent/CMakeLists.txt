set(SRC src/main.cpp src/torrent.cpp)
set(HDR include/main.h include/torrent.h)

add_executable(bigtorrent ${SRC} ${HDR})

target_link_libraries(bigtorrent PUBLIC bencode)
target_include_directories(bigtorrent PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../bencode/include/ include)
