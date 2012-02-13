/*
 * Platform data definition for built-in EHCI controller of the
 * Ralink RT3662/RT3883 SoCs
 *
 * Copyright (C) 2011-2012 Gabor Juhos <juhosg@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#ifndef _RT3883_EHCI_PLATFORM_H
#define _RT3883_EHCI_PLATFORM_H

struct rt3883_ehci_platform_data {
	void	(*start_hw)(void);
	void	(*stop_hw)(void);
};

#endif /*  _RT3883_EHCI_PLATFORM_H */
