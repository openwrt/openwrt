// SPDX-License-Identifier: GPL-2.0
/*
 * Minimal RT63365/RT63368 Frame Engine (PDMA) Ethernet driver.
 *
 * Written for the ZyXEL Keenetic LTE. It relies on the bootloader having
 * initialised the Frame Engine and the internal MT7530-style GSW: it
 * programs only PDMA ring 0 and exposes one netdev, the DSA conduit towards
 * the switch CPU port.
 * The EN751221 econet-eth driver cannot be used: it drives QDMA, which this
 * SoC does not use.
 *
 * FE/PDMA/GSW registers are accessed natively (__raw_*).
 */

#include <linux/bitops.h>
#include <linux/iopoll.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/etherdevice.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/mii.h>
#include <linux/if_vlan.h>
#include <linux/netdevice.h>
#include <linux/of.h>
#include <linux/of_mdio.h>
#include <linux/of_net.h>
#include <linux/phy.h>
#include <linux/platform_device.h>
#include <linux/skbuff.h>
#include <linux/spinlock.h>

#define DRV_NAME		"rt63368-eth"

#define FE_DMA_GLO_CFG		0x0008
#define FE_INT_STATUS		0x0010
#define FE_INT_ENABLE		0x0014
#define GDMA1_FWD_CFG		0x0020
#define GDM_JMB_EN		BIT(19)
#define GDM_STRPCRC		BIT(16)
/* Unicast/broadcast/multicast/other forward ports; 0 = CPU, 7 = discard. */
#define GDM_FWD_PORTS		GENMASK(14, 0)
#define GDMA1_MAC_ADRL		0x002c
#define GDMA1_MAC_ADRH		0x0030

#define PDMA_BASE		0x0800
#define TX_BASE_PTR(q)		(PDMA_BASE + (q) * 0x10 + 0x000)
#define TX_MAX_CNT(q)		(PDMA_BASE + (q) * 0x10 + 0x004)
#define TX_CTX_IDX(q)		(PDMA_BASE + (q) * 0x10 + 0x008)
#define RX_BASE_PTR(q)		(PDMA_BASE + (q) * 0x10 + 0x100)
#define RX_MAX_CNT(q)		(PDMA_BASE + (q) * 0x10 + 0x104)
#define RX_CALC_IDX(q)		(PDMA_BASE + (q) * 0x10 + 0x108)
#define RX_DRX_IDX(q)		(PDMA_BASE + (q) * 0x10 + 0x10c)
#define PDMA_GLO_CFG		(PDMA_BASE + 0x204)
#define PDMA_RST_IDX		(PDMA_BASE + 0x208)
#define PDMA_INT_STATUS		(PDMA_BASE + 0x220)
#define PDMA_INT_MASK		(PDMA_BASE + 0x228)

#define PDMA_BYTE_SWAP		BIT(31)
#define TX_WB_DDONE		BIT(6)
#define PDMA_BT_SIZE		GENMASK(5, 4)
#define PDMA_BT_SIZE_32DW	(3U << 4)
#define RX_DMA_BUSY		BIT(3)
#define RX_DMA_EN		BIT(2)
#define TX_DMA_BUSY		BIT(1)
#define TX_DMA_EN		BIT(0)

#define RST_DRX_IDX0		BIT(16)
#define RST_DTX_IDX0		BIT(0)

#define RX_COHERENT		BIT(31)
#define RX_DLY_INT		BIT(30)
#define TX_COHERENT		BIT(29)
#define RX_DONE_INT0		BIT(16)
#define TX_DONE_INT0		BIT(0)
#define PDMA_IRQ_MASK		(RX_COHERENT | TX_COHERENT | \
				 RX_DLY_INT | RX_DONE_INT0 | TX_DONE_INT0)

#define DESC_DDONE		BIT(31)
#define DESC_LS0		BIT(30)
#define DESC_LEN_SHIFT		16
#define DESC_LEN_MASK		GENMASK(29, 16)
#define TXD4_PN_GDMA1		(1U << 24)
#define TXD4_QN_3		(3U << 16)

#define RING_SIZE		64
#define RX_BUF_SIZE		2048
/* Up to a VLAN-tagged frame plus the 4-byte MTK DSA tag, without FCS. */
#define MAX_FRAME_LEN		1536
#define NAPI_WEIGHT		64

/* The GSW (MT7530 register map) sits inside the FE window. */
#define GSW_OFFSET		0x8000
#define GSW_PMCR(p)		(0x3000 + (p) * 0x100)
#define GSW_PMSR(p)		(0x3008 + (p) * 0x100)
#define GSW_CHIP_ID		0x7ffc
#define GSW_TGPC(p)		(0x4010 + (p) * 0x100)
#define GSW_RGPC(p)		(0x4020 + (p) * 0x100)

/*
 * MDIO master: the internal GSW PHY indirect access register. The external
 * MT7530 answers at MDIO address 31 (32-bit registers through page 31, low
 * half at (addr >> 2) & 0xf, high half at 16); its PHYs are at 0..4.
 */
#define GSW_PIAC		0x7004
#define PIAC_ACS_ST		BIT(31)
#define PIAC_REG_SHIFT		25
#define PIAC_PHY_SHIFT		20
#define PIAC_CMD_WRITE		(1U << 18)
#define PIAC_CMD_READ		(2U << 18)
#define PIAC_ST_START		(1U << 16)
#define PIAC_DATA		GENMASK(15, 0)
#define MT7530_MDIO_ADDR	31

struct rt63368_desc {
	u32 info1;
	u32 info2;
	u32 info3;
	u32 info4;
} __aligned(16);

struct rt63368_priv {
	struct device *dev;
	struct net_device *ndev;
	void __iomem *base;
	struct napi_struct napi;
	spinlock_t tx_lock;

	struct rt63368_desc *rx_ring;
	dma_addr_t rx_ring_dma;
	struct sk_buff *rx_skb[RING_SIZE];
	dma_addr_t rx_dma[RING_SIZE];
	u16 rx_head;

	struct rt63368_desc *tx_ring;
	dma_addr_t tx_ring_dma;
	struct sk_buff *tx_skb[RING_SIZE];
	dma_addr_t tx_dma[RING_SIZE];
	u16 tx_len[RING_SIZE];
	u16 tx_head;
	u16 tx_tail;
	u16 tx_used;

	struct mii_bus *mii;
};

static inline u32 fe_read(struct rt63368_priv *priv, u32 reg)
{
	return __raw_readl(priv->base + reg);
}

static inline void fe_write(struct rt63368_priv *priv, u32 reg, u32 value)
{
	__raw_writel(value, priv->base + reg);
}

static inline u32 gsw_read(struct rt63368_priv *priv, u32 reg)
{
	return __raw_readl(priv->base + GSW_OFFSET + reg);
}

static int rt63368_piac_wait(struct rt63368_priv *priv, u32 *val)
{
	return readx_poll_timeout_atomic(__raw_readl,
					 priv->base + GSW_OFFSET + GSW_PIAC,
					 *val, !(*val & PIAC_ACS_ST), 1, 10000);
}

static int rt63368_mdio_read(struct rt63368_priv *priv, int phy, int reg)
{
	u32 val;
	int err;

	err = rt63368_piac_wait(priv, &val);
	if (err)
		return err;
	__raw_writel(PIAC_ACS_ST | PIAC_ST_START | PIAC_CMD_READ |
		     (phy << PIAC_PHY_SHIFT) | (reg << PIAC_REG_SHIFT),
		     priv->base + GSW_OFFSET + GSW_PIAC);
	err = rt63368_piac_wait(priv, &val);
	return err ? err : (int)(val & PIAC_DATA);
}

static int rt63368_mdio_write(struct rt63368_priv *priv, int phy, int reg,
			      u16 data)
{
	u32 val;
	int err;

	err = rt63368_piac_wait(priv, &val);
	if (err)
		return err;
	__raw_writel(PIAC_ACS_ST | PIAC_ST_START | PIAC_CMD_WRITE |
		     (phy << PIAC_PHY_SHIFT) | (reg << PIAC_REG_SHIFT) | data,
		     priv->base + GSW_OFFSET + GSW_PIAC);
	return rt63368_piac_wait(priv, &val);
}

static int rt63368_mii_read(struct mii_bus *bus, int phy, int reg)
{
	return rt63368_mdio_read(bus->priv, phy, reg);
}

static int rt63368_mii_write(struct mii_bus *bus, int phy, int reg, u16 val)
{
	return rt63368_mdio_write(bus->priv, phy, reg, val);
}

/* Register the MDIO bus if the DT describes one (the external MT7530). */
static int rt63368_mdio_init(struct rt63368_priv *priv)
{
	struct device_node *np;
	struct mii_bus *bus;
	int err;

	np = of_get_child_by_name(priv->dev->of_node, "mdio-bus");
	if (!np)
		return 0;
	if (!of_device_is_available(np)) {
		of_node_put(np);
		return 0;
	}

	bus = devm_mdiobus_alloc(priv->dev);
	if (!bus) {
		of_node_put(np);
		return -ENOMEM;
	}
	bus->name = "rt63368 GSW MDIO";
	bus->read = rt63368_mii_read;
	bus->write = rt63368_mii_write;
	bus->priv = priv;
	bus->parent = priv->dev;
	snprintf(bus->id, MII_BUS_ID_SIZE, "%s-mii", dev_name(priv->dev));

	err = devm_of_mdiobus_register(priv->dev, bus, np);
	of_node_put(np);
	if (err)
		return err;
	priv->mii = bus;
	return 0;
}

static void rt63368_pdma_stop(struct rt63368_priv *priv)
{
	u32 cfg = fe_read(priv, PDMA_GLO_CFG);
	unsigned int timeout = 10000;

	cfg &= ~(RX_DMA_EN | TX_DMA_EN);
	fe_write(priv, PDMA_GLO_CFG, cfg);

	while ((fe_read(priv, PDMA_GLO_CFG) & (RX_DMA_BUSY | TX_DMA_BUSY)) &&
	       --timeout)
		udelay(1);

	if (!timeout)
		dev_warn(priv->dev, "PDMA did not become idle: %08x\n",
			 fe_read(priv, PDMA_GLO_CFG));
}

static int rt63368_map_rx(struct rt63368_priv *priv, unsigned int index,
			  struct sk_buff *skb)
{
	dma_addr_t dma;

	dma = dma_map_single(priv->dev, skb->data, RX_BUF_SIZE,
			     DMA_FROM_DEVICE);
	if (dma_mapping_error(priv->dev, dma))
		return -EIO;

	priv->rx_skb[index] = skb;
	priv->rx_dma[index] = dma;
	priv->rx_ring[index].info1 = lower_32_bits(dma);
	priv->rx_ring[index].info3 = 0;
	priv->rx_ring[index].info4 = 0;
	dma_wmb();
	priv->rx_ring[index].info2 = DESC_LS0;
	return 0;
}

static void rt63368_free_rings(struct rt63368_priv *priv)
{
	unsigned int i;

	for (i = 0; i < RING_SIZE; i++) {
		if (priv->rx_skb[i]) {
			dma_unmap_single(priv->dev, priv->rx_dma[i],
					 RX_BUF_SIZE, DMA_FROM_DEVICE);
			dev_kfree_skb_any(priv->rx_skb[i]);
			priv->rx_skb[i] = NULL;
		}
		if (priv->tx_skb[i]) {
			dma_unmap_single(priv->dev, priv->tx_dma[i],
					 priv->tx_len[i], DMA_TO_DEVICE);
			dev_kfree_skb_any(priv->tx_skb[i]);
			priv->tx_skb[i] = NULL;
		}
	}

	if (priv->rx_ring) {
		dma_free_coherent(priv->dev, sizeof(*priv->rx_ring) * RING_SIZE,
				  priv->rx_ring, priv->rx_ring_dma);
		priv->rx_ring = NULL;
	}
	if (priv->tx_ring) {
		dma_free_coherent(priv->dev, sizeof(*priv->tx_ring) * RING_SIZE,
				  priv->tx_ring, priv->tx_ring_dma);
		priv->tx_ring = NULL;
	}
}

static int rt63368_alloc_rings(struct rt63368_priv *priv)
{
	unsigned int i;

	priv->rx_ring = dma_alloc_coherent(priv->dev,
					   sizeof(*priv->rx_ring) * RING_SIZE,
					   &priv->rx_ring_dma, GFP_KERNEL);
	priv->tx_ring = dma_alloc_coherent(priv->dev,
					   sizeof(*priv->tx_ring) * RING_SIZE,
					   &priv->tx_ring_dma, GFP_KERNEL);
	if (!priv->rx_ring || !priv->tx_ring)
		goto err;

	for (i = 0; i < RING_SIZE; i++) {
		struct sk_buff *skb;

		skb = netdev_alloc_skb_ip_align(priv->ndev, RX_BUF_SIZE);
		if (!skb || rt63368_map_rx(priv, i, skb)) {
			if (skb)
				dev_kfree_skb_any(skb);
			goto err;
		}
		priv->tx_ring[i].info2 = DESC_DDONE | DESC_LS0;
		priv->tx_ring[i].info4 = TXD4_PN_GDMA1 | TXD4_QN_3;
	}

	priv->rx_head = 0;
	priv->tx_head = 0;
	priv->tx_tail = 0;
	priv->tx_used = 0;
	return 0;

err:
	rt63368_free_rings(priv);
	return -ENOMEM;
}

static unsigned int rt63368_tx_clean(struct rt63368_priv *priv)
{
	unsigned int cleaned = 0;

	while (priv->tx_used) {
		unsigned int index = priv->tx_tail;
		struct rt63368_desc *desc = &priv->tx_ring[index];

		if (!(READ_ONCE(desc->info2) & DESC_DDONE))
			break;
		dma_rmb();
		dma_unmap_single(priv->dev, priv->tx_dma[index],
				 priv->tx_len[index], DMA_TO_DEVICE);
		dev_kfree_skb_any(priv->tx_skb[index]);
		priv->tx_skb[index] = NULL;
		priv->tx_tail = (index + 1) % RING_SIZE;
		priv->tx_used--;
		cleaned++;
	}

	if (cleaned && netif_queue_stopped(priv->ndev) &&
	    priv->tx_used < RING_SIZE - 1)
		netif_wake_queue(priv->ndev);
	return cleaned;
}

static int rt63368_rx_poll(struct rt63368_priv *priv, int budget)
{
	struct net_device *ndev = priv->ndev;
	int done = 0;

	while (done < budget) {
		unsigned int index = priv->rx_head;
		struct rt63368_desc *desc = &priv->rx_ring[index];
		struct sk_buff *skb, *replacement;
		dma_addr_t replacement_dma;
		unsigned int len;
		u32 info2;

		info2 = READ_ONCE(desc->info2);
		if (!(info2 & DESC_DDONE))
			break;
		dma_rmb();
		len = (info2 & DESC_LEN_MASK) >> DESC_LEN_SHIFT;
		skb = priv->rx_skb[index];
		dma_unmap_single(priv->dev, priv->rx_dma[index], RX_BUF_SIZE,
				 DMA_FROM_DEVICE);

		replacement = netdev_alloc_skb_ip_align(ndev, RX_BUF_SIZE);
		if (!replacement)
			goto drop_reuse;
		replacement_dma = dma_map_single(priv->dev, replacement->data,
						 RX_BUF_SIZE, DMA_FROM_DEVICE);
		if (dma_mapping_error(priv->dev, replacement_dma)) {
			dev_kfree_skb_any(replacement);
			goto drop_reuse;
		}

		priv->rx_skb[index] = replacement;
		priv->rx_dma[index] = replacement_dma;
		desc->info1 = lower_32_bits(replacement_dma);
		desc->info3 = 0;
		desc->info4 = 0;
		dma_wmb();
		desc->info2 = DESC_LS0;

		if (likely(len >= ETH_ZLEN && len <= MAX_FRAME_LEN)) {
			skb_put(skb, len);
			skb->protocol = eth_type_trans(skb, ndev);
			skb->ip_summed = CHECKSUM_NONE;
			napi_gro_receive(&priv->napi, skb);
			ndev->stats.rx_packets++;
			ndev->stats.rx_bytes += len;
		} else {
			ndev->stats.rx_length_errors++;
			ndev->stats.rx_dropped++;
			dev_kfree_skb_any(skb);
		}
		goto reposted;

drop_reuse:
		ndev->stats.rx_dropped++;
		if (rt63368_map_rx(priv, index, skb)) {
			dev_kfree_skb_any(skb);
			priv->rx_skb[index] = NULL;
			netdev_err(ndev, "unable to remap RX buffer %u\n", index);
			break;
		}

reposted:
		/* Drain the write buffer before handing the slot back. */
		(void)READ_ONCE(desc->info2);
		fe_write(priv, RX_CALC_IDX(0), index);
		priv->rx_head = (index + 1) % RING_SIZE;
		done++;
	}
	return done;
}

static int rt63368_poll(struct napi_struct *napi, int budget)
{
	struct rt63368_priv *priv = container_of(napi, struct rt63368_priv,
						 napi);
	unsigned long flags;
	int done;

	spin_lock_irqsave(&priv->tx_lock, flags);
	rt63368_tx_clean(priv);
	spin_unlock_irqrestore(&priv->tx_lock, flags);
	done = rt63368_rx_poll(priv, budget);

	if (done < budget && napi_complete_done(napi, done))
		fe_write(priv, PDMA_INT_MASK, PDMA_IRQ_MASK);
	return done;
}

static irqreturn_t rt63368_irq(int irq, void *data)
{
	struct net_device *ndev = data;
	struct rt63368_priv *priv = netdev_priv(ndev);
	u32 status = fe_read(priv, PDMA_INT_STATUS);

	if (!(status & PDMA_IRQ_MASK))
		return IRQ_NONE;
	fe_write(priv, PDMA_INT_STATUS, status);

	if (status & TX_COHERENT)
		ndev->stats.tx_errors++;
	if (status & RX_COHERENT)
		ndev->stats.rx_errors++;
	if (status & (RX_COHERENT | TX_COHERENT))
		netdev_err_once(ndev, "coherent DMA error, status=%08x\n",
				status);

	if (napi_schedule_prep(&priv->napi)) {
		fe_write(priv, PDMA_INT_MASK, 0);
		__napi_schedule(&priv->napi);
	}
	return IRQ_HANDLED;
}

static netdev_tx_t rt63368_start_xmit(struct sk_buff *skb,
				      struct net_device *ndev)
{
	struct rt63368_priv *priv = netdev_priv(ndev);
	struct rt63368_desc *desc;
	unsigned int index, len;
	unsigned long flags;
	dma_addr_t dma;

	if (skb_put_padto(skb, ETH_ZLEN)) {
		ndev->stats.tx_dropped++;
		return NETDEV_TX_OK;
	}
	len = skb->len;
	dma = dma_map_single(priv->dev, skb->data, len, DMA_TO_DEVICE);
	if (dma_mapping_error(priv->dev, dma)) {
		ndev->stats.tx_dropped++;
		dev_kfree_skb_any(skb);
		return NETDEV_TX_OK;
	}

	spin_lock_irqsave(&priv->tx_lock, flags);
	rt63368_tx_clean(priv);
	if (unlikely(priv->tx_used >= RING_SIZE - 1)) {
		netif_stop_queue(ndev);
		spin_unlock_irqrestore(&priv->tx_lock, flags);
		dma_unmap_single(priv->dev, dma, len, DMA_TO_DEVICE);
		return NETDEV_TX_BUSY;
	}

	index = priv->tx_head;
	desc = &priv->tx_ring[index];
	priv->tx_skb[index] = skb;
	priv->tx_dma[index] = dma;
	priv->tx_len[index] = len;
	desc->info1 = lower_32_bits(dma);
	desc->info3 = 0;
	desc->info4 = TXD4_PN_GDMA1 | TXD4_QN_3;
	dma_wmb();
	desc->info2 = DESC_LS0 | (len << DESC_LEN_SHIFT);
	/*
	 * Read the descriptor back before ringing the doorbell, as the vendor
	 * driver does: this drains the CPU write buffer so the DMA engine never
	 * fetches the stale DDONE=1 word (TX_COHERENT interrupt).
	 */
	(void)READ_ONCE(desc->info2);

	priv->tx_head = (index + 1) % RING_SIZE;
	priv->tx_used++;
	fe_write(priv, TX_CTX_IDX(0), priv->tx_head);
	if (priv->tx_used >= RING_SIZE - 1)
		netif_stop_queue(ndev);
	spin_unlock_irqrestore(&priv->tx_lock, flags);

	ndev->stats.tx_packets++;
	ndev->stats.tx_bytes += len;
	return NETDEV_TX_OK;
}

static int rt63368_open(struct net_device *ndev)
{
	struct rt63368_priv *priv = netdev_priv(ndev);
	u32 cfg;
	int err;

	rt63368_pdma_stop(priv);
	fe_write(priv, PDMA_INT_MASK, 0);
	fe_write(priv, PDMA_INT_STATUS, fe_read(priv, PDMA_INT_STATUS));

	err = rt63368_alloc_rings(priv);
	if (err)
		return err;

	fe_write(priv, TX_BASE_PTR(0), lower_32_bits(priv->tx_ring_dma));
	fe_write(priv, TX_MAX_CNT(0), RING_SIZE);
	fe_write(priv, TX_CTX_IDX(0), 0);
	fe_write(priv, PDMA_RST_IDX, RST_DTX_IDX0);
	fe_write(priv, RX_BASE_PTR(0), lower_32_bits(priv->rx_ring_dma));
	fe_write(priv, RX_MAX_CNT(0), RING_SIZE);
	fe_write(priv, RX_CALC_IDX(0), RING_SIZE - 1);
	fe_write(priv, PDMA_RST_IDX, RST_DRX_IDX0);

	err = request_irq(ndev->irq, rt63368_irq, 0, ndev->name, ndev);
	if (err)
		goto err_rings;

	napi_enable(&priv->napi);
	fe_write(priv, PDMA_INT_STATUS, fe_read(priv, PDMA_INT_STATUS));
	fe_write(priv, PDMA_INT_MASK, PDMA_IRQ_MASK);

	cfg = fe_read(priv, PDMA_GLO_CFG);
	cfg &= ~(PDMA_BT_SIZE | RX_DMA_EN | TX_DMA_EN);
	cfg |= PDMA_BYTE_SWAP | TX_WB_DDONE | PDMA_BT_SIZE_32DW |
	       RX_DMA_EN | TX_DMA_EN;
	fe_write(priv, PDMA_GLO_CFG, cfg);

	netif_carrier_on(ndev);
	netif_start_queue(ndev);
	return 0;

err_rings:
	rt63368_free_rings(priv);
	return err;
}

static int rt63368_stop(struct net_device *ndev)
{
	struct rt63368_priv *priv = netdev_priv(ndev);

	netif_stop_queue(ndev);
	netif_carrier_off(ndev);
	fe_write(priv, PDMA_INT_MASK, 0);
	rt63368_pdma_stop(priv);
	free_irq(ndev->irq, ndev);
	napi_disable(&priv->napi);
	rt63368_free_rings(priv);
	return 0;
}

static void rt63368_write_mac(struct rt63368_priv *priv, const u8 *addr)
{
	fe_write(priv, GDMA1_MAC_ADRL, addr[2] << 24 | addr[3] << 16 |
				       addr[4] << 8 | addr[5]);
	fe_write(priv, GDMA1_MAC_ADRH, addr[0] << 8 | addr[1]);
}

static int rt63368_set_mac_address(struct net_device *ndev, void *data)
{
	struct rt63368_priv *priv = netdev_priv(ndev);
	int err;

	err = eth_mac_addr(ndev, data);
	if (err)
		return err;
	rt63368_write_mac(priv, ndev->dev_addr);
	return 0;
}

static const struct net_device_ops rt63368_netdev_ops = {
	.ndo_open		= rt63368_open,
	.ndo_stop		= rt63368_stop,
	.ndo_start_xmit		= rt63368_start_xmit,
	.ndo_set_mac_address	= rt63368_set_mac_address,
	.ndo_validate_addr	= eth_validate_addr,
};

static int rt63368_probe(struct platform_device *pdev)
{
	struct rt63368_priv *priv;
	struct net_device *ndev;
	u8 addr[ETH_ALEN];
	u32 low, high;
	int err, irq;

	ndev = devm_alloc_etherdev(&pdev->dev, sizeof(*priv));
	if (!ndev)
		return -ENOMEM;
	SET_NETDEV_DEV(ndev, &pdev->dev);
	platform_set_drvdata(pdev, ndev);

	priv = netdev_priv(ndev);
	priv->dev = &pdev->dev;
	priv->ndev = ndev;
	spin_lock_init(&priv->tx_lock);

	priv->base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(priv->base))
		return PTR_ERR(priv->base);

	irq = platform_get_irq(pdev, 0);
	if (irq < 0)
		return irq;
	ndev->irq = irq;

	err = dma_set_mask_and_coherent(&pdev->dev, DMA_BIT_MASK(32));
	if (err)
		return err;

	ndev->netdev_ops = &rt63368_netdev_ops;
	ndev->watchdog_timeo = 5 * HZ;
	ndev->max_mtu = MAX_FRAME_LEN - ETH_HLEN - VLAN_HLEN;
	/* Let DSA find this netdev as the conduit of the MT7530 CPU port. */
	ndev->dev.of_node = pdev->dev.of_node;
	netif_napi_add_weight(ndev, &priv->napi, rt63368_poll, NAPI_WEIGHT);

	/*
	 * DT address first (normally an NVMEM cell in the board's rf-eeprom),
	 * then what the bootloader left in GDMA1 (only set after a TFTP boot),
	 * else a random address.
	 */
	err = of_get_ethdev_address(pdev->dev.of_node, ndev);
	if (err == -EPROBE_DEFER)
		return err;
	if (err) {
		low = fe_read(priv, GDMA1_MAC_ADRL);
		high = fe_read(priv, GDMA1_MAC_ADRH);
		addr[0] = high >> 8;
		addr[1] = high;
		addr[2] = low >> 24;
		addr[3] = low >> 16;
		addr[4] = low >> 8;
		addr[5] = low;
		if (is_valid_ether_addr(addr))
			eth_hw_addr_set(ndev, addr);
		else
			eth_hw_addr_random(ndev);
	}
	rt63368_write_mac(priv, ndev->dev_addr);

	/*
	 * GDMA1 drops frames longer than 1518 bytes unless jumbo frames are
	 * enabled, and with the 4-byte MTK DSA tag of the external MT7530 a
	 * full 1500-byte packet is 1522 bytes. Keep the bootloader's jumbo
	 * length; both switches already cap frames at 1536 bytes.
	 */
	fe_write(priv, GDMA1_FWD_CFG, fe_read(priv, GDMA1_FWD_CFG) | GDM_JMB_EN);

	/*
	 * Like the vendor macSetMACCR(): forward all received frames to the
	 * CPU (PDMA). U-Boot only does this when it uses the network (TFTP);
	 * after a boot from NAND the fields keep their reset value 7 (discard)
	 * and nothing is received.
	 */
	fe_write(priv, GDMA1_FWD_CFG,
		 (fe_read(priv, GDMA1_FWD_CFG) & ~GDM_FWD_PORTS) | GDM_STRPCRC);

	err = register_netdev(ndev);
	if (err) {
		netif_napi_del(&priv->napi);
		return err;
	}

	/* After register_netdev(), so DSA finds its conduit when it probes. */
	err = rt63368_mdio_init(priv);
	if (err)
		netdev_warn(ndev, "MDIO bus registration failed: %d\n", err);

	return 0;
}

static void rt63368_remove(struct platform_device *pdev)
{
	struct net_device *ndev = platform_get_drvdata(pdev);
	struct rt63368_priv *priv = netdev_priv(ndev);

	unregister_netdev(ndev);
	netif_napi_del(&priv->napi);
}

static const struct of_device_id rt63368_of_match[] = {
	{ .compatible = "econet,rt63368-eth" },
	{ }
};
MODULE_DEVICE_TABLE(of, rt63368_of_match);

static struct platform_driver rt63368_driver = {
	.probe = rt63368_probe,
	.remove = rt63368_remove,
	.driver = {
		.name = DRV_NAME,
		.of_match_table = rt63368_of_match,
	},
};
module_platform_driver(rt63368_driver);

MODULE_DESCRIPTION("RT63368 Frame Engine PDMA Ethernet driver");
MODULE_LICENSE("GPL");
