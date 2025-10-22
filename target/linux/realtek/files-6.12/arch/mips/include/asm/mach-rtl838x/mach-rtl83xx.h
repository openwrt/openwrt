/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2006-2012 Tony Wu (tonywu@realtek.com)
 * Copyright (C) 2020 B. Koblitz
 */
#ifndef _MACH_RTL838X_H_
#define _MACH_RTL838X_H_

#include <asm/types.h>
#include <linux/types.h>

/*
 * Register access macros
 */

#define RTL838X_SW_BASE			((volatile void *) 0xBB000000)

#define sw_r32(reg)			readl(RTL838X_SW_BASE + reg)
#define sw_w32(val, reg)		writel(val, RTL838X_SW_BASE + reg)
#define sw_w32_mask(clear, set, reg)	sw_w32((sw_r32(reg) & ~(clear)) | (set), reg)

#define RTL838X_MODEL_NAME_INFO		(0x00D4)
#define RTL838X_CHIP_INFO		(0x00D8)
#define RTL839X_MODEL_NAME_INFO		(0x0FF0)
#define RTL839X_CHIP_INFO		(0x0FF4)
#define RTL93XX_MODEL_NAME_INFO		(0x0004)
#define RTL93XX_CHIP_INFO		(0x0008)

#define RTL838X_LED_GLB_CTRL		(0xA000)
#define RTL839X_LED_GLB_CTRL		(0x00E4)
#define RTL930X_LED_GLB_CTRL		(0xCC00)
#define RTL931X_LED_GLB_CTRL		(0x0600)

#define RTL838X_INT_RW_CTRL		(0x0058)
#define RTL838X_EXT_VERSION		(0x00D0)
#define RTL838X_PLL_CML_CTRL		(0x0FF8)

/*
 * Reset
 */
#define RTL838X_RST_GLB_CTRL_0		(0x003c)
#define RTL838X_RST_GLB_CTRL_1		(0x0040)
#define RTL839X_RST_GLB_CTRL		(0x0014)
#define RTL930X_RST_GLB_CTRL_0		(0x000c)
#define RTL931X_RST_GLB_CTRL		(0x0400)

/* LED control by switch */
#define RTL838X_LED_MODE_SEL		(0x1004)
#define RTL838X_LED_MODE_CTRL		(0xA004)
#define RTL838X_LED_P_EN_CTRL		(0xA008)

/* LED control by software */
#define RTL838X_LED_SW_CTRL		(0xA00C)
#define RTL839X_LED_SW_CTRL		(0xA00C)
#define RTL838X_LED_SW_P_EN_CTRL	(0xA010)
#define RTL839X_LED_SW_P_EN_CTRL	(0x012C)
#define RTL838X_LED0_SW_P_EN_CTRL	(0xA010)
#define RTL839X_LED0_SW_P_EN_CTRL	(0x012C)
#define RTL838X_LED1_SW_P_EN_CTRL	(0xA014)
#define RTL839X_LED1_SW_P_EN_CTRL	(0x0130)
#define RTL838X_LED2_SW_P_EN_CTRL	(0xA018)
#define RTL839X_LED2_SW_P_EN_CTRL	(0x0134)
#define RTL838X_LED_SW_P_CTRL		(0xA01C)
#define RTL838X_LED_SW_P_CTRL_PORT(p)	(RTL838X_LED_SW_P_CTRL + (((p) << 2)))
#define RTL839X_LED_SW_P_CTRL		(0x0144)

#define RTL839X_MAC_EFUSE_CTRL		(0x02ac)

/*
 * MDIO via Realtek's SMI interface
 */
#define RTL838X_SMI_GLB_CTRL		(0xa100)
#define RTL838X_SMI_POLL_CTRL		(0xa17c)

#define RTL839X_SMI_GLB_CTRL		(0x03f8)
#define RTL839X_SMI_PORT_POLLING_CTRL	(0x03fc)

#define RTL930X_SMI_POLL_CTRL		(0xca90)
#define RTL931X_SMI_PORT_POLLING_CTRL	(0x0CCC)

/* Switch interrupts */
#define RTL838X_IMR_GLB			(0x1100)
#define RTL838X_IMR_PORT_LINK_STS_CHG	(0x1104)
#define RTL838X_ISR_GLB_SRC		(0x1148)
#define RTL838X_ISR_PORT_LINK_STS_CHG	(0x114C)

#define RTL839X_IMR_GLB			(0x0064)
#define RTL839X_IMR_PORT_LINK_STS_CHG	(0x0068)
#define RTL839X_ISR_GLB_SRC		(0x009c)
#define RTL839X_ISR_PORT_LINK_STS_CHG	(0x00a0)

#define RTL930X_IMR_GLB			(0xC628)
#define RTL930X_IMR_PORT_LINK_STS_CHG	(0xC62C)
#define RTL930X_ISR_GLB			(0xC658)
#define RTL930X_ISR_PORT_LINK_STS_CHG	(0xC660)

/* IMR_GLB does not exit on RTL931X */
#define RTL931X_IMR_PORT_LINK_STS_CHG	(0x126C)
#define RTL931X_ISR_GLB_SRC		(0x12B4)
#define RTL931X_ISR_PORT_LINK_STS_CHG	(0x12B8)

/* Definition of family IDs */
#define RTL8380_FAMILY_ID		(0x8380)
#define RTL8390_FAMILY_ID		(0x8390)
#define RTL9300_FAMILY_ID		(0x9300)
#define RTL9310_FAMILY_ID		(0x9310)

/* Basic SoC Features */
#define RTL838X_CPU_PORT		28
#define RTL839X_CPU_PORT		52
#define RTL930X_CPU_PORT		28
#define RTL931X_CPU_PORT		56

struct rtl83xx_soc_info {
	unsigned char *name;
	unsigned int id;
	unsigned int family;
	unsigned int revision;
	unsigned int cpu;
	bool testchip;
	unsigned char *compatible;
	int cpu_port;
};

#endif /* _MACH_RTL838X_H_ */
