/*
 * Platform data definition for the built-in NAND controller of the
 * Ralink RT305X/RT3662/RT3883 SoCs
 *
 * Copyright (C) 2011-2012 Gabor Juhos <juhosg@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#ifndef _RAMIPS_NAND_PLATFORM_H
#define _RAMIPS_NAND_PLATFORM_H

#define RAMIPS_NAND_DRIVER_NAME		"ramips-nand"

struct ramips_nand_platform_data {
	const char		*name;
	struct mtd_partition	*parts;
	int			nr_parts;
};

#endif /* _RAMIPS_NAND_PLATFORM_H */
