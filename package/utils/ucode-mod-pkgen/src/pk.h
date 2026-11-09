// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2024 Felix Fietkau <nbd@nbd.name>
 */
#ifndef __UCODE_PK_H
#define __UCODE_PK_H

#include <ucode/lib.h>
#include <ucode/vm.h>

#include <mbedtls/bignum.h>
#include <mbedtls/pk.h>
#include <mbedtls/oid.h>
#include <mbedtls/error.h>
#include <mbedtls/version.h>

#if MBEDTLS_VERSION_MAJOR < 3
#define MBEDTLS_LEGACY
#endif

int random_cb(void *ctx, unsigned char *out, size_t len);
uc_value_t *uc_generate_pkcs12(uc_vm_t *vm, size_t nargs);
int64_t get_int_arg(uc_value_t *obj, const char *key, int64_t defval);
extern int mbedtls_errno;
extern char buf[32 * 1024];

#define C(ret)						\
	({						\
		int __ret = (ret);			\
		mbedtls_errno = __ret < 0 ? __ret : 0;	\
		__ret;					\
	})

#define CHECK(ret) do {					\
	if (C(ret) < 0)					\
		return NULL;				\
} while (0)
#define CHECK_INT(ret) do {				\
	if (C(ret) < 0)					\
		return -1;				\
} while (0)

#define INVALID_ARG()	do { C(-1); return NULL; } while (0)

#endif
