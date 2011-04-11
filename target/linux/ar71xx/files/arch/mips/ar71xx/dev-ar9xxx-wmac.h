/*
 *  Atheros AR9XXX SoCs built-in WMAC device support
 *
 *  Copyright (C) 2010-2011 Jaiganesh Narayanan <jnarayanan@atheros.com>
 *  Copyright (C) 2008-2009 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  Parts of this file are based on Atheros 2.6.15/2.6.31 BSP
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#ifndef _AR71XX_DEV_AR9XXX_WMAC_H
#define _AR71XX_DEV_AR9XXX_WMAC_H

void ar9xxx_add_device_wmac(u8 *cal_data, u8 *mac_addr) __init;

#endif /* _AR71XX_DEV_AR9XXX_WMAC_H */
