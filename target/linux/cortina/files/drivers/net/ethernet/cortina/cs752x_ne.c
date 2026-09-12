// SPDX-License-Identifier: GPL-2.0
#include <linux/bitfield.h>
#include <linux/bits.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/etherdevice.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/iopoll.h>
#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/of.h>
#include <linux/of_net.h>
#include <linux/phylink.h>
#include <linux/platform_device.h>
#include <linux/skbuff.h>
#include <linux/spinlock.h>
#include <linux/timer.h>

#include "cs752x_ne.h"

#define CS_NE_PORTS			3
#define CS_NE_RX_INSTANCES		9
#define CS_NE_TX_INSTANCES		2
#define CS_NE_VOQS			112
#define CS_NE_CPU_VOQ_BASE		48
#define CS_NE_MAX_PKT_LEN		9022
#define CS_NE_MIN_PKT_LEN		64
#define CS_NE_MAX_CPU_BUFFERS		4096
#define CS_NE_BUF_DATA_MAX		3584
#define CS_NE_BUF_HDR_OFF		0x100
#define CS_NE_TX_DESC_ORDER		10
#define CS_NE_TX_DESC_NUM		BIT(CS_NE_TX_DESC_ORDER)
#define CS_NE_TX_MIN_LEN		24
#define CS_NE_TX_TIMER_MS		10
#define CS_NE_ACCESS_TIMEOUT_US		10000
#define CS_NE_HDRA_CPU_PKT		0xc30001

#define GLOBAL_NETWORK_ENGINE_INTENABLE_0	0x074
#define GLOBAL_IO_DRIVE_STRENGTH		0x0b4
#define   GMAC_DS_MASK				GENMASK(1, 0)
#define GLOBAL_SOFTWARE2			0x0bc

#define NI_INTF_RST_CONFIG		0x000
#define   NI_INTF_RST_GE_ALL		GENMASK(2, 0)
#define NI_ETH_INT_CONFIG1		0x004
#define   NI_INT_CFG_GE_MASK(p)		(GENMASK(2, 0) << ((p) * 8))
#define   NI_INT_CFG_GE(p, v)		((v) << ((p) * 8))
#define   NI_INT_CFG_RGMII_1000		2
#define   NI_INT_CFG_RGMII_100		3
#define   NI_PHY_MODE_GE(p)		BIT(4 + (p) * 8)
#define   NI_RMII_CLKSRC_GE(p)		BIT(5 + (p) * 8)
#define   NI_TX_USE_GEFIFO_GE(p)	BIT(6 + (p) * 8)
#define   NI_INV_CLK_OUT_GE(p)		BIT(7 + (p) * 8)
#define NI_ETH_INT_CONFIG2		0x008
#define   NI_INT_CONFIG2_GE_MASK(p)	(0xff << ((p) * 8))
#define   NI_TX_INTF_LP_TIME_GE(p)	BIT(5 + (p) * 8)
#define NI_ETH_MGMT_PT_CONFIG		0x00c
#define NI_ETH_MAC_CONFIG0(p)		(0x010 + (p) * 12)
#define   NI_MAC_SPEED_10		BIT(0)
#define   NI_MAC_HALF_DUPLEX		BIT(1)
#define   NI_MAC_RX_EN			BIT(2)
#define   NI_MAC_TX_EN			BIT(3)
#define   NI_MAC_RX_FLOW_DISABLE	BIT(8)
#define   NI_MAC_TX_FLOW_DISABLE	BIT(12)
#define   NI_MAC_RX_RST			BIT(30)
#define   NI_MAC_TX_RST			BIT(31)
#define   NI_MAC_CONFIG0_INIT		0xc0100800
#define NI_ETH_MAC_CONFIG2(p)		(0x018 + (p) * 12)
#define NI_MAC_ADDR0			0x040
#define NI_MAC_ADDR1			0x044
#define NI_PKT_LEN_CONFIG		0x050
#define   NI_MAX_PKT_SIZE		GENMASK(13, 0)
#define   NI_MIN_PKT_SIZE		GENMASK(25, 16)
#define NI_RX_CNTRL_CONFIG0(i)		(0x058 + (i) * 8)
#define   NI_RUNT_DROP_DIS		BIT(6)
#define NI_RX_CNTRL_CONFIG1(i)		(0x05c + (i) * 8)
#define NI_RX_AGG_CONFIG		0x098
#define   NI_RX_PORT_CAL_DIS		BIT(0)
#define NI_RX_PORT_CAL_ACCESS		0x0ac
#define NI_RX_PORT_CAL_DATA		0x0b0
#define NI_SCH_BP_THLD_ETH		0x0c0
#define   NI_SCH_BP_THLD_ETH_ASIC	0x082a00f8
#define NI_MISC_CONFIG			0x0c8
#define   NI_MC_ACCEPT_ALL		BIT(14)
#define   NI_RXMIB_MODE			BIT(16)
#define   NI_TXMIB_MODE			BIT(17)
#define NI_TX_VOQ_LKUP_ACCESS		0x0cc
#define NI_TX_VOQ_LKUP_DATA1		0x0d0
#define NI_TX_VOQ_LKUP_DATA0		0x0d4
#define   NI_TXEM_DISCRC		BIT(4)
#define NI_CPUXRAM_ADRCFG_RX(i)		(0x1ac + (i) * 4)
#define NI_CPUXRAM_ADRCFG_TX(i)		(0x1d0 + (i) * 4)
#define NI_CPUXRAM_CFG			0x1d8
#define   NI_CPUXRAM_PKT_DIS_MASK	GENMASK(10, 0)
#define   NI_CPUXRAM_PTR_TOGGLE		GENMASK(9, 0)
#define NI_CPUXRAM_SCH_BP_CFG(i)	(0x1e4 + (i) * 4)
#define   NI_XRAM_SCH_RDY_MODE		BIT(15)
#define   NI_XRAM_SCH_RDY_FREE_THLD	30
#define NI_CPUXRAM_CPU_CFG_RX(i)	(0x204 + (i) * 24)
#define NI_CPUXRAM_CPU_STA_RX(i)	(0x208 + (i) * 24)
#define NI_CPUXRAM_INT_COLSC_CFG(i)	(0x20c + (i) * 24)
#define   NI_INT_COLSC_PKT		GENMASK(29, 20)
#define   NI_INT_COLSC_FIRST_EN		BIT(30)
#define NI_CPUXRAM_RXPKT_INTERRUPT(i)	(0x3a0 + (i) * 8)
#define NI_CPUXRAM_RXPKT_INTENABLE(i)	(0x3a4 + (i) * 8)
#define   NI_XRAM_PTR_MASK		GENMASK(10, 0)

#define NE_ACCESS			BIT(31)
#define NE_ACCESS_WRITE			BIT(30)

#define QM_CONFIG_0			0x000
#define   QM_INIT			BIT(0)
#define   QM_EGRESS_FC_INT_THRESHOLD	GENMASK(5, 4)
#define   QM_MTU			GENMASK(15, 8)
#define   QM_MAX_PKT_LEN		GENMASK(29, 16)
#define   QM_CPU_FULL_ACCESS		BIT(31)
#define QM_CONFIG_1			0x004
#define   QM_MEM_CONFIG			GENMASK(1, 0)
#define   QM_CPU_BANKS			GENMASK(6, 4)
#define   QM_PAGING_SIZE		BIT(9)
#define QM_STATUS_0			0x014
#define   QM_INIT_DONE			BIT(0)
#define QM_CPU_PATH_CONFIG_0		0x038
#define   QM_CPU_BUFFERS		GENMASK(12, 0)
#define   QM_MAX_CPU_PKT_LEN		GENMASK(29, 16)
#define QM_CPU_PATH_CONFIG_1		0x03c
#define   QM_LINUX_MODE			BIT(0)
#define   QM_LINUX_2BYTE_ALIGN		BIT(2)
#define QM_CPU_PATH_VOQ_MAP(i)		(0x040 + (i) * 4)
#define QM_CPU_PATH_LINUX_SDRAM_ADDR(i)	(0x060 + (i) * 4)
#define QM_CPU_PATH_STATUS_0		0x080
#define   QM_SDRAM_ADDR_FIFO_WR_CNT	GENMASK(4, 0)
#define QM_QUE_PROFILE_MEM_ACCESS	0x1c0
#define QM_QUE_PROFILE_MEM_DATA		0x1c4
#define   QM_PROFILE_PACKET_PORT	BIT(1)
#define   QM_PROFILE_DS_SPEED		GENMASK(10, 2)
#define   QM_PROFILE_PRIORITY		GENMASK(13, 11)
#define QM_SPARE			0x1f8

#define DMA_LSO_RXDMA_CONTROL		0x000
#define DMA_LSO_TXDMA_CONTROL		0x004
#define   DMA_ENABLE			BIT(0)
#define   DMA_BURST_LEN			GENMASK(3, 2)
#define DMA_LSO_TXQ_CONTROL(q)		(0x008 + (q) * 4)
#define DMA_LSO_TXQ_BASE_DEPTH(q)	(0x0c8 + (q) * 4)
#define DMA_LSO_TXQ_WPTR(q)		(0x0e8 + (q) * 8)
#define DMA_LSO_TXQ_RPTR(q)		(0x0ec + (q) * 8)
#define   DMA_PTR_MASK			GENMASK(12, 0)
#define DMA_LSO_INTERRUPT_0		0x1d0
#define DMA_LSO_INTENABLE_0		0x1d4
#define DMA_LSO_TXQ_INTERRUPT(q)	(0x228 + (q) * 8)
#define DMA_LSO_TXQ_INTENABLE(q)	(0x22c + (q) * 8)
#define DMA_AXI_CONFIG			0x300
#define   DMA_AXI_WRITE_ARB_CH0_2	GENMASK(15, 13)
#define   DMA_AXI_READ_ARB_CH0_2	GENMASK(23, 21)
#define   DMA_AXI_WRITE_OUTTRANS	GENMASK(25, 24)
#define DMA_AXI_READ_DRR_0_3		0x30c
#define DMA_AXI_READ_DRR_4_7		0x310
#define DMA_AXI_WRITE_DRR_0_3		0x314
#define DMA_AXI_WRITE_DRR_4_7		0x318
#define DMA_AXI_READ_TIMEOUT		0x320
#define DMA_AXI_WRITE_TIMEOUT		0x324

#define FETOP_FE_PRSR_CFG_0		0x03c
#define   FE_L4_CHKSUM_CHK_DISABLE	BIT(8)

#define TX_DESC_OWN			BIT(31)
#define TX_DESC_SOF			BIT(28)
#define TX_DESC_EOF			BIT(27)
#define TX_DESC_LEN			GENMASK(15, 0)
#define TX_LSO_SEGMENT_EN		BIT(16)
#define TX_LSO_IP_LENFIX_EN		BIT(22)
#define TX_HDRA_DVOQ			GENMASK(10, 3)

#define XR_NEXT_LINK			GENMASK(11, 0)
#define XR_ERROR			BIT(29)
#define XR_OWNERSHIP			BIT(31)
#define HDRE_PKT_SIZE			GENMASK(13, 0)
#define HDRE_CPU_HEADER_HI		BIT(4)
#define XRAM_BUF_MASK			0xfffff000

static const u16 cs_ne_xram_rx_units[CS_NE_RX_INSTANCES] = {
	525, 525, 525, 7, 7, 7, 91, 91, 56
};
static const u16 cs_ne_xram_tx_units[CS_NE_TX_INSTANCES] = { 214, 0 };
static const u16 cs_ne_rx_pool_size[CS_NE_PORTS] = { 1024, 1024, 256 };

struct cs_ne;

struct cs_ne_tx_buf {
	struct sk_buff *skb;
	dma_addr_t dma;
	u16 len;
};

struct cs_ne_txq {
	u32 *desc;
	dma_addr_t desc_dma;
	struct cs_ne_tx_buf bufs[CS_NE_TX_DESC_NUM];
	u16 head;
	u16 tail;
	spinlock_t lock;
	struct timer_list timer;
	struct cs_ne *ne;
};

struct cs_ne_port {
	struct cs_ne *ne;
	struct net_device *ndev;
	struct device_node *np;
	struct napi_struct napi;
	struct phylink *phylink;
	struct phylink_config phylink_config;
	phy_interface_t interface;
	int id;
	int irq;
	u32 rd;
	bool opened;
};

struct cs_ne {
	struct device *dev;
	void __iomem *global;
	void __iomem *ni;
	void __iomem *dma;
	void __iomem *fe;
	void __iomem *qm;
	void __iomem *tm;
	void __iomem *sch;
	void __iomem *xram;
	struct cs_ne_port *ports[CS_NE_PORTS];
	struct cs_ne_txq txq;
	bool internal_buf;
};

static bool internal_buf;
module_param(internal_buf, bool, 0444);
static int napi_budget = 16;
module_param(napi_budget, int, 0444);

static inline u32 ni_rd(struct cs_ne *ne, u32 off)
{
	return readl(ne->ni + off);
}

static inline void ni_wr(struct cs_ne *ne, u32 off, u32 val)
{
	writel(val, ne->ni + off);
}

static inline void ni_rmw(struct cs_ne *ne, u32 off, u32 mask, u32 val)
{
	writel((readl(ne->ni + off) & ~mask) | (val & mask), ne->ni + off);
}

static inline u32 qm_rd(struct cs_ne *ne, u32 off)
{
	return readl(ne->qm + off);
}

static inline void qm_wr(struct cs_ne *ne, u32 off, u32 val)
{
	writel(val, ne->qm + off);
}

static int cs_ne_wait_access(void __iomem *reg)
{
	u32 val;

	return readl_poll_timeout_atomic(reg, val, !(val & NE_ACCESS), 1,
					 CS_NE_ACCESS_TIMEOUT_US);
}

static int cs_ne_set_port_calendar(struct cs_ne *ne, u16 slot, u8 pspid)
{
	ni_wr(ne, NI_RX_PORT_CAL_DATA, pspid & 0x7);
	ni_wr(ne, NI_RX_PORT_CAL_ACCESS,
	      NE_ACCESS | NE_ACCESS_WRITE | (slot & 0x7f));
	return cs_ne_wait_access(ne->ni + NI_RX_PORT_CAL_ACCESS);
}

static int cs_ne_set_voq_map(struct cs_ne *ne, u8 voq, u8 did)
{
	ni_wr(ne, NI_TX_VOQ_LKUP_DATA0, (did & 0xf) | NI_TXEM_DISCRC);
	ni_wr(ne, NI_TX_VOQ_LKUP_DATA1, 0);
	ni_wr(ne, NI_TX_VOQ_LKUP_ACCESS,
	      NE_ACCESS | NE_ACCESS_WRITE | (voq & 0x7f));
	return cs_ne_wait_access(ne->ni + NI_TX_VOQ_LKUP_ACCESS);
}

static void cs_ne_voq_map_init(struct cs_ne *ne)
{
	int voq, did;

	for (voq = 0; voq < CS_NE_VOQS; voq++) {
		if (voq < 24)
			did = voq / 8;
		else if (voq < 32)
			did = 4;
		else if (voq < 40)
			did = 5;
		else if (voq < 48)
			did = 6;
		else
			did = 8 + (voq - 48) / 8;
		if (cs_ne_set_voq_map(ne, voq, did))
			dev_warn(ne->dev, "voq map %d timeout\n", voq);
	}
}

static void cs_ne_port_calendar_init(struct cs_ne *ne)
{
	int i, j;

	for (i = 0; i < 12; i++)
		for (j = 0; j < 8; j++)
			cs_ne_set_port_calendar(ne, i * 8 + j, j);
	for (i = 0; i < 12; i++)
		cs_ne_set_port_calendar(ne, i * 8 + 2, 4);
}

static void cs_ne_xram_init(struct cs_ne *ne)
{
	u32 next = 0, val;
	int i;

	for (i = 0; i < CS_NE_RX_INSTANCES; i++)
		ni_wr(ne, NI_CPUXRAM_ADRCFG_RX(i), 0);
	for (i = 0; i < CS_NE_TX_INSTANCES; i++)
		ni_wr(ne, NI_CPUXRAM_ADRCFG_TX(i), 0);

	for (i = 0; i < CS_NE_RX_INSTANCES; i++) {
		val = 0;
		if (cs_ne_xram_rx_units[i]) {
			val = next | ((next + cs_ne_xram_rx_units[i] - 1) << 16);
			next += cs_ne_xram_rx_units[i];
		}
		ni_wr(ne, NI_CPUXRAM_ADRCFG_RX(i), val);
	}
	for (i = 0; i < CS_NE_TX_INSTANCES; i++) {
		val = 0;
		if (cs_ne_xram_tx_units[i]) {
			val = next | ((next + cs_ne_xram_tx_units[i] - 1) << 16);
			next += cs_ne_xram_tx_units[i];
		}
		ni_wr(ne, NI_CPUXRAM_ADRCFG_TX(i), val);
	}

	ni_rmw(ne, NI_CPUXRAM_CFG, NI_CPUXRAM_PKT_DIS_MASK, 0);
	ni_rmw(ne, NI_CPUXRAM_CFG, NI_CPUXRAM_PTR_TOGGLE, NI_CPUXRAM_PTR_TOGGLE);
	ni_rmw(ne, NI_CPUXRAM_CFG, NI_CPUXRAM_PTR_TOGGLE, 0);
}

static void cs_ne_qm_init_cfg(struct cs_ne *ne)
{
	u32 mem_mb = readl(ne->global + GLOBAL_SOFTWARE2) >> 20;
	u32 mem_cfg, val;

	switch (mem_mb) {
	case 128:
		mem_cfg = 0;
		break;
	case 256:
		mem_cfg = 1;
		break;
	case 1024:
		mem_cfg = 3;
		break;
	default:
		mem_cfg = 2;
		break;
	}
	dev_dbg(ne->dev, "QM: DRAM %u MB, %s buffers\n", mem_mb,
		ne->internal_buf ? "internal" : "external");

	if (ne->internal_buf)
		writel(readl(ne->qm + 0x0f8) | BIT(1), ne->qm + 0x0f8);

	val = FIELD_PREP(QM_MAX_PKT_LEN, CS_NE_MAX_PKT_LEN) |
	      FIELD_PREP(QM_MTU, (ETH_DATA_LEN + ETH_HLEN) & 0xff) |
	      FIELD_PREP(QM_EGRESS_FC_INT_THRESHOLD, 1);
	qm_wr(ne, QM_CONFIG_0, val);

	val = qm_rd(ne, QM_CONFIG_1);
	val &= ~(QM_MEM_CONFIG | QM_CPU_BANKS | QM_PAGING_SIZE);
	val |= FIELD_PREP(QM_MEM_CONFIG, mem_cfg) |
	       FIELD_PREP(QM_CPU_BANKS, 7) | QM_PAGING_SIZE;
	qm_wr(ne, QM_CONFIG_1, val);
	qm_wr(ne, QM_SPARE, 0);
}

static void cs_ne_qm_cpu_path_init(struct cs_ne *ne)
{
	int i;

	qm_wr(ne, QM_CPU_PATH_CONFIG_0,
	      FIELD_PREP(QM_CPU_BUFFERS, CS_NE_MAX_CPU_BUFFERS) |
	      FIELD_PREP(QM_MAX_CPU_PKT_LEN, CS_NE_MAX_PKT_LEN + 16));
	qm_wr(ne, QM_CPU_PATH_CONFIG_1, QM_LINUX_MODE | QM_LINUX_2BYTE_ALIGN);
	for (i = 0; i < 8; i++)
		qm_wr(ne, QM_CPU_PATH_VOQ_MAP(i),
		      i | (i << 3) | (i << 6) | (i << 9) | (i << 12) |
		      (i << 15) | (i << 18) | (i << 21));
}

static int cs_ne_qm_set_profile(struct cs_ne *ne, int idx, u32 data)
{
	int ret;

	qm_wr(ne, QM_CONFIG_0, qm_rd(ne, QM_CONFIG_0) | QM_CPU_FULL_ACCESS);
	qm_wr(ne, QM_QUE_PROFILE_MEM_DATA, data);
	qm_wr(ne, QM_QUE_PROFILE_MEM_ACCESS,
	      NE_ACCESS | NE_ACCESS_WRITE | (idx & 0x7f));
	ret = cs_ne_wait_access(ne->qm + QM_QUE_PROFILE_MEM_ACCESS);
	qm_wr(ne, QM_CONFIG_0, qm_rd(ne, QM_CONFIG_0) & ~QM_CPU_FULL_ACCESS);
	return ret;
}

static int cs_ne_qm_init(struct cs_ne *ne)
{
	u32 val, data;
	int ret, i;

	qm_wr(ne, QM_CONFIG_0, qm_rd(ne, QM_CONFIG_0) | QM_INIT);
	ret = readl_poll_timeout(ne->qm + QM_STATUS_0, val, val & QM_INIT_DONE,
				 10, 100000);
	qm_wr(ne, QM_CONFIG_0, qm_rd(ne, QM_CONFIG_0) & ~QM_INIT);
	if (ret) {
		dev_err(ne->dev, "QM init timeout, status 0x%08x\n", val);
		return ret;
	}

	for (i = 0; i < CS_NE_VOQS; i++) {
		data = FIELD_PREP(QM_PROFILE_DS_SPEED, 500);
		if (i < 24)
			data |= QM_PROFILE_PACKET_PORT |
				FIELD_PREP(QM_PROFILE_PRIORITY, 3);
		else if (i < 40)
			data |= FIELD_PREP(QM_PROFILE_PRIORITY, 1);
		else if (i < 48)
			data |= FIELD_PREP(QM_PROFILE_PRIORITY, 4);
		if (cs_ne_qm_set_profile(ne, i, data))
			dev_warn(ne->dev, "QM profile %d timeout\n", i);
	}
	return 0;
}

static int cs_ne_dma_init(struct cs_ne *ne)
{
	struct cs_ne_txq *txq = &ne->txq;
	u32 val;

	txq->desc = dmam_alloc_coherent(ne->dev, CS_NE_TX_DESC_NUM * 32,
					&txq->desc_dma, GFP_KERNEL);
	if (!txq->desc)
		return -ENOMEM;
	txq->ne = ne;
	spin_lock_init(&txq->lock);

	writel(FIELD_PREP(DMA_BURST_LEN, 3), ne->dma + DMA_LSO_RXDMA_CONTROL);
	writel(DMA_ENABLE | FIELD_PREP(DMA_BURST_LEN, 3),
	       ne->dma + DMA_LSO_TXDMA_CONTROL);
	writel((txq->desc_dma & ~0xf) | CS_NE_TX_DESC_ORDER,
	       ne->dma + DMA_LSO_TXQ_BASE_DEPTH(0));
	writel(0, ne->dma + DMA_LSO_TXQ_WPTR(0));
	writel(1, ne->dma + DMA_LSO_TXQ_CONTROL(0));

	writel(0xfffc0000, ne->dma + DMA_AXI_READ_TIMEOUT);
	writel(0xfffc0000, ne->dma + DMA_AXI_WRITE_TIMEOUT);
	val = readl(ne->dma + DMA_AXI_CONFIG);
	val |= DMA_AXI_WRITE_ARB_CH0_2 | DMA_AXI_READ_ARB_CH0_2 |
	       DMA_AXI_WRITE_OUTTRANS;
	writel(val, ne->dma + DMA_AXI_CONFIG);
	writel(0xffffffff, ne->dma + DMA_AXI_READ_DRR_0_3);
	writel(0x0000ffff, ne->dma + DMA_AXI_READ_DRR_4_7);
	writel(0xffffffff, ne->dma + DMA_AXI_WRITE_DRR_0_3);
	writel(0x0000ffff, ne->dma + DMA_AXI_WRITE_DRR_4_7);
	writel(0, ne->dma + DMA_LSO_INTENABLE_0);
	writel(0, ne->dma + DMA_LSO_TXQ_INTENABLE(0));
	return 0;
}

static void cs_ne_ni_init(struct cs_ne *ne)
{
	u32 val;
	int i;

	val = ni_rd(ne, NI_INTF_RST_CONFIG);
	ni_wr(ne, NI_INTF_RST_CONFIG, val | NI_INTF_RST_GE_ALL);
	ni_wr(ne, NI_INTF_RST_CONFIG, val & ~NI_INTF_RST_GE_ALL);

	ni_wr(ne, NI_SCH_BP_THLD_ETH, NI_SCH_BP_THLD_ETH_ASIC);
	for (i = 0; i < CS_NE_PORTS; i++)
		ni_wr(ne, NI_ETH_MAC_CONFIG0(i), NI_MAC_CONFIG0_INIT);

	val = readl(ne->global + GLOBAL_IO_DRIVE_STRENGTH);
	writel(val | GMAC_DS_MASK, ne->global + GLOBAL_IO_DRIVE_STRENGTH);
	writel(0, ne->global + GLOBAL_NETWORK_ENGINE_INTENABLE_0);

	ni_rmw(ne, NI_RX_AGG_CONFIG, NI_RX_PORT_CAL_DIS, 0);
	cs_ne_port_calendar_init(ne);
	cs_ne_qm_init_cfg(ne);
	cs_ne_xram_init(ne);
	cs_ne_qm_cpu_path_init(ne);

	for (i = 0; i < 8; i++)
		ni_wr(ne, NI_RX_CNTRL_CONFIG1(i), 0);
	for (i = 3; i < 6; i++)
		ni_rmw(ne, NI_RX_CNTRL_CONFIG0(i), NI_RUNT_DROP_DIS,
		       NI_RUNT_DROP_DIS);
	for (i = 0; i < 8; i++)
		ni_wr(ne, NI_CPUXRAM_SCH_BP_CFG(i),
		      NI_XRAM_SCH_RDY_MODE | NI_XRAM_SCH_RDY_FREE_THLD);

	ni_rmw(ne, NI_PKT_LEN_CONFIG, NI_MAX_PKT_SIZE | NI_MIN_PKT_SIZE,
	       FIELD_PREP(NI_MAX_PKT_SIZE, CS_NE_MAX_PKT_LEN) |
	       FIELD_PREP(NI_MIN_PKT_SIZE, CS_NE_MIN_PKT_LEN));
	ni_rmw(ne, NI_ETH_MGMT_PT_CONFIG, GENMASK(2, 0), 3);
	ni_rmw(ne, NI_MISC_CONFIG,
	       NI_RXMIB_MODE | NI_TXMIB_MODE | NI_MC_ACCEPT_ALL,
	       NI_RXMIB_MODE | NI_TXMIB_MODE | NI_MC_ACCEPT_ALL);
	cs_ne_voq_map_init(ne);

	for (i = 0; i < CS_NE_PORTS; i++) {
		ni_wr(ne, NI_CPUXRAM_INT_COLSC_CFG(i),
		      NI_INT_COLSC_FIRST_EN | FIELD_PREP(NI_INT_COLSC_PKT, 4));
		ni_wr(ne, NI_CPUXRAM_RXPKT_INTENABLE(i), 0);
		ni_wr(ne, NI_CPUXRAM_RXPKT_INTERRUPT(i), 1);
	}
}

static void cs_ne_qm_push_buffer(struct cs_ne *ne, int inst, dma_addr_t dma)
{
	u32 val;

	readl_poll_timeout_atomic(ne->qm + QM_CPU_PATH_STATUS_0, val,
				  FIELD_GET(QM_SDRAM_ADDR_FIFO_WR_CNT, val) < 28,
				  1, 1000);
	qm_wr(ne, QM_CPU_PATH_LINUX_SDRAM_ADDR(inst), dma);
}

static int cs_ne_rx_refill(struct cs_ne_port *port, int count)
{
	struct cs_ne *ne = port->ne;
	struct page *page;
	dma_addr_t dma;
	int i;

	for (i = 0; i < count; i++) {
		page = dev_alloc_page();
		if (!page)
			break;
		dma = dma_map_page(ne->dev, page, 0, PAGE_SIZE, DMA_FROM_DEVICE);
		if (dma_mapping_error(ne->dev, dma)) {
			__free_page(page);
			break;
		}
		cs_ne_qm_push_buffer(ne, port->id, dma);
	}
	return i;
}

static void cs_ne_rx_recycle(struct cs_ne_port *port, dma_addr_t dma)
{
	struct cs_ne *ne = port->ne;

	dma_sync_single_for_device(ne->dev, dma, PAGE_SIZE, DMA_FROM_DEVICE);
	cs_ne_qm_push_buffer(ne, port->id, dma);
}

static void *cs_ne_rx_buf_cpu(struct cs_ne *ne, dma_addr_t dma, u32 off,
			      u32 len)
{
	struct page *page = pfn_to_page(PFN_DOWN(dma));

	dma_sync_single_for_cpu(ne->dev, dma + off, len, DMA_FROM_DEVICE);
	return page_address(page) + off;
}

static int cs_ne_rx_poll(struct napi_struct *napi, int budget)
{
	struct cs_ne_port *port = container_of(napi, struct cs_ne_port, napi);
	struct cs_ne *ne = port->ne;
	struct net_device *ndev;
	void __iomem *desc;
	struct sk_buff *skb;
	u32 wr, rd, next, w1, hdre_hi, hdre_lo, pkt_size;
	u32 off, total, seg, nbufs;
	dma_addr_t bufs[4];
	int done = 0, i;
	bool cpu_hdr;

	wr = ni_rd(ne, NI_CPUXRAM_CPU_STA_RX(port->id)) & NI_XRAM_PTR_MASK;
	rd = port->rd;

	while (rd != wr && done < budget) {
		desc = ne->xram + rd * 8;
		w1 = readl(desc + 4);
		if (w1 & XR_OWNERSHIP) {
			netdev_warn_once(port->ndev, "xram entry %u owned by hw\n", rd);
			port->ndev->stats.rx_fifo_errors++;
			break;
		}
		next = FIELD_GET(XR_NEXT_LINK, w1);
		if (next == wr &&
		    (ni_rd(ne, NI_CPUXRAM_CPU_STA_RX(port->id)) & NI_XRAM_PTR_MASK) == wr)
			udelay(16);
		hdre_hi = readl(desc + 8);
		hdre_lo = readl(desc + 12);
		bufs[0] = readl(desc + 40) & XRAM_BUF_MASK;
		rd = next;

		cpu_hdr = hdre_hi & HDRE_CPU_HEADER_HI;
		ndev = port->ndev;
		pkt_size = FIELD_GET(HDRE_PKT_SIZE, hdre_lo);
		off = CS_NE_BUF_HDR_OFF + 8 + 16 + 2;
		total = pkt_size > 16 + 4 ? pkt_size - 16 - 4 : 0;

		if (!cpu_hdr || (w1 & XR_ERROR) || !total ||
		    total > CS_NE_MAX_PKT_LEN || !pfn_valid(PFN_DOWN(bufs[0]))) {
			ndev->stats.rx_errors++;
			if (pfn_valid(PFN_DOWN(bufs[0])))
				cs_ne_rx_recycle(port, bufs[0]);
			done++;
			continue;
		}

		nbufs = 1;
		if (total > CS_NE_BUF_DATA_MAX - (off & 0xff))
			nbufs += DIV_ROUND_UP(total - (CS_NE_BUF_DATA_MAX -
						       (off & 0xff)),
					      CS_NE_BUF_DATA_MAX);
		if (nbufs > 4)
			nbufs = 4;
		for (i = 1; i < nbufs; i++)
			bufs[i] = readl(desc + 40 + i * 4) & XRAM_BUF_MASK;

		skb = napi_alloc_skb(napi, total);
		if (!skb) {
			ndev->stats.rx_dropped++;
			for (i = 0; i < nbufs; i++)
				if (pfn_valid(PFN_DOWN(bufs[i])))
					cs_ne_rx_recycle(port, bufs[i]);
			done++;
			continue;
		}

		seg = min_t(u32, total, CS_NE_BUF_DATA_MAX - (off & 0xff));
		skb_put_data(skb, cs_ne_rx_buf_cpu(ne, bufs[0], off, seg), seg);
		cs_ne_rx_recycle(port, bufs[0]);
		total -= seg;
		for (i = 1; i < nbufs && total; i++) {
			if (!pfn_valid(PFN_DOWN(bufs[i]))) {
				ndev->stats.rx_errors++;
				break;
			}
			seg = min_t(u32, total, CS_NE_BUF_DATA_MAX);
			skb_put_data(skb, cs_ne_rx_buf_cpu(ne, bufs[i],
							   CS_NE_BUF_HDR_OFF, seg),
				     seg);
			cs_ne_rx_recycle(port, bufs[i]);
			total -= seg;
		}
		for (; i < nbufs; i++)
			if (pfn_valid(PFN_DOWN(bufs[i])))
				cs_ne_rx_recycle(port, bufs[i]);
		if (total) {
			dev_kfree_skb(skb);
			done++;
			continue;
		}

		skb->protocol = eth_type_trans(skb, ndev);
		ndev->stats.rx_packets++;
		ndev->stats.rx_bytes += skb->len;
		napi_gro_receive(napi, skb);
		done++;
	}

	ni_wr(ne, NI_CPUXRAM_CPU_CFG_RX(port->id), rd);
	port->rd = rd;

	if (done < budget && napi_complete_done(napi, done))
		ni_wr(ne, NI_CPUXRAM_RXPKT_INTENABLE(port->id), 1);
	return done;
}

static irqreturn_t cs_ne_rx_irq(int irq, void *data)
{
	struct cs_ne_port *port = data;
	struct cs_ne *ne = port->ne;
	u32 status;

	status = ni_rd(ne, NI_CPUXRAM_RXPKT_INTERRUPT(port->id));
	if (!status)
		return IRQ_NONE;
	ni_wr(ne, NI_CPUXRAM_RXPKT_INTENABLE(port->id), 0);
	ni_wr(ne, NI_CPUXRAM_RXPKT_INTERRUPT(port->id), status);
	napi_schedule(&port->napi);
	return IRQ_HANDLED;
}

static void cs_ne_tx_complete(struct cs_ne *ne)
{
	struct cs_ne_txq *txq = &ne->txq;
	struct cs_ne_tx_buf *buf;
	u32 rptr;
	int i;

	rptr = readl(ne->dma + DMA_LSO_TXQ_RPTR(0)) & DMA_PTR_MASK;
	while (txq->tail != rptr) {
		buf = &txq->bufs[txq->tail];
		if (buf->skb) {
			dma_unmap_single(ne->dev, buf->dma, buf->len,
					 DMA_TO_DEVICE);
			dev_kfree_skb_any(buf->skb);
			buf->skb = NULL;
		}
		txq->tail = (txq->tail + 1) & (CS_NE_TX_DESC_NUM - 1);
	}
	for (i = 0; i < CS_NE_PORTS; i++)
		if (ne->ports[i] && ne->ports[i]->opened &&
		    netif_queue_stopped(ne->ports[i]->ndev))
			netif_wake_queue(ne->ports[i]->ndev);
}

static void cs_ne_tx_timer(struct timer_list *t)
{
	struct cs_ne_txq *txq = timer_container_of(txq, t, timer);
	struct cs_ne *ne = txq->ne;

	spin_lock(&txq->lock);
	cs_ne_tx_complete(ne);
	if (txq->tail != txq->head)
		mod_timer(&txq->timer, jiffies + msecs_to_jiffies(CS_NE_TX_TIMER_MS));
	spin_unlock(&txq->lock);
}

static netdev_tx_t cs_ne_start_xmit(struct sk_buff *skb, struct net_device *ndev)
{
	struct cs_ne_port *port = netdev_priv(ndev);
	struct cs_ne *ne = port->ne;
	struct cs_ne_txq *txq = &ne->txq;
	struct cs_ne_tx_buf *buf;
	u32 *desc, word3, free;
	dma_addr_t dma;
	int len;

	if (skb_linearize(skb)) {
		dev_kfree_skb_any(skb);
		ndev->stats.tx_dropped++;
		return NETDEV_TX_OK;
	}
	len = skb->len;
	if (len < CS_NE_TX_MIN_LEN) {
		if (skb_padto(skb, CS_NE_TX_MIN_LEN))
			return NETDEV_TX_OK;
		len = CS_NE_TX_MIN_LEN;
	}

	spin_lock(&txq->lock);
	cs_ne_tx_complete(ne);
	free = (txq->tail - txq->head - 1) & (CS_NE_TX_DESC_NUM - 1);
	if (free < 2) {
		netif_stop_queue(ndev);
		spin_unlock(&txq->lock);
		return NETDEV_TX_BUSY;
	}

	dma = dma_map_single(ne->dev, skb->data, len, DMA_TO_DEVICE);
	if (dma_mapping_error(ne->dev, dma)) {
		spin_unlock(&txq->lock);
		dev_kfree_skb_any(skb);
		ndev->stats.tx_dropped++;
		return NETDEV_TX_OK;
	}

	buf = &txq->bufs[txq->head];
	buf->skb = skb;
	buf->dma = dma;
	buf->len = len;

	word3 = TX_LSO_SEGMENT_EN | CS_NE_MAX_PKT_LEN;
	if (len < CS_NE_MIN_PKT_LEN)
		word3 |= TX_LSO_IP_LENFIX_EN;

	desc = txq->desc + txq->head * 8;
	desc[1] = dma;
	desc[2] = len << 16;
	desc[3] = word3;
	desc[4] = CS_NE_HDRA_CPU_PKT | FIELD_PREP(TX_HDRA_DVOQ, port->id * 8);
	desc[5] = 0;
	desc[6] = 0;
	desc[7] = 0;
	dma_wmb();
	desc[0] = TX_DESC_OWN | TX_DESC_SOF | TX_DESC_EOF |
		  FIELD_PREP(TX_DESC_LEN, len);
	dma_wmb();

	txq->head = (txq->head + 1) & (CS_NE_TX_DESC_NUM - 1);
	writel(txq->head, ne->dma + DMA_LSO_TXQ_WPTR(0));

	ndev->stats.tx_packets++;
	ndev->stats.tx_bytes += len;
	if (!timer_pending(&txq->timer))
		mod_timer(&txq->timer, jiffies + msecs_to_jiffies(CS_NE_TX_TIMER_MS));
	spin_unlock(&txq->lock);
	return NETDEV_TX_OK;
}

static void cs_ne_mac_enable(struct cs_ne_port *port, bool enable)
{
	u32 mask = NI_MAC_RX_RST | NI_MAC_TX_RST | NI_MAC_RX_EN | NI_MAC_TX_EN;

	ni_rmw(port->ne, NI_ETH_MAC_CONFIG0(port->id), mask,
	       enable ? (NI_MAC_RX_EN | NI_MAC_TX_EN) : 0);
}

static void cs_ne_port_hw_init(struct cs_ne_port *port)
{
	struct cs_ne *ne = port->ne;
	int p = port->id;

	ni_rmw(ne, NI_ETH_INT_CONFIG1,
	       NI_INT_CFG_GE_MASK(p) | NI_PHY_MODE_GE(p) | NI_RMII_CLKSRC_GE(p) |
	       NI_TX_USE_GEFIFO_GE(p) | NI_INV_CLK_OUT_GE(p),
	       NI_INT_CFG_GE(p, NI_INT_CFG_RGMII_1000) | NI_TX_USE_GEFIFO_GE(p));
	ni_rmw(ne, NI_ETH_INT_CONFIG2, NI_INT_CONFIG2_GE_MASK(p), 0);
	ni_rmw(ne, NI_ETH_MAC_CONFIG0(p),
	       NI_MAC_RX_FLOW_DISABLE | NI_MAC_TX_FLOW_DISABLE,
	       NI_MAC_RX_FLOW_DISABLE | NI_MAC_TX_FLOW_DISABLE);
}

static void cs_ne_set_hw_addr(struct cs_ne_port *port)
{
	struct cs_ne *ne = port->ne;
	const u8 *a = port->ndev->dev_addr;

	ni_wr(ne, NI_MAC_ADDR0, a[0] | (a[1] << 8) | (a[2] << 16) | (a[3] << 24));
	ni_rmw(ne, NI_MAC_ADDR1, 0xff, a[4]);
	ni_rmw(ne, NI_ETH_MAC_CONFIG2(port->id), 0xff, a[5]);
}

static void cs_ne_mac_config(struct phylink_config *config, unsigned int mode,
			     const struct phylink_link_state *state)
{
}

static void cs_ne_mac_link_down(struct phylink_config *config,
				unsigned int mode, phy_interface_t interface)
{
	struct cs_ne_port *port = container_of(config, struct cs_ne_port,
					       phylink_config);

	cs_ne_mac_enable(port, false);
}

static void cs_ne_mac_link_up(struct phylink_config *config,
			      struct phy_device *phy, unsigned int mode,
			      phy_interface_t interface, int speed, int duplex,
			      bool tx_pause, bool rx_pause)
{
	struct cs_ne_port *port = container_of(config, struct cs_ne_port,
					       phylink_config);
	struct cs_ne *ne = port->ne;
	int p = port->id;
	u32 val = 0;

	cs_ne_mac_enable(port, false);
	if (speed == SPEED_10)
		val |= NI_MAC_SPEED_10;
	if (duplex != DUPLEX_FULL)
		val |= NI_MAC_HALF_DUPLEX;
	if (!rx_pause)
		val |= NI_MAC_RX_FLOW_DISABLE;
	if (!tx_pause)
		val |= NI_MAC_TX_FLOW_DISABLE;
	ni_rmw(ne, NI_ETH_MAC_CONFIG0(p),
	       NI_MAC_SPEED_10 | NI_MAC_HALF_DUPLEX | NI_MAC_RX_FLOW_DISABLE |
	       NI_MAC_TX_FLOW_DISABLE, val);
	ni_rmw(ne, NI_ETH_INT_CONFIG1, NI_INT_CFG_GE_MASK(p),
	       NI_INT_CFG_GE(p, speed == SPEED_1000 ? NI_INT_CFG_RGMII_1000 :
			     NI_INT_CFG_RGMII_100));
	ni_rmw(ne, NI_ETH_INT_CONFIG2, NI_INT_CONFIG2_GE_MASK(p),
	       NI_TX_INTF_LP_TIME_GE(p));
	cs_ne_mac_enable(port, true);
}

static const struct phylink_mac_ops cs_ne_phylink_ops = {
	.mac_config = cs_ne_mac_config,
	.mac_link_down = cs_ne_mac_link_down,
	.mac_link_up = cs_ne_mac_link_up,
};

static int cs_ne_open(struct net_device *ndev)
{
	struct cs_ne_port *port = netdev_priv(ndev);
	struct cs_ne *ne = port->ne;
	int ret;

	ret = phylink_of_phy_connect(port->phylink, port->np, 0);
	if (ret) {
		netdev_err(ndev, "phylink connect failed: %d\n", ret);
		return ret;
	}

	port->rd = ni_rd(ne, NI_CPUXRAM_CPU_CFG_RX(port->id)) & NI_XRAM_PTR_MASK;
	napi_enable(&port->napi);
	ret = request_irq(port->irq, cs_ne_rx_irq, 0, ndev->name, port);
	if (ret) {
		napi_disable(&port->napi);
		phylink_disconnect_phy(port->phylink);
		return ret;
	}
	port->opened = true;
	ni_wr(ne, NI_CPUXRAM_RXPKT_INTERRUPT(port->id), 1);
	ni_wr(ne, NI_CPUXRAM_RXPKT_INTENABLE(port->id), 1);
	phylink_start(port->phylink);
	netif_start_queue(ndev);
	return 0;
}

static int cs_ne_stop(struct net_device *ndev)
{
	struct cs_ne_port *port = netdev_priv(ndev);
	struct cs_ne *ne = port->ne;

	netif_stop_queue(ndev);
	phylink_stop(port->phylink);
	port->opened = false;
	ni_wr(ne, NI_CPUXRAM_RXPKT_INTENABLE(port->id), 0);
	free_irq(port->irq, port);
	napi_disable(&port->napi);
	phylink_disconnect_phy(port->phylink);
	return 0;
}

static int cs_ne_set_mac_address(struct net_device *ndev, void *addr)
{
	int ret;

	ret = eth_mac_addr(ndev, addr);
	if (ret)
		return ret;
	cs_ne_set_hw_addr(netdev_priv(ndev));
	return 0;
}

static int cs_ne_eth_ioctl(struct net_device *ndev, struct ifreq *ifr, int cmd)
{
	struct cs_ne_port *port = netdev_priv(ndev);

	return phylink_mii_ioctl(port->phylink, ifr, cmd);
}

static const struct net_device_ops cs_ne_netdev_ops = {
	.ndo_open = cs_ne_open,
	.ndo_stop = cs_ne_stop,
	.ndo_start_xmit = cs_ne_start_xmit,
	.ndo_set_mac_address = cs_ne_set_mac_address,
	.ndo_validate_addr = eth_validate_addr,
	.ndo_eth_ioctl = cs_ne_eth_ioctl,
};

static int cs_ne_probe_port(struct cs_ne *ne, struct device_node *np)
{
	struct device *dev = ne->dev;
	struct cs_ne_port *port;
	struct net_device *ndev;
	char irqname[8];
	u32 id;
	int ret;

	ret = of_property_read_u32(np, "reg", &id);
	if (ret || id >= CS_NE_PORTS) {
		dev_err(dev, "%pOF: bad port reg\n", np);
		return -EINVAL;
	}

	ndev = devm_alloc_etherdev(dev, sizeof(*port));
	if (!ndev)
		return -ENOMEM;
	SET_NETDEV_DEV(ndev, dev);
	port = netdev_priv(ndev);
	port->ne = ne;
	port->ndev = ndev;
	port->np = np;
	port->id = id;

	snprintf(irqname, sizeof(irqname), "eth%u", id);
	port->irq = platform_get_irq_byname(to_platform_device(dev), irqname);
	if (port->irq < 0)
		return port->irq;

	ret = of_get_phy_mode(np, &port->interface);
	if (ret)
		port->interface = PHY_INTERFACE_MODE_RGMII;

	ret = of_get_ethdev_address(np, ndev);
	if (ret)
		eth_hw_addr_random(ndev);

	ndev->dev.of_node = np;
	ndev->netdev_ops = &cs_ne_netdev_ops;
	ndev->min_mtu = ETH_MIN_MTU;
	ndev->max_mtu = CS_NE_MAX_PKT_LEN - ETH_HLEN - ETH_FCS_LEN - 16;
	netif_napi_add_weight(ndev, &port->napi, cs_ne_rx_poll, napi_budget);

	port->phylink_config.dev = &ndev->dev;
	port->phylink_config.type = PHYLINK_NETDEV;
	port->phylink_config.mac_capabilities = MAC_SYM_PAUSE | MAC_ASYM_PAUSE |
		MAC_10 | MAC_100 | MAC_1000FD;
	phy_interface_set_rgmii(port->phylink_config.supported_interfaces);
	port->phylink = phylink_create(&port->phylink_config,
				       of_fwnode_handle(np), port->interface,
				       &cs_ne_phylink_ops);
	if (IS_ERR(port->phylink))
		return PTR_ERR(port->phylink);

	cs_ne_port_hw_init(port);
	cs_ne_set_hw_addr(port);
	ne->ports[id] = port;

	cs_ne_rx_refill(port, cs_ne_rx_pool_size[id]);

	ret = register_netdev(ndev);
	if (ret) {
		phylink_destroy(port->phylink);
		ne->ports[id] = NULL;
		return ret;
	}
	return 0;
}

static void __iomem *cs_ne_iomap(struct platform_device *pdev, const char *name)
{
	struct resource *res;

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, name);
	if (!res) {
		dev_err(&pdev->dev, "missing resource %s\n", name);
		return NULL;
	}
	return devm_ioremap(&pdev->dev, res->start, resource_size(res));
}

static int cs_ne_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *np;
	struct cs_ne *ne;
	int ret, nports = 0;

	ne = devm_kzalloc(dev, sizeof(*ne), GFP_KERNEL);
	if (!ne)
		return -ENOMEM;
	ne->dev = dev;
	ne->internal_buf = internal_buf;
	platform_set_drvdata(pdev, ne);

	ne->global = cs_ne_iomap(pdev, "global");
	ne->ni = cs_ne_iomap(pdev, "ni");
	ne->dma = cs_ne_iomap(pdev, "dma");
	ne->fe = cs_ne_iomap(pdev, "fe");
	ne->qm = cs_ne_iomap(pdev, "qm");
	ne->tm = cs_ne_iomap(pdev, "tm");
	ne->sch = cs_ne_iomap(pdev, "sch");
	ne->xram = cs_ne_iomap(pdev, "xram");
	if (!ne->global || !ne->ni || !ne->dma || !ne->fe || !ne->qm ||
	    !ne->tm || !ne->sch || !ne->xram)
		return -ENODEV;

	ret = dma_set_mask_and_coherent(dev, DMA_BIT_MASK(32));
	if (ret)
		return ret;

	for_each_available_child_of_node(dev->of_node, np) {
		u8 addr[ETH_ALEN];

		if (!of_node_name_eq(np, "ethernet-port"))
			continue;
		ret = of_get_mac_address(np, addr);
		if (ret == -EPROBE_DEFER) {
			of_node_put(np);
			return ret;
		}
	}

	cs_ne_ni_init(ne);
	ret = cs_ne_dma_init(ne);
	if (ret)
		return ret;
	timer_setup(&ne->txq.timer, cs_ne_tx_timer, 0);

	ret = cs_ne_qm_init(ne);
	if (ret)
		return ret;
	ret = cs752x_sch_init(dev, ne->sch);
	if (ret)
		dev_warn(dev, "SCH init: %d\n", ret);
	ret = cs752x_tm_init(dev, ne->tm, ne->internal_buf);
	if (ret)
		dev_dbg(dev, "TM init: %d\n", ret);
	ret = cs752x_fe_init(dev, ne->fe);
	if (ret)
		dev_warn(dev, "FE init: %d\n", ret);
	writel(readl(ne->fe + FETOP_FE_PRSR_CFG_0) & ~FE_L4_CHKSUM_CHK_DISABLE,
	       ne->fe + FETOP_FE_PRSR_CFG_0);

	for_each_available_child_of_node(dev->of_node, np) {
		if (!of_node_name_eq(np, "ethernet-port"))
			continue;
		ret = cs_ne_probe_port(ne, np);
		if (ret)
			dev_err(dev, "%pOF: probe failed: %d\n", np, ret);
		else
			nports++;
	}
	if (!nports)
		return -ENODEV;
	return 0;
}

static const struct of_device_id cs_ne_of_match[] = {
	{ .compatible = "cortina,cs752x-eth" },
	{ }
};
MODULE_DEVICE_TABLE(of, cs_ne_of_match);

static struct platform_driver cs_ne_driver = {
	.probe = cs_ne_probe,
	.driver = {
		.name = "cs752x-eth",
		.of_match_table = cs_ne_of_match,
		.suppress_bind_attrs = true,
	},
};
builtin_platform_driver(cs_ne_driver);

MODULE_DESCRIPTION("Cortina CS752x network engine driver");
MODULE_LICENSE("GPL");
