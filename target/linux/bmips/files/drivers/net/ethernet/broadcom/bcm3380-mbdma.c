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

#define MBDMA_MAC_ID			0

// MbdmaStatus.Reg32
#define MBDMA_STATUS			0x0000
#define MBDMA_STATUS_GBL_INTR_MASK	BIT(31)
#define MBDMA_STATUS_INTR_MASK		GENMASK(25, 16)
#define MBDMA_STATUS_INTR_MASK_SET	(BIT(25) | BIT(23) | BIT(22) | BIT(21) | BIT(16))
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
#define MBDMA_RX_MAX_BURST_KEEP		0xe00fffff
#define MBDMA_RX_MAX_BURST_VALUE	0x04000000
#define MBDMA_RX_MAC_ID_KEEP		0xfffc0fff
#define MBDMA_RX_MSG_ID_KEEP		0xfffff0ff
#define MBDMA_RX_MSG_ID_VALUE		0x00000500

// MbdmaRegisters.Lanmsgaddress0
#define MBDMA_LAN_MSG_ADDRESS0		0x0044

// MbdmaTxChanControl.Reg32
#define MBDMA_TX_CHAN_CONTROL1		0x0060
#define MBDMA_TX_MAX_BURST_KEEP		0xe00fffff
#define MBDMA_TX_MAX_BURST_VALUE	0x04000000
#define MBDMA_TX_MSG_ID_KEEP		0xffffc0ff
#define MBDMA_TX_MSG_ID_VALUE		0x00000100
#define MBDMA_TX_MAC_ID_KEEP		0xffffff0f
#define MBDMA_TX_MAC_ID_VALUE		0x00000090
#define MBDMA_TX_MAX_REQS_KEEP		0xfffffff0
#define MBDMA_TX_MAX_REQS_VALUE		0x00000004

// MbdmaRegisters.Lanmsgaddress1
#define MBDMA_LAN_MSG_ADDRESS1		0x0064

// MbdmaRegisters.Lantxmsgfifo01
#define MBDMA_LAN_TX_MSG_FIFO01		0x0500

struct bcm3380_mbdma {
	struct unimac_mbdma api;
	struct device *dev;
	void __iomem *base;
	resource_size_t phys;
	bool ready;
};

static int bcm3380_mbdma_prepare(struct unimac_mbdma *api,
				 struct bcm3380_fpm_pool *fpm_pool,
				 u32 in_msg_data_bus_addr)
{
	if (!api)
		return -EINVAL;

	struct bcm3380_mbdma *mbdma =
		container_of(api, struct bcm3380_mbdma, api);

	if (!mbdma || !mbdma->ready || !fpm_pool || !in_msg_data_bus_addr)
		return -EINVAL;

	writel_be(fpm_buffer_base_dma(fpm_pool), mbdma->base + MBDMA_BUFFER_BASE);
	writel_be(fpm_buffer_size_code(fpm_pool), mbdma->base + MBDMA_BUFFER_SIZE);
	writel_be(fpm_alloc_free_bus_addr(fpm_pool), mbdma->base + MBDMA_TOKEN_ADDRESS);

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
	writel_be((val & MBDMA_RX_MAX_BURST_KEEP) | MBDMA_RX_MAX_BURST_VALUE,
		  mbdma->base + MBDMA_RX_CHAN_CONTROL0);
	val = readl_be(mbdma->base + MBDMA_RX_CHAN_CONTROL0);
	writel_be(val & MBDMA_RX_MAC_ID_KEEP, mbdma->base + MBDMA_RX_CHAN_CONTROL0);
	val = readl_be(mbdma->base + MBDMA_RX_CHAN_CONTROL0);
	writel_be((val & MBDMA_RX_MSG_ID_KEEP) | MBDMA_RX_MSG_ID_VALUE,
		  mbdma->base + MBDMA_RX_CHAN_CONTROL0);
	writel_be(in_msg_data_bus_addr, mbdma->base + MBDMA_LAN_MSG_ADDRESS0);

	val = readl_be(mbdma->base + MBDMA_TX_CHAN_CONTROL1);
	writel_be((val & MBDMA_TX_MAX_BURST_KEEP) | MBDMA_TX_MAX_BURST_VALUE,
		  mbdma->base + MBDMA_TX_CHAN_CONTROL1);
	val = readl_be(mbdma->base + MBDMA_TX_CHAN_CONTROL1);
	writel_be((val & MBDMA_TX_MSG_ID_KEEP) | MBDMA_TX_MSG_ID_VALUE,
		  mbdma->base + MBDMA_TX_CHAN_CONTROL1);
	val = readl_be(mbdma->base + MBDMA_TX_CHAN_CONTROL1);
	writel_be((val & MBDMA_TX_MAC_ID_KEEP) | MBDMA_TX_MAC_ID_VALUE,
		  mbdma->base + MBDMA_TX_CHAN_CONTROL1);
	val = readl_be(mbdma->base + MBDMA_TX_CHAN_CONTROL1);
	writel_be((val & MBDMA_TX_MAX_REQS_KEEP) | MBDMA_TX_MAX_REQS_VALUE,
		  mbdma->base + MBDMA_TX_CHAN_CONTROL1);
	writel_be(in_msg_data_bus_addr, mbdma->base + MBDMA_LAN_MSG_ADDRESS1);

	val = readl_be(mbdma->base + MBDMA_STATUS);
	writel_be((val & ~MBDMA_STATUS_CLEAR) | MBDMA_STATUS_SET,
		  mbdma->base + MBDMA_STATUS);

	return 0;
}

static u32 bcm3380_mbdma_tx_fifo_bus_addr(struct unimac_mbdma *api, u32 mac_id)
{
	if (!api)
		return 0;

	struct bcm3380_mbdma *mbdma =
		container_of(api, struct bcm3380_mbdma, api);

	if (!mbdma || !mbdma->ready || mac_id != MBDMA_MAC_ID)
		return 0;

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
	mbdma->api.is_dev = bcm3380_mbdma_is_dev;
	mbdma->api.prepare = bcm3380_mbdma_prepare;
	mbdma->api.tx_fifo_bus_addr = bcm3380_mbdma_tx_fifo_bus_addr;
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
