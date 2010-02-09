/*
 * ath9k platform data defines
 *
 * Copyright (C) 2008 Gabor Juhos <juhosg@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#ifndef _LINUX_ATH9K_PLATFORM_H
#define _LINUX_ATH9K_PLATFORM_H

#define ATH9K_PLAT_EEP_MAX_WORDS	2048

struct ath9k_platform_data {
	u16 eeprom_data[ATH9K_PLAT_EEP_MAX_WORDS];
	u8 *macaddr;

	unsigned long quirk_wndr3700:1;
};

#endif /* _LINUX_ATH9K_PLATFORM_H */
