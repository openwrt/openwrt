// SPDX-License-Identifier: GPL-2.0-only
/*
 * Crypto acceleration support for Realtek crypto engine. Based on ideas from
 * Rockchip & SafeXcel driver plus Realtek OpenWrt RTK.
 *
 * Copyright (c) 2022, Markus Stockhausen <markus.stockhausen@gmx.de>
 */

#include <crypto/internal/hash.h>
#include <crypto/internal/skcipher.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/module.h>
#include <linux/of_irq.h>
#include <linux/platform_device.h>

#include "realtek_crypto.h"

inline int rtcr_inc_src_idx(int idx, int cnt)
{
	return (idx + cnt) & (RTCR_SRC_RING_SIZE - 1);
}

inline int rtcr_inc_dst_idx(int idx, int cnt)
{
	return (idx + cnt) & (RTCR_DST_RING_SIZE - 1);
}

inline int rtcr_inc_buf_idx(int idx, int cnt)
{
	return (idx + cnt) & (RTCR_BUF_RING_SIZE - 1);
}

inline int rtcr_space_plus_pad(int len)
{
	return (len + 31) & ~31;
}

int rtcr_alloc_ring(struct rtcr_crypto_dev *cdev, int srclen, int *srcidx,
		    int *dstidx, int buflen, char **buf)
{
	int srcfree, dstfree, buffree, bufidx;
	int srcalloc = (srclen + 1) & ~1, bufalloc = 0;
	int ret = -ENOSPC;

	spin_lock(&cdev->ringlock);

	bufidx = cdev->cpu_buf_idx;
	if (buflen > 0) {
		bufalloc = rtcr_space_plus_pad(buflen);
		if (bufidx + bufalloc > RTCR_BUF_RING_SIZE) {
			if (unlikely(cdev->cpu_buf_idx > bufidx)) {
				dev_err(cdev->dev, "buffer ring full\n");
				goto err_nospace;
			}
			/* end of buffer is free but too small, skip it */
			bufidx = 0;
		}
	}

	srcfree = rtcr_inc_src_idx(cdev->pp_src_idx - cdev->cpu_src_idx, -1);
	dstfree = rtcr_inc_dst_idx(cdev->pp_dst_idx - cdev->cpu_dst_idx, -1);
	buffree = rtcr_inc_buf_idx(cdev->pp_buf_idx - bufidx, -1);

	if (unlikely(srcfree < srcalloc)) {
		dev_err(cdev->dev, "source ring full\n");
		goto err_nospace;
	}
	if (unlikely(dstfree < 1)) {
		dev_err(cdev->dev, "destination ring full\n");
		goto err_nospace;
	}
	if (unlikely(buffree < bufalloc)) {
		dev_err(cdev->dev, "buffer ring full\n");
		goto err_nospace;
	}

	*srcidx = cdev->cpu_src_idx;
	cdev->cpu_src_idx = rtcr_inc_src_idx(cdev->cpu_src_idx, srcalloc);

	*dstidx = cdev->cpu_dst_idx;
	cdev->cpu_dst_idx = rtcr_inc_dst_idx(cdev->cpu_dst_idx, 1);

	ret = 0;
	cdev->wbk_ring[*dstidx].len = buflen;
	if (buflen > 0) {
		*buf = &cdev->buf_ring[bufidx];
		cdev->wbk_ring[*dstidx].src = *buf;
		cdev->cpu_buf_idx = rtcr_inc_buf_idx(bufidx, bufalloc);
	}

err_nospace:
	spin_unlock(&cdev->ringlock);

	return ret;
}

static inline void rtcr_ack_irq(struct rtcr_crypto_dev *cdev)
{
	int v = ioread32(cdev->base + RTCR_REG_CMD);

	if (unlikely((v != RTCR_CMD_DDOKIP) && v))
		dev_err(cdev->dev, "unexpected IRQ result 0x%08x\n", v);
	v = RTCR_CMD_SDUEIP | RTCR_CMD_SDLEIP | RTCR_CMD_DDUEIP |
	     RTCR_CMD_DDOKIP | RTCR_CMD_DABFIP;

	iowrite32(v, cdev->base + RTCR_REG_CMD);
}

static void rtcr_done_task(unsigned long data)
{
	struct rtcr_crypto_dev *cdev = (struct rtcr_crypto_dev *)data;
	int stop_src_idx, stop_dst_idx, idx, len;
	struct scatterlist *sg;
	unsigned long flags;

	spin_lock_irqsave(&cdev->asiclock, flags);
	stop_src_idx = cdev->asic_src_idx;
	stop_dst_idx = cdev->asic_dst_idx;
	spin_unlock_irqrestore(&cdev->asiclock, flags);

	idx = cdev->pp_dst_idx;

	while (idx != stop_dst_idx) {
		len = cdev->wbk_ring[idx].len;
		switch (len) {
		case RTCR_WB_LEN_SG_DIRECT:
			/* already written to the destination by the engine */
			break;
		case RTCR_WB_LEN_HASH:
			/* write back hash from destination ring */
			memcpy(cdev->wbk_ring[idx].dst,
			       cdev->dst_ring[idx].vector,
			       RTCR_HASH_VECTOR_SIZE);
			break;
		default:
			/* write back data from buffer */
			sg = (struct scatterlist *)cdev->wbk_ring[idx].dst;
			sg_pcopy_from_buffer(sg, sg_nents(sg),
					     cdev->wbk_ring[idx].src,
					     len, cdev->wbk_ring[idx].off);
			len = rtcr_space_plus_pad(len);
			cdev->pp_buf_idx = ((char *)cdev->wbk_ring[idx].src - cdev->buf_ring) + len;
		}

		cdev->wbk_ring[idx].len = RTCR_WB_LEN_DONE;
		idx = rtcr_inc_dst_idx(idx, 1);
	}

	wake_up_all(&cdev->done_queue);
	cdev->pp_src_idx = stop_src_idx;
	cdev->pp_dst_idx = stop_dst_idx;
}

static irqreturn_t rtcr_handle_irq(int irq, void *dev_id)
{
	struct rtcr_crypto_dev *cdev = dev_id;
	u32 p;

	spin_lock(&cdev->asiclock);

	rtcr_ack_irq(cdev);
	cdev->busy = false;

	p = (u32)phys_to_virt((u32)ioread32(cdev->base + RTCR_REG_SRC));
	cdev->asic_src_idx = (p - (u32)cdev->src_ring) / RTCR_SRC_DESC_SIZE;

	p = (u32)phys_to_virt((u32)ioread32(cdev->base + RTCR_REG_DST));
	cdev->asic_dst_idx = (p - (u32)cdev->dst_ring) / RTCR_DST_DESC_SIZE;

	tasklet_schedule(&cdev->done_task);
	spin_unlock(&cdev->asiclock);

	return IRQ_HANDLED;
}

void rtcr_add_src_ahash_to_ring(struct rtcr_crypto_dev *cdev, int idx,
				int opmode, int totallen)
{
	dma_addr_t dma = cdev->src_dma + idx * RTCR_SRC_DESC_SIZE;
	struct rtcr_src_desc *src = &cdev->src_ring[idx];

	src->len = totallen;
	src->opmode = opmode | RTCR_SRC_OP_FS |
		      RTCR_SRC_OP_DUMMY_LEN | RTCR_SRC_OP_OWN_ASIC |
		      RTCR_SRC_OP_CALC_EOR(idx);

	dma_sync_single_for_device(cdev->dev, dma, RTCR_SRC_DESC_SIZE, DMA_TO_DEVICE);
}

void rtcr_add_src_skcipher_to_ring(struct rtcr_crypto_dev *cdev, int idx,
				   int opmode, int totallen,
				   struct rtcr_skcipher_ctx *sctx)
{
	dma_addr_t dma = cdev->src_dma + idx * RTCR_SRC_DESC_SIZE;
	struct rtcr_src_desc *src = &cdev->src_ring[idx];

	src->len = totallen;
	if (opmode & RTCR_SRC_OP_KAM_ENC)
		src->paddr = sctx->keydma;
	else
		src->paddr = sctx->keydma + AES_KEYSIZE_256;

	src->opmode = RTCR_SRC_OP_FS | RTCR_SRC_OP_OWN_ASIC |
		      RTCR_SRC_OP_MS_CRYPTO | RTCR_SRC_OP_CRYPT_ECB |
		      RTCR_SRC_OP_CALC_EOR(idx) | opmode | sctx->keylen;

	dma_sync_single_for_device(cdev->dev, dma, RTCR_SRC_DESC_SIZE, DMA_TO_DEVICE);
}

void rtcr_add_src_to_ring(struct rtcr_crypto_dev *cdev, int idx, void *vaddr,
			  int blocklen, int totallen)
{
	dma_addr_t dma = cdev->src_dma + idx * RTCR_SRC_DESC_SIZE;
	struct rtcr_src_desc *src = &cdev->src_ring[idx];

	src->len = totallen;
	src->paddr = virt_to_phys(vaddr);
	src->opmode = RTCR_SRC_OP_OWN_ASIC | RTCR_SRC_OP_CALC_EOR(idx) | blocklen;

	dma_sync_single_for_device(cdev->dev, dma, RTCR_SRC_DESC_SIZE, DMA_BIDIRECTIONAL);
}

inline void rtcr_add_src_pad_to_ring(struct rtcr_crypto_dev *cdev, int idx, int len)
{
	/* align 16 byte source descriptors with 32 byte cache lines */
	if (!(idx & 1))
		rtcr_add_src_to_ring(cdev, idx + 1, NULL, 0, len);
}

void rtcr_add_dst_to_ring(struct rtcr_crypto_dev *cdev, int idx, void *reqdst,
			  int reqlen, void *wbkdst, int wbkoff)
{
	dma_addr_t dma = cdev->dst_dma + idx * RTCR_DST_DESC_SIZE;
	struct rtcr_dst_desc *dst = &cdev->dst_ring[idx];
	struct rtcr_wbk_desc *wbk = &cdev->wbk_ring[idx];

	dst->paddr = virt_to_phys(reqdst);
	dst->opmode = RTCR_DST_OP_OWN_ASIC | RTCR_DST_OP_CALC_EOR(idx) | reqlen;

	wbk->dst = wbkdst;
	wbk->off = wbkoff;

	dma_sync_single_for_device(cdev->dev, dma, RTCR_DST_DESC_SIZE, DMA_BIDIRECTIONAL);
}

inline int rtcr_wait_for_request(struct rtcr_crypto_dev *cdev, int idx)
{
	int *len = &cdev->wbk_ring[idx].len;

	wait_event(cdev->done_queue, *len == RTCR_WB_LEN_DONE);
	return 0;
}

void rtcr_kick_engine(struct rtcr_crypto_dev *cdev)
{
	unsigned long flags;

	spin_lock_irqsave(&cdev->asiclock, flags);

	if (!cdev->busy) {
		cdev->busy = true;
		/* engine needs up to 5us to reset poll bit */
		iowrite32(RTCR_CMD_POLL, cdev->base + RTCR_REG_CMD);
	}

	spin_unlock_irqrestore(&cdev->asiclock, flags);
}

static struct rtcr_alg_template *rtcr_algs[] = {
	&rtcr_ahash_md5,
	&rtcr_ahash_sha1,
	&rtcr_skcipher_ecb_aes,
	&rtcr_skcipher_cbc_aes,
	&rtcr_skcipher_ctr_aes,
};

static void rtcr_unregister_algorithms(int end)
{
	int i;

	for (i = 0; i < end; i++) {
		if (rtcr_algs[i]->type == RTCR_ALG_SKCIPHER)
			crypto_unregister_skcipher(&rtcr_algs[i]->alg.skcipher);
		else
			crypto_unregister_ahash(&rtcr_algs[i]->alg.ahash);
	}
}

static int rtcr_register_algorithms(struct rtcr_crypto_dev *cdev)
{
	int i, ret = 0;

	for (i = 0; i < ARRAY_SIZE(rtcr_algs); i++) {
		rtcr_algs[i]->cdev = cdev;
		if (rtcr_algs[i]->type == RTCR_ALG_SKCIPHER)
			ret = crypto_register_skcipher(&rtcr_algs[i]->alg.skcipher);
		else {
			rtcr_algs[i]->alg.ahash.halg.statesize =
				max(sizeof(struct rtcr_ahash_req),
				offsetof(struct rtcr_ahash_req, vector) +
				sizeof(union rtcr_fallback_state));
			ret = crypto_register_ahash(&rtcr_algs[i]->alg.ahash);
		}
		if (ret)
			goto err_cipher_algs;
	}

	return 0;

err_cipher_algs:
	rtcr_unregister_algorithms(i);

	return ret;
}

static void rtcr_init_engine(struct rtcr_crypto_dev *cdev)
{
	int v;

	v = ioread32(cdev->base + RTCR_REG_CMD);
	v |= RTCR_CMD_SRST;
	iowrite32(v, cdev->base + RTCR_REG_CMD);

	usleep_range(10000, 20000);

	iowrite32(RTCR_CTR_CKE | RTCR_CTR_SDM16 | RTCR_CTR_DDM16 |
		  RTCR_CTR_SDUEIE | RTCR_CTR_SDLEIE | RTCR_CTR_DDUEIE |
		  RTCR_CTR_DDOKIE | RTCR_CTR_DABFIE, cdev->base + RTCR_REG_CTR);

	rtcr_ack_irq(cdev);
	usleep_range(10000, 20000);
}

static void rtcr_exit_engine(struct rtcr_crypto_dev *cdev)
{
	iowrite32(0, cdev->base + RTCR_REG_CTR);
}

static void rtcr_init_rings(struct rtcr_crypto_dev *cdev)
{
	iowrite32(cdev->src_dma, cdev->base + RTCR_REG_SRC);
	iowrite32(cdev->dst_dma, cdev->base + RTCR_REG_DST);

	cdev->asic_dst_idx = cdev->asic_src_idx = 0;
	cdev->cpu_src_idx = cdev->cpu_dst_idx = cdev->cpu_buf_idx = 0;
	cdev->pp_src_idx = cdev->pp_dst_idx = cdev->pp_buf_idx = 0;
}

static int rtcr_crypto_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct rtcr_crypto_dev *cdev;
	unsigned long flags = 0;
	struct resource *res;
	void __iomem *base;
	int irq, ret;

#ifdef CONFIG_MIPS
	if ((cpu_dcache_line_size() != 16) && (cpu_dcache_line_size() != 32)) {
		dev_err(dev, "cache line size not 16 or 32 bytes\n");
		ret = -EINVAL;
		goto err_map;
	}
#endif

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		dev_err(dev, "no IO address given\n");
		ret = -ENODEV;
		goto err_map;
	}

	base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR_OR_NULL(base)) {
		dev_err(dev, "failed to map IO address\n");
		ret = -EINVAL;
		goto err_map;
	}

	cdev = devm_kzalloc(dev, sizeof(*cdev), GFP_KERNEL);
	if (!cdev) {
		dev_err(dev, "failed to allocate device memory\n");
		ret = -ENOMEM;
		goto err_mem;
	}

	irq = irq_of_parse_and_map(pdev->dev.of_node, 0);
	if (!irq) {
		dev_err(dev, "failed to determine device interrupt\n");
		ret = -EINVAL;
		goto err_of_irq;
	}

	if (devm_request_irq(dev, irq, rtcr_handle_irq, flags, "realtek-crypto", cdev)) {
		dev_err(dev, "failed to request device interrupt\n");
		ret = -ENXIO;
		goto err_request_irq;
	}

	platform_set_drvdata(pdev, cdev);
	cdev->base = base;
	cdev->dev = dev;
	cdev->irq = irq;
	cdev->pdev = pdev;

	cdev->src_dma = dma_map_single(cdev->dev, cdev->src_ring,
				       RTCR_SRC_DESC_SIZE * RTCR_SRC_RING_SIZE, DMA_BIDIRECTIONAL);
	cdev->dst_dma = dma_map_single(cdev->dev, cdev->dst_ring,
				       RTCR_DST_DESC_SIZE * RTCR_DST_RING_SIZE, DMA_BIDIRECTIONAL);

	dma_map_single(dev, (void *)empty_zero_page, PAGE_SIZE, DMA_TO_DEVICE);

	init_waitqueue_head(&cdev->done_queue);
	tasklet_init(&cdev->done_task, rtcr_done_task, (unsigned long)cdev);
	spin_lock_init(&cdev->ringlock);
	spin_lock_init(&cdev->asiclock);

	/* Init engine first as it resets the ring pointers */
	rtcr_init_engine(cdev);
	rtcr_init_rings(cdev);
	rtcr_register_algorithms(cdev);

	dev_info(dev, "%d KB buffer, max %d requests of up to %d bytes\n",
		 RTCR_BUF_RING_SIZE / 1024, RTCR_DST_RING_SIZE,
		 RTCR_MAX_REQ_SIZE);
	dev_info(dev, "ready for AES/SHA1/MD5 crypto acceleration\n");

	return 0;

err_request_irq:
	irq_dispose_mapping(irq);
err_of_irq:
	kfree(cdev);
err_mem:
	iounmap(base);
err_map:
	return ret;
}

static void rtcr_crypto_remove(struct platform_device *pdev)
{
	struct rtcr_crypto_dev *cdev = platform_get_drvdata(pdev);

	dma_unmap_single(cdev->dev, cdev->src_dma,
			 RTCR_SRC_DESC_SIZE * RTCR_SRC_RING_SIZE, DMA_BIDIRECTIONAL);
	dma_unmap_single(cdev->dev, cdev->dst_dma,
			 RTCR_DST_DESC_SIZE * RTCR_DST_RING_SIZE, DMA_BIDIRECTIONAL);

	rtcr_exit_engine(cdev);
	rtcr_unregister_algorithms(ARRAY_SIZE(rtcr_algs));
	tasklet_kill(&cdev->done_task);
}

static const struct of_device_id rtcr_id_table[] = {
	{ .compatible = "realtek,realtek-crypto" },
	{}
};
MODULE_DEVICE_TABLE(of, rtcr_id_table);

static struct platform_driver rtcr_driver = {
	.probe		= rtcr_crypto_probe,
	.remove		= rtcr_crypto_remove,
	.driver		= {
		.name	= "realtek-crypto",
		.of_match_table	= rtcr_id_table,
	},
};

module_platform_driver(rtcr_driver);

MODULE_AUTHOR("Markus Stockhausen <markus.stockhausen@gmx.de>");
MODULE_DESCRIPTION("Support for Realtek's cryptographic engine");
MODULE_LICENSE("GPL");
