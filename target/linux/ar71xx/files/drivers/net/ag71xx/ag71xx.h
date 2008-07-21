/*
 *  Atheros AR71xx built-in ethernet mac driver
 *
 *  Copyright (C) 2008 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  Based on Atheros' AG7100 driver
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#ifndef __AG71XX_H
#define __AG71XX_H

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/ethtool.h>
#include <linux/etherdevice.h>
#include <linux/phy.h>
#include <linux/skbuff.h>
#include <linux/dma-mapping.h>

#include <linux/bitops.h>

#include <asm/mach-ar71xx/ar71xx.h>
#include <asm/mach-ar71xx/platform.h>

#define ETH_FCS_LEN	4

#define AG71XX_DRV_NAME		"ag71xx"
#define AG71XX_DRV_VERSION	"0.3.9"

#define AG71XX_NAPI_TX		1

#define AG71XX_NAPI_WEIGHT	64

#define AG71XX_INT_ERR	(AG71XX_INT_RX_BE | AG71XX_INT_TX_BE)
#define AG71XX_INT_TX	(AG71XX_INT_TX_PS)
#define AG71XX_INT_RX	(AG71XX_INT_RX_PR | AG71XX_INT_RX_OF)

#ifdef AG71XX_NAPI_TX
#define AG71XX_INT_POLL	(AG71XX_INT_RX | AG71XX_INT_TX)
#define AG71XX_INT_INIT	(AG71XX_INT_ERR | AG71XX_INT_POLL)
#else
#define AG71XX_INT_POLL	(AG71XX_INT_RX)
#define AG71XX_INT_INIT	(AG71XX_INT_ERR | AG71XX_INT_POLL | AG71XX_INT_TX)
#endif

#define AG71XX_TX_FIFO_LEN	2048
#define AG71XX_TX_MTU_LEN	1536
#define AG71XX_RX_PKT_RESERVE	64
#define AG71XX_RX_PKT_SIZE	\
	(AG71XX_RX_PKT_RESERVE + ETH_HLEN + ETH_FRAME_LEN + ETH_FCS_LEN)

#define AG71XX_TX_RING_SIZE	64
#define AG71XX_TX_THRES_STOP	(AG71XX_TX_RING_SIZE - 4)
#define AG71XX_TX_THRES_WAKEUP	\
		(AG71XX_TX_RING_SIZE - (AG71XX_TX_RING_SIZE / 4))

#define AG71XX_RX_RING_SIZE	128

#undef DEBUG
#ifdef DEBUG
#define DBG(fmt, args...)	printk(KERN_DEBUG fmt, ## args)
#else
#define DBG(fmt, args...)	do {} while (0)
#endif

#define ag71xx_assert(_cond)						\
do {									\
	if (_cond)							\
		break;							\
	printk("%s,%d: assertion failed\n", __FILE__, __LINE__);	\
	BUG();								\
} while (0)

struct ag71xx_desc {
	u32	data;
	u32	ctrl;
#define DESC_EMPTY	BIT(31)
#define DESC_MORE	BIT(24)
#define DESC_PKTLEN_M	0x1fff
	u32	next;
};

struct ag71xx_buf {
	struct sk_buff	*skb;
};

struct ag71xx_ring {
	struct ag71xx_buf	*buf;
	struct ag71xx_desc	*descs;
	dma_addr_t		descs_dma;
	unsigned int		curr;
	unsigned int		dirty;
	unsigned int		size;
};

struct ag71xx {
	void __iomem		*mac_base;
	void __iomem		*mii_ctrl;

	spinlock_t		lock;
	struct platform_device	*pdev;
	struct net_device	*dev;
	struct napi_struct	napi;

	struct ag71xx_ring	rx_ring;
	struct ag71xx_ring	tx_ring;

	struct phy_device	*phy_dev;
	struct mii_bus		mii_bus;

	unsigned int		link;
	unsigned int		speed;
	int 			duplex;
};

extern struct ethtool_ops ag71xx_ethtool_ops;

extern int ag71xx_mdio_init(struct ag71xx *ag, int id);
extern void ag71xx_mdio_cleanup(struct ag71xx *ag);
extern int ag71xx_mii_peek(struct ag71xx *ag);
extern void ag71xx_mii_ctrl_set_if(struct ag71xx *ag, unsigned int mii_if);
extern void ag71xx_mii_ctrl_set_speed(struct ag71xx *ag, unsigned int speed);
extern void ag71xx_link_update(struct ag71xx *ag);

static inline struct ag71xx_platform_data *ag71xx_get_pdata(struct ag71xx *ag)
{
	return ag->pdev->dev.platform_data;
}

static inline void ag71xx_wr(struct ag71xx *ag, unsigned reg, u32 value)
{
	__raw_writel(value, ag->mac_base + reg);
}

static inline u32 ag71xx_rr(struct ag71xx *ag, unsigned reg)
{
	return __raw_readl(ag->mac_base + reg);
}

static inline void ag71xx_sb(struct ag71xx *ag, unsigned reg, u32 mask)
{
	void __iomem *r = ag->mac_base + reg;

	__raw_writel(__raw_readl(r) | mask, r);
}

static inline void ag71xx_cb(struct ag71xx *ag, unsigned reg, u32 mask)
{
	void __iomem *r = ag->mac_base + reg;

	__raw_writel(__raw_readl(r) & ~mask, r);
}

static inline int ag71xx_desc_empty(struct ag71xx_desc *desc)
{
	return ((desc->ctrl & DESC_EMPTY) != 0);
}

static inline int ag71xx_desc_pktlen(struct ag71xx_desc *desc)
{
	return (desc->ctrl & DESC_PKTLEN_M);
}

/* Register offsets */
#define AG71XX_REG_MAC_CFG1	0x0000
#define AG71XX_REG_MAC_CFG2	0x0004
#define AG71XX_REG_MAC_IPG	0x0008
#define AG71XX_REG_MAC_HDX	0x000c
#define AG71XX_REG_MAC_MFL	0x0010
#define AG71XX_REG_MII_CFG	0x0020
#define AG71XX_REG_MII_CMD	0x0024
#define AG71XX_REG_MII_ADDR	0x0028
#define AG71XX_REG_MII_CTRL	0x002c
#define AG71XX_REG_MII_STATUS	0x0030
#define AG71XX_REG_MII_IND	0x0034
#define AG71XX_REG_MAC_IFCTL	0x0038
#define AG71XX_REG_MAC_ADDR1	0x0040
#define AG71XX_REG_MAC_ADDR2	0x0044
#define AG71XX_REG_FIFO_CFG0	0x0048
#define AG71XX_REG_FIFO_CFG1	0x004c
#define AG71XX_REG_FIFO_CFG2	0x0050
#define AG71XX_REG_FIFO_CFG3	0x0054
#define AG71XX_REG_FIFO_CFG4	0x0058
#define AG71XX_REG_FIFO_CFG5	0x005c
#define AG71XX_REG_FIFO_RAM0	0x0060
#define AG71XX_REG_FIFO_RAM1	0x0064
#define AG71XX_REG_FIFO_RAM2	0x0068
#define AG71XX_REG_FIFO_RAM3	0x006c
#define AG71XX_REG_FIFO_RAM4	0x0070
#define AG71XX_REG_FIFO_RAM5	0x0074
#define AG71XX_REG_FIFO_RAM6	0x0078
#define AG71XX_REG_FIFO_RAM7	0x007c

#define AG71XX_REG_TX_CTRL	0x0180
#define AG71XX_REG_TX_DESC	0x0184
#define AG71XX_REG_TX_STATUS	0x0188
#define AG71XX_REG_RX_CTRL	0x018c
#define AG71XX_REG_RX_DESC	0x0190
#define AG71XX_REG_RX_STATUS	0x0194
#define AG71XX_REG_INT_ENABLE	0x0198
#define AG71XX_REG_INT_STATUS	0x019c

#define MAC_CFG1_TXE		BIT(0)
#define MAC_CFG1_STX		BIT(1)
#define MAC_CFG1_RXE		BIT(2)
#define MAC_CFG1_SRX		BIT(3)
#define MAC_CFG1_LB		BIT(8)
#define MAC_CFG1_SR		BIT(31)

#define MAC_CFG2_FDX		BIT(0)
#define MAC_CFG2_CRC_EN		BIT(1)
#define MAC_CFG2_PAD_CRC_EN	BIT(2)
#define MAC_CFG2_LEN_CHECK	BIT(4)
#define MAC_CFG2_HUGE_FRAME_EN	BIT(5)
#define MAC_CFG2_IF_1000	BIT(9)
#define MAC_CFG2_IF_10_100	BIT(8)

#define AG71XX_INT_TX_PS	BIT(0)
#define AG71XX_INT_TX_UR	BIT(1)
#define AG71XX_INT_TX_BE	BIT(3)
#define AG71XX_INT_RX_PR	BIT(4)
#define AG71XX_INT_RX_OF	BIT(6)
#define AG71XX_INT_RX_BE	BIT(7)

#define MAC_IFCTL_SPEED		BIT(16)

#define MII_CFG_CLK_DIV_4	0
#define MII_CFG_CLK_DIV_6	2
#define MII_CFG_CLK_DIV_8	3
#define MII_CFG_CLK_DIV_10	4
#define MII_CFG_CLK_DIV_14	5
#define MII_CFG_CLK_DIV_20	6
#define MII_CFG_CLK_DIV_28	7

#define MII_CMD_WRITE		0x0
#define MII_CMD_READ		0x1
#define MII_ADDR_S		8
#define MII_IND_BUSY		BIT(0)
#define MII_IND_INVALID		BIT(2)

#define TX_CTRL_TXE		BIT(0)

#define TX_STATUS_PS		BIT(0)
#define TX_STATUS_UR		BIT(1)
#define TX_STATUS_BE		BIT(3)

#define RX_CTRL_RXE		BIT(0)

#define RX_STATUS_PR		BIT(0)
#define RX_STATUS_OF		BIT(1)
#define RX_STATUS_BE		BIT(3)

#define FIFO_CFG5_BYTE_PER_CLK	BIT(19)

#define MII_CTRL_SPEED_S	4
#define MII_CTRL_SPEED_M	3
#define MII_CTRL_SPEED_10	0
#define MII_CTRL_SPEED_100	1
#define MII_CTRL_SPEED_1000	2

static inline void ag71xx_int_enable(struct ag71xx *ag, u32 ints)
{
	ag71xx_sb(ag, AG71XX_REG_INT_ENABLE, ints);
}

static inline void ag71xx_int_disable(struct ag71xx *ag, u32 ints)
{
	ag71xx_cb(ag, AG71XX_REG_INT_ENABLE, ints);
}

#endif /* _AG71XX_H */
