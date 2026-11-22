#include "common.h"
#include "hash.h"

/**
 * sf_ce_sg_nents_for_len - return total count of entries in scatterlist
 *                    needed to satisfy the supplied length
 * @sg:		The scatterlist
 * @len:	The total required length
 * @extra:	The extra bytes at the last entry
 *
 * Description:
 * Determines the number of entries in sg that are required to meet
 * the supplied length, taking into acount chaining as well
 *
 * Returns:
 *   the number of sg entries needed, negative error on failure
 *
 **/
static int sf_ce_sg_nents_for_len(struct scatterlist *sg, u64 len, unsigned int *extra)
{
	int nents;
	u64 total;

	if (!len)
		return 0;

	for (nents = 0, total = 0; sg; sg = sg_next(sg)) {
		nents++;
		total += sg->length;
		if (total >= len) {
			*extra = total - len;
			return nents;
		}
	}

	return -EINVAL;
}

/* Initialize request context */
static int
sf_ce_ahash_cra_init(struct crypto_tfm *tfm)
{
	struct sf_ce_ahash_ctx *ctx = crypto_tfm_ctx(tfm);

	crypto_ahash_set_reqsize(__crypto_ahash_cast(tfm),
				 sizeof(struct sf_ce_ahash_reqctx));
	ctx->priv = g_dev;

	return 0;
}

static int
sf_ce_sha256_init(struct ahash_request *req)
{
	struct crypto_ahash *tfm = crypto_ahash_reqtfm(req);
	struct sf_ce_ahash_ctx *ctx = crypto_ahash_ctx(tfm);
	struct device *dev = ctx->priv->dev;
	struct sf_ce_ahash_reqctx *reqctx = ahash_request_ctx(req);
	__be32 *hashw = (__be32 *)reqctx->hash;
	dma_addr_t hash_phys, block_phys;
	int ret;

	memset(reqctx, 0, sizeof(*reqctx));
	hashw[0] = cpu_to_be32(SHA256_H0);
	hashw[1] = cpu_to_be32(SHA256_H1);
	hashw[2] = cpu_to_be32(SHA256_H2);
	hashw[3] = cpu_to_be32(SHA256_H3);
	hashw[4] = cpu_to_be32(SHA256_H4);
	hashw[5] = cpu_to_be32(SHA256_H5);
	hashw[6] = cpu_to_be32(SHA256_H6);
	hashw[7] = cpu_to_be32(SHA256_H7);

	/* bi-dir mapping for hash initial because it's used for both TX and RX */
	hash_phys = dma_map_single(dev, reqctx->hash, sizeof(reqctx->hash), DMA_BIDIRECTIONAL);
	if (unlikely((ret = dma_mapping_error(dev, hash_phys))))
		return ret;

	reqctx->hash_phys = hash_phys;

	/* map partial block */
	block_phys = dma_map_single(dev, reqctx->block, sizeof(reqctx->block), DMA_TO_DEVICE);
	if (unlikely((ret = dma_mapping_error(dev, block_phys)))) {
		dma_unmap_single(dev, reqctx->hash_phys, sizeof(reqctx->hash),
				 DMA_BIDIRECTIONAL);
		return ret;
	}

	reqctx->block_phys = block_phys;
	return 0;
}

static int
sf_ce_sha224_init(struct ahash_request *req)
{
	struct crypto_ahash *tfm = crypto_ahash_reqtfm(req);
	struct sf_ce_ahash_ctx *ctx = crypto_ahash_ctx(tfm);
	struct device *dev = ctx->priv->dev;
	struct sf_ce_ahash_reqctx *reqctx = ahash_request_ctx(req);
	__be32 *hashw = (__be32 *)reqctx->hash;
	dma_addr_t hash_phys, block_phys;
	int ret;

	memset(reqctx, 0, sizeof(*reqctx));
	hashw[0] = cpu_to_be32(SHA224_H0);
	hashw[1] = cpu_to_be32(SHA224_H1);
	hashw[2] = cpu_to_be32(SHA224_H2);
	hashw[3] = cpu_to_be32(SHA224_H3);
	hashw[4] = cpu_to_be32(SHA224_H4);
	hashw[5] = cpu_to_be32(SHA224_H5);
	hashw[6] = cpu_to_be32(SHA224_H6);
	hashw[7] = cpu_to_be32(SHA224_H7);

	/* bi-dir mapping for hash initial because it's used for both TX and RX */
	hash_phys = dma_map_single(dev, reqctx->hash, sizeof(reqctx->hash), DMA_BIDIRECTIONAL);
	if (unlikely((ret = dma_mapping_error(dev, hash_phys))))
		return ret;

	reqctx->hash_phys = hash_phys;

	/* map partial block */
	block_phys = dma_map_single(dev, reqctx->block, sizeof(reqctx->block), DMA_TO_DEVICE);
	if (unlikely((ret = dma_mapping_error(dev, block_phys)))) {
		dma_unmap_single(dev, reqctx->hash_phys, sizeof(reqctx->hash),
				 DMA_BIDIRECTIONAL);
		return ret;
	}

	reqctx->block_phys = block_phys;
	return 0;
}

static int
sf_ce_sha256_update(struct ahash_request *req)
{
	struct crypto_ahash *tfm = crypto_ahash_reqtfm(req);
	struct sf_ce_ahash_ctx *ctx = crypto_ahash_ctx(tfm);
	struct sf_ce_ahash_reqctx *reqctx = ahash_request_ctx(req);
	struct sf_ce_dev *priv = ctx->priv;
	struct sf_ce_chan *ch = &priv->chan[DMA_CH_SHA];
	struct sf_ce_desc *tx, *rx;
	struct scatterlist *sg;
	unsigned long partial_offset, cur_tx, cur_rx;
	dma_addr_t block_phys = 0, hash_phys = 0;
	unsigned int pktlen, buf1len, new_partial_offset, i, extra_bytes;
	int mapped, nents, ret;

	/* If we don't have at least one block, copy the content of the list to
	 * our temp buffer and exit.
	 */
	partial_offset = reqctx->length & (SHA256_BLOCK_SIZE - 1);
	if (partial_offset + req->nbytes < SHA256_BLOCK_SIZE) {
		reqctx->length += req->nbytes;
		scatterwalk_map_and_copy(&reqctx->block[partial_offset], req->src,
					 0, req->nbytes, 0);
		return 0;
	}
	/* pktlen: must be multiple of block size */
	pktlen = ALIGN_DOWN(partial_offset + req->nbytes, SHA256_BLOCK_SIZE);
	/* new_partial_offset: new partial data offset after update */
	new_partial_offset = (reqctx->length + req->nbytes) & (SHA256_BLOCK_SIZE - 1);

	nents = sf_ce_sg_nents_for_len(req->src, req->nbytes - new_partial_offset, &extra_bytes);
	if (unlikely(nents <= 0))
		return nents;

	mapped = dma_map_sg(priv->dev, req->src, nents, DMA_TO_DEVICE);
	if (unlikely(mapped != nents))
		return -ENOMEM;

	reqctx->nents = nents;
	block_phys = reqctx->block_phys;
	if (partial_offset) {
		reqctx->block_len = partial_offset;
		dma_sync_single_for_device(priv->dev, block_phys, partial_offset, DMA_TO_DEVICE);
	}

	hash_phys = reqctx->hash_phys;
	spin_lock_bh(&ch->ring_lock);
	tx = ch->dma_tx;
	cur_tx = ch->cur_tx;

	/* first descriptor: hash initial, and partial block (size can be zero) */
	tx[cur_tx].des0 = cpu_to_le32(hash_phys);
	tx[cur_tx].des1 = cpu_to_le32(block_phys);
	tx[cur_tx].des2 = cpu_to_le32(FIELD_PREP(CE_TDES2_ED, CE_ENDIAN_BIG) | FIELD_PREP(CE_TDES2_B1L, SHA256_DIGEST_SIZE)|
				      FIELD_PREP(CE_TDES2_B2L, partial_offset) | FIELD_PREP(CE_TDES2_B2T, CE_BUF_PAYLOAD));
	tx[cur_tx].des3 = cpu_to_le32(CE_TDES3_OWN | CE_TDES3_FD | FIELD_PREP(CE_TDES3_B1T, CE_BUF_HASH_INIT) |
				      FIELD_PREP(CE_TDES3_CT, CE_SHA256) | FIELD_PREP(CE_TDES3_PL, pktlen));
	pr_debug("tdes0: %08X\n", tx[cur_tx].des0);
	pr_debug("tdes1: %08X\n", tx[cur_tx].des1);
	pr_debug("tdes2: %08X\n", tx[cur_tx].des2);
	pr_debug("tdes3: %08X\n", tx[cur_tx].des3);
	cur_tx = (cur_tx + 1) % DMA_RING_SIZE;
	for_each_sg(req->src, sg, nents, i) {
		int ld = (i + 1 == nents);

		if (i % 2 == 0) { /* buffer at des0 */
			tx[cur_tx].des0 = cpu_to_le32(sg_dma_address(sg));
			/* the last buffer may contain more bytes than we want,
			 * make sure to limit it */
			buf1len = ld ? sg_dma_len(sg) - extra_bytes : sg_dma_len(sg);
			pr_debug("tdes0: %08X\n", tx[cur_tx].des0);
		} else { /* buffer at des1 */
			tx[cur_tx].des1 = cpu_to_le32(sg_dma_address(sg));
			tx[cur_tx].des2 = cpu_to_le32(FIELD_PREP(CE_TDES2_ED, CE_ENDIAN_BIG) |
						      FIELD_PREP(CE_TDES2_B1L, buf1len) |
					/* the last buffer may contain more bytes than we want,
					 * make sure to limit it */
						      FIELD_PREP(CE_TDES2_B2L, ld ? sg_dma_len(sg) - extra_bytes : sg_dma_len(sg)) |
						      FIELD_PREP(CE_TDES2_B2T, CE_BUF_PAYLOAD));
			tx[cur_tx].des3 = cpu_to_le32(CE_TDES3_OWN | FIELD_PREP(CE_TDES3_LD, ld) |
						      FIELD_PREP(CE_TDES3_B1T, CE_BUF_PAYLOAD) |
						      FIELD_PREP(CE_TDES3_CT, CE_SHA256) |
						      FIELD_PREP(CE_TDES3_PL, pktlen));
			pr_debug("tdes1: %08X\n", tx[cur_tx].des1);
			pr_debug("tdes2: %08X\n", tx[cur_tx].des2);
			pr_debug("tdes3: %08X\n", tx[cur_tx].des3);
			cur_tx = (cur_tx + 1) % DMA_RING_SIZE;
		}
	}
	/* fix up the last descriptor if nents is odd */
	if (i % 2 != 0) {
		tx[cur_tx].des1 = cpu_to_le32(0);
		tx[cur_tx].des2 = cpu_to_le32(FIELD_PREP(CE_TDES2_ED, CE_ENDIAN_BIG) |
					      FIELD_PREP(CE_TDES2_B1L, buf1len));
		tx[cur_tx].des3 = cpu_to_le32(CE_TDES3_OWN | CE_TDES3_LD |
					      FIELD_PREP(CE_TDES3_B1T, CE_BUF_PAYLOAD) |
					      FIELD_PREP(CE_TDES3_CT, CE_SHA256) |
					      FIELD_PREP(CE_TDES3_PL, pktlen));
		pr_debug("tdes1: %08X\n", tx[cur_tx].des1);
		pr_debug("tdes2: %08X\n", tx[cur_tx].des2);
		pr_debug("tdes3: %08X\n", tx[cur_tx].des3);
		cur_tx = (cur_tx + 1) % DMA_RING_SIZE;
	}
	ch->cur_tx = cur_tx;

	/* One rx descriptor */
	cur_rx = ch->cur_rx;
	rx = ch->dma_rx;
	rx[cur_rx].des0 = cpu_to_le32(hash_phys);
	rx[cur_rx].des1 = cpu_to_le32(0);
	rx[cur_rx].des2 = cpu_to_le32(FIELD_PREP(CE_RDES2_B1L, SHA256_DIGEST_SIZE));
	rx[cur_rx].des3 = cpu_to_le32(CE_RDES3_OWN | CE_RDES3_IOC);
	pr_debug("rdes0: %08X\n", rx[cur_rx].des0);
	pr_debug("rdes1: %08X\n", rx[cur_rx].des1);
	pr_debug("rdes2: %08X\n", rx[cur_rx].des2);
	pr_debug("rdes3: %08X\n", rx[cur_rx].des3);
	/* link this request to the rx descriptor */
	ch->areqs[cur_rx] = &req->base;
	cur_rx = (cur_rx + 1) % DMA_RING_SIZE;
	ch->cur_rx = cur_rx;
	spin_unlock_bh(&ch->ring_lock);

	/* inform the DMA for the new data */
	reg_write_flush(priv, CE_DMA_CH_RxDESC_TAIL_LPTR(ch->ch_num), ch->dma_rx_phy + sizeof(struct sf_ce_desc) * cur_rx);
	reg_write(priv, CE_DMA_CH_TxDESC_TAIL_LPTR(ch->ch_num), ch->dma_tx_phy + sizeof(struct sf_ce_desc) * cur_tx);

	return -EINPROGRESS;
}

static int
sf_ce_sha256_final(struct ahash_request *req)
{
	struct crypto_ahash *tfm = crypto_ahash_reqtfm(req);
	struct sf_ce_ahash_ctx *ctx = crypto_ahash_ctx(tfm);
	struct sf_ce_ahash_reqctx *reqctx = ahash_request_ctx(req);
	dma_addr_t block_phys = 0, hash_phys = 0;
	struct sf_ce_dev *priv = ctx->priv;
	struct sf_ce_chan *ch = &priv->chan[DMA_CH_SHA];
	struct sf_ce_desc *tx, *rx;
	unsigned long cur_tx, cur_rx;
	unsigned int pad_len, index;
	int ret;

	/* Pad out to 56 mod 64. */
	index = reqctx->length & 0x3f;
	pad_len = (index < 56) ? (56 - index) : ((64+56) - index);

	reqctx->block[index] = 0x80;
	memset(reqctx->block + index + 1, 0, pad_len - 1);

	/* Append length */
	put_unaligned_be64(reqctx->length * BITS_PER_BYTE, reqctx->block + index + pad_len);
	pad_len += sizeof(u64);

	block_phys = reqctx->block_phys;
	dma_sync_single_for_device(priv->dev, block_phys, index + pad_len, DMA_TO_DEVICE);

	hash_phys = reqctx->hash_phys;

	spin_lock_bh(&ch->ring_lock);
	/* One tx descriptor */
	tx = ch->dma_tx;
	cur_tx = ch->cur_tx;
	tx[cur_tx].des0 = cpu_to_le32(hash_phys);
	tx[cur_tx].des1 = cpu_to_le32(block_phys);
	tx[cur_tx].des2 = cpu_to_le32(FIELD_PREP(CE_TDES2_ED, CE_ENDIAN_BIG) | FIELD_PREP(CE_TDES2_B1L, SHA256_DIGEST_SIZE)|
				      FIELD_PREP(CE_TDES2_B2L, index + pad_len) | FIELD_PREP(CE_TDES2_B2T, CE_BUF_PAYLOAD));
	tx[cur_tx].des3 = cpu_to_le32(CE_TDES3_OWN | CE_TDES3_FD | CE_TDES3_LD |
				      FIELD_PREP(CE_TDES3_B1T, CE_BUF_HASH_INIT) |
				      FIELD_PREP(CE_TDES3_CT, CE_SHA256) | FIELD_PREP(CE_TDES3_PL, index + pad_len));
	pr_debug("tdes0: %08X\n", tx[cur_tx].des0);
	pr_debug("tdes1: %08X\n", tx[cur_tx].des1);
	pr_debug("tdes2: %08X\n", tx[cur_tx].des2);
	pr_debug("tdes3: %08X\n", tx[cur_tx].des3);
	cur_tx = (cur_tx + 1) % DMA_RING_SIZE;
	ch->cur_tx = cur_tx;
	/* One rx descriptor */
	cur_rx = ch->cur_rx;
	rx = ch->dma_rx;
	rx[cur_rx].des0 = cpu_to_le32(hash_phys);
	rx[cur_rx].des1 = cpu_to_le32(0);
	rx[cur_rx].des2 = cpu_to_le32(FIELD_PREP(CE_RDES2_B1L, SHA256_DIGEST_SIZE));
	rx[cur_rx].des3 = cpu_to_le32(CE_RDES3_OWN | CE_RDES3_IOC);
	pr_debug("rdes0: %08X\n", rx[cur_rx].des0);
	pr_debug("rdes1: %08X\n", rx[cur_rx].des1);
	pr_debug("rdes2: %08X\n", rx[cur_rx].des2);
	pr_debug("rdes3: %08X\n", rx[cur_rx].des3);
	/* link this request to the rx descriptor */
	ch->areqs[cur_rx] = &req->base;
	cur_rx = (cur_rx + 1) % DMA_RING_SIZE;
	ch->cur_rx = cur_rx;

	reqctx->block_len = index + pad_len;
	reqctx->nents = 0;
	reqctx->final = true;
	spin_unlock_bh(&ch->ring_lock);
	/* inform the DMA for the new data */
	reg_write_flush(priv, CE_DMA_CH_RxDESC_TAIL_LPTR(ch->ch_num), ch->dma_rx_phy + sizeof(struct sf_ce_desc) * cur_rx);
	reg_write(priv, CE_DMA_CH_TxDESC_TAIL_LPTR(ch->ch_num), ch->dma_tx_phy + sizeof(struct sf_ce_desc) * cur_tx);
	return -EINPROGRESS;
}

static int
sf_ce_sha256_finup(struct ahash_request *req)
{
	struct crypto_ahash *tfm = crypto_ahash_reqtfm(req);
	struct sf_ce_ahash_ctx *ctx = crypto_ahash_ctx(tfm);
	struct sf_ce_ahash_reqctx *reqctx = ahash_request_ctx(req);
	struct sf_ce_dev *priv = ctx->priv;
	struct sf_ce_chan *ch = &priv->chan[DMA_CH_SHA];
	struct sf_ce_desc *tx, *rx;
	struct scatterlist *sg;
	unsigned long partial_offset, cur_tx, cur_rx;
	dma_addr_t block_phys = 0, hash_phys = 0;
	unsigned int pad_len, pktlen, index, buf1len, i;
	int mapped, nents, ret;

	nents = sg_nents_for_len(req->src, req->nbytes);
	if (unlikely(nents < 0))
		return nents;

	mapped = dma_map_sg(priv->dev, req->src, nents, DMA_TO_DEVICE);
	if (unlikely(mapped != nents))
		return -ENOMEM;

	reqctx->nents = nents;

	/* partial block len */
	partial_offset = reqctx->length & (SHA256_BLOCK_SIZE - 1);

	/* Calculate padding */

	/* Pad out to 56 mod 64. */
	index = (req->nbytes + partial_offset + reqctx->length) & 0x3f;
	pad_len = (index < 56) ? (56 - index) : ((64+56) - index);

	reqctx->block[partial_offset] = 0x80;
	memset(reqctx->block + partial_offset + 1, 0, pad_len - 1);

	/* Append length */
	put_unaligned_be64((req->nbytes + reqctx->length) * BITS_PER_BYTE,
			   reqctx->block + partial_offset + pad_len);

	pad_len += sizeof(u64);
	reqctx->block_len = partial_offset + pad_len;
	pktlen = reqctx->block_len + req->nbytes;

	block_phys = reqctx->block_phys;
	dma_sync_single_for_device(priv->dev, block_phys, reqctx->block_len, DMA_TO_DEVICE);

	hash_phys = reqctx->hash_phys;

	spin_lock_bh(&ch->ring_lock);
	tx = ch->dma_tx;
	cur_tx = ch->cur_tx;

	/* first descriptor: hash initial, and partial block (size can be zero) */
	tx[cur_tx].des0 = cpu_to_le32(hash_phys);
	tx[cur_tx].des1 = cpu_to_le32(block_phys);
	tx[cur_tx].des2 = cpu_to_le32(FIELD_PREP(CE_TDES2_ED, CE_ENDIAN_BIG) | FIELD_PREP(CE_TDES2_B1L, SHA256_DIGEST_SIZE)|
				      FIELD_PREP(CE_TDES2_B2L, partial_offset) | FIELD_PREP(CE_TDES2_B2T, CE_BUF_PAYLOAD));
	tx[cur_tx].des3 = cpu_to_le32(CE_TDES3_OWN | CE_TDES3_FD | FIELD_PREP(CE_TDES3_B1T, CE_BUF_HASH_INIT) |
				      FIELD_PREP(CE_TDES3_CT, CE_SHA256) | FIELD_PREP(CE_TDES3_PL, pktlen));
	pr_debug("tdes0: %08X\n", tx[cur_tx].des0);
	pr_debug("tdes1: %08X\n", tx[cur_tx].des1);
	pr_debug("tdes2: %08X\n", tx[cur_tx].des2);
	pr_debug("tdes3: %08X\n", tx[cur_tx].des3);
	cur_tx = (cur_tx + 1) % DMA_RING_SIZE;
	for_each_sg(req->src, sg, nents, i) {
		if (i % 2 == 0) { /* buffer at des0 */
			tx[cur_tx].des0 = cpu_to_le32(sg_dma_address(sg));
			pr_debug("tdes0: %08X\n", tx[cur_tx].des0);
			buf1len = sg_dma_len(sg);
		} else { /* buffer at des1 */
			tx[cur_tx].des1 = cpu_to_le32(sg_dma_address(sg));
			tx[cur_tx].des2 = cpu_to_le32(FIELD_PREP(CE_TDES2_ED, CE_ENDIAN_BIG) |
						      FIELD_PREP(CE_TDES2_B1L, buf1len) |
						      FIELD_PREP(CE_TDES2_B2L, sg_dma_len(sg)) |
						      FIELD_PREP(CE_TDES2_B2T, CE_BUF_PAYLOAD));
			tx[cur_tx].des3 = cpu_to_le32(CE_TDES3_OWN |
						      FIELD_PREP(CE_TDES3_B1T, CE_BUF_PAYLOAD) |
						      FIELD_PREP(CE_TDES3_CT, CE_SHA256) |
						      FIELD_PREP(CE_TDES3_PL, pktlen));
			pr_debug("tdes1: %08X\n", tx[cur_tx].des1);
			pr_debug("tdes2: %08X\n", tx[cur_tx].des2);
			pr_debug("tdes3: %08X\n", tx[cur_tx].des3);
			cur_tx = (cur_tx + 1) % DMA_RING_SIZE;
		}
	}

	/* fix up the last descriptor with padding */
	if (i % 2 != 0) {
		tx[cur_tx].des1 = cpu_to_le32(block_phys + partial_offset);
		tx[cur_tx].des2 = cpu_to_le32(FIELD_PREP(CE_TDES2_ED, CE_ENDIAN_BIG) |
					      FIELD_PREP(CE_TDES2_B1L, buf1len) |
					      FIELD_PREP(CE_TDES2_B2L, pad_len) |
					      FIELD_PREP(CE_TDES2_B2T, CE_BUF_PAYLOAD));
		tx[cur_tx].des3 = cpu_to_le32(CE_TDES3_OWN | CE_TDES3_LD |
					      FIELD_PREP(CE_TDES3_B1T, CE_BUF_PAYLOAD) |
					      FIELD_PREP(CE_TDES3_CT, CE_SHA256) |
					      FIELD_PREP(CE_TDES3_PL, pktlen));
		pr_debug("tdes1: %08X\n", tx[cur_tx].des1);
		pr_debug("tdes2: %08X\n", tx[cur_tx].des2);
		pr_debug("tdes3: %08X\n", tx[cur_tx].des3);
	} else {
		tx[cur_tx].des0 = cpu_to_le32(block_phys + partial_offset);
		tx[cur_tx].des1 = cpu_to_le32(0);
		tx[cur_tx].des2 = cpu_to_le32(FIELD_PREP(CE_TDES2_ED, CE_ENDIAN_BIG) |
					      FIELD_PREP(CE_TDES2_B1L, pad_len));
		tx[cur_tx].des3 = cpu_to_le32(CE_TDES3_OWN | CE_TDES3_LD |
					      FIELD_PREP(CE_TDES3_B1T, CE_BUF_PAYLOAD) |
					      FIELD_PREP(CE_TDES3_CT, CE_SHA256) |
					      FIELD_PREP(CE_TDES3_PL, pktlen));
		pr_debug("tdes0: %08X\n", tx[cur_tx].des0);
		pr_debug("tdes1: %08X\n", tx[cur_tx].des1);
		pr_debug("tdes2: %08X\n", tx[cur_tx].des2);
		pr_debug("tdes3: %08X\n", tx[cur_tx].des3);
	}
	cur_tx = (cur_tx + 1) % DMA_RING_SIZE;
	ch->cur_tx = cur_tx;

	/* One rx descriptor */
	cur_rx = ch->cur_rx;
	rx = ch->dma_rx;
	rx[cur_rx].des0 = cpu_to_le32(hash_phys);
	rx[cur_rx].des1 = cpu_to_le32(0);
	rx[cur_rx].des2 = cpu_to_le32(FIELD_PREP(CE_RDES2_B1L, SHA256_DIGEST_SIZE));
	rx[cur_rx].des3 = cpu_to_le32(CE_RDES3_OWN | CE_RDES3_IOC);
	pr_debug("rdes0: %08X\n", rx[cur_rx].des0);
	pr_debug("rdes1: %08X\n", rx[cur_rx].des1);
	pr_debug("rdes2: %08X\n", rx[cur_rx].des2);
	pr_debug("rdes3: %08X\n", rx[cur_rx].des3);
	/* link this request to the rx descriptor */
	ch->areqs[cur_rx] = &req->base;
	cur_rx = (cur_rx + 1) % DMA_RING_SIZE;
	ch->cur_rx = cur_rx;
	reqctx->final = true;
	spin_unlock_bh(&ch->ring_lock);

	/* inform the DMA for the new data */
	reg_write_flush(priv, CE_DMA_CH_RxDESC_TAIL_LPTR(ch->ch_num), ch->dma_rx_phy + sizeof(struct sf_ce_desc) * cur_rx);
	reg_write(priv, CE_DMA_CH_TxDESC_TAIL_LPTR(ch->ch_num), ch->dma_tx_phy + sizeof(struct sf_ce_desc) * cur_tx);

	return -EINPROGRESS;
err_map_hash:
	dma_unmap_single(priv->dev, dma_unmap_addr(reqctx, block_phys),
			 reqctx->block_len, DMA_TO_DEVICE);
err_map_block:
	dma_unmap_sg(priv->dev, req->src, nents, DMA_TO_DEVICE);
	return ret;
}

static int
sf_ce_sha256_digest(struct ahash_request *req)
{
	struct crypto_ahash *tfm = crypto_ahash_reqtfm(req);
	struct sf_ce_ahash_ctx *ctx = crypto_ahash_ctx(tfm);
	struct sf_ce_ahash_reqctx *reqctx = ahash_request_ctx(req);
	struct sf_ce_dev *priv = ctx->priv;
	struct sf_ce_chan *ch = &priv->chan[DMA_CH_SHA];
	struct sf_ce_desc *tx, *rx;
	struct scatterlist *sg;
	unsigned long cur_tx, cur_rx;
	dma_addr_t block_phys = 0, hash_phys = 0;
	unsigned int buf1len, i, index, pad_len;
	int mapped, nents, ret;

	nents = sg_nents_for_len(req->src, req->nbytes);
	if (unlikely(nents < 0))
		return nents;

	mapped = dma_map_sg(priv->dev, req->src, nents, DMA_TO_DEVICE);
	if (unlikely(mapped != nents))
		return -ENOMEM;

	reqctx->nents = nents;

	/* Pad out to 56 mod 64. */
	index = req->nbytes & 0x3f;
	pad_len = (index < 56) ? (56 - index) : ((64+56) - index);

	reqctx->block[0] = 0x80;
	memset(reqctx->block + 1, 0, pad_len - 1);

	/* Append length */
	put_unaligned_be64(req->nbytes * BITS_PER_BYTE, reqctx->block + pad_len);

	pad_len += sizeof(u64);
	reqctx->block_len = pad_len;
	/* Map partial buffer and padding */
	block_phys = dma_map_single(priv->dev, reqctx->block, sizeof(reqctx->block), DMA_TO_DEVICE);
	if (unlikely((ret = dma_mapping_error(priv->dev, block_phys))))
		goto err_map_block;

	reqctx->block_phys = block_phys;

	/* bi-dir mapping for hash initial because it's used for both TX and RX */
	hash_phys = dma_map_single(priv->dev, reqctx->hash, sizeof(reqctx->hash), DMA_BIDIRECTIONAL);
	if (unlikely((ret = dma_mapping_error(priv->dev, hash_phys))))
		goto err_map_hash;

	reqctx->hash_phys = hash_phys;

	spin_lock_bh(&ch->ring_lock);
	tx = ch->dma_tx;
	cur_tx = ch->cur_tx;

	for_each_sg(req->src, sg, nents, i) {
		int fd = (i == 1);

		if (i % 2 == 0) { /* buffer at des0 */
			tx[cur_tx].des0 = cpu_to_le32(sg_dma_address(sg));
			pr_debug("tdes0: %08X\n", tx[cur_tx].des0);
			buf1len = sg_dma_len(sg);
		} else { /* buffer at des1 */
			tx[cur_tx].des1 = cpu_to_le32(sg_dma_address(sg));
			tx[cur_tx].des2 = cpu_to_le32(FIELD_PREP(CE_TDES2_ED, CE_ENDIAN_BIG) | FIELD_PREP(CE_TDES2_SC, fd) |
						      FIELD_PREP(CE_TDES2_B1L, buf1len) |
						      FIELD_PREP(CE_TDES2_B2L, sg_dma_len(sg)) |
						      FIELD_PREP(CE_TDES2_B2T, CE_BUF_PAYLOAD));
			tx[cur_tx].des3 = cpu_to_le32(CE_TDES3_OWN | FIELD_PREP(CE_TDES3_FD, fd) |
						      FIELD_PREP(CE_TDES3_B1T, CE_BUF_PAYLOAD) |
						      FIELD_PREP(CE_TDES3_CT, CE_SHA256) |
						      FIELD_PREP(CE_TDES3_PL, req->nbytes + reqctx->block_len));
			pr_debug("tdes1: %08X\n", tx[cur_tx].des1);
			pr_debug("tdes2: %08X\n", tx[cur_tx].des2);
			pr_debug("tdes3: %08X\n", tx[cur_tx].des3);
			cur_tx = (cur_tx + 1) % DMA_RING_SIZE;
		}
	}

	/* fix up the last descriptor with padding */
	if (i % 2 != 0) {
		tx[cur_tx].des1 = cpu_to_le32(block_phys);
		tx[cur_tx].des2 = cpu_to_le32(FIELD_PREP(CE_TDES2_ED, CE_ENDIAN_BIG) |
					      FIELD_PREP(CE_TDES2_B1L, buf1len) |
					      FIELD_PREP(CE_TDES2_B2L, pad_len) |
					      FIELD_PREP(CE_TDES2_B2T, CE_BUF_PAYLOAD));
		tx[cur_tx].des3 = cpu_to_le32(CE_TDES3_OWN | FIELD_PREP(CE_TDES3_FD, i == 1) | CE_TDES3_LD |
					      FIELD_PREP(CE_TDES3_B1T, CE_BUF_PAYLOAD) |
					      FIELD_PREP(CE_TDES3_CT, CE_SHA256) |
					      FIELD_PREP(CE_TDES3_PL, req->nbytes + reqctx->block_len));
		pr_debug("tdes1: %08X\n", tx[cur_tx].des1);
		pr_debug("tdes2: %08X\n", tx[cur_tx].des2);
		pr_debug("tdes3: %08X\n", tx[cur_tx].des3);
	} else {
		tx[cur_tx].des0 = cpu_to_le32(block_phys);
		tx[cur_tx].des1 = cpu_to_le32(0);
		tx[cur_tx].des2 = cpu_to_le32(FIELD_PREP(CE_TDES2_ED, CE_ENDIAN_BIG) |
					      FIELD_PREP(CE_TDES2_B1L, pad_len));
		tx[cur_tx].des3 = cpu_to_le32(CE_TDES3_OWN | FIELD_PREP(CE_TDES3_FD, i == 0) | CE_TDES3_LD |
					      FIELD_PREP(CE_TDES3_B1T, CE_BUF_PAYLOAD) |
					      FIELD_PREP(CE_TDES3_CT, CE_SHA256) |
					      FIELD_PREP(CE_TDES3_PL, req->nbytes + reqctx->block_len));
		pr_debug("tdes0: %08X\n", tx[cur_tx].des0);
		pr_debug("tdes1: %08X\n", tx[cur_tx].des1);
		pr_debug("tdes2: %08X\n", tx[cur_tx].des2);
		pr_debug("tdes3: %08X\n", tx[cur_tx].des3);
	}

	cur_tx = (cur_tx + 1) % DMA_RING_SIZE;
	ch->cur_tx = cur_tx;

	/* One rx descriptor */
	cur_rx = ch->cur_rx;
	rx = ch->dma_rx;
	rx[cur_rx].des0 = cpu_to_le32(hash_phys);
	rx[cur_rx].des1 = cpu_to_le32(0);
	rx[cur_rx].des2 = cpu_to_le32(FIELD_PREP(CE_RDES2_B1L, SHA256_DIGEST_SIZE));
	rx[cur_rx].des3 = cpu_to_le32(CE_RDES3_OWN | CE_RDES3_IOC);
	pr_debug("rdes0: %08X\n", rx[cur_rx].des0);
	pr_debug("rdes1: %08X\n", rx[cur_rx].des1);
	pr_debug("rdes2: %08X\n", rx[cur_rx].des2);
	pr_debug("rdes3: %08X\n", rx[cur_rx].des3);

	/* link this request to the rx descriptor */
	ch->areqs[cur_rx] = &req->base;
	cur_rx = (cur_rx + 1) % DMA_RING_SIZE;
	ch->cur_rx = cur_rx;
	reqctx->final = true;
	spin_unlock_bh(&ch->ring_lock);

	/* inform the DMA for the new data */
	reg_write_flush(priv, CE_DMA_CH_RxDESC_TAIL_LPTR(ch->ch_num), ch->dma_rx_phy + sizeof(struct sf_ce_desc) * cur_rx);
	reg_write(priv, CE_DMA_CH_TxDESC_TAIL_LPTR(ch->ch_num), ch->dma_tx_phy + sizeof(struct sf_ce_desc) * cur_tx);

	return -EINPROGRESS;
err_map_hash:
	dma_unmap_single(priv->dev, dma_unmap_addr(reqctx, block_phys), reqctx->block_len, DMA_TO_DEVICE);
err_map_block:
	dma_unmap_sg(priv->dev, req->src, nents, DMA_TO_DEVICE);
	return ret;
}

static int
sf_ce_sha256_export(struct ahash_request *req, void *out)
{
	struct sf_ce_ahash_ctx *ctx = crypto_tfm_ctx(req->base.tfm);
	struct sf_ce_ahash_reqctx *reqctx = ahash_request_ctx(req);
	struct sf_ce_sha256_state *state = out;
	struct device *dev = ctx->priv->dev;

	dma_unmap_single(dev, reqctx->block_phys, sizeof(reqctx->block), DMA_TO_DEVICE);
	dma_unmap_single(dev, reqctx->hash_phys, sizeof(reqctx->hash), DMA_BIDIRECTIONAL);
	memcpy(state->block, reqctx->block, sizeof(state->block));
	memcpy(state->hash, reqctx->hash, sizeof(state->hash));
	state->count = reqctx->length;

	return 0;
}

static int
sf_ce_sha256_import(struct ahash_request *req, const void *in)
{
	struct sf_ce_ahash_ctx *ctx = crypto_tfm_ctx(req->base.tfm);
	struct sf_ce_ahash_reqctx *reqctx = ahash_request_ctx(req);
	const struct sf_ce_sha256_state *state = in;
	struct device *dev = ctx->priv->dev;
	dma_addr_t hash_phys, block_phys;
	int ret;

	reqctx->final = false;
	memcpy(reqctx->block, state->block, sizeof(state->block));
	memcpy(reqctx->hash, state->hash, sizeof(state->hash));
	reqctx->length = state->count;

	/* bi-dir mapping for hash initial because it's used for both TX and RX */
	hash_phys = dma_map_single(dev, reqctx->hash, sizeof(reqctx->hash), DMA_BIDIRECTIONAL);
	if (unlikely((ret = dma_mapping_error(dev, hash_phys))))
		return ret;

	reqctx->hash_phys = hash_phys;

	/* map partial block */
	block_phys = dma_map_single(dev, reqctx->block, sizeof(reqctx->block), DMA_TO_DEVICE);
	if (unlikely((ret = dma_mapping_error(dev, block_phys)))) {
		dma_unmap_single(dev, reqctx->hash_phys, sizeof(reqctx->hash),
				 DMA_BIDIRECTIONAL);
		return ret;
	}

	reqctx->block_phys = block_phys;
	return 0;
}

struct ahash_alg sf_ce_sha256 __read_mostly = {
	.init	= sf_ce_sha256_init,
	.update	= sf_ce_sha256_update,
	.final	= sf_ce_sha256_final,
	.finup	= sf_ce_sha256_finup,
	.digest	= sf_ce_sha256_digest,
	.export = sf_ce_sha256_export,
	.import = sf_ce_sha256_import,
	.halg.digestsize	= SHA256_DIGEST_SIZE,
	.halg.statesize		= sizeof(struct sf_ce_sha256_state),
	.halg.base	=	{
		.cra_name		= "sha256",
		.cra_driver_name	= "siflower-ce-sha256",
		.cra_priority		= 300,
		.cra_blocksize		= SHA256_BLOCK_SIZE,
		.cra_ctxsize		= sizeof(struct sf_ce_ahash_ctx),
		.cra_flags		= CRYPTO_ALG_ASYNC |
					  CRYPTO_ALG_KERN_DRIVER_ONLY,
		.cra_module		= THIS_MODULE,
		.cra_init		= sf_ce_ahash_cra_init,
	},
};

static int sf_ce_sha224_digest(struct ahash_request *req)
{
	return sf_ce_sha224_init(req) ?: sf_ce_sha256_finup(req);
}

struct ahash_alg sf_ce_sha224 __read_mostly = {
	.init	= sf_ce_sha224_init,
	.update	= sf_ce_sha256_update,
	.final	= sf_ce_sha256_final,
	.finup	= sf_ce_sha256_finup,
	.digest	= sf_ce_sha224_digest,
	.export = sf_ce_sha256_export,
	.import = sf_ce_sha256_import,
	.halg.digestsize	= SHA224_DIGEST_SIZE,
	.halg.statesize		= sizeof(struct sf_ce_sha256_state),
	.halg.base	=	{
		.cra_name		= "sha224",
		.cra_driver_name	= "siflower-ce-sha224",
		.cra_priority		= 300,
		.cra_blocksize		= SHA224_BLOCK_SIZE,
		.cra_ctxsize		= sizeof(struct sf_ce_ahash_ctx),
		.cra_flags		= CRYPTO_ALG_ASYNC |
					  CRYPTO_ALG_KERN_DRIVER_ONLY,
		.cra_module		= THIS_MODULE,
		.cra_init		= sf_ce_ahash_cra_init,
	},
};
