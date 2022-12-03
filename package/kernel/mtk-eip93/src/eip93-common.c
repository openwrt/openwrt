// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 - 2021
 *
 * Richard van Schagen <vschagen@icloud.com>
 */


#include <crypto/aes.h>
#include <crypto/ctr.h>
#include <crypto/hmac.h>
#include <crypto/sha.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/scatterlist.h>

#include "eip93-cipher.h"
#include "eip93-common.h"
#include "eip93-main.h"
#include "eip93-regs.h"

inline void *mtk_ring_next_wptr(struct mtk_device *mtk,
						struct mtk_desc_ring *ring)
{
	void *ptr = ring->write;

	if ((ring->write == ring->read - ring->offset) ||
		(ring->read == ring->base && ring->write == ring->base_end))
		return ERR_PTR(-ENOMEM);

	if (ring->write == ring->base_end)
		ring->write = ring->base;
	else
		ring->write += ring->offset;

	return ptr;
}

inline void *mtk_ring_next_rptr(struct mtk_device *mtk,
						struct mtk_desc_ring *ring)
{
	void *ptr = ring->read;

	if (ring->write == ring->read)
		return ERR_PTR(-ENOENT);

	if (ring->read == ring->base_end)
		ring->read = ring->base;
	else
		ring->read += ring->offset;

	return ptr;
}

inline int mtk_put_descriptor(struct mtk_device *mtk,
					struct eip93_descriptor_s *desc)
{
	struct eip93_descriptor_s *cdesc;
	struct eip93_descriptor_s *rdesc;
	unsigned long irqflags;

	spin_lock_irqsave(&mtk->ring->write_lock, irqflags);

	rdesc = mtk_ring_next_wptr(mtk, &mtk->ring->rdr);

	if (IS_ERR(rdesc)) {
		spin_unlock_irqrestore(&mtk->ring->write_lock, irqflags);
		return -ENOENT;
	}

	cdesc = mtk_ring_next_wptr(mtk, &mtk->ring->cdr);

	if (IS_ERR(cdesc)) {
		spin_unlock_irqrestore(&mtk->ring->write_lock, irqflags);
		return -ENOENT;
	}

	memset(rdesc, 0, sizeof(struct eip93_descriptor_s));

	memcpy(cdesc, desc, sizeof(struct eip93_descriptor_s));

	atomic_dec(&mtk->ring->free);
	spin_unlock_irqrestore(&mtk->ring->write_lock, irqflags);

	return 0;
}

inline void *mtk_get_descriptor(struct mtk_device *mtk)
{
	struct eip93_descriptor_s *cdesc;
	void *ptr;
	unsigned long irqflags;

	spin_lock_irqsave(&mtk->ring->read_lock, irqflags);

	cdesc = mtk_ring_next_rptr(mtk, &mtk->ring->cdr);

	if (IS_ERR(cdesc)) {
		spin_unlock_irqrestore(&mtk->ring->read_lock, irqflags);
		return ERR_PTR(-ENOENT);
	}

	memset(cdesc, 0, sizeof(struct eip93_descriptor_s));

	ptr = mtk_ring_next_rptr(mtk, &mtk->ring->rdr);
	if (IS_ERR(ptr)) {
		spin_unlock_irqrestore(&mtk->ring->read_lock, irqflags);
		return ERR_PTR(-ENOENT);
	}

	atomic_inc(&mtk->ring->free);
	spin_unlock_irqrestore(&mtk->ring->read_lock, irqflags);
	return ptr;
}

inline int mtk_get_free_saState(struct mtk_device *mtk)
{
	struct mtk_state_pool *saState_pool;
	int i;

	for (i = 0; i < MTK_RING_SIZE; i++) {
		saState_pool = &mtk->ring->saState_pool[i];
		if (saState_pool->in_use == false) {
			saState_pool->in_use = true;
			return i;
		}

	}

	return -ENOENT;
}

static inline void mtk_free_sg_copy(const int len, struct scatterlist **sg)
{
	if (!*sg || !len)
		return;

	free_pages((unsigned long)sg_virt(*sg), get_order(len));
	kfree(*sg);
	*sg = NULL;
}

static inline int mtk_make_sg_copy(struct scatterlist *src,
			struct scatterlist **dst,
			const uint32_t len, const bool copy)
{
	void *pages;

	*dst = kmalloc(sizeof(**dst), GFP_KERNEL);
	if (!*dst)
		return -ENOMEM;

	pages = (void *)__get_free_pages(GFP_KERNEL | GFP_DMA,
					get_order(len));

	if (!pages) {
		kfree(*dst);
		*dst = NULL;
		return -ENOMEM;
	}

	sg_init_table(*dst, 1);
	sg_set_buf(*dst, pages, len);

	/* copy only as requested */
	if (copy)
		sg_copy_to_buffer(src, sg_nents(src), pages, len);

	return 0;
}

static inline bool mtk_is_sg_aligned(struct scatterlist *sg, u32 len,
						const int blksize)
{
	int nents;

	for (nents = 0; sg; sg = sg_next(sg), ++nents) {
		if (!IS_ALIGNED(sg->offset, 4))
			return false;

		if (len <= sg->length) {
			if (!IS_ALIGNED(len, blksize))
				return false;

			return true;
		}

		if (!IS_ALIGNED(sg->length, blksize))
			return false;

		len -= sg->length;
	}
	return false;
}

int check_valid_request(struct mtk_cipher_reqctx *rctx)
{
	struct scatterlist *src = rctx->sg_src;
	struct scatterlist *dst = rctx->sg_dst;
	uint32_t src_nents, dst_nents;
	u32 textsize = rctx->textsize;
	u32 authsize = rctx->authsize;
	u32 blksize = rctx->blksize;
	u32 totlen_src = rctx->assoclen + rctx->textsize;
	u32 totlen_dst = rctx->assoclen + rctx->textsize;
	u32 copy_len;
	bool src_align, dst_align;
	int err = -EINVAL;

	if (!IS_CTR(rctx->flags)) {
		if (!IS_ALIGNED(textsize, blksize))
			return err;
	}

	if (authsize) {
		if (IS_ENCRYPT(rctx->flags))
			totlen_dst += authsize;
		else
			totlen_src += authsize;
	}

	src_nents = sg_nents_for_len(src, totlen_src);
	dst_nents = sg_nents_for_len(dst, totlen_dst);

	if (src == dst) {
		src_nents = max(src_nents, dst_nents);
		dst_nents = src_nents;
		if (unlikely((totlen_src || totlen_dst) && (src_nents <= 0)))
			return err;

	} else {
		if (unlikely(totlen_src && (src_nents <= 0)))
			return err;

		if (unlikely(totlen_dst && (dst_nents <= 0)))
			return err;
	}

	if (authsize) {
		if (dst_nents == 1 && src_nents == 1) {
			src_align = mtk_is_sg_aligned(src, totlen_src, blksize);
			if (src ==  dst)
				dst_align = src_align;
			else
				dst_align = mtk_is_sg_aligned(dst,
						totlen_dst, blksize);
		} else {
			src_align = false;
			dst_align = false;
		}
	} else {
		src_align = mtk_is_sg_aligned(src, totlen_src, blksize);
		if (src == dst)
			dst_align = src_align;
		else
			dst_align = mtk_is_sg_aligned(dst, totlen_dst, blksize);
	}

	copy_len = max(totlen_src, totlen_dst);
	if (!src_align) {
		err = mtk_make_sg_copy(src, &rctx->sg_src, copy_len, true);
		if (err)
			return err;
	}

	if (!dst_align) {
		err = mtk_make_sg_copy(dst, &rctx->sg_dst, copy_len, false);
		if (err)
			return err;
	}

	rctx->src_nents = sg_nents_for_len(rctx->sg_src, totlen_src);
	rctx->dst_nents = sg_nents_for_len(rctx->sg_dst, totlen_dst);

	return 0;
}
/*
 * Set saRecord function:
 * Even saRecord is set to "0", keep " = 0" for readability.
 */
void mtk_set_saRecord(struct saRecord_s *saRecord, const unsigned int keylen,
				const unsigned long flags)
{
	saRecord->saCmd0.bits.ivSource = 2;
	if (IS_ECB(flags))
		saRecord->saCmd0.bits.saveIv = 0;
	else
		saRecord->saCmd0.bits.saveIv = 1;

	saRecord->saCmd0.bits.opGroup = 0;
	saRecord->saCmd0.bits.opCode = 0;

	switch ((flags & MTK_ALG_MASK)) {
	case MTK_ALG_AES:
		saRecord->saCmd0.bits.cipher = 3;
		saRecord->saCmd1.bits.aesKeyLen = keylen >> 3;
		break;
	case MTK_ALG_3DES:
		saRecord->saCmd0.bits.cipher = 1;
		break;
	case MTK_ALG_DES:
		saRecord->saCmd0.bits.cipher = 0;
		break;
	default:
		saRecord->saCmd0.bits.cipher = 15;
	}

	switch ((flags & MTK_HASH_MASK)) {
	case MTK_HASH_SHA256:
		saRecord->saCmd0.bits.hash = 3;
		break;
	case MTK_HASH_SHA224:
		saRecord->saCmd0.bits.hash = 2;
		break;
	case MTK_HASH_SHA1:
		saRecord->saCmd0.bits.hash = 1;
		break;
	case MTK_HASH_MD5:
		saRecord->saCmd0.bits.hash = 0;
		break;
	default:
		saRecord->saCmd0.bits.hash = 15;
	}

	saRecord->saCmd0.bits.hdrProc = 0;
	saRecord->saCmd0.bits.padType = 3;
	saRecord->saCmd0.bits.extPad = 0;
	saRecord->saCmd0.bits.scPad = 0;

	switch ((flags & MTK_MODE_MASK)) {
	case MTK_MODE_CBC:
		saRecord->saCmd1.bits.cipherMode = 1;
		break;
	case MTK_MODE_CTR:
		saRecord->saCmd1.bits.cipherMode = 2;
		break;
	case MTK_MODE_ECB:
		saRecord->saCmd1.bits.cipherMode = 0;
		break;
	}

	saRecord->saCmd1.bits.byteOffset = 0;
	saRecord->saCmd1.bits.hashCryptOffset = 0;
	saRecord->saCmd0.bits.digestLength = 0;
	saRecord->saCmd1.bits.copyPayload = 0;

	if (IS_HMAC(flags)) {
		saRecord->saCmd1.bits.hmac = 1;
		saRecord->saCmd1.bits.copyDigest = 1;
		saRecord->saCmd1.bits.copyHeader = 1;
	} else {
		saRecord->saCmd1.bits.hmac = 0;
		saRecord->saCmd1.bits.copyDigest = 0;
		saRecord->saCmd1.bits.copyHeader = 0;
	}

	/* Default for now, might be used for ESP offload */
	saRecord->saCmd1.bits.seqNumCheck = 0;
	saRecord->saSpi = 0x0;
	saRecord->saSeqNumMask[0] = 0xFFFFFFFF;
	saRecord->saSeqNumMask[1] = 0x0;
}
EXPORT_SYMBOL_GPL(mtk_set_saRecord);
/*
 * Poor mans Scatter/gather function:
 * Create a Descriptor for every segment to avoid copying buffers.
 * For performance better to wait for hardware to perform multiple DMA
 *
 */
static inline int mtk_scatter_combine(struct mtk_device *mtk,
			struct mtk_cipher_reqctx *rctx,
			u32 datalen, u32 split, int offsetin)
{
	struct eip93_descriptor_s *cdesc = rctx->cdesc;
	struct scatterlist *sgsrc = rctx->sg_src;
	struct scatterlist *sgdst = rctx->sg_dst;
	unsigned int remainin = sg_dma_len(sgsrc);
	unsigned int remainout = sg_dma_len(sgdst);
	dma_addr_t saddr = sg_dma_address(sgsrc);
	dma_addr_t daddr = sg_dma_address(sgdst);
	dma_addr_t stateAddr;
	u32 srcAddr, dstAddr, len, n;
	bool nextin = false;
	bool nextout = false;
	int offsetout = 0;
	int ndesc_cdr = 0, err;

	if (IS_ECB(rctx->flags))
		rctx->saState_base = 0;

	if (split < datalen) {
		stateAddr = rctx->saState_base_ctr;
		n = split;
	} else {
		stateAddr = rctx->saState_base;
		n = datalen;
	}

	do {
		if (nextin) {
			sgsrc = sg_next(sgsrc);
			remainin = sg_dma_len(sgsrc);
			if (remainin == 0)
				continue;

			saddr = sg_dma_address(sgsrc);
			offsetin = 0;
			nextin = false;
		}

		if (nextout) {
			sgdst = sg_next(sgdst);
			remainout = sg_dma_len(sgdst);
			if (remainout == 0)
				continue;

			daddr = sg_dma_address(sgdst);
			offsetout = 0;
			nextout = false;
		}
		srcAddr = saddr + offsetin;
		dstAddr = daddr + offsetout;

		if (remainin == remainout) {
			len = remainin;
			if (len > n) {
				len = n;
				remainin -= n;
				remainout -= n;
				offsetin += n;
				offsetout += n;
			} else {
				nextin = true;
				nextout = true;
			}
		} else if (remainin < remainout) {
			len = remainin;
			if (len > n) {
				len = n;
				remainin -= n;
				remainout -= n;
				offsetin += n;
				offsetout += n;
			} else {
				offsetout += len;
				remainout -= len;
				nextin = true;
			}
		} else {
			len = remainout;
			if (len > n) {
				len = n;
				remainin -= n;
				remainout -= n;
				offsetin += n;
				offsetout += n;
			} else {
				offsetin += len;
				remainin -= len;
				nextout = true;
			}
		}
		n -= len;

		cdesc->srcAddr = srcAddr;
		cdesc->dstAddr = dstAddr;
		cdesc->stateAddr = stateAddr;
		cdesc->peLength.bits.peReady = 0;
		cdesc->peLength.bits.byPass = 0;
		cdesc->peLength.bits.length = len;
		cdesc->peLength.bits.hostReady = 1;

		if (n == 0) {
			n = datalen - split;
			split = datalen;
			stateAddr = rctx->saState_base;
		}

		if (n == 0)
			cdesc->userId |= MTK_DESC_LAST | MTK_DESC_FINISH;

		/* Loop - Delay - No need to rollback
		 * Maybe refine by slowing down at MTK_RING_BUSY
		 */
again:
		err = mtk_put_descriptor(mtk, cdesc);
		if (err) {
			udelay(500);
			goto again;
		}
		/* Writing new descriptor count starts DMA action */
		writel(1, mtk->base + EIP93_REG_PE_CD_COUNT);

		ndesc_cdr++;
	} while (n);

	return -EINPROGRESS;
}

int mtk_send_req(struct crypto_async_request *async,
			const u8 *reqiv, struct mtk_cipher_reqctx *rctx)
{
	struct mtk_crypto_ctx *ctx = crypto_tfm_ctx(async->tfm);
	struct mtk_device *mtk = ctx->mtk;
	struct scatterlist *src = rctx->sg_src;
	struct scatterlist *dst = rctx->sg_dst;
	struct saState_s *saState;
	struct mtk_state_pool *saState_pool;
	struct eip93_descriptor_s cdesc;
	unsigned long flags = rctx->flags;
	int idx;
	int offsetin = 0, err = -ENOMEM;
	u32 datalen = rctx->assoclen + rctx->textsize;
	u32 split = datalen;
	u32 start, end, ctr, blocks;
	u32 iv[AES_BLOCK_SIZE / sizeof(u32)];

	rctx->saState_ctr = NULL;
	rctx->saState = NULL;

	if (IS_ECB(flags))
		goto skip_iv;

	memcpy(iv, reqiv, rctx->ivsize);

	if (!IS_ALIGNED((u32)reqiv, rctx->ivsize) || IS_RFC3686(flags)) {
		rctx->flags &= ~MTK_DESC_DMA_IV;
		flags = rctx->flags;
	}

	if (IS_DMA_IV(flags)) {
		rctx->saState = (void *)reqiv;
	} else  {
		idx = mtk_get_free_saState(mtk);
		if (idx < 0)
			goto send_err;
		saState_pool = &mtk->ring->saState_pool[idx];
		rctx->saState_idx = idx;
		rctx->saState = saState_pool->base;
		rctx->saState_base = saState_pool->base_dma;
		memcpy(rctx->saState->stateIv, iv, rctx->ivsize);
	}

	saState = rctx->saState;

	if (IS_RFC3686(flags)) {
		saState->stateIv[0] = ctx->saNonce;
		saState->stateIv[1] = iv[0];
		saState->stateIv[2] = iv[1];
		saState->stateIv[3] = htonl(1);
	} else if (!IS_HMAC(flags) && IS_CTR(flags)) {
		/* Compute data length. */
		blocks = DIV_ROUND_UP(rctx->textsize, AES_BLOCK_SIZE);
		ctr = ntohl(iv[3]);
		/* Check 32bit counter overflow. */
		start = ctr;
		end = start + blocks - 1;
		if (end < start) {
			split = AES_BLOCK_SIZE * -start;
			/*
			 * Increment the counter manually to cope with
			 * the hardware counter overflow.
			 */
			iv[3] = 0xffffffff;
			crypto_inc((u8 *)iv, AES_BLOCK_SIZE);
			idx = mtk_get_free_saState(mtk);
			if (idx < 0)
				goto free_state;
			saState_pool = &mtk->ring->saState_pool[idx];
			rctx->saState_ctr_idx = idx;
			rctx->saState_ctr = saState_pool->base;
			rctx->saState_base_ctr = saState_pool->base_dma;

			memcpy(rctx->saState_ctr->stateIv, reqiv, rctx->ivsize);
			memcpy(saState->stateIv, iv, rctx->ivsize);
		}
	}

	if (IS_DMA_IV(flags)) {
		rctx->saState_base = dma_map_single(mtk->dev, (void *)reqiv,
						rctx->ivsize, DMA_TO_DEVICE);
		if (dma_mapping_error(mtk->dev, rctx->saState_base))
			goto free_state;
	}
skip_iv:
	cdesc.peCrtlStat.bits.hostReady = 1;
	cdesc.peCrtlStat.bits.prngMode = 0;
	cdesc.peCrtlStat.bits.hashFinal = 0;
	cdesc.peCrtlStat.bits.padCrtlStat = 0;
	cdesc.peCrtlStat.bits.peReady = 0;
	cdesc.saAddr = rctx->saRecord_base;
	cdesc.arc4Addr = (uintptr_t)async;
	cdesc.userId = (flags & (MTK_DESC_AEAD | MTK_DESC_SKCIPHER));
	rctx->cdesc = &cdesc;

	/* map DMA_BIDIRECTIONAL to invalidate cache on destination
	 * implies __dma_cache_wback_inv
	 */
	dma_map_sg(mtk->dev, dst, rctx->dst_nents, DMA_BIDIRECTIONAL);
	if (src != dst)
		dma_map_sg(mtk->dev, src, rctx->src_nents, DMA_TO_DEVICE);

	err = mtk_scatter_combine(mtk, rctx, datalen, split, offsetin);

	return err;

free_state:
	if (rctx->saState) {
		saState_pool = &mtk->ring->saState_pool[rctx->saState_idx];
		saState_pool->in_use = false;
	}

	if (rctx->saState_ctr) {
		saState_pool = &mtk->ring->saState_pool[rctx->saState_ctr_idx];
		saState_pool->in_use = false;
	}
send_err:
	return err;
}

void mtk_unmap_dma(struct mtk_device *mtk, struct mtk_cipher_reqctx *rctx,
			struct scatterlist *reqsrc, struct scatterlist *reqdst)
{
	u32 len = rctx->assoclen + rctx->textsize;
	u32 authsize = rctx->authsize;
	u32 flags = rctx->flags;
	u32 *otag;
	int i;

	if (rctx->sg_src == rctx->sg_dst) {
		dma_unmap_sg(mtk->dev, rctx->sg_dst, rctx->dst_nents,
							DMA_BIDIRECTIONAL);
		goto process_tag;
	}

	dma_unmap_sg(mtk->dev, rctx->sg_src, rctx->src_nents,
							DMA_TO_DEVICE);

	if (rctx->sg_src != reqsrc)
		mtk_free_sg_copy(len +  rctx->authsize, &rctx->sg_src);

	dma_unmap_sg(mtk->dev, rctx->sg_dst, rctx->dst_nents,
							DMA_BIDIRECTIONAL);

	/* SHA tags need conversion from net-to-host */
process_tag:
	if (IS_DECRYPT(flags))
		authsize = 0;

	if (authsize) {
		if (!IS_HASH_MD5(flags)) {
			otag = sg_virt(rctx->sg_dst) + len;
			for (i = 0; i < (authsize / 4); i++)
				otag[i] = ntohl(otag[i]);
		}
	}

	if (rctx->sg_dst != reqdst) {
		sg_copy_from_buffer(reqdst, sg_nents(reqdst),
				sg_virt(rctx->sg_dst), len + authsize);
		mtk_free_sg_copy(len + rctx->authsize, &rctx->sg_dst);
	}
}

void mtk_handle_result(struct mtk_device *mtk, struct mtk_cipher_reqctx *rctx,
			u8 *reqiv)
{
	struct mtk_state_pool *saState_pool;

	if (IS_DMA_IV(rctx->flags))
		dma_unmap_single(mtk->dev, rctx->saState_base, rctx->ivsize,
						DMA_TO_DEVICE);

	if (!IS_ECB(rctx->flags))
		memcpy(reqiv, rctx->saState->stateIv, rctx->ivsize);

	if ((rctx->saState) && !(IS_DMA_IV(rctx->flags))) {
		saState_pool = &mtk->ring->saState_pool[rctx->saState_idx];
		saState_pool->in_use = false;
	}

	if (rctx->saState_ctr) {
		saState_pool = &mtk->ring->saState_pool[rctx->saState_ctr_idx];
		saState_pool->in_use = false;
	}
}

#if IS_ENABLED(CONFIG_CRYPTO_DEV_EIP93_HMAC)
/* basically this is set hmac - key */
int mtk_authenc_setkey(struct crypto_shash *cshash, struct saRecord_s *sa,
			const u8 *authkey, unsigned int authkeylen)
{
	int bs = crypto_shash_blocksize(cshash);
	int ds = crypto_shash_digestsize(cshash);
	int ss = crypto_shash_statesize(cshash);
	u8 *ipad, *opad;
	unsigned int i, err;

	SHASH_DESC_ON_STACK(shash, cshash);

	shash->tfm = cshash;

	/* auth key
	 *
	 * EIP93 can only authenticate with hash of the key
	 * do software shash until EIP93 hash function complete.
	 */
	ipad = kcalloc(2, SHA256_BLOCK_SIZE + ss, GFP_KERNEL);
	if (!ipad)
		return -ENOMEM;

	opad = ipad + SHA256_BLOCK_SIZE + ss;

	if (authkeylen > bs) {
		err = crypto_shash_digest(shash, authkey,
					authkeylen, ipad);
		if (err)
			return err;

		authkeylen = ds;
	} else
		memcpy(ipad, authkey, authkeylen);

	memset(ipad + authkeylen, 0, bs - authkeylen);
	memcpy(opad, ipad, bs);

	for (i = 0; i < bs; i++) {
		ipad[i] ^= HMAC_IPAD_VALUE;
		opad[i] ^= HMAC_OPAD_VALUE;
	}

	err = crypto_shash_init(shash) ?:
				 crypto_shash_update(shash, ipad, bs) ?:
				 crypto_shash_export(shash, ipad) ?:
				 crypto_shash_init(shash) ?:
				 crypto_shash_update(shash, opad, bs) ?:
				 crypto_shash_export(shash, opad);

	if (err)
		return err;

	/* add auth key */
	memcpy(&sa->saIDigest, ipad, SHA256_DIGEST_SIZE);
	memcpy(&sa->saODigest, opad, SHA256_DIGEST_SIZE);

	kfree(ipad);
	return 0;
}
EXPORT_SYMBOL_GPL(mtk_authenc_setkey);
#endif
