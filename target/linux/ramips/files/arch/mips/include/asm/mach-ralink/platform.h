/*
 * Ralink RT288x SoC specific platform definitions
 *
 * Copyright (C) 2008 Gabor Juhos <juhosg@openwrt.org>
 * Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#ifndef __ASM_MACH_RT288X_PLATFORM_H
#define __ASM_MACH_RT288X_PLATFORM_H

struct physmap_flash_data;

extern void rt288x_register_flash(unsigned int id,
				  struct physmap_flash_data *pdata) __init;

#endif  /* __ASM_MACH_RT288X_PLATFORM_H */
