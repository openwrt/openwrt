cmake_minimum_required(VERSION 3.13)

PROJECT(ucode-pkgen C)
ADD_DEFINITIONS(-Os -ggdb -Wall -Werror --std=gnu99 -ffunction-sections -fwrapv -D_GNU_SOURCE)

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

FIND_LIBRARY(mbedtls NAMES mbedtls)
FIND_LIBRARY(ucode NAMES ucode)
FIND_PATH(mbedtls_include_dir NAMES mbedtls/pk.h)
FIND_PATH(ucode_include_dir NAMES ucode/module.h)
INCLUDE_DIRECTORIES(${mbedtls_include_dir} ${ucode_include_dir})

ADD_LIBRARY(pkgen_lib MODULE ucode.c pkcs12.c)
SET_TARGET_PROPERTIES(pkgen_lib PROPERTIES OUTPUT_NAME pkgen PREFIX "")
TARGET_LINK_OPTIONS(pkgen_lib PRIVATE ${UCODE_MODULE_LINK_OPTIONS})
TARGET_LINK_LIBRARIES(pkgen_lib ${mbedtls})

INSTALL(TARGETS pkgen_lib LIBRARY DESTINATION lib/ucode)
