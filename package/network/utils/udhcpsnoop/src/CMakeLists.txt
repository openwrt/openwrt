cmake_minimum_required(VERSION 3.10)

PROJECT(udhcpsnoop C)
INCLUDE(GNUInstallDirs)
ADD_DEFINITIONS(-Os -ggdb -Wall -Werror --std=gnu99 -Wmissing-declarations)

SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")

SET(SOURCES main.c ubus.c dev.c dhcp.c cache.c)
SET(LIBS ubox ubus)

ADD_EXECUTABLE(udhcpsnoop ${SOURCES})
TARGET_LINK_LIBRARIES(udhcpsnoop ${LIBS})
INSTALL(TARGETS udhcpsnoop
	RUNTIME DESTINATION ${CMAKE_INSTALL_SBINDIR}
)
