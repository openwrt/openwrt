/*
 *  Ralink RT305x SoC platform device registration
 *
 *  Copyright (C) 2010 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#ifndef _RT305X_ESW_PLATFORM_H
#define _RT305X_ESW_PLATFORM_H

enum {
	RT305X_ESW_VLAN_CONFIG_NONE = 0,
	RT305X_ESW_VLAN_CONFIG_LLLLW,
	RT305X_ESW_VLAN_CONFIG_WLLLL,
};

struct rt305x_esw_platform_data
{
	u8 vlan_config;
};

#endif /* _RT305X_ESW_PLATFORM_H */
