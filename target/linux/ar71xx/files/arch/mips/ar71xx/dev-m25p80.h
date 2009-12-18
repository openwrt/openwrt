/*
 *  Copyright (C) 2009 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#ifndef _AR71XX_DEV_M25P80_H
#define _AR71XX_DEV_M25P80_H

#include <linux/spi/flash.h>

void ar71xx_add_device_m25p80(struct flash_platform_data *pdata) __init;

#endif /* _AR71XX_DEV_M25P80_H */
