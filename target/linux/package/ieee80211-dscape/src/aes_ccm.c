/*
 * Copyright 2003-2004, Instant802 Networks, Inc.
 * Copyright 2005, Devicescape Software, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/types.h>
#include <linux/netdevice.h>

#include <net/ieee80211.h>
#include "ieee80211_key.h"
#include "aes_ccm.h"

#include "aes.c"

static inline void aes_ccm_prepare(u32 *rk, u8 *b_0, u8 *aad, u8 *b,
				   u8 *s_0, u8 *a)
{
	int i;

	ieee80211_aes_encrypt(rk, b_0, b);

	/* Extra Authenticate-only data (always two AES blocks) */
	for (i = 0; i < AES_BLOCK_LEN; i++)
		aad[i] ^= b[i];
	ieee80211_aes_encrypt(rk, aad, b);

	aad += AES_BLOCK_LEN;

	for (i = 0; i < AES_BLOCK_LEN; i++)
		aad[i] ^= b[i];
	ieee80211_aes_encrypt(rk, aad, a);

	/* Mask out bits from auth-only-b_0 */
	b_0[0] &= 0x07;

	/* S_0 is used to encrypt T (= MIC) */
	b_0[14] = 0;
	b_0[15] = 0;
	ieee80211_aes_encrypt(rk, b_0, s_0);
}


void ieee80211_aes_ccm_encrypt(u32 *rk, u8 *b_0, u8 *aad, u8 *data,
			       size_t data_len, u8 *cdata, u8 *mic)
{
	int i, j, last_len, num_blocks;
	u8 *pos, *cpos;
	u8 b[AES_BLOCK_LEN], s_0[AES_BLOCK_LEN], e[AES_BLOCK_LEN];

	num_blocks = (data_len + AES_BLOCK_LEN - 1) / AES_BLOCK_LEN;
	last_len = data_len % AES_BLOCK_LEN;
	aes_ccm_prepare(rk, b_0, aad, b, s_0, b);

	/* Process payload blocks */
	pos = data;
	cpos = cdata;
	for (j = 1; j <= num_blocks; j++) {
		int blen = (j == num_blocks && last_len) ?
			last_len : AES_BLOCK_LEN;

		/* Authentication followed by encryption */
		for (i = 0; i < blen; i++)
			b[i] ^= pos[i];
		ieee80211_aes_encrypt(rk, b, b);

		b_0[14] = (j >> 8) & 0xff;
		b_0[15] = j & 0xff;
		ieee80211_aes_encrypt(rk, b_0, e);
		for (i = 0; i < blen; i++)
			*cpos++ = *pos++ ^ e[i];
	}

	for (i = 0; i < CCMP_MIC_LEN; i++)
		mic[i] = b[i] ^ s_0[i];
}


int ieee80211_aes_ccm_decrypt(u32 *rk, u8 *b_0, u8 *aad, u8 *cdata,
			      size_t data_len, u8 *mic, u8 *data)
{
	int i, j, last_len, num_blocks;
	u8 *pos, *cpos;
	u8 b[AES_BLOCK_LEN], s_0[AES_BLOCK_LEN], a[AES_BLOCK_LEN];

	num_blocks = (data_len + AES_BLOCK_LEN - 1) / AES_BLOCK_LEN;
	last_len = data_len % AES_BLOCK_LEN;
	aes_ccm_prepare(rk, b_0, aad, b, s_0, a);

	/* Process payload blocks */
	cpos = cdata;
	pos = data;
	for (j = 1; j <= num_blocks; j++) {
		int blen = (j == num_blocks && last_len) ?
			last_len : AES_BLOCK_LEN;

		/* Decryption followed by authentication */
		b_0[14] = (j >> 8) & 0xff;
		b_0[15] = j & 0xff;
		ieee80211_aes_encrypt(rk, b_0, b);
		for (i = 0; i < blen; i++) {
			*pos = *cpos++ ^ b[i];
			a[i] ^= *pos++;
		}

		ieee80211_aes_encrypt(rk, a, a);
	}

	for (i = 0; i < CCMP_MIC_LEN; i++) {
		if ((mic[i] ^ s_0[i]) != a[i])
			return -1;
	}

	return 0;
}

