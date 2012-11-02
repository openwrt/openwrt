/*
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 *
 * Copyright (C) 2010 John Crispin <blogic@openwrt.org>
 */

#ifndef _LTQ_FALCON_H__
#define _LTQ_FALCON_H__

#ifdef CONFIG_SOC_FALCON

#include <lantiq.h>

/* Chip IDs */
#define SOC_ID_FALCON		0x01B8

/* SoC Types */
#define SOC_TYPE_FALCON		0x01

/* ASC0/1 - serial port */
#define LTQ_ASC0_BASE_ADDR	0x1E100C00
#define LTQ_ASC1_BASE_ADDR	0x1E100B00
#define LTQ_ASC_SIZE		0x100

#define LTQ_ASC_TIR(x)          (INT_NUM_IM3_IRL0 + (x * 8))
#define LTQ_ASC_RIR(x)          (INT_NUM_IM3_IRL0 + (x * 8) + 1)
#define LTQ_ASC_EIR(x)          (INT_NUM_IM3_IRL0 + (x * 8) + 2)

/*
 * during early_printk no ioremap possible at this early stage
 * lets use KSEG1 instead
 */
#define LTQ_EARLY_ASC		KSEG1ADDR(LTQ_ASC0_BASE_ADDR)

/* ICU - interrupt control unit */
#define LTQ_ICU_BASE_ADDR	0x1F880200
#define LTQ_ICU_SIZE		0x100

/* WDT */
#define LTQ_WDT_BASE_ADDR	0x1F8803F0
#define LTQ_WDT_SIZE		0x10

#define LTQ_RST_CAUSE_WDTRST	0x0002

/* EBU - external bus unit */
#define LTQ_EBU_BASE_ADDR       0x18000000
#define LTQ_EBU_SIZE            0x0100

#define LTQ_EBU_MODCON  0x000C

/* GPIO */
#define LTQ_GPIO0_BASE_ADDR     0x1D810000
#define LTQ_GPIO0_SIZE          0x0080
#define LTQ_GPIO1_BASE_ADDR     0x1E800100
#define LTQ_GPIO1_SIZE          0x0080
#define LTQ_GPIO2_BASE_ADDR     0x1D810100
#define LTQ_GPIO2_SIZE          0x0080
#define LTQ_GPIO3_BASE_ADDR     0x1E800200
#define LTQ_GPIO3_SIZE          0x0080
#define LTQ_GPIO4_BASE_ADDR     0x1E800300
#define LTQ_GPIO4_SIZE          0x0080
#define LTQ_PADCTRL0_BASE_ADDR  0x1DB01000
#define LTQ_PADCTRL0_SIZE       0x0100
#define LTQ_PADCTRL1_BASE_ADDR  0x1E800400
#define LTQ_PADCTRL1_SIZE       0x0100
#define LTQ_PADCTRL2_BASE_ADDR  0x1DB02000
#define LTQ_PADCTRL2_SIZE       0x0100
#define LTQ_PADCTRL3_BASE_ADDR  0x1E800500
#define LTQ_PADCTRL3_SIZE       0x0100
#define LTQ_PADCTRL4_BASE_ADDR  0x1E800600
#define LTQ_PADCTRL4_SIZE       0x0100

/* I2C */
#define GPON_I2C_BASE		0x1E200000
#define GPON_I2C_SIZE		0x00010000

/* CHIP ID */
#define LTQ_STATUS_BASE_ADDR	0x1E802000

#define LTQ_FALCON_CHIPID	((u32 *)(KSEG1 + LTQ_STATUS_BASE_ADDR + 0x0c))
#define LTQ_FALCON_CHIPTYPE	((u32 *)(KSEG1 + LTQ_STATUS_BASE_ADDR + 0x38))
#define LTQ_FALCON_CHIPCONF	((u32 *)(KSEG1 + LTQ_STATUS_BASE_ADDR + 0x40))

/* SYSCTL - start/stop/restart/configure/... different parts of the Soc */
#define LTQ_SYS1_BASE_ADDR      0x1EF00000
#define LTQ_SYS1_SIZE           0x0100
#define LTQ_STATUS_BASE_ADDR	0x1E802000
#define LTQ_STATUS_SIZE		0x0080
#define LTQ_SYS_ETH_BASE_ADDR	0x1DB00000
#define LTQ_SYS_ETH_SIZE	0x0100
#define LTQ_SYS_GPE_BASE_ADDR	0x1D700000
#define LTQ_SYS_GPE_SIZE	0x0100

#define SYSCTL_SYS1		0
#define SYSCTL_SYSETH		1
#define SYSCTL_SYSGPE		2

/* Activation Status Register */
#define ACTS_ASC1_ACT	0x00000800
#define ACTS_I2C_ACT	0x00004000
#define ACTS_P0		0x00010000
#define ACTS_P1		0x00010000
#define ACTS_P2		0x00020000
#define ACTS_P3		0x00020000
#define ACTS_P4		0x00040000
#define ACTS_PADCTRL0	0x00100000
#define ACTS_PADCTRL1	0x00100000
#define ACTS_PADCTRL2	0x00200000
#define ACTS_PADCTRL3	0x00200000
#define ACTS_PADCTRL4	0x00400000
#define ACTS_I2C_ACT	0x00004000

/* global register ranges */
extern __iomem void *ltq_ebu_membase;
extern __iomem void *ltq_sys1_membase;
#define ltq_ebu_w32(x, y)	ltq_w32((x), ltq_ebu_membase + (y))
#define ltq_ebu_r32(x)		ltq_r32(ltq_ebu_membase + (x))
#define ltq_ebu_w32_mask(clear, set, reg)   \
	ltq_ebu_w32((ltq_ebu_r32(reg) & ~(clear)) | (set), reg)

#define ltq_sys1_w32(x, y)	ltq_w32((x), ltq_sys1_membase + (y))
#define ltq_sys1_r32(x)		ltq_r32(ltq_sys1_membase + (x))
#define ltq_sys1_w32_mask(clear, set, reg)   \
	ltq_sys1_w32((ltq_sys1_r32(reg) & ~(clear)) | (set), reg)

/* gpio wrapper to help configure the pin muxing */
extern int ltq_gpio_mux_set(unsigned int pin, unsigned int mux);

/* to keep the irq code generic we need to define these to 0 as falcon
   has no EIU/EBU */
#define LTQ_EIU_BASE_ADDR	0
#define LTQ_EBU_PCC_ISTAT	0

static inline int ltq_is_ar9(void)
{
	return 0;
}

static inline int ltq_is_vr9(void)
{
	return 0;
}

static inline int ltq_is_falcon(void)
{
	return 1;
}

#endif /* CONFIG_SOC_FALCON */
#endif /* _LTQ_XWAY_H__ */
