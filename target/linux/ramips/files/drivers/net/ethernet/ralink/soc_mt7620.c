/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; version 2 of the License
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
 *   Copyright (C) 2009-2013 John Crispin <blogic@openwrt.org>
 */

#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/if_vlan.h>

#include <asm/mach-ralink/ralink_regs.h>

#include <mt7620.h>
#include "ralink_soc_eth.h"
#include "gsw_mt7620a.h"

#define MT7620A_CDMA_CSG_CFG	0x400
#define MT7620_DMA_VID		(MT7620A_CDMA_CSG_CFG | 0x30)
#define MT7620A_DMA_2B_OFFSET	BIT(31)
#define MT7620A_RESET_FE	BIT(21)
#define MT7620A_RESET_ESW	BIT(23)
#define MT7620_L4_VALID		BIT(23)

#define MT7620_TX_DMA_UDF	BIT(15)
#define TX_DMA_FP_BMAP		((0xff) << 19)

#define SYSC_REG_RESET_CTRL     0x34

#define CDMA_ICS_EN		BIT(2)
#define CDMA_UCS_EN		BIT(1)
#define CDMA_TCS_EN		BIT(0)

#define GDMA_ICS_EN		BIT(22)
#define GDMA_TCS_EN		BIT(21)
#define GDMA_UCS_EN		BIT(20)

/* frame engine counters */
#define MT7620_REG_MIB_OFFSET	0x1000
#define MT7620_PPE_AC_BCNT0	(MT7620_REG_MIB_OFFSET + 0x00)
#define MT7620_GDM1_TX_GBCNT	(MT7620_REG_MIB_OFFSET + 0x300)
#define MT7620_GDM2_TX_GBCNT	(MT7620_GDM1_TX_GBCNT + 0x40)

static const u32 mt7620_reg_table[FE_REG_COUNT] = {
	[FE_REG_PDMA_GLO_CFG] = RT5350_PDMA_GLO_CFG,
	[FE_REG_PDMA_RST_CFG] = RT5350_PDMA_RST_CFG,
	[FE_REG_DLY_INT_CFG] = RT5350_DLY_INT_CFG,
	[FE_REG_TX_BASE_PTR0] = RT5350_TX_BASE_PTR0,
	[FE_REG_TX_MAX_CNT0] = RT5350_TX_MAX_CNT0,
	[FE_REG_TX_CTX_IDX0] = RT5350_TX_CTX_IDX0,
	[FE_REG_RX_BASE_PTR0] = RT5350_RX_BASE_PTR0,
	[FE_REG_RX_MAX_CNT0] = RT5350_RX_MAX_CNT0,
	[FE_REG_RX_CALC_IDX0] = RT5350_RX_CALC_IDX0,
	[FE_REG_FE_INT_ENABLE] = RT5350_FE_INT_ENABLE,
	[FE_REG_FE_INT_STATUS] = RT5350_FE_INT_STATUS,
	[FE_REG_FE_DMA_VID_BASE] = MT7620_DMA_VID,
	[FE_REG_FE_COUNTER_BASE] = MT7620_GDM1_TX_GBCNT,
};

static void mt7620_fe_reset(void)
{
	rt_sysc_w32(MT7620A_RESET_FE | MT7620A_RESET_ESW, SYSC_REG_RESET_CTRL);
	rt_sysc_w32(0, SYSC_REG_RESET_CTRL);
}

static void mt7620_rxcsum_config(bool enable)
{
	if (enable)
		fe_w32(fe_r32(MT7620A_GDMA1_FWD_CFG) | (GDMA_ICS_EN |
					GDMA_TCS_EN | GDMA_UCS_EN),
				MT7620A_GDMA1_FWD_CFG);
	else
		fe_w32(fe_r32(MT7620A_GDMA1_FWD_CFG) & ~(GDMA_ICS_EN |
					GDMA_TCS_EN | GDMA_UCS_EN),
				MT7620A_GDMA1_FWD_CFG);
}

static void mt7620_txcsum_config(bool enable)
{
	if (enable)
		fe_w32(fe_r32(MT7620A_CDMA_CSG_CFG) | (CDMA_ICS_EN |
					CDMA_UCS_EN | CDMA_TCS_EN),
				MT7620A_CDMA_CSG_CFG);
	else
		fe_w32(fe_r32(MT7620A_CDMA_CSG_CFG) & ~(CDMA_ICS_EN |
					CDMA_UCS_EN | CDMA_TCS_EN),
				MT7620A_CDMA_CSG_CFG);
}

static int mt7620_fwd_config(struct fe_priv *priv)
{
	struct net_device *dev = priv_netdev(priv);

	fe_w32(fe_r32(MT7620A_GDMA1_FWD_CFG) & ~7, MT7620A_GDMA1_FWD_CFG);

	mt7620_txcsum_config((dev->features & NETIF_F_IP_CSUM));
	mt7620_rxcsum_config((dev->features & NETIF_F_RXCSUM));

	return 0;
}

static void mt7620_tx_dma(struct fe_priv *priv, int idx, struct sk_buff *skb)
{
	priv->tx_dma[idx].txd4 = 0;
}

static void mt7620_rx_dma(struct fe_priv *priv, int idx, int len)
{
	priv->rx_dma[idx].rxd2 = RX_DMA_PLEN0(len);
}

static void mt7620_init_data(struct fe_soc_data *data,
		struct net_device *netdev)
{
	struct fe_priv *priv = netdev_priv(netdev);

	priv->flags = FE_FLAG_PADDING_64B;
	netdev->hw_features = NETIF_F_IP_CSUM | NETIF_F_RXCSUM |
		NETIF_F_HW_VLAN_CTAG_TX;

	if (mt7620_get_eco() >= 5)
		netdev->hw_features |= NETIF_F_SG | NETIF_F_TSO | NETIF_F_TSO6 |
			NETIF_F_IPV6_CSUM;
}

static struct fe_soc_data mt7620_data = {
	.mac = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55 },
	.init_data = mt7620_init_data,
	.reset_fe = mt7620_fe_reset,
	.set_mac = mt7620_set_mac,
	.fwd_config = mt7620_fwd_config,
	.tx_dma = mt7620_tx_dma,
	.rx_dma = mt7620_rx_dma,
	.switch_init = mt7620_gsw_probe,
	.switch_config = mt7620_gsw_config,
	.port_init = mt7620_port_init,
	.reg_table = mt7620_reg_table,
	.pdma_glo_cfg = FE_PDMA_SIZE_16DWORDS | MT7620A_DMA_2B_OFFSET,
	.rx_dly_int = RT5350_RX_DLY_INT,
	.tx_dly_int = RT5350_TX_DLY_INT,
	.checksum_bit = MT7620_L4_VALID,
	.tx_udf_bit = MT7620_TX_DMA_UDF,
	.has_carrier = mt7620a_has_carrier,
	.mdio_read = mt7620_mdio_read,
	.mdio_write = mt7620_mdio_write,
	.mdio_adjust_link = mt7620_mdio_link_adjust,
};

const struct of_device_id of_fe_match[] = {
	{ .compatible = "ralink,mt7620a-eth", .data = &mt7620_data },
	{},
};

MODULE_DEVICE_TABLE(of, of_fe_match);
