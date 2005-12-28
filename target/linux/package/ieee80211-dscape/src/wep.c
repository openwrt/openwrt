/*
 * Software WEP encryption implementation
 * Copyright 2002, Jouni Malinen <jkmaline@cc.hut.fi>
 * Copyright 2003, Instant802 Networks, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/config.h>
#include <linux/version.h>
#include <linux/netdevice.h>
#include <linux/types.h>
#include <linux/random.h>
#include <linux/compiler.h>

#include <net/ieee80211.h>
#include "ieee80211_i.h"
#include "wep.h"


static const __u32 crc32_table[256] = {
	0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
	0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
	0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
	0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
	0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
	0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
	0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
	0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
	0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
	0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
	0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
	0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
	0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
	0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
	0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
	0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
	0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
	0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
	0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
	0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
	0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
	0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
	0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
	0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
	0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
	0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
	0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
	0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
	0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
	0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
	0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
	0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
	0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
	0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
	0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
	0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
	0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
	0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
	0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
	0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
	0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
	0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
	0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
	0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
	0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
	0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
	0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
	0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
	0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
	0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
	0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
	0x2d02ef8dL
};


void ieee80211_wep_init(struct ieee80211_local *local)
{
	/* start WEP IV from a random value */
	get_random_bytes(&local->wep_iv, WEP_IV_LEN);
}


static inline int ieee80211_wep_weak_iv(u32 iv, int keylen)
{
	/* Fluhrer, Mantin, and Shamir have reported weaknesses in the
	 * key scheduling algorithm of RC4. At least IVs (KeyByte + 3,
	 * 0xff, N) can be used to speedup attacks, so avoid using them. */
	if ((iv & 0xff00) == 0xff00) {
		u8 B = (iv >> 16) & 0xff;
		if (B >= 3 && B < 3 + keylen)
			return 1;
	}
	return 0;
}


void ieee80211_wep_get_iv(struct ieee80211_local *local,
			  struct ieee80211_key *key, u8 *iv)
{
	local->wep_iv++;
	if (ieee80211_wep_weak_iv(local->wep_iv, key->keylen))
		local->wep_iv += 0x0100;

	if (iv == NULL)
		return;

	*iv++ = (local->wep_iv >> 16) & 0xff;
	*iv++ = (local->wep_iv >> 8) & 0xff;
	*iv++ = local->wep_iv & 0xff;
	*iv++ = key->keyidx << 6;
}


u8 * ieee80211_wep_add_iv(struct ieee80211_local *local,
			  struct sk_buff *skb,
			  struct ieee80211_key *key)
{
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *) skb->data;
	u16 fc;
	int hdrlen;
	u8 *newhdr;

	fc = le16_to_cpu(hdr->frame_control);
	fc |= WLAN_FC_ISWEP;
	hdr->frame_control = cpu_to_le16(fc);

	if ((skb_headroom(skb) < WEP_IV_LEN ||
	     skb_tailroom(skb) < WEP_ICV_LEN)) {
		I802_DEBUG_INC(local->tx_expand_skb_head);
		if (unlikely(pskb_expand_head(skb, WEP_IV_LEN, WEP_ICV_LEN,
					      GFP_ATOMIC)))
			return NULL;
	}

	hdrlen = ieee80211_get_hdrlen(fc);
	newhdr = skb_push(skb, WEP_IV_LEN); 
	memmove(newhdr, newhdr + WEP_IV_LEN, hdrlen);
	ieee80211_wep_get_iv(local, key, newhdr + hdrlen);
	return newhdr + hdrlen;
}


void ieee80211_wep_remove_iv(struct ieee80211_local *local,
			     struct sk_buff *skb,
			     struct ieee80211_key *key)
{
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *) skb->data;
	u16 fc;
	int hdrlen;

	fc = le16_to_cpu(hdr->frame_control);
	hdrlen = ieee80211_get_hdrlen(fc);
	memmove(skb->data + WEP_IV_LEN, skb->data, hdrlen);
	skb_pull(skb, WEP_IV_LEN);
}


/* Perform WEP encryption using given key. data buffer must have tailroom
 * for 4-byte ICV. data_len must not include this ICV. Note: this function
 * does _not_ add IV. data = RC4(data | CRC32(data)) */
void ieee80211_wep_encrypt_data(u8 *rc4key, size_t klen, u8 *data,
				size_t data_len)
{
	u32 i, j, k, crc;
	u8 S[256];
	u8 kpos, *pos;
#define S_SWAP(a,b) do { u8 t = S[a]; S[a] = S[b]; S[b] = t; } while(0)

	/* Setup RC4 state */
	for (i = 0; i < 256; i++)
		S[i] = i;
	j = 0;
	kpos = 0;
	for (i = 0; i < 256; i++) {
		j = (j + S[i] + rc4key[kpos]) & 0xff;
		kpos++;
		if (kpos >= klen)
			kpos = 0;
		S_SWAP(i, j);
	}

	/* Compute CRC32 over unencrypted data and apply RC4 to data */
	pos = data;
	crc = ~0;
	i = j = 0;
	for (k = 0; k < data_len; k++) {
		crc = crc32_table[(crc ^ *pos) & 0xff] ^ (crc >> 8);
		i = (i + 1) & 0xff;
		j = (j + S[i]) & 0xff;
		S_SWAP(i, j);
		*pos++ ^= S[(S[i] + S[j]) & 0xff];
	}
	crc = ~crc;

	/* Append little-endian CRC32 and encrypt it to produce ICV */
	pos[0] = crc;
	pos[1] = crc >> 8;
	pos[2] = crc >> 16;
	pos[3] = crc >> 24;
	for (k = 0; k < 4; k++) {
		i = (i + 1) & 0xff;
		j = (j + S[i]) & 0xff;
		S_SWAP(i, j);
		*pos++ ^= S[(S[i] + S[j]) & 0xff];
	}
}


/* Perform WEP encryption on given skb. 4 bytes of extra space (IV) in the
 * beginning of the buffer 4 bytes of extra space (ICV) in the end of the
 * buffer will be added. Both IV and ICV will be transmitted, so the
 * payload length increases with 8 bytes.
 *
 * WEP frame payload: IV + TX key idx, RC4(data), ICV = RC4(CRC32(data))
 */
int ieee80211_wep_encrypt(struct ieee80211_local *local, struct sk_buff *skb,
			  struct ieee80211_key *key)
{
	u32 klen;
	u8 *rc4key, *iv;
	size_t len;

	if (key == NULL || key->alg != ALG_WEP)
		return -1;

	klen = 3 + key->keylen;
	rc4key = kmalloc(klen, GFP_ATOMIC);
	if (rc4key == NULL)
		return -1;

	iv = ieee80211_wep_add_iv(local, skb, key);
	if (iv == NULL) {
		kfree(rc4key);
		return -1;
	}

	len = skb->len - (iv + WEP_IV_LEN - skb->data);

	/* Prepend 24-bit IV to RC4 key */
	memcpy(rc4key, iv, 3);

	/* Copy rest of the WEP key (the secret part) */
	memcpy(rc4key + 3, key->key, key->keylen);

	/* Add room for ICV */
	skb_put(skb, WEP_ICV_LEN);

	ieee80211_wep_encrypt_data(rc4key, klen, iv + WEP_IV_LEN, len);

	kfree(rc4key);

	return 0;
}


/* Perform WEP decryption using given key. data buffer includes encrypted
 * payload, including 4-byte ICV, but _not_ IV. data_len must not include ICV.
 * Return 0 on success and -1 on ICV mismatch. */
int ieee80211_wep_decrypt_data(u8 *rc4key, size_t klen, u8 *data,
			       size_t data_len)
{
	u32 i, j, k, crc;
	u8 S[256];
	u8 kpos, *pos, crcbuf[WEP_ICV_LEN], *cpos;

	/* Setup RC4 state */
	for (i = 0; i < 256; i++)
		S[i] = i;
	j = 0;
	kpos = 0;
	for (i = 0; i < 256; i++) {
		j = (j + S[i] + rc4key[kpos]) & 0xff;
		kpos++;
		if (kpos >= klen)
			kpos = 0;
		S_SWAP(i, j);
	}

	/* Apply RC4 to data and compute CRC32 over decrypted data */
	pos = data;
	crc = ~0;
	i = j = 0;
	for (k = 0; k < data_len; k++) {
		i = (i + 1) & 0xff;
		j = (j + S[i]) & 0xff;
		S_SWAP(i, j);
		*pos ^= S[(S[i] + S[j]) & 0xff];
		crc = crc32_table[(crc ^ *pos++) & 0xff] ^ (crc >> 8);
	}
	crc = ~crc;

	/* Decrypt little-endian CRC32 and verify that it matches with the
	 * received ICV */
	cpos = crcbuf;
	crcbuf[0] = crc;
	crcbuf[1] = crc >> 8;
	crcbuf[2] = crc >> 16;
	crcbuf[3] = crc >> 24;
	for (k = 0; k < WEP_ICV_LEN; k++) {
		i = (i + 1) & 0xff;
		j = (j + S[i]) & 0xff;
		S_SWAP(i, j);
		if (*cpos++ != (*pos++ ^ S[(S[i] + S[j]) & 0xff])) {
			/* ICV mismatch */
			return -1;
		}
	}

	return 0;
}


/* Perform WEP decryption on given skb. Buffer includes whole WEP part of
 * the frame: IV (4 bytes), encrypted payload (including SNAP header),
 * ICV (4 bytes). skb->len includes both IV and ICV.
 *
 * Returns 0 if frame was decrypted successfully and ICV was correct and -1 on
 * failure. If frame is OK, IV and ICV will be removed, i.e., decrypted payload
 * is moved to the beginning of the skb and skb length will be reduced.
 */
int ieee80211_wep_decrypt(struct ieee80211_local *local, struct sk_buff *skb,
			  struct ieee80211_key *key)
{
	u32 klen;
	u8 *rc4key;
	u8 keyidx;
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *) skb->data;
	u16 fc;
	int hdrlen;
	size_t len;
	int ret = 0;

	fc = le16_to_cpu(hdr->frame_control);
	if (!(fc & WLAN_FC_ISWEP))
		return -1;

	hdrlen = ieee80211_get_hdrlen(fc);

	if (skb->len < 8 + hdrlen)
		return -1;

	len = skb->len - hdrlen - 8;

	keyidx = skb->data[hdrlen + 3] >> 6;

	if (key == NULL || keyidx != key->keyidx || key->alg != ALG_WEP)
		return -1;

	klen = 3 + key->keylen;

	rc4key = kmalloc(klen, GFP_ATOMIC);
	if (rc4key == NULL)
		return -1;

	/* Prepend 24-bit IV to RC4 key */
	memcpy(rc4key, skb->data + hdrlen, 3);

	/* Copy rest of the WEP key (the secret part) */
	memcpy(rc4key + 3, key->key, key->keylen);

	if (ieee80211_wep_decrypt_data(rc4key, klen,
				       skb->data + hdrlen + WEP_IV_LEN,
				       len)) {
		printk(KERN_DEBUG "WEP decrypt failed (ICV)\n");
		ret = -1;
	}

	kfree(rc4key);

	/* Trim ICV */
	skb_trim(skb, skb->len - WEP_ICV_LEN);

	/* Remove IV */
	memmove(skb->data + WEP_IV_LEN, skb->data, hdrlen);
	skb_pull(skb, WEP_IV_LEN);

	return ret;
}


int ieee80211_wep_get_keyidx(struct sk_buff *skb)
{
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *) skb->data;
	u16 fc;
	int hdrlen;

	fc = le16_to_cpu(hdr->frame_control);
	if (!(fc & WLAN_FC_ISWEP))
		return -1;

	hdrlen = ieee80211_get_hdrlen(fc);

	if (skb->len < 8 + hdrlen)
		return -1;

	return skb->data[hdrlen + 3] >> 6;
}


u8 * ieee80211_wep_is_weak_iv(struct sk_buff *skb, struct ieee80211_key *key)
{
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *) skb->data;
	u16 fc;
	int hdrlen;
	u8 *ivpos;
	u32 iv;

	fc = le16_to_cpu(hdr->frame_control);
	if (!(fc & WLAN_FC_ISWEP))
		return NULL;

	hdrlen = ieee80211_get_hdrlen(fc);
	ivpos = skb->data + hdrlen;
	iv = (ivpos[0] << 16) | (ivpos[1] << 8) | ivpos[2];

	if (ieee80211_wep_weak_iv(iv, key->keylen))
		return ivpos;

	return NULL;
}
