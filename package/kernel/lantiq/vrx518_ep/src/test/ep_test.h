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

#ifndef EP_TEST_H
#define EP_TEST_H

/* SB address on xBar */
#define SB_XBAR_BASE		0x280000
#define SB_XBAR_DES_RXBASE	SB_XBAR_BASE
#define SB_XBAR_DES_TXBASE	(SB_XBAR_BASE + 0x400)
#define SB_XBAR_DATA_BASE	(SB_XBAR_BASE + 0x800)
#define SB_XBAR_ADDR(x)		(SB_XBAR_BASE + ((((x) - 0xA000)) << 2))

/*----------------------------------------------------------
 * ACA Shadow Registers
 * 3 * 4 = 12
 * *_STATUS need to be initialized to nonzero by PPE driver
 *----------------------------------------------------------
 */

#define __ACA_SHADOW_REG_BASE			0xADF0

#define __TX_IN_ACA_ACCUM_COUNT			0xADF0

#define __TX_IN_ACA_ACCUM_STATUS		0xADF1

#define __TX_IN_QUEUE_PD_BASE_ADDR_OFFSET	0xADF2

#define __TX_OUT_ACA_ACCUM_COUNT		0xADF3

#define __TX_OUT_ACA_ACCUM_STATUS		0xADF4

#define __TX_OUT_QUEUE_PD_BASE_ADDR_OFFSET	0xADF5

#define __RX_IN_ACA_ACCUM_COUNT			0xADF6

#define __RX_IN_ACA_ACCUM_STATUS		0xADF7

#define __RX_IN_QUEUE_PD_BASE_ADDR_OFFSET	0xADF8

#define __RX_OUT_ACA_ACCUM_COUNT		0xADF9

#define __RX_OUT_ACA_ACCUM_STATUS		0xADFA

#define __RX_OUT_QUEUE_PD_BASE_ADDR_OFFSET	0xADFB

#define TXIN_PD_DES_NUM		64
#define TXIN_PD_DBASE		0x105400
#define TXIN_SOC_DES_NUM	32
#define TXIN_SOC_DBASE		0x24000000
#define TXIN_HOST_DES_NUM	32
#define TXIN_HD_DES_SIZE	4 /* size in DWORD */
#define TXIN_PD_DES_SIZE	2 /* size in DWORD */

#define TXOUT_PD_DES_NUM	32
#define TXOUT_PD_DBASE		0x105700
#define TXOUT_SOC_DES_NUM	32
#define TXOUT_SOC_DBASE		0x24001000
#define TXOUT_HOST_DES_NUM	32
#define TXOUT_HD_DES_SIZE	1 /* size in DWORD */
#define TXOUT_PD_DES_SIZE	2 /* size in DWORD */

#define RXOUT_PD_DES_NUM	32
#define RXOUT_PD_DBASE		0x105C00
#define RXOUT_SOC_DES_NUM	32
#define RXOUT_SOC_DBASE		0x24002000
#define RXOUT_HOST_DES_NUM	32
#define RXOUT_HD_DES_SIZE	4 /* size in DWORD */
#define RXOUT_PD_DES_SIZE	2 /* size in DWORD */

/* PPE interrupt */
#define PPE_MBOX_TEST_BIT	0x1
#define PPE_MBOX_IRQ_TEST_NUM	100

#define PPE_MBOX_BASE		0x334800

#define MBOX_REG(X)	(PPE_MBOX_BASE + (X))
#define MBOX_IGU0_ISRS	MBOX_REG(0x0)
#define MBOX_IGU0_ISRC	MBOX_REG(0x4)
#define MBOX_IGU0_ISR	MBOX_REG(0x8)
#define MBOX_IGU0_IER	MBOX_REG(0xc)

#define HOST_IF_BASE		0x50000
#define HOST_IF_REG(X)		(HOST_IF_BASE + (X))
#define TXIN_CONV_CFG		HOST_IF_REG(0x14)
#define RXIN_HD_ACCUM_ADD	HOST_IF_REG(0xC8) /* UMT Message trigger */
#define TXIN_HD_ACCUM_ADD	HOST_IF_REG(0xCC) /* UMT Message trigger */
#define RXOUT_ACA_ACCUM_ADD	HOST_IF_REG(0xE0) /* PPE FW tigger */
#define TXOUT_ACA_ACCUM_ADD	HOST_IF_REG(0xE4) /* PPE FW tigger */

#define CDMA_BASE	0x2D0000
#define CDMA_REG(X)	(CDMA_BASE + (X))

#define DMA_CLC		CDMA_REG(0x00)
#define DMA_ID		CDMA_REG(0x08)
#define DMA_CTRL	CDMA_REG(0x10)

#define DMA_CTRL_RST		BIT(0)
#define DMA_CTRL_DSRAM_PATH	BIT(1)
#define DMA_CTRL_CH_FL		BIT(6)
#define DMA_CTRL_DS_FOD		BIT(7)
#define DMA_CTRL_DRB		BIT(8)
#define DMA_CTRL_ENBE		BIT(9)
#define DMA_CTRL_PRELOAD_INT_S	10
#define DMA_CTRL_PRELOAD_INT	0x0C00u
#define DMA_CTRL_PRELOAD_EN	BIT(12)
#define DMA_CTRL_MBRST_CNT_S	16
#define DMA_CTRL_MBRST_CNT	0x3FF0000u
#define DMA_CTRL_MBRSTARB	BIT(30)
#define DMA_CTRL_PKTARB		BIT(31)

#define DMA_CPOLL	CDMA_REG(0x14)
#define DMA_CPOLL_CNT_S		4
#define DMA_CPOLL_CNT		0xFFF0u
#define DMA_CPOLL_EN		BIT(31)

#define DMA_CS		CDMA_REG(0x18)
#define DMA_CCTRL	CDMA_REG(0x1C)
#define DMA_CCTRL_ON		BIT(0)
#define DMA_CCTRL_RST		BIT(1)
#define DMA_CCTRL_DIR_TX	BIT(8)
#define DMA_CCTRL_CLASS_S	9
#define DMA_CCTRL_CLASS		0xE00u
#define DMA_CCTRL_PRTNR_S	12
#define DMA_CCTRL_PRTNR		0xF000u
#define DMA_CCTRL_TXWGT_S	16
#define DMA_CCTRL_TXWGT		0x30000u
#define DMA_CCTRL_CLASSH_S	18
#define DMA_CCTRL_CLASSH	0xC0000u
#define DMA_CCTRL_PDEN		BIT(23)
#define DMA_CCTRL_P2PCPY	BIT(24)
#define DMA_CCTRL_LBEN		BIT(25)
#define DMA_CCTRL_LBCHNR_S	26
#define DMA_CCTRL_LBCHNR	0xFC000000u

#define DMA_CDBA	CDMA_REG(0x20)
#define DMA_CDLEN	CDMA_REG(0x24)
#define DMA_CIS		CDMA_REG(0x28)
#define DMA_CIE		CDMA_REG(0x2C)

#define DMA_CI_EOP		BIT(1)
#define DMA_CI_DUR		BIT(2)
#define DMA_CI_DESCPT		BIT(3)
#define DMA_CI_CHOFF		BIT(4)
#define DMA_CI_RDERR		BIT(5)
#define DMA_CI_ALL	(DMA_CI_EOP | DMA_CI_DUR | DMA_CI_DESCPT\
			| DMA_CI_CHOFF | DMA_CI_RDERR)

#define DMA_CI_DEFAULT	(DMA_CI_EOP | DMA_CI_DESCPT)
#define DMA_CDPTNRD	CDMA_REG(0x34)

#define DMA_PS		CDMA_REG(0x40)
#define DMA_PCTRL	CDMA_REG(0x44)
#define DMA_PCTRL_RXBL16	BIT(0)
#define DMA_PCTRL_TXBL16	BIT(1)
#define DMA_PCTRL_RXBL_S	2
#define DMA_PCTRL_RXBL		0xCu
#define DMA_PCTRL_TXBL_S	4
#define DMA_PCTRL_TXBL		0x30u
#define DMA_PCTRL_PDEN		BIT(6)
#define DMA_PCTRL_PDEN_S	6
#define DMA_PCTRL_RXENDI_S	8
#define DMA_PCTRL_RXENDI	0x300u
#define DMA_PCTRL_TXENDI_S	10
#define DMA_PCTRL_TXENDI	0xC00u
#define DMA_PCTRL_TXWGT_S	12
#define DMA_PCTRL_TXWGT		0x7000u
#define DMA_PCTRL_MEM_FLUSH	BIT(16)

#define DMA_IRNEN	CDMA_REG(0xF4)
#define DMA_IRNCR	CDMA_REG(0xF8)
#define DMA_IRNICR	CDMA_REG(0xFC)

#ifdef CONFIG_CPU_BIG_ENDIAN
struct aca_dma_desc {
	/* DW0 */
	u32 dw0;
	/* DW1 */
	u32 dw1;
	/* DW2 */
	u32 data_pointer;
	/* DW3 */
	u32 own:1;
	u32 c:1;
	u32 sop:1;
	u32 eop:1;
	u32 dic:1;
	u32 pdu_type:1;
	u32 byte_off:3;
	u32 qid:4;
	u32 mpoa_pt:1;
	u32 mpoa_mode:2;
	u32 data_len:16;
}__packed __aligned(16);

/* 2 DWs format descriptor */
struct aca_dma_desc_2dw {
	u32 data_pointer; /* Descriptor data pointer */
	union {
		struct {
			u32 own:1;
			u32 c:1;
			u32 sop:1;
			u32 eop:1;
			u32 meta_data0:2;
			u32 byte_offset:3;
			u32 meta_data1:7;
			u32 data_len:16;
		} __packed field;
		u32 word;
	} __packed status;
} __packed __aligned(8);
#else
struct aca_dma_desc {
	/* DW0 */
	u32 dw0;
	/* DW1 */
	u32 dw1;
	/* DW2 */
	u32 data_pointer;
	/* DW 3 */
	u32 data_len:16;
	u32 mpoa_mode:2;
	u32 mpoa_pt:1;
	u32 qid:4;
	u32 byte_off:3;
	u32 pdu_type:1;
	u32 dic:1;
	u32 eop:1;
	u32 sop:1;
	u32 c:1;
	u32 own:1;
}__packed __aligned(16);

/* 2 DWs format descriptor */
struct aca_dma_desc_2dw {
	u32 data_pointer; /* Descriptor data pointer */
	union {
		struct {
			u32 data_len:16;
			u32 meta_data1:7;
			u32 byte_offset:3;
			u32 meta_data0:2;
			u32 eop:1;
			u32 sop:1;
			u32 c:1;
			u32 own:1;
		} __packed field;
		u32 word;
	} __packed status;
} __packed __aligned(8);
#endif
#endif /* EP_TEST_H */
