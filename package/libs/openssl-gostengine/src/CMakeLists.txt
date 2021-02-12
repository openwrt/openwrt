cmake_minimum_required(VERSION 2.8)
project(ccgost C)

# -- Fix for Entware & Entware-ng (Tested with OpenSSL 1.1.1i)
find_package(OpenSSL REQUIRED)
if( OpenSSL_FOUND )
  set (OPENSSL_INCLUDE "${OPENSSL_INCLUDE_DIR}/")
  include_directories(${OPENSSL_INCLUDE})
  message(STATUS "Using OpenSSL ${OPENSSL_VERSION}")
  message(STATUS "OpenSSL include dir: ${OPENSSL_INCLUDE}")
  message(STATUS "OpenSSL libraries: ${OPENSSL_LIBRARIES}")
endif()
# -- End Fix for Entware & Entware-ng (Tested with OpenSSL 1.1.1i)

if (CMAKE_C_COMPILER_ID MATCHES "Clang")
   message(STATUS "Using Clang and adding -Qunused-arguments flag")
   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Qunused-arguments")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c98 -O3 -Werror -Qunused-arguments -Wno-unused-function -Wno-missing-braces -Wall")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -Werror -Wall -Wno-unused-parameter -Wno-unused-function -Wno-missing-braces -ggdb")

include (TestBigEndian)
TEST_BIG_ENDIAN(IS_BIG_ENDIAN)
if(IS_BIG_ENDIAN)
 message(STATUS "BIG_ENDIAN")
else()
 message(STATUS "LITTLE_ENDIAN")
 set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c98 -O3 -Werror -Wall")
 set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DL_ENDIAN")
endif()

set(BIN_DIRECTORY bin)

set(OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/${BIN_DIRECTORY})

#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY})

set(GOST_89_SOURCE_FILES
        gost89.c
        gost89.h
        )

set(GOST_HASH_SOURCE_FILES
        gosthash.c
        gosthash.h
        )

set(GOST_HASH_2012_SOURCE_FILES
        gosthash2012.c
        gosthash2012.h
        gosthash2012_const.h
        gosthash2012_precalc.h
        gosthash2012_ref.h
        gosthash2012_sse2.h
        )

set(GOST_GRASSHOPPER_SOURCE_FILES
        gost_grasshopper.h
        gost_grasshopper_core.h
        gost_grasshopper_core.c
        gost_grasshopper_defines.h
        gost_grasshopper_defines.c
        gost_grasshopper_math.h
        gost_grasshopper_galois_precompiled.c
        gost_grasshopper_precompiled.c
        gost_grasshopper_cipher.h
        gost_grasshopper_cipher.c
        gost_grasshopper_mac.h
        gost_grasshopper_mac.c
        )

set(GOST_CORE_SOURCE_FILES
        e_gost_err.c
        e_gost_err.h
        gost_asn1.c
        gost_crypt.c
        gost_ctl.c
        gost_eng.c
        gost_keywrap.c
        gost_keywrap.h
        gost_lcl.h
        gost_params.c
        )

set(GOST_EC_SOURCE_FILES
        gost_ec_keyx.c
        gost_ec_sign.c
        )

set(GOST_LIB_SOURCE_FILES
        ${GOST_EC_SOURCE_FILES}
        ${GOST_89_SOURCE_FILES}
        ${GOST_HASH_SOURCE_FILES}
        ${GOST_GRASSHOPPER_SOURCE_FILES}
        ${GOST_HASH_2012_SOURCE_FILES})

set(GOST_ENGINE_SOURCE_FILES
        ${GOST_CORE_SOURCE_FILES}
        gost_ameth.c
        gost_md.c
        gost_md2012.c
        gost_pmeth.c)

add_library(gost STATIC ${GOST_LIB_SOURCE_FILES})
set_target_properties(gost PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_library(gost_engine MODULE ${GOST_ENGINE_SOURCE_FILES})
set_target_properties(gost_engine PROPERTIES PREFIX "" OUTPUT_NAME "gost")

target_link_libraries(gost_engine crypto gost)

set(GOST_12_SUM_SOURCE_FILES
        gost12sum.c
        )

add_executable(gost12sum ${GOST_12_SUM_SOURCE_FILES})

target_link_libraries(gost12sum gost)

set(GOST_SUM_SOURCE_FILES
        gostsum.c
        )

add_executable(gostsum ${GOST_SUM_SOURCE_FILES})

target_link_libraries(gostsum gost)

set(GOST_SUM_12_SOURCE_FILES
        gostsum12.c
        )

# -- Fix for Entware & Entware-ng (Tested with OpenSSL 1.1.1i)
INSTALL(
  TARGETS gost
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib  # <-- Add this line
  COMPONENT library
)
# -- End Fix for Entware & Entware-ng (Tested with OpenSSL 1.1.1i)