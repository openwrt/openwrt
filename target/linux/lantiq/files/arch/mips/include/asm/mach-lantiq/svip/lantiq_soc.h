/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 *  Copyright (C) 2010 John Crispin <blogic@openwrt.org>
 */

#ifndef _LTQ_SVIP_H__
#define _LTQ_SVIP_H__

#ifdef CONFIG_SOC_SVIP

#include <lantiq.h>

/* Chip IDs */
#define SOC_ID_SVIP		0x169

/* SoC Types */
#define SOC_TYPE_SVIP		0x01

/* ASC0/1 - serial port */
#define LTQ_ASC0_BASE_ADDR	0x14100100
#define LTQ_ASC1_BASE_ADDR	0x14100200
#define LTQ_ASC_SIZE		0x100
#define LTQ_EARLY_ASC		KSEG1ADDR(LTQ_ASC0_BASE_ADDR)

#define LTQ_ASC_TIR(x)		(INT_NUM_IM0_IRL0 + (x * 8))
#define LTQ_ASC_RIR(x)		(INT_NUM_IM0_IRL0 + (x * 8) + 2)
#define LTQ_ASC_EIR(x)		(INT_NUM_IM0_IRL0 + (x * 8) + 3)

/* ICU - interrupt control unit */
#define LTQ_ICU_BASE_ADDR	0x14106000
#define LTQ_ICU_BASE_ADDR1	0x14106028
#define LTQ_ICU_BASE_ADDR2	0x1E016000
#define LTQ_ICU_BASE_ADDR3	0x1E016028
#define LTQ_ICU_BASE_ADDR4	0x14106050
#define LTQ_ICU_BASE_ADDR5	0x14106078
#define LTQ_ICU_SIZE		0x100

/* WDT */
#define LTQ_WDT_BASE_ADDR	0x1F8803F0
#define LTQ_WDT_SIZE		0x10

/* Status */
#define LTQ_STATUS_BASE_ADDR	(KSEG1 + 0x1E000500)
#define LTQ_STATUS_CHIPID	((u32 *)(LTQ_STATUS_BASE_ADDR + 0x000C))

#define LTQ_EIU_BASE_ADDR	0

#define ltq_ebu_w32(x, y)       ltq_w32((x), ltq_ebu_membase + (y))
#define ltq_ebu_r32(x)          ltq_r32(ltq_ebu_membase + (x))

extern __iomem void *ltq_ebu_membase;

extern void ltq_gpio_configure(int port, int pin, bool dirin, bool puen,
			       bool altsel0, bool altsel1);
extern int ltq_port_get_dir(unsigned int port, unsigned int pin);
extern int ltq_port_get_puden(unsigned int port, unsigned int pin);
extern int ltq_port_get_altsel0(unsigned int port, unsigned int pin);
extern int ltq_port_get_altsel1(unsigned int port, unsigned int pin);

#define ltq_is_ar9()	0
#define ltq_is_vr9()	0
#define ltq_is_falcon()	0

#define BS_FLASH                0
#define LTQ_RST_CAUSE_WDTRST    0x2

#endif /* CONFIG_SOC_SVIP */
#endif /* _LTQ_SVIP_H__ */
