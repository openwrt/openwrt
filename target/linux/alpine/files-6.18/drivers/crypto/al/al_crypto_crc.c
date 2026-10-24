/*
 * drivers/crypto/al_crypto_crc.c
 *
 * Annapurna Labs Crypto driver - crc/checksum algorithms
 *
 * Copyright (C) 2013 Annapurna Labs Ltd.
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
#include <linux/scatterlist.h>

#include <mach/al_hal_ssm_crc_memcpy.h>
#include <mach/al_hal_ssm_crypto.h>

#include "al_crypto.h"

#define AL_CRYPTO_CRA_PRIORITY 300

static int crc_init(struct ahash_request *req);

static int crc_update(struct ahash_request *req);

static int crc_final(struct ahash_request *req);

static int crc_finup(struct ahash_request *req);

static int crc_digest(struct ahash_request *req);

static int crc_export(struct ahash_request *req, void *out);

static int crc_import(struct ahash_request *req, const void *in);

static int crc_setkey(struct crypto_ahash *ahash, const u8 *key,
                      unsigned int keylen);

struct al_crc_req_ctx {
  /* Make sure the following field isn't share the same cache line
   * with other fields.
   * This field is DMAed */
  uint32_t result ____cacheline_aligned;
  bool last ____cacheline_aligned;
  struct al_crypto_cache_state cache_state;
  dma_addr_t crc_dma_addr;
};

struct al_crc_ctx {
  struct al_crypto_chan *chan;
  enum al_crc_checksum_type crcsum_type;
  uint32_t key;
};

struct al_crc_template {
  char name[CRYPTO_MAX_ALG_NAME];
  char driver_name[CRYPTO_MAX_ALG_NAME];
  unsigned int blocksize;
  struct ahash_alg template_ahash;
  enum al_crc_checksum_type crcsum_type;
};

static struct al_crc_template driver_crc[] = {
    {
        .name = "crc32c",
        .driver_name = "crc32c-al",
        .blocksize = CHKSUM_BLOCK_SIZE,
        .template_ahash =
            {
                .init = crc_init,
                .update = crc_update,
                .final = crc_final,
                .finup = crc_finup,
                .digest = crc_digest,
                .export = crc_export,
                .import = crc_import,
                .setkey = crc_setkey,
                .halg =
                    {
                        .digestsize = CHKSUM_DIGEST_SIZE,
                        .statesize = sizeof(struct al_crc_req_ctx),
                    },
            },
        .crcsum_type = AL_CRC_CHECKSUM_CRC32C,
    },
};

struct al_crc {
  struct list_head entry;
  struct al_crypto_device *device;
  enum al_crc_checksum_type crcsum_type;
  struct ahash_alg ahash_alg;
};

/******************************************************************************
 *****************************************************************************/
static int al_crc_cra_init(struct crypto_tfm *tfm) {
  struct crypto_alg *base = tfm->__crt_alg;
  struct hash_alg_common *halg =
      container_of(base, struct hash_alg_common, base);
  struct ahash_alg *alg = container_of(halg, struct ahash_alg, halg);
  struct al_crc *al_crc = container_of(alg, struct al_crc, ahash_alg);
  struct al_crc_ctx *ctx = crypto_tfm_ctx(tfm);
  struct al_crypto_device *device = al_crc->device;
  int chan_idx =
      (atomic_inc_return(&device->crc_tfm_count) % device->crc_channels) +
      (device->num_channels - device->crc_channels);

  ctx->chan = device->channels[chan_idx];

  ctx->crcsum_type = al_crc->crcsum_type;

  ctx->key = 0;

  crypto_ahash_set_reqsize(__crypto_ahash_cast(tfm),
                           sizeof(struct al_crc_req_ctx));

  AL_CRYPTO_STATS_LOCK(&ctx->chan->stats_gen_lock);
  AL_CRYPTO_STATS_INC(ctx->chan->stats_gen.crc_tfms, 1);
  AL_CRYPTO_STATS_UNLOCK(&ctx->chan->stats_gen_lock);

  return 0;
}

/******************************************************************************
 *****************************************************************************/
static void al_crc_cra_exit(struct crypto_tfm *tfm) {
  struct al_crc_ctx *ctx = crypto_tfm_ctx(tfm);

  AL_CRYPTO_STATS_LOCK(&ctx->chan->stats_gen_lock);
  AL_CRYPTO_STATS_DEC(ctx->chan->stats_gen.crc_tfms, 1);
  AL_CRYPTO_STATS_UNLOCK(&ctx->chan->stats_gen_lock);

  return;
}

/******************************************************************************
 *****************************************************************************/
/* DMA unmap buffers for crc request
 */
static inline void al_crypto_dma_unmap_crc(struct al_crypto_chan *chan,
                                           struct al_crypto_sw_desc *desc) {
  struct ahash_request *req = (struct ahash_request *)desc->req;
  struct al_crc_req_ctx *req_ctx = ahash_request_ctx(req);
  struct crypto_ahash *ahash = crypto_ahash_reqtfm(req);
  unsigned int digestsize = crypto_ahash_digestsize(ahash);

  if (desc->src_nents)
    dma_unmap_sg(to_dev(chan), req->src, desc->src_nents, DMA_TO_DEVICE);

  if (req_ctx->last) {
    dma_unmap_single(to_dev(chan), req_ctx->crc_dma_addr, digestsize,
                     DMA_BIDIRECTIONAL);
    put_unaligned_le32(req_ctx->result, req->result);
  }
}

/******************************************************************************
 *****************************************************************************/
/* Cleanup single crc request - invoked from cleanup tasklet (interrupt
 * handler)
 */
void al_crypto_cleanup_single_crc(struct al_crypto_chan *chan,
                                  struct al_crypto_sw_desc *desc,
                                  uint32_t comp_status) {
  struct ahash_request *req = (struct ahash_request *)desc->req;
  struct crypto_ahash *ahash = crypto_ahash_reqtfm(req);
  struct al_crc_ctx *ctx = crypto_ahash_ctx(ahash);
  struct al_crc_req_ctx *req_ctx = ahash_request_ctx(req);

  al_crypto_dma_unmap_crc(chan, desc);

  /* LRU list access has to be protected */
  if (req_ctx->last) {
    spin_lock(&ctx->chan->prep_lock);
    if (req_ctx->cache_state.cached)
      al_crypto_cache_remove_lru(chan, &req_ctx->cache_state);
    spin_unlock(&ctx->chan->prep_lock);
  }

  req->base.complete(&req->base, 0);
}

/******************************************************************************
 *****************************************************************************/
static int crc_init(struct ahash_request *req) {
  struct crypto_ahash *ahash = crypto_ahash_reqtfm(req);
  struct al_crc_ctx *ctx = crypto_ahash_ctx(ahash);
  struct al_crc_req_ctx *req_ctx = ahash_request_ctx(req);
  struct al_crypto_chan *chan = ctx->chan;
  unsigned int digestsize = crypto_ahash_digestsize(ahash);

  req_ctx->last = false;
  req_ctx->cache_state.cached = false;

  put_unaligned_le32(ctx->key, &req_ctx->result);

  req_ctx->crc_dma_addr = dma_map_single(to_dev(chan), &req_ctx->result,
                                         digestsize, DMA_BIDIRECTIONAL);
  if (dma_mapping_error(to_dev(chan), req_ctx->crc_dma_addr)) {
    dev_err(to_dev(chan), "dma_map_single failed\n");
    return -ENOMEM;
  }

  return 0;
}

/******************************************************************************
 *****************************************************************************/
static inline void
crc_req_prepare_xaction_buffers(struct ahash_request *req,
                                struct al_crypto_sw_desc *desc, int nbytes,
                                int src_nents, int *src_idx) {
  *src_idx = 0;

  if (src_nents)
    sg_map_to_xaction_buffers(req->src, desc->src_bufs, nbytes, src_idx);
}

/******************************************************************************
 *****************************************************************************/
static inline void crc_update_stats(int nbytes, struct al_crypto_chan *chan) {
  AL_CRYPTO_STATS_INC(chan->stats_prep.crc_reqs, 1);
  AL_CRYPTO_STATS_INC(chan->stats_prep.crc_bytes, nbytes);

  if (nbytes <= 512)
    AL_CRYPTO_STATS_INC(chan->stats_prep.crc_reqs_le512, 1);
  else if ((nbytes > 512) && (nbytes <= 2048))
    AL_CRYPTO_STATS_INC(chan->stats_prep.crc_reqs_512_2048, 1);
  else if ((nbytes > 2048) && (nbytes <= 4096))
    AL_CRYPTO_STATS_INC(chan->stats_prep.crc_reqs_2048_4096, 1);
  else
    AL_CRYPTO_STATS_INC(chan->stats_prep.crc_reqs_gt4096, 1);
}

/******************************************************************************
 *****************************************************************************/
static inline void crc_req_prepare_xaction(struct ahash_request *req,
                                           int nbytes,
                                           struct al_crypto_sw_desc *desc,
                                           int src_nents) {
  struct crypto_ahash *ahash = crypto_ahash_reqtfm(req);
  struct al_crc_ctx *ctx = crypto_ahash_ctx(ahash);
  struct al_crc_req_ctx *req_ctx = ahash_request_ctx(req);
  struct al_crypto_chan *chan = ctx->chan;
  struct al_crc_transaction *xaction;
  unsigned int digestsize = crypto_ahash_digestsize(ahash);
  int src_idx;

  /* prepare hal transaction */
  xaction = &desc->hal_crc_xaction;
  memset(xaction, 0, sizeof(struct al_crc_transaction));
  xaction->crcsum_type = ctx->crcsum_type;
  xaction->xor_valid = AL_TRUE;
  xaction->in_xor = ~0;
  xaction->res_xor = ~0;

  /* if the entry is not cached, take stored iv */
  if (!(req_ctx->cache_state.cached)) {
    xaction->crc_iv_in.addr = req_ctx->crc_dma_addr;
    xaction->crc_iv_in.len = digestsize;
  }

  /* both store in cache and output intermediate result */
  /* cached result will be used unless it will be replaced */
  xaction->crc_out.addr = req_ctx->crc_dma_addr;
  xaction->crc_out.len = digestsize;

  if (likely(!req_ctx->last)) {
    xaction->st_crc_out = AL_TRUE;

    if (!req_ctx->cache_state.cached) {
      xaction->cached_crc_indx =
          al_crypto_cache_replace_lru(chan, &req_ctx->cache_state, NULL);
      xaction->flags = AL_SSM_BARRIER;
    } else {
      al_crypto_cache_update_lru(chan, &req_ctx->cache_state);
      xaction->cached_crc_indx = req_ctx->cache_state.idx;
    }
  }

  crc_req_prepare_xaction_buffers(req, desc, nbytes, src_nents, &src_idx);

  xaction->src.bufs = &desc->src_bufs[0];
  xaction->src.num = src_idx;

  dev_dbg(to_dev(chan), "%s: req_ctx->cache_state.cached=%d\n", __func__,
          req_ctx->cache_state.cached);

  xaction->flags = AL_SSM_INTERRUPT;

  crc_update_stats(nbytes, chan);
}

/******************************************************************************
 *****************************************************************************/
/* Main CRC processing function that handles update/final/finup and digest
 *
 * Implementation is based on the assumption that the caller waits for
 * completion of every operation before issuing the next operation
 */
static int crc_process_req(struct ahash_request *req, unsigned int nbytes) {
  struct crypto_ahash *ahash = crypto_ahash_reqtfm(req);
  struct al_crc_ctx *ctx = crypto_ahash_ctx(ahash);
  struct al_crc_req_ctx *req_ctx = ahash_request_ctx(req);
  struct al_crypto_chan *chan = ctx->chan;
  unsigned int digestsize = crypto_ahash_digestsize(ahash);

  int idx;
  int src_nents = 0;
  struct al_crypto_sw_desc *desc;
  int rc = 0;

  dev_dbg(to_dev(chan), "%s: nbytes=%d, last=%d\n", __func__, nbytes,
          req_ctx->last);

  if (nbytes) {
    src_nents = sg_nents_for_len(req->src, nbytes);

    dev_dbg(to_dev(chan), "%s: src_nents=%d\n", __func__, src_nents);

    dma_map_sg(to_dev(chan), req->src, src_nents, DMA_TO_DEVICE);

    spin_lock_bh(&chan->prep_lock);
    if (likely(al_crypto_get_sw_desc(chan, 1) == 0))
      idx = chan->head;
    else {
      spin_unlock_bh(&chan->prep_lock);
      dev_err(to_dev(chan), "%s: al_crypto_get_sw_desc failed!\n", __func__);

      if (src_nents)
        dma_unmap_sg(to_dev(chan), req->src, src_nents, DMA_TO_DEVICE);

      return -ENOSPC;
    }

    chan->sw_desc_num_locked = 1;
    chan->tx_desc_produced = 0;

    desc = al_crypto_get_ring_ent(chan, idx);
    desc->req = (void *)req;
    desc->req_type = AL_CRYPTO_REQ_CRC;
    desc->src_nents = src_nents;

    crc_req_prepare_xaction(req, nbytes, desc, src_nents);

    /* send crypto transaction to engine */
    rc = al_crc_csum_prepare(chan->hal_crypto, chan->idx,
                             &desc->hal_crc_xaction);
    if (unlikely(rc != 0)) {
      dev_err(to_dev(chan), "al_crypto_dma_prepare failed!\n");

      al_crypto_dma_unmap_crc(chan, desc);

      spin_unlock_bh(&chan->prep_lock);
      return rc;
    }

    chan->tx_desc_produced += desc->hal_crc_xaction.tx_descs_count;

    al_crypto_tx_submit(chan);

    spin_unlock_bh(&chan->prep_lock);

    return -EINPROGRESS;
  } else if (likely(req_ctx->last)) {
    dma_unmap_single(to_dev(chan), req_ctx->crc_dma_addr, digestsize,
                     DMA_BIDIRECTIONAL);
    put_unaligned_le32(req_ctx->result, req->result);

    spin_lock_bh(&ctx->chan->prep_lock);
    if (req_ctx->cache_state.cached)
      al_crypto_cache_remove_lru(chan, &req_ctx->cache_state);
    spin_unlock_bh(&ctx->chan->prep_lock);
  }

  return rc;
}

/******************************************************************************
 *****************************************************************************/
static int crc_update(struct ahash_request *req) {
  struct al_crc_req_ctx *req_ctx = ahash_request_ctx(req);

  req_ctx->last = false;

  return crc_process_req(req, req->nbytes);
}

/******************************************************************************
 *****************************************************************************/
static int crc_final(struct ahash_request *req) {
  struct al_crc_req_ctx *req_ctx = ahash_request_ctx(req);

  req_ctx->last = true;

  return crc_process_req(req, 0);
}

/******************************************************************************
 *****************************************************************************/
static int crc_finup(struct ahash_request *req) {
  struct al_crc_req_ctx *req_ctx = ahash_request_ctx(req);

  req_ctx->last = true;

  return crc_process_req(req, req->nbytes);
}

/******************************************************************************
 *****************************************************************************/
static int crc_digest(struct ahash_request *req) {
  struct al_crc_req_ctx *req_ctx = ahash_request_ctx(req);

  crc_init(req);
  req_ctx->last = true;

  return crc_process_req(req, req->nbytes);
}

/******************************************************************************
 *****************************************************************************/
static int crc_export(struct ahash_request *req, void *out) {
  struct al_crc_req_ctx *state = ahash_request_ctx(req);

  memcpy(out, state, sizeof(struct al_crc_req_ctx));

  return 0;
}

/******************************************************************************
 *****************************************************************************/
static int crc_import(struct ahash_request *req, const void *in) {
  struct al_crc_req_ctx *state = ahash_request_ctx(req);

  memcpy(state, in, sizeof(struct al_crc_req_ctx));

  return 0;
}

/******************************************************************************
 *****************************************************************************/
static int crc_setkey(struct crypto_ahash *ahash, const u8 *key,
                      unsigned int keylen) {
  struct al_crc_ctx *ctx = crypto_ahash_ctx(ahash);

  if (keylen != sizeof(ctx->key))
    return -EINVAL;

  ctx->key = ~get_unaligned_le32(key);

  return 0;
}

/******************************************************************************
 *****************************************************************************/
static struct al_crc *al_crc_alloc(struct al_crypto_device *device,
                                   struct al_crc_template *template) {
  struct al_crc *t_alg;
  struct ahash_alg *halg;
  struct crypto_alg *alg;

  t_alg = kzalloc(sizeof(struct al_crc), GFP_KERNEL);
  if (!t_alg) {
    dev_err(&device->pdev->dev, "failed to allocate t_alg\n");
    return ERR_PTR(-ENOMEM);
  }

  t_alg->ahash_alg = template->template_ahash;
  halg = &t_alg->ahash_alg;
  alg = &halg->halg.base;

  snprintf(alg->cra_name, CRYPTO_MAX_ALG_NAME, "%s", template->name);
  snprintf(alg->cra_driver_name, CRYPTO_MAX_ALG_NAME, "%s",
           template->driver_name);

  alg->cra_module = THIS_MODULE;
  alg->cra_init = al_crc_cra_init;
  alg->cra_exit = al_crc_cra_exit;
  alg->cra_priority = AL_CRYPTO_CRA_PRIORITY;
  alg->cra_blocksize = template->blocksize;
  alg->cra_alignmask = 0;
  alg->cra_ctxsize = sizeof(struct al_crc_ctx);
  alg->cra_flags = CRYPTO_ALG_ASYNC | CRYPTO_ALG_OPTIONAL_KEY;

  t_alg->crcsum_type = template->crcsum_type;
  t_alg->device = device;

  return t_alg;
}

/******************************************************************************
 *****************************************************************************/
int al_crypto_crc_init(struct al_crypto_device *device) {
  int i;
  int err = 0;

  INIT_LIST_HEAD(&device->crc_list);

  if (!device->crc_channels)
    return 0;

  atomic_set(&device->crc_tfm_count, -1);

  /* register crypto algorithms the device supports */
  for (i = 0; i < ARRAY_SIZE(driver_crc); i++) {
    struct al_crc *t_alg;

    t_alg = al_crc_alloc(device, &driver_crc[i]);
    if (IS_ERR(t_alg)) {
      err = PTR_ERR(t_alg);
      dev_warn(&device->pdev->dev, "%s alg allocation failed\n",
               driver_crc[i].driver_name);
      continue;
    }

    err = crypto_register_ahash(&t_alg->ahash_alg);
    if (err) {
      dev_warn(&device->pdev->dev, "%s alg registration failed\n",
               t_alg->ahash_alg.halg.base.cra_driver_name);
      kfree(t_alg);
    } else
      list_add_tail(&t_alg->entry, &device->crc_list);
  }

  if (!list_empty(&device->crc_list))
    dev_info(&device->pdev->dev,
             "crc/csum algorithms registered in /proc/crypto\n");

  return 0;
}

/******************************************************************************
 *****************************************************************************/
void al_crypto_crc_terminate(struct al_crypto_device *device) {
  struct al_crc *t_alg, *n;

  if (!device->crc_list.next)
    return;

  list_for_each_entry_safe(t_alg, n, &device->crc_list, entry) {
    crypto_unregister_ahash(&t_alg->ahash_alg);
    list_del(&t_alg->entry);
    kfree(t_alg);
  }
}
