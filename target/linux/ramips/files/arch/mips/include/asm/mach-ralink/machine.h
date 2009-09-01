/*
 * Ralink machine types
 *
 * Copyright (C) 2009 Gabor Juhos <juhosg@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

enum ramips_mach_type {
	RAMIPS_MACH_GENERIC,
	RAMIPS_MACH_RT_N15,		/* Asus RT-N15 */
	RAMIPS_MACH_WHR_G300N,		/* Buffalo WHR-G300N */
};

extern enum ramips_mach_type ramips_mach;
