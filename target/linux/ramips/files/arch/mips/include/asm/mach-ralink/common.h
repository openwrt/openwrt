/*
 *  Copyright (C) 2009 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#ifndef __ASM_MACH_RALINK_COMMON_H
#define __ASM_MACH_RALINK_COMMON_H

void __init ramips_intc_irq_init(unsigned intc_base, unsigned irq,
				 unsigned irq_base);
u32 ramips_intc_get_status(void);

#endif /* __ASM_MACH_RALINK_COMMON_H */
