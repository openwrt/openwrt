/*
 * Ralink RT305x specific SOC defines
 *
 * Copyright (C) 2009 Gabor Juhos <juhosg@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#ifndef __RT288X_RALINK_SOC_H
#define __RT288X_RALINK_SOC_H

#define RALINK_SOC_SDRAM_BASE		0
#define RALINK_SOC_MEM_SIZE_MIN		(2 * 1024 * 1024)
#define RALINK_SOC_MEM_SIZE_MAX		(64 * 1024 * 1024)

#define RALINK_SOC_GPIO_BASE	0x10000600

#define RALINK_SOC_GPIO0_COUNT	24
#define RALINK_SOC_GPIO1_COUNT	16
#define RALINK_SOC_GPIO2_COUNT	12

#endif /* __RT288X_RALINK_SOC_H */
