cmake_minimum_required(VERSION 2.8)

PROJECT(mt76-test C)
ADD_DEFINITIONS(-Os -Wall -Werror --std=gnu99 -g3)

ADD_EXECUTABLE(mt76-test main.c fields.c eeprom.c)
TARGET_LINK_LIBRARIES(mt76-test nl-tiny)

SET(CMAKE_INSTALL_PREFIX /usr)

INSTALL(TARGETS mt76-test
	RUNTIME DESTINATION sbin
)
