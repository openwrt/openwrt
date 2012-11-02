/******************************************************************************

  Copyright (c) 2007
  Infineon Technologies AG
  St. Martin Strasse 53; 81669 Munich, Germany

  Any use of this Software is subject to the conclusion of a respective
  License Agreement. Without such a License Agreement no rights to the
  Software are granted.

 ******************************************************************************/

#ifndef __SYS1_REG_H
#define __SYS1_REG_H

#define sys1_r32(reg) ltq_r32(&sys1->reg)
#define sys1_w32(val, reg) ltq_w32(val, &sys1->reg)
#define sys1_w32_mask(clear, set, reg) ltq_w32_mask(clear, set, &sys1->reg)

/** SYS1 register structure */
struct svip_reg_sys1 {
	unsigned long clksr; /* 0x0000 */
	unsigned long clkenr; /* 0x0004 */
	unsigned long clkclr; /* 0x0008 */
	unsigned long reserved0[1];
	unsigned long l2ccr; /* 0x0010 */
	unsigned long fpicr; /* 0x0014 */
	unsigned long wdtcr; /* 0x0018 */
	unsigned long reserved1[1];
	unsigned long cpucr[6]; /* 0x0020 */
	unsigned long reserved2[2];
	unsigned long rsr; /* 0x0040 */
	unsigned long rreqr; /* 0x0044 */
	unsigned long rrlsr; /* 0x0048 */
	unsigned long rbtr; /* 0x004c */
	unsigned long irncr; /* 0x0050 */
	unsigned long irnicr; /* 0x0054 */
	unsigned long irnen; /* 0x0058 */
	unsigned long reserved3[1];
	unsigned long cpursr[6]; /* 0x0060 */
	unsigned long reserved4[2];
	unsigned long cpusrssr[6]; /* 0x0080 */
	unsigned long reserved5[2];
	unsigned long cpuwrssr[6]; /* 0x00a0 */
};

/*******************************************************************************
 * SYS1 Clock Status Register
 ******************************************************************************/
/* (r) Clock Enable for L2C */
#define SYS1_CLKSR_L2C (0x1 << 31)
/* (r) Clock Enable for DDR2 */
#define SYS1_CLKSR_DDR2 (0x1 << 30)
/* (r) Clock Enable for SMI2 */
#define SYS1_CLKSR_SMI2 (0x1 << 29)
/* (r) Clock Enable for SMI1 */
#define SYS1_CLKSR_SMI1 (0x1 << 28)
/* (r) Clock Enable for SMI0 */
#define SYS1_CLKSR_SMI0 (0x1 << 27)
/* (r) Clock Enable for FMI0 */
#define SYS1_CLKSR_FMI0 (0x1 << 26)
/* (r) Clock Enable for PORT0 */
#define SYS1_CLKSR_PORT0 (0x1 << 0)
/* (r) Clock Enable for PCM3 */
#define SYS1_CLKSR_PCM3 (0x1 << 19)
/* (r) Clock Enable for PCM2 */
#define SYS1_CLKSR_PCM2 (0x1 << 18)
/* (r) Clock Enable for PCM1 */
#define SYS1_CLKSR_PCM1 (0x1 << 17)
/* (r) Clock Enable for PCM0 */
#define SYS1_CLKSR_PCM0 (0x1 << 16)
/* (r) Clock Enable for ASC1 */
#define SYS1_CLKSR_ASC1 (0x1 << 15)
/* (r) Clock Enable for ASC0 */
#define SYS1_CLKSR_ASC0 (0x1 << 14)
/* (r) Clock Enable for SSC2 */
#define SYS1_CLKSR_SSC2 (0x1 << 13)
/* (r) Clock Enable for SSC1 */
#define SYS1_CLKSR_SSC1 (0x1 << 12)
/* (r) Clock Enable for SSC0 */
#define SYS1_CLKSR_SSC0 (0x1 << 11)
/* (r) Clock Enable for GPTC */
#define SYS1_CLKSR_GPTC (0x1 << 10)
/* (r) Clock Enable for DMA */
#define SYS1_CLKSR_DMA (0x1 << 9)
/* (r) Clock Enable for FSCT */
#define SYS1_CLKSR_FSCT (0x1 << 8)
/* (r) Clock Enable for ETHSW */
#define SYS1_CLKSR_ETHSW (0x1 << 7)
/* (r) Clock Enable for EBU */
#define SYS1_CLKSR_EBU (0x1 << 6)
/* (r) Clock Enable for TRNG */
#define SYS1_CLKSR_TRNG (0x1 << 5)
/* (r) Clock Enable for DEU */
#define SYS1_CLKSR_DEU (0x1 << 4)
/* (r) Clock Enable for PORT3 */
#define SYS1_CLKSR_PORT3 (0x1 << 3)
/* (r) Clock Enable for PORT2 */
#define SYS1_CLKSR_PORT2 (0x1 << 2)
/* (r) Clock Enable for PORT1 */
#define SYS1_CLKSR_PORT1 (0x1 << 1)

/*******************************************************************************
 * SYS1 Clock Enable Register
 ******************************************************************************/
/* (w) Clock Enable Request for L2C */
#define SYS1_CLKENR_L2C (0x1 << 31)
/* (w) Clock Enable Request for DDR2 */
#define SYS1_CLKENR_DDR2 (0x1 << 30)
/* (w) Clock Enable Request for SMI2 */
#define SYS1_CLKENR_SMI2 (0x1 << 29)
/* (w) Clock Enable Request for SMI1 */
#define SYS1_CLKENR_SMI1 (0x1 << 28)
/* (w) Clock Enable Request for SMI0 */
#define SYS1_CLKENR_SMI0 (0x1 << 27)
/* (w) Clock Enable Request for FMI0 */
#define SYS1_CLKENR_FMI0 (0x1 << 26)
/* (w) Clock Enable Request for PORT0 */
#define SYS1_CLKENR_PORT0 (0x1 << 0)
/* (w) Clock Enable Request for PCM3 */
#define SYS1_CLKENR_PCM3 (0x1 << 19)
/* (w) Clock Enable Request for PCM2 */
#define SYS1_CLKENR_PCM2 (0x1 << 18)
/* (w) Clock Enable Request for PCM1 */
#define SYS1_CLKENR_PCM1 (0x1 << 17)
/* (w) Clock Enable Request for PCM0 */
#define SYS1_CLKENR_PCM0 (0x1 << 16)
/* (w) Clock Enable Request for ASC1 */
#define SYS1_CLKENR_ASC1 (0x1 << 15)
/* (w) Clock Enable Request for ASC0 */
#define SYS1_CLKENR_ASC0 (0x1 << 14)
/* (w) Clock Enable Request for SSC2 */
#define SYS1_CLKENR_SSC2 (0x1 << 13)
/* (w) Clock Enable Request for SSC1 */
#define SYS1_CLKENR_SSC1 (0x1 << 12)
/* (w) Clock Enable Request for SSC0 */
#define SYS1_CLKENR_SSC0 (0x1 << 11)
/* (w) Clock Enable Request for GPTC */
#define SYS1_CLKENR_GPTC (0x1 << 10)
/* (w) Clock Enable Request for DMA */
#define SYS1_CLKENR_DMA (0x1 << 9)
/* (w) Clock Enable Request for FSCT */
#define SYS1_CLKENR_FSCT (0x1 << 8)
/* (w) Clock Enable Request for ETHSW */
#define SYS1_CLKENR_ETHSW (0x1 << 7)
/* (w) Clock Enable Request for EBU */
#define SYS1_CLKENR_EBU (0x1 << 6)
/* (w) Clock Enable Request for TRNG */
#define SYS1_CLKENR_TRNG (0x1 << 5)
/* (w) Clock Enable Request for DEU */
#define SYS1_CLKENR_DEU (0x1 << 4)
/* (w) Clock Enable Request for PORT3 */
#define SYS1_CLKENR_PORT3 (0x1 << 3)
/* (w) Clock Enable Request for PORT2 */
#define SYS1_CLKENR_PORT2 (0x1 << 2)
/* (w) Clock Enable Request for PORT1 */
#define SYS1_CLKENR_PORT1 (0x1 << 1)

/*******************************************************************************
 * SYS1 Clock Clear Register
 ******************************************************************************/
/* (w) Clock Disable Request for L2C */
#define SYS1_CLKCLR_L2C (0x1 << 31)
/* (w) Clock Disable Request for DDR2 */
#define SYS1_CLKCLR_DDR2 (0x1 << 30)
/* (w) Clock Disable Request for SMI2 */
#define SYS1_CLKCLR_SMI2 (0x1 << 29)
/* (w) Clock Disable Request for SMI1 */
#define SYS1_CLKCLR_SMI1 (0x1 << 28)
/* (w) Clock Disable Request for SMI0 */
#define SYS1_CLKCLR_SMI0 (0x1 << 27)
/* (w) Clock Disable Request for FMI0 */
#define SYS1_CLKCLR_FMI0 (0x1 << 26)
/* (w) Clock Disable Request for PORT0 */
#define SYS1_CLKCLR_PORT0 (0x1 << 0)
/* (w) Clock Disable Request for PCM3 */
#define SYS1_CLKCLR_PCM3 (0x1 << 19)
/* (w) Clock Disable Request for PCM2 */
#define SYS1_CLKCLR_PCM2 (0x1 << 18)
/* (w) Clock Disable Request for PCM1 */
#define SYS1_CLKCLR_PCM1 (0x1 << 17)
/* (w) Clock Disable Request for PCM0 */
#define SYS1_CLKCLR_PCM0 (0x1 << 16)
/* (w) Clock Disable Request for ASC1 */
#define SYS1_CLKCLR_ASC1 (0x1 << 15)
/* (w) Clock Disable Request for ASC0 */
#define SYS1_CLKCLR_ASC0 (0x1 << 14)
/* (w) Clock Disable Request for SSC2 */
#define SYS1_CLKCLR_SSC2 (0x1 << 13)
/* (w) Clock Disable Request for SSC1 */
#define SYS1_CLKCLR_SSC1 (0x1 << 12)
/* (w) Clock Disable Request for SSC0 */
#define SYS1_CLKCLR_SSC0 (0x1 << 11)
/* (w) Clock Disable Request for GPTC */
#define SYS1_CLKCLR_GPTC (0x1 << 10)
/* (w) Clock Disable Request for DMA */
#define SYS1_CLKCLR_DMA (0x1 << 9)
/* (w) Clock Disable Request for FSCT */
#define SYS1_CLKCLR_FSCT (0x1 << 8)
/* (w) Clock Disable Request for ETHSW */
#define SYS1_CLKCLR_ETHSW (0x1 << 7)
/* (w) Clock Disable Request for EBU */
#define SYS1_CLKCLR_EBU (0x1 << 6)
/* (w) Clock Disable Request for TRNG */
#define SYS1_CLKCLR_TRNG (0x1 << 5)
/* (w) Clock Disable Request for DEU */
#define SYS1_CLKCLR_DEU (0x1 << 4)
/* (w) Clock Disable Request for PORT3 */
#define SYS1_CLKCLR_PORT3 (0x1 << 3)
/* (w) Clock Disable Request for PORT2 */
#define SYS1_CLKCLR_PORT2 (0x1 << 2)
/* (w) Clock Disable Request for PORT1 */
#define SYS1_CLKCLR_PORT1 (0x1 << 1)

/*******************************************************************************
 * SYS1 FPI Control Register
 ******************************************************************************/

/* FPI Bus Clock divider (0) */
#define SYS1_FPICR_FPIDIV   (0x1)
#define SYS1_FPICR_FPIDIV_VAL(val)   (((val) & 0x1) << 0)
#define SYS1_FPICR_FPIDIV_GET(val)   ((((val) & SYS1_FPICR_FPIDIV) >> 0) & 0x1)
#define SYS1_FPICR_FPIDIV_SET(reg,val) (reg) = ((reg & ~SYS1_FPICR_FPIDIV) | (((val) & 0x1) << 0))

/*******************************************************************************
 * SYS1 Clock Control Register for CPUn
 ******************************************************************************/

/* Enable bit for clock of CPUn (1) */
#define SYS1_CPUCR_CPUCLKEN    (0x1 << 1)
#define SYS1_CPUCR_CPUCLKEN_VAL(val)   (((val) & 0x1) << 1)
#define SYS1_CPUCR_CPUCLKEN_GET(val)   ((((val) & SYS1_CPUCR_CPUCLKEN) >> 1) & 0x1)
#define SYS1_CPUCR_CPUCLKEN_SET(reg,val) (reg) = ((reg & ~SYS1_CPUCR_CPUCLKEN) | (((val) & 0x1) << 1))
/* Divider factor for clock of CPUn (0) */
#define SYS1_CPUCR_CPUDIV    (0x1)
#define SYS1_CPUCR_CPUDIV_VAL(val)   (((val) & 0x1) << 0)
#define SYS1_CPUCR_CPUDIV_GET(val)   ((((val) & SYS1_CPUCR_CPUDIV) >> 0) & 0x1)
#define SYS1_CPUCR_CPUDIV_SET(reg,val) (reg) = ((reg & ~SYS1_CPUCR_CPUDIV) | (((val) & 0x1) << 0))

/*******************************************************************************
 * SYS1 Reset Request Register
 ******************************************************************************/

/* HRSTOUT Reset Request (18) */
#define SYS1_RREQ_HRSTOUT   (0x1 << 18)
#define SYS1_RREQ_HRSTOUT_VAL(val)   (((val) & 0x1) << 18)
#define SYS1_RREQ_HRSTOUT_SET(reg,val) (reg) = (((reg & ~SYS1_RREQ_HRSTOUT) | (((val) & 1) << 18))
						    /* FBS0 Reset Request (17) */
#define SYS1_RREQ_FBS0   (0x1 << 17)
#define SYS1_RREQ_FBS0_VAL(val)   (((val) & 0x1) << 17)
#define SYS1_RREQ_FBS0_SET(reg,val) (reg) = (((reg & ~SYS1_RREQ_FBS0) | (((val) & 1) << 17))
						 /* SUBSYS Reset Request (16) */
#define SYS1_RREQ_SUBSYS   (0x1 << 16)
#define SYS1_RREQ_SUBSYS_VAL(val)   (((val) & 0x1) << 16)
#define SYS1_RREQ_SUBSYS_SET(reg,val) (reg) = (((reg & ~SYS1_RREQ_SUBSYS) | (((val) & 1) << 16))
						   /* Watchdog5 Reset Request (13) */
#define SYS1_RREQ_WDT5   (0x1 << 13)
#define SYS1_RREQ_WDT5_VAL(val)   (((val) & 0x1) << 13)
#define SYS1_RREQ_WDT5_SET(reg,val) (reg) = (((reg & ~SYS1_RREQ_WDT5) | (((val) & 1) << 13))
						 /* Watchdog4 Reset Request (12) */
#define SYS1_RREQ_WDT4   (0x1 << 12)
#define SYS1_RREQ_WDT4_VAL(val)   (((val) & 0x1) << 12)
#define SYS1_RREQ_WDT4_SET(reg,val) (reg) = (((reg & ~SYS1_RREQ_WDT4) | (((val) & 1) << 12))
						 /* Watchdog3 Reset Request (11) */
#define SYS1_RREQ_WDT3   (0x1 << 11)
#define SYS1_RREQ_WDT3_VAL(val)   (((val) & 0x1) << 11)
#define SYS1_RREQ_WDT3_SET(reg,val) (reg) = (((reg & ~SYS1_RREQ_WDT3) | (((val) & 1) << 11))
						 /* Watchdog2 Reset Request (10) */
#define SYS1_RREQ_WDT2   (0x1 << 10)
#define SYS1_RREQ_WDT2_VAL(val)   (((val) & 0x1) << 10)
#define SYS1_RREQ_WDT2_SET(reg,val) (reg) = (((reg & ~SYS1_RREQ_WDT2) | (((val) & 1) << 10))
						 /* Watchdog1 Reset Request (9) */
#define SYS1_RREQ_WDT1   (0x1 << 9)
#define SYS1_RREQ_WDT1_VAL(val)   (((val) & 0x1) << 9)
#define SYS1_RREQ_WDT1_SET(reg,val) (reg) = (((reg & ~SYS1_RREQ_WDT1) | (((val) & 1) << 9))
						 /* Watchdog0 Reset Request (8) */
#define SYS1_RREQ_WDT0   (0x1 << 8)
#define SYS1_RREQ_WDT0_VAL(val)   (((val) & 0x1) << 8)
#define SYS1_RREQ_WDT0_SET(reg,val) (reg) = (((reg & ~SYS1_RREQ_WDT0) | (((val) & 1) << 8))
						 /* CPU5 Reset Request (5) */
#define SYS1_RREQ_CPU5   (0x1 << 5)
#define SYS1_RREQ_CPU5_VAL(val)   (((val) & 0x1) << 5)
#define SYS1_RREQ_CPU5_SET(reg,val) (reg) = ((reg & ~SYS1_RREQ_CPU5) | (((val) & 1) << 5))
						 /* CPU4 Reset Request (4) */
#define SYS1_RREQ_CPU4   (0x1 << 4)
#define SYS1_RREQ_CPU4_VAL(val)   (((val) & 0x1) << 4)
#define SYS1_RREQ_CPU4_SET(reg,val) (reg) = ((reg & ~SYS1_RREQ_CPU4) | (((val) & 1) << 4))
						 /* CPU3 Reset Request (3) */
#define SYS1_RREQ_CPU3   (0x1 << 3)
#define SYS1_RREQ_CPU3_VAL(val)   (((val) & 0x1) << 3)
#define SYS1_RREQ_CPU3_SET(reg,val) (reg) = ((reg & ~SYS1_RREQ_CPU3) | (((val) & 1) << 3))
						 /* CPU2 Reset Request (2) */
#define SYS1_RREQ_CPU2   (0x1 << 2)
#define SYS1_RREQ_CPU2_VAL(val)   (((val) & 0x1) << 2)
#define SYS1_RREQ_CPU2_SET(reg,val) (reg) = ((reg & ~SYS1_RREQ_CPU2) | (((val) & 1) << 2))
						 /* CPU1 Reset Request (1) */
#define SYS1_RREQ_CPU1   (0x1 << 1)
#define SYS1_RREQ_CPU1_VAL(val)   (((val) & 0x1) << 1)
#define SYS1_RREQ_CPU1_SET(reg,val) (reg) = ((reg & ~SYS1_RREQ_CPU1) | (((val) & 1) << 1))
/* CPU0 Reset Request (0) */
#define SYS1_RREQ_CPU0   (0x1)
#define SYS1_RREQ_CPU0_VAL(val)   (((val) & 0x1) << 0)
#define SYS1_RREQ_CPU0_SET(reg,val) (reg) = ((reg & ~SYS1_RREQ_CPU0) | (((val) & 1) << 0))

/*******************************************************************************
 * SYS1 Reset Release Register
 ******************************************************************************/

/* HRSTOUT Reset Release (18) */
#define SYS1_RRLSR_HRSTOUT   (0x1 << 18)
#define SYS1_RRLSR_HRSTOUT_VAL(val)   (((val) & 0x1) << 18)
#define SYS1_RRLSR_HRSTOUT_SET(reg,val) (reg) = ((reg & ~SYS1_RRLSR_HRSTOUT) | (((val) & 1) << 18))
/* FBS0 Reset Release (17) */
#define SYS1_RRLSR_FBS0   (0x1 << 17)
#define SYS1_RRLSR_FBS0_VAL(val)   (((val) & 0x1) << 17)
#define SYS1_RRLSR_FBS0_SET(reg,val) (reg) = ((reg & ~SYS1_RRLSR_FBS0) | (((val) & 1) << 17))
/* SUBSYS Reset Release (16) */
#define SYS1_RRLSR_SUBSYS   (0x1 << 16)
#define SYS1_RRLSR_SUBSYS_VAL(val)   (((val) & 0x1) << 16)
#define SYS1_RRLSR_SUBSYS_SET(reg,val) (reg) = ((reg & ~SYS1_RRLSR_SUBSYS) | (((val) & 1) << 16))
/* Watchdog5 Reset Release (13) */
#define SYS1_RRLSR_WDT5   (0x1 << 13)
#define SYS1_RRLSR_WDT5_VAL(val)   (((val) & 0x1) << 13)
#define SYS1_RRLSR_WDT5_SET(reg,val) (reg) = ((reg & ~SYS1_RRLSR_WDT5) | (((val) & 1) << 13))
/* Watchdog4 Reset Release (12) */
#define SYS1_RRLSR_WDT4   (0x1 << 12)
#define SYS1_RRLSR_WDT4_VAL(val)   (((val) & 0x1) << 12)
#define SYS1_RRLSR_WDT4_SET(reg,val) (reg) = ((reg & ~SYS1_RRLSR_WDT4) | (((val) & 1) << 12))
/* Watchdog3 Reset Release (11) */
#define SYS1_RRLSR_WDT3   (0x1 << 11)
#define SYS1_RRLSR_WDT3_VAL(val)   (((val) & 0x1) << 11)
#define SYS1_RRLSR_WDT3_SET(reg,val) (reg) = ((reg & ~SYS1_RRLSR_WDT3) | (((val) & 1) << 11))
/* Watchdog2 Reset Release (10) */
#define SYS1_RRLSR_WDT2   (0x1 << 10)
#define SYS1_RRLSR_WDT2_VAL(val)   (((val) & 0x1) << 10)
#define SYS1_RRLSR_WDT2_SET(reg,val) (reg) = ((reg & ~SYS1_RRLSR_WDT2) | (((val) & 1) << 10))
/* Watchdog1 Reset Release (9) */
#define SYS1_RRLSR_WDT1   (0x1 << 9)
#define SYS1_RRLSR_WDT1_VAL(val)   (((val) & 0x1) << 9)
#define SYS1_RRLSR_WDT1_SET(reg,val) (reg) = ((reg & ~SYS1_RRLSR_WDT1) | (((val) & 1) << 9))
/* Watchdog0 Reset Release (8) */
#define SYS1_RRLSR_WDT0   (0x1 << 8)
#define SYS1_RRLSR_WDT0_VAL(val)   (((val) & 0x1) << 8)
#define SYS1_RRLSR_WDT0_SET(reg,val) (reg) = ((reg & ~SYS1_RRLSR_WDT0) | (((val) & 1) << 8))
/* CPU5 Reset Release (5) */
#define SYS1_RRLSR_CPU5   (0x1 << 5)
#define SYS1_RRLSR_CPU5_VAL(val)   (((val) & 0x1) << 5)
#define SYS1_RRLSR_CPU5_SET(reg,val) (reg) = ((reg & ~SYS1_RRLSR_CPU5) | (((val) & 1) << 5))
/* CPU4 Reset Release (4) */
#define SYS1_RRLSR_CPU4   (0x1 << 4)
#define SYS1_RRLSR_CPU4_VAL(val)   (((val) & 0x1) << 4)
#define SYS1_RRLSR_CPU4_SET(reg,val) (reg) = ((reg & ~SYS1_RRLSR_CPU4) | (((val) & 1) << 4))
/* CPU3 Reset Release (3) */
#define SYS1_RRLSR_CPU3   (0x1 << 3)
#define SYS1_RRLSR_CPU3_VAL(val)   (((val) & 0x1) << 3)
#define SYS1_RRLSR_CPU3_SET(reg,val) (reg) = ((reg & ~SYS1_RRLSR_CPU3) | (((val) & 1) << 3))
/* CPU2 Reset Release (2) */
#define SYS1_RRLSR_CPU2   (0x1 << 2)
#define SYS1_RRLSR_CPU2_VAL(val)   (((val) & 0x1) << 2)
#define SYS1_RRLSR_CPU2_SET(reg,val) (reg) = ((reg & ~SYS1_RRLSR_CPU2) | (((val) & 1) << 2))
/* CPU1 Reset Release (1) */
#define SYS1_RRLSR_CPU1   (0x1 << 1)
#define SYS1_RRLSR_CPU1_VAL(val)   (((val) & 0x1) << 1)
#define SYS1_RRLSR_CPU1_SET(reg,val) (reg) = ((reg & ~SYS1_RRLSR_CPU1) | (((val) & 1) << 1))
/* CPU0 Reset Release (0) */
#define SYS1_RRLSR_CPU0   (0x1)
#define SYS1_RRLSR_CPU0_VAL(val)   (((val) & 0x1) << 0)
#define SYS1_RRLSR_CPU0_SET(reg,val) (reg) = ((reg & ~SYS1_RRLSR_CPU0) | (((val) & 1) << 0))

#endif
