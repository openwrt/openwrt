/*
 * Copyright (c) 2009 David McCullough <david.mccullough@securecomputing.com>
 *
 * Copyright (c) 2003-2007 Cavium Networks (support@cavium.com). All rights
 * reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 * must display the following acknowledgement:
 * This product includes software developed by Cavium Networks
 * 4. Cavium Networks' name may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 
 * This Software, including technical data, may be subject to U.S. export
 * control laws, including the U.S. Export Administration Act and its
 * associated regulations, and may be subject to export or import regulations
 * in other countries. You warrant that You will comply strictly in all
 * respects with all such regulations and acknowledge that you have the
 * responsibility to obtain licenses to export, re-export or import the
 * Software.
 * 
 * TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" AND
 * WITH ALL FAULTS AND CAVIUM MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
 * EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO THE
 * SOFTWARE, INCLUDING ITS CONDITION, ITS CONFORMITY TO ANY REPRESENTATION OR
 * DESCRIPTION, OR THE EXISTENCE OF ANY LATENT OR PATENT DEFECTS, AND CAVIUM
 * SPECIFICALLY DISCLAIMS ALL IMPLIED (IF ANY) WARRANTIES OF TITLE,
 * MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF
 * VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. THE ENTIRE RISK ARISING OUT OF USE OR
 * PERFORMANCE OF THE SOFTWARE LIES WITH YOU.
*/
/****************************************************************************/

#include <linux/scatterlist.h>
#include <asm/octeon/octeon.h>
#include "octeon-asm.h"

/****************************************************************************/

extern unsigned long octeon_crypto_enable(struct octeon_cop2_state *);
extern void octeon_crypto_disable(struct octeon_cop2_state *, unsigned long);

#define SG_INIT(s, p, i, l) \
	{ \
	    (i) = 0; \
	    (l) = (s)[0].length; \
	    (p) = (typeof(p)) sg_virt((s)); \
		CVMX_PREFETCH0((p)); \
	}

#define SG_CONSUME(s, p, i, l) \
	{ \
		(p)++; \
		(l) -= sizeof(*(p)); \
		if ((l) < 0) { \
			dprintk("%s, %d: l = %d\n", __FILE__, __LINE__, l); \
		} else if ((l) == 0) { \
		    (i)++; \
		    (l) = (s)[0].length; \
		    (p) = (typeof(p)) sg_virt(s); \
			CVMX_PREFETCH0((p)); \
		} \
	}

#define ESP_HEADER_LENGTH     8
#define DES_CBC_IV_LENGTH     8
#define AES_CBC_IV_LENGTH     16
#define ESP_HMAC_LEN          12

#define ESP_HEADER_LENGTH 8
#define DES_CBC_IV_LENGTH 8

/****************************************************************************/

#define CVM_LOAD_SHA_UNIT(dat, next)  { \
   if (next == 0) {                     \
      next = 1;                         \
      CVMX_MT_HSH_DAT (dat, 0);         \
   } else if (next == 1) {              \
      next = 2;                         \
      CVMX_MT_HSH_DAT (dat, 1);         \
   } else if (next == 2) {              \
      next = 3;                    \
      CVMX_MT_HSH_DAT (dat, 2);         \
   } else if (next == 3) {              \
      next = 4;                         \
      CVMX_MT_HSH_DAT (dat, 3);         \
   } else if (next == 4) {              \
      next = 5;                           \
      CVMX_MT_HSH_DAT (dat, 4);         \
   } else if (next == 5) {              \
      next = 6;                         \
      CVMX_MT_HSH_DAT (dat, 5);         \
   } else if (next == 6) {              \
      next = 7;                         \
      CVMX_MT_HSH_DAT (dat, 6);         \
   } else {                             \
     CVMX_MT_HSH_STARTSHA (dat);        \
     next = 0;                          \
   }                                    \
}

#define CVM_LOAD2_SHA_UNIT(dat1, dat2, next)  { \
   if (next == 0) {                      \
      CVMX_MT_HSH_DAT (dat1, 0);         \
      CVMX_MT_HSH_DAT (dat2, 1);         \
      next = 2;                          \
   } else if (next == 1) {               \
      CVMX_MT_HSH_DAT (dat1, 1);         \
      CVMX_MT_HSH_DAT (dat2, 2);         \
      next = 3;                          \
   } else if (next == 2) {               \
      CVMX_MT_HSH_DAT (dat1, 2);         \
      CVMX_MT_HSH_DAT (dat2, 3);         \
      next = 4;                          \
   } else if (next == 3) {               \
      CVMX_MT_HSH_DAT (dat1, 3);         \
      CVMX_MT_HSH_DAT (dat2, 4);         \
      next = 5;                          \
   } else if (next == 4) {               \
      CVMX_MT_HSH_DAT (dat1, 4);         \
      CVMX_MT_HSH_DAT (dat2, 5);         \
      next = 6;                          \
   } else if (next == 5) {               \
      CVMX_MT_HSH_DAT (dat1, 5);         \
      CVMX_MT_HSH_DAT (dat2, 6);         \
      next = 7;                          \
   } else if (next == 6) {               \
      CVMX_MT_HSH_DAT (dat1, 6);         \
      CVMX_MT_HSH_STARTSHA (dat2);       \
      next = 0;                          \
   } else {                              \
     CVMX_MT_HSH_STARTSHA (dat1);        \
     CVMX_MT_HSH_DAT (dat2, 0);          \
     next = 1;                           \
   }                                     \
}

/****************************************************************************/

#define CVM_LOAD_MD5_UNIT(dat, next)  { \
   if (next == 0) {                     \
      next = 1;                         \
      CVMX_MT_HSH_DAT (dat, 0);         \
   } else if (next == 1) {              \
      next = 2;                         \
      CVMX_MT_HSH_DAT (dat, 1);         \
   } else if (next == 2) {              \
      next = 3;                    \
      CVMX_MT_HSH_DAT (dat, 2);         \
   } else if (next == 3) {              \
      next = 4;                         \
      CVMX_MT_HSH_DAT (dat, 3);         \
   } else if (next == 4) {              \
      next = 5;                           \
      CVMX_MT_HSH_DAT (dat, 4);         \
   } else if (next == 5) {              \
      next = 6;                         \
      CVMX_MT_HSH_DAT (dat, 5);         \
   } else if (next == 6) {              \
      next = 7;                         \
      CVMX_MT_HSH_DAT (dat, 6);         \
   } else {                             \
     CVMX_MT_HSH_STARTMD5 (dat);        \
     next = 0;                          \
   }                                    \
}

#define CVM_LOAD2_MD5_UNIT(dat1, dat2, next)  { \
   if (next == 0) {                      \
      CVMX_MT_HSH_DAT (dat1, 0);         \
      CVMX_MT_HSH_DAT (dat2, 1);         \
      next = 2;                          \
   } else if (next == 1) {               \
      CVMX_MT_HSH_DAT (dat1, 1);         \
      CVMX_MT_HSH_DAT (dat2, 2);         \
      next = 3;                          \
   } else if (next == 2) {               \
      CVMX_MT_HSH_DAT (dat1, 2);         \
      CVMX_MT_HSH_DAT (dat2, 3);         \
      next = 4;                          \
   } else if (next == 3) {               \
      CVMX_MT_HSH_DAT (dat1, 3);         \
      CVMX_MT_HSH_DAT (dat2, 4);         \
      next = 5;                          \
   } else if (next == 4) {               \
      CVMX_MT_HSH_DAT (dat1, 4);         \
      CVMX_MT_HSH_DAT (dat2, 5);         \
      next = 6;                          \
   } else if (next == 5) {               \
      CVMX_MT_HSH_DAT (dat1, 5);         \
      CVMX_MT_HSH_DAT (dat2, 6);         \
      next = 7;                          \
   } else if (next == 6) {               \
      CVMX_MT_HSH_DAT (dat1, 6);         \
      CVMX_MT_HSH_STARTMD5 (dat2);       \
      next = 0;                          \
   } else {                              \
     CVMX_MT_HSH_STARTMD5 (dat1);        \
     CVMX_MT_HSH_DAT (dat2, 0);          \
     next = 1;                           \
   }                                     \
}

/****************************************************************************/

static inline uint64_t
swap64(uint64_t a)
{
    return ((a >> 56) |
       (((a >> 48) & 0xfful) << 8) |
       (((a >> 40) & 0xfful) << 16) |
       (((a >> 32) & 0xfful) << 24) |
       (((a >> 24) & 0xfful) << 32) |
       (((a >> 16) & 0xfful) << 40) |
       (((a >> 8) & 0xfful) << 48) | (((a >> 0) & 0xfful) << 56));
}

/****************************************************************************/

void
octo_calc_hash(__u8 auth, unsigned char *key, uint64_t *inner, uint64_t *outer)
{
    uint8_t hash_key[64];
    uint64_t *key1;
    register uint64_t xor1 = 0x3636363636363636ULL;
    register uint64_t xor2 = 0x5c5c5c5c5c5c5c5cULL;
    struct octeon_cop2_state state;
    unsigned long flags;

    dprintk("%s()\n", __FUNCTION__);

    memset(hash_key, 0, sizeof(hash_key));
    memcpy(hash_key, (uint8_t *) key, (auth ? 20 : 16));
    key1 = (uint64_t *) hash_key;
    flags = octeon_crypto_enable(&state);
    if (auth) {
       CVMX_MT_HSH_IV(0x67452301EFCDAB89ULL, 0);
       CVMX_MT_HSH_IV(0x98BADCFE10325476ULL, 1);
       CVMX_MT_HSH_IV(0xC3D2E1F000000000ULL, 2);
    } else {
       CVMX_MT_HSH_IV(0x0123456789ABCDEFULL, 0);
       CVMX_MT_HSH_IV(0xFEDCBA9876543210ULL, 1);
    }

    CVMX_MT_HSH_DAT((*key1 ^ xor1), 0);
    key1++;
    CVMX_MT_HSH_DAT((*key1 ^ xor1), 1);
    key1++;
    CVMX_MT_HSH_DAT((*key1 ^ xor1), 2);
    key1++;
    CVMX_MT_HSH_DAT((*key1 ^ xor1), 3);
    key1++;
    CVMX_MT_HSH_DAT((*key1 ^ xor1), 4);
    key1++;
    CVMX_MT_HSH_DAT((*key1 ^ xor1), 5);
    key1++;
    CVMX_MT_HSH_DAT((*key1 ^ xor1), 6);
    key1++;
    if (auth)
	CVMX_MT_HSH_STARTSHA((*key1 ^ xor1));
    else
	CVMX_MT_HSH_STARTMD5((*key1 ^ xor1));

    CVMX_MF_HSH_IV(inner[0], 0);
    CVMX_MF_HSH_IV(inner[1], 1);
    if (auth) {
	inner[2] = 0;
	CVMX_MF_HSH_IV(((uint64_t *) inner)[2], 2);
    }

    memset(hash_key, 0, sizeof(hash_key));
    memcpy(hash_key, (uint8_t *) key, (auth ? 20 : 16));
    key1 = (uint64_t *) hash_key;
    if (auth) {
      CVMX_MT_HSH_IV(0x67452301EFCDAB89ULL, 0);
      CVMX_MT_HSH_IV(0x98BADCFE10325476ULL, 1);
      CVMX_MT_HSH_IV(0xC3D2E1F000000000ULL, 2);
    } else {
      CVMX_MT_HSH_IV(0x0123456789ABCDEFULL, 0);
      CVMX_MT_HSH_IV(0xFEDCBA9876543210ULL, 1);
    }

    CVMX_MT_HSH_DAT((*key1 ^ xor2), 0);
    key1++;
    CVMX_MT_HSH_DAT((*key1 ^ xor2), 1);
    key1++;
    CVMX_MT_HSH_DAT((*key1 ^ xor2), 2);
    key1++;
    CVMX_MT_HSH_DAT((*key1 ^ xor2), 3);
    key1++;
    CVMX_MT_HSH_DAT((*key1 ^ xor2), 4);
    key1++;
    CVMX_MT_HSH_DAT((*key1 ^ xor2), 5);
    key1++;
    CVMX_MT_HSH_DAT((*key1 ^ xor2), 6);
    key1++;
    if (auth)
       CVMX_MT_HSH_STARTSHA((*key1 ^ xor2));
    else 
       CVMX_MT_HSH_STARTMD5((*key1 ^ xor2));

    CVMX_MF_HSH_IV(outer[0], 0);
    CVMX_MF_HSH_IV(outer[1], 1);
    if (auth) {
      outer[2] = 0;
      CVMX_MF_HSH_IV(outer[2], 2);
    }
    octeon_crypto_disable(&state, flags);
    return;
}

/****************************************************************************/
/* DES functions */

int
octo_des_cbc_encrypt(
    struct octo_sess *od,
    struct scatterlist *sg, int sg_len,
    int auth_off, int auth_len,
    int crypt_off, int crypt_len,
    int icv_off, uint8_t *ivp)
{
    uint64_t *data;
    int data_i, data_l;
    struct octeon_cop2_state state;
    unsigned long flags;

    dprintk("%s()\n", __FUNCTION__);

    if (unlikely(od == NULL || sg==NULL || sg_len==0 || ivp==NULL ||
	    (crypt_off & 0x7) || (crypt_off + crypt_len > sg_len))) {
	dprintk("%s: Bad parameters od=%p sg=%p sg_len=%d "
		"auth_off=%d auth_len=%d crypt_off=%d crypt_len=%d "
		"icv_off=%d ivp=%p\n", __FUNCTION__, od, sg, sg_len,
		auth_off, auth_len, crypt_off, crypt_len, icv_off, ivp);
	return -EINVAL;
    }

    SG_INIT(sg, data, data_i, data_l);

    CVMX_PREFETCH0(ivp);
    CVMX_PREFETCH0(od->octo_enckey);

    flags = octeon_crypto_enable(&state);

    /* load 3DES Key */
    CVMX_MT_3DES_KEY(((uint64_t *) od->octo_enckey)[0], 0);
    if (od->octo_encklen == 24) {
	CVMX_MT_3DES_KEY(((uint64_t *) od->octo_enckey)[1], 1);
	CVMX_MT_3DES_KEY(((uint64_t *) od->octo_enckey)[2], 2);
    } else if (od->octo_encklen == 8) {
	CVMX_MT_3DES_KEY(((uint64_t *) od->octo_enckey)[0], 1);
	CVMX_MT_3DES_KEY(((uint64_t *) od->octo_enckey)[0], 2);
    } else {
	octeon_crypto_disable(&state, flags);
	dprintk("%s: Bad key length %d\n", __FUNCTION__, od->octo_encklen);
	return -EINVAL;
    }

    CVMX_MT_3DES_IV(* (uint64_t *) ivp);

    while (crypt_off > 0) {
	SG_CONSUME(sg, data, data_i, data_l);
	crypt_off -= 8;
    }

    while (crypt_len > 0) {
	CVMX_MT_3DES_ENC_CBC(*data);
	CVMX_MF_3DES_RESULT(*data);
	SG_CONSUME(sg, data, data_i, data_l);
	crypt_len -= 8;
    }

    octeon_crypto_disable(&state, flags);
    return 0;
}


int
octo_des_cbc_decrypt(
    struct octo_sess *od,
    struct scatterlist *sg, int sg_len,
    int auth_off, int auth_len,
    int crypt_off, int crypt_len,
    int icv_off, uint8_t *ivp)
{
    uint64_t *data;
    int data_i, data_l;
    struct octeon_cop2_state state;
    unsigned long flags;

    dprintk("%s()\n", __FUNCTION__);

    if (unlikely(od == NULL || sg==NULL || sg_len==0 || ivp==NULL ||
	    (crypt_off & 0x7) || (crypt_off + crypt_len > sg_len))) {
	dprintk("%s: Bad parameters od=%p sg=%p sg_len=%d "
		"auth_off=%d auth_len=%d crypt_off=%d crypt_len=%d "
		"icv_off=%d ivp=%p\n", __FUNCTION__, od, sg, sg_len,
		auth_off, auth_len, crypt_off, crypt_len, icv_off, ivp);
	return -EINVAL;
    }

    SG_INIT(sg, data, data_i, data_l);

    CVMX_PREFETCH0(ivp);
    CVMX_PREFETCH0(od->octo_enckey);

    flags = octeon_crypto_enable(&state);

    /* load 3DES Key */
    CVMX_MT_3DES_KEY(((uint64_t *) od->octo_enckey)[0], 0);
    if (od->octo_encklen == 24) {
	CVMX_MT_3DES_KEY(((uint64_t *) od->octo_enckey)[1], 1);
	CVMX_MT_3DES_KEY(((uint64_t *) od->octo_enckey)[2], 2);
    } else if (od->octo_encklen == 8) {
	CVMX_MT_3DES_KEY(((uint64_t *) od->octo_enckey)[0], 1);
	CVMX_MT_3DES_KEY(((uint64_t *) od->octo_enckey)[0], 2);
    } else {
	octeon_crypto_disable(&state, flags);
	dprintk("%s: Bad key length %d\n", __FUNCTION__, od->octo_encklen);
	return -EINVAL;
    }

    CVMX_MT_3DES_IV(* (uint64_t *) ivp);

    while (crypt_off > 0) {
	SG_CONSUME(sg, data, data_i, data_l);
	crypt_off -= 8;
    }

    while (crypt_len > 0) {
	CVMX_MT_3DES_DEC_CBC(*data);
	CVMX_MF_3DES_RESULT(*data);
	SG_CONSUME(sg, data, data_i, data_l);
	crypt_len -= 8;
    }

    octeon_crypto_disable(&state, flags);
    return 0;
}

/****************************************************************************/
/* AES functions */

int
octo_aes_cbc_encrypt(
    struct octo_sess *od,
    struct scatterlist *sg, int sg_len,
    int auth_off, int auth_len,
    int crypt_off, int crypt_len,
    int icv_off, uint8_t *ivp)
{
    uint64_t *data, *pdata;
    int data_i, data_l;
    struct octeon_cop2_state state;
    unsigned long flags;

    dprintk("%s()\n", __FUNCTION__);

    if (unlikely(od == NULL || sg==NULL || sg_len==0 || ivp==NULL ||
	    (crypt_off & 0x7) || (crypt_off + crypt_len > sg_len))) {
	dprintk("%s: Bad parameters od=%p sg=%p sg_len=%d "
		"auth_off=%d auth_len=%d crypt_off=%d crypt_len=%d "
		"icv_off=%d ivp=%p\n", __FUNCTION__, od, sg, sg_len,
		auth_off, auth_len, crypt_off, crypt_len, icv_off, ivp);
	return -EINVAL;
    }

    SG_INIT(sg, data, data_i, data_l);

    CVMX_PREFETCH0(ivp);
    CVMX_PREFETCH0(od->octo_enckey);

    flags = octeon_crypto_enable(&state);

    /* load AES Key */
    CVMX_MT_AES_KEY(((uint64_t *) od->octo_enckey)[0], 0);
    CVMX_MT_AES_KEY(((uint64_t *) od->octo_enckey)[1], 1);

    if (od->octo_encklen == 16) {
	CVMX_MT_AES_KEY(0x0, 2);
	CVMX_MT_AES_KEY(0x0, 3);
    } else if (od->octo_encklen == 24) {
	CVMX_MT_AES_KEY(((uint64_t *) od->octo_enckey)[2], 2);
	CVMX_MT_AES_KEY(0x0, 3);
    } else if (od->octo_encklen == 32) {
	CVMX_MT_AES_KEY(((uint64_t *) od->octo_enckey)[2], 2);
	CVMX_MT_AES_KEY(((uint64_t *) od->octo_enckey)[3], 3);
    } else {
	octeon_crypto_disable(&state, flags);
	dprintk("%s: Bad key length %d\n", __FUNCTION__, od->octo_encklen);
	return -EINVAL;
    }
    CVMX_MT_AES_KEYLENGTH(od->octo_encklen / 8 - 1);

    CVMX_MT_AES_IV(((uint64_t *) ivp)[0], 0);
    CVMX_MT_AES_IV(((uint64_t *) ivp)[1], 1);

    while (crypt_off > 0) {
	SG_CONSUME(sg, data, data_i, data_l);
	crypt_off -= 8;
    }

    while (crypt_len > 0) {
	pdata = data;
	CVMX_MT_AES_ENC_CBC0(*data);
	SG_CONSUME(sg, data, data_i, data_l);
	CVMX_MT_AES_ENC_CBC1(*data);
	CVMX_MF_AES_RESULT(*pdata, 0);
	CVMX_MF_AES_RESULT(*data, 1);
	SG_CONSUME(sg, data, data_i, data_l);
	crypt_len -= 16;
    }

    octeon_crypto_disable(&state, flags);
    return 0;
}


int
octo_aes_cbc_decrypt(
    struct octo_sess *od,
    struct scatterlist *sg, int sg_len,
    int auth_off, int auth_len,
    int crypt_off, int crypt_len,
    int icv_off, uint8_t *ivp)
{
    uint64_t *data, *pdata;
    int data_i, data_l;
    struct octeon_cop2_state state;
    unsigned long flags;

    dprintk("%s()\n", __FUNCTION__);

    if (unlikely(od == NULL || sg==NULL || sg_len==0 || ivp==NULL ||
	    (crypt_off & 0x7) || (crypt_off + crypt_len > sg_len))) {
	dprintk("%s: Bad parameters od=%p sg=%p sg_len=%d "
		"auth_off=%d auth_len=%d crypt_off=%d crypt_len=%d "
		"icv_off=%d ivp=%p\n", __FUNCTION__, od, sg, sg_len,
		auth_off, auth_len, crypt_off, crypt_len, icv_off, ivp);
	return -EINVAL;
    }

    SG_INIT(sg, data, data_i, data_l);

    CVMX_PREFETCH0(ivp);
    CVMX_PREFETCH0(od->octo_enckey);

    flags = octeon_crypto_enable(&state);

    /* load AES Key */
    CVMX_MT_AES_KEY(((uint64_t *) od->octo_enckey)[0], 0);
    CVMX_MT_AES_KEY(((uint64_t *) od->octo_enckey)[1], 1);

    if (od->octo_encklen == 16) {
	CVMX_MT_AES_KEY(0x0, 2);
	CVMX_MT_AES_KEY(0x0, 3);
    } else if (od->octo_encklen == 24) {
	CVMX_MT_AES_KEY(((uint64_t *) od->octo_enckey)[2], 2);
	CVMX_MT_AES_KEY(0x0, 3);
    } else if (od->octo_encklen == 32) {
	CVMX_MT_AES_KEY(((uint64_t *) od->octo_enckey)[2], 2);
	CVMX_MT_AES_KEY(((uint64_t *) od->octo_enckey)[3], 3);
    } else {
	octeon_crypto_disable(&state, flags);
	dprintk("%s: Bad key length %d\n", __FUNCTION__, od->octo_encklen);
	return -EINVAL;
    }
    CVMX_MT_AES_KEYLENGTH(od->octo_encklen / 8 - 1);

    CVMX_MT_AES_IV(((uint64_t *) ivp)[0], 0);
    CVMX_MT_AES_IV(((uint64_t *) ivp)[1], 1);

    while (crypt_off > 0) {
	SG_CONSUME(sg, data, data_i, data_l);
	crypt_off -= 8;
    }

    while (crypt_len > 0) {
	pdata = data;
	CVMX_MT_AES_DEC_CBC0(*data);
	SG_CONSUME(sg, data, data_i, data_l);
	CVMX_MT_AES_DEC_CBC1(*data);
	CVMX_MF_AES_RESULT(*pdata, 0);
	CVMX_MF_AES_RESULT(*data, 1);
	SG_CONSUME(sg, data, data_i, data_l);
	crypt_len -= 16;
    }

    octeon_crypto_disable(&state, flags);
    return 0;
}

/****************************************************************************/
/* MD5 */

int
octo_null_md5_encrypt(
    struct octo_sess *od,
    struct scatterlist *sg, int sg_len,
    int auth_off, int auth_len,
    int crypt_off, int crypt_len,
    int icv_off, uint8_t *ivp)
{
    register int next = 0;
    uint64_t *data;
    uint64_t tmp1, tmp2;
    int data_i, data_l, alen = auth_len;
    struct octeon_cop2_state state;
    unsigned long flags;

    dprintk("%s()\n", __FUNCTION__);

    if (unlikely(od == NULL || sg==NULL || sg_len==0 ||
	    (auth_off & 0x7) || (auth_off + auth_len > sg_len))) {
	dprintk("%s: Bad parameters od=%p sg=%p sg_len=%d "
		"auth_off=%d auth_len=%d crypt_off=%d crypt_len=%d "
		"icv_off=%d ivp=%p\n", __FUNCTION__, od, sg, sg_len,
		auth_off, auth_len, crypt_off, crypt_len, icv_off, ivp);
	return -EINVAL;
    }

    SG_INIT(sg, data, data_i, data_l);

    flags = octeon_crypto_enable(&state);

    /* Load MD5 IV */
    CVMX_MT_HSH_IV(od->octo_hminner[0], 0);
    CVMX_MT_HSH_IV(od->octo_hminner[1], 1);

    while (auth_off > 0) {
	SG_CONSUME(sg, data, data_i, data_l);
	auth_off -= 8;
    }

    while (auth_len > 0) {
	CVM_LOAD_MD5_UNIT(*data, next);
	auth_len -= 8;
	SG_CONSUME(sg, data, data_i, data_l);
    }

    /* finish the hash */
    CVMX_PREFETCH0(od->octo_hmouter);
#if 0
    if (unlikely(inplen)) {
	uint64_t tmp = 0;
	uint8_t *p = (uint8_t *) & tmp;
	p[inplen] = 0x80;
	do {
	    inplen--;
	    p[inplen] = ((uint8_t *) data)[inplen];
	} while (inplen);
	CVM_LOAD_MD5_UNIT(tmp, next);
    } else {
	CVM_LOAD_MD5_UNIT(0x8000000000000000ULL, next);
    }
#else
    CVM_LOAD_MD5_UNIT(0x8000000000000000ULL, next);
#endif

    /* Finish Inner hash */
    while (next != 7) {
	CVM_LOAD_MD5_UNIT(((uint64_t) 0x0ULL), next);
    }
    CVMX_ES64(tmp1, ((alen + 64) << 3));
    CVM_LOAD_MD5_UNIT(tmp1, next);

    /* Get the inner hash of HMAC */
    CVMX_MF_HSH_IV(tmp1, 0);
    CVMX_MF_HSH_IV(tmp2, 1);

    /* Initialize hash unit */
    CVMX_MT_HSH_IV(od->octo_hmouter[0], 0);
    CVMX_MT_HSH_IV(od->octo_hmouter[1], 1);

    CVMX_MT_HSH_DAT(tmp1, 0);
    CVMX_MT_HSH_DAT(tmp2, 1);
    CVMX_MT_HSH_DAT(0x8000000000000000ULL, 2);
    CVMX_MT_HSH_DATZ(3);
    CVMX_MT_HSH_DATZ(4);
    CVMX_MT_HSH_DATZ(5);
    CVMX_MT_HSH_DATZ(6);
    CVMX_ES64(tmp1, ((64 + 16) << 3));
    CVMX_MT_HSH_STARTMD5(tmp1);

    /* save the HMAC */
    SG_INIT(sg, data, data_i, data_l);
    while (icv_off > 0) {
	SG_CONSUME(sg, data, data_i, data_l);
	icv_off -= 8;
    }
    CVMX_MF_HSH_IV(*data, 0);
    SG_CONSUME(sg, data, data_i, data_l);
    CVMX_MF_HSH_IV(tmp1, 1);
    *(uint32_t *)data = (uint32_t) (tmp1 >> 32);

    octeon_crypto_disable(&state, flags);
    return 0;
}

/****************************************************************************/
/* SHA1 */

int
octo_null_sha1_encrypt(
    struct octo_sess *od,
    struct scatterlist *sg, int sg_len,
    int auth_off, int auth_len,
    int crypt_off, int crypt_len,
    int icv_off, uint8_t *ivp)
{
    register int next = 0;
    uint64_t *data;
    uint64_t tmp1, tmp2, tmp3;
    int data_i, data_l, alen = auth_len;
    struct octeon_cop2_state state;
    unsigned long flags;

    dprintk("%s()\n", __FUNCTION__);

    if (unlikely(od == NULL || sg==NULL || sg_len==0 ||
	    (auth_off & 0x7) || (auth_off + auth_len > sg_len))) {
	dprintk("%s: Bad parameters od=%p sg=%p sg_len=%d "
		"auth_off=%d auth_len=%d crypt_off=%d crypt_len=%d "
		"icv_off=%d ivp=%p\n", __FUNCTION__, od, sg, sg_len,
		auth_off, auth_len, crypt_off, crypt_len, icv_off, ivp);
	return -EINVAL;
    }

    SG_INIT(sg, data, data_i, data_l);

    flags = octeon_crypto_enable(&state);

    /* Load SHA1 IV */
    CVMX_MT_HSH_IV(od->octo_hminner[0], 0);
    CVMX_MT_HSH_IV(od->octo_hminner[1], 1);
    CVMX_MT_HSH_IV(od->octo_hminner[2], 2);

    while (auth_off > 0) {
	SG_CONSUME(sg, data, data_i, data_l);
	auth_off -= 8;
    }

    while (auth_len > 0) {
	CVM_LOAD_SHA_UNIT(*data, next);
	auth_len -= 8;
	SG_CONSUME(sg, data, data_i, data_l);
    }

    /* finish the hash */
    CVMX_PREFETCH0(od->octo_hmouter);
#if 0
    if (unlikely(inplen)) {
	uint64_t tmp = 0;
	uint8_t *p = (uint8_t *) & tmp;
	p[inplen] = 0x80;
	do {
	    inplen--;
	    p[inplen] = ((uint8_t *) data)[inplen];
	} while (inplen);
	CVM_LOAD_MD5_UNIT(tmp, next);
    } else {
	CVM_LOAD_MD5_UNIT(0x8000000000000000ULL, next);
    }
#else
    CVM_LOAD_SHA_UNIT(0x8000000000000000ULL, next);
#endif

    /* Finish Inner hash */
    while (next != 7) {
	CVM_LOAD_SHA_UNIT(((uint64_t) 0x0ULL), next);
    }
    CVM_LOAD_SHA_UNIT((uint64_t) ((alen + 64) << 3), next);

    /* Get the inner hash of HMAC */
    CVMX_MF_HSH_IV(tmp1, 0);
    CVMX_MF_HSH_IV(tmp2, 1);
    tmp3 = 0;
    CVMX_MF_HSH_IV(tmp3, 2);

    /* Initialize hash unit */
    CVMX_MT_HSH_IV(od->octo_hmouter[0], 0);
    CVMX_MT_HSH_IV(od->octo_hmouter[1], 1);
    CVMX_MT_HSH_IV(od->octo_hmouter[2], 2);

    CVMX_MT_HSH_DAT(tmp1, 0);
    CVMX_MT_HSH_DAT(tmp2, 1);
    tmp3 |= 0x0000000080000000;
    CVMX_MT_HSH_DAT(tmp3, 2);
    CVMX_MT_HSH_DATZ(3);
    CVMX_MT_HSH_DATZ(4);
    CVMX_MT_HSH_DATZ(5);
    CVMX_MT_HSH_DATZ(6);
    CVMX_MT_HSH_STARTSHA((uint64_t) ((64 + 20) << 3));

    /* save the HMAC */
    SG_INIT(sg, data, data_i, data_l);
    while (icv_off > 0) {
	SG_CONSUME(sg, data, data_i, data_l);
	icv_off -= 8;
    }
    CVMX_MF_HSH_IV(*data, 0);
    SG_CONSUME(sg, data, data_i, data_l);
    CVMX_MF_HSH_IV(tmp1, 1);
    *(uint32_t *)data = (uint32_t) (tmp1 >> 32);

    octeon_crypto_disable(&state, flags);
    return 0;
}

/****************************************************************************/
/* DES MD5 */

int
octo_des_cbc_md5_encrypt(
    struct octo_sess *od,
    struct scatterlist *sg, int sg_len,
    int auth_off, int auth_len,
    int crypt_off, int crypt_len,
    int icv_off, uint8_t *ivp)
{
    register int next = 0;
    union {
	uint32_t data32[2];
	uint64_t data64[1];
    } mydata;
    uint64_t *data = &mydata.data64[0];
    uint32_t *data32;
    uint64_t tmp1, tmp2;
    int data_i, data_l, alen = auth_len;
    struct octeon_cop2_state state;
    unsigned long flags;

    dprintk("%s()\n", __FUNCTION__);

    if (unlikely(od == NULL || sg==NULL || sg_len==0 || ivp==NULL ||
	    (crypt_off & 0x3) || (crypt_off + crypt_len > sg_len) ||
	    (crypt_len  & 0x7) ||
	    (auth_len  & 0x7) ||
	    (auth_off & 0x3) || (auth_off + auth_len > sg_len))) {
	dprintk("%s: Bad parameters od=%p sg=%p sg_len=%d "
		"auth_off=%d auth_len=%d crypt_off=%d crypt_len=%d "
		"icv_off=%d ivp=%p\n", __FUNCTION__, od, sg, sg_len,
		auth_off, auth_len, crypt_off, crypt_len, icv_off, ivp);
	return -EINVAL;
    }

    SG_INIT(sg, data32, data_i, data_l);

    CVMX_PREFETCH0(ivp);
    CVMX_PREFETCH0(od->octo_enckey);

    flags = octeon_crypto_enable(&state);

    /* load 3DES Key */
    CVMX_MT_3DES_KEY(((uint64_t *) od->octo_enckey)[0], 0);
    if (od->octo_encklen == 24) {
	CVMX_MT_3DES_KEY(((uint64_t *) od->octo_enckey)[1], 1);
	CVMX_MT_3DES_KEY(((uint64_t *) od->octo_enckey)[2], 2);
    } else if (od->octo_encklen == 8) {
	CVMX_MT_3DES_KEY(((uint64_t *) od->octo_enckey)[0], 1);
	CVMX_MT_3DES_KEY(((uint64_t *) od->octo_enckey)[0], 2);
    } else {
	octeon_crypto_disable(&state, flags);
	dprintk("%s: Bad key length %d\n", __FUNCTION__, od->octo_encklen);
	return -EINVAL;
    }

    CVMX_MT_3DES_IV(* (uint64_t *) ivp);

    /* Load MD5 IV */
    CVMX_MT_HSH_IV(od->octo_hminner[0], 0);
    CVMX_MT_HSH_IV(od->octo_hminner[1], 1);

    while (crypt_off > 0 && auth_off > 0) {
	SG_CONSUME(sg, data32, data_i, data_l);
	crypt_off -= 4;
	auth_off -= 4;
    }

    while (crypt_len > 0 || auth_len > 0) {
    	uint32_t *first = data32;
	mydata.data32[0] = *first;
	SG_CONSUME(sg, data32, data_i, data_l);
	mydata.data32[1] = *data32;
    	if (crypt_off <= 0) {
	    if (crypt_len > 0) {
		CVMX_MT_3DES_ENC_CBC(*data);
		CVMX_MF_3DES_RESULT(*data);
		crypt_len -= 8;
	    }
	} else
	    crypt_off -= 8;
    	if (auth_off <= 0) {
	    if (auth_len > 0) {
		CVM_LOAD_MD5_UNIT(*data, next);
		auth_len -= 8;
	    }
	} else
	    auth_off -= 8;
	*first = mydata.data32[0];
	*data32 = mydata.data32[1];
	SG_CONSUME(sg, data32, data_i, data_l);
    }

    /* finish the hash */
    CVMX_PREFETCH0(od->octo_hmouter);
#if 0
    if (unlikely(inplen)) {
	uint64_t tmp = 0;
	uint8_t *p = (uint8_t *) & tmp;
	p[inplen] = 0x80;
	do {
	    inplen--;
	    p[inplen] = ((uint8_t *) data)[inplen];
	} while (inplen);
	CVM_LOAD_MD5_UNIT(tmp, next);
    } else {
	CVM_LOAD_MD5_UNIT(0x8000000000000000ULL, next);
    }
#else
    CVM_LOAD_MD5_UNIT(0x8000000000000000ULL, next);
#endif

    /* Finish Inner hash */
    while (next != 7) {
	CVM_LOAD_MD5_UNIT(((uint64_t) 0x0ULL), next);
    }
    CVMX_ES64(tmp1, ((alen + 64) << 3));
    CVM_LOAD_MD5_UNIT(tmp1, next);

    /* Get the inner hash of HMAC */
    CVMX_MF_HSH_IV(tmp1, 0);
    CVMX_MF_HSH_IV(tmp2, 1);

    /* Initialize hash unit */
    CVMX_MT_HSH_IV(od->octo_hmouter[0], 0);
    CVMX_MT_HSH_IV(od->octo_hmouter[1], 1);

    CVMX_MT_HSH_DAT(tmp1, 0);
    CVMX_MT_HSH_DAT(tmp2, 1);
    CVMX_MT_HSH_DAT(0x8000000000000000ULL, 2);
    CVMX_MT_HSH_DATZ(3);
    CVMX_MT_HSH_DATZ(4);
    CVMX_MT_HSH_DATZ(5);
    CVMX_MT_HSH_DATZ(6);
    CVMX_ES64(tmp1, ((64 + 16) << 3));
    CVMX_MT_HSH_STARTMD5(tmp1);

    /* save the HMAC */
    SG_INIT(sg, data32, data_i, data_l);
    while (icv_off > 0) {
	SG_CONSUME(sg, data32, data_i, data_l);
	icv_off -= 4;
    }
    CVMX_MF_HSH_IV(tmp1, 0);
    *data32 = (uint32_t) (tmp1 >> 32);
    SG_CONSUME(sg, data32, data_i, data_l);
    *data32 = (uint32_t) tmp1;
    SG_CONSUME(sg, data32, data_i, data_l);
    CVMX_MF_HSH_IV(tmp1, 1);
    *data32 = (uint32_t) (tmp1 >> 32);

    octeon_crypto_disable(&state, flags);
    return 0;
}

int
octo_des_cbc_md5_decrypt(
    struct octo_sess *od,
    struct scatterlist *sg, int sg_len,
    int auth_off, int auth_len,
    int crypt_off, int crypt_len,
    int icv_off, uint8_t *ivp)
{
    register int next = 0;
    union {
	uint32_t data32[2];
	uint64_t data64[1];
    } mydata;
    uint64_t *data = &mydata.data64[0];
    uint32_t *data32;
    uint64_t tmp1, tmp2;
    int data_i, data_l, alen = auth_len;
    struct octeon_cop2_state state;
    unsigned long flags;

    dprintk("%s()\n", __FUNCTION__);

    if (unlikely(od == NULL || sg==NULL || sg_len==0 || ivp==NULL ||
	    (crypt_off & 0x3) || (crypt_off + crypt_len > sg_len) ||
	    (crypt_len  & 0x7) ||
	    (auth_len  & 0x7) ||
	    (auth_off & 0x3) || (auth_off + auth_len > sg_len))) {
	dprintk("%s: Bad parameters od=%p sg=%p sg_len=%d "
		"auth_off=%d auth_len=%d crypt_off=%d crypt_len=%d "
		"icv_off=%d ivp=%p\n", __FUNCTION__, od, sg, sg_len,
		auth_off, auth_len, crypt_off, crypt_len, icv_off, ivp);
	return -EINVAL;
    }

    SG_INIT(sg, data32, data_i, data_l);

    CVMX_PREFETCH0(ivp);
    CVMX_PREFETCH0(od->octo_enckey);

    flags = octeon_crypto_enable(&state);

    /* load 3DES Key */
    CVMX_MT_3DES_KEY(((uint64_t *) od->octo_enckey)[0], 0);
    if (od->octo_encklen == 24) {
	CVMX_MT_3DES_KEY(((uint64_t *) od->octo_enckey)[1], 1);
	CVMX_MT_3DES_KEY(((uint64_t *) od->octo_enckey)[2], 2);
    } else if (od->octo_encklen == 8) {
	CVMX_MT_3DES_KEY(((uint64_t *) od->octo_enckey)[0], 1);
	CVMX_MT_3DES_KEY(((uint64_t *) od->octo_enckey)[0], 2);
    } else {
	octeon_crypto_disable(&state, flags);
	dprintk("%s: Bad key length %d\n", __FUNCTION__, od->octo_encklen);
	return -EINVAL;
    }

    CVMX_MT_3DES_IV(* (uint64_t *) ivp);

    /* Load MD5 IV */
    CVMX_MT_HSH_IV(od->octo_hminner[0], 0);
    CVMX_MT_HSH_IV(od->octo_hminner[1], 1);

    while (crypt_off > 0 && auth_off > 0) {
	SG_CONSUME(sg, data32, data_i, data_l);
	crypt_off -= 4;
	auth_off -= 4;
    }

    while (crypt_len > 0 || auth_len > 0) {
    	uint32_t *first = data32;
	mydata.data32[0] = *first;
	SG_CONSUME(sg, data32, data_i, data_l);
	mydata.data32[1] = *data32;
    	if (auth_off <= 0) {
	    if (auth_len > 0) {
		CVM_LOAD_MD5_UNIT(*data, next);
		auth_len -= 8;
	    }
	} else
	    auth_off -= 8;
    	if (crypt_off <= 0) {
	    if (crypt_len > 0) {
		CVMX_MT_3DES_DEC_CBC(*data);
		CVMX_MF_3DES_RESULT(*data);
		crypt_len -= 8;
	    }
	} else
	    crypt_off -= 8;
	*first = mydata.data32[0];
	*data32 = mydata.data32[1];
	SG_CONSUME(sg, data32, data_i, data_l);
    }

    /* finish the hash */
    CVMX_PREFETCH0(od->octo_hmouter);
#if 0
    if (unlikely(inplen)) {
	uint64_t tmp = 0;
	uint8_t *p = (uint8_t *) & tmp;
	p[inplen] = 0x80;
	do {
	    inplen--;
	    p[inplen] = ((uint8_t *) data)[inplen];
	} while (inplen);
	CVM_LOAD_MD5_UNIT(tmp, next);
    } else {
	CVM_LOAD_MD5_UNIT(0x8000000000000000ULL, next);
    }
#else
    CVM_LOAD_MD5_UNIT(0x8000000000000000ULL, next);
#endif

    /* Finish Inner hash */
    while (next != 7) {
	CVM_LOAD_MD5_UNIT(((uint64_t) 0x0ULL), next);
    }
    CVMX_ES64(tmp1, ((alen + 64) << 3));
    CVM_LOAD_MD5_UNIT(tmp1, next);

    /* Get the inner hash of HMAC */
    CVMX_MF_HSH_IV(tmp1, 0);
    CVMX_MF_HSH_IV(tmp2, 1);

    /* Initialize hash unit */
    CVMX_MT_HSH_IV(od->octo_hmouter[0], 0);
    CVMX_MT_HSH_IV(od->octo_hmouter[1], 1);

    CVMX_MT_HSH_DAT(tmp1, 0);
    CVMX_MT_HSH_DAT(tmp2, 1);
    CVMX_MT_HSH_DAT(0x8000000000000000ULL, 2);
    CVMX_MT_HSH_DATZ(3);
    CVMX_MT_HSH_DATZ(4);
    CVMX_MT_HSH_DATZ(5);
    CVMX_MT_HSH_DATZ(6);
    CVMX_ES64(tmp1, ((64 + 16) << 3));
    CVMX_MT_HSH_STARTMD5(tmp1);

    /* save the HMAC */
    SG_INIT(sg, data32, data_i, data_l);
    while (icv_off > 0) {
	SG_CONSUME(sg, data32, data_i, data_l);
	icv_off -= 4;
    }
    CVMX_MF_HSH_IV(tmp1, 0);
    *data32 = (uint32_t) (tmp1 >> 32);
    SG_CONSUME(sg, data32, data_i, data_l);
    *data32 = (uint32_t) tmp1;
    SG_CONSUME(sg, data32, data_i, data_l);
    CVMX_MF_HSH_IV(tmp1, 1);
    *data32 = (uint32_t) (tmp1 >> 32);

    octeon_crypto_disable(&state, flags);
    return 0;
}

/****************************************************************************/
/* DES SHA */

int
octo_des_cbc_sha1_encrypt(
    struct octo_sess *od,
    struct scatterlist *sg, int sg_len,
    int auth_off, int auth_len,
    int crypt_off, int crypt_len,
    int icv_off, uint8_t *ivp)
{
    register int next = 0;
    union {
	uint32_t data32[2];
	uint64_t data64[1];
    } mydata;
    uint64_t *data = &mydata.data64[0];
    uint32_t *data32;
    uint64_t tmp1, tmp2, tmp3;
    int data_i, data_l, alen = auth_len;
    struct octeon_cop2_state state;
    unsigned long flags;

    dprintk("%s()\n", __FUNCTION__);

    if (unlikely(od == NULL || sg==NULL || sg_len==0 || ivp==NULL ||
	    (crypt_off & 0x3) || (crypt_off + crypt_len > sg_len) ||
	    (crypt_len  & 0x7) ||
	    (auth_len  & 0x7) ||
	    (auth_off & 0x3) || (auth_off + auth_len > sg_len))) {
	dprintk("%s: Bad parameters od=%p sg=%p sg_len=%d "
		"auth_off=%d auth_len=%d crypt_off=%d crypt_len=%d "
		"icv_off=%d ivp=%p\n", __FUNCTION__, od, sg, sg_len,
		auth_off, auth_len, crypt_off, crypt_len, icv_off, ivp);
	return -EINVAL;
    }

    SG_INIT(sg, data32, data_i, data_l);

    CVMX_PREFETCH0(ivp);
    CVMX_PREFETCH0(od->octo_enckey);

    flags = octeon_crypto_enable(&state);

    /* load 3DES Key */
    CVMX_MT_3DES_KEY(((uint64_t *) od->octo_enckey)[0], 0);
    if (od->octo_encklen == 24) {
	CVMX_MT_3DES_KEY(((uint64_t *) od->octo_enckey)[1], 1);
	CVMX_MT_3DES_KEY(((uint64_t *) od->octo_enckey)[2], 2);
    } else if (od->octo_encklen == 8) {
	CVMX_MT_3DES_KEY(((uint64_t *) od->octo_enckey)[0], 1);
	CVMX_MT_3DES_KEY(((uint64_t *) od->octo_enckey)[0], 2);
    } else {
	octeon_crypto_disable(&state, flags);
	dprintk("%s: Bad key length %d\n", __FUNCTION__, od->octo_encklen);
	return -EINVAL;
    }

    CVMX_MT_3DES_IV(* (uint64_t *) ivp);

    /* Load SHA1 IV */
    CVMX_MT_HSH_IV(od->octo_hminner[0], 0);
    CVMX_MT_HSH_IV(od->octo_hminner[1], 1);
    CVMX_MT_HSH_IV(od->octo_hminner[2], 2);

    while (crypt_off > 0 && auth_off > 0) {
	SG_CONSUME(sg, data32, data_i, data_l);
	crypt_off -= 4;
	auth_off -= 4;
    }

    while (crypt_len > 0 || auth_len > 0) {
    	uint32_t *first = data32;
	mydata.data32[0] = *first;
	SG_CONSUME(sg, data32, data_i, data_l);
	mydata.data32[1] = *data32;
    	if (crypt_off <= 0) {
	    if (crypt_len > 0) {
		CVMX_MT_3DES_ENC_CBC(*data);
		CVMX_MF_3DES_RESULT(*data);
		crypt_len -= 8;
	    }
	} else
	    crypt_off -= 8;
    	if (auth_off <= 0) {
	    if (auth_len > 0) {
		CVM_LOAD_SHA_UNIT(*data, next);
		auth_len -= 8;
	    }
	} else
	    auth_off -= 8;
	*first = mydata.data32[0];
	*data32 = mydata.data32[1];
	SG_CONSUME(sg, data32, data_i, data_l);
    }

    /* finish the hash */
    CVMX_PREFETCH0(od->octo_hmouter);
#if 0
    if (unlikely(inplen)) {
	uint64_t tmp = 0;
	uint8_t *p = (uint8_t *) & tmp;
	p[inplen] = 0x80;
	do {
	    inplen--;
	    p[inplen] = ((uint8_t *) data)[inplen];
	} while (inplen);
	CVM_LOAD_SHA_UNIT(tmp, next);
    } else {
	CVM_LOAD_SHA_UNIT(0x8000000000000000ULL, next);
    }
#else
    CVM_LOAD_SHA_UNIT(0x8000000000000000ULL, next);
#endif

    /* Finish Inner hash */
    while (next != 7) {
	CVM_LOAD_SHA_UNIT(((uint64_t) 0x0ULL), next);
    }
	CVM_LOAD_SHA_UNIT((uint64_t) ((alen + 64) << 3), next);

    /* Get the inner hash of HMAC */
    CVMX_MF_HSH_IV(tmp1, 0);
    CVMX_MF_HSH_IV(tmp2, 1);
    tmp3 = 0;
    CVMX_MF_HSH_IV(tmp3, 2);

    /* Initialize hash unit */
    CVMX_MT_HSH_IV(od->octo_hmouter[0], 0);
    CVMX_MT_HSH_IV(od->octo_hmouter[1], 1);
    CVMX_MT_HSH_IV(od->octo_hmouter[2], 2);

    CVMX_MT_HSH_DAT(tmp1, 0);
    CVMX_MT_HSH_DAT(tmp2, 1);
    tmp3 |= 0x0000000080000000;
    CVMX_MT_HSH_DAT(tmp3, 2);
    CVMX_MT_HSH_DATZ(3);
    CVMX_MT_HSH_DATZ(4);
    CVMX_MT_HSH_DATZ(5);
    CVMX_MT_HSH_DATZ(6);
    CVMX_MT_HSH_STARTSHA((uint64_t) ((64 + 20) << 3));

    /* save the HMAC */
    SG_INIT(sg, data32, data_i, data_l);
    while (icv_off > 0) {
	SG_CONSUME(sg, data32, data_i, data_l);
	icv_off -= 4;
    }
    CVMX_MF_HSH_IV(tmp1, 0);
    *data32 = (uint32_t) (tmp1 >> 32);
    SG_CONSUME(sg, data32, data_i, data_l);
    *data32 = (uint32_t) tmp1;
    SG_CONSUME(sg, data32, data_i, data_l);
    CVMX_MF_HSH_IV(tmp1, 1);
    *data32 = (uint32_t) (tmp1 >> 32);

    octeon_crypto_disable(&state, flags);
    return 0;
}

int
octo_des_cbc_sha1_decrypt(
    struct octo_sess *od,
    struct scatterlist *sg, int sg_len,
    int auth_off, int auth_len,
    int crypt_off, int crypt_len,
    int icv_off, uint8_t *ivp)
{
    register int next = 0;
    union {
	uint32_t data32[2];
	uint64_t data64[1];
    } mydata;
    uint64_t *data = &mydata.data64[0];
    uint32_t *data32;
    uint64_t tmp1, tmp2, tmp3;
    int data_i, data_l, alen = auth_len;
    struct octeon_cop2_state state;
    unsigned long flags;

    dprintk("%s()\n", __FUNCTION__);

    if (unlikely(od == NULL || sg==NULL || sg_len==0 || ivp==NULL ||
	    (crypt_off & 0x3) || (crypt_off + crypt_len > sg_len) ||
	    (crypt_len  & 0x7) ||
	    (auth_len  & 0x7) ||
	    (auth_off & 0x3) || (auth_off + auth_len > sg_len))) {
	dprintk("%s: Bad parameters od=%p sg=%p sg_len=%d "
		"auth_off=%d auth_len=%d crypt_off=%d crypt_len=%d "
		"icv_off=%d ivp=%p\n", __FUNCTION__, od, sg, sg_len,
		auth_off, auth_len, crypt_off, crypt_len, icv_off, ivp);
	return -EINVAL;
    }

    SG_INIT(sg, data32, data_i, data_l);

    CVMX_PREFETCH0(ivp);
    CVMX_PREFETCH0(od->octo_enckey);

    flags = octeon_crypto_enable(&state);

    /* load 3DES Key */
    CVMX_MT_3DES_KEY(((uint64_t *) od->octo_enckey)[0], 0);
    if (od->octo_encklen == 24) {
	CVMX_MT_3DES_KEY(((uint64_t *) od->octo_enckey)[1], 1);
	CVMX_MT_3DES_KEY(((uint64_t *) od->octo_enckey)[2], 2);
    } else if (od->octo_encklen == 8) {
	CVMX_MT_3DES_KEY(((uint64_t *) od->octo_enckey)[0], 1);
	CVMX_MT_3DES_KEY(((uint64_t *) od->octo_enckey)[0], 2);
    } else {
	octeon_crypto_disable(&state, flags);
	dprintk("%s: Bad key length %d\n", __FUNCTION__, od->octo_encklen);
	return -EINVAL;
    }

    CVMX_MT_3DES_IV(* (uint64_t *) ivp);

    /* Load SHA1 IV */
    CVMX_MT_HSH_IV(od->octo_hminner[0], 0);
    CVMX_MT_HSH_IV(od->octo_hminner[1], 1);
    CVMX_MT_HSH_IV(od->octo_hminner[2], 2);

    while (crypt_off > 0 && auth_off > 0) {
	SG_CONSUME(sg, data32, data_i, data_l);
	crypt_off -= 4;
	auth_off -= 4;
    }

    while (crypt_len > 0 || auth_len > 0) {
    	uint32_t *first = data32;
	mydata.data32[0] = *first;
	SG_CONSUME(sg, data32, data_i, data_l);
	mydata.data32[1] = *data32;
    	if (auth_off <= 0) {
	    if (auth_len > 0) {
		CVM_LOAD_SHA_UNIT(*data, next);
		auth_len -= 8;
	    }
	} else
	    auth_off -= 8;
    	if (crypt_off <= 0) {
	    if (crypt_len > 0) {
		CVMX_MT_3DES_DEC_CBC(*data);
		CVMX_MF_3DES_RESULT(*data);
		crypt_len -= 8;
	    }
	} else
	    crypt_off -= 8;
	*first = mydata.data32[0];
	*data32 = mydata.data32[1];
	SG_CONSUME(sg, data32, data_i, data_l);
    }

    /* finish the hash */
    CVMX_PREFETCH0(od->octo_hmouter);
#if 0
    if (unlikely(inplen)) {
	uint64_t tmp = 0;
	uint8_t *p = (uint8_t *) & tmp;
	p[inplen] = 0x80;
	do {
	    inplen--;
	    p[inplen] = ((uint8_t *) data)[inplen];
	} while (inplen);
	CVM_LOAD_SHA_UNIT(tmp, next);
    } else {
	CVM_LOAD_SHA_UNIT(0x8000000000000000ULL, next);
    }
#else
    CVM_LOAD_SHA_UNIT(0x8000000000000000ULL, next);
#endif

    /* Finish Inner hash */
    while (next != 7) {
	CVM_LOAD_SHA_UNIT(((uint64_t) 0x0ULL), next);
    }
    CVM_LOAD_SHA_UNIT((uint64_t) ((alen + 64) << 3), next);

    /* Get the inner hash of HMAC */
    CVMX_MF_HSH_IV(tmp1, 0);
    CVMX_MF_HSH_IV(tmp2, 1);
    tmp3 = 0;
    CVMX_MF_HSH_IV(tmp3, 2);

    /* Initialize hash unit */
    CVMX_MT_HSH_IV(od->octo_hmouter[0], 0);
    CVMX_MT_HSH_IV(od->octo_hmouter[1], 1);
    CVMX_MT_HSH_IV(od->octo_hmouter[2], 2);

    CVMX_MT_HSH_DAT(tmp1, 0);
    CVMX_MT_HSH_DAT(tmp2, 1);
    tmp3 |= 0x0000000080000000;
    CVMX_MT_HSH_DAT(tmp3, 2);
    CVMX_MT_HSH_DATZ(3);
    CVMX_MT_HSH_DATZ(4);
    CVMX_MT_HSH_DATZ(5);
    CVMX_MT_HSH_DATZ(6);
    CVMX_MT_HSH_STARTSHA((uint64_t) ((64 + 20) << 3));
    /* save the HMAC */
    SG_INIT(sg, data32, data_i, data_l);
    while (icv_off > 0) {
	SG_CONSUME(sg, data32, data_i, data_l);
	icv_off -= 4;
    }
    CVMX_MF_HSH_IV(tmp1, 0);
    *data32 = (uint32_t) (tmp1 >> 32);
    SG_CONSUME(sg, data32, data_i, data_l);
    *data32 = (uint32_t) tmp1;
    SG_CONSUME(sg, data32, data_i, data_l);
    CVMX_MF_HSH_IV(tmp1, 1);
    *data32 = (uint32_t) (tmp1 >> 32);

    octeon_crypto_disable(&state, flags);
    return 0;
}

/****************************************************************************/
/* AES MD5 */

int
octo_aes_cbc_md5_encrypt(
    struct octo_sess *od,
    struct scatterlist *sg, int sg_len,
    int auth_off, int auth_len,
    int crypt_off, int crypt_len,
    int icv_off, uint8_t *ivp)
{
    register int next = 0;
    union {
	uint32_t data32[2];
	uint64_t data64[1];
    } mydata[2];
    uint64_t *pdata = &mydata[0].data64[0];
    uint64_t *data =  &mydata[1].data64[0];
    uint32_t *data32;
    uint64_t tmp1, tmp2;
    int data_i, data_l, alen = auth_len;
    struct octeon_cop2_state state;
    unsigned long flags;

    dprintk("%s()\n", __FUNCTION__);

    if (unlikely(od == NULL || sg==NULL || sg_len==0 || ivp==NULL ||
	    (crypt_off & 0x3) || (crypt_off + crypt_len > sg_len) ||
	    (crypt_len  & 0x7) ||
	    (auth_len  & 0x7) ||
	    (auth_off & 0x3) || (auth_off + auth_len > sg_len))) {
	dprintk("%s: Bad parameters od=%p sg=%p sg_len=%d "
		"auth_off=%d auth_len=%d crypt_off=%d crypt_len=%d "
		"icv_off=%d ivp=%p\n", __FUNCTION__, od, sg, sg_len,
		auth_off, auth_len, crypt_off, crypt_len, icv_off, ivp);
	return -EINVAL;
    }

    SG_INIT(sg, data32, data_i, data_l);

    CVMX_PREFETCH0(ivp);
    CVMX_PREFETCH0(od->octo_enckey);

    flags = octeon_crypto_enable(&state);

    /* load AES Key */
    CVMX_MT_AES_KEY(((uint64_t *) od->octo_enckey)[0], 0);
    CVMX_MT_AES_KEY(((uint64_t *) od->octo_enckey)[1], 1);

    if (od->octo_encklen == 16) {
	CVMX_MT_AES_KEY(0x0, 2);
	CVMX_MT_AES_KEY(0x0, 3);
    } else if (od->octo_encklen == 24) {
	CVMX_MT_AES_KEY(((uint64_t *) od->octo_enckey)[2], 2);
	CVMX_MT_AES_KEY(0x0, 3);
    } else if (od->octo_encklen == 32) {
	CVMX_MT_AES_KEY(((uint64_t *) od->octo_enckey)[2], 2);
	CVMX_MT_AES_KEY(((uint64_t *) od->octo_enckey)[3], 3);
    } else {
	octeon_crypto_disable(&state, flags);
	dprintk("%s: Bad key length %d\n", __FUNCTION__, od->octo_encklen);
	return -EINVAL;
    }
    CVMX_MT_AES_KEYLENGTH(od->octo_encklen / 8 - 1);

    CVMX_MT_AES_IV(((uint64_t *) ivp)[0], 0);
    CVMX_MT_AES_IV(((uint64_t *) ivp)[1], 1);

    /* Load MD5 IV */
    CVMX_MT_HSH_IV(od->octo_hminner[0], 0);
    CVMX_MT_HSH_IV(od->octo_hminner[1], 1);

    while (crypt_off > 0 && auth_off > 0) {
	SG_CONSUME(sg, data32, data_i, data_l);
	crypt_off -= 4;
	auth_off -= 4;
    }

    /* align auth and crypt */
    while (crypt_off > 0 && auth_len > 0) {
	mydata[0].data32[0] = *data32;
	SG_CONSUME(sg, data32, data_i, data_l);
	mydata[0].data32[1] = *data32;
	SG_CONSUME(sg, data32, data_i, data_l);
	CVM_LOAD_MD5_UNIT(*pdata, next);
	crypt_off -= 8;
	auth_len -= 8;
    }

    while (crypt_len > 0) {
    	uint32_t *pdata32[3];

	pdata32[0] = data32;
	mydata[0].data32[0] = *data32;
	SG_CONSUME(sg, data32, data_i, data_l);

	pdata32[1] = data32;
	mydata[0].data32[1] = *data32;
	SG_CONSUME(sg, data32, data_i, data_l);

	pdata32[2] = data32;
	mydata[1].data32[0] = *data32;
	SG_CONSUME(sg, data32, data_i, data_l);

	mydata[1].data32[1] = *data32;

	CVMX_MT_AES_ENC_CBC0(*pdata);
	CVMX_MT_AES_ENC_CBC1(*data);
	CVMX_MF_AES_RESULT(*pdata, 0);
	CVMX_MF_AES_RESULT(*data, 1);
	crypt_len -= 16;

	if (auth_len > 0) {
	    CVM_LOAD_MD5_UNIT(*pdata, next);
	    auth_len -= 8;
	}
	if (auth_len > 0) {
	    CVM_LOAD_MD5_UNIT(*data, next);
	    auth_len -= 8;
	}

	*pdata32[0] = mydata[0].data32[0];
	*pdata32[1] = mydata[0].data32[1];
	*pdata32[2] = mydata[1].data32[0];
	*data32     = mydata[1].data32[1];

	SG_CONSUME(sg, data32, data_i, data_l);
    }

    /* finish any left over hashing */
    while (auth_len > 0) {
	mydata[0].data32[0] = *data32;
	SG_CONSUME(sg, data32, data_i, data_l);
	mydata[0].data32[1] = *data32;
	SG_CONSUME(sg, data32, data_i, data_l);
	CVM_LOAD_MD5_UNIT(*pdata, next);
	auth_len -= 8;
    }

    /* finish the hash */
    CVMX_PREFETCH0(od->octo_hmouter);
#if 0
    if (unlikely(inplen)) {
	uint64_t tmp = 0;
	uint8_t *p = (uint8_t *) & tmp;
	p[inplen] = 0x80;
	do {
	    inplen--;
	    p[inplen] = ((uint8_t *) data)[inplen];
	} while (inplen);
	CVM_LOAD_MD5_UNIT(tmp, next);
    } else {
	CVM_LOAD_MD5_UNIT(0x8000000000000000ULL, next);
    }
#else
    CVM_LOAD_MD5_UNIT(0x8000000000000000ULL, next);
#endif

    /* Finish Inner hash */
    while (next != 7) {
	CVM_LOAD_MD5_UNIT(((uint64_t) 0x0ULL), next);
    }
    CVMX_ES64(tmp1, ((alen + 64) << 3));
    CVM_LOAD_MD5_UNIT(tmp1, next);

    /* Get the inner hash of HMAC */
    CVMX_MF_HSH_IV(tmp1, 0);
    CVMX_MF_HSH_IV(tmp2, 1);

    /* Initialize hash unit */
    CVMX_MT_HSH_IV(od->octo_hmouter[0], 0);
    CVMX_MT_HSH_IV(od->octo_hmouter[1], 1);

    CVMX_MT_HSH_DAT(tmp1, 0);
    CVMX_MT_HSH_DAT(tmp2, 1);
    CVMX_MT_HSH_DAT(0x8000000000000000ULL, 2);
    CVMX_MT_HSH_DATZ(3);
    CVMX_MT_HSH_DATZ(4);
    CVMX_MT_HSH_DATZ(5);
    CVMX_MT_HSH_DATZ(6);
    CVMX_ES64(tmp1, ((64 + 16) << 3));
    CVMX_MT_HSH_STARTMD5(tmp1);

    /* save the HMAC */
    SG_INIT(sg, data32, data_i, data_l);
    while (icv_off > 0) {
	SG_CONSUME(sg, data32, data_i, data_l);
	icv_off -= 4;
    }
    CVMX_MF_HSH_IV(tmp1, 0);
    *data32 = (uint32_t) (tmp1 >> 32);
    SG_CONSUME(sg, data32, data_i, data_l);
    *data32 = (uint32_t) tmp1;
    SG_CONSUME(sg, data32, data_i, data_l);
    CVMX_MF_HSH_IV(tmp1, 1);
    *data32 = (uint32_t) (tmp1 >> 32);

    octeon_crypto_disable(&state, flags);
    return 0;
}

int
octo_aes_cbc_md5_decrypt(
    struct octo_sess *od,
    struct scatterlist *sg, int sg_len,
    int auth_off, int auth_len,
    int crypt_off, int crypt_len,
    int icv_off, uint8_t *ivp)
{
    register int next = 0;
    union {
	uint32_t data32[2];
	uint64_t data64[1];
    } mydata[2];
    uint64_t *pdata = &mydata[0].data64[0];
    uint64_t *data =  &mydata[1].data64[0];
    uint32_t *data32;
    uint64_t tmp1, tmp2;
    int data_i, data_l, alen = auth_len;
    struct octeon_cop2_state state;
    unsigned long flags;

    dprintk("%s()\n", __FUNCTION__);

    if (unlikely(od == NULL || sg==NULL || sg_len==0 || ivp==NULL ||
	    (crypt_off & 0x3) || (crypt_off + crypt_len > sg_len) ||
	    (crypt_len  & 0x7) ||
	    (auth_len  & 0x7) ||
	    (auth_off & 0x3) || (auth_off + auth_len > sg_len))) {
	dprintk("%s: Bad parameters od=%p sg=%p sg_len=%d "
		"auth_off=%d auth_len=%d crypt_off=%d crypt_len=%d "
		"icv_off=%d ivp=%p\n", __FUNCTION__, od, sg, sg_len,
		auth_off, auth_len, crypt_off, crypt_len, icv_off, ivp);
	return -EINVAL;
    }

    SG_INIT(sg, data32, data_i, data_l);

    CVMX_PREFETCH0(ivp);
    CVMX_PREFETCH0(od->octo_enckey);

    flags = octeon_crypto_enable(&state);

    /* load AES Key */
    CVMX_MT_AES_KEY(((uint64_t *) od->octo_enckey)[0], 0);
    CVMX_MT_AES_KEY(((uint64_t *) od->octo_enckey)[1], 1);

    if (od->octo_encklen == 16) {
	CVMX_MT_AES_KEY(0x0, 2);
	CVMX_MT_AES_KEY(0x0, 3);
    } else if (od->octo_encklen == 24) {
	CVMX_MT_AES_KEY(((uint64_t *) od->octo_enckey)[2], 2);
	CVMX_MT_AES_KEY(0x0, 3);
    } else if (od->octo_encklen == 32) {
	CVMX_MT_AES_KEY(((uint64_t *) od->octo_enckey)[2], 2);
	CVMX_MT_AES_KEY(((uint64_t *) od->octo_enckey)[3], 3);
    } else {
	octeon_crypto_disable(&state, flags);
	dprintk("%s: Bad key length %d\n", __FUNCTION__, od->octo_encklen);
	return -EINVAL;
    }
    CVMX_MT_AES_KEYLENGTH(od->octo_encklen / 8 - 1);

    CVMX_MT_AES_IV(((uint64_t *) ivp)[0], 0);
    CVMX_MT_AES_IV(((uint64_t *) ivp)[1], 1);

    /* Load MD5 IV */
    CVMX_MT_HSH_IV(od->octo_hminner[0], 0);
    CVMX_MT_HSH_IV(od->octo_hminner[1], 1);

    while (crypt_off > 0 && auth_off > 0) {
	SG_CONSUME(sg, data32, data_i, data_l);
	crypt_off -= 4;
	auth_off -= 4;
    }

    /* align auth and crypt */
    while (crypt_off > 0 && auth_len > 0) {
	mydata[0].data32[0] = *data32;
	SG_CONSUME(sg, data32, data_i, data_l);
	mydata[0].data32[1] = *data32;
	SG_CONSUME(sg, data32, data_i, data_l);
	CVM_LOAD_MD5_UNIT(*pdata, next);
	crypt_off -= 8;
	auth_len -= 8;
    }

    while (crypt_len > 0) {
    	uint32_t *pdata32[3];

	pdata32[0] = data32;
	mydata[0].data32[0] = *data32;
	SG_CONSUME(sg, data32, data_i, data_l);
	pdata32[1] = data32;
	mydata[0].data32[1] = *data32;
	SG_CONSUME(sg, data32, data_i, data_l);
	pdata32[2] = data32;
	mydata[1].data32[0] = *data32;
	SG_CONSUME(sg, data32, data_i, data_l);
	mydata[1].data32[1] = *data32;

	if (auth_len > 0) {
	    CVM_LOAD_MD5_UNIT(*pdata, next);
	    auth_len -= 8;
	}

	if (auth_len > 0) {
	    CVM_LOAD_MD5_UNIT(*data, next);
	    auth_len -= 8;
	}

	CVMX_MT_AES_DEC_CBC0(*pdata);
	CVMX_MT_AES_DEC_CBC1(*data);
	CVMX_MF_AES_RESULT(*pdata, 0);
	CVMX_MF_AES_RESULT(*data, 1);
	crypt_len -= 16;

	*pdata32[0] = mydata[0].data32[0];
	*pdata32[1] = mydata[0].data32[1];
	*pdata32[2] = mydata[1].data32[0];
	*data32     = mydata[1].data32[1];

	SG_CONSUME(sg, data32, data_i, data_l);
    }

    /* finish left over hash if any */
    while (auth_len > 0) {
	mydata[0].data32[0] = *data32;
	SG_CONSUME(sg, data32, data_i, data_l);
	mydata[0].data32[1] = *data32;
	SG_CONSUME(sg, data32, data_i, data_l);
	CVM_LOAD_MD5_UNIT(*pdata, next);
	auth_len -= 8;
    }


    /* finish the hash */
    CVMX_PREFETCH0(od->octo_hmouter);
#if 0
    if (unlikely(inplen)) {
	uint64_t tmp = 0;
	uint8_t *p = (uint8_t *) & tmp;
	p[inplen] = 0x80;
	do {
	    inplen--;
	    p[inplen] = ((uint8_t *) data)[inplen];
	} while (inplen);
	CVM_LOAD_MD5_UNIT(tmp, next);
    } else {
	CVM_LOAD_MD5_UNIT(0x8000000000000000ULL, next);
    }
#else
    CVM_LOAD_MD5_UNIT(0x8000000000000000ULL, next);
#endif

    /* Finish Inner hash */
    while (next != 7) {
	CVM_LOAD_MD5_UNIT(((uint64_t) 0x0ULL), next);
    }
    CVMX_ES64(tmp1, ((alen + 64) << 3));
    CVM_LOAD_MD5_UNIT(tmp1, next);

    /* Get the inner hash of HMAC */
    CVMX_MF_HSH_IV(tmp1, 0);
    CVMX_MF_HSH_IV(tmp2, 1);

    /* Initialize hash unit */
    CVMX_MT_HSH_IV(od->octo_hmouter[0], 0);
    CVMX_MT_HSH_IV(od->octo_hmouter[1], 1);

    CVMX_MT_HSH_DAT(tmp1, 0);
    CVMX_MT_HSH_DAT(tmp2, 1);
    CVMX_MT_HSH_DAT(0x8000000000000000ULL, 2);
    CVMX_MT_HSH_DATZ(3);
    CVMX_MT_HSH_DATZ(4);
    CVMX_MT_HSH_DATZ(5);
    CVMX_MT_HSH_DATZ(6);
    CVMX_ES64(tmp1, ((64 + 16) << 3));
    CVMX_MT_HSH_STARTMD5(tmp1);

    /* save the HMAC */
    SG_INIT(sg, data32, data_i, data_l);
    while (icv_off > 0) {
	SG_CONSUME(sg, data32, data_i, data_l);
	icv_off -= 4;
    }
    CVMX_MF_HSH_IV(tmp1, 0);
    *data32 = (uint32_t) (tmp1 >> 32);
    SG_CONSUME(sg, data32, data_i, data_l);
    *data32 = (uint32_t) tmp1;
    SG_CONSUME(sg, data32, data_i, data_l);
    CVMX_MF_HSH_IV(tmp1, 1);
    *data32 = (uint32_t) (tmp1 >> 32);

    octeon_crypto_disable(&state, flags);
    return 0;
}

/****************************************************************************/
/* AES SHA1 */

int
octo_aes_cbc_sha1_encrypt(
    struct octo_sess *od,
    struct scatterlist *sg, int sg_len,
    int auth_off, int auth_len,
    int crypt_off, int crypt_len,
    int icv_off, uint8_t *ivp)
{
    register int next = 0;
    union {
	uint32_t data32[2];
	uint64_t data64[1];
    } mydata[2];
    uint64_t *pdata = &mydata[0].data64[0];
    uint64_t *data =  &mydata[1].data64[0];
    uint32_t *data32;
    uint64_t tmp1, tmp2, tmp3;
    int data_i, data_l, alen = auth_len;
    struct octeon_cop2_state state;
    unsigned long flags;

    dprintk("%s(a_off=%d a_len=%d c_off=%d c_len=%d icv_off=%d)\n",
			__FUNCTION__, auth_off, auth_len, crypt_off, crypt_len, icv_off);

    if (unlikely(od == NULL || sg==NULL || sg_len==0 || ivp==NULL ||
	    (crypt_off & 0x3) || (crypt_off + crypt_len > sg_len) ||
	    (crypt_len  & 0x7) ||
	    (auth_len  & 0x7) ||
	    (auth_off & 0x3) || (auth_off + auth_len > sg_len))) {
	dprintk("%s: Bad parameters od=%p sg=%p sg_len=%d "
		"auth_off=%d auth_len=%d crypt_off=%d crypt_len=%d "
		"icv_off=%d ivp=%p\n", __FUNCTION__, od, sg, sg_len,
		auth_off, auth_len, crypt_off, crypt_len, icv_off, ivp);
	return -EINVAL;
    }

    SG_INIT(sg, data32, data_i, data_l);

    CVMX_PREFETCH0(ivp);
    CVMX_PREFETCH0(od->octo_enckey);

    flags = octeon_crypto_enable(&state);

    /* load AES Key */
    CVMX_MT_AES_KEY(((uint64_t *) od->octo_enckey)[0], 0);
    CVMX_MT_AES_KEY(((uint64_t *) od->octo_enckey)[1], 1);

    if (od->octo_encklen == 16) {
	CVMX_MT_AES_KEY(0x0, 2);
	CVMX_MT_AES_KEY(0x0, 3);
    } else if (od->octo_encklen == 24) {
	CVMX_MT_AES_KEY(((uint64_t *) od->octo_enckey)[2], 2);
	CVMX_MT_AES_KEY(0x0, 3);
    } else if (od->octo_encklen == 32) {
	CVMX_MT_AES_KEY(((uint64_t *) od->octo_enckey)[2], 2);
	CVMX_MT_AES_KEY(((uint64_t *) od->octo_enckey)[3], 3);
    } else {
	octeon_crypto_disable(&state, flags);
	dprintk("%s: Bad key length %d\n", __FUNCTION__, od->octo_encklen);
	return -EINVAL;
    }
    CVMX_MT_AES_KEYLENGTH(od->octo_encklen / 8 - 1);

    CVMX_MT_AES_IV(((uint64_t *) ivp)[0], 0);
    CVMX_MT_AES_IV(((uint64_t *) ivp)[1], 1);

    /* Load SHA IV */
    CVMX_MT_HSH_IV(od->octo_hminner[0], 0);
    CVMX_MT_HSH_IV(od->octo_hminner[1], 1);
    CVMX_MT_HSH_IV(od->octo_hminner[2], 2);

    while (crypt_off > 0 && auth_off > 0) {
	SG_CONSUME(sg, data32, data_i, data_l);
	crypt_off -= 4;
	auth_off -= 4;
    }

    /* align auth and crypt */
    while (crypt_off > 0 && auth_len > 0) {
	mydata[0].data32[0] = *data32;
	SG_CONSUME(sg, data32, data_i, data_l);
	mydata[0].data32[1] = *data32;
	SG_CONSUME(sg, data32, data_i, data_l);
	CVM_LOAD_SHA_UNIT(*pdata, next);
	crypt_off -= 8;
	auth_len -= 8;
    }

    while (crypt_len > 0) {
    	uint32_t *pdata32[3];

	pdata32[0] = data32;
	mydata[0].data32[0] = *data32;
	SG_CONSUME(sg, data32, data_i, data_l);
	pdata32[1] = data32;
	mydata[0].data32[1] = *data32;
	SG_CONSUME(sg, data32, data_i, data_l);
	pdata32[2] = data32;
	mydata[1].data32[0] = *data32;
	SG_CONSUME(sg, data32, data_i, data_l);
	mydata[1].data32[1] = *data32;

	CVMX_MT_AES_ENC_CBC0(*pdata);
	CVMX_MT_AES_ENC_CBC1(*data);
	CVMX_MF_AES_RESULT(*pdata, 0);
	CVMX_MF_AES_RESULT(*data, 1);
	crypt_len -= 16;

	if (auth_len > 0) {
	    CVM_LOAD_SHA_UNIT(*pdata, next);
	    auth_len -= 8;
	}
	if (auth_len > 0) {
	    CVM_LOAD_SHA_UNIT(*data, next);
	    auth_len -= 8;
	}

	*pdata32[0] = mydata[0].data32[0];
	*pdata32[1] = mydata[0].data32[1];
	*pdata32[2] = mydata[1].data32[0];
	*data32     = mydata[1].data32[1];

	SG_CONSUME(sg, data32, data_i, data_l);
    }

    /* finish and hashing */
    while (auth_len > 0) {
	mydata[0].data32[0] = *data32;
	SG_CONSUME(sg, data32, data_i, data_l);
	mydata[0].data32[1] = *data32;
	SG_CONSUME(sg, data32, data_i, data_l);
	CVM_LOAD_SHA_UNIT(*pdata, next);
	auth_len -= 8;
    }

    /* finish the hash */
    CVMX_PREFETCH0(od->octo_hmouter);
#if 0
    if (unlikely(inplen)) {
	uint64_t tmp = 0;
	uint8_t *p = (uint8_t *) & tmp;
	p[inplen] = 0x80;
	do {
	    inplen--;
	    p[inplen] = ((uint8_t *) data)[inplen];
	} while (inplen);
	CVM_LOAD_SHA_UNIT(tmp, next);
    } else {
	CVM_LOAD_SHA_UNIT(0x8000000000000000ULL, next);
    }
#else
    CVM_LOAD_SHA_UNIT(0x8000000000000000ULL, next);
#endif

    /* Finish Inner hash */
    while (next != 7) {
	CVM_LOAD_SHA_UNIT(((uint64_t) 0x0ULL), next);
    }
    CVM_LOAD_SHA_UNIT((uint64_t) ((alen + 64) << 3), next);

    /* Get the inner hash of HMAC */
    CVMX_MF_HSH_IV(tmp1, 0);
    CVMX_MF_HSH_IV(tmp2, 1);
    tmp3 = 0;
    CVMX_MF_HSH_IV(tmp3, 2);

    /* Initialize hash unit */
    CVMX_MT_HSH_IV(od->octo_hmouter[0], 0);
    CVMX_MT_HSH_IV(od->octo_hmouter[1], 1);
    CVMX_MT_HSH_IV(od->octo_hmouter[2], 2);

    CVMX_MT_HSH_DAT(tmp1, 0);
    CVMX_MT_HSH_DAT(tmp2, 1);
    tmp3 |= 0x0000000080000000;
    CVMX_MT_HSH_DAT(tmp3, 2);
    CVMX_MT_HSH_DATZ(3);
    CVMX_MT_HSH_DATZ(4);
    CVMX_MT_HSH_DATZ(5);
    CVMX_MT_HSH_DATZ(6);
    CVMX_MT_HSH_STARTSHA((uint64_t) ((64 + 20) << 3));

    /* finish the hash */
    CVMX_PREFETCH0(od->octo_hmouter);
#if 0
    if (unlikely(inplen)) {
	uint64_t tmp = 0;
	uint8_t *p = (uint8_t *) & tmp;
	p[inplen] = 0x80;
	do {
	    inplen--;
	    p[inplen] = ((uint8_t *) data)[inplen];
	} while (inplen);
	CVM_LOAD_MD5_UNIT(tmp, next);
    } else {
	CVM_LOAD_MD5_UNIT(0x8000000000000000ULL, next);
    }
#else
    CVM_LOAD_MD5_UNIT(0x8000000000000000ULL, next);
#endif

    /* save the HMAC */
    SG_INIT(sg, data32, data_i, data_l);
    while (icv_off > 0) {
	SG_CONSUME(sg, data32, data_i, data_l);
	icv_off -= 4;
    }
    CVMX_MF_HSH_IV(tmp1, 0);
    *data32 = (uint32_t) (tmp1 >> 32);
    SG_CONSUME(sg, data32, data_i, data_l);
    *data32 = (uint32_t) tmp1;
    SG_CONSUME(sg, data32, data_i, data_l);
    CVMX_MF_HSH_IV(tmp1, 1);
    *data32 = (uint32_t) (tmp1 >> 32);

    octeon_crypto_disable(&state, flags);
    return 0;
}

int
octo_aes_cbc_sha1_decrypt(
    struct octo_sess *od,
    struct scatterlist *sg, int sg_len,
    int auth_off, int auth_len,
    int crypt_off, int crypt_len,
    int icv_off, uint8_t *ivp)
{
    register int next = 0;
    union {
	uint32_t data32[2];
	uint64_t data64[1];
    } mydata[2];
    uint64_t *pdata = &mydata[0].data64[0];
    uint64_t *data =  &mydata[1].data64[0];
    uint32_t *data32;
    uint64_t tmp1, tmp2, tmp3;
    int data_i, data_l, alen = auth_len;
    struct octeon_cop2_state state;
    unsigned long flags;

    dprintk("%s(a_off=%d a_len=%d c_off=%d c_len=%d icv_off=%d)\n",
			__FUNCTION__, auth_off, auth_len, crypt_off, crypt_len, icv_off);

    if (unlikely(od == NULL || sg==NULL || sg_len==0 || ivp==NULL ||
	    (crypt_off & 0x3) || (crypt_off + crypt_len > sg_len) ||
	    (crypt_len  & 0x7) ||
	    (auth_len  & 0x7) ||
	    (auth_off & 0x3) || (auth_off + auth_len > sg_len))) {
	dprintk("%s: Bad parameters od=%p sg=%p sg_len=%d "
		"auth_off=%d auth_len=%d crypt_off=%d crypt_len=%d "
		"icv_off=%d ivp=%p\n", __FUNCTION__, od, sg, sg_len,
		auth_off, auth_len, crypt_off, crypt_len, icv_off, ivp);
	return -EINVAL;
    }

    SG_INIT(sg, data32, data_i, data_l);

    CVMX_PREFETCH0(ivp);
    CVMX_PREFETCH0(od->octo_enckey);

    flags = octeon_crypto_enable(&state);

    /* load AES Key */
    CVMX_MT_AES_KEY(((uint64_t *) od->octo_enckey)[0], 0);
    CVMX_MT_AES_KEY(((uint64_t *) od->octo_enckey)[1], 1);

    if (od->octo_encklen == 16) {
	CVMX_MT_AES_KEY(0x0, 2);
	CVMX_MT_AES_KEY(0x0, 3);
    } else if (od->octo_encklen == 24) {
	CVMX_MT_AES_KEY(((uint64_t *) od->octo_enckey)[2], 2);
	CVMX_MT_AES_KEY(0x0, 3);
    } else if (od->octo_encklen == 32) {
	CVMX_MT_AES_KEY(((uint64_t *) od->octo_enckey)[2], 2);
	CVMX_MT_AES_KEY(((uint64_t *) od->octo_enckey)[3], 3);
    } else {
	octeon_crypto_disable(&state, flags);
	dprintk("%s: Bad key length %d\n", __FUNCTION__, od->octo_encklen);
	return -EINVAL;
    }
    CVMX_MT_AES_KEYLENGTH(od->octo_encklen / 8 - 1);

    CVMX_MT_AES_IV(((uint64_t *) ivp)[0], 0);
    CVMX_MT_AES_IV(((uint64_t *) ivp)[1], 1);

    /* Load SHA1 IV */
    CVMX_MT_HSH_IV(od->octo_hminner[0], 0);
    CVMX_MT_HSH_IV(od->octo_hminner[1], 1);
    CVMX_MT_HSH_IV(od->octo_hminner[2], 2);

    while (crypt_off > 0 && auth_off > 0) {
	SG_CONSUME(sg, data32, data_i, data_l);
	crypt_off -= 4;
	auth_off -= 4;
    }

    /* align auth and crypt */
    while (crypt_off > 0 && auth_len > 0) {
	mydata[0].data32[0] = *data32;
	SG_CONSUME(sg, data32, data_i, data_l);
	mydata[0].data32[1] = *data32;
	SG_CONSUME(sg, data32, data_i, data_l);
	CVM_LOAD_SHA_UNIT(*pdata, next);
	crypt_off -= 8;
	auth_len -= 8;
    }

    while (crypt_len > 0) {
    	uint32_t *pdata32[3];

	pdata32[0] = data32;
	mydata[0].data32[0] = *data32;
	SG_CONSUME(sg, data32, data_i, data_l);
	pdata32[1] = data32;
	mydata[0].data32[1] = *data32;
	SG_CONSUME(sg, data32, data_i, data_l);
	pdata32[2] = data32;
	mydata[1].data32[0] = *data32;
	SG_CONSUME(sg, data32, data_i, data_l);
	mydata[1].data32[1] = *data32;

	if (auth_len > 0) {
	    CVM_LOAD_SHA_UNIT(*pdata, next);
	    auth_len -= 8;
	}
	if (auth_len > 0) {
	    CVM_LOAD_SHA_UNIT(*data, next);
	    auth_len -= 8;
	}

	CVMX_MT_AES_DEC_CBC0(*pdata);
	CVMX_MT_AES_DEC_CBC1(*data);
	CVMX_MF_AES_RESULT(*pdata, 0);
	CVMX_MF_AES_RESULT(*data, 1);
	crypt_len -= 16;

	*pdata32[0] = mydata[0].data32[0];
	*pdata32[1] = mydata[0].data32[1];
	*pdata32[2] = mydata[1].data32[0];
	*data32     = mydata[1].data32[1];

	SG_CONSUME(sg, data32, data_i, data_l);
    }

    /* finish and leftover hashing */
    while (auth_len > 0) {
	mydata[0].data32[0] = *data32;
	SG_CONSUME(sg, data32, data_i, data_l);
	mydata[0].data32[1] = *data32;
	SG_CONSUME(sg, data32, data_i, data_l);
	CVM_LOAD_SHA_UNIT(*pdata, next);
	auth_len -= 8;
    }

    /* finish the hash */
    CVMX_PREFETCH0(od->octo_hmouter);
#if 0
    if (unlikely(inplen)) {
	uint64_t tmp = 0;
	uint8_t *p = (uint8_t *) & tmp;
	p[inplen] = 0x80;
	do {
	    inplen--;
	    p[inplen] = ((uint8_t *) data)[inplen];
	} while (inplen);
	CVM_LOAD_SHA_UNIT(tmp, next);
    } else {
	CVM_LOAD_SHA_UNIT(0x8000000000000000ULL, next);
    }
#else
    CVM_LOAD_SHA_UNIT(0x8000000000000000ULL, next);
#endif

    /* Finish Inner hash */
    while (next != 7) {
	CVM_LOAD_SHA_UNIT(((uint64_t) 0x0ULL), next);
    }
	CVM_LOAD_SHA_UNIT((uint64_t) ((alen + 64) << 3), next);

    /* Get the inner hash of HMAC */
    CVMX_MF_HSH_IV(tmp1, 0);
    CVMX_MF_HSH_IV(tmp2, 1);
    tmp3 = 0;
    CVMX_MF_HSH_IV(tmp3, 2);

    /* Initialize hash unit */
    CVMX_MT_HSH_IV(od->octo_hmouter[0], 0);
    CVMX_MT_HSH_IV(od->octo_hmouter[1], 1);
    CVMX_MT_HSH_IV(od->octo_hmouter[2], 2);

    CVMX_MT_HSH_DAT(tmp1, 0);
    CVMX_MT_HSH_DAT(tmp2, 1);
    tmp3 |= 0x0000000080000000;
    CVMX_MT_HSH_DAT(tmp3, 2);
    CVMX_MT_HSH_DATZ(3);
    CVMX_MT_HSH_DATZ(4);
    CVMX_MT_HSH_DATZ(5);
    CVMX_MT_HSH_DATZ(6);
    CVMX_MT_HSH_STARTSHA((uint64_t) ((64 + 20) << 3));

    /* finish the hash */
    CVMX_PREFETCH0(od->octo_hmouter);
#if 0
    if (unlikely(inplen)) {
	uint64_t tmp = 0;
	uint8_t *p = (uint8_t *) & tmp;
	p[inplen] = 0x80;
	do {
	    inplen--;
	    p[inplen] = ((uint8_t *) data)[inplen];
	} while (inplen);
	CVM_LOAD_MD5_UNIT(tmp, next);
    } else {
	CVM_LOAD_MD5_UNIT(0x8000000000000000ULL, next);
    }
#else
    CVM_LOAD_MD5_UNIT(0x8000000000000000ULL, next);
#endif

    /* save the HMAC */
    SG_INIT(sg, data32, data_i, data_l);
    while (icv_off > 0) {
	SG_CONSUME(sg, data32, data_i, data_l);
	icv_off -= 4;
    }
    CVMX_MF_HSH_IV(tmp1, 0);
    *data32 = (uint32_t) (tmp1 >> 32);
    SG_CONSUME(sg, data32, data_i, data_l);
    *data32 = (uint32_t) tmp1;
    SG_CONSUME(sg, data32, data_i, data_l);
    CVMX_MF_HSH_IV(tmp1, 1);
    *data32 = (uint32_t) (tmp1 >> 32);

    octeon_crypto_disable(&state, flags);
    return 0;
}

/****************************************************************************/
