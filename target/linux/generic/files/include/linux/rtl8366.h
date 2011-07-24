/*
 * Platform data definition for the Realtek RTL8366RB/S ethernet switch driver
 *
 * Copyright (C) 2009-2010 Gabor Juhos <juhosg@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#ifndef _RTL8366_H
#define _RTL8366_H

#define RTL8366_DRIVER_NAME	"rtl8366"
#define RTL8366S_DRIVER_NAME	"rtl8366s"
#define RTL8366RB_DRIVER_NAME	"rtl8366rb"

struct rtl8366_initval {
	unsigned	reg;
	u16		val;
};

struct rtl8366_platform_data {
	unsigned	gpio_sda;
	unsigned	gpio_sck;
	unsigned	num_initvals;
	struct rtl8366_initval *initvals;
};

#endif /*  _RTL8366_H */
