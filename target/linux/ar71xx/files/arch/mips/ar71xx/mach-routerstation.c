/*
 *  Ubiquiti RouterStation support
 *
 *  Copyright (C) 2008 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/platform_device.h>
#include <linux/spi/spi.h>
#include <linux/spi/flash.h>

#include <asm/mips_machine.h>
#include <asm/mach-ar71xx/ar71xx.h>
#include <asm/mach-ar71xx/pci.h>
#include <asm/mach-ar71xx/platform.h>

static struct spi_board_info routerstation_spi_info[] = {
	{
		.bus_num	= 0,
		.chip_select	= 0,
		.max_speed_hz	= 25000000,
		.modalias	= "m25p80",
	}
};

static void __init routerstation_setup(void)
{
	ar71xx_add_device_spi(NULL, routerstation_spi_info,
					ARRAY_SIZE(routerstation_spi_info));
}

MIPS_MACHINE(AR71XX_MACH_ROUTERSTATION, "Ubiquiti RouterStation",
					routerstation_setup);
