// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * BCM3380 UniMAC MBDMA provider
 *
 * Copyright (C) 2026 Hang Zhou <929513338@qq.com>
 */

#include <linux/bits.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/slab.h>

#include <soc/bcm/bcm3380-fpm.h>
#include <soc/bcm/bcm3383-mbdma.h>

// MbdmaStatus.Reg32
#define MBDMA_STATUS			0x0000
#define MBDMA_STATUS_GBL_INTR_MASK	BIT(31)
#define MBDMA_STATUS_INTR_MASK		GENMASK(25, 16)
#define MBDMA_STATUS_INTR_MASK_TX_MSGQ_OVERFLOW	BIT(25)
#define MBDMA_STATUS_INTR_MASK_INVALID_TX_MSG	BIT(24)
#define MBDMA_STATUS_INTR_MASK_DIAG_INTR	BIT(23)
#define MBDMA_STATUS_INTR_MASK_UBUS_ERROR	BIT(22)
#define MBDMA_STATUS_INTR_MASK_TOKEN_RD_ERROR	BIT(21)
#define MBDMA_STATUS_INTR_MASK_INVALID_TOKEN	BIT(20)
#define MBDMA_STATUS_INTR_MASK_ALLOC_FIFO_EMPTY	BIT(19)
#define MBDMA_STATUS_INTR_MASK_ALLOC_FIFO_FULL	BIT(18)
#define MBDMA_STATUS_INTR_MASK_FREE_FIFO_EMPTY	BIT(17)
#define MBDMA_STATUS_INTR_MASK_FREE_FIFO_FULL	BIT(16)
/*
 * GPL MbdmaStatus.Bits.IntrMask:
 * mask TxMsgqOverflow, DiagIntr, UbusError, TokenRdError, and FreeFifoFull.
 */
#define MBDMA_STATUS_INTR_MASK_SET	(MBDMA_STATUS_INTR_MASK_TX_MSGQ_OVERFLOW | \
					 MBDMA_STATUS_INTR_MASK_DIAG_INTR | \
					 MBDMA_STATUS_INTR_MASK_UBUS_ERROR | \
					 MBDMA_STATUS_INTR_MASK_TOKEN_RD_ERROR | \
					 MBDMA_STATUS_INTR_MASK_FREE_FIFO_FULL)
#define MBDMA_STATUS_TX_MSGQ_OVERFLOW	BIT(9)
#define MBDMA_STATUS_INVALID_TX_MSG	BIT(8)
#define MBDMA_STATUS_UBUS_ERROR		BIT(6)
#define MBDMA_STATUS_TOKEN_RD_ERROR	BIT(5)
#define MBDMA_STATUS_INVALID_TOKEN	BIT(4)
#define MBDMA_STATUS_ALLOC_FIFO_EMPTY	BIT(3)
#define MBDMA_STATUS_FREE_FIFO_FULL	BIT(0)
#define MBDMA_STATUS_CLEAR		(MBDMA_STATUS_GBL_INTR_MASK | \
					 MBDMA_STATUS_INTR_MASK | \
					 MBDMA_STATUS_TX_MSGQ_OVERFLOW | \
					 MBDMA_STATUS_INVALID_TX_MSG | \
					 MBDMA_STATUS_UBUS_ERROR | \
					 MBDMA_STATUS_TOKEN_RD_ERROR | \
					 MBDMA_STATUS_INVALID_TOKEN | \
					 MBDMA_STATUS_ALLOC_FIFO_EMPTY | \
					 MBDMA_STATUS_FREE_FIFO_FULL)
#define MBDMA_STATUS_SET		(MBDMA_STATUS_GBL_INTR_MASK | \
					 MBDMA_STATUS_INTR_MASK_SET | \
					 MBDMA_STATUS_TX_MSGQ_OVERFLOW | \
					 MBDMA_STATUS_INVALID_TX_MSG | \
					 MBDMA_STATUS_UBUS_ERROR | \
					 MBDMA_STATUS_TOKEN_RD_ERROR | \
					 MBDMA_STATUS_INVALID_TOKEN | \
					 MBDMA_STATUS_ALLOC_FIFO_EMPTY | \
					 MBDMA_STATUS_FREE_FIFO_FULL)

// MbdmaTokenCacheCtl.Reg32
#define MBDMA_TOKEN_CACHE_CTL		0x0004
#define MBDMA_TOKEN_CACHE_ALLOC_ENABLE	BIT(31)
#define MBDMA_TOKEN_CACHE_ALLOC_MAX_BURST_MASK	GENMASK(28, 24)
#define MBDMA_TOKEN_CACHE_ALLOC_MAX_BURST_2	BIT(25)
#define MBDMA_TOKEN_CACHE_ALLOC_THRESH_MASK	GENMASK(23, 16)
#define MBDMA_TOKEN_CACHE_ALLOC_THRESH_4	BIT(18)
#define MBDMA_TOKEN_CACHE_FREE_ENABLE	BIT(15)
#define MBDMA_TOKEN_CACHE_FREE_MAX_BURST_MASK	GENMASK(12, 8)
#define MBDMA_TOKEN_CACHE_FREE_MAX_BURST_16	BIT(12)
#define MBDMA_TOKEN_CACHE_FREE_THRESH_MASK	GENMASK(7, 0)
#define MBDMA_TOKEN_CACHE_FREE_THRESH_16	BIT(4)
#define MBDMA_TOKEN_CACHE_CLEAR		(MBDMA_TOKEN_CACHE_ALLOC_ENABLE | \
					 MBDMA_TOKEN_CACHE_ALLOC_MAX_BURST_MASK | \
					 MBDMA_TOKEN_CACHE_ALLOC_THRESH_MASK | \
					 MBDMA_TOKEN_CACHE_FREE_ENABLE | \
					 MBDMA_TOKEN_CACHE_FREE_MAX_BURST_MASK | \
					 MBDMA_TOKEN_CACHE_FREE_THRESH_MASK)
#define MBDMA_TOKEN_CACHE_SET		(MBDMA_TOKEN_CACHE_ALLOC_ENABLE | \
					 MBDMA_TOKEN_CACHE_ALLOC_MAX_BURST_2 | \
					 MBDMA_TOKEN_CACHE_ALLOC_THRESH_4 | \
					 MBDMA_TOKEN_CACHE_FREE_ENABLE | \
					 MBDMA_TOKEN_CACHE_FREE_MAX_BURST_16 | \
					 MBDMA_TOKEN_CACHE_FREE_THRESH_16)

// MbdmaRegisters.Tokenaddress
#define MBDMA_TOKEN_ADDRESS		0x0008

// MbdmaGlobalCtl.Reg32
#define MBDMA_GLOBAL_CTL		0x000c
#define MBDMA_GLOBAL_CTL_FLUSH_CACHE	BIT(23)
#define MBDMA_GLOBAL_CTL_ALLOC_LIMIT_MASK	GENMASK(31, 24)
#define MBDMA_GLOBAL_CTL_ALLOC_LIMIT_6	(BIT(26) | BIT(25))
#define MBDMA_GLOBAL_CTL_LAN_TX_MSG_ID_3W_MASK	GENMASK(11, 6)
#define MBDMA_GLOBAL_CTL_LAN_TX_MSG_ID_3W_49	(BIT(11) | BIT(10) | BIT(6))
#define MBDMA_GLOBAL_CTL_LAN_TX_MSG_ID_2W_MASK	GENMASK(5, 0)
#define MBDMA_GLOBAL_CTL_LAN_TX_MSG_ID_2W_1	BIT(0)
#define MBDMA_GLOBAL_CTL_CLEAR		(MBDMA_GLOBAL_CTL_ALLOC_LIMIT_MASK | \
					 MBDMA_GLOBAL_CTL_FLUSH_CACHE | \
					 MBDMA_GLOBAL_CTL_LAN_TX_MSG_ID_3W_MASK | \
					 MBDMA_GLOBAL_CTL_LAN_TX_MSG_ID_2W_MASK)
#define MBDMA_GLOBAL_CTL_SET		(MBDMA_GLOBAL_CTL_ALLOC_LIMIT_6 | \
					 MBDMA_GLOBAL_CTL_LAN_TX_MSG_ID_3W_49 | \
					 MBDMA_GLOBAL_CTL_LAN_TX_MSG_ID_2W_1)

// MbdmaRegisters.Bufferbase
#define MBDMA_BUFFER_BASE		0x0010

// MbdmaBufferSize.Reg32
#define MBDMA_BUFFER_SIZE		0x0014

// MbdmaRxChanControl.Reg32
#define MBDMA_RX_CHAN_CONTROL0		0x0040
#define MBDMA_RX_MAX_BURST_MASK		GENMASK(28, 20)
#define MBDMA_RX_MAX_BURST_SHIFT	20
#define MBDMA_RX_MAX_BURST_64		(64 << MBDMA_RX_MAX_BURST_SHIFT)
#define MBDMA_RX_MSG_ID_MASK		GENMASK(17, 12)
#define MBDMA_RX_MSG_ID_SHIFT		12
#define MBDMA_RX_MSG_ID_0		(0 << MBDMA_RX_MSG_ID_SHIFT)
#define MBDMA_RX_MAC_ID_MASK		GENMASK(11, 8)
#define MBDMA_RX_MAC_ID_SHIFT		8
#define MBDMA_RX_MAC_ID(id)		((id) << MBDMA_RX_MAC_ID_SHIFT)
/*
 * GPL MbdmaRxChanControl fields:
 * StartStopFlush=0, EavMode=0, MaxBurst=64, AddrForward=0,
 * ErrForward=0, MsgId=0, MacId=<DTS mac-id>, Qos=0.
 */

// MbdmaRegisters.Lanmsgaddress0
#define MBDMA_LAN_MSG_ADDRESS0		0x0044

// MbdmaTxChanControl.Reg32
#define MBDMA_TX_CHAN_CONTROL1		0x0060
#define MBDMA_TX_MAX_BURST_MASK		GENMASK(28, 20)
#define MBDMA_TX_MAX_BURST_SHIFT	20
#define MBDMA_TX_MAX_BURST_64		(64 << MBDMA_TX_MAX_BURST_SHIFT)
#define MBDMA_TX_MSG_ID_MASK		GENMASK(13, 8)
#define MBDMA_TX_MSG_ID_SHIFT		8
#define MBDMA_TX_MSG_ID_1		(1 << MBDMA_TX_MSG_ID_SHIFT)
#define MBDMA_TX_MAC_ID_MASK		GENMASK(7, 4)
#define MBDMA_TX_MAC_ID_SHIFT		4
#define MBDMA_TX_MAC_ID(id)		((id) << MBDMA_TX_MAC_ID_SHIFT)
#define MBDMA_TX_MAX_REQS_MASK		GENMASK(3, 0)
#define MBDMA_TX_MAX_REQS_4		4
/*
 * GPL MbdmaTxChanControl fields:
 * StartStopFlush=0, EavMode=0, MaxBurst=64, TxStatOnError=0,
 * MsgId=1, MacId=<DTS mac-id>, MaxReqs=4.
 */

// MbdmaRegisters.Lanmsgaddress1
#define MBDMA_LAN_MSG_ADDRESS1		0x0064

// MbdmaRegisters.Lantxmsgfifo01
#define MBDMA_LAN_TX_MSG_FIFO01		0x0500

struct bcm3380_mbdma {
	struct unimac_mbdma api;
	struct device *dev;
	void __iomem *base;
	struct bcm3380_fpm_pool *fpm_pool;
	resource_size_t phys;
	bool ready;
};

static void mbdma_put_fpm_pool(void *data)
{
	fpm_pool_put(data);
}

static u32 bcm3380_mbdma_prepare(struct unimac_mbdma *api,
				 u32 in_msg_data_bus_addr, u32 mac_id)
{
	if (!api)
		return 0;
	if (mac_id > 0xf)
		return 0;

	struct bcm3380_mbdma *mbdma =
		container_of(api, struct bcm3380_mbdma, api);

	if (!mbdma || !mbdma->ready || !mbdma->fpm_pool || !in_msg_data_bus_addr)
		return 0;

	writel_be(fpm_buffer_base_dma(mbdma->fpm_pool), mbdma->base + MBDMA_BUFFER_BASE);
	writel_be(fpm_buffer_size_code(mbdma->fpm_pool), mbdma->base + MBDMA_BUFFER_SIZE);
	writel_be(fpm_alloc_free_bus_addr(mbdma->fpm_pool), mbdma->base + MBDMA_TOKEN_ADDRESS);

	u32 val = readl_be(mbdma->base + MBDMA_GLOBAL_CTL);
	writel_be(val | MBDMA_GLOBAL_CTL_FLUSH_CACHE, mbdma->base + MBDMA_GLOBAL_CTL);
	val = readl_be(mbdma->base + MBDMA_GLOBAL_CTL);
	writel_be((val & ~MBDMA_GLOBAL_CTL_CLEAR) | MBDMA_GLOBAL_CTL_SET,
		  mbdma->base + MBDMA_GLOBAL_CTL);
	val = readl_be(mbdma->base + MBDMA_TOKEN_CACHE_CTL);
	writel_be((val & ~MBDMA_TOKEN_CACHE_CLEAR) | MBDMA_TOKEN_CACHE_SET,
		  mbdma->base + MBDMA_TOKEN_CACHE_CTL);

	dev_info(mbdma->dev,
		 "UniMAC FPM pool1: bufferbase=0x%08x buffersize=0x%08x tokenaddress=0x%08x\n",
		 readl_be(mbdma->base + MBDMA_BUFFER_BASE),
		 readl_be(mbdma->base + MBDMA_BUFFER_SIZE),
		 readl_be(mbdma->base + MBDMA_TOKEN_ADDRESS));

	val = readl_be(mbdma->base + MBDMA_RX_CHAN_CONTROL0);
	writel_be((val & ~MBDMA_RX_MAX_BURST_MASK) | MBDMA_RX_MAX_BURST_64,
		  mbdma->base + MBDMA_RX_CHAN_CONTROL0);
	val = readl_be(mbdma->base + MBDMA_RX_CHAN_CONTROL0);
	writel_be((val & ~MBDMA_RX_MSG_ID_MASK) | MBDMA_RX_MSG_ID_0,
		  mbdma->base + MBDMA_RX_CHAN_CONTROL0);
	val = readl_be(mbdma->base + MBDMA_RX_CHAN_CONTROL0);
	writel_be((val & ~MBDMA_RX_MAC_ID_MASK) | MBDMA_RX_MAC_ID(mac_id),
		  mbdma->base + MBDMA_RX_CHAN_CONTROL0);
	writel_be(in_msg_data_bus_addr, mbdma->base + MBDMA_LAN_MSG_ADDRESS0);

	val = readl_be(mbdma->base + MBDMA_TX_CHAN_CONTROL1);
	writel_be((val & ~MBDMA_TX_MAX_BURST_MASK) | MBDMA_TX_MAX_BURST_64,
		  mbdma->base + MBDMA_TX_CHAN_CONTROL1);
	val = readl_be(mbdma->base + MBDMA_TX_CHAN_CONTROL1);
	writel_be((val & ~MBDMA_TX_MSG_ID_MASK) | MBDMA_TX_MSG_ID_1,
		  mbdma->base + MBDMA_TX_CHAN_CONTROL1);
	val = readl_be(mbdma->base + MBDMA_TX_CHAN_CONTROL1);
	writel_be((val & ~MBDMA_TX_MAC_ID_MASK) | MBDMA_TX_MAC_ID(mac_id),
		  mbdma->base + MBDMA_TX_CHAN_CONTROL1);
	val = readl_be(mbdma->base + MBDMA_TX_CHAN_CONTROL1);
	writel_be((val & ~MBDMA_TX_MAX_REQS_MASK) | MBDMA_TX_MAX_REQS_4,
		  mbdma->base + MBDMA_TX_CHAN_CONTROL1);
	writel_be(in_msg_data_bus_addr, mbdma->base + MBDMA_LAN_MSG_ADDRESS1);

	val = readl_be(mbdma->base + MBDMA_STATUS);
	writel_be((val & ~MBDMA_STATUS_CLEAR) | MBDMA_STATUS_SET,
		  mbdma->base + MBDMA_STATUS);

	return mbdma->phys + MBDMA_LAN_TX_MSG_FIFO01;
}

static bool bcm3380_mbdma_is_dev(struct unimac_mbdma *api, struct device *dev)
{
	if (!api)
		return false;

	struct bcm3380_mbdma *mbdma =
		container_of(api, struct bcm3380_mbdma, api);

	return mbdma->dev == dev;
}

static struct bcm3380_fpm_pool *
bcm3380_mbdma_get_fpm_pool(struct unimac_mbdma *api)
{
	if (!api)
		return NULL;

	struct bcm3380_mbdma *mbdma =
		container_of(api, struct bcm3380_mbdma, api);

	return mbdma->fpm_pool;
}

static int mbdma_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct bcm3380_mbdma *mbdma = devm_kzalloc(dev, sizeof(*mbdma),
						   GFP_KERNEL);
	if (!mbdma)
		return -ENOMEM;

	struct resource *res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	mbdma->base = devm_ioremap_resource(dev, res);
	if (IS_ERR(mbdma->base))
		return PTR_ERR(mbdma->base);

	mbdma->dev = dev;
	mbdma->phys = res->start;
	int ret = fpm_pool_get(dev, &mbdma->fpm_pool);
	if (ret)
		return dev_err_probe(dev, ret, "failed to get FPM pool provider\n");

	ret = devm_add_action_or_reset(dev, mbdma_put_fpm_pool, mbdma->fpm_pool);
	if (ret)
		return ret;

	mbdma->api.is_dev = bcm3380_mbdma_is_dev;
	mbdma->api.get_fpm_pool = bcm3380_mbdma_get_fpm_pool;
	mbdma->api.prepare = bcm3380_mbdma_prepare;
	mbdma->ready = true;
	platform_set_drvdata(pdev, &mbdma->api);

	dev_info(dev, "BCM3380 UniMAC MBDMA provider ready\n");

	return 0;
}

static const struct of_device_id bcm3380_mbdma_of_match[] = {
	{ .compatible = "brcm,bcm3380-mbdma" },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, bcm3380_mbdma_of_match);

static struct platform_driver bcm3380_mbdma_driver = {
	.probe = mbdma_probe,
	.driver = {
		.name = "bcm3380-mbdma",
		.of_match_table = bcm3380_mbdma_of_match,
	},
};
module_platform_driver(bcm3380_mbdma_driver);

MODULE_DESCRIPTION("BCM3380 UniMAC MBDMA provider");
MODULE_AUTHOR("Hang Zhou <929513338@qq.com>");
MODULE_LICENSE("GPL");
