set(LIB_NAME "bencode")
set(PROJECT_NAME ${LIB_NAME})
project("${PROJECT_NAME}")

set(LIB_VERSION_MAJOR 0)
set(LIB_VERSION_STRING 0.1)

INCLUDE(GNUInstallDirs)

set(LIBRARY_BASE_PATH ${PROJECT_SOURCE_DIR})
include_directories (${LIBRARY_BASE_PATH}/include)

set(SRC
    ${LIBRARY_BASE_PATH}/src/bencode.c
)

set(HDR
    ${LIBRARY_BASE_PATH}/include/bencode.h
)

add_library(${LIB_NAME} SHARED ${SRC})

# Add compiler definitions
add_definitions("-g")

# Install library
install (TARGETS ${LIB_NAME} DESTINATION lib)
