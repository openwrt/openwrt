#include "common.h"
#include "aes.h"
#include "hash.h"

/* AES-CCM gives correct results but stalls under sustained load */
static bool enable_ccm;
module_param(enable_ccm, bool, 0444);
MODULE_PARM_DESC(enable_ccm, "Register the AES-CCM implementation (default off, unstable under load)");

struct sf_ce_dev *g_dev;

#if defined(CONFIG_HAVE_EFFICIENT_UNALIGNED_ACCESS) && defined(CONFIG_CC_HAS_INT128)
/* Swap the byte order of an int128. */
static __always_inline __uint128_t swab128(__uint128_t v)
{
	u64 low, high, tmp;

	low = (u64)v;
	high = (u64)(v >> 64);

	/* swap low and high */
	tmp = swab64(low);
	low = swab64(high);
	high = tmp;

	return (__uint128_t)high << 64 | low;
}

#ifdef __LITTLE_ENDIAN
#define cpu_to_be128(x) swab128(x)
#define be128_to_cpu(x) swab128(x)
#else
#define cpu_to_be128(x) (x)
#define be128_to_cpu(x) (x)
#endif /* __LITTLE_ENDIAN */

static void sf_ce_inc(u8 iv[AES_BLOCK_SIZE], unsigned int count)
{
	__uint128_t iv128;

	iv128 = *(__uint128_t *)iv;
	iv128 = be128_to_cpu(iv128);
	iv128 += count;
	iv128 = cpu_to_be128(iv128);
	*(__uint128_t *)iv = iv128;
}
#else
static void sf_ce_inc(u8 iv[AES_BLOCK_SIZE], unsigned int count)
{
	crypto_inc(iv, count);
}
#endif

static void sf_ce_irq_bh(unsigned long data)
{
	struct sf_ce_chan *ch = (struct sf_ce_chan *)data;
	struct sf_ce_dev *priv = container_of(ch, struct sf_ce_dev, chan[ch->ch_num]);
	struct device *dev = priv->dev;
	u32 curr_tx_desc;
	unsigned int i;

	// update tx pointer
	curr_tx_desc = reg_read(priv, CE_DMA_CH_CUR_TxDESC_LADDR(ch->ch_num));
#ifdef CONFIG_CRYPTO_SIFLOWER_HASH
	spin_lock(&ch->ring_lock);
#endif
	ch->dirty_tx = (curr_tx_desc - ch->dma_tx_phy) / sizeof(*ch->dma_tx) % DMA_RING_SIZE;
	for (i = ch->dirty_rx; i != ch->cur_rx; i = (i + 1) % DMA_RING_SIZE) {
		struct crypto_async_request *areq;
		enum crypt_algo alg;
		int ret = 0;
		u32 rdes3;

		// TODO: load pipelining, or read DMA current rx register instead
		rdes3 = le32_to_cpu(READ_ONCE(ch->dma_rx[i].des3));
		if (unlikely(rdes3 & CE_RDES3_OWN))
			break;

		areq = ch->areqs[i];
		if (unlikely(!areq))
			continue;

		ch->areqs[i] = NULL;
		if (WARN(!(rdes3 & CE_RDES3_WB_LD),
			 "expected RX last descriptor but it's not!\n")) {
			ret = -EBADMSG;
			goto areq_complete;
		}

		alg = FIELD_GET(CE_RDES3_WB_CT, rdes3);
		switch (alg)
		{
#ifdef CONFIG_CRYPTO_SIFLOWER_HASH
		case CE_MD5:
		case CE_SHA1:
		case CE_SHA256: {
			/* rx handler for md5/sha1/sha256/sha224 */
			struct ahash_request *req;
			struct sf_ce_ahash_reqctx *reqctx;
			unsigned int new_partial_offset;
			unsigned int digest_size;

			req = ahash_request_cast(areq);
			reqctx = ahash_request_ctx(req);
			digest_size = crypto_ahash_digestsize(crypto_ahash_reqtfm(req));

			dma_unmap_sg(dev, req->src, reqctx->nents,
				     DMA_TO_DEVICE);

			if (reqctx->final) {
				dma_unmap_single(dev, reqctx->hash_phys,
						 sizeof(reqctx->hash),
						 DMA_BIDIRECTIONAL);
				dma_unmap_single(dev, reqctx->block_phys,
						 sizeof(reqctx->block),
						 DMA_TO_DEVICE);

				/* sha1 special case for its dma offset */
				if (alg == CE_SHA1)
					memcpy(req->result,
					       reqctx->hash + SHA1_DMA_OFFSET,
					       digest_size);
				else
					memcpy(req->result, reqctx->hash,
					       digest_size);
			} else {
				reqctx->length += req->nbytes;
				/* new_partial_offset: new partial data offset after update */
				new_partial_offset = reqctx->length & 0x3f;
				scatterwalk_map_and_copy(
					reqctx->block, req->src,
					req->nbytes - new_partial_offset,
					new_partial_offset, 0);
			}
			break;
		}
		case CE_SHA512: {
			/* rx handler for sha512/sha384 */
			struct ahash_request *req;
			struct sf_ce_ahash512_reqctx *reqctx;
			unsigned int new_partial_offset;
			unsigned int digest_size;

			req = ahash_request_cast(areq);
			reqctx = ahash_request_ctx(req);
			digest_size = crypto_ahash_digestsize(crypto_ahash_reqtfm(req));

			dma_unmap_sg(dev, req->src, reqctx->nents,
				     DMA_TO_DEVICE);

			if (reqctx->final) {
				dma_unmap_single(dev, reqctx->hash_phys,
						 sizeof(reqctx->hash),
						 DMA_BIDIRECTIONAL);
				dma_unmap_single(dev, reqctx->block_phys,
						 sizeof(reqctx->block),
						 DMA_TO_DEVICE);
				memcpy(req->result, reqctx->hash, digest_size);
			} else {
				reqctx->length += req->nbytes;
				/* new_partial_offset: new partial data offset after update */
				new_partial_offset = reqctx->length & 0x7f;
				scatterwalk_map_and_copy(
					reqctx->block, req->src,
					req->nbytes - new_partial_offset,
					new_partial_offset, 0);
			}
			break;
		}
#endif
		case CE_AES_CTR: {
			struct skcipher_request *req;
			struct sf_ce_aes_reqctx *reqctx;

			req = skcipher_request_cast(areq);
			reqctx = skcipher_request_ctx(req);

			if (reqctx->tmp_buf) {
				dma_unmap_single(dev, reqctx->tmp_buf_phys, ALIGN(req->cryptlen, ARCH_DMA_MINALIGN), DMA_FROM_DEVICE);
				scatterwalk_map_and_copy(reqctx->tmp_buf, req->dst, 0, req->cryptlen, 1);
				kfree(reqctx->tmp_buf);
				dma_unmap_sg(dev, req->src, reqctx->ssg_len, DMA_TO_DEVICE);
				reqctx->tmp_buf = NULL;
			} else if (req->src == req->dst) {
				dma_unmap_sg(dev, req->src, reqctx->ssg_len, DMA_BIDIRECTIONAL);
			} else {
				dma_unmap_sg(dev, req->src, reqctx->ssg_len, DMA_TO_DEVICE);
				dma_unmap_sg(dev, req->dst, reqctx->dsg_len, DMA_FROM_DEVICE);
			}
			dma_unmap_single(dev, reqctx->iv_phys, AES_BLOCK_SIZE, DMA_TO_DEVICE);

			/* increase iv by blocks */
			sf_ce_inc(req->iv, DIV_ROUND_UP(req->cryptlen, AES_BLOCK_SIZE));
			break;
		}
		case CE_AES_GCM: {
			struct aead_request *req = aead_request_cast(areq);
			struct crypto_aead *aead = crypto_aead_reqtfm(req);
			struct sf_ce_aes_gcm_reqctx *reqctx = aead_request_ctx(req);
			bool is_decrypt = rdes3 & CE_RDES3_WB_CM;
			const unsigned int ptext_len =
				req->cryptlen - (is_decrypt ? crypto_aead_authsize(aead) : 0);
			const unsigned int ctext_and_tag_len =
				req->cryptlen + (is_decrypt ? 0 : crypto_aead_authsize(aead));

			ret = (rdes3 & CE_RDES3_WB_AEAD_VERIFY) ? 0 : -EBADMSG;
			if (reqctx->tmp_buf) {
				/* same size as mapped in sf_ce_aes_{gcm,ccm}_op() */
				dma_unmap_single(dev, reqctx->tmp_buf_phys, ALIGN(ctext_and_tag_len, DMA_RX_ALIGN), DMA_FROM_DEVICE);
				scatterwalk_map_and_copy(reqctx->tmp_buf,
							 req->dst, req->assoclen, is_decrypt ? ptext_len : ctext_and_tag_len,
							 1);
				kfree(reqctx->tmp_buf);
				dma_unmap_sg(dev, req->src, reqctx->ssg_len, DMA_TO_DEVICE);
			} else if (req->src == req->dst) {
				dma_unmap_sg(dev, req->src, reqctx->ssg_len, DMA_BIDIRECTIONAL);
			} else {
				dma_unmap_sg(dev, req->src, reqctx->ssg_len, DMA_TO_DEVICE);
				dma_unmap_sg(dev, req->dst, reqctx->dsg_len, DMA_FROM_DEVICE);
			}
			dma_unmap_single(dev, reqctx->iv_extra_tag_phys,
					 sizeof(reqctx->iv) +
						 sizeof(reqctx->alen_dma) +
						 sizeof(reqctx->plen_dma) +
						 sizeof(reqctx->taglen_dma) +sizeof(reqctx->tag),
					 DMA_TO_DEVICE);
			break;
		}
		case CE_AES_CCM: {
			struct aead_request *req = aead_request_cast(areq);
			struct crypto_aead *aead = crypto_aead_reqtfm(req);
			struct sf_ce_aes_ccm_reqctx *reqctx = aead_request_ctx(req);
			bool is_decrypt = rdes3 & CE_RDES3_WB_CM;
			const unsigned int ptext_len =
				req->cryptlen - (is_decrypt ? crypto_aead_authsize(aead) : 0);
			const unsigned int ctext_and_tag_len =
				req->cryptlen + (is_decrypt ? 0 : crypto_aead_authsize(aead));

			ret = (rdes3 & CE_RDES3_WB_AEAD_VERIFY) ? 0 : -EBADMSG;
			if (reqctx->tmp_buf) {
				/* same size as mapped in sf_ce_aes_{gcm,ccm}_op() */
				dma_unmap_single(dev, reqctx->tmp_buf_phys, ALIGN(ctext_and_tag_len, DMA_RX_ALIGN), DMA_FROM_DEVICE);
				scatterwalk_map_and_copy(reqctx->tmp_buf,
							 req->dst, req->assoclen, is_decrypt ? ptext_len : ctext_and_tag_len,
							 1);
				kfree(reqctx->tmp_buf);
				dma_unmap_sg(dev, req->src, reqctx->ssg_len, DMA_TO_DEVICE);
			} else if (req->src == req->dst) {
				dma_unmap_sg(dev, req->src, reqctx->ssg_len, DMA_BIDIRECTIONAL);
			} else {
				dma_unmap_sg(dev, req->src, reqctx->ssg_len, DMA_TO_DEVICE);
				dma_unmap_sg(dev, req->dst, reqctx->dsg_len, DMA_FROM_DEVICE);
			}
			dma_unmap_single(dev, reqctx->iv_extra_tag_phys, offsetofend(struct sf_ce_aes_ccm_reqctx, alen_dma),
					 DMA_TO_DEVICE);
			break;
		}
		default:
			break;
		}
areq_complete:
		crypto_request_complete(areq, ret);
	}
	ch->dirty_rx = i;
#ifdef CONFIG_CRYPTO_SIFLOWER_HASH
	spin_unlock(&ch->ring_lock);
#endif

	reg_write_flush(priv, CE_DMA_CH_INT_EN(ch->ch_num), CE_DMA_INT_DEFAULT_EN);
}

static irqreturn_t sf_ce_rx_irq(int irq, void *dev_id)
{
	struct sf_ce_chan *ch = dev_id;
	struct sf_ce_dev *priv = container_of(ch, struct sf_ce_dev, chan[ch->ch_num]);
	u32 status;

	status = reg_read(priv, CE_DMA_CH_STATUS(ch->ch_num));
	pr_debug("ch %u interrupt status: %#x\n", ch->ch_num, status);
	if (unlikely(!(status & CE_RI)))
		return IRQ_NONE;

	reg_write(priv, CE_DMA_CH_STATUS(ch->ch_num), CE_RI | CE_RBU);

	reg_write(priv, CE_DMA_CH_INT_EN(ch->ch_num), 0);
	tasklet_schedule(&ch->bh);

	return IRQ_HANDLED;
}

static void sf_ce_hw_init(struct sf_ce_dev *priv)
{
	int i;

	/* DMA Configuration */
	reg_write(priv, CE_US_ENDIAN_CFG, FIELD_PREP(CE_US_ENDIAN_CFG_0, CE_ENDIAN_BIG));

	reg_write(priv, CE_DMA_SYSBUS_MODE, FIELD_PREP(CE_WR_OSR_LMT, 1) | FIELD_PREP(CE_RD_OSR_LMT, 1) | CE_BLEN8 | CE_BLEN4);
	reg_write(priv, CE_TX_EDMA_CTRL, CE_TEDM);
	reg_write(priv, CE_RX_EDMA_CTRL, CE_REDM);

	for (i = 0; i < DMA_CH_NUM; i++) {
		reg_rmw(priv, CE_DMA_CH_RX_CONTROL(i), CE_RxPBL | CE_RBSZ,
			FIELD_PREP(CE_RxPBL, 8));
		reg_rmw(priv, CE_DMA_CH_TX_CONTROL(i), CE_TxPBL,
			FIELD_PREP(CE_TxPBL, 4));
	}
}

static int sf_ce_desc_init(struct sf_ce_dev *priv)
{
	int i;

	for (i = 0; i < DMA_CH_NUM; i++) {
		struct sf_ce_chan *ch = &priv->chan[i];
		ch->dma_rx =
			dmam_alloc_coherent(priv->dev,
					    sizeof(*ch->dma_rx) * DMA_RING_SIZE,
					    &ch->dma_rx_phy, GFP_KERNEL);
		if (!ch->dma_rx)
			return -ENOMEM;

		ch->dma_tx =
			dmam_alloc_coherent(priv->dev,
					    sizeof(*ch->dma_tx) * DMA_RING_SIZE,
					    &ch->dma_tx_phy, GFP_KERNEL);
		if (!ch->dma_tx)
			return -ENOMEM;

		ch->areqs = devm_kcalloc(priv->dev, DMA_RING_SIZE, sizeof(*ch->areqs), GFP_KERNEL);
		if (!ch->areqs)
			return -ENOMEM;

		reg_write(priv, CE_DMA_CH_RxDESC_LADDR(i), ch->dma_rx_phy);
		reg_write(priv, CE_DMA_CH_RxDESC_TAIL_LPTR(i),
			  ch->dma_rx_phy + sizeof(*ch->dma_rx) * DMA_RING_SIZE);
		reg_write(priv, CE_DMA_CH_RxDESC_RING_LEN(i),
			  DMA_RING_SIZE - 1);
		reg_write(priv, CE_DMA_CH_TxDESC_LADDR(i), ch->dma_tx_phy);
		reg_write(priv, CE_DMA_CH_TxDESC_TAIL_LPTR(i),
			  ch->dma_tx_phy + sizeof(*ch->dma_tx) * DMA_RING_SIZE);
		reg_write(priv, CE_DMA_CH_TxDESC_RING_LEN(i),
			  DMA_RING_SIZE - 1);

		dev_dbg(priv->dev,
			"TX ptr: %px, TX phy: %pad, RX ptr: %px, RX phy: %pad\n",
			ch->dma_tx, &ch->dma_tx_phy, ch->dma_rx, &ch->dma_rx_phy);
	}
	return 0;
}

static void sf_ce_start(struct sf_ce_dev *priv)
{
	int i;

	for (i = 0; i < DMA_CH_NUM; i++) {
		reg_set(priv, CE_DMA_CH_RX_CONTROL(i), CE_RXST);
		reg_set(priv, CE_DMA_CH_TX_CONTROL(i), CE_TXST);
		reg_write(priv, CE_DMA_CH_INT_EN(i), CE_DMA_INT_DEFAULT_EN);
	}
}

static void sf_ce_stop(struct sf_ce_dev *priv)
{
	int i;

	for (i = 0; i < DMA_CH_NUM; i++) {
		reg_write(priv, CE_DMA_CH_INT_EN(i), 0);
		reg_clear(priv, CE_DMA_CH_TX_CONTROL(i), CE_TXST);
		reg_clear(priv, CE_DMA_CH_RX_CONTROL(i), CE_RXST);
	}
}

static int sf_ce_probe(struct platform_device *pdev)
{
	struct sf_ce_dev *priv;
	char irqname[] = "rx0";
	int i, ret;

	if (g_dev)
		return -EEXIST;

	priv = devm_kzalloc(&pdev->dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	platform_set_drvdata(pdev, priv);
	priv->base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(priv->base))
		return PTR_ERR(priv->base);

	priv->dev = &pdev->dev;

	for (i = 0; i < DMA_CH_NUM; i++) {
		priv->chan[i].ch_num = i;
#ifdef CONFIG_CRYPTO_SIFLOWER_HASH
		spin_lock_init(&priv->chan[i].ring_lock);
#endif
		tasklet_init(&priv->chan[i].bh, sf_ce_irq_bh, (unsigned long)&priv->chan[i]);
	}

	priv->csr_clk = devm_clk_get(&pdev->dev, "csr");
	if (IS_ERR(priv->csr_clk))
		return PTR_ERR(priv->csr_clk);

	priv->app_clk = devm_clk_get(&pdev->dev, "app");
	if (IS_ERR(priv->app_clk))
		return PTR_ERR(priv->app_clk);

	priv->csr_rstc = devm_reset_control_get(&pdev->dev, "csr");
	if (IS_ERR(priv->csr_rstc))
		return PTR_ERR(priv->csr_rstc);

	priv->app_rstc = devm_reset_control_get(&pdev->dev, "app");
	if (IS_ERR(priv->app_rstc))
		return PTR_ERR(priv->app_rstc);

	// RX IRQ
	for (i = 0; i < DMA_CH_NUM; i++, irqname[sizeof(irqname) - 2]++) {
		int irq;

		irq = platform_get_irq_byname(pdev, irqname);
		if (irq < 0)
			return irq;

		ret = devm_request_irq(&pdev->dev, irq, sf_ce_rx_irq, 0,
				       "siflower-crypto", &priv->chan[i]);
		if (ret)
			return ret;

		irq_set_affinity_hint(irq, cpumask_of(cpumask_local_spread(i, NUMA_NO_NODE)));
		priv->chan[i].rx_irq = irq;
	}

	priv->zero_pad_phys = dma_map_single(priv->dev, empty_zero_page, PAGE_SIZE, DMA_TO_DEVICE);
	ret = dma_mapping_error(priv->dev, priv->zero_pad_phys);
	if (ret)
		return ret;

	ret = clk_prepare_enable(priv->csr_clk);
	if (ret)
		goto err_csr_clk;

	ret = clk_prepare_enable(priv->app_clk);
	if (ret)
		goto err_app_clk;

	ret = reset_control_assert(priv->csr_rstc);
	if (ret)
		goto err_csr_rstc;

	ret = reset_control_assert(priv->app_rstc);
	if (ret)
		goto err_csr_rstc;

	ret = reset_control_deassert(priv->csr_rstc);
	if (ret)
		goto err_csr_rstc;

	ret = reset_control_deassert(priv->app_rstc);
	if (ret)
		goto err_app_rstc;

	sf_ce_hw_init(priv);

	ret = sf_ce_desc_init(priv);
	if (ret)
		goto err_desc_init;

	sf_ce_start(priv);

	g_dev = priv;
	ret = crypto_register_aead(&sf_ce_aes_gcm);
	if (ret)
		goto err_aes_gcm;

	if (enable_ccm) {
		ret = crypto_register_aead(&sf_ce_aes_ccm);
		if (ret)
			goto err_aes_ccm;
	}

	ret = crypto_register_skcipher(&sf_ce_aes_ctr);
	if (ret)
		goto err_aes_ctr;

#ifdef CONFIG_CRYPTO_SIFLOWER_HASH
	ret = crypto_register_ahash(&sf_ce_sha1);
	if (ret)
		goto err_sha1;

	ret = crypto_register_ahash(&sf_ce_sha256);
	if (ret)
		goto err_sha256;

	ret = crypto_register_ahash(&sf_ce_sha224);
	if (ret)
		goto err_sha224;

	ret = crypto_register_ahash(&sf_ce_sha512);
	if (ret)
		goto err_sha512;

	ret = crypto_register_ahash(&sf_ce_sha384);
	if (ret)
		goto err_sha384;

	ret = crypto_register_ahash(&sf_ce_md5);
	if (ret)
		goto err_md5;
#endif
	return 0;
#ifdef CONFIG_CRYPTO_SIFLOWER_HASH
err_md5:
	crypto_unregister_ahash(&sf_ce_sha384);
err_sha384:
	crypto_unregister_ahash(&sf_ce_sha512);
err_sha512:
	crypto_unregister_ahash(&sf_ce_sha224);
err_sha224:
	crypto_unregister_ahash(&sf_ce_sha256);
err_sha256:
	crypto_unregister_ahash(&sf_ce_sha1);
err_sha1:
	crypto_unregister_skcipher(&sf_ce_aes_ctr);
#endif
err_aes_ctr:
	if (enable_ccm)
		crypto_unregister_aead(&sf_ce_aes_ccm);
err_aes_ccm:
	crypto_unregister_aead(&sf_ce_aes_gcm);
err_aes_gcm:
	g_dev = NULL;
	sf_ce_stop(priv);
err_desc_init:
	reset_control_assert(priv->app_rstc);
err_app_rstc:
	reset_control_assert(priv->csr_rstc);
err_csr_rstc:
	clk_disable_unprepare(priv->app_clk);
err_app_clk:
	clk_disable_unprepare(priv->csr_clk);
err_csr_clk:
	dma_unmap_single_attrs(priv->dev, priv->zero_pad_phys, PAGE_SIZE, DMA_TO_DEVICE, DMA_ATTR_SKIP_CPU_SYNC);
	return ret;
}

static void sf_ce_remove(struct platform_device *pdev)
{
	struct sf_ce_dev *priv;
	int i;

	priv = platform_get_drvdata(pdev);
	WARN_ON(priv != g_dev);

#ifdef CONFIG_CRYPTO_SIFLOWER_HASH
	crypto_unregister_ahash(&sf_ce_md5);
	crypto_unregister_ahash(&sf_ce_sha384);
	crypto_unregister_ahash(&sf_ce_sha512);
	crypto_unregister_ahash(&sf_ce_sha224);
	crypto_unregister_ahash(&sf_ce_sha256);
	crypto_unregister_ahash(&sf_ce_sha1);
#endif
	crypto_unregister_skcipher(&sf_ce_aes_ctr);
	if (enable_ccm)
		crypto_unregister_aead(&sf_ce_aes_ccm);
	crypto_unregister_aead(&sf_ce_aes_gcm);
	sf_ce_stop(priv);

	for (i = 0; i < DMA_CH_NUM; i++) {
		tasklet_kill(&priv->chan[i].bh);
		irq_set_affinity_hint(priv->chan[i].tx_irq, NULL);
		irq_set_affinity_hint(priv->chan[i].rx_irq, NULL);
	}

	reset_control_assert(priv->app_rstc);
	reset_control_assert(priv->csr_rstc);
	clk_disable_unprepare(priv->app_clk);
	clk_disable_unprepare(priv->csr_clk);
	dma_unmap_single_attrs(priv->dev, priv->zero_pad_phys, PAGE_SIZE, DMA_TO_DEVICE, DMA_ATTR_SKIP_CPU_SYNC);
	g_dev = NULL;
}

static const struct of_device_id sf_ce_match[] = {
	{ .compatible = "siflower,sf21a6826-crypto" },
	{ .compatible = "siflower,sf21-crypto" },
	{},
};
MODULE_DEVICE_TABLE(of, sf_ce_match);

static struct platform_driver sf_ce_driver = {
	.driver = {
		.name		= "siflower-crypto",
		.of_match_table	= of_match_ptr(sf_ce_match),
	},
	.probe = sf_ce_probe,
	.remove = sf_ce_remove,
};
module_platform_driver(sf_ce_driver);

MODULE_LICENSE("GPL");
