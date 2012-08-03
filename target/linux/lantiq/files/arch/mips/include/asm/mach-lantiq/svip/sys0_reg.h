/******************************************************************************

  Copyright (c) 2007
  Infineon Technologies AG
  St. Martin Strasse 53; 81669 Munich, Germany

  Any use of this Software is subject to the conclusion of a respective
  License Agreement. Without such a License Agreement no rights to the
  Software are granted.

 ******************************************************************************/

#ifndef __SYS0_REG_H
#define __SYS0_REG_H

#define sys0_r32(reg) ltq_r32(&sys0->reg)
#define sys0_w32(val, reg) ltq_w32(val, &sys0->reg)
#define sys0_w32_mask(clear, set, reg) ltq_w32_mask(clear, set, &sys0->reg)

/** SYS0 register structure */
struct svip_reg_sys0 {
	unsigned long sr; /* 0x0000 */
	unsigned long bcr; /* 0x0004 */
	unsigned long pll1cr; /* 0x0008 */
	unsigned long pll2cr; /* 0x000c */
	unsigned long tscr; /* 0x0010 */
	unsigned long phyclkr; /* 0x0014 */
};

/*******************************************************************************
 * SYS0 Status Register
 ******************************************************************************/

/* Endian select pin (31) */
#define SYS0_SR_ESEL   (0x1 << 31)
#define SYS0_SR_ESEL_GET(val)   ((((val) & SYS0_SR_ESEL) >> 31) & 0x1)
/* Boot mode pins (27:24) */
#define SYS0_SR_BMODE   (0xf << 24)
#define SYS0_SR_BMODE_GET(val)   ((((val) & SYS0_SR_BMODE) >> 24) & 0xf)
/* PLL2 Lock (18) */
#define SYS0_SR_PLL2LOCK   (0x1 << 18)
#define SYS0_SR_PLL2LOCK_GET(val)   ((((val) & SYS0_SR_PLL2LOCK) >> 18) & 0x1)
/* PLL1 Lock (17) */
#define SYS0_SR_PLL1LOCK   (0x1 << 17)
#define SYS0_SR_PLL1LOCK_GET(val)   ((((val) & SYS0_SR_PLL1LOCK) >> 17) & 0x1)
/* Discrete Timing Oscillator Lock (16) */
#define SYS0_SR_DTOLOCK   (0x1 << 16)
#define SYS0_SR_DTOLOCK_GET(val)   ((((val) & SYS0_SR_DTOLOCK) >> 16) & 0x1)
/* Hardware Reset Indication (1) */
#define SYS0_SR_HRSTIN   (0x1 << 1)
#define SYS0_SR_HRSTIN_VAL(val)   (((val) & 0x1) << 1)
#define SYS0_SR_HRSTIN_GET(val)   ((((val) & SYS0_SR_HRSTIN) >> 1) & 0x1)
#define SYS0_SR_HRSTIN_SET(reg,val) (reg) = ((reg & ~SYS0_SR_HRSTIN) | (((val) & 0x1) << 1))
/* Power-on Reset Indication (0) */
#define SYS0_SR_POR   (0x1 << 0)
#define SYS0_SR_POR_VAL(val)   (((val) & 0x1) << 0)
#define SYS0_SR_POR_GET(val)   ((((val) & SYS0_SR_POR) >> 0) & 0x1)
#define SYS0_SR_POR_SET(reg,val) (reg) = ((reg & ~SYS0_SR_POR) | (((val) & 0x1) << 0))

/*******************************************************************************
 * SYS0 Boot Control Register
 ******************************************************************************/

/* Configuration of Boot Source for CPU5 (25) */
#define SYS0_BCR_BMODECPU5   (0x1 << 25)
#define SYS0_BCR_BMODECPU5_VAL(val)   (((val) & 0x1) << 25)
#define SYS0_BCR_BMODECPU5_GET(val)   ((((val) & SYS0_BCR_BMODECPU5) >> 25) & 0x1)
#define SYS0_BCR_BMODECPU5_SET(reg,val) (reg) = ((reg & ~SYS0_BCR_BMODECPU5) | (((val) & 0x1) << 25))
/* Configuration of Boot Source for CPU4 (24) */
#define SYS0_BCR_BMODECPU4   (0x1 << 24)
#define SYS0_BCR_BMODECPU4_VAL(val)   (((val) & 0x1) << 24)
#define SYS0_BCR_BMODECPU4_GET(val)   ((((val) & SYS0_BCR_BMODECPU4) >> 24) & 0x1)
#define SYS0_BCR_BMODECPU4_SET(reg,val) (reg) = ((reg & ~SYS0_BCR_BMODECPU4) | (((val) & 0x1) << 24))
/* Configuration of Boot Source for CPU3 (23) */
#define SYS0_BCR_BMODECPU3   (0x1 << 23)
#define SYS0_BCR_BMODECPU3_VAL(val)   (((val) & 0x1) << 23)
#define SYS0_BCR_BMODECPU3_GET(val)   ((((val) & SYS0_BCR_BMODECPU3) >> 23) & 0x1)
#define SYS0_BCR_BMODECPU3_SET(reg,val) (reg) = ((reg & ~SYS0_BCR_BMODECPU3) | (((val) & 0x1) << 23))
/* Configuration of Boot Source for CPU2 (22) */
#define SYS0_BCR_BMODECPU2   (0x1 << 22)
#define SYS0_BCR_BMODECPU2_VAL(val)   (((val) & 0x1) << 22)
#define SYS0_BCR_BMODECPU2_GET(val)   ((((val) & SYS0_BCR_BMODECPU2) >> 22) & 0x1)
#define SYS0_BCR_BMODECPU2_SET(reg,val) (reg) = ((reg & ~SYS0_BCR_BMODECPU2) | (((val) & 0x1) << 22))
/* Configuration of Boot Source for CPU1 (21) */
#define SYS0_BCR_BMODECPU1   (0x1 << 21)
#define SYS0_BCR_BMODECPU1_VAL(val)   (((val) & 0x1) << 21)
#define SYS0_BCR_BMODECPU1_GET(val)   ((((val) & SYS0_BCR_BMODECPU1) >> 21) & 0x1)
#define SYS0_BCR_BMODECPU1_SET(reg,val) (reg) = ((reg & ~SYS0_BCR_BMODECPU1) | (((val) & 0x1) << 21))
/* Configuration of Boot Source for CPU0 (20:16) */
#define SYS0_BCR_BMODECPU0   (0x1f << 16)
#define SYS0_BCR_BMODECPU0_VAL(val)   (((val) & 0x1f) << 16)
#define SYS0_BCR_BMODECPU0_GET(val)   ((((val) & SYS0_BCR_BMODECPU0) >> 16) & 0x1f)
#define SYS0_BCR_BMODECPU0_SET(reg,val) (reg) = ((reg & ~SYS0_BCR_BMODECPU0) | (((val) & 0x1f) << 16))
/* Configuration of Endianess for CPU5 (5) */
#define SYS0_BCR_ESELCPU5   (0x1 << 5)
#define SYS0_BCR_ESELCPU5_VAL(val)   (((val) & 0x1) << 5)
#define SYS0_BCR_ESELCPU5_GET(val)   ((((val) & SYS0_BCR_ESELCPU5) >> 5) & 0x1)
#define SYS0_BCR_ESELCPU5_SET(reg,val) (reg) = ((reg & ~SYS0_BCR_ESELCPU5) | (((val) & 0x1) << 5))
/* Configuration of Endianess for CPU4 (4) */
#define SYS0_BCR_ESELCPU4   (0x1 << 4)
#define SYS0_BCR_ESELCPU4_VAL(val)   (((val) & 0x1) << 4)
#define SYS0_BCR_ESELCPU4_GET(val)   ((((val) & SYS0_BCR_ESELCPU4) >> 4) & 0x1)
#define SYS0_BCR_ESELCPU4_SET(reg,val) (reg) = ((reg & ~SYS0_BCR_ESELCPU4) | (((val) & 0x1) << 4))
/* Configuration of Endianess for CPU3 (3) */
#define SYS0_BCR_ESELCPU3   (0x1 << 3)
#define SYS0_BCR_ESELCPU3_VAL(val)   (((val) & 0x1) << 3)
#define SYS0_BCR_ESELCPU3_GET(val)   ((((val) & SYS0_BCR_ESELCPU3) >> 3) & 0x1)
#define SYS0_BCR_ESELCPU3_SET(reg,val) (reg) = ((reg & ~SYS0_BCR_ESELCPU3) | (((val) & 0x1) << 3))
/* Configuration of Endianess for CPU2 (2) */
#define SYS0_BCR_ESELCPU2   (0x1 << 2)
#define SYS0_BCR_ESELCPU2_VAL(val)   (((val) & 0x1) << 2)
#define SYS0_BCR_ESELCPU2_GET(val)   ((((val) & SYS0_BCR_ESELCPU2) >> 2) & 0x1)
#define SYS0_BCR_ESELCPU2_SET(reg,val) (reg) = ((reg & ~SYS0_BCR_ESELCPU2) | (((val) & 0x1) << 2))
/* Configuration of Endianess for CPU1 (1) */
#define SYS0_BCR_ESELCPU1   (0x1 << 1)
#define SYS0_BCR_ESELCPU1_VAL(val)   (((val) & 0x1) << 1)
#define SYS0_BCR_ESELCPU1_GET(val)   ((((val) & SYS0_BCR_ESELCPU1) >> 1) & 0x1)
#define SYS0_BCR_ESELCPU1_SET(reg,val) (reg) = ((reg & ~SYS0_BCR_ESELCPU1) | (((val) & 0x1) << 1))
/* Configuration of Endianess for CPU0  (0) */
#define SYS0_BCR_ESELCPU0   (0x1)
#define SYS0_BCR_ESELCPU0_VAL(val)   (((val) & 0x1) << 0)
#define SYS0_BCR_ESELCPU0_GET(val)   ((((val) & SYS0_BCR_ESELCPU0) >> 0) & 0x1)
#define SYS0_BCR_ESELCPU0_SET(reg,val) (reg) = ((reg & ~SYS0_BCR_ESELCPU0) | (((val) & 0x1) << 0))

/*******************************************************************************
 * PLL1 Control Register
 ******************************************************************************/

/* PLL1 Bypass Enable (31) */
#define SYS0_PLL1CR_OSCBYP   (0x1 << 31)
#define SYS0_PLL1CR_OSCBYP_VAL(val)   (((val) & 0x1) << 31)
#define SYS0_PLL1CR_OSCBYP_GET(val)   ((((val) & SYS0_PLL1CR_OSCBYP) >> 31) & 0x1)
#define SYS0_PLL1CR_OSCBYP_SET(reg,val) (reg) = ((reg & ~SYS0_PLL1CR_OSCBYP) | (((val) & 0x1) << 31))
/* PLL1 Divider Value (1:0) */
#define SYS0_PLL1CR_PLLDIV   (0x3)
#define SYS0_PLL1CR_PLLDIV_VAL(val)   (((val) & 0x3) << 0)
#define SYS0_PLL1CR_PLLDIV_GET(val)   ((((val) & SYS0_PLL1CR_PLLDIV) >> 0) & 0x3)
#define SYS0_PLL1CR_PLLDIV_SET(reg,val) (reg) = ((reg & ~SYS0_PLL1CR_PLLDIV) | (((val) & 0x3) << 0))

/*******************************************************************************
 * PLL2 Control Register
 ******************************************************************************/

/* PLL2 clear deepsleep (31) */
#define SYS0_PLL2CR_CLRDS   (0x1 << 31)
#define SYS0_PLL2CR_CLRDS_VAL(val)   (((val) & 0x1) << 31)
#define SYS0_PLL2CR_CLRDS_GET(val)   ((((val) & SYS0_PLL2CR_CLRDS) >> 31) & 0x1)
#define SYS0_PLL2CR_CLRDS_SET(reg,val) (reg) = ((reg & ~SYS0_PLL2CR_CLRDS) | (((val) & 0x1) << 31))
/* PLL2 set deepsleep (30) */
#define SYS0_PLL2CR_SETDS   (0x1 << 30)
#define SYS0_PLL2CR_SETDS_VAL(val)   (((val) & 0x1) << 30)
#define SYS0_PLL2CR_SETDS_GET(val)   ((((val) & SYS0_PLL2CR_SETDS) >> 30) & 0x1)
#define SYS0_PLL2CR_SETDS_SET(reg,val) (reg) = ((reg & ~SYS0_PLL2CR_SETDS) | (((val) & 0x1) << 30))
/* PLL2 Fractional division enable (16) */
#define SYS0_PLL2CR_FRACTEN   (0x1 << 16)
#define SYS0_PLL2CR_FRACTEN_VAL(val)   (((val) & 0x1) << 16)
#define SYS0_PLL2CR_FRACTEN_GET(val)   ((((val) & SYS0_PLL2CR_FRACTEN) >> 16) & 0x1)
#define SYS0_PLL2CR_FRACTEN_SET(reg,val) (reg) = ((reg & ~SYS0_PLL2CR_FRACTEN) | (((val) & 0x1) << 16))
/* PLL2 Fractional division value (9:0) */
#define SYS0_FRACTVAL   (0x3f)
#define SYS0_FRACTVAL_VAL(val)   (((val) & 0x3f) << 0)
#define SYS0_FRACTVAL_GET(val)   ((((val) & SYS0_FRACTVAL) >> 0) & 0x3f)
#define SYS0_FRACTVAL_SET(reg,val) (reg) = ((reg & ~SYS0_FRACTVAL) | (((val) & 0x3f) << 0))

#endif
