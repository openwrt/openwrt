/*
 * Copyright 2003-2004, Instant802 Networks, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef AES_CCM_H
#define AES_CCM_H

#define AES_BLOCK_LEN 16
#define AES_STATE_LEN 44

void ieee80211_aes_key_setup_encrypt(u32 rk[/*44*/], const u8 key[]);
void ieee80211_aes_encrypt(const u32 rk[/*44*/], const u8 pt[16], u8 ct[16]);
void ieee80211_aes_ccm_encrypt(u32 rk[/*44*/], u8 *b_0, u8 *aad, u8 *data,
			       size_t data_len, u8 *cdata, u8 *mic);
int ieee80211_aes_ccm_decrypt(u32 rk[/*44*/], u8 *b_0, u8 *aad, u8 *cdata,
			      size_t data_len, u8 *mic, u8 *data);

#endif /* AES_CCM_H */
