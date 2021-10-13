cmake_minimum_required(VERSION 2.6)

PROJECT(ustatus C)
INCLUDE(GNUInstallDirs)
ADD_DEFINITIONS(-Os -ggdb -Wall -Werror --std=gnu99 -Wmissing-declarations -D__KERNEL__)

SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")

ADD_EXECUTABLE(xdpload xdpload.c xdp.c map.c prog.c net.c)
TARGET_LINK_LIBRARIES(xdpload bpf nl-tiny)
INSTALL(TARGETS xdpload
	RUNTIME DESTINATION ${CMAKE_INSTALL_SBINDIR}
)

ADD_EXECUTABLE(xdplist xdplist.c prog.c net.c map.c)
TARGET_LINK_LIBRARIES(xdplist bpf nl-tiny)
INSTALL(TARGETS xdplist
	RUNTIME DESTINATION ${CMAKE_INSTALL_SBINDIR}
)

ADD_EXECUTABLE(uxdp uxdp.c prog.c net.c map.c)
TARGET_LINK_LIBRARIES(uxdp bpf nl-tiny)
INSTALL(TARGETS uxdp
	RUNTIME DESTINATION ${CMAKE_INSTALL_SBINDIR}
)
