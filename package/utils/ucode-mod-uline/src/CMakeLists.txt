cmake_minimum_required(VERSION 3.13)

PROJECT(uline C)
ADD_DEFINITIONS(-Os -ggdb -Wall -Werror --std=gnu99 -ffunction-sections -fwrapv -D_GNU_SOURCE -Wno-error=unused-function -Wno-parentheses -Wno-sign-compare)

OPTION(USE_SYSTEM_WCHAR "Use system multibyte implementation for UTF-8" OFF)
IF(CMAKE_C_COMPILER_VERSION VERSION_GREATER 6)
	ADD_DEFINITIONS(-Wextra -Werror=implicit-function-declaration)
	ADD_DEFINITIONS(-Wformat -Werror=format-security -Werror=format-nonliteral)
ENDIF()
ADD_DEFINITIONS(-Wmissing-declarations -Wno-error=unused-variable -Wno-unused-parameter)

IF(APPLE)
  SET(UCODE_MODULE_LINK_OPTIONS "LINKER:-undefined,dynamic_lookup")
ELSE()
  SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "-Wl,--gc-sections")
ENDIF()

IF(DEBUG)
  ADD_DEFINITIONS(-DDEBUG -g3 -O0)
ELSE()
  ADD_DEFINITIONS(-DNDEBUG)
ENDIF()

FIND_LIBRARY(ucode NAMES ucode)
FIND_LIBRARY(libubox NAMES ubox)
FIND_PATH(uloop_include_dir NAMES libubox/uloop.h)
FIND_PATH(ucode_include_dir NAMES ucode/module.h)
INCLUDE_DIRECTORIES(${ucode_include_dir} ${uloop_include_dir})

ADD_LIBRARY(uline STATIC uline.c utf8.c vt100.c)
set_property(TARGET uline PROPERTY POSITION_INDEPENDENT_CODE ON)
IF(USE_SYSTEM_WCHAR)
  TARGET_COMPILE_DEFINITIONS(uline PUBLIC USE_SYSTEM_WCHAR)
ENDIF()

ADD_LIBRARY(uline_lib MODULE ucode.c)
SET_TARGET_PROPERTIES(uline_lib PROPERTIES OUTPUT_NAME uline PREFIX "")
TARGET_LINK_OPTIONS(uline_lib PRIVATE ${UCODE_MODULE_LINK_OPTIONS})
TARGET_LINK_LIBRARIES(uline_lib uline ${libubox})

install(FILES uline.h DESTINATION include)
INSTALL(TARGETS uline LIBRARY DESTINATION lib)
INSTALL(TARGETS uline_lib LIBRARY DESTINATION lib/ucode)
