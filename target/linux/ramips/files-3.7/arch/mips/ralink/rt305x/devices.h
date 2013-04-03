/*
 * Ralink RT305x SoC specific platform device definitions
 *
 * Copyright (C) 2009-2011 Gabor Juhos <juhosg@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#ifndef __RT305X_DEVICES_H
#define __RT305X_DEVICES_H

#include <asm/mach-ralink/rt305x_esw_platform.h>

struct physmap_flash_data;
struct spi_board_info;

extern struct physmap_flash_data rt305x_flash0_data;
extern struct physmap_flash_data rt305x_flash1_data;

extern struct rt305x_esw_platform_data rt305x_esw_data;

void rt305x_register_flash(unsigned int id);
void rt305x_register_ethernet(void);
void rt305x_register_wifi(void);
void rt305x_register_wdt(void);
void rt305x_register_spi(struct spi_board_info *info, int n);
void rt305x_register_usb(void);

#endif  /* __RT305X_DEVICES_H */

