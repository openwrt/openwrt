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

#define RTL838X_INT_RW_CTRL		(0x0058)
#define RTL838X_EXT_VERSION		(0x00D0)

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
	int cpu_port;
	int memory_size;
};

#endif /* _MACH_RTL838X_H_ */
