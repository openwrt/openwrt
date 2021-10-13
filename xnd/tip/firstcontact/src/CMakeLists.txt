cmake_minimum_required(VERSION 2.6)

PROJECT(firstcontact C)
INCLUDE(GNUInstallDirs)
ADD_DEFINITIONS(-Os -ggdb -Wall -Werror --std=gnu99 -Wmissing-declarations)

SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")

ADD_EXECUTABLE(firstcontact firstcontact.c)
TARGET_LINK_LIBRARIES(firstcontact curl crypto ssl ubox)
INSTALL(TARGETS firstcontact
	RUNTIME DESTINATION ${CMAKE_INSTALL_SBINDIR}
)
