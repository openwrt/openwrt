// SPDX-License-Identifier: GPL-2.0
#include <linux/bitfield.h>
#include <linux/bits.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/etherdevice.h>
#include <linux/hrtimer.h>
#include <linux/ethtool.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/iopoll.h>
#include <linux/module.h>
#include <linux/mfd/syscon.h>
#include <linux/regmap.h>
#include <linux/reset.h>
#include <linux/netdevice.h>
#include <linux/of.h>
#include <linux/of_net.h>
#include <linux/of_reserved_mem.h>
#include <linux/phylink.h>
#include <linux/platform_device.h>
#include <linux/skbuff.h>
#include <linux/spinlock.h>
#include <linux/rtnetlink.h>
#include <linux/sizes.h>
#include <linux/workqueue.h>
#include <linux/xarray.h>

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
#define CS_NE_TX_MAX_FRAGS		16
#define CS_NE_TX_TIMER_US		200
#define CS_NE_TX_STOP_TIMER_US		100000
#define CS_NE_TX_REAP_THRESH		(CS_NE_TX_DESC_NUM / 4)
#define CS_NE_ACCESS_TIMEOUT_US		10000
#define CS_NE_RX_TAIL_SETTLE_US		16
/* Bound immediate NAPI repolls before waiting for a DMA-owned head. */
#define CS_NE_RX_OWN_POLLS		4
#define CS_NE_HDRA_CPU_PKT		0xc30001

#define GLOBAL_NETWORK_ENGINE_INTENABLE_0	0x074
#define GLOBAL_IO_DRIVE_STRENGTH		0x0b4
#define   GMAC_DS_MASK				GENMASK(1, 0)

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
#define   NI_INT_COLSC_DLY_TIME		GENMASK(19, 0)
#define   NI_INT_COLSC_PKT		GENMASK(29, 20)
#define   NI_INT_COLSC_FIRST_EN		BIT(30)
#define   NI_INT_COLSC_EN		BIT(31)
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
#define CS_NE_RX_DMA_SHIFT		12
#define CS_NE_RX_BUF_SIZE		SZ_4K
#define CS_NE_RX_MAX_BUFS		4

static const u16 cs_ne_xram_rx_units[CS_NE_RX_INSTANCES] = {
	525, 525, 525, 7, 7, 7, 91, 91, 56
};

static const u16 cs_ne_xram_tx_units[CS_NE_TX_INSTANCES] = { 214, 0 };
static const u16 cs_ne_rx_pool_size[CS_NE_PORTS] = { 1024, 1024, 256 };

struct cs_ne;

enum cs_ne_rx_state {
	CS_NE_RX_PENDING,
	CS_NE_RX_HW,
	CS_NE_RX_CPU,
};

struct cs_ne_rx_buf {
	struct page *page;
	void *cpu;
	dma_addr_t dma;
	struct list_head pending;
	enum cs_ne_rx_state state;
	u16 sync_off;
	u16 sync_len;
	u8 pool;
};

struct cs_ne_tx_buf {
	struct sk_buff *skb;
	dma_addr_t dma;
	u16 len;
	bool page;
};

struct cs_ne_txq {
	u32 *desc;
	dma_addr_t desc_dma;
	struct cs_ne_tx_buf bufs[CS_NE_TX_DESC_NUM];
	u16 head;
	u16 tail;
	/* Serializes shared TX ring ownership and all port queue state. */
	spinlock_t lock;
	struct hrtimer timer;
	struct cs_ne *ne;
	/* Shared admission gate: prevents new TX and queue wakeups while down. */
	bool stopping;
	/* Terminal cleanup, unlike last-port close, also stops completion polls. */
	bool poll_stopping;
	bool slow_poll;
	bool fault;
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
	u8 rx_owned_polls;
	struct delayed_work rx_owned_retry;
	/*
	 * Published under txq.lock after IRQ/NAPI setup; gates shared wakeups.
	 * netif_running() is already true while ndo_open() can still fail.
	 */
	bool opened;
	bool registered;
};

#define CS_NE_NUM_RESETS 7
static const char * const cs_ne_reset_names[CS_NE_NUM_RESETS] = {
	"ni", "fe", "tm", "sch", "qm", "dma", "xram",
};

struct cs_ne {
	struct device *dev;
	/* Shared NI registers contain fields belonging to several MACs. */
	spinlock_t ni_lock;
	struct regmap *global;
	struct reset_control_bulk_data resets[CS_NE_NUM_RESETS];
	void __iomem *ni;
	void __iomem *dma;
	void __iomem *fe;
	void __iomem *qm;
	void __iomem *tm;
	void __iomem *sch;
	void __iomem *xram;
	struct cs_ne_port *ports[CS_NE_PORTS];
	u8 addresses[CS_NE_PORTS][ETH_ALEN];
	unsigned int address_ports;
	unsigned int random_addresses;
	struct cs_ne_txq txq;
	/* RX mappings stay registered until hardware has been quiesced. */
	struct xarray rx_buffers;
	struct list_head rx_pending;
	/* Serializes RX ownership, FIFO writes, retry and IRQ rearm/stop. */
	spinlock_t rx_lock;
	struct delayed_work rx_retry;
	bool rx_stopping;
	bool hw_active;
	bool hw_accessible;
	bool dma_quiesced;
	bool dma_pinned;
	bool hw_cleanup_done;
};

static u32 ni_rd(struct cs_ne *ne, u32 off)
{
	return readl(ne->ni + off);
}

static void ni_wr(struct cs_ne *ne, u32 off, u32 val)
{
	writel(val, ne->ni + off);
}

static void ni_rmw(struct cs_ne *ne, u32 off, u32 mask, u32 val)
{
	unsigned long flags;

	spin_lock_irqsave(&ne->ni_lock, flags);
	writel((readl(ne->ni + off) & ~mask) | (val & mask), ne->ni + off);
	spin_unlock_irqrestore(&ne->ni_lock, flags);
}

static u32 qm_rd(struct cs_ne *ne, u32 off)
{
	return readl(ne->qm + off);
}

static void qm_wr(struct cs_ne *ne, u32 off, u32 val)
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

static int cs_ne_voq_map_init(struct cs_ne *ne)
{
	int voq, did, ret;

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
		ret = cs_ne_set_voq_map(ne, voq, did);
		if (ret)
			return dev_err_probe(ne->dev, ret, "VOQ map %d timeout\n", voq);
	}
	return 0;
}

static int cs_ne_port_calendar_init(struct cs_ne *ne)
{
	int ret, i, j;

	for (i = 0; i < 12; i++) {
		for (j = 0; j < 8; j++) {
			ret = cs_ne_set_port_calendar(ne, i * 8 + j, j);
			if (ret)
				return dev_err_probe(ne->dev, ret, "port calendar timeout\n");
		}
	}
	for (i = 0; i < 12; i++) {
		ret = cs_ne_set_port_calendar(ne, i * 8 + 2, 4);
		if (ret)
			return dev_err_probe(ne->dev, ret, "port calendar override timeout\n");
	}
	return 0;
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

static int cs_ne_check_reserved_memory(struct cs_ne *ne)
{
	struct device_node *np;
	struct reserved_mem *rmem;
	bool no_map;

	np = of_parse_phandle(ne->dev->of_node, "memory-region", 0);
	if (!np)
		return dev_err_probe(ne->dev, -EINVAL, "QM reserved memory is required\n");
	rmem = of_reserved_mem_lookup(np);
	no_map = of_property_read_bool(np, "no-map");
	of_node_put(np);
	/* Only the 512 MiB DDR layout is currently verified on CS7542. */
	if (!rmem || !no_map || rmem->base != 0x1c000000 || rmem->size != SZ_64M)
		return dev_err_probe(ne->dev, -EINVAL, "unsupported QM DDR reservation\n");
	return 0;
}

static int cs_ne_qm_init_cfg(struct cs_ne *ne)
{
	u32 val;

	val = FIELD_PREP(QM_MAX_PKT_LEN, CS_NE_MAX_PKT_LEN) |
	      FIELD_PREP(QM_MTU, (ETH_DATA_LEN + ETH_HLEN) & 0xff) |
	      FIELD_PREP(QM_EGRESS_FC_INT_THRESHOLD, 1);
	qm_wr(ne, QM_CONFIG_0, val);

	val = qm_rd(ne, QM_CONFIG_1);
	val &= ~(QM_MEM_CONFIG | QM_CPU_BANKS | QM_PAGING_SIZE);
	val |= FIELD_PREP(QM_MEM_CONFIG, 2) |
	       FIELD_PREP(QM_CPU_BANKS, 7) | QM_PAGING_SIZE;
	qm_wr(ne, QM_CONFIG_1, val);
	qm_wr(ne, QM_SPARE, 0);
	return 0;
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
		ret = cs_ne_qm_set_profile(ne, i, data);
		if (ret) {
			dev_err(ne->dev, "QM profile %d timeout\n", i);
			return ret;
		}
	}
	return 0;
}

static int cs_ne_dma_init(struct cs_ne *ne)
{
	struct cs_ne_txq *txq = &ne->txq;
	u32 val;

	txq->desc = dma_alloc_coherent(ne->dev, CS_NE_TX_DESC_NUM * 32,
				       &txq->desc_dma, GFP_KERNEL);
	if (!txq->desc)
		return -ENOMEM;
	txq->ne = ne;
	txq->stopping = true;
	spin_lock_init(&txq->lock);

	writel(FIELD_PREP(DMA_BURST_LEN, 3), ne->dma + DMA_LSO_RXDMA_CONTROL);
	writel(FIELD_PREP(DMA_BURST_LEN, 3), ne->dma + DMA_LSO_TXDMA_CONTROL);
	writel(0, ne->dma + DMA_LSO_TXQ_CONTROL(0));
	writel((txq->desc_dma & ~0xf) | CS_NE_TX_DESC_ORDER,
	       ne->dma + DMA_LSO_TXQ_BASE_DEPTH(0));
	txq->head = readl(ne->dma + DMA_LSO_TXQ_RPTR(0)) & DMA_PTR_MASK &
		    (CS_NE_TX_DESC_NUM - 1);
	txq->tail = txq->head;
	writel(txq->head, ne->dma + DMA_LSO_TXQ_WPTR(0));

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

static int cs_ne_ni_init(struct cs_ne *ne)
{
	u32 val;
	int ret, i;

	val = ni_rd(ne, NI_INTF_RST_CONFIG);
	ni_wr(ne, NI_INTF_RST_CONFIG, val | NI_INTF_RST_GE_ALL);
	ni_wr(ne, NI_INTF_RST_CONFIG, val & ~NI_INTF_RST_GE_ALL);

	ni_wr(ne, NI_SCH_BP_THLD_ETH, NI_SCH_BP_THLD_ETH_ASIC);
	for (i = 0; i < CS_NE_PORTS; i++)
		ni_wr(ne, NI_ETH_MAC_CONFIG0(i), NI_MAC_CONFIG0_INIT);

	ret = regmap_update_bits(ne->global, GLOBAL_IO_DRIVE_STRENGTH,
				 GMAC_DS_MASK, GMAC_DS_MASK);
	if (ret)
		return ret;
	ret = regmap_write(ne->global, GLOBAL_NETWORK_ENGINE_INTENABLE_0, 0);
	if (ret)
		return ret;

	ni_rmw(ne, NI_RX_AGG_CONFIG, NI_RX_PORT_CAL_DIS, 0);
	ret = cs_ne_port_calendar_init(ne);
	if (ret)
		return ret;
	ret = cs_ne_qm_init_cfg(ne);
	if (ret)
		return ret;
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
	ret = cs_ne_voq_map_init(ne);
	if (ret)
		return ret;

	for (i = 0; i < CS_NE_PORTS; i++) {
		ni_wr(ne, NI_CPUXRAM_INT_COLSC_CFG(i),
		      NI_INT_COLSC_FIRST_EN | FIELD_PREP(NI_INT_COLSC_PKT, 4));
		ni_wr(ne, NI_CPUXRAM_RXPKT_INTENABLE(i), 0);
		ni_wr(ne, NI_CPUXRAM_RXPKT_INTERRUPT(i), 1);
	}
	return 0;
}

/* Called with rx_lock: all CPU free-list producers share one FIFO. */
static bool cs_ne_qm_push_buffer(struct cs_ne *ne, struct cs_ne_rx_buf *buf)
{
	u32 value = qm_rd(ne, QM_CPU_PATH_STATUS_0);

	/* Never write a full FIFO, or spin waiting for it with IRQs disabled. */
	if (FIELD_GET(QM_SDRAM_ADDR_FIFO_WR_CNT, value) >= 28)
		return false;
	list_del_init(&buf->pending);
	buf->state = CS_NE_RX_HW;
	qm_wr(ne, QM_CPU_PATH_LINUX_SDRAM_ADDR(buf->pool), lower_32_bits(buf->dma));
	return true;
}

static void cs_ne_rx_kick(struct cs_ne *ne)
{
	struct cs_ne_rx_buf *buf;
	unsigned long flags;

	spin_lock_irqsave(&ne->rx_lock, flags);
	if (ne->rx_stopping)
		goto unlock;
	while (!list_empty(&ne->rx_pending)) {
		buf = list_first_entry(&ne->rx_pending, struct cs_ne_rx_buf, pending);
		if (!cs_ne_qm_push_buffer(ne, buf))
			break;
	}
	/* Serialized with the stop flag so cancellation cannot miss a requeue. */
	if (!list_empty(&ne->rx_pending))
		mod_delayed_work(system_wq, &ne->rx_retry, msecs_to_jiffies(1));
unlock:
	spin_unlock_irqrestore(&ne->rx_lock, flags);
}

static void cs_ne_rx_retry(struct work_struct *work)
{
	struct cs_ne *ne = container_of(to_delayed_work(work), struct cs_ne, rx_retry);

	cs_ne_rx_kick(ne);
}

static void cs_ne_rx_cancel_retry(void *data)
{
	struct cs_ne *ne = data;
	unsigned long flags;

	spin_lock_irqsave(&ne->rx_lock, flags);
	ne->rx_stopping = true;
	spin_unlock_irqrestore(&ne->rx_lock, flags);
	cancel_delayed_work_sync(&ne->rx_retry);
}

/* Probe errors run before any RX address is published to hardware. */
static void cs_ne_rx_cleanup(void *data)
{
	struct cs_ne *ne = data;
	struct cs_ne_rx_buf *buf;
	unsigned long index;

	cs_ne_rx_cancel_retry(ne);
	xa_for_each(&ne->rx_buffers, index, buf) {
		/* A verified held reset revokes every hardware buffer reference. */
		if (!ne->dma_quiesced && WARN_ON_ONCE(buf->state != CS_NE_RX_PENDING))
			continue;
		xa_erase(&ne->rx_buffers, index);
		list_del_init(&buf->pending);
		dma_unmap_page(ne->dev, buf->dma, CS_NE_RX_BUF_SIZE, DMA_FROM_DEVICE);
		__free_page(buf->page);
		kfree(buf);
	}
	if (xa_empty(&ne->rx_buffers))
		xa_destroy(&ne->rx_buffers);
}

static int cs_ne_rx_refill(struct cs_ne_port *port, int count)
{
	struct cs_ne *ne = port->ne;
	struct cs_ne_rx_buf *buf;
	unsigned long flags;
	int ret, i;

	for (i = 0; i < count; i++) {
		buf = kzalloc(sizeof(*buf), GFP_KERNEL);
		if (!buf)
			break;
		buf->page = dev_alloc_page();
		if (!buf->page)
			goto free_buf;
		buf->cpu = page_address(buf->page);
		if (!buf->cpu)
			goto free_page;
		buf->dma = dma_map_page(ne->dev, buf->page, 0, CS_NE_RX_BUF_SIZE,
					DMA_FROM_DEVICE);
		if (dma_mapping_error(ne->dev, buf->dma))
			goto free_page;
		/* Hardware stores a 32-bit, 4 KiB aligned buffer address. */
		if (upper_32_bits(buf->dma) || !IS_ALIGNED(buf->dma, CS_NE_RX_BUF_SIZE))
			goto unmap;
		buf->pool = port->id;
		buf->state = CS_NE_RX_PENDING;
		INIT_LIST_HEAD(&buf->pending);
		ret = xa_insert(&ne->rx_buffers, buf->dma >> CS_NE_RX_DMA_SHIFT,
				buf, GFP_KERNEL);
		if (ret)
			goto unmap;
		spin_lock_irqsave(&ne->rx_lock, flags);
		list_add_tail(&buf->pending, &ne->rx_pending);
		spin_unlock_irqrestore(&ne->rx_lock, flags);
		continue;
unmap:
		dma_unmap_page(ne->dev, buf->dma, CS_NE_RX_BUF_SIZE, DMA_FROM_DEVICE);
free_page:
		__free_page(buf->page);
free_buf:
		kfree(buf);
		break;
	}
	cs_ne_rx_kick(ne);
	return i;
}

/* Claim every buffer before copying or recycling any part of the packet. */
static bool cs_ne_rx_claim(struct cs_ne *ne, const dma_addr_t *dma,
			   struct cs_ne_rx_buf **bufs, unsigned int count)
{
	struct cs_ne_rx_buf *buf;
	unsigned long flags;
	unsigned int i;
	bool valid = true;

	if (!count || count > CS_NE_RX_MAX_BUFS)
		return false;
	spin_lock_irqsave(&ne->rx_lock, flags);
	for (i = 0; i < count; i++) {
		bufs[i] = NULL;
		buf = xa_load(&ne->rx_buffers, dma[i] >> CS_NE_RX_DMA_SHIFT);
		if (!buf || buf->dma != dma[i] || buf->state != CS_NE_RX_HW) {
			valid = false;
			continue;
		}
		buf->state = CS_NE_RX_CPU;
		buf->sync_off = 0;
		buf->sync_len = 0;
		bufs[i] = buf;
	}
	spin_unlock_irqrestore(&ne->rx_lock, flags);
	return valid;
}

static bool cs_ne_rx_range_valid(u32 off, u32 len)
{
	return off <= CS_NE_RX_BUF_SIZE && len <= CS_NE_RX_BUF_SIZE - off;
}

static void *cs_ne_rx_buf_cpu(struct cs_ne *ne, struct cs_ne_rx_buf *buf,
			      u32 off, u32 len)
{
	u32 start, end;

	if (!len || WARN_ON_ONCE(buf->state != CS_NE_RX_CPU ||
				 !cs_ne_rx_range_valid(off, len)))
		return NULL;
	start = buf->sync_len ? min_t(u32, off, buf->sync_off) : off;
	end = max_t(u32, off + len, buf->sync_off + buf->sync_len);
	buf->sync_off = start;
	buf->sync_len = end - start;
	dma_sync_single_range_for_cpu(ne->dev, buf->dma, off, len, DMA_FROM_DEVICE);
	return buf->cpu + off;
}

static void cs_ne_rx_recycle(struct cs_ne *ne, struct cs_ne_rx_buf *buf)
{
	unsigned long flags;

	if (!buf)
		return;
	/* A claimed mapping cannot be handed to another CPU while it is copied. */
	if (WARN_ON_ONCE(buf->state != CS_NE_RX_CPU))
		return;
	if (buf->sync_len)
		dma_sync_single_range_for_device(ne->dev, buf->dma, buf->sync_off,
						 buf->sync_len, DMA_FROM_DEVICE);
	spin_lock_irqsave(&ne->rx_lock, flags);
	buf->sync_len = 0;
	buf->state = CS_NE_RX_PENDING;
	list_add_tail(&buf->pending, &ne->rx_pending);
	spin_unlock_irqrestore(&ne->rx_lock, flags);
}

static void cs_ne_rx_owned_retry(struct work_struct *work)
{
	struct cs_ne_port *port = container_of(to_delayed_work(work),
					     struct cs_ne_port, rx_owned_retry);
	struct cs_ne *ne = port->ne;
	unsigned long flags;

	local_bh_disable();
	spin_lock_irqsave(&ne->rx_lock, flags);
	if (READ_ONCE(port->opened) && !ne->rx_stopping)
		napi_schedule(&port->napi);
	spin_unlock_irqrestore(&ne->rx_lock, flags);
	local_bh_enable();
}

static void cs_ne_tx_reap(struct cs_ne *ne);

static int cs_ne_rx_poll(struct napi_struct *napi, int budget)
{
	struct cs_ne_port *port = container_of(napi, struct cs_ne_port, napi);
	struct cs_ne *ne = port->ne;
	struct net_device *ndev = port->ndev;
	struct cs_ne_rx_buf *bufs[CS_NE_RX_MAX_BUFS];
	dma_addr_t dma[CS_NE_RX_MAX_BUFS];
	void __iomem *desc;
	struct sk_buff *skb;
	void *data;
	u32 wr, rd, next, w1, hdre_hi, hdre_lo, pkt_size;
	u32 off, total, dma_bytes, seg, nbufs, remaining;
	unsigned int i;
	int done = 0;
	bool valid, retry = false, defer = false;
	unsigned long flags;

	cs_ne_tx_reap(ne);
	wr = ni_rd(ne, NI_CPUXRAM_CPU_STA_RX(port->id)) & NI_XRAM_PTR_MASK;
	rd = port->rd;
	while (rd != wr && done < budget) {
		desc = ne->xram + rd * 8;
		w1 = readl(desc + 4);
		if (w1 & XR_OWNERSHIP) {
			if (port->rx_owned_polls < CS_NE_RX_OWN_POLLS) {
				port->rx_owned_polls++;
				retry = true;
			} else {
				if (port->rx_owned_polls == CS_NE_RX_OWN_POLLS) {
					ndev->stats.rx_fifo_errors++;
					port->rx_owned_polls++;
				}
				defer = true;
			}
			break;
		}
		port->rx_owned_polls = 0;
		next = FIELD_GET(XR_NEXT_LINK, w1);
		if (next == wr &&
		    (ni_rd(ne, NI_CPUXRAM_CPU_STA_RX(port->id)) & NI_XRAM_PTR_MASK) == wr) {
			/*
			 * The next link reaches the unchanged hardware write pointer:
			 * this is the current tail packet. Retain the vendor's 16us
			 * DDR-arrival guard before reading its header and payload.
			 * NAPI softirq context cannot use a sleeping delay here.
			 */
			udelay(CS_NE_RX_TAIL_SETTLE_US);
		}
		hdre_hi = readl(desc + 8);
		hdre_lo = readl(desc + 12);
		rd = next;
		pkt_size = FIELD_GET(HDRE_PKT_SIZE, hdre_lo);
		off = CS_NE_BUF_HDR_OFF + 8 + 16 + 2;
		dma_bytes = pkt_size > 16 ? pkt_size - 16 : 0;
		total = dma_bytes > ETH_FCS_LEN ? dma_bytes - ETH_FCS_LEN : 0;
		valid = total >= ETH_HLEN && total <= CS_NE_MAX_PKT_LEN;
		/* Recycle every DMA buffer, including a trailing FCS-only one. */
		nbufs = 1;
		if (dma_bytes > CS_NE_BUF_DATA_MAX - (off & 0xff))
			nbufs += DIV_ROUND_UP(dma_bytes - (CS_NE_BUF_DATA_MAX - (off & 0xff)),
					      CS_NE_BUF_DATA_MAX);
		if (nbufs > CS_NE_RX_MAX_BUFS) {
			valid = false;
			nbufs = CS_NE_RX_MAX_BUFS;
		}
		for (i = 0; i < nbufs; i++)
			dma[i] = readl(desc + 40 + i * 4) & XRAM_BUF_MASK;
		valid = cs_ne_rx_claim(ne, dma, bufs, nbufs) && valid;
		valid = valid && (hdre_hi & HDRE_CPU_HEADER_HI) && !(w1 & XR_ERROR);
		if (!valid) {
			ndev->stats.rx_errors++;
			goto recycle;
		}
		skb = napi_alloc_skb(napi, total);
		if (!skb) {
			ndev->stats.rx_dropped++;
			goto recycle;
		}
		remaining = total;
		for (i = 0; i < nbufs && remaining; i++) {
			seg = min_t(u32, remaining, CS_NE_BUF_DATA_MAX - (off & 0xff));
			data = cs_ne_rx_buf_cpu(ne, bufs[i], off, seg);
			if (!data)
				break;
			skb_put_data(skb, data, seg);
			remaining -= seg;
			off = CS_NE_BUF_HDR_OFF;
		}
		if (remaining) {
			dev_kfree_skb(skb);
			ndev->stats.rx_errors++;
		} else {
			ndev->stats.rx_packets++;
			ndev->stats.rx_bytes += skb->len;
			skb->protocol = eth_type_trans(skb, ndev);
			napi_gro_receive(napi, skb);
		}
recycle:
		for (i = 0; i < nbufs; i++)
			cs_ne_rx_recycle(ne, bufs[i]);
		done++;
	}
	cs_ne_rx_kick(ne);
	ni_wr(ne, NI_CPUXRAM_CPU_CFG_RX(port->id), rd);
	port->rd = rd;
	if (retry)
		return budget;
	if (done < budget && napi_complete_done(napi, done)) {
		spin_lock_irqsave(&ne->rx_lock, flags);
		if (READ_ONCE(port->opened) && !ne->rx_stopping) {
			/*
			 * Keep DMA-owned buffers untouched and the IRQ masked.
			 * A delayed poll also handles completion without a new edge.
			 */
			if (defer)
				mod_delayed_work(system_wq, &port->rx_owned_retry,
						 msecs_to_jiffies(1));
			else
				ni_wr(ne, NI_CPUXRAM_RXPKT_INTENABLE(port->id), 1);
		}
		spin_unlock_irqrestore(&ne->rx_lock, flags);
	}
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

static void cs_ne_tx_stop_all(struct cs_ne *ne)
{
	int i;

	for (i = 0; i < CS_NE_PORTS; i++)
		if (ne->ports[i] && READ_ONCE(ne->ports[i]->opened))
			netif_stop_queue(ne->ports[i]->ndev);
}

static void cs_ne_tx_complete(struct cs_ne *ne)
{
	struct cs_ne_txq *txq = &ne->txq;
	struct cs_ne_tx_buf *buf;
	u32 rptr;
	int i;

	if (txq->fault)
		return;
	rptr = readl(ne->dma + DMA_LSO_TXQ_RPTR(0)) & DMA_PTR_MASK;
	/* Never consume unsubmitted descriptors or loop past the ring boundary. */
	if (rptr >= CS_NE_TX_DESC_NUM ||
	    ((rptr - txq->tail) & (CS_NE_TX_DESC_NUM - 1)) >
	    ((txq->head - txq->tail) & (CS_NE_TX_DESC_NUM - 1))) {
		txq->fault = true;
		cs_ne_tx_stop_all(ne);
		dev_err_ratelimited(ne->dev, "invalid TX completion pointer %#x\n", rptr);
		return;
	}
	/* This engine retires through RPTR; completed descriptors keep OWN set. */
	while (txq->tail != rptr) {
		buf = &txq->bufs[txq->tail];
		if (buf->len) {
			if (buf->page)
				dma_unmap_page(ne->dev, buf->dma, buf->len,
					       DMA_TO_DEVICE);
			else
				dma_unmap_single(ne->dev, buf->dma, buf->len,
						 DMA_TO_DEVICE);
			buf->len = 0;
		}
		if (buf->skb) {
			dev_consume_skb_any(buf->skb);
			buf->skb = NULL;
		}
		txq->tail = (txq->tail + 1) & (CS_NE_TX_DESC_NUM - 1);
	}
	if (txq->stopping)
		return;
	if (((txq->tail - txq->head - 1) & (CS_NE_TX_DESC_NUM - 1)) <
	    CS_NE_TX_MAX_FRAGS + 2)
		return;
	for (i = 0; i < CS_NE_PORTS; i++)
		if (ne->ports[i] && READ_ONCE(ne->ports[i]->opened) &&
		    netif_queue_stopped(ne->ports[i]->ndev))
			netif_wake_queue(ne->ports[i]->ndev);
}

static u32 cs_ne_tx_free(const struct cs_ne_txq *txq)
{
	return (txq->tail - txq->head - 1) & (CS_NE_TX_DESC_NUM - 1);
}

/*
 * There is no TX completion interrupt in use, so completed descriptors are
 * reaped from the RX NAPI poll and from this timer. It has to be short:
 * with HZ=100 a timer_list fires at most every 10 ms, and TCP small queues
 * then limit a socket to what it can send per tick. Called with txq->lock.
 */
static void cs_ne_tx_arm_timer(struct cs_ne_txq *txq)
{
	if (!txq->poll_stopping && !txq->fault && txq->tail != txq->head &&
	    (!hrtimer_is_queued(&txq->timer) || (!txq->stopping && txq->slow_poll))) {
		/* No active socket needs fast completion while all ports are down.
		 * Reopening shortens an already queued slow poll immediately.
		 */
		txq->slow_poll = txq->stopping;
		hrtimer_start(&txq->timer,
			      us_to_ktime(txq->slow_poll ? CS_NE_TX_STOP_TIMER_US :
					  CS_NE_TX_TIMER_US), HRTIMER_MODE_REL_SOFT);
	}
}

static enum hrtimer_restart cs_ne_tx_timer(struct hrtimer *t)
{
	struct cs_ne_txq *txq = container_of(t, struct cs_ne_txq, timer);

	spin_lock(&txq->lock);
	cs_ne_tx_complete(txq->ne);
	cs_ne_tx_arm_timer(txq);
	spin_unlock(&txq->lock);
	return HRTIMER_NORESTART;
}

/* Set the stop flag under the same lock as every timer rearm. */
static void cs_ne_tx_cancel_timer(void *data)
{
	struct cs_ne_txq *txq = data;

	spin_lock_bh(&txq->lock);
	txq->stopping = true;
	txq->poll_stopping = true;
	spin_unlock_bh(&txq->lock);
	hrtimer_cancel(&txq->timer);
}

static void cs_ne_tx_reap(struct cs_ne *ne)
{
	struct cs_ne_txq *txq = &ne->txq;

	if (READ_ONCE(txq->tail) == READ_ONCE(txq->head))
		return;
	spin_lock(&txq->lock);
	cs_ne_tx_complete(ne);
	spin_unlock(&txq->lock);
}

static void cs_ne_tx_desc(struct cs_ne_txq *txq, int port_id, dma_addr_t dma,
			  u32 len, u32 total, bool sof, bool eof)
{
	u32 *desc = txq->desc + txq->head * 8;
	u32 word0, word3;

	word3 = TX_LSO_SEGMENT_EN | CS_NE_MAX_PKT_LEN;
	if (eof && total < CS_NE_MIN_PKT_LEN)
		word3 |= TX_LSO_IP_LENFIX_EN;
	word0 = TX_DESC_OWN | FIELD_PREP(TX_DESC_LEN, len);
	if (sof)
		word0 |= TX_DESC_SOF;
	if (eof)
		word0 |= TX_DESC_EOF;

	desc[1] = dma;
	desc[2] = sof ? total << 16 : 0;
	desc[3] = word3;
	desc[4] = CS_NE_HDRA_CPU_PKT | FIELD_PREP(TX_HDRA_DVOQ, port_id * 8);
	desc[5] = 0;
	desc[6] = 0;
	desc[7] = 0;
	dma_wmb();
	desc[0] = word0;
}

static netdev_tx_t cs_ne_xmit_sg(struct sk_buff *skb, struct net_device *ndev)
{
	struct cs_ne_port *port = netdev_priv(ndev);
	struct cs_ne *ne = port->ne;
	struct cs_ne_txq *txq = &ne->txq;
	int nfrags = skb_shinfo(skb)->nr_frags;
	u16 first = txq->head;
	struct cs_ne_tx_buf *buf;
	dma_addr_t dma;
	u32 free, len;
	int i;

	free = (txq->tail - txq->head - 1) & (CS_NE_TX_DESC_NUM - 1);
	if (free < nfrags + 2) {
		cs_ne_tx_stop_all(ne);
		return NETDEV_TX_BUSY;
	}

	for (i = 0; i <= nfrags; i++) {
		const skb_frag_t *frag = i ? &skb_shinfo(skb)->frags[i - 1] : NULL;

		buf = &txq->bufs[txq->head];
		if (frag) {
			len = skb_frag_size(frag);
			dma = skb_frag_dma_map(ne->dev, frag, 0, len,
					       DMA_TO_DEVICE);
		} else {
			len = skb_headlen(skb);
			dma = dma_map_single(ne->dev, skb->data, len,
					     DMA_TO_DEVICE);
		}
		if (dma_mapping_error(ne->dev, dma))
			goto unmap;
		buf->skb = NULL;
		buf->dma = dma;
		buf->len = len;
		buf->page = !!frag;
		cs_ne_tx_desc(txq, port->id, dma, len, skb->len, !frag,
			      i == nfrags);
		txq->head = (txq->head + 1) & (CS_NE_TX_DESC_NUM - 1);
	}
	buf->skb = skb;
	dma_wmb();
	writel(txq->head, ne->dma + DMA_LSO_TXQ_WPTR(0));

	ndev->stats.tx_packets++;
	ndev->stats.tx_bytes += skb->len;
	free = (txq->tail - txq->head - 1) & (CS_NE_TX_DESC_NUM - 1);
	if (free < CS_NE_TX_MAX_FRAGS + 2)
		cs_ne_tx_stop_all(ne);
	return NETDEV_TX_OK;

unmap:
	while (txq->head != first) {
		txq->head = (txq->head - 1) & (CS_NE_TX_DESC_NUM - 1);
		buf = &txq->bufs[txq->head];
		txq->desc[txq->head * 8] = 0;
		if (buf->page)
			dma_unmap_page(ne->dev, buf->dma, buf->len,
				       DMA_TO_DEVICE);
		else
			dma_unmap_single(ne->dev, buf->dma, buf->len,
					 DMA_TO_DEVICE);
		buf->len = 0;
	}
	dev_kfree_skb_any(skb);
	ndev->stats.tx_dropped++;
	return NETDEV_TX_OK;
}

static netdev_tx_t cs_ne_start_xmit(struct sk_buff *skb, struct net_device *ndev)
{
	struct cs_ne_port *port = netdev_priv(ndev);
	struct cs_ne *ne = port->ne;
	struct cs_ne_txq *txq = &ne->txq;
	struct cs_ne_tx_buf *buf;
	u32 *desc, word3, free;
	dma_addr_t dma;
	netdev_tx_t ret;
	int len;

	if (skb_is_nonlinear(skb) && !skb_has_frag_list(skb) &&
	    skb_shinfo(skb)->nr_frags <= CS_NE_TX_MAX_FRAGS &&
	    skb_headlen(skb) >= ETH_HLEN && skb->len >= CS_NE_TX_MIN_LEN) {
		spin_lock(&txq->lock);
		if (cs_ne_tx_free(txq) < CS_NE_TX_REAP_THRESH)
			cs_ne_tx_complete(ne);
		if (txq->stopping || txq->fault) {
			spin_unlock(&txq->lock);
			return NETDEV_TX_BUSY;
		}
		ret = cs_ne_xmit_sg(skb, ndev);
		cs_ne_tx_arm_timer(txq);
		spin_unlock(&txq->lock);
		return ret;
	}

	if (skb_linearize(skb)) {
		dev_kfree_skb_any(skb);
		ndev->stats.tx_dropped++;
		return NETDEV_TX_OK;
	}
	len = skb->len;
	if (len < CS_NE_TX_MIN_LEN) {
		if (skb_padto(skb, CS_NE_TX_MIN_LEN)) {
			ndev->stats.tx_dropped++;
			return NETDEV_TX_OK;
		}
		len = CS_NE_TX_MIN_LEN;
	}

	spin_lock(&txq->lock);
	if (cs_ne_tx_free(txq) < CS_NE_TX_REAP_THRESH)
		cs_ne_tx_complete(ne);
	if (txq->stopping || txq->fault) {
		spin_unlock(&txq->lock);
		return NETDEV_TX_BUSY;
	}
	free = cs_ne_tx_free(txq);
	if (free < 2) {
		cs_ne_tx_stop_all(ne);
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
	buf->page = false;

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
	free = (txq->tail - txq->head - 1) & (CS_NE_TX_DESC_NUM - 1);
	if (free < CS_NE_TX_MAX_FRAGS + 2)
		cs_ne_tx_stop_all(ne);
	cs_ne_tx_arm_timer(txq);
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
	unsigned long flags;
	int ret;

	/* RTNL also protects the final probe activation. */
	if (ne->rx_stopping)
		return -ENODEV;
	if (READ_ONCE(ne->txq.fault))
		return -EIO;

	ret = phylink_of_phy_connect(port->phylink, port->np, 0);
	if (ret) {
		netdev_err(ndev, "phylink connect failed: %d\n", ret);
		return ret;
	}

	port->rd = ni_rd(ne, NI_CPUXRAM_CPU_CFG_RX(port->id)) & NI_XRAM_PTR_MASK;
	port->rx_owned_polls = 0;
	napi_enable(&port->napi);
	ret = request_irq(port->irq, cs_ne_rx_irq, 0, ndev->name, port);
	if (ret)
		goto err_disable_napi;

	spin_lock_bh(&ne->txq.lock);
	if (ne->txq.fault) {
		spin_unlock_bh(&ne->txq.lock);
		ret = -EIO;
		goto err_free_irq;
	}
	WRITE_ONCE(port->opened, true);
	ne->txq.stopping = false;
	cs_ne_tx_arm_timer(&ne->txq);
	/* A concurrent TX fault must be able to stop this queue permanently. */
	netif_start_queue(ndev);
	spin_unlock_bh(&ne->txq.lock);
	spin_lock_irqsave(&ne->rx_lock, flags);
	ni_wr(ne, NI_CPUXRAM_RXPKT_INTERRUPT(port->id), 1);
	ni_wr(ne, NI_CPUXRAM_RXPKT_INTENABLE(port->id), 0);
	spin_unlock_irqrestore(&ne->rx_lock, flags);
	/* A packet queued while down need not produce a new interrupt edge.
	 * Poll once with interrupts masked; NAPI completion rearms them.
	 */
	local_bh_disable();
	napi_schedule(&port->napi);
	local_bh_enable();
	phylink_start(port->phylink);
	return 0;

err_free_irq:
	free_irq(port->irq, port);
err_disable_napi:
	napi_disable(&port->napi);
	phylink_disconnect_phy(port->phylink);
	return ret;
}

static int cs_ne_stop(struct net_device *ndev)
{
	struct cs_ne_port *port = netdev_priv(ndev);
	struct cs_ne *ne = port->ne;
	unsigned long flags;
	bool last = true;
	int i;

	/* Serialize queue wakeups before waiting for ndo_start_xmit to finish. */
	spin_lock_bh(&ne->txq.lock);
	WRITE_ONCE(port->opened, false);
	for (i = 0; i < CS_NE_PORTS; i++)
		if (ne->ports[i] && READ_ONCE(ne->ports[i]->opened))
			last = false;
	if (last)
		ne->txq.stopping = true;
	spin_unlock_bh(&ne->txq.lock);
	netif_tx_disable(ndev);
	phylink_stop(port->phylink);
	spin_lock_irqsave(&ne->rx_lock, flags);
	ni_wr(ne, NI_CPUXRAM_RXPKT_INTENABLE(port->id), 0);
	spin_unlock_irqrestore(&ne->rx_lock, flags);
	free_irq(port->irq, port);
	napi_disable(&port->napi);
	cancel_delayed_work_sync(&port->rx_owned_retry);
	phylink_disconnect_phy(port->phylink);
	if (last) {
		/* DMA may complete after NAPI is disabled. Reap only confirmed
		 * completions and retain polling until the shared ring is empty.
		 * Hardware-owned mappings remain live until completion or reset.
		 */
		spin_lock_bh(&ne->txq.lock);
		cs_ne_tx_complete(ne);
		cs_ne_tx_arm_timer(&ne->txq);
		spin_unlock_bh(&ne->txq.lock);
	}
	return 0;
}

static int cs_ne_set_mac_address(struct net_device *ndev, void *addr)
{
	struct cs_ne_port *port = netdev_priv(ndev);
	struct cs_ne *ne = port->ne;
	struct sockaddr *sa = addr;
	int ret, i;

	ret = eth_prepare_mac_addr_change(ndev, addr);
	if (ret)
		return ret;
	/* NI_MAC_ADDR0/1 are shared; only the last byte is per-port. */
	for (i = 0; i < CS_NE_PORTS; i++) {
		if (i == port->id || !(ne->address_ports & BIT(i)))
			continue;
		if (memcmp(sa->sa_data, ne->addresses[i], ETH_ALEN - 1))
			return -EOPNOTSUPP;
		if (ether_addr_equal(sa->sa_data, ne->addresses[i]))
			return -EADDRINUSE;
	}
	eth_commit_mac_addr_change(ndev, addr);
	ether_addr_copy(ne->addresses[port->id], ndev->dev_addr);
	cs_ne_set_hw_addr(port);
	return 0;
}

static int cs_ne_eth_ioctl(struct net_device *ndev, struct ifreq *ifr, int cmd)
{
	struct cs_ne_port *port = netdev_priv(ndev);

	return phylink_mii_ioctl(port->phylink, ifr, cmd);
}

static int cs_ne_get_link_ksettings(struct net_device *ndev,
				    struct ethtool_link_ksettings *cmd)
{
	struct cs_ne_port *port = netdev_priv(ndev);

	return phylink_ethtool_ksettings_get(port->phylink, cmd);
}

static int cs_ne_set_link_ksettings(struct net_device *ndev,
				    const struct ethtool_link_ksettings *cmd)
{
	struct cs_ne_port *port = netdev_priv(ndev);

	return phylink_ethtool_ksettings_set(port->phylink, cmd);
}

static const struct ethtool_ops cs_ne_ethtool_ops = {
	.get_link = ethtool_op_get_link,
	.get_link_ksettings = cs_ne_get_link_ksettings,
	.set_link_ksettings = cs_ne_set_link_ksettings,
};

static const struct net_device_ops cs_ne_netdev_ops = {
	.ndo_open = cs_ne_open,
	.ndo_stop = cs_ne_stop,
	.ndo_start_xmit = cs_ne_start_xmit,
	.ndo_set_mac_address = cs_ne_set_mac_address,
	.ndo_validate_addr = eth_validate_addr,
	.ndo_eth_ioctl = cs_ne_eth_ioctl,
};

static void cs_ne_port_cleanup(void *data)
{
	struct cs_ne_port *port = data;

	if (port->registered)
		unregister_netdev(port->ndev);
	cancel_delayed_work_sync(&port->rx_owned_retry);
	phylink_destroy(port->phylink);
	netif_napi_del(&port->napi);
	/* The shared completion poll can outlive a closed port. */
	spin_lock_bh(&port->ne->txq.lock);
	port->ne->ports[port->id] = NULL;
	spin_unlock_bh(&port->ne->txq.lock);
}

static int cs_ne_cache_addresses(struct cs_ne *ne)
{
	struct device_node *np;
	u8 base[ETH_ALEN];
	unsigned int present = 0, fixed = 0;
	u32 id;
	int ret, i, j;

	for_each_available_child_of_node(ne->dev->of_node, np) {
		if (!of_node_name_eq(np, "ethernet-port"))
			continue;
		ret = of_property_read_u32(np, "reg", &id);
		if (ret || id >= CS_NE_PORTS || (present & BIT(id))) {
			of_node_put(np);
			return -EINVAL;
		}
		present |= BIT(id);
		ret = of_get_mac_address(np, ne->addresses[id]);
		if (ret == -EPROBE_DEFER) {
			of_node_put(np);
			return ret;
		}
		if (!ret) {
			if (fixed && memcmp(base, ne->addresses[id], ETH_ALEN - 1)) {
				of_node_put(np);
				return dev_err_probe(ne->dev, -EINVAL,
						     "MACs must share their first five bytes\n");
			}
			ether_addr_copy(base, ne->addresses[id]);
			fixed |= BIT(id);
		}
	}
	if (!present)
		return -ENODEV;
	if (!fixed)
		eth_random_addr(base);
	for (i = 0; i < CS_NE_PORTS; i++) {
		if (!(present & BIT(i)))
			continue;
		if (!(fixed & BIT(i))) {
			ether_addr_copy(ne->addresses[i], base);
			/* At most three ports share the 256 available last bytes. */
			for (;;) {
				for (j = 0; j < CS_NE_PORTS; j++)
					if ((fixed & BIT(j)) &&
					    ether_addr_equal(ne->addresses[i], ne->addresses[j]))
						break;
				if (j == CS_NE_PORTS)
					break;
				ne->addresses[i][ETH_ALEN - 1]++;
			}
			ne->random_addresses |= BIT(i);
			fixed |= BIT(i);
		}
		for (j = 0; j < i; j++)
			if ((present & BIT(j)) &&
			    ether_addr_equal(ne->addresses[i], ne->addresses[j]))
				return -EADDRINUSE;
	}
	ne->address_ports = present;
	return 0;
}

static int cs_ne_probe_port(struct cs_ne *ne, struct device_node *np)
{
	struct device *dev = ne->dev;
	struct cs_ne_port *port;
	struct net_device *ndev;
	char irqname[8];
	u32 id;
	int ret;

	ret = of_property_read_u32(np, "reg", &id);
	if (ret || id >= CS_NE_PORTS || ne->ports[id]) {
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
		return ret;

	eth_hw_addr_set(ndev, ne->addresses[id]);
	if (ne->random_addresses & BIT(id))
		ndev->addr_assign_type = NET_ADDR_RANDOM;

	ndev->dev.of_node = np;
	ndev->netdev_ops = &cs_ne_netdev_ops;
	ndev->ethtool_ops = &cs_ne_ethtool_ops;
	ndev->hw_features = NETIF_F_SG;
	ndev->features = NETIF_F_SG;
	ndev->vlan_features = NETIF_F_SG;
	ndev->min_mtu = ETH_MIN_MTU;
	ndev->max_mtu = CS_NE_MAX_PKT_LEN - ETH_HLEN - ETH_FCS_LEN - 16;
	INIT_DELAYED_WORK(&port->rx_owned_retry, cs_ne_rx_owned_retry);
	netif_napi_add(ndev, &port->napi, cs_ne_rx_poll);

	port->phylink_config.dev = &ndev->dev;
	port->phylink_config.type = PHYLINK_NETDEV;
	port->phylink_config.mac_capabilities = MAC_SYM_PAUSE | MAC_ASYM_PAUSE |
		MAC_10 | MAC_100 | MAC_1000FD;
	phy_interface_set_rgmii(port->phylink_config.supported_interfaces);
	port->phylink = phylink_create(&port->phylink_config,
				       of_fwnode_handle(np), port->interface,
				       &cs_ne_phylink_ops);
	if (IS_ERR(port->phylink)) {
		ret = PTR_ERR(port->phylink);
		netif_napi_del(&port->napi);
		return ret;
	}
	ret = devm_add_action_or_reset(dev, cs_ne_port_cleanup, port);
	if (ret)
		return ret;

	cs_ne_port_hw_init(port);
	cs_ne_set_hw_addr(port);
	spin_lock_bh(&ne->txq.lock);
	ne->ports[id] = port;
	spin_unlock_bh(&ne->txq.lock);

	if (cs_ne_rx_refill(port, cs_ne_rx_pool_size[id]) != cs_ne_rx_pool_size[id])
		return -ENOMEM;

	return 0;
}

/*
 * On teardown, keep each block in reset even if another assertion fails.
 * Continue through the DMA and XRAM controls to stop as much of the
 * datapath as possible before leaving any live mappings pinned.
 */
static int cs_ne_assert_resets(struct cs_ne *ne)
{
	int first = 0, ret, i;

	for (i = 0; i < CS_NE_NUM_RESETS; i++) {
		ret = reset_control_assert(ne->resets[i].rstc);
		if (ret && !first)
			first = ret;
	}
	return first;
}

/* Last devres action: on a failed reset retain mappings and their device. */
static void cs_ne_context_release(void *data)
{
	struct cs_ne *ne = data;

	if (ne->dma_pinned) {
		get_device(ne->dev);
		dev_err(ne->dev, "DMA reset failed; allocations retained until reboot\n");
		return;
	}
	kfree(ne);
}

/* All ports/IRQs/NAPI have been removed before this action runs. */
static void cs_ne_hw_cleanup(void *data)
{
	struct cs_ne *ne = data;
	struct cs_ne_txq *txq = &ne->txq;
	struct cs_ne_tx_buf *buf;
	int ret, i;

	if (ne->hw_cleanup_done)
		return;
	ne->hw_cleanup_done = true;
	cs_ne_rx_cancel_retry(ne);
	cs_ne_tx_cancel_timer(txq);
	if (ne->hw_accessible) {
		ni_rmw(ne, NI_CPUXRAM_CFG, NI_CPUXRAM_PKT_DIS_MASK,
		       NI_CPUXRAM_PKT_DIS_MASK);
		writel(0, ne->dma + DMA_LSO_TXQ_CONTROL(0));
		writel(0, ne->dma + DMA_LSO_TXDMA_CONTROL);
		readl(ne->dma + DMA_LSO_TXDMA_CONTROL);
	}

	/* The entire owned NE + LSO/SSP domain stays reset through release. */
	ret = cs_ne_assert_resets(ne);
	if (ret && ne->hw_active) {
		ne->dma_pinned = true;
		return;
	}
	ne->dma_quiesced = !ret;
	ne->hw_active = false;
	cs_ne_rx_cleanup(ne);
	for (i = 0; i < CS_NE_TX_DESC_NUM; i++) {
		buf = &txq->bufs[i];
		if (buf->len) {
			if (buf->page)
				dma_unmap_page(ne->dev, buf->dma, buf->len, DMA_TO_DEVICE);
			else
				dma_unmap_single(ne->dev, buf->dma, buf->len, DMA_TO_DEVICE);
			buf->len = 0;
		}
		if (buf->skb) {
			dev_kfree_skb_any(buf->skb);
			buf->skb = NULL;
		}
	}
	if (txq->desc) {
		dma_free_coherent(ne->dev, CS_NE_TX_DESC_NUM * 32,
				  txq->desc, txq->desc_dma);
		txq->desc = NULL;
	}
}

static void cs_ne_remove(struct platform_device *pdev)
{
	struct cs_ne *ne = platform_get_drvdata(pdev);
	int i;

	if (!ne || ne->hw_cleanup_done)
		return;
	for (i = 0; i < CS_NE_PORTS; i++)
		if (ne->ports[i])
			devm_release_action(ne->dev, cs_ne_port_cleanup, ne->ports[i]);
	devm_release_action(ne->dev, cs_ne_hw_cleanup, ne);
	platform_set_drvdata(pdev, NULL);
}

static void cs_ne_shutdown(struct platform_device *pdev)
{
	cs_ne_remove(pdev);
}

static void __iomem *cs_ne_iomap(struct platform_device *pdev, const char *name,
				 size_t minimum_size)
{
	struct resource *res;

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, name);
	if (!res || resource_size(res) < minimum_size)
		return ERR_PTR(-EINVAL);
	return devm_ioremap_resource(&pdev->dev, res);
}

static int cs_ne_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *np;
	struct cs_ne *ne;
	int ret, i, nports = 0;

	ne = kzalloc(sizeof(*ne), GFP_KERNEL);
	if (!ne)
		return -ENOMEM;
	ne->dev = dev;
	spin_lock_init(&ne->ni_lock);
	ret = devm_add_action_or_reset(dev, cs_ne_context_release, ne);
	if (ret)
		return ret;
	xa_init(&ne->rx_buffers);
	INIT_LIST_HEAD(&ne->rx_pending);
	spin_lock_init(&ne->rx_lock);
	INIT_DELAYED_WORK(&ne->rx_retry, cs_ne_rx_retry);
	/* Keep all allocated buffers CPU-owned until probe commits. */
	ne->rx_stopping = true;
	ne->txq.ne = ne;
	ne->txq.stopping = true;
	spin_lock_init(&ne->txq.lock);
	hrtimer_setup(&ne->txq.timer, cs_ne_tx_timer, CLOCK_MONOTONIC,
		      HRTIMER_MODE_REL_SOFT);
	platform_set_drvdata(pdev, ne);

	ne->global = syscon_regmap_lookup_by_phandle(dev->of_node, "cortina,global-syscon");
	if (IS_ERR(ne->global))
		return dev_err_probe(dev, PTR_ERR(ne->global), "global syscon\n");
	ret = cs_ne_check_reserved_memory(ne);
	if (ret)
		return ret;
	ne->ni = cs_ne_iomap(pdev, "ni", 0x500);
	ne->dma = cs_ne_iomap(pdev, "dma", 0x800);
	ne->fe = cs_ne_iomap(pdev, "fe", 0x4000);
	ne->qm = cs_ne_iomap(pdev, "qm", 0x400);
	ne->tm = cs_ne_iomap(pdev, "tm", 0x800);
	ne->sch = cs_ne_iomap(pdev, "sch", 0x100);
	ne->xram = cs_ne_iomap(pdev, "xram", 0x100000);
	if (IS_ERR(ne->ni))
		return dev_err_probe(dev, PTR_ERR(ne->ni), "NI resource\n");
	if (IS_ERR(ne->dma))
		return dev_err_probe(dev, PTR_ERR(ne->dma), "complete LSO/SSP DMA resource\n");
	if (IS_ERR(ne->fe))
		return dev_err_probe(dev, PTR_ERR(ne->fe), "FE resource\n");
	if (IS_ERR(ne->qm))
		return dev_err_probe(dev, PTR_ERR(ne->qm), "QM resource\n");
	if (IS_ERR(ne->tm))
		return dev_err_probe(dev, PTR_ERR(ne->tm), "TM resource\n");
	if (IS_ERR(ne->sch))
		return dev_err_probe(dev, PTR_ERR(ne->sch), "SCH resource\n");
	if (IS_ERR(ne->xram))
		return dev_err_probe(dev, PTR_ERR(ne->xram), "XRAM resource\n");

	for (i = 0; i < CS_NE_NUM_RESETS; i++)
		ne->resets[i].id = cs_ne_reset_names[i];
	ret = devm_reset_control_bulk_get_exclusive(dev, CS_NE_NUM_RESETS, ne->resets);
	if (ret)
		return dev_err_probe(dev, ret, "exclusive engine resets\n");
	ret = dma_set_mask_and_coherent(dev, DMA_BIT_MASK(32));
	if (ret)
		return ret;

	ret = cs_ne_cache_addresses(ne);
	if (ret)
		return ret;

	/* No hardware state needs unwinding until the first reset attempt. */
	ret = devm_add_action(dev, cs_ne_hw_cleanup, ne);
	if (ret)
		return ret;

	ret = cs_ne_assert_resets(ne);
	if (ret)
		return dev_err_probe(dev, ret, "assert engine reset\n");
	/* Reset pulse width, not an inferred DMA idle condition. */
	usleep_range(100, 200);
	ret = reset_control_bulk_deassert(CS_NE_NUM_RESETS, ne->resets);
	if (ret)
		return dev_err_probe(dev, ret, "release engine reset\n");
	ne->hw_accessible = true;
	ret = cs_ne_ni_init(ne);
	if (ret)
		return ret;
	ret = cs_ne_dma_init(ne);
	if (ret)
		return ret;

	ret = cs_ne_qm_init(ne);
	if (ret)
		return ret;
	ret = cs752x_sch_init(dev, ne->sch);
	if (ret)
		return dev_err_probe(dev, ret, "SCH init failed\n");
	ret = cs752x_tm_init(dev, ne->tm);
	if (ret)
		return dev_err_probe(dev, ret, "TM init failed\n");
	ret = cs752x_fe_init(dev, ne->fe);
	if (ret)
		return dev_err_probe(dev, ret, "FE init failed\n");
	writel(readl(ne->fe + FETOP_FE_PRSR_CFG_0) & ~FE_L4_CHKSUM_CHK_DISABLE,
	       ne->fe + FETOP_FE_PRSR_CFG_0);

	/* phylink_create() and destroy() may acquire RTNL themselves. */
	for_each_available_child_of_node(dev->of_node, np) {
		if (!of_node_name_eq(np, "ethernet-port"))
			continue;
		ret = cs_ne_probe_port(ne, np);
		if (ret) {
			dev_err(dev, "%pOF: probe failed: %d\n", np, ret);
			of_node_put(np);
			return ret;
		}
		nports++;
	}
	if (!nports)
		return -ENODEV;

	/* No netdev may open while the shared pools are only partly prepared. */
	rtnl_lock();
	for (i = 0; i < CS_NE_PORTS; i++) {
		if (!ne->ports[i])
			continue;
		ret = register_netdevice(ne->ports[i]->ndev);
		if (ret)
			goto unlock;
		ne->ports[i]->registered = true;
	}

	/* Cleanup must revoke hardware references before releasing any mappings. */
	ne->hw_active = true;
	/* There are no fallible probe operations after publishing DMA addresses. */
	spin_lock_irq(&ne->rx_lock);
	ne->rx_stopping = false;
	spin_unlock_irq(&ne->rx_lock);
	cs_ne_rx_kick(ne);
	writel(DMA_ENABLE | FIELD_PREP(DMA_BURST_LEN, 3),
	       ne->dma + DMA_LSO_TXDMA_CONTROL);
	writel(1, ne->dma + DMA_LSO_TXQ_CONTROL(0));
	ret = 0;
unlock:
	rtnl_unlock();
	return ret;
}

static const struct of_device_id cs_ne_of_match[] = {
	{ .compatible = "cortina,cs7542-ethernet" },
	{ }
};
MODULE_DEVICE_TABLE(of, cs_ne_of_match);

static struct platform_driver cs_ne_driver = {
	.probe = cs_ne_probe,
	.remove = cs_ne_remove,
	.shutdown = cs_ne_shutdown,
	.driver = {
		.name = "cs7542-ethernet",
		.of_match_table = cs_ne_of_match,
	},
};
module_platform_driver(cs_ne_driver);

MODULE_DESCRIPTION("Cortina CS7542 network engine driver");
MODULE_LICENSE("GPL");
