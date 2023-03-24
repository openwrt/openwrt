# SPDX-License-Identifier: GPL-2.0-or-later
# Copyright (C) 2022 Eneas Ulir de Queiroz

cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)
project(uencrypt LANGUAGES C)

option(USE_WOLFSSL "Use WolfSSL as crypto provider" OFF)
option(USE_MBEDTLS "Use mbedTLS as crypto provider" OFF)
if (USE_MBEDTLS)
	if (USE_WOLFSSL)
		message(WARNING "USE_MBEDTLS and USE_WOLFSSL are both set. Building with USE_MBEDTLS.")
	endif()
	add_definitions(-DUSE_MBEDTLS)
	find_library(MBEDCRYPTO_LIBRARY mbedcrypto REQUIRED)
	set(CRYPTO_LIBRARIES ${MBEDCRYPTO_LIBRARY})
	set(CRYPTO_SOURCES ${PROJECT_NAME}-mbedtls.c)
else()
	set(CRYPTO_SOURCES ${PROJECT_NAME}-openssl.c)
	if (USE_WOLFSSL)
		add_definitions(-DUSE_WOLFSSL)
		find_library(WOLFSSL_LIBRARY wolfssl REQUIRED)
		set(CRYPTO_LIBRARIES ${WOLFSSL_LIBRARY})
	else()
		find_package(OpenSSL REQUIRED)
		set(CRYPTO_LIBRARIES ${OPENSSL_CRYPTO_LIBRARY})
	endif()
endif()
add_executable(${PROJECT_NAME} ${PROJECT_NAME}.c ${PROJECT_NAME}.h ${CRYPTO_SOURCES})

target_link_libraries(${PROJECT_NAME} ${CRYPTO_LIBRARIES})

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
