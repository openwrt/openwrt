/*
 *  Copyright (C) 2009-2011 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#ifndef __ASM_MACH_RALINK_COMMON_H
#define __ASM_MACH_RALINK_COMMON_H

#define RAMIPS_SYS_TYPE_LEN	64
extern unsigned char ramips_sys_type[RAMIPS_SYS_TYPE_LEN];
extern unsigned long ramips_mem_base;
extern unsigned long ramips_mem_size_min;
extern unsigned long ramips_mem_size_max;

void ramips_intc_irq_init(unsigned intc_base, unsigned irq, unsigned irq_base);
u32 ramips_intc_get_status(void);

void ramips_soc_prom_init(void);
void ramips_soc_setup(void);
void ramips_early_serial_setup(int line, unsigned base, unsigned freq,
			       unsigned irq);

#endif /* __ASM_MACH_RALINK_COMMON_H */
