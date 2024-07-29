set(SRC 
    src/main.cpp 
    src/tracker.cpp
    src/tracker_response.cpp
)

set(HDR 
    include/tracker.h
    include/tracker_response.h
)

find_package(OpenSSL REQUIRED)
add_executable(bigtorrent ${SRC} ${HDR})

target_link_libraries(bigtorrent PUBLIC bencode curl OpenSSL::SSL OpenSSL::Crypto)
target_include_directories(bigtorrent PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../libbencode/include/ include)
