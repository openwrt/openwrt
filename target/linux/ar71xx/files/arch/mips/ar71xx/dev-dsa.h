/*
 *  Atheros AR71xx DSA switch device support
 *
 *  Copyright (C) 2008-2009 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#ifndef _AR71XX_DEV_DSA_H
#define _AR71XX_DEV_DSA_H

#include <net/dsa.h>

void ar71xx_add_device_dsa(unsigned int id,
			   struct dsa_platform_data *d) __init;

#endif /* _AR71XX_DEV_DSA_H */
