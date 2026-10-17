/* SPDX-License-Identifier: GPL-2.0-or-later
 * Copyright (C) 2026 Nick French
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "des.h"

typedef struct cipher_s {
    const char *name;
    int keysize;
    int ivsize;
} cipher_t;

typedef struct ctx_s {
    des_ctx gcrypt_ctx;
    int enc;
} ctx_t;

static const cipher_t des_ecb_cipher = {
    .name = "DES-ECB",
    .keysize = 8,
    .ivsize = 0
};

const cipher_t *get_default_cipher(void) {
    return &des_ecb_cipher;
}

const cipher_t *get_cipher_or_print_error(char *name) {
    if (strcasecmp(name, "des-ecb") == 0) {
        return &des_ecb_cipher;
    }
    fprintf(stderr, "Error: invalid cipher: %s.\n", name);
    fprintf(stderr, "Supported ciphers: des-ecb\n");
    return NULL;
}

int get_cipher_ivsize(const cipher_t *cipher) {
    return cipher ? cipher->ivsize : 0;
}

int get_cipher_keysize(const cipher_t *cipher) {
    return cipher ? cipher->keysize : 0;
}

ctx_t *create_ctx(const cipher_t *cipher, const unsigned char *key,
                  const unsigned char *iv, int enc) {
    (void)iv; //iv unused for des-ecb

    if (cipher != &des_ecb_cipher) {
        fprintf(stderr, "desonly engine only supports 'des-ecb' cipher.\n");
        return NULL;
    }
    
    ctx_t *ctx = malloc(sizeof(ctx_t));
    if (!ctx) return NULL;
    
    ctx->enc = enc;

    if (des_setkey(ctx->gcrypt_ctx, key) != 0) {
        fprintf(stderr, "DES key schedule initiation failed.\n");
        free(ctx);
        return NULL;
    }
    return ctx;
}

void free_ctx(ctx_t *ctx) {
    if (ctx) free(ctx);
}

int do_crypt(FILE *infile, FILE *outfile, ctx_t *ctx) {
    uint8_t input_block[8];
    uint8_t output_block[8];
    size_t bytes_read;

    if (!infile || !outfile || !ctx) return -1;

    while ((bytes_read = fread(input_block, 1, 8, infile)) == 8) {
        if (ctx->enc) {
            des_ecb_crypt(ctx->gcrypt_ctx, input_block, output_block, 0);
        } else {
            des_ecb_crypt(ctx->gcrypt_ctx, input_block, output_block, 1);
        }

        if (fwrite(output_block, 1, 8, outfile) != 8) {
            return -1; 
        }
    }

    if (bytes_read > 0) {
        fprintf(stderr, "Input data size is not an exact multiple of 8 bytes.\n");
        return -1;
    }

    return 0;
}

