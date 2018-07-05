/*
 *  Copyright (C) 2017 Weijie Gao <hackpascal@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#ifndef _REALTEK_DEV_M25P80_H
#define _REALTEK_DEV_M25P80_H

#include <linux/spi/flash.h>

void realtek_register_m25p80(struct flash_platform_data *pdata);

#endif /* _REALTEK_DEV_M25P80_H */
