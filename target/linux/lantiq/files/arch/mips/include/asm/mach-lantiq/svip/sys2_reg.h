/******************************************************************************

  Copyright (c) 2007
  Infineon Technologies AG
  St. Martin Strasse 53; 81669 Munich, Germany

  Any use of this Software is subject to the conclusion of a respective
  License Agreement. Without such a License Agreement no rights to the
  Software are granted.

 ******************************************************************************/

#ifndef __SYS2_REG_H
#define __SYS2_REG_H

#define sys2_r32(reg) ltq_r32(&sys2->reg)
#define sys2_w32(val, reg) ltq_w32(val, &sys2->reg)
#define sys2_w32_mask(clear, set, reg) ltq_w32_mask(clear, set, &sys2->reg)

/** SYS2 register structure */
struct svip_reg_sys2 {
	volatile unsigned long  clksr;  /*  0x0000 */
	volatile unsigned long  clkenr;  /*  0x0004 */
	volatile unsigned long  clkclr;  /*  0x0008 */
	volatile unsigned long  reserved0[1];
	volatile unsigned long  rsr;  /*  0x0010 */
	volatile unsigned long  rreqr;  /*  0x0014 */
	volatile unsigned long  rrlsr;  /*  0x0018 */
};

/*******************************************************************************
 * SYS2 Clock Status Register
 ******************************************************************************/

/* Clock Enable for PORT4 */
#define SYS2_CLKSR_PORT4 (0x1 << 27)
#define SYS2_CLKSR_PORT4_VAL(val) (((val) & 0x1) << 27)
#define SYS2_CLKSR_PORT4_GET(val) (((val) & SYS2_CLKSR_PORT4) >> 27)
/* Clock Enable for HWSYNC */
#define SYS2_CLKSR_HWSYNC (0x1 << 26)
#define SYS2_CLKSR_HWSYNC_VAL(val) (((val) &
#define SYS2_CLKSR_HWSYNC_GET(val) (((val) & SYS2_CLKSR_HWSYNC) >> 26)
					 /* Clock Enable for MBS */
#define SYS2_CLKSR_MBS (0x1 << 25)
#define SYS2_CLKSR_MBS_VAL(val) (((val) & 0x1) << 25)
#define SYS2_CLKSR_MBS_GET(val) (((val) & SYS2_CLKSR_MBS) >> 25)
					 /* Clock Enable for SWINT */
#define SYS2_CLKSR_SWINT (0x1 << 24)
#define SYS2_CLKSR_SWINT_VAL(val) (((val) & 0x1) << 24)
#define SYS2_CLKSR_SWINT_GET(val) (((val) & SYS2_CLKSR_SWINT) >> 24)
					 /* Clock Enable for HWACC3 */
#define SYS2_CLKSR_HWACC3 (0x1 << 19)
#define SYS2_CLKSR_HWACC3_VAL(val) (((val) &
#define SYS2_CLKSR_HWACC3_GET(val) (((val) & SYS2_CLKSR_HWACC3) >> 19)
					 /* Clock Enable for HWACC2 */
#define SYS2_CLKSR_HWACC2 (0x1 << 18)
#define SYS2_CLKSR_HWACC2_VAL(val) (((val) &
#define SYS2_CLKSR_HWACC2_GET(val) (((val) & SYS2_CLKSR_HWACC2) >> 18)
					 /* Clock Enable for HWACC1 */
#define SYS2_CLKSR_HWACC1 (0x1 << 17)
#define SYS2_CLKSR_HWACC1_VAL(val) (((val) &
#define SYS2_CLKSR_HWACC1_GET(val) (((val) & SYS2_CLKSR_HWACC1) >> 17)
					 /* Clock Enable for HWACC0 */
#define SYS2_CLKSR_HWACC0 (0x1 << 16)
#define SYS2_CLKSR_HWACC0_VAL(val) (((val) &
#define SYS2_CLKSR_HWACC0_GET(val) (((val) & SYS2_CLKSR_HWACC0) >> 16)
					 /* Clock Enable for SIF7 */
#define SYS2_CLKSR_SIF7 (0x1 << 15)
#define SYS2_CLKSR_SIF7_VAL(val) (((val) & 0x1) << 15)
#define SYS2_CLKSR_SIF7_GET(val) (((val) & SYS2_CLKSR_SIF7) >> 15)
					 /* Clock Enable for SIF6 */
#define SYS2_CLKSR_SIF6 (0x1 << 14)
#define SYS2_CLKSR_SIF6_VAL(val) (((val) & 0x1) << 14)
#define SYS2_CLKSR_SIF6_GET(val) (((val) & SYS2_CLKSR_SIF6) >> 14)
					 /* Clock Enable for SIF5 */
#define SYS2_CLKSR_SIF5 (0x1 << 13)
#define SYS2_CLKSR_SIF5_VAL(val) (((val) & 0x1) << 13)
#define SYS2_CLKSR_SIF5_GET(val) (((val) & SYS2_CLKSR_SIF5) >> 13)
					 /* Clock Enable for SIF4 */
#define SYS2_CLKSR_SIF4 (0x1 << 12)
#define SYS2_CLKSR_SIF4_VAL(val) (((val) & 0x1) << 12)
#define SYS2_CLKSR_SIF4_GET(val) (((val) & SYS2_CLKSR_SIF4) >> 12)
					 /* Clock Enable for SIF3 */
#define SYS2_CLKSR_SIF3 (0x1 << 11)
#define SYS2_CLKSR_SIF3_VAL(val) (((val) & 0x1) << 11)
#define SYS2_CLKSR_SIF3_GET(val) (((val) & SYS2_CLKSR_SIF3) >> 11)
/* Clock Enable for SIF2 */
#define SYS2_CLKSR_SIF2 (0x1 << 10)
#define SYS2_CLKSR_SIF2_VAL(val) (((val) & 0x1) << 10)
#define SYS2_CLKSR_SIF2_GET(val) (((val) & SYS2_CLKSR_SIF2) >> 10)
/* Clock Enable for SIF1 */
#define SYS2_CLKSR_SIF1 (0x1 << 9)
#define SYS2_CLKSR_SIF1_VAL(val) (((val) & 0x1) << 9)
#define SYS2_CLKSR_SIF1_GET(val) (((val) & SYS2_CLKSR_SIF1) >> 9)
/* Clock Enable for SIF0 */
#define SYS2_CLKSR_SIF0 (0x1 << 8)
#define SYS2_CLKSR_SIF0_VAL(val) (((val) & 0x1) << 8)
#define SYS2_CLKSR_SIF0_GET(val) (((val) & SYS2_CLKSR_SIF0) >> 8)
/* Clock Enable for DFEV7 */
#define SYS2_CLKSR_DFEV7 (0x1 << 7)
#define SYS2_CLKSR_DFEV7_VAL(val) (((val) & 0x1) << 7)
#define SYS2_CLKSR_DFEV7_GET(val) (((val) & SYS2_CLKSR_DFEV7) >> 7)
/* Clock Enable for DFEV6 */
#define SYS2_CLKSR_DFEV6 (0x1 << 6)
#define SYS2_CLKSR_DFEV6_VAL(val) (((val) & 0x1) << 6)
#define SYS2_CLKSR_DFEV6_GET(val) (((val) & SYS2_CLKSR_DFEV6) >> 6)
/* Clock Enable for DFEV5 */
#define SYS2_CLKSR_DFEV5 (0x1 << 5)
#define SYS2_CLKSR_DFEV5_VAL(val) (((val) & 0x1) << 5)
#define SYS2_CLKSR_DFEV5_GET(val) (((val) & SYS2_CLKSR_DFEV5) >> 5)
/* Clock Enable for DFEV4 */
#define SYS2_CLKSR_DFEV4 (0x1 << 4)
#define SYS2_CLKSR_DFEV4_VAL(val) (((val) & 0x1) << 4)
#define SYS2_CLKSR_DFEV4_GET(val) (((val) & SYS2_CLKSR_DFEV4) >> 4)
/* Clock Enable for DFEV3 */
#define SYS2_CLKSR_DFEV3 (0x1 << 3)
#define SYS2_CLKSR_DFEV3_VAL(val) (((val) & 0x1) << 3)
#define SYS2_CLKSR_DFEV3_GET(val) (((val) & SYS2_CLKSR_DFEV3) >> 3)
/* Clock Enable for DFEV2 */
#define SYS2_CLKSR_DFEV2 (0x1 << 2)
#define SYS2_CLKSR_DFEV2_VAL(val) (((val) & 0x1) << 2)
#define SYS2_CLKSR_DFEV2_GET(val) (((val) & SYS2_CLKSR_DFEV2) >> 2)
/* Clock Enable for DFEV1 */
#define SYS2_CLKSR_DFEV1 (0x1 << 1)
#define SYS2_CLKSR_DFEV1_VAL(val) (((val) & 0x1) << 1)
#define SYS2_CLKSR_DFEV1_GET(val) (((val) & SYS2_CLKSR_DFEV1) >> 1)
/* Clock Enable for DFEV0 */
#define SYS2_CLKSR_DFEV0 (0x1)
#define SYS2_CLKSR_DFEV0_VAL(val) (((val) & 0x1))
#define SYS2_CLKSR_DFEV0_GET(val) ((val) & SYS2_CLKSR_DFEV0)

/*******************************************************************************
 * SYS2 Clock Enable Register
 ******************************************************************************/

/* Clock Enable Request for PORT4 */
#define SYS2_CLKENR_PORT4 (0x1 << 27)
#define SYS2_CLKENR_PORT4_VAL(val) (((val) & 0x1) << 27)
#define SYS2_CLKENR_PORT4_SET (reg,val) (reg) = ((reg & ~SYS2_CLKENR_PORT4) | ((val & 0x1) << 27))
/* Clock Enable Request for HWSYNC */
#define SYS2_CLKENR_HWSYNC (0x1 << 26)
#define SYS2_CLKENR_HWSYNC_VAL(val) (((val) & 0x1) << 26)
#define SYS2_CLKENR_HWSYNC_SET (reg,val) (reg) = ((reg & ~SYS2_CLKENR_HWSYNC) | ((val & 0x1) << 26))
/* Clock Enable Request for MBS */
#define SYS2_CLKENR_MBS (0x1 << 25)
#define SYS2_CLKENR_MBS_VAL(val) (((val) & 0x1) << 25)
#define SYS2_CLKENR_MBS_SET (reg,val) (reg) = ((reg & ~SYS2_CLKENR_MBS) | ((val & 0x1) << 25))
/* Clock Enable Request for SWINT */
#define SYS2_CLKENR_SWINT (0x1 << 24)
#define SYS2_CLKENR_SWINT_VAL(val) (((val) & 0x1) << 24)
#define SYS2_CLKENR_SWINT_SET (reg,val) (reg) = ((reg & ~SYS2_CLKENR_SWINT) | ((val & 0x1) << 24))
/* Clock Enable Request for HWACC3 */
#define SYS2_CLKENR_HWACC3 (0x1 << 19)
#define SYS2_CLKENR_HWACC3_VAL(val) (((val) & 0x1) << 19)
#define SYS2_CLKENR_HWACC3_SET (reg,val) (reg) = ((reg & ~SYS2_CLKENR_HWACC3) | ((val & 0x1) << 19))
/* Clock Enable Request for HWACC2 */
#define SYS2_CLKENR_HWACC2 (0x1 << 18)
#define SYS2_CLKENR_HWACC2_VAL(val) (((val) & 0x1) << 18)
#define SYS2_CLKENR_HWACC2_SET (reg,val) (reg) = ((reg & ~SYS2_CLKENR_HWACC2) | ((val & 0x1) << 18))
/* Clock Enable Request for HWACC1 */
#define SYS2_CLKENR_HWACC1 (0x1 << 17)
#define SYS2_CLKENR_HWACC1_VAL(val) (((val) & 0x1) << 17)
#define SYS2_CLKENR_HWACC1_SET (reg,val) (reg) = ((reg & ~SYS2_CLKENR_HWACC1) | ((val & 0x1) << 17))
/* Clock Enable Request for HWACC0 */
#define SYS2_CLKENR_HWACC0 (0x1 << 16)
#define SYS2_CLKENR_HWACC0_VAL(val) (((val) & 0x1) << 16)
#define SYS2_CLKENR_HWACC0_SET (reg,val) (reg) = ((reg & ~SYS2_CLKENR_HWACC0) | ((val & 0x1) << 16))
/* Clock Enable Request for SIF7 */
#define SYS2_CLKENR_SIF7 (0x1 << 15)
#define SYS2_CLKENR_SIF7_VAL(val) (((val) & 0x1) << 15)
#define SYS2_CLKENR_SIF7_SET (reg,val) (reg) = ((reg & ~SYS2_CLKENR_SIF7) | ((val & 0x1) << 15))
/* Clock Enable Request for SIF6 */
#define SYS2_CLKENR_SIF6 (0x1 << 14)
#define SYS2_CLKENR_SIF6_VAL(val) (((val) & 0x1) << 14)
#define SYS2_CLKENR_SIF6_SET (reg,val) (reg) = ((reg & ~SYS2_CLKENR_SIF6) | ((val & 0x1) << 14))
/* Clock Enable Request for SIF5 */
#define SYS2_CLKENR_SIF5 (0x1 << 13)
#define SYS2_CLKENR_SIF5_VAL(val) (((val) & 0x1) << 13)
#define SYS2_CLKENR_SIF5_SET (reg,val) (reg) = ((reg & ~SYS2_CLKENR_SIF5) | ((val & 0x1) << 13))
/* Clock Enable Request for SIF4 */
#define SYS2_CLKENR_SIF4 (0x1 << 12)
#define SYS2_CLKENR_SIF4_VAL(val) (((val) & 0x1) << 12)
#define SYS2_CLKENR_SIF4_SET (reg,val) (reg) = ((reg & ~SYS2_CLKENR_SIF4) | ((val & 0x1) << 12))
/* Clock Enable Request for SIF3 */
#define SYS2_CLKENR_SIF3 (0x1 << 11)
#define SYS2_CLKENR_SIF3_VAL(val) (((val) & 0x1) << 11)
#define SYS2_CLKENR_SIF3_SET (reg,val) (reg) = ((reg & ~SYS2_CLKENR_SIF3) | ((val & 0x1) << 11))
/* Clock Enable Request for SIF2 */
#define SYS2_CLKENR_SIF2 (0x1 << 10)
#define SYS2_CLKENR_SIF2_VAL(val) (((val) & 0x1) << 10)
#define SYS2_CLKENR_SIF2_SET (reg,val) (reg) = ((reg & ~SYS2_CLKENR_SIF2) | ((val & 0x1) << 10))
/* Clock Enable Request for SIF1 */
#define SYS2_CLKENR_SIF1 (0x1 << 9)
#define SYS2_CLKENR_SIF1_VAL(val) (((val) & 0x1) << 9)
#define SYS2_CLKENR_SIF1_SET (reg,val) (reg) = ((reg & ~SYS2_CLKENR_SIF1) | ((val & 0x1) << 9))
/* Clock Enable Request for SIF0 */
#define SYS2_CLKENR_SIF0 (0x1 << 8)
#define SYS2_CLKENR_SIF0_VAL(val) (((val) & 0x1) << 8)
#define SYS2_CLKENR_SIF0_SET (reg,val) (reg) = ((reg & ~SYS2_CLKENR_SIF0) | ((val & 0x1) << 8))
/* Clock Enable Request for DFEV7 */
#define SYS2_CLKENR_DFEV7 (0x1 << 7)
#define SYS2_CLKENR_DFEV7_VAL(val) (((val) & 0x1) << 7)
#define SYS2_CLKENR_DFEV7_SET (reg,val) (reg) = ((reg & ~SYS2_CLKENR_DFEV7) | ((val & 0x1) << 7))
/* Clock Enable Request for DFEV6 */
#define SYS2_CLKENR_DFEV6 (0x1 << 6)
#define SYS2_CLKENR_DFEV6_VAL(val) (((val) & 0x1) << 6)
#define SYS2_CLKENR_DFEV6_SET (reg,val) (reg) = ((reg & ~SYS2_CLKENR_DFEV6) | ((val & 0x1) << 6))
/* Clock Enable Request for DFEV5 */
#define SYS2_CLKENR_DFEV5 (0x1 << 5)
#define SYS2_CLKENR_DFEV5_VAL(val) (((val) & 0x1) << 5)
#define SYS2_CLKENR_DFEV5_SET (reg,val) (reg) = ((reg & ~SYS2_CLKENR_DFEV5) | ((val & 0x1) << 5))
/* Clock Enable Request for DFEV4 */
#define SYS2_CLKENR_DFEV4 (0x1 << 4)
#define SYS2_CLKENR_DFEV4_VAL(val) (((val) & 0x1) << 4)
#define SYS2_CLKENR_DFEV4_SET (reg,val) (reg) = ((reg & ~SYS2_CLKENR_DFEV4) | ((val & 0x1) << 4))
/* Clock Enable Request for DFEV3 */
#define SYS2_CLKENR_DFEV3 (0x1 << 3)
#define SYS2_CLKENR_DFEV3_VAL(val) (((val) & 0x1) << 3)
#define SYS2_CLKENR_DFEV3_SET (reg,val) (reg) = ((reg & ~SYS2_CLKENR_DFEV3) | ((val & 0x1) << 3))
/* Clock Enable Request for DFEV2 */
#define SYS2_CLKENR_DFEV2 (0x1 << 2)
#define SYS2_CLKENR_DFEV2_VAL(val) (((val) & 0x1) << 2)
#define SYS2_CLKENR_DFEV2_SET (reg,val) (reg) = ((reg & ~SYS2_CLKENR_DFEV2) | ((val & 0x1) << 2))
/* Clock Enable Request for DFEV1 */
#define SYS2_CLKENR_DFEV1 (0x1 << 1)
#define SYS2_CLKENR_DFEV1_VAL(val) (((val) & 0x1) << 1)
#define SYS2_CLKENR_DFEV1_SET (reg,val) (reg) = ((reg & ~SYS2_CLKENR_DFEV1) | ((val & 0x1) << 1))
/* Clock Enable Request for DFEV0 */
#define SYS2_CLKENR_DFEV0 (0x1)
#define SYS2_CLKENR_DFEV0_VAL(val) (((val) & 0x1))
#define SYS2_CLKENR_DFEV0_SET (reg,val) (reg) = ((reg & ~SYS2_CLKENR_DFEV0) | ((val & 0x1)))

/*******************************************************************************
 * SYS2 Clock Clear Register
 ******************************************************************************/

/* Clock Disable Request for PORT4 */
#define SYS2_CLKCLR_PORT4 (0x1 << 27)
#define SYS2_CLKCLR_PORT4_VAL(val) (((val) & 0x1) << 27)
#define SYS2_CLKCLR_PORT4_SET (reg,val) (reg) = ((reg & ~SYS2_CLKCLR_PORT4) | ((val & 0x1) << 27))
/* Clock Disable Request for HWSYNC */
#define SYS2_CLKCLR_HWSYNC (0x1 << 26)
#define SYS2_CLKCLR_HWSYNC_VAL(val) (((val) & 0x1) << 26)
#define SYS2_CLKCLR_HWSYNC_SET (reg,val) (reg) = ((reg & ~SYS2_CLKCLR_HWSYNC) | ((val & 0x1) << 26))
/* Clock Disable Request for MBS */
#define SYS2_CLKCLR_MBS (0x1 << 25)
#define SYS2_CLKCLR_MBS_VAL(val) (((val) & 0x1) << 25)
#define SYS2_CLKCLR_MBS_SET (reg,val) (reg) = ((reg & ~SYS2_CLKCLR_MBS) | ((val & 0x1) << 25))
/* Clock Disable Request for SWINT */
#define SYS2_CLKCLR_SWINT (0x1 << 24)
#define SYS2_CLKCLR_SWINT_VAL(val) (((val) & 0x1) << 24)
#define SYS2_CLKCLR_SWINT_SET (reg,val) (reg) = ((reg & ~SYS2_CLKCLR_SWINT) | ((val & 0x1) << 24))
/* Clock Disable Request for HWACC3 */
#define SYS2_CLKCLR_HWACC3 (0x1 << 19)
#define SYS2_CLKCLR_HWACC3_VAL(val) (((val) & 0x1) << 19)
#define SYS2_CLKCLR_HWACC3_SET (reg,val) (reg) = ((reg & ~SYS2_CLKCLR_HWACC3) | ((val & 0x1) << 19))
/* Clock Disable Request for HWACC2 */
#define SYS2_CLKCLR_HWACC2 (0x1 << 18)
#define SYS2_CLKCLR_HWACC2_VAL(val) (((val) & 0x1) << 18)
#define SYS2_CLKCLR_HWACC2_SET (reg,val) (reg) = ((reg & ~SYS2_CLKCLR_HWACC2) | ((val & 0x1) << 18))
/* Clock Disable Request for HWACC1 */
#define SYS2_CLKCLR_HWACC1 (0x1 << 17)
#define SYS2_CLKCLR_HWACC1_VAL(val) (((val) & 0x1) << 17)
#define SYS2_CLKCLR_HWACC1_SET (reg,val) (reg) = ((reg & ~SYS2_CLKCLR_HWACC1) | ((val & 0x1) << 17))
/* Clock Disable Request for HWACC0 */
#define SYS2_CLKCLR_HWACC0 (0x1 << 16)
#define SYS2_CLKCLR_HWACC0_VAL(val) (((val) & 0x1) << 16)
#define SYS2_CLKCLR_HWACC0_SET (reg,val) (reg) = ((reg & ~SYS2_CLKCLR_HWACC0) | ((val & 0x1) << 16))
/* Clock Disable Request for SIF7 */
#define SYS2_CLKCLR_SIF7 (0x1 << 15)
#define SYS2_CLKCLR_SIF7_VAL(val) (((val) & 0x1) << 15)
#define SYS2_CLKCLR_SIF7_SET (reg,val) (reg) = ((reg & ~SYS2_CLKCLR_SIF7) | ((val & 0x1) << 15))
/* Clock Disable Request for SIF6 */
#define SYS2_CLKCLR_SIF6 (0x1 << 14)
#define SYS2_CLKCLR_SIF6_VAL(val) (((val) & 0x1) << 14)
#define SYS2_CLKCLR_SIF6_SET (reg,val) (reg) = ((reg & ~SYS2_CLKCLR_SIF6) | ((val & 0x1) << 14))
/* Clock Disable Request for SIF5 */
#define SYS2_CLKCLR_SIF5 (0x1 << 13)
#define SYS2_CLKCLR_SIF5_VAL(val) (((val) & 0x1) << 13)
#define SYS2_CLKCLR_SIF5_SET (reg,val) (reg) = ((reg & ~SYS2_CLKCLR_SIF5) | ((val & 0x1) << 13))
/* Clock Disable Request for SIF4 */
#define SYS2_CLKCLR_SIF4 (0x1 << 12)
#define SYS2_CLKCLR_SIF4_VAL(val) (((val) & 0x1) << 12)
#define SYS2_CLKCLR_SIF4_SET (reg,val) (reg) = ((reg & ~SYS2_CLKCLR_SIF4) | ((val & 0x1) << 12))
/* Clock Disable Request for SIF3 */
#define SYS2_CLKCLR_SIF3 (0x1 << 11)
#define SYS2_CLKCLR_SIF3_VAL(val) (((val) & 0x1) << 11)
#define SYS2_CLKCLR_SIF3_SET (reg,val) (reg) = ((reg & ~SYS2_CLKCLR_SIF3) | ((val & 0x1) << 11))
/* Clock Disable Request for SIF2 */
#define SYS2_CLKCLR_SIF2 (0x1 << 10)
#define SYS2_CLKCLR_SIF2_VAL(val) (((val) & 0x1) << 10)
#define SYS2_CLKCLR_SIF2_SET (reg,val) (reg) = ((reg & ~SYS2_CLKCLR_SIF2) | ((val & 0x1) << 10))
/* Clock Disable Request for SIF1 */
#define SYS2_CLKCLR_SIF1 (0x1 << 9)
#define SYS2_CLKCLR_SIF1_VAL(val) (((val) & 0x1) << 9)
#define SYS2_CLKCLR_SIF1_SET (reg,val) (reg) = ((reg & ~SYS2_CLKCLR_SIF1) | ((val & 0x1) << 9))
/* Clock Disable Request for SIF0 */
#define SYS2_CLKCLR_SIF0 (0x1 << 8)
#define SYS2_CLKCLR_SIF0_VAL(val) (((val) & 0x1) << 8)
#define SYS2_CLKCLR_SIF0_SET (reg,val) (reg) = ((reg & ~SYS2_CLKCLR_SIF0) | ((val & 0x1) << 8))
/* Clock Disable Request for DFEV7 */
#define SYS2_CLKCLR_DFEV7 (0x1 << 7)
#define SYS2_CLKCLR_DFEV7_VAL(val) (((val) & 0x1) << 7)
#define SYS2_CLKCLR_DFEV7_SET (reg,val) (reg) = ((reg & ~SYS2_CLKCLR_DFEV7) | ((val & 0x1) << 7))
/* Clock Disable Request for DFEV6 */
#define SYS2_CLKCLR_DFEV6 (0x1 << 6)
#define SYS2_CLKCLR_DFEV6_VAL(val) (((val) & 0x1) << 6)
#define SYS2_CLKCLR_DFEV6_SET (reg,val) (reg) = ((reg & ~SYS2_CLKCLR_DFEV6) | ((val & 0x1) << 6))
/* Clock Disable Request for DFEV5 */
#define SYS2_CLKCLR_DFEV5 (0x1 << 5)
#define SYS2_CLKCLR_DFEV5_VAL(val) (((val) & 0x1) << 5)
#define SYS2_CLKCLR_DFEV5_SET (reg,val) (reg) = ((reg & ~SYS2_CLKCLR_DFEV5) | ((val & 0x1) << 5))
/* Clock Disable Request for DFEV4 */
#define SYS2_CLKCLR_DFEV4 (0x1 << 4)
#define SYS2_CLKCLR_DFEV4_VAL(val) (((val) & 0x1) << 4)
#define SYS2_CLKCLR_DFEV4_SET (reg,val) (reg) = ((reg & ~SYS2_CLKCLR_DFEV4) | ((val & 0x1) << 4))
/* Clock Disable Request for DFEV3 */
#define SYS2_CLKCLR_DFEV3 (0x1 << 3)
#define SYS2_CLKCLR_DFEV3_VAL(val) (((val) & 0x1) << 3)
#define SYS2_CLKCLR_DFEV3_SET (reg,val) (reg) = ((reg & ~SYS2_CLKCLR_DFEV3) | ((val & 0x1) << 3))
/* Clock Disable Request for DFEV2 */
#define SYS2_CLKCLR_DFEV2 (0x1 << 2)
#define SYS2_CLKCLR_DFEV2_VAL(val) (((val) & 0x1) << 2)
#define SYS2_CLKCLR_DFEV2_SET (reg,val) (reg) = ((reg & ~SYS2_CLKCLR_DFEV2) | ((val & 0x1) << 2))
/* Clock Disable Request for DFEV1 */
#define SYS2_CLKCLR_DFEV1 (0x1 << 1)
#define SYS2_CLKCLR_DFEV1_VAL(val) (((val) & 0x1) << 1)
#define SYS2_CLKCLR_DFEV1_SET (reg,val) (reg) = ((reg & ~SYS2_CLKCLR_DFEV1) | ((val & 0x1) << 1))
/* Clock Disable Request for DFEV0 */
#define SYS2_CLKCLR_DFEV0 (0x1)
#define SYS2_CLKCLR_DFEV0_VAL(val) (((val) & 0x1))
#define SYS2_CLKCLR_DFEV0_SET (reg,val) (reg) = ((reg & ~SYS2_CLKCLR_DFEV0) | ((val & 0x1)))

/*******************************************************************************
 * SYS2 Reset Status Register
 ******************************************************************************/

/* HWACC3 Reset */
#define SYS2_RSR_HWACC3 (0x1 << 11)
#define SYS2_RSR_HWACC3_VAL(val) (((val) & 0x1) << 11)
#define SYS2_RSR_HWACC3_GET(val) (((val) & SYS2_RSR_HWACC3) >> 11)
/* HWACC2 Reset */
#define SYS2_RSR_HWACC2 (0x1 << 10)
#define SYS2_RSR_HWACC2_VAL(val) (((val) & 0x1) << 10)
#define SYS2_RSR_HWACC2_GET(val) (((val) & SYS2_RSR_HWACC2) >> 10)
/* HWACC1 Reset */
#define SYS2_RSR_HWACC1 (0x1 << 9)
#define SYS2_RSR_HWACC1_VAL(val) (((val) & 0x1) << 9)
#define SYS2_RSR_HWACC1_GET(val) (((val) & SYS2_RSR_HWACC1) >> 9)
/* HWACC0 Reset */
#define SYS2_RSR_HWACC0 (0x1 << 8)
#define SYS2_RSR_HWACC0_VAL(val) (((val) & 0x1) << 8)
#define SYS2_RSR_HWACC0_GET(val) (((val) & SYS2_RSR_HWACC0) >> 8)
/* DFEV7 Reset */
#define SYS2_RSR_DFEV7 (0x1 << 7)
#define SYS2_RSR_DFEV7_VAL(val) (((val) & 0x1) << 7)
#define SYS2_RSR_DFEV7_GET(val) (((val) & SYS2_RSR_DFEV7) >> 7)
/* DFEV6 Reset */
#define SYS2_RSR_DFEV6 (0x1 << 6)
#define SYS2_RSR_DFEV6_VAL(val) (((val) & 0x1) << 6)
#define SYS2_RSR_DFEV6_GET(val) (((val) & SYS2_RSR_DFEV6) >> 6)
/* DFEV5 Reset */
#define SYS2_RSR_DFEV5 (0x1 << 5)
#define SYS2_RSR_DFEV5_VAL(val) (((val) & 0x1) << 5)
#define SYS2_RSR_DFEV5_GET(val) (((val) & SYS2_RSR_DFEV5) >> 5)
/* DFEV4 Reset */
#define SYS2_RSR_DFEV4 (0x1 << 4)
#define SYS2_RSR_DFEV4_VAL(val) (((val) & 0x1) << 4)
#define SYS2_RSR_DFEV4_GET(val) (((val) & SYS2_RSR_DFEV4) >> 4)
/* DFEV3 Reset */
#define SYS2_RSR_DFEV3 (0x1 << 3)
#define SYS2_RSR_DFEV3_VAL(val) (((val) & 0x1) << 3)
#define SYS2_RSR_DFEV3_GET(val) (((val) & SYS2_RSR_DFEV3) >> 3)
/* DFEV2 Reset */
#define SYS2_RSR_DFEV2 (0x1 << 2)
#define SYS2_RSR_DFEV2_VAL(val) (((val) & 0x1) << 2)
#define SYS2_RSR_DFEV2_GET(val) (((val) & SYS2_RSR_DFEV2) >> 2)
/* DFEV1 Reset */
#define SYS2_RSR_DFEV1 (0x1 << 1)
#define SYS2_RSR_DFEV1_VAL(val) (((val) & 0x1) << 1)
#define SYS2_RSR_DFEV1_GET(val) (((val) & SYS2_RSR_DFEV1) >> 1)
/* DFEV0 Reset */
#define SYS2_RSR_DFEV0 (0x1)
#define SYS2_RSR_DFEV0_VAL(val) (((val) & 0x1))
#define SYS2_RSR_DFEV0_GET(val) ((val) & SYS2_RSR_DFEV0)

/******************************************************************************
 * SYS2 Reset Request Register
 ******************************************************************************/

/* HWACC3 Reset Request */
#define SYS2_RREQR_HWACC3 (0x1 << 11)
#define SYS2_RREQR_HWACC3_VAL(val) (((val) & 0x1) << 11)
#define SYS2_RREQR_HWACC3_SET (reg,val) (reg) = ((reg & ~SYS2_RREQR_HWACC3) | ((val & 0x1) << 11))
/* HWACC2 Reset Request */
#define SYS2_RREQR_HWACC2 (0x1 << 10)
#define SYS2_RREQR_HWACC2_VAL(val) (((val) & 0x1) << 10)
#define SYS2_RREQR_HWACC2_SET (reg,val) (reg) = ((reg & ~SYS2_RREQR_HWACC2) | ((val & 0x1) << 10))
/* HWACC1 Reset Request */
#define SYS2_RREQR_HWACC1 (0x1 << 9)
#define SYS2_RREQR_HWACC1_VAL(val) (((val) & 0x1) << 9)
#define SYS2_RREQR_HWACC1_SET (reg,val) (reg) = ((reg & ~SYS2_RREQR_HWACC1) | ((val & 0x1) << 9))
/* HWACC0 Reset Request */
#define SYS2_RREQR_HWACC0 (0x1 << 8)
#define SYS2_RREQR_HWACC0_VAL(val) (((val) & 0x1) << 8)
#define SYS2_RREQR_HWACC0_SET (reg,val) (reg) = ((reg & ~SYS2_RREQR_HWACC0) | ((val & 0x1) << 8))
/* DFEV7 Reset Request */
#define SYS2_RREQR_DFEV7 (0x1 << 7)
#define SYS2_RREQR_DFEV7_VAL(val) (((val) & 0x1) << 7)
#define SYS2_RREQR_DFEV7_SET (reg,val) (reg) = ((reg & ~SYS2_RREQR_DFEV7) | ((val & 0x1) << 7))
/* DFEV6 Reset Request */
#define SYS2_RREQR_DFEV6 (0x1 << 6)
#define SYS2_RREQR_DFEV6_VAL(val) (((val) & 0x1) << 6)
#define SYS2_RREQR_DFEV6_SET (reg,val) (reg) = ((reg & ~SYS2_RREQR_DFEV6) | ((val & 0x1) << 6))
/* DFEV5 Reset Request */
#define SYS2_RREQR_DFEV5 (0x1 << 5)
#define SYS2_RREQR_DFEV5_VAL(val) (((val) & 0x1) << 5)
#define SYS2_RREQR_DFEV5_SET (reg,val) (reg) = ((reg & ~SYS2_RREQR_DFEV5) | ((val & 0x1) << 5))
/* DFEV4 Reset Request */
#define SYS2_RREQR_DFEV4 (0x1 << 4)
#define SYS2_RREQR_DFEV4_VAL(val) (((val) & 0x1) << 4)
#define SYS2_RREQR_DFEV4_SET (reg,val) (reg) = ((reg & ~SYS2_RREQR_DFEV4) | ((val & 0x1) << 4))
/* DFEV3 Reset Request */
#define SYS2_RREQR_DFEV3 (0x1 << 3)
#define SYS2_RREQR_DFEV3_VAL(val) (((val) & 0x1) << 3)
#define SYS2_RREQR_DFEV3_SET (reg,val) (reg) = ((reg & ~SYS2_RREQR_DFEV3) | ((val & 0x1) << 3))
/* DFEV2 Reset Request */
#define SYS2_RREQR_DFEV2 (0x1 << 2)
#define SYS2_RREQR_DFEV2_VAL(val) (((val) & 0x1) << 2)
#define SYS2_RREQR_DFEV2_SET (reg,val) (reg) = ((reg & ~SYS2_RREQR_DFEV2) | ((val & 0x1) << 2))
/* DFEV1 Reset Request */
#define SYS2_RREQR_DFEV1 (0x1 << 1)
#define SYS2_RREQR_DFEV1_VAL(val) (((val) & 0x1) << 1)
#define SYS2_RREQR_DFEV1_SET (reg,val) (reg) = ((reg & ~SYS2_RREQR_DFEV1) | ((val & 0x1) << 1))
/* DFEV0 Reset Request */
#define SYS2_RREQR_DFEV0 (0x1)
#define SYS2_RREQR_DFEV0_VAL(val) (((val) & 0x1))
#define SYS2_RREQR_DFEV0_SET (reg,val) (reg) = ((reg & ~SYS2_RREQR_DFEV0) | ((val & 0x1)))

/*******************************************************************************
 * SYS2 Reset Release Register
 ******************************************************************************/

/* HWACC3 Reset Release */
#define SYS2_RRLSR_HWACC3 (0x1 << 11)
#define SYS2_RRLSR_HWACC3_VAL(val) (((val) & 0x1) << 11)
#define SYS2_RRLSR_HWACC3_SET (reg,val) (reg) = ((reg & ~SYS2_RRLSR_HWACC3) | ((val & 0x1) << 11))
/* HWACC2 Reset Release */
#define SYS2_RRLSR_HWACC2 (0x1 << 10)
#define SYS2_RRLSR_HWACC2_VAL(val) (((val) & 0x1) << 10)
#define SYS2_RRLSR_HWACC2_SET (reg,val) (reg) = ((reg & ~SYS2_RRLSR_HWACC2) | ((val & 0x1) << 10))
/* HWACC1 Reset Release */
#define SYS2_RRLSR_HWACC1 (0x1 << 9)
#define SYS2_RRLSR_HWACC1_VAL(val) (((val) & 0x1) << 9)
#define SYS2_RRLSR_HWACC1_SET (reg,val) (reg) = ((reg & ~SYS2_RRLSR_HWACC1) | ((val & 0x1) << 9))
/* HWACC0 Reset Release */
#define SYS2_RRLSR_HWACC0 (0x1 << 8)
#define SYS2_RRLSR_HWACC0_VAL(val) (((val) & 0x1) << 8)
#define SYS2_RRLSR_HWACC0_SET (reg,val) (reg) = ((reg & ~SYS2_RRLSR_HWACC0) | ((val & 0x1) << 8))
/* DFEV7 Reset Release */
#define SYS2_RRLSR_DFEV7 (0x1 << 7)
#define SYS2_RRLSR_DFEV7_VAL(val) (((val) & 0x1) << 7)
#define SYS2_RRLSR_DFEV7_SET (reg,val) (reg) = ((reg & ~SYS2_RRLSR_DFEV7) | ((val & 0x1) << 7))
/* DFEV6 Reset Release */
#define SYS2_RRLSR_DFEV6 (0x1 << 6)
#define SYS2_RRLSR_DFEV6_VAL(val) (((val) & 0x1) << 6)
#define SYS2_RRLSR_DFEV6_SET (reg,val) (reg) = ((reg & ~SYS2_RRLSR_DFEV6) | ((val & 0x1) << 6))
/* DFEV5 Reset Release */
#define SYS2_RRLSR_DFEV5 (0x1 << 5)
#define SYS2_RRLSR_DFEV5_VAL(val) (((val) & 0x1) << 5)
#define SYS2_RRLSR_DFEV5_SET (reg,val) (reg) = ((reg & ~SYS2_RRLSR_DFEV5) | ((val & 0x1) << 5))
/* DFEV4 Reset Release */
#define SYS2_RRLSR_DFEV4 (0x1 << 4)
#define SYS2_RRLSR_DFEV4_VAL(val) (((val) & 0x1) << 4)
#define SYS2_RRLSR_DFEV4_SET (reg,val) (reg) = ((reg & ~SYS2_RRLSR_DFEV4) | ((val & 0x1) << 4))
/* DFEV3 Reset Release */
#define SYS2_RRLSR_DFEV3 (0x1 << 3)
#define SYS2_RRLSR_DFEV3_VAL(val) (((val) & 0x1) << 3)
#define SYS2_RRLSR_DFEV3_SET (reg,val) (reg) = ((reg & ~SYS2_RRLSR_DFEV3) | ((val & 0x1) << 3))
/* DFEV2 Reset Release */
#define SYS2_RRLSR_DFEV2 (0x1 << 2)
#define SYS2_RRLSR_DFEV2_VAL(val) (((val) & 0x1) << 2)
#define SYS2_RRLSR_DFEV2_SET (reg,val) (reg) = ((reg & ~SYS2_RRLSR_DFEV2) | ((val & 0x1) << 2))
/* DFEV1 Reset Release */
#define SYS2_RRLSR_DFEV1 (0x1 << 1)
#define SYS2_RRLSR_DFEV1_VAL(val) (((val) & 0x1) << 1)
#define SYS2_RRLSR_DFEV1_SET (reg,val) (reg) = ((reg & ~SYS2_RRLSR_DFEV1) | ((val & 0x1) << 1))
/* DFEV0 Reset Release */
#define SYS2_RRLSR_DFEV0 (0x1)
#define SYS2_RRLSR_DFEV0_VAL(val) (((val) & 0x1))
#define SYS2_RRLSR_DFEV0_SET (reg,val) (reg) = ((reg & ~SYS2_RRLSR_DFEV0) | ((val & 0x1)))

#endif /* __SYS2_H */

