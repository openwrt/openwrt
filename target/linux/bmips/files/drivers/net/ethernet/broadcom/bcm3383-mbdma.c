// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * BCM3383 shared UniMAC MBDMA provider
 *
 * Copyright (C) 2026 Hang Zhou <929513338@qq.com>
 */

#include <linux/bits.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/reset.h>
#include <linux/slab.h>
#include <linux/sysfs.h>

#include <soc/bcm/bcm3380-fpm.h>
#include <soc/bcm/bcm3383-mbdma.h>

// MbdmaStatus.Reg32
#define MBDMA_STATUS					0x0000
#define MBDMA_STATUS_TX_MSGQ_OVERFLOW			BIT(9)
#define MBDMA_STATUS_INVALID_TX_MSG			BIT(8)
#define MBDMA_STATUS_UBUS_ERROR				BIT(6)
#define MBDMA_STATUS_TOKEN_RD_ERROR			BIT(5)
#define MBDMA_STATUS_INVALID_TOKEN			BIT(4)
#define MBDMA_STATUS_ALLOC_FIFO_EMPTY			BIT(3)
#define MBDMA_STATUS_FREE_FIFO_FULL			BIT(0)

// MbdmaTokenCacheCtl.Reg32
#define MBDMA_TOKEN_CACHE_CTL				0x0004
#define MBDMA_TOKEN_CACHE_CTL_BOOTLOADER		0x00009010

// MbdmaRegisters.Tokenaddress
#define MBDMA_TOKEN_ADDRESS				0x0008

// MbdmaGlobalCtl.Reg32
#define MBDMA_GLOBAL_CTL				0x000c
#define MBDMA_GLOBAL_CTL_BOOTLOADER			0x00000081

// MbdmaRegisters.Bufferbase
#define MBDMA_BUFFER_BASE				0x0010

// MbdmaRegisters.Tokencachectl2
#define MBDMA_TOKEN_CACHE_CTL2				0x0044
#define MBDMA_TOKEN_CACHE_CTL2_BURST_2			0x02020202

// MbdmaRegisters.Tokencachectl3
#define MBDMA_TOKEN_CACHE_CTL3				0x0048
#define MBDMA_TOKEN_CACHE_CTL3_ALLOC_ENABLE_ALL		0x0000000f

// MbdmaRegisters.Tokenaddress256/512/1k/2k
#define MBDMA_TOKEN_ADDRESS_256				0x004c
#define MBDMA_TOKEN_ADDRESS_512				0x0050
#define MBDMA_TOKEN_ADDRESS_1K				0x0054
#define MBDMA_TOKEN_ADDRESS_2K				0x0058

// MbdmaRxChanControl.Reg32
#define MBDMA_RX_CHAN_CONTROL00				0x0100
#define MBDMA_RX_CHAN_CONTROL01				0x0120
#define MBDMA_RX_CHAN_CONTROL_VALUE			0x01000001
#define MBDMA_RX_CHAN_CONTROL_MAC_ID_SHIFT		8

// MbdmaRegisters.Lanmsgaddress00/01
#define MBDMA_LAN_MSG_ADDRESS00			0x0104
#define MBDMA_LAN_MSG_ADDRESS01			0x0124
#define MBDMA_RX_CHAN_STATUS00				0x0108
#define MBDMA_RX_CHAN_STATUS01				0x0128

// MbdmaTxChanControl.Reg32
#define MBDMA_TX_CHAN_CONTROL02				0x0140
#define MBDMA_TX_CHAN_CONTROL03				0x0160
#define MBDMA_TX_CHAN_CONTROL04				0x0180
#define MBDMA_TX_CHAN_CONTROL05				0x01a0
#define MBDMA_TX_CHAN_CONTROL_VALUE			0x01000301
#define MBDMA_TX_CHAN_CONTROL_MAC_ID_SHIFT		4

// MbdmaRegisters.Lanmsgaddress02..05
#define MBDMA_LAN_MSG_ADDRESS02			0x0144
#define MBDMA_LAN_MSG_ADDRESS03			0x0164
#define MBDMA_LAN_MSG_ADDRESS04			0x0184
#define MBDMA_LAN_MSG_ADDRESS05			0x01a4
#define MBDMA_TX_CHAN_CONTROL022			0x0148
#define MBDMA_TX_CHAN_CONTROL032			0x0168
#define MBDMA_TX_CHAN_CONTROL042			0x0188
#define MBDMA_TX_CHAN_CONTROL052			0x01a8

// MbdmaRegisters.Lantxmsgfifo02..05
#define MBDMA_LAN_TX_MSG_FIFO02			0x0500
#define MBDMA_LAN_TX_MSG_FIFO03			0x0508
#define MBDMA_LAN_TX_MSG_FIFO04			0x0510
#define MBDMA_LAN_TX_MSG_FIFO05			0x0518

struct bcm3383_mbdma {
	struct unimac_mbdma api;
	struct device *dev;
	void __iomem *base;
	resource_size_t phys;
	struct mutex lock;
	struct clk_bulk_data *clocks;
	int num_clocks;
	bool ready;
};

static void mbdma_disable_clocks(void *data)
{
	struct bcm3383_mbdma *mbdma = data;

	clk_bulk_disable_unprepare(mbdma->num_clocks, mbdma->clocks);
}

static ssize_t status_show(struct device *dev, struct device_attribute *attr,
			   char *buf)
{
	struct unimac_mbdma *api = dev_get_drvdata(dev);

	if (!api)
		return sysfs_emit(buf, "not ready\n");

	struct bcm3383_mbdma *mbdma =
		container_of(api, struct bcm3383_mbdma, api);
	ssize_t len = 0;

	if (!mbdma || !mbdma->ready)
		return sysfs_emit(buf, "not ready\n");

	len += sysfs_emit_at(buf, len, "status: 0x%08x\n",
			     readl_be(mbdma->base + MBDMA_STATUS));
	len += sysfs_emit_at(buf, len, "global_ctl: 0x%08x\n",
			     readl_be(mbdma->base + MBDMA_GLOBAL_CTL));
	len += sysfs_emit_at(buf, len, "token_cache_ctl: 0x%08x\n",
			     readl_be(mbdma->base + MBDMA_TOKEN_CACHE_CTL));
	len += sysfs_emit_at(buf, len, "token_cache_ctl2: 0x%08x\n",
			     readl_be(mbdma->base + MBDMA_TOKEN_CACHE_CTL2));
	len += sysfs_emit_at(buf, len, "token_cache_ctl3: 0x%08x\n",
			     readl_be(mbdma->base + MBDMA_TOKEN_CACHE_CTL3));
	len += sysfs_emit_at(buf, len, "buffer_base: 0x%08x\n",
			     readl_be(mbdma->base + MBDMA_BUFFER_BASE));
	len += sysfs_emit_at(buf, len, "token_free: 0x%08x\n",
			     readl_be(mbdma->base + MBDMA_TOKEN_ADDRESS));
	len += sysfs_emit_at(buf, len, "token_256: 0x%08x\n",
			     readl_be(mbdma->base + MBDMA_TOKEN_ADDRESS_256));
	len += sysfs_emit_at(buf, len, "token_512: 0x%08x\n",
			     readl_be(mbdma->base + MBDMA_TOKEN_ADDRESS_512));
	len += sysfs_emit_at(buf, len, "token_1k: 0x%08x\n",
			     readl_be(mbdma->base + MBDMA_TOKEN_ADDRESS_1K));
	len += sysfs_emit_at(buf, len, "token_2k: 0x%08x\n",
			     readl_be(mbdma->base + MBDMA_TOKEN_ADDRESS_2K));
	len += sysfs_emit_at(buf, len,
			     "rx00_ctl: 0x%08x rx00_msg: 0x%08x rx00_sts: 0x%08x\n",
			     readl_be(mbdma->base + MBDMA_RX_CHAN_CONTROL00),
			     readl_be(mbdma->base + MBDMA_LAN_MSG_ADDRESS00),
			     readl_be(mbdma->base + MBDMA_RX_CHAN_STATUS00));
	len += sysfs_emit_at(buf, len,
			     "rx01_ctl: 0x%08x rx01_msg: 0x%08x rx01_sts: 0x%08x\n",
			     readl_be(mbdma->base + MBDMA_RX_CHAN_CONTROL01),
			     readl_be(mbdma->base + MBDMA_LAN_MSG_ADDRESS01),
			     readl_be(mbdma->base + MBDMA_RX_CHAN_STATUS01));
	len += sysfs_emit_at(buf, len,
			     "tx02_ctl: 0x%08x tx02_msg: 0x%08x tx02_ctl2: 0x%08x fifo: 0x%08llx\n",
			     readl_be(mbdma->base + MBDMA_TX_CHAN_CONTROL02),
			     readl_be(mbdma->base + MBDMA_LAN_MSG_ADDRESS02),
			     readl_be(mbdma->base + MBDMA_TX_CHAN_CONTROL022),
			     (unsigned long long)(mbdma->phys + MBDMA_LAN_TX_MSG_FIFO02));
	len += sysfs_emit_at(buf, len,
			     "tx03_ctl: 0x%08x tx03_msg: 0x%08x tx03_ctl2: 0x%08x fifo: 0x%08llx\n",
			     readl_be(mbdma->base + MBDMA_TX_CHAN_CONTROL03),
			     readl_be(mbdma->base + MBDMA_LAN_MSG_ADDRESS03),
			     readl_be(mbdma->base + MBDMA_TX_CHAN_CONTROL032),
			     (unsigned long long)(mbdma->phys + MBDMA_LAN_TX_MSG_FIFO03));
	len += sysfs_emit_at(buf, len,
			     "tx04_ctl: 0x%08x tx04_msg: 0x%08x tx04_ctl2: 0x%08x fifo: 0x%08llx\n",
			     readl_be(mbdma->base + MBDMA_TX_CHAN_CONTROL04),
			     readl_be(mbdma->base + MBDMA_LAN_MSG_ADDRESS04),
			     readl_be(mbdma->base + MBDMA_TX_CHAN_CONTROL042),
			     (unsigned long long)(mbdma->phys + MBDMA_LAN_TX_MSG_FIFO04));
	len += sysfs_emit_at(buf, len,
			     "tx05_ctl: 0x%08x tx05_msg: 0x%08x tx05_ctl2: 0x%08x fifo: 0x%08llx\n",
			     readl_be(mbdma->base + MBDMA_TX_CHAN_CONTROL05),
			     readl_be(mbdma->base + MBDMA_LAN_MSG_ADDRESS05),
			     readl_be(mbdma->base + MBDMA_TX_CHAN_CONTROL052),
			     (unsigned long long)(mbdma->phys + MBDMA_LAN_TX_MSG_FIFO05));

	return len;
}
static DEVICE_ATTR_RO(status);

static struct attribute *mbdma_attrs[] = {
	&dev_attr_status.attr,
	NULL,
};

static const struct attribute_group mbdma_attr_group = {
	.attrs = mbdma_attrs,
};

static void mbdma_write_tx_channel(struct bcm3383_mbdma *mbdma, u32 control,
				   u32 address, u32 mac_id,
				   u32 in_msg_data_bus_addr)
{
	writel_be(MBDMA_TX_CHAN_CONTROL_VALUE |
		  (mac_id << MBDMA_TX_CHAN_CONTROL_MAC_ID_SHIFT),
		  mbdma->base + control);
	writel_be(in_msg_data_bus_addr, mbdma->base + address);
}

static u32 bcm3383_mbdma_prepare(struct unimac_mbdma *api,
				 struct bcm3380_fpm_pool *fpm_pool,
				 u32 in_msg_data_bus_addr, u32 mac_id)
{
	if (!api)
		return 0;
	if (mac_id > 1)
		return 0;

	struct bcm3383_mbdma *mbdma =
		container_of(api, struct bcm3383_mbdma, api);

	if (!mbdma || !mbdma->ready || !fpm_pool || !in_msg_data_bus_addr)
		return 0;

	mutex_lock(&mbdma->lock);

	u32 tx_fifo_bus_addr;

	writel_be(fpm_buffer_base_dma(fpm_pool), mbdma->base + MBDMA_BUFFER_BASE);
	writel_be(fpm_alloc_free_bus_addr_for_size(fpm_pool, 2048),
		  mbdma->base + MBDMA_TOKEN_ADDRESS);
	writel_be(fpm_alloc_free_bus_addr_for_size(fpm_pool, 256),
		  mbdma->base + MBDMA_TOKEN_ADDRESS_256);
	writel_be(fpm_alloc_free_bus_addr_for_size(fpm_pool, 512),
		  mbdma->base + MBDMA_TOKEN_ADDRESS_512);
	writel_be(fpm_alloc_free_bus_addr_for_size(fpm_pool, 1024),
		  mbdma->base + MBDMA_TOKEN_ADDRESS_1K);
	writel_be(fpm_alloc_free_bus_addr_for_size(fpm_pool, 2048),
		  mbdma->base + MBDMA_TOKEN_ADDRESS_2K);
	writel_be(MBDMA_TOKEN_CACHE_CTL_BOOTLOADER,
		  mbdma->base + MBDMA_TOKEN_CACHE_CTL);
	writel_be(MBDMA_TOKEN_CACHE_CTL2_BURST_2,
		  mbdma->base + MBDMA_TOKEN_CACHE_CTL2);
	writel_be(MBDMA_TOKEN_CACHE_CTL3_ALLOC_ENABLE_ALL,
		  mbdma->base + MBDMA_TOKEN_CACHE_CTL3);
	writel_be(MBDMA_GLOBAL_CTL_BOOTLOADER, mbdma->base + MBDMA_GLOBAL_CTL);

	if (mac_id == 0) {
		writel_be(MBDMA_RX_CHAN_CONTROL_VALUE |
			  (mac_id << MBDMA_RX_CHAN_CONTROL_MAC_ID_SHIFT),
			  mbdma->base + MBDMA_RX_CHAN_CONTROL00);
		writel_be(in_msg_data_bus_addr, mbdma->base + MBDMA_LAN_MSG_ADDRESS00);
		mbdma_write_tx_channel(mbdma, MBDMA_TX_CHAN_CONTROL02,
				       MBDMA_LAN_MSG_ADDRESS02, mac_id,
				       in_msg_data_bus_addr);
		tx_fifo_bus_addr = mbdma->phys + MBDMA_LAN_TX_MSG_FIFO02;
	} else {
		writel_be(MBDMA_RX_CHAN_CONTROL_VALUE |
			  (mac_id << MBDMA_RX_CHAN_CONTROL_MAC_ID_SHIFT),
			  mbdma->base + MBDMA_RX_CHAN_CONTROL01);
		writel_be(in_msg_data_bus_addr, mbdma->base + MBDMA_LAN_MSG_ADDRESS01);
		mbdma_write_tx_channel(mbdma, MBDMA_TX_CHAN_CONTROL04,
				       MBDMA_LAN_MSG_ADDRESS04, mac_id,
				       in_msg_data_bus_addr);
		tx_fifo_bus_addr = mbdma->phys + MBDMA_LAN_TX_MSG_FIFO04;
	}

	u32 status = readl_be(mbdma->base + MBDMA_STATUS);
	writel_be((status & ~GENMASK(16, 0)) | GENMASK(16, 0),
		  mbdma->base + MBDMA_STATUS);

	dev_info(mbdma->dev,
		 "BCM3383 MBDMA ready: bufferbase=0x%08x global=0x%08x token_cache=0x%08x token_cache2=0x%08x token_cache3=0x%08x\n",
		 readl_be(mbdma->base + MBDMA_BUFFER_BASE),
		 readl_be(mbdma->base + MBDMA_GLOBAL_CTL),
		 readl_be(mbdma->base + MBDMA_TOKEN_CACHE_CTL),
		 readl_be(mbdma->base + MBDMA_TOKEN_CACHE_CTL2),
		 readl_be(mbdma->base + MBDMA_TOKEN_CACHE_CTL3));

	mutex_unlock(&mbdma->lock);

	return tx_fifo_bus_addr;
}

static bool bcm3383_mbdma_is_dev(struct unimac_mbdma *api, struct device *dev)
{
	if (!api)
		return false;

	struct bcm3383_mbdma *mbdma =
		container_of(api, struct bcm3383_mbdma, api);

	return mbdma->dev == dev;
}

static int mbdma_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct bcm3383_mbdma *mbdma = devm_kzalloc(dev, sizeof(*mbdma),
						   GFP_KERNEL);
	if (!mbdma)
		return -ENOMEM;

	struct resource *res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	mbdma->base = devm_ioremap_resource(dev, res);
	if (IS_ERR(mbdma->base))
		return PTR_ERR(mbdma->base);

	mbdma->num_clocks = devm_clk_bulk_get_all(dev, &mbdma->clocks);
	if (mbdma->num_clocks < 0)
		return mbdma->num_clocks;

	int ret = clk_bulk_prepare_enable(mbdma->num_clocks, mbdma->clocks);
	if (ret)
		return dev_err_probe(dev, ret, "failed to enable shared UniMAC clocks\n");

	ret = devm_add_action_or_reset(dev, mbdma_disable_clocks, mbdma);
	if (ret)
		return ret;

	struct reset_control_bulk_data resets[] = {
		{ .id = "unimac" },
	};
	ret = devm_reset_control_bulk_get_exclusive(dev, ARRAY_SIZE(resets), resets);
	if (ret)
		return dev_err_probe(dev, ret, "failed to get shared UniMAC resets\n");

	ret = reset_control_bulk_assert(ARRAY_SIZE(resets), resets);
	if (ret)
		return dev_err_probe(dev, ret, "failed to assert shared UniMAC resets\n");
	usleep_range(1000, 2000);

	ret = reset_control_bulk_deassert(ARRAY_SIZE(resets), resets);
	if (ret)
		return dev_err_probe(dev, ret, "failed to deassert shared UniMAC resets\n");
	usleep_range(10000, 20000);

	mbdma->dev = dev;
	mbdma->phys = res->start;
	mbdma->api.is_dev = bcm3383_mbdma_is_dev;
	mbdma->api.prepare = bcm3383_mbdma_prepare;
	mutex_init(&mbdma->lock);
	mbdma->ready = true;
	platform_set_drvdata(pdev, &mbdma->api);

	ret = sysfs_create_group(&dev->kobj, &mbdma_attr_group);
	if (ret)
		return ret;

	dev_info(dev, "BCM3383 shared UniMAC MBDMA provider ready\n");

	return 0;
}

static void mbdma_remove(struct platform_device *pdev)
{
	sysfs_remove_group(&pdev->dev.kobj, &mbdma_attr_group);
}

static const struct of_device_id mbdma_of_match[] = {
	{ .compatible = "brcm,bcm3383-mbdma" },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, mbdma_of_match);

static struct platform_driver mbdma_driver = {
	.probe = mbdma_probe,
	.remove = mbdma_remove,
	.driver = {
		.name = "bcm3383-mbdma",
		.of_match_table = mbdma_of_match,
	},
};
module_platform_driver(mbdma_driver);

MODULE_AUTHOR("Hang Zhou <929513338@qq.com>");
MODULE_DESCRIPTION("BCM3383 shared UniMAC MBDMA provider");
MODULE_LICENSE("GPL v2");
