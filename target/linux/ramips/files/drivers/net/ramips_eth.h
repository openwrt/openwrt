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
 *   based on Ralink SDK3.3
 *   Copyright (C) 2009 John Crispin <blogic@openwrt.org>
 */

#ifndef RAMIPS_ETH_H
#define RAMIPS_ETH_H

#include <linux/mii.h>
#include <linux/interrupt.h>
#include <linux/netdevice.h>

#define NUM_RX_DESC     256
#define NUM_TX_DESC     256

#define RAMIPS_DELAY_EN_INT		0x80
#define RAMIPS_DELAY_MAX_INT		0x04
#define RAMIPS_DELAY_MAX_TOUT		0x04
#define RAMIPS_DELAY_CHAN		(((RAMIPS_DELAY_EN_INT | RAMIPS_DELAY_MAX_INT) << 8) | RAMIPS_DELAY_MAX_TOUT)
#define RAMIPS_DELAY_INIT		((RAMIPS_DELAY_CHAN << 16) | RAMIPS_DELAY_CHAN)
#define RAMIPS_PSE_FQFC_CFG_INIT	0x80504000

/* interrupt bits */
#define RAMIPS_CNT_PPE_AF		BIT(31)
#define RAMIPS_CNT_GDM_AF		BIT(29)
#define RAMIPS_PSE_P2_FC		BIT(26)
#define RAMIPS_PSE_BUF_DROP		BIT(24)
#define RAMIPS_GDM_OTHER_DROP		BIT(23)
#define RAMIPS_PSE_P1_FC		BIT(22)
#define RAMIPS_PSE_P0_FC		BIT(21)
#define RAMIPS_PSE_FQ_EMPTY		BIT(20)
#define RAMIPS_GE1_STA_CHG		BIT(18)
#define RAMIPS_TX_COHERENT		BIT(17)
#define RAMIPS_RX_COHERENT		BIT(16)
#define RAMIPS_TX_DONE_INT3		BIT(11)
#define RAMIPS_TX_DONE_INT2		BIT(10)
#define RAMIPS_TX_DONE_INT1		BIT(9)
#define RAMIPS_TX_DONE_INT0		BIT(8)
#define RAMIPS_RX_DONE_INT0		BIT(2)
#define RAMIPS_TX_DLY_INT		BIT(1)
#define RAMIPS_RX_DLY_INT		BIT(0)

/* registers */
#define RAMIPS_FE_OFFSET		0x0000
#define RAMIPS_GDMA_OFFSET		0x0020
#define RAMIPS_PSE_OFFSET		0x0040
#define RAMIPS_GDMA2_OFFSET		0x0060
#define RAMIPS_CDMA_OFFSET		0x0080
#define RAMIPS_PDMA_OFFSET		0x0100
#define RAMIPS_PPE_OFFSET		0x0200
#define RAMIPS_CMTABLE_OFFSET		0x0400
#define RAMIPS_POLICYTABLE_OFFSET	0x1000

#define RAMIPS_MDIO_ACCESS		(RAMIPS_FE_OFFSET + 0x00)
#define RAMIPS_MDIO_CFG			(RAMIPS_FE_OFFSET + 0x04)
#define RAMIPS_FE_GLO_CFG		(RAMIPS_FE_OFFSET + 0x08)
#define RAMIPS_FE_RST_GL		(RAMIPS_FE_OFFSET + 0x0C)
#define RAMIPS_FE_INT_STATUS		(RAMIPS_FE_OFFSET + 0x10)
#define RAMIPS_FE_INT_ENABLE		(RAMIPS_FE_OFFSET + 0x14)
#define RAMIPS_MDIO_CFG2		(RAMIPS_FE_OFFSET + 0x18)
#define RAMIPS_FOC_TS_T			(RAMIPS_FE_OFFSET + 0x1C)

#define	RAMIPS_GDMA1_FWD_CFG		(RAMIPS_GDMA_OFFSET + 0x00)
#define RAMIPS_GDMA1_SCH_CFG		(RAMIPS_GDMA_OFFSET + 0x04)
#define RAMIPS_GDMA1_SHPR_CFG		(RAMIPS_GDMA_OFFSET + 0x08)
#define RAMIPS_GDMA1_MAC_ADRL		(RAMIPS_GDMA_OFFSET + 0x0C)
#define RAMIPS_GDMA1_MAC_ADRH		(RAMIPS_GDMA_OFFSET + 0x10)

#define	RAMIPS_GDMA2_FWD_CFG		(RAMIPS_GDMA2_OFFSET + 0x00)
#define RAMIPS_GDMA2_SCH_CFG		(RAMIPS_GDMA2_OFFSET + 0x04)
#define RAMIPS_GDMA2_SHPR_CFG		(RAMIPS_GDMA2_OFFSET + 0x08)
#define RAMIPS_GDMA2_MAC_ADRL		(RAMIPS_GDMA2_OFFSET + 0x0C)
#define RAMIPS_GDMA2_MAC_ADRH		(RAMIPS_GDMA2_OFFSET + 0x10)

#define RAMIPS_PSE_FQ_CFG		(RAMIPS_PSE_OFFSET + 0x00)
#define RAMIPS_CDMA_FC_CFG		(RAMIPS_PSE_OFFSET + 0x04)
#define RAMIPS_GDMA1_FC_CFG		(RAMIPS_PSE_OFFSET + 0x08)
#define RAMIPS_GDMA2_FC_CFG		(RAMIPS_PSE_OFFSET + 0x0C)

#define RAMIPS_CDMA_CSG_CFG		(RAMIPS_CDMA_OFFSET + 0x00)
#define RAMIPS_CDMA_SCH_CFG		(RAMIPS_CDMA_OFFSET + 0x04)

#define RAMIPS_PDMA_GLO_CFG		(RAMIPS_PDMA_OFFSET + 0x00)
#define RAMIPS_PDMA_RST_CFG		(RAMIPS_PDMA_OFFSET + 0x04)
#define RAMIPS_PDMA_SCH_CFG		(RAMIPS_PDMA_OFFSET + 0x08)
#define RAMIPS_DLY_INT_CFG		(RAMIPS_PDMA_OFFSET + 0x0C)
#define RAMIPS_TX_BASE_PTR0		(RAMIPS_PDMA_OFFSET + 0x10)
#define RAMIPS_TX_MAX_CNT0		(RAMIPS_PDMA_OFFSET + 0x14)
#define RAMIPS_TX_CTX_IDX0		(RAMIPS_PDMA_OFFSET + 0x18)
#define RAMIPS_TX_DTX_IDX0		(RAMIPS_PDMA_OFFSET + 0x1C)
#define RAMIPS_TX_BASE_PTR1		(RAMIPS_PDMA_OFFSET + 0x20)
#define RAMIPS_TX_MAX_CNT1		(RAMIPS_PDMA_OFFSET + 0x24)
#define RAMIPS_TX_CTX_IDX1		(RAMIPS_PDMA_OFFSET + 0x28)
#define RAMIPS_TX_DTX_IDX1		(RAMIPS_PDMA_OFFSET + 0x2C)
#define RAMIPS_RX_BASE_PTR0		(RAMIPS_PDMA_OFFSET + 0x30)
#define RAMIPS_RX_MAX_CNT0		(RAMIPS_PDMA_OFFSET + 0x34)
#define RAMIPS_RX_CALC_IDX0		(RAMIPS_PDMA_OFFSET + 0x38)
#define RAMIPS_RX_DRX_IDX0		(RAMIPS_PDMA_OFFSET + 0x3C)
#define RAMIPS_TX_BASE_PTR2		(RAMIPS_PDMA_OFFSET + 0x40)
#define RAMIPS_TX_MAX_CNT2		(RAMIPS_PDMA_OFFSET + 0x44)
#define RAMIPS_TX_CTX_IDX2		(RAMIPS_PDMA_OFFSET + 0x48)
#define RAMIPS_TX_DTX_IDX2		(RAMIPS_PDMA_OFFSET + 0x4C)
#define RAMIPS_TX_BASE_PTR3		(RAMIPS_PDMA_OFFSET + 0x50)
#define RAMIPS_TX_MAX_CNT3		(RAMIPS_PDMA_OFFSET + 0x54)
#define RAMIPS_TX_CTX_IDX3		(RAMIPS_PDMA_OFFSET + 0x58)
#define RAMIPS_TX_DTX_IDX3		(RAMIPS_PDMA_OFFSET + 0x5C)
#define RAMIPS_RX_BASE_PTR1		(RAMIPS_PDMA_OFFSET + 0x60)
#define RAMIPS_RX_MAX_CNT1		(RAMIPS_PDMA_OFFSET + 0x64)
#define RAMIPS_RX_CALC_IDX1		(RAMIPS_PDMA_OFFSET + 0x68)
#define RAMIPS_RX_DRX_IDX1		(RAMIPS_PDMA_OFFSET + 0x6C)

/* uni-cast port */
#define RAMIPS_GDM1_ICS_EN		BIT(22)
#define RAMIPS_GDM1_TCS_EN		BIT(21)
#define RAMIPS_GDM1_UCS_EN		BIT(20)
#define RAMIPS_GDM1_JMB_EN		BIT(19)
#define RAMIPS_GDM1_STRPCRC		BIT(16)
#define RAMIPS_GDM1_UFRC_P_CPU		(0 << 12)
#define RAMIPS_GDM1_UFRC_P_GDMA1	(1 << 12)
#define RAMIPS_GDM1_UFRC_P_PPE		(6 << 12)

/* checksums */
#define RAMIPS_ICS_GEN_EN		BIT(2)
#define RAMIPS_UCS_GEN_EN		BIT(1)
#define RAMIPS_TCS_GEN_EN		BIT(0)

/* dma ring */
#define RAMIPS_PST_DRX_IDX0		BIT(16)
#define RAMIPS_PST_DTX_IDX3		BIT(3)
#define RAMIPS_PST_DTX_IDX2		BIT(2)
#define RAMIPS_PST_DTX_IDX1		BIT(1)
#define RAMIPS_PST_DTX_IDX0		BIT(0)

#define RAMIPS_TX_WB_DDONE		BIT(6)
#define RAMIPS_RX_DMA_BUSY		BIT(3)
#define RAMIPS_TX_DMA_BUSY		BIT(1)
#define RAMIPS_RX_DMA_EN		BIT(2)
#define RAMIPS_TX_DMA_EN		BIT(0)

#define RAMIPS_PDMA_SIZE_4DWORDS	(0 << 4)
#define RAMIPS_PDMA_SIZE_8DWORDS	(1 << 4)
#define RAMIPS_PDMA_SIZE_16DWORDS	(2 << 4)

#define RAMIPS_US_CYC_CNT_MASK		0xff
#define RAMIPS_US_CYC_CNT_SHIFT		0x8
#define RAMIPS_US_CYC_CNT_DIVISOR	1000000

#define RX_DMA_PLEN0(x)			((x >> 16) & 0x3fff)
#define RX_DMA_LSO			BIT(30)
#define RX_DMA_DONE			BIT(31)

struct ramips_rx_dma {
	unsigned int rxd1;
	unsigned int rxd2;
	unsigned int rxd3;
	unsigned int rxd4;
};

#define TX_DMA_PLEN0_MASK		((0x3fff) << 16)
#define TX_DMA_PLEN0(x)			((x & 0x3fff) << 16)
#define TX_DMA_LSO			BIT(30)
#define TX_DMA_DONE			BIT(31)
#define TX_DMA_QN(x)			(x << 16)
#define TX_DMA_PN(x)			(x << 24)
#define TX_DMA_QN_MASK			TX_DMA_QN(0x7)
#define TX_DMA_PN_MASK			TX_DMA_PN(0x7)

struct ramips_tx_dma {
	unsigned int txd1;
	unsigned int txd2;
	unsigned int txd3;
	unsigned int txd4;
};

struct raeth_priv
{
	unsigned int		phy_rx;
	struct tasklet_struct	rx_tasklet;
	struct ramips_rx_dma	*rx;
	struct sk_buff		*rx_skb[NUM_RX_DESC];

	unsigned int		phy_tx;
	struct tasklet_struct	tx_housekeeping_tasklet;
	struct ramips_tx_dma	*tx;
	struct sk_buff		*tx_skb[NUM_RX_DESC];

	unsigned int		skb_free_idx;

	spinlock_t		page_lock;
	struct ramips_eth_platform_data *plat;
};

#endif /* RAMIPS_ETH_H */
