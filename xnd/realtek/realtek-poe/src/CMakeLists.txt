cmake_minimum_required(VERSION 2.6)

PROJECT(realtek-poe C)
ADD_DEFINITIONS(-Os -ggdb -Wextra -Wall -Werror --std=gnu99 -Wmissing-declarations -Wno-unused-parameter)

SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")

SET(SOURCES main.c)

IF(DEBUG)
  ADD_DEFINITIONS(-DDEBUG -g3)
ENDIF()

FIND_LIBRARY(ubus NAMES ubus)
FIND_LIBRARY(uci NAMES uci)
FIND_LIBRARY(ubox NAMES ubox)
FIND_PATH(uci_include_dir NAMES uci.h)
FIND_PATH(ubus_include_dir NAMES libubus.h)
FIND_PATH(ubox_include_dir NAMES libubox/usock.h)
INCLUDE_DIRECTORIES(${ubox_include_dir} ${ubus_include_dir} ${uci_include_dir})

ADD_EXECUTABLE(realtek-poe ${SOURCES})

TARGET_LINK_LIBRARIES(realtek-poe ${ubox} ${ubus} ${uci})

INSTALL(TARGETS realtek-poe
	RUNTIME DESTINATION sbin
)
