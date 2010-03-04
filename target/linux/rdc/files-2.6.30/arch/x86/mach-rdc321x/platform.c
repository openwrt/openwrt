/*
 *  Generic RDC321x platform devices
 *
 *  Copyright (C) 2007-2009 OpenWrt.org
 *  Copyright (C) 2007 Florian Fainelli <florian@openwrt.org>
 *  Copyright (C) 2008-2009 Daniel Gimpelevich <daniel@gimpelevich.san-francisco.ca.us>
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
#include <linux/version.h>
#include <linux/leds.h>
#include <linux/gpio_buttons.h>
#include <linux/input.h>
#include <linux/mtd/map.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/physmap.h>
#include <linux/root_dev.h>

/* Flash */
#ifdef CONFIG_MTD_R8610
#define CONFIG_MTD_RDC3210 1
#elif defined CONFIG_MTD_RDC3210
static struct resource rdc_flash_resource[] = {
	[0] = {
		.start = (u32)-CONFIG_MTD_RDC3210_SIZE,
		.end = (u32)-1,
		.flags = IORESOURCE_MEM,
	},
};

static struct platform_device rdc_flash_device = {
	.name = "rdc321x-flash",
	.id = -1,
	.num_resources = ARRAY_SIZE(rdc_flash_resource),
	.resource = rdc_flash_resource,
};
#else
static struct mtd_partition rdc_flash_parts[15];

static struct resource rdc_flash_resource = {
	.end = (u32)-1,
	.flags = IORESOURCE_MEM,
};

static struct physmap_flash_data rdc_flash_data = {
	.parts = rdc_flash_parts,
};

static struct platform_device rdc_flash_device = {
	.name = "physmap-flash",
	.id = -1,
	.resource = &rdc_flash_resource,
	.num_resources = 1,
	.dev.platform_data = &rdc_flash_data,
};
#endif

/* LEDS */
static struct gpio_led default_leds[] = {
	{ .name = "rdc321x:dmz", .gpio = 1, .active_low = 1},
};

static struct gpio_led sitecom_leds[] = {
	{ .name = "rdc321x:power", .gpio = 15, .active_low = 1},
	{ .name = "rdc321x:usb0", .gpio = 0, .active_low = 1},
	{ .name = "rdc321x:usb1", .gpio = 1, .active_low = 1},
};

static struct gpio_led_platform_data rdc321x_led_data = {
	.num_leds = ARRAY_SIZE(default_leds),
	.leds = default_leds,
};

static struct platform_device rdc321x_leds = {
	.name = "leds-gpio",
	.id = -1,
	.dev = {
		.platform_data = &rdc321x_led_data,
	}
};

/* Button */
static struct gpio_button rdc321x_gpio_btn[] = {
	{
		.gpio = 6,
		.code = BTN_0,
		.desc = "Reset",
		.active_low = 1,
	}
};

static struct gpio_buttons_platform_data rdc321x_gpio_btn_data = {
	.buttons = rdc321x_gpio_btn,
	.nbuttons = ARRAY_SIZE(rdc321x_gpio_btn),
};

static struct platform_device rdc321x_button = {
	.name = "gpio-buttons",
	.id = -1,
	.dev = {
		.platform_data = &rdc321x_gpio_btn_data,
	}
};

static struct platform_device *rdc321x_devs[] = {
	&rdc_flash_device,
	&rdc321x_leds,
	&rdc321x_button,
};

static int probe_flash_start(struct map_info *the_map)
{
	struct mtd_info *res;

	the_map->virt = ioremap(the_map->phys, the_map->size);
	if (the_map->virt == NULL)
		return 1;
	for (the_map->bankwidth = 32; the_map->bankwidth; the_map->bankwidth
			>>= 1) {
		res = do_map_probe("cfi_probe", the_map);
		if (res == NULL)
			res = do_map_probe("jedec_probe", the_map);
		if (res != NULL)
			break;
	}
	iounmap(the_map->virt);
	if (res != NULL)
		the_map->phys = (u32)-(s32)(the_map->size = res->size);
	return res == NULL;
}

static int __init rdc_board_setup(void)
{
#ifndef CONFIG_MTD_RDC3210
	struct map_info rdc_map_info;
	u32 the_header[8];

	ROOT_DEV = 0;
	rdc_map_info.name = rdc_flash_device.name;
	rdc_map_info.size = 0x800000;	//8MB
	rdc_map_info.phys = (u32) -rdc_map_info.size;
	rdc_map_info.bankwidth = 2;
	rdc_map_info.set_vpp = NULL;
	simple_map_init(&rdc_map_info);
	while (probe_flash_start(&rdc_map_info)) {
		if (rdc_map_info.size /= 2 < 0x100000)	//1MB
			panic("Could not find start of flash!");
		rdc_map_info.phys = (u32) -rdc_map_info.size;
	}
	rdc_flash_resource.start = rdc_map_info.phys;
	rdc_flash_data.width = rdc_map_info.bankwidth;
	rdc_map_info.virt = ioremap_nocache(rdc_map_info.phys, 0x10);
	if (rdc_map_info.virt == NULL)
		panic("Could not ioremap to read device magic!");
	the_header[0] = ((u32 *)rdc_map_info.virt)[0];
	the_header[1] = ((u32 *)rdc_map_info.virt)[1];
	the_header[2] = ((u32 *)rdc_map_info.virt)[2];
	the_header[3] = ((u32 *)rdc_map_info.virt)[3];
	iounmap(rdc_map_info.virt);
	rdc_map_info.virt = ioremap_nocache(rdc_map_info.phys + 0x8000, 0x10);
	if (rdc_map_info.virt == NULL)
		panic("Could not ioremap to read device magic!");
	the_header[4] = ((u32 *)rdc_map_info.virt)[0];
	the_header[5] = ((u32 *)rdc_map_info.virt)[1];
	the_header[6] = ((u32 *)rdc_map_info.virt)[2];
	the_header[7] = ((u32 *)rdc_map_info.virt)[3];
	iounmap(rdc_map_info.virt);
	if (!memcmp(the_header, "GMTK", 4)) {	/* Gemtek */
		/* TODO */
	} else if (!memcmp(the_header + 4, "CSYS", 4)) {	/* Sitecom */
		rdc_flash_parts[0].name = "system";
		rdc_flash_parts[0].offset = 0;
		rdc_flash_parts[0].size = rdc_map_info.size - 0x10000;
		rdc_flash_parts[1].name = "config";
		rdc_flash_parts[1].offset = 0;
		rdc_flash_parts[1].size = 0x8000;
		rdc_flash_parts[2].name = "magic";
		rdc_flash_parts[2].offset = 0x8000;
		rdc_flash_parts[2].size = 0x14;
		rdc_flash_parts[3].name = "kernel";
		rdc_flash_parts[3].offset = 0x8014;
		rdc_flash_parts[3].size = the_header[5];
		rdc_flash_parts[4].name = "rootfs";
		rdc_flash_parts[4].offset = 0x8014 + the_header[5];
		rdc_flash_parts[4].size = rdc_flash_parts[0].size - rdc_flash_parts[4].offset;
		rdc_flash_parts[5].name = "bootloader";
		rdc_flash_parts[5].offset = rdc_flash_parts[0].size;
		rdc_flash_parts[5].size = 0x10000;
		rdc_flash_data.nr_parts = 6;

		rdc321x_led_data.num_leds = ARRAY_SIZE(sitecom_leds);
		rdc321x_led_data.leds = sitecom_leds;
	} else if (!memcmp(((u8 *)the_header) + 14, "Li", 2)) {	/* AMIT */
		rdc_flash_parts[0].name = "kernel_parthdr";
		rdc_flash_parts[0].offset = 0;
		rdc_flash_parts[0].size = 0x10;
		rdc_flash_parts[1].name = "kernel";
		rdc_flash_parts[1].offset = 0x10;
		rdc_flash_parts[1].size = 0xffff0;
		rdc_flash_parts[2].name = "rootfs_parthdr";
		rdc_flash_parts[2].offset = 0x100000;
		rdc_flash_parts[2].size = 0x10;
		rdc_flash_parts[3].name = "rootfs";
		rdc_flash_parts[3].offset = 0x100010;
		rdc_flash_parts[3].size = rdc_map_info.size - 0x160010;
		rdc_flash_parts[4].name = "config_parthdr";
		rdc_flash_parts[4].offset = rdc_map_info.size - 0x60000;
		rdc_flash_parts[4].size = 0x10;
		rdc_flash_parts[5].name = "config";
		rdc_flash_parts[5].offset = rdc_map_info.size - 0x5fff0;
		rdc_flash_parts[5].size = 0xfff0;
		rdc_flash_parts[6].name = "recoveryfs_parthdr";
		rdc_flash_parts[6].offset = rdc_map_info.size - 0x50000;
		rdc_flash_parts[6].size = 0x10;
		rdc_flash_parts[7].name = "recoveryfs";
		rdc_flash_parts[7].offset = rdc_map_info.size - 0x4fff0;
		rdc_flash_parts[7].size = 0x3fff0;
		rdc_flash_parts[8].name = "recovery_parthdr";
		rdc_flash_parts[8].offset = rdc_map_info.size - 0x10000;
		rdc_flash_parts[8].size = 0x10;
		rdc_flash_parts[9].name = "recovery";
		rdc_flash_parts[9].offset = rdc_map_info.size - 0xfff0;
		rdc_flash_parts[9].size = 0x7ff0;
		rdc_flash_parts[10].name = "productinfo_parthdr";
		rdc_flash_parts[10].offset = rdc_map_info.size - 0x8000;
		rdc_flash_parts[10].size = 0x10;
		rdc_flash_parts[11].name = "productinfo";
		rdc_flash_parts[11].offset = rdc_map_info.size - 0x7ff0;
		rdc_flash_parts[11].size = 0x1ff0;
		rdc_flash_parts[12].name = "bootloader_parthdr";
		rdc_flash_parts[12].offset = rdc_map_info.size - 0x6000;
		rdc_flash_parts[12].size = 0x10;
		rdc_flash_parts[13].name = "bootloader";
		rdc_flash_parts[13].offset = rdc_map_info.size - 0x5ff0;
		rdc_flash_parts[13].size = 0x5ff0;
		rdc_flash_parts[14].name = "everything";
		rdc_flash_parts[14].offset = 0;
		rdc_flash_parts[14].size = rdc_map_info.size;
		rdc_flash_data.nr_parts = 15;
	} else {	/* ZyXEL */
		rdc_flash_parts[0].name = "kernel";
		rdc_flash_parts[0].offset = 0;
		rdc_flash_parts[0].size = 0x100000;
		rdc_flash_parts[1].name = "rootfs";
		rdc_flash_parts[1].offset = 0x100000;
		rdc_flash_parts[1].size = rdc_map_info.size - 0x140000;
		rdc_flash_parts[2].name = "linux";
		rdc_flash_parts[2].offset = 0;
		rdc_flash_parts[2].size = rdc_map_info.size - 0x40000;
		rdc_flash_parts[3].name = "config";
		rdc_flash_parts[3].offset = rdc_map_info.size - 0x40000;
		rdc_flash_parts[3].size = 0x10000;
		rdc_flash_parts[4].name = "productinfo";
		rdc_flash_parts[4].offset = rdc_map_info.size - 0x30000;
		rdc_flash_parts[4].size = 0x10000;
		rdc_flash_parts[5].name = "bootloader";
		rdc_flash_parts[5].offset = rdc_map_info.size - 0x20000;
		rdc_flash_parts[5].size = 0x20000;
		rdc_flash_data.nr_parts = 6;
	}
#endif
	return platform_add_devices(rdc321x_devs, ARRAY_SIZE(rdc321x_devs));
}

#ifdef CONFIG_MTD_RDC3210
arch_initcall(rdc_board_setup);
#else
late_initcall(rdc_board_setup);
#endif
