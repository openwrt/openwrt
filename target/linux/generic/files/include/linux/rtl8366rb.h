/*
 * Platform data definition for the Realtek RTL8366S ethernet switch driver
 *
 * Copyright (C) 2009-2010 Gabor Juhos <juhosg@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#ifndef _RTL8366RB_H
#define _RTL8366RB_H

#define RTL8366RB_DRIVER_NAME	"rtl8366rb"

struct rtl8366rb_platform_data {
	unsigned	gpio_sda;
	unsigned	gpio_sck;
};

#endif /*  _RTL8366RB_SMI_H */
