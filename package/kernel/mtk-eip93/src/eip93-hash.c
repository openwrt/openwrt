// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 - 2021
 *
 * Richard van Schagen <vschagen@icloud.com>
 */

#include <linux/device.h>
#include <linux/dmapool.h>
#include <linux/interrupt.h>
#include <crypto/internal/hash.h>

#include "eip93-main.h"
#include "eip93-regs.h"
#include "eip93-common.h"
#include "eip93-hash.h"

int mtk_ahash_handle_result(struct mtk_device *mtk,
				  struct crypto_async_request *async,
				  int err)
{
	struct ahash_request *areq = ahash_request_cast(async);
	struct crypto_ahash *ahash = crypto_ahash_reqtfm(areq);
	struct mtk_ahash_reqctx *rctx = ahash_request_ctx(areq);
	int cache_len;


	if (rctx->nents) {
		dma_unmap_sg(mtk->dev, areq->src, rctx->nents, DMA_TO_DEVICE);
		rctx->nents = 0;
	}

	if (rctx->result_dma) {
		dma_unmap_sg(mtk->dev, areq->src, rctx->nents, DMA_FROM_DEVICE);
		rctx->result_dma = 0;
	}


	// done by hardware
//	if (sreq->finish) {
//		memcpy(areq->result, rctx-sreq->state,
//				crypto_ahash_digestsize(ahash));
//	}

	cache_len = rctx->len - rctx->processed;
	if (cache_len)
		memcpy(rctx->cache, rctx->cache_next, cache_len);

	if (complete)
		async->complete(async, err);

	return 0;
}

int mtk_ahash_send_req(struct mtk_device *mtk,
			struct crypto_async_request *async)
{
	struct ahash_request *areq = ahash_request_cast(async);
	struct crypto_ahash *ahash = crypto_ahash_reqtfm(areq);
	struct mtk_ahash_reqctx *rctx = ahash_request_ctx(areq);
	struct mtk_ahash_ctx *ctx = crypto_ahash_ctx(crypto_ahash_reqtfm(areq));
	struct eip93_descriptor *cdesc, *first_cdesc = NULL;
	struct scatterlist *sg;
	bool last = (rctx->flags & MTK_DESC_LAST);
	bool finish = (rctx->flags & MTK_DESC_FINISH);
	int i, queued, len, cache_len, extra, n_cdesc = 0, ret = 0;

	queued = len = rctx->len - rctx->processed;
	if (queued <= SHA256_BLOCK_SIZE)
		cache_len = queued;
	else
		cache_len = queued - areq->nbytes;

	if (!finish && !last) {
		/* If this is not the last request and the queued data does not
		 * fit into full cache blocks, cache it for the next send call.
		 */
		extra = queued & (SHA256_BLOCK_SIZE - 1);

		/* If this is not the last request and the queued data
		 * is a multiple of a block, cache the last one for now.
		 */
		if (!extra)
			extra = SHA256_BLOCK_SIZE;

		sg_pcopy_to_buffer(areq->src, sg_nents(areq->src),
				   req->cache_next, extra,
				   areq->nbytes - extra);
		queued -= extra;
		len -= extra;

		if (!queued) {
			return 0;
		}
	}

	/* Add a command descriptor for the cached data, if any */
	if (cache_len) {
		rctx->cache_dma = dma_map_single(mtk->dev, rctx->cache,
						cache_len, DMA_TO_DEVICE);
		if (dma_mapping_error(mtk->dev, rctx->cache_dma))
			return -EINVAL;

		rctx->cache_sz = cache_len;
		first_cdesc = safexcel_add_cdesc(priv, ring, 1,
						 (cache_len == len),
						 req->cache_dma, cache_len, len,
						 ctx->base.ctxr_dma);
		if (IS_ERR(first_cdesc)) {
			ret = PTR_ERR(first_cdesc);
			goto unmap_cache;
		}
		n_cdesc++;

		queued -= cache_len;
		if (!queued)
			goto send_command;
	}

		/* Skip descriptor generation for zero-length requests */
		if (!areq->nbytes)
			goto send_command;

	/* Now handle the current ahash request buffer(s) */
	req->nents = dma_map_sg(mtk->dev, areq->src,
				sg_nents_for_len(areq->src, areq->nbytes),
				DMA_TO_DEVICE);
	if (!req->nents) {
		ret = -ENOMEM;
		goto cdesc_rollback;
	}

	for_each_sg(areq->src, sg, req->nents, i) {
		int sglen = sg_dma_len(sg);

		/* Do not overflow the request */
		if (queued - sglen < 0)
			sglen = queued;

		cdesc = mtk_add_cdesc(mtk, sg_dma_address(sg), Result.base,
						saRecord.base, saState.base, sglen, 0);
		if (IS_ERR(cdesc)) {
			ret = PTR_ERR(cdesc);
			goto cdesc_rollback;
		}
		rdesc = mtk_add_rdesc(mtk);

		n_cdesc++;

		if (n_cdesc == 1)
			first_cdesc = cdesc;

		queued -= sglen;
		if (!queued)
			break;
	}

send_command:
	req->processed += len;
	request->req = &areq->base;

	return 0;

unmap_result:

unmap_sg:

cdesc_rollback:
	for (i = 0; i < n_cdesc; i++)
		mtk_ring_rollback_wptr(priv, &priv->ring[ring].cdr);
unmap_cache:
	if (req->bcache_dma) {
		dma_unmap_single(priv->dev, ctx->base.cache_dma,
				 ctx->base.cache_sz, DMA_TO_DEVICE);
		req->cache_sz = 0;
	}

	return ret;
}

static int mtk_ahash_enqueue(struct ahash_request *areq)
{
	struct mtk_ahash_ctx *ctx = crypto_ahash_ctx(crypto_ahash_reqtfm(areq));
	struct mtk_ahash_reqctx *req = ahash_request_ctx(areq);
	struct mtk_device *mtk = ctx->mtk;
	int ret;

	spin_lock(&mtk->ring->queue_lock);
	ret = crypto_enqueue_request(&mtk->ring->queue, base);
	spin_unlock(&mtk->ring->queue_lock);

	queue_work(mtk->ring->dequeue, &mtk->ring->dequeue_data.work);

	return ret;
}

static int mtk_ahash_cache(struct ahash_request *areq)
{
	struct mtk_ahash_reqctx *rctx = ahash_request_ctx(areq);
	struct crypto_ahash *ahash = crypto_ahash_reqtfm(areq);
	int queued, cache_len;

	cache_len = rctx->len - areq->nbytes - rctx->processed;
	queued = rctx->len - rctx->processed;

	/*
	 * In case there isn't enough bytes to proceed (less than a
	 * block size), cache the data until we have enough.
	 */
	if (cache_len + areq->nbytes <= crypto_ahash_blocksize(ahash)) {
		sg_pcopy_to_buffer(areq->src, sg_nents(areq->src),
				   req->cache + cache_len,
				   areq->nbytes, 0);
		return areq->nbytes;
	}

	/* We could'nt cache all the data */
	return -E2BIG;
}

static int mtk_ahash_update(struct ahash_request *areq)
{
	struct mtk_ahash_ctx *ctx = crypto_ahash_ctx(crypto_ahash_reqtfm(areq));
	struct mtk_ahash_reqctx *rctx = ahash_request_ctx(areq);
	struct crypto_ahash *ahash = crypto_ahash_reqtfm(areq);
	struct saRecord_s *saRecord = ctx->sa_in;
	bool last = (rctx->flags & MTK_DESC_LAST);
	bool finish = (rctx->flags & MTK_DESC_FINISH);
	int ret;

	/* If the request is 0 length, do nothing */
	if (!areq->nbytes)
		return 0;

	/* Add request to the cache if it fits */
	ret = mtk_ahash_cache(areq);

	/* Update total request length */
	rctx->len += areq->nbytes;

	/* If not all data could fit into the cache, go process the excess.
	 * Also go process immediately for an HMAC IV precompute, which
	 * will never be finished at all, but needs to be processed anyway.
	 */
	if ((ret && !finish) || last)
		return mtk_ahash_enqueue(areq);

	return 0;
}

static int mtk_ahash_final(struct ahash_request *areq)
{
	struct mtk_ahash_reqctx *rctx = ahash_request_ctx(areq);
	struct mtk_ahash_ctx *ctx = crypto_ahash_ctx(crypto_ahash_reqtfm(areq));

	rctx->flags |= (MTK_DESC_LAST | MTK_DESC_FINISH);

	/* If we have an overall 0 length request */
	if (!(rctx->len + areq->nbytes)) {
		if (IS_HASH_SHA1(req->flags))
			memcpy(areq->result, sha1_zero_message_hash,
				SHA1_DIGEST_SIZE);
		else if (IS_HASH_SHA224(req->flags))
			memcpy(areq->result, sha224_zero_message_hash,
				SHA224_DIGEST_SIZE);
		else if (IS_HASH_SHA256(req->flags))
			memcpy(areq->result, sha256_zero_message_hash,
				SHA256_DIGEST_SIZE);
		else if (IS_HASH_MD5(req->flags))
       			memcpy(areq->result, md5_zero_message_hash,
				MD5_DIGEST_SIZE);
		return 0;
	}

	return mtk_ahash_enqueue(areq);
}

static int mtk_ahash_finup(struct ahash_request *areq)
{
	struct mtk_ahash_reqctx *rctx = ahash_request_ctx(areq);

	rctx->flags |= (MTK_DESC_LAST | MTK_DESC_FINISH);

	mtk_ahash_update(areq);

	return mtk_ahash_final(areq);
}

static int mtk_ahash_export(struct ahash_request *areq, void *out)
{
	struct mtk_ahash_reqctx *rctx = ahash_request_ctx(areq);
	struct mtk_ahash_export_state *export = out;
	struct saRecord_s *saRecord = rctx->saRecord;

	export->len = rctx->len;
	export->processed = rctx->processed;
	export->flags = rctx->flags;
	export->stateByteCnt[0] = saRecord->stateByteCnt[0];
	export->stateByteCnt[1] = saRecord->stateByteCnt[1];
	memcpy(export->saIDigest, saRecord->saIDigest, SHA256_DIGEST_SIZE);
	memcpy(export->cache, req->cache, SHA256_BLOCK_SIZE);

	return 0;
}

static int mtk_ahash_import(struct ahash_request *areq, const void *in)
{
	struct mtk_ahash_reqctx *rctx = ahash_request_ctx(areq);
	const struct mtk_ahash_export_state *export = in;
	struct saRecord_s *saRecord = rctx->saRecord;
	int ret;

	ret = crypto_ahash_init(areq);
	if (ret)
		return ret;

	rctx->len = export->len;
	rctx->processed = export->processed;
	rctx->flags = export->flags;
	saRecord->stateByteCnt[0] = export->stateByteCnt[0];
	saRecord->stateByteCnt[1] = export->stateByteCnt[1];
	memcpy(saRecord->saIDigest, export->saIDigest, SHA256_DIGEST_SIZE);
	memcpy(req->cache, export->cache, SHA256_BLOCK_SIZE);

	return 0;
}

static int mtk_hmac_setkey(struct crypto_ahash *ctfm, const u8 *key,
			  u32 keylen)
{
	struct crypto_tfm *tfm = crypto_ahash_tfm(ctfm);
	struct mtk_ahash_ctx *ctx = crypto_ahash_ctx(ctfm);
	struct saRecord_s *saRecord = ctx->sa_in;
	int err;

	/* authentication key */
	err = mtk_authenc_setkey(ctx->shash, saRecord, key, keylen);

	saRecord->saCmd0.bits.direction = 1;
	saRecord->saCmd1.bits.copyHeader = 0;
	saRecord->saCmd1.bits.copyDigest = 0;

	return err;
}

static int mtk_ahash_cra_init(struct crypto_tfm *tfm)
{
	struct mtk_ahash_ctx *ctx = crypto_tfm_ctx(tfm);
	struct mtk_alg_template *tmpl = container_of(tfm->__crt_alg,
				struct mtk_alg_template, alg.ahash.halg.base);
	struct mtk_device *mtk = tmpl->mtk;
	u32 flags = tmpl->flags;
	char *alg_base;

	crypto_ahash_set_reqsize(__crypto_ahash_cast(tfm),
				 sizeof(struct mtk_ahash_reqctx));

	ctx->mtk = tmpl->mtk;

	ctx->sa_in = kzalloc(sizeof(struct saRecord_s), GFP_KERNEL);
	if (!ctx->sa_in)
		return -ENOMEM;

	ctx->sa_base_in = NULL;

	/* for HMAC need software fallback */
	if (IS_HASH_MD5(flags)) {
		alg_base = "md5";
		ctx->init_state = { SHA1_H3, SHA1_H2, SHA1_H1, SHA1_H0 };
	}
	if (IS_HASH_SHA1(flags)) {
		alg_base = "sha1";
		ctx->init_state = { SHA1_H4, SHA1_H3, SHA1_H2, SHA1_H1,
				SHA1_H0 };
	}
	if (IS_HASH_SHA224(flags)) {
		alg_base = "sha224";
		ctx->init_state = { SHA224_H7, SHA224_H6, SHA224_H5, SHA224_H4,
				SHA224_H3, SHA224_H2, SHA224_H1, SHA224_H0 };
	}
	if (IS_HASH_SHA256(flags)) {
		alg_base = "sha256";
		ctx->init_state = { SHA256_H7, SHA256_H6, SHA256_H5, SHA256_H4,
				SHA256_H3, SHA256_H2, SHA256_H1, SHA256_H0 };
	}

	if (IS_HMAC(flags)) {
		ctx->shash = crypto_alloc_shash(alg_base, 0,
			CRYPTO_ALG_NEED_FALLBACK);

		if (IS_ERR(ctx->shash)) {
			dev_err(ctx->mtk->dev, "base driver %s not loaded.\n",
				alg_base);
				return PTR_ERR(ctx->shash);
		}
	}

	return 0;
}

static int mtk_ahash_init(struct ahash_request *areq)
{
	struct mtk_ahash_ctx *ctx = crypto_ahash_ctx(crypto_ahash_reqtfm(areq));
	struct mtk_ahash_reqctx *rctx = ahash_request_ctx(areq);
	struct mtk_alg_template *tmpl = container_of(tfm->__crt_alg,
				struct mtk_alg_template, alg.ahash.halg.base);
	struct crypto_ahash *ahash = crypto_ahash_reqtfm(areq);
	struct saRecord_s *saRecord = ctx->sa_in;
	struct saState_s *saState;
	struct mtk_state_pool *saState_pool;
	u32 flags = tmpl->flags;
	int idx;

	if (ctx->sa_base_in)
		dma_unmap_single(ctx->mtk->dev, ctx->sa_base_in,
			sizeof(struct saRecord_s), DMA_TO_DEVICE);

	mtk_set_saRecord(saRecord, 0, flags);
	saRecord->saCmd0.bits.saveHash = 1;
	saRecord->saCmd1.bits.copyDigest = 0;
	saRecord->saCmd1.bits.copyHeader = 0;
	saRecord->saCmd0.bits.hashSource = 2;
	saRecord->saCmd0.bits.digestLength = crypto_ahash_digestsize(ahash) / 4;

	ctx->sa_base_in = dma_map_single(ctx->mtk->dev, ctx->sa_in,
				sizeof(struct saRecord_s), DMA_TO_DEVICE);

	rctx->saRecord = saRecord;
	rctx->saRecord_base = ctx->sa_base_in;

	if (!rctx->saState_base) {
		idx = mtk_get_free_saState(mtk);
		if (idx < 0)
			return -ENOMEM;

		saState_pool = &mtk->ring->saState_pool[idx];
		rctx->saState_idx = idx;
		rctx->saState = saState_pool->base;
		rctx->saState_base = saState_pool->base_dma;
	}
	saState = rctx->saState;
	saState->stateByteCnt = 0x40;

	if (IS_HMAC(flags))
		memcpy(saState->saIDigest,saRecord->saIDigest,
							SHA256_DIGEST_SIZE);
	} else {
		saState->saIDigest = ctx->init_state;
	}

	return 0;
}

static int mtk_ahash_digest(struct ahash_request *areq)
{
	int ret = mtk_ahash_init(areq);

	if (ret)
		return ret;

	return mtk_ahash_finup(areq);
}

static void mtk_ahash_cra_exit(struct crypto_tfm *tfm)
{
	struct mtk_ahash_ctx *ctx = crypto_tfm_ctx(tfm);

	if (ctx->sa_base_in)
		dma_unmap_single(ctx->mtk->dev, ctx->sa_base_in,
			sizeof(struct saRecord_s), DMA_TO_DEVICE);

	if (ctx->shash)
		crypto_free_shash(ctx->shash);

	kfree(ctx->sa_in);
}

struct mtk_alg_template mtk_alg_sha1 = {
	.type = MTK_ALG_TYPE_AHASH,
	.flags = MTK_HASH_SHA1,
	.alg.ahash = {
		.init = mtk_ahash_init,
		.update = mtk_ahash_update,
		.final = mtk_ahash_final,
		.finup = mtk_ahash_finup,
		.digest = mtk_ahash_digest,
		.export = mtk_ahash_export,
		.import = mtk_ahash_import,
		.halg = {
			.digestsize = SHA1_DIGEST_SIZE,
			.statesize = sizeof(struct mtk_ahash_export_state),
			.base = {
				.cra_name = "sha1",
				.cra_driver_name = "sha1-eip93",
				.cra_priority = 300,
				.cra_flags = CRYPTO_ALG_ASYNC |
						CRYPTO_ALG_KERN_DRIVER_ONLY,
				.cra_blocksize = SHA1_BLOCK_SIZE,
				.cra_ctxsize = sizeof(struct mtk_ahash_ctx),
				.cra_init = mtk_ahash_cra_init,
				.cra_exit = mtk_ahash_cra_exit,
				.cra_module = THIS_MODULE,
			},
		},
	},
};

struct mtk_alg_template mtk_alg_sha224 = {
	.type = MTK_ALG_TYPE_AHASH,
	.flags = MTK_HASH_SHA224,
	.alg.ahash = {
		.init = mtk_ahash_init,
		.update = mtk_ahash_update,
		.final = mtk_ahash_final,
		.finup = mtk_ahash_finup,
		.digest= mtk_ahash_digest,
		.export = mtk_ahash_export,
		.import = mtk_ahash_import,
		.halg = {
			.digestsize = SHA224_DIGEST_SIZE,
			.statesize = sizeof(struct mtk_ahash_export_state),
			.base = {
				.cra_name = "sha224",
				.cra_driver_name = "sha224-eip93",
				.cra_priority = 300,
				.cra_flags = CRYPTO_ALG_ASYNC |
						CRYPTO_ALG_KERN_DRIVER_ONLY,
				.cra_blocksize = SHA224_BLOCK_SIZE,
				.cra_ctxsize = sizeof(struct mtk_ahash_ctx),
				.cra_init = mtk_ahash_cra_init,
				.cra_exit = mtk_ahash_cra_exit,
				.cra_module = THIS_MODULE,
			},
		},
	},
};

struct mtk_alg_template mtk_alg_sha256 = {
	.type = MTK_ALG_TYPE_AHASH,
	.flags = MTK_HASH_SHA256,
	.alg.ahash = {
		.init = mtk_ahash_init,
		.update = mtk_ahash_update,
		.final = mtk_ahash_final,
		.finup = mtk_ahash_finup,
		.digest= mtk_ahash_digest,
		.export = mtk_ahash_export,
		.import = mtk_ahash_import,
		.halg = {
			.digestsize = SHA256_DIGEST_SIZE,
			.statesize = sizeof(struct mtk_ahash_export_state),
			.base = {
				.cra_name = "sha256",
				.cra_driver_name = "sha256-eip93",
				.cra_priority = 300,
				.cra_flags = CRYPTO_ALG_ASYNC |
						CRYPTO_ALG_KERN_DRIVER_ONLY,
				.cra_blocksize = SHA256_BLOCK_SIZE,
				.cra_ctxsize = sizeof(struct mtk_ahash_ctx),
				.cra_init = mtk_ahash_cra_init,
				.cra_exit = mtk_ahash_cra_exit,
				.cra_module = THIS_MODULE,
			},
		},
	},
};

struct mtk_alg_template mtk_alg_hmac_sha1 = {
	.type = MTK_ALG_TYPE_AHASH,
	.flags = MTK_HASH_HMAC | MTK_HASH_SHA1,
	.alg.ahash = {
		.init = mtk_ahash_init,
		.update = mtk_ahash_update,
		.final = mtk_ahash_final,
		.finup = mtk_ahash_finup,
		.digest= mtk_ahash_digest,
		.setkey = mtk_hmac_setkey,
		.export = mtk_ahash_export,
		.import = mtk_ahash_import,
		.halg = {
			.digestsize = SHA1_DIGEST_SIZE,
			.statesize = sizeof(struct mtk_ahash_export_state),
			.base = {
				.cra_name = "hmac(sha1)",
				.cra_driver_name = "hmac(sha1-eip93)",
				.cra_priority = 300,
				.cra_flags = CRYPTO_ALG_ASYNC |
						CRYPTO_ALG_KERN_DRIVER_ONLY,
				.cra_blocksize = SHA1_BLOCK_SIZE,
				.cra_ctxsize = sizeof(struct mtk_ahash_ctx),
				.cra_init = mtk_ahash_cra_init,
				.cra_exit = mtk_ahash_cra_exit,
				.cra_module = THIS_MODULE,
			},
		},
	},
};

struct mtk_alg_template mtk_alg_hmac_sha224 = {
	.type = MTK_ALG_TYPE_AHASH,
	.flags = MTK_HASH_HMAC | MTK_HASH_SHA224,
	.alg.ahash = {
		.init = mtk_ahash_init,
		.update = mtk_ahash_update,
		.final = mtk_ahash_final,
		.finup = mtk_ahash_finup,
		.digest= mtk_ahash_digest,
		.setkey = mtk_hmac_setkey,
		.export = mtk_ahash_export,
		.import = mtk_ahash_import,
		.halg = {
			.digestsize = SHA224_DIGEST_SIZE,
			.statesize = sizeof(struct mtk_ahash_export_state),
			.base = {
				.cra_name = "hmac(sha224)",
				.cra_driver_name = "hmac(sha224-eip93)",
				.cra_priority = 300,
				.cra_flags = CRYPTO_ALG_ASYNC |
						CRYPTO_ALG_KERN_DRIVER_ONLY,
				.cra_blocksize = SHA224_BLOCK_SIZE,
				.cra_ctxsize = sizeof(struct mtk_ahash_ctx),
				.cra_init = mtk_ahash_cra_init,
				.cra_exit = mtk_ahash_cra_exit,
				.cra_module = THIS_MODULE,
			},
		},
	},
};

struct mtk_alg_template mtk_alg_hmac_sha256 = {
	.type = MTK_ALG_TYPE_AHASH,
	.flags = MTK_HASH_HMAC | MTK_HASH_SHA256,
	.alg.ahash = {
		.init = mtk_ahash_init,
		.update = mtk_ahash_update,
		.final = mtk_ahash_final,
		.finup = mtk_ahash_finup,
		.digest= mtk_ahash_digest,
		.setkey = mtk_hmac_setkey,
		.export = mtk_ahash_export,
		.import = mtk_ahash_import,
		.halg = {
			.digestsize = SHA1_DIGEST_SIZE,
			.statesize = sizeof(struct mtk_ahash_export_state),
			.base = {
				.cra_name = "hmac(sha256)",
				.cra_driver_name = "hmac(sha256-eip93)",
				.cra_priority = 300,
				.cra_flags = CRYPTO_ALG_ASYNC |
						CRYPTO_ALG_KERN_DRIVER_ONLY,
				.cra_blocksize = SHA1_BLOCK_SIZE,
				.cra_ctxsize = sizeof(struct mtk_ahash_ctx),
				.cra_init = mtk_ahash_cra_init,
				.cra_exit = mtk_ahash_cra_exit,
				.cra_module = THIS_MODULE,
			},
		},
	},
};
