/******************************************************************************

  Copyright (c) 2007
  Infineon Technologies AG
  St. Martin Strasse 53; 81669 Munich, Germany

  Any use of this Software is subject to the conclusion of a respective
  License Agreement. Without such a License Agreement no rights to the
  Software are granted.

 ******************************************************************************/

#ifndef __DMA_REG_H
#define __DMA_REG_H

#define dma_r32(reg) ltq_r32(&dma->reg)
#define dma_w32(val, reg) ltq_w32(val, &dma->reg)
#define dma_w32_mask(clear, set, reg) ltq_w32_mask(clear, set, &dma->reg)

/** DMA register structure */
struct svip_reg_dma {
	volatile unsigned long  clc;  /*  0x00 */
	volatile unsigned long  reserved0;  /*  0x04 */
	volatile unsigned long  id;  /*  0x08 */
	volatile unsigned long  reserved1;  /*  0x0c */
	volatile unsigned long  ctrl;  /*  0x10 */
	volatile unsigned long  cpoll;  /*  0x14 */
	volatile unsigned long  cs;  /*  0x18 */
	volatile unsigned long  cctrl;  /*  0x1C */
	volatile unsigned long  cdba;  /*  0x20 */
	volatile unsigned long  cdlen;  /*  0x24 */
	volatile unsigned long  cis;  /*  0x28 */
	volatile unsigned long  cie;  /*  0x2C */
	volatile unsigned long  cgbl;  /*  0x30 */
	volatile unsigned long  reserved2[3];  /*  0x34 */
	volatile unsigned long  ps;  /*  0x40 */
	volatile unsigned long  pctrl;  /*  0x44 */
	volatile unsigned long  reserved3[43];  /*  0x48 */
	volatile unsigned long  irnen;  /*  0xF4 */
	volatile unsigned long  irncr;  /*  0xF8 */
	volatile unsigned long  irnicr;  /*  0xFC */
};

/*******************************************************************************
 * CLC Register
 ******************************************************************************/

/* Fast Shut-Off Enable Bit (5) */
#define DMA_CLC_FSOE   (0x1 << 5)
#define DMA_CLC_FSOE_VAL(val)   (((val) & 0x1) << 5)
#define DMA_CLC_FSOE_GET(val)   ((((val) & DMA_CLC_FSOE) >> 5) & 0x1)
#define DMA_CLC_FSOE_SET(reg,val) (reg) = ((reg & ~DMA_CLC_FSOE) | (((val) & 0x1) << 5))
/* Suspend Bit Write Enable for OCDS (4) */
#define DMA_CLC_SBWE   (0x1 << 4)
#define DMA_CLC_SBWE_VAL(val)   (((val) & 0x1) << 4)
#define DMA_CLC_SBWE_SET(reg,val) (reg) = (((reg & ~DMA_CLC_SBWE) | (val) & 1) << 4)
/* External Request Disable (3) */
#define DMA_CLC_EDIS   (0x1 << 3)
#define DMA_CLC_EDIS_VAL(val)   (((val) & 0x1) << 3)
#define DMA_CLC_EDIS_GET(val)   ((((val) & DMA_CLC_EDIS) >> 3) & 0x1)
#define DMA_CLC_EDIS_SET(reg,val) (reg) = ((reg & ~DMA_CLC_EDIS) | (((val) & 0x1) << 3))
/* Suspend Enable Bit for OCDS (2) */
#define DMA_CLC_SPEN   (0x1 << 2)
#define DMA_CLC_SPEN_VAL(val)   (((val) & 0x1) << 2)
#define DMA_CLC_SPEN_GET(val)   ((((val) & DMA_CLC_SPEN) >> 2) & 0x1)
#define DMA_CLC_SPEN_SET(reg,val) (reg) = ((reg & ~DMA_CLC_SPEN) | (((val) & 0x1) << 2))
/* Disable Status Bit (1) */
#define DMA_CLC_DISS   (0x1 << 1)
#define DMA_CLC_DISS_GET(val)   ((((val) & DMA_CLC_DISS) >> 1) & 0x1)
/* Disable Request Bit (0) */
#define DMA_CLC_DISR   (0x1)
#define DMA_CLC_DISR_VAL(val)   (((val) & 0x1) << 0)
#define DMA_CLC_DISR_GET(val)   ((((val) & DMA_CLC_DISR) >> 0) & 0x1)
#define DMA_CLC_DISR_SET(reg,val) (reg) = ((reg & ~DMA_CLC_DISR) | (((val) & 0x1) << 0))

/*******************************************************************************
 * ID Register
 ******************************************************************************/

/* Number of Channels (25:20) */
#define DMA_ID_CHNR   (0x3f << 20)
#define DMA_ID_CHNR_GET(val)   ((((val) & DMA_ID_CHNR) >> 20) & 0x3f)
/* Number of Ports (19:16) */
#define DMA_ID_PRTNR   (0xf << 16)
#define DMA_ID_PRTNR_GET(val)   ((((val) & DMA_ID_PRTNR) >> 16) & 0xf)
/* Module ID (15:8) */
#define DMA_ID_ID   (0xff << 8)
#define DMA_ID_ID_GET(val)   ((((val) & DMA_ID_ID) >> 8) & 0xff)
/* Revision (4:0) */
#define DMA_ID_REV   (0x1f)
#define DMA_ID_REV_GET(val)   ((((val) & DMA_ID_REV) >> 0) & 0x1f)

/*******************************************************************************
 * Control Register
 ******************************************************************************/

/* Global Software Reset (0) */
#define DMA_CTRL_RST   (0x1)
#define DMA_CTRL_RST_GET(val)   ((((val) & DMA_CTRL_RST) >> 0) & 0x1)

/*******************************************************************************
 * Channel Polling Register
 ******************************************************************************/

/* Enable (31) */
#define DMA_CPOLL_EN   (0x1 << 31)
#define DMA_CPOLL_EN_VAL(val)   (((val) & 0x1) << 31)
#define DMA_CPOLL_EN_GET(val)   ((((val) & DMA_CPOLL_EN) >> 31) & 0x1)
#define DMA_CPOLL_EN_SET(reg,val) (reg) = ((reg & ~DMA_CPOLL_EN) | (((val) & 0x1) << 31))
/* Counter (15:4) */
#define DMA_CPOLL_CNT   (0xfff << 4)
#define DMA_CPOLL_CNT_VAL(val)   (((val) & 0xfff) << 4)
#define DMA_CPOLL_CNT_GET(val)   ((((val) & DMA_CPOLL_CNT) >> 4) & 0xfff)
#define DMA_CPOLL_CNT_SET(reg,val) (reg) = ((reg & ~DMA_CPOLL_CNT) | (((val) & 0xfff) << 4))

/*******************************************************************************
 * Global Buffer Length Register
 ******************************************************************************/

/* Global Buffer Length (15:0) */
#define DMA_CGBL_GBL   (0xffff)
#define DMA_CGBL_GBL_VAL(val)   (((val) & 0xffff) << 0)
#define DMA_CGBL_GBL_GET(val)   ((((val) & DMA_CGBL_GBL) >> 0) & 0xffff)
#define DMA_CGBL_GBL_SET(reg,val) (reg) = ((reg & ~DMA_CGBL_GBL) | (((val) & 0xffff) << 0))

/*******************************************************************************
 * Channel Select Register
 ******************************************************************************/

/* Channel Selection (4:0) */
#define DMA_CS_CS   (0x1f)
#define DMA_CS_CS_VAL(val)   (((val) & 0x1f) << 0)
#define DMA_CS_CS_GET(val)   ((((val) & DMA_CS_CS) >> 0) & 0x1f)
#define DMA_CS_CS_SET(reg,val) (reg) = ((reg & ~DMA_CS_CS) | (((val) & 0x1f) << 0))

/*******************************************************************************
 * Channel Control Register
 ******************************************************************************/

/* Peripheral to Peripheral Copy (24) */
#define DMA_CCTRL_P2PCPY   (0x1 << 24)
#define DMA_CCTRL_P2PCPY_VAL(val)   (((val) & 0x1) << 24)
#define DMA_CCTRL_P2PCPY_GET(val)   ((((val) & DMA_CCTRL_P2PCPY) >> 24) & 0x1)
#define DMA_CCTRL_P2PCPY_SET(reg,val) (reg) = ((reg & ~DMA_CCTRL_P2PCPY) | (((val) & 0x1) << 24))
/* Channel Weight for Transmit Direction (17:16) */
#define DMA_CCTRL_TXWGT   (0x3 << 16)
#define DMA_CCTRL_TXWGT_VAL(val)   (((val) & 0x3) << 16)
#define DMA_CCTRL_TXWGT_GET(val)   ((((val) & DMA_CCTRL_TXWGT) >> 16) & 0x3)
#define DMA_CCTRL_TXWGT_SET(reg,val) (reg) = ((reg & ~DMA_CCTRL_TXWGT) | (((val) & 0x3) << 16))
/* Port Assignment (13:11) */
#define DMA_CCTRL_PRTNR   (0x7 << 11)
#define DMA_CCTRL_PRTNR_GET(val)   ((((val) & DMA_CCTRL_PRTNR) >> 11) & 0x7)
/* Class (10:9) */
#define DMA_CCTRL_CLASS   (0x3 << 9)
#define DMA_CCTRL_CLASS_VAL(val)   (((val) & 0x3) << 9)
#define DMA_CCTRL_CLASS_GET(val)   ((((val) & DMA_CCTRL_CLASS) >> 9) & 0x3)
#define DMA_CCTRL_CLASS_SET(reg,val) (reg) = ((reg & ~DMA_CCTRL_CLASS) | (((val) & 0x3) << 9))
/* Direction (8) */
#define DMA_CCTRL_DIR   (0x1 << 8)
#define DMA_CCTRL_DIR_GET(val)   ((((val) & DMA_CCTRL_DIR) >> 8) & 0x1)
/* Reset (1) */
#define DMA_CCTRL_RST   (0x1 << 1)
#define DMA_CCTRL_RST_VAL(val)   (((val) & 0x1) << 1)
#define DMA_CCTRL_RST_GET(val)   ((((val) & DMA_CCTRL_RST) >> 1) & 0x1)
#define DMA_CCTRL_RST_SET(reg,val) (reg) = ((reg & ~DMA_CCTRL_RST) | (((val) & 0x1) << 1))
/* Channel On or Off (0) */
#define DMA_CCTRL_ON_OFF   (0x1)
#define DMA_CCTRL_ON_OFF_VAL(val)   (((val) & 0x1) << 0)
#define DMA_CCTRL_ON_OFF_GET(val)   ((((val) & DMA_CCTRL_ON_OFF) >> 0) & 0x1)
#define DMA_CCTRL_ON_OFF_SET(reg,val) (reg) = ((reg & ~DMA_CCTRL_ON_OFF) | (((val) & 0x1) << 0))

/*******************************************************************************
 * Channel Descriptor Base Address Register
 ******************************************************************************/

/* Channel Descriptor Base Address (29:3) */
#define DMA_CDBA_CDBA   (0x7ffffff << 3)
#define DMA_CDBA_CDBA_VAL(val)   (((val) & 0x7ffffff) << 3)
#define DMA_CDBA_CDBA_GET(val)   ((((val) & DMA_CDBA_CDBA) >> 3) & 0x7ffffff)
#define DMA_CDBA_CDBA_SET(reg,val) (reg) = ((reg & ~DMA_CDBA_CDBA) | (((val) & 0x7ffffff) << 3))

/*******************************************************************************
 * Channel Descriptor Length Register
 ******************************************************************************/

/* Channel Descriptor Length (7:0) */
#define DMA_CDLEN_CDLEN   (0xff)
#define DMA_CDLEN_CDLEN_VAL(val)   (((val) & 0xff) << 0)
#define DMA_CDLEN_CDLEN_GET(val)   ((((val) & DMA_CDLEN_CDLEN) >> 0) & 0xff)
#define DMA_CDLEN_CDLEN_SET(reg,val) (reg) = ((reg & ~DMA_CDLEN_CDLEN) | (((val) & 0xff) << 0))

/*******************************************************************************
 * Channel Interrupt Status Register
 ******************************************************************************/

/* SAI Read Error Interrupt (5) */
#define DMA_CIS_RDERR   (0x1 << 5)
#define DMA_CIS_RDERR_GET(val)   ((((val) & DMA_CIS_RDERR) >> 5) & 0x1)
/* Channel Off Interrupt (4) */
#define DMA_CIS_CHOFF   (0x1 << 4)
#define DMA_CIS_CHOFF_GET(val)   ((((val) & DMA_CIS_CHOFF) >> 4) & 0x1)
/* Descriptor Complete Interrupt (3) */
#define DMA_CIS_DESCPT   (0x1 << 3)
#define DMA_CIS_DESCPT_GET(val)   ((((val) & DMA_CIS_DESCPT) >> 3) & 0x1)
/* Descriptor Under-Run Interrupt (2) */
#define DMA_CIS_DUR   (0x1 << 2)
#define DMA_CIS_DUR_GET(val)   ((((val) & DMA_CIS_DUR) >> 2) & 0x1)
/* End of Packet Interrupt (1) */
#define DMA_CIS_EOP   (0x1 << 1)
#define DMA_CIS_EOP_GET(val)   ((((val) & DMA_CIS_EOP) >> 1) & 0x1)

/*******************************************************************************
 * Channel Interrupt Enable Register
 ******************************************************************************/

/* SAI Read Error Interrupt (5) */
#define DMA_CIE_RDERR   (0x1 << 5)
#define DMA_CIE_RDERR_GET(val)   ((((val) & DMA_CIE_RDERR) >> 5) & 0x1)
/* Channel Off Interrupt (4) */
#define DMA_CIE_CHOFF   (0x1 << 4)
#define DMA_CIE_CHOFF_GET(val)   ((((val) & DMA_CIE_CHOFF) >> 4) & 0x1)
/* Descriptor Complete Interrupt Enable (3) */
#define DMA_CIE_DESCPT   (0x1 << 3)
#define DMA_CIE_DESCPT_GET(val)   ((((val) & DMA_CIE_DESCPT) >> 3) & 0x1)
/* Descriptor Under Run Interrupt Enable (2) */
#define DMA_CIE_DUR   (0x1 << 2)
#define DMA_CIE_DUR_GET(val)   ((((val) & DMA_CIE_DUR) >> 2) & 0x1)
/* End of Packet Interrupt Enable (1) */
#define DMA_CIE_EOP   (0x1 << 1)
#define DMA_CIE_EOP_GET(val)   ((((val) & DMA_CIE_EOP) >> 1) & 0x1)

/*******************************************************************************
 * Port Select Register
 ******************************************************************************/

/* Port Selection (2:0) */
#define DMA_PS_PS   (0x7)
#define DMA_PS_PS_VAL(val)   (((val) & 0x7) << 0)
#define DMA_PS_PS_GET(val)   ((((val) & DMA_PS_PS) >> 0) & 0x7)
#define DMA_PS_PS_SET(reg,val) (reg) = ((reg & ~DMA_PS_PS) | (((val) & 0x7) << 0))

/*******************************************************************************
 * Port Control Register
 ******************************************************************************/

/* General Purpose Control (16) */
#define DMA_PCTRL_GPC   (0x1 << 16)
#define DMA_PCTRL_GPC_VAL(val)   (((val) & 0x1) << 16)
#define DMA_PCTRL_GPC_GET(val)   ((((val) & DMA_PCTRL_GPC) >> 16) & 0x1)
#define DMA_PCTRL_GPC_SET(reg,val) (reg) = ((reg & ~DMA_PCTRL_GPC) | (((val) & 0x1) << 16))
/* Port Weight for Transmit Direction (14:12) */
#define DMA_PCTRL_TXWGT   (0x7 << 12)
#define DMA_PCTRL_TXWGT_VAL(val)   (((val) & 0x7) << 12)
#define DMA_PCTRL_TXWGT_GET(val)   ((((val) & DMA_PCTRL_TXWGT) >> 12) & 0x7)
#define DMA_PCTRL_TXWGT_SET(reg,val) (reg) = ((reg & ~DMA_PCTRL_TXWGT) | (((val) & 0x7) << 12))
/* Endianness for Transmit Direction (11:10) */
#define DMA_PCTRL_TXENDI   (0x3 << 10)
#define DMA_PCTRL_TXENDI_VAL(val)   (((val) & 0x3) << 10)
#define DMA_PCTRL_TXENDI_GET(val)   ((((val) & DMA_PCTRL_TXENDI) >> 10) & 0x3)
#define DMA_PCTRL_TXENDI_SET(reg,val) (reg) = ((reg & ~DMA_PCTRL_TXENDI) | (((val) & 0x3) << 10))
/* Endianness for Receive Direction (9:8) */
#define DMA_PCTRL_RXENDI   (0x3 << 8)
#define DMA_PCTRL_RXENDI_VAL(val)   (((val) & 0x3) << 8)
#define DMA_PCTRL_RXENDI_GET(val)   ((((val) & DMA_PCTRL_RXENDI) >> 8) & 0x3)
#define DMA_PCTRL_RXENDI_SET(reg,val) (reg) = ((reg & ~DMA_PCTRL_RXENDI) | (((val) & 0x3) << 8))
/* Packet Drop Enable (6) */
#define DMA_PCTRL_PDEN   (0x1 << 6)
#define DMA_PCTRL_PDEN_VAL(val)   (((val) & 0x1) << 6)
#define DMA_PCTRL_PDEN_GET(val)   ((((val) & DMA_PCTRL_PDEN) >> 6) & 0x1)
#define DMA_PCTRL_PDEN_SET(reg,val) (reg) = ((reg & ~DMA_PCTRL_PDEN) | (((val) & 0x1) << 6))
/* Burst Length for Transmit Direction (5:4) */
#define DMA_PCTRL_TXBL   (0x3 << 4)
#define DMA_PCTRL_TXBL_VAL(val)   (((val) & 0x3) << 4)
#define DMA_PCTRL_TXBL_GET(val)   ((((val) & DMA_PCTRL_TXBL) >> 4) & 0x3)
#define DMA_PCTRL_TXBL_SET(reg,val) (reg) = ((reg & ~DMA_PCTRL_TXBL) | (((val) & 0x3) << 4))
/* Burst Length for Receive Direction (3:2) */
#define DMA_PCTRL_RXBL   (0x3 << 2)
#define DMA_PCTRL_RXBL_VAL(val)   (((val) & 0x3) << 2)
#define DMA_PCTRL_RXBL_GET(val)   ((((val) & DMA_PCTRL_RXBL) >> 2) & 0x3)
#define DMA_PCTRL_RXBL_SET(reg,val) (reg) = ((reg & ~DMA_PCTRL_RXBL) | (((val) & 0x3) << 2))

/*******************************************************************************
 * DMA_IRNEN Register
 ******************************************************************************/

/* Channel x Interrupt Request Enable (23) */
#define DMA_IRNEN_CH23   (0x1 << 23)
#define DMA_IRNEN_CH23_VAL(val)   (((val) & 0x1) << 23)
#define DMA_IRNEN_CH23_GET(val)   ((((val) & DMA_IRNEN_CH23) >> 23) & 0x1)
#define DMA_IRNEN_CH23_SET(reg,val) (reg) = ((reg & ~DMA_IRNEN_CH23) | (((val) & 0x1) << 23))

/*******************************************************************************
 * DMA_IRNCR Register
 ******************************************************************************/

/* Channel x Interrupt (23) */
#define DMA_IRNCR_CH23   (0x1 << 23)
#define DMA_IRNCR_CH23_GET(val)   ((((val) & DMA_IRNCR_CH23) >> 23) & 0x1)

/*******************************************************************************
 * DMA_IRNICR Register
 ******************************************************************************/

/* Channel x Interrupt Request (23) */
#define DMA_IRNICR_CH23   (0x1 << 23)
#define DMA_IRNICR_CH23_GET(val)   ((((val) & DMA_IRNICR_CH23) >> 23) & 0x1)

#endif
