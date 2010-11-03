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
  \brief  ifx deu driver module
*/

/*!
  \file	ifxmips_md5_hmac.c
  \ingroup IFX_DEU
  \brief MD5-HMAC encryption deu driver file
*/

/*!
 \defgroup IFX_MD5_HMAC_FUNCTIONS IFX_MD5_HMAC_FUNCTIONS
 \ingroup IFX_DEU
 \brief ifx md5-hmac driver functions
*/

/* Project Header files */
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
#define MD5_HMAC_DBN_TEMP_SIZE  1024 // size in dword, needed for dbn workaround
#define HASH_START   IFX_HASH_CON

static spinlock_t lock;
#define CRTCL_SECT_INIT        spin_lock_init(&lock)
#define CRTCL_SECT_START       spin_lock_irqsave(&lock, flag)
#define CRTCL_SECT_END         spin_unlock_irqrestore(&lock, flag)

struct md5_hmac_ctx {
    u32 hash[MD5_HASH_WORDS];
    u32 block[MD5_BLOCK_WORDS];
    u64 byte_count;
    u32 dbn;
    u32 temp[MD5_HMAC_DBN_TEMP_SIZE];
};

extern int disable_deudma;

/*! \fn static u32 endian_swap(u32 input)
 *  \ingroup IFX_MD5_HMAC_FUNCTIONS
 *  \brief perform dword level endian swap
 *  \param input value of dword that requires to be swapped
*/
static u32 endian_swap(u32 input)
{
    u8 *ptr = (u8 *)&input;

    return ((ptr[3] << 24) | (ptr[2] << 16) | (ptr[1] << 8) | ptr[0]);
}

/*! \fn static void md5_hmac_transform(struct crypto_tfm *tfm, u32 const *in)
 *  \ingroup IFX_MD5_HMAC_FUNCTIONS
 *  \brief save input block to context
 *  \param tfm linux crypto algo transform
 *  \param in 64-byte block of input
*/
static void md5_hmac_transform(struct crypto_tfm *tfm, u32 const *in)
{
    struct md5_hmac_ctx *mctx = crypto_tfm_ctx(tfm);

    memcpy(&mctx->temp[mctx->dbn<<4], in, 64); //dbn workaround
        mctx->dbn += 1;

    if ( (mctx->dbn<<4) > MD5_HMAC_DBN_TEMP_SIZE )
    {
        printk("MD5_HMAC_DBN_TEMP_SIZE exceeded\n");
    }

}

/*! \fn int md5_hmac_setkey(struct crypto_tfm *tfm, const u8 *key, unsigned int keylen)
 *  \ingroup IFX_MD5_HMAC_FUNCTIONS
 *  \brief sets md5 hmac key
 *  \param tfm linux crypto algo transform
 *  \param key input key
 *  \param keylen key length greater than 64 bytes IS NOT SUPPORTED
*/
int md5_hmac_setkey(struct crypto_tfm *tfm, const u8 *key, unsigned int keylen)
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

/*! \fn void md5_hmac_init(struct crypto_tfm *tfm)
 *  \ingroup IFX_MD5_HMAC_FUNCTIONS
 *  \brief initialize md5 hmac context
 *  \param tfm linux crypto algo transform
*/
void md5_hmac_init(struct crypto_tfm *tfm)
{
    struct md5_hmac_ctx *mctx = crypto_tfm_ctx(tfm);

    memset(mctx, 0, sizeof(struct md5_hmac_ctx));
    mctx->dbn = 0; //dbn workaround
}

/*! \fn void md5_hmac_update(struct crypto_tfm *tfm, const u8 *data, unsigned int len)
 *  \ingroup IFX_MD5_HMAC_FUNCTIONS
 *  \brief on-the-fly md5 hmac computation
 *  \param tfm linux crypto algo transform
 *  \param data input data
 *  \param len size of input data
*/
void md5_hmac_update(struct crypto_tfm *tfm, const u8 *data, unsigned int len)
{
    struct md5_hmac_ctx *mctx = crypto_tfm_ctx(tfm);
    const u32 avail = sizeof(mctx->block) - (mctx->byte_count & 0x3f);

    mctx->byte_count += len;

    if (avail > len) {
        memcpy((char *)mctx->block + (sizeof(mctx->block) - avail),
               data, len);
        return;
    }

    memcpy((char *)mctx->block + (sizeof(mctx->block) - avail),
           data, avail);

    md5_hmac_transform(tfm, mctx->block);
    data += avail;
    len -= avail;

    while (len >= sizeof(mctx->block)) {
        memcpy(mctx->block, data, sizeof(mctx->block));
        md5_hmac_transform(tfm, mctx->block);
        data += sizeof(mctx->block);
        len -= sizeof(mctx->block);
    }

    memcpy(mctx->block, data, len);

}

/*! \fn void md5_hmac_final(struct crypto_tfm *tfm, u8 *out)
 *  \ingroup IFX_MD5_HMAC_FUNCTIONS
 *  \brief compute final md5 hmac value
 *  \param tfm linux crypto algo transform
 *  \param out final md5 hmac output value
*/
void md5_hmac_final(struct crypto_tfm *tfm, u8 *out)
{
    struct md5_hmac_ctx *mctx = crypto_tfm_ctx(tfm);
    const unsigned int offset = mctx->byte_count & 0x3f;
    char *p = (char *)mctx->block + offset;
    int padding = 56 - (offset + 1);
    volatile struct deu_hash_t *hashs = (struct deu_hash_t *) HASH_START;
    u32 flag;
    int i = 0;
    int dbn;
    u32 *in = &mctx->temp[0];

    *p++ = 0x80;
    if (padding < 0) {
        memset(p, 0x00, padding + sizeof (u64));
        md5_hmac_transform(tfm, mctx->block);
        p = (char *)mctx->block;
        padding = 56;
    }

    memset(p, 0, padding);
    mctx->block[14] = endian_swap((mctx->byte_count + 64) << 3); // need to add 512 bit of the IPAD operation
    mctx->block[15] = 0x00000000;

    md5_hmac_transform(tfm, mctx->block);

    CRTCL_SECT_START;

    printk("dbn = %d\n", mctx->dbn);
    hashs->DBN = mctx->dbn;

    *IFX_HASH_CON = 0x0703002D; //khs, go, init, ndc, endi, kyue, hmen, md5

    //wait for processing
    while (hashs->controlr.BSY) {
        // this will not take long
    }

for (dbn = 0; dbn < mctx->dbn; dbn++)
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
 * \brief MD5_HMAC function mappings
*/

static struct crypto_alg ifxdeu_md5_hmac_alg = {
    .cra_name           =   "hmac(md5)",
    .cra_driver_name    =   "ifxdeu-md5_hmac",
    .cra_flags          =   CRYPTO_ALG_TYPE_DIGEST,
    .cra_blocksize          =   MD5_HMAC_BLOCK_SIZE,
    .cra_ctxsize            =   sizeof(struct md5_hmac_ctx),
    .cra_module         =   THIS_MODULE,
    .cra_list           =   LIST_HEAD_INIT(ifxdeu_md5_hmac_alg.cra_list),
    .cra_u              =   { .digest = {
    .dia_digestsize         =   MD5_DIGEST_SIZE,
        .dia_setkey             =       md5_hmac_setkey,
        .dia_init               =       md5_hmac_init,
        .dia_update             =       md5_hmac_update,
        .dia_final              =       md5_hmac_final } }
};

/*! \fn int __init ifxdeu_init_md5_hmac (void)
 *  \ingroup IFX_MD5_HMAC_FUNCTIONS
 *  \brief initialize md5 hmac driver
*/
int __init ifxdeu_init_md5_hmac (void)
{
    int ret;

    if ((ret = crypto_register_alg(&ifxdeu_md5_hmac_alg)))
        goto md5_hmac_err;

    CRTCL_SECT_INIT;

    printk (KERN_NOTICE "IFX DEU MD5_HMAC initialized%s.\n", disable_deudma ? "" : " (DMA)");
    return ret;

md5_hmac_err:
    printk(KERN_ERR "IFX DEU MD5_HMAC initialization failed!\n");
    return ret;
}

/** \fn void __exit ifxdeu_fini_md5_hmac (void)
 *  \ingroup IFX_MD5_HMAC_FUNCTIONS
 *  \brief unregister md5 hmac driver
*/
void __exit ifxdeu_fini_md5_hmac (void)
{
    crypto_unregister_alg (&ifxdeu_md5_hmac_alg);
}

