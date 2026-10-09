// SPDX-License-Identifier: GPL-2.0-only
/*
 * Crypto acceleration support for Realtek crypto engine. Based on ideas from
 * Rockchip & SafeXcel driver plus Realtek OpenWrt RTK.
 *
 * Copyright (c) 2022, Markus Stockhausen <markus.stockhausen@gmx.de>
 */

#include <crypto/internal/skcipher.h>
#include <linux/dma-mapping.h>

#include "realtek_crypto.h"

static inline void rtcr_inc_iv(u8 *iv, int cnt)
{
	u32 *ctr = (u32 *)iv + 4;
	u32 old, new, carry = cnt;

	/* avoid looping with crypto_inc() */
	do {
		old = be32_to_cpu(*--ctr);
		new = old + carry;
		*ctr = cpu_to_be32(new);
		carry = (new < old) && (ctr > (u32 *)iv) ? 1 : 0;
	} while (carry);
}

static inline void rtcr_cut_skcipher_len(int *reqlen, int opmode, u8 *iv)
{
	int len = min(*reqlen, RTCR_MAX_REQ_SIZE);

	if (opmode & RTCR_SRC_OP_CRYPT_CTR) {
		/* limit data as engine does not wrap around cleanly */
		u32 ctr = be32_to_cpu(*((u32 *)iv + 3));
		int blocks = min(~ctr, 0x3fffu) + 1;

		len = min(blocks * AES_BLOCK_SIZE, len);
	}

	*reqlen = len;
}

static inline void rtcr_max_skcipher_len(int *reqlen, struct scatterlist **sg,
					 int *sgoff, int *sgcnt)
{
	int len, cnt, sgnoff, sgmax = RTCR_MAX_SG_SKCIPHER, datalen, maxlen = *reqlen;
	struct scatterlist *sgn;

redo:
	datalen = cnt = 0;
	sgnoff = *sgoff;
	sgn = *sg;

	while (sgn && (datalen < maxlen) && (cnt < sgmax)) {
		cnt++;
		len = min((int)sg_dma_len(sgn) - sgnoff, maxlen - datalen);
		datalen += len;
		if (len + sgnoff < sg_dma_len(sgn)) {
			sgnoff = sgnoff + len;
			break;
		}
		sgn = sg_next(sgn);
		sgnoff = 0;
		if (unlikely((cnt == sgmax) && (datalen < AES_BLOCK_SIZE))) {
			/* expand search to get at least one block */
			sgmax = AES_BLOCK_SIZE;
			maxlen = min(maxlen, AES_BLOCK_SIZE);
		}
	}

	if (unlikely((datalen < maxlen) && (datalen & (AES_BLOCK_SIZE - 1)))) {
		/* recalculate to get aligned size */
		maxlen = datalen & ~(AES_BLOCK_SIZE - 1);
		goto redo;
	}

	*sg = sgn;
	*sgoff = sgnoff;
	*sgcnt = cnt;
	*reqlen = datalen;
}

static int rtcr_process_skcipher(struct skcipher_request *sreq, int opmode)
{
	char *dataout, *iv, ivbk[AES_BLOCK_SIZE], datain[AES_BLOCK_SIZE];
	int padlen, sgnoff, sgcnt, reqlen, ret, fblen, sgmap, sgdir;
	struct crypto_skcipher *tfm = crypto_skcipher_reqtfm(sreq);
	struct rtcr_skcipher_ctx *sctx = crypto_skcipher_ctx(tfm);
	int totallen = sreq->cryptlen, sgoff = 0, dgoff = 0;
	struct rtcr_crypto_dev *cdev = sctx->cdev;
	struct scatterlist *sg = sreq->src, *sgn;
	int idx, srcidx, dstidx, len, datalen;
	dma_addr_t ivdma, outdma, indma;

	if (!totallen)
		return 0;

	if ((totallen & (AES_BLOCK_SIZE - 1)) && (!(opmode & RTCR_SRC_OP_CRYPT_CTR)))
		return -EINVAL;

redo:
	indma = outdma = 0;
	sgmap = 0;
	sgnoff = sgoff;
	sgn = sg;
	datalen = totallen;

	/* limit input so that engine can process it */
	rtcr_cut_skcipher_len(&datalen, opmode, sreq->iv);
	rtcr_max_skcipher_len(&datalen, &sgn, &sgnoff, &sgcnt);

	/* CTR padding */
	padlen = (AES_BLOCK_SIZE - datalen) & (AES_BLOCK_SIZE - 1);
	reqlen = datalen + padlen;

	fblen = 0;
	if (sgcnt > RTCR_MAX_SG_SKCIPHER) {
		/* single AES block with too many SGs */
		fblen = datalen;
		sg_pcopy_to_buffer(sg, sgcnt, datain, datalen, sgoff);
	}

	if ((opmode & RTCR_SRC_OP_CRYPT_CBC) &&
	    (!(opmode & RTCR_SRC_OP_KAM_ENC))) {
		/* CBC decryption IV might get overwritten */
		sg_pcopy_to_buffer(sg, sgcnt, ivbk, AES_BLOCK_SIZE,
				   sgoff + datalen - AES_BLOCK_SIZE);
	}

	/* Get free space in the ring */
	if (padlen || (datalen + dgoff > sg_dma_len(sreq->dst))) {
		len = datalen;
	} else {
		len = RTCR_WB_LEN_SG_DIRECT;
		dataout = sg_virt(sreq->dst) + dgoff;
	}

	ret = rtcr_alloc_ring(cdev, 2 + (fblen ? 1 : sgcnt) + (padlen ? 1 : 0),
			      &srcidx, &dstidx, len, &dataout);
	if (ret)
		return ret;

	/* Write back any uncommitted data to memory */
	if (dataout == sg_virt(sreq->src) + sgoff) {
		sgdir = DMA_BIDIRECTIONAL;
		sgmap = dma_map_sg(cdev->dev, sg, sgcnt, sgdir);
	} else {
		outdma = dma_map_single(cdev->dev, dataout, reqlen, DMA_BIDIRECTIONAL);
		if (fblen)
			indma = dma_map_single(cdev->dev, datain, reqlen, DMA_TO_DEVICE);
		else {
			sgdir = DMA_TO_DEVICE;
			sgmap = dma_map_sg(cdev->dev, sg, sgcnt, sgdir);
		}
	}

	if (sreq->iv)
		ivdma = dma_map_single(cdev->dev, sreq->iv, AES_BLOCK_SIZE, DMA_TO_DEVICE);
	/*
	 * Feed input data into the rings. Start with destination ring and fill
	 * source ring afterwards. Ensure that the owner flag of the first source
	 * ring is the last that becomes visible to the engine.
	 */
	rtcr_add_dst_to_ring(cdev, dstidx, dataout, reqlen, sreq->dst, dgoff);

	idx = rtcr_inc_src_idx(srcidx, 1);
	rtcr_add_src_to_ring(cdev, idx, sreq->iv, AES_BLOCK_SIZE, reqlen);

	if (fblen) {
		idx = rtcr_inc_src_idx(idx, 1);
		rtcr_add_src_to_ring(cdev, idx, (void *)datain, fblen, reqlen);
	}

	datalen -= fblen;
	while (datalen) {
		len = min((int)sg_dma_len(sg) - sgoff, datalen);

		idx = rtcr_inc_src_idx(idx, 1);
		rtcr_add_src_to_ring(cdev, idx, sg_virt(sg) + sgoff, len, reqlen);

		datalen -= len;
		sg = sg_next(sg);
		sgoff = 0;
	}

	if (padlen) {
		idx = rtcr_inc_src_idx(idx, 1);
		rtcr_add_src_to_ring(cdev, idx, (void *)empty_zero_page, padlen, reqlen);
	}

	rtcr_add_src_pad_to_ring(cdev, idx, reqlen);
	rtcr_add_src_skcipher_to_ring(cdev, srcidx, opmode, reqlen, sctx);

	/* Off we go */
	rtcr_kick_engine(cdev);
	if (rtcr_wait_for_request(cdev, dstidx))
		return -EINVAL;

	if (sreq->iv)
		dma_unmap_single(cdev->dev, ivdma, AES_BLOCK_SIZE, DMA_TO_DEVICE);
	if (outdma)
		dma_unmap_single(cdev->dev, outdma, reqlen, DMA_BIDIRECTIONAL);
	if (indma)
		dma_unmap_single(cdev->dev, indma, reqlen, DMA_TO_DEVICE);
	if (sgmap)
		dma_unmap_sg(cdev->dev, sg, sgcnt, sgdir);

	/* Handle IV feedback as engine does not provide it */
	if (opmode & RTCR_SRC_OP_CRYPT_CTR) {
		rtcr_inc_iv(sreq->iv, reqlen / AES_BLOCK_SIZE);
	} else if (opmode & RTCR_SRC_OP_CRYPT_CBC) {
		iv = opmode & RTCR_SRC_OP_KAM_ENC ?
		     dataout + reqlen - AES_BLOCK_SIZE : ivbk;
		memcpy(sreq->iv, iv, AES_BLOCK_SIZE);
	}

	sg = sgn;
	sgoff = sgnoff;
	dgoff += reqlen;
	totallen -= min(reqlen, totallen);

	if (totallen)
		goto redo;

	return 0;
}

static int rtcr_skcipher_encrypt(struct skcipher_request *sreq)
{
	struct crypto_skcipher *tfm = crypto_skcipher_reqtfm(sreq);
	struct rtcr_skcipher_ctx *sctx = crypto_skcipher_ctx(tfm);
	int opmode = sctx->opmode | RTCR_SRC_OP_KAM_ENC;

	return rtcr_process_skcipher(sreq, opmode);
}

static int rtcr_skcipher_decrypt(struct skcipher_request *sreq)
{
	struct crypto_skcipher *tfm = crypto_skcipher_reqtfm(sreq);
	struct rtcr_skcipher_ctx *sctx = crypto_skcipher_ctx(tfm);
	int opmode = sctx->opmode;

	opmode |= sctx->opmode & RTCR_SRC_OP_CRYPT_CTR ?
		  RTCR_SRC_OP_KAM_ENC : RTCR_SRC_OP_KAM_DEC;

	return rtcr_process_skcipher(sreq, opmode);
}

static int rtcr_skcipher_setkey(struct crypto_skcipher *cipher,
				const u8 *key, unsigned int keylen)
{
	struct crypto_tfm *tfm = crypto_skcipher_tfm(cipher);
	struct rtcr_skcipher_ctx *sctx = crypto_tfm_ctx(tfm);
	struct rtcr_crypto_dev *cdev = sctx->cdev;
	struct crypto_aes_ctx kctx;
	int p, i;

	if (aes_expandkey(&kctx, key, keylen))
		return -EINVAL;

	sctx->keylen = keylen;
	sctx->opmode = (sctx->opmode & ~RTCR_SRC_OP_CIPHER_MASK) |
			RTCR_SRC_OP_CIPHER_FROM_KEY(keylen);

	memcpy(sctx->keyenc, key, keylen);
	/* decryption key is derived from expanded key */
	p = ((keylen / 4) + 6) * 4;
	for (i = 0; i < 8; i++) {
		sctx->keydec[i] = cpu_to_le32(kctx.key_enc[p + i]);
		if (i == 3)
			p -= keylen == AES_KEYSIZE_256 ? 8 : 6;
	}

	dma_sync_single_for_device(cdev->dev, sctx->keydma, 2 * AES_KEYSIZE_256, DMA_TO_DEVICE);

	return 0;
}

static int rtcr_skcipher_cra_init(struct crypto_tfm *tfm)
{
	struct rtcr_skcipher_ctx *sctx = crypto_tfm_ctx(tfm);
	struct rtcr_alg_template *tmpl;

	tmpl = container_of(tfm->__crt_alg, struct rtcr_alg_template,
			    alg.skcipher.base);

	sctx->cdev = tmpl->cdev;
	sctx->opmode = tmpl->opmode;
	sctx->keydma = dma_map_single(sctx->cdev->dev, sctx->keyenc,
				      2 * AES_KEYSIZE_256, DMA_TO_DEVICE);

	return 0;
}

static void rtcr_skcipher_cra_exit(struct crypto_tfm *tfm)
{
	struct rtcr_skcipher_ctx *sctx = crypto_tfm_ctx(tfm);
	struct rtcr_crypto_dev *cdev = sctx->cdev;

	dma_unmap_single(cdev->dev, sctx->keydma, 2 * AES_KEYSIZE_256, DMA_TO_DEVICE);
	memzero_explicit(sctx, tfm->__crt_alg->cra_ctxsize);
}

struct rtcr_alg_template rtcr_skcipher_ecb_aes = {
	.type = RTCR_ALG_SKCIPHER,
	.opmode = RTCR_SRC_OP_MS_CRYPTO | RTCR_SRC_OP_CRYPT_ECB,
	.alg.skcipher = {
		.setkey = rtcr_skcipher_setkey,
		.encrypt = rtcr_skcipher_encrypt,
		.decrypt = rtcr_skcipher_decrypt,
		.min_keysize = AES_MIN_KEY_SIZE,
		.max_keysize = AES_MAX_KEY_SIZE,
		.base = {
			.cra_name = "ecb(aes)",
			.cra_driver_name = "realtek-ecb-aes",
			.cra_priority = 300,
			.cra_flags = CRYPTO_ALG_ASYNC,
			.cra_blocksize = AES_BLOCK_SIZE,
			.cra_ctxsize = sizeof(struct rtcr_skcipher_ctx),
			.cra_alignmask = 0,
			.cra_init = rtcr_skcipher_cra_init,
			.cra_exit = rtcr_skcipher_cra_exit,
			.cra_module = THIS_MODULE,
		},
	},
};

struct rtcr_alg_template rtcr_skcipher_cbc_aes = {
	.type = RTCR_ALG_SKCIPHER,
	.opmode = RTCR_SRC_OP_MS_CRYPTO | RTCR_SRC_OP_CRYPT_CBC,
	.alg.skcipher = {
		.setkey = rtcr_skcipher_setkey,
		.encrypt = rtcr_skcipher_encrypt,
		.decrypt = rtcr_skcipher_decrypt,
		.min_keysize = AES_MIN_KEY_SIZE,
		.max_keysize = AES_MAX_KEY_SIZE,
		.ivsize	= AES_BLOCK_SIZE,
		.base = {
			.cra_name = "cbc(aes)",
			.cra_driver_name = "realtek-cbc-aes",
			.cra_priority = 300,
			.cra_flags = CRYPTO_ALG_ASYNC,
			.cra_blocksize = AES_BLOCK_SIZE,
			.cra_ctxsize = sizeof(struct rtcr_skcipher_ctx),
			.cra_alignmask = 0,
			.cra_init = rtcr_skcipher_cra_init,
			.cra_exit = rtcr_skcipher_cra_exit,
			.cra_module = THIS_MODULE,
		},
	},
};

struct rtcr_alg_template rtcr_skcipher_ctr_aes = {
	.type = RTCR_ALG_SKCIPHER,
	.opmode = RTCR_SRC_OP_MS_CRYPTO | RTCR_SRC_OP_CRYPT_CTR,
	.alg.skcipher = {
		.setkey = rtcr_skcipher_setkey,
		.encrypt = rtcr_skcipher_encrypt,
		.decrypt = rtcr_skcipher_decrypt,
		.min_keysize = AES_MIN_KEY_SIZE,
		.max_keysize = AES_MAX_KEY_SIZE,
		.ivsize	= AES_BLOCK_SIZE,
		.base = {
			.cra_name = "ctr(aes)",
			.cra_driver_name = "realtek-ctr-aes",
			.cra_priority = 300,
			.cra_flags = CRYPTO_ALG_ASYNC,
			.cra_blocksize = 1,
			.cra_ctxsize = sizeof(struct rtcr_skcipher_ctx),
			.cra_alignmask = 0,
			.cra_init = rtcr_skcipher_cra_init,
			.cra_exit = rtcr_skcipher_cra_exit,
			.cra_module = THIS_MODULE,
		},
	},
};
