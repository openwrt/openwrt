/*******************************************************************************

  Intel SmartPHY DSL PCIe Endpoint/ACA Linux Test driver
  Copyright(c) 2016 Intel Corporation.

  This program is free software; you can redistribute it and/or modify it
  under the terms and conditions of the GNU General Public License,
  version 2, as published by the Free Software Foundation.

  This program is distributed in the hope it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.

  The full GNU General Public License is included in this distribution in
  the file called "COPYING".

*******************************************************************************/

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/delay.h>

#include <net/dc_ep.h>

#include "ep_test.h"

#define DRV_VERSION "1.0.0"
static const char ep_test_driver_version[] = DRV_VERSION;
static struct dc_ep_dev pcie_dev[DC_EP_MAX_PEER + 1];
static int ppe_irq_num;

#define ep_wr32(value, reg)	(writel(value, dev->membase + reg))
#define ep_rd32(reg)		(readl(dev->membase + reg))

#define ep_wr32_mask(clr, set, reg)		\
	ep_wr32(((ep_rd32(reg) & ~(clr)) | (set)), (reg))

struct aca_hd_desc {
	void *base;
	dma_addr_t phy_base;
	size_t size;/* in bytes */
};

struct aca_hd_desc_cfg {
	struct aca_hd_desc txin;
	struct aca_hd_desc txout;
	struct aca_hd_desc rxout;
};

static struct aca_hd_desc_cfg aca_soc_hd_desc[DC_EP_MAX_PEER + 1];

static void ep_mem_write(u8 __iomem *dst, const void *src, size_t len)
{
	int i;
	const u32 *src_addr = src;

	if (len % 4)
		pr_info("Warning!!: Copy len is not multiple of 4\n");

	len = len >> 2;

	for (i = 0; i < len; i++)
		writel(src_addr[i], (dst + (i << 2)));
}

static irqreturn_t dc_ep_ppe_intr(int irq, void *dev_id)
{
	struct dc_ep_dev *dev = dev_id;

	ppe_irq_num++;
	if (ep_rd32(MBOX_IGU0_ISR) == 0) {
		pr_err("Fatal error, dummy interrupt\n");
		return IRQ_NONE;
	}

	ep_wr32(PPE_MBOX_TEST_BIT, MBOX_IGU0_ISRC);
	ep_rd32(MBOX_IGU0_ISR);
	return IRQ_HANDLED;
}

static void dc_ep_ppe_mbox_reg_dump(struct dc_ep_dev *dev)
{
	pr_info("MBOX_IGU0_ISRS addr %p data 0x%08x\n",
		dev->membase + MBOX_IGU0_ISRS,
		ep_rd32(MBOX_IGU0_ISRS));
	pr_info("MBOX_IGU0_ISRC addr %p data 0x%08x\n",
		dev->membase + MBOX_IGU0_ISRC,
		ep_rd32(MBOX_IGU0_ISRC));
	pr_info("MBOX_IGU0_ISR  addr %p data 0x%08x\n",
		dev->membase + MBOX_IGU0_ISR,
		ep_rd32(MBOX_IGU0_ISR));
	pr_info("MBOX_IGU0_IER  addr %p data 0x%08x\n",
		dev->membase + MBOX_IGU0_IER,
		ep_rd32(MBOX_IGU0_IER));
}

#define PPE_INT_TIMEOUT		10
static int dc_ep_ppe_mbox_int_stress_test(struct dc_ep_dev *dev)
{
	int i;
	int j;
	int ret;

	/* Clear it first */
	ep_wr32(PPE_MBOX_TEST_BIT, MBOX_IGU0_ISRC);

	ret = request_irq(dev->irq, dc_ep_ppe_intr, 0, "PPE_MSI", dev);
	if (ret) {
		pr_err("%s request irq %d failed\n", __func__, dev->irq);
		return -1;
	}
	pr_info("PPE test\n");
	ep_wr32(PPE_MBOX_TEST_BIT, MBOX_IGU0_IER);
	ppe_irq_num = 0;
	/* Purposely trigger interrupt */
	for (i = 0; i < PPE_MBOX_IRQ_TEST_NUM; i++) {
		j = 0;
		while ((ep_rd32(MBOX_IGU0_ISR) & PPE_MBOX_TEST_BIT)) {
			j++;
			if (j > PPE_INT_TIMEOUT)
				break;
		}
		ep_wr32(PPE_MBOX_TEST_BIT, MBOX_IGU0_ISRS);
		/* Write flush */
		ep_rd32(MBOX_IGU0_ISR);
	}
	mdelay(10);
	pr_info("irq triggered %d expected %d\n", ppe_irq_num,
		PPE_MBOX_IRQ_TEST_NUM);
	dc_ep_ppe_mbox_reg_dump(dev);
	ppe_irq_num = 0;
	return 0;
}

static void umt_txin_send(struct dc_ep_dev *dev,
	u8 __iomem *soc_dbase, int num)
{
	int i;
	struct aca_dma_desc desc;

	memset(&desc, 0, sizeof(desc));
	desc.own = 0;
	desc.sop = 1;
	desc.eop = 1;
	desc.dic = 1;
	desc.pdu_type = 1;
	desc.data_len = 127;
	desc.data_pointer = 0x26000000;
	desc.dw1 = 0x700;
	desc.dw0 = 0x0000007f;

	for (i = 0; i < num; i++) {
		desc.data_pointer += roundup(desc.data_len, 4);
		ep_mem_write(soc_dbase + i * sizeof(desc),
			(void *)&desc, sizeof(desc));
	}

	ep_wr32(num, TXIN_HD_ACCUM_ADD);
}

static void ppe_txout_send(struct dc_ep_dev *dev,
	u8 __iomem *ppe_sb_base, int num)
{
	int i;
	struct aca_dma_desc_2dw desc;

	memset(&desc, 0, sizeof(desc));
	desc.status.field.own = 1;
	desc.status.field.sop = 1;
	desc.status.field.eop = 1;
	desc.status.field.data_len = 127;
	desc.data_pointer = 0x26100000;

	for (i = 0; i < num; i++) {
		desc.data_pointer += roundup(desc.status.field.data_len, 4);
		ep_mem_write(ppe_sb_base + i * sizeof(desc),
			(void *)&desc, sizeof(desc));
	}

	ep_wr32(num, TXOUT_ACA_ACCUM_ADD);
}

static void ppe_rxout_send(struct dc_ep_dev *dev,
	u8 __iomem *ppe_sb_base, int num)
{
	int i;
	struct aca_dma_desc_2dw desc;

	memset(&desc, 0, sizeof(desc));
	desc.status.field.own = 0;
	desc.status.field.sop = 1;
	desc.status.field.eop = 1;
	desc.status.field.meta_data0 = 0x3;
	desc.status.field.meta_data1 = 0x7f;
	desc.status.field.data_len = 127;
	desc.data_pointer = 0x26200000;

	for (i = 0; i < num; i++) {
		desc.data_pointer += roundup(desc.status.field.data_len, 4);
		ep_mem_write(ppe_sb_base + i * sizeof(desc),
			(void *)&desc, sizeof(desc));
	}

	ep_wr32(num, RXOUT_ACA_ACCUM_ADD);
}

static void dc_aca_test_init(struct dc_ep_dev *dev, void *soc_base)
{
	umt_txin_send(dev, (u8 __iomem *)soc_base, 8);
	ppe_txout_send(dev, (TXOUT_PD_DBASE + dev->membase), 8);
	ppe_rxout_send(dev, (RXOUT_PD_DBASE + dev->membase), 8);
}

static const char *sysclk_str[SYS_CLK_MAX] = {
	"36MHz",
	"288MHz",
};

static const char *ppeclk_str[PPE_CLK_MAX] = {
	"36MHz",
	"576MHz",
	"494MHz",
	"432MHz",
	"288MHz",
};

#define ACA_PMU_CTRL		0x11C
#define ACA_PMU_DMA		BIT(2)
#define ACA_PMU_EMA		BIT(22)

enum {
	DMA_ENDIAN_TYPE0 = 0,
	DMA_ENDIAN_TYPE1,	/*!< Byte Swap(B0B1B2B3 => B1B0B3B2) */
	DMA_ENDIAN_TYPE2,	/*!< Word Swap (B0B1B2B3 => B2B3B0B1) */
	DMA_ENDIAN_TYPE3,	/*!< DWord Swap (B0B1B2B3 => B3B2B1B0) */
	DMA_ENDIAN_MAX,
};

#ifdef CONFIG_CPU_BIG_ENDIAN
#define DMA_ENDIAN_DEFAULT	DMA_ENDIAN_TYPE3
#else
#define DMA_ENDIAN_DEFAULT	DMA_ENDIAN_TYPE0
#endif

enum {
	DMA_BURSTL_2DW = 1,	/*!< 2 DWORD DMA burst length */
	DMA_BURSTL_4DW = 2,	/*!< 4 DWORD DMA burst length */
	DMA_BURSTL_8DW = 3,	/*!< 8 DWORD DMA burst length */
	DMA_BURSTL_16DW = 16,
};

#define DMA_BURSTL_DEFAULT	DMA_BURSTL_16DW

#define DMA_TX_PORT_DEFAULT_WEIGHT	1
/** Default Port Transmit weight value */
#define DMA_TX_CHAN_DEFAULT_WEIGHT	1

enum {
	DMA_RX_CH = 0,  /*!< Rx channel */
	DMA_TX_CH = 1,  /*!< Tx channel */
};

enum {
	DMA_PKT_DROP_DISABLE = 0,
	DMA_PKT_DROP_ENABLE,
};

#ifdef CONFIG_CPU_BIG_ENDIAN
/* 2 DWs format descriptor */
struct rx_desc_2dw {
	u32 data_pointer; /* Descriptor data pointer */
	union {
		struct {
			u32 own:1;
			u32 c:1;
			u32 sop:1;
			u32 eop:1;
			u32 meta:2;
			u32 byte_offset:3;
			u32 meta_data:7;
			u32 data_len:16;
		} __packed field;
		u32 word;
	} __packed status;
} __packed __aligned(8);

struct tx_desc_2dw {
	u32 data_pointer; /* Descriptor data pointer */
	union {
		struct {
			u32 own:1;
			u32 c:1;
			u32 sop:1;
			u32 eop:1;
			u32 meta:2;
			u32 byte_offset:3;
			u32 meta_data:7;
			u32 data_len:16;
		} __packed field;
		u32 word;
	} __packed status;
} __packed __aligned(8);
#else
/* 2 DWs format descriptor */
struct rx_desc_2dw {
	u32 data_pointer; /* Descriptor data pointer */
	union {
		struct {
			u32 data_len:16;
			u32 meta_data:7;
			u32 byte_offset:3;
			u32 meta:2;
			u32 eop:1;
			u32 sop:1;
			u32 c:1;
			u32 own:1;
		} __packed field;
		u32 word;
	} __packed status;
} __packed __aligned(8);

struct tx_desc_2dw {
	u32 data_pointer; /* Descriptor data pointer */
	union {
		struct {
			u32 data_len:16;
			u32 meta_data:7;
			u32 byte_offset:3;
			u32 meta:2;
			u32 eop:1;
			u32 sop:1;
			u32 c:1;
			u32 own:1;
		} __packed field;
		u32 word;
	} __packed status;
} __packed __aligned(8);
#endif

enum {
	SOC_TO_EP = 0,
	EP_TO_SOC,
};

static int dma_pkt_size = 1024;
static int dma_mode = SOC_TO_EP;
static int dma_burst = 16;
static int desc_num = 32;

module_param(dma_pkt_size, int, 0);
MODULE_PARM_DESC(dma_pkt_size, "Single packet length");

module_param(dma_mode, int, 0);
MODULE_PARM_DESC(dma_mode, "mode 0 -- Soc->EP, mode 1-- EP->SoC");


static void dma_ctrl_rst(struct dc_ep_dev *dev)
{
	ep_wr32_mask(ACA_PMU_DMA | ACA_PMU_EMA, 0, ACA_PMU_CTRL);

	udelay(10);
	ep_wr32_mask(0, 1, DMA_CTRL);
	udelay(10);
	ep_wr32(0, DMA_CLC);
}

static void dma_chan_rst(struct dc_ep_dev *dev, int cn)
{
	ep_wr32(cn, DMA_CS);
	ep_wr32(0x2, DMA_CCTRL);
	while (ep_rd32(DMA_CCTRL) & 0x01)
		udelay(10);
}

static void dma_port_cfg(struct dc_ep_dev *dev)
{
	u32 reg = 0;

	reg |= (DMA_TX_PORT_DEFAULT_WEIGHT << 12);
	reg |= (DMA_ENDIAN_TYPE0 << 10);
	reg |= (DMA_ENDIAN_TYPE0 << 8);
	reg |= (DMA_PKT_DROP_DISABLE << 6);
	reg |= 0x3;
	ep_wr32(0, DMA_PS);
	ep_wr32(reg, DMA_PCTRL);
}

static void dma_byte_enable(struct dc_ep_dev *dev, int enable)
{
	if (enable)
		ep_wr32_mask(0, BIT(9), DMA_CTRL);
	else
		ep_wr32_mask(BIT(9), 0, DMA_CTRL);
}

static void dma_tx_ch_cfg(struct dc_ep_dev *dev, int ch, u32 desc_base,
	u32 desc_phys, dma_addr_t data_base, int desc_num)
{
	int i;
	struct tx_desc_2dw *tx_desc;

	for (i = 0; i < desc_num; i++) {
		tx_desc = (struct tx_desc_2dw *)
			(desc_base + (i * sizeof(*tx_desc)));
		tx_desc->data_pointer = (((u32)(data_base +
			(i * dma_pkt_size))) & 0xfffffff8);
		tx_desc->status.word = 0;
		tx_desc->status.field.byte_offset = 0;
		tx_desc->status.field.data_len = dma_pkt_size;

		tx_desc->status.field.sop = 1;
		tx_desc->status.field.eop = 1;
		tx_desc->status.field.own = 1;
		wmb();
	#if 0
		pr_info("Tx desc num %d word 0x%08x data pointer 0x%08x\n",
			i, tx_desc->status.word, tx_desc->data_pointer);
	#endif
	}
	ep_wr32(ch, DMA_CS);
	ep_wr32(desc_phys, DMA_CDBA);
	ep_wr32(desc_num, DMA_CDLEN);
	ep_wr32(0, DMA_CIE);
}

static void dma_rx_ch_cfg(struct dc_ep_dev *dev, int ch, u32 desc_base,
	u32 desc_phys, dma_addr_t data_base, int desc_num)
{
	int i;
	struct rx_desc_2dw *rx_desc;

	for (i = 0; i < desc_num; i++) {
		rx_desc = (struct rx_desc_2dw *)(desc_base
			+ (i * sizeof(*rx_desc)));
		rx_desc->data_pointer = (((u32)(data_base +
			(i * dma_pkt_size))) & 0xfffffff8);

		rx_desc->status.word = 0;
		rx_desc->status.field.sop = 1;
		rx_desc->status.field.eop = 1;
		rx_desc->status.field.byte_offset = 0;
		rx_desc->status.field.data_len = dma_pkt_size;
		rx_desc->status.field.own = 1; /* DMA own the descriptor */
		wmb();
	#if 0
		pr_info("Rx desc num %d word 0x%08x data pointer 0x%08x\n",
			i, rx_desc->status.word, rx_desc->data_pointer);
	#endif
	}

	ep_wr32(ch, DMA_CS);
	ep_wr32(desc_phys, DMA_CDBA);
	ep_wr32(desc_num, DMA_CDLEN);
	ep_wr32(0, DMA_CIE);
}

static void dma_chan_on(struct dc_ep_dev *dev, u8 cn)
{
	ep_wr32(cn, DMA_CS);
	ep_wr32_mask(0, BIT(0), DMA_CCTRL);
}

static void dma_chan_off(struct dc_ep_dev *dev, u8 cn)
{
	ep_wr32(cn, DMA_CS);
	ep_wr32_mask(BIT(0), 0,  DMA_CCTRL);
	udelay(10);
}

#define DEFAULT_TEST_PATTEN	0x12345678

#define REG32(addr)		(*((volatile u32*)(addr)))

#ifdef CONFIG_CPU_BIG_ENDIAN
#define ___swab32(x) ((u32)(				\
	(((u32)(x) & (u32)0x000000ffUL) << 24) |	\
	(((u32)(x) & (u32)0x0000ff00UL) <<  8) |	\
	(((u32)(x) & (u32)0x00ff0000UL) >>  8) |	\
	(((u32)(x) & (u32)0xff000000UL) >> 24)))
#else
#define ___swab32(x)		(x)
#endif

static void dma_sdram_preload(void *sdram_data_tx_ptr, void *sdram_data_rx_ptr)
{
	int i;
	int j;

	u32 testaddr = (u32)sdram_data_tx_ptr;

	for (i = 0; i < desc_num; i++) {
		for (j = 0; j < dma_pkt_size; j = j + 4) {
			REG32(testaddr + i * dma_pkt_size + j)
				= DEFAULT_TEST_PATTEN;
		}
	}
	pr_info("SDR Preload(0x55aa00ff) with data on TX location done\n");

	testaddr = (u32)sdram_data_rx_ptr;
	pr_info("RX Preload start address:0x%08x\n", (u32)(testaddr));

	for (i = 0; i < desc_num; i++) {
		for (j = 0; j < roundup(dma_pkt_size,
			dma_burst << 2); j = j + 4)
			REG32(testaddr + i * dma_pkt_size + j) = 0xcccccccc;
	}
	pr_info("SDR locations for Memcopy RX preset to 0xcccccccc done\n");
}

static void memcopy_data_check(u32 rx_data_addr)
{
	int i, j;
	u32 read_data;

	for (i = 0; i < desc_num; i++) {
		for (j = 0; j < dma_pkt_size; j = j + 4) {
			read_data = REG32(rx_data_addr + i * dma_pkt_size + j);
			if (read_data != ___swab32(DEFAULT_TEST_PATTEN))
				pr_info("Memcopy ERROR at addr 0x%08x data 0x%08x\n",
				(rx_data_addr + j), read_data);
		}
	}
}

static u32 plat_throughput_calc(u32 payload, int cycles)
{
	return (u32)((payload * 300) / cycles);
}

#define DMA_CPOLL_CNT_MASK 0xFFF0u

static void dma_ctrl_global_polling_enable(struct dc_ep_dev *dev, int interval)
{
	u32 reg = 0;

	reg |= (1 << 31);
	reg |= (interval << 4);

	ep_wr32_mask(DMA_CPOLL_CNT_MASK,
		reg,  DMA_CPOLL);
}

static void dma_controller_cfg(struct dc_ep_dev *dev)
{
	ep_wr32_mask(0, BIT(31), DMA_CTRL);
	ep_wr32_mask(BIT(30), 0, DMA_CTRL);
	ep_wr32_mask(0, BIT(1), DMA_CTRL);
	ep_wr32_mask(0, BIT(13), DMA_CTRL);
}

#define PDRAM_OFFSET		0x200200
#define PDRAM_TX_DESC_OFFSET	0x200000
#define PDRAM_RX_DESC_OFFSET	0x200100
#define ACA_SRAM_OFFSET		0x100000
#define PPE_SB_TX_DESC_OFFSET	0x280000
#define PPE_SB_RX_DESC_OFFSET	0x281000

#define PPE_FPI_TX_DESC_OFFSET	0x320000
#define PPE_FPI_RX_DESC_OFFSET	0x321000

static void dma_test(struct dc_ep_dev *dev, int mode, int rcn, int tcn)
{
	u32 loop = 0;
	void *tx_data;
	void *rx_data;
	dma_addr_t tx_data_phys = 0;
	dma_addr_t rx_data_phys = 0;
	u32 start, end;
	u32 cycles;
	struct rx_desc_2dw *rx_desc;
	struct tx_desc_2dw *tx_desc;
	struct tx_desc_2dw *last_tx_desc;
	struct rx_desc_2dw *last_rx_desc;
	dma_addr_t tx_desc_phys;
	dma_addr_t rx_desc_phys;
	u32 membase = (u32)(dev->membase);

	rx_desc = (struct rx_desc_2dw *)(membase + PDRAM_RX_DESC_OFFSET);
	rx_desc_phys = (dev->phy_membase + PDRAM_RX_DESC_OFFSET);
	tx_desc = (struct tx_desc_2dw *)(membase + PDRAM_TX_DESC_OFFSET);
	tx_desc_phys = (dev->phy_membase + PDRAM_TX_DESC_OFFSET);
	last_rx_desc = rx_desc + (desc_num - 1);
	last_tx_desc = tx_desc + (desc_num - 1);

	if (mode == SOC_TO_EP) { /* Read from SoC DDR to local PDBRAM  */
		tx_data = dma_alloc_coherent(NULL,
			desc_num * dma_pkt_size, &tx_data_phys, GFP_DMA);
		rx_data_phys = (dma_addr_t)(dev->phy_membase + PDRAM_OFFSET);
		rx_data = (void *)(membase + PDRAM_OFFSET);
	} else { /* Write from local PDBRAM to remote DDR */
		tx_data_phys = (dma_addr_t)(dev->phy_membase + PDRAM_OFFSET);
		tx_data = (void *)(membase + PDRAM_OFFSET);
		rx_data = dma_alloc_coherent(NULL, desc_num * dma_pkt_size,
			 &rx_data_phys, GFP_DMA);
	}

	pr_info("tx_desc_base %p tx_desc_phys 0x%08x tx_data %p tx_data_phys 0x%08x\n",
		tx_desc, (u32)tx_desc_phys, tx_data, (u32)tx_data_phys);

	pr_info("rx_desc_base %p rx_desc_phys 0x%08x rx_data %p rx_data_phys 0x%08x\n",
		rx_desc, (u32)rx_desc_phys, rx_data, (u32)rx_data_phys);

	pr_info("dma burst %d desc number %d packet size %d\n",
		dma_burst, desc_num, dma_pkt_size);

	dma_ctrl_rst(dev);
	dma_chan_rst(dev, rcn);
	dma_chan_rst(dev, tcn);
	dma_port_cfg(dev);
	dma_controller_cfg(dev);
	dma_byte_enable(dev, 1);

	dma_ctrl_global_polling_enable(dev, 24);

	dma_sdram_preload(tx_data, rx_data);

	dma_tx_ch_cfg(dev, tcn, (u32)tx_desc, tx_desc_phys,
		tx_data_phys, desc_num);
	dma_rx_ch_cfg(dev, rcn, (u32)rx_desc, rx_desc_phys,
		rx_data_phys, desc_num);

	udelay(5); /* Make sure that RX descriptor prefetched */

	start = get_cycles();
	dma_chan_on(dev, rcn);
	dma_chan_on(dev, tcn);

	/* wait till tx chan desc own is 0 */
	while (last_tx_desc->status.field.own == 1) {
		loop++;
		udelay(1);
	}
	end = get_cycles();
	cycles = end - start;
	pr_info("cylces %d throughput %dMb\n", cycles,
		plat_throughput_calc(desc_num * dma_pkt_size * 8, cycles));
	pr_info("loop times %d\n", loop);
	while (last_rx_desc->status.field.own == 1) {
		loop++;
		udelay(1);
	}

	memcopy_data_check((u32)rx_data);
	dma_chan_off(dev, rcn);
	dma_chan_off(dev, tcn);
	if (mode == SOC_TO_EP) {
		dma_free_coherent(NULL, desc_num * dma_pkt_size,
			tx_data, tx_data_phys);
	} else {
		dma_free_coherent(NULL, desc_num * dma_pkt_size,
			rx_data, rx_data_phys);
	}
}

static int aca_soc_desc_alloc(int dev)
{
	dma_addr_t phy_addr;
	void *base;
	u32 size;

	if (dev < 0 || dev > (DC_EP_MAX_PEER + 1))
		return -EINVAL;

	/* TXIN */
	size = TXIN_SOC_DES_NUM * TXIN_HD_DES_SIZE * 4;
	base  = dma_alloc_coherent(NULL, size, &phy_addr, GFP_DMA);
	if (!base)
		goto txin;
	aca_soc_hd_desc[dev].txin.base = base;
	aca_soc_hd_desc[dev].txin.phy_base = phy_addr;
	aca_soc_hd_desc[dev].txin.size = size;
	pr_info("txin soc desc base %p phy 0x%08x size 0x%08x\n",
		base, (u32)phy_addr, size);

	/* TXOUT */
	size = TXOUT_SOC_DES_NUM * TXOUT_HD_DES_SIZE * 4;
	base  = dma_alloc_coherent(NULL, size, &phy_addr, GFP_DMA);
	if (!base)
		goto txout;
	aca_soc_hd_desc[dev].txout.base = base;
	aca_soc_hd_desc[dev].txout.phy_base = phy_addr;
	aca_soc_hd_desc[dev].txout.size = size;
	pr_info("txout soc desc base %p phy 0x%08x size 0x%08x\n",
		base, (u32)phy_addr, size);
	/* RXOUT */
	size = RXOUT_SOC_DES_NUM * RXOUT_HD_DES_SIZE * 4;
	base  = dma_alloc_coherent(NULL, size, &phy_addr, GFP_DMA);
	if (!base)
		goto rxout;
	aca_soc_hd_desc[dev].rxout.base = base;
	aca_soc_hd_desc[dev].rxout.phy_base = phy_addr;
	aca_soc_hd_desc[dev].rxout.size = size;
	pr_info("rxout soc desc base %p phy 0x%08x size 0x%08x\n",
		base, (u32)phy_addr, size);
	return 0;
rxout:
	dma_free_coherent(NULL, aca_soc_hd_desc[dev].txout.size,
		aca_soc_hd_desc[dev].txout.base,
		aca_soc_hd_desc[dev].txout.phy_base);
txout:
	dma_free_coherent(NULL, aca_soc_hd_desc[dev].txin.size,
		aca_soc_hd_desc[dev].txin.base,
		aca_soc_hd_desc[dev].txin.phy_base);
txin:
	return -ENOMEM;
}

static int aca_soc_desc_free(int dev)
{
	dma_addr_t phy_addr;
	void *base;
	size_t size;

	if (dev < 0 || dev > (DC_EP_MAX_PEER + 1))
		return -EINVAL;

	/* TXIN */
	base = aca_soc_hd_desc[dev].txin.base;
	phy_addr = aca_soc_hd_desc[dev].txin.phy_base;
	size = aca_soc_hd_desc[dev].txin.size;
	dma_free_coherent(NULL, size, base, phy_addr);

	/* TXOUT */
	base = aca_soc_hd_desc[dev].txout.base;
	phy_addr = aca_soc_hd_desc[dev].txout.phy_base;
	size = aca_soc_hd_desc[dev].txout.size;
	dma_free_coherent(NULL, size, base, phy_addr);

	/* RXOUT */
	base = aca_soc_hd_desc[dev].rxout.base;
	phy_addr = aca_soc_hd_desc[dev].rxout.phy_base;
	size = aca_soc_hd_desc[dev].rxout.size;
	dma_free_coherent(NULL, size, base, phy_addr);
	return 0;
}

static int __init dc_ep_test_init(void)
{
	int i, j;
	int dev_num;
	struct dc_ep_dev dev;
	int func = 0;
	u32 sysclk = 0;
	u32 ppeclk = 0;

	if (dc_ep_dev_num_get(&dev_num)) {
		pr_err("%s failed to get total device number\n", __func__);
		return -EIO;
	}

	pr_info("%s: total %d EPs found\n", __func__, dev_num);

	for (i = 0; i < dev_num; i++)
		aca_soc_desc_alloc(i);

	for (i = 0; i < dev_num; i++) {
		struct aca_param aca_cfg = {
			.aca_txin = {
				.soc_desc_base
					= aca_soc_hd_desc[i].txin.phy_base,
				.soc_desc_num = TXIN_SOC_DES_NUM,
				.pp_buf_desc_num = 32,
				.pd_desc_base = TXIN_PD_DBASE,
				.pd_desc_num = TXIN_PD_DES_NUM,
				.hd_size_in_dw = TXIN_HD_DES_SIZE,
				.pd_size_in_dw = TXIN_PD_DES_SIZE,
				.byteswap = 1,
			},
			.aca_txout = {
				.soc_desc_base
					= aca_soc_hd_desc[i].txout.phy_base,
				.soc_desc_num = TXOUT_SOC_DES_NUM,
				.pp_buf_desc_num = 32,
				.pd_desc_base = TXOUT_PD_DBASE,
				.pd_desc_num = TXOUT_PD_DES_NUM,
				.hd_size_in_dw = TXOUT_HD_DES_SIZE,
				.pd_size_in_dw = TXOUT_PD_DES_SIZE,
				.byteswap = 1,
			},
			.aca_rxout = {
				.soc_desc_base
					= aca_soc_hd_desc[i].rxout.phy_base,
				.soc_desc_num = RXOUT_SOC_DES_NUM,
				.pp_buf_desc_num = 32,
				.pd_desc_base = RXOUT_PD_DBASE,
				.pd_desc_num = RXOUT_PD_DES_NUM,
				.hd_size_in_dw = RXOUT_HD_DES_SIZE,
				.pd_size_in_dw = RXOUT_PD_DES_SIZE,
				.byteswap = 1,
			},
		};
		struct aca_modem_param modem_cfg = {
			.mdm_txout = {
				.stat = SB_XBAR_ADDR(__TX_OUT_ACA_ACCUM_STATUS),
				.pd      = SB_XBAR_ADDR(__TX_OUT_QUEUE_PD_BASE_ADDR_OFFSET),
				.acc_cnt = SB_XBAR_ADDR(__TX_OUT_ACA_ACCUM_COUNT),
			},
			.mdm_rxout = {
				.stat    = SB_XBAR_ADDR(__RX_OUT_ACA_ACCUM_STATUS),
				.pd      = SB_XBAR_ADDR(__RX_OUT_QUEUE_PD_BASE_ADDR_OFFSET),
				.acc_cnt = SB_XBAR_ADDR(__RX_OUT_ACA_ACCUM_COUNT),
			},
			.mdm_rxin = {
				.stat    = SB_XBAR_ADDR(__RX_IN_ACA_ACCUM_STATUS),
				.pd      = SB_XBAR_ADDR(__RX_IN_QUEUE_PD_BASE_ADDR_OFFSET),
				.acc_cnt = SB_XBAR_ADDR(__RX_IN_ACA_ACCUM_COUNT),
			},
		};
		if (dc_ep_dev_info_req(i, DC_EP_INT_PPE, &dev))
			pr_info("%s failed to get pcie ep %d information\n",
			__func__, i);
		pr_info("irq %d\n", dev.irq);
		pr_info("phyiscal membase 0x%08x virtual membase 0x%p\n",
			dev.phy_membase, dev.membase);
		if (dev_num > 1) {
			for (j = 0; j < dev.peer_num; j++) {
				pr_info("phyiscal peer membase 0x%08x virtual peer membase 0x%p\n",
					dev.peer_phy_membase[j], dev.peer_membase[j]);
			}
		}
		/* For module unload perpose */
		memcpy(&pcie_dev[i], &dev, sizeof(struct dc_ep_dev));
		dc_ep_ppe_mbox_int_stress_test(&pcie_dev[i]);
		dev.hw_ops->clk_on(&dev, PMU_CDMA | PMU_EMA | PMU_PPM2);
		dev.hw_ops->clk_set(&dev, SYS_CLK_288MHZ, PPE_CLK_576MHZ);
		dev.hw_ops->pinmux_set(&dev, 14, MUX_FUNC_ALT1);
		dev.hw_ops->pinmux_set(&dev, 15, MUX_FUNC_ALT2);
		dev.hw_ops->pinmux_get(&dev, 15, &func);
		pr_info("gpio 15 func %d\n", func);
		dev.hw_ops->pinmux_set(&dev, 13, MUX_FUNC_GPIO);
		dev.hw_ops->gpio_dir(&dev, 13, GPIO_DIR_OUT);
		dev.hw_ops->gpio_set(&dev, 13, 1);
		dev.hw_ops->gpio_get(&dev, 13, &func);
		pr_info("gpio 13 value %d\n", func);
		dev.hw_ops->gpio_pupd_set(&dev, 14, GPIO_PULL_DOWN);
		dev.hw_ops->gpio_od_set(&dev, 0, 1);
		dev.hw_ops->gpio_src_set(&dev, 0, GPIO_SLEW_RATE_FAST);
		dev.hw_ops->gpio_dcc_set(&dev, 0, GPIO_DRV_CUR_8MA);
		dev.hw_ops->clk_get(&dev, &sysclk, &ppeclk);
		pr_info("ppe clk %s sys clk %s\n", ppeclk_str[ppeclk],
			sysclk_str[sysclk]);
		dev.hw_ops->aca_init(&dev, &aca_cfg, &modem_cfg);
		dev.hw_ops->aca_start(&dev, ACA_ALL_EN, 1);

		pr_info("ACA test\n");
		dc_aca_test_init(&dev, aca_soc_hd_desc[i].txin.base);

		pr_info("DMA test\n");
		dma_pkt_size = 64;
		dma_test(&dev, dma_mode, 0, 1);
#if 0
		dma_pkt_size = 128;
		dma_test(&dev, dma_mode, 0, 1);
		dma_pkt_size = 256;
		dma_test(&dev, dma_mode, 0, 1);
		dma_pkt_size = 512;
		dma_test(&dev, dma_mode, 0, 1);
		dma_pkt_size = 1024;
		dma_test(&dev, dma_mode, 0, 1);
		dma_pkt_size = 2048;
		dma_test(&dev, dma_mode, 0, 1);

		dma_mode = EP_TO_SOC;
		dma_pkt_size = 64;
		dma_test(&dev, dma_mode, 0, 1);
		dma_pkt_size = 128;
		dma_test(&dev, dma_mode, 0, 1);
		dma_pkt_size = 256;
		dma_test(&dev, dma_mode, 0, 1);
		dma_pkt_size = 512;
		dma_test(&dev, dma_mode, 0, 1);
		dma_pkt_size = 1024;
		dma_test(&dev, dma_mode, 0, 1);
		dma_pkt_size = 2048;
		dma_test(&dev, dma_mode, 0, 1);
#endif
	}

	pr_info("Intel(R) SmartPHY DSL(VRX518) PCIe EP Test Driver - %s\n",
		ep_test_driver_version);
	return 0;
}

static void __exit dc_ep_test_exit(void)
{
	int i;
	int dev_num;
	u32 func = ACA_ALL_EN;
	struct dc_ep_dev *dev;

	if (dc_ep_dev_num_get(&dev_num)) {
		pr_err("%s failed to get total device number\n", __func__);
		return;
	}
	pr_info("%s: total %d EPs found\n", __func__, dev_num);
	for (i = 0; i < dev_num; i++) {
		dev = &pcie_dev[i];
		free_irq(dev->irq, dev);
		dev->hw_ops->aca_stop(dev, &func, 1);
		dev->hw_ops->clk_off(dev, PMU_EMA);
		if (dc_ep_dev_info_release(i)) {
			pr_info("%s failed to release pcie ep %d information\n",
				__func__, i);
		}
		aca_soc_desc_free(i);
	}
}

module_init(dc_ep_test_init);
module_exit(dc_ep_test_exit);

MODULE_AUTHOR("Intel Corporation, <Chuanhua.lei@intel.com>");
MODULE_DESCRIPTION("Intel(R) SmartPHY (VRX518) PCIe EP/ACA test driver");
MODULE_LICENSE("GPL");
