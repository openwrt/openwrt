cmake_minimum_required(VERSION 2.6)

PROJECT(maverick C)
INCLUDE(GNUInstallDirs)
ADD_DEFINITIONS(-Os -ggdb -Wall -Werror --std=gnu99 -Wmissing-declarations)

SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")

ADD_EXECUTABLE(maverick maverick.c)
TARGET_LINK_LIBRARIES(maverick ubox)
INSTALL(TARGETS maverick
	RUNTIME DESTINATION ${CMAKE_INSTALL_SBINDIR}
)
