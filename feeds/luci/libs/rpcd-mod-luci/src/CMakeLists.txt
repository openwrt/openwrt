cmake_minimum_required(VERSION 2.6)

PROJECT(rpcd-mod-luci C)

ADD_DEFINITIONS(-Os -Wall -Werror --std=gnu99 -g3 -Wmissing-declarations)

SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")

IF(APPLE)
  INCLUDE_DIRECTORIES(/opt/local/include)
  LINK_DIRECTORIES(/opt/local/lib)
ENDIF()

IF (NOT DEFINED LIBNL_LIBS)
  FIND_LIBRARY(libnl NAMES libnl-3 libnl nl-3 nl)
  SET(LIBNL_LIBS ${libnl})
ENDIF()

ADD_LIBRARY(rpcd-mod-luci MODULE luci.c)
TARGET_LINK_LIBRARIES(rpcd-mod-luci ubox ubus uci dl ${LIBNL_LIBS})
SET_TARGET_PROPERTIES(rpcd-mod-luci PROPERTIES OUTPUT_NAME luci PREFIX "")

INSTALL(TARGETS rpcd-mod-luci LIBRARY DESTINATION lib)
