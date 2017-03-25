cmake_minimum_required(VERSION 2.6)

PROJECT(fritz-tools C)
ADD_DEFINITIONS(-Wall -Werror --std=gnu99 -Wmissing-declarations)

SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")

ADD_EXECUTABLE(fritz_tffs_read fritz_tffs_read.c)
ADD_EXECUTABLE(fritz_cal_extract fritz_cal_extract.c)
TARGET_LINK_LIBRARIES(fritz_cal_extract z)

INSTALL(TARGETS fritz_tffs_read fritz_cal_extract RUNTIME DESTINATION bin)
