/*
 *  Sitecom RDC321x platform devices
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
#include <linux/mtd/physmap.h>
#include <linux/input.h>

#include <asm/rdc_boards.h>

struct image_header {
	char	magic[4];
	u32	kernel_length;
	u32	ramdisk_length;
	char	magic2[4];
	u32	kernel_length2;
};

static struct gpio_led sitecom_leds[] = {
	{ .name = "rdc321x:power", .gpio = 15, .active_low = 1},
	{ .name = "rdc321x:usb0", .gpio = 0, .active_low = 1},
	{ .name = "rdc321x:usb1", .gpio = 1, .active_low = 1},
};

static struct gpio_button sitecom_btns[] = {
	{
		.gpio = 6,
		.code = BTN_0,
		.desc = "Reset",
		.active_low = 1,
	}
};

static int __init parse_sitecom_partitions(struct mtd_info *master, struct mtd_partition **pparts, unsigned long plat_data)
{
	struct image_header header;
	int res;
	size_t len;
	struct mtd_partition *rdc_flash_parts;
	struct rdc_platform_data *pdata = (struct rdc_platform_data *) plat_data;
	
	if (master->size != 0x400000) //4MB
		return -ENOSYS;

	res =  master->read(master, 0x8000, sizeof(header), &len, (char *)&header);
	if (res)
		return res;

	if (strncmp(header.magic, "CSYS", 4) || strncmp(header.magic2, "WRRM", 4))
		return -ENOSYS;
	
	rdc_flash_parts = kzalloc(sizeof(struct mtd_partition) * 5, GFP_KERNEL);
	
	rdc_flash_parts[0].name = "firmware";
	rdc_flash_parts[0].offset = 0x8000;
	rdc_flash_parts[0].size = 0x3F0000;
	rdc_flash_parts[1].name = "config";
	rdc_flash_parts[1].offset = 0;
	rdc_flash_parts[1].size = 0x8000;
	rdc_flash_parts[2].name = "kernel";
	rdc_flash_parts[2].offset = 0x8014;
	rdc_flash_parts[2].size = header.kernel_length;
	rdc_flash_parts[3].name = "rootfs";
	rdc_flash_parts[3].offset = 0x8014 + header.kernel_length;
	rdc_flash_parts[3].size = 0x3F0000 - rdc_flash_parts[3].offset;
	rdc_flash_parts[4].name = "bootloader";
	rdc_flash_parts[4].offset = 0x3F0000;
	rdc_flash_parts[4].size = 0x10000;

	*pparts = rdc_flash_parts;
	
	pdata->led_data.num_leds = ARRAY_SIZE(sitecom_leds);
	pdata->led_data.leds = sitecom_leds;
	pdata->button_data.nbuttons = ARRAY_SIZE(sitecom_btns);
	pdata->button_data.buttons = sitecom_btns;
	
	return 5;
}

struct mtd_part_parser __initdata sitecom_parser = {
	.owner = THIS_MODULE,
	.parse_fn = parse_sitecom_partitions,
	.name = "Sitecom",
};

static int __init sitecom_setup(void)
{
	return register_mtd_parser(&sitecom_parser);
}

arch_initcall(sitecom_setup);
