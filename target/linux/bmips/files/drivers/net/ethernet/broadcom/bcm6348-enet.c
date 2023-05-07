// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * BCM6348 Ethernet Controller Driver
 *
 * Copyright (C) 2020 Álvaro Fernández Rojas <noltari@gmail.com>
 * Copyright (C) 2015 Jonas Gorski <jonas.gorski@gmail.com>
 * Copyright (C) 2008 Maxime Bizon <mbizon@freebox.fr>
 */

#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/err.h>
#include <linux/etherdevice.h>
#include <linux/if_vlan.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/mtd/mtd.h>
#include <linux/of_address.h>
#include <linux/of_clk.h>
#include <linux/of_irq.h>
#include <linux/of_mdio.h>
#include <linux/of_net.h>
#include <linux/of_platform.h>
#include <linux/phy.h>
#include <linux/platform_device.h>
#include <linux/reset.h>

/* DMA channels */
#define DMA_CHAN_WIDTH			0x10

/* Controller Configuration Register */
#define DMA_CFG_REG			0x0
#define DMA_CFG_EN_SHIFT		0
#define DMA_CFG_EN_MASK			(1 << DMA_CFG_EN_SHIFT)
#define DMA_CFG_FLOWCH_MASK(x)		(1 << ((x >> 1) + 1))

/* Flow Control Descriptor Low Threshold register */
#define DMA_FLOWCL_REG(x)		(0x4 + (x) * 6)

/* Flow Control Descriptor High Threshold register */
#define DMA_FLOWCH_REG(x)		(0x8 + (x) * 6)

/* Flow Control Descriptor Buffer Alloca Threshold register */
#define DMA_BUFALLOC_REG(x)		(0xc + (x) * 6)
#define DMA_BUFALLOC_FORCE_SHIFT	31
#define DMA_BUFALLOC_FORCE_MASK		(1 << DMA_BUFALLOC_FORCE_SHIFT)

/* Channel Configuration register */
#define DMAC_CHANCFG_REG		0x0
#define DMAC_CHANCFG_EN_SHIFT		0
#define DMAC_CHANCFG_EN_MASK		(1 << DMAC_CHANCFG_EN_SHIFT)
#define DMAC_CHANCFG_PKTHALT_SHIFT	1
#define DMAC_CHANCFG_PKTHALT_MASK	(1 << DMAC_CHANCFG_PKTHALT_SHIFT)
#define DMAC_CHANCFG_BUFHALT_SHIFT	2
#define DMAC_CHANCFG_BUFHALT_MASK	(1 << DMAC_CHANCFG_BUFHALT_SHIFT)
#define DMAC_CHANCFG_CHAINING_SHIFT	2
#define DMAC_CHANCFG_CHAINING_MASK	(1 << DMAC_CHANCFG_CHAINING_SHIFT)
#define DMAC_CHANCFG_WRAP_EN_SHIFT	3
#define DMAC_CHANCFG_WRAP_EN_MASK	(1 << DMAC_CHANCFG_WRAP_EN_SHIFT)
#define DMAC_CHANCFG_FLOWC_EN_SHIFT	4
#define DMAC_CHANCFG_FLOWC_EN_MASK	(1 << DMAC_CHANCFG_FLOWC_EN_SHIFT)

/* Interrupt Control/Status register */
#define DMAC_IR_REG			0x4
#define DMAC_IR_BUFDONE_MASK		(1 << 0)
#define DMAC_IR_PKTDONE_MASK		(1 << 1)
#define DMAC_IR_NOTOWNER_MASK		(1 << 2)

/* Interrupt Mask register */
#define DMAC_IRMASK_REG			0x8

/* Maximum Burst Length */
#define DMAC_MAXBURST_REG		0xc

/* Ring Start Address register */
#define DMAS_RSTART_REG			0x0

/* State Ram Word 2 */
#define DMAS_SRAM2_REG			0x4

/* State Ram Word 3 */
#define DMAS_SRAM3_REG			0x8

/* State Ram Word 4 */
#define DMAS_SRAM4_REG			0xc

struct bcm6348_iudma_desc {
	u32 len_stat;
	u32 address;
};

/* control */
#define DMADESC_LENGTH_SHIFT		16
#define DMADESC_LENGTH_MASK		(0xfff << DMADESC_LENGTH_SHIFT)
#define DMADESC_OWNER_MASK		(1 << 15)
#define DMADESC_EOP_MASK		(1 << 14)
#define DMADESC_SOP_MASK		(1 << 13)
#define DMADESC_ESOP_MASK		(DMADESC_EOP_MASK | DMADESC_SOP_MASK)
#define DMADESC_WRAP_MASK		(1 << 12)

/* status */
#define DMADESC_UNDER_MASK		(1 << 9)
#define DMADESC_APPEND_CRC		(1 << 8)
#define DMADESC_OVSIZE_MASK		(1 << 4)
#define DMADESC_RXER_MASK		(1 << 2)
#define DMADESC_CRC_MASK		(1 << 1)
#define DMADESC_OV_MASK			(1 << 0)
#define DMADESC_ERR_MASK		(DMADESC_UNDER_MASK | \
					 DMADESC_OVSIZE_MASK | \
					 DMADESC_RXER_MASK | \
					 DMADESC_CRC_MASK | \
					 DMADESC_OV_MASK)

struct bcm6348_iudma {
	void __iomem *dma_base;
	void __iomem *dma_chan;
	void __iomem *dma_sram;

	spinlock_t dma_base_lock;

	struct clk **clock;
	unsigned int num_clocks;

	struct reset_control **reset;
	unsigned int num_resets;

	unsigned int dma_channels;
};

int bcm6348_iudma_drivers_register(struct platform_device *pdev);

static inline u32 dma_readl(struct bcm6348_iudma *iudma, u32 off)
{
	u32 val;

	spin_lock(&iudma->dma_base_lock);
	val = __raw_readl(iudma->dma_base + off);
	spin_unlock(&iudma->dma_base_lock);

	return val;
}

static inline void dma_writel(struct bcm6348_iudma *iudma, u32 val, u32 off)
{
	spin_lock(&iudma->dma_base_lock);
	__raw_writel(val, iudma->dma_base + off);
	spin_unlock(&iudma->dma_base_lock);
}

static inline u32 dmac_readl(struct bcm6348_iudma *iudma, u32 off, int chan)
{
	return __raw_readl(iudma->dma_chan + chan * DMA_CHAN_WIDTH + off);
}

static inline void dmac_writel(struct bcm6348_iudma *iudma, u32 val, u32 off,
			       int chan)
{
	__raw_writel(val, iudma->dma_chan + chan * DMA_CHAN_WIDTH + off);
}

static inline void dmas_writel(struct bcm6348_iudma *iudma, u32 val, u32 off,
			       int chan)
{
	__raw_writel(val, iudma->dma_sram + chan * DMA_CHAN_WIDTH + off);
}

static void bcm6348_iudma_chan_stop(struct bcm6348_iudma *iudma, int chan)
{
	int limit = 1000;

	dmac_writel(iudma, 0, DMAC_CHANCFG_REG, chan);

	do {
		u32 val;

		val = dmac_readl(iudma, DMAC_CHANCFG_REG, chan);
		if (!(val & DMAC_CHANCFG_EN_MASK))
			break;

		udelay(1);
	} while (limit--);
}

static int bcm6348_iudma_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *node = dev->of_node;
	struct bcm6348_iudma *iudma;
	unsigned i;
	int num_resets;
	int ret;

	iudma = devm_kzalloc(dev, sizeof(*iudma), GFP_KERNEL);
	if (!iudma)
		return -ENOMEM;

	if (of_property_read_u32(node, "dma-channels", &iudma->dma_channels))
		return -ENODEV;

	iudma->dma_base = devm_platform_ioremap_resource_byname(pdev, "dma");
	if (IS_ERR_OR_NULL(iudma->dma_base))
		return PTR_ERR(iudma->dma_base);

	iudma->dma_chan = devm_platform_ioremap_resource_byname(pdev,
								"dma-channels");
	if (IS_ERR_OR_NULL(iudma->dma_chan))
		return PTR_ERR(iudma->dma_chan);

	iudma->dma_sram = devm_platform_ioremap_resource_byname(pdev,
								"dma-sram");
	if (IS_ERR_OR_NULL(iudma->dma_sram))
		return PTR_ERR(iudma->dma_sram);

	iudma->num_clocks = of_clk_get_parent_count(node);
	if (iudma->num_clocks) {
		iudma->clock = devm_kcalloc(dev, iudma->num_clocks,
					    sizeof(struct clk *), GFP_KERNEL);
		if (IS_ERR_OR_NULL(iudma->clock))
			return PTR_ERR(iudma->clock);
	}
	for (i = 0; i < iudma->num_clocks; i++) {
		iudma->clock[i] = of_clk_get(node, i);
		if (IS_ERR_OR_NULL(iudma->clock[i])) {
			dev_err(dev, "error getting iudma clock %d\n", i);
			return PTR_ERR(iudma->clock[i]);
		}

		ret = clk_prepare_enable(iudma->clock[i]);
		if (ret) {
			dev_err(dev, "error enabling iudma clock %d\n", i);
			return ret;
		}
	}

	num_resets = of_count_phandle_with_args(node, "resets",
						"#reset-cells");
	if (num_resets > 0)
		iudma->num_resets = num_resets;
	else
		iudma->num_resets = 0;
	if (iudma->num_resets) {
		iudma->reset = devm_kcalloc(dev, iudma->num_resets,
					    sizeof(struct reset_control *),
					    GFP_KERNEL);
		if (IS_ERR_OR_NULL(iudma->reset))
			return PTR_ERR(iudma->reset);
	}
	for (i = 0; i < iudma->num_resets; i++) {
		iudma->reset[i] = devm_reset_control_get_by_index(dev, i);
		if (IS_ERR_OR_NULL(iudma->reset[i])) {
			dev_err(dev, "error getting iudma reset %d\n", i);
			return PTR_ERR(iudma->reset[i]);
		}

		ret = reset_control_reset(iudma->reset[i]);
		if (ret) {
			dev_err(dev, "error performing iudma reset %d\n", i);
			return ret;
		}
	}

	dma_writel(iudma, 0, DMA_CFG_REG);
	for (i = 0; i < iudma->dma_channels; i++)
		bcm6348_iudma_chan_stop(iudma, i);
	dma_writel(iudma, DMA_CFG_EN_MASK, DMA_CFG_REG);

	spin_lock_init(&iudma->dma_base_lock);

	dev_info(dev, "bcm6348-iudma @ 0x%px\n", iudma->dma_base);

	platform_set_drvdata(pdev, iudma);

	return bcm6348_iudma_drivers_register(pdev);
}

static const struct of_device_id bcm6348_iudma_of_match[] = {
	{ .compatible = "brcm,bcm6338-iudma", },
	{ .compatible = "brcm,bcm6348-iudma", },
	{ .compatible = "brcm,bcm6358-iudma", },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, bcm6348_emac_of_match);

static struct platform_driver bcm6348_iudma_driver = {
	.driver = {
		.name = "bcm6348-iudma",
		.of_match_table = of_match_ptr(bcm6348_iudma_of_match),
	},
	.probe	= bcm6348_iudma_probe,
};
module_platform_driver(bcm6348_iudma_driver);

/*
 * BCM6348 Eternet MACs
 */

/* MTU */
#define ENET_MAX_MTU			2046

#define ENET_TAG_SIZE			6
#define ENET_MTU_OVERHEAD		(VLAN_ETH_HLEN + VLAN_HLEN + \
					 ENET_TAG_SIZE)

/* Default number of descriptor */
#define ENET_DEF_RX_DESC		64
#define ENET_DEF_TX_DESC		32
#define ENET_DEF_CPY_BREAK		128

/* Maximum burst len for dma (4 bytes unit) */
#define ENET_DMA_MAXBURST		8

/* Receiver Configuration register */
#define ENET_RXCFG_REG			0x0
#define ENET_RXCFG_ALLMCAST_SHIFT	1
#define ENET_RXCFG_ALLMCAST_MASK	(1 << ENET_RXCFG_ALLMCAST_SHIFT)
#define ENET_RXCFG_PROMISC_SHIFT	3
#define ENET_RXCFG_PROMISC_MASK		(1 << ENET_RXCFG_PROMISC_SHIFT)
#define ENET_RXCFG_LOOPBACK_SHIFT	4
#define ENET_RXCFG_LOOPBACK_MASK	(1 << ENET_RXCFG_LOOPBACK_SHIFT)
#define ENET_RXCFG_ENFLOW_SHIFT		5
#define ENET_RXCFG_ENFLOW_MASK		(1 << ENET_RXCFG_ENFLOW_SHIFT)

/* Receive Maximum Length register */
#define ENET_RXMAXLEN_REG		0x4
#define ENET_RXMAXLEN_SHIFT		0
#define ENET_RXMAXLEN_MASK		(0x7ff << ENET_RXMAXLEN_SHIFT)

/* Transmit Maximum Length register */
#define ENET_TXMAXLEN_REG		0x8
#define ENET_TXMAXLEN_SHIFT		0
#define ENET_TXMAXLEN_MASK		(0x7ff << ENET_TXMAXLEN_SHIFT)

/* MII Status/Control register */
#define ENET_MIISC_REG			0x10
#define ENET_MIISC_MDCFREQDIV_SHIFT	0
#define ENET_MIISC_MDCFREQDIV_MASK	(0x7f << ENET_MIISC_MDCFREQDIV_SHIFT)
#define ENET_MIISC_PREAMBLEEN_SHIFT	7
#define ENET_MIISC_PREAMBLEEN_MASK	(1 << ENET_MIISC_PREAMBLEEN_SHIFT)

/* MII Data register */
#define ENET_MIID_REG			0x14
#define ENET_MIID_DATA_SHIFT		0
#define ENET_MIID_DATA_MASK		(0xffff << ENET_MIID_DATA_SHIFT)
#define ENET_MIID_TA_SHIFT		16
#define ENET_MIID_TA_MASK		(0x3 << ENET_MIID_TA_SHIFT)
#define ENET_MIID_REG_SHIFT		18
#define ENET_MIID_REG_MASK		(0x1f << ENET_MIID_REG_SHIFT)
#define ENET_MIID_PHY_SHIFT		23
#define ENET_MIID_PHY_MASK		(0x1f << ENET_MIID_PHY_SHIFT)
#define ENET_MIID_OP_SHIFT		28
#define ENET_MIID_OP_WRITE		(0x5 << ENET_MIID_OP_SHIFT)
#define ENET_MIID_OP_READ		(0x6 << ENET_MIID_OP_SHIFT)

/* Ethernet Interrupt Mask register */
#define ENET_IRMASK_REG			0x18

/* Ethernet Interrupt register */
#define ENET_IR_REG			0x1c
#define ENET_IR_MII			BIT(0)
#define ENET_IR_MIB			BIT(1)
#define ENET_IR_FLOWC			BIT(2)

/* Ethernet Control register */
#define ENET_CTL_REG			0x2c
#define ENET_CTL_ENABLE_SHIFT		0
#define ENET_CTL_ENABLE_MASK		(1 << ENET_CTL_ENABLE_SHIFT)
#define ENET_CTL_DISABLE_SHIFT		1
#define ENET_CTL_DISABLE_MASK		(1 << ENET_CTL_DISABLE_SHIFT)
#define ENET_CTL_SRESET_SHIFT		2
#define ENET_CTL_SRESET_MASK		(1 << ENET_CTL_SRESET_SHIFT)
#define ENET_CTL_EPHYSEL_SHIFT		3
#define ENET_CTL_EPHYSEL_MASK		(1 << ENET_CTL_EPHYSEL_SHIFT)

/* Transmit Control register */
#define ENET_TXCTL_REG			0x30
#define ENET_TXCTL_FD_SHIFT		0
#define ENET_TXCTL_FD_MASK		(1 << ENET_TXCTL_FD_SHIFT)

/* Transmit Watermask register */
#define ENET_TXWMARK_REG		0x34
#define ENET_TXWMARK_WM_SHIFT		0
#define ENET_TXWMARK_WM_MASK		(0x3f << ENET_TXWMARK_WM_SHIFT)

/* MIB Control register */
#define ENET_MIBCTL_REG			0x38
#define ENET_MIBCTL_RDCLEAR_SHIFT	0
#define ENET_MIBCTL_RDCLEAR_MASK	(1 << ENET_MIBCTL_RDCLEAR_SHIFT)

/* Perfect Match Data Low register */
#define ENET_PML_REG(x)			(0x58 + (x) * 8)
#define ENET_PMH_REG(x)			(0x5c + (x) * 8)
#define ENET_PMH_DATAVALID_SHIFT	16
#define ENET_PMH_DATAVALID_MASK		(1 << ENET_PMH_DATAVALID_SHIFT)

/* MIB register */
#define ENET_MIB_REG(x)			(0x200 + (x) * 4)
#define ENET_MIB_REG_COUNT		55

/*
 * TX transmit threshold (4 bytes unit), FIFO is 256 bytes, the value
 * must be low enough so that a DMA transfer of above burst length can
 * not overflow the fifo
 */
#define ENET_TX_FIFO_TRESH		32

struct bcm6348_emac {
	struct bcm6348_iudma *iudma;
	void __iomem *base;

	struct clk **clock;
	unsigned int num_clocks;

	struct reset_control **reset;
	unsigned int num_resets;

	int copybreak;

	int irq_rx;
	int irq_tx;

	/* hw view of rx & tx dma ring */
	dma_addr_t rx_desc_dma;
	dma_addr_t tx_desc_dma;

	/* allocated size (in bytes) for rx & tx dma ring */
	unsigned int rx_desc_alloc_size;
	unsigned int tx_desc_alloc_size;

	struct napi_struct napi;

	/* dma channel id for rx */
	int rx_chan;

	/* number of dma desc in rx ring */
	int rx_ring_size;

	/* cpu view of rx dma ring */
	struct bcm6348_iudma_desc *rx_desc_cpu;

	/* current number of armed descriptor given to hardware for rx */
	int rx_desc_count;

	/* next rx descriptor to fetch from hardware */
	int rx_curr_desc;

	/* next dirty rx descriptor to refill */
	int rx_dirty_desc;

	/* size of allocated rx skbs */
	unsigned int rx_skb_size;

	/* list of skb given to hw for rx */
	struct sk_buff **rx_skb;

	/* used when rx skb allocation failed, so we defer rx queue
	 * refill */
	struct timer_list rx_timeout;

	/* lock rx_timeout against rx normal operation */
	spinlock_t rx_lock;

	/* dma channel id for tx */
	int tx_chan;

	/* number of dma desc in tx ring */
	int tx_ring_size;

	/* cpu view of tx dma ring */
	struct bcm6348_iudma_desc *tx_desc_cpu;

	/* number of available descriptor for tx */
	int tx_desc_count;

	/* next tx descriptor avaiable */
	int tx_curr_desc;

	/* next dirty tx descriptor to reclaim */
	int tx_dirty_desc;

	/* list of skb given to hw for tx */
	struct sk_buff **tx_skb;

	/* lock used by tx reclaim and xmit */
	spinlock_t tx_lock;

	/* network device reference */
	struct net_device *net_dev;

	/* platform device reference */
	struct platform_device *pdev;

	/* external mii bus */
	bool ext_mii;

	/* phy */
	int old_link;
	int old_duplex;
	int old_pause;
};

static inline void emac_writel(struct bcm6348_emac *emac, u32 val, u32 off)
{
	__raw_writel(val, emac->base + off);
}

static inline u32 emac_readl(struct bcm6348_emac *emac, u32 off)
{
	return __raw_readl(emac->base + off);
}

/*
 * refill rx queue
 */
static int bcm6348_emac_refill_rx(struct net_device *ndev)
{
	struct bcm6348_emac *emac = netdev_priv(ndev);
	struct bcm6348_iudma *iudma = emac->iudma;
	struct platform_device *pdev = emac->pdev;
	struct device *dev = &pdev->dev;

	while (emac->rx_desc_count < emac->rx_ring_size) {
		struct bcm6348_iudma_desc *desc;
		struct sk_buff *skb;
		dma_addr_t p;
		int desc_idx;
		u32 len_stat;

		desc_idx = emac->rx_dirty_desc;
		desc = &emac->rx_desc_cpu[desc_idx];

		if (!emac->rx_skb[desc_idx]) {
			skb = netdev_alloc_skb(ndev, emac->rx_skb_size);
			if (!skb)
				break;
			emac->rx_skb[desc_idx] = skb;
			p = dma_map_single(dev, skb->data, emac->rx_skb_size,
					   DMA_FROM_DEVICE);
			desc->address = p;
		}

		len_stat = emac->rx_skb_size << DMADESC_LENGTH_SHIFT;
		len_stat |= DMADESC_OWNER_MASK;
		if (emac->rx_dirty_desc == emac->rx_ring_size - 1) {
			len_stat |= DMADESC_WRAP_MASK;
			emac->rx_dirty_desc = 0;
		} else {
			emac->rx_dirty_desc++;
		}
		wmb();
		desc->len_stat = len_stat;

		emac->rx_desc_count++;

		/* tell dma engine we allocated one buffer */
		dma_writel(iudma, 1, DMA_BUFALLOC_REG(emac->rx_chan));
	}

	/* If rx ring is still empty, set a timer to try allocating
	 * again at a later time. */
	if (emac->rx_desc_count == 0 && netif_running(ndev)) {
		dev_warn(dev, "unable to refill rx ring\n");
		emac->rx_timeout.expires = jiffies + HZ;
		add_timer(&emac->rx_timeout);
	}

	return 0;
}

/*
 * timer callback to defer refill rx queue in case we're OOM
 */
static void bcm6348_emac_refill_rx_timer(struct timer_list *t)
{
	struct bcm6348_emac *emac = from_timer(emac, t, rx_timeout);
	struct net_device *ndev = emac->net_dev;

	spin_lock(&emac->rx_lock);
	bcm6348_emac_refill_rx(ndev);
	spin_unlock(&emac->rx_lock);
}

/*
 * extract packet from rx queue
 */
static int bcm6348_emac_receive_queue(struct net_device *ndev, int budget)
{
	struct bcm6348_emac *emac = netdev_priv(ndev);
	struct bcm6348_iudma *iudma = emac->iudma;
	struct platform_device *pdev = emac->pdev;
	struct device *dev = &pdev->dev;
	int processed = 0;

	/* don't scan ring further than number of refilled
	 * descriptor */
	if (budget > emac->rx_desc_count)
		budget = emac->rx_desc_count;

	do {
		struct bcm6348_iudma_desc *desc;
		struct sk_buff *skb;
		int desc_idx;
		u32 len_stat;
		unsigned int len;

		desc_idx = emac->rx_curr_desc;
		desc = &emac->rx_desc_cpu[desc_idx];

		/* make sure we actually read the descriptor status at
		 * each loop */
		rmb();

		len_stat = desc->len_stat;

		/* break if dma ownership belongs to hw */
		if (len_stat & DMADESC_OWNER_MASK)
			break;

		processed++;
		emac->rx_curr_desc++;
		if (emac->rx_curr_desc == emac->rx_ring_size)
			emac->rx_curr_desc = 0;
		emac->rx_desc_count--;

		/* if the packet does not have start of packet _and_
		 * end of packet flag set, then just recycle it */
		if ((len_stat & DMADESC_ESOP_MASK) != DMADESC_ESOP_MASK) {
			ndev->stats.rx_dropped++;
			continue;
		}

		/* valid packet */
		skb = emac->rx_skb[desc_idx];
		len = (len_stat & DMADESC_LENGTH_MASK)
		      >> DMADESC_LENGTH_SHIFT;
		/* don't include FCS */
		len -= 4;

		if (len < emac->copybreak) {
			struct sk_buff *nskb;

			nskb = napi_alloc_skb(&emac->napi, len);
			if (!nskb) {
				/* forget packet, just rearm desc */
				ndev->stats.rx_dropped++;
				continue;
			}

			dma_sync_single_for_cpu(dev, desc->address,
						len, DMA_FROM_DEVICE);
			memcpy(nskb->data, skb->data, len);
			dma_sync_single_for_device(dev, desc->address,
						   len, DMA_FROM_DEVICE);
			skb = nskb;
		} else {
			dma_unmap_single(dev, desc->address,
					 emac->rx_skb_size, DMA_FROM_DEVICE);
			emac->rx_skb[desc_idx] = NULL;
		}

		skb_put(skb, len);
		skb->protocol = eth_type_trans(skb, ndev);
		ndev->stats.rx_packets++;
		ndev->stats.rx_bytes += len;
		netif_receive_skb(skb);
	} while (--budget > 0);

	if (processed || !emac->rx_desc_count) {
		bcm6348_emac_refill_rx(ndev);

		/* kick rx dma */
		dmac_writel(iudma, DMAC_CHANCFG_EN_MASK, DMAC_CHANCFG_REG,
			    emac->rx_chan);
	}

	return processed;
}

/*
 * try to or force reclaim of transmitted buffers
 */
static int bcm6348_emac_tx_reclaim(struct net_device *ndev, int force)
{
	struct bcm6348_emac *emac = netdev_priv(ndev);
	struct platform_device *pdev = emac->pdev;
	struct device *dev = &pdev->dev;
	int released = 0;

	while (emac->tx_desc_count < emac->tx_ring_size) {
		struct bcm6348_iudma_desc *desc;
		struct sk_buff *skb;

		/* We run in a bh and fight against start_xmit, which
		 * is called with bh disabled */
		spin_lock(&emac->tx_lock);

		desc = &emac->tx_desc_cpu[emac->tx_dirty_desc];

		if (!force && (desc->len_stat & DMADESC_OWNER_MASK)) {
			spin_unlock(&emac->tx_lock);
			break;
		}

		/* ensure other field of the descriptor were not read
		 * before we checked ownership */
		rmb();

		skb = emac->tx_skb[emac->tx_dirty_desc];
		emac->tx_skb[emac->tx_dirty_desc] = NULL;
		dma_unmap_single(dev, desc->address, skb->len, DMA_TO_DEVICE);

		emac->tx_dirty_desc++;
		if (emac->tx_dirty_desc == emac->tx_ring_size)
			emac->tx_dirty_desc = 0;
		emac->tx_desc_count++;

		spin_unlock(&emac->tx_lock);

		if (desc->len_stat & DMADESC_UNDER_MASK)
			ndev->stats.tx_errors++;

		dev_kfree_skb(skb);
		released++;
	}

	if (netif_queue_stopped(ndev) && released)
		netif_wake_queue(ndev);

	return released;
}

static int bcm6348_emac_poll(struct napi_struct *napi, int budget)
{
	struct bcm6348_emac *emac = container_of(napi, struct bcm6348_emac,
						 napi);
	struct bcm6348_iudma *iudma = emac->iudma;
	struct net_device *ndev = emac->net_dev;
	int rx_work_done;

	/* ack interrupts */
	dmac_writel(iudma, DMAC_IR_PKTDONE_MASK, DMAC_IR_REG,
		    emac->rx_chan);
	dmac_writel(iudma, DMAC_IR_PKTDONE_MASK, DMAC_IR_REG,
		    emac->tx_chan);

	/* reclaim sent skb */
	bcm6348_emac_tx_reclaim(ndev, 0);

	spin_lock(&emac->rx_lock);
	rx_work_done = bcm6348_emac_receive_queue(ndev, budget);
	spin_unlock(&emac->rx_lock);

	if (rx_work_done >= budget) {
		/* rx queue is not yet empty/clean */
		return rx_work_done;
	}

	/* no more packet in rx/tx queue, remove device from poll
	 * queue */
	napi_complete_done(napi, rx_work_done);

	/* restore rx/tx interrupt */
	dmac_writel(iudma, DMAC_IR_PKTDONE_MASK, DMAC_IRMASK_REG,
		    emac->rx_chan);
	dmac_writel(iudma, DMAC_IR_PKTDONE_MASK, DMAC_IRMASK_REG,
		    emac->tx_chan);

	return rx_work_done;
}

/*
 * emac interrupt handler
 */
static irqreturn_t bcm6348_emac_isr_mac(int irq, void *dev_id)
{
	struct net_device *ndev = dev_id;
	struct bcm6348_emac *emac = netdev_priv(ndev);
	u32 stat;

	stat = emac_readl(emac, ENET_IR_REG);
	if (!(stat & ENET_IR_MIB))
		return IRQ_NONE;

	/* clear & mask interrupt */
	emac_writel(emac, ENET_IR_MIB, ENET_IR_REG);
	emac_writel(emac, 0, ENET_IRMASK_REG);

	return IRQ_HANDLED;
}

/*
 * rx/tx dma interrupt handler
 */
static irqreturn_t bcm6348_emac_isr_dma(int irq, void *dev_id)
{
	struct net_device *ndev = dev_id;
	struct bcm6348_emac *emac = netdev_priv(ndev);
	struct bcm6348_iudma *iudma = emac->iudma;

	/* mask rx/tx interrupts */
	dmac_writel(iudma, 0, DMAC_IRMASK_REG, emac->rx_chan);
	dmac_writel(iudma, 0, DMAC_IRMASK_REG, emac->tx_chan);

	napi_schedule(&emac->napi);

	return IRQ_HANDLED;
}

/*
 * tx request callback
 */
static netdev_tx_t bcm6348_emac_start_xmit(struct sk_buff *skb,
					   struct net_device *ndev)
{
	struct bcm6348_emac *emac = netdev_priv(ndev);
	struct bcm6348_iudma *iudma = emac->iudma;
	struct platform_device *pdev = emac->pdev;
	struct device *dev = &pdev->dev;
	struct bcm6348_iudma_desc *desc;
	u32 len_stat;
	netdev_tx_t ret;

	/* lock against tx reclaim */
	spin_lock(&emac->tx_lock);

	/* make sure the tx hw queue is not full, should not happen
	 * since we stop queue before it's the case */
	if (unlikely(!emac->tx_desc_count)) {
		netif_stop_queue(ndev);
		dev_err(dev, "xmit called with no tx desc available?\n");
		ret = NETDEV_TX_BUSY;
		goto out_unlock;
	}

	/* point to the next available desc */
	desc = &emac->tx_desc_cpu[emac->tx_curr_desc];
	emac->tx_skb[emac->tx_curr_desc] = skb;

	/* fill descriptor */
	desc->address = dma_map_single(dev, skb->data, skb->len,
				       DMA_TO_DEVICE);

	len_stat = (skb->len << DMADESC_LENGTH_SHIFT) & DMADESC_LENGTH_MASK;
	len_stat |= DMADESC_ESOP_MASK | DMADESC_APPEND_CRC |
		    DMADESC_OWNER_MASK;

	emac->tx_curr_desc++;
	if (emac->tx_curr_desc == emac->tx_ring_size) {
		emac->tx_curr_desc = 0;
		len_stat |= DMADESC_WRAP_MASK;
	}
	emac->tx_desc_count--;

	/* dma might be already polling, make sure we update desc
	 * fields in correct order */
	wmb();
	desc->len_stat = len_stat;
	wmb();

	/* kick tx dma */
	dmac_writel(iudma, DMAC_CHANCFG_EN_MASK, DMAC_CHANCFG_REG,
		    emac->tx_chan);

	/* stop queue if no more desc available */
	if (!emac->tx_desc_count)
		netif_stop_queue(ndev);

	ndev->stats.tx_bytes += skb->len;
	ndev->stats.tx_packets++;
	ret = NETDEV_TX_OK;

out_unlock:
	spin_unlock(&emac->tx_lock);
	return ret;
}

/*
 * Change the interface's emac address.
 */
static int bcm6348_emac_set_mac_address(struct net_device *ndev, void *p)
{
	struct bcm6348_emac *emac = netdev_priv(ndev);
	struct sockaddr *addr = p;
	u32 val;

	eth_hw_addr_set(ndev, addr->sa_data);

	/* use perfect match register 0 to store my emac address */
	val = (ndev->dev_addr[2] << 24) | (ndev->dev_addr[3] << 16) |
		(ndev->dev_addr[4] << 8) | ndev->dev_addr[5];
	emac_writel(emac, val, ENET_PML_REG(0));

	val = (ndev->dev_addr[0] << 8 | ndev->dev_addr[1]);
	val |= ENET_PMH_DATAVALID_MASK;
	emac_writel(emac, val, ENET_PMH_REG(0));

	return 0;
}

/*
 * Change rx mode (promiscuous/allmulti) and update multicast list
 */
static void bcm6348_emac_set_multicast_list(struct net_device *ndev)
{
	struct bcm6348_emac *emac = netdev_priv(ndev);
	struct netdev_hw_addr *ha;
	u32 val;
	unsigned int i;

	val = emac_readl(emac, ENET_RXCFG_REG);

	if (ndev->flags & IFF_PROMISC)
		val |= ENET_RXCFG_PROMISC_MASK;
	else
		val &= ~ENET_RXCFG_PROMISC_MASK;

	/* only 3 perfect match registers left, first one is used for
	 * own mac address */
	if ((ndev->flags & IFF_ALLMULTI) || netdev_mc_count(ndev) > 3)
		val |= ENET_RXCFG_ALLMCAST_MASK;
	else
		val &= ~ENET_RXCFG_ALLMCAST_MASK;

	/* no need to set perfect match registers if we catch all
	 * multicast */
	if (val & ENET_RXCFG_ALLMCAST_MASK) {
		emac_writel(emac, val, ENET_RXCFG_REG);
		return;
	}

	i = 0;
	netdev_for_each_mc_addr(ha, ndev) {
		u8 *dmi_addr;
		u32 tmp;

		if (i == 3)
			break;

		/* update perfect match registers */
		dmi_addr = ha->addr;
		tmp = (dmi_addr[2] << 24) | (dmi_addr[3] << 16) |
			(dmi_addr[4] << 8) | dmi_addr[5];
		emac_writel(emac, tmp, ENET_PML_REG(i + 1));

		tmp = (dmi_addr[0] << 8 | dmi_addr[1]);
		tmp |= ENET_PMH_DATAVALID_MASK;
		emac_writel(emac, tmp, ENET_PMH_REG(i++ + 1));
	}

	for (; i < 3; i++) {
		emac_writel(emac, 0, ENET_PML_REG(i + 1));
		emac_writel(emac, 0, ENET_PMH_REG(i + 1));
	}

	emac_writel(emac, val, ENET_RXCFG_REG);
}

/*
 * disable emac
 */
static void bcm6348_emac_disable_mac(struct bcm6348_emac *emac)
{
	int limit;
	u32 val;

	val = emac_readl(emac, ENET_CTL_REG);
	val |= ENET_CTL_DISABLE_MASK;
	emac_writel(emac, val, ENET_CTL_REG);

	limit = 1000;
	do {
		val = emac_readl(emac, ENET_CTL_REG);
		if (!(val & ENET_CTL_DISABLE_MASK))
			break;
		udelay(1);
	} while (limit--);
}

/*
 * set emac duplex parameters
 */
static void bcm6348_emac_set_duplex(struct bcm6348_emac *emac, int fullduplex)
{
	u32 val;

	val = emac_readl(emac, ENET_TXCTL_REG);
	if (fullduplex)
		val |= ENET_TXCTL_FD_MASK;
	else
		val &= ~ENET_TXCTL_FD_MASK;
	emac_writel(emac, val, ENET_TXCTL_REG);
}

/*
 * set emac flow control parameters
 */
static void bcm6348_emac_set_flow(struct bcm6348_emac *emac, bool rx_en, bool tx_en)
{
	struct bcm6348_iudma *iudma = emac->iudma;
	u32 val;

	val = emac_readl(emac, ENET_RXCFG_REG);
	if (rx_en)
		val |= ENET_RXCFG_ENFLOW_MASK;
	else
		val &= ~ENET_RXCFG_ENFLOW_MASK;
	emac_writel(emac, val, ENET_RXCFG_REG);

	dmas_writel(iudma, emac->rx_desc_dma, DMAS_RSTART_REG, emac->rx_chan);
	dmas_writel(iudma, emac->tx_desc_dma, DMAS_RSTART_REG, emac->tx_chan);

	val = dma_readl(iudma, DMA_CFG_REG);
	if (tx_en)
		val |= DMA_CFG_FLOWCH_MASK(emac->rx_chan);
	else
		val &= ~DMA_CFG_FLOWCH_MASK(emac->rx_chan);
	dma_writel(iudma, val, DMA_CFG_REG);
}

/*
 * adjust emac phy
 */
static void bcm6348_emac_adjust_phy(struct net_device *ndev)
{
	struct phy_device *phydev = ndev->phydev;
	struct bcm6348_emac *emac = netdev_priv(ndev);
	struct platform_device *pdev = emac->pdev;
	struct device *dev = &pdev->dev;
	bool status_changed = false;

	if (emac->old_link != phydev->link) {
		status_changed = true;
		emac->old_link = phydev->link;
	}

	if (phydev->link && phydev->duplex != emac->old_duplex) {
		bcm6348_emac_set_duplex(emac, phydev->duplex == DUPLEX_FULL);
		status_changed = true;
		emac->old_duplex = phydev->duplex;
	}

	if (phydev->link && phydev->pause != emac->old_pause) {
		bool rx_pause_en, tx_pause_en;

		if (phydev->pause) {
			rx_pause_en = true;
			tx_pause_en = true;
		} else {
			rx_pause_en = false;
			tx_pause_en = false;
		}

		bcm6348_emac_set_flow(emac, rx_pause_en, tx_pause_en);
		status_changed = true;
		emac->old_pause = phydev->pause;
	}

	if (status_changed)
		dev_info(dev, "%s: phy link %s %s/%s/%s/%s\n",
			 ndev->name,
			 phydev->link ? "UP" : "DOWN",
			 phy_modes(phydev->interface),
			 phy_speed_to_str(phydev->speed),
			 phy_duplex_to_str(phydev->duplex),
			 phydev->pause ? "rx/tx" : "off");
}


static int bcm6348_emac_open(struct net_device *ndev)
{
	struct bcm6348_emac *emac = netdev_priv(ndev);
	struct bcm6348_iudma *iudma = emac->iudma;
	struct platform_device *pdev = emac->pdev;
	struct device *dev = &pdev->dev;
	struct sockaddr addr;
	unsigned int i, size;
	int ret;
	void *p;
	u32 val;

	/* mask all interrupts and request them */
	emac_writel(emac, 0, ENET_IRMASK_REG);
	dmac_writel(iudma, 0, DMAC_IRMASK_REG, emac->rx_chan);
	dmac_writel(iudma, 0, DMAC_IRMASK_REG, emac->tx_chan);

	ret = request_irq(ndev->irq, bcm6348_emac_isr_mac, 0, ndev->name,
			  ndev);
	if (ret)
		return ret;

	ret = request_irq(emac->irq_rx, bcm6348_emac_isr_dma,
			  0, ndev->name, ndev);
	if (ret)
		goto out_freeirq;

	ret = request_irq(emac->irq_tx, bcm6348_emac_isr_dma,
			  0, ndev->name, ndev);
	if (ret)
		goto out_freeirq_rx;

	/* initialize perfect match registers */
	for (i = 0; i < 4; i++) {
		emac_writel(emac, 0, ENET_PML_REG(i));
		emac_writel(emac, 0, ENET_PMH_REG(i));
	}

	/* write device mac address */
	memcpy(addr.sa_data, ndev->dev_addr, ETH_ALEN);
	bcm6348_emac_set_mac_address(ndev, &addr);

	/* allocate rx dma ring */
	size = emac->rx_ring_size * sizeof(struct bcm6348_iudma_desc);
	p = dma_alloc_coherent(dev, size, &emac->rx_desc_dma, GFP_KERNEL);
	if (!p) {
		dev_err(dev, "cannot allocate rx ring %u\n", size);
		ret = -ENOMEM;
		goto out_freeirq_tx;
	}

	memset(p, 0, size);
	emac->rx_desc_alloc_size = size;
	emac->rx_desc_cpu = p;

	/* allocate tx dma ring */
	size = emac->tx_ring_size * sizeof(struct bcm6348_iudma_desc);
	p = dma_alloc_coherent(dev, size, &emac->tx_desc_dma, GFP_KERNEL);
	if (!p) {
		dev_err(dev, "cannot allocate tx ring\n");
		ret = -ENOMEM;
		goto out_free_rx_ring;
	}

	memset(p, 0, size);
	emac->tx_desc_alloc_size = size;
	emac->tx_desc_cpu = p;

	emac->tx_skb = kzalloc(sizeof(struct sk_buff *) * emac->tx_ring_size,
			       GFP_KERNEL);
	if (!emac->tx_skb) {
		dev_err(dev, "cannot allocate rx skb queue\n");
		ret = -ENOMEM;
		goto out_free_tx_ring;
	}

	emac->tx_desc_count = emac->tx_ring_size;
	emac->tx_dirty_desc = 0;
	emac->tx_curr_desc = 0;
	spin_lock_init(&emac->tx_lock);

	/* init & fill rx ring with skbs */
	emac->rx_skb = kzalloc(sizeof(struct sk_buff *) * emac->rx_ring_size,
			       GFP_KERNEL);
	if (!emac->rx_skb) {
		dev_err(dev, "cannot allocate rx skb queue\n");
		ret = -ENOMEM;
		goto out_free_tx_skb;
	}

	emac->rx_desc_count = 0;
	emac->rx_dirty_desc = 0;
	emac->rx_curr_desc = 0;

	/* initialize flow control buffer allocation */
	dma_writel(iudma, DMA_BUFALLOC_FORCE_MASK | 0,
		   DMA_BUFALLOC_REG(emac->rx_chan));

	if (bcm6348_emac_refill_rx(ndev)) {
		dev_err(dev, "cannot allocate rx skb queue\n");
		ret = -ENOMEM;
		goto out;
	}

	/* write rx & tx ring addresses */
	dmas_writel(iudma, emac->rx_desc_dma,
		    DMAS_RSTART_REG, emac->rx_chan);
	dmas_writel(iudma, emac->tx_desc_dma,
		    DMAS_RSTART_REG, emac->tx_chan);

	/* clear remaining state ram for rx & tx channel */
	dmas_writel(iudma, 0, DMAS_SRAM2_REG, emac->rx_chan);
	dmas_writel(iudma, 0, DMAS_SRAM2_REG, emac->tx_chan);
	dmas_writel(iudma, 0, DMAS_SRAM3_REG, emac->rx_chan);
	dmas_writel(iudma, 0, DMAS_SRAM3_REG, emac->tx_chan);
	dmas_writel(iudma, 0, DMAS_SRAM4_REG, emac->rx_chan);
	dmas_writel(iudma, 0, DMAS_SRAM4_REG, emac->tx_chan);

	/* set max rx/tx length */
	emac_writel(emac, ndev->mtu, ENET_RXMAXLEN_REG);
	emac_writel(emac, ndev->mtu, ENET_TXMAXLEN_REG);

	/* set dma maximum burst len */
	dmac_writel(iudma, ENET_DMA_MAXBURST,
		    DMAC_MAXBURST_REG, emac->rx_chan);
	dmac_writel(iudma, ENET_DMA_MAXBURST,
		    DMAC_MAXBURST_REG, emac->tx_chan);

	/* set correct transmit fifo watermark */
	emac_writel(emac, ENET_TX_FIFO_TRESH, ENET_TXWMARK_REG);

	/* set flow control low/high threshold to 1/3 / 2/3 */
	val = emac->rx_ring_size / 3;
	dma_writel(iudma, val, DMA_FLOWCL_REG(emac->rx_chan));
	val = (emac->rx_ring_size * 2) / 3;
	dma_writel(iudma, val, DMA_FLOWCH_REG(emac->rx_chan));

	/* all set, enable emac and interrupts, start dma engine and
	 * kick rx dma channel
	 */
	wmb();
	val = emac_readl(emac, ENET_CTL_REG);
	val |= ENET_CTL_ENABLE_MASK;
	emac_writel(emac, val, ENET_CTL_REG);
	dmac_writel(iudma, DMAC_CHANCFG_EN_MASK,
		    DMAC_CHANCFG_REG, emac->rx_chan);

	/* watch "mib counters about to overflow" interrupt */
	emac_writel(emac, ENET_IR_MIB, ENET_IR_REG);
	emac_writel(emac, ENET_IR_MIB, ENET_IRMASK_REG);

	/* watch "packet transferred" interrupt in rx and tx */
	dmac_writel(iudma, DMAC_IR_PKTDONE_MASK,
		    DMAC_IR_REG, emac->rx_chan);
	dmac_writel(iudma, DMAC_IR_PKTDONE_MASK,
		    DMAC_IR_REG, emac->tx_chan);

	/* make sure we enable napi before rx interrupt  */
	napi_enable(&emac->napi);

	dmac_writel(iudma, DMAC_IR_PKTDONE_MASK,
		    DMAC_IRMASK_REG, emac->rx_chan);
	dmac_writel(iudma, DMAC_IR_PKTDONE_MASK,
		    DMAC_IRMASK_REG, emac->tx_chan);

	if (ndev->phydev)
		phy_start(ndev->phydev);

	netif_carrier_on(ndev);
	netif_start_queue(ndev);

	return 0;

out:
	for (i = 0; i < emac->rx_ring_size; i++) {
		struct bcm6348_iudma_desc *desc;

		if (!emac->rx_skb[i])
			continue;

		desc = &emac->rx_desc_cpu[i];
		dma_unmap_single(dev, desc->address, emac->rx_skb_size,
				 DMA_FROM_DEVICE);
		kfree_skb(emac->rx_skb[i]);
	}
	kfree(emac->rx_skb);

out_free_tx_skb:
	kfree(emac->tx_skb);

out_free_tx_ring:
	dma_free_coherent(dev, emac->tx_desc_alloc_size,
			  emac->tx_desc_cpu, emac->tx_desc_dma);

out_free_rx_ring:
	dma_free_coherent(dev, emac->rx_desc_alloc_size,
			  emac->rx_desc_cpu, emac->rx_desc_dma);

out_freeirq_tx:
	if (emac->irq_tx != -1)
		free_irq(emac->irq_tx, ndev);

out_freeirq_rx:
	free_irq(emac->irq_rx, ndev);

out_freeirq:
	if (ndev->phydev)
		phy_disconnect(ndev->phydev);

	return ret;
}

static int bcm6348_emac_stop(struct net_device *ndev)
{
	struct bcm6348_emac *emac = netdev_priv(ndev);
	struct bcm6348_iudma *iudma = emac->iudma;
	struct device *dev = &emac->pdev->dev;
	unsigned int i;

	netif_stop_queue(ndev);
	napi_disable(&emac->napi);
	if (ndev->phydev)
		phy_stop(ndev->phydev);
	del_timer_sync(&emac->rx_timeout);

	/* mask all interrupts */
	emac_writel(emac, 0, ENET_IRMASK_REG);
	dmac_writel(iudma, 0, DMAC_IRMASK_REG, emac->rx_chan);
	dmac_writel(iudma, 0, DMAC_IRMASK_REG, emac->tx_chan);

	/* disable dma & emac */
	bcm6348_iudma_chan_stop(iudma, emac->tx_chan);
	bcm6348_iudma_chan_stop(iudma, emac->rx_chan);
	bcm6348_emac_disable_mac(emac);

	/* force reclaim of all tx buffers */
	bcm6348_emac_tx_reclaim(ndev, 1);

	/* free the rx skb ring */
	for (i = 0; i < emac->rx_ring_size; i++) {
		struct bcm6348_iudma_desc *desc;

		if (!emac->rx_skb[i])
			continue;

		desc = &emac->rx_desc_cpu[i];
		dma_unmap_single_attrs(dev, desc->address, emac->rx_skb_size,
				       DMA_FROM_DEVICE,
				       DMA_ATTR_SKIP_CPU_SYNC);
		kfree_skb(emac->rx_skb[i]);
	}

	/* free remaining allocated memory */
	kfree(emac->rx_skb);
	kfree(emac->tx_skb);
	dma_free_coherent(dev, emac->rx_desc_alloc_size, emac->rx_desc_cpu,
			  emac->rx_desc_dma);
	dma_free_coherent(dev, emac->tx_desc_alloc_size, emac->tx_desc_cpu,
			  emac->tx_desc_dma);
	free_irq(emac->irq_tx, ndev);
	free_irq(emac->irq_rx, ndev);
	free_irq(ndev->irq, ndev);

	netdev_reset_queue(ndev);

	return 0;
}

static const struct net_device_ops bcm6348_emac_ops = {
	.ndo_open = bcm6348_emac_open,
	.ndo_stop = bcm6348_emac_stop,
	.ndo_start_xmit = bcm6348_emac_start_xmit,
	.ndo_set_mac_address = bcm6348_emac_set_mac_address,
	.ndo_set_rx_mode = bcm6348_emac_set_multicast_list,
};

static int bcm6348_emac_mdio_op(struct bcm6348_emac *emac, uint32_t data)
{
	int limit;

	/* Make sure mii interrupt status is cleared */
	emac_writel(emac, ENET_IR_MII, ENET_IR_REG);

	/* Issue mii op */
	emac_writel(emac, data, ENET_MIID_REG);
	wmb();

	/* busy wait on mii interrupt bit, with timeout */
	limit = 1000;
	do {
		if (emac_readl(emac, ENET_IR_REG) & ENET_IR_MII)
			break;
		udelay(1);
	} while (limit-- > 0);

	return (limit < 0) ? 1 : 0;
}

static int bcm6348_emac_mdio_read(struct mii_bus *bus, int phy_id, int loc)
{
	struct bcm6348_emac *emac = bus->priv;
	struct platform_device *pdev = emac->pdev;
	struct device *dev = &pdev->dev;
	uint32_t reg;

	reg = 0x2 << ENET_MIID_TA_SHIFT;
	reg |= loc << ENET_MIID_REG_SHIFT;
	reg |= phy_id << ENET_MIID_PHY_SHIFT;
	reg |= ENET_MIID_OP_READ;

	if (bcm6348_emac_mdio_op(emac, reg)) {
		dev_err(dev, "mdio_read: phy=%d loc=%x timeout!\n",
			phy_id, loc);
		return -EINVAL;
	}

	reg = emac_readl(emac, ENET_MIID_REG);
	reg = (reg >> ENET_MIID_DATA_SHIFT) & ENET_MIID_DATA_MASK;

	return (int) reg;
}

static int bcm6348_emac_mdio_write(struct mii_bus *bus, int phy_id,
				   int loc, uint16_t val)
{
	struct bcm6348_emac *emac = bus->priv;
	struct platform_device *pdev = emac->pdev;
	struct device *dev = &pdev->dev;
	uint32_t reg;

	reg = (val << ENET_MIID_DATA_SHIFT) & ENET_MIID_DATA_MASK;
	reg |= 0x2 << ENET_MIID_TA_SHIFT;
	reg |= loc << ENET_MIID_REG_SHIFT;
	reg |= phy_id << ENET_MIID_PHY_SHIFT;
	reg |= ENET_MIID_OP_WRITE;

	if (bcm6348_emac_mdio_op(emac, reg)) {
		dev_err(dev, "mdio_write: phy=%d loc=%x timeout!\n",
			phy_id, loc);
		return -EINVAL;
	}

	bcm6348_emac_mdio_op(emac, reg);

	return 0;
}

static int bcm6348_emac_mdio_init(struct bcm6348_emac *emac,
				  struct device_node *np)
{
	struct platform_device *pdev = emac->pdev;
	struct device *dev = &pdev->dev;
	struct device_node *mnp;
	struct mii_bus *mii_bus;
	int ret;

	mnp = of_get_child_by_name(np, "mdio");
	if (!mnp)
		return -ENODEV;

	mii_bus = devm_mdiobus_alloc(dev);
	if (!mii_bus) {
		of_node_put(mnp);
		return -ENOMEM;
	}

	mii_bus->priv = emac;
	mii_bus->name = np->full_name;
	snprintf(mii_bus->id, MII_BUS_ID_SIZE, "%s-mii", dev_name(dev));
	mii_bus->parent = dev;
	mii_bus->read = bcm6348_emac_mdio_read;
	mii_bus->write = bcm6348_emac_mdio_write;
	mii_bus->phy_mask = 0x3f;

	ret = devm_of_mdiobus_register(dev, mii_bus, mnp);
	of_node_put(mnp);
	if (ret) {
		dev_err(dev, "MDIO bus registration failed\n");
		return ret;
	}

	dev_info(dev, "MDIO bus init\n");

	return 0;
}

/*
 * preinit hardware to allow mii operation while device is down
 */
static void bcm6348_emac_hw_preinit(struct bcm6348_emac *emac)
{
	u32 val;
	int limit;

	/* make sure emac is disabled */
	bcm6348_emac_disable_mac(emac);

	/* soft reset emac */
	val = ENET_CTL_SRESET_MASK;
	emac_writel(emac, val, ENET_CTL_REG);
	wmb();

	limit = 1000;
	do {
		val = emac_readl(emac, ENET_CTL_REG);
		if (!(val & ENET_CTL_SRESET_MASK))
			break;
		udelay(1);
	} while (limit--);

	/* select correct mii interface */
	val = emac_readl(emac, ENET_CTL_REG);
	if (emac->ext_mii)
		val |= ENET_CTL_EPHYSEL_MASK;
	else
		val &= ~ENET_CTL_EPHYSEL_MASK;
	emac_writel(emac, val, ENET_CTL_REG);

	/* turn on mdc clock */
	emac_writel(emac, (0x1f << ENET_MIISC_MDCFREQDIV_SHIFT) |
		    ENET_MIISC_PREAMBLEEN_MASK, ENET_MIISC_REG);

	/* set mib counters to self-clear when read */
	val = emac_readl(emac, ENET_MIBCTL_REG);
	val |= ENET_MIBCTL_RDCLEAR_MASK;
	emac_writel(emac, val, ENET_MIBCTL_REG);
}

static int bcm6348_emac_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *node = dev->of_node;
	struct device_node *dma_node;
	struct platform_device *dma_pdev;
	struct bcm6348_emac *emac;
	struct bcm6348_iudma *iudma;
	struct net_device *ndev;
	unsigned i;
	int num_resets;
	int ret;

	dma_node = of_parse_phandle(node, "brcm,iudma", 0);
	if (!dma_node)
		return -EINVAL;

	dma_pdev = of_find_device_by_node(dma_node);
	of_node_put(dma_node);
	if (!dma_pdev)
		return -EINVAL;

	iudma = platform_get_drvdata(dma_pdev);
	if (!iudma)
		return -EPROBE_DEFER;

	ndev = devm_alloc_etherdev(dev, sizeof(*emac));
	if (!ndev)
		return -ENOMEM;

	platform_set_drvdata(pdev, ndev);
	SET_NETDEV_DEV(ndev, dev);

	emac = netdev_priv(ndev);
	emac->iudma = iudma;
	emac->pdev = pdev;
	emac->net_dev = ndev;

	emac->base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR_OR_NULL(emac->base))
		return PTR_ERR(emac->base);

	ndev->irq = of_irq_get_byname(node, "emac");
	if (!ndev->irq)
		return -ENODEV;

	emac->irq_rx = of_irq_get_byname(node, "rx");
	if (!emac->irq_rx)
		return -ENODEV;

	emac->irq_tx = of_irq_get_byname(node, "tx");
	if (!emac->irq_tx)
		return -ENODEV;

	if (of_property_read_u32(node, "dma-rx", &emac->rx_chan))
		return -ENODEV;

	if (of_property_read_u32(node, "dma-tx", &emac->tx_chan))
		return -ENODEV;

	emac->ext_mii = of_property_read_bool(node, "brcm,external-mii");

	emac->rx_ring_size = ENET_DEF_RX_DESC;
	emac->tx_ring_size = ENET_DEF_TX_DESC;
	emac->copybreak = ENET_DEF_CPY_BREAK;

	emac->old_link = 0;
	emac->old_duplex = -1;
	emac->old_pause = -1;

	of_get_mac_address(node, ndev->dev_addr);
	if (is_valid_ether_addr(ndev->dev_addr)) {
		dev_info(dev, "mtd mac %pM\n", ndev->dev_addr);
	} else {
		random_ether_addr(ndev->dev_addr);
		dev_info(dev, "random mac %pM\n", ndev->dev_addr);
	}

	emac->rx_skb_size = ALIGN(ndev->mtu + ENET_MTU_OVERHEAD,
				  ENET_DMA_MAXBURST * 4);

	emac->num_clocks = of_clk_get_parent_count(node);
	if (emac->num_clocks) {
		emac->clock = devm_kcalloc(dev, emac->num_clocks,
					   sizeof(struct clk *), GFP_KERNEL);
		if (IS_ERR_OR_NULL(emac->clock))
			return PTR_ERR(emac->clock);
	}
	for (i = 0; i < emac->num_clocks; i++) {
		emac->clock[i] = of_clk_get(node, i);
		if (IS_ERR_OR_NULL(emac->clock[i])) {
			dev_err(dev, "error getting emac clock %d\n", i);
			return PTR_ERR(emac->clock[i]);
		}

		ret = clk_prepare_enable(emac->clock[i]);
		if (ret) {
			dev_err(dev, "error enabling emac clock %d\n", i);
			return ret;
		}
	}

	num_resets = of_count_phandle_with_args(node, "resets",
						"#reset-cells");
	if (num_resets > 0)
		emac->num_resets = num_resets;
	else
		emac->num_resets = 0;
	if (emac->num_resets) {
		emac->reset = devm_kcalloc(dev, emac->num_resets,
					   sizeof(struct reset_control *),
					   GFP_KERNEL);
		if (IS_ERR_OR_NULL(emac->reset))
			return PTR_ERR(emac->reset);
		
	}
	for (i = 0; i < emac->num_resets; i++) {
		emac->reset[i] = devm_reset_control_get_by_index(dev, i);
		if (IS_ERR_OR_NULL(emac->reset[i])) {
			dev_err(dev, "error getting emac reset %d\n", i);
			return PTR_ERR(emac->reset[i]);
		}

		ret = reset_control_reset(emac->reset[i]);
		if (ret) {
			dev_err(dev, "error performing emac reset %d\n", i);
			return ret;
		}
	}

	/* do minimal hardware init to be able to probe mii bus */
	bcm6348_emac_hw_preinit(emac);

	ret = bcm6348_emac_mdio_init(emac, node);
	if (ret)
		return ret;

	spin_lock_init(&emac->rx_lock);

	timer_setup(&emac->rx_timeout, bcm6348_emac_refill_rx_timer, 0);

	/* zero mib counters */
	for (i = 0; i < ENET_MIB_REG_COUNT; i++)
		emac_writel(emac, 0, ENET_MIB_REG(i));

	/* register netdevice */
	ndev->netdev_ops = &bcm6348_emac_ops;
	ndev->min_mtu = ETH_ZLEN - ETH_HLEN;
	ndev->mtu = ETH_DATA_LEN - VLAN_ETH_HLEN;
	ndev->max_mtu = ENET_MAX_MTU - VLAN_ETH_HLEN;
	netif_napi_add(ndev, &emac->napi, bcm6348_emac_poll, 16);
	SET_NETDEV_DEV(ndev, dev);

	ret = devm_register_netdev(dev, ndev);
	if (ret)
		goto out_disable_clk;

	netif_carrier_off(ndev);

	ndev->phydev = of_phy_get_and_connect(ndev, node,
					      bcm6348_emac_adjust_phy);
	if (IS_ERR_OR_NULL(ndev->phydev))
		dev_warn(dev, "PHY not found!\n");

	dev_info(dev, "%s at 0x%px, IRQ %d\n", ndev->name, emac->base,
		 ndev->irq);

	return 0;

out_disable_clk:
	for (i = 0; i < emac->num_resets; i++)
		reset_control_assert(emac->reset[i]);

	for (i = 0; i < emac->num_clocks; i++)
		clk_disable_unprepare(emac->clock[i]);

	return ret;
}

static int bcm6348_emac_remove(struct platform_device *pdev)
{
	struct net_device *ndev = platform_get_drvdata(pdev);
	struct bcm6348_emac *emac = netdev_priv(ndev);
	unsigned int i;

	emac_writel(emac, 0, ENET_MIISC_REG);

	for (i = 0; i < emac->num_resets; i++)
		reset_control_assert(emac->reset[i]);

	for (i = 0; i < emac->num_clocks; i++)
		clk_disable_unprepare(emac->clock[i]);

	return 0;
}

static const struct of_device_id bcm6348_emac_of_match[] = {
	{ .compatible = "brcm,bcm6338-emac", },
	{ .compatible = "brcm,bcm6348-emac", },
	{ .compatible = "brcm,bcm6358-emac", },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, bcm6348_emac_of_match);

static struct platform_driver bcm6348_emac_driver = {
	.driver = {
		.name = "bcm6348-emac",
		.of_match_table = of_match_ptr(bcm6348_emac_of_match),
	},
	.probe	= bcm6348_emac_probe,
	.remove	= bcm6348_emac_remove,
};

int bcm6348_iudma_drivers_register(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	int ret;

	ret = platform_driver_register(&bcm6348_emac_driver);
	if (ret)
		dev_err(dev, "error registering emac driver!\n");

	return ret;
}

MODULE_AUTHOR("Álvaro Fernández Rojas <noltari@gmail.com>");
MODULE_DESCRIPTION("BCM6348 Ethernet Controller Driver");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:bcm6348-enet");
