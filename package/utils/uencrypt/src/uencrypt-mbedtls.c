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

static char* upperstr(char *str) {
    for (char *s = str; *s; s++)
        *s = toupper((unsigned char) *s);
    return str;
}

struct cipher_def {
    const char *name;
    psa_key_type_t key_type;
    size_t key_bits;
    psa_algorithm_t alg;
    size_t iv_size;
    size_t block_size;
};

struct cipher_ctx {
    psa_cipher_operation_t op;
    mbedtls_svc_key_id_t key_id;
    size_t block_size;
};

static const struct cipher_def ciphers[] = {
#if defined(PSA_WANT_KEY_TYPE_AES) && defined(PSA_WANT_ALG_CBC_NO_PADDING)
    { "AES-128-CBC", PSA_KEY_TYPE_AES, 128, PSA_ALG_CBC_NO_PADDING, 16, 16 },
    { "AES-192-CBC", PSA_KEY_TYPE_AES, 192, PSA_ALG_CBC_NO_PADDING, 16, 16 },
    { "AES-256-CBC", PSA_KEY_TYPE_AES, 256, PSA_ALG_CBC_NO_PADDING, 16, 16 },
#endif
#if defined(PSA_WANT_KEY_TYPE_AES) && defined(PSA_WANT_ALG_ECB_NO_PADDING)
    { "AES-128-ECB", PSA_KEY_TYPE_AES, 128, PSA_ALG_ECB_NO_PADDING, 0, 16 },
    { "AES-192-ECB", PSA_KEY_TYPE_AES, 192, PSA_ALG_ECB_NO_PADDING, 0, 16 },
    { "AES-256-ECB", PSA_KEY_TYPE_AES, 256, PSA_ALG_ECB_NO_PADDING, 0, 16 },
#endif
#if defined(PSA_WANT_KEY_TYPE_AES) && defined(PSA_WANT_ALG_CTR)
    { "AES-128-CTR", PSA_KEY_TYPE_AES, 128, PSA_ALG_CTR, 16, 1 },
    { "AES-192-CTR", PSA_KEY_TYPE_AES, 192, PSA_ALG_CTR, 16, 1 },
    { "AES-256-CTR", PSA_KEY_TYPE_AES, 256, PSA_ALG_CTR, 16, 1 },
#endif
#if defined(PSA_WANT_KEY_TYPE_AES) && defined(PSA_WANT_ALG_OFB)
    { "AES-128-OFB", PSA_KEY_TYPE_AES, 128, PSA_ALG_OFB, 16, 1 },
    { "AES-192-OFB", PSA_KEY_TYPE_AES, 192, PSA_ALG_OFB, 16, 1 },
    { "AES-256-OFB", PSA_KEY_TYPE_AES, 256, PSA_ALG_OFB, 16, 1 },
#endif
#if defined(PSA_WANT_KEY_TYPE_CHACHA20) && defined(PSA_WANT_ALG_STREAM_CIPHER)
    { "CHACHA20", PSA_KEY_TYPE_CHACHA20, 256, PSA_ALG_STREAM_CIPHER, 12, 1 },
#endif
#if defined(PSA_WANT_KEY_TYPE_ARIA) && defined(PSA_WANT_ALG_CBC_NO_PADDING)
    { "ARIA-128-CBC", PSA_KEY_TYPE_ARIA, 128, PSA_ALG_CBC_NO_PADDING, 16, 16 },
    { "ARIA-192-CBC", PSA_KEY_TYPE_ARIA, 192, PSA_ALG_CBC_NO_PADDING, 16, 16 },
    { "ARIA-256-CBC", PSA_KEY_TYPE_ARIA, 256, PSA_ALG_CBC_NO_PADDING, 16, 16 },
#endif
#if defined(PSA_WANT_KEY_TYPE_CAMELLIA) && defined(PSA_WANT_ALG_CBC_NO_PADDING)
    { "CAMELLIA-128-CBC", PSA_KEY_TYPE_CAMELLIA, 128, PSA_ALG_CBC_NO_PADDING, 16, 16 },
    { "CAMELLIA-192-CBC", PSA_KEY_TYPE_CAMELLIA, 192, PSA_ALG_CBC_NO_PADDING, 16, 16 },
    { "CAMELLIA-256-CBC", PSA_KEY_TYPE_CAMELLIA, 256, PSA_ALG_CBC_NO_PADDING, 16, 16 },
#endif
    { NULL, 0, 0, 0, 0, 0 },
};

const cipher_t *get_default_cipher(void)
{
    const struct cipher_def *c;

    for (c = ciphers; c->name; c++)
        if (!strcmp(c->name, "AES-128-CBC"))
            return c;
    return c;
}

const cipher_t *get_cipher_or_print_error(char *name)
{
    const struct cipher_def *c;

    upperstr(name);
    for (c = ciphers; c->name; c++)
        if (!strcmp(name, c->name))
            return c;

    fprintf(stderr, "Error: invalid cipher: %s.\n", name);
    fprintf(stderr, "Supported ciphers: \n");
    for (c = ciphers; c->name; c++)
        fprintf(stderr, "\t%s\n", c->name);
    return NULL;
}

int get_cipher_ivsize(const cipher_t *cipher)
{
    return ((const struct cipher_def *) cipher)->iv_size;
}

int get_cipher_keysize(const cipher_t *cipher)
{
    return ((const struct cipher_def *) cipher)->key_bits >> 3;
}

ctx_t *create_ctx(const cipher_t *cipher, const unsigned char *key,
                  const unsigned char *iv, int enc, int padding)
{
    const struct cipher_def *c = cipher;
    struct cipher_ctx *ctx;
    psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;
    psa_algorithm_t alg = c->alg;
    psa_status_t status;

    if (!c->name) {
        fprintf(stderr, "Error: default cipher AES-128-CBC is not available in this build; use -c\n");
        return NULL;
    }

    if (padding) {
        if (alg != PSA_ALG_CBC_NO_PADDING) {
            fprintf(stderr, "Error: padding is only supported with CBC ciphers.\n");
            return NULL;
        }
#if defined(PSA_WANT_ALG_CBC_PKCS7)
        alg = PSA_ALG_CBC_PKCS7;
#else
        fprintf(stderr, "Error: PKCS7 padding is not available in this build.\n");
        return NULL;
#endif
    }

    status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        fprintf(stderr, "Error: psa_crypto_init: %d\n", (int) status);
        return NULL;
    }

    ctx = calloc(1, sizeof(*ctx));
    if (!ctx) {
        fprintf(stderr, "Error: create_ctx: out of memory.\n");
        return NULL;
    }
    ctx->op = psa_cipher_operation_init();
    ctx->key_id = MBEDTLS_SVC_KEY_ID_INIT;
    ctx->block_size = c->block_size;

    psa_set_key_type(&attr, c->key_type);
    psa_set_key_bits(&attr, c->key_bits);
    psa_set_key_algorithm(&attr, alg);
    psa_set_key_usage_flags(&attr, enc ? PSA_KEY_USAGE_ENCRYPT : PSA_KEY_USAGE_DECRYPT);

    status = psa_import_key(&attr, key, c->key_bits >> 3, &ctx->key_id);
    if (status != PSA_SUCCESS) {
        fprintf(stderr, "Error: psa_import_key: %d\n", (int) status);
        goto abort;
    }

    status = enc ? psa_cipher_encrypt_setup(&ctx->op, ctx->key_id, alg)
                 : psa_cipher_decrypt_setup(&ctx->op, ctx->key_id, alg);
    if (status != PSA_SUCCESS) {
        fprintf(stderr, "Error: psa_cipher_%s_setup: %d\n",
                enc ? "encrypt" : "decrypt", (int) status);
        goto abort;
    }

    if (iv && c->iv_size) {
        status = psa_cipher_set_iv(&ctx->op, iv, c->iv_size);
        if (status != PSA_SUCCESS) {
            fprintf(stderr, "Error: psa_cipher_set_iv: %d\n", (int) status);
            goto abort;
        }
    }

    return ctx;

abort:
    free_ctx(ctx);

    return NULL;
}

int do_crypt(FILE *infile, FILE *outfile, ctx_t *ctx)
{
    struct cipher_ctx *c = ctx;
    unsigned char inbuf[CRYPT_BUF_SIZE];
    unsigned char outbuf[CRYPT_BUF_SIZE + PSA_BLOCK_CIPHER_BLOCK_MAX_SIZE];
    size_t inlen, outlen, step;
    psa_status_t status;
    size_t written;

    if (c->block_size > 1) {
        step = CRYPT_BUF_SIZE - (CRYPT_BUF_SIZE % c->block_size);
    } else {
        step = CRYPT_BUF_SIZE;
    }

    for (;;) {
        inlen = fread(inbuf, 1, step, infile);
        if (inlen <= 0)
            break;
        status = psa_cipher_update(&c->op, inbuf, inlen,
                                   outbuf, sizeof(outbuf), &outlen);
        if (status != PSA_SUCCESS) {
            fprintf(stderr, "Error: psa_cipher_update: %d\n", (int) status);
            return (int) status;
        }
        written = fwrite(outbuf, 1, outlen, outfile);
        if (written != outlen) {
            fprintf(stderr, "Error: cipher_update short write.\n");
            return -1;
        }
    }

    status = psa_cipher_finish(&c->op, outbuf, sizeof(outbuf), &outlen);
    if (status != PSA_SUCCESS) {
        fprintf(stderr, "Error: psa_cipher_finish: %d\n", (int) status);
        return (int) status;
    }
    written = fwrite(outbuf, 1, outlen, outfile);
    if (written != outlen) {
        fprintf(stderr, "Error: cipher_finish short write.\n");
        return -1;
    }

    return 0;
}

void free_ctx(ctx_t *ctx)
{
    struct cipher_ctx *c = ctx;

    if (c) {
        psa_cipher_abort(&c->op);
        psa_destroy_key(c->key_id);
        free(c);
    }
}
