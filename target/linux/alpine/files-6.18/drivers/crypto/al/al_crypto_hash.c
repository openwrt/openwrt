/*
 * drivers/crypto/al_crypto_hash.c
 *
 * Annapurna Labs Crypto driver - hash algorithms
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
#include <crypto/hash.h>
#include <crypto/internal/hash.h>
#include <crypto/md5.h>
#include <crypto/sha1.h>
#include <crypto/sha2.h>
#include <linux/scatterlist.h>

#include <mach/al_hal_ssm_crypto.h>

#include "al_crypto.h"

#define AL_CRYPTO_CRA_PRIORITY 300

#define AL_CRYPTO_HASH_MAX_BLOCK_SIZE SHA512_BLOCK_SIZE
#define AL_CRYPTO_HASH_MAX_DIGEST_SIZE SHA512_DIGEST_SIZE

static int ahash_init(struct ahash_request *req);

static int ahash_update(struct ahash_request *req);

static int ahash_final(struct ahash_request *req);

static int ahash_finup(struct ahash_request *req);

static int ahash_digest(struct ahash_request *req);

static int ahash_export(struct ahash_request *req, void *out);

static int ahash_import(struct ahash_request *req, const void *in);

static int ahash_setkey(struct crypto_ahash *ahash, const u8 *key,
                        unsigned int keylen);

/* ahash request ctx */
struct al_crypto_hash_req_ctx {
  /* Note 1:
   *	buf_0 and buf_1 are used for keeping the data that
   *  was not hashed during current update for the next update
   * Note 2:
   *  buf_0, buf_1 and interm are DMAed so they shouldn't
   *  share the same cache line
   * with other fields
   *	*/
  uint8_t buf_0[AL_CRYPTO_HASH_MAX_BLOCK_SIZE] ____cacheline_aligned;
  uint8_t buf_1[AL_CRYPTO_HASH_MAX_BLOCK_SIZE] ____cacheline_aligned;
  /* intermediate state */
  uint8_t interm[AL_CRYPTO_HASH_MAX_DIGEST_SIZE] ____cacheline_aligned;
  int buflen_0 ____cacheline_aligned;
  int buflen_1;
  uint8_t current_buf; /* select active buffer for current update */
  dma_addr_t buf_dma_addr;
  int buf_dma_len;
  dma_addr_t interm_dma_addr;
  bool first;
  bool last;
  uint32_t hashed_len;
};

struct al_crypto_hash_template {
  char name[CRYPTO_MAX_ALG_NAME];
  char driver_name[CRYPTO_MAX_ALG_NAME];
  char hmac_name[CRYPTO_MAX_ALG_NAME];
  char hmac_driver_name[CRYPTO_MAX_ALG_NAME];
  unsigned int blocksize;
  struct ahash_alg template_ahash;
  enum al_crypto_sa_auth_type auth_type;
  enum al_crypto_sa_sha2_mode sha2_mode;
  enum al_crypto_sa_op sa_op;
  char sw_hash_name[CRYPTO_MAX_ALG_NAME];
  unsigned int sw_hash_interm_offset;
  unsigned int sw_hash_interm_size;
};

static struct al_crypto_hash_template driver_hash[] = {
    {
        .name = "sha1",
        .driver_name = "sha1-al",
        .hmac_name = "hmac(sha1)",
        .hmac_driver_name = "hmac-sha1-al",
        .blocksize = SHA1_BLOCK_SIZE,
        .template_ahash =
            {
                .init = ahash_init,
                .update = ahash_update,
                .final = ahash_final,
                .finup = ahash_finup,
                .digest = ahash_digest,
                .export = ahash_export,
                .import = ahash_import,
                .setkey = ahash_setkey,
                .halg =
                    {
                        .digestsize = SHA1_DIGEST_SIZE,
                        .statesize = sizeof(struct al_crypto_hash_req_ctx),
                    },
            },
        .auth_type = AL_CRYPT_AUTH_SHA1,
        .sha2_mode = 0,
        .sa_op = AL_CRYPT_AUTH_ONLY,
        .sw_hash_name = "sha1",
        .sw_hash_interm_offset = offsetof(struct sha1_state, state),
        .sw_hash_interm_size = sizeof(((struct sha1_state *)0)->state),
    },
    {
        .name = "sha256",
        .driver_name = "sha256-al",
        .hmac_name = "hmac(sha256)",
        .hmac_driver_name = "hmac-sha256-al",
        .blocksize = SHA256_BLOCK_SIZE,
        .template_ahash =
            {
                .init = ahash_init,
                .update = ahash_update,
                .final = ahash_final,
                .finup = ahash_finup,
                .digest = ahash_digest,
                .export = ahash_export,
                .import = ahash_import,
                .setkey = ahash_setkey,
                .halg =
                    {
                        .digestsize = SHA256_DIGEST_SIZE,
                        .statesize = sizeof(struct al_crypto_hash_req_ctx),
                    },
            },
        .auth_type = AL_CRYPT_AUTH_SHA2,
        .sha2_mode = AL_CRYPT_SHA2_256,
        .sa_op = AL_CRYPT_AUTH_ONLY,
        .sw_hash_name = "sha256",
        .sw_hash_interm_offset = offsetof(struct sha256_state, state),
        .sw_hash_interm_size = sizeof(((struct sha256_state *)0)->state),
    },
    {
        .name = "sha256",
        .driver_name = "sha256-al",
        .hmac_name = "hmac(sha256)",
        .hmac_driver_name = "hmac-sha256-al",
        .blocksize = SHA256_BLOCK_SIZE,
        .template_ahash =
            {
                .init = ahash_init,
                .update = ahash_update,
                .final = ahash_final,
                .finup = ahash_finup,
                .digest = ahash_digest,
                .export = ahash_export,
                .import = ahash_import,
                .setkey = ahash_setkey,
                .halg =
                    {
                        .digestsize = SHA256_DIGEST_SIZE,
                        .statesize = sizeof(struct al_crypto_hash_req_ctx),
                    },
            },
        .auth_type = AL_CRYPT_AUTH_SHA2,
        .sha2_mode = AL_CRYPT_SHA2_256,
        .sa_op = AL_CRYPT_AUTH_ONLY,
        .sw_hash_name = "sha256",
        .sw_hash_interm_offset = offsetof(struct sha256_state, state),
        .sw_hash_interm_size = sizeof(((struct sha256_state *)0)->state),
    },
    {
        .name = "sha512",
        .driver_name = "sha512-al",
        .hmac_name = "hmac(sha512)",
        .hmac_driver_name = "hmac-sha512-al",
        .blocksize = SHA512_BLOCK_SIZE,
        .template_ahash =
            {
                .init = ahash_init,
                .update = ahash_update,
                .final = ahash_final,
                .finup = ahash_finup,
                .digest = ahash_digest,
                .export = ahash_export,
                .import = ahash_import,
                .setkey = ahash_setkey,
                .halg =
                    {
                        .digestsize = SHA512_DIGEST_SIZE,
                        .statesize = sizeof(struct al_crypto_hash_req_ctx),
                    },
            },
        .auth_type = AL_CRYPT_AUTH_SHA2,
        .sha2_mode = AL_CRYPT_SHA2_512,
        .sa_op = AL_CRYPT_AUTH_ONLY,
        .sw_hash_name = "sha512",
        .sw_hash_interm_offset = offsetof(struct sha512_state, state),
        .sw_hash_interm_size = sizeof(((struct sha512_state *)0)->state),
    },
    {
        .name = "sha384",
        .driver_name = "sha384-al",
        .hmac_name = "hmac(sha384)",
        .hmac_driver_name = "hmac-sha384-al",
        .blocksize = SHA384_BLOCK_SIZE,
        .template_ahash =
            {
                .init = ahash_init,
                .update = ahash_update,
                .final = ahash_final,
                .finup = ahash_finup,
                .digest = ahash_digest,
                .export = ahash_export,
                .import = ahash_import,
                .setkey = ahash_setkey,
                .halg =
                    {
                        .digestsize = SHA384_DIGEST_SIZE,
                        .statesize = sizeof(struct al_crypto_hash_req_ctx),
                    },
            },
        .auth_type = AL_CRYPT_AUTH_SHA2,
        .sha2_mode = AL_CRYPT_SHA2_384,
        .sa_op = AL_CRYPT_AUTH_ONLY,
        .sw_hash_name = "sha384",
        .sw_hash_interm_offset = offsetof(struct sha512_state, state),
        .sw_hash_interm_size = sizeof(((struct sha512_state *)0)->state),
    },
    {
        .name = "md5",
        .driver_name = "md5-al",
        .hmac_name = "hmac(md5)",
        .hmac_driver_name = "hmac-md5-al",
        .blocksize = MD5_HMAC_BLOCK_SIZE,
        .template_ahash =
            {
                .init = ahash_init,
                .update = ahash_update,
                .final = ahash_final,
                .finup = ahash_finup,
                .digest = ahash_digest,
                .export = ahash_export,
                .import = ahash_import,
                .setkey = ahash_setkey,
                .halg =
                    {
                        .digestsize = MD5_DIGEST_SIZE,
                        .statesize = sizeof(struct al_crypto_hash_req_ctx),
                    },
            },
        .auth_type = AL_CRYPT_AUTH_MD5,
        .sha2_mode = 0,
        .sa_op = AL_CRYPT_AUTH_ONLY,
        .sw_hash_name = "md5",
        .sw_hash_interm_offset = 0,
        .sw_hash_interm_size = sizeof(struct md5_state),
    },
};

struct al_crypto_hash {
  struct list_head entry;
  struct al_crypto_device *device;
  enum al_crypto_sa_auth_type auth_type;
  enum al_crypto_sa_sha2_mode sha2_mode;
  enum al_crypto_sa_op sa_op;
  struct ahash_alg ahash_alg;
  char sw_hash_name[CRYPTO_MAX_ALG_NAME];
  unsigned int sw_hash_interm_offset;
  unsigned int sw_hash_interm_size;
};

/******************************************************************************
 *****************************************************************************/
static u8 zero_message_hash_md5[MD5_DIGEST_SIZE] = {
    0xd4, 0x1d, 0x8c, 0xd9, 0x8f, 0x00, 0xb2, 0x04,
    0xe9, 0x80, 0x09, 0x98, 0xec, 0xf8, 0x42, 0x7e};

static u8 zero_message_hash_sha1[SHA1_DIGEST_SIZE] = {
    0xda, 0x39, 0xa3, 0xee, 0x5e, 0x6b, 0x4b, 0x0d, 0x32, 0x55,
    0xbf, 0xef, 0x95, 0x60, 0x18, 0x90, 0xaf, 0xd8, 0x07, 0x09};

static u8 zero_message_hash_sha256[SHA256_DIGEST_SIZE] = {
    0xe3, 0xb0, 0xc4, 0x42, 0x98, 0xfc, 0x1c, 0x14, 0x9a, 0xfb, 0xf4,
    0xc8, 0x99, 0x6f, 0xb9, 0x24, 0x27, 0xae, 0x41, 0xe4, 0x64, 0x9b,
    0x93, 0x4c, 0xa4, 0x95, 0x99, 0x1b, 0x78, 0x52, 0xb8, 0x55};

static u8 zero_message_hash_sha384[SHA384_DIGEST_SIZE] = {
    0x38, 0xb0, 0x60, 0xa7, 0x51, 0xac, 0x96, 0x38, 0x4c, 0xd9, 0x32, 0x7e,
    0xb1, 0xb1, 0xe3, 0x6a, 0x21, 0xfd, 0xb7, 0x11, 0x14, 0xbe, 0x07, 0x43,
    0x4c, 0x0c, 0xc7, 0xbf, 0x63, 0xf6, 0xe1, 0xda, 0x27, 0x4e, 0xde, 0xbf,
    0xe7, 0x6f, 0x65, 0xfb, 0xd5, 0x1a, 0xd2, 0xf1, 0x48, 0x98, 0xb9, 0x5b,
};

static u8 zero_message_hash_sha512[SHA512_DIGEST_SIZE] = {
    0xcf, 0x83, 0xe1, 0x35, 0x7e, 0xef, 0xb8, 0xbd, 0xf1, 0x54, 0x28,
    0x50, 0xd6, 0x6d, 0x80, 0x07, 0xd6, 0x20, 0xe4, 0x05, 0x0b, 0x57,
    0x15, 0xdc, 0x83, 0xf4, 0xa9, 0x21, 0xd3, 0x6c, 0xe9, 0xce, 0x47,
    0xd0, 0xd1, 0x3c, 0x5d, 0x85, 0xf2, 0xb0, 0xff, 0x83, 0x18, 0xd2,
    0x87, 0x7e, 0xec, 0x2f, 0x63, 0xb9, 0x31, 0xbd, 0x47, 0x41, 0x7a,
    0x81, 0xa5, 0x38, 0x32, 0x7a, 0xf9, 0x27, 0xda, 0x3e,
};

/******************************************************************************
 *****************************************************************************/
static inline int to_signature_size(int digest_size) {
  return (digest_size / 4) - 1;
}

/******************************************************************************
 *****************************************************************************/
static int al_crypto_hash_cra_init(struct crypto_tfm *tfm) {
  struct crypto_ahash *ahash = __crypto_ahash_cast(tfm);
  struct ahash_alg *alg = __crypto_ahash_alg(tfm->__crt_alg);
  struct al_crypto_hash *al_crypto_hash =
      container_of(alg, struct al_crypto_hash, ahash_alg);
  struct al_crypto_ctx *ctx = crypto_tfm_ctx(tfm);
  struct al_crypto_device *device = al_crypto_hash->device;
  int chan_idx = atomic_inc_return(&device->tfm_count) %
                 (device->num_channels - device->crc_channels);
  struct crypto_shash *sw_hash = NULL;

  dev_dbg(&device->pdev->dev, "%s: cra_name=%s\n", __func__,
          crypto_tfm_alg_name(tfm));

  memset(&ctx->sa, 0, sizeof(struct al_crypto_sa));

  /* Allocate SW hash for hmac long key hashing and key XOR ipad/opad
   * intermediate calculations
   */
  if (strlen(al_crypto_hash->sw_hash_name)) {
    /* TODO: is CRYPTO_ALG_NEED_FALLBACK needed here? */
    sw_hash = crypto_alloc_shash(al_crypto_hash->sw_hash_name, 0,
                                 CRYPTO_ALG_NEED_FALLBACK);
    if (IS_ERR(sw_hash)) {
      dev_err(&device->pdev->dev,
              "%s: Failed to allocate SW hash '%s' with %ld\n", __func__,
              al_crypto_hash->sw_hash_name, PTR_ERR(sw_hash));
      return PTR_ERR(sw_hash);
    }
  }
  ctx->sw_hash = sw_hash;

  ctx->chan = device->channels[chan_idx];

  ctx->sa.auth_type = al_crypto_hash->auth_type;
  ctx->sa.sha2_mode = al_crypto_hash->sha2_mode;
  ctx->sa.sa_op = al_crypto_hash->sa_op;
  ctx->sa.signature_size = to_signature_size(crypto_ahash_digestsize(ahash));

  ctx->sa.auth_hmac_en = false;
  ctx->cache_state.cached = false;
  ctx->hw_sa =
      dma_alloc_coherent(&device->pdev->dev, sizeof(struct al_crypto_hw_sa),
                         &ctx->hw_sa_dma_addr, GFP_KERNEL);

  al_crypto_hw_sa_init(&ctx->sa, ctx->hw_sa);

  crypto_ahash_set_reqsize(ahash, sizeof(struct al_crypto_hash_req_ctx));

  if (ctx->sw_hash) {
    ctx->hmac_pads =
        kmalloc(2 * crypto_shash_descsize(ctx->sw_hash), GFP_KERNEL);
    if (!ctx->hmac_pads)
      return -ENOMEM;
  }

  AL_CRYPTO_STATS_LOCK(&ctx->chan->stats_gen_lock);
  AL_CRYPTO_STATS_INC(ctx->chan->stats_gen.ahash_tfms, 1);
  AL_CRYPTO_STATS_UNLOCK(&ctx->chan->stats_gen_lock);

  return 0;
}

/******************************************************************************
 *****************************************************************************/
static void al_crypto_hash_cra_exit(struct crypto_tfm *tfm) {
  struct ahash_alg *alg = __crypto_ahash_alg(tfm->__crt_alg);
  struct al_crypto_hash *al_crypto_hash =
      container_of(alg, struct al_crypto_hash, ahash_alg);
  struct al_crypto_ctx *ctx = crypto_tfm_ctx(tfm);
  struct al_crypto_device *device = al_crypto_hash->device;

  dev_dbg(&device->pdev->dev, "%s: cra_name=%s\n", __func__,
          crypto_tfm_alg_name(tfm));

  /* LRU list access has to be protected */
  spin_lock_bh(&ctx->chan->prep_lock);
  if (ctx->cache_state.cached)
    al_crypto_cache_remove_lru(ctx->chan, &ctx->cache_state);
  spin_unlock_bh(&ctx->chan->prep_lock);

  if (ctx->hw_sa_dma_addr)
    dma_free_coherent(&device->pdev->dev, sizeof(struct al_crypto_hw_sa),
                      ctx->hw_sa, ctx->hw_sa_dma_addr);

  if (ctx->sw_hash)
    crypto_free_shash(ctx->sw_hash);

  if (ctx->hmac_pads)
    kfree(ctx->hmac_pads);

  AL_CRYPTO_STATS_LOCK(&ctx->chan->stats_gen_lock);
  AL_CRYPTO_STATS_DEC(ctx->chan->stats_gen.ahash_tfms, 1);
  AL_CRYPTO_STATS_UNLOCK(&ctx->chan->stats_gen_lock);

  return;
}

/******************************************************************************
 *****************************************************************************/
/* DMA unmap buffers for ahash request
 */
static inline void al_crypto_dma_unmap_ahash(struct al_crypto_chan *chan,
                                             struct al_crypto_sw_desc *desc,
                                             unsigned int digestsize) {
  struct ahash_request *req = (struct ahash_request *)desc->req;
  struct al_crypto_hash_req_ctx *req_ctx = ahash_request_ctx(req);

  if (req_ctx->buf_dma_len)
    dma_unmap_single(to_dev(chan), req_ctx->buf_dma_addr, req_ctx->buf_dma_len,
                     DMA_TO_DEVICE);
  if (desc->src_nents)
    dma_unmap_sg(to_dev(chan), req->src, desc->src_nents, DMA_TO_DEVICE);

  if (req_ctx->last) {
    dma_unmap_single(to_dev(chan), req_ctx->interm_dma_addr,
                     AL_CRYPTO_HASH_MAX_DIGEST_SIZE, DMA_BIDIRECTIONAL);
    memcpy(req->result, req_ctx->interm, digestsize);
  }
}

/******************************************************************************
 *****************************************************************************/
static inline void zero_message_result_copy(struct ahash_request *req) {
  struct crypto_ahash *ahash = crypto_ahash_reqtfm(req);
  struct al_crypto_ctx *ctx = crypto_ahash_ctx(ahash);
  unsigned int digestsize = crypto_ahash_digestsize(ahash);
  struct al_crypto_chan *chan = ctx->chan;
  u8 *zero_message = NULL;

  dev_dbg(to_dev(chan), "%s: auth_type=%d\n", __func__, ctx->sa.auth_type);

  switch (ctx->sa.auth_type) {
  case (AL_CRYPT_AUTH_MD5):
    zero_message = zero_message_hash_md5;
    break;
  case (AL_CRYPT_AUTH_SHA1):
    zero_message = zero_message_hash_sha1;
    break;
  case (AL_CRYPT_AUTH_SHA2):
    if (ctx->sa.sha2_mode == AL_CRYPT_SHA2_256)
      zero_message = zero_message_hash_sha256;
    if (ctx->sa.sha2_mode == AL_CRYPT_SHA2_384)
      zero_message = zero_message_hash_sha384;
    if (ctx->sa.sha2_mode == AL_CRYPT_SHA2_512)
      zero_message = zero_message_hash_sha512;
    break;
  default:
    dev_err(to_dev(chan), "ERROR, unsupported zero message\n");
    return;
  }

  memcpy(req->result, zero_message, digestsize);
}
/******************************************************************************
 *****************************************************************************/
/* Cleanup single ahash request - invoked from cleanup tasklet (interrupt
 * handler)
 */
void al_crypto_cleanup_single_ahash(struct al_crypto_chan *chan,
                                    struct al_crypto_sw_desc *desc,
                                    uint32_t comp_status) {
  struct ahash_request *req = (struct ahash_request *)desc->req;
  struct crypto_ahash *ahash = crypto_ahash_reqtfm(req);
  unsigned int digestsize = crypto_ahash_digestsize(ahash);

  dev_dbg(to_dev(chan), "%s: comp_status=%x\n", __func__, comp_status);

  al_crypto_dma_unmap_ahash(chan, desc, digestsize);

  req->base.complete(&req->base, 0);
}

/******************************************************************************
 *****************************************************************************/
static int ahash_init(struct ahash_request *req) {
  struct crypto_ahash *ahash = crypto_ahash_reqtfm(req);
  struct al_crypto_ctx *ctx = crypto_ahash_ctx(ahash);
  struct al_crypto_hash_req_ctx *req_ctx = ahash_request_ctx(req);
  struct al_crypto_chan *chan = ctx->chan;

  dev_dbg(to_dev(chan), "%s: cra_name=%s nbytes=%d\n", __func__,
          crypto_tfm_alg_name(crypto_ahash_tfm(ahash)), req->nbytes);

  req_ctx->first = true;
  req_ctx->last = false;

  req_ctx->current_buf = 0;
  req_ctx->buflen_0 = 0;
  req_ctx->buflen_1 = 0;
  req_ctx->buf_dma_addr = 0;
  req_ctx->interm_dma_addr =
      dma_map_single(to_dev(chan), req_ctx->interm,
                     AL_CRYPTO_HASH_MAX_DIGEST_SIZE, DMA_BIDIRECTIONAL);
  if (dma_mapping_error(to_dev(chan), req_ctx->interm_dma_addr)) {
    dev_err(to_dev(chan), "dma_map_single failed!\n");
    return -ENOMEM;
  }
  req_ctx->hashed_len = 0;

  return 0;
}

/******************************************************************************
 *****************************************************************************/
static inline void
ahash_req_prepare_xaction_buffers(struct ahash_request *req,
                                  struct al_crypto_sw_desc *desc, int to_hash,
                                  int src_nents, int *buflen, int *src_idx) {
  struct crypto_ahash *ahash = crypto_ahash_reqtfm(req);
  struct al_crypto_ctx *ctx = crypto_ahash_ctx(ahash);
  struct al_crypto_hash_req_ctx *req_ctx = ahash_request_ctx(req);
  struct al_crypto_transaction *xaction = &desc->hal_xaction;

  dev_dbg(to_dev(ctx->chan), "%s: to_hash=%d buflen=%d src_nents=%d\n",
          __func__, to_hash, *buflen, src_nents);

  *src_idx = 0;

  if (*buflen) {
    desc->src_bufs[*src_idx].addr = req_ctx->buf_dma_addr;
    desc->src_bufs[*src_idx].len = *buflen;
    xaction->auth_in_len += desc->src_bufs[*src_idx].len;
    (*src_idx)++;
  }

  if (src_nents)
    sg_map_to_xaction_buffers(req->src, desc->src_bufs, to_hash - *buflen,
                              src_idx);
}

/******************************************************************************
 *****************************************************************************/
static inline void ahash_update_stats(struct al_crypto_transaction *xaction,
                                      struct al_crypto_chan *chan) {
  AL_CRYPTO_STATS_INC(chan->stats_prep.ahash_reqs, 1);
  AL_CRYPTO_STATS_INC(chan->stats_prep.ahash_bytes, xaction->auth_in_len);

  if (xaction->auth_in_len <= 512)
    AL_CRYPTO_STATS_INC(chan->stats_prep.ahash_reqs_le512, 1);
  else if ((xaction->auth_in_len > 512) && (xaction->auth_in_len <= 2048))
    AL_CRYPTO_STATS_INC(chan->stats_prep.ahash_reqs_512_2048, 1);
  else if ((xaction->auth_in_len > 2048) && (xaction->auth_in_len <= 4096))
    AL_CRYPTO_STATS_INC(chan->stats_prep.ahash_reqs_2048_4096, 1);
  else
    AL_CRYPTO_STATS_INC(chan->stats_prep.ahash_reqs_gt4096, 1);
}

/******************************************************************************
 *****************************************************************************/
static inline void ahash_req_prepare_xaction(struct ahash_request *req,
                                             struct al_crypto_sw_desc *desc,
                                             int to_hash, int src_nents,
                                             uint8_t *buf, int *buflen) {
  struct crypto_ahash *ahash = crypto_ahash_reqtfm(req);
  struct al_crypto_ctx *ctx = crypto_ahash_ctx(ahash);
  struct al_crypto_hash_req_ctx *req_ctx = ahash_request_ctx(req);
  struct al_crypto_chan *chan = ctx->chan;
  struct al_crypto_transaction *xaction;
  unsigned int digestsize = crypto_ahash_digestsize(ahash);
  unsigned int ivsize;
  int src_idx;

  dev_dbg(
      to_dev(chan),
      "%s: to_hash=%d src_ents=%d buflen=%d digestsize=%d first=%d last=%d\n",
      __func__, to_hash, src_nents, *buflen, digestsize, req_ctx->first,
      req_ctx->last);

  /* In SHA384 the ivsize is 64 bytes and not 48 bytes. */
  ivsize = (digestsize == SHA384_DIGEST_SIZE) ? SHA512_DIGEST_SIZE : digestsize;

  /* prepare hal transaction */
  xaction = &desc->hal_xaction;
  memset(xaction, 0, sizeof(struct al_crypto_transaction));
  xaction->auth_sign_in.len = 0;
  xaction->auth_fl_valid = AL_TRUE;
  xaction->auth_in_off = 0;
  /* if first, there's no input intermediate */
  if (unlikely(req_ctx->first)) {
    req_ctx->first = false;
    xaction->auth_first = AL_TRUE;
    xaction->auth_iv_in.len = 0;
    xaction->auth_iv_in.addr = (al_phys_addr_t)(uintptr_t)NULL;
  } else {
    xaction->auth_first = AL_FALSE;
    xaction->auth_iv_in.addr = xaction->auth_iv_out.addr =
        req_ctx->interm_dma_addr;
    xaction->auth_iv_in.len = xaction->auth_iv_out.len = ivsize;
  }

  if (unlikely(req_ctx->last)) {
    xaction->auth_last = AL_TRUE;
    xaction->auth_sign_out.addr = req_ctx->interm_dma_addr;
    xaction->auth_sign_out.len = digestsize;
    xaction->auth_iv_out.len = 0;
    xaction->auth_iv_out.addr = (al_phys_addr_t)(uintptr_t)NULL;
    xaction->auth_bcnt = req_ctx->hashed_len;

    /* count the first hmac key^ipad block */
    if (ctx->sa.auth_hmac_en)
      xaction->auth_bcnt += crypto_tfm_alg_blocksize(crypto_ahash_tfm(ahash));
  } else {
    xaction->auth_last = AL_FALSE;
    xaction->auth_iv_out.addr = req_ctx->interm_dma_addr;
    xaction->auth_iv_out.len = ivsize;
    xaction->auth_sign_out.len = 0;
    xaction->auth_bcnt = 0;
  }

  xaction->dir = AL_CRYPT_ENCRYPT;
  xaction->auth_in_len = 0;

  ahash_req_prepare_xaction_buffers(req, desc, to_hash, src_nents, buflen,
                                    &src_idx);

  BUG_ON(src_idx > AL_SSM_MAX_SRC_DESCS);

  xaction->src_size = xaction->auth_in_len;
  xaction->src.bufs = &desc->src_bufs[0];
  xaction->src.num = src_idx;

  dev_dbg(to_dev(chan), "%s: ctx->cache_state.cached=%d\n", __func__,
          ctx->cache_state.cached);

  if (!ctx->cache_state.cached) {
    xaction->sa_indx =
        al_crypto_cache_replace_lru(chan, &ctx->cache_state, NULL);
    xaction->sa_in.addr = ctx->hw_sa_dma_addr;
    xaction->sa_in.len = sizeof(struct al_crypto_hw_sa);
  } else {
    al_crypto_cache_update_lru(chan, &ctx->cache_state);
    xaction->sa_indx = ctx->cache_state.idx;
    xaction->sa_in.len = 0;
  }

  dev_dbg(to_dev(chan), "%s: sa_op=%d, auth_type=%d, sha2_mode=%d\n", __func__,
          ctx->sa.sa_op, ctx->sa.auth_type, ctx->sa.sha2_mode);

  xaction->flags = AL_SSM_INTERRUPT;

  ahash_update_stats(xaction, chan);
}

/******************************************************************************
 *****************************************************************************/
/* Main hash processing function that handles update/final/finup and digest
 *
 * Implementation is based on the assumption that the caller waits for
 * completion of every operation before issuing the next operation
 */
static int ahash_process_req(struct ahash_request *req, unsigned int nbytes) {
  struct crypto_ahash *ahash = crypto_ahash_reqtfm(req);
  struct al_crypto_ctx *ctx = crypto_ahash_ctx(ahash);
  struct al_crypto_hash_req_ctx *req_ctx = ahash_request_ctx(req);
  unsigned int digestsize = crypto_ahash_digestsize(ahash);
  struct al_crypto_chan *chan = ctx->chan;
  uint8_t *buf = req_ctx->current_buf ? req_ctx->buf_1 : req_ctx->buf_0;
  int *buflen = req_ctx->current_buf ? &req_ctx->buflen_1 : &req_ctx->buflen_0;
  uint8_t *next_buf = req_ctx->current_buf ? req_ctx->buf_0 : req_ctx->buf_1;
  int *next_buflen =
      req_ctx->current_buf ? &req_ctx->buflen_0 : &req_ctx->buflen_1;
  int in_len = *buflen + nbytes;
  int to_hash, idx;
  int src_nents = 0;
  struct al_crypto_sw_desc *desc;
  int src_sg_nents = sg_nents(req->src);
  int rc = 0;

  dev_dbg(to_dev(chan),
          "%s: nbytes=%d first=%d last=%d inlen=%d buflen=%d next_buflen=%d "
          "src_sg_nents=%d auth_hmac_en=%d\n",
          __func__, nbytes, req_ctx->first, req_ctx->last, in_len, *buflen,
          *next_buflen, src_sg_nents, ctx->sa.auth_hmac_en);

  if (!req_ctx->last) {
    /* if aligned, do not hash last block */
    *next_buflen =
        (in_len & (crypto_tfm_alg_blocksize(crypto_ahash_tfm(ahash)) - 1))
            ?: crypto_tfm_alg_blocksize(crypto_ahash_tfm(ahash));
    to_hash = in_len - *next_buflen;

    /* Ignore not last empty update requests */
    if (unlikely(in_len == 0))
      return rc;
  } else {
    if (unlikely(in_len == 0)) {
      if (!ctx->sa.auth_hmac_en)
        zero_message_result_copy(req);
      else {
        uint8_t *ipad = ctx->hmac_pads;
        uint8_t *opad = ctx->hmac_pads + crypto_shash_descsize(ctx->sw_hash);

        struct shash_desc *desc = kmalloc(
            sizeof(struct shash_desc) + crypto_shash_descsize(ctx->sw_hash),
            GFP_KERNEL);
        if (!desc)
          return -ENOMEM;

        dev_dbg(to_dev(chan), "%s:%d: digestsize=%d\n", __func__, __LINE__,
                crypto_shash_digestsize(ctx->sw_hash));

        desc->tfm = ctx->sw_hash;
        crypto_shash_set_flags(ctx->sw_hash,
                               crypto_shash_get_flags(ctx->sw_hash) &
                                   CRYPTO_TFM_REQ_MAY_SLEEP);
        rc = crypto_shash_init(desc) ? :
					crypto_shash_import(desc, ipad) ? :
					crypto_shash_final(desc, req->result) ? :
					crypto_shash_import(desc, opad) ? :
					crypto_shash_finup(desc, req->result,
							   crypto_shash_digestsize(ctx->sw_hash),
							   req->result);
        kfree(desc);
      }
      return rc;
    }

    *next_buflen = 0;
    to_hash = in_len;
  }

  dev_dbg(to_dev(chan), "%s: to_hash=%d buflen=%d next_buflen=%d\n", __func__,
          to_hash, *buflen, *next_buflen);

  if (!to_hash) {
    sg_copy_to_buffer(req->src, src_sg_nents, buf + *buflen, nbytes);
    *buflen = *next_buflen;
    return rc;
  }

  if (*buflen) {
    req_ctx->buf_dma_addr =
        dma_map_single(to_dev(chan), buf, *buflen, DMA_TO_DEVICE);
    if (dma_mapping_error(to_dev(chan), req_ctx->buf_dma_addr))
      return -ENOMEM;
    req_ctx->buf_dma_len = *buflen;
  } else
    req_ctx->buf_dma_len = 0;

  spin_lock_bh(&chan->prep_lock);
  if (likely(al_crypto_get_sw_desc(chan, 1) == 0))
    idx = chan->head;
  else {
    spin_unlock_bh(&chan->prep_lock);
    if (req_ctx->buf_dma_len)
      dma_unmap_single(to_dev(chan), req_ctx->buf_dma_addr,
                       req_ctx->buf_dma_len, DMA_TO_DEVICE);
    return -ENOSPC;
  }

  if (*next_buflen) {
    sg_pcopy_to_buffer(req->src, src_sg_nents, next_buf, *next_buflen,
                       nbytes - *next_buflen);
    req_ctx->current_buf = !req_ctx->current_buf;
  }

  if (nbytes) {
    BUG_ON(nbytes < (*next_buflen));

    src_nents = sg_nents_for_len(req->src, nbytes - (*next_buflen));

    dev_dbg(to_dev(chan), "%s: src_nents=%d\n", __func__, src_nents);

    dma_map_sg(to_dev(chan), req->src, src_nents, DMA_TO_DEVICE);
  }

  chan->sw_desc_num_locked = 1;
  chan->tx_desc_produced = 0;

  desc = al_crypto_get_ring_ent(chan, idx);
  desc->req = (void *)req;
  desc->req_type = AL_CRYPTO_REQ_AHASH;
  desc->src_nents = src_nents;

  ahash_req_prepare_xaction(req, desc, to_hash, src_nents, buf, buflen);

  /* send crypto transaction to engine */
  rc = al_crypto_dma_prepare(chan->hal_crypto, chan->idx, &desc->hal_xaction);
  if (unlikely(rc != 0)) {
    dev_err(to_dev(chan), "%s: al_crypto_dma_prepare failed with %d\n",
            __func__, rc);

    al_crypto_dma_unmap_ahash(chan, desc, digestsize);

    spin_unlock_bh(&chan->prep_lock);
    return rc;
  }

  chan->tx_desc_produced += desc->hal_xaction.tx_descs_count;

  al_crypto_tx_submit(chan);

  req_ctx->hashed_len += to_hash;

  spin_unlock_bh(&chan->prep_lock);

  return -EINPROGRESS;
}

/******************************************************************************
 *****************************************************************************/
static int ahash_update(struct ahash_request *req) {
  struct crypto_ahash *ahash = crypto_ahash_reqtfm(req);
  struct al_crypto_ctx *ctx = crypto_ahash_ctx(ahash);
  struct al_crypto_hash_req_ctx *req_ctx = ahash_request_ctx(req);

  dev_dbg(to_dev(ctx->chan), "%s:%d\n", __func__, __LINE__);

  req_ctx->last = false;

  return ahash_process_req(req, req->nbytes);
}

/******************************************************************************
 *****************************************************************************/
static int ahash_final(struct ahash_request *req) {
  struct crypto_ahash *ahash = crypto_ahash_reqtfm(req);
  struct al_crypto_ctx *ctx = crypto_ahash_ctx(ahash);
  struct al_crypto_hash_req_ctx *req_ctx = ahash_request_ctx(req);

  dev_dbg(to_dev(ctx->chan), "%s:%d\n", __func__, __LINE__);

  req_ctx->last = true;

  return ahash_process_req(req, 0);
}

/******************************************************************************
 *****************************************************************************/
static int ahash_finup(struct ahash_request *req) {
  struct crypto_ahash *ahash = crypto_ahash_reqtfm(req);
  struct al_crypto_ctx *ctx = crypto_ahash_ctx(ahash);
  struct al_crypto_hash_req_ctx *req_ctx = ahash_request_ctx(req);

  dev_dbg(to_dev(ctx->chan), "%s:%d\n", __func__, __LINE__);

  req_ctx->last = true;

  return ahash_process_req(req, req->nbytes);
}

/******************************************************************************
 *****************************************************************************/
static int ahash_digest(struct ahash_request *req) {
  struct crypto_ahash *ahash = crypto_ahash_reqtfm(req);
  struct al_crypto_ctx *ctx = crypto_ahash_ctx(ahash);
  struct al_crypto_hash_req_ctx *req_ctx = ahash_request_ctx(req);

  dev_dbg(to_dev(ctx->chan), "%s:%d\n", __func__, __LINE__);

  ahash_init(req);
  req_ctx->last = true;

  return ahash_process_req(req, req->nbytes);
}

/******************************************************************************
 *****************************************************************************/
static int ahash_export(struct ahash_request *req, void *out) {
  struct crypto_ahash *ahash = crypto_ahash_reqtfm(req);
  struct al_crypto_ctx *ctx = crypto_ahash_ctx(ahash);
  struct al_crypto_hash_req_ctx *req_ctx = ahash_request_ctx(req);

  dev_dbg(to_dev(ctx->chan), "%s:%d\n", __func__, __LINE__);

  memcpy(out, req_ctx, sizeof(*req_ctx));

  return 0;
}

/******************************************************************************
 *****************************************************************************/
static int ahash_import(struct ahash_request *req, const void *in) {
  struct crypto_ahash *ahash = crypto_ahash_reqtfm(req);
  struct al_crypto_ctx *ctx = crypto_ahash_ctx(ahash);
  struct al_crypto_hash_req_ctx *req_ctx = ahash_request_ctx(req);

  dev_dbg(to_dev(ctx->chan), "%s:%d\n", __func__, __LINE__);

  memcpy(req_ctx, in, sizeof(*req_ctx));

  return 0;
}

/******************************************************************************
 *****************************************************************************/
/* Generate intermediate hash of hmac^opad and hmac^ipad using sw hash engine
 * and place the results in ctx->sa.
 */
int hmac_setkey(struct al_crypto_ctx *ctx, const u8 *key, unsigned int keylen,
                unsigned int sw_hash_interm_offset,
                unsigned int sw_hash_interm_size) {
  unsigned int blocksize, digestsize, descsize;
  int i, rc;

  dev_dbg(to_dev(ctx->chan),
          "%s: keylen=%d sw_hash_interm_offset=%d sw_hash_interm_size=%d\n",
          __func__, keylen, sw_hash_interm_offset, sw_hash_interm_size);

  /* Based on code from the hmac module */
  blocksize = crypto_shash_blocksize(ctx->sw_hash);
  digestsize = crypto_shash_digestsize(ctx->sw_hash);
  descsize = crypto_shash_descsize(ctx->sw_hash);

  dev_dbg(to_dev(ctx->chan), "%s: blocksize=%d digestsize=%d descsize=%d\n",
          __func__, blocksize, digestsize, descsize);

  {
    uint8_t *ipad = ctx->hmac_pads;
    uint8_t *opad = ctx->hmac_pads + descsize;

    struct shash_desc *desc =
        kmalloc(sizeof(struct shash_desc) + crypto_shash_descsize(ctx->sw_hash),
                GFP_KERNEL);
    if (!desc)
      return -ENOMEM;

    desc->tfm = ctx->sw_hash;
    crypto_shash_set_flags(ctx->sw_hash, crypto_shash_get_flags(ctx->sw_hash) &
                                             CRYPTO_TFM_REQ_MAY_SLEEP);

    /* hash the key if longer than blocksize */
    if (keylen > blocksize) {
      int err;

      err = crypto_shash_digest(desc, key, keylen, ipad);
      if (err) {
        kfree(desc);
        return err;
      }

      keylen = digestsize;
    } else
      memcpy(ipad, key, keylen);

    memset(ipad + keylen, 0, blocksize - keylen);
    memcpy(opad, ipad, blocksize);

    /* Generate XORs with ipad and opad */
    for (i = 0; i < blocksize; i++) {
      ipad[i] ^= AL_CRYPTO_HASH_HMAC_IPAD;
      opad[i] ^= AL_CRYPTO_HASH_HMAC_OPAD;
    }

    /* Generate intermediate results using SW hash */
    rc = crypto_shash_init(desc) ? :
			crypto_shash_update(desc, ipad, blocksize) ? :
			crypto_shash_export(desc, ipad) ? :
			crypto_shash_init(desc) ? :
			crypto_shash_update(desc, opad, blocksize) ? :
			crypto_shash_export(desc, opad);

    if (rc == 0) {
      unsigned int offset = sw_hash_interm_offset;
      unsigned int size = sw_hash_interm_size;

      /* Copy intermediate results to SA */
      memcpy(ctx->sa.hmac_iv_in, ipad + offset, size);
      memcpy(ctx->sa.hmac_iv_out, opad + offset, size);

      print_hex_dump_debug(KBUILD_MODNAME ": hmac_iv_in: ", DUMP_PREFIX_OFFSET,
                           16, 1, ctx->sa.hmac_iv_in, size, false);
      print_hex_dump_debug(KBUILD_MODNAME ": hmac_iv_out: ", DUMP_PREFIX_OFFSET,
                           16, 1, ctx->sa.hmac_iv_out, size, false);

      ctx->sa.auth_hmac_en = true;
    }

    kfree(desc);
  }
  return rc;
}

/******************************************************************************
 *****************************************************************************/
static int ahash_setkey(struct crypto_ahash *ahash, const u8 *key,
                        unsigned int keylen) {
  struct ahash_alg *alg = crypto_ahash_alg(ahash);
  struct al_crypto_hash *al_crypto_hash =
      container_of(alg, struct al_crypto_hash, ahash_alg);
  struct al_crypto_ctx *ctx = crypto_ahash_ctx(ahash);
  int rc;

  dev_dbg(to_dev(ctx->chan), "%s: keylen=%d sw_hash=%d\n", __func__, keylen,
          !!ctx->sw_hash);

  if (!ctx->sw_hash)
    return 0;

  rc = hmac_setkey(ctx, key, keylen, al_crypto_hash->sw_hash_interm_offset,
                   al_crypto_hash->sw_hash_interm_size);
  if (rc == 0) {
    al_crypto_hw_sa_init(&ctx->sa, ctx->hw_sa);

    /* mark the sa as not cached, will update in next xaction */
    spin_lock_bh(&ctx->chan->prep_lock);
    if (ctx->cache_state.cached)
      al_crypto_cache_remove_lru(ctx->chan, &ctx->cache_state);
    spin_unlock_bh(&ctx->chan->prep_lock);
  }

  return rc;
}

/******************************************************************************
 *****************************************************************************/
static struct al_crypto_hash *
al_crypto_hash_alloc(struct al_crypto_device *device,
                     struct al_crypto_hash_template *template, bool keyed) {
  struct al_crypto_hash *t_alg;
  struct ahash_alg *halg;
  struct crypto_alg *alg;

  t_alg = kzalloc(sizeof(struct al_crypto_hash), GFP_KERNEL);
  if (!t_alg) {
    dev_err(&device->pdev->dev, "failed to allocate t_alg\n");
    return ERR_PTR(-ENOMEM);
  }

  t_alg->ahash_alg = template->template_ahash;
  halg = &t_alg->ahash_alg;
  alg = &halg->halg.base;

  dev_dbg(&device->pdev->dev,
          "%s: name=%s sw_hash_name=%s keyed=%d statesize=%d\n", __func__,
          alg->cra_name, t_alg->sw_hash_name, keyed, halg->halg.statesize);

  if (keyed) {
    snprintf(alg->cra_name, CRYPTO_MAX_ALG_NAME, "%s", template->hmac_name);
    snprintf(alg->cra_driver_name, CRYPTO_MAX_ALG_NAME, "%s",
             template->hmac_driver_name);
    snprintf(t_alg->sw_hash_name, CRYPTO_MAX_ALG_NAME, "%s",
             template->sw_hash_name);
  } else {
    snprintf(alg->cra_name, CRYPTO_MAX_ALG_NAME, "%s", template->name);
    snprintf(alg->cra_driver_name, CRYPTO_MAX_ALG_NAME, "%s",
             template->driver_name);
    snprintf(t_alg->sw_hash_name, CRYPTO_MAX_ALG_NAME, "%s", "");
  }

  if (!keyed)
    halg->setkey = NULL;

  alg->cra_module = THIS_MODULE;
  alg->cra_init = al_crypto_hash_cra_init;
  alg->cra_exit = al_crypto_hash_cra_exit;
  alg->cra_priority = AL_CRYPTO_CRA_PRIORITY;
  alg->cra_blocksize = template->blocksize;
  alg->cra_alignmask = 0;
  alg->cra_ctxsize = sizeof(struct al_crypto_ctx);
  alg->cra_flags =
      CRYPTO_ALG_ASYNC | CRYPTO_ALG_TYPE_AHASH | CRYPTO_ALG_NEED_FALLBACK;

  t_alg->auth_type = template->auth_type;
  t_alg->sha2_mode = template->sha2_mode;
  t_alg->sa_op = template->sa_op;
  t_alg->device = device;
  t_alg->sw_hash_interm_offset = template->sw_hash_interm_offset;
  t_alg->sw_hash_interm_size = template->sw_hash_interm_size;

  return t_alg;
}

/******************************************************************************
 *****************************************************************************/
int al_crypto_hash_init(struct al_crypto_device *device) {
  int i;
  int err = 0;

  INIT_LIST_HEAD(&device->hash_list);

  /* tfm count is initialized in alg, move to core?? */
  /* atomic_set(&device->tfm_count, -1); */

  /* register crypto algorithms the device supports */
  for (i = 0; i < ARRAY_SIZE(driver_hash); i++) {
    struct al_crypto_hash *t_alg;

    /* register hmac version */
    t_alg = al_crypto_hash_alloc(device, &driver_hash[i], true);
    if (IS_ERR(t_alg)) {
      err = PTR_ERR(t_alg);
      dev_warn(&device->pdev->dev, "%s alg allocation failed with %d\n",
               driver_hash[i].driver_name, err);
      continue;
    }

    err = crypto_register_ahash(&t_alg->ahash_alg);
    if (err) {
      dev_warn(&device->pdev->dev, "%s alg registration failed with %d\n",
               t_alg->ahash_alg.halg.base.cra_driver_name, err);
      kfree(t_alg);
    } else
      list_add_tail(&t_alg->entry, &device->hash_list);

    /* register unkeyed version */
    t_alg = al_crypto_hash_alloc(device, &driver_hash[i], false);
    if (IS_ERR(t_alg)) {
      err = PTR_ERR(t_alg);
      dev_warn(&device->pdev->dev, "%s alg allocation failed with err %d\n",
               driver_hash[i].driver_name, err);
      continue;
    }

    err = crypto_register_ahash(&t_alg->ahash_alg);
    if (err) {
      dev_warn(&device->pdev->dev, "%s alg registration failed with %d\n",
               t_alg->ahash_alg.halg.base.cra_driver_name, err);
      kfree(t_alg);
    } else
      list_add_tail(&t_alg->entry, &device->hash_list);
  }

  if (!list_empty(&device->hash_list))
    dev_info(&device->pdev->dev,
             "hash algorithms registered in /proc/crypto\n");

  return err;
}

/******************************************************************************
 *****************************************************************************/
void al_crypto_hash_terminate(struct al_crypto_device *device) {
  struct al_crypto_hash *t_alg, *n;

  if (!device->hash_list.next)
    return;

  list_for_each_entry_safe(t_alg, n, &device->hash_list, entry) {
    crypto_unregister_ahash(&t_alg->ahash_alg);
    list_del(&t_alg->entry);
    kfree(t_alg);
  }
}
