/* SPDX-License-Identifier: GPL-2.0-only */
/* Realtek RTL96XX clock headers */

/* Core registers (e.g OCP PLL) */

#define RTL_SOC_BASE		(0xB8000000)

#define RTL_OCP_PLL_CTRL0		(0x200)
#define RTL_OCP_PLL_CTRL3		(0x20C)

#define RTL_OC0_CMU_GCR			(0x380)

#define RTL_PLL_CTRL0_CPU_FREQ_SEL0(v)		(((v) >> 16) & 0x3f)
#define RTL_PLL_CTRL3_EN_DIV2_CPU0(v)		(((v) >> 18) & 0x1)
#define RTL_CMU_GCR_CMU_MODE(v)				(((v) >> 0) & 0x3)
#define RTL_CMU_GCR_FREQ_DIV(v)				(((v) >> 4) & 0x7)

/* Switch registers (e.g. RGXX_PLL) */

#define RTL_SW_CORE_BASE		(0xBB000000)

#define RTL_PHY_RG5X_PLL		(0x1F054)
