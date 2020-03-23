cmake_minimum_required(VERSION 2.6)

PROJECT(rbextract C)
ADD_DEFINITIONS(-Wall --std=gnu99 -Wmissing-declarations)

SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")

FIND_PATH(lzo_include_dir lzo/lzo1x.h)
INCLUDE_DIRECTORIES(${lzo_include_dir})

ADD_EXECUTABLE(rbextract rbextract.c rle.c)
TARGET_LINK_LIBRARIES(rbextract lzo2)

INSTALL(TARGETS rbextract RUNTIME DESTINATION sbin)
