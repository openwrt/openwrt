/*
 *  AR913x parallel flash driver platform data definitions
 *
 *  Copyright (C) 2008-2012 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#ifndef __AR913X_FLASH_H
#define __AR913X_FLASH_H

struct mtd_partition;

struct ar913x_flash_platform_data {
	unsigned int		width;
	u8			is_shared:1;
	unsigned int		nr_parts;
	struct mtd_partition	*parts;
};

#endif /* __AR913X_FLASH_H */
