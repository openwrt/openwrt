/*******************************************************************************

  Intel SmartPHY DSL PCIe Endpoint/ACA Linux driver
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

#ifndef ACA_H
#define ACA_H

#define HOST_IF_BASE		0x50000
#define ACA_CORE_BASE		0x50800
#define GENRISC_IRAM_BASE	0x58000
#define GENRISC_SPRAM_BASE	0x5C000
#define GENRISC_BASE		0x5D000
#define MAC_HT_EXT_BASE		0x5D400
#define ACA_SRAM_BASE		0x100000
#define ACA_SRAM_SIZE		0x2000 /* Project specific */
#define ACA_HOSTIF_ADDR_SHIFT	2

#define ACA_HOSTIF_ADDR(addr)	((addr) >> ACA_HOSTIF_ADDR_SHIFT)

#define ACA_HIF_LOC_POS		0x100060
#define ACA_HIF_PARAM_ADDR	0x100064
#define ACA_ACC_FW_SIZE		0x400
#define ACA_LOOP_CNT		1000

/* TODO: change name after karthik explained */
#define TXIN_DST_OWNBIT		0xC4
#define TXOUT_DST_OWNBIT	0x1C4
#define RXOUT_SRC_OWNBIT	0x3C4
#define RXIN_DST_OWNBIT		0x2C4

/* Genrisc Internal Host Descriptor(Ping/Pong) decided by ACA fw header */
/* ACA Core */
#define ACA_CORE_REG(X)	(ACA_CORE_BASE + (X))
#define TXIN_CFG1	ACA_CORE_REG(0x0)
#define TXIN_CFG2	ACA_CORE_REG(0x4)
#define TXIN_CFG3	ACA_CORE_REG(0x8)
#define TXIN_DST_OWWBIT_CFG4	ACA_CORE_REG(TXIN_DST_OWNBIT)

#define TXOUT_CFG1	ACA_CORE_REG(0x100)
#define TXOUT_CFG2	ACA_CORE_REG(0x104)
#define TXOUT_CFG3	ACA_CORE_REG(0x108)
#define TXOUT_DST_OWWBIT_CFG4	ACA_CORE_REG(TXOUT_DST_OWNBIT)

#define RXOUT_CFG1	ACA_CORE_REG(0x300)
#define RXOUT_CFG2	ACA_CORE_REG(0x304)
#define RXOUT_CFG3	ACA_CORE_REG(0x308)
#define RXOUT_SRC_OWNBIT_CFG3	ACA_CORE_REG(RXOUT_SRC_OWNBIT)

#define RXIN_CFG1	ACA_CORE_REG(0x200)
#define RXIN_CFG2	ACA_CORE_REG(0x204)
#define RXIN_CFG3	ACA_CORE_REG(0x208)
#define RXIN_SRC_OWNBIT_CFG3	ACA_CORE_REG(RXIN_DST_OWNBIT)

/* Genrisc */
#define GNRC_REG(X)		(GENRISC_BASE + (X))
#define GNRC_STOP_OP		GNRC_REG(0x60)
#define GNRC_CONTINUE_OP	GNRC_REG(0x64)
#define GNRC_START_OP		GNRC_REG(0x90)

/* HOST Interface Register */
#define HOST_IF_REG(X)		(HOST_IF_BASE + (X))
#define HD_DESC_IN_DW		0x7u
#define HD_DESC_IN_DW_S		0
#define PD_DESC_IN_DW		0x70u
#define PD_DESC_IN_DW_S		4
#define BYTE_SWAP_EN		BIT(28)

#define TXIN_CONV_CFG		HOST_IF_REG(0x14)
#define TXOUT_CONV_CFG		HOST_IF_REG(0x18)
#define RXIN_CONV_CFG		HOST_IF_REG(0x1C)
#define RXOUT_CONV_CFG		HOST_IF_REG(0x20)

#define TXIN_COUNTERS		HOST_IF_REG(0x44)
#define TXOUT_COUNTERS		HOST_IF_REG(0x48)
#define RXIN_COUNTERS		HOST_IF_REG(0x4c)
#define RXOUT_COUNTERS		HOST_IF_REG(0x50)

#define TXOUT_RING_CFG		HOST_IF_REG(0x98)
#define RXOUT_RING_CFG		HOST_IF_REG(0x9C)

#define ACA_PENDING_JOB		0x00000300
#define ACA_PENDING_JOB_S	8
#define ACA_AVAIL_BUF		0x00030000
#define ACA_AVAIL_BUF_S		16
#define ACA_PP_BUFS		2

#define HOST_TYPE		HOST_IF_REG(0xA0)
#define TXOUT_COUNTERS_UPDATE	HOST_IF_REG(0xAC)
#define RXOUT_COUNTERS_UPDATE	HOST_IF_REG(0xB4)
#define RXIN_HD_ACCUM_ADD	HOST_IF_REG(0xC8) /* UMT Message trigger */
#define TXIN_HD_ACCUM_ADD	HOST_IF_REG(0xCC) /* UMT Message trigger */
#define RXOUT_HD_ACCUM_ADD	HOST_IF_REG(0xD0)
#define TXOUT_HD_ACCUM_ADD	HOST_IF_REG(0xD4)
#define RXOUT_ACA_ACCUM_ADD	HOST_IF_REG(0xE0) /* PPE FW tigger */
#define TXOUT_ACA_ACCUM_ADD	HOST_IF_REG(0xE4) /* PPE FW tigger */
#define RXOUT_HD_ACCUM_SUB	HOST_IF_REG(0xF8)
#define TXOUT_HD_ACCUM_SUB	HOST_IF_REG(0xFC)
#define RXIN_ACA_ACCUM_SUB	HOST_IF_REG(0x100)
#define TXIN_ACA_ACCUM_SUB	HOST_IF_REG(0x104)
#define TXIN_ACA_HD_ACC_CNT	HOST_IF_REG(0x11C)
#define UMT_ORDER_CFG		HOST_IF_REG(0x234)
#define RXIN_HD_ACCUM_ADD_BE	HOST_IF_REG(0x250)
#define TXIN_HD_ACCUM_ADD_BE	HOST_IF_REG(0x254)
#define RXOUT_HD_ACCUM_SUB_BE	HOST_IF_REG(0x268)
#define TXOUT_HD_ACCUM_SUB_BE	HOST_IF_REG(0x26c)

/* MAC_HT_EXTENSION Register */
#define MAC_HT_EXT_REG(X)	(MAC_HT_EXT_BASE + (X))

#define HT_GCLK_ENABLE		MAC_HT_EXT_REG(0)
#define HT_SW_RST_RELEASE	MAC_HT_EXT_REG(0x4)
#define HT_SW_RST_ASSRT		MAC_HT_EXT_REG(0x1C)
#define SW_RST_GENRISC		BIT(14)
#define SW_RST_RXOUT		BIT(26)
#define SW_RST_RXIN		BIT(27)
#define SW_RST_TXOUT		BIT(28)
#define SW_RST_TXIN		BIT(29)
#define SW_RST_HOSTIF_REG	BIT(30)
#define OCP_ARB_ACC_PAGE_REG	MAC_HT_EXT_REG(0x1C4)
#define AHB_ARB_HP_REG		MAC_HT_EXT_REG(0x1C8)

/* Genrisc FW Configuration */
#define GNRC_SPRAM_REG(X)	(GENRISC_SPRAM_BASE + (X))

/* TX IN */
#define GNRC_TXIN_TGT_STAT		GNRC_SPRAM_REG(0x04)
#define GNRC_TXIN_TGT_PD_OFF		GNRC_SPRAM_REG(0x08)
#define GNRC_TXIN_TGT_ACCM_CNT		GNRC_SPRAM_REG(0x0C)

/* TX OUT */
#define GNRC_TXOUT_TGT_STAT		GNRC_SPRAM_REG(0x10)
#define GNRC_TXOUT_TGT_PD_OFF		GNRC_SPRAM_REG(0x14)
#define GNRC_TXOUT_TGT_ACCM_CNT		GNRC_SPRAM_REG(0x18)

/* RX IN */
#define GNRC_RXIN_TGT_STAT		GNRC_SPRAM_REG(0x1C)
#define GNRC_RXIN_TGT_PD_OFF		GNRC_SPRAM_REG(0x20)
#define GNRC_RXIN_TGT_ACCM_CNT		GNRC_SPRAM_REG(0x24)

/* RX OUT XXX not consistent */
#define GNRC_RXOUT_TGT_STAT		GNRC_SPRAM_REG(0x28)
#define GNRC_RXOUT_TGT_PD_OFF		GNRC_SPRAM_REG(0x2C)
#define GNRC_RXOUT_TGT_ACCM_CNT		GNRC_SPRAM_REG(0x30)

/* 4 Ring 8 UMT case SoC cumulative counter address configuration */
#define GNRC_TXIN_CMLT_CNT_ADDR		GNRC_SPRAM_REG(0x34)
#define GNRC_TXOUT_CMLT_CNT_ADDR	GNRC_SPRAM_REG(0x38)
#define GNRC_RXOUT_CMLT_CNT_ADDR	GNRC_SPRAM_REG(0x3C)
#define GNRC_RXIN_CMLT_CNT_ADDR		GNRC_SPRAM_REG(0x40)


#define GNRC_SOURCE_TXIN_CMLT_CNT_ADDR	GNRC_SPRAM_REG(0x54)
#define GNRC_SOURCE_TXOUT_CMLT_CNT_ADDR	GNRC_SPRAM_REG(0x58)
#define GNRC_SOURCE_RXOUT_CMLT_CNT_ADDR	GNRC_SPRAM_REG(0x5c)
#define GNRC_SOURCE_RXIN_CMLT_CNT_ADDR	GNRC_SPRAM_REG(0x60)

/* Txin index prefill */
#define GNRC_TXIN_BUF_PREFILL		GNRC_SPRAM_REG(0x44)
/* Task enable bitmap */
#define GNRC_EN_TASK_BITMAP		GNRC_SPRAM_REG(0x64)

#define ACA_SRAM_REG(X)	(ACA_SRAM_BASE + (X))
#define ACA_TXOUT_PING_BUFFER_START ACA_SRAM_REG(0x1528)


/* XBAR SSX0 */
#define ACA_SSX0_BASE			0x180000
#define ACA_SSX0_IA_BASE(id)		(ACA_SSX0_BASE + (((id) - 1) << 10))
#define ACA_AGENT_CTRL(id)		(ACA_SSX0_IA_BASE(id) + 0x20)
#define ACA_AGENT_STATUS(id)		(ACA_SSX0_IA_BASE(id) + 0x28)

#define XBAR_CTRL_CORE_RESET		BIT(0)
#define XBAR_CTRL_REJECT		BIT(4)

#define XBAR_STAT_CORE_RESET		BIT(0)
#define XBAR_STAT_REQ_ACTIVE		BIT(4)
#define XBAR_STAT_RESP_WAITING		BIT(5)
#define XBAR_STAT_BURST			BIT(6)
#define XBAR_STAT_READEX		BIT(7)

enum {
	ACA_ACC_IA04 = 4,
	ACA_M_IA06 = 6,
};

/* Should be passed from ACA FW header */
#define DESC_NUM_PER_CH		1

/* ACA DMA REG */
#define ACA_DMA_BASE		0x60000

#define ACA_DMA_REG(X)		(ACA_DMA_BASE + (X))
#define ADMA_CLC		ACA_DMA_REG(0x0)
#define ADMA_ID			ACA_DMA_REG(0x8)
#define ADMA_CTRL		ACA_DMA_REG(0x10)
#define ADMA_CPOLL		ACA_DMA_REG(0x14)

#define ADMA_ID_REV		0x1Fu
#define ADMA_ID_REV_S		0
#define ADMA_ID_ID		0xFF00u
#define ADMA_ID_ID_S		8
#define ADMA_ID_PRTNR		0xF0000u
#define ADMA_ID_PRTNR_S		16
#define ADMA_ID_CHNR		0x7F00000u
#define ADMA_ID_CHNR_S		20

#define ADMA_CPOLL_EN		BIT(31)

#define ADMA_CPOLL_CNT		0xFFF0u
#define ADMA_CPOLL_CNT_S	4
#define ADMA_DEFAULT_POLL	24
#define ADMA_CS			ACA_DMA_REG(0x18)
#define ADMA_CCTRL		ACA_DMA_REG(0x1C)
#define ADMA_CDBA		ACA_DMA_REG(0x20)
#define ADMA_CDLEN		ACA_DMA_REG(0x24)
#define ADMA_CIS		ACA_DMA_REG(0x28)
#define ADMA_CIE		ACA_DMA_REG(0x2C)

#define ADMA_CI_EOP		BIT(1)
#define ADMA_CI_DUR		BIT(2)
#define ADMA_CI_DESCPT		BIT(3)
#define ADMA_CI_CHOFF		BIT(4)
#define ADMA_CI_RDERR		BIT(5)
#define ADMA_CI_ALL		(ADMA_CI_EOP | ADMA_CI_DUR | ADMA_CI_DESCPT\
				| ADMA_CI_CHOFF | ADMA_CI_RDERR)

#define ADMA_CDPTNRD		ACA_DMA_REG(0x34)
#define ADMA_PS			ACA_DMA_REG(0x40)
#define ADMA_PCTRL		ACA_DMA_REG(0x44)

/* DMA CCTRL BIT */
#define CCTRL_RST		1 /* Channel Reset */
#define CCTRL_ONOFF		0 /* Channel On/Off */

/* DMA CTRL BIT */
#define CTRL_PKTARB		31 /* Packet Arbitration */
#define CTRL_MDC		15 /* Meta data copy */
#define CTRL_DDBR		14 /* Dynamic Burst */
#define CTRL_DCNF		13 /* Descriptor Length CFG*/
#define CTRL_ENBE		9 /* Byte Enable */
#define CTRL_DRB		8 /* Descriptor read back */
#define CTRL_DSRAM		1 /* Dedicated Descriptor Access port Enable */
#define CTRL_RST		0 /* Global Reset */

/* DMA PORT BIT */
#define PCTRL_FLUSH		16
#define PCTRL_TXENDI		10 /* TX DIR Endianess */
#define PCTRL_RXENDI		8 /* RX DIR Endianess */
#define PCTRL_TXBL		4 /* TX burst 2/4/8 */
#define PCTRL_RXBL		2 /* RX burst 2/4/8 */
#define PCTRL_TXBL16		1 /* TX burst of 16 */
#define PCTRL_RXBL16		0 /* RX burst of 16 */

/*DMA ID BIT */
#define ID_CHNR			20 /* Channel Number */

/*DMA POLLING BIT */
#define POLL_EN			31 /* Polling Enable */
#define POLL_CNT		4 /* Polling Counter */

#define ACA_DMA_CHAN_MAX	12

enum aca_sec_id {
	ACA_SEC_HIF = 0x1,
	ACA_SEC_GNR = 0x2,
	ACA_SEC_MAC_HT = 0x3,
	ACA_SEC_MEM_TXIN = 0x4,
	ACA_SEC_MEM_TXIN_PDRING = 0x5,
	ACA_SEC_MEM_TXOUT = 0x6,
	ACA_SEC_MEM_TXOUT_PDRING = 0x7,
	ACA_SEC_MEM_RXOUT = 0x8,
	ACA_SEC_MEM_RXOUT_PDRING = 0x9,
	ACA_SEC_MEM_RXIN = 0xa,
	ACA_SEC_MEM_RXIN_PDRING = 0xb,
	ACA_SEC_DMA = 0xc,
	ACA_SEC_FW_INIT = 0xd,
	ACA_SEC_FW = 0x88,
};

enum aca_fw_id {
	ACA_FW_TXIN = 1,
	ACA_FW_TXOUT = 2,
	ACA_FW_RXIN = 3,
	ACA_FW_RXOUT = 4,
	ACA_FW_GNRC = 5,
	ACA_FW_MAX = 5,
};

enum aca_img_type {
	ACA_VRX518_IMG,
	ACA_VRX618_IMG,
	ACA_FALCON_IMG,
	ACA_PUMA_IMG,
	ACA_IMG_MAX,
};

enum aca_soc_type {
	ACA_SOC_XRX300 = 1,
	ACA_SOC_XRX500 = 2,
	ACA_SOC_PUMA   = 4,
	ACA_SOC_3RD_PARTY = 8,
};

#define ACA_SOC_MASK	0xf

/* Common information element, len has different variants */
struct aca_fw_ie {
	__be32 id;
	__be32 len;
} __packed;

struct aca_fw_reg {
	__be32 offset;
	__be32 value;
} __packed;

struct aca_sram_desc {
	__be32 dnum;
	__be32 dbase;
} __packed;

struct aca_fw_dma {
	__be32 cid;
	__be32 base;
} __packed;

/* ACA internal header part */
struct aca_int_hdr {
	__be32 id;
	__be32 offset;
	__be32 size;
	__be32 load_addr;
} __packed;

struct aca_fw_param {
	__be32 st_sz;
	__be32 init_addr;
} __packed;

struct aca_mem_layout {
	u32 txin_host_desc_base;
	u32 txin_host_dnum;
	u32 txout_host_desc_base;
	u32 txout_host_dnum;
	u32 rxin_host_desc_base;
	u32 rxin_host_dnum;
	u32 rxout_host_desc_base;
	u32 rxout_host_dnum;
};

struct aca_pdmem_layout {
	u32 txin_pd_desc_base;
	u32 txin_pd_dnum;
	u32 txout_pd_desc_base;
	u32 txout_pd_dnum;
	u32 rxin_pd_desc_base;
	u32 rxin_pd_dnum;
	u32 rxout_pd_desc_base;
	u32 rxout_pd_dnum;
};

struct aca_fw_addr_tuple {
	u32 fw_id;
	u32 fw_load_addr;
	size_t fw_size;
	const char *fw_base;
};

struct aca_fw_dl_addr {
	u32 fw_num;
	struct aca_fw_addr_tuple fw_addr[ACA_FW_MAX];
};

struct aca_fw_info {
	const struct firmware *fw;
	const void *fw_data;
	size_t fw_len;
	struct aca_mem_layout mem_layout;
	struct aca_pdmem_layout pdmem_layout;
	struct aca_fw_param fw_param;
	struct aca_fw_dl_addr fw_dl;
	u32 chan_num;
	u32 adma_desc_base[ACA_DMA_CHAN_MAX];
};

union fw_ver {
#ifdef CONFIG_CPU_BIG_ENDIAN
	struct {
		u32 build:4;
		u32 branch:4;
		u32 major:8;
		u32 minor:16;
	} __packed field;
#else
	struct {
		u32 minor:16;
		u32 major:8;
		u32 branch:4;
		u32 build:4;
	} __packed field;
#endif /* CONFIG_CPU_BIG_ENDIAN */
	u32 all;
} __packed;

union img_soc_type {
#ifdef CONFIG_CPU_BIG_ENDIAN
	struct {
		u32 img_type:16;
		u32 soc_type:16;
	} __packed field;
#else
	struct {
		u32 soc_type:16;
		u32 img_type:16;
	} __packed field;
#endif /* CONFIG_CPU_BIG_ENDIAN */
	u32 all;
} __packed;

/* Fixed header part */
struct aca_fw_f_hdr {
	__be32 ver;
	__be32 type;
	__be32 hdr_size;
	__be32 fw_size;
	__be32 num_section;
} __packed;

struct aca_hif_param {
	u32 soc_desc_base;
	u32 soc_desc_num;
	u32 pp_buf_base;
	u32 pp_buf_num;
	u32 pd_desc_base;
	u32 pd_desc_num;
	u32 pd_desc_threshold;
} __packed;

struct aca_hif_params {
	u32 task_mask;
	struct aca_hif_param txin;
	struct aca_hif_param txout;
	struct aca_hif_param rxin;
	struct aca_hif_param rxout;
	u32 dbg_base;
	u32 dbg_size;
	u32 magic;
} __packed;

#define ACA_MAGIC	0x25062016

struct dc_aca {
	bool initialized;
	spinlock_t	clk_lock;
	spinlock_t	rcu_lock;
	struct mutex	pin_lock;
	struct aca_fw_info fw_info;
	struct aca_hif_params *hif_params;
	u32 max_gpio;
	u32 adma_chans;
};
#endif /* ACA_H */
