/*
 * Quick & dirty crypto testing module.
 *
 * This will only exist until we have a better testing mechanism
 * (e.g. a char device).
 *
 * Copyright (c) 2002 James Morris <jmorris@intercode.com.au>
 * Copyright (c) 2002 Jean-Francois Dive <jef@linuxbe.org>
 * Copyright (c) 2007 Nokia Siemens Networks
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 */
#ifndef _IFXMIPS_CRYPTO_TCRYPT_H
#define _IFXMIPS_CRYPTO_TCRYPT_H

struct cipher_speed_template {
	const char *key;
	unsigned int klen;
};

struct hash_speed {
	unsigned int blen;	/* buffer length */
	unsigned int plen;	/* per-update length */
};

/*
 * DES test vectors.
 */
#define DES3_SPEED_VECTORS	1
#define CRYPTO_ALG_TYPE_SPEED_TEST 0xB

static int alg_speed_test(const char *alg, const char *driver,
			  unsigned int sec,
			  struct cipher_speed_template *t,
			  unsigned int tcount, u8 *keysize);

static const struct cipher_speed_template des3_speed_template[] = {
	{
		.key	= "\x01\x23\x45\x67\x89\xab\xcd\xef"
			  "\x55\x55\x55\x55\x55\x55\x55\x55"
			  "\xfe\xdc\xba\x98\x76\x54\x32\x10",
		.klen	= 24,
	}
};

/*
 * Cipher speed tests
 */
static const u8 speed_template_8[] = {8, 0};
static const u8 speed_template_24[] = {24, 0};
static const u8 speed_template_8_32[] = {8, 32, 0};
static const u8 speed_template_16_32[] = {16, 32, 0};
static const u8 speed_template_16_24_32[] = {16, 24, 32, 0};
static const u8 speed_template_32_40_48[] = {32, 40, 48, 0};
static const u8 speed_template_32_48_64[] = {32, 48, 64, 0};

/*
 * Digest speed tests
 */
static const struct hash_speed generic_hash_speed_template[] = {
	{ .blen = 16,	.plen = 16, },
	{ .blen = 64,	.plen = 16, },
	{ .blen = 64,	.plen = 64, },
	{ .blen = 256,	.plen = 16, },
	{ .blen = 256,	.plen = 64, },
	{ .blen = 256,	.plen = 256, },
	{ .blen = 1024,	.plen = 16, },
	{ .blen = 1024,	.plen = 256, },
	{ .blen = 1024,	.plen = 1024, },
	{ .blen = 2048,	.plen = 16, },
	{ .blen = 2048,	.plen = 256, },
	{ .blen = 2048,	.plen = 1024, },
	{ .blen = 2048,	.plen = 2048, },
	{ .blen = 4096,	.plen = 16, },
	{ .blen = 4096,	.plen = 256, },
	{ .blen = 4096,	.plen = 1024, },
	{ .blen = 4096,	.plen = 4096, },
	{ .blen = 8192,	.plen = 16, },
	{ .blen = 8192,	.plen = 256, },
	{ .blen = 8192,	.plen = 1024, },
	{ .blen = 8192,	.plen = 4096, },
	{ .blen = 8192,	.plen = 8192, },

	/* End marker */
	{  .blen = 0,	.plen = 0, }
};

#endif	/* _CRYPTO_TCRYPT_H */
