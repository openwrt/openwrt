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
//#include <linux/kernel.h>
//#include <linux/list.h>
//#include <linux/device.h>
#include <linux/platform_device.h>
//#include <linux/version.h>
//#include <linux/input.h>
#include <linux/mtd/map.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/physmap.h>
#include <linux/root_dev.h>

#include <asm/rdc_boards.h>

static struct rdc_platform_data rdcplat_data;

/* LEDS */
static struct platform_device rdc321x_leds = {
	.name = "leds-gpio",
	.id = -1,
	.dev = {
		.platform_data = &rdcplat_data.led_data,
	}
};

/* Button */
static struct platform_device rdc321x_buttons = {
	.name = "gpio-buttons",
	.id = -1,
	.dev = {
		.platform_data = &rdcplat_data.button_data,
	}
};
	
static __initdata struct platform_device *rdc321x_devs[] = {
	&rdc321x_leds,
	&rdc321x_buttons,
};

const char *__initdata boards[] = {
	"Sitecom",
	"AR525W",
	"Bifferboard",
	"R8610",
	0
};

static struct map_info rdc_map_info = {
	.name = "rdc_flash",
	.size = 0x800000,	//8MB
	.phys = 0xFF800000,	//(u32) -rdc_map_info.size;
	.bankwidth = 2
};

static int __init rdc_board_setup(void)
{
	struct mtd_partition *partitions;
	int count, res;
	struct mtd_info *mtdinfo;
	
	simple_map_init(&rdc_map_info);
	
	while (true) {
		rdc_map_info.virt = ioremap(rdc_map_info.phys, rdc_map_info.size);
		if (rdc_map_info.virt == NULL)
			continue;
		
		mtdinfo = do_map_probe("cfi_probe", &rdc_map_info);
		if (mtdinfo == NULL)
			mtdinfo = do_map_probe("jedec_probe", &rdc_map_info);
		if (mtdinfo != NULL)
			break;

		iounmap(rdc_map_info.virt);
		if ((rdc_map_info.size >>= 1) < 0x100000)	//1MB
			panic("RDC321x: Could not find start of flash!");
		rdc_map_info.phys = (u32) -rdc_map_info.size;
	}
	
	count = parse_mtd_partitions(mtdinfo, boards, &partitions, (unsigned long) &rdcplat_data);
	
	if (count <= 0) {
		panic("RDC321x: can't identify board type");
		return -ENOSYS;
	}
	
	ROOT_DEV = 0;
	res = add_mtd_partitions(mtdinfo, partitions, count);
	if (res)
		return res;
	
	return platform_add_devices(rdc321x_devs, ARRAY_SIZE(rdc321x_devs));

}

late_initcall(rdc_board_setup);
