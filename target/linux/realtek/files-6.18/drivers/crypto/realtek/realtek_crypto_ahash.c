// SPDX-License-Identifier: GPL-2.0-only
/*
 * Crypto acceleration support for Realtek crypto engine. Based on ideas from
 * Rockchip & SafeXcel driver plus Realtek OpenWrt RTK.
 *
 * Copyright (c) 2022, Markus Stockhausen <markus.stockhausen@gmx.de>
 */

#include <crypto/internal/hash.h>
#include <linux/dma-mapping.h>

#include "realtek_crypto.h"

static inline struct ahash_request *fallback_request_ctx(struct ahash_request *areq)
{
	char *p = (char *)ahash_request_ctx(areq);

	return (struct ahash_request *)(p + offsetof(struct rtcr_ahash_req, vector));
}

static inline void *fallback_export_state(void *export)
{
	char *p = (char *)export;

	return (void *)(p + offsetof(struct rtcr_ahash_req, vector));
}

static int rtcr_process_hash(struct ahash_request *areq, int opmode)
{
	unsigned int len, nextbuflen, datalen, padlen, reqlen, sgmap = 0;
	struct rtcr_ahash_req *hreq = ahash_request_ctx(areq);
	struct crypto_ahash *tfm = crypto_ahash_reqtfm(areq);
	struct rtcr_ahash_ctx *hctx = crypto_ahash_ctx(tfm);
	int sgcnt = hreq->state & RTCR_REQ_SG_MASK;
	struct rtcr_crypto_dev *cdev = hctx->cdev;
	struct scatterlist *sg = areq->src;
	int idx, srcidx, dstidx, ret;
	u64 pad[RTCR_HASH_PAD_SIZE];
	dma_addr_t paddma, bufdma;
	char *ppad;

	/* Quick checks if processing is really needed */
	if (unlikely(!areq->nbytes) && !(opmode & RTCR_HASH_FINAL))
		return 0;

	if (hreq->buflen + areq->nbytes < 64 && !(opmode & RTCR_HASH_FINAL)) {
		hreq->buflen += sg_pcopy_to_buffer(areq->src, sg_nents(areq->src),
						   hreq->buf + hreq->buflen,
						   areq->nbytes, 0);
		return 0;
	}

	/* calculate required parts of the request */
	datalen = (opmode & RTCR_HASH_UPDATE) ? areq->nbytes : 0;
	if (opmode & RTCR_HASH_FINAL) {
		nextbuflen = 0;
		padlen = 64 - ((hreq->buflen + datalen) & 63);
		if (padlen < 9)
			padlen += 64;
		hreq->totallen += hreq->buflen + datalen;

		memset(pad, 0, sizeof(pad) - sizeof(u64));
		ppad = (char *)&pad[RTCR_HASH_PAD_SIZE] - padlen;
		*ppad = 0x80;
		pad[RTCR_HASH_PAD_SIZE - 1] = hreq->state & RTCR_REQ_MD5 ?
					      cpu_to_le64(hreq->totallen << 3) :
					      cpu_to_be64(hreq->totallen << 3);
	} else {
		nextbuflen = (hreq->buflen + datalen) & 63;
		padlen = 0;
		datalen -= nextbuflen;
		hreq->totallen += hreq->buflen + datalen;
	}
	reqlen = hreq->buflen + datalen + padlen;

	/* Write back any uncommitted data to memory. */
	if (hreq->buflen)
		bufdma = dma_map_single(cdev->dev, hreq->buf, hreq->buflen, DMA_TO_DEVICE);
	if (padlen)
		paddma = dma_map_single(cdev->dev, ppad, padlen, DMA_TO_DEVICE);
	if (datalen)
		sgmap = dma_map_sg(cdev->dev, sg, sgcnt, DMA_TO_DEVICE);

	/* Get free space in the ring */
	sgcnt = 1 + (hreq->buflen ? 1 : 0) + (datalen ? sgcnt : 0) + (padlen ? 1 : 0);

	ret = rtcr_alloc_ring(cdev, sgcnt, &srcidx, &dstidx, RTCR_WB_LEN_HASH, NULL);
	if (ret)
		return ret;
	/*
	 * Feed input data into the rings. Start with destination ring and fill
	 * source ring afterwards. Ensure that the owner flag of the first source
	 * ring is the last that becomes visible to the engine.
	 */
	rtcr_add_dst_to_ring(cdev, dstidx, NULL, 0, hreq->vector, 0);

	idx = srcidx;
	if (hreq->buflen) {
		idx = rtcr_inc_src_idx(idx, 1);
		rtcr_add_src_to_ring(cdev, idx, hreq->buf, hreq->buflen, reqlen);
	}

	while (datalen) {
		len = min(sg_dma_len(sg), datalen);

		idx = rtcr_inc_src_idx(idx, 1);
		rtcr_add_src_to_ring(cdev, idx, sg_virt(sg), len, reqlen);

		datalen -= len;
		if (datalen)
			sg = sg_next(sg);
	}

	if (padlen) {
		idx = rtcr_inc_src_idx(idx, 1);
		rtcr_add_src_to_ring(cdev, idx, ppad, padlen, reqlen);
	}

	rtcr_add_src_pad_to_ring(cdev, idx, reqlen);
	rtcr_add_src_ahash_to_ring(cdev, srcidx, hctx->opmode, reqlen);

	/* Off we go */
	rtcr_kick_engine(cdev);
	if (rtcr_wait_for_request(cdev, dstidx))
		return -EINVAL;

	if (sgmap)
		dma_unmap_sg(cdev->dev, sg, sgcnt, DMA_TO_DEVICE);
	if (hreq->buflen)
		dma_unmap_single(cdev->dev, bufdma, hreq->buflen, DMA_TO_DEVICE);
	if (nextbuflen)
		sg_pcopy_to_buffer(sg, sg_nents(sg), hreq->buf, nextbuflen, len);
	if (padlen) {
		dma_unmap_single(cdev->dev, paddma, padlen, DMA_TO_DEVICE);
		memcpy(areq->result, hreq->vector, crypto_ahash_digestsize(tfm));
	}

	hreq->state |= RTCR_REQ_FB_ACT;
	hreq->buflen = nextbuflen;

	return 0;
}

static void rtcr_check_request(struct ahash_request *areq, int opmode)
{
	struct rtcr_ahash_req *hreq = ahash_request_ctx(areq);
	struct scatterlist *sg = areq->src;
	int reqlen, sgcnt, sgmax;

	if (hreq->state & RTCR_REQ_FB_ACT)
		return;

	if (reqlen > RTCR_MAX_REQ_SIZE) {
		hreq->state |= RTCR_REQ_FB_ACT;
		return;
	}

	sgcnt = 0;
	sgmax = RTCR_MAX_SG_AHASH - (hreq->buflen ? 1 : 0);
	reqlen = areq->nbytes;
	if (!(opmode & RTCR_HASH_FINAL)) {
		reqlen -= (hreq->buflen + reqlen) & 63;
		sgmax--;
	}

	while (reqlen > 0) {
		reqlen -= sg_dma_len(sg);
		sgcnt++;
		sg = sg_next(sg);
	}

	if (sgcnt > sgmax)
		hreq->state |= RTCR_REQ_FB_ACT;
	else
		hreq->state = (hreq->state & ~RTCR_REQ_SG_MASK) | sgcnt;
}

static bool rtcr_check_fallback(struct ahash_request *areq)
{
	struct ahash_request *freq = fallback_request_ctx(areq);
	struct rtcr_ahash_req *hreq = ahash_request_ctx(areq);
	struct crypto_ahash *tfm = crypto_ahash_reqtfm(areq);
	struct rtcr_ahash_ctx *hctx = crypto_ahash_ctx(tfm);
	union rtcr_fallback_state state;

	if (!(hreq->state & RTCR_REQ_FB_ACT))
		return false;

	if (!(hreq->state & RTCR_REQ_FB_RDY)) {
		/* Convert state to generic fallback state */
		if (hreq->state & RTCR_REQ_MD5) {
			memcpy(state.md5.hash, hreq->vector, MD5_DIGEST_SIZE);
			if (hreq->totallen)
				cpu_to_le32_array(state.md5.hash, 4);
			memcpy(state.md5.block, hreq->buf, SHA1_BLOCK_SIZE);
			state.md5.byte_count = hreq->totallen + (u64)hreq->buflen;
		} else {
			memcpy(state.sha1.state, hreq->vector, SHA1_DIGEST_SIZE);
			memcpy(state.sha1.buffer, &hreq->buf, SHA1_BLOCK_SIZE);
			state.sha1.count = hreq->totallen + (u64)hreq->buflen;
		}
	}

	ahash_request_set_tfm(freq, hctx->fback);
	ahash_request_set_crypt(freq, areq->src, areq->result, areq->nbytes);

	if (!(hreq->state & RTCR_REQ_FB_RDY)) {
		crypto_ahash_import(freq, &state);
		hreq->state |= RTCR_REQ_FB_RDY;
	}

	return true;
}

static int rtcr_ahash_init(struct ahash_request *areq)
{
	struct rtcr_ahash_req *hreq = ahash_request_ctx(areq);
	struct crypto_ahash *tfm = crypto_ahash_reqtfm(areq);
	int ds = crypto_ahash_digestsize(tfm);

	memset(hreq, 0, sizeof(*hreq));

	hreq->vector[0] = SHA1_H0;
	hreq->vector[1] = SHA1_H1;
	hreq->vector[2] = SHA1_H2;
	hreq->vector[3] = SHA1_H3;
	hreq->vector[4] = SHA1_H4;

	hreq->state |= (ds == MD5_DIGEST_SIZE) ? RTCR_REQ_MD5 : RTCR_REQ_SHA1;

	return 0;
}

static int rtcr_ahash_update(struct ahash_request *areq)
{
	struct ahash_request *freq = fallback_request_ctx(areq);

	rtcr_check_request(areq, RTCR_HASH_UPDATE);
	if (rtcr_check_fallback(areq))
		return crypto_ahash_update(freq);
	return rtcr_process_hash(areq, RTCR_HASH_UPDATE);
}

static int rtcr_ahash_final(struct ahash_request *areq)
{
	struct ahash_request *freq = fallback_request_ctx(areq);

	if (rtcr_check_fallback(areq))
		return crypto_ahash_final(freq);

	return rtcr_process_hash(areq, RTCR_HASH_FINAL);
}

static int rtcr_ahash_finup(struct ahash_request *areq)
{
	struct ahash_request *freq = fallback_request_ctx(areq);

	rtcr_check_request(areq, RTCR_HASH_FINAL | RTCR_HASH_UPDATE);
	if (rtcr_check_fallback(areq))
		return crypto_ahash_finup(freq);

	return rtcr_process_hash(areq, RTCR_HASH_FINAL | RTCR_HASH_UPDATE);
}

static int rtcr_ahash_digest(struct ahash_request *areq)
{
	struct ahash_request *freq = fallback_request_ctx(areq);
	int ret;

	ret = rtcr_ahash_init(areq);
	if (ret)
		return ret;

	rtcr_check_request(areq, RTCR_HASH_FINAL | RTCR_HASH_UPDATE);
	if (rtcr_check_fallback(areq))
		return crypto_ahash_digest(freq);

	return rtcr_process_hash(areq, RTCR_HASH_FINAL | RTCR_HASH_UPDATE);
}

static int rtcr_ahash_import(struct ahash_request *areq, const void *in)
{
	const void *fexp = (const void *)fallback_export_state((void *)in);
	struct ahash_request *freq = fallback_request_ctx(areq);
	struct rtcr_ahash_req *hreq = ahash_request_ctx(areq);
	const struct rtcr_ahash_req *hexp = in;

	hreq->state = get_unaligned(&hexp->state);
	if (hreq->state & RTCR_REQ_FB_ACT)
		hreq->state |= RTCR_REQ_FB_RDY;

	if (rtcr_check_fallback(areq))
		return crypto_ahash_import(freq, fexp);

	memcpy(hreq, hexp, sizeof(struct rtcr_ahash_req));

	return 0;
}

static int rtcr_ahash_export(struct ahash_request *areq, void *out)
{
	struct ahash_request *freq = fallback_request_ctx(areq);
	struct rtcr_ahash_req *hreq = ahash_request_ctx(areq);
	void *fexp = fallback_export_state(out);
	struct rtcr_ahash_req *hexp = out;

	if (rtcr_check_fallback(areq)) {
		put_unaligned(hreq->state, &hexp->state);
		return crypto_ahash_export(freq, fexp);
	}

	memcpy(hexp, hreq, sizeof(struct rtcr_ahash_req));

	return 0;
}

static int rtcr_ahash_cra_init(struct crypto_tfm *tfm)
{
	struct crypto_ahash *ahash = __crypto_ahash_cast(tfm);
	struct rtcr_ahash_ctx *hctx = crypto_tfm_ctx(tfm);
	struct rtcr_crypto_dev *cdev = hctx->cdev;
	struct rtcr_alg_template *tmpl;

	tmpl = container_of(__crypto_ahash_alg(tfm->__crt_alg),
			     struct rtcr_alg_template, alg.ahash);

	hctx->cdev = tmpl->cdev;
	hctx->opmode = tmpl->opmode;
	hctx->fback = crypto_alloc_ahash(crypto_tfm_alg_name(tfm), 0,
					 CRYPTO_ALG_ASYNC | CRYPTO_ALG_NEED_FALLBACK);

	if (IS_ERR(hctx->fback)) {
		dev_err(cdev->dev, "could not allocate fallback for %s\n",
			crypto_tfm_alg_name(tfm));
		return PTR_ERR(hctx->fback);
	}

	crypto_ahash_set_reqsize(ahash, max(sizeof(struct rtcr_ahash_req),
					    offsetof(struct rtcr_ahash_req, vector) +
					    sizeof(struct ahash_request) +
					    crypto_ahash_reqsize(hctx->fback)));

	return 0;
}

static void rtcr_ahash_cra_exit(struct crypto_tfm *tfm)
{
	struct rtcr_ahash_ctx *hctx = crypto_tfm_ctx(tfm);

	crypto_free_ahash(hctx->fback);
}

struct rtcr_alg_template rtcr_ahash_md5 = {
	.type = RTCR_ALG_AHASH,
	.opmode = RTCR_SRC_OP_MS_HASH | RTCR_SRC_OP_HASH_MD5,
	.alg.ahash = {
		.init = rtcr_ahash_init,
		.update = rtcr_ahash_update,
		.final = rtcr_ahash_final,
		.finup = rtcr_ahash_finup,
		.export = rtcr_ahash_export,
		.import = rtcr_ahash_import,
		.digest = rtcr_ahash_digest,
		.halg = {
			.digestsize = MD5_DIGEST_SIZE,
			/* statesize calculated during initialization */
			.base = {
				.cra_name = "md5",
				.cra_driver_name = "realtek-md5",
				.cra_priority = 300,
				.cra_flags = CRYPTO_ALG_ASYNC | CRYPTO_ALG_NEED_FALLBACK,
				.cra_blocksize = SHA1_BLOCK_SIZE,
				.cra_ctxsize = sizeof(struct rtcr_ahash_ctx),
				.cra_alignmask = 0,
				.cra_init = rtcr_ahash_cra_init,
				.cra_exit = rtcr_ahash_cra_exit,
				.cra_module = THIS_MODULE,
			}
		}
	}
};

struct rtcr_alg_template rtcr_ahash_sha1 = {
	.type = RTCR_ALG_AHASH,
	.opmode = RTCR_SRC_OP_MS_HASH | RTCR_SRC_OP_HASH_SHA1,
	.alg.ahash = {
		.init = rtcr_ahash_init,
		.update = rtcr_ahash_update,
		.final = rtcr_ahash_final,
		.finup = rtcr_ahash_finup,
		.export = rtcr_ahash_export,
		.import = rtcr_ahash_import,
		.digest = rtcr_ahash_digest,
		.halg = {
			.digestsize = SHA1_DIGEST_SIZE,
			/* statesize calculated during initialization */
			.base = {
				.cra_name = "sha1",
				.cra_driver_name = "realtek-sha1",
				.cra_priority = 300,
				.cra_flags = CRYPTO_ALG_ASYNC | CRYPTO_ALG_NEED_FALLBACK,
				.cra_blocksize = SHA1_BLOCK_SIZE,
				.cra_ctxsize = sizeof(struct rtcr_ahash_ctx),
				.cra_alignmask = 0,
				.cra_init = rtcr_ahash_cra_init,
				.cra_exit = rtcr_ahash_cra_exit,
				.cra_module = THIS_MODULE,
			}
		}
	}
};
