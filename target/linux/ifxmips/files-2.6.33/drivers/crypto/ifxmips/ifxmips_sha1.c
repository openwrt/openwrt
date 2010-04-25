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
  \file	ifxmips_sha1.c
  \ingroup IFX_DEU
  \brief SHA1 encryption deu driver file
*/

/*!
  \defgroup IFX_SHA1_FUNCTIONS IFX_SHA1_FUNCTIONS
  \ingroup IFX_DEU
  \brief ifx deu sha1 functions
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
#include "ifxmips_deu.h"

#define SHA1_DIGEST_SIZE    20
#define SHA1_HMAC_BLOCK_SIZE    64
#define HASH_START   IFX_HASH_CON

static spinlock_t lock;
#define CRTCL_SECT_INIT        spin_lock_init(&lock)
#define CRTCL_SECT_START       spin_lock_irqsave(&lock, flag)
#define CRTCL_SECT_END         spin_unlock_irqrestore(&lock, flag)

/*
 * \brief SHA1 private structure
*/
struct sha1_ctx {
        u64 count;
        u32 state[5];
        u8 buffer[64];
};

extern int disable_deudma;


/*! \fn static void sha1_transform (u32 *state, const u32 *in)
 *  \ingroup IFX_SHA1_FUNCTIONS
 *  \brief main interface to sha1 hardware
 *  \param state current state
 *  \param in 64-byte block of input
*/
static void sha1_transform (u32 *state, const u32 *in)
{
    int i = 0;
    volatile struct deu_hash_t *hashs = (struct deu_hash_t *) HASH_START;
    u32 flag;

    CRTCL_SECT_START;

    for (i = 0; i < 16; i++) {
        hashs->MR = in[i];
    };

    //wait for processing
    while (hashs->controlr.BSY) {
        // this will not take long
    }

    CRTCL_SECT_END;
}

/*! \fn static void sha1_init(struct crypto_tfm *tfm)
 *  \ingroup IFX_SHA1_FUNCTIONS
 *  \brief initialize sha1 hardware
 *  \param tfm linux crypto algo transform
*/
static void sha1_init(struct crypto_tfm *tfm)
{
    struct sha1_ctx *sctx = crypto_tfm_ctx(tfm);

    SHA_HASH_INIT;

    sctx->count = 0;
}

/*! \fn static void sha1_update(struct crypto_tfm *tfm, const u8 *data, unsigned int len)
 *  \ingroup IFX_SHA1_FUNCTIONS
 *  \brief on-the-fly sha1 computation
 *  \param tfm linux crypto algo transform
 *  \param data input data
 *  \param len size of input data
*/
static void sha1_update(struct crypto_tfm *tfm, const u8 *data,
            unsigned int len)
{
    struct sha1_ctx *sctx = crypto_tfm_ctx(tfm);
    unsigned int i, j;

    j = (sctx->count >> 3) & 0x3f;
    sctx->count += len << 3;

    if ((j + len) > 63) {
        memcpy (&sctx->buffer[j], data, (i = 64 - j));
        sha1_transform (sctx->state, (const u32 *)sctx->buffer);
        for (; i + 63 < len; i += 64) {
            sha1_transform (sctx->state, (const u32 *)&data[i]);
        }

        j = 0;
    }
    else
        i = 0;

    memcpy (&sctx->buffer[j], &data[i], len - i);
}

/*! \fn static void sha1_final(struct crypto_tfm *tfm, u8 *out)
 *  \ingroup IFX_SHA1_FUNCTIONS
 *  \brief compute final sha1 value
 *  \param tfm linux crypto algo transform
 *  \param out final md5 output value
*/
static void sha1_final(struct crypto_tfm *tfm, u8 *out)
{
    struct sha1_ctx *sctx = crypto_tfm_ctx(tfm);
    u32 index, padlen;
    u64 t;
    u8 bits[8] = { 0, };
    static const u8 padding[64] = { 0x80, };
    volatile struct deu_hash_t *hashs = (struct deu_hash_t *) HASH_START;
    ulong flag;

    t = sctx->count;
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
    sha1_update (tfm, padding, padlen);

    /* Append length */
    sha1_update (tfm, bits, sizeof bits);

    CRTCL_SECT_START;

    *((u32 *) out + 0) = hashs->D1R;
    *((u32 *) out + 1) = hashs->D2R;
    *((u32 *) out + 2) = hashs->D3R;
    *((u32 *) out + 3) = hashs->D4R;
    *((u32 *) out + 4) = hashs->D5R;

        CRTCL_SECT_END;

    // Wipe context
    memset (sctx, 0, sizeof *sctx);
}

/*
 * \brief SHA1 function mappings
*/
struct crypto_alg ifxdeu_sha1_alg = {
    .cra_name   =   "sha1",
    .cra_driver_name=   "ifxdeu-sha1",
    .cra_flags  =   CRYPTO_ALG_TYPE_DIGEST,
    .cra_blocksize  =   SHA1_HMAC_BLOCK_SIZE,
    .cra_ctxsize    =   sizeof(struct sha1_ctx),
    .cra_module =   THIS_MODULE,
    .cra_alignmask  =   3,
    .cra_list       =       LIST_HEAD_INIT(ifxdeu_sha1_alg.cra_list),
    .cra_u      =   { .digest = {
    .dia_digestsize =   SHA1_DIGEST_SIZE,
    .dia_init       =   sha1_init,
    .dia_update     =   sha1_update,
    .dia_final      =   sha1_final } }
};

/*! \fn int __init ifxdeu_init_sha1 (void)
 *  \ingroup IFX_SHA1_FUNCTIONS
 *  \brief initialize sha1 driver
*/
int __init ifxdeu_init_sha1 (void)
{
    int ret;

    if ((ret = crypto_register_alg(&ifxdeu_sha1_alg)))
        goto sha1_err;

    CRTCL_SECT_INIT;

    printk (KERN_NOTICE "IFX DEU SHA1 initialized%s.\n", disable_deudma ? "" : " (DMA)");
    return ret;

sha1_err:
    printk(KERN_ERR "IFX DEU SHA1 initialization failed!\n");
    return ret;
}

/*! \fn void __exit ifxdeu_fini_sha1 (void)
 *  \ingroup IFX_SHA1_FUNCTIONS
 *  \brief unregister sha1 driver
*/
void __exit ifxdeu_fini_sha1 (void)
{
    crypto_unregister_alg (&ifxdeu_sha1_alg);
}
