/*
 *  Atheros AR934x SoC built-in WMAC device support
 *
 *  Copyright (C) 2010-2011 Jaiganesh Narayanan <jnarayanan@atheros.com>
 *
 *  Parts of this file are based on Atheros linux 2.6.31 BSP
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#ifndef _AR71XX_DEV_AR934X_WMAC_H
#define _AR71XX_DEV_AR934X_WMAC_H

void ar934x_add_device_wmac(u8 *cal_data, u8 *mac_addr) __init;

#endif /* _AR71XX_DEV_AR934X_WMAC_H */
