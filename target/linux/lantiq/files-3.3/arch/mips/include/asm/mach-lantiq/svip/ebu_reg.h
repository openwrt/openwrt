/******************************************************************************

  Copyright (c) 2007
  Infineon Technologies AG
  St. Martin Strasse 53; 81669 Munich, Germany

  Any use of this Software is subject to the conclusion of a respective
  License Agreement. Without such a License Agreement no rights to the
  Software are granted.

 ******************************************************************************/

#ifndef __EBU_REG_H
#define __EBU_REG_H

#define ebu_r32(reg) ltq_r32(&ebu->reg)
#define ebu_w32(val, reg) ltq_w32(val, &ebu->reg)
#define ebu_w32_mask(clear, set, reg) ltq_w32_mask(clear, set, &ebu->reg)

/** EBU register structure */
struct svip_reg_ebu {
	volatile unsigned long  clc;  /*  0x0000 */
	volatile unsigned long  reserved0;  /*  0x04 */
	volatile unsigned long  id;  /*  0x0008 */
	volatile unsigned long  reserved1;  /*  0x0c */
	volatile unsigned long  con;  /*  0x0010 */
	volatile unsigned long  reserved2[3];  /*  0x14 */
	volatile unsigned long  addr_sel_0;  /*  0x0020 */
	volatile unsigned long  addr_sel_1;  /*  0x0024 */
	volatile unsigned long  addr_sel_2;  /*  0x0028 */
	volatile unsigned long  addr_sel_3;  /*  0x002c */
	volatile unsigned long  reserved3[12];  /*  0x30 */
	volatile unsigned long  con_0;  /*  0x0060 */
	volatile unsigned long  con_1;  /*  0x0064 */
	volatile unsigned long  con_2;  /*  0x0068 */
	volatile unsigned long  con_3;  /*  0x006c */
	volatile unsigned long  reserved4[4];  /*  0x70 */
	volatile unsigned long  emu_addr;  /*  0x0080 */
	volatile unsigned long  emu_bc;  /*  0x0084 */
	volatile unsigned long  emu_con;  /*  0x0088 */
	volatile unsigned long  reserved5;  /*  0x8c */
	volatile unsigned long  pcc_con;  /*  0x0090 */
	volatile unsigned long  pcc_stat;  /*  0x0094 */
	volatile unsigned long  reserved6[2];  /*  0x98 */
	volatile unsigned long  pcc_istat;  /*  0x00A0 */
	volatile unsigned long  pcc_ien;  /*  0x00A4 */
	volatile unsigned long  pcc_int_out;  /*  0x00A8 */
	volatile unsigned long  pcc_irs;  /*  0x00AC */
	volatile unsigned long  nand_con;  /*  0x00B0 */
	volatile unsigned long  nand_wait;  /*  0x00B4 */
	volatile unsigned long  nand_ecc0;  /*  0x00B8 */
	volatile unsigned long  nand_ecc_ac;  /*  0x00BC */
};

/*******************************************************************************
 * EBU
 ******************************************************************************/
#define LTQ_EBU_CLC   ((volatile unsigned int*)(LTQ_EBU_BASE + 0x0000))
#define LTQ_EBU_ID   ((volatile unsigned int*)(LTQ_EBU_BASE + 0x0008))
#define LTQ_EBU_CON   ((volatile unsigned int*)(LTQ_EBU_BASE + 0x0010))
#define LTQ_EBU_ADDR_SEL_0   ((volatile unsigned int*)(LTQ_EBU_BASE + 0x0020))
#define LTQ_EBU_ADDR_SEL_1   ((volatile unsigned int*)(LTQ_EBU_BASE + 0x0024))
#define LTQ_EBU_ADDR_SEL_2   ((volatile unsigned int*)(LTQ_EBU_BASE + 0x0028))
#define LTQ_EBU_ADDR_SEL_3   ((volatile unsigned int*)(LTQ_EBU_BASE + 0x002c))
#define LTQ_EBU_CON_0   ((volatile unsigned int*)(LTQ_EBU_BASE + 0x0060))
#define LTQ_EBU_CON_1   ((volatile unsigned int*)(LTQ_EBU_BASE + 0x0064))
#define LTQ_EBU_CON_2   ((volatile unsigned int*)(LTQ_EBU_BASE + 0x0068))
#define LTQ_EBU_CON_3   ((volatile unsigned int*)(LTQ_EBU_BASE + 0x006c))
#define LTQ_EBU_EMU_BC   ((volatile unsigned int*)(LTQ_EBU_BASE + 0x0084))
#define LTQ_EBU_PCC_CON   ((volatile unsigned int*)(LTQ_EBU_BASE + 0x0090))
#define LTQ_EBU_PCC_STAT   ((volatile unsigned int*)(LTQ_EBU_BASE + 0x0094))
#define LTQ_EBU_PCC_ISTAT   ((volatile unsigned int*)(LTQ_EBU_BASE + 0x00A0))
#define LTQ_EBU_PCC_IEN   ((volatile unsigned int*)(LTQ_EBU_BASE + 0x00A4))
#define LTQ_EBU_PCC_INT_OUT   ((volatile unsigned int*)(LTQ_EBU_BASE + 0x00A8))
#define LTQ_EBU_PCC_IRS   ((volatile unsigned int*)(LTQ_EBU_BASE + 0x00AC))
#define LTQ_EBU_NAND_CON   ((volatile unsigned int*)(LTQ_EBU_BASE + 0x00B0))
#define LTQ_EBU_NAND_WAIT   ((volatile unsigned int*)(LTQ_EBU_BASE + 0x00B4))
#define LTQ_EBU_NAND_ECC0   ((volatile unsigned int*)(LTQ_EBU_BASE + 0x00B8))
#define LTQ_EBU_NAND_ECC_AC   ((volatile unsigned int*)(LTQ_EBU_BASE + 0x00BC))
#define LTQ_EBU_EMU_ADDR   ((volatile unsigned int*)(LTQ_EBU_BASE + 0x0080))
#define LTQ_EBU_EMU_CON   ((volatile unsigned int*)(LTQ_EBU_BASE + 0x0088))

/*******************************************************************************
 * EBU Clock Control Register
 ******************************************************************************/

/* EBU Disable Status Bit (1) */
#define LTQ_EBU_CLC_DISS   (0x1 << 1)
#define LTQ_EBU_CLC_DISS_GET(val)   ((((val) & LTQ_EBU_CLC_DISS) >> 1) & 0x1)
/* Used for Enable/disable Control of the EBU (0) */
#define LTQ_EBU_CLC_DISR   (0x1)
#define LTQ_EBU_CLC_DISR_VAL(val)   (((val) & 0x1) << 0)
#define LTQ_EBU_CLC_DISR_GET(val)   ((((val) & LTQ_EBU_CLC_DISR) >> 0) & 0x1)
#define LTQ_EBU_CLC_DISR_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_CLC_DISR) | (((val) & 0x1) << 0))

/*******************************************************************************
 * EBU Identification Register (Internal)
 ******************************************************************************/

/* Module Number (31:8) */
#define LTQ_EBU_ID_MODNUM   (0xffffff << 8)
#define LTQ_EBU_ID_MODNUM_GET(val)   ((((val) & LTQ_EBU_ID_MODNUM) >> 8) & 0xffffff)
/* Revision Number (7:0) */
#define LTQ_EBU_ID_REVNUM   (0xff)
#define LTQ_EBU_ID_REVNUM_GET(val)   ((((val) & LTQ_EBU_ID_REVNUM) >> 0) & 0xff)

/*******************************************************************************
 * External Bus Unit Control Register
 ******************************************************************************/

/* Driver Turn-Around Control, Chip Select Triggered (22:20) */
#define LTQ_EBU_CON_DTACS   (0x7 << 20)
#define LTQ_EBU_CON_DTACS_VAL(val)   (((val) & 0x7) << 20)
#define LTQ_EBU_CON_DTACS_GET(val)   ((((val) & LTQ_EBU_CON_DTACS) >> 20) & 0x7)
#define LTQ_EBU_CON_DTACS_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_CON_DTACS) | (((val) & 0x7) << 20))
/* Driver Turn-Around Control, Read-write Triggered (18:16) */
#define LTQ_EBU_CON_DTARW   (0x7 << 16)
#define LTQ_EBU_CON_DTARW_VAL(val)   (((val) & 0x7) << 16)
#define LTQ_EBU_CON_DTARW_GET(val)   ((((val) & LTQ_EBU_CON_DTARW) >> 16) & 0x7)
#define LTQ_EBU_CON_DTARW_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_CON_DTARW) | (((val) & 0x7) << 16))
/* Time-Out Control (15:8) */
#define LTQ_EBU_CON_TOUTC   (0xff << 8)
#define LTQ_EBU_CON_TOUTC_VAL(val)   (((val) & 0xff) << 8)
#define LTQ_EBU_CON_TOUTC_GET(val)   ((((val) & LTQ_EBU_CON_TOUTC) >> 8) & 0xff)
#define LTQ_EBU_CON_TOUTC_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_CON_TOUTC) | (((val) & 0xff) << 8))
/* Arbitration Mode (7:6) */
#define LTQ_EBU_CON_ARBMODE   (0x3 << 6)
#define LTQ_EBU_CON_ARBMODE_VAL(val)   (((val) & 0x3) << 6)
#define LTQ_EBU_CON_ARBMODE_GET(val)   ((((val) & LTQ_EBU_CON_ARBMODE) >> 6) & 0x3)
#define LTQ_EBU_CON_ARBMODE_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_CON_ARBMODE) | (((val) & 0x3) << 6))
/* Arbitration Synchronization (5) */
#define LTQ_EBU_CON_ARBSYNC   (0x1 << 5)
#define LTQ_EBU_CON_ARBSYNC_VAL(val)   (((val) & 0x1) << 5)
#define LTQ_EBU_CON_ARBSYNC_GET(val)   ((((val) & LTQ_EBU_CON_ARBSYNC) >> 5) & 0x1)
#define LTQ_EBU_CON_ARBSYNC_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_CON_ARBSYNC) | (((val) & 0x1) << 5))

/*******************************************************************************
 * Address Select Registers
 ******************************************************************************/

/* Memory Region Base Address (31:12) */
#define LTQ_EBU_ADDR_SEL_0_BASE   (0xfffff << 12)
#define LTQ_EBU_ADDR_SEL_0_BASE_VAL(val)   (((val) & 0xfffff) << 12)
#define LTQ_EBU_ADDR_SEL_0_BASE_GET(val)   ((((val) & LTQ_EBU_ADDR_SEL_0_BASE) >> 12) & 0xfffff)
#define LTQ_EBU_ADDR_SEL_0_BASE_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_ADDR_SEL_0_BASE) | (((val) & 0xfffff) << 12))
/* Memory Region Address Mask (7:4) */
#define LTQ_EBU_ADDR_SEL_0_MASK   (0xf << 4)
#define LTQ_EBU_ADDR_SEL_0_MASK_VAL(val)   (((val) & 0xf) << 4)
#define LTQ_EBU_ADDR_SEL_0_MASK_GET(val)   ((((val) & LTQ_EBU_ADDR_SEL_0_MASK) >> 4) & 0xf)
#define LTQ_EBU_ADDR_SEL_0_MASK_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_ADDR_SEL_0_MASK) | (((val) & 0xf) << 4))
/* Memory Region Mirror Enable Control (1) */
#define LTQ_EBU_ADDR_SEL_0_MRME   (0x1 << 1)
#define LTQ_EBU_ADDR_SEL_0_MRME_VAL(val)   (((val) & 0x1) << 1)
#define LTQ_EBU_ADDR_SEL_0_MRME_GET(val)   ((((val) & LTQ_EBU_ADDR_SEL_0_MRME) >> 1) & 0x1)
#define LTQ_EBU_ADDR_SEL_0_MRME_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_ADDR_SEL_0_MRME) | (((val) & 0x1) << 1))
/* Memory Region Enable Control (0) */
#define LTQ_EBU_ADDR_SEL_0_REGEN   (0x1)
#define LTQ_EBU_ADDR_SEL_0_REGEN_VAL(val)   (((val) & 0x1) << 0)
#define LTQ_EBU_ADDR_SEL_0_REGEN_GET(val)   ((((val) & LTQ_EBU_ADDR_SEL_0_REGEN) >> 0) & 0x1)
#define LTQ_EBU_ADDR_SEL_0_REGEN_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_ADDR_SEL_0_REGEN) | (((val) & 0x1) << 0))

/*******************************************************************************
 * Bus Configuration Registers
 ******************************************************************************/

/* Memory Region Write Protection (31) */
#define LTQ_EBU_CON_0_WRDIS   (0x1 << 31)
#define LTQ_EBU_CON_0_WRDIS_VAL(val)   (((val) & 0x1) << 31)
#define LTQ_EBU_CON_0_WRDIS_GET(val)   ((((val) & LTQ_EBU_CON_0_WRDIS) >> 31) & 0x1)
#define LTQ_EBU_CON_0_WRDIS_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_CON_0_WRDIS) | (((val) & 0x1) << 31))
/* Address Swapping (30) */
#define LTQ_EBU_CON_0_ADSWP   (0x1 << 30)
#define LTQ_EBU_CON_0_ADSWP_VAL(val)   (((val) & 0x1) << 30)
#define LTQ_EBU_CON_0_ADSWP_GET(val)   ((((val) & LTQ_EBU_CON_0_ADSWP) >> 30) & 0x1)
#define LTQ_EBU_CON_0_ADSWP_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_CON_0_ADSWP) | (((val) & 0x1) << 30))
/* Address Generation Control (26:24) */
#define LTQ_EBU_CON_0_AGEN   (0x7 << 24)
#define LTQ_EBU_CON_0_AGEN_VAL(val)   (((val) & 0x7) << 24)
#define LTQ_EBU_CON_0_AGEN_GET(val)   ((((val) & LTQ_EBU_CON_0_AGEN) >> 24) & 0x7)
#define LTQ_EBU_CON_0_AGEN_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_CON_0_AGEN) | (((val) & 0x7) << 24))
/* Extended Address Setup Control (22) */
#define LTQ_EBU_CON_0_SETUP   (0x1 << 22)
#define LTQ_EBU_CON_0_SETUP_VAL(val)   (((val) & 0x1) << 22)
#define LTQ_EBU_CON_0_SETUP_GET(val)   ((((val) & LTQ_EBU_CON_0_SETUP) >> 22) & 0x1)
#define LTQ_EBU_CON_0_SETUP_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_CON_0_SETUP) | (((val) & 0x1) << 22))
/* Variable Wait-State Insertion Control (21:20) */
#define LTQ_EBU_CON_0_WAIT   (0x3 << 20)
#define LTQ_EBU_CON_0_WAIT_VAL(val)   (((val) & 0x3) << 20)
#define LTQ_EBU_CON_0_WAIT_GET(val)   ((((val) & LTQ_EBU_CON_0_WAIT) >> 20) & 0x3)
#define LTQ_EBU_CON_0_WAIT_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_CON_0_WAIT) | (((val) & 0x3) << 20))
/* Active WAIT Level Control (19) */
#define LTQ_EBU_CON_0_WINV   (0x1 << 19)
#define LTQ_EBU_CON_0_WINV_VAL(val)   (((val) & 0x1) << 19)
#define LTQ_EBU_CON_0_WINV_GET(val)   ((((val) & LTQ_EBU_CON_0_WINV) >> 19) & 0x1)
#define LTQ_EBU_CON_0_WINV_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_CON_0_WINV) | (((val) & 0x1) << 19))
/* External Device Data Width Control (17:16) */
#define LTQ_EBU_CON_0_PW   (0x3 << 16)
#define LTQ_EBU_CON_0_PW_VAL(val)   (((val) & 0x3) << 16)
#define LTQ_EBU_CON_0_PW_GET(val)   ((((val) & LTQ_EBU_CON_0_PW) >> 16) & 0x3)
#define LTQ_EBU_CON_0_PW_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_CON_0_PW) | (((val) & 0x3) << 16))
/* Address Latch Enable ALE Duration Control (15:14) */
#define LTQ_EBU_CON_0_ALEC   (0x3 << 14)
#define LTQ_EBU_CON_0_ALEC_VAL(val)   (((val) & 0x3) << 14)
#define LTQ_EBU_CON_0_ALEC_GET(val)   ((((val) & LTQ_EBU_CON_0_ALEC) >> 14) & 0x3)
#define LTQ_EBU_CON_0_ALEC_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_CON_0_ALEC) | (((val) & 0x3) << 14))
/* Byte Control Signal Timing Mode Control (13:12) */
#define LTQ_EBU_CON_0_BCGEN   (0x3 << 12)
#define LTQ_EBU_CON_0_BCGEN_VAL(val)   (((val) & 0x3) << 12)
#define LTQ_EBU_CON_0_BCGEN_GET(val)   ((((val) & LTQ_EBU_CON_0_BCGEN) >> 12) & 0x3)
#define LTQ_EBU_CON_0_BCGEN_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_CON_0_BCGEN) | (((val) & 0x3) << 12))
/* Write Access Wait-State Control (10:8) */
#define LTQ_EBU_CON_0_WAITWRC   (0x7 << 8)
#define LTQ_EBU_CON_0_WAITWRC_VAL(val)   (((val) & 0x7) << 8)
#define LTQ_EBU_CON_0_WAITWRC_GET(val)   ((((val) & LTQ_EBU_CON_0_WAITWRC) >> 8) & 0x7)
#define LTQ_EBU_CON_0_WAITWRC_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_CON_0_WAITWRC) | (((val) & 0x7) << 8))
/* Read Access Wait-State Control (7:6) */
#define LTQ_EBU_CON_0_WAITRDC   (0x3 << 6)
#define LTQ_EBU_CON_0_WAITRDC_VAL(val)   (((val) & 0x3) << 6)
#define LTQ_EBU_CON_0_WAITRDC_GET(val)   ((((val) & LTQ_EBU_CON_0_WAITRDC) >> 6) & 0x3)
#define LTQ_EBU_CON_0_WAITRDC_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_CON_0_WAITRDC) | (((val) & 0x3) << 6))
/* Hold/Pause Cycle Control (5:4) */
#define LTQ_EBU_CON_0_HOLDC   (0x3 << 4)
#define LTQ_EBU_CON_0_HOLDC_VAL(val)   (((val) & 0x3) << 4)
#define LTQ_EBU_CON_0_HOLDC_GET(val)   ((((val) & LTQ_EBU_CON_0_HOLDC) >> 4) & 0x3)
#define LTQ_EBU_CON_0_HOLDC_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_CON_0_HOLDC) | (((val) & 0x3) << 4))
/* Recovery Cycle Control (3:2) */
#define LTQ_EBU_CON_0_RECOVC   (0x3 << 2)
#define LTQ_EBU_CON_0_RECOVC_VAL(val)   (((val) & 0x3) << 2)
#define LTQ_EBU_CON_0_RECOVC_GET(val)   ((((val) & LTQ_EBU_CON_0_RECOVC) >> 2) & 0x3)
#define LTQ_EBU_CON_0_RECOVC_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_CON_0_RECOVC) | (((val) & 0x3) << 2))
/* Wait Cycle Multiplier Control (1:0) */
#define LTQ_EBU_CON_0_CMULT   (0x3)
#define LTQ_EBU_CON_0_CMULT_VAL(val)   (((val) & 0x3) << 0)
#define LTQ_EBU_CON_0_CMULT_GET(val)   ((((val) & LTQ_EBU_CON_0_CMULT) >> 0) & 0x3)
#define LTQ_EBU_CON_0_CMULT_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_CON_0_CMULT) | (((val) & 0x3) << 0))

/*******************************************************************************
 * External Bus Unit Emulator Bus Configuration Register
 ******************************************************************************/

/* Write Protection (31) */
#define LTQ_EBU_EMU_BC_WRITE   (0x1 << 31)
#define LTQ_EBU_EMU_BC_WRITE_VAL(val)   (((val) & 0x1) << 31)
#define LTQ_EBU_EMU_BC_WRITE_GET(val)   ((((val) & LTQ_EBU_EMU_BC_WRITE) >> 31) & 0x1)
#define LTQ_EBU_EMU_BC_WRITE_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_EMU_BC_WRITE) | (((val) & 0x1) << 31))
/* Address Generation Control (26:24) */
#define LTQ_EBU_EMU_BC_AGEN   (0x7 << 24)
#define LTQ_EBU_EMU_BC_AGEN_VAL(val)   (((val) & 0x7) << 24)
#define LTQ_EBU_EMU_BC_AGEN_GET(val)   ((((val) & LTQ_EBU_EMU_BC_AGEN) >> 24) & 0x7)
#define LTQ_EBU_EMU_BC_AGEN_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_EMU_BC_AGEN) | (((val) & 0x7) << 24))
/* Extended Address Setup Control (22) */
#define LTQ_EBU_EMU_BC_SETUP   (0x1 << 22)
#define LTQ_EBU_EMU_BC_SETUP_VAL(val)   (((val) & 0x1) << 22)
#define LTQ_EBU_EMU_BC_SETUP_GET(val)   ((((val) & LTQ_EBU_EMU_BC_SETUP) >> 22) & 0x1)
#define LTQ_EBU_EMU_BC_SETUP_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_EMU_BC_SETUP) | (((val) & 0x1) << 22))
/* Variable Waitstate Insertion Control (21:20) */
#define LTQ_EBU_EMU_BC_WAIT   (0x3 << 20)
#define LTQ_EBU_EMU_BC_WAIT_VAL(val)   (((val) & 0x3) << 20)
#define LTQ_EBU_EMU_BC_WAIT_GET(val)   ((((val) & LTQ_EBU_EMU_BC_WAIT) >> 20) & 0x3)
#define LTQ_EBU_EMU_BC_WAIT_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_EMU_BC_WAIT) | (((val) & 0x3) << 20))
/* Active WAIT Level Control (19) */
#define LTQ_EBU_EMU_BC_WINV   (0x1 << 19)
#define LTQ_EBU_EMU_BC_WINV_VAL(val)   (((val) & 0x1) << 19)
#define LTQ_EBU_EMU_BC_WINV_GET(val)   ((((val) & LTQ_EBU_EMU_BC_WINV) >> 19) & 0x1)
#define LTQ_EBU_EMU_BC_WINV_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_EMU_BC_WINV) | (((val) & 0x1) << 19))
/* External Device Data Width Control (17:16) */
#define LTQ_EBU_EMU_BC_PORTW   (0x3 << 16)
#define LTQ_EBU_EMU_BC_PORTW_VAL(val)   (((val) & 0x3) << 16)
#define LTQ_EBU_EMU_BC_PORTW_GET(val)   ((((val) & LTQ_EBU_EMU_BC_PORTW) >> 16) & 0x3)
#define LTQ_EBU_EMU_BC_PORTW_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_EMU_BC_PORTW) | (((val) & 0x3) << 16))
/* Address Latch Enable Function (15:14) */
#define LTQ_EBU_EMU_BC_ALEC   (0x3 << 14)
#define LTQ_EBU_EMU_BC_ALEC_VAL(val)   (((val) & 0x3) << 14)
#define LTQ_EBU_EMU_BC_ALEC_GET(val)   ((((val) & LTQ_EBU_EMU_BC_ALEC) >> 14) & 0x3)
#define LTQ_EBU_EMU_BC_ALEC_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_EMU_BC_ALEC) | (((val) & 0x3) << 14))
/* Byte Control Signal Timing Mode (13:12) */
#define LTQ_EBU_EMU_BC_BCGEN   (0x3 << 12)
#define LTQ_EBU_EMU_BC_BCGEN_VAL(val)   (((val) & 0x3) << 12)
#define LTQ_EBU_EMU_BC_BCGEN_GET(val)   ((((val) & LTQ_EBU_EMU_BC_BCGEN) >> 12) & 0x3)
#define LTQ_EBU_EMU_BC_BCGEN_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_EMU_BC_BCGEN) | (((val) & 0x3) << 12))
/* Write Access Waitstate Control (10:8) */
#define LTQ_EBU_EMU_BC_WAITWRC   (0x7 << 8)
#define LTQ_EBU_EMU_BC_WAITWRC_VAL(val)   (((val) & 0x7) << 8)
#define LTQ_EBU_EMU_BC_WAITWRC_GET(val)   ((((val) & LTQ_EBU_EMU_BC_WAITWRC) >> 8) & 0x7)
#define LTQ_EBU_EMU_BC_WAITWRC_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_EMU_BC_WAITWRC) | (((val) & 0x7) << 8))
/* Read Access Waitstate Control (7:6) */
#define LTQ_EBU_EMU_BC_WAITRDC   (0x3 << 6)
#define LTQ_EBU_EMU_BC_WAITRDC_VAL(val)   (((val) & 0x3) << 6)
#define LTQ_EBU_EMU_BC_WAITRDC_GET(val)   ((((val) & LTQ_EBU_EMU_BC_WAITRDC) >> 6) & 0x3)
#define LTQ_EBU_EMU_BC_WAITRDC_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_EMU_BC_WAITRDC) | (((val) & 0x3) << 6))
/* Hold/Pause Cycle Control (5:4) */
#define LTQ_EBU_EMU_BC_HOLDC   (0x3 << 4)
#define LTQ_EBU_EMU_BC_HOLDC_VAL(val)   (((val) & 0x3) << 4)
#define LTQ_EBU_EMU_BC_HOLDC_GET(val)   ((((val) & LTQ_EBU_EMU_BC_HOLDC) >> 4) & 0x3)
#define LTQ_EBU_EMU_BC_HOLDC_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_EMU_BC_HOLDC) | (((val) & 0x3) << 4))
/* Recovery Cycles Control (3:2) */
#define LTQ_EBU_EMU_BC_RECOVC   (0x3 << 2)
#define LTQ_EBU_EMU_BC_RECOVC_VAL(val)   (((val) & 0x3) << 2)
#define LTQ_EBU_EMU_BC_RECOVC_GET(val)   ((((val) & LTQ_EBU_EMU_BC_RECOVC) >> 2) & 0x3)
#define LTQ_EBU_EMU_BC_RECOVC_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_EMU_BC_RECOVC) | (((val) & 0x3) << 2))
/* Cycle Multiplier Control (1:0) */
#define LTQ_EBU_EMU_BC_CMULT   (0x3)
#define LTQ_EBU_EMU_BC_CMULT_VAL(val)   (((val) & 0x3) << 0)
#define LTQ_EBU_EMU_BC_CMULT_GET(val)   ((((val) & LTQ_EBU_EMU_BC_CMULT) >> 0) & 0x3)
#define LTQ_EBU_EMU_BC_CMULT_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_EMU_BC_CMULT) | (((val) & 0x3) << 0))

/*******************************************************************************
 * PC-Card Control Register
 ******************************************************************************/

/* External Interrupt Input IREQ (3:1) */
#define LTQ_EBU_PCC_CON_IREQ   (0x7 << 1)
#define LTQ_EBU_PCC_CON_IREQ_VAL(val)   (((val) & 0x7) << 1)
#define LTQ_EBU_PCC_CON_IREQ_GET(val)   ((((val) & LTQ_EBU_PCC_CON_IREQ) >> 1) & 0x7)
#define LTQ_EBU_PCC_CON_IREQ_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_PCC_CON_IREQ) | (((val) & 0x7) << 1))
/* PC Card ON (0) */
#define LTQ_EBU_PCC_CON_ON   (0x1)
#define LTQ_EBU_PCC_CON_ON_VAL(val)   (((val) & 0x1) << 0)
#define LTQ_EBU_PCC_CON_ON_GET(val)   ((((val) & LTQ_EBU_PCC_CON_ON) >> 0) & 0x1)
#define LTQ_EBU_PCC_CON_ON_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_PCC_CON_ON) | (((val) & 0x1) << 0))

/*******************************************************************************
 * PCC Status Register
 ******************************************************************************/

/* Interrupt Request (6) */
#define LTQ_EBU_PCC_STAT_IRQ   (0x1 << 6)
#define LTQ_EBU_PCC_STAT_IRQ_GET(val)   ((((val) & LTQ_EBU_PCC_STAT_IRQ) >> 6) & 0x1)
/* PC-Card Overcurrent (5) */
#define LTQ_EBU_PCC_STAT_OC   (0x1 << 5)
#define LTQ_EBU_PCC_STAT_OC_GET(val)   ((((val) & LTQ_EBU_PCC_STAT_OC) >> 5) & 0x1)
/* PC-Card Socket Power On (4) */
#define LTQ_EBU_PCC_STAT_SPON   (0x1 << 4)
#define LTQ_EBU_PCC_STAT_SPON_GET(val)   ((((val) & LTQ_EBU_PCC_STAT_SPON) >> 4) & 0x1)
/* Card Detect Status (1:0) */
#define LTQ_EBU_PCC_STAT_CD   (0x3)
#define LTQ_EBU_PCC_STAT_CD_GET(val)   ((((val) & LTQ_EBU_PCC_STAT_CD) >> 0) & 0x3)

/*******************************************************************************
 * PCC Interrupt Status Register
 ******************************************************************************/

/* Interrupt Request Active Interrupt (4) */
#define LTQ_EBU_PCC_ISTAT_IREQ   (0x1 << 4)
#define LTQ_EBU_PCC_ISTAT_IREQ_VAL(val)   (((val) & 0x1) << 4)
#define LTQ_EBU_PCC_ISTAT_IREQ_GET(val)   ((((val) & LTQ_EBU_PCC_ISTAT_IREQ) >> 4) & 0x1)
#define LTQ_EBU_PCC_ISTAT_IREQ_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_PCC_ISTAT_IREQ) | (((val) & 0x1) << 4))
/* Over Current Status Change Interrupt (3) */
#define LTQ_EBU_PCC_ISTAT_OC   (0x1 << 3)
#define LTQ_EBU_PCC_ISTAT_OC_VAL(val)   (((val) & 0x1) << 3)
#define LTQ_EBU_PCC_ISTAT_OC_GET(val)   ((((val) & LTQ_EBU_PCC_ISTAT_OC) >> 3) & 0x1)
#define LTQ_EBU_PCC_ISTAT_OC_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_PCC_ISTAT_OC) | (((val) & 0x1) << 3))
/* Socket Power on Status Change Interrupt (2) */
#define LTQ_EBU_PCC_ISTAT_SPON   (0x1 << 2)
#define LTQ_EBU_PCC_ISTAT_SPON_VAL(val)   (((val) & 0x1) << 2)
#define LTQ_EBU_PCC_ISTAT_SPON_GET(val)   ((((val) & LTQ_EBU_PCC_ISTAT_SPON) >> 2) & 0x1)
#define LTQ_EBU_PCC_ISTAT_SPON_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_PCC_ISTAT_SPON) | (((val) & 0x1) << 2))
/* Voltage Sense Status Change Interrupt (1) */
#define LTQ_EBU_PCC_ISTAT_VS   (0x1 << 1)
#define LTQ_EBU_PCC_ISTAT_VS_VAL(val)   (((val) & 0x1) << 1)
#define LTQ_EBU_PCC_ISTAT_VS_GET(val)   ((((val) & LTQ_EBU_PCC_ISTAT_VS) >> 1) & 0x1)
#define LTQ_EBU_PCC_ISTAT_VS_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_PCC_ISTAT_VS) | (((val) & 0x1) << 1))
/* Card Detect Status Change Interrupt (0) */
#define LTQ_EBU_PCC_ISTAT_CD   (0x1)
#define LTQ_EBU_PCC_ISTAT_CD_VAL(val)   (((val) & 0x1) << 0)
#define LTQ_EBU_PCC_ISTAT_CD_GET(val)   ((((val) & LTQ_EBU_PCC_ISTAT_CD) >> 0) & 0x1)
#define LTQ_EBU_PCC_ISTAT_CD_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_PCC_ISTAT_CD) | (((val) & 0x1) << 0))

/*******************************************************************************
 * PCC Interrupt Enable Register
 ******************************************************************************/

/* Enable of Interrupt Request IR (4) */
#define LTQ_EBU_PCC_IEN_IR   (0x1 << 4)
#define LTQ_EBU_PCC_IEN_IR_VAL(val)   (((val) & 0x1) << 4)
#define LTQ_EBU_PCC_IEN_IR_GET(val)   ((((val) & LTQ_EBU_PCC_IEN_IR) >> 4) & 0x1)
#define LTQ_EBU_PCC_IEN_IR_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_PCC_IEN_IR) | (((val) & 0x1) << 4))
/* Enable of Interrupt Request OC event (3) */
#define LTQ_EBU_PCC_IEN_OC   (0x1 << 3)
#define LTQ_EBU_PCC_IEN_OC_VAL(val)   (((val) & 0x1) << 3)
#define LTQ_EBU_PCC_IEN_OC_GET(val)   ((((val) & LTQ_EBU_PCC_IEN_OC) >> 3) & 0x1)
#define LTQ_EBU_PCC_IEN_OC_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_PCC_IEN_OC) | (((val) & 0x1) << 3))
/* Enable of Interrupt Request Socket Power On (2) */
#define LTQ_EBU_PCC_IEN_PWRON   (0x1 << 2)
#define LTQ_EBU_PCC_IEN_PWRON_VAL(val)   (((val) & 0x1) << 2)
#define LTQ_EBU_PCC_IEN_PWRON_GET(val)   ((((val) & LTQ_EBU_PCC_IEN_PWRON) >> 2) & 0x1)
#define LTQ_EBU_PCC_IEN_PWRON_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_PCC_IEN_PWRON) | (((val) & 0x1) << 2))
/* Enable of Interrupt Request Voltage Sense (1) */
#define LTQ_EBU_PCC_IEN_VS   (0x1 << 1)
#define LTQ_EBU_PCC_IEN_VS_VAL(val)   (((val) & 0x1) << 1)
#define LTQ_EBU_PCC_IEN_VS_GET(val)   ((((val) & LTQ_EBU_PCC_IEN_VS) >> 1) & 0x1)
#define LTQ_EBU_PCC_IEN_VS_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_PCC_IEN_VS) | (((val) & 0x1) << 1))
/* Enable of Interrupt Request Card Detect (0) */
#define LTQ_EBU_PCC_IEN_CD   (0x1)
#define LTQ_EBU_PCC_IEN_CD_VAL(val)   (((val) & 0x1) << 0)
#define LTQ_EBU_PCC_IEN_CD_GET(val)   ((((val) & LTQ_EBU_PCC_IEN_CD) >> 0) & 0x1)
#define LTQ_EBU_PCC_IEN_CD_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_PCC_IEN_CD) | (((val) & 0x1) << 0))

/*******************************************************************************
 * PCC Interrupt Output Status Register
 ******************************************************************************/

/* Status of Interrupt Request IR (4) */
#define LTQ_EBU_PCC_INT_OUT_IR   (0x1 << 4)
#define LTQ_EBU_PCC_INT_OUT_IR_GET(val)   ((((val) & LTQ_EBU_PCC_INT_OUT_IR) >> 4) & 0x1)
/* Status of Interrupt Request OC (3) */
#define LTQ_EBU_PCC_INT_OUT_OC   (0x1 << 3)
#define LTQ_EBU_PCC_INT_OUT_OC_GET(val)   ((((val) & LTQ_EBU_PCC_INT_OUT_OC) >> 3) & 0x1)
/* Status of Interrupt Request Socket Power On (2) */
#define LTQ_EBU_PCC_INT_OUT_PWRON   (0x1 << 2)
#define LTQ_EBU_PCC_INT_OUT_PWRON_GET(val)   ((((val) & LTQ_EBU_PCC_INT_OUT_PWRON) >> 2) & 0x1)
/* Status of Interrupt Request Voltage Sense (1) */
#define LTQ_EBU_PCC_INT_OUT_VS   (0x1 << 1)
#define LTQ_EBU_PCC_INT_OUT_VS_GET(val)   ((((val) & LTQ_EBU_PCC_INT_OUT_VS) >> 1) & 0x1)
/* Status of Interrupt Request Card Detect (0) */
#define LTQ_EBU_PCC_INT_OUT_CD   (0x1)
#define LTQ_EBU_PCC_INT_OUT_CD_GET(val)   ((((val) & LTQ_EBU_PCC_INT_OUT_CD) >> 0) & 0x1)

/*******************************************************************************
 * PCC Interrupt Request Set Register
 ******************************************************************************/

/* Set Interrupt Request IR (4) */
#define LTQ_EBU_PCC_IRS_IR   (0x1 << 4)
#define LTQ_EBU_PCC_IRS_IR_VAL(val)   (((val) & 0x1) << 4)
#define LTQ_EBU_PCC_IRS_IR_SET(reg,val) (reg) = (((reg & ~LTQ_EBU_PCC_IRS_IR) | (val) & 1) << 4)
/* Set Interrupt Request OC (3) */
#define LTQ_EBU_PCC_IRS_OC   (0x1 << 3)
#define LTQ_EBU_PCC_IRS_OC_VAL(val)   (((val) & 0x1) << 3)
#define LTQ_EBU_PCC_IRS_OC_SET(reg,val) (reg) = (((reg & ~LTQ_EBU_PCC_IRS_OC) | (val) & 1) << 3)
/* Set Interrupt Request Socket Power On (2) */
#define LTQ_EBU_PCC_IRS_PWRON   (0x1 << 2)
#define LTQ_EBU_PCC_IRS_PWRON_VAL(val)   (((val) & 0x1) << 2)
#define LTQ_EBU_PCC_IRS_PWRON_SET(reg,val) (reg) = (((reg & ~LTQ_EBU_PCC_IRS_PWRON) | (val) & 1) << 2)
/* Set Interrupt Request Voltage Sense (1) */
#define LTQ_EBU_PCC_IRS_VS   (0x1 << 1)
#define LTQ_EBU_PCC_IRS_VS_VAL(val)   (((val) & 0x1) << 1)
#define LTQ_EBU_PCC_IRS_VS_SET(reg,val) (reg) = (((reg & ~LTQ_EBU_PCC_IRS_VS) | (val) & 1) << 1)
/* Set Interrupt Request Card Detect (0) */
#define LTQ_EBU_PCC_IRS_CD   (0x1)
#define LTQ_EBU_PCC_IRS_CD_VAL(val)   (((val) & 0x1) << 0)
#define LTQ_EBU_PCC_IRS_CD_SET(reg,val) (reg) = (((reg & ~LTQ_EBU_PCC_IRS_CD) | (val) & 1) << 0)

/*******************************************************************************
 * NAND Flash Control Register
 ******************************************************************************/

/* ECC Enabling (31) */
#define LTQ_EBU_NAND_CON_ECC_ON   (0x1 << 31)
#define LTQ_EBU_NAND_CON_ECC_ON_VAL(val)   (((val) & 0x1) << 31)
#define LTQ_EBU_NAND_CON_ECC_ON_GET(val)   ((((val) & LTQ_EBU_NAND_CON_ECC_ON) >> 31) & 0x1)
#define LTQ_EBU_NAND_CON_ECC_ON_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_NAND_CON_ECC_ON) | (((val) & 0x1) << 31))
/* Latch enable (23:18) */
#define LTQ_EBU_NAND_CON_LAT_EN   (0x3f << 18)
#define LTQ_EBU_NAND_CON_LAT_EN_VAL(val)   (((val) & 0x3f) << 18)
#define LTQ_EBU_NAND_CON_LAT_EN_GET(val)   ((((val) & LTQ_EBU_NAND_CON_LAT_EN) >> 18) & 0x3f)
#define LTQ_EBU_NAND_CON_LAT_EN_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_NAND_CON_LAT_EN) | (((val) & 0x3f) << 18))
/* Output ChipSelect# Selection (11:10) */
#define LTQ_EBU_NAND_CON_OUT_CS_S   (0x3 << 10)
#define LTQ_EBU_NAND_CON_OUT_CS_S_VAL(val)   (((val) & 0x3) << 10)
#define LTQ_EBU_NAND_CON_OUT_CS_S_GET(val)   ((((val) & LTQ_EBU_NAND_CON_OUT_CS_S) >> 10) & 0x3)
#define LTQ_EBU_NAND_CON_OUT_CS_S_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_NAND_CON_OUT_CS_S) | (((val) & 0x3) << 10))
/* Input ChipSelect# Selection (9:8) */
#define LTQ_EBU_NAND_CON_IN_CS_S   (0x3 << 8)
#define LTQ_EBU_NAND_CON_IN_CS_S_VAL(val)   (((val) & 0x3) << 8)
#define LTQ_EBU_NAND_CON_IN_CS_S_GET(val)   ((((val) & LTQ_EBU_NAND_CON_IN_CS_S) >> 8) & 0x3)
#define LTQ_EBU_NAND_CON_IN_CS_S_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_NAND_CON_IN_CS_S) | (((val) & 0x3) << 8))
/* Set PRE (7) */
#define LTQ_EBU_NAND_CON_PRE_P   (0x1 << 7)
#define LTQ_EBU_NAND_CON_PRE_P_VAL(val)   (((val) & 0x1) << 7)
#define LTQ_EBU_NAND_CON_PRE_P_GET(val)   ((((val) & LTQ_EBU_NAND_CON_PRE_P) >> 7) & 0x1)
#define LTQ_EBU_NAND_CON_PRE_P_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_NAND_CON_PRE_P) | (((val) & 0x1) << 7))
/* Set WP Active Polarity (6) */
#define LTQ_EBU_NAND_CON_WP_P   (0x1 << 6)
#define LTQ_EBU_NAND_CON_WP_P_VAL(val)   (((val) & 0x1) << 6)
#define LTQ_EBU_NAND_CON_WP_P_GET(val)   ((((val) & LTQ_EBU_NAND_CON_WP_P) >> 6) & 0x1)
#define LTQ_EBU_NAND_CON_WP_P_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_NAND_CON_WP_P) | (((val) & 0x1) << 6))
/* Set SE Active Polarity (5) */
#define LTQ_EBU_NAND_CON_SE_P   (0x1 << 5)
#define LTQ_EBU_NAND_CON_SE_P_VAL(val)   (((val) & 0x1) << 5)
#define LTQ_EBU_NAND_CON_SE_P_GET(val)   ((((val) & LTQ_EBU_NAND_CON_SE_P) >> 5) & 0x1)
#define LTQ_EBU_NAND_CON_SE_P_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_NAND_CON_SE_P) | (((val) & 0x1) << 5))
/* Set CS Active Polarity (4) */
#define LTQ_EBU_NAND_CON_CS_P   (0x1 << 4)
#define LTQ_EBU_NAND_CON_CS_P_VAL(val)   (((val) & 0x1) << 4)
#define LTQ_EBU_NAND_CON_CS_P_GET(val)   ((((val) & LTQ_EBU_NAND_CON_CS_P) >> 4) & 0x1)
#define LTQ_EBU_NAND_CON_CS_P_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_NAND_CON_CS_P) | (((val) & 0x1) << 4))
/* Set CLE Active Polarity (3) */
#define LTQ_EBU_NAND_CON_CLE_P   (0x1 << 3)
#define LTQ_EBU_NAND_CON_CLE_P_VAL(val)   (((val) & 0x1) << 3)
#define LTQ_EBU_NAND_CON_CLE_P_GET(val)   ((((val) & LTQ_EBU_NAND_CON_CLE_P) >> 3) & 0x1)
#define LTQ_EBU_NAND_CON_CLE_P_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_NAND_CON_CLE_P) | (((val) & 0x1) << 3))
/* Set ALE Active Polarity (2) */
#define LTQ_EBU_NAND_CON_ALE_P   (0x1 << 2)
#define LTQ_EBU_NAND_CON_ALE_P_VAL(val)   (((val) & 0x1) << 2)
#define LTQ_EBU_NAND_CON_ALE_P_GET(val)   ((((val) & LTQ_EBU_NAND_CON_ALE_P) >> 2) & 0x1)
#define LTQ_EBU_NAND_CON_ALE_P_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_NAND_CON_ALE_P) | (((val) & 0x1) << 2))
/* NAND CS Mux with EBU CS Enable (1) */
#define LTQ_EBU_NAND_CON_CSMUX_E   (0x1 << 1)
#define LTQ_EBU_NAND_CON_CSMUX_E_VAL(val)   (((val) & 0x1) << 1)
#define LTQ_EBU_NAND_CON_CSMUX_E_GET(val)   ((((val) & LTQ_EBU_NAND_CON_CSMUX_E) >> 1) & 0x1)
#define LTQ_EBU_NAND_CON_CSMUX_E_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_NAND_CON_CSMUX_E) | (((val) & 0x1) << 1))
/* NAND FLASH Mode Support (0) */
#define LTQ_EBU_NAND_CON_NANDMODE   (0x1)
#define LTQ_EBU_NAND_CON_NANDMODE_VAL(val)   (((val) & 0x1) << 0)
#define LTQ_EBU_NAND_CON_NANDMODE_GET(val)   ((((val) & LTQ_EBU_NAND_CON_NANDMODE) >> 0) & 0x1)
#define LTQ_EBU_NAND_CON_NANDMODE_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_NAND_CON_NANDMODE) | (((val) & 0x1) << 0))

/*******************************************************************************
 * NAND Flash State Register
 ******************************************************************************/

/* Reserved (31:3) */
#define LTQ_EBU_NAND_WAIT_RES   (0x1fffffff << 3)
#define LTQ_EBU_NAND_WAIT_RES_GET(val)   ((((val) & LTQ_EBU_NAND_WAIT_RES) >> 3) & 0x1fffffff)
/* NAND Write Complete (3) */
#define LTQ_EBU_NAND_WAIT_WR_C   (0x1 << 3)
#define LTQ_EBU_NAND_WAIT_WR_C_GET(val)   ((((val) & LTQ_EBU_NAND_WAIT_WR_C) >> 3) & 0x1)
/* Record the RD Edge (rising ) (2) */
#define LTQ_EBU_NAND_WAIT_RD_EDGE   (0x1 << 2)
#define LTQ_EBU_NAND_WAIT_RD_EDGE_GET(val)   ((((val) & LTQ_EBU_NAND_WAIT_RD_EDGE) >> 2) & 0x1)
/* Record the BY# Edge (falling) (1) */
#define LTQ_EBU_NAND_WAIT_BY_EDGE   (0x1 << 1)
#define LTQ_EBU_NAND_WAIT_BY_EDGE_GET(val)   ((((val) & LTQ_EBU_NAND_WAIT_BY_EDGE) >> 1) & 0x1)
/* Rd/BY# value (0) */
#define LTQ_EBU_NAND_WAIT_RDBY_VALUE   (0x1)
#define LTQ_EBU_NAND_WAIT_RDBY_VALUE_GET(val)   ((((val) & LTQ_EBU_NAND_WAIT_RDBY_VALUE) >> 0) & 0x1)

/*******************************************************************************
 * NAND ECC Result Register 0
 ******************************************************************************/

/* Reserved (31:24) */
#define LTQ_EBU_NAND_ECC0_RES   (0xff << 24)
#define LTQ_EBU_NAND_ECC0_RES_GET(val)   ((((val) & LTQ_EBU_NAND_ECC0_RES) >> 24) & 0xff)
/* ECC value (23:16) */
#define LTQ_EBU_NAND_ECC0_ECC_B2   (0xff << 16)
#define LTQ_EBU_NAND_ECC0_ECC_B2_VAL(val)   (((val) & 0xff) << 16)
#define LTQ_EBU_NAND_ECC0_ECC_B2_GET(val)   ((((val) & LTQ_EBU_NAND_ECC0_ECC_B2) >> 16) & 0xff)
#define LTQ_EBU_NAND_ECC0_ECC_B2_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_NAND_ECC0_ECC_B2) | (((val) & 0xff) << 16))
/* ECC value (15:8) */
#define LTQ_EBU_NAND_ECC0_ECC_B1   (0xff << 8)
#define LTQ_EBU_NAND_ECC0_ECC_B1_VAL(val)   (((val) & 0xff) << 8)
#define LTQ_EBU_NAND_ECC0_ECC_B1_GET(val)   ((((val) & LTQ_EBU_NAND_ECC0_ECC_B1) >> 8) & 0xff)
#define LTQ_EBU_NAND_ECC0_ECC_B1_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_NAND_ECC0_ECC_B1) | (((val) & 0xff) << 8))
/* ECC value (7:0) */
#define LTQ_EBU_NAND_ECC0_ECC_B0   (0xff)
#define LTQ_EBU_NAND_ECC0_ECC_B0_VAL(val)   (((val) & 0xff) << 0)
#define LTQ_EBU_NAND_ECC0_ECC_B0_GET(val)   ((((val) & LTQ_EBU_NAND_ECC0_ECC_B0) >> 0) & 0xff)
#define LTQ_EBU_NAND_ECC0_ECC_B0_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_NAND_ECC0_ECC_B0) | (((val) & 0xff) << 0))

/*******************************************************************************
 * NAND ECC Address Counter Register
 ******************************************************************************/

/* Reserved (31:9) */
#define LTQ_EBU_NAND_ECC_AC_RES   (0x7fffff << 9)
#define LTQ_EBU_NAND_ECC_AC_RES_GET(val)   ((((val) & LTQ_EBU_NAND_ECC_AC_RES) >> 9) & 0x7fffff)
/* ECC address counter (8:0) */
#define LTQ_EBU_NAND_ECC_AC_ECC_AC   (0x1ff)
#define LTQ_EBU_NAND_ECC_AC_ECC_AC_VAL(val)   (((val) & 0x1ff) << 0)
#define LTQ_EBU_NAND_ECC_AC_ECC_AC_GET(val)   ((((val) & LTQ_EBU_NAND_ECC_AC_ECC_AC) >> 0) & 0x1ff)
#define LTQ_EBU_NAND_ECC_AC_ECC_AC_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_NAND_ECC_AC_ECC_AC) | (((val) & 0x1ff) << 0))

/*******************************************************************************
 * Internal Address Emulation Register
 ******************************************************************************/

/* Memory Region Base Address (31:12) */
#define LTQ_EBU_EMU_ADDR_BASE   (0xfffff << 12)
#define LTQ_EBU_EMU_ADDR_BASE_VAL(val)   (((val) & 0xfffff) << 12)
#define LTQ_EBU_EMU_ADDR_BASE_GET(val)   ((((val) & LTQ_EBU_EMU_ADDR_BASE) >> 12) & 0xfffff)
#define LTQ_EBU_EMU_ADDR_BASE_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_EMU_ADDR_BASE) | (((val) & 0xfffff) << 12))
/* Memory Region Address Mask (7:4) */
#define LTQ_EBU_EMU_ADDR_MASK   (0xf << 4)
#define LTQ_EBU_EMU_ADDR_MASK_VAL(val)   (((val) & 0xf) << 4)
#define LTQ_EBU_EMU_ADDR_MASK_GET(val)   ((((val) & LTQ_EBU_EMU_ADDR_MASK) >> 4) & 0xf)
#define LTQ_EBU_EMU_ADDR_MASK_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_EMU_ADDR_MASK) | (((val) & 0xf) << 4))
/* Memory Region Mirror Segment B Control (1) */
#define LTQ_EBU_EMU_ADDR_MRMB   (0x1 << 1)
#define LTQ_EBU_EMU_ADDR_MRMB_VAL(val)   (((val) & 0x1) << 1)
#define LTQ_EBU_EMU_ADDR_MRMB_GET(val)   ((((val) & LTQ_EBU_EMU_ADDR_MRMB) >> 1) & 0x1)
#define LTQ_EBU_EMU_ADDR_MRMB_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_EMU_ADDR_MRMB) | (((val) & 0x1) << 1))
/* Memory Region Enable Control (0) */
#define LTQ_EBU_EMU_ADDR_MREC   (0x1)
#define LTQ_EBU_EMU_ADDR_MREC_VAL(val)   (((val) & 0x1) << 0)
#define LTQ_EBU_EMU_ADDR_MREC_GET(val)   ((((val) & LTQ_EBU_EMU_ADDR_MREC) >> 0) & 0x1)
#define LTQ_EBU_EMU_ADDR_MREC_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_EMU_ADDR_MREC) | (((val) & 0x1) << 0))

/*******************************************************************************
 * nternal Emulator Configuration Register
 ******************************************************************************/

/* Overlay Memory Control Region 3 (3) */
#define LTQ_EBU_EMU_CON_OVL3   (0x1 << 3)
#define LTQ_EBU_EMU_CON_OVL3_VAL(val)   (((val) & 0x1) << 3)
#define LTQ_EBU_EMU_CON_OVL3_GET(val)   ((((val) & LTQ_EBU_EMU_CON_OVL3) >> 3) & 0x1)
#define LTQ_EBU_EMU_CON_OVL3_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_EMU_CON_OVL3) | (((val) & 0x1) << 3))
/* Overlay Memory Control Region 2 (2) */
#define LTQ_EBU_EMU_CON_OVL2   (0x1 << 2)
#define LTQ_EBU_EMU_CON_OVL2_VAL(val)   (((val) & 0x1) << 2)
#define LTQ_EBU_EMU_CON_OVL2_GET(val)   ((((val) & LTQ_EBU_EMU_CON_OVL2) >> 2) & 0x1)
#define LTQ_EBU_EMU_CON_OVL2_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_EMU_CON_OVL2) | (((val) & 0x1) << 2))
/* Overlay Memory Control Region 1 (1) */
#define LTQ_EBU_EMU_CON_OVL1   (0x1 << 1)
#define LTQ_EBU_EMU_CON_OVL1_VAL(val)   (((val) & 0x1) << 1)
#define LTQ_EBU_EMU_CON_OVL1_GET(val)   ((((val) & LTQ_EBU_EMU_CON_OVL1) >> 1) & 0x1)
#define LTQ_EBU_EMU_CON_OVL1_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_EMU_CON_OVL1) | (((val) & 0x1) << 1))
/* Overlay Memory Control Region 0 (0) */
#define LTQ_EBU_EMU_CON_OVL0   (0x1)
#define LTQ_EBU_EMU_CON_OVL0_VAL(val)   (((val) & 0x1) << 0)
#define LTQ_EBU_EMU_CON_OVL0_GET(val)   ((((val) & LTQ_EBU_EMU_CON_OVL0) >> 0) & 0x1)
#define LTQ_EBU_EMU_CON_OVL0_SET(reg,val) (reg) = ((reg & ~LTQ_EBU_EMU_CON_OVL0) | (((val) & 0x1) << 0))

#endif /* __LTQ_EBU_H */
