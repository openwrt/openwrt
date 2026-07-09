/*
 * drivers/crypto/al_crypto_skcipher.c
 *
 * Annapurna Labs Crypto driver - ablckcipher/aead algorithms
 *
 * Copyright (C) 2012 Annapurna Labs Ltd.
 *
 * Algorithm registration code and chained scatter/gather lists
 * handling based on caam driver.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include "linux/crypto.h"
#include "linux/export.h"
#include <crypto/algapi.h>
#include <crypto/internal/des.h>
#include <crypto/internal/skcipher.h>
#include <crypto/scatterwalk.h>
#include <linux/random.h>
#include <linux/rtnetlink.h>
#include <linux/scatterlist.h>

#include <mach/al_hal_ssm_crypto.h>

#include "al_crypto.h"

#define AL_CRYPTO_CRA_PRIORITY 300

static int al_crypto_init_tfm(struct crypto_skcipher *tfm);

static void al_crypto_exit_tfm(struct crypto_skcipher *tfm);

static int al_crypto_setkey_des(struct crypto_skcipher *tfm, const u8 *key,
                                unsigned int keylen);

static int al_crypto_setkey_aes(struct crypto_skcipher *tfm, const u8 *key,
                                unsigned int keylen);

static int al_crypto_do_crypt(struct skcipher_request *req, bool lock);

static int al_crypto_encrypt(struct skcipher_request *req);

static int al_crypto_decrypt(struct skcipher_request *req);

struct al_crypto_skcipher_req_ctx {
  enum al_crypto_dir dir;
  u8 *backup_iv;
  unsigned int remaining;
  unsigned int cryptlen;
  struct scatterlist *src;
  struct scatterlist *dst;
};

struct al_crypto_alg {
  struct list_head entry;
  struct al_crypto_device *device;
  enum al_crypto_sa_enc_type enc_type;
  enum al_crypto_sa_op sa_op;
  struct skcipher_alg alg;
};

struct al_crypto_skcipher_template {
  char name[CRYPTO_MAX_ALG_NAME];
  char driver_name[CRYPTO_MAX_ALG_NAME];
  unsigned int blocksize;
  enum al_crypto_sa_enc_type enc_type;
  enum al_crypto_sa_op sa_op;
  struct skcipher_alg alg;
};

static struct al_crypto_skcipher_template driver_algs[] = {
    {
        .name = "cbc(aes)",
        .driver_name = "cbc-aes-al",
        .blocksize = AES_BLOCK_SIZE,
        .enc_type = AL_CRYPT_AES_CBC,
        .sa_op = AL_CRYPT_ENC_ONLY,
        .alg =
            {
                .init = al_crypto_init_tfm,
                .exit = al_crypto_exit_tfm,
                .setkey = al_crypto_setkey_aes,
                .encrypt = al_crypto_encrypt,
                .decrypt = al_crypto_decrypt,
                .min_keysize = AES_MIN_KEY_SIZE,
                .max_keysize = AES_MAX_KEY_SIZE,
                .ivsize = AES_BLOCK_SIZE,
            },
    },
    {
        .name = "ecb(aes)",
        .driver_name = "ecb-aes-al",
        .blocksize = AES_BLOCK_SIZE,
        .enc_type = AL_CRYPT_AES_ECB,
        .sa_op = AL_CRYPT_ENC_ONLY,
        .alg =
            {
                .init = al_crypto_init_tfm,
                .exit = al_crypto_exit_tfm,
                .setkey = al_crypto_setkey_aes,
                .encrypt = al_crypto_encrypt,
                .decrypt = al_crypto_decrypt,
                .min_keysize = AES_MIN_KEY_SIZE,
                .max_keysize = AES_MAX_KEY_SIZE,
                .ivsize = AES_BLOCK_SIZE,
            },
    },
    {
        .name = "ctr(aes)",
        .driver_name = "ctr-aes-al",
        .blocksize = 1,
        .enc_type = AL_CRYPT_AES_CTR,
        .sa_op = AL_CRYPT_ENC_ONLY,
        .alg =
            {
                .init = al_crypto_init_tfm,
                .exit = al_crypto_exit_tfm,
                .setkey = al_crypto_setkey_aes,
                .encrypt = al_crypto_encrypt,
                .decrypt = al_crypto_decrypt,
                .min_keysize = AES_MIN_KEY_SIZE,
                .max_keysize = AES_MAX_KEY_SIZE,
                .ivsize = AES_BLOCK_SIZE,
            },
    },
    {
        .name = "cbc(des)",
        .driver_name = "cbc-des-al",
        .blocksize = DES_BLOCK_SIZE,
        .enc_type = AL_CRYPT_DES_CBC,
        .sa_op = AL_CRYPT_ENC_ONLY,
        .alg =
            {
                .init = al_crypto_init_tfm,
                .exit = al_crypto_exit_tfm,
                .setkey = al_crypto_setkey_des,
                .encrypt = al_crypto_encrypt,
                .decrypt = al_crypto_decrypt,
                .min_keysize = DES_KEY_SIZE,
                .max_keysize = DES_KEY_SIZE,
                .ivsize = DES_BLOCK_SIZE,
            },
    },
    {
        .name = "ecb(des)",
        .driver_name = "ecb-des-al",
        .blocksize = DES_BLOCK_SIZE,
        .enc_type = AL_CRYPT_DES_ECB,
        .sa_op = AL_CRYPT_ENC_ONLY,
        .alg =
            {
                .init = al_crypto_init_tfm,
                .exit = al_crypto_exit_tfm,
                .setkey = al_crypto_setkey_des,
                .encrypt = al_crypto_encrypt,
                .decrypt = al_crypto_decrypt,
                .min_keysize = DES_KEY_SIZE,
                .max_keysize = DES_KEY_SIZE,
                .ivsize = 0,
            },
    },
    {
        .name = "ecb(des3_ede)",
        .driver_name = "ecb-des3-ede-al",
        .blocksize = DES_BLOCK_SIZE,
        .enc_type = AL_CRYPT_TRIPDES_ECB,
        .sa_op = AL_CRYPT_ENC_ONLY,
        .alg =
            {
                .init = al_crypto_init_tfm,
                .exit = al_crypto_exit_tfm,
                .setkey = al_crypto_setkey_des,
                .encrypt = al_crypto_encrypt,
                .decrypt = al_crypto_decrypt,
                .min_keysize = DES3_EDE_KEY_SIZE,
                .max_keysize = DES3_EDE_KEY_SIZE,
                .ivsize = 0,
            },
    },
    {
        .name = "cbc(des3_ede)",
        .driver_name = "cbc-des3-ede-al",
        .blocksize = DES_BLOCK_SIZE,
        .enc_type = AL_CRYPT_TRIPDES_CBC,
        .sa_op = AL_CRYPT_ENC_ONLY,
        .alg =
            {
                .init = al_crypto_init_tfm,
                .exit = al_crypto_exit_tfm,
                .setkey = al_crypto_setkey_des,
                .encrypt = al_crypto_encrypt,
                .decrypt = al_crypto_decrypt,
                .min_keysize = DES3_EDE_KEY_SIZE,
                .max_keysize = DES3_EDE_KEY_SIZE,
                .ivsize = DES_BLOCK_SIZE,
            },
    },
};

/******************************************************************************
 *****************************************************************************/
static int al_crypto_init_tfm(struct crypto_skcipher *tfm) {
  struct al_crypto_ctx *ctx = crypto_skcipher_ctx(tfm);
  struct skcipher_alg *alg = crypto_skcipher_alg(tfm);
  struct al_crypto_alg *al_crypto_alg =
      container_of(alg, struct al_crypto_alg, alg);
  struct al_crypto_device *device = al_crypto_alg->device;
  int chan_idx = atomic_inc_return(&device->tfm_count) %
                 (device->num_channels - device->crc_channels);

  dev_dbg(&device->pdev->dev, "%s: cra_name=%s\n", __func__,
          crypto_tfm_alg_name(crypto_skcipher_tfm(tfm)));

  memset(ctx, 0, sizeof(struct al_crypto_ctx));

  ctx->chan = device->channels[chan_idx];

  ctx->sa.enc_type = al_crypto_alg->enc_type;
  ctx->sa.sa_op = al_crypto_alg->sa_op;

  ctx->hw_sa =
      dma_alloc_coherent(&device->pdev->dev, sizeof(struct al_crypto_hw_sa),
                         &ctx->hw_sa_dma_addr, GFP_KERNEL);

  crypto_skcipher_set_reqsize(tfm, sizeof(struct al_crypto_skcipher_req_ctx));

  ctx->iv = dma_alloc_coherent(&device->pdev->dev, AL_CRYPTO_MAX_IV_LENGTH,
                               &ctx->iv_dma_addr, GFP_KERNEL);

  AL_CRYPTO_STATS_LOCK(&ctx->chan->stats_gen_lock);
  AL_CRYPTO_STATS_INC(ctx->chan->stats_gen.skcipher_tfms, 1);
  AL_CRYPTO_STATS_UNLOCK(&ctx->chan->stats_gen_lock);

  return 0;
}

/******************************************************************************
 *****************************************************************************/
static void al_crypto_exit_tfm(struct crypto_skcipher *tfm) {
  struct al_crypto_ctx *ctx = crypto_skcipher_ctx(tfm);
  struct skcipher_alg *alg = crypto_skcipher_alg(tfm);
  struct al_crypto_alg *al_crypto_alg =
      container_of(alg, struct al_crypto_alg, alg);
  struct al_crypto_device *device = al_crypto_alg->device;

  dev_dbg(&device->pdev->dev, "%s: cra_name=%s\n", __func__,
          crypto_tfm_alg_name(crypto_skcipher_tfm(tfm)));

  /* LRU list access has to be protected */
  spin_lock_bh(&ctx->chan->prep_lock);
  if (ctx->cache_state.cached)
    al_crypto_cache_remove_lru(ctx->chan, &ctx->cache_state);
  spin_unlock_bh(&ctx->chan->prep_lock);

  if (ctx->hw_sa_dma_addr)
    dma_free_coherent(&device->pdev->dev, sizeof(struct al_crypto_hw_sa),
                      ctx->hw_sa, ctx->hw_sa_dma_addr);

  if (ctx->iv_dma_addr)
    dma_free_coherent(&device->pdev->dev, AL_CRYPTO_MAX_IV_LENGTH, ctx->iv,
                      ctx->iv_dma_addr);

  AL_CRYPTO_STATS_LOCK(&ctx->chan->stats_gen_lock);
  AL_CRYPTO_STATS_DEC(ctx->chan->stats_gen.skcipher_tfms, 1);
  AL_CRYPTO_STATS_UNLOCK(&ctx->chan->stats_gen_lock);
}

/******************************************************************************
 *****************************************************************************/
static int al_crypto_setkey_des(struct crypto_skcipher *tfm, const u8 *key,
                                unsigned int keylen) {
  struct al_crypto_ctx *ctx = crypto_skcipher_ctx(tfm);
  int ret;

  dev_dbg(to_dev(ctx->chan), "%s: keylen=%d\n", __func__, keylen);
  print_hex_dump_debug(KBUILD_MODNAME ": key: ", DUMP_PREFIX_OFFSET, 16, 1, key,
                       keylen, false);

  if ((ctx->sa.enc_type == AL_CRYPT_TRIPDES_CBC) ||
      (ctx->sa.enc_type == AL_CRYPT_TRIPDES_ECB)) {
    ctx->sa.tripdes_m = AL_CRYPT_TRIPDES_EDE;
    if (keylen != DES3_EDE_KEY_SIZE)
      return -EINVAL;
  } else {
    ctx->sa.tripdes_m = 0;
    if (keylen != DES_KEY_SIZE)
      return -EINVAL;

    /* check for weak keys. */
    /* Weak keys are keys that cause the encryption mode of DES
     * to act identically to the decryption mode of DES */
    ret = verify_skcipher_des_key(tfm, key);
    if (ret)
      return ret;
  }

  /* TODO: optimize HAL to hold ptrs to save this memcpy */
  /* copy the key to the sa */
  memcpy(&ctx->sa.enc_key, key, keylen);

  al_crypto_hw_sa_init(&ctx->sa, ctx->hw_sa);

  /* mark the sa as not cached, will update in next xaction */
  spin_lock_bh(&ctx->chan->prep_lock);
  if (ctx->cache_state.cached)
    al_crypto_cache_remove_lru(ctx->chan, &ctx->cache_state);
  spin_unlock_bh(&ctx->chan->prep_lock);

  return 0;
}

/******************************************************************************
 *****************************************************************************/
static int al_crypto_setkey_aes(struct crypto_skcipher *tfm, const u8 *key,
                                unsigned int keylen) {
  struct al_crypto_ctx *ctx = crypto_skcipher_ctx(tfm);

  dev_dbg(to_dev(ctx->chan), "%s: keylen=%d\n", __func__, keylen);
  print_hex_dump_debug(KBUILD_MODNAME ": key: ", DUMP_PREFIX_OFFSET, 16, 1, key,
                       keylen, false);

  if (al_crypto_keylen_to_sa_aes_ksize(keylen, &ctx->sa.aes_ksize))
    return -EINVAL;

  /* As for now we don't support GCM or CCM modes */
  BUG_ON((ctx->sa.enc_type == AL_CRYPT_AES_GCM) ||
         (ctx->sa.enc_type == AL_CRYPT_AES_CCM));

  /* TODO: optimize HAL to hold ptrs to save this memcpy */
  /* copy the key to the sa */
  memcpy(&ctx->sa.enc_key, key, keylen);

  /* Sets the counter increment to 128 bit to be aligned with the
   * linux implementation. We know it contradicts the NIST spec.
   * If and when the linux will be aligned with the spec we should fix it
   * too.
   * This variable is relevant only for CTR, GCM and CCM modes*/
  ctx->sa.cntr_size = AL_CRYPT_CNTR_128_BIT;

  al_crypto_hw_sa_init(&ctx->sa, ctx->hw_sa);

  /* mark the sa as not cached, will update in next xaction */
  spin_lock_bh(&ctx->chan->prep_lock);
  if (ctx->cache_state.cached)
    al_crypto_cache_remove_lru(ctx->chan, &ctx->cache_state);
  spin_unlock_bh(&ctx->chan->prep_lock);

  return 0;
}

/******************************************************************************
 *****************************************************************************/
/* DMA unmap buffers for skcipher request
 */
static inline void al_crypto_dma_unmap(struct al_crypto_chan *chan,
                                       struct skcipher_request *req,
                                       int src_nents, int dst_nents,
                                       struct al_crypto_sw_desc *desc) {
  struct al_crypto_skcipher_req_ctx *rctx = skcipher_request_ctx(req);

  dev_dbg(to_dev(chan), "%s: src_nents=%d dst_nents=%d\n", __func__, src_nents,
          dst_nents);

  if (likely(rctx->src == rctx->dst)) {
    dma_unmap_sg(to_dev(chan), rctx->src, src_nents, DMA_BIDIRECTIONAL);
  } else {
    dma_unmap_sg(to_dev(chan), rctx->src, src_nents, DMA_TO_DEVICE);
    dma_unmap_sg(to_dev(chan), rctx->dst, dst_nents, DMA_FROM_DEVICE);
  }

  if (desc && desc->hal_xaction.enc_iv_in.len)
    dma_unmap_single(to_dev(chan), desc->hal_xaction.enc_iv_in.addr,
                     desc->hal_xaction.enc_iv_in.len, DMA_TO_DEVICE);
}

/******************************************************************************
 *****************************************************************************/
static inline void al_crypto_update_stats(struct al_crypto_transaction *xaction,
                                          struct al_crypto_chan *chan) {
  if (xaction->dir == AL_CRYPT_ENCRYPT) {
    AL_CRYPTO_STATS_INC(chan->stats_prep.skcipher_encrypt_reqs, 1);
    AL_CRYPTO_STATS_INC(chan->stats_prep.skcipher_encrypt_bytes,
                        xaction->enc_in_len);
  } else {
    AL_CRYPTO_STATS_INC(chan->stats_prep.skcipher_decrypt_reqs, 1);
    AL_CRYPTO_STATS_INC(chan->stats_prep.skcipher_decrypt_bytes,
                        xaction->enc_in_len);
  }

  if (xaction->enc_in_len <= 512)
    AL_CRYPTO_STATS_INC(chan->stats_prep.skcipher_reqs_le512, 1);
  else if ((xaction->enc_in_len > 512) && (xaction->enc_in_len <= 2048))
    AL_CRYPTO_STATS_INC(chan->stats_prep.skcipher_reqs_512_2048, 1);
  else if ((xaction->enc_in_len > 2048) && (xaction->enc_in_len <= 4096))
    AL_CRYPTO_STATS_INC(chan->stats_prep.skcipher_reqs_2048_4096, 1);
  else
    AL_CRYPTO_STATS_INC(chan->stats_prep.skcipher_reqs_gt4096, 1);
}

/******************************************************************************
 *****************************************************************************/
static inline void
al_crypto_prepare_xaction_buffers(struct skcipher_request *req,
                                  struct al_crypto_sw_desc *desc) {
  struct al_crypto_skcipher_req_ctx *rctx = skcipher_request_ctx(req);
  struct al_crypto_transaction *xaction = &desc->hal_xaction;
  int src_idx, dst_idx;
  int i;

  src_idx = 0;
  dst_idx = 0;

  sg_map_to_xaction_buffers(rctx->src, desc->src_bufs, rctx->cryptlen,
                            &src_idx);

  if (likely(rctx->src == rctx->dst)) {
    for (i = 0; i < src_idx; i++)
      desc->dst_bufs[i] = desc->src_bufs[i];
    dst_idx = src_idx;
  } else
    sg_map_to_xaction_buffers(rctx->dst, desc->dst_bufs, rctx->cryptlen,
                              &dst_idx);

  xaction->src_size = xaction->enc_in_len = rctx->cryptlen;
  xaction->src.bufs = &desc->src_bufs[0];
  xaction->src.num = src_idx;
  xaction->dst.bufs = &desc->dst_bufs[0];
  xaction->dst.num = dst_idx;
}

/******************************************************************************
 *****************************************************************************/
/* Prepare crypto transaction to be processed by HAL and submit to HAL
 * Grabs and releases producer lock for relevant sw ring
 */
static int al_crypto_do_crypt(struct skcipher_request *req, bool lock) {
  struct crypto_skcipher *tfm = crypto_skcipher_reqtfm(req);
  struct al_crypto_ctx *ctx = crypto_skcipher_ctx(tfm);
  struct al_crypto_skcipher_req_ctx *rctx = skcipher_request_ctx(req);
  enum al_crypto_dir dir = rctx->dir;
  struct al_crypto_chan *chan = ctx->chan;
  struct al_crypto_transaction *xaction;
  int src_nents = 0, dst_nents = 0;
  int ivsize = crypto_skcipher_ivsize(tfm);
  gfp_t gfp_flags =
      (req->base.flags & CRYPTO_TFM_REQ_MAY_SLEEP) ? GFP_KERNEL : GFP_ATOMIC;
  struct al_crypto_sw_desc *desc;
  int idx, rc;

  dev_dbg(to_dev(chan),
          "%s: chan->idx=%d lock=%d cryptlen=%d remaining=%d ivsize=%d "
          "(req->src %s req->dst) src_nents=%d dst_nents=%d\n",
          __func__, chan->idx, lock, req->cryptlen, rctx->remaining, ivsize,
          req->src == req->dst ? "==" : "!=", sg_nents(req->src),
          sg_nents(req->dst));

  if (req->cryptlen <= U16_MAX) {
    rctx->cryptlen = rctx->remaining;
    rctx->src = req->src;
    rctx->dst = req->dst;
  } else {
    struct scatterlist *sg[1];
    size_t sizes[1];
    off_t skip;

    rctx->cryptlen = min(rctx->remaining,
                         ALIGN_DOWN(U16_MAX, crypto_skcipher_blocksize(tfm)));
    sizes[0] = rctx->cryptlen;
    skip = req->cryptlen - rctx->remaining;

    dev_dbg(to_dev(chan), "%s: sizes=%d skip=%ld\n", __func__, sizes[0], skip);

    rc = sg_split(req->src, 0, skip, ARRAY_SIZE(sizes), sizes, sg, NULL,
                  gfp_flags);
    if (rc)
      goto fail_free_sgls;

    rctx->src = sg[0];

    if (req->dst == req->src) {
      rctx->dst = rctx->src;
    } else {
      rc = sg_split(req->dst, 0, skip, ARRAY_SIZE(sizes), sizes, sg, NULL,
                    gfp_flags);
      if (rc)
        goto fail_free_sgls;

      rctx->dst = sg[0];
    }
  }

  if (dir == AL_CRYPT_DECRYPT || ctx->sa.enc_type == AL_CRYPT_AES_CTR) {
    if (rctx->src == rctx->dst) {
      rctx->backup_iv = kmalloc(ivsize, gfp_flags);
      if (!rctx->backup_iv) {
        rc = -ENOMEM;
        goto fail_free_sgls;
      }

      BUG_ON(ctx->sa.enc_type != AL_CRYPT_AES_CTR && rctx->cryptlen < ivsize);

      if (rctx->cryptlen >= ivsize)
        scatterwalk_map_and_copy(rctx->backup_iv, rctx->src,
                                 ALIGN_DOWN(rctx->cryptlen - ivsize, ivsize),
                                 ivsize, 0);
    }
  }

  print_hex_dump_debug(KBUILD_MODNAME ": iv: ", DUMP_PREFIX_OFFSET, 16, 1,
                       req->iv, ivsize, false);

  src_nents = sg_nents_for_len(rctx->src, rctx->cryptlen);
  if (rctx->dst != rctx->src)
    dst_nents = sg_nents_for_len(rctx->dst, rctx->cryptlen);
  else
    dst_nents = src_nents;

  dev_dbg(to_dev(chan),
          "%s: src_nents_for_len=%d dst_nents_for_len=%d cryptlen=%d "
          "(rctx->src %s rctx->dst) src_nents=%d dst_nents=%d\n",
          __func__, src_nents, dst_nents, rctx->cryptlen,
          rctx->src == rctx->dst ? "==" : "!=", sg_nents(rctx->src),
          sg_nents(rctx->dst));

  if (unlikely(src_nents < 0)) {
    dev_err(to_dev(chan), "Insufficient bytes (%d) in src S/G\n",
            rctx->cryptlen);
    rc = src_nents;
    goto fail_free_sgls;
  }
  if (unlikely(dst_nents < 0)) {
    dev_err(to_dev(chan), "Insufficient bytes (%d) in dst S/G\n",
            rctx->cryptlen);
    rc = dst_nents;
    goto fail_free_sgls;
  }

  /* Currently supported max sg chain length is
   * AL_CRYPTO_OP_MAX_DATA_BUFS(12) which is minimum of descriptors left
   * for data in a transaction:
   * tx: 31(supported by HW) - 1(metadata) - 1(sa_in) -
   *			1(enc_iv_in|auth_iv_in) - 1(auth_sign_in) = 27
   * rx: 31(supported by HW) - 1(sa_out) - 1(enc_iv_out|auth_iv_out) -
   *			1(next_enc_iv_out) - 1(auth_sign_out) = 27
   */
  BUG_ON((src_nents > AL_CRYPTO_OP_MAX_BUFS) ||
         (dst_nents > AL_CRYPTO_OP_MAX_BUFS));

  if (likely(rctx->src == rctx->dst)) {
    dma_map_sg(to_dev(chan), rctx->src, src_nents, DMA_BIDIRECTIONAL);
  } else {
    dma_map_sg(to_dev(chan), rctx->src, src_nents, DMA_TO_DEVICE);
    dma_map_sg(to_dev(chan), rctx->dst, dst_nents, DMA_FROM_DEVICE);
  }

  if (likely(lock))
    spin_lock_bh(&chan->prep_lock);

  if (likely(al_crypto_get_sw_desc(chan, 1) == 0))
    idx = chan->head;
  else {
    rc = crypto_enqueue_request(&chan->sw_queue, &req->base);
    goto fail_dma_unmap;
  }

  chan->sw_desc_num_locked = 1;
  chan->tx_desc_produced = 0;

  desc = al_crypto_get_ring_ent(chan, idx);
  desc->req = (void *)req;
  desc->req_type = AL_CRYPTO_REQ_SKCIPHER;
  desc->src_nents = src_nents;
  desc->dst_nents = dst_nents;

  /* prepare hal transaction */
  xaction = &desc->hal_xaction;
  memset(xaction, 0, sizeof(struct al_crypto_transaction));
  xaction->dir = dir;

  al_crypto_prepare_xaction_buffers(req, desc);

  if ((ctx->sa.enc_type != AL_CRYPT_AES_ECB) &&
      (ctx->sa.enc_type != AL_CRYPT_DES_ECB) &&
      (ctx->sa.enc_type != AL_CRYPT_TRIPDES_ECB)) {
    xaction->enc_iv_in.addr =
        dma_map_single(to_dev(chan), req->iv, ivsize, DMA_TO_DEVICE);
    if (dma_mapping_error(to_dev(chan), xaction->enc_iv_in.addr)) {
      rc = -ENOMEM;
      goto fail_dma_unmap;
    }
    xaction->enc_iv_in.len = ivsize;

    xaction->enc_next_iv_out.addr = ctx->iv_dma_addr;
    xaction->enc_next_iv_out.len = ivsize;
  }

  if (!ctx->cache_state.cached) {
    xaction->sa_indx =
        al_crypto_cache_replace_lru(chan, &ctx->cache_state, NULL);
    xaction->sa_in.addr = ctx->hw_sa_dma_addr;
    xaction->sa_in.len = sizeof(struct al_crypto_hw_sa);
  } else {
    al_crypto_cache_update_lru(chan, &ctx->cache_state);
    xaction->sa_indx = ctx->cache_state.idx;
  }

  xaction->flags = AL_SSM_INTERRUPT;

  al_crypto_update_stats(xaction, chan);

  /* send crypto transaction to engine */
  rc = al_crypto_dma_prepare(chan->hal_crypto, chan->idx, &desc->hal_xaction);
  if (unlikely(rc != 0))
    goto fail_dma_unmap;

  chan->tx_desc_produced += desc->hal_xaction.tx_descs_count;

  dev_dbg(to_dev(chan),
          "%s: chan->idx=%d dir=%d ctx->sa.enc_type=%d "
          "ctx->cache_state.cached=%d tx_descs_count=%d\n",
          __func__, chan->idx, dir, ctx->sa.enc_type, ctx->cache_state.cached,
          desc->hal_xaction.tx_descs_count);

  al_crypto_tx_submit(chan);

  if (likely(lock))
    spin_unlock_bh(&chan->prep_lock);

  return -EINPROGRESS;

fail_dma_unmap:

  al_crypto_dma_unmap(chan, req, src_nents, dst_nents, desc);

  if (likely(lock))
    spin_unlock_bh(&chan->prep_lock);

fail_free_sgls:
  if (rctx->src != NULL && rctx->src != req->src) {
    kfree(rctx->src);
    rctx->src = NULL;
  }
  if (rctx->dst != NULL && rctx->dst != req->dst) {
    kfree(rctx->dst);
    rctx->dst = NULL;
  }

  return rc;
}

/******************************************************************************
 *****************************************************************************/
static int al_crypto_encrypt(struct skcipher_request *req) {
  struct crypto_skcipher *tfm = crypto_skcipher_reqtfm(req);
  struct al_crypto_ctx *ctx = crypto_skcipher_ctx(tfm);
  struct al_crypto_skcipher_req_ctx *rctx = skcipher_request_ctx(req);
  struct al_crypto_chan *chan = ctx->chan;
  unsigned int blksize = crypto_skcipher_blocksize(tfm);

  dev_dbg(to_dev(chan), "%s: cryptlen=%d\n", __func__, req->cryptlen);
  print_hex_dump_debug(KBUILD_MODNAME ": iv: ", DUMP_PREFIX_OFFSET, 16, 1,
                       req->iv, crypto_skcipher_ivsize(tfm), false);

  if (req->cryptlen < blksize)
    return -EINVAL;

  rctx->dir = AL_CRYPT_ENCRYPT;
  rctx->remaining = req->cryptlen;

  return al_crypto_do_crypt(req, true);
}

/******************************************************************************
 *****************************************************************************/
static int al_crypto_decrypt(struct skcipher_request *req) {
  struct crypto_skcipher *tfm = crypto_skcipher_reqtfm(req);
  struct al_crypto_ctx *ctx = crypto_skcipher_ctx(tfm);
  struct al_crypto_skcipher_req_ctx *rctx = skcipher_request_ctx(req);
  struct al_crypto_chan *chan = ctx->chan;
  unsigned int blksize = crypto_skcipher_blocksize(tfm);

  dev_dbg(to_dev(chan), "%s: cryptlen=%d\n", __func__, req->cryptlen);
  print_hex_dump_debug(KBUILD_MODNAME ": iv: ", DUMP_PREFIX_OFFSET, 16, 1,
                       req->iv, crypto_skcipher_ivsize(tfm), false);

  if (req->cryptlen < blksize)
    return -EINVAL;

  rctx->dir = AL_CRYPT_DECRYPT;
  rctx->remaining = req->cryptlen;

  return al_crypto_do_crypt(req, true);
}

/******************************************************************************
 *****************************************************************************/
static struct al_crypto_alg *
al_crypto_alg_alloc(struct al_crypto_device *device,
                    struct al_crypto_skcipher_template *template) {
  struct al_crypto_alg *t_alg;
  struct skcipher_alg *alg;

  t_alg = kzalloc(sizeof(struct al_crypto_alg), GFP_KERNEL);
  if (!t_alg) {
    dev_err(&device->pdev->dev, "failed to allocate t_alg\n");
    return ERR_PTR(-ENOMEM);
  }

  alg = &t_alg->alg;
  *alg = template->alg;

  snprintf(alg->base.cra_name, CRYPTO_MAX_ALG_NAME, "%s", template->name);
  snprintf(alg->base.cra_driver_name, CRYPTO_MAX_ALG_NAME, "%s",
           template->driver_name);

  alg->base.cra_module = THIS_MODULE;
  alg->base.cra_priority = AL_CRYPTO_CRA_PRIORITY;
  alg->base.cra_blocksize = template->blocksize;
  alg->base.cra_alignmask = 0;
  alg->base.cra_ctxsize = sizeof(struct al_crypto_ctx);
  alg->base.cra_flags = CRYPTO_ALG_ASYNC | CRYPTO_ALG_KERN_DRIVER_ONLY;

  alg->chunksize = alg->base.cra_blocksize;

  t_alg->enc_type = template->enc_type;
  t_alg->sa_op = template->sa_op;
  t_alg->device = device;

  return t_alg;
}

/******************************************************************************
 *****************************************************************************/
static void al_crypto_req_iv_out(struct skcipher_request *req) {
  struct crypto_skcipher *tfm = crypto_skcipher_reqtfm(req);
  struct al_crypto_skcipher_req_ctx *rctx = skcipher_request_ctx(req);
  unsigned int ivsize = crypto_skcipher_ivsize(tfm);

  if (rctx->dir == AL_CRYPT_ENCRYPT) {
    BUG_ON(rctx->cryptlen < ivsize);
    scatterwalk_map_and_copy(req->iv, rctx->dst, rctx->cryptlen - ivsize,
                             ivsize, 0);
  } else {
    if (rctx->src == rctx->dst) {
      memcpy(req->iv, rctx->backup_iv, ivsize);
      kfree(rctx->backup_iv);
    } else {
      BUG_ON(rctx->cryptlen < ivsize);
      scatterwalk_map_and_copy(req->iv, rctx->src, rctx->cryptlen - ivsize,
                               ivsize, 0);
    }
  }
}

/******************************************************************************
 *****************************************************************************/
static void al_crypto_req_aes_ctr_iv_out(struct skcipher_request *req) {
  struct crypto_skcipher *tfm = crypto_skcipher_reqtfm(req);
  struct al_crypto_ctx *ctx = crypto_skcipher_ctx(tfm);
  struct al_crypto_skcipher_req_ctx *rctx = skcipher_request_ctx(req);
  unsigned int ivsize = crypto_skcipher_ivsize(tfm);
  unsigned int keylen;
  u8 pt[AES_BLOCK_SIZE];

  BUG_ON(al_crypto_sa_aes_ksize_to_keylen(ctx->sa.aes_ksize, &keylen));

  if (rctx->cryptlen >= ivsize) {
    if (rctx->src == rctx->dst) {
      memcpy(pt, rctx->backup_iv, ivsize);
      kfree(rctx->backup_iv);
    } else {
      scatterwalk_map_and_copy(pt, rctx->src,
                               ALIGN_DOWN(rctx->cryptlen - ivsize, ivsize),
                               ivsize, 0);
    }

    scatterwalk_map_and_copy(req->iv, rctx->dst,
                             ALIGN_DOWN(rctx->cryptlen - ivsize, ivsize),
                             ivsize, 0);

    print_hex_dump_debug(KBUILD_MODNAME ": aes-ctr pt: ", DUMP_PREFIX_OFFSET,
                         16, 1, pt, ivsize, false);
    print_hex_dump_debug(KBUILD_MODNAME ": aes-ctr ct: ", DUMP_PREFIX_OFFSET,
                         16, 1, req->iv, ivsize, false);

    crypto_xor(req->iv, pt, ivsize);
    aes_expandkey(&ctx->aes_key, ctx->sa.enc_key, keylen);
    aes_decrypt(&ctx->aes_key, req->iv, req->iv);
    crypto_inc(req->iv, ivsize);
  }

  if (rctx->cryptlen & (ivsize - 1))
    crypto_inc(req->iv, ivsize);
}

/******************************************************************************
 *****************************************************************************/
int al_crypto_skcipher_process_queue(struct al_crypto_chan *chan) {
  struct crypto_async_request *async_req, *backlog;
  struct skcipher_request *req;
  int err = 0;

  spin_lock_bh(&chan->prep_lock);

  while (al_crypto_ring_space(chan) > 0) {
    backlog = crypto_get_backlog(&chan->sw_queue);
    async_req = crypto_dequeue_request(&chan->sw_queue);

    dev_dbg(to_dev(chan), "%s: backlog=%d asyn_req=%d\n", __func__, !!backlog,
            !!async_req);

    if (!async_req)
      break;

    if (backlog)
      backlog->complete(backlog, -EINPROGRESS);

    req = container_of(async_req, struct skcipher_request, base);

    err = al_crypto_do_crypt(req, false);
    if (err != -EINPROGRESS)
      break;
  }

  spin_unlock_bh(&chan->prep_lock);

  return err;
}

/******************************************************************************
 *****************************************************************************/
/* Cleanup single skcipher request - invoked from cleanup tasklet (interrupt
 * handler)
 */
void al_crypto_skcipher_cleanup_single(struct al_crypto_chan *chan,
                                       struct al_crypto_sw_desc *desc,
                                       uint32_t comp_status) {
  struct skcipher_request *req = (struct skcipher_request *)desc->req;
  struct crypto_skcipher *tfm = crypto_skcipher_reqtfm(req);
  struct al_crypto_ctx *ctx = crypto_skcipher_ctx(tfm);
  struct al_crypto_skcipher_req_ctx *rctx = skcipher_request_ctx(req);
  unsigned int ivsize = crypto_skcipher_ivsize(tfm);
  int rc;

  dev_dbg(to_dev(chan), "%s: chan->idx=%d comp_status=%x\n", __func__,
          chan->idx, comp_status);

  al_crypto_dma_unmap(chan, req, desc->src_nents, desc->dst_nents, desc);

  if (ctx->sa.enc_type != AL_CRYPT_AES_CTR)
    al_crypto_req_iv_out(req);
  else
    al_crypto_req_aes_ctr_iv_out(req);

  print_hex_dump_debug(KBUILD_MODNAME ": out iv: ", DUMP_PREFIX_OFFSET, 16, 1,
                       req->iv, ivsize, false);

  if (rctx->src != NULL && rctx->src != req->src) {
    kfree(rctx->src);
    rctx->src = NULL;
  }
  if (rctx->dst != NULL && rctx->dst != req->dst) {
    kfree(rctx->dst);
    rctx->dst = NULL;
  }

  rctx->remaining -= rctx->cryptlen;

  if (rctx->remaining > 0) {
    rc = al_crypto_do_crypt(req, false);
    if (rc != -EINPROGRESS) {
      dev_err(to_dev(chan), "al_crypto_do_crypt failed with %d\n", rc);
      req->base.complete(&req->base, rc);
    }
    return;
  }

  req->base.complete(&req->base, 0);
}

/******************************************************************************
 *****************************************************************************/
int al_crypto_skcipher_init(struct al_crypto_device *device) {
  int err = 0;
  int i;

  INIT_LIST_HEAD(&device->skcipher_list);

  atomic_set(&device->tfm_count, -1);

  /* register crypto algorithms the device supports */
  for (i = 0; i < ARRAY_SIZE(driver_algs); i++) {
    struct al_crypto_alg *t_alg;

    t_alg = al_crypto_alg_alloc(device, &driver_algs[i]);
    if (IS_ERR(t_alg)) {
      err = PTR_ERR(t_alg);
      dev_warn(&device->pdev->dev, "%s alg allocation failed with %d\n",
               driver_algs[i].driver_name, err);
      continue;
    }

    err = crypto_register_skciphers(&t_alg->alg, 1);
    if (err) {
      dev_warn(&device->pdev->dev, "%s alg registration failed with %d\n",
               t_alg->alg.base.cra_driver_name, err);
      kfree(t_alg);
    } else
      list_add_tail(&t_alg->entry, &device->skcipher_list);
  }

  if (!list_empty(&device->skcipher_list))
    dev_info(&device->pdev->dev,
             "skcipher algorithms registered in /proc/crypto\n");

  return err;
}

/******************************************************************************
 *****************************************************************************/
void al_crypto_skcipher_terminate(struct al_crypto_device *device) {
  struct al_crypto_alg *t_alg, *n;

  if (!device->skcipher_list.next)
    return;

  list_for_each_entry_safe(t_alg, n, &device->skcipher_list, entry) {
    crypto_unregister_skciphers(&t_alg->alg, 1);
    list_del(&t_alg->entry);
    kfree(t_alg);
  }
}
