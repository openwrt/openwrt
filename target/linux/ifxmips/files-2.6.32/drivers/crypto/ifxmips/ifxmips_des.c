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
  \brief ifx deu driver
*/

/*!
  \file		ifxmips_des.c
  \ingroup 	IFX_DEU
  \brief 	DES encryption DEU driver file
*/

/*!
  \defgroup IFX_DES_FUNCTIONS IFX_DES_FUNCTIONS
  \ingroup IFX_DEU
  \brief IFX DES Encryption functions
*/

/* Project Header Files */
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/crypto.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <asm/byteorder.h>
#include <crypto/algapi.h>
#include "ifxmips_deu.h"

/* DMA specific header and variables */
#ifdef CONFIG_CRYPTO_DEV_IFXMIPS_DMA
#include "ifxmips_deu_dma.h"
#include <asm/ifx/irq.h>
#include <asm/ifx/ifx_dma_core.h>
extern _ifx_deu_device ifx_deu[1];
extern u32 *des_buff_in;
extern u32 *des_buff_out;
#ifndef CONFIG_CRYPTO_DEV_IFXMIPS_POLL_DMA
#define CONFIG_CRYPTO_DEV_IFXMIPS_POLL_DMA
#endif  /* CONFIG_CRYPTO_DEV_IFXMIPS_POLL_DMA */
#endif  /* CONFIG_CRYPTO_DEV_IFXMIPS_DMA */

spinlock_t des_lock;
#define CRTCL_SECT_INIT        spin_lock_init(&des_lock)
#define CRTCL_SECT_START       spin_lock_irqsave(&des_lock, flag)
#define CRTCL_SECT_END         spin_unlock_irqrestore(&des_lock, flag)

/* Preprocessor declarations */
#define DES_3DES_START          IFX_DES_CON
#define DES_KEY_SIZE            8
#define DES_EXPKEY_WORDS        32
#define DES_BLOCK_SIZE          8
#define DES3_EDE_KEY_SIZE       (3 * DES_KEY_SIZE)
#define DES3_EDE_EXPKEY_WORDS   (3 * DES_EXPKEY_WORDS)
#define DES3_EDE_BLOCK_SIZE     DES_BLOCK_SIZE

/* Function Declaration to prevent warning messages */
void des_chip_init (void);
u32 endian_swap(u32 input);
u32 input_swap(u32 input);
int aes_memory_allocate(int value);
int des_memory_allocate(int value);
void memory_release(u32 *buffer);
u32* memory_alignment(const u8 *arg, u32 *buff_alloc, int in_out, int nbytes);
void aes_dma_memory_copy(u32 *outcopy, u32 *out_dma, u8 *out_arg, int nbytes);
void des_dma_memory_copy(u32 *outcopy, u32 *out_dma, u8 *out_arg, int nbytes);

#ifndef CONFIG_CRYPTO_DEV_IFXMIPS_DMA
void ifx_deu_des (void *ctx_arg, u8 *out_arg, const u8 *in_arg,
             u8 *iv_arg, u32 nbytes, int encdec, int mode);
#else
void ifx_deu_des_core (void *ctx_arg, u8 *out_arg, const u8 *in_arg,
             u8 *iv_arg, u32 nbytes, int encdec, int mode);
#endif /* CONFIG_CRYPTO_DEV_IFXMIPS_DMA */

struct des_ctx {
    int controlr_M;
    int key_length;
    u8 iv[DES_BLOCK_SIZE];
    u32 expkey[DES3_EDE_EXPKEY_WORDS];
};

extern int disable_deudma;

/*! \fn	int des_setkey(struct crypto_tfm *tfm, const u8 *key, unsigned int key_len)
 *  \ingroup IFX_DES_FUNCTIONS
 *  \brief sets DES key
 *  \param tfm linux crypto algo transform
 *  \param key input key
 *  \param key_len key length
*/
int des_setkey(struct crypto_tfm *tfm, const u8 *key,
                      unsigned int key_len)
{
    struct des_ctx *ctx = crypto_tfm_ctx(tfm);

    DPRINTF(0, "ctx @%p, key_len %d %d\n", ctx, key_len);

    ctx->controlr_M = 0;   /* des */
    ctx->key_length = key_len;

    memcpy ((u8 *) (ctx->expkey), key, key_len);

    return 0;
}

#ifndef CONFIG_CRYPTO_DEV_IFXMIPS_DMA
/*! \fn void ifx_deu_des(void *ctx_arg, u8 *out_arg, const u8 *in_arg, u8 *iv_arg, u32 nbytes, int encdec, int mode)
 *  \ingroup IFX_DES_FUNCTIONS
 *  \brief main interface to DES hardware
 *  \param ctx_arg crypto algo context
 *  \param out_arg output bytestream
 *  \param in_arg input bytestream
 *  \param iv_arg initialization vector
 *  \param nbytes length of bytestream
 *  \param encdec 1 for encrypt; 0 for decrypt
 *  \param mode operation mode such as ebc, cbc
*/

void ifx_deu_des (void *ctx_arg, u8 *out_arg, const u8 *in_arg,
             u8 *iv_arg, u32 nbytes, int encdec, int mode)
#else
/*! \fn void ifx_deu_des_core(void *ctx_arg, u8 *out_arg, const u8 *in_arg, u8 *iv_arg, u32 nbytes, int encdec, int mode)
 *  \ingroup IFX_DES_FUNCTIONS
 *  \brief main interface to DES hardware
 *  \param ctx_arg crypto algo context
 *  \param out_arg output bytestream
 *  \param in_arg input bytestream
 *  \param iv_arg initialization vector
 *  \param nbytes length of bytestream
 *  \param encdec 1 for encrypt; 0 for decrypt
 *  \param mode operation mode such as ebc, cbc
*/

void ifx_deu_des_core (void *ctx_arg, u8 *out_arg, const u8 *in_arg,
             u8 *iv_arg, u32 nbytes, int encdec, int mode)
#endif
{
    volatile struct des_t *des = (struct des_t *) DES_3DES_START;
    struct des_ctx *dctx = ctx_arg;
    u32 *key = dctx->expkey;
    ulong flag;

#ifndef CONFIG_CRYPTO_DEV_IFXMIPS_DMA
    int i = 0;
    int nblocks = 0;
#else
    volatile struct deu_dma_t *dma = (struct deu_dma_t *) IFX_DEU_DMA_CON;
    struct dma_device_info *dma_device = ifx_deu[0].dma_device;
    //deu_drv_priv_t *deu_priv = (deu_drv_priv_t *)dma_device->priv;
    int wlen = 0;
    u32 *outcopy = NULL;
    u32 *dword_mem_aligned_in = NULL;

#ifdef CONFIG_CRYPTO_DEV_IFXMIPS_POLL_DMA
    u32 timeout = 0;
    u32 *out_dma = NULL;
#endif

#endif

    DPRINTF(0, "ctx @%p, mode %d, encdec %d\n", dctx, mode, encdec);

    CRTCL_SECT_START;

    des->controlr.E_D = !encdec; /* encryption */
    des->controlr.O = mode;      /* 0 ECB, 1 CBC, 2 OFB, 3 CFB, 4 CTR */
    des->controlr.SM = 1;        /* start after writing input register */
    des->controlr.DAU = 0;       /* Disable Automatic Update of init vector */
    des->controlr.ARS = 1;       /* Autostart Select - write to IHR */

    des->controlr.M = dctx->controlr_M;
    /* write keys */
    if (dctx->controlr_M == 0) {
        /* DES mode */
        des->K1HR = DEU_ENDIAN_SWAP(*((u32 *) key + 0));
        des->K1LR = DEU_ENDIAN_SWAP(*((u32 *) key + 1));
#ifdef CRYPTO_DEBUG
        printk ("key1: %x\n", (*((u32 *) key + 0)));
        printk ("key2: %x\n", (*((u32 *) key + 1)));
#endif
    } else {
        /* 3DES mode (EDE-x) */
        switch (dctx->key_length) {
        case 24:
            des->K3HR = DEU_ENDIAN_SWAP(*((u32 *) key + 4));
            des->K3LR = DEU_ENDIAN_SWAP(*((u32 *) key + 5));
            /* no break; */
        case 16:
            des->K2HR = DEU_ENDIAN_SWAP(*((u32 *) key + 2));
            des->K2LR = DEU_ENDIAN_SWAP(*((u32 *) key + 3));
            /* no break; */
        case 8:
            des->K1HR = DEU_ENDIAN_SWAP(*((u32 *) key + 0));
            des->K1LR = DEU_ENDIAN_SWAP(*((u32 *) key + 1));
            break;
        default:
            CRTCL_SECT_END;
            return;
        }
    }

    /* write init vector (not required for ECB mode) */
    if (mode > 0) {
        des->IVHR = DEU_ENDIAN_SWAP(*(u32 *) iv_arg);
        des->IVLR = DEU_ENDIAN_SWAP(*((u32 *) iv_arg + 1));
    }

#ifndef CONFIG_CRYPTO_DEV_IFXMIPS_DMA
    nblocks = nbytes / 4;

    for (i = 0; i < nblocks; i += 2) {
        /* wait for busy bit to clear */

        /*--- Workaround ----------------------------------------------------
        do a dummy read to the busy flag because it is not raised early
        enough in CFB/OFB 3DES modes */
#ifdef CRYPTO_DEBUG
        printk ("ihr: %x\n", (*((u32 *) in_arg + i)));
        printk ("ilr: %x\n", (*((u32 *) in_arg + 1 + i)));
#endif
        des->IHR = INPUT_ENDIAN_SWAP(*((u32 *) in_arg + i));
        des->ILR = INPUT_ENDIAN_SWAP(*((u32 *) in_arg + 1 + i)); /* start crypto */

        while (des->controlr.BUS) {
            /* this will not take long */
        }

        *((u32 *) out_arg + 0 + i) = des->OHR;
        *((u32 *) out_arg + 1 + i) = des->OLR;

#ifdef CRYPTO_DEBUG
        printk ("ohr: %x\n", (*((u32 *) out_arg + i)));
        printk ("olr: %x\n", (*((u32 *) out_arg + 1 + i)));
#endif
    }

#else /* dma mode */

    /* Prepare Rx buf length used in dma psuedo interrupt */
    //deu_priv->deu_rx_buf = out_arg;
    //deu_priv->deu_rx_len = nbytes;

    /* memory alignment issue */
    dword_mem_aligned_in = (u32 *) DEU_DWORD_REORDERING(in_arg, des_buff_in, BUFFER_IN, nbytes);

    dma->controlr.ALGO = 0;       //DES
    des->controlr.DAU = 0;
    dma->controlr.BS = 0;
    dma->controlr.EN = 1;

    while (des->controlr.BUS) {
        // wait for AES to be ready
    };

    wlen = dma_device_write (dma_device, (u8 *) dword_mem_aligned_in, nbytes, NULL);
    if (wlen != nbytes) {
        dma->controlr.EN = 0;
        CRTCL_SECT_END;
        printk (KERN_ERR "[%s %s %d]: dma_device_write fail!\n", __FILE__, __func__, __LINE__);
        return; // -EINVAL;
    }

    WAIT_DES_DMA_READY();

#ifdef CONFIG_CRYPTO_DEV_IFXMIPS_POLL_DMA

    outcopy = (u32 *) DEU_DWORD_REORDERING(out_arg, des_buff_out, BUFFER_OUT, nbytes);

    // polling DMA rx channel
    while ((dma_device_read (dma_device, (u8 **) &out_dma, NULL)) == 0) {
        timeout++;

        if (timeout >= 333000) {
            dma->controlr.EN = 0;
            CRTCL_SECT_END;
            printk (KERN_ERR "[%s %s %d]: timeout!!\n", __FILE__, __func__, __LINE__);
            return; // -EINVAL;
        }
    }

    WAIT_DES_DMA_READY();

    DES_MEMORY_COPY(outcopy, out_dma, out_arg, nbytes);

#else

    CRTCL_SECT_END;    /* Sleep and wait for Rx finished */
    DEU_WAIT_EVENT(deu_priv->deu_thread_wait, DEU_EVENT, deu_priv->deu_event_flags);
    CRTCL_SECT_START;

#endif

#endif /* dma mode */

    if (mode > 0) {
        *(u32 *) iv_arg = DEU_ENDIAN_SWAP(des->IVHR);
        *((u32 *) iv_arg + 1) = DEU_ENDIAN_SWAP(des->IVLR);
    };

    CRTCL_SECT_END;
}

//definitions from linux/include/crypto.h:
//#define CRYPTO_TFM_MODE_ECB           0x00000001
//#define CRYPTO_TFM_MODE_CBC           0x00000002
//#define CRYPTO_TFM_MODE_CFB           0x00000004
//#define CRYPTO_TFM_MODE_CTR           0x00000008
//#define CRYPTO_TFM_MODE_OFB           0x00000010 // not even defined
//but hardware definition: 0 ECB 1 CBC 2 OFB 3 CFB 4 CTR

/*! \fn void ifx_deu_des(void *ctx_arg, u8 *out_arg, const u8 *in_arg, u8 *iv_arg, u32 nbytes, int encdec, int mode)
 *  \ingroup IFX_DES_FUNCTIONS
 *  \brief main interface to DES hardware
 *  \param ctx_arg crypto algo context
 *  \param out_arg output bytestream
 *  \param in_arg input bytestream
 *  \param iv_arg initialization vector
 *  \param nbytes length of bytestream
 *  \param encdec 1 for encrypt; 0 for decrypt
 *  \param mode operation mode such as ebc, cbc
*/

#ifdef CONFIG_CRYPTO_DEV_IFXMIPS_DMA
void ifx_deu_des (void *ctx_arg, u8 *out_arg, const u8 *in_arg,
             u8 *iv_arg, u32 nbytes, int encdec, int mode)
{
    u32 remain = nbytes;
    u32 inc;

    DPRINTF(0, "\n");

    while (remain > 0)
    {
        if (remain >= DEU_MAX_PACKET_SIZE)
        {
            inc = DEU_MAX_PACKET_SIZE;
        }
        else
        {
            inc = remain;
        }

        remain -= inc;

        ifx_deu_des_core(ctx_arg, out_arg, in_arg, iv_arg, inc, encdec, mode);

        out_arg += inc;
        in_arg += inc;
    }
}
#endif


/*! \fn  void ifx_deu_des_ecb (void *ctx, uint8_t *dst, const uint8_t *src, uint8_t *iv, size_t nbytes, int encdec, int inplace)
 *  \ingroup IFX_DES_FUNCTIONS
 *  \brief sets DES hardware to ECB mode
 *  \param ctx crypto algo context
 *  \param dst output bytestream
 *  \param src input bytestream
 *  \param iv initialization vector
 *  \param nbytes length of bytestream
 *  \param encdec 1 for encrypt; 0 for decrypt
 *  \param inplace not used
*/

void ifx_deu_des_ecb (void *ctx, uint8_t *dst, const uint8_t *src,
                uint8_t *iv, size_t nbytes, int encdec, int inplace)
{
    DPRINTF(0, "ctx @%p\n", ctx);
    ifx_deu_des (ctx, dst, src, NULL, nbytes, encdec, 0);
}

/*! \fn  void ifx_deu_des_cbc (void *ctx, uint8_t *dst, const uint8_t *src, uint8_t *iv, size_t nbytes, int encdec, int inplace)
 *  \ingroup IFX_DES_FUNCTIONS
 *  \brief sets DES hardware to CBC mode
 *  \param ctx crypto algo context
 *  \param dst output bytestream
 *  \param src input bytestream
 *  \param iv initialization vector
 *  \param nbytes length of bytestream
 *  \param encdec 1 for encrypt; 0 for decrypt
 *  \param inplace not used
*/
void ifx_deu_des_cbc (void *ctx, uint8_t *dst, const uint8_t *src,
                uint8_t *iv, size_t nbytes, int encdec, int inplace)
{
    DPRINTF(0, "ctx @%p\n", ctx);
    ifx_deu_des (ctx, dst, src, iv, nbytes, encdec, 1);
}

/*! \fn  void ifx_deu_des_ofb (void *ctx, uint8_t *dst, const uint8_t *src, uint8_t *iv, size_t nbytes, int encdec, int inplace)
 *  \ingroup IFX_DES_FUNCTIONS
 *  \brief sets DES hardware to OFB mode
 *  \param ctx crypto algo context
 *  \param dst output bytestream
 *  \param src input bytestream
 *  \param iv initialization vector
 *  \param nbytes length of bytestream
 *  \param encdec 1 for encrypt; 0 for decrypt
 *  \param inplace not used
*/
void ifx_deu_des_ofb (void *ctx, uint8_t *dst, const uint8_t *src,
                uint8_t *iv, size_t nbytes, int encdec, int inplace)
{
    DPRINTF(0, "ctx @%p\n", ctx);
    ifx_deu_des (ctx, dst, src, iv, nbytes, encdec, 2);
}

/*! \fn void ifx_deu_des_cfb (void *ctx, uint8_t *dst, const uint8_t *src, uint8_t *iv, size_t nbytes, int encdec, int inplace)
    \ingroup IFX_DES_FUNCTIONS
    \brief sets DES hardware to CFB mode
    \param ctx crypto algo context
    \param dst output bytestream
    \param src input bytestream
    \param iv initialization vector
    \param nbytes length of bytestream
    \param encdec 1 for encrypt; 0 for decrypt
    \param inplace not used
*/
void ifx_deu_des_cfb (void *ctx, uint8_t *dst, const uint8_t *src,
                uint8_t *iv, size_t nbytes, int encdec, int inplace)
{
    DPRINTF(0, "ctx @%p\n", ctx);
    ifx_deu_des (ctx, dst, src, iv, nbytes, encdec, 3);
}

/*! \fn void ifx_deu_des_ctr (void *ctx, uint8_t *dst, const uint8_t *src, uint8_t *iv, size_t nbytes, int encdec, int inplace)
 *  \ingroup IFX_DES_FUNCTIONS
 *  \brief sets DES hardware to CTR mode
 *  \param ctx crypto algo context
 *  \param dst output bytestream
 *  \param src input bytestream
 *  \param iv initialization vector
 *  \param nbytes length of bytestream
 *  \param encdec 1 for encrypt; 0 for decrypt
 *  \param inplace not used
*/
void ifx_deu_des_ctr (void *ctx, uint8_t *dst, const uint8_t *src,
                uint8_t *iv, size_t nbytes, int encdec, int inplace)
{
    DPRINTF(0, "ctx @%p\n", ctx);
    ifx_deu_des (ctx, dst, src, iv, nbytes, encdec, 4);
}

/*! \fn void des_encrypt (struct crypto_tfm *tfm, uint8_t *out, const uint8_t *in)
 *  \ingroup IFX_DES_FUNCTIONS
 *  \brief encrypt DES_BLOCK_SIZE of data
 *  \param tfm linux crypto algo transform
 *  \param out output bytestream
 *  \param in input bytestream
*/
void des_encrypt (struct crypto_tfm *tfm, uint8_t * out, const uint8_t * in)
{
    struct des_ctx *ctx = crypto_tfm_ctx(tfm);
    DPRINTF(0, "ctx @%p\n", ctx);
    ifx_deu_des (ctx, out, in, NULL, DES_BLOCK_SIZE, CRYPTO_DIR_ENCRYPT, 0);
}

/*! \fn void des_decrypt (struct crypto_tfm *tfm, uint8_t *out, const uint8_t *in)
 *  \ingroup IFX_DES_FUNCTIONS
 *  \brief encrypt DES_BLOCK_SIZE of data
 *  \param tfm linux crypto algo transform
 *  \param out output bytestream
 *  \param in input bytestream
*/
void des_decrypt (struct crypto_tfm *tfm, uint8_t * out, const uint8_t * in)
{
    struct des_ctx *ctx = crypto_tfm_ctx(tfm);
    DPRINTF(0, "ctx @%p\n", ctx);
    ifx_deu_des (ctx, out, in, NULL, DES_BLOCK_SIZE, CRYPTO_DIR_DECRYPT, 0);
}

/*
 *   \brief RFC2451:
 *
 *   For DES-EDE3, there is no known need to reject weak or
 *   complementation keys.  Any weakness is obviated by the use of
 *   multiple keys.
 *
 *   However, if the first two or last two independent 64-bit keys are
 *   equal (k1 == k2 or k2 == k3), then the DES3 operation is simply the
 *   same as DES.  Implementers MUST reject keys that exhibit this
 *   property.
 *
 */

/*! \fn int des3_ede_setkey(struct crypto_tfm *tfm, const u8 *key, unsigned int keylen)
 *  \ingroup IFX_DES_FUNCTIONS
 *  \brief sets 3DES key
 *  \param tfm linux crypto algo transform
 *  \param key input key
 *  \param keylen key length
*/
int des3_ede_setkey(struct crypto_tfm *tfm, const u8 *key,
                    unsigned int key_len)
{
    struct des_ctx *ctx = crypto_tfm_ctx(tfm);

    DPRINTF(0, "ctx @%p, key_len %d\n", ctx, key_len);

    ctx->controlr_M = key_len / 8 + 1;      // 3DES EDE1 / EDE2 / EDE3 Mode
    ctx->key_length = key_len;

    memcpy ((u8 *) (ctx->expkey), key, key_len);

    return 0;
}

/*
 * \brief DES function mappings
*/
struct crypto_alg ifxdeu_des_alg = {
    .cra_name               =       "des",
    .cra_driver_name        =       "ifxdeu-des",
    .cra_flags              =       CRYPTO_ALG_TYPE_CIPHER,
    .cra_blocksize          =       DES_BLOCK_SIZE,
    .cra_ctxsize            =       sizeof(struct des_ctx),
    .cra_module             =       THIS_MODULE,
    .cra_alignmask          =       3,
    .cra_list               =       LIST_HEAD_INIT(ifxdeu_des_alg.cra_list),
    .cra_u                  =       { .cipher = {
    .cia_min_keysize        =       DES_KEY_SIZE,
    .cia_max_keysize        =       DES_KEY_SIZE,
    .cia_setkey             =       des_setkey,
    .cia_encrypt            =       des_encrypt,
    .cia_decrypt            =       des_decrypt } }
};

/*
 * \brief DES function mappings
*/
struct crypto_alg ifxdeu_des3_ede_alg = {
        .cra_name               =       "des3_ede",
        .cra_driver_name        =       "ifxdeu-des3_ede",
        .cra_flags              =       CRYPTO_ALG_TYPE_CIPHER,
        .cra_blocksize          =       DES_BLOCK_SIZE,
        .cra_ctxsize            =       sizeof(struct des_ctx),
        .cra_module             =       THIS_MODULE,
        .cra_alignmask          =       3,
        .cra_list               =       LIST_HEAD_INIT(ifxdeu_des3_ede_alg.cra_list),
        .cra_u                  =       { .cipher = {
        .cia_min_keysize        =       DES_KEY_SIZE,
        .cia_max_keysize        =       DES_KEY_SIZE,
        .cia_setkey             =       des3_ede_setkey,
        .cia_encrypt            =       des_encrypt,
        .cia_decrypt            =       des_decrypt } }
};

/*! \fn int ecb_des_encrypt(struct blkcipher_desc *desc, struct scatterlist *dst, struct scatterlist *src, unsigned int nbytes)
 *  \ingroup IFX_DES_FUNCTIONS
 *  \brief ECB DES encrypt using linux crypto blkcipher
 *  \param desc blkcipher descriptor
 *  \param dst output scatterlist
 *  \param src input scatterlist
 *  \param nbytes data size in bytes
*/
int ecb_des_encrypt(struct blkcipher_desc *desc,
                    struct scatterlist *dst, struct scatterlist *src,
                    unsigned int nbytes)
{
    struct des_ctx *ctx = crypto_blkcipher_ctx(desc->tfm);
    struct blkcipher_walk walk;
    int err;

    DPRINTF(0, "ctx @%p\n", ctx);

    blkcipher_walk_init(&walk, dst, src, nbytes);
    err = blkcipher_walk_virt(desc, &walk);

    while ((nbytes = walk.nbytes)) {
            nbytes -= (nbytes % DES_BLOCK_SIZE);
            ifx_deu_des_ecb(ctx, walk.dst.virt.addr, walk.src.virt.addr,
                           NULL, nbytes, CRYPTO_DIR_ENCRYPT, 0);
            nbytes &= DES_BLOCK_SIZE - 1;
            err = blkcipher_walk_done(desc, &walk, nbytes);
    }

    return err;
}

/*! \fn int ecb_des_decrypt(struct blkcipher_desc *desc, struct scatterlist *dst, struct scatterlist *src, unsigned int nbytes)
 *  \ingroup IFX_DES_FUNCTIONS
 *  \brief ECB DES decrypt using linux crypto blkcipher
 *  \param desc blkcipher descriptor
 *  \param dst output scatterlist
 *  \param src input scatterlist
 *  \param nbytes data size in bytes
 *  \return err
*/
int ecb_des_decrypt(struct blkcipher_desc *desc,
                    struct scatterlist *dst, struct scatterlist *src,
                    unsigned int nbytes)
{
    struct des_ctx *ctx = crypto_blkcipher_ctx(desc->tfm);
    struct blkcipher_walk walk;
    int err;

    DPRINTF(0, "ctx @%p\n", ctx);

    blkcipher_walk_init(&walk, dst, src, nbytes);
    err = blkcipher_walk_virt(desc, &walk);

    while ((nbytes = walk.nbytes)) {
            nbytes -= (nbytes % DES_BLOCK_SIZE);
            ifx_deu_des_ecb(ctx, walk.dst.virt.addr, walk.src.virt.addr,
                           NULL, nbytes, CRYPTO_DIR_DECRYPT, 0);
            nbytes &= DES_BLOCK_SIZE - 1;
            err = blkcipher_walk_done(desc, &walk, nbytes);
    }

    return err;
}

/*
 * \brief DES function mappings
*/
struct crypto_alg ifxdeu_ecb_des_alg = {
    .cra_name               =       "ecb(des)",
    .cra_driver_name        =       "ifxdeu-ecb(des)",
    .cra_flags              =       CRYPTO_ALG_TYPE_BLKCIPHER,
    .cra_blocksize          =       DES_BLOCK_SIZE,
    .cra_ctxsize            =       sizeof(struct des_ctx),
    .cra_type               =       &crypto_blkcipher_type,
    .cra_module             =       THIS_MODULE,
    .cra_list               =       LIST_HEAD_INIT(ifxdeu_ecb_des_alg.cra_list),
    .cra_u                  =       {
        .blkcipher = {
            .min_keysize            =       DES_KEY_SIZE,
            .max_keysize            =       DES_KEY_SIZE,
            .setkey                 =       des_setkey,
            .encrypt                =       ecb_des_encrypt,
            .decrypt                =       ecb_des_decrypt,
        }
    }
};

/*
 * \brief DES function mappings
*/
struct crypto_alg ifxdeu_ecb_des3_ede_alg = {
    .cra_name               =       "ecb(des3_ede)",
    .cra_driver_name        =       "ifxdeu-ecb(des3_ede)",
    .cra_flags              =       CRYPTO_ALG_TYPE_BLKCIPHER,
    .cra_blocksize          =       DES3_EDE_BLOCK_SIZE,
    .cra_ctxsize            =       sizeof(struct des_ctx),
    .cra_type               =       &crypto_blkcipher_type,
    .cra_module             =       THIS_MODULE,
    .cra_list               =       LIST_HEAD_INIT(ifxdeu_ecb_des3_ede_alg.cra_list),
    .cra_u                  =       {
        .blkcipher = {
            .min_keysize            =       DES3_EDE_KEY_SIZE,
            .max_keysize            =       DES3_EDE_KEY_SIZE,
            .setkey                 =       des3_ede_setkey,
            .encrypt                =       ecb_des_encrypt,
            .decrypt                =       ecb_des_decrypt,
        }
    }
};

/*! \fn int cbc_des_encrypt(struct blkcipher_desc *desc, struct scatterlist *dst, struct scatterlist *src, unsigned int nbytes)
 *  \ingroup IFX_DES_FUNCTIONS
 *  \brief CBC DES encrypt using linux crypto blkcipher
 *  \param desc blkcipher descriptor
 *  \param dst output scatterlist
 *  \param src input scatterlist
 *  \param nbytes data size in bytes
 *  \return err
*/
int cbc_des_encrypt(struct blkcipher_desc *desc,
                    struct scatterlist *dst, struct scatterlist *src,
                    unsigned int nbytes)
{
    struct des_ctx *ctx = crypto_blkcipher_ctx(desc->tfm);
    struct blkcipher_walk walk;
    int err;

    DPRINTF(0, "ctx @%p\n", ctx);

    blkcipher_walk_init(&walk, dst, src, nbytes);
    err = blkcipher_walk_virt(desc, &walk);

    while ((nbytes = walk.nbytes)) {
            u8 *iv = walk.iv;
            //printk("iv = %08x\n", *(u32 *)iv);
            nbytes -= (nbytes % DES_BLOCK_SIZE);
            ifx_deu_des_cbc(ctx, walk.dst.virt.addr, walk.src.virt.addr,
                           iv, nbytes, CRYPTO_DIR_ENCRYPT, 0);
            nbytes &= DES_BLOCK_SIZE - 1;
            err = blkcipher_walk_done(desc, &walk, nbytes);
    }

    return err;
}

/*! \fn int cbc_des_decrypt(struct blkcipher_desc *desc, struct scatterlist *dst, struct scatterlist *src, unsigned int nbytes)
 *  \ingroup IFX_DES_FUNCTIONS
 *  \brief CBC DES decrypt using linux crypto blkcipher
 *  \param desc blkcipher descriptor
 *  \param dst output scatterlist
 *  \param src input scatterlist
 *  \param nbytes data size in bytes
 *  \return err
*/
int cbc_des_decrypt(struct blkcipher_desc *desc,
                    struct scatterlist *dst, struct scatterlist *src,
                    unsigned int nbytes)
{
    struct des_ctx *ctx = crypto_blkcipher_ctx(desc->tfm);
    struct blkcipher_walk walk;
    int err;

    DPRINTF(0, "ctx @%p\n", ctx);

    blkcipher_walk_init(&walk, dst, src, nbytes);
    err = blkcipher_walk_virt(desc, &walk);

    while ((nbytes = walk.nbytes)) {
            u8 *iv = walk.iv;
            //printk("iv = %08x\n", *(u32 *)iv);
            nbytes -= (nbytes % DES_BLOCK_SIZE);
            ifx_deu_des_cbc(ctx, walk.dst.virt.addr, walk.src.virt.addr,
                           iv, nbytes, CRYPTO_DIR_DECRYPT, 0);
            nbytes &= DES_BLOCK_SIZE - 1;
            err = blkcipher_walk_done(desc, &walk, nbytes);
    }

    return err;
}

/*
 * \brief DES function mappings
*/
struct crypto_alg ifxdeu_cbc_des_alg = {
    .cra_name               =       "cbc(des)",
    .cra_driver_name        =       "ifxdeu-cbc(des)",
    .cra_flags              =       CRYPTO_ALG_TYPE_BLKCIPHER,
    .cra_blocksize          =       DES_BLOCK_SIZE,
    .cra_ctxsize            =       sizeof(struct des_ctx),
    .cra_type               =       &crypto_blkcipher_type,
    .cra_module             =       THIS_MODULE,
    .cra_list               =       LIST_HEAD_INIT(ifxdeu_cbc_des_alg.cra_list),
    .cra_u                  =       {
        .blkcipher = {
            .min_keysize            =       DES_KEY_SIZE,
            .max_keysize            =       DES_KEY_SIZE,
            .ivsize                 =       DES_BLOCK_SIZE,
            .setkey                 =       des_setkey,
            .encrypt                =       cbc_des_encrypt,
            .decrypt                =       cbc_des_decrypt,
        }
    }
};

/*
 * \brief DES function mappings
*/
struct crypto_alg ifxdeu_cbc_des3_ede_alg = {
    .cra_name               =       "cbc(des3_ede)",
    .cra_driver_name        =       "ifxdeu-cbc(des3_ede)",
    .cra_flags              =       CRYPTO_ALG_TYPE_BLKCIPHER,
    .cra_blocksize          =       DES3_EDE_BLOCK_SIZE,
    .cra_ctxsize            =       sizeof(struct des_ctx),
    .cra_type               =       &crypto_blkcipher_type,
    .cra_module             =       THIS_MODULE,
    .cra_list               =       LIST_HEAD_INIT(ifxdeu_cbc_des3_ede_alg.cra_list),
    .cra_u                  =       {
        .blkcipher = {
            .min_keysize            =       DES3_EDE_KEY_SIZE,
            .max_keysize            =       DES3_EDE_KEY_SIZE,
            .ivsize                 =       DES_BLOCK_SIZE,
            .setkey                 =       des3_ede_setkey,
            .encrypt                =       cbc_des_encrypt,
            .decrypt                =       cbc_des_decrypt,
        }
    }
};

/*! \fn int __init ifxdeu_init_des (void)
 *  \ingroup IFX_DES_FUNCTIONS
 *  \brief initialize des driver
*/
int __init ifxdeu_init_des (void)
{
    int ret = 0;

    ret = crypto_register_alg(&ifxdeu_des_alg);
    if (ret < 0)
        goto des_err;

    ret = crypto_register_alg(&ifxdeu_ecb_des_alg);
    if (ret < 0)
        goto ecb_des_err;

    ret = crypto_register_alg(&ifxdeu_cbc_des_alg);
    if (ret < 0)
        goto cbc_des_err;

    ret = crypto_register_alg(&ifxdeu_des3_ede_alg);
    if (ret < 0)
        goto des3_ede_err;

    ret = crypto_register_alg(&ifxdeu_ecb_des3_ede_alg);
    if (ret < 0)
        goto ecb_des3_ede_err;

    ret = crypto_register_alg(&ifxdeu_cbc_des3_ede_alg);
    if (ret < 0)
        goto cbc_des3_ede_err;

    des_chip_init();

    CRTCL_SECT_INIT;

#ifdef CONFIG_CRYPTO_DEV_IFXMIPS_DMA
    if (ALLOCATE_MEMORY(BUFFER_IN, DES_ALGO) < 0) {
        printk(KERN_ERR "[%s %s %d]: malloc memory fail!\n", __FILE__, __func__, __LINE__);
        goto cbc_des3_ede_err;
    }
    if (ALLOCATE_MEMORY(BUFFER_OUT, DES_ALGO) < 0) {
        printk(KERN_ERR "[%s %s %d]: malloc memory fail!\n", __FILE__, __func__, __LINE__);
        goto cbc_des3_ede_err;
    }
#endif

    printk (KERN_NOTICE "IFX DEU DES initialized %s.\n", disable_deudma ? "" : " (DMA)");
    return ret;

des_err:
    crypto_unregister_alg(&ifxdeu_des_alg);
    printk(KERN_ERR "IFX des initialization failed!\n");
    return ret;
ecb_des_err:
    crypto_unregister_alg(&ifxdeu_ecb_des_alg);
    printk (KERN_ERR "IFX ecb_des initialization failed!\n");
    return ret;
cbc_des_err:
    crypto_unregister_alg(&ifxdeu_cbc_des_alg);
    printk (KERN_ERR "IFX cbc_des initialization failed!\n");
    return ret;
des3_ede_err:
    crypto_unregister_alg(&ifxdeu_des3_ede_alg);
    printk(KERN_ERR "IFX des3_ede initialization failed!\n");
    return ret;
ecb_des3_ede_err:
    crypto_unregister_alg(&ifxdeu_ecb_des3_ede_alg);
    printk (KERN_ERR "IFX ecb_des3_ede initialization failed!\n");
    return ret;
cbc_des3_ede_err:
    crypto_unregister_alg(&ifxdeu_cbc_des3_ede_alg);
    printk (KERN_ERR "IFX cbc_des3_ede initialization failed!\n");
    return ret;
}

/*! \fn void __exit ifxdeu_fini_des (void)
 *  \ingroup IFX_DES_FUNCTIONS
 *  \brief unregister des driver
*/
void __exit ifxdeu_fini_des (void)
{
    crypto_unregister_alg (&ifxdeu_des_alg);
    crypto_unregister_alg (&ifxdeu_ecb_des_alg);
    crypto_unregister_alg (&ifxdeu_cbc_des_alg);
    crypto_unregister_alg (&ifxdeu_des3_ede_alg);
    crypto_unregister_alg (&ifxdeu_ecb_des3_ede_alg);
    crypto_unregister_alg (&ifxdeu_cbc_des3_ede_alg);

#ifdef CONFIG_CRYPTO_DEV_IFXMIPS_DMA
    FREE_MEMORY(des_buff_in);
    FREE_MEMORY(des_buff_out);
#endif /* CONFIG_CRYPTO_DEV_IFXMIPS_DMA_DANUBE */
}
