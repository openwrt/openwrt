/*
 *   This program is free software; you can redistribute it and/or modify it
 *   under the terms of the GNU General Public License version 2 as published
 *   by the Free Software Foundation.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 *
 *   Copyright (C) 2011 John Crispin <blogic@openwrt.org>
 */

#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/uaccess.h>
#include <linux/in.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/phy.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/skbuff.h>
#include <linux/mm.h>
#include <linux/platform_device.h>
#include <linux/ethtool.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/dma-mapping.h>
#include <linux/module.h>
#include <linux/clk.h>

#include <asm/checksum.h>

#include <lantiq_soc.h>
#include <xway_dma.h>
#include <lantiq_platform.h>

#define LTQ_SWITCH_BASE                 0x1E108000
#define LTQ_SWITCH_CORE_BASE            LTQ_SWITCH_BASE
#define LTQ_SWITCH_TOP_PDI_BASE         LTQ_SWITCH_CORE_BASE
#define LTQ_SWITCH_BM_PDI_BASE          (LTQ_SWITCH_CORE_BASE + 4 * 0x40)
#define LTQ_SWITCH_MAC_PDI_0_BASE       (LTQ_SWITCH_CORE_BASE + 4 * 0x900)
#define LTQ_SWITCH_MAC_PDI_X_BASE(x)    (LTQ_SWITCH_MAC_PDI_0_BASE + x * 0x30)
#define LTQ_SWITCH_TOPLEVEL_BASE        (LTQ_SWITCH_BASE + 4 * 0xC40)
#define LTQ_SWITCH_MDIO_PDI_BASE        (LTQ_SWITCH_TOPLEVEL_BASE)
#define LTQ_SWITCH_MII_PDI_BASE         (LTQ_SWITCH_TOPLEVEL_BASE + 4 * 0x36)
#define LTQ_SWITCH_PMAC_PDI_BASE        (LTQ_SWITCH_TOPLEVEL_BASE + 4 * 0x82)

#define LTQ_ETHSW_MAC_CTRL0_PADEN               (1 << 8)
#define LTQ_ETHSW_MAC_CTRL0_FCS                 (1 << 7)
#define LTQ_ETHSW_MAC_CTRL1_SHORTPRE            (1 << 8)
#define LTQ_ETHSW_MAC_CTRL2_MLEN                (1 << 3)
#define LTQ_ETHSW_MAC_CTRL2_LCHKL               (1 << 2)
#define LTQ_ETHSW_MAC_CTRL2_LCHKS_DIS           0
#define LTQ_ETHSW_MAC_CTRL2_LCHKS_UNTAG         1
#define LTQ_ETHSW_MAC_CTRL2_LCHKS_TAG           2
#define LTQ_ETHSW_MAC_CTRL6_RBUF_DLY_WP_SHIFT   9
#define LTQ_ETHSW_MAC_CTRL6_RXBUF_BYPASS        (1 << 6)
#define LTQ_ETHSW_GLOB_CTRL_SE                  (1 << 15)
#define LTQ_ETHSW_MDC_CFG1_MCEN                 (1 << 8)
#define LTQ_ETHSW_PMAC_HD_CTL_FC                (1 << 10)
#define LTQ_ETHSW_PMAC_HD_CTL_RC                (1 << 4)
#define LTQ_ETHSW_PMAC_HD_CTL_AC                (1 << 2)
#define ADVERTIZE_MPD          (1 << 10)

#define MDIO_DEVAD_NONE                    (-1)

#define LTQ_ETH_RX_BUFFER_CNT           PKTBUFSRX

#define LTQ_MDIO_DRV_NAME               "ltq-mdio"
#define LTQ_ETH_DRV_NAME                "ltq-eth"

#define LTQ_ETHSW_MAX_GMAC              1
#define LTQ_ETHSW_PMAC                  1

#define ltq_setbits(a, set) \
        ltq_w32(ltq_r32(a) | (set), a)

enum ltq_reset_modules {
	LTQ_RESET_CORE,
	LTQ_RESET_DMA,
	LTQ_RESET_ETH,
	LTQ_RESET_PHY,
	LTQ_RESET_HARD,
	LTQ_RESET_SOFT,
};

static inline void
dbg_ltq_writel(void *a, unsigned int b)
{
	ltq_w32(b, a);
}

int ltq_reset_once(enum ltq_reset_modules module, ulong usec);

struct ltq_ethsw_mac_pdi_x_regs {
	u32     pstat;          /* Port status */
	u32     pisr;           /* Interrupt status */
	u32     pier;           /* Interrupt enable */
	u32     ctrl_0;         /* Control 0 */
	u32     ctrl_1;         /* Control 1 */
	u32     ctrl_2;         /* Control 2 */
	u32     ctrl_3;         /* Control 3 */
	u32     ctrl_4;         /* Control 4 */
	u32     ctrl_5;         /* Control 5 */
	u32     ctrl_6;         /* Control 6 */
	u32     bufst;          /* TX/RX buffer control */
	u32     testen;         /* Test enable */
};

struct ltq_ethsw_mac_pdi_regs {
	struct ltq_ethsw_mac_pdi_x_regs mac[12];
};

struct ltq_ethsw_mdio_pdi_regs {
	u32     glob_ctrl;      /* Global control 0 */
	u32     rsvd0[7];
	u32     mdio_ctrl;      /* MDIO control */
	u32     mdio_read;      /* MDIO read data */
	u32     mdio_write;     /* MDIO write data */
	u32     mdc_cfg_0;      /* MDC clock configuration 0 */
	u32     mdc_cfg_1;      /* MDC clock configuration 1 */
	u32     rsvd[3];
	u32     phy_addr_5;     /* PHY address port 5 */
	u32     phy_addr_4;     /* PHY address port 4 */
	u32     phy_addr_3;     /* PHY address port 3 */
	u32     phy_addr_2;     /* PHY address port 2 */
	u32     phy_addr_1;     /* PHY address port 1 */
	u32     phy_addr_0;     /* PHY address port 0 */
	u32     mdio_stat_0;    /* MDIO PHY polling status port 0 */
	u32     mdio_stat_1;    /* MDIO PHY polling status port 1 */
	u32     mdio_stat_2;    /* MDIO PHY polling status port 2 */
	u32     mdio_stat_3;    /* MDIO PHY polling status port 3 */
	u32     mdio_stat_4;    /* MDIO PHY polling status port 4 */
	u32     mdio_stat_5;    /* MDIO PHY polling status port 5 */
};

struct ltq_ethsw_mii_pdi_regs {
	u32     mii_cfg0;       /* xMII port 0 configuration */
	u32     pcdu0;          /* Port 0 clock delay configuration */
	u32     mii_cfg1;       /* xMII port 1 configuration */
	u32     pcdu1;          /* Port 1 clock delay configuration */
	u32     mii_cfg2;       /* xMII port 2 configuration */
	u32     rsvd0;
	u32     mii_cfg3;       /* xMII port 3 configuration */
	u32     rsvd1;
	u32     mii_cfg4;       /* xMII port 4 configuration */
	u32     rsvd2;
	u32     mii_cfg5;       /* xMII port 5 configuration */
	u32     pcdu5;          /* Port 5 clock delay configuration */
};

struct ltq_ethsw_pmac_pdi_regs {
	u32     hd_ctl;         /* PMAC header control */
	u32     tl;             /* PMAC type/length */
	u32     sa1;            /* PMAC source address 1 */
	u32     sa2;            /* PMAC source address 2 */
	u32     sa3;            /* PMAC source address 3 */
	u32     da1;            /* PMAC destination address 1 */
	u32     da2;            /* PMAC destination address 2 */
	u32     da3;            /* PMAC destination address 3 */
	u32     vlan;           /* PMAC VLAN */
	u32     rx_ipg;         /* PMAC interpacket gap in RX direction */
	u32     st_etype;       /* PMAC special tag ethertype */
	u32     ewan;           /* PMAC ethernet WAN group */
};

struct ltq_mdio_phy_addr_reg {
	union {
		struct {
			unsigned rsvd:1;
			unsigned lnkst:2;       /* Link status control */
			unsigned speed:2;       /* Speed control */
			unsigned fdup:2;        /* Full duplex control */
			unsigned fcontx:2;      /* Flow control mode TX */
			unsigned fconrx:2;      /* Flow control mode RX */
			unsigned addr:5;        /* PHY address */
		} bits;
		u16 val;
	};
};

enum ltq_mdio_phy_addr_lnkst {
	LTQ_MDIO_PHY_ADDR_LNKST_AUTO = 0,
	LTQ_MDIO_PHY_ADDR_LNKST_UP = 1,
	LTQ_MDIO_PHY_ADDR_LNKST_DOWN = 2,
};

enum ltq_mdio_phy_addr_speed {
	LTQ_MDIO_PHY_ADDR_SPEED_M10 = 0,
	LTQ_MDIO_PHY_ADDR_SPEED_M100 = 1,
	LTQ_MDIO_PHY_ADDR_SPEED_G1 = 2,
	LTQ_MDIO_PHY_ADDR_SPEED_AUTO = 3,
};

enum ltq_mdio_phy_addr_fdup {
	LTQ_MDIO_PHY_ADDR_FDUP_AUTO = 0,
	LTQ_MDIO_PHY_ADDR_FDUP_ENABLE = 1,
	LTQ_MDIO_PHY_ADDR_FDUP_DISABLE = 3,
};

enum ltq_mdio_phy_addr_fcon {
	LTQ_MDIO_PHY_ADDR_FCON_AUTO = 0,
	LTQ_MDIO_PHY_ADDR_FCON_ENABLE = 1,
	LTQ_MDIO_PHY_ADDR_FCON_DISABLE = 3,
};

struct ltq_mii_mii_cfg_reg {
	union {
		struct {
			unsigned res:1;         /* Hardware reset */
			unsigned en:1;          /* xMII interface enable */
			unsigned isol:1;        /* xMII interface isolate */
			unsigned ldclkdis:1;    /* Link down clock disable */
			unsigned rsvd:1;
			unsigned crs:2;         /* CRS sensitivity config */
			unsigned rgmii_ibs:1;   /* RGMII In Band status */
			unsigned rmii:1;        /* RMII ref clock direction */
			unsigned miirate:3;     /* xMII interface clock rate */
			unsigned miimode:4;     /* xMII interface mode */
		} bits;
		u16 val;
	};
};

enum ltq_mii_mii_cfg_miirate {
	LTQ_MII_MII_CFG_MIIRATE_M2P5 = 0,
	LTQ_MII_MII_CFG_MIIRATE_M25 = 1,
	LTQ_MII_MII_CFG_MIIRATE_M125 = 2,
	LTQ_MII_MII_CFG_MIIRATE_M50 = 3,
	LTQ_MII_MII_CFG_MIIRATE_AUTO = 4,
};

enum ltq_mii_mii_cfg_miimode {
	LTQ_MII_MII_CFG_MIIMODE_MIIP = 0,
	LTQ_MII_MII_CFG_MIIMODE_MIIM = 1,
	LTQ_MII_MII_CFG_MIIMODE_RMIIP = 2,
	LTQ_MII_MII_CFG_MIIMODE_RMIIM = 3,
	LTQ_MII_MII_CFG_MIIMODE_RGMII = 4,
};

struct ltq_eth_priv {
	struct ltq_dma_device *dma_dev;
	struct mii_dev *bus;
	struct eth_device *dev;
	struct phy_device *phymap[LTQ_ETHSW_MAX_GMAC];
	int rx_num;
};

enum ltq_mdio_mbusy {
	LTQ_MDIO_MBUSY_IDLE = 0,
	LTQ_MDIO_MBUSY_BUSY = 1,
};

enum ltq_mdio_op {
	LTQ_MDIO_OP_WRITE = 1,
	LTQ_MDIO_OP_READ = 2,
};

struct ltq_mdio_access {
	union {
		struct {
			unsigned rsvd:3;
			unsigned mbusy:1;
			unsigned op:2;
			unsigned phyad:5;
			unsigned regad:5;
		} bits;
		u16 val;
	};
};

enum LTQ_ETH_PORT_FLAGS {
	LTQ_ETH_PORT_NONE       = 0,
	LTQ_ETH_PORT_PHY        = 1,
	LTQ_ETH_PORT_SWITCH     = (1 << 1),
	LTQ_ETH_PORT_MAC        = (1 << 2),
};

struct ltq_eth_port_config {
	u8 num;
	u8 phy_addr;
	u16 flags;
	phy_interface_t phy_if;
};

struct ltq_eth_board_config {
	const struct ltq_eth_port_config *ports;
	int num_ports;
};

static const struct ltq_eth_port_config eth_port_config[] = {
	/* GMAC0: external Lantiq PEF7071 10/100/1000 PHY for LAN port 0 */
	{ 0, 0x0, LTQ_ETH_PORT_PHY, PHY_INTERFACE_MODE_RGMII },
	/* GMAC1: external Lantiq PEF7071 10/100/1000 PHY for LAN port 1 */
	{ 1, 0x1, LTQ_ETH_PORT_PHY, PHY_INTERFACE_MODE_RGMII },
};

static const struct ltq_eth_board_config board_config = {
	.ports = eth_port_config,
	.num_ports = ARRAY_SIZE(eth_port_config),
};

static struct ltq_ethsw_mac_pdi_regs *ltq_ethsw_mac_pdi_regs =
	(struct ltq_ethsw_mac_pdi_regs *) CKSEG1ADDR(LTQ_SWITCH_MAC_PDI_0_BASE);

static struct ltq_ethsw_mdio_pdi_regs *ltq_ethsw_mdio_pdi_regs =
	(struct ltq_ethsw_mdio_pdi_regs *) CKSEG1ADDR(LTQ_SWITCH_MDIO_PDI_BASE);

static struct ltq_ethsw_mii_pdi_regs *ltq_ethsw_mii_pdi_regs =
	(struct ltq_ethsw_mii_pdi_regs *) CKSEG1ADDR(LTQ_SWITCH_MII_PDI_BASE);

static struct ltq_ethsw_pmac_pdi_regs *ltq_ethsw_pmac_pdi_regs =
	(struct ltq_ethsw_pmac_pdi_regs *) CKSEG1ADDR(LTQ_SWITCH_PMAC_PDI_BASE);


#define MAX_DMA_CHAN		0x8
#define MAX_DMA_CRC_LEN		0x4
#define MAX_DMA_DATA_LEN	0x600

/* use 2 static channels for TX/RX
   depending on the SoC we need to use different DMA channels for ethernet */
#define LTQ_ETOP_TX_CHANNEL	1
#define LTQ_ETOP_RX_CHANNEL	0

#define IS_TX(x)		(x == LTQ_ETOP_TX_CHANNEL)
#define IS_RX(x)		(x == LTQ_ETOP_RX_CHANNEL)

#define DRV_VERSION	"1.0"

static void __iomem *ltq_vrx200_membase;

struct ltq_vrx200_chan {
	int idx;
	int tx_free;
	struct net_device *netdev;
	struct napi_struct napi;
	struct ltq_dma_channel dma;
	struct sk_buff *skb[LTQ_DESC_NUM];
};

struct ltq_vrx200_priv {
	struct net_device *netdev;
	struct ltq_eth_data *pldata;
	struct resource *res;

	struct mii_bus *mii_bus;
	struct phy_device *phydev;

	struct ltq_vrx200_chan ch[MAX_DMA_CHAN];
	int tx_free[MAX_DMA_CHAN >> 1];

	spinlock_t lock;

	struct clk *clk_ppe;
};

static int ltq_vrx200_mdio_wr(struct mii_bus *bus, int phy_addr,
				int phy_reg, u16 phy_data);

static int
ltq_vrx200_alloc_skb(struct ltq_vrx200_chan *ch)
{
	ch->skb[ch->dma.desc] = dev_alloc_skb(MAX_DMA_DATA_LEN);
	if (!ch->skb[ch->dma.desc])
		return -ENOMEM;
	ch->dma.desc_base[ch->dma.desc].addr = dma_map_single(NULL,
		ch->skb[ch->dma.desc]->data, MAX_DMA_DATA_LEN,
		DMA_FROM_DEVICE);
	ch->dma.desc_base[ch->dma.desc].addr =
		CPHYSADDR(ch->skb[ch->dma.desc]->data);
	ch->dma.desc_base[ch->dma.desc].ctl =
		LTQ_DMA_OWN | LTQ_DMA_RX_OFFSET(NET_IP_ALIGN) |
		MAX_DMA_DATA_LEN;
	skb_reserve(ch->skb[ch->dma.desc], NET_IP_ALIGN);
	return 0;
}

static void
ltq_vrx200_hw_receive(struct ltq_vrx200_chan *ch)
{
	struct ltq_vrx200_priv *priv = netdev_priv(ch->netdev);
	struct ltq_dma_desc *desc = &ch->dma.desc_base[ch->dma.desc];
	struct sk_buff *skb = ch->skb[ch->dma.desc];
	int len = (desc->ctl & LTQ_DMA_SIZE_MASK) - MAX_DMA_CRC_LEN;
	unsigned long flags;

	spin_lock_irqsave(&priv->lock, flags);
	if (ltq_vrx200_alloc_skb(ch)) {
		netdev_err(ch->netdev,
			"failed to allocate new rx buffer, stopping DMA\n");
		ltq_dma_close(&ch->dma);
	}
	ch->dma.desc++;
	ch->dma.desc %= LTQ_DESC_NUM;
	spin_unlock_irqrestore(&priv->lock, flags);

	skb_put(skb, len);
	skb->dev = ch->netdev;
	skb->protocol = eth_type_trans(skb, ch->netdev);
	netif_receive_skb(skb);
}

static int
ltq_vrx200_poll_rx(struct napi_struct *napi, int budget)
{
	struct ltq_vrx200_chan *ch = container_of(napi,
				struct ltq_vrx200_chan, napi);
	struct ltq_vrx200_priv *priv = netdev_priv(ch->netdev);
	int rx = 0;
	int complete = 0;
	unsigned long flags;

	while ((rx < budget) && !complete) {
		struct ltq_dma_desc *desc = &ch->dma.desc_base[ch->dma.desc];

		if ((desc->ctl & (LTQ_DMA_OWN | LTQ_DMA_C)) == LTQ_DMA_C) {
			ltq_vrx200_hw_receive(ch);
			rx++;
		} else {
			complete = 1;
		}
	}
	if (complete || !rx) {
		napi_complete(&ch->napi);
		spin_lock_irqsave(&priv->lock, flags);
		ltq_dma_ack_irq(&ch->dma);
		spin_unlock_irqrestore(&priv->lock, flags);
	}
	return rx;
}

static int
ltq_vrx200_poll_tx(struct napi_struct *napi, int budget)
{
	struct ltq_vrx200_chan *ch =
		container_of(napi, struct ltq_vrx200_chan, napi);
	struct ltq_vrx200_priv *priv = netdev_priv(ch->netdev);
	struct netdev_queue *txq =
		netdev_get_tx_queue(ch->netdev, ch->idx >> 1);
	unsigned long flags;

	spin_lock_irqsave(&priv->lock, flags);
	while ((ch->dma.desc_base[ch->tx_free].ctl &
			(LTQ_DMA_OWN | LTQ_DMA_C)) == LTQ_DMA_C) {
		dev_kfree_skb_any(ch->skb[ch->tx_free]);
		ch->skb[ch->tx_free] = NULL;
		memset(&ch->dma.desc_base[ch->tx_free], 0,
			sizeof(struct ltq_dma_desc));
		ch->tx_free++;
		ch->tx_free %= LTQ_DESC_NUM;
	}
	spin_unlock_irqrestore(&priv->lock, flags);

	if (netif_tx_queue_stopped(txq))
		netif_tx_start_queue(txq);
	napi_complete(&ch->napi);
	spin_lock_irqsave(&priv->lock, flags);
	ltq_dma_ack_irq(&ch->dma);
	spin_unlock_irqrestore(&priv->lock, flags);
	return 1;
}

static irqreturn_t
ltq_vrx200_dma_irq(int irq, void *_priv)
{
	struct ltq_vrx200_priv *priv = _priv;
	int ch = irq - LTQ_DMA_ETOP;

	napi_schedule(&priv->ch[ch].napi);
	return IRQ_HANDLED;
}

static void
ltq_vrx200_free_channel(struct net_device *dev, struct ltq_vrx200_chan *ch)
{
	struct ltq_vrx200_priv *priv = netdev_priv(dev);

	ltq_dma_free(&ch->dma);
	if (ch->dma.irq)
		free_irq(ch->dma.irq, priv);
	if (IS_RX(ch->idx)) {
		int desc;
		for (desc = 0; desc < LTQ_DESC_NUM; desc++)
			dev_kfree_skb_any(ch->skb[ch->dma.desc]);
	}
}

static void
ltq_vrx200_hw_exit(struct net_device *dev)
{
	struct ltq_vrx200_priv *priv = netdev_priv(dev);
	int i;

	clk_disable(priv->clk_ppe);

	for (i = 0; i < MAX_DMA_CHAN; i++)
		if (IS_TX(i) || IS_RX(i))
			ltq_vrx200_free_channel(dev, &priv->ch[i]);
}

static void *ltq_eth_phy_addr_reg(int num)
{
	switch (num) {
	case 0:
		return &ltq_ethsw_mdio_pdi_regs->phy_addr_0;
	case 1:
		return &ltq_ethsw_mdio_pdi_regs->phy_addr_1;
	case 2:
		return &ltq_ethsw_mdio_pdi_regs->phy_addr_2;
	case 3:
		return &ltq_ethsw_mdio_pdi_regs->phy_addr_3;
	case 4:
		return &ltq_ethsw_mdio_pdi_regs->phy_addr_4;
	case 5:
		return &ltq_ethsw_mdio_pdi_regs->phy_addr_5;
	}

	return NULL;
}

static void *ltq_eth_mii_cfg_reg(int num)
{
	switch (num) {
	case 0:
		return &ltq_ethsw_mii_pdi_regs->mii_cfg0;
	case 1:
		return &ltq_ethsw_mii_pdi_regs->mii_cfg1;
	case 2:
		return &ltq_ethsw_mii_pdi_regs->mii_cfg2;
	case 3:
		return &ltq_ethsw_mii_pdi_regs->mii_cfg3;
	case 4:
		return &ltq_ethsw_mii_pdi_regs->mii_cfg4;
	case 5:
		return &ltq_ethsw_mii_pdi_regs->mii_cfg5;
	}

	return NULL;
}

static void ltq_eth_gmac_update(struct phy_device *phydev, int num)
{
	struct ltq_mdio_phy_addr_reg phy_addr_reg;
	struct ltq_mii_mii_cfg_reg mii_cfg_reg;
	void *phy_addr = ltq_eth_phy_addr_reg(num);
	void *mii_cfg = ltq_eth_mii_cfg_reg(num);

	phy_addr_reg.val = ltq_r32(phy_addr);
	mii_cfg_reg.val = ltq_r32(mii_cfg);

	phy_addr_reg.bits.addr = phydev->addr;

	if (phydev->link)
		phy_addr_reg.bits.lnkst = LTQ_MDIO_PHY_ADDR_LNKST_UP;
	else
		phy_addr_reg.bits.lnkst = LTQ_MDIO_PHY_ADDR_LNKST_DOWN;

	switch (phydev->speed) {
	case SPEED_1000:
		phy_addr_reg.bits.speed = LTQ_MDIO_PHY_ADDR_SPEED_G1;
		mii_cfg_reg.bits.miirate = LTQ_MII_MII_CFG_MIIRATE_M125;
		break;
	case SPEED_100:
		phy_addr_reg.bits.speed = LTQ_MDIO_PHY_ADDR_SPEED_M100;
		switch (mii_cfg_reg.bits.miimode) {
		case LTQ_MII_MII_CFG_MIIMODE_RMIIM:
		case LTQ_MII_MII_CFG_MIIMODE_RMIIP:
			mii_cfg_reg.bits.miirate = LTQ_MII_MII_CFG_MIIRATE_M50;
			break;
		default:
			mii_cfg_reg.bits.miirate = LTQ_MII_MII_CFG_MIIRATE_M25;
			break;
		}
		break;
	default:
		phy_addr_reg.bits.speed = LTQ_MDIO_PHY_ADDR_SPEED_M10;
		mii_cfg_reg.bits.miirate = LTQ_MII_MII_CFG_MIIRATE_M2P5;
		break;
	}

	if (phydev->duplex == DUPLEX_FULL)
		phy_addr_reg.bits.fdup = LTQ_MDIO_PHY_ADDR_FDUP_ENABLE;
	else
		phy_addr_reg.bits.fdup = LTQ_MDIO_PHY_ADDR_FDUP_DISABLE;

	dbg_ltq_writel(phy_addr, phy_addr_reg.val);
	dbg_ltq_writel(mii_cfg, mii_cfg_reg.val);
	udelay(1);
}


static void ltq_eth_port_config(struct ltq_vrx200_priv *priv,
	const struct ltq_eth_port_config *port)
{
	struct ltq_mii_mii_cfg_reg mii_cfg_reg;
	void *mii_cfg = ltq_eth_mii_cfg_reg(port->num);
	int setup_gpio = 0;

	mii_cfg_reg.val = ltq_r32(mii_cfg);


	switch (port->num) {
	case 0: /* xMII0 */
	case 1: /* xMII1 */
		switch (port->phy_if) {
		case PHY_INTERFACE_MODE_MII:
			if (port->flags & LTQ_ETH_PORT_PHY)
				/* MII MAC mode, connected to external PHY */
				mii_cfg_reg.bits.miimode =
					LTQ_MII_MII_CFG_MIIMODE_MIIM;
			else
				/* MII PHY mode, connected to external MAC */
				mii_cfg_reg.bits.miimode =
					LTQ_MII_MII_CFG_MIIMODE_MIIP;
				setup_gpio = 1;
			break;
		case PHY_INTERFACE_MODE_RMII:
			if (port->flags & LTQ_ETH_PORT_PHY)
				/* RMII MAC mode, connected to external PHY */
				mii_cfg_reg.bits.miimode =
					LTQ_MII_MII_CFG_MIIMODE_RMIIM;
			else
				/* RMII PHY mode, connected to external MAC */
				mii_cfg_reg.bits.miimode =
					LTQ_MII_MII_CFG_MIIMODE_RMIIP;
				setup_gpio = 1;
				break;
		case PHY_INTERFACE_MODE_RGMII:
			/* RGMII MAC mode, connected to external PHY */
			mii_cfg_reg.bits.miimode =
				LTQ_MII_MII_CFG_MIIMODE_RGMII;
			setup_gpio = 1;
			break;
		default:
			break;
		}
		break;
	case 2: /* internal GPHY0 */
	case 3: /* internal GPHY0 */
	case 4: /* internal GPHY1 */
		switch (port->phy_if) {
			case PHY_INTERFACE_MODE_MII:
			case PHY_INTERFACE_MODE_GMII:
				/* MII MAC mode, connected to internal GPHY */
				mii_cfg_reg.bits.miimode =
					LTQ_MII_MII_CFG_MIIMODE_MIIM;
				setup_gpio = 1;
				break;
			default:
				break;
		}
		break;
	case 5: /* internal GPHY1 or xMII2 */
		switch (port->phy_if) {
		case PHY_INTERFACE_MODE_MII:
			/* MII MAC mode, connected to internal GPHY */
			mii_cfg_reg.bits.miimode =
				LTQ_MII_MII_CFG_MIIMODE_MIIM;
			setup_gpio = 1;
			break;
		case PHY_INTERFACE_MODE_RGMII:
			/* RGMII MAC mode, connected to external PHY */
			mii_cfg_reg.bits.miimode =
				LTQ_MII_MII_CFG_MIIMODE_RGMII;
			setup_gpio = 1;
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}

	/* Enable MII interface */
	mii_cfg_reg.bits.en = port->flags ? 1 : 0;
	dbg_ltq_writel(mii_cfg, mii_cfg_reg.val);

}

static void ltq_eth_gmac_init(int num)
{
	struct ltq_mdio_phy_addr_reg phy_addr_reg;
	struct ltq_mii_mii_cfg_reg mii_cfg_reg;
	void *phy_addr = ltq_eth_phy_addr_reg(num);
	void *mii_cfg = ltq_eth_mii_cfg_reg(num);
	struct ltq_ethsw_mac_pdi_x_regs *mac_pdi_regs;

	mac_pdi_regs = &ltq_ethsw_mac_pdi_regs->mac[num];

	/* Reset PHY status to link down */
	phy_addr_reg.val = ltq_r32(phy_addr);
	phy_addr_reg.bits.addr = num;
	phy_addr_reg.bits.lnkst = LTQ_MDIO_PHY_ADDR_LNKST_DOWN;
	phy_addr_reg.bits.speed = LTQ_MDIO_PHY_ADDR_SPEED_M10;
	phy_addr_reg.bits.fdup = LTQ_MDIO_PHY_ADDR_FDUP_DISABLE;
	dbg_ltq_writel(phy_addr, phy_addr_reg.val);

	/* Reset and disable MII interface */
	mii_cfg_reg.val = ltq_r32(mii_cfg);
	mii_cfg_reg.bits.en = 0;
	mii_cfg_reg.bits.res = 1;
	mii_cfg_reg.bits.miirate = LTQ_MII_MII_CFG_MIIRATE_M2P5;
	dbg_ltq_writel(mii_cfg, mii_cfg_reg.val);

	/*
	* Enable padding of short frames, enable frame checksum generation
	* in transmit direction
	*/
	dbg_ltq_writel(&mac_pdi_regs->ctrl_0, LTQ_ETHSW_MAC_CTRL0_PADEN |
		LTQ_ETHSW_MAC_CTRL0_FCS);

	/* Set inter packet gap size to 12 bytes */
	dbg_ltq_writel(&mac_pdi_regs->ctrl_1, 12);

	/*
	* Configure frame length checks:
	* - allow jumbo frames
	* - enable long length check
	* - enable short length without VLAN tags
	*/
	dbg_ltq_writel(&mac_pdi_regs->ctrl_2, LTQ_ETHSW_MAC_CTRL2_MLEN |
		LTQ_ETHSW_MAC_CTRL2_LCHKL |
		LTQ_ETHSW_MAC_CTRL2_LCHKS_UNTAG);
}


static void ltq_eth_pmac_init(void)
{
	struct ltq_ethsw_mac_pdi_x_regs *mac_pdi_regs;

	mac_pdi_regs = &ltq_ethsw_mac_pdi_regs->mac[LTQ_ETHSW_PMAC];

	/*
	* Enable padding of short frames, enable frame checksum generation
	* in transmit direction
	*/
	dbg_ltq_writel(&mac_pdi_regs->ctrl_0, LTQ_ETHSW_MAC_CTRL0_PADEN |
		LTQ_ETHSW_MAC_CTRL0_FCS);

	/*
	* Configure frame length checks:
	* - allow jumbo frames
	* - enable long length check
	* - enable short length without VLAN tags
	*/
	dbg_ltq_writel(&mac_pdi_regs->ctrl_2, LTQ_ETHSW_MAC_CTRL2_MLEN |
		LTQ_ETHSW_MAC_CTRL2_LCHKL |
		LTQ_ETHSW_MAC_CTRL2_LCHKS_UNTAG);

	/*
	* Apply workaround for buffer congestion:
	* - shorten preambel to 1 byte
	* - set minimum inter packet gap size to 7 bytes
	* - enable receive buffer bypass mode
	*/
	dbg_ltq_writel(&mac_pdi_regs->ctrl_1, LTQ_ETHSW_MAC_CTRL1_SHORTPRE | 7);
	dbg_ltq_writel(&mac_pdi_regs->ctrl_6,
		(6 << LTQ_ETHSW_MAC_CTRL6_RBUF_DLY_WP_SHIFT) |
		LTQ_ETHSW_MAC_CTRL6_RXBUF_BYPASS);

	/* Set request assertion threshold to 8, IPG counter to 11 */
	dbg_ltq_writel(&ltq_ethsw_pmac_pdi_regs->rx_ipg, 0x8B);

	/*
	* Configure frame header control:
	* - enable reaction on pause frames (flow control)
	* - remove CRC for packets from PMAC to DMA
	* - add CRC for packets from DMA to PMAC
	*/
	dbg_ltq_writel(&ltq_ethsw_pmac_pdi_regs->hd_ctl, LTQ_ETHSW_PMAC_HD_CTL_FC |
		/*LTQ_ETHSW_PMAC_HD_CTL_RC | */LTQ_ETHSW_PMAC_HD_CTL_AC);
}

static int
ltq_vrx200_hw_init(struct net_device *dev)
{
	struct ltq_vrx200_priv *priv = netdev_priv(dev);
	int err = 0;
	int i;

	netdev_info(dev, "setting up dma\n");
	ltq_dma_init_port(DMA_PORT_ETOP);

	netdev_info(dev, "setting up pmu\n");
	clk_enable(priv->clk_ppe);

	/* Reset ethernet and switch subsystems */
	netdev_info(dev, "reset core\n");
	ltq_reset_once(BIT(8), 10);

	/* Enable switch macro */
	ltq_setbits(&ltq_ethsw_mdio_pdi_regs->glob_ctrl,
		LTQ_ETHSW_GLOB_CTRL_SE);

	/* Disable MDIO auto-polling for all ports */
	dbg_ltq_writel(&ltq_ethsw_mdio_pdi_regs->mdc_cfg_0, 0);

	/*
	 * Enable and set MDIO management clock to 2.5 MHz. This is the
	 * maximum clock for FE PHYs.
	 * Formula for clock is:
	 *
	 *      50 MHz
	 * x = ----------- - 1
	 *      2 * f_MDC
	 */
	dbg_ltq_writel(&ltq_ethsw_mdio_pdi_regs->mdc_cfg_1,
		LTQ_ETHSW_MDC_CFG1_MCEN | 9);

	/* Init MAC connected to CPU  */
	ltq_eth_pmac_init();

	/* Init MACs connected to external MII interfaces */
	for (i = 0; i < LTQ_ETHSW_MAX_GMAC; i++)
		ltq_eth_gmac_init(i);

	for (i = 0; i < MAX_DMA_CHAN && !err; i++) {
		int irq = LTQ_DMA_ETOP + i;
		struct ltq_vrx200_chan *ch = &priv->ch[i];

		ch->idx = ch->dma.nr = i;

		if (IS_TX(i)) {
			ltq_dma_alloc_tx(&ch->dma);
			err = request_irq(irq, ltq_vrx200_dma_irq, IRQF_DISABLED,
				"vrx200_tx", priv);
		} else if (IS_RX(i)) {
			ltq_dma_alloc_rx(&ch->dma);
			for (ch->dma.desc = 0; ch->dma.desc < LTQ_DESC_NUM;
					ch->dma.desc++)
				if (ltq_vrx200_alloc_skb(ch))
					err = -ENOMEM;
			ch->dma.desc = 0;
			err = request_irq(irq, ltq_vrx200_dma_irq, IRQF_DISABLED,
				"vrx200_rx", priv);
		}
		if (!err)
			ch->dma.irq = irq;
	}
	for (i = 0; i < board_config.num_ports; i++)
		ltq_eth_port_config(priv, &board_config.ports[i]);
	return err;
}

static void
ltq_vrx200_get_drvinfo(struct net_device *dev, struct ethtool_drvinfo *info)
{
	strcpy(info->driver, "Lantiq ETOP");
	strcpy(info->bus_info, "internal");
	strcpy(info->version, DRV_VERSION);
}

static int
ltq_vrx200_get_settings(struct net_device *dev, struct ethtool_cmd *cmd)
{
	struct ltq_vrx200_priv *priv = netdev_priv(dev);

	return phy_ethtool_gset(priv->phydev, cmd);
}

static int
ltq_vrx200_set_settings(struct net_device *dev, struct ethtool_cmd *cmd)
{
	struct ltq_vrx200_priv *priv = netdev_priv(dev);

	return phy_ethtool_sset(priv->phydev, cmd);
}

static int
ltq_vrx200_nway_reset(struct net_device *dev)
{
	struct ltq_vrx200_priv *priv = netdev_priv(dev);

	return phy_start_aneg(priv->phydev);
}

static const struct ethtool_ops ltq_vrx200_ethtool_ops = {
	.get_drvinfo = ltq_vrx200_get_drvinfo,
	.get_settings = ltq_vrx200_get_settings,
	.set_settings = ltq_vrx200_set_settings,
	.nway_reset = ltq_vrx200_nway_reset,
};

static inline int ltq_mdio_poll(struct mii_bus *bus)
{
	struct ltq_mdio_access acc;
	unsigned cnt = 10000;

	while (likely(cnt--)) {
		acc.val = ltq_r32(&ltq_ethsw_mdio_pdi_regs->mdio_ctrl);
		if (!acc.bits.mbusy)
			return 0;
	}

	return 1;
}

static int
ltq_vrx200_mdio_wr(struct mii_bus *bus, int addr, int regnum, u16 val)
{
	struct ltq_mdio_access acc;
	int ret;

	acc.val = 0;
	acc.bits.mbusy = LTQ_MDIO_MBUSY_BUSY;
	acc.bits.op = LTQ_MDIO_OP_WRITE;
	acc.bits.phyad = addr;
	acc.bits.regad = regnum;

	ret = ltq_mdio_poll(bus);
	if (ret)
		return ret;

	dbg_ltq_writel(&ltq_ethsw_mdio_pdi_regs->mdio_write, val);
	dbg_ltq_writel(&ltq_ethsw_mdio_pdi_regs->mdio_ctrl, acc.val);

	return 0;
}

static int
ltq_vrx200_mdio_rd(struct mii_bus *bus, int addr, int regnum)
{
	struct ltq_mdio_access acc;
	int ret;

	acc.val = 0;
	acc.bits.mbusy = LTQ_MDIO_MBUSY_BUSY;
	acc.bits.op = LTQ_MDIO_OP_READ;
	acc.bits.phyad = addr;
	acc.bits.regad = regnum;

	ret = ltq_mdio_poll(bus);
	if (ret)
		goto timeout;

	dbg_ltq_writel(&ltq_ethsw_mdio_pdi_regs->mdio_ctrl, acc.val);

	ret = ltq_mdio_poll(bus);
	if (ret)
		goto timeout;

	ret = ltq_r32(&ltq_ethsw_mdio_pdi_regs->mdio_read);

	return ret;
timeout:
	return -1;
}

static void
ltq_vrx200_mdio_link(struct net_device *dev)
{
	struct ltq_vrx200_priv *priv = netdev_priv(dev);
	ltq_eth_gmac_update(priv->phydev, 0);
}

static int
ltq_vrx200_mdio_probe(struct net_device *dev)
{
	struct ltq_vrx200_priv *priv = netdev_priv(dev);
	struct phy_device *phydev = NULL;
	int val;

	phydev = priv->mii_bus->phy_map[0];

	if (!phydev) {
		netdev_err(dev, "no PHY found\n");
		return -ENODEV;
	}

	phydev = phy_connect(dev, dev_name(&phydev->dev), &ltq_vrx200_mdio_link,
			0, 0);

	if (IS_ERR(phydev)) {
		netdev_err(dev, "Could not attach to PHY\n");
		return PTR_ERR(phydev);
	}

	phydev->supported &= (SUPPORTED_10baseT_Half
			      | SUPPORTED_10baseT_Full
			      | SUPPORTED_100baseT_Half
			      | SUPPORTED_100baseT_Full
			      | SUPPORTED_1000baseT_Half
			      | SUPPORTED_1000baseT_Full
			      | SUPPORTED_Autoneg
			      | SUPPORTED_MII
			      | SUPPORTED_TP);
	phydev->advertising = phydev->supported;
	priv->phydev = phydev;

	pr_info("%s: attached PHY [%s] (phy_addr=%s, irq=%d)\n",
	       dev->name, phydev->drv->name,
	       dev_name(&phydev->dev), phydev->irq);

	val = ltq_vrx200_mdio_rd(priv->mii_bus, MDIO_DEVAD_NONE, MII_CTRL1000);
	val |= ADVERTIZE_MPD;
	ltq_vrx200_mdio_wr(priv->mii_bus, MDIO_DEVAD_NONE, MII_CTRL1000, val);
	ltq_vrx200_mdio_wr(priv->mii_bus, 0, 0, 0x1040);

        phy_start_aneg(phydev);

	return 0;
}

static int
ltq_vrx200_mdio_init(struct net_device *dev)
{
	struct ltq_vrx200_priv *priv = netdev_priv(dev);
	int i;
	int err;

	priv->mii_bus = mdiobus_alloc();
	if (!priv->mii_bus) {
		netdev_err(dev, "failed to allocate mii bus\n");
		err = -ENOMEM;
		goto err_out;
	}

	priv->mii_bus->priv = dev;
	priv->mii_bus->read = ltq_vrx200_mdio_rd;
	priv->mii_bus->write = ltq_vrx200_mdio_wr;
	priv->mii_bus->name = "ltq_mii";
	snprintf(priv->mii_bus->id, MII_BUS_ID_SIZE, "%x", 0);
	priv->mii_bus->irq = kmalloc(sizeof(int) * PHY_MAX_ADDR, GFP_KERNEL);
	if (!priv->mii_bus->irq) {
		err = -ENOMEM;
		goto err_out_free_mdiobus;
	}

	for (i = 0; i < PHY_MAX_ADDR; ++i)
		priv->mii_bus->irq[i] = PHY_POLL;

	if (mdiobus_register(priv->mii_bus)) {
		err = -ENXIO;
		goto err_out_free_mdio_irq;
	}

	if (ltq_vrx200_mdio_probe(dev)) {
		err = -ENXIO;
		goto err_out_unregister_bus;
	}
	return 0;

err_out_unregister_bus:
	mdiobus_unregister(priv->mii_bus);
err_out_free_mdio_irq:
	kfree(priv->mii_bus->irq);
err_out_free_mdiobus:
	mdiobus_free(priv->mii_bus);
err_out:
	return err;
}

static void
ltq_vrx200_mdio_cleanup(struct net_device *dev)
{
	struct ltq_vrx200_priv *priv = netdev_priv(dev);

	phy_disconnect(priv->phydev);
	mdiobus_unregister(priv->mii_bus);
	kfree(priv->mii_bus->irq);
	mdiobus_free(priv->mii_bus);
}

void phy_dump(struct net_device *dev)
{
        struct ltq_vrx200_priv *priv = netdev_priv(dev);
	int i;
	for (i = 0; i < 0x1F; i++) {
		unsigned int val = ltq_vrx200_mdio_rd(priv->mii_bus, 0, i);
		printk("%d %4X\n", i, val);
	}
}

static int
ltq_vrx200_open(struct net_device *dev)
{
	struct ltq_vrx200_priv *priv = netdev_priv(dev);
	int i;
	unsigned long flags;

	for (i = 0; i < MAX_DMA_CHAN; i++) {
		struct ltq_vrx200_chan *ch = &priv->ch[i];

		if (!IS_TX(i) && (!IS_RX(i)))
			continue;
		napi_enable(&ch->napi);
		spin_lock_irqsave(&priv->lock, flags);
		ltq_dma_open(&ch->dma);
		spin_unlock_irqrestore(&priv->lock, flags);
	}
	if (priv->phydev) {
		phy_start(priv->phydev);
		phy_dump(dev);
	}
	netif_tx_start_all_queues(dev);
	return 0;
}

static int
ltq_vrx200_stop(struct net_device *dev)
{
	struct ltq_vrx200_priv *priv = netdev_priv(dev);
	int i;
	unsigned long flags;

	netif_tx_stop_all_queues(dev);
	if (priv->phydev)
		phy_stop(priv->phydev);
	for (i = 0; i < MAX_DMA_CHAN; i++) {
		struct ltq_vrx200_chan *ch = &priv->ch[i];

		if (!IS_RX(i) && !IS_TX(i))
			continue;
		napi_disable(&ch->napi);
		spin_lock_irqsave(&priv->lock, flags);
		ltq_dma_close(&ch->dma);
		spin_unlock_irqrestore(&priv->lock, flags);
	}
	return 0;
}

static int
ltq_vrx200_tx(struct sk_buff *skb, struct net_device *dev)
{
	int queue = skb_get_queue_mapping(skb);
	struct netdev_queue *txq = netdev_get_tx_queue(dev, queue);
	struct ltq_vrx200_priv *priv = netdev_priv(dev);
	struct ltq_vrx200_chan *ch = &priv->ch[(queue << 1) | 1];
	struct ltq_dma_desc *desc = &ch->dma.desc_base[ch->dma.desc];
	unsigned long flags;
	u32 byte_offset;
	int len;

	len = skb->len < ETH_ZLEN ? ETH_ZLEN : skb->len;

	if ((desc->ctl & (LTQ_DMA_OWN | LTQ_DMA_C)) || ch->skb[ch->dma.desc]) {
		netdev_err(dev, "tx ring full\n");
		netif_tx_stop_queue(txq);
		return NETDEV_TX_BUSY;
	}

	/* dma needs to start on a 16 byte aligned address */
	byte_offset = CPHYSADDR(skb->data) % 16;
	ch->skb[ch->dma.desc] = skb;

	dev->trans_start = jiffies;

	spin_lock_irqsave(&priv->lock, flags);
	desc->addr = ((unsigned int) dma_map_single(NULL, skb->data, len,
						DMA_TO_DEVICE)) - byte_offset;
	wmb();
	desc->ctl = LTQ_DMA_OWN | LTQ_DMA_SOP | LTQ_DMA_EOP |
		LTQ_DMA_TX_OFFSET(byte_offset) | (len & LTQ_DMA_SIZE_MASK);
	ch->dma.desc++;
	ch->dma.desc %= LTQ_DESC_NUM;
	spin_unlock_irqrestore(&priv->lock, flags);

	if (ch->dma.desc_base[ch->dma.desc].ctl & LTQ_DMA_OWN)
		netif_tx_stop_queue(txq);

	return NETDEV_TX_OK;
}

static int
ltq_vrx200_ioctl(struct net_device *dev, struct ifreq *rq, int cmd)
{
	struct ltq_vrx200_priv *priv = netdev_priv(dev);

	/* TODO: mii-toll reports "No MII transceiver present!." ?!*/
	return phy_mii_ioctl(priv->phydev, rq, cmd);
}

static u16
ltq_vrx200_select_queue(struct net_device *dev, struct sk_buff *skb)
{
	/* we are currently only using the first queue */
	return 0;
}

static int
ltq_vrx200_init(struct net_device *dev)
{
	struct ltq_vrx200_priv *priv = netdev_priv(dev);
	struct sockaddr mac;
	int err;

	ether_setup(dev);
	dev->watchdog_timeo = 10 * HZ;

	err = ltq_vrx200_hw_init(dev);
	if (err)
		goto err_hw;

	memcpy(&mac, &priv->pldata->mac, sizeof(struct sockaddr));
	if (!is_valid_ether_addr(mac.sa_data)) {
		pr_warn("vrx200: invalid MAC, using random\n");
		random_ether_addr(mac.sa_data);
	}
	eth_mac_addr(dev, &mac);

	if (!ltq_vrx200_mdio_init(dev))
		dev->ethtool_ops = &ltq_vrx200_ethtool_ops;
	else
		pr_warn("vrx200: mdio probe failed\n");;
	return 0;

err_hw:
	ltq_vrx200_hw_exit(dev);
	return err;
}

static void
ltq_vrx200_tx_timeout(struct net_device *dev)
{
	int err;

	ltq_vrx200_hw_exit(dev);
	err = ltq_vrx200_hw_init(dev);
	if (err)
		goto err_hw;
	dev->trans_start = jiffies;
	netif_wake_queue(dev);
	return;

err_hw:
	ltq_vrx200_hw_exit(dev);
	netdev_err(dev, "failed to restart vrx200 after TX timeout\n");
}

static const struct net_device_ops ltq_eth_netdev_ops = {
	.ndo_open = ltq_vrx200_open,
	.ndo_stop = ltq_vrx200_stop,
	.ndo_start_xmit = ltq_vrx200_tx,
	.ndo_change_mtu = eth_change_mtu,
	.ndo_do_ioctl = ltq_vrx200_ioctl,
	.ndo_set_mac_address = eth_mac_addr,
	.ndo_validate_addr = eth_validate_addr,
	.ndo_select_queue = ltq_vrx200_select_queue,
	.ndo_init = ltq_vrx200_init,
	.ndo_tx_timeout = ltq_vrx200_tx_timeout,
};

static int __devinit
ltq_vrx200_probe(struct platform_device *pdev)
{
	struct net_device *dev;
	struct ltq_vrx200_priv *priv;
	struct resource *res;
	int err;
	int i;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		dev_err(&pdev->dev, "failed to get vrx200 resource\n");
		err = -ENOENT;
		goto err_out;
	}

	res = devm_request_mem_region(&pdev->dev, res->start,
		resource_size(res), dev_name(&pdev->dev));
	if (!res) {
		dev_err(&pdev->dev, "failed to request vrx200 resource\n");
		err = -EBUSY;
		goto err_out;
	}

	ltq_vrx200_membase = devm_ioremap_nocache(&pdev->dev,
		res->start, resource_size(res));
	if (!ltq_vrx200_membase) {
		dev_err(&pdev->dev, "failed to remap vrx200 engine %d\n",
			pdev->id);
		err = -ENOMEM;
		goto err_out;
	}

	if (ltq_gpio_request(&pdev->dev, 42, 2, 1, "MDIO") ||
			ltq_gpio_request(&pdev->dev, 43, 2, 1, "MDC")) {
		dev_err(&pdev->dev, "failed to request MDIO gpios\n");
		err = -EBUSY;
		goto err_out;
	}

	dev = alloc_etherdev_mq(sizeof(struct ltq_vrx200_priv), 4);
	strcpy(dev->name, "eth%d");
	dev->netdev_ops = &ltq_eth_netdev_ops;
	priv = netdev_priv(dev);
	priv->res = res;
	priv->pldata = dev_get_platdata(&pdev->dev);
	priv->netdev = dev;

	priv->clk_ppe = clk_get(&pdev->dev, NULL);
	if (IS_ERR(priv->clk_ppe))
		return PTR_ERR(priv->clk_ppe);

	spin_lock_init(&priv->lock);

	for (i = 0; i < MAX_DMA_CHAN; i++) {
		if (IS_TX(i))
			netif_napi_add(dev, &priv->ch[i].napi,
				ltq_vrx200_poll_tx, 8);
		else if (IS_RX(i))
			netif_napi_add(dev, &priv->ch[i].napi,
				ltq_vrx200_poll_rx, 32);
		priv->ch[i].netdev = dev;
	}

	err = register_netdev(dev);
	if (err)
		goto err_free;

	platform_set_drvdata(pdev, dev);
	return 0;

err_free:
	kfree(dev);
err_out:
	return err;
}

static int __devexit
ltq_vrx200_remove(struct platform_device *pdev)
{
	struct net_device *dev = platform_get_drvdata(pdev);

	if (dev) {
		netif_tx_stop_all_queues(dev);
		ltq_vrx200_hw_exit(dev);
		ltq_vrx200_mdio_cleanup(dev);
		unregister_netdev(dev);
	}
	return 0;
}

static struct platform_driver ltq_mii_driver = {
	.probe = ltq_vrx200_probe,
	.remove = __devexit_p(ltq_vrx200_remove),
	.driver = {
		.name = "ltq_vrx200",
		.owner = THIS_MODULE,
	},
};

module_platform_driver(ltq_mii_driver);

MODULE_AUTHOR("John Crispin <blogic@openwrt.org>");
MODULE_DESCRIPTION("Lantiq SoC ETOP");
MODULE_LICENSE("GPL");
