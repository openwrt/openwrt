/* SPDX-License-Identifier: GPL-2.0-or-later
 * Copyright (C) 2023 Eneas Ulir de Queiroz
 */

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <mbedtls/cipher.h>

int do_crypt(FILE *infile, FILE *outfile, const mbedtls_cipher_info_t *cipher_info,
	     const unsigned char *key, const unsigned char *iv, int enc, int padding)
{
    mbedtls_cipher_context_t ctx;
    unsigned char inbuf[1024], outbuf[1024 + MBEDTLS_MAX_BLOCK_LENGTH];
    size_t inlen, outlen;
    int ret = 0;
    int step;

    mbedtls_cipher_init(&ctx);
    if ((ret = mbedtls_cipher_setup(&ctx, cipher_info))) {
	fprintf(stderr, "Error: mbedtls_cipher_setup: %d\n", ret);
	goto out;
    }
    step = iv ? 1024 : mbedtls_cipher_get_block_size(&ctx);
    if ((ret = mbedtls_cipher_setkey(&ctx, key, (int) mbedtls_cipher_get_key_bitlen(&ctx),
			      enc ? MBEDTLS_ENCRYPT : MBEDTLS_DECRYPT))) {
	fprintf(stderr, "Error: mbedtls_cipher_setkey: %d\n", ret);
	goto out;
    }
    if (iv && (ret = mbedtls_cipher_set_iv(&ctx, iv, cipher_info->iv_size))) {
	fprintf(stderr, "Error: mbedtls_cipher_set_iv: %d\n", ret);
	goto out;
    }

    if (cipher_info->block_size > 1) {
	if (cipher_info->mode == MBEDTLS_MODE_CBC) {
	    if ((ret = mbedtls_cipher_set_padding_mode(&ctx,
						       padding ? MBEDTLS_PADDING_PKCS7
							       : MBEDTLS_PADDING_NONE))) {
		fprintf(stderr, "Error: mbedtls_cipher_set_padding_mode: %d\n", ret);
		goto out;
	    }
	} else {
	    if (cipher_info->mode != MBEDTLS_MODE_CBC && padding) {
		fprintf(stderr, "Error: mbedTLS only supports padding with CBC ciphers.\n");
		goto out;
	    }
	}
    }

    if ((ret = mbedtls_cipher_reset(&ctx))) {
	fprintf(stderr, "Error: mbedtls_cipher_reset: %d\n", ret);
	goto out;
    }

    for (;;) {
	inlen = fread(inbuf, 1, step, infile);
	if (inlen <= 0)
	    break;
	if ((ret = mbedtls_cipher_update(&ctx, inbuf, inlen, outbuf, &outlen))) {
	    fprintf(stderr, "Error: mbedtls_cipher_update: %d\n", ret);
	    goto out;
	}
	fwrite(outbuf, 1, outlen, outfile);
    }
    if ((ret = mbedtls_cipher_finish(&ctx, outbuf, &outlen))) {
	fprintf(stderr, "Error: mbedtls_cipher_finish: %d\n", ret);
	goto out;
    }
    fwrite(outbuf, 1, outlen, outfile);

out:
    mbedtls_cipher_free(&ctx);
    return ret;
}

static void check_enc_dec(const int enc)
{
    if (enc == -1)
	return;
    fprintf(stderr, "Error: both -d and -e were specified.\n");
    exit(EXIT_FAILURE);
}

static void check_cipher(const mbedtls_cipher_info_t *cipher_info)
{
    const int *list;

    if (cipher_info == NULL) {
	fprintf(stderr, "Error: invalid cipher: %s.\n", optarg);
	fprintf(stderr, "Supported ciphers: \n");
	for (list = mbedtls_cipher_list(); *list; list++) {
	    cipher_info = mbedtls_cipher_info_from_type(*list);
	    if (!cipher_info)
		continue;
	    fprintf(stderr, "\t%s\n", cipher_info->name);
	}
	exit(EXIT_FAILURE);
    }
}

static void show_usage(const char* name)
{
    fprintf(stderr, "Usage: %s: [-d | -e] [-n] -k key [-i iv] [-c cipher]\n"
		    "-d = decrypt; -e = encrypt; -n = no padding\n", name);
}

char *hexstr2buf(const char *str, size_t *len)
{
    char *buf;
    size_t inlen = strlen(str);

    *len = 0;
    if (inlen % 2)
	return NULL;

    *len = inlen >> 1;
    buf = malloc(*len);
    for  (size_t x = 0; x < *len; x++)
	sscanf(str + x * 2, "%2hhx", buf + x);
    return buf;
}

static char* upperstr(char *str) {
    for (char *s = str; *s; s++)
	*s = toupper((unsigned char) *s);
    return str;
}

int main(int argc, char *argv[])
{
    int enc = -1;
    unsigned char *iv = NULL;
    unsigned char *key = NULL;
    size_t keylen = 0, ivlen = 0;
    int opt;
    int padding = 1;
    const mbedtls_cipher_info_t *cipher_info =
			mbedtls_cipher_info_from_type (MBEDTLS_CIPHER_AES_128_CBC);
    int ret;

    while ((opt = getopt(argc, argv, "c:dei:k:n")) != -1) {
	switch (opt) {
	case 'c':
	    cipher_info = mbedtls_cipher_info_from_string(upperstr(optarg));
	    check_cipher(cipher_info);
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
	    iv = (unsigned char *) hexstr2buf((const char *)optarg, &ivlen);
	    if (iv == NULL) {
		fprintf(stderr, "Error setting IV to %s. The IV should be encoded in hex.\n",
			optarg);
		exit(EINVAL);
	    }
	    break;
	case 'k':
	    key = (unsigned char *) hexstr2buf((const char *)optarg, &keylen);
	    if (key == NULL) {
		fprintf(stderr, "Error setting key to %s. The key should be encoded in hex.\n",
			optarg);
		exit(EINVAL);
	    }
	    break;
	case 'n':
	    padding = 0;
	    break;
	default:
	    show_usage(argv[0]);
	    exit(EINVAL);
	}
    }
    if (cipher_info->iv_size) {
	if (iv == NULL) {
	    fprintf(stderr, "Error: iv not set.\n");
	    show_usage(argv[0]);
	    exit(EXIT_FAILURE);
	}
	if (ivlen != cipher_info->iv_size) {
	    fprintf(stderr, "Error: IV must be %d bytes; given IV is %zd bytes.\n",
		    cipher_info->iv_size, ivlen);
	    exit(EXIT_FAILURE);
	}
    }
    if (key == NULL) {
	fprintf(stderr, "Error: key not set.\n");
	show_usage(argv[0]);
	exit(EXIT_FAILURE);
    }
    if (keylen != cipher_info->key_bitlen >> 3) {
	fprintf(stderr, "Error: key must be %d bytes; given key is %zd bytes.\n",
		cipher_info->key_bitlen >> 3, keylen);
	exit(EXIT_FAILURE);
    }
    ret = do_crypt(stdin, stdout, cipher_info, key, iv, !!enc, padding);
    if (iv)
	memset(iv, 0, ivlen);
    memset(key, 0, keylen);
    free(iv);
    free(key);
    return ret;
}
