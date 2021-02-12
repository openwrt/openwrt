/*
 * GOST R 34.11-2012 core functions definitions.
 *
 * Copyright (c) 2013 Cryptocom LTD.
 * This file is distributed under the same license as OpenSSL.
 *
 * Author: Alexey Degtyarev <alexey@renatasystems.org>
 *
 */

#include <string.h>

#ifdef OPENSSL_IA32_SSE2
# ifdef __MMX__
#  ifdef __SSE2__
#   define __GOST3411_HAS_SSE2__
#  endif
# endif
#endif

#ifdef __GOST3411_HAS_SSE2__
# if (__GNUC__ < 4) || (__GNUC__ == 4 && __GNUC_MINOR__ < 2)
#  undef __GOST3411_HAS_SSE2__
# endif
#endif

#ifndef L_ENDIAN
# define __GOST3411_BIG_ENDIAN__
#endif
#if defined __GOST3411_HAS_SSE2__
# include "gosthash2012_sse2.h"
#else
# include "gosthash2012_ref.h"
#endif

#ifdef _MSC_VER
# define ALIGN(x) __declspec(align(x))
#else
# define ALIGN(x) __attribute__ ((__aligned__(x)))
#endif

ALIGN(16)
typedef union uint512_u {
    unsigned long long QWORD[8];
} uint512_u;

#include "gosthash2012_const.h"
#include "gosthash2012_precalc.h"

/* GOST R 34.11-2012 hash context */
ALIGN(16)
typedef struct gost2012_hash_ctx {
    ALIGN(16) unsigned char buffer[64];
    union uint512_u hash;
    union uint512_u h;
    union uint512_u N;
    union uint512_u Sigma;
    size_t bufsize;
    unsigned int digest_size;
} gost2012_hash_ctx;

void init_gost2012_hash_ctx(gost2012_hash_ctx * CTX,
                            const unsigned int digest_size);
void gost2012_hash_block(gost2012_hash_ctx * CTX,
                         const unsigned char *data, size_t len);
void gost2012_finish_hash(gost2012_hash_ctx * CTX, unsigned char *digest);
