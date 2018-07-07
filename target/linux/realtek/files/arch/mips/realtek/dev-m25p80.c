/*
 *  Copyright (C) 2017 Weijie Gao <hackpascal@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/spi/spi.h>
#include <linux/spi/flash.h>

#include "dev-spi.h"
#include "dev-m25p80.h"

static struct spi_board_info realtek_spi_flash_info = {
	.bus_num	= 0,
	.chip_select	= 0,
	.max_speed_hz	= 25000000,
	.modalias	= "m25p80",
};

void __init realtek_register_m25p80(struct flash_platform_data *pdata)
{
	realtek_spi_flash_info.platform_data = pdata;
	realtek_register_spi(&realtek_spi_flash_info, 1);
}
