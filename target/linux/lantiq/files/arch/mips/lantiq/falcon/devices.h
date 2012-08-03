/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Copyright (C) 2011 Thomas Langer <thomas.langer@lantiq.com>
 * Copyright (C) 2011 John Crispin <blogic@openwrt.org>
 */

#ifndef _FALCON_DEVICES_H__
#define _FALCON_DEVICES_H__

#include <linux/spi/spi.h>
#include <linux/spi/flash.h>

#include "../devices.h"

extern void falcon_register_nand(void);
extern void falcon_register_gpio(void);
extern void falcon_register_gpio_extra(void);
extern void falcon_register_spi_flash(struct spi_board_info *data);
extern void falcon_register_i2c(void);

#endif
