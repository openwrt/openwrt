/* SPDX-License-Identifier: GPL-2.0-or-later
 * Copyright (C) 2022-2023 Eneas Ulir de Queiroz
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "uencrypt.h"

const cipher_t *get_default_cipher(void)
{
    return EVP_aes_128_cbc();
}

#ifndef USE_WOLFSSL
static void print_ciphers(const OBJ_NAME *name,void *arg) {
    fprintf(arg, "\t%s\n", name->name);
}
#endif

const cipher_t *get_cipher_or_print_error(char *name)
{
    const EVP_CIPHER *cipher;

    if ((cipher = EVP_get_cipherbyname(name)))
	return cipher;

    fprintf(stderr, "Error: invalid cipher: %s.\n", name);
#ifndef USE_WOLFSSL
    fprintf(stderr, "Supported ciphers: \n");
    OBJ_NAME_do_all_sorted(OBJ_NAME_TYPE_CIPHER_METH, print_ciphers, stderr);
#endif
    return NULL;
}

int get_cipher_ivsize(const cipher_t *cipher)
{
    return EVP_CIPHER_iv_length(cipher);
}

int get_cipher_keysize(const cipher_t *cipher)
{
    return EVP_CIPHER_key_length(cipher);
}

ctx_t *create_ctx(const cipher_t *cipher, const unsigned char *key,
		  const unsigned char *iv, int enc, int padding)
{
    EVP_CIPHER_CTX *ctx;
    int ret;

    ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
	fprintf (stderr, "Error: create_ctx: out of memory.\n");
	return NULL;
    }
    ret = EVP_CipherInit_ex(ctx, cipher, NULL, key, iv, enc);
    if (!ret) {
	fprintf(stderr, "Error:EVP_CipherInit_ex: %d\n", ret);
	goto abort;
    }
    ret = EVP_CIPHER_CTX_set_padding(ctx, padding);
    if (!ret) {
	fprintf(stderr, "Error:EVP_CIPHER_CTX_set_padding: %d\n", ret);
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
    unsigned char outbuf[CRYPT_BUF_SIZE + EVP_MAX_BLOCK_LENGTH];
    int inlen, outlen;
    int ret;

    for (;;) {
	inlen = fread(inbuf, 1, CRYPT_BUF_SIZE, infile);
	if (inlen <= 0)
	    break;
	ret = EVP_CipherUpdate(ctx, outbuf, &outlen, inbuf, inlen);
	if (!ret) {
	    fprintf(stderr, "Error: EVP_CipherUpdate: %d\n", ret);
	    return ret;
	}
	ret = fwrite(outbuf, 1, outlen, outfile);
	if (ret != outlen) {
	    fprintf(stderr, "Error: CipherUpdate short write.\n");
	    return ret - outlen;
	}
    }
    ret = EVP_CipherFinal_ex(ctx, outbuf, &outlen);
    if (!ret) {
	fprintf(stderr, "Error: EVP_CipherFinal: %d\n", ret);
	return ret;
    }
    ret = fwrite(outbuf, 1, outlen, outfile);
    if (ret != outlen) {
	fprintf(stderr, "Error: CipherFinal short write.\n");
	return ret - outlen;
    }

    return 0;
}

void free_ctx(ctx_t *ctx)
{
    EVP_CIPHER_CTX_free(ctx);
}
