cmake_minimum_required(VERSION 3.13)

PROJECT(ucode-uht C)
ADD_DEFINITIONS(-Os -ggdb -Wall -Werror --std=gnu99 -ffunction-sections -fwrapv -D_GNU_SOURCE)

IF(CMAKE_C_COMPILER_VERSION VERSION_GREATER 6)
	ADD_DEFINITIONS(-Wextra -Werror=implicit-function-declaration)
	ADD_DEFINITIONS(-Wformat -Werror=format-security -Werror=format-nonliteral)
ENDIF()
ADD_DEFINITIONS(-Wmissing-declarations -Wno-error=unused-variable -Wno-unused-parameter)

IF(APPLE)
  SET(UCODE_MODULE_LINK_OPTIONS "LINKER:-undefined,dynamic_lookup")
  ADD_DEFINITIONS(-DBIND_8_COMPAT)
ELSE()
  SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "-Wl,--gc-sections")
ENDIF()

IF(DEBUG)
  ADD_DEFINITIONS(-DDEBUG -g3 -O0)
ELSE()
  ADD_DEFINITIONS(-DNDEBUG)
ENDIF()

FIND_LIBRARY(ubox NAMES ubox)
FIND_LIBRARY(ucode NAMES ucode)
FIND_PATH(uloop_include_dir NAMES libubox/uloop.h)
FIND_PATH(ucode_include_dir NAMES ucode/module.h)
INCLUDE_DIRECTORIES(${uloop_include_dir} ${ucode_include_dir})

ADD_LIBRARY(uht_lib MODULE ucode.c uht.c xxhash32.c)
SET_TARGET_PROPERTIES(uht_lib PROPERTIES OUTPUT_NAME uht PREFIX "")
TARGET_LINK_OPTIONS(uht_lib PRIVATE ${UCODE_MODULE_LINK_OPTIONS})
TARGET_LINK_LIBRARIES(uht_lib ${ubox})

INSTALL(TARGETS uht_lib LIBRARY DESTINATION lib/ucode)
