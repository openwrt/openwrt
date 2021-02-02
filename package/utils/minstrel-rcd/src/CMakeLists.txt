cmake_minimum_required(VERSION 2.6)

PROJECT(minstrel-rcd C)

ADD_DEFINITIONS(-Wall -Werror)
IF(CMAKE_C_COMPILER_VERSION VERSION_GREATER 6)
	ADD_DEFINITIONS(-Wextra -Werror=implicit-function-declaration)
	ADD_DEFINITIONS(-Wformat -Werror=format-security -Werror=format-nonliteral)
ENDIF()
ADD_DEFINITIONS(-Os -std=gnu99 -g3 -Wmissing-declarations -Wno-unused-parameter)

FIND_LIBRARY(ubox_library NAMES ubox)
FIND_PATH(ubox_include_dir libubox/usock.h)
INCLUDE_DIRECTORIES(${ubox_include_dir})

ADD_EXECUTABLE(minstrel-rcd main.c phy.c server.c client.c)
TARGET_LINK_LIBRARIES(minstrel-rcd ${ubox_library})

INSTALL(TARGETS minstrel-rcd
	RUNTIME DESTINATION sbin
)
