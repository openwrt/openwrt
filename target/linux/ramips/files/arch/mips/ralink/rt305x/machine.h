/*
 * Ralink RT305x SoC specific setup
 *
 * Copyright (C) 2009 Gabor Juhos <juhosg@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

enum rt305x_mach_type {
	RT305X_MACH_GENERIC,
	RT305X_MACH_WHR_G300N,		/* Buffalo WHR-G300N */
};

extern enum rt305x_mach_type rt305x_mach;
