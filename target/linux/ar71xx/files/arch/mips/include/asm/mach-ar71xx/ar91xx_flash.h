/*
 *  AR91xx parallel flash driver platform data definitions
 *
 *  Copyright (C) 2008-2009 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#ifndef __AR91XX_FLASH_H
#define __AR91XX_FLASH_H

struct mtd_partition;

struct ar91xx_flash_platform_data {
	unsigned int		width;
	u8			is_shared:1;
#ifdef CONFIG_MTD_PARTITIONS
	unsigned int		nr_parts;
	struct mtd_partition	*parts;
#endif
};

#endif /* __AR91XX_FLASH_H */
