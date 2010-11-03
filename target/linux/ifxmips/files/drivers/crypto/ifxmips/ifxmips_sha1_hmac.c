/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 *
 *   Copyright (C) 2010 Ralph Hempel <ralph.hempel@lantiq.com>
 *   Copyright (C) 2009 Mohammad Firdaus
 */

/*!
  \defgroup IFX_DEU IFX_DEU_DRIVERS
  \ingroup API
  \brief ifx deu driver module
*/

/*!
  \file	ifxmips_sha1_hmac.c
  \ingroup IFX_DEU
  \brief SHA1-HMAC deu driver file
*/

/*!
  \defgroup IFX_SHA1_HMAC_FUNCTIONS IFX_SHA1_HMAC_FUNCTIONS
  \ingroup IFX_DEU
  \brief ifx sha1 hmac functions
*/


/* Project header */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/mm.h>
#include <linux/crypto.h>
#include <linux/cryptohash.h>
#include <linux/types.h>
#include <asm/scatterlist.h>
#include <asm/byteorder.h>
#include <linux/delay.h>
#include "ifxmips_deu.h"

#ifdef CONFIG_CRYPTO_DEV_IFXMIPS_SHA1_HMAC

#define SHA1_DIGEST_SIZE    20
#define SHA1_HMAC_BLOCK_SIZE    64
#define SHA1_HMAC_DBN_TEMP_SIZE 1024 // size in dword, needed for dbn workaround
#define HASH_START   IFX_HASH_CON

static spinlock_t lock;
#define CRTCL_SECT_INIT        spin_lock_init(&lock)
#define CRTCL_SECT_START       spin_lock_irqsave(&lock, flag)
#define CRTCL_SECT_END         spin_unlock_irqrestore(&lock, flag)

struct sha1_hmac_ctx {
        u64 count;
        u32 state[5];
        u8 buffer[64];
    u32 dbn;
    u32 temp[SHA1_HMAC_DBN_TEMP_SIZE];
};

extern int disable_deudma;

/*! \fn static void sha1_hmac_transform(struct crypto_tfm *tfm, u32 const *in)
 *  \ingroup IFX_SHA1_HMAC_FUNCTIONS
 *  \brief save input block to context
 *  \param tfm linux crypto algo transform
 *  \param in 64-byte block of input
*/
static void sha1_hmac_transform(struct crypto_tfm *tfm, u32 const *in)
{
    struct sha1_hmac_ctx *sctx = crypto_tfm_ctx(tfm);

    memcpy(&sctx->temp[sctx->dbn<<4], in, 64); //dbn workaround
    sctx->dbn += 1;

    if ( (sctx->dbn<<4) > SHA1_HMAC_DBN_TEMP_SIZE )
    {
        printk("SHA1_HMAC_DBN_TEMP_SIZE exceeded\n");
    }

}

/*! \fn int sha1_hmac_setkey(struct crypto_tfm *tfm, const u8 *key, unsigned int keylen)
 *  \ingroup IFX_SHA1_HMAC_FUNCTIONS
 *  \brief sets sha1 hmac key
 *  \param tfm linux crypto algo transform
 *  \param key input key
 *  \param keylen key length greater than 64 bytes IS NOT SUPPORTED
*/
int sha1_hmac_setkey(struct crypto_tfm *tfm, const u8 *key, unsigned int keylen)
{
    volatile struct deu_hash_t *hash = (struct deu_hash_t *) HASH_START;
    int i, j;
    u32 *in_key = (u32 *)key;

    hash->KIDX = 0x80000000; // reset all 16 words of the key to '0'
    asm("sync");

    j = 0;
    for (i = 0; i < keylen; i+=4)
    {
         hash->KIDX = j;
         asm("sync");
         hash->KEY = *((u32 *) in_key + j);
         j++;
    }

    return 0;
}

/*! \fn void sha1_hmac_init(struct crypto_tfm *tfm)
 *  \ingroup IFX_SHA1_HMAC_FUNCTIONS
 *  \brief initialize sha1 hmac context
 *  \param tfm linux crypto algo transform
*/
void sha1_hmac_init(struct crypto_tfm *tfm)
{
    struct sha1_hmac_ctx *sctx = crypto_tfm_ctx(tfm);

        memset(sctx, 0, sizeof(struct sha1_hmac_ctx));
    sctx->dbn = 0; //dbn workaround
}

/*! \fn static void sha1_hmac_update(struct crypto_tfm *tfm, const u8 *data, unsigned int len)
 *  \ingroup IFX_SHA1_HMAC_FUNCTIONS
 *  \brief on-the-fly sha1 hmac computation
 *  \param tfm linux crypto algo transform
 *  \param data input data
 *  \param len size of input data
*/
static void sha1_hmac_update(struct crypto_tfm *tfm, const u8 *data,
            unsigned int len)
{
    struct sha1_hmac_ctx *sctx = crypto_tfm_ctx(tfm);
    unsigned int i, j;

    j = (sctx->count >> 3) & 0x3f;
    sctx->count += len << 3;
    //printk("sctx->count = %d\n", (sctx->count >> 3));

    if ((j + len) > 63) {
        memcpy (&sctx->buffer[j], data, (i = 64 - j));
        sha1_hmac_transform (tfm, (const u32 *)sctx->buffer);
        for (; i + 63 < len; i += 64) {
            sha1_hmac_transform (tfm, (const u32 *)&data[i]);
        }

        j = 0;
    }
    else
        i = 0;

    memcpy (&sctx->buffer[j], &data[i], len - i);
}

/*! \fn static void sha1_hmac_final(struct crypto_tfm *tfm, u8 *out)
 *  \ingroup IFX_SHA1_HMAC_FUNCTIONS
 *  \brief ompute final sha1 hmac value
 *  \param tfm linux crypto algo transform
 *  \param out final sha1 hmac output value
*/
static void sha1_hmac_final(struct crypto_tfm *tfm, u8 *out)
{
        struct sha1_hmac_ctx *sctx = crypto_tfm_ctx(tfm);
    u32 index, padlen;
    u64 t;
    u8 bits[8] = { 0, };
    static const u8 padding[64] = { 0x80, };
    volatile struct deu_hash_t *hashs = (struct deu_hash_t *) HASH_START;
    ulong flag;
    int i = 0;
    int dbn;
    u32 *in = &sctx->temp[0];

    t = sctx->count + 512; // need to add 512 bit of the IPAD operation
    bits[7] = 0xff & t;
    t >>= 8;
    bits[6] = 0xff & t;
    t >>= 8;
    bits[5] = 0xff & t;
    t >>= 8;
    bits[4] = 0xff & t;
    t >>= 8;
    bits[3] = 0xff & t;
    t >>= 8;
    bits[2] = 0xff & t;
    t >>= 8;
    bits[1] = 0xff & t;
    t >>= 8;
    bits[0] = 0xff & t;

    /* Pad out to 56 mod 64 */
    index = (sctx->count >> 3) & 0x3f;
    padlen = (index < 56) ? (56 - index) : ((64 + 56) - index);
    sha1_hmac_update (tfm, padding, padlen);

    /* Append length */
    sha1_hmac_update (tfm, bits, sizeof bits);

    CRTCL_SECT_START;

    hashs->DBN = sctx->dbn;

    //for vr9 change, ENDI = 1
    *IFX_HASH_CON = HASH_CON_VALUE;

    //wait for processing
    while (hashs->controlr.BSY) {
        // this will not take long
    }

    for (dbn = 0; dbn < sctx->dbn; dbn++)
    {
    for (i = 0; i < 16; i++) {
        hashs->MR = in[i];
    };

    hashs->controlr.GO = 1;
    asm("sync");

    //wait for processing
    while (hashs->controlr.BSY) {
            // this will not take long
    }

    in += 16;
}


#if 1
    //wait for digest ready
    while (! hashs->controlr.DGRY) {
        // this will not take long
    }
#endif

    *((u32 *) out + 0) = hashs->D1R;
    *((u32 *) out + 1) = hashs->D2R;
    *((u32 *) out + 2) = hashs->D3R;
    *((u32 *) out + 3) = hashs->D4R;
    *((u32 *) out + 4) = hashs->D5R;

    CRTCL_SECT_END;
}

/*
 * \brief SHA1-HMAC function mappings
*/

struct crypto_alg ifxdeu_sha1_hmac_alg = {
        .cra_name       =       "hmac(sha1)",
        .cra_driver_name=       "ifxdeu-sha1_hmac",
        .cra_flags      =       CRYPTO_ALG_TYPE_DIGEST,
        .cra_blocksize  =       SHA1_HMAC_BLOCK_SIZE,
        .cra_ctxsize    =       sizeof(struct sha1_hmac_ctx),
        .cra_module   =         THIS_MODULE,
        .cra_alignmask  =       3,
        .cra_list       =       LIST_HEAD_INIT(ifxdeu_sha1_hmac_alg.cra_list),
        .cra_u          =       { .digest = {
        .dia_digestsize =       SHA1_DIGEST_SIZE,
        .dia_setkey     =       sha1_hmac_setkey,
        .dia_init       =       sha1_hmac_init,
        .dia_update     =       sha1_hmac_update,
        .dia_final      =       sha1_hmac_final } }
};


/*! \fn int __init ifxdeu_init_sha1_hmac (void)
 *  \ingroup IFX_SHA1_HMAC_FUNCTIONS
 *  \brief initialize sha1 hmac driver
*/
int __init ifxdeu_init_sha1_hmac (void)
{
    int ret;

    if ((ret = crypto_register_alg(&ifxdeu_sha1_hmac_alg)))
        goto sha1_err;

    CRTCL_SECT_INIT;

    printk (KERN_NOTICE "IFX DEU SHA1_HMAC initialized%s.\n", disable_deudma ? "" : " (DMA)");
    return ret;

sha1_err:
    printk(KERN_ERR "IFX DEU SHA1_HMAC initialization failed!\n");
    return ret;
}

/*! \fn void __exit ifxdeu_fini_sha1_hmac (void)
 *  \ingroup IFX_SHA1_HMAC_FUNCTIONS
 *  \brief unregister sha1 hmac driver
*/
void __exit ifxdeu_fini_sha1_hmac (void)
{
    crypto_unregister_alg (&ifxdeu_sha1_hmac_alg);
}

#endif
