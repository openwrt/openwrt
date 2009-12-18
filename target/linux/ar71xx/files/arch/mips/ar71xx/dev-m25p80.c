/*
 *  Copyright (C) 2009 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/spi/spi.h>
#include <linux/spi/flash.h>

#include "devices.h"
#include "dev-m25p80.h"

static struct spi_board_info ar71xx_spi_info[] = {
	{
		.bus_num	= 0,
		.chip_select	= 0,
		.max_speed_hz	= 25000000,
		.modalias	= "m25p80",
	}
};

void __init ar71xx_add_device_m25p80(struct flash_platform_data *pdata)
{
	ar71xx_spi_info[0].platform_data = pdata;
	ar71xx_add_device_spi(NULL, ar71xx_spi_info,
			      ARRAY_SIZE(ar71xx_spi_info));
}
