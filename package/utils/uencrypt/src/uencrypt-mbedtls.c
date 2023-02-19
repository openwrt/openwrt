/* SPDX-License-Identifier: GPL-2.0-or-later
 * Copyright (C) 2023 Eneas Ulir de Queiroz
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "uencrypt.h"

unsigned char *hexstr2buf(const char *str, long *len)
{
    unsigned char *buf;
    long inlen = strlen(str);

    *len = 0;
    if (inlen % 2)
	return NULL;

    *len = inlen >> 1;
    buf = malloc(*len);
    for  (long x = 0; x < *len; x++)
	sscanf(str + x * 2, "%2hhx", buf + x);
    return buf;
}

const cipher_t *get_default_cipher(void)
{
    return mbedtls_cipher_info_from_type (MBEDTLS_CIPHER_AES_128_CBC);
}

static char* upperstr(char *str) {
    for (char *s = str; *s; s++)
	*s = toupper((unsigned char) *s);
    return str;
}

const cipher_t *get_cipher_or_print_error(char *name)
{
    const mbedtls_cipher_info_t *cipher;

    cipher = mbedtls_cipher_info_from_string(upperstr(name));
    if (cipher)
	return cipher;

    fprintf(stderr, "Error: invalid cipher: %s.\n", name);
    fprintf(stderr, "Supported ciphers: \n");
    for (const int *list = mbedtls_cipher_list(); *list; list++) {
	cipher = mbedtls_cipher_info_from_type(*list);
	if (!cipher)
	    continue;
	fprintf(stderr, "\t%s\n", cipher->name);
    }
    return NULL;
}

int get_cipher_ivsize(const cipher_t *cipher)
{
    const mbedtls_cipher_info_t *c = cipher;

    return c->iv_size;
}

int get_cipher_keysize(const cipher_t *cipher)
{
    const mbedtls_cipher_info_t *c = cipher;

    return c->key_bitlen >> 3;
}

ctx_t *create_ctx(const cipher_t *cipher, const unsigned char *key,
		  const unsigned char *iv, int enc, int padding)
{
    mbedtls_cipher_context_t *ctx;
    const mbedtls_cipher_info_t *cipher_info=cipher;
    int ret;

    ctx = malloc(sizeof (mbedtls_cipher_context_t));
    if (!ctx) {
	fprintf (stderr, "Error: create_ctx: out of memory.\n");
	return NULL;
    }

    mbedtls_cipher_init(ctx);
    ret = mbedtls_cipher_setup(ctx, cipher_info);
    if (ret) {
	fprintf(stderr, "Error: mbedtls_cipher_setup: %d\n", ret);
	goto abort;
    }
    ret = mbedtls_cipher_setkey(ctx, key,
				(int) mbedtls_cipher_get_key_bitlen(ctx),
				enc ? MBEDTLS_ENCRYPT : MBEDTLS_DECRYPT);
    if (ret) {
	fprintf(stderr, "Error: mbedtls_cipher_setkey: %d\n", ret);
	goto abort;
    }
    if (iv) {
        ret = mbedtls_cipher_set_iv(ctx, iv, mbedtls_cipher_get_iv_size(ctx));
	if (ret) {
	    fprintf(stderr, "Error: mbedtls_cipher_set_iv: %d\n", ret);
	    goto abort;
	}
    }

    if (cipher_info->mode == MBEDTLS_MODE_CBC) {
	ret = mbedtls_cipher_set_padding_mode(ctx, padding ?
						   MBEDTLS_PADDING_PKCS7 :
						   MBEDTLS_PADDING_NONE);
	if (ret) {
	    fprintf(stderr, "Error: mbedtls_cipher_set_padding_mode: %d\n",
		    ret);
	    goto abort;
	}
    } else {
	if (cipher_info->block_size > 1 && padding) {
	    fprintf(stderr,
		    "Error: mbedTLS only allows padding with CBC ciphers.\n");
	    goto abort;
	}
    }

    ret = mbedtls_cipher_reset(ctx);
    if (ret) {
	fprintf(stderr, "Error: mbedtls_cipher_reset: %d\n", ret);
	goto abort;
    }
    return ctx;

abort:
    free_ctx(ctx);
    return NULL;
}

int do_crypt(FILE *infile, FILE *outfile, ctx_t *ctx)
{
    unsigned char inbuf[CRYPT_BUF_SIZE];
    unsigned char outbuf[CRYPT_BUF_SIZE + MBEDTLS_MAX_BLOCK_LENGTH];
    size_t inlen, outlen, step;
    int ret;

    if (mbedtls_cipher_get_cipher_mode(ctx) == MBEDTLS_MODE_ECB) {
	step = mbedtls_cipher_get_block_size(ctx);
	if (step > CRYPT_BUF_SIZE) {
	    step = CRYPT_BUF_SIZE;
	}
    } else {
	step = CRYPT_BUF_SIZE;
    }

    for (;;) {
	inlen = fread(inbuf, 1, step, infile);
	if (inlen <= 0)
	    break;
	ret = mbedtls_cipher_update(ctx, inbuf, inlen, outbuf, &outlen);
	if (ret) {
	    fprintf(stderr, "Error: mbedtls_cipher_update: %d\n", ret);
	    return ret;
	}
	ret = fwrite(outbuf, 1, outlen, outfile);
	if (ret != outlen) {
	    fprintf(stderr, "Error: cipher_update short write.\n");
	    return ret - outlen;
	}
    }
    ret = mbedtls_cipher_finish(ctx, outbuf, &outlen);
    if (ret) {
	fprintf(stderr, "Error: mbedtls_cipher_finish: %d\n", ret);
	return ret;
    }
    ret = fwrite(outbuf, 1, outlen, outfile);
    if (ret != outlen) {
	fprintf(stderr, "Error: cipher_finish short write.\n");
	return ret - outlen;
    }

    return 0;
}

void free_ctx(ctx_t *ctx)
{
    if (ctx) {
	mbedtls_cipher_free(ctx);
	free(ctx);
    }
}
