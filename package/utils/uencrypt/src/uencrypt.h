/* SPDX-License-Identifier: GPL-2.0-or-later
 * Copyright (C) 2022-2023 Eneas Ulir de Queiroz
 */

#include <stdio.h>

#define CRYPT_BUF_SIZE 1024

#ifdef USE_MBEDTLS
# include <mbedtls/cipher.h>

# if defined(MBEDTLS_MAX_BLOCK_LENGTH) \
     && MBEDTLS_MAX_BLOCK_LENGTH > CRYPT_BUF_SIZE
#  undef CRYPT_BUF_SIZE
#  define CRYPT_BUF_SIZE MAX_BLOCK_LENGTH
# endif

unsigned char *hexstr2buf(const char* str, long *len);

#else /* USE_MBEDTLS */
# ifdef USE_WOLFSSL
#  include <wolfssl/options.h>
#  include <wolfssl/openssl/evp.h>
# else
#  include <openssl/evp.h>
# endif

# if defined(EVP_MAX_BLOCK_LENGTH) \
     && EVP_MAX_BLOCK_LENGTH > CRYPT_BUF_SIZE
#  undef CRYPT_BUF_SIZE
#  define CRYPT_BUF_SIZE EVP_MAX_BLOCK_LENGTH
# endif

# define hexstr2buf OPENSSL_hexstr2buf

#endif /* USE_MBEDTLS */

typedef void cipher_t;
typedef void ctx_t;

const cipher_t *get_default_cipher(void);
const cipher_t *get_cipher_or_print_error(char *name);
int get_cipher_ivsize(const cipher_t *cipher);
int get_cipher_keysize(const cipher_t *cipher);

ctx_t *create_ctx(const cipher_t *cipher, const unsigned char *key,
		  const unsigned char *iv, int enc, int padding);
int do_crypt(FILE *infile, FILE *outfile, ctx_t *ctx);
void free_ctx(ctx_t *ctx);
