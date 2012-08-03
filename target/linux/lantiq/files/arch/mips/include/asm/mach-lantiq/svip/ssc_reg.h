/******************************************************************************

  Copyright (c) 2007
  Infineon Technologies AG
  St. Martin Strasse 53; 81669 Munich, Germany

  Any use of this Software is subject to the conclusion of a respective
  License Agreement. Without such a License Agreement no rights to the
  Software are granted.

 ******************************************************************************/

#ifndef __SSC_REG_H
#define __SSC_REG_H

/** SSC register structure */
struct svip_reg_ssc {
	volatile unsigned long  clc;  /*  0x00 */
	volatile unsigned long  pisel;  /*  0x04 */
	volatile unsigned long  id;  /*  0x08 */
	volatile unsigned long  reserved0;  /*  0x0c */
	volatile unsigned long  mcon;  /*  0x10 */
	volatile unsigned long  state;  /*  0x14 */
	volatile unsigned long  whbstate;  /*  0x18 */
	volatile unsigned long  reserved1;  /*  0x1c */
	volatile unsigned long  tb;  /*  0x20 */
	volatile unsigned long  rb;  /*  0x24 */
	volatile unsigned long  reserved2[2];  /*  0x28 */
	volatile unsigned long  rxfcon;  /*  0x30 */
	volatile unsigned long  txfcon;  /*  0x34 */
	volatile unsigned long  fstat;  /*  0x38 */
	volatile unsigned long  reserved3;  /*  0x3c */
	volatile unsigned long  br;  /*  0x40 */
	volatile unsigned long  brstat;  /*  0x44 */
	volatile unsigned long  reserved4[6];  /*  0x48 */
	volatile unsigned long  sfcon;  /*  0x60 */
	volatile unsigned long  sfstat;  /*  0x64 */
	volatile unsigned long  reserved5[2];  /*  0x68 */
	volatile unsigned long  gpocon;  /*  0x70 */
	volatile unsigned long  gpostat;  /*  0x74 */
	volatile unsigned long  whbgpostat;  /*  0x78 */
	volatile unsigned long  reserved6;  /*  0x7c */
	volatile unsigned long  rxreq;  /*  0x80 */
	volatile unsigned long  rxcnt;  /*  0x84 */
	volatile unsigned long  reserved7[25];  /*  0x88 */
	volatile unsigned long  dma_con;  /*  0xEC */
	volatile unsigned long  reserved8;  /*  0xf0 */
	volatile unsigned long  irnen;  /*  0xF4 */
	volatile unsigned long  irncr;  /*  0xF8 */
	volatile unsigned long  irnicr;  /*  0xFC */
};

/*******************************************************************************
 * CLC Register
 ******************************************************************************/

/* Clock Divider for Sleep Mode (23:16) */
#define SSC_CLC_SMC   (0xff << 16)
#define SSC_CLC_SMC_VAL(val)   (((val) & 0xff) << 16)
#define SSC_CLC_SMC_GET(val)   ((((val) & SSC_CLC_SMC) >> 16) & 0xff)
#define SSC_CLC_SMC_SET(reg,val) (reg) = ((reg & ~SSC_CLC_SMC) | (((val) & 0xff) << 16))
/* Clock Divider for Normal Run Mode (15:8) */
#define SSC_CLC_RMC   (0xff << 8)
#define SSC_CLC_RMC_VAL(val)   (((val) & 0xff) << 8)
#define SSC_CLC_RMC_GET(val)   ((((val) & SSC_CLC_RMC) >> 8) & 0xff)
#define SSC_CLC_RMC_SET(reg,val) (reg) = ((reg & ~SSC_CLC_RMC) | (((val) & 0xff) << 8))
/* Fast Shut-Off Enable Bit (5) */
#define SSC_CLC_FSOE   (0x1 << 5)
#define SSC_CLC_FSOE_VAL(val)   (((val) & 0x1) << 5)
#define SSC_CLC_FSOE_GET(val)   ((((val) & SSC_CLC_FSOE) >> 5) & 0x1)
#define SSC_CLC_FSOE_SET(reg,val) (reg) = ((reg & ~SSC_CLC_FSOE) | (((val) & 0x1) << 5))
/* Suspend Bit Write Enable for OCDS (4) */
#define SSC_CLC_SBWE   (0x1 << 4)
#define SSC_CLC_SBWE_VAL(val)   (((val) & 0x1) << 4)
#define SSC_CLC_SBWE_SET(reg,val) (reg) = (((reg & ~SSC_CLC_SBWE) | (val) & 1) << 4)
/* External Request Disable (3) */
#define SSC_CLC_EDIS   (0x1 << 3)
#define SSC_CLC_EDIS_VAL(val)   (((val) & 0x1) << 3)
#define SSC_CLC_EDIS_GET(val)   ((((val) & SSC_CLC_EDIS) >> 3) & 0x1)
#define SSC_CLC_EDIS_SET(reg,val) (reg) = ((reg & ~SSC_CLC_EDIS) | (((val) & 0x1) << 3))
/* Suspend Enable Bit for OCDS (2) */
#define SSC_CLC_SPEN   (0x1 << 2)
#define SSC_CLC_SPEN_VAL(val)   (((val) & 0x1) << 2)
#define SSC_CLC_SPEN_GET(val)   ((((val) & SSC_CLC_SPEN) >> 2) & 0x1)
#define SSC_CLC_SPEN_SET(reg,val) (reg) = ((reg & ~SSC_CLC_SPEN) | (((val) & 0x1) << 2))
/* Disable Status Bit (1) */
#define SSC_CLC_DISS   (0x1 << 1)
#define SSC_CLC_DISS_GET(val)   ((((val) & SSC_CLC_DISS) >> 1) & 0x1)
/* Disable Request Bit (0) */
#define SSC_CLC_DISR   (0x1)
#define SSC_CLC_DISR_VAL(val)   (((val) & 0x1) << 0)
#define SSC_CLC_DISR_GET(val)   ((((val) & SSC_CLC_DISR) >> 0) & 0x1)
#define SSC_CLC_DISR_SET(reg,val) (reg) = ((reg & ~SSC_CLC_DISR) | (((val) & 0x1) << 0))

/*******************************************************************************
 * ID Register
 ******************************************************************************/

/* Transmit FIFO Size (29:24) */
#define SSC_ID_TXFS   (0x3f << 24)
#define SSC_ID_TXFS_GET(val)   ((((val) & SSC_ID_TXFS) >> 24) & 0x3f)
/* Receive FIFO Size (21:16) */
#define SSC_ID_RXFS   (0x3f << 16)
#define SSC_ID_RXFS_GET(val)   ((((val) & SSC_ID_RXFS) >> 16) & 0x3f)
/* Module ID (15:8) */
#define SSC_ID_ID   (0xff << 8)
#define SSC_ID_ID_GET(val)   ((((val) & SSC_ID_ID) >> 8) & 0xff)
/* Configuration (5) */
#define SSC_ID_CFG   (0x1 << 5)
#define SSC_ID_CFG_GET(val)   ((((val) & SSC_ID_CFG) >> 5) & 0x1)
/* Revision (4:0) */
#define SSC_ID_REV   (0x1f)
#define SSC_ID_REV_GET(val)   ((((val) & SSC_ID_REV) >> 0) & 0x1f)

/*******************************************************************************
 * MCON Register
 ******************************************************************************/

/* Echo Mode (24) */
#define SSC_MCON_EM   (0x1 << 24)
#define SSC_MCON_EM_VAL(val)   (((val) & 0x1) << 24)
#define SSC_MCON_EM_GET(val)   ((((val) & SSC_MCON_EM) >> 24) & 0x1)
#define SSC_MCON_EM_SET(reg,val) (reg) = ((reg & ~SSC_MCON_EM) | (((val) & 0x1) << 24))
/* Idle Bit Value (23) */
#define SSC_MCON_IDLE   (0x1 << 23)
#define SSC_MCON_IDLE_VAL(val)   (((val) & 0x1) << 23)
#define SSC_MCON_IDLE_GET(val)   ((((val) & SSC_MCON_IDLE) >> 23) & 0x1)
#define SSC_MCON_IDLE_SET(reg,val) (reg) = ((reg & ~SSC_MCON_IDLE) | (((val) & 0x1) << 23))
/* Enable Byte Valid Control (22) */
#define SSC_MCON_ENBV   (0x1 << 22)
#define SSC_MCON_ENBV_VAL(val)   (((val) & 0x1) << 22)
#define SSC_MCON_ENBV_GET(val)   ((((val) & SSC_MCON_ENBV) >> 22) & 0x1)
#define SSC_MCON_ENBV_SET(reg,val) (reg) = ((reg & ~SSC_MCON_ENBV) | (((val) & 0x1) << 22))
/* Data Width Selection (20:16) */
#define SSC_MCON_BM   (0x1f << 16)
#define SSC_MCON_BM_VAL(val)   (((val) & 0x1f) << 16)
#define SSC_MCON_BM_GET(val)   ((((val) & SSC_MCON_BM) >> 16) & 0x1f)
#define SSC_MCON_BM_SET(reg,val) (reg) = ((reg & ~SSC_MCON_BM) | (((val) & 0x1f) << 16))
/* Receive Underflow Error Enable (12) */
#define SSC_MCON_RUEN   (0x1 << 12)
#define SSC_MCON_RUEN_VAL(val)   (((val) & 0x1) << 12)
#define SSC_MCON_RUEN_GET(val)   ((((val) & SSC_MCON_RUEN) >> 12) & 0x1)
#define SSC_MCON_RUEN_SET(reg,val) (reg) = ((reg & ~SSC_MCON_RUEN) | (((val) & 0x1) << 12))
/* Transmit Underflow Error Enable (11) */
#define SSC_MCON_TUEN   (0x1 << 11)
#define SSC_MCON_TUEN_VAL(val)   (((val) & 0x1) << 11)
#define SSC_MCON_TUEN_GET(val)   ((((val) & SSC_MCON_TUEN) >> 11) & 0x1)
#define SSC_MCON_TUEN_SET(reg,val) (reg) = ((reg & ~SSC_MCON_TUEN) | (((val) & 0x1) << 11))
/* Abort Error Enable (10) */
#define SSC_MCON_AEN   (0x1 << 10)
#define SSC_MCON_AEN_VAL(val)   (((val) & 0x1) << 10)
#define SSC_MCON_AEN_GET(val)   ((((val) & SSC_MCON_AEN) >> 10) & 0x1)
#define SSC_MCON_AEN_SET(reg,val) (reg) = ((reg & ~SSC_MCON_AEN) | (((val) & 0x1) << 10))
/* Receive Overflow Error Enable (9) */
#define SSC_MCON_REN   (0x1 << 9)
#define SSC_MCON_REN_VAL(val)   (((val) & 0x1) << 9)
#define SSC_MCON_REN_GET(val)   ((((val) & SSC_MCON_REN) >> 9) & 0x1)
#define SSC_MCON_REN_SET(reg,val) (reg) = ((reg & ~SSC_MCON_REN) | (((val) & 0x1) << 9))
/* Transmit Overflow Error Enable (8) */
#define SSC_MCON_TEN   (0x1 << 8)
#define SSC_MCON_TEN_VAL(val)   (((val) & 0x1) << 8)
#define SSC_MCON_TEN_GET(val)   ((((val) & SSC_MCON_TEN) >> 8) & 0x1)
#define SSC_MCON_TEN_SET(reg,val) (reg) = ((reg & ~SSC_MCON_TEN) | (((val) & 0x1) << 8))
/* Loop Back Control (7) */
#define SSC_MCON_LB   (0x1 << 7)
#define SSC_MCON_LB_VAL(val)   (((val) & 0x1) << 7)
#define SSC_MCON_LB_GET(val)   ((((val) & SSC_MCON_LB) >> 7) & 0x1)
#define SSC_MCON_LB_SET(reg,val) (reg) = ((reg & ~SSC_MCON_LB) | (((val) & 0x1) << 7))
/* Clock Polarity Control (6) */
#define SSC_MCON_PO   (0x1 << 6)
#define SSC_MCON_PO_VAL(val)   (((val) & 0x1) << 6)
#define SSC_MCON_PO_GET(val)   ((((val) & SSC_MCON_PO) >> 6) & 0x1)
#define SSC_MCON_PO_SET(reg,val) (reg) = ((reg & ~SSC_MCON_PO) | (((val) & 0x1) << 6))
/* Clock Phase Control (5) */
#define SSC_MCON_PH   (0x1 << 5)
#define SSC_MCON_PH_VAL(val)   (((val) & 0x1) << 5)
#define SSC_MCON_PH_GET(val)   ((((val) & SSC_MCON_PH) >> 5) & 0x1)
#define SSC_MCON_PH_SET(reg,val) (reg) = ((reg & ~SSC_MCON_PH) | (((val) & 0x1) << 5))
/* Heading Control (4) */
#define SSC_MCON_HB   (0x1 << 4)
#define SSC_MCON_HB_VAL(val)   (((val) & 0x1) << 4)
#define SSC_MCON_HB_GET(val)   ((((val) & SSC_MCON_HB) >> 4) & 0x1)
#define SSC_MCON_HB_SET(reg,val) (reg) = ((reg & ~SSC_MCON_HB) | (((val) & 0x1) << 4))
/* Chip Select Enable (3) */
#define SSC_MCON_CSBEN   (0x1 << 3)
#define SSC_MCON_CSBEN_VAL(val)   (((val) & 0x1) << 3)
#define SSC_MCON_CSBEN_GET(val)   ((((val) & SSC_MCON_CSBEN) >> 3) & 0x1)
#define SSC_MCON_CSBEN_SET(reg,val) (reg) = ((reg & ~SSC_MCON_CSBEN) | (((val) & 0x1) << 3))
/* Chip Select Invert (2) */
#define SSC_MCON_CSBINV   (0x1 << 2)
#define SSC_MCON_CSBINV_VAL(val)   (((val) & 0x1) << 2)
#define SSC_MCON_CSBINV_GET(val)   ((((val) & SSC_MCON_CSBINV) >> 2) & 0x1)
#define SSC_MCON_CSBINV_SET(reg,val) (reg) = ((reg & ~SSC_MCON_CSBINV) | (((val) & 0x1) << 2))
/* Receive Off (1) */
#define SSC_MCON_RXOFF   (0x1 << 1)
#define SSC_MCON_RXOFF_VAL(val)   (((val) & 0x1) << 1)
#define SSC_MCON_RXOFF_GET(val)   ((((val) & SSC_MCON_RXOFF) >> 1) & 0x1)
#define SSC_MCON_RXOFF_SET(reg,val) (reg) = ((reg & ~SSC_MCON_RXOFF) | (((val) & 0x1) << 1))
/* Transmit Off (0) */
#define SSC_MCON_TXOFF   (0x1)
#define SSC_MCON_TXOFF_VAL(val)   (((val) & 0x1) << 0)
#define SSC_MCON_TXOFF_GET(val)   ((((val) & SSC_MCON_TXOFF) >> 0) & 0x1)
#define SSC_MCON_TXOFF_SET(reg,val) (reg) = ((reg & ~SSC_MCON_TXOFF) | (((val) & 0x1) << 0))

/*******************************************************************************
 * STATE Register
 ******************************************************************************/

/* Receive End-of-Message (31) */
#define SSC_STATE_RXEOM   (0x1 << 31)
#define SSC_STATE_RXEOM_GET(val)   ((((val) & SSC_STATE_RXEOM) >> 31) & 0x1)
/* Receive Byte Valid (30:28) */
#define SSC_STATE_RXBV   (0x7 << 28)
#define SSC_STATE_RXBV_GET(val)   ((((val) & SSC_STATE_RXBV) >> 28) & 0x7)
/* Transmit End-of-Message (27) */
#define SSC_STATE_TXEOM   (0x1 << 27)
#define SSC_STATE_TXEOM_GET(val)   ((((val) & SSC_STATE_TXEOM) >> 27) & 0x1)
/* Transmit Byte Valid (26:24) */
#define SSC_STATE_TXBV   (0x7 << 24)
#define SSC_STATE_TXBV_GET(val)   ((((val) & SSC_STATE_TXBV) >> 24) & 0x7)
/* Bit Count Field (20:16) */
#define SSC_STATE_BC   (0x1f << 16)
#define SSC_STATE_BC_GET(val)   ((((val) & SSC_STATE_BC) >> 16) & 0x1f)
/* Busy Flag (13) */
#define SSC_STATE_BSY   (0x1 << 13)
#define SSC_STATE_BSY_GET(val)   ((((val) & SSC_STATE_BSY) >> 13) & 0x1)
/* Receive Underflow Error Flag (12) */
#define SSC_STATE_RUE   (0x1 << 12)
#define SSC_STATE_RUE_GET(val)   ((((val) & SSC_STATE_RUE) >> 12) & 0x1)
/* Transmit Underflow Error Flag (11) */
#define SSC_STATE_TUE   (0x1 << 11)
#define SSC_STATE_TUE_GET(val)   ((((val) & SSC_STATE_TUE) >> 11) & 0x1)
/* Abort Error Flag (10) */
#define SSC_STATE_AE   (0x1 << 10)
#define SSC_STATE_AE_GET(val)   ((((val) & SSC_STATE_AE) >> 10) & 0x1)
/* Receive Error Flag (9) */
#define SSC_STATE_RE   (0x1 << 9)
#define SSC_STATE_RE_GET(val)   ((((val) & SSC_STATE_RE) >> 9) & 0x1)
/* Transmit Error Flag (8) */
#define SSC_STATE_TE   (0x1 << 8)
#define SSC_STATE_TE_GET(val)   ((((val) & SSC_STATE_TE) >> 8) & 0x1)
/* Mode Error Flag (7) */
#define SSC_STATE_ME   (0x1 << 7)
#define SSC_STATE_ME_GET(val)   ((((val) & SSC_STATE_ME) >> 7) & 0x1)
/* Slave Selected (2) */
#define SSC_STATE_SSEL   (0x1 << 2)
#define SSC_STATE_SSEL_GET(val)   ((((val) & SSC_STATE_SSEL) >> 2) & 0x1)
/* Master Select Bit (1) */
#define SSC_STATE_MS   (0x1 << 1)
#define SSC_STATE_MS_GET(val)   ((((val) & SSC_STATE_MS) >> 1) & 0x1)
/* Enable Bit (0) */
#define SSC_STATE_EN   (0x1)
#define SSC_STATE_EN_GET(val)   ((((val) & SSC_STATE_EN) >> 0) & 0x1)

/*******************************************************************************
 * WHBSTATE Register
 ******************************************************************************/

/* Set Transmit Underflow Error Flag Bit (15) */
#define SSC_WHBSTATE_SETTUE   (0x1 << 15)
#define SSC_WHBSTATE_SETTUE_VAL(val)   (((val) & 0x1) << 15)
#define SSC_WHBSTATE_SETTUE_SET(reg,val) (reg) = (((reg & ~SSC_WHBSTATE_SETTUE) | (val) & 1) << 15)
/* Set Abort Error Flag Bit (14) */
#define SSC_WHBSTATE_SETAE   (0x1 << 14)
#define SSC_WHBSTATE_SETAE_VAL(val)   (((val) & 0x1) << 14)
#define SSC_WHBSTATE_SETAE_SET(reg,val) (reg) = (((reg & ~SSC_WHBSTATE_SETAE) | (val) & 1) << 14)
/* Set Receive Error Flag Bit (13) */
#define SSC_WHBSTATE_SETRE   (0x1 << 13)
#define SSC_WHBSTATE_SETRE_VAL(val)   (((val) & 0x1) << 13)
#define SSC_WHBSTATE_SETRE_SET(reg,val) (reg) = (((reg & ~SSC_WHBSTATE_SETRE) | (val) & 1) << 13)
/* Set Transmit Error Flag Bit (12) */
#define SSC_WHBSTATE_SETTE   (0x1 << 12)
#define SSC_WHBSTATE_SETTE_VAL(val)   (((val) & 0x1) << 12)
#define SSC_WHBSTATE_SETTE_SET(reg,val) (reg) = (((reg & ~SSC_WHBSTATE_SETTE) | (val) & 1) << 12)
/* Clear Transmit Underflow Error Flag Bit (11) */
#define SSC_WHBSTATE_CLRTUE   (0x1 << 11)
#define SSC_WHBSTATE_CLRTUE_VAL(val)   (((val) & 0x1) << 11)
#define SSC_WHBSTATE_CLRTUE_SET(reg,val) (reg) = (((reg & ~SSC_WHBSTATE_CLRTUE) | (val) & 1) << 11)
/* Clear Abort Error Flag Bit (10) */
#define SSC_WHBSTATE_CLRAE   (0x1 << 10)
#define SSC_WHBSTATE_CLRAE_VAL(val)   (((val) & 0x1) << 10)
#define SSC_WHBSTATE_CLRAE_SET(reg,val) (reg) = (((reg & ~SSC_WHBSTATE_CLRAE) | (val) & 1) << 10)
/* Clear Receive Error Flag Bit (9) */
#define SSC_WHBSTATE_CLRRE   (0x1 << 9)
#define SSC_WHBSTATE_CLRRE_VAL(val)   (((val) & 0x1) << 9)
#define SSC_WHBSTATE_CLRRE_SET(reg,val) (reg) = (((reg & ~SSC_WHBSTATE_CLRRE) | (val) & 1) << 9)
/* Clear Transmit Error Flag Bit (8) */
#define SSC_WHBSTATE_CLRTE   (0x1 << 8)
#define SSC_WHBSTATE_CLRTE_VAL(val)   (((val) & 0x1) << 8)
#define SSC_WHBSTATE_CLRTE_SET(reg,val) (reg) = (((reg & ~SSC_WHBSTATE_CLRTE) | (val) & 1) << 8)
/* Set Mode Error Flag Bit (7) */
#define SSC_WHBSTATE_SETME   (0x1 << 7)
#define SSC_WHBSTATE_SETME_VAL(val)   (((val) & 0x1) << 7)
#define SSC_WHBSTATE_SETME_SET(reg,val) (reg) = (((reg & ~SSC_WHBSTATE_SETME) | (val) & 1) << 7)
/* Clear Mode Error Flag Bit (6) */
#define SSC_WHBSTATE_CLRME   (0x1 << 6)
#define SSC_WHBSTATE_CLRME_VAL(val)   (((val) & 0x1) << 6)
#define SSC_WHBSTATE_CLRME_SET(reg,val) (reg) = (((reg & ~SSC_WHBSTATE_CLRME) | (val) & 1) << 6)
/* Set Receive Underflow Error Bit (5) */
#define SSC_WHBSTATE_SETRUE   (0x1 << 5)
#define SSC_WHBSTATE_SETRUE_VAL(val)   (((val) & 0x1) << 5)
#define SSC_WHBSTATE_SETRUE_SET(reg,val) (reg) = (((reg & ~SSC_WHBSTATE_SETRUE) | (val) & 1) << 5)
/* Clear Receive Underflow Error Bit (4) */
#define SSC_WHBSTATE_CLRRUE   (0x1 << 4)
#define SSC_WHBSTATE_CLRRUE_VAL(val)   (((val) & 0x1) << 4)
#define SSC_WHBSTATE_CLRRUE_SET(reg,val) (reg) = (((reg & ~SSC_WHBSTATE_CLRRUE) | (val) & 1) << 4)
/* Set Master Select Bit (3) */
#define SSC_WHBSTATE_SETMS   (0x1 << 3)
#define SSC_WHBSTATE_SETMS_VAL(val)   (((val) & 0x1) << 3)
#define SSC_WHBSTATE_SETMS_SET(reg,val) (reg) = (((reg & ~SSC_WHBSTATE_SETMS) | (val) & 1) << 3)
/* Clear Master Select Bit (2) */
#define SSC_WHBSTATE_CLRMS   (0x1 << 2)
#define SSC_WHBSTATE_CLRMS_VAL(val)   (((val) & 0x1) << 2)
#define SSC_WHBSTATE_CLRMS_SET(reg,val) (reg) = (((reg & ~SSC_WHBSTATE_CLRMS) | (val) & 1) << 2)
/* Set Enable Bit (1) */
#define SSC_WHBSTATE_SETEN   (0x1 << 1)
#define SSC_WHBSTATE_SETEN_VAL(val)   (((val) & 0x1) << 1)
#define SSC_WHBSTATE_SETEN_SET(reg,val) (reg) = (((reg & ~SSC_WHBSTATE_SETEN) | (val) & 1) << 1)
/* Clear Enable Bit (0) */
#define SSC_WHBSTATE_CLREN   (0x1)
#define SSC_WHBSTATE_CLREN_VAL(val)   (((val) & 0x1) << 0)
#define SSC_WHBSTATE_CLREN_SET(reg,val) (reg) = (((reg & ~SSC_WHBSTATE_CLREN) | (val) & 1) << 0)

/*******************************************************************************
 * TB Register
 ******************************************************************************/

/* Transmit Data Register Value (31:0) */
#define SSC_TB_TB_VAL   (0xFFFFFFFFL)
#define SSC_TB_TB_VAL_VAL(val)   (((val) & 0xFFFFFFFFL) << 0)
#define SSC_TB_TB_VAL_GET(val)   ((((val) & SSC_TB_TB_VAL) >> 0) & 0xFFFFFFFFL)
#define SSC_TB_TB_VAL_SET(reg,val) (reg) = ((reg & ~SSC_TB_TB_VAL) | (((val) & 0xFFFFFFFFL) << 0))

/*******************************************************************************
 * RB Register
 ******************************************************************************/

/* Receive Data Register Value (31:0) */
#define SSC_RB_RB_VAL   (0xFFFFFFFFL)
#define SSC_RB_RB_VAL_GET(val)   ((((val) & SSC_RB_RB_VAL) >> 0) & 0xFFFFFFFFL)

/*******************************************************************************
 * FSTAT Register
 ******************************************************************************/

/* Transmit FIFO Filling Level (13:8) */
#define SSC_FSTAT_TXFFL   (0x3f << 8)
#define SSC_FSTAT_TXFFL_GET(val)   ((((val) & SSC_FSTAT_TXFFL) >> 8) & 0x3f)
/* Receive FIFO Filling Level (5:0) */
#define SSC_FSTAT_RXFFL   (0x3f)
#define SSC_FSTAT_RXFFL_GET(val)   ((((val) & SSC_FSTAT_RXFFL) >> 0) & 0x3f)

/*******************************************************************************
 * PISEL Register
 ******************************************************************************/

/* Slave Mode Clock Input Select (2) */
#define SSC_PISEL_CIS   (0x1 << 2)
#define SSC_PISEL_CIS_VAL(val)   (((val) & 0x1) << 2)
#define SSC_PISEL_CIS_GET(val)   ((((val) & SSC_PISEL_CIS) >> 2) & 0x1)
#define SSC_PISEL_CIS_SET(reg,val) (reg) = ((reg & ~SSC_PISEL_CIS) | (((val) & 0x1) << 2))
/* Slave Mode Receiver Input Select (1) */
#define SSC_PISEL_SIS   (0x1 << 1)
#define SSC_PISEL_SIS_VAL(val)   (((val) & 0x1) << 1)
#define SSC_PISEL_SIS_GET(val)   ((((val) & SSC_PISEL_SIS) >> 1) & 0x1)
#define SSC_PISEL_SIS_SET(reg,val) (reg) = ((reg & ~SSC_PISEL_SIS) | (((val) & 0x1) << 1))
/* Master Mode Receiver Input Select (0) */
#define SSC_PISEL_MIS   (0x1)
#define SSC_PISEL_MIS_VAL(val)   (((val) & 0x1) << 0)
#define SSC_PISEL_MIS_GET(val)   ((((val) & SSC_PISEL_MIS) >> 0) & 0x1)
#define SSC_PISEL_MIS_SET(reg,val) (reg) = ((reg & ~SSC_PISEL_MIS) | (((val) & 0x1) << 0))

/*******************************************************************************
 * RXFCON Register
 ******************************************************************************/

/* Receive FIFO Interrupt Trigger Level (13:8) */
#define SSC_RXFCON_RXFITL   (0x3f << 8)
#define SSC_RXFCON_RXFITL_VAL(val)   (((val) & 0x3f) << 8)
#define SSC_RXFCON_RXFITL_GET(val)   ((((val) & SSC_RXFCON_RXFITL) >> 8) & 0x3f)
#define SSC_RXFCON_RXFITL_SET(reg,val) (reg) = ((reg & ~SSC_RXFCON_RXFITL) | (((val) & 0x3f) << 8))
/* Receive FIFO Flush (1) */
#define SSC_RXFCON_RXFLU   (0x1 << 1)
#define SSC_RXFCON_RXFLU_VAL(val)   (((val) & 0x1) << 1)
#define SSC_RXFCON_RXFLU_SET(reg,val) (reg) = (((reg & ~SSC_RXFCON_RXFLU) | (val) & 1) << 1)
/* Receive FIFO Enable (0) */
#define SSC_RXFCON_RXFEN   (0x1)
#define SSC_RXFCON_RXFEN_VAL(val)   (((val) & 0x1) << 0)
#define SSC_RXFCON_RXFEN_GET(val)   ((((val) & SSC_RXFCON_RXFEN) >> 0) & 0x1)
#define SSC_RXFCON_RXFEN_SET(reg,val) (reg) = ((reg & ~SSC_RXFCON_RXFEN) | (((val) & 0x1) << 0))

/*******************************************************************************
 * TXFCON Register
 ******************************************************************************/

/* Transmit FIFO Interrupt Trigger Level (13:8) */
#define SSC_TXFCON_TXFITL   (0x3f << 8)
#define SSC_TXFCON_TXFITL_VAL(val)   (((val) & 0x3f) << 8)
#define SSC_TXFCON_TXFITL_GET(val)   ((((val) & SSC_TXFCON_TXFITL) >> 8) & 0x3f)
#define SSC_TXFCON_TXFITL_SET(reg,val) (reg) = ((reg & ~SSC_TXFCON_TXFITL) | (((val) & 0x3f) << 8))
/* Transmit FIFO Flush (1) */
#define SSC_TXFCON_TXFLU   (0x1 << 1)
#define SSC_TXFCON_TXFLU_VAL(val)   (((val) & 0x1) << 1)
#define SSC_TXFCON_TXFLU_SET(reg,val) (reg) = (((reg & ~SSC_TXFCON_TXFLU) | (val) & 1) << 1)
/* Transmit FIFO Enable (0) */
#define SSC_TXFCON_TXFEN   (0x1)
#define SSC_TXFCON_TXFEN_VAL(val)   (((val) & 0x1) << 0)
#define SSC_TXFCON_TXFEN_GET(val)   ((((val) & SSC_TXFCON_TXFEN) >> 0) & 0x1)
#define SSC_TXFCON_TXFEN_SET(reg,val) (reg) = ((reg & ~SSC_TXFCON_TXFEN) | (((val) & 0x1) << 0))

/*******************************************************************************
 * BR Register
 ******************************************************************************/

/* Baudrate Timer Reload Register Value (15:0) */
#define SSC_BR_BR_VAL   (0xffff)
#define SSC_BR_BR_VAL_VAL(val)   (((val) & 0xffff) << 0)
#define SSC_BR_BR_VAL_GET(val)   ((((val) & SSC_BR_BR_VAL) >> 0) & 0xffff)
#define SSC_BR_BR_VAL_SET(reg,val) (reg) = ((reg & ~SSC_BR_BR_VAL) | (((val) & 0xffff) << 0))

/*******************************************************************************
 * BRSTAT Register
 ******************************************************************************/

/* Baudrate Timer Register Value (15:0) */
#define SSC_BRSTAT_BT_VAL   (0xffff)
#define SSC_BRSTAT_BT_VAL_GET(val)   ((((val) & SSC_BRSTAT_BT_VAL) >> 0) & 0xffff)

/*******************************************************************************
 * SFCON Register
 ******************************************************************************/

/* Pause Length (31:22) */
#define SSC_SFCON_PLEN   (0x3ff << 22)
#define SSC_SFCON_PLEN_VAL(val)   (((val) & 0x3ff) << 22)
#define SSC_SFCON_PLEN_GET(val)   ((((val) & SSC_SFCON_PLEN) >> 22) & 0x3ff)
#define SSC_SFCON_PLEN_SET(reg,val) (reg) = ((reg & ~SSC_SFCON_PLEN) | (((val) & 0x3ff) << 22))
/* Stop After Pause (20) */
#define SSC_SFCON_STOP   (0x1 << 20)
#define SSC_SFCON_STOP_VAL(val)   (((val) & 0x1) << 20)
#define SSC_SFCON_STOP_GET(val)   ((((val) & SSC_SFCON_STOP) >> 20) & 0x1)
#define SSC_SFCON_STOP_SET(reg,val) (reg) = ((reg & ~SSC_SFCON_STOP) | (((val) & 0x1) << 20))
/* Idle Clock Configuration (19:18) */
#define SSC_SFCON_ICLK   (0x3 << 18)
#define SSC_SFCON_ICLK_VAL(val)   (((val) & 0x3) << 18)
#define SSC_SFCON_ICLK_GET(val)   ((((val) & SSC_SFCON_ICLK) >> 18) & 0x3)
#define SSC_SFCON_ICLK_SET(reg,val) (reg) = ((reg & ~SSC_SFCON_ICLK) | (((val) & 0x3) << 18))
/* Idle Data Configuration (17:16) */
#define SSC_SFCON_IDAT   (0x3 << 16)
#define SSC_SFCON_IDAT_VAL(val)   (((val) & 0x3) << 16)
#define SSC_SFCON_IDAT_GET(val)   ((((val) & SSC_SFCON_IDAT) >> 16) & 0x3)
#define SSC_SFCON_IDAT_SET(reg,val) (reg) = ((reg & ~SSC_SFCON_IDAT) | (((val) & 0x3) << 16))
/* Data Length (15:4) */
#define SSC_SFCON_DLEN   (0xfff << 4)
#define SSC_SFCON_DLEN_VAL(val)   (((val) & 0xfff) << 4)
#define SSC_SFCON_DLEN_GET(val)   ((((val) & SSC_SFCON_DLEN) >> 4) & 0xfff)
#define SSC_SFCON_DLEN_SET(reg,val) (reg) = ((reg & ~SSC_SFCON_DLEN) | (((val) & 0xfff) << 4))
/* Enable Interrupt After Pause (3) */
#define SSC_SFCON_IAEN   (0x1 << 3)
#define SSC_SFCON_IAEN_VAL(val)   (((val) & 0x1) << 3)
#define SSC_SFCON_IAEN_GET(val)   ((((val) & SSC_SFCON_IAEN) >> 3) & 0x1)
#define SSC_SFCON_IAEN_SET(reg,val) (reg) = ((reg & ~SSC_SFCON_IAEN) | (((val) & 0x1) << 3))
/* Enable Interrupt Before Pause (2) */
#define SSC_SFCON_IBEN   (0x1 << 2)
#define SSC_SFCON_IBEN_VAL(val)   (((val) & 0x1) << 2)
#define SSC_SFCON_IBEN_GET(val)   ((((val) & SSC_SFCON_IBEN) >> 2) & 0x1)
#define SSC_SFCON_IBEN_SET(reg,val) (reg) = ((reg & ~SSC_SFCON_IBEN) | (((val) & 0x1) << 2))
/* Serial Frame Enable (0) */
#define SSC_SFCON_SFEN   (0x1)
#define SSC_SFCON_SFEN_VAL(val)   (((val) & 0x1) << 0)
#define SSC_SFCON_SFEN_GET(val)   ((((val) & SSC_SFCON_SFEN) >> 0) & 0x1)
#define SSC_SFCON_SFEN_SET(reg,val) (reg) = ((reg & ~SSC_SFCON_SFEN) | (((val) & 0x1) << 0))

/*******************************************************************************
 * SFSTAT Register
 ******************************************************************************/

/* Pause Count (31:22) */
#define SSC_SFSTAT_PCNT   (0x3ff << 22)
#define SSC_SFSTAT_PCNT_GET(val)   ((((val) & SSC_SFSTAT_PCNT) >> 22) & 0x3ff)
/* Data Bit Count (15:4) */
#define SSC_SFSTAT_DCNT   (0xfff << 4)
#define SSC_SFSTAT_DCNT_GET(val)   ((((val) & SSC_SFSTAT_DCNT) >> 4) & 0xfff)
/* Pause Busy (1) */
#define SSC_SFSTAT_PBSY   (0x1 << 1)
#define SSC_SFSTAT_PBSY_GET(val)   ((((val) & SSC_SFSTAT_PBSY) >> 1) & 0x1)
/* Data Busy (0) */
#define SSC_SFSTAT_DBSY   (0x1)
#define SSC_SFSTAT_DBSY_GET(val)   ((((val) & SSC_SFSTAT_DBSY) >> 0) & 0x1)

/*******************************************************************************
 * GPOCON Register
 ******************************************************************************/

/* Output OUTn Is Chip Select (15:8) */
#define SSC_GPOCON_ISCSBN   (0xff << 8)
#define SSC_GPOCON_ISCSBN_VAL(val)   (((val) & 0xff) << 8)
#define SSC_GPOCON_ISCSBN_GET(val)   ((((val) & SSC_GPOCON_ISCSBN) >> 8) & 0xff)
#define SSC_GPOCON_ISCSBN_SET(reg,val) (reg) = ((reg & ~SSC_GPOCON_ISCSBN) | (((val) & 0xff) << 8))
/* Invert Output OUTn (7:0) */
#define SSC_GPOCON_INVOUTN   (0xff)
#define SSC_GPOCON_INVOUTN_VAL(val)   (((val) & 0xff) << 0)
#define SSC_GPOCON_INVOUTN_GET(val)   ((((val) & SSC_GPOCON_INVOUTN) >> 0) & 0xff)
#define SSC_GPOCON_INVOUTN_SET(reg,val) (reg) = ((reg & ~SSC_GPOCON_INVOUTN) | (((val) & 0xff) << 0))

/*******************************************************************************
 * GPOSTAT Register
 ******************************************************************************/

/* Output Register Bit n (7:0) */
#define SSC_GPOSTAT_OUTN   (0xff)
#define SSC_GPOSTAT_OUTN_GET(val)   ((((val) & SSC_GPOSTAT_OUTN) >> 0) & 0xff)

/*******************************************************************************
 * WHBGPOSTAT
 ******************************************************************************/

/* Set Output Register Bit n (15:8) */
#define SSC_WHBGPOSTAT_SETOUTN   (0xff << 8)
#define SSC_WHBGPOSTAT_SETOUTN_VAL(val)   (((val) & 0xff) << 8)
#define SSC_WHBGPOSTAT_SETOUTN_SET(reg,val) (reg) = (((reg & ~SSC_WHBGPOSTAT_SETOUTN) | (val) & 1) << 8)
/* Clear Output Register Bit n (7:0) */
#define SSC_WHBGPOSTAT_CLROUTN   (0xff)
#define SSC_WHBGPOSTAT_CLROUTN_VAL(val)   (((val) & 0xff) << 0)
#define SSC_WHBGPOSTAT_CLROUTN_SET(reg,val) (reg) = (((reg & ~SSC_WHBGPOSTAT_CLROUTN) | (val) & 1) << 0)

/*******************************************************************************
 * RXREQ Register
 ******************************************************************************/

/* Receive Count Value (15:0) */
#define SSC_RXREQ_RXCNT   (0xffff)
#define SSC_RXREQ_RXCNT_VAL(val)   (((val) & 0xffff) << 0)
#define SSC_RXREQ_RXCNT_GET(val)   ((((val) & SSC_RXREQ_RXCNT) >> 0) & 0xffff)
#define SSC_RXREQ_RXCNT_SET(reg,val) (reg) = ((reg & ~SSC_RXREQ_RXCNT) | (((val) & 0xffff) << 0))

/*******************************************************************************
 * RXCNT Register
 ******************************************************************************/

/* Receive To Do Value (15:0) */
#define SSC_RXCNT_TODO   (0xffff)
#define SSC_RXCNT_TODO_GET(val)   ((((val) & SSC_RXCNT_TODO) >> 0) & 0xffff)

/*******************************************************************************
 * DMA_CON Register
 ******************************************************************************/

/* Receive Class (3:2) */
#define SSC_DMA_CON_RXCLS   (0x3 << 2)
#define SSC_DMA_CON_RXCLS_VAL(val)   (((val) & 0x3) << 2)
#define SSC_DMA_CON_RXCLS_GET(val)   ((((val) & SSC_DMA_CON_RXCLS) >> 2) & 0x3)
#define SSC_DMA_CON_RXCLS_SET(reg,val) (reg) = ((reg & ~SSC_DMA_CON_RXCLS) | (((val) & 0x3) << 2))
/* Transmit Path On (1) */
#define SSC_DMA_CON_TXON   (0x1 << 1)
#define SSC_DMA_CON_TXON_VAL(val)   (((val) & 0x1) << 1)
#define SSC_DMA_CON_TXON_GET(val)   ((((val) & SSC_DMA_CON_TXON) >> 1) & 0x1)
#define SSC_DMA_CON_TXON_SET(reg,val) (reg) = ((reg & ~SSC_DMA_CON_TXON) | (((val) & 0x1) << 1))
/* Receive Path On (0) */
#define SSC_DMA_CON_RXON   (0x1)
#define SSC_DMA_CON_RXON_VAL(val)   (((val) & 0x1) << 0)
#define SSC_DMA_CON_RXON_GET(val)   ((((val) & SSC_DMA_CON_RXON) >> 0) & 0x1)
#define SSC_DMA_CON_RXON_SET(reg,val) (reg) = ((reg & ~SSC_DMA_CON_RXON) | (((val) & 0x1) << 0))

/*******************************************************************************
 * IRNEN Register
 ******************************************************************************/

/* Frame End Interrupt Request Enable (3) */
#define SSC_IRNEN_F   (0x1 << 3)
#define SSC_IRNEN_F_VAL(val)   (((val) & 0x1) << 3)
#define SSC_IRNEN_F_GET(val)   ((((val) & SSC_IRNEN_F) >> 3) & 0x1)
#define SSC_IRNEN_F_SET(reg,val) (reg) = ((reg & ~SSC_IRNEN_F) | (((val) & 0x1) << 3))
/* Error Interrupt Request Enable (2) */
#define SSC_IRNEN_E   (0x1 << 2)
#define SSC_IRNEN_E_VAL(val)   (((val) & 0x1) << 2)
#define SSC_IRNEN_E_GET(val)   ((((val) & SSC_IRNEN_E) >> 2) & 0x1)
#define SSC_IRNEN_E_SET(reg,val) (reg) = ((reg & ~SSC_IRNEN_E) | (((val) & 0x1) << 2))
/* Receive Interrupt Request Enable (1) */
#define SSC_IRNEN_R   (0x1 << 1)
#define SSC_IRNEN_R_VAL(val)   (((val) & 0x1) << 1)
#define SSC_IRNEN_R_GET(val)   ((((val) & SSC_IRNEN_R) >> 1) & 0x1)
#define SSC_IRNEN_R_SET(reg,val) (reg) = ((reg & ~SSC_IRNEN_R) | (((val) & 0x1) << 1))
/* Transmit Interrupt Request Enable (0) */
#define SSC_IRNEN_T   (0x1)
#define SSC_IRNEN_T_VAL(val)   (((val) & 0x1) << 0)
#define SSC_IRNEN_T_GET(val)   ((((val) & SSC_IRNEN_T) >> 0) & 0x1)
#define SSC_IRNEN_T_SET(reg,val) (reg) = ((reg & ~SSC_IRNEN_T) | (((val) & 0x1) << 0))

/*******************************************************************************
 * IRNICR Register
 ******************************************************************************/

/* Frame End Interrupt Request (3) */
#define SSC_IRNICR_F   (0x1 << 3)
#define SSC_IRNICR_F_GET(val)   ((((val) & SSC_IRNICR_F) >> 3) & 0x1)
/* Error Interrupt Request (2) */
#define SSC_IRNICR_E   (0x1 << 2)
#define SSC_IRNICR_E_GET(val)   ((((val) & SSC_IRNICR_E) >> 2) & 0x1)
/* Receive Interrupt Request (1) */
#define SSC_IRNICR_R   (0x1 << 1)
#define SSC_IRNICR_R_GET(val)   ((((val) & SSC_IRNICR_R) >> 1) & 0x1)
/* Transmit Interrupt Request (0) */
#define SSC_IRNICR_T   (0x1)
#define SSC_IRNICR_T_GET(val)   ((((val) & SSC_IRNICR_T) >> 0) & 0x1)

/*******************************************************************************
 * IRNCR Register
 ******************************************************************************/

/* Frame End Interrupt Request (3) */
#define SSC_IRNCR_F   (0x1 << 3)
#define SSC_IRNCR_F_GET(val)   ((((val) & SSC_IRNCR_F) >> 3) & 0x1)
/* Error Interrupt Request (2) */
#define SSC_IRNCR_E   (0x1 << 2)
#define SSC_IRNCR_E_GET(val)   ((((val) & SSC_IRNCR_E) >> 2) & 0x1)
/* Receive Interrupt Request (1) */
#define SSC_IRNCR_R   (0x1 << 1)
#define SSC_IRNCR_R_GET(val)   ((((val) & SSC_IRNCR_R) >> 1) & 0x1)
/* Transmit Interrupt Request (0) */
#define SSC_IRNCR_T   (0x1)
#define SSC_IRNCR_T_GET(val)   ((((val) & SSC_IRNCR_T) >> 0) & 0x1)

#endif /* __SSC_H */
