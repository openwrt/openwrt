/*
 *  Asus RT-G32 rev B board support
 *
 *  Copyright (C) 2011 Sergiy <piratfm@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/spi/spi.h>
#include <linux/spi/flash.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>

#include <asm/mach-ralink/machine.h>
#include <asm/mach-ralink/dev-gpio-buttons.h>
#include <asm/mach-ralink/rt305x.h>
#include <asm/mach-ralink/rt305x_regs.h>

#include "devices.h"

#define RT_G32B_GPIO_BUTTON_WPS		0	/* active low */
#define RT_G32B_GPIO_BUTTON_RESET	10	/* active low */

#define RT_G32B_BUTTONS_POLL_INTERVAL	20

#ifdef CONFIG_MTD_PARTITIONS
static struct mtd_partition rt_g32b_partitions[] = {
	{
		.name	= "u-boot",
		.offset	= 0,
		.size	= 0x030000,
		.mask_flags = MTD_WRITEABLE,
	}, {
		.name	= "devdata",
		.offset	= 0x030000,
		.size	= 0x010000,
		.mask_flags = MTD_WRITEABLE,
	}, {
		.name	= "devconf",
		.offset	= 0x040000,
		.size	= 0x010000,
		.mask_flags = MTD_WRITEABLE,
	}, {
		.name	= "kernel",
		.offset	= 0x050000,
		.size	= 0x0d0000,
	}, {
		.name	= "rootfs",
		.offset	= 0x120000,
		.size	= 0x2e0000,
	}, {
		.name	= "firmware",
		.offset	= 0x050000,
		.size	= 0x3b0000,
	}
};
#endif /* CONFIG_MTD_PARTITIONS */

const struct flash_platform_data rt_g32b_flash = {
	.type		= "mx25l3205d",
#ifdef CONFIG_MTD_PARTITIONS
	.parts		= rt_g32b_partitions,
	.nr_parts	= ARRAY_SIZE(rt_g32b_partitions),
#endif
};

struct spi_board_info __initdata rt_g32b_spi_slave_info[] = {
	{
		.modalias	= "m25p80",
		.platform_data	= &rt_g32b_flash,
		.irq		= -1,
		.max_speed_hz	= 10000000,
		.bus_num	= 0,
		.chip_select	= 0,
	},
};

static struct gpio_button rt_g32b_gpio_buttons[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.threshold	= 3,
		.gpio		= RT_G32B_GPIO_BUTTON_RESET,
		.active_low	= 1,
	}, {
		.desc		= "wps",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.threshold	= 3,
		.gpio		= RT_G32B_GPIO_BUTTON_WPS,
		.active_low	= 1,
	}
};

static void __init rt_g32b_init(void)
{
	rt305x_gpio_init(RT305X_GPIO_MODE_GPIO << RT305X_GPIO_MODE_UART0_SHIFT);
	rt305x_register_spi(rt_g32b_spi_slave_info,
			    ARRAY_SIZE(rt_g32b_spi_slave_info));

	rt305x_esw_data.vlan_config = RT305X_ESW_VLAN_CONFIG_LLLLW;
	rt305x_register_ethernet();
	ramips_register_gpio_buttons(-1, RT_G32B_BUTTONS_POLL_INTERVAL,
				     ARRAY_SIZE(rt_g32b_gpio_buttons),
				     rt_g32b_gpio_buttons);
	rt305x_register_wifi();
	rt305x_register_wdt();
}

MIPS_MACHINE(RAMIPS_MACH_RT_G32_B1, "RT-G32-B1", "Asus RT-G32 B1",
	     rt_g32b_init);
