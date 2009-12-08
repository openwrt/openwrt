/*
 * Platform data definition for the Realtek RTL8366 ethernet switch driver
 *
 * Copyright (C) 2009 Gabor Juhos <juhosg@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#ifndef _RTL8366_SMI_H
#define _RTL8366_SMI_H

struct rtl8366_smi_platform_data {
	unsigned	gpio_sda;
	unsigned	gpio_sck;
};

#endif /*  _RTL8366_SMI_H */
