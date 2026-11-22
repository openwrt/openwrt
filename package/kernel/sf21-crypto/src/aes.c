#include "common.h"
#include "aes.h"

#define SF_AES_GCM_MIN_PTEXT_LEN	9

#ifdef CONFIG_CRYPTO_MANAGER_DISABLE_TESTS
#define SW_FALLBACK_THRES	128
#endif
/* requests above this hang the engine (13-bit Tx buffer length fields) */
#define SF_AES_MAX_LEN		8192
/***
 * Buffer alignment requirements:
 *
 * Tx (CPU to device):
 * No address alignment for Tx buffers. But some buffer type requires a total
 * length of multiple of 8 bytes.
 *
 * Rx (device to CPU):
 * Both Start Address and End Address (Start Address + length) must be 8-byte
 * aligned.
 *
 * The Start Address of the first Rx buffer, or the End Address of the last Rx
 * buffer may be unaligned, but DMA will overwrite the unaligned portion with
 * garbage data.
 *
 * Rx total buffer size of AES-CTR must be multiple of 16 bytes, as it
 * internally pads the result to AES_BLOCK_SIZE and does not trim the output.
 *
 * For noncoherent DMA, Rx buffer must be aligned to ARCH_DMA_MINALIGN to
 * prevent data loss.
 */

static bool sf_ce_need_bounce_buffer(struct scatterlist *dst, int nents, unsigned int align)
{
	struct scatterlist *sg;
	int i;

	for_each_sg(dst, sg, nents, i) {
		if (!IS_ALIGNED(sg->offset, align) || !IS_ALIGNED(sg->length, align))
			return true;
	}

	return false;
}

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
static int sf_ce_sg_nents_for_len(struct scatterlist *sg, u64 len, u64 *extra)
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

static unsigned int
sf_ce_dma_tx_avail(struct sf_ce_chan *ch)
{
	if (ch->dirty_tx > ch->cur_tx)
		return ch->dirty_tx - ch->cur_tx - 1;
	else
		return DMA_RING_SIZE - ch->cur_tx + ch->dirty_tx - 1;
}

static int
sf_ce_aes_ctr_do_fallback(struct skcipher_request *req, struct crypto_sync_skcipher *fbtfm,
			  bool is_decrypt)
{
	SYNC_SKCIPHER_REQUEST_ON_STACK(subreq, fbtfm);
	int ret;

	skcipher_request_set_sync_tfm(subreq, fbtfm);
	skcipher_request_set_callback(subreq, req->base.flags, NULL, NULL);
	skcipher_request_set_crypt(subreq, req->src, req->dst, req->cryptlen,
				   req->iv);
	ret = is_decrypt ? crypto_skcipher_decrypt(subreq) :
			   crypto_skcipher_encrypt(subreq);
	skcipher_request_zero(subreq);
	return ret;
}

static int
sf_ce_aes_ctr_op(struct skcipher_request *req, bool is_decrypt)
{
	struct sf_ce_aes_reqctx *reqctx = skcipher_request_ctx(req);
	struct sf_ce_aes_ctx *ctx = crypto_tfm_ctx(req->base.tfm);
	struct sf_ce_dev *priv = ctx->priv;
	struct device *dev = priv->dev;
	struct sf_ce_chan *ch = NULL;
	dma_addr_t iv_phys;
	int ret = 0;
	struct sf_ce_desc *rx, *tx;
	unsigned int cur_tx, cur_rx;
	struct scatterlist *sg;
	unsigned int i = 0, nbytes = 0, buf1len = 0;
	u64 src_extra_bytes = 0, dst_extra_bytes = 0;

	/* the engine never completes an empty request */
	if (unlikely(!req->cryptlen))
		return 0;
	if (unlikely(req->cryptlen > SF_AES_MAX_LEN))
		return sf_ce_aes_ctr_do_fallback(req, ctx->fback, is_decrypt);

#ifdef SW_FALLBACK_THRES
	if (unlikely(req->cryptlen < SW_FALLBACK_THRES)) {
		/* fallback to software if len is too small */
		return sf_ce_aes_ctr_do_fallback(req, ctx->fback, is_decrypt);
	}
#endif

	reqctx->tmp_buf = NULL;
	reqctx->tmp_buf_phys = 0;
	/* req->iv may not be physically contiguous, so copy it to reqctx,
	 * which is contiguous and can be used for DMA.
	 */
	memcpy(reqctx->iv, req->iv, AES_BLOCK_SIZE);
	ret = sf_ce_sg_nents_for_len(req->src, req->cryptlen, &src_extra_bytes);
	if (ret <= 0)
		return ret;
	reqctx->ssg_len = ret;
	ret = sf_ce_sg_nents_for_len(req->dst, req->cryptlen, &dst_extra_bytes);
	if (ret <= 0)
		return ret;
	reqctx->dsg_len = ret;
	if (src_extra_bytes)
		pr_debug("src_extra_bytes: %lld\n", src_extra_bytes);
	if (dst_extra_bytes)
		pr_debug("dst_extra_bytes: %lld\n", dst_extra_bytes);
	if (sf_ce_need_bounce_buffer(req->dst, reqctx->dsg_len, ARCH_DMA_MINALIGN)) {
		void *tmp_buf;
		gfp_t flags;

		if (unlikely(reqctx->ssg_len != dma_map_sg(dev, req->src,
							   reqctx->ssg_len,
							   DMA_TO_DEVICE)))
			return -ENOMEM;

		flags = (req->base.flags & CRYPTO_TFM_REQ_MAY_SLEEP) ?
			GFP_KERNEL : GFP_ATOMIC;
		tmp_buf = kmalloc(ALIGN(req->cryptlen, ARCH_DMA_MINALIGN), flags);
		if (!tmp_buf)
			return -ENOMEM;

		reqctx->tmp_buf = tmp_buf;
		reqctx->tmp_buf_phys = dma_map_single(dev, tmp_buf, ALIGN(req->cryptlen, ARCH_DMA_MINALIGN), DMA_FROM_DEVICE);
		if (dma_mapping_error(dev, reqctx->tmp_buf_phys))
			return -ENOMEM;
	} else if (req->src == req->dst) {
		if (unlikely(reqctx->ssg_len != dma_map_sg(dev, req->src,
							   reqctx->ssg_len,
							   DMA_BIDIRECTIONAL)))
			return -ENOMEM;
	} else {
		if (unlikely(reqctx->ssg_len != dma_map_sg(dev, req->src,
							   reqctx->ssg_len,
							   DMA_TO_DEVICE)))
			return -ENOMEM;

		if (unlikely(reqctx->dsg_len != dma_map_sg(dev, req->dst,
							   reqctx->dsg_len,
							   DMA_FROM_DEVICE)))
			return -ENOMEM;
	}

	iv_phys = dma_map_single(dev, reqctx->iv, AES_BLOCK_SIZE, DMA_TO_DEVICE);
	if (unlikely((ret = dma_mapping_error(dev, iv_phys))))
		return ret;

	reqctx->iv_phys = iv_phys;
	nbytes = ALIGN(req->cryptlen, AES_BLOCK_SIZE);

#ifdef CONFIG_CRYPTO_SIFLOWER_HASH
	ch = &priv->chan[DMA_CH_AES];
	spin_lock_bh(&ch->ring_lock);
#else
	local_bh_disable();
	ch = &priv->chan[smp_processor_id()];
#endif
	if (unlikely(sf_ce_dma_tx_avail(ch) < DIV_ROUND_UP(reqctx->ssg_len, 2) + 1)) {
		kfree(reqctx->tmp_buf);
#ifdef CONFIG_CRYPTO_SIFLOWER_HASH
		spin_unlock_bh(&ch->ring_lock);
#else
		local_bh_enable();
#endif
		pr_err_ratelimited("tx ring full\n");
		return sf_ce_aes_ctr_do_fallback(req, ctx->fback, is_decrypt);
	}
	tx = ch->dma_tx;
	cur_tx = ch->cur_tx;

	//1: key and iv
	tx[cur_tx].des0 = ctx->key_phys; // buffer1: key
	tx[cur_tx].des1 = reqctx->iv_phys; // buffer2: iv
	tx[cur_tx].des2 = FIELD_PREP(CE_TDES2_ED, CE_ENDIAN_BIG) |
		 FIELD_PREP(CE_TDES2_B1L, ctx->keylen) |
		 FIELD_PREP(CE_TDES2_B2L, AES_BLOCK_SIZE) |
		 FIELD_PREP(CE_TDES2_B2T, CE_BUF_AES_IV);
	tx[cur_tx].des3 = CE_TDES3_OWN | CE_TDES3_FD |
		 FIELD_PREP(CE_TDES3_B1T, CE_BUF_AES_KEY) |
		 FIELD_PREP(CE_TDES3_CM, is_decrypt ? CM_DECRYPT : CM_ENCRYPT) |
		 FIELD_PREP(CE_TDES3_CT, CE_AES_CTR) |
		 FIELD_PREP(CE_TDES3_PL, nbytes);
	pr_debug("tdes0: %08X\n", tx[cur_tx].des0);
	pr_debug("tdes1: %08X\n", tx[cur_tx].des1);
	pr_debug("tdes2: %08X\n", tx[cur_tx].des2);
	pr_debug("tdes3: %08X\n", tx[cur_tx].des3);

	cur_tx = (cur_tx + 1) % DMA_RING_SIZE;

	for_each_sg(req->src, sg, reqctx->ssg_len, i) {
		int ld = (i == reqctx->ssg_len - 1);
		if (i % 2 == 0) { /* buffer at des0 */
			tx[cur_tx].des0 = sg_dma_address(sg);
			buf1len = sg_dma_len(sg);
			pr_debug("tdes0: %08X\n", tx[cur_tx].des0);
		} else { /* buffer at des1 */
			tx[cur_tx].des1 = sg_dma_address(sg);
			tx[cur_tx].des2 = FIELD_PREP(CE_TDES2_ED, CE_ENDIAN_BIG) |
				FIELD_PREP(CE_TDES2_B2L, sg_dma_len(sg) - (ld ? src_extra_bytes : 0)) |
				FIELD_PREP(CE_TDES2_B2T, CE_BUF_PAYLOAD) |
				FIELD_PREP(CE_TDES2_B1L, buf1len);
			tx[cur_tx].des3 =
				CE_TDES3_OWN | FIELD_PREP(CE_TDES3_LD, ld) |
				FIELD_PREP(CE_TDES3_B1T, CE_BUF_PAYLOAD) |
				FIELD_PREP(CE_TDES3_CM, is_decrypt ?
								CM_DECRYPT :
								CM_ENCRYPT) |
				FIELD_PREP(CE_TDES3_CT, CE_AES_CTR) |
				FIELD_PREP(CE_TDES3_PL, nbytes);
			pr_debug("tdes1: %08X\n", tx[cur_tx].des1);
			pr_debug("tdes2: %08X\n", tx[cur_tx].des2);
			pr_debug("tdes3: %08X\n", tx[cur_tx].des3);
			cur_tx = (cur_tx + 1) % DMA_RING_SIZE;
		}
	}

	/* fix up the last desc */
	if (i % 2) {
		tx[cur_tx].des1 = 0;
		tx[cur_tx].des2 = FIELD_PREP(CE_TDES2_ED, CE_ENDIAN_BIG) | FIELD_PREP(CE_TDES2_B2T, CE_BUF_PAYLOAD) | FIELD_PREP(CE_TDES2_B1L, buf1len - src_extra_bytes);
		tx[cur_tx].des3 =
			CE_TDES3_OWN | CE_TDES3_LD |
			FIELD_PREP(CE_TDES3_B1T, CE_BUF_PAYLOAD) |
			FIELD_PREP(CE_TDES3_CM,
				   is_decrypt ? CM_DECRYPT : CM_ENCRYPT) |
			FIELD_PREP(CE_TDES3_CT, CE_AES_CTR) |
			FIELD_PREP(CE_TDES3_PL, nbytes);
		pr_debug("tdes1: %08X\n", tx[cur_tx].des1);
		pr_debug("tdes2: %08X\n", tx[cur_tx].des2);
		pr_debug("tdes3: %08X\n", tx[cur_tx].des3);
		cur_tx = (cur_tx + 1) % DMA_RING_SIZE;
	}
	ch->cur_tx = cur_tx;

	rx = ch->dma_rx;
	cur_rx = ch->cur_rx;
	// prepare rx desc

	if (reqctx->tmp_buf_phys) {
		/* one large rx buffer for tmp buffer */
		rx[cur_rx].des0 = reqctx->tmp_buf_phys;
		rx[cur_rx].des1 = 0;
		rx[cur_rx].des2 = FIELD_PREP(CE_RDES2_B1L, ALIGN(nbytes, AES_BLOCK_SIZE));
		rx[cur_rx].des3 = CE_RDES3_OWN | CE_RDES3_IOC;
		pr_debug("rdes0: %08X\n", rx[cur_rx].des0);
		pr_debug("rdes1: %08X\n", rx[cur_rx].des1);
		pr_debug("rdes2: %08X\n", rx[cur_rx].des2);
		pr_debug("rdes3: %08X\n", rx[cur_rx].des3);
		/* link this request to the rx descriptor */
		ch->areqs[cur_rx] = &req->base;
		cur_rx = (cur_rx + 1) % DMA_RING_SIZE;
	} else {
		// merge every two buffers into one dma desc list
		for_each_sg (req->dst, sg, reqctx->dsg_len, i) {
			int ld = (i == reqctx->dsg_len - 1);

			if (i % 2 == 0) {
				rx[cur_rx].des0 = sg_dma_address(sg);
				buf1len = sg_dma_len(sg);
				pr_debug("rdes0: %08X\n", rx[cur_rx].des0);
			} else {
				rx[cur_rx].des1 = sg_dma_address(sg);
				rx[cur_rx].des2 =
					FIELD_PREP(CE_RDES2_B1L, buf1len) |
					FIELD_PREP(CE_RDES2_B2L,
						   sg_dma_len(sg) - (ld ? dst_extra_bytes : 0));
				rx[cur_rx].des3 = CE_RDES3_OWN |
						  FIELD_PREP(CE_RDES3_IOC, ld);
				if (ld) {
					/* link this request to the rx descriptor */
					ch->areqs[cur_rx] = &req->base;
				}
				pr_debug("rdes1: %08X\n", rx[cur_rx].des1);
				pr_debug("rdes2: %08X\n", rx[cur_rx].des2);
				pr_debug("rdes3: %08X\n", rx[cur_rx].des3);
				cur_rx = (cur_rx + 1) % DMA_RING_SIZE;
			}
		}
		if (i % 2) {
			rx[cur_rx].des1 = 0;
			rx[cur_rx].des2 = FIELD_PREP(CE_RDES2_B1L, buf1len - dst_extra_bytes);
			rx[cur_rx].des3 = CE_RDES3_OWN | CE_RDES3_IOC;
			/* link this request to the rx descriptor */
			ch->areqs[cur_rx] = &req->base;
			pr_debug("rdes1: %08X\n", rx[cur_rx].des1);
			pr_debug("rdes2: %08X\n", rx[cur_rx].des2);
			pr_debug("rdes3: %08X\n", rx[cur_rx].des3);
			cur_rx = (cur_rx + 1) % DMA_RING_SIZE;
		}
	}

	ch->cur_rx = cur_rx;
#ifdef CONFIG_CRYPTO_SIFLOWER_HASH
	spin_unlock_bh(&ch->ring_lock);
#else
	local_bh_enable();
#endif
	reg_write_flush(priv, CE_DMA_CH_RxDESC_TAIL_LPTR(ch->ch_num), ch->dma_rx_phy + sizeof(struct sf_ce_desc) * cur_rx);
	reg_write(priv, CE_DMA_CH_TxDESC_TAIL_LPTR(ch->ch_num), ch->dma_tx_phy + sizeof(struct sf_ce_desc) * cur_tx);

	return -EINPROGRESS;
}

static int
sf_ce_aes_cra_init(struct crypto_tfm *tfm)
{
	struct sf_ce_aes_ctx *ctx = crypto_tfm_ctx(tfm);
	struct sf_ce_dev *priv = g_dev;
	struct device *dev = priv->dev;
	int ret;

	/* Allocate fallback implementation */
	ctx->fback = crypto_alloc_sync_skcipher(crypto_tfm_alg_name(tfm), 0, CRYPTO_ALG_NEED_FALLBACK);
	if (IS_ERR(ctx->fback))
		return PTR_ERR(ctx->fback);
	crypto_skcipher_set_reqsize(__crypto_skcipher_cast(tfm),
				    sizeof(struct sf_ce_aes_reqctx));
	ctx->priv = priv;
	ctx->key_phys = dma_map_single_attrs(dev, ctx->key, sizeof(ctx->key),
					     DMA_TO_DEVICE, DMA_ATTR_SKIP_CPU_SYNC);

	ret = dma_mapping_error(dev, ctx->key_phys);
	if (ret)
		crypto_free_sync_skcipher(ctx->fback);
	return ret;
}

static void
sf_ce_aes_cra_exit(struct crypto_tfm *tfm)
{
	struct sf_ce_aes_ctx *ctx = crypto_tfm_ctx(tfm);
	struct device *dev = ctx->priv->dev;

	dma_unmap_single(dev, ctx->key_phys, sizeof(ctx->key), DMA_TO_DEVICE);
	crypto_free_sync_skcipher(ctx->fback);
}

static int
sf_ce_aes_ctr_setkey(struct crypto_skcipher *tfm, const u8 *key,
		     unsigned int keylen)
{
	struct sf_ce_aes_ctx *ctx = crypto_skcipher_ctx(tfm);
	struct device *dev = ctx->priv->dev;
	int ret;

	ret = crypto_sync_skcipher_setkey(ctx->fback, key, keylen);
	if (ret)
		return ret;

	ctx->keylen = keylen;
	memcpy(ctx->key, key, keylen);
	dma_sync_single_for_device(dev, ctx->key_phys, keylen, DMA_TO_DEVICE);
	return 0;
}

static int
sf_ce_aes_ctr_enc(struct skcipher_request *req)
{
	return sf_ce_aes_ctr_op(req, false);
}

static int
sf_ce_aes_ctr_dec(struct skcipher_request *req)
{
	return sf_ce_aes_ctr_op(req, true);
}

struct skcipher_alg sf_ce_aes_ctr __read_mostly = {
	.setkey = sf_ce_aes_ctr_setkey,
	.encrypt = sf_ce_aes_ctr_enc,
	.decrypt = sf_ce_aes_ctr_dec,
	.ivsize = AES_BLOCK_SIZE,
	.min_keysize = AES_MIN_KEY_SIZE,
	.max_keysize = AES_MAX_KEY_SIZE,
	.base = {
		.cra_name		= "ctr(aes)",
		.cra_driver_name	= "siflower-ce-aes-ctr",
		.cra_priority		= 300,
		.cra_blocksize		= 1,
		.cra_ctxsize		= sizeof(struct sf_ce_aes_ctx),
		.cra_flags		= CRYPTO_ALG_ASYNC |
					  CRYPTO_ALG_KERN_DRIVER_ONLY |
					  CRYPTO_ALG_NEED_FALLBACK,
		.cra_module		= THIS_MODULE,
		.cra_init		= sf_ce_aes_cra_init,
		.cra_exit		= sf_ce_aes_cra_exit,
	},
};


static int sf_ce_aes_gcm_cra_init(struct crypto_tfm *tfm)
{
	struct sf_ce_aes_gcm_ctx *ctx = crypto_tfm_ctx(tfm);
	struct sf_ce_dev *priv = g_dev;
	struct device *dev = priv->dev;
	int ret;

	BUILD_BUG_ON(&ctx->key[sizeof(ctx->key)] != &ctx->k0[0]);

	/* Allocate fallback implementation */
	ctx->fback = crypto_alloc_aead(crypto_tfm_alg_name(tfm), 0, CRYPTO_ALG_NEED_FALLBACK);
	if (IS_ERR(ctx->fback))
		return PTR_ERR(ctx->fback);
	crypto_aead_set_reqsize(__crypto_aead_cast(tfm),
				max(sizeof(struct sf_ce_aes_gcm_reqctx), sizeof(struct aead_request) + crypto_aead_reqsize(ctx->fback)));
	ctx->priv = g_dev;
	ctx->key_0_phys = dma_map_single_attrs(dev, ctx->key, sizeof(ctx->key) + sizeof(ctx->k0),
					       DMA_TO_DEVICE, DMA_ATTR_SKIP_CPU_SYNC);
	ret = dma_mapping_error(dev, ctx->key_0_phys);
	if (ret)
		crypto_free_aead(ctx->fback);
	return ret;
}

static void
sf_ce_aes_gcm_cra_exit(struct crypto_tfm *tfm)
{
	struct sf_ce_aes_gcm_ctx *ctx = crypto_tfm_ctx(tfm);
	struct device *dev = ctx->priv->dev;

	dma_unmap_single_attrs(dev, ctx->key_0_phys, sizeof(ctx->key) + sizeof(ctx->k0), DMA_TO_DEVICE, DMA_ATTR_SKIP_CPU_SYNC);
	crypto_free_aead(ctx->fback);
}

static int
sf_ce_aes_gcm_setkey(struct crypto_aead *tfm, const u8 *key,
		     unsigned int keylen)
{
	struct sf_ce_aes_gcm_ctx *ctx = crypto_aead_ctx(tfm);
	struct sf_ce_dev *priv = ctx->priv;
	struct crypto_aes_ctx aes;
	int ret;

	ret = aes_expandkey(&aes, key, keylen);
	if (ret)
		return ret;

	ctx->keylen = keylen;
	memcpy(ctx->key, key, keylen);
	aes_encrypt(&aes, ctx->k0, (const u8 *)empty_zero_page);
	dma_sync_single_for_device(priv->dev, ctx->key_0_phys, sizeof(ctx->key) + sizeof(ctx->k0), DMA_TO_DEVICE);
	memzero_explicit(&aes, sizeof(aes));
	return crypto_aead_setkey(ctx->fback, key, keylen);
}

static int
sf_ce_aes_gcm_setauthsize(struct crypto_aead *tfm, unsigned int authsize)
{
	struct sf_ce_aes_gcm_ctx *ctx = crypto_aead_ctx(tfm);

	return crypto_aead_setauthsize(ctx->fback, authsize);
}

static int
sf_ce_aes_gcm_do_fallback(struct aead_request *req, bool is_decrypt)
{
	struct aead_request *subreq = aead_request_ctx(req);
	struct crypto_aead *aead = crypto_aead_reqtfm(req);
	struct sf_ce_aes_gcm_ctx *ctx = crypto_aead_ctx(aead);

	aead_request_set_tfm(subreq, ctx->fback);
	aead_request_set_callback(subreq, req->base.flags,
				  req->base.complete,
				  req->base.data);
	aead_request_set_crypt(subreq, req->src, req->dst,
			       req->cryptlen, req->iv);
	aead_request_set_ad(subreq, req->assoclen);

	return is_decrypt ? crypto_aead_decrypt(subreq) : crypto_aead_encrypt(subreq);
}

static int
sf_ce_aes_gcm_op(struct aead_request *req, bool is_decrypt)
{
	struct sf_ce_aes_gcm_reqctx *reqctx = aead_request_ctx(req);
	struct crypto_aead *aead = crypto_aead_reqtfm(req);
	struct sf_ce_aes_gcm_ctx *ctx = crypto_aead_ctx(aead);
	struct sf_ce_dev *priv = ctx->priv;
	struct device *dev = priv->dev;
	struct sf_ce_chan *ch = NULL;
	dma_addr_t iv_extra_tag_phys;
	int ret;
	struct sf_ce_desc *rx, *tx;
	unsigned int cur_tx, cur_rx;
	struct scatterlist *sg = NULL, *dsg_without_adata = NULL, __dsg_without_adata[2];
	unsigned int i, buf1len = 0, pl;
	unsigned int adata_sgs = 0;
	u64 src_extra_bytes = 0, dst_extra_bytes = 0;
	int alen = req->assoclen;
	int dsg_nents_without_adata = 0;
	const unsigned int ptext_len = req->cryptlen - (is_decrypt ? crypto_aead_authsize(aead) : 0);
	const unsigned int ctext_and_tag_len = req->cryptlen + (is_decrypt ? 0 : crypto_aead_authsize(aead));

	if (unlikely(req->assoclen + ctext_and_tag_len > SF_AES_MAX_LEN))
		return sf_ce_aes_gcm_do_fallback(req, is_decrypt);
#ifdef SW_FALLBACK_THRES
	if (unlikely(ptext_len < max(SW_FALLBACK_THRES, SF_AES_GCM_MIN_PTEXT_LEN))) {
		/* fallback to software if len is too small */
		return sf_ce_aes_gcm_do_fallback(req, is_decrypt);
	}
#else
	/* hw mininum acceptable ptext len */
	if (unlikely(ptext_len < SF_AES_GCM_MIN_PTEXT_LEN)) {
		return sf_ce_aes_gcm_do_fallback(req, is_decrypt);
	}
#endif
	if (unlikely(crypto_aead_authsize(aead) != 16)) {
		/* TODO: handle uncommon tag size (hw always output 16 bytes, need to be trimmed by sw) */
		return sf_ce_aes_gcm_do_fallback(req, is_decrypt);
	}
	pr_debug("%s\n", is_decrypt ? "decrypt" : "encrypt");

	reqctx->tmp_buf = NULL;
	reqctx->tmp_buf_phys = 0;
	/* req->iv may not be physically contiguous, so copy it to reqctx,
	 * which is contiguous and can be used for DMA.
	 */
	memcpy(reqctx->iv, req->iv, GCM_AES_IV_SIZE);
	memset(reqctx->iv + GCM_AES_IV_SIZE, 0, sizeof(reqctx->iv) - GCM_AES_IV_SIZE);
	reqctx->alen_dma = cpu_to_be64(req->assoclen);
	reqctx->plen_dma = cpu_to_be64(ptext_len);
	dsg_without_adata = scatterwalk_ffwd(__dsg_without_adata, req->dst, req->assoclen);
	dsg_nents_without_adata = sg_nents(dsg_without_adata);
	pr_debug("assoclen: %u, cryptlen: %u, taglen: %u\n", req->assoclen, req->cryptlen, crypto_aead_authsize(aead));
	if (is_decrypt) {
		ret = sf_ce_sg_nents_for_len(req->src, ptext_len + req->assoclen,
					     &src_extra_bytes);
		if (ret <= 0)
			return ret;
		reqctx->ssg_len = ret;
		ret = sf_ce_sg_nents_for_len(req->dst, ptext_len + req->assoclen,
					     &dst_extra_bytes);
		if (ret <= 0)
			return ret;
		reqctx->dsg_len = ret;
	} else {
		ret = sf_ce_sg_nents_for_len(req->src, ptext_len + req->assoclen,
					     &src_extra_bytes);
		if (ret <= 0)
			return ret;
		reqctx->ssg_len = ret;
		ret = sf_ce_sg_nents_for_len(req->dst, ctext_and_tag_len + req->assoclen,
					     &dst_extra_bytes);
		if (ret <= 0)
			return ret;
		reqctx->dsg_len = ret;
	}
	pr_debug("reqctx->ssg_len: %u, reqctx->dsg_len %u\n", reqctx->dsg_len, reqctx->ssg_len);
	if (req->assoclen)
		pr_debug("assoclen: %u\n", req->assoclen);
	if (src_extra_bytes)
		pr_debug("src_extra_bytes: %lld\n", src_extra_bytes);
	if (dst_extra_bytes)
		pr_debug("dst_extra_bytes: %lld\n", dst_extra_bytes);

	if (is_decrypt) {
		reqctx->taglen_dma = cpu_to_be64(crypto_aead_authsize(aead));
		scatterwalk_map_and_copy(reqctx->tag, req->src, req->assoclen + ptext_len, crypto_aead_authsize(aead), 0);
	}
	if (sf_ce_need_bounce_buffer(dsg_without_adata, dsg_nents_without_adata, ARCH_DMA_MINALIGN)) {
		void *tmp_buf;
		gfp_t flags;

		if (unlikely(reqctx->ssg_len != dma_map_sg(dev, req->src,
							   reqctx->ssg_len,
							   DMA_TO_DEVICE)))
			return -ENOMEM;

		flags = (req->base.flags & CRYPTO_TFM_REQ_MAY_SLEEP) ?
			GFP_KERNEL : GFP_ATOMIC;
		tmp_buf = kmalloc(ALIGN(ctext_and_tag_len, DMA_RX_ALIGN), flags);
		if (!tmp_buf)
			return -ENOMEM;

		reqctx->tmp_buf = tmp_buf;
		reqctx->tmp_buf_phys = dma_map_single(dev, tmp_buf, ALIGN(ctext_and_tag_len, DMA_RX_ALIGN), DMA_FROM_DEVICE);
	} else if (req->src == req->dst) {
		reqctx->dsg_len = reqctx->ssg_len;

		if (unlikely(reqctx->ssg_len != dma_map_sg(dev, req->src,
							   reqctx->ssg_len,
							   DMA_BIDIRECTIONAL)))
			return -ENOMEM;
	} else {
		if (unlikely(reqctx->ssg_len != dma_map_sg(dev, req->src,
							   reqctx->ssg_len,
							   DMA_TO_DEVICE)))
			return -ENOMEM;

		if (unlikely(reqctx->dsg_len != dma_map_sg(dev, req->dst,
							   reqctx->dsg_len,
							   DMA_FROM_DEVICE)))
			return -ENOMEM;
	}
	dma_map_sg_attrs(dev, dsg_without_adata, dsg_nents_without_adata, DMA_TO_DEVICE, DMA_ATTR_SKIP_CPU_SYNC);

	iv_extra_tag_phys = dma_map_single(dev, reqctx->iv, sizeof(reqctx->iv) + sizeof(reqctx->alen_dma) + sizeof(reqctx->plen_dma) + sizeof(reqctx->taglen_dma) + sizeof(reqctx->tag), DMA_TO_DEVICE);
	if (unlikely((ret = dma_mapping_error(dev, iv_extra_tag_phys))))
		return ret;

	reqctx->iv_extra_tag_phys = iv_extra_tag_phys;
	pl = ptext_len + ALIGN(req->assoclen, AES_BLOCK_SIZE);

#ifdef CONFIG_CRYPTO_SIFLOWER_HASH
	ch = &priv->chan[DMA_CH_AES];
	spin_lock_bh(&ch->ring_lock);
#else
	local_bh_disable();
	ch = &priv->chan[smp_processor_id()];
#endif
	if (unlikely(sf_ce_dma_tx_avail(ch) < DIV_ROUND_UP(reqctx->ssg_len, 2) + 6)) {
		kfree(reqctx->tmp_buf);
#ifdef CONFIG_CRYPTO_SIFLOWER_HASH
		spin_unlock_bh(&ch->ring_lock);
#else
		local_bh_enable();
#endif
		pr_err_ratelimited("tx ring full\n");
		return sf_ce_aes_gcm_do_fallback(req, is_decrypt);
	}
	tx = ch->dma_tx;
	cur_tx = ch->cur_tx;

	//1: key and iv
	tx[cur_tx].des0 = ctx->key_0_phys; // buffer1: key
	tx[cur_tx].des1 = reqctx->iv_extra_tag_phys; // buffer2: iv
	tx[cur_tx].des2 = FIELD_PREP(CE_TDES2_ED, CE_ENDIAN_BIG) |
		 FIELD_PREP(CE_TDES2_B1L, ctx->keylen) |
		 FIELD_PREP(CE_TDES2_B2L, AES_BLOCK_SIZE) |
		 FIELD_PREP(CE_TDES2_B2T, CE_BUF_AES_IV);
	tx[cur_tx].des3 = CE_TDES3_OWN | CE_TDES3_FD |
		 FIELD_PREP(CE_TDES3_B1T, CE_BUF_AES_KEY) |
		 FIELD_PREP(CE_TDES3_CM, is_decrypt ? CM_DECRYPT : CM_ENCRYPT) |
		 FIELD_PREP(CE_TDES3_CT, CE_AES_GCM) |
		 FIELD_PREP(CE_TDES3_PL, pl);

	pr_debug("tdes0: %08X\n", tx[cur_tx].des0);
	pr_debug("tdes1: %08X\n", tx[cur_tx].des1);
	pr_debug("tdes2: %08X\n", tx[cur_tx].des2);
	pr_debug("tdes3: %08X\n", tx[cur_tx].des3);
	cur_tx = (cur_tx + 1) % DMA_RING_SIZE;

	if (is_decrypt) {
		// prepare tagparams and k0
		tx[cur_tx].des0 = reqctx->iv_extra_tag_phys + sizeof(reqctx->iv) + sizeof(reqctx->alen_dma) + sizeof(reqctx->plen_dma);
		tx[cur_tx].des1 = ctx->key_0_phys + sizeof(ctx->key);
		tx[cur_tx].des2 =
			FIELD_PREP(CE_TDES2_ED, CE_ENDIAN_BIG) |
			FIELD_PREP(CE_TDES2_B1L, sizeof(reqctx->taglen_dma) + sizeof(reqctx->tag)) |
			FIELD_PREP(CE_TDES2_B2L, sizeof(ctx->k0)) |
			FIELD_PREP(CE_TDES2_B2T, CE_BUF_AES_AEAD_K0);
		tx[cur_tx].des3 =
			CE_TDES3_OWN |
			FIELD_PREP(CE_TDES3_B1T, CE_BUF_AES_AEAD_TAG) |
			FIELD_PREP(CE_TDES3_CM,
				   is_decrypt ? CM_DECRYPT : CM_ENCRYPT) |
			FIELD_PREP(CE_TDES3_CT, CE_AES_GCM) |
			FIELD_PREP(CE_TDES3_PL, pl);
		pr_debug("tdes0: %08X\n", tx[cur_tx].des0);
		pr_debug("tdes1: %08X\n", tx[cur_tx].des1);
		pr_debug("tdes2: %08X\n", tx[cur_tx].des2);
		pr_debug("tdes3: %08X\n", tx[cur_tx].des3);
		cur_tx = (cur_tx + 1) % DMA_RING_SIZE;
		// extra
		tx[cur_tx].des0 = reqctx->iv_extra_tag_phys + sizeof(reqctx->iv); // buffer2: extra
		tx[cur_tx].des1 = 0;
		tx[cur_tx].des2 =
			FIELD_PREP(CE_TDES2_ED, CE_ENDIAN_BIG) |
			FIELD_PREP(CE_TDES2_B1L, sizeof(reqctx->alen_dma) + sizeof(reqctx->plen_dma)) |
			FIELD_PREP(CE_TDES2_B2L, 0) |
			FIELD_PREP(CE_TDES2_B2T, CE_BUF_AES_AEAD_HEADER);
		tx[cur_tx].des3 =
			CE_TDES3_OWN |
			FIELD_PREP(CE_TDES3_B1T, CE_BUF_AES_AEAD_EXTRA) |
			FIELD_PREP(CE_TDES3_CM,
				   is_decrypt ? CM_DECRYPT : CM_ENCRYPT) |
			FIELD_PREP(CE_TDES3_CT, CE_AES_GCM) |
			FIELD_PREP(CE_TDES3_PL, pl);
		pr_debug("tdes0: %08X\n", tx[cur_tx].des0);
		pr_debug("tdes1: %08X\n", tx[cur_tx].des1);
		pr_debug("tdes2: %08X\n", tx[cur_tx].des2);
		pr_debug("tdes3: %08X\n", tx[cur_tx].des3);
		cur_tx = (cur_tx + 1) % DMA_RING_SIZE;
	} else {
		//2: k0 and extra
	tx[cur_tx].des0 = ctx->key_0_phys + sizeof(ctx->key); // buffer1: k0
	tx[cur_tx].des1 = reqctx->iv_extra_tag_phys + sizeof(reqctx->iv); // buffer2: extra
	tx[cur_tx].des2 = FIELD_PREP(CE_TDES2_ED, CE_ENDIAN_BIG) |
		 FIELD_PREP(CE_TDES2_B1L, sizeof(ctx->k0)) |
		 FIELD_PREP(CE_TDES2_B2L, sizeof(reqctx->alen_dma) + sizeof(reqctx->plen_dma)) |
		 FIELD_PREP(CE_TDES2_B2T, CE_BUF_AES_AEAD_EXTRA);
	tx[cur_tx].des3 = CE_TDES3_OWN |
		 FIELD_PREP(CE_TDES3_B1T, CE_BUF_AES_AEAD_K0) |
		 FIELD_PREP(CE_TDES3_CM, is_decrypt ? CM_DECRYPT : CM_ENCRYPT) |
		 FIELD_PREP(CE_TDES3_CT, CE_AES_GCM) |
		 FIELD_PREP(CE_TDES3_PL, pl);
	pr_debug("tdes0: %08X\n", tx[cur_tx].des0);
	pr_debug("tdes1: %08X\n", tx[cur_tx].des1);
	pr_debug("tdes2: %08X\n", tx[cur_tx].des2);
	pr_debug("tdes3: %08X\n", tx[cur_tx].des3);
	cur_tx = (cur_tx + 1) % DMA_RING_SIZE;
	}

	// 3: assoc
	if (req->assoclen) {
		/* adata is typically in the first buffer, but may scatter */
		const unsigned int adata_pad_sg = adata_sgs = sg_nents_for_len(req->src, req->assoclen);

		for_each_sg (req->src, sg, adata_sgs, i) {
			if (i % 2 == 0) { /* buffer at des0 */
				tx[cur_tx].des0 = sg_dma_address(sg);

				if (sg_dma_len(sg) > alen) {
					/* this buffer contains adata and pltext */
					buf1len = alen;
				} else {
					/* this buffer contains only adata */
					buf1len = sg_dma_len(sg);
				}
				alen -= sg_dma_len(sg);
				pr_debug("tdes0: %08X\n", tx[cur_tx].des0);
			} else { /* buffer at des1 */
				unsigned int buf2len;
				if (sg_dma_len(sg) > alen) {
					/* this buffer contains adata and pltext */
					buf2len = alen;
				} else {
					/* this buffer contains only adata */
					buf2len = sg_dma_len(sg);
				}
				alen -= sg_dma_len(sg);
				tx[cur_tx].des1 = sg_dma_address(sg);
				tx[cur_tx].des2 =
					FIELD_PREP(CE_TDES2_ED, CE_ENDIAN_BIG) |
					FIELD_PREP(CE_TDES2_B2L,
						  buf2len) |
					FIELD_PREP(CE_TDES2_B2T,
						   CE_BUF_AES_AEAD_HEADER) |
					FIELD_PREP(CE_TDES2_B1L, buf1len);
				tx[cur_tx].des3 =
					CE_TDES3_OWN |
					FIELD_PREP(CE_TDES3_B1T,
						   CE_BUF_AES_AEAD_HEADER) |
					FIELD_PREP(CE_TDES3_CM,
						   is_decrypt ? CM_DECRYPT :
								CM_ENCRYPT) |
					FIELD_PREP(CE_TDES3_CT, CE_AES_GCM) |
					FIELD_PREP(CE_TDES3_PL, pl);
				pr_debug("tdes1: %08X\n", tx[cur_tx].des1);
				pr_debug("tdes2: %08X\n", tx[cur_tx].des2);
				pr_debug("tdes3: %08X\n", tx[cur_tx].des3);
				cur_tx = (cur_tx + 1) % DMA_RING_SIZE;
			}
			if (alen < 0) {
				adata_sgs--;
				break;
			}
		}
		/* fix up the last desc */
		if (adata_pad_sg % 2) {
			tx[cur_tx].des1 = priv->zero_pad_phys;
			tx[cur_tx].des2 =
					FIELD_PREP(CE_TDES2_ED, CE_ENDIAN_BIG) |
					FIELD_PREP(CE_TDES2_B2L,
						  ALIGN(req->assoclen, AES_BLOCK_SIZE) - req->assoclen) |
					FIELD_PREP(CE_TDES2_B2T,
						   CE_BUF_AES_AEAD_HEADER) |
					FIELD_PREP(CE_TDES2_B1L, buf1len);
			tx[cur_tx].des3 =
					CE_TDES3_OWN |
					FIELD_PREP(CE_TDES3_B1T,
						   CE_BUF_AES_AEAD_HEADER) |
					FIELD_PREP(CE_TDES3_CM,
						   is_decrypt ? CM_DECRYPT :
								CM_ENCRYPT) |
					FIELD_PREP(CE_TDES3_CT, CE_AES_GCM) |
					FIELD_PREP(CE_TDES3_PL, pl);
			pr_debug("tdes1: %08X\n", tx[cur_tx].des1);
			pr_debug("tdes2: %08X\n", tx[cur_tx].des2);
			pr_debug("tdes3: %08X\n", tx[cur_tx].des3);
			cur_tx = (cur_tx + 1) % DMA_RING_SIZE;
		} else if (ALIGN(req->assoclen, AES_BLOCK_SIZE) - req->assoclen) {
			tx[cur_tx].des0 = priv->zero_pad_phys;
			tx[cur_tx].des1 = 0;
			tx[cur_tx].des2 =
				FIELD_PREP(CE_TDES2_ED, CE_ENDIAN_BIG) |
				FIELD_PREP(CE_TDES2_B1L, ALIGN(req->assoclen,
							       AES_BLOCK_SIZE) -
								 req->assoclen) |
					FIELD_PREP(CE_TDES2_B2T,
						   CE_BUF_AES_AEAD_HEADER);
			tx[cur_tx].des3 = CE_TDES3_OWN |
					  FIELD_PREP(CE_TDES3_B1T,
						     CE_BUF_AES_AEAD_HEADER) |
					  FIELD_PREP(CE_TDES3_CM,
						     is_decrypt ? CM_DECRYPT :
								  CM_ENCRYPT) |
					  FIELD_PREP(CE_TDES3_CT, CE_AES_GCM) |
					  FIELD_PREP(CE_TDES3_PL, pl);

			pr_debug("tdes0: %08X\n", tx[cur_tx].des0);
			pr_debug("tdes1: %08X\n", tx[cur_tx].des1);
			pr_debug("tdes2: %08X\n", tx[cur_tx].des2);
			pr_debug("tdes3: %08X\n", tx[cur_tx].des3);
			cur_tx = (cur_tx + 1) % DMA_RING_SIZE;
		}
	} else {
		sg = req->src;
		adata_sgs = 0;
	}

	/* after adata is set, alen may be negative if there is a buffer which contains both adata and plen
	*/
	for_each_sg(sg, sg, reqctx->ssg_len - adata_sgs, i) {
		int ld = (i == reqctx->ssg_len - adata_sgs - 1);
		if (i % 2 == 0) { /* buffer at des0 */
			if (alen < 0) {
				tx[cur_tx].des0 = sg_dma_address(sg)+sg_dma_len(sg)+alen;
				/* the last buffer may contain more bytes than we want,
				 * make sure to limit it */
				buf1len = -alen - (ld ? src_extra_bytes : 0);
				alen = 0;
			} else {
				tx[cur_tx].des0 = sg_dma_address(sg);
				/* the last buffer may contain more bytes than we want,
				 * make sure to limit it */
				buf1len = sg_dma_len(sg) - (ld ? src_extra_bytes : 0);
			}
			pr_debug("tdes0: %08X\n", tx[cur_tx].des0);
		} else { /* buffer at des1 */
			tx[cur_tx].des1 = sg_dma_address(sg);
			tx[cur_tx].des2 = FIELD_PREP(CE_TDES2_ED, CE_ENDIAN_BIG) |
				FIELD_PREP(CE_TDES2_B2L, ld ? sg_dma_len(sg) - src_extra_bytes : sg_dma_len(sg)) |
				FIELD_PREP(CE_TDES2_B2T, CE_BUF_PAYLOAD) |
				FIELD_PREP(CE_TDES2_B1L, buf1len);
			tx[cur_tx].des3 =
				CE_TDES3_OWN | FIELD_PREP(CE_TDES3_LD, ld) |
				FIELD_PREP(CE_TDES3_B1T, CE_BUF_PAYLOAD) |
				FIELD_PREP(CE_TDES3_CM, is_decrypt ?
								CM_DECRYPT :
								CM_ENCRYPT) |
				FIELD_PREP(CE_TDES3_CT, CE_AES_GCM) |
				FIELD_PREP(CE_TDES3_PL, pl);
			pr_debug("tdes1: %08X\n", tx[cur_tx].des1);
			pr_debug("tdes2: %08X\n", tx[cur_tx].des2);
			pr_debug("tdes3: %08X\n", tx[cur_tx].des3);
			cur_tx = (cur_tx + 1) % DMA_RING_SIZE;
		}
	}

	/* fix up the last desc */
	if (i % 2) {
		tx[cur_tx].des1 = 0;
		tx[cur_tx].des2 = FIELD_PREP(CE_TDES2_ED, CE_ENDIAN_BIG) | FIELD_PREP(CE_TDES2_B2T, CE_BUF_PAYLOAD) | FIELD_PREP(CE_TDES2_B1L, buf1len);
		tx[cur_tx].des3 =
			CE_TDES3_OWN | CE_TDES3_LD |
			FIELD_PREP(CE_TDES3_B1T, CE_BUF_PAYLOAD) |
			FIELD_PREP(CE_TDES3_CM,
				   is_decrypt ? CM_DECRYPT : CM_ENCRYPT) |
			FIELD_PREP(CE_TDES3_CT, CE_AES_GCM) |
			FIELD_PREP(CE_TDES3_PL, pl);
		pr_debug("tdes1: %08X\n", tx[cur_tx].des1);
		pr_debug("tdes2: %08X\n", tx[cur_tx].des2);
		pr_debug("tdes3: %08X\n", tx[cur_tx].des3);
		cur_tx = (cur_tx + 1) % DMA_RING_SIZE;
	}
	ch->cur_tx = cur_tx;

	rx = ch->dma_rx;
	cur_rx = ch->cur_rx;
	// prepare rx desc

	if (reqctx->tmp_buf_phys) {
		/* one large rx buffer for tmp buffer */
		rx[cur_rx].des0 = reqctx->tmp_buf_phys;
		rx[cur_rx].des1 = 0;
		rx[cur_rx].des2 = FIELD_PREP(CE_RDES2_B1L, ALIGN(ctext_and_tag_len, ARCH_DMA_MINALIGN));
		rx[cur_rx].des3 = CE_RDES3_OWN | CE_RDES3_IOC | FIELD_PREP(CE_RDES3_TL, 16); // hw bug: rx taglen must be set to 16 on aead.
		pr_debug("rdes0: %08X\n", rx[cur_rx].des0);
		pr_debug("rdes1: %08X\n", rx[cur_rx].des1);
		pr_debug("rdes2: %08X\n", rx[cur_rx].des2);
		pr_debug("rdes3: %08X\n", rx[cur_rx].des3);
		/* link this request to the rx descriptor */
		ch->areqs[cur_rx] = &req->base;
		cur_rx = (cur_rx + 1) % DMA_RING_SIZE;
	} else {
		// merge every two buffers into one dma desc list
		for_each_sg (dsg_without_adata, sg, dsg_nents_without_adata, i) {
			int ld = (i == dsg_nents_without_adata - 1);

			if (i % 2 == 0) {
				rx[cur_rx].des0 = sg_dma_address(sg);
				buf1len = sg_dma_len(sg);
				pr_debug("rdes0: %08X\n", rx[cur_rx].des0);
			} else {
				rx[cur_rx].des1 = sg_dma_address(sg);
				rx[cur_rx].des2 =
					FIELD_PREP(CE_RDES2_B1L, buf1len) |
					FIELD_PREP(CE_RDES2_B2L,
						   sg_dma_len(sg));
				rx[cur_rx].des3 = CE_RDES3_OWN |
						  FIELD_PREP(CE_RDES3_IOC, ld) | FIELD_PREP(CE_RDES3_TL, 16); // hw bug: rx taglen must be set to 16 on aead.
				if (ld) {
					/* link this request to the rx descriptor */
					ch->areqs[cur_rx] = &req->base;
				}
				pr_debug("rdes1: %08X\n", rx[cur_rx].des1);
				pr_debug("rdes2: %08X\n", rx[cur_rx].des2);
				pr_debug("rdes3: %08X\n", rx[cur_rx].des3);
				cur_rx = (cur_rx + 1) % DMA_RING_SIZE;
			}
		}
		if (i % 2) {
			rx[cur_rx].des1 = 0;
			rx[cur_rx].des2 = FIELD_PREP(CE_RDES2_B1L, buf1len);
			rx[cur_rx].des3 = CE_RDES3_OWN | CE_RDES3_IOC | FIELD_PREP(CE_RDES3_TL, 16); // hw bug: rx taglen must be set to 16 on aead.
			/* link this request to the rx descriptor */
			ch->areqs[cur_rx] = &req->base;
			pr_debug("rdes1: %08X\n", rx[cur_rx].des1);
			pr_debug("rdes2: %08X\n", rx[cur_rx].des2);
			pr_debug("rdes3: %08X\n", rx[cur_rx].des3);
			cur_rx = (cur_rx + 1) % DMA_RING_SIZE;
		}
	}

	ch->cur_rx = cur_rx;
#ifdef CONFIG_CRYPTO_SIFLOWER_HASH
	spin_unlock_bh(&ch->ring_lock);
#else
	local_bh_enable();
#endif
	reg_write_flush(priv, CE_DMA_CH_RxDESC_TAIL_LPTR(ch->ch_num), ch->dma_rx_phy + sizeof(struct sf_ce_desc) * (cur_rx ?: DMA_RING_SIZE));
	reg_write(priv, CE_DMA_CH_TxDESC_TAIL_LPTR(ch->ch_num), ch->dma_tx_phy + sizeof(struct sf_ce_desc) * (cur_tx ?: DMA_RING_SIZE));

	return -EINPROGRESS;
}

static int sf_ce_aes_gcm_enc(struct aead_request *req)
{
	return sf_ce_aes_gcm_op(req, false);
}

static int sf_ce_aes_gcm_dec(struct aead_request *req)
{
	return sf_ce_aes_gcm_op(req, true);
}

struct aead_alg sf_ce_aes_gcm __read_mostly = {
	.setkey	= sf_ce_aes_gcm_setkey,
	.setauthsize = sf_ce_aes_gcm_setauthsize,
	.encrypt = sf_ce_aes_gcm_enc,
	.decrypt = sf_ce_aes_gcm_dec,
	.ivsize	= GCM_AES_IV_SIZE,
	.maxauthsize = AES_BLOCK_SIZE,
	.base = {
		.cra_name		= "gcm(aes)",
		.cra_driver_name	= "siflower-ce-aes-gcm",
		.cra_priority		= 300,
		.cra_blocksize		= 1,
		.cra_ctxsize		= sizeof(struct sf_ce_aes_gcm_ctx),
		.cra_flags		= CRYPTO_ALG_ASYNC |
					  CRYPTO_ALG_KERN_DRIVER_ONLY |
					  CRYPTO_ALG_NEED_FALLBACK,
		.cra_module		= THIS_MODULE,
		.cra_init		= sf_ce_aes_gcm_cra_init,
		.cra_exit		= sf_ce_aes_gcm_cra_exit,
	},
};

/* taken from crypto/ccm.c */
static int set_msg_len(u8 *block, unsigned int msglen, int csize)
{
	__be32 data;

	memset(block, 0, csize);
	block += csize;

	if (csize >= 4)
		csize = 4;
	else if (msglen > (unsigned int)(1 << (8 * csize)))
		return -EOVERFLOW;

	data = cpu_to_be32(msglen);
	memcpy(block - csize, (u8 *)&data + 4 - csize, csize);

	return 0;
}

/* based on code from crypto/ccm.c */
static int generate_b0(const u8 *iv, unsigned int assoclen, unsigned int authsize,
		       unsigned int plen, u8 *b0)
{
	unsigned int l, lp, m = authsize;
	int rc;

	memcpy(b0, iv, AES_BLOCK_SIZE);

	lp = b0[0];
	l = lp + 1;

	/* set m, bits 3-5 */
	*b0 |= (8 * ((m - 2) / 2));

	/* set adata, bit 6, if associated data is used */
	if (assoclen)
		*b0 |= BIT(6);

	rc = set_msg_len(b0 + AES_BLOCK_SIZE - l, plen, l);

	return rc;
}

static int sf_ce_aes_ccm_cra_init(struct crypto_tfm *tfm)
{
	struct sf_ce_aes_ccm_ctx *ctx = crypto_tfm_ctx(tfm);
	struct sf_ce_dev *priv = g_dev;
	struct device *dev = priv->dev;
	int ret;

	/* Allocate fallback implementation */
	ctx->fback = crypto_alloc_aead(crypto_tfm_alg_name(tfm), 0, CRYPTO_ALG_NEED_FALLBACK);
	if (IS_ERR(ctx->fback))
		return PTR_ERR(ctx->fback);
	crypto_aead_set_reqsize(__crypto_aead_cast(tfm),
				max(sizeof(struct sf_ce_aes_ccm_reqctx), sizeof(struct aead_request) + crypto_aead_reqsize(ctx->fback)));
	ctx->priv = g_dev;
	ctx->key_0_phys = dma_map_single_attrs(dev, ctx->key, sizeof(ctx->key),
					       DMA_TO_DEVICE, DMA_ATTR_SKIP_CPU_SYNC);
	ret = dma_mapping_error(dev, ctx->key_0_phys);
	if (ret)
		crypto_free_aead(ctx->fback);
	return ret;
}

static void
sf_ce_aes_ccm_cra_exit(struct crypto_tfm *tfm)
{
	struct sf_ce_aes_ccm_ctx *ctx = crypto_tfm_ctx(tfm);
	struct device *dev = ctx->priv->dev;

	dma_unmap_single_attrs(dev, ctx->key_0_phys, sizeof(ctx->key), DMA_TO_DEVICE, DMA_ATTR_SKIP_CPU_SYNC);
	crypto_free_aead(ctx->fback);
}

static int
sf_ce_aes_ccm_setkey(struct crypto_aead *tfm, const u8 *key,
		     unsigned int keylen)
{
	struct sf_ce_aes_ccm_ctx *ctx = crypto_aead_ctx(tfm);
	struct device *dev = ctx->priv->dev;
	int ret;

	ret = crypto_aead_setkey(ctx->fback, key, keylen);
	if (ret)
		return ret;

	ctx->keylen = keylen;
	memcpy(ctx->key, key, keylen);
	dma_sync_single_for_device(dev, ctx->key_0_phys, sizeof(ctx->key), DMA_TO_DEVICE);
	return 0;
}

static int
sf_ce_aes_ccm_setauthsize(struct crypto_aead *tfm, unsigned int authsize)
{
	struct sf_ce_aes_ccm_ctx *ctx = crypto_aead_ctx(tfm);

	return crypto_aead_setauthsize(ctx->fback, authsize);
}

static int
sf_ce_aes_ccm_do_fallback(struct aead_request *req, bool is_decrypt)
{
	struct aead_request *subreq = aead_request_ctx(req);
	struct crypto_aead *aead = crypto_aead_reqtfm(req);
	struct sf_ce_aes_ccm_ctx *ctx = crypto_aead_ctx(aead);

	aead_request_set_tfm(subreq, ctx->fback);
	aead_request_set_callback(subreq, req->base.flags,
				  req->base.complete,
				  req->base.data);
	aead_request_set_crypt(subreq, req->src, req->dst,
			       req->cryptlen, req->iv);
	aead_request_set_ad(subreq, req->assoclen);

	return is_decrypt ? crypto_aead_decrypt(subreq) : crypto_aead_encrypt(subreq);
}

static inline int crypto_ccm_check_iv(const u8 *iv)
{
	/* 2 <= L <= 8, so 1 <= L' <= 7. */
	if (1 > iv[0] || iv[0] > 7)
		return -EINVAL;

	return 0;
}

static int
sf_ce_aes_ccm_op(struct aead_request *req, bool is_decrypt)
{
	struct sf_ce_aes_ccm_reqctx *reqctx = aead_request_ctx(req);
	struct crypto_aead *aead = crypto_aead_reqtfm(req);
	struct sf_ce_aes_ccm_ctx *ctx = crypto_aead_ctx(aead);
	struct sf_ce_dev *priv = ctx->priv;
	struct device *dev = priv->dev;
	struct sf_ce_chan *ch = NULL;
	dma_addr_t iv_extra_tag_phys;
	int ret;
	struct sf_ce_desc *rx, *tx;
	unsigned int cur_tx, cur_rx;
	struct scatterlist *sg = NULL, *dsg_without_adata = NULL, __dsg_without_adata[2];
	unsigned int i, buf1len = 0, pl;
	unsigned int adata_sgs = 0;
	u64 src_extra_bytes = 0, dst_extra_bytes = 0;
	int alen = req->assoclen;
	int dsg_nents_without_adata = 0;
	const unsigned int ptext_len = req->cryptlen - (is_decrypt ? crypto_aead_authsize(aead) : 0);
	const unsigned int ctext_and_tag_len = req->cryptlen + (is_decrypt ? 0 : crypto_aead_authsize(aead));

	if (unlikely(req->assoclen + ctext_and_tag_len > SF_AES_MAX_LEN))
		return sf_ce_aes_ccm_do_fallback(req, is_decrypt);
#ifdef SW_FALLBACK_THRES
	if (unlikely(ptext_len < max(SW_FALLBACK_THRES, SF_AES_GCM_MIN_PTEXT_LEN))) {
		/* fallback to software if len is too small */
		return sf_ce_aes_ccm_do_fallback(req, is_decrypt);
	}
#else
	/* hw mininum acceptable ptext len */
	if (unlikely(ptext_len < SF_AES_GCM_MIN_PTEXT_LEN)) {
		return sf_ce_aes_ccm_do_fallback(req, is_decrypt);
	}
#endif
	if (unlikely(crypto_aead_authsize(aead) != 16)) {
		/* TODO: handle uncommon tag size (hw always output 16 bytes, need to be trimmed by sw) */
		return sf_ce_aes_ccm_do_fallback(req, is_decrypt);
	}
	pr_debug("%s\n", is_decrypt ? "decrypt" : "encrypt");

	ret = crypto_ccm_check_iv(req->iv);
	if (ret)
		return ret;

	/* Build B0 */
	ret = generate_b0(req->iv, req->assoclen, crypto_aead_authsize(aead), ptext_len, reqctx->b0);
	if (ret)
		return ret;

	reqctx->tmp_buf = NULL;
	reqctx->tmp_buf_phys = 0;

	/* req->iv may not be physically contiguous, so copy it to reqctx,
	 * which is contiguous and can be used for DMA.
	 */
	memcpy(reqctx->iv, req->iv, AES_BLOCK_SIZE);
	/* the counter starts at 0 regardless of what the caller left there (as crypto/ccm.c) */
	memset(reqctx->iv + 15 - reqctx->iv[0], 0, reqctx->iv[0] + 1);
	reqctx->alen_dma = cpu_to_be16(req->assoclen);
	dsg_without_adata = scatterwalk_ffwd(__dsg_without_adata, req->dst, req->assoclen);
	dsg_nents_without_adata = sg_nents(dsg_without_adata);
	BUILD_BUG_ON(&reqctx->b0[AES_BLOCK_SIZE] != (u8*)&reqctx->alen_dma);
	pr_debug("assoclen: %u, cryptlen: %u, taglen: %u\n", req->assoclen, req->cryptlen, crypto_aead_authsize(aead));
	if (is_decrypt) {
		ret = sf_ce_sg_nents_for_len(req->src, ptext_len + req->assoclen,
					     &src_extra_bytes);
		if (ret <= 0)
			return ret;
		reqctx->ssg_len = ret;
		ret = sf_ce_sg_nents_for_len(req->dst, ptext_len + req->assoclen,
					     &dst_extra_bytes);
		if (ret <= 0)
			return ret;
		reqctx->dsg_len = ret;
	} else {
		ret = sf_ce_sg_nents_for_len(req->src, ptext_len + req->assoclen,
					     &src_extra_bytes);
		if (ret <= 0)
			return ret;
		reqctx->ssg_len = ret;
		ret = sf_ce_sg_nents_for_len(req->dst, ctext_and_tag_len + req->assoclen,
					     &dst_extra_bytes);
		if (ret <= 0)
			return ret;
		reqctx->dsg_len = ret;
	}
	pr_debug("reqctx->ssg_len: %u, reqctx->dsg_len %u\n", reqctx->dsg_len, reqctx->ssg_len);
	if (req->assoclen)
		pr_debug("assoclen: %u\n", req->assoclen);
	if (src_extra_bytes)
		pr_debug("src_extra_bytes: %lld\n", src_extra_bytes);
	if (dst_extra_bytes)
		pr_debug("dst_extra_bytes: %lld\n", dst_extra_bytes);

	if (is_decrypt) {
		reqctx->taglen_dma = cpu_to_be64(crypto_aead_authsize(aead));
		scatterwalk_map_and_copy(reqctx->tag, req->src, req->assoclen + ptext_len, crypto_aead_authsize(aead), 0);
	}
	if (sf_ce_need_bounce_buffer(dsg_without_adata, dsg_nents_without_adata, ARCH_DMA_MINALIGN)) {
		void *tmp_buf;
		gfp_t flags;

		if (unlikely(reqctx->ssg_len != dma_map_sg(dev, req->src,
							   reqctx->ssg_len,
							   DMA_TO_DEVICE)))
			return -ENOMEM;

		flags = (req->base.flags & CRYPTO_TFM_REQ_MAY_SLEEP) ?
			GFP_KERNEL : GFP_ATOMIC;
		tmp_buf = kmalloc(ALIGN(ctext_and_tag_len, DMA_RX_ALIGN), flags);
		if (!tmp_buf)
			return -ENOMEM;

		reqctx->tmp_buf = tmp_buf;
		reqctx->tmp_buf_phys = dma_map_single(dev, tmp_buf, ALIGN(ctext_and_tag_len, DMA_RX_ALIGN), DMA_FROM_DEVICE);
	} else if (req->src == req->dst) {
		reqctx->dsg_len = reqctx->ssg_len;

		if (unlikely(reqctx->ssg_len != dma_map_sg(dev, req->src,
							   reqctx->ssg_len,
							   DMA_BIDIRECTIONAL)))
			return -ENOMEM;
	} else {
		if (unlikely(reqctx->ssg_len != dma_map_sg(dev, req->src,
							   reqctx->ssg_len,
							   DMA_TO_DEVICE)))
			return -ENOMEM;

		if (unlikely(reqctx->dsg_len != dma_map_sg(dev, req->dst,
							   reqctx->dsg_len,
							   DMA_FROM_DEVICE)))
			return -ENOMEM;
	}
	dma_map_sg_attrs(dev, dsg_without_adata, dsg_nents_without_adata, DMA_TO_DEVICE, DMA_ATTR_SKIP_CPU_SYNC);

	iv_extra_tag_phys = dma_map_single(dev, reqctx->iv, sizeof(reqctx->iv) + sizeof(reqctx->b0) + sizeof(reqctx->alen_dma) + sizeof(reqctx->tag) + sizeof(reqctx->taglen_dma), DMA_TO_DEVICE);
	if (unlikely((ret = dma_mapping_error(dev, iv_extra_tag_phys))))
		return ret;

	reqctx->iv_extra_tag_phys = iv_extra_tag_phys;
	pl = req->cryptlen + req->assoclen;

#ifdef CONFIG_CRYPTO_SIFLOWER_HASH
	ch = &priv->chan[DMA_CH_AES];
	spin_lock_bh(&ch->ring_lock);
#else
	local_bh_disable();
	ch = &priv->chan[smp_processor_id()];
#endif
	if (unlikely(sf_ce_dma_tx_avail(ch) < DIV_ROUND_UP(reqctx->ssg_len, 2) + 1)) {
		kfree(reqctx->tmp_buf);
#ifdef CONFIG_CRYPTO_SIFLOWER_HASH
		spin_unlock_bh(&ch->ring_lock);
#else
		local_bh_enable();
#endif
		pr_err_ratelimited("tx ring full\n");
		return sf_ce_aes_ccm_do_fallback(req, is_decrypt);
	}
	tx = ch->dma_tx;
	cur_tx = ch->cur_tx;

	//1: key and iv
	tx[cur_tx].des0 = ctx->key_0_phys; // buffer1: key
	tx[cur_tx].des1 = reqctx->iv_extra_tag_phys; // buffer2: iv
	tx[cur_tx].des2 = FIELD_PREP(CE_TDES2_ED, CE_ENDIAN_BIG) |
		 FIELD_PREP(CE_TDES2_B1L, ctx->keylen) |
		 FIELD_PREP(CE_TDES2_B2L, AES_BLOCK_SIZE) |
		 FIELD_PREP(CE_TDES2_B2T, CE_BUF_AES_IV);
	tx[cur_tx].des3 = CE_TDES3_OWN | CE_TDES3_FD |
		 FIELD_PREP(CE_TDES3_B1T, CE_BUF_AES_KEY) |
		 FIELD_PREP(CE_TDES3_CM, is_decrypt ? CM_DECRYPT : CM_ENCRYPT) |
		 FIELD_PREP(CE_TDES3_CT, CE_AES_CCM) |
		 FIELD_PREP(CE_TDES3_PL, pl);
				pr_debug("tdes0: %08X\n", tx[cur_tx].des0);
				pr_debug("tdes1: %08X\n", tx[cur_tx].des1);
				pr_debug("tdes2: %08X\n", tx[cur_tx].des2);
				pr_debug("tdes3: %08X\n", tx[cur_tx].des3);
	cur_tx = (cur_tx + 1) % DMA_RING_SIZE;

	if (is_decrypt) {
		// prepare tagparams
		tx[cur_tx].des0 = reqctx->iv_extra_tag_phys + offsetof(struct sf_ce_aes_ccm_reqctx, taglen_dma);
		tx[cur_tx].des1 = reqctx->iv_extra_tag_phys + offsetof(struct sf_ce_aes_ccm_reqctx, tag);
		tx[cur_tx].des2 =
			FIELD_PREP(CE_TDES2_ED, CE_ENDIAN_BIG) |
			FIELD_PREP(CE_TDES2_B1L, sizeof(reqctx->taglen_dma)) |
			FIELD_PREP(CE_TDES2_B2L, sizeof(reqctx->tag)) |
			FIELD_PREP(CE_TDES2_B2T, CE_BUF_AES_AEAD_TAG);
		tx[cur_tx].des3 =
			CE_TDES3_OWN |
			FIELD_PREP(CE_TDES3_B1T, CE_BUF_AES_AEAD_TAG) |
			FIELD_PREP(CE_TDES3_CM,
				   is_decrypt ? CM_DECRYPT : CM_ENCRYPT) |
			FIELD_PREP(CE_TDES3_CT, CE_AES_CCM) |
			FIELD_PREP(CE_TDES3_PL, pl);
				pr_debug("tdes0: %08X\n", tx[cur_tx].des0);
				pr_debug("tdes1: %08X\n", tx[cur_tx].des1);
				pr_debug("tdes2: %08X\n", tx[cur_tx].des2);
				pr_debug("tdes3: %08X\n", tx[cur_tx].des3);
		cur_tx = (cur_tx + 1) % DMA_RING_SIZE;
	}
	//2: extra
	tx[cur_tx].des0 = reqctx->iv_extra_tag_phys + offsetof(struct sf_ce_aes_ccm_reqctx, b0); // buffer1: extra
	tx[cur_tx].des1 = 0;
	tx[cur_tx].des2 = FIELD_PREP(CE_TDES2_ED, CE_ENDIAN_BIG) |
		 FIELD_PREP(CE_TDES2_B1L, sizeof(reqctx->b0) + (req->assoclen ? sizeof(reqctx->alen_dma) : 0)) |
		 FIELD_PREP(CE_TDES2_B2T, CE_BUF_AES_AEAD_EXTRA);
	tx[cur_tx].des3 = CE_TDES3_OWN |
		 FIELD_PREP(CE_TDES3_B1T, CE_BUF_AES_AEAD_EXTRA) |
		 FIELD_PREP(CE_TDES3_CM, is_decrypt ? CM_DECRYPT : CM_ENCRYPT) |
		 FIELD_PREP(CE_TDES3_CT, CE_AES_CCM) |
		 FIELD_PREP(CE_TDES3_PL, pl);
				pr_debug("tdes0: %08X\n", tx[cur_tx].des0);
				pr_debug("tdes1: %08X\n", tx[cur_tx].des1);
				pr_debug("tdes2: %08X\n", tx[cur_tx].des2);
				pr_debug("tdes3: %08X\n", tx[cur_tx].des3);
	cur_tx = (cur_tx + 1) % DMA_RING_SIZE;

	// 3: assoc
	if (req->assoclen) {
		/* adata is typically in the first buffer, but may scatter */
		const unsigned int adata_pad_sg = adata_sgs = sg_nents_for_len(req->src, req->assoclen);

		for_each_sg (req->src, sg, adata_sgs, i) {
			if (i % 2 == 0) { /* buffer at des0 */
				tx[cur_tx].des0 = sg_dma_address(sg);

				if (sg_dma_len(sg) > alen) {
					/* this buffer contains adata and pltext */
					buf1len = alen;
				} else {
					/* this buffer contains only adata */
					buf1len = sg_dma_len(sg);
				}
				alen -= sg_dma_len(sg);
				pr_debug("tdes0: %08X\n", tx[cur_tx].des0);
			} else { /* buffer at des1 */
				unsigned int buf2len;
				if (sg_dma_len(sg) > alen) {
					/* this buffer contains adata and pltext */
					buf2len = alen;
				} else {
					/* this buffer contains only adata */
					buf2len = sg_dma_len(sg);
				}
				alen -= sg_dma_len(sg);
				tx[cur_tx].des1 = sg_dma_address(sg);
				tx[cur_tx].des2 =
					FIELD_PREP(CE_TDES2_ED, CE_ENDIAN_BIG) |
					FIELD_PREP(CE_TDES2_B2L,
						  buf2len) |
					FIELD_PREP(CE_TDES2_B2T,
						   CE_BUF_AES_AEAD_HEADER) |
					FIELD_PREP(CE_TDES2_B1L, buf1len);
				tx[cur_tx].des3 =
					CE_TDES3_OWN |
					FIELD_PREP(CE_TDES3_B1T,
						   CE_BUF_AES_AEAD_HEADER) |
					FIELD_PREP(CE_TDES3_CM,
						   is_decrypt ? CM_DECRYPT :
								CM_ENCRYPT) |
					FIELD_PREP(CE_TDES3_CT, CE_AES_CCM) |
					FIELD_PREP(CE_TDES3_PL, pl);
				pr_debug("tdes1: %08X\n", tx[cur_tx].des1);
				pr_debug("tdes2: %08X\n", tx[cur_tx].des2);
				pr_debug("tdes3: %08X\n", tx[cur_tx].des3);
				cur_tx = (cur_tx + 1) % DMA_RING_SIZE;
			}
			if (alen < 0) {
				adata_sgs--;
				break;
			}
		}
		/* fix up the last desc */
		if (adata_pad_sg % 2) {
			tx[cur_tx].des1 = priv->zero_pad_phys;
			tx[cur_tx].des2 =
					FIELD_PREP(CE_TDES2_ED, CE_ENDIAN_BIG) |
					FIELD_PREP(CE_TDES2_B2L,
						  ALIGN(2 + req->assoclen, AES_BLOCK_SIZE) - (2 + req->assoclen)) |
					FIELD_PREP(CE_TDES2_B2T,
						   CE_BUF_AES_AEAD_HEADER) |
					FIELD_PREP(CE_TDES2_B1L, buf1len);
			tx[cur_tx].des3 =
					CE_TDES3_OWN |
					FIELD_PREP(CE_TDES3_B1T,
						   CE_BUF_AES_AEAD_HEADER) |
					FIELD_PREP(CE_TDES3_CM,
						   is_decrypt ? CM_DECRYPT :
								CM_ENCRYPT) |
					FIELD_PREP(CE_TDES3_CT, CE_AES_CCM) |
					FIELD_PREP(CE_TDES3_PL, pl);
			pr_debug("tdes1: %08X\n", tx[cur_tx].des1);
			pr_debug("tdes2: %08X\n", tx[cur_tx].des2);
			pr_debug("tdes3: %08X\n", tx[cur_tx].des3);
			cur_tx = (cur_tx + 1) % DMA_RING_SIZE;
		} else if (ALIGN(2 + req->assoclen, AES_BLOCK_SIZE) - (2 + req->assoclen)) {
			tx[cur_tx].des0 = priv->zero_pad_phys;
			tx[cur_tx].des1 = 0;
			tx[cur_tx].des2 =
				FIELD_PREP(CE_TDES2_ED, CE_ENDIAN_BIG) |
				FIELD_PREP(CE_TDES2_B1L, ALIGN(2 + req->assoclen,
							       AES_BLOCK_SIZE) -
								 (2 + req->assoclen)) |
					FIELD_PREP(CE_TDES2_B2T,
						   CE_BUF_AES_AEAD_HEADER);
			tx[cur_tx].des3 = CE_TDES3_OWN |
					  FIELD_PREP(CE_TDES3_B1T,
						     CE_BUF_AES_AEAD_HEADER) |
					  FIELD_PREP(CE_TDES3_CM,
						     is_decrypt ? CM_DECRYPT :
								  CM_ENCRYPT) |
					  FIELD_PREP(CE_TDES3_CT, CE_AES_CCM) |
					  FIELD_PREP(CE_TDES3_PL, pl);

			pr_debug("tdes0: %08X\n", tx[cur_tx].des0);
			pr_debug("tdes1: %08X\n", tx[cur_tx].des1);
			pr_debug("tdes2: %08X\n", tx[cur_tx].des2);
			pr_debug("tdes3: %08X\n", tx[cur_tx].des3);
			cur_tx = (cur_tx + 1) % DMA_RING_SIZE;
		}
	} else {
		sg = req->src;
		adata_sgs = 0;
	}

	/* after adata is set, alen may be negative if there is a buffer which contains both adata and plen
	*/
	for_each_sg(sg, sg, reqctx->ssg_len - adata_sgs, i) {
		int ld = (i == reqctx->ssg_len - adata_sgs - 1);
		if (i % 2 == 0) { /* buffer at des0 */
			if (alen < 0) {
				tx[cur_tx].des0 = sg_dma_address(sg)+sg_dma_len(sg)+alen;
				/* the last buffer may contain more bytes than we want,
				 * make sure to limit it */
				buf1len = -alen - (ld ? src_extra_bytes : 0);
				alen = 0;
			} else {
				tx[cur_tx].des0 = sg_dma_address(sg);
				/* the last buffer may contain more bytes than we want,
				 * make sure to limit it */
				buf1len = sg_dma_len(sg) - (ld ? src_extra_bytes : 0);
			}
			pr_debug("tdes0: %08X\n", tx[cur_tx].des0);
		} else { /* buffer at des1 */
			tx[cur_tx].des1 = sg_dma_address(sg);
			tx[cur_tx].des2 = FIELD_PREP(CE_TDES2_ED, CE_ENDIAN_BIG) |
				FIELD_PREP(CE_TDES2_B2L, ld ? sg_dma_len(sg) - src_extra_bytes : sg_dma_len(sg)) |
				FIELD_PREP(CE_TDES2_B2T, CE_BUF_PAYLOAD) |
				FIELD_PREP(CE_TDES2_B1L, buf1len);
			tx[cur_tx].des3 =
				CE_TDES3_OWN | FIELD_PREP(CE_TDES3_LD, ld) |
				FIELD_PREP(CE_TDES3_B1T, CE_BUF_PAYLOAD) |
				FIELD_PREP(CE_TDES3_CM, is_decrypt ?
								CM_DECRYPT :
								CM_ENCRYPT) |
				FIELD_PREP(CE_TDES3_CT, CE_AES_CCM) |
				FIELD_PREP(CE_TDES3_PL, pl);
			pr_debug("tdes1: %08X\n", tx[cur_tx].des1);
			pr_debug("tdes2: %08X\n", tx[cur_tx].des2);
			pr_debug("tdes3: %08X\n", tx[cur_tx].des3);
			cur_tx = (cur_tx + 1) % DMA_RING_SIZE;
		}
	}

	/* fix up the last desc */
	if (i % 2) {
		tx[cur_tx].des1 = 0;
		tx[cur_tx].des2 = FIELD_PREP(CE_TDES2_ED, CE_ENDIAN_BIG) | FIELD_PREP(CE_TDES2_B2T, CE_BUF_PAYLOAD) | FIELD_PREP(CE_TDES2_B1L, buf1len);
		tx[cur_tx].des3 =
			CE_TDES3_OWN | CE_TDES3_LD |
			FIELD_PREP(CE_TDES3_B1T, CE_BUF_PAYLOAD) |
			FIELD_PREP(CE_TDES3_CM,
				   is_decrypt ? CM_DECRYPT : CM_ENCRYPT) |
			FIELD_PREP(CE_TDES3_CT, CE_AES_CCM) |
			FIELD_PREP(CE_TDES3_PL, pl);
		pr_debug("tdes1: %08X\n", tx[cur_tx].des1);
		pr_debug("tdes2: %08X\n", tx[cur_tx].des2);
		pr_debug("tdes3: %08X\n", tx[cur_tx].des3);
		cur_tx = (cur_tx + 1) % DMA_RING_SIZE;
	}
	ch->cur_tx = cur_tx;

	rx = ch->dma_rx;
	cur_rx = ch->cur_rx;
	// prepare rx desc

	if (reqctx->tmp_buf_phys) {
		/* one large rx buffer for tmp buffer */
		rx[cur_rx].des0 = reqctx->tmp_buf_phys;
		rx[cur_rx].des1 = 0;
		rx[cur_rx].des2 = FIELD_PREP(CE_RDES2_B1L, ALIGN(ctext_and_tag_len, ARCH_DMA_MINALIGN));
		rx[cur_rx].des3 = CE_RDES3_OWN | CE_RDES3_IOC | FIELD_PREP(CE_RDES3_TL, 16); // hw bug: rx taglen must be set to 16 on aead.
		pr_debug("rdes0: %08X\n", rx[cur_rx].des0);
		pr_debug("rdes1: %08X\n", rx[cur_rx].des1);
		pr_debug("rdes2: %08X\n", rx[cur_rx].des2);
		pr_debug("rdes3: %08X\n", rx[cur_rx].des3);
		/* link this request to the rx descriptor */
		ch->areqs[cur_rx] = &req->base;
		cur_rx = (cur_rx + 1) % DMA_RING_SIZE;
	} else {
		// merge every two buffers into one dma desc list
		for_each_sg (dsg_without_adata, sg, dsg_nents_without_adata, i) {
			int ld = (i == dsg_nents_without_adata - 1);

			if (i % 2 == 0) {
				rx[cur_rx].des0 = sg_dma_address(sg);
				buf1len = sg_dma_len(sg);
				pr_debug("rdes0: %08X\n", rx[cur_rx].des0);
			} else {
				rx[cur_rx].des1 = sg_dma_address(sg);
				rx[cur_rx].des2 =
					FIELD_PREP(CE_RDES2_B1L, buf1len) |
					FIELD_PREP(CE_RDES2_B2L,
						   sg_dma_len(sg));
				rx[cur_rx].des3 = CE_RDES3_OWN |
						  FIELD_PREP(CE_RDES3_IOC, ld) | FIELD_PREP(CE_RDES3_TL, 16); // hw bug: rx taglen must be set to 16 on aead.
				if (ld) {
					/* link this request to the rx descriptor */
					ch->areqs[cur_rx] = &req->base;
				}
				pr_debug("rdes1: %08X\n", rx[cur_rx].des1);
				pr_debug("rdes2: %08X\n", rx[cur_rx].des2);
				pr_debug("rdes3: %08X\n", rx[cur_rx].des3);
				cur_rx = (cur_rx + 1) % DMA_RING_SIZE;
			}
		}
		if (i % 2) {
			rx[cur_rx].des1 = 0;
			rx[cur_rx].des2 = FIELD_PREP(CE_RDES2_B1L, buf1len);
			rx[cur_rx].des3 = CE_RDES3_OWN | CE_RDES3_IOC | FIELD_PREP(CE_RDES3_TL, 16); // hw bug: rx taglen must be set to 16 on aead.
			/* link this request to the rx descriptor */
			ch->areqs[cur_rx] = &req->base;
			pr_debug("rdes1: %08X\n", rx[cur_rx].des1);
			pr_debug("rdes2: %08X\n", rx[cur_rx].des2);
			pr_debug("rdes3: %08X\n", rx[cur_rx].des3);
			cur_rx = (cur_rx + 1) % DMA_RING_SIZE;
		}
	}

	ch->cur_rx = cur_rx;
#ifdef CONFIG_CRYPTO_SIFLOWER_HASH
	spin_unlock_bh(&ch->ring_lock);
#else
	local_bh_enable();
#endif
	reg_write_flush(priv, CE_DMA_CH_RxDESC_TAIL_LPTR(ch->ch_num), ch->dma_rx_phy + sizeof(struct sf_ce_desc) * (cur_rx ?: DMA_RING_SIZE));
	reg_write(priv, CE_DMA_CH_TxDESC_TAIL_LPTR(ch->ch_num), ch->dma_tx_phy + sizeof(struct sf_ce_desc) * (cur_tx ?: DMA_RING_SIZE));

	return -EINPROGRESS;
}

static int sf_ce_aes_ccm_enc(struct aead_request *req)
{
	return sf_ce_aes_ccm_op(req, false);
}

static int sf_ce_aes_ccm_dec(struct aead_request *req)
{
	return sf_ce_aes_ccm_op(req, true);
}

struct aead_alg sf_ce_aes_ccm __read_mostly = {
	.setkey	= sf_ce_aes_ccm_setkey,
	.setauthsize = sf_ce_aes_ccm_setauthsize,
	.encrypt = sf_ce_aes_ccm_enc,
	.decrypt = sf_ce_aes_ccm_dec,
	.ivsize	= AES_BLOCK_SIZE,
	.maxauthsize = AES_BLOCK_SIZE,
	.base = {
		.cra_name		= "ccm(aes)",
		.cra_driver_name	= "siflower-ce-aes-ccm",
		.cra_priority		= 300,
		.cra_blocksize		= 1,
		.cra_ctxsize		= sizeof(struct sf_ce_aes_ccm_ctx),
		.cra_flags		= CRYPTO_ALG_ASYNC |
					  CRYPTO_ALG_KERN_DRIVER_ONLY |
					  CRYPTO_ALG_NEED_FALLBACK,
		.cra_module		= THIS_MODULE,
		.cra_init		= sf_ce_aes_ccm_cra_init,
		.cra_exit		= sf_ce_aes_ccm_cra_exit,
	},
};
