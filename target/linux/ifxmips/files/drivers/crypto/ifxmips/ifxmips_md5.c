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
  \defgroup    IFX_DEU IFX_DEU_DRIVERS
  \ingroup API
  \brief ifx deu driver module
*/

/*!
  \file		ifxmips_md5.c
  \ingroup 	IFX_DEU
  \brief 	MD5 encryption deu driver file
*/

/*!
  \defgroup IFX_MD5_FUNCTIONS IFX_MD5_FUNCTIONS
  \ingroup IFX_DEU
  \brief ifx deu MD5 functions
*/

/*Project header files */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/crypto.h>
#include <linux/types.h>
#include <asm/byteorder.h>
#include "ifxmips_deu.h"

#define MD5_DIGEST_SIZE     16
#define MD5_HMAC_BLOCK_SIZE 64
#define MD5_BLOCK_WORDS     16
#define MD5_HASH_WORDS      4
#define HASH_START   IFX_HASH_CON

static spinlock_t lock;
#define CRTCL_SECT_INIT        spin_lock_init(&lock)
#define CRTCL_SECT_START       spin_lock_irqsave(&lock, flag)
#define CRTCL_SECT_END         spin_unlock_irqrestore(&lock, flag)

struct md5_ctx {
    u32 hash[MD5_HASH_WORDS];
    u32 block[MD5_BLOCK_WORDS];
    u64 byte_count;
};

extern int disable_deudma;

/*! \fn static u32 endian_swap(u32 input)
 *  \ingroup IFX_MD5_FUNCTIONS
 *  \brief perform dword level endian swap
 *  \param input value of dword that requires to be swapped
*/
static u32 endian_swap(u32 input)
{
    u8 *ptr = (u8 *)&input;

    return ((ptr[3] << 24) | (ptr[2] << 16) | (ptr[1] << 8) | ptr[0]);
}

/*! \fn static void md5_transform(u32 *hash, u32 const *in)
 *  \ingroup IFX_MD5_FUNCTIONS
 *  \brief main interface to md5 hardware
 *  \param hash current hash value
 *  \param in 64-byte block of input
*/
static void md5_transform(u32 *hash, u32 const *in)
{
        int i;
    volatile struct deu_hash_t *hashs = (struct deu_hash_t *) HASH_START;
    ulong flag;

    CRTCL_SECT_START;

    for (i = 0; i < 16; i++) {
        hashs->MR = endian_swap(in[i]);
    };

    //wait for processing
    while (hashs->controlr.BSY) {
        // this will not take long
    }

    CRTCL_SECT_END;
}

/*! \fn static inline void md5_transform_helper(struct md5_ctx *ctx)
 *  \ingroup IFX_MD5_FUNCTIONS
 *  \brief interfacing function for md5_transform()
 *  \param ctx crypto context
*/
static inline void md5_transform_helper(struct md5_ctx *ctx)
{
    //le32_to_cpu_array(ctx->block, sizeof(ctx->block) / sizeof(u32));
    md5_transform(ctx->hash, ctx->block);
}

/*! \fn static void md5_init(struct crypto_tfm *tfm)
 *  \ingroup IFX_MD5_FUNCTIONS
 *  \brief initialize md5 hardware
 *  \param tfm linux crypto algo transform
*/
static void md5_init(struct crypto_tfm *tfm)
{
    struct md5_ctx *mctx = crypto_tfm_ctx(tfm);
        volatile struct deu_hash_t *hash = (struct deu_hash_t *) HASH_START;

    hash->controlr.SM = 1;
    hash->controlr.ALGO = 1;    // 1 = md5  0 = sha1
    hash->controlr.INIT = 1;    // Initialize the hash operation by writing a '1' to the INIT bit.

    mctx->byte_count = 0;
}

/*! \fn static void md5_update(struct crypto_tfm *tfm, const u8 *data, unsigned int len)
 *  \ingroup IFX_MD5_FUNCTIONS
 *  \brief on-the-fly md5 computation
 *  \param tfm linux crypto algo transform
 *  \param data input data
 *  \param len size of input data
*/
static void md5_update(struct crypto_tfm *tfm, const u8 *data, unsigned int len)
{
    struct md5_ctx *mctx = crypto_tfm_ctx(tfm);
    const u32 avail = sizeof(mctx->block) - (mctx->byte_count & 0x3f);

    mctx->byte_count += len;

    if (avail > len) {
        memcpy((char *)mctx->block + (sizeof(mctx->block) - avail),
               data, len);
        return;
    }

    memcpy((char *)mctx->block + (sizeof(mctx->block) - avail),
           data, avail);

    md5_transform_helper(mctx);
    data += avail;
    len -= avail;

    while (len >= sizeof(mctx->block)) {
        memcpy(mctx->block, data, sizeof(mctx->block));
        md5_transform_helper(mctx);
        data += sizeof(mctx->block);
        len -= sizeof(mctx->block);
    }

    memcpy(mctx->block, data, len);
}

/*! \fn static void md5_final(struct crypto_tfm *tfm, u8 *out)
 *  \ingroup IFX_MD5_FUNCTIONS
 *  \brief compute final md5 value
 *  \param tfm linux crypto algo transform
 *  \param out final md5 output value
*/
static void md5_final(struct crypto_tfm *tfm, u8 *out)
{
    struct md5_ctx *mctx = crypto_tfm_ctx(tfm);
    const unsigned int offset = mctx->byte_count & 0x3f;
    char *p = (char *)mctx->block + offset;
    int padding = 56 - (offset + 1);
    volatile struct deu_hash_t *hashs = (struct deu_hash_t *) HASH_START;
    u32 flag;

    *p++ = 0x80;
    if (padding < 0) {
        memset(p, 0x00, padding + sizeof (u64));
        md5_transform_helper(mctx);
        p = (char *)mctx->block;
        padding = 56;
    }

    memset(p, 0, padding);
    mctx->block[14] = endian_swap(mctx->byte_count << 3);
    mctx->block[15] = endian_swap(mctx->byte_count >> 29);

#if 0
    le32_to_cpu_array(mctx->block, (sizeof(mctx->block) -
                      sizeof(u64)) / sizeof(u32));
#endif

    md5_transform(mctx->hash, mctx->block);

    CRTCL_SECT_START;

    *((u32 *) out + 0) = endian_swap (hashs->D1R);
    *((u32 *) out + 1) = endian_swap (hashs->D2R);
    *((u32 *) out + 2) = endian_swap (hashs->D3R);
    *((u32 *) out + 3) = endian_swap (hashs->D4R);

    CRTCL_SECT_END;

    // Wipe context
    memset(mctx, 0, sizeof(*mctx));
}

/*
 * \brief MD5 function mappings
*/
static struct crypto_alg ifxdeu_md5_alg = {
    .cra_name           =   "md5",
    .cra_driver_name    =   "ifxdeu-md5",
    .cra_flags          =   CRYPTO_ALG_TYPE_DIGEST,
    .cra_blocksize          =   MD5_HMAC_BLOCK_SIZE,
    .cra_ctxsize            =   sizeof(struct md5_ctx),
    .cra_module         =   THIS_MODULE,
    .cra_list           =   LIST_HEAD_INIT(ifxdeu_md5_alg.cra_list),
    .cra_u              =   { .digest = {
    .dia_digestsize         =   MD5_DIGEST_SIZE,
    .dia_init               =   md5_init,
    .dia_update             =   md5_update,
    .dia_final              =   md5_final } }
};

/*! \fn int __init ifxdeu_init_md5 (void)
 *  \ingroup IFX_MD5_FUNCTIONS
 *  \brief initialize md5 driver
*/
int __init ifxdeu_init_md5 (void)
{
    int ret;

    if ((ret = crypto_register_alg(&ifxdeu_md5_alg)))
        goto md5_err;

    CRTCL_SECT_INIT;

    printk (KERN_NOTICE "IFX DEU MD5 initialized%s.\n", disable_deudma ? "" : " (DMA)");
    return ret;

md5_err:
    printk(KERN_ERR "IFX DEU MD5 initialization failed!\n");
    return ret;
}

/*! \fn void __exit ifxdeu_fini_md5 (void)
  * \ingroup IFX_MD5_FUNCTIONS
  * \brief unregister md5 driver
*/

void __exit ifxdeu_fini_md5 (void)
{
    crypto_unregister_alg (&ifxdeu_md5_alg);
}

