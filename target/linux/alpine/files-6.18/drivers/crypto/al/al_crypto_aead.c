/*
 * drivers/crypto/al_crypto_aead.c
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
#include <crypto/aead.h>
#include <crypto/algapi.h>
#include <crypto/authenc.h>
#include <crypto/hash.h>
#include <crypto/internal/aead.h>
#include <crypto/internal/des.h>
#include <crypto/scatterwalk.h>
#include <crypto/sha1.h>
#include <crypto/sha2.h>
#include <linux/random.h>
#include <linux/rtnetlink.h>
#include <linux/scatterlist.h>

#include <mach/al_hal_ssm_crypto.h>

#include "al_crypto.h"

#define AL_CRYPTO_CRA_PRIORITY 300

static int al_crypto_init_tfm(struct crypto_aead *tfm);

static void al_crypto_exit_tfm(struct crypto_aead *tfm);

static int al_crypto_setkey(struct crypto_aead *tfm, const u8 *key,
                            unsigned int keylen);

static int al_crypto_setauthsize(struct crypto_aead *tfm,
                                 unsigned int authsize);

static int al_crypto_encrypt(struct aead_request *req);

static int al_crypto_decrypt(struct aead_request *req);

struct al_crypto_aead_req_ctx {
  u8 iv[AL_CRYPTO_MAX_IV_LENGTH] ____cacheline_aligned;
  enum al_crypto_dir dir;
  size_t cryptlen;
  struct scatterlist *in_sg;
  struct scatterlist *out_sg;
  struct scatterlist sgl;
  size_t pages_sg;
  size_t total_in;
  size_t total_out;
  size_t total_out_save;
  int sgs_copied;
};

struct al_crypto_alg {
  struct list_head entry;
  struct al_crypto_device *device;
  enum al_crypto_sa_enc_type enc_type;
  enum al_crypto_sa_op sa_op;
  enum al_crypto_sa_auth_type auth_type;
  enum al_crypto_sa_sha2_mode sha2_mode;
  char sw_hash_name[CRYPTO_MAX_ALG_NAME];
  unsigned int sw_hash_interm_offset;
  unsigned int sw_hash_interm_size;
  struct aead_alg alg;
};

struct al_crypto_aead_template {
  char name[CRYPTO_MAX_ALG_NAME];
  char driver_name[CRYPTO_MAX_ALG_NAME];
  unsigned int blocksize;
  enum al_crypto_sa_enc_type enc_type;
  enum al_crypto_sa_op sa_op;
  enum al_crypto_sa_auth_type auth_type;
  enum al_crypto_sa_sha2_mode sha2_mode;
  char sw_hash_name[CRYPTO_MAX_ALG_NAME];
  unsigned int sw_hash_interm_offset;
  unsigned int sw_hash_interm_size;
  struct aead_alg alg;
};

static struct al_crypto_aead_template driver_algs[] = {
    {
        .name = "authenc(hmac(sha1),cbc(aes))",
        .driver_name = "authenc-hmac-sha1-cbc-aes-al",
        .blocksize = AES_BLOCK_SIZE,
        .enc_type = AL_CRYPT_AES_CBC,
        .sa_op = AL_CRYPT_ENC_AUTH,
        .auth_type = AL_CRYPT_AUTH_SHA1,
        .sha2_mode = 0,
        .sw_hash_name = "sha1",
        .sw_hash_interm_offset = offsetof(struct sha1_state, state),
        .sw_hash_interm_size = sizeof(((struct sha1_state *)0)->state),
        .alg =
            {
                .init = al_crypto_init_tfm,
                .exit = al_crypto_exit_tfm,
                .setkey = al_crypto_setkey,
                .setauthsize = al_crypto_setauthsize,
                .encrypt = al_crypto_encrypt,
                .decrypt = al_crypto_decrypt,
                .ivsize = AES_BLOCK_SIZE,
                .maxauthsize = SHA1_DIGEST_SIZE,
            },
    },
    {
        .name = "authenc(hmac(sha256),cbc(aes))",
        .driver_name = "authenc-hmac-sha256-cbc-aes-al",
        .blocksize = AES_BLOCK_SIZE,
        .enc_type = AL_CRYPT_AES_CBC,
        .sa_op = AL_CRYPT_ENC_AUTH,
        .auth_type = AL_CRYPT_AUTH_SHA2,
        .sha2_mode = AL_CRYPT_SHA2_256,
        .sw_hash_name = "sha256",
        .sw_hash_interm_offset = offsetof(struct sha256_state, state),
        .sw_hash_interm_size = sizeof(((struct sha256_state *)0)->state),
        .alg =
            {
                .init = al_crypto_init_tfm,
                .exit = al_crypto_exit_tfm,
                .setkey = al_crypto_setkey,
                .setauthsize = al_crypto_setauthsize,
                .encrypt = al_crypto_encrypt,
                .decrypt = al_crypto_decrypt,
                .ivsize = AES_BLOCK_SIZE,
                .maxauthsize = SHA256_DIGEST_SIZE,
            },
    },
    {
        .name = "authenc(hmac(sha384),cbc(aes))",
        .driver_name = "authenc-hmac-sha384-cbc-aes-al",
        .blocksize = AES_BLOCK_SIZE,
        .enc_type = AL_CRYPT_AES_CBC,
        .sa_op = AL_CRYPT_ENC_AUTH,
        .auth_type = AL_CRYPT_AUTH_SHA2,
        .sha2_mode = AL_CRYPT_SHA2_384,
        .sw_hash_name = "sha256",
        .sw_hash_interm_offset = offsetof(struct sha512_state, state),
        .sw_hash_interm_size = sizeof(((struct sha512_state *)0)->state),
        .alg =
            {
                .init = al_crypto_init_tfm,
                .exit = al_crypto_exit_tfm,
                .setkey = al_crypto_setkey,
                .setauthsize = al_crypto_setauthsize,
                .encrypt = al_crypto_encrypt,
                .decrypt = al_crypto_decrypt,
                .ivsize = AES_BLOCK_SIZE,
                .maxauthsize = SHA384_DIGEST_SIZE,
            },
    },
    {
        .name = "authenc(hmac(sha512),cbc(aes))",
        .driver_name = "authenc-hmac-sha512-cbc-aes-al",
        .blocksize = AES_BLOCK_SIZE,
        .enc_type = AL_CRYPT_AES_CBC,
        .sa_op = AL_CRYPT_ENC_AUTH,
        .auth_type = AL_CRYPT_AUTH_SHA2,
        .sha2_mode = AL_CRYPT_SHA2_512,
        .sw_hash_name = "sha512",
        .sw_hash_interm_offset = offsetof(struct sha512_state, state),
        .sw_hash_interm_size = sizeof(((struct sha512_state *)0)->state),
        .alg =
            {
                .init = al_crypto_init_tfm,
                .exit = al_crypto_exit_tfm,
                .setkey = al_crypto_setkey,
                .setauthsize = al_crypto_setauthsize,
                .encrypt = al_crypto_encrypt,
                .decrypt = al_crypto_decrypt,
                .ivsize = AES_BLOCK_SIZE,
                .maxauthsize = SHA512_DIGEST_SIZE,
            },
    },
};

/******************************************************************************
 *****************************************************************************/
static int al_crypto_init_tfm(struct crypto_aead *tfm) {
  struct al_crypto_ctx *ctx = crypto_aead_ctx(tfm);
  struct aead_alg *alg = crypto_aead_alg(tfm);
  struct al_crypto_alg *al_crypto_alg =
      container_of(alg, struct al_crypto_alg, alg);
  struct al_crypto_device *device = al_crypto_alg->device;
  int chan_idx = atomic_inc_return(&device->tfm_count) %
                 (device->num_channels - device->crc_channels);

  dev_dbg(&device->pdev->dev, "%s: cra_name=%s alignmask=%x\n", __func__,
          crypto_tfm_alg_name(crypto_aead_tfm(tfm)),
          crypto_aead_alignmask(tfm));

  memset(ctx, 0, sizeof(struct al_crypto_ctx));

  ctx->chan = device->channels[chan_idx];

  ctx->sa.enc_type = al_crypto_alg->enc_type;
  ctx->sa.sa_op = al_crypto_alg->sa_op;
  ctx->sa.auth_type = al_crypto_alg->auth_type;
  ctx->sa.sha2_mode = al_crypto_alg->sha2_mode;

  /* Allocate SW hash for hmac long key hashing and key XOR ipad/opad
   * intermediate calculations
   */
  if (strlen(al_crypto_alg->sw_hash_name)) {
    ctx->sw_hash = crypto_alloc_shash(al_crypto_alg->sw_hash_name, 0,
                                      CRYPTO_ALG_NEED_FALLBACK);
    if (IS_ERR(ctx->sw_hash)) {
      dev_err(to_dev(ctx->chan), "failed to allocate sw hash for aead");
      return PTR_ERR(ctx->sw_hash);
    }

    ctx->hmac_pads =
        kmalloc(2 * crypto_shash_descsize(ctx->sw_hash), GFP_KERNEL);
    if (!ctx->hmac_pads)
      return -ENOMEM;
  }

  crypto_aead_set_reqsize(tfm, sizeof(struct al_crypto_aead_req_ctx));

  ctx->hw_sa =
      dma_alloc_coherent(&device->pdev->dev, sizeof(struct al_crypto_hw_sa),
                         &ctx->hw_sa_dma_addr, GFP_KERNEL);

  ctx->iv = dma_alloc_coherent(&device->pdev->dev, AL_CRYPTO_MAX_IV_LENGTH,
                               &ctx->iv_dma_addr, GFP_KERNEL);
  /* random first IV */
  get_random_bytes(ctx->iv, AL_CRYPTO_MAX_IV_LENGTH);

  AL_CRYPTO_STATS_LOCK(&ctx->chan->stats_gen_lock);
  AL_CRYPTO_STATS_INC(ctx->chan->stats_gen.aead_tfms, 1);
  AL_CRYPTO_STATS_UNLOCK(&ctx->chan->stats_gen_lock);

  return 0;
}

/******************************************************************************
 *****************************************************************************/
static void al_crypto_exit_tfm(struct crypto_aead *tfm) {
  struct al_crypto_ctx *ctx = crypto_aead_ctx(tfm);
  struct aead_alg *alg = crypto_aead_alg(tfm);
  struct al_crypto_alg *al_crypto_alg =
      container_of(alg, struct al_crypto_alg, alg);
  struct al_crypto_device *device = al_crypto_alg->device;

  dev_dbg(&device->pdev->dev, "%s: cra_name=%s\n", __func__,
          crypto_tfm_alg_name(crypto_aead_tfm(tfm)));

  /* LRU list access has to be protected */
  spin_lock_bh(&ctx->chan->prep_lock);
  if (ctx->cache_state.cached)
    al_crypto_cache_remove_lru(ctx->chan, &ctx->cache_state);
  spin_unlock_bh(&ctx->chan->prep_lock);

  if (ctx->sw_hash)
    crypto_free_shash(ctx->sw_hash);

  if (ctx->hmac_pads)
    kfree(ctx->hmac_pads);

  if (ctx->hw_sa_dma_addr)
    dma_free_coherent(&device->pdev->dev, sizeof(struct al_crypto_hw_sa),
                      ctx->hw_sa, ctx->hw_sa_dma_addr);

  if (ctx->iv_dma_addr)
    dma_free_coherent(&device->pdev->dev, AL_CRYPTO_MAX_IV_LENGTH, ctx->iv,
                      ctx->iv_dma_addr);

  AL_CRYPTO_STATS_LOCK(&ctx->chan->stats_gen_lock);
  AL_CRYPTO_STATS_DEC(ctx->chan->stats_gen.aead_tfms, 1);
  AL_CRYPTO_STATS_UNLOCK(&ctx->chan->stats_gen_lock);
}

/******************************************************************************
 *****************************************************************************/
static int al_crypto_setkey(struct crypto_aead *tfm, const u8 *key,
                            unsigned int keylen) {
  struct al_crypto_ctx *ctx = crypto_aead_ctx(tfm);
  struct aead_alg *alg = crypto_aead_alg(tfm);
  struct al_crypto_alg *al_crypto_alg =
      container_of(alg, struct al_crypto_alg, alg);
  struct crypto_authenc_keys authenc_keys;
  int rc;

  dev_dbg(to_dev(ctx->chan), "%s: keylen=%d\n", __func__, keylen);

  /* Currently only AES is supported */
  BUG_ON((ctx->sa.enc_type != AL_CRYPT_AES_CBC) &&
         (ctx->sa.enc_type != AL_CRYPT_AES_ECB) &&
         (ctx->sa.enc_type != AL_CRYPT_AES_CTR));

  rc = crypto_authenc_extractkeys(&authenc_keys, key, keylen);
  if (rc)
    return rc;

  dev_dbg(to_dev(ctx->chan), "%s: authkeylen=%d enckeylen=%d\n", __func__,
          authenc_keys.authkeylen, authenc_keys.enckeylen);
  print_hex_dump_debug(KBUILD_MODNAME ": authkey: ", DUMP_PREFIX_OFFSET, 16, 1,
                       authenc_keys.authkey, authenc_keys.authkeylen, false);
  print_hex_dump_debug(KBUILD_MODNAME ": enckey: ", DUMP_PREFIX_OFFSET, 16, 1,
                       authenc_keys.enckey, authenc_keys.enckeylen, false);

  if (al_crypto_keylen_to_sa_aes_ksize(authenc_keys.enckeylen,
                                       &ctx->sa.aes_ksize))
    return -EINVAL;

  if (ctx->sw_hash) {
    rc = hmac_setkey(ctx, authenc_keys.authkey, authenc_keys.authkeylen,
                     al_crypto_alg->sw_hash_interm_offset,
                     al_crypto_alg->sw_hash_interm_size);
    if (rc)
      return rc;

    print_hex_dump_debug(KBUILD_MODNAME ": hmac_iv_in: ", DUMP_PREFIX_OFFSET,
                         16, 1, ctx->sa.hmac_iv_in,
                         al_crypto_alg->sw_hash_interm_size, false);
    print_hex_dump_debug(KBUILD_MODNAME ": hmac_iv_out: ", DUMP_PREFIX_OFFSET,
                         16, 1, ctx->sa.hmac_iv_out,
                         al_crypto_alg->sw_hash_interm_size, false);
  }

  /* TODO: optimize HAL to hold ptrs to save this memcpy */
  /* copy the key to the sa */
  memcpy(&ctx->sa.enc_key, authenc_keys.enckey, authenc_keys.enckeylen);

  ctx->sa.sign_after_enc = true;
  ctx->sa.auth_after_dec = false;

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
static int al_crypto_setauthsize(struct crypto_aead *tfm,
                                 unsigned int authsize) {
  struct al_crypto_ctx *ctx = crypto_aead_ctx(tfm);
  struct aead_alg *alg = crypto_aead_alg(tfm);
  int signature_size = (authsize >> 2) - 1;

  dev_dbg(to_dev(ctx->chan),
          "%s: authsize=%d maxauthsize=%d signature_size=%d\n", __func__,
          authsize, alg->maxauthsize, signature_size);

  if (signature_size < 0 || authsize > alg->maxauthsize || (authsize & 3))
    return -EINVAL;

  ctx->sa.signature_size = signature_size;
  ctx->sa.auth_signature_msb = true;

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
/* DMA unmap buffers for aead request
 */
static inline void al_crypto_dma_unmap(struct al_crypto_chan *chan,
                                       struct aead_request *req, int src_nents,
                                       int dst_nents,
                                       struct al_crypto_sw_desc *desc) {
  struct al_crypto_aead_req_ctx *rctx = aead_request_ctx(req);

  if (likely(rctx->in_sg == rctx->out_sg)) {
    dma_unmap_sg(to_dev(chan), rctx->in_sg, src_nents, DMA_BIDIRECTIONAL);
  } else {
    dma_unmap_sg(to_dev(chan), rctx->in_sg, src_nents, DMA_TO_DEVICE);
    dma_unmap_sg(to_dev(chan), rctx->out_sg, dst_nents, DMA_FROM_DEVICE);
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
    AL_CRYPTO_STATS_INC(chan->stats_prep.aead_encrypt_hash_reqs, 1);
    AL_CRYPTO_STATS_INC(chan->stats_prep.aead_encrypt_bytes,
                        xaction->enc_in_len);
    AL_CRYPTO_STATS_INC(chan->stats_prep.aead_hash_bytes, xaction->auth_in_len);
  } else {
    AL_CRYPTO_STATS_INC(chan->stats_prep.aead_decrypt_validate_reqs, 1);
    AL_CRYPTO_STATS_INC(chan->stats_prep.aead_decrypt_bytes,
                        xaction->enc_in_len);
    AL_CRYPTO_STATS_INC(chan->stats_prep.aead_validate_bytes,
                        xaction->auth_in_len);
  }

  if (xaction->auth_in_len <= 512)
    AL_CRYPTO_STATS_INC(chan->stats_prep.aead_reqs_le512, 1);
  else if ((xaction->auth_in_len > 512) && (xaction->auth_in_len <= 2048))
    AL_CRYPTO_STATS_INC(chan->stats_prep.aead_reqs_512_2048, 1);
  else if ((xaction->auth_in_len > 2048) && (xaction->auth_in_len <= 4096))
    AL_CRYPTO_STATS_INC(chan->stats_prep.aead_reqs_2048_4096, 1);
  else
    AL_CRYPTO_STATS_INC(chan->stats_prep.aead_reqs_gt4096, 1);
}

/******************************************************************************
 *****************************************************************************/
static void al_crypto_dump_xaction_buffers(struct al_crypto_sw_desc *desc) {
  struct aead_request *req = (struct aead_request *)desc->req;
  struct crypto_aead *tfm = crypto_aead_reqtfm(req);
  struct al_crypto_ctx *ctx = crypto_aead_ctx(tfm);
  struct al_crypto_chan *chan = ctx->chan;
  struct al_crypto_aead_req_ctx *rctx = aead_request_ctx(req);
  struct al_crypto_transaction *xaction = &desc->hal_xaction;
  struct scatterlist *sg;
  int i;

  i = 0;
  sg = rctx->in_sg;
  while (sg) {
    dev_dbg(to_dev(chan), "%s: src sg%d offset=%x dma_address=%x dma_len=%d\n",
            __func__, i, sg->offset, sg_dma_address(sg), sg_dma_len(sg));
    sg = sg_next(sg);
    i++;
  }

  i = 0;
  sg = rctx->out_sg;
  while (sg) {
    dev_dbg(to_dev(chan), "%s: dst sg%d offset=%x dma_address=%x dma_len=%d\n",
            __func__, i, sg->offset, sg_dma_address(sg), sg_dma_len(sg));
    sg = sg_next(sg);
    i++;
  }

  dev_dbg(to_dev(chan), "%s: src.num=%d dst.num=%d\n", __func__,
          xaction->src.num, xaction->dst.num);

  for (i = 0; i < xaction->src.num; i++) {
    dev_dbg(to_dev(chan), "%s: src buf%d addr=%x len=%d\n", __func__, i,
            desc->src_bufs[i].addr, desc->src_bufs[i].len);
  }

  for (i = 0; i < xaction->dst.num; i++) {
    dev_dbg(to_dev(chan), "%s: dst buf%d addr=%x len=%d\n", __func__, i,
            desc->dst_bufs[i].addr, desc->dst_bufs[i].len);
  }
}

/******************************************************************************
 *****************************************************************************/
static int al_crypto_prepare_xaction_buffers(struct aead_request *req,
                                             struct al_crypto_sw_desc *desc,
                                             u8 *iv) {
  struct crypto_aead *tfm = crypto_aead_reqtfm(req);
  struct al_crypto_ctx *ctx = crypto_aead_ctx(tfm);
  struct al_crypto_aead_req_ctx *rctx = aead_request_ctx(req);
  struct al_crypto_chan *chan = ctx->chan;
  struct al_crypto_transaction *xaction = &desc->hal_xaction;
  int authsize = crypto_aead_authsize(tfm);
  int ivsize = crypto_aead_ivsize(tfm);
  int src_idx = 0, dst_idx = 0;
  struct scatterlist *sg;
  int i;

  print_hex_dump_debug(KBUILD_MODNAME ": iv: ", DUMP_PREFIX_OFFSET, 16, 1, iv,
                       ivsize, false);

  /* IV might be allocated on stack, copy for DMA */
  memcpy(rctx->iv, iv, ivsize);
  /* map IV */
  xaction->enc_iv_in.addr =
      dma_map_single(to_dev(chan), rctx->iv, ivsize, DMA_TO_DEVICE);
  if (dma_mapping_error(to_dev(chan), xaction->enc_iv_in.addr)) {
    dev_err(to_dev(chan), "%s: dma_map_single failed!\n", __func__);
    return -ENOMEM;
  }
  xaction->enc_iv_in.len = ivsize;

  xaction->src_size = rctx->total_in;

  /* add assoc+enc+auth data */
  sg_map_to_xaction_buffers(rctx->in_sg, desc->src_bufs, xaction->src_size,
                            &src_idx);
  if (rctx->in_sg == rctx->out_sg) {
    for (i = 0; i < src_idx; i++)
      desc->dst_bufs[i] = desc->src_bufs[i];
    dst_idx = src_idx;
  } else {
    sg_map_to_xaction_buffers(rctx->out_sg, desc->dst_bufs, rctx->total_out,
                              &dst_idx);
  }

  xaction->auth_in_len += req->assoclen;
  xaction->enc_in_off += req->assoclen;

  xaction->auth_in_len += rctx->cryptlen;
  xaction->enc_in_len = rctx->cryptlen;

  xaction->src.bufs = &desc->src_bufs[0];
  xaction->src.num = src_idx;
  xaction->dst.bufs = &desc->dst_bufs[0];
  xaction->dst.num = dst_idx;

  dev_dbg(to_dev(chan),
          "%s: src_size=%d src_idx=%d dst_idx=%d auth_in_off=%d auth_in_len=%d "
          "enc_in_off=%d enc_in_len=%d\n",
          __func__, xaction->src_size, src_idx, dst_idx, xaction->auth_in_off,
          xaction->auth_in_len, xaction->enc_in_off, xaction->enc_in_len);

  /* set signature buffer for auth */
  if (rctx->dir == AL_CRYPT_ENCRYPT) {
    sg = rctx->out_sg;
    while (!sg_is_last(sg))
      sg = sg_next(sg);

    dev_dbg(to_dev(chan), "%s: last dst sg dma_address=%x dma_len=%d\n",
            __func__, sg_dma_address(sg), sg_dma_len(sg));

    /* assume that auth result is not scattered */
    BUG_ON(sg_dma_len(sg) < authsize);
    xaction->auth_sign_out.addr =
        sg_dma_address(sg) + sg_dma_len(sg) - authsize;
    xaction->auth_sign_out.len = authsize;

    dev_dbg(to_dev(chan), "%s: auth_sign_out.addr=%x auth_sign_out.len=%d\n",
            __func__, xaction->auth_sign_out.addr, xaction->auth_sign_out.len);
  } else {
    sg = rctx->in_sg;
    while (!sg_is_last(sg))
      sg = sg_next(sg);

    dev_dbg(to_dev(chan), "%s: last src sg dma_address=%x dma_len=%d\n",
            __func__, sg_dma_address(sg), sg_dma_len(sg));

    /* assume that auth result is not scattered */
    BUG_ON(sg_dma_len(sg) < authsize);
    xaction->auth_sign_in.addr = sg_dma_address(sg) + sg_dma_len(sg) - authsize;
    xaction->auth_sign_in.len = authsize;

    dev_dbg(to_dev(chan), "%s: auth_sign_in.addr=%x auth_sign_in.len=%d\n",
            __func__, xaction->auth_sign_in.addr, xaction->auth_sign_in.len);
  }

  al_crypto_dump_xaction_buffers(desc);

  return 0;
}

/******************************************************************************
 *****************************************************************************/
/* Prepare encryption+auth transaction to be processed by HAL
 */
static int al_crypto_prepare_xaction(struct aead_request *req,
                                     struct al_crypto_sw_desc *desc, u8 *iv) {
  struct crypto_aead *tfm = crypto_aead_reqtfm(req);
  struct al_crypto_ctx *ctx = crypto_aead_ctx(tfm);
  struct al_crypto_chan *chan = ctx->chan;
  struct al_crypto_aead_req_ctx *rctx = aead_request_ctx(req);
  struct al_crypto_transaction *xaction;
  int rc = 0;

  dev_dbg(to_dev(chan), "%s: dir=%d\n", __func__, rctx->dir);

  xaction = &desc->hal_xaction;
  memset(xaction, 0, sizeof(struct al_crypto_transaction));
  xaction->dir = rctx->dir;

  rc = al_crypto_prepare_xaction_buffers(req, desc, iv);
  if (unlikely(rc != 0)) {
    dev_err(to_dev(chan), "%s: al_crypto_prepare_xaction_buffers failed!\n",
            __func__);
    return rc;
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

  return rc;
}

static void al_crypto_sg_copy_buf(void *buf, struct scatterlist *sg,
                                  unsigned int start, unsigned int nbytes,
                                  int out) {
  if (!nbytes)
    return;

  scatterwalk_map_and_copy(buf, sg, start, nbytes, out);
}

static bool al_crypto_is_aligned(struct scatterlist *sg, size_t align) {
  while (sg) {
    if (!IS_ALIGNED(sg->offset, align))
      return false;
    if (!IS_ALIGNED(sg->length, align))
      return false;
    sg = sg_next(sg);
  }
  return true;
}

static bool al_crypto_need_copy_sg(struct aead_request *req) {
  struct crypto_aead *tfm = crypto_aead_reqtfm(req);
  struct al_crypto_aead_req_ctx *rctx = aead_request_ctx(req);
  struct al_crypto_ctx *ctx = crypto_aead_ctx(tfm);
  struct al_crypto_chan *chan = ctx->chan;
  int authsize = crypto_aead_authsize(tfm);
  struct scatterlist *sg;

  /* auth tag cannot be scattered */
  if (rctx->dir == AL_CRYPT_ENCRYPT) {
    sg = rctx->out_sg;
    while (!sg_is_last(sg))
      sg = sg_next(sg);
    dev_dbg(to_dev(chan), "%s: last dst sg_dma_len=%d\n", __func__,
            sg_dma_len(sg));
    if (sg_dma_len(sg) < authsize)
      return true;
  } else {
    sg = rctx->in_sg;
    while (!sg_is_last(sg))
      sg = sg_next(sg);
    dev_dbg(to_dev(chan), "%s: last src sg_dma_len=%d\n", __func__,
            sg_dma_len(sg));
    if (sg_dma_len(sg) < authsize)
      return true;
  }

  /* hw can't handle separate src and dst sgs ? */
  if (rctx->in_sg != rctx->out_sg)
    return true;

  if (!al_crypto_is_aligned(rctx->in_sg, AES_BLOCK_SIZE))
    return true;
  if (!al_crypto_is_aligned(rctx->out_sg, AES_BLOCK_SIZE))
    return true;

  return false;
}

static int al_crypto_copy_sg(struct aead_request *req) {
  struct crypto_aead *tfm = crypto_aead_reqtfm(req);
  struct al_crypto_ctx *ctx = crypto_aead_ctx(tfm);
  struct al_crypto_chan *chan = ctx->chan;
  struct al_crypto_aead_req_ctx *rctx = aead_request_ctx(req);
  gfp_t gfp_flags =
      (req->base.flags & CRYPTO_TFM_REQ_MAY_SLEEP) ? GFP_KERNEL : GFP_ATOMIC;
  bool need_copy = al_crypto_need_copy_sg(req);
  int len;
  void *buf;

  rctx->sgs_copied = 0;

  len = max(rctx->total_in, rctx->total_out);

  dev_dbg(to_dev(chan), "%s: copy sg %sneeded buflen=%d\n", __func__,
          need_copy ? "" : "not ", len);

  if (!need_copy)
    return 0;

  rctx->pages_sg = get_order(len);
  buf = (void *)__get_free_pages(gfp_flags, rctx->pages_sg);
  if (!buf) {
    dev_err(to_dev(chan), "Can't allocate pages when unaligned\n");
    return -EFAULT;
  }

  al_crypto_sg_copy_buf(buf, req->src, 0, rctx->total_in, 0);

  sg_init_one(&rctx->sgl, buf, len);
  rctx->in_sg = rctx->out_sg = &rctx->sgl;

  rctx->total_in = rctx->total_out = len;

  rctx->sgs_copied = 1;

  return 0;
}

/******************************************************************************
 *****************************************************************************/
/* Prepare aead encryption and auth dma, call hal transaction preparation
 * function and submit the request to HAL.
 * Grabs and releases producer lock for relevant sw ring
 */
static int al_crypto_do_crypt(struct aead_request *req, u8 *iv) {
  struct crypto_aead *tfm = crypto_aead_reqtfm(req);
  struct al_crypto_ctx *ctx = crypto_aead_ctx(tfm);
  struct al_crypto_chan *chan = ctx->chan;
  struct al_crypto_aead_req_ctx *rctx = aead_request_ctx(req);
  int authsize = crypto_aead_authsize(tfm);
  struct al_crypto_sw_desc *desc;
  int src_nents = 0, dst_nents = 0;
  int idx, rc;

  rctx->in_sg = req->src;
  rctx->out_sg = req->dst;

  if (rctx->dir == AL_CRYPT_ENCRYPT) {
    rctx->total_in = req->assoclen + rctx->cryptlen;
    rctx->total_out = req->assoclen + rctx->cryptlen + authsize;
  } else {
    rctx->total_in = req->assoclen + rctx->cryptlen + authsize;
    rctx->total_out = req->assoclen + rctx->cryptlen;
  }

  rctx->total_out_save = rctx->total_out;

  if (rctx->out_sg == rctx->in_sg)
    rctx->total_in = rctx->total_out = max(rctx->total_in, rctx->total_out);

  rc = al_crypto_copy_sg(req);
  if (rc)
    return rc;

  if (rctx->out_sg != rctx->in_sg) {
    src_nents = sg_nents_for_len(rctx->in_sg, rctx->total_in);
    dst_nents = sg_nents_for_len(rctx->out_sg, rctx->total_out);
  } else {
    src_nents = sg_nents_for_len(rctx->in_sg, rctx->total_in);
    dst_nents = src_nents;
  }

  dev_dbg(to_dev(chan),
          "%s: src_nents=%d dst_nents=%d authsize=%d (rctx->in_sg %s "
          "rctx->out_sg)\n",
          __func__, src_nents, dst_nents, authsize,
          rctx->in_sg == rctx->out_sg ? "==" : "!=");

  /* Currently supported max sg chain length is
   * AL_CRYPTO_OP_MAX_DATA_BUFS(12) which is minimum of descriptors left
   * for data in a transaction:
   * tx: 31(supported by HW) - 1(metadata) - 1(sa_in) -
   *			1(enc_iv_in|auth_iv_in) - 1(auth_sign_in) = 27
   * rx: 31(supported by HW) - 1(sa_out) - 1(enc_iv_out|auth_iv_out) -
   *			1(next_enc_iv_out) - 1(auth_sign_out) = 27
   */
  BUG_ON((src_nents + 1 > AL_CRYPTO_OP_MAX_BUFS) ||
         (dst_nents + 1 > AL_CRYPTO_OP_MAX_BUFS));

  if (likely(rctx->in_sg == rctx->out_sg)) {
    dma_map_sg(to_dev(chan), rctx->in_sg, src_nents, DMA_BIDIRECTIONAL);
  } else {
    dma_map_sg(to_dev(chan), rctx->in_sg, src_nents, DMA_TO_DEVICE);
    dma_map_sg(to_dev(chan), rctx->out_sg, dst_nents, DMA_FROM_DEVICE);
  }

  spin_lock_bh(&chan->prep_lock);
  if (likely(al_crypto_get_sw_desc(chan, 1) == 0))
    idx = chan->head;
  else {
    dev_dbg(to_dev(chan), "%s: al_crypto_get_sw_desc failed!\n", __func__);
    al_crypto_dma_unmap(chan, req, src_nents, dst_nents, NULL);
    spin_unlock_bh(&chan->prep_lock);
    return -EBUSY;
  }

  chan->sw_desc_num_locked = 1;
  chan->tx_desc_produced = 0;

  desc = al_crypto_get_ring_ent(chan, idx);
  desc->req = (void *)req;
  desc->req_type = AL_CRYPTO_REQ_AEAD;
  desc->src_nents = src_nents;
  desc->dst_nents = dst_nents;

  rc = al_crypto_prepare_xaction(req, desc, iv);
  if (unlikely(rc != 0)) {
    dev_err(to_dev(chan), "%s: al_crypto_prepare_xaction failed!\n", __func__);
    al_crypto_dma_unmap(chan, req, src_nents, dst_nents, desc);
    spin_unlock_bh(&chan->prep_lock);
    return rc;
  }

  /* send crypto transaction to engine */
  rc = al_crypto_dma_prepare(chan->hal_crypto, chan->idx, &desc->hal_xaction);
  if (unlikely(rc != 0)) {
    dev_err(to_dev(chan), "%s: al_crypto_dma_prepare failed!\n", __func__);
    al_crypto_dma_unmap(chan, req, src_nents, dst_nents, desc);
    spin_unlock_bh(&chan->prep_lock);
    return rc;
  }

  chan->tx_desc_produced += desc->hal_xaction.tx_descs_count;

  dev_dbg(to_dev(chan), "%s: tx_desc_produced=%d\n", __func__,
          chan->tx_desc_produced);

  al_crypto_tx_submit(chan);

  spin_unlock_bh(&chan->prep_lock);

  return -EINPROGRESS;
}

/******************************************************************************
 *****************************************************************************/
static int al_crypto_encrypt(struct aead_request *req) {
  struct crypto_aead *tfm = crypto_aead_reqtfm(req);
  struct al_crypto_ctx *ctx = crypto_aead_ctx(tfm);
  struct al_crypto_chan *chan = ctx->chan;
  struct al_crypto_aead_req_ctx *rctx = aead_request_ctx(req);

  dev_dbg(to_dev(chan), "%s: cryptlen=%d assoclen=%d\n", __func__,
          req->cryptlen, req->assoclen);
  print_hex_dump_debug(KBUILD_MODNAME ": iv: ", DUMP_PREFIX_OFFSET, 16, 1,
                       req->iv, crypto_aead_ivsize(tfm), false);

  rctx->dir = AL_CRYPT_ENCRYPT;
  rctx->cryptlen = req->cryptlen;

  return al_crypto_do_crypt(req, req->iv);
}

/******************************************************************************
 *****************************************************************************/
static int al_crypto_decrypt(struct aead_request *req) {
  struct crypto_aead *tfm = crypto_aead_reqtfm(req);
  struct al_crypto_ctx *ctx = crypto_aead_ctx(tfm);
  struct al_crypto_chan *chan = ctx->chan;
  struct al_crypto_aead_req_ctx *rctx = aead_request_ctx(req);
  int authsize = crypto_aead_authsize(tfm);

  dev_dbg(to_dev(chan), "%s: cryptlen=%d assoclen=%d authsize=%d\n", __func__,
          req->cryptlen, req->assoclen, authsize);
  print_hex_dump_debug(KBUILD_MODNAME ": iv: ", DUMP_PREFIX_OFFSET, 16, 1,
                       req->iv, crypto_aead_ivsize(tfm), false);

  rctx->dir = AL_CRYPT_DECRYPT;
  /* req->cryptlen includes the authsize when decrypting */
  rctx->cryptlen = req->cryptlen - authsize;

  return al_crypto_do_crypt(req, req->iv);
}

/******************************************************************************
 *****************************************************************************/
static struct al_crypto_alg *
al_crypto_alg_alloc(struct al_crypto_device *device,
                    struct al_crypto_aead_template *template) {
  struct al_crypto_alg *t_alg;
  struct aead_alg *alg;

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
  alg->base.cra_alignmask = AES_BLOCK_SIZE - 1;
  alg->base.cra_ctxsize = sizeof(struct al_crypto_ctx);
  alg->base.cra_flags = CRYPTO_ALG_ASYNC | CRYPTO_ALG_KERN_DRIVER_ONLY;

  alg->chunksize = alg->base.cra_blocksize;

  t_alg->enc_type = template->enc_type;
  t_alg->sa_op = template->sa_op;
  t_alg->auth_type = template->auth_type;
  t_alg->sha2_mode = template->sha2_mode;
  t_alg->device = device;

  snprintf(t_alg->sw_hash_name, CRYPTO_MAX_ALG_NAME, "%s",
           template->sw_hash_name);
  t_alg->sw_hash_interm_offset = template->sw_hash_interm_offset;
  t_alg->sw_hash_interm_size = template->sw_hash_interm_size;

  return t_alg;
}

/******************************************************************************
 *****************************************************************************/
/* Cleanup single aead request - invoked from cleanup tasklet (interrupt
 * handler)
 */
void al_crypto_aead_cleanup_single(struct al_crypto_chan *chan,
                                   struct al_crypto_sw_desc *desc,
                                   uint32_t comp_status) {
  struct aead_request *req = (struct aead_request *)desc->req;
  struct al_crypto_aead_req_ctx *rctx = aead_request_ctx(req);
  int err = 0;

  dev_dbg(to_dev(chan), "%s: chan->idx=%d comp_status=%x\n", __func__,
          chan->idx, comp_status);

  al_crypto_dma_unmap(chan, req, desc->src_nents, desc->dst_nents, desc);

  if (comp_status & AL_CRYPT_AUTH_ERROR)
    err = -EBADMSG;

  dev_dbg(to_dev(chan), "%s: assoclen=%d cryptlen=%d\n", __func__,
          req->assoclen, rctx->cryptlen);

  if (rctx->sgs_copied) {
    void *buf = sg_virt(&rctx->sgl);
    al_crypto_sg_copy_buf(buf, req->dst, 0, rctx->total_out_save, 1);
    free_pages((unsigned long)buf, rctx->pages_sg);
  }

  req->base.complete(&req->base, err);
}

/******************************************************************************
 *****************************************************************************/
int al_crypto_aead_init(struct al_crypto_device *device) {
  int err = 0;
  int i;

  INIT_LIST_HEAD(&device->aead_list);

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

    err = crypto_register_aeads(&t_alg->alg, 1);
    if (err) {
      dev_warn(&device->pdev->dev, "%s alg registration failed with %d\n",
               t_alg->alg.base.cra_driver_name, err);
      kfree(t_alg);
    } else
      list_add_tail(&t_alg->entry, &device->aead_list);
  }

  if (!list_empty(&device->aead_list))
    dev_info(&device->pdev->dev,
             "aead algorithms registered in /proc/crypto\n");

  return err;
}

/******************************************************************************
 *****************************************************************************/
void al_crypto_aead_terminate(struct al_crypto_device *device) {
  struct al_crypto_alg *t_alg, *n;

  if (!device->aead_list.next)
    return;

  list_for_each_entry_safe(t_alg, n, &device->aead_list, entry) {
    crypto_unregister_aeads(&t_alg->alg, 1);
    list_del(&t_alg->entry);
    kfree(t_alg);
  }
}
