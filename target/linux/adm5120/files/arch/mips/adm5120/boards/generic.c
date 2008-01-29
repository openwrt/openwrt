/*
 *  $Id$
 *
 *  Generic ADM5120 based board
 *
 *  Copyright (C) 2007 OpenWrt.org
 *  Copyright (C) 2007 Gabor Juhos <juhosg at openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 */

#include <linux/kernel.h>
#include <linux/init.h>

#include <asm/bootinfo.h>
#include <asm/gpio.h>

#include <adm5120_board.h>
#include <adm5120_platform.h>

static struct platform_device *generic_devices[] __initdata = {
	&adm5120_flash0_device,
	&adm5120_hcd_device,
};

/*--------------------------------------------------------------------------*/

ADM5120_BOARD_START(GENERIC, "Generic ADM5120 board")
	.eth_num_ports	= 6,
	.num_devices	= ARRAY_SIZE(generic_devices),
	.devices	= generic_devices,
ADM5120_BOARD_END
