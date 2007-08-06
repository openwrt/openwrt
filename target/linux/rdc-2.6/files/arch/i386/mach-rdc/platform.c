/*
 *  $Id: platform.c 8331 2007-08-03 15:59:23Z florian $
 *
 *  Generic RDC321x platform devices
 *
 *  Copyright (C) 2007 OpenWrt.org
 *  Copyright (C) 2007 Florian Fainelli <florian@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the
 *  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA  02110-1301, USA.
 *
 */

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/device.h>
#include <linux/platform_device.h>

#include <asm/gpio.h>
#include <asm/mach-rdc/rdc321x_defs.h>

/* FIXME : Flash */
static struct resource rdc_flash_resource[] = {
	[0] = {
		.start = RDC_FLASH_BASE,
		.end = RDC_FLASH_BASE+CONFIG_MTD_RDC3210_SIZE-1,
		.flags = IORESOURCE_MEM,
	},
};

static struct platform_device rdc_flash_device = {
	.name = "rdc321x-flash",
	.id = -1,
	.num_resources = ARRAY_SIZE(rdc_flash_resource),
	.resource = rdc_flash_resource,
};

/* LEDS */
static struct platform_device rdc321x_leds = {
	.name = "rdc321x-leds",
	.id = -1,
	.num_resources = 0,
};

static int __init rdc_board_setup(void)
{
	int err;

	err = platform_device_register(&rdc_flash_device);
	if (err)
		printk(KERN_ALERT "rdc321x: failed to register flash\n");

	err = platform_device_register(&rdc321x_leds);
	if (err)
		printk(KERN_ALERT "rdc321x: failed to register LEDS\n");

	return err; 
}

arch_initcall(rdc_board_setup);
