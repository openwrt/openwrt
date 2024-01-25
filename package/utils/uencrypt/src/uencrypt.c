/* SPDX-License-Identifier: GPL-2.0-or-later
 * Copyright (C) 2023 Eneas Ulir de Queiroz
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "uencrypt.h"

static void check_enc_dec(const int enc)
{
    if (enc == -1)
	return;
    fprintf(stderr, "Error: both -d and -e were specified.\n");
    exit(EXIT_FAILURE);
}

static void show_usage(const char* name)
{
    fprintf(stderr, "Usage: %s: [-d | -e] [-n] -k key [-i iv] [-c cipher]\n"
		    "-d = decrypt; -e = encrypt; -n = no padding\n", name);
}

static void uencrypt_clear_free(void *ptr, size_t len)
{
    if (ptr) {
	memset(ptr, 0, len);
	free(ptr);
    }
}

int main(int argc, char *argv[])
{
    int enc = -1;
    unsigned char *iv = NULL;
    unsigned char *key = NULL;
    long keylen = 0, ivlen = 0;
    int opt;
    int padding = 1;
    const cipher_t *cipher = get_default_cipher();
    ctx_t* ctx;
    int ret = EXIT_FAILURE;

    while ((opt = getopt(argc, argv, "c:dei:k:n")) != -1) {
	switch (opt) {
	case 'c':
	    if (!(cipher = get_cipher_or_print_error(optarg)))
		exit(EXIT_FAILURE);
	    break;
	case 'd':
	    check_enc_dec(enc);
	    enc = 0;
	    break;
	case 'e':
	    check_enc_dec(enc);
	    enc = 1;
	    break;
	case 'i':
	    iv = hexstr2buf(optarg, &ivlen);
	    if (iv == NULL) {
		fprintf(stderr, "Error setting IV to %s. The IV should be encoded in hex.\n",
			optarg);
		exit(EINVAL);
	    }
	    memset(optarg, '*', strlen(optarg));
	    break;
	case 'k':
	    key = hexstr2buf(optarg, &keylen);
	    if (key == NULL) {
		fprintf(stderr, "Error setting key to %s. The key should be encoded in hex.\n",
			optarg);
		exit(EINVAL);
	    }
	    memset(optarg, '*', strlen(optarg));
	    break;
	case 'n':
	    padding = 0;
	    break;
	default:
	    show_usage(argv[0]);
	    exit(EINVAL);
	}
    }
    if (ivlen != get_cipher_ivsize(cipher)) {
	fprintf(stderr, "Error: IV must be %d bytes; given IV is %zd bytes.\n",
		get_cipher_ivsize(cipher), ivlen);
	exit(EXIT_FAILURE);
    }
    if (keylen != get_cipher_keysize(cipher)) {
	fprintf(stderr, "Error: key must be %d bytes; given key is %zd bytes.\n",
		get_cipher_keysize(cipher), keylen);
	exit(EXIT_FAILURE);
    }
    ctx = create_ctx(cipher, key, iv, !!enc, padding);
    if (ctx) {
	ret = do_crypt(stdin, stdout, ctx);
	free_ctx(ctx);
    }
    uencrypt_clear_free(iv, ivlen);
    uencrypt_clear_free(key, keylen);
    return ret;
}
