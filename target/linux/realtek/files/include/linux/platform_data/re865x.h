/*
 *  Realtek RE865X-style built-in ethernet specific platform data definitions
 *
 *  Copyright (C) 2017 Weijie Gao <hackpascal@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#ifndef __LINUX_PLATFORM_RE865X_H
#define __LINUX_PLATFORM_RE865X_H

#include <linux/if_ether.h>

struct re865x_platform_data {
	u8		mac_addr[ETH_ALEN];
};

#endif /* __LINUX_PLATFORM_RE865X_H */
