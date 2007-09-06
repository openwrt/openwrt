/*
 *  $Id$
 *
 *  Edimax boards
 *
 *  Copyright (C) 2007 OpenWrt.org
 *  Copyright (C) 2007 Gabor Juhos <juhosg at openwrt.org>
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

#include <linux/kernel.h>
#include <linux/init.h>

#include <asm/bootinfo.h>
#include <asm/gpio.h>

#include <asm/mach-adm5120/adm5120_board.h>
#include <asm/mach-adm5120/adm5120_platform.h>

static struct mtd_partition br6104k_partitions[] = {
	{
		.name	= "admboot",
		.offset	= 0,
		.size	= 32*1024,
		.mask_flags = MTD_WRITEABLE,
	} , {
		.name	= "config",
		.offset	= MTDPART_OFS_APPEND,
		.size	= 32*1024,
	} , {
		.name	= "firmware",
		.offset	= MTDPART_OFS_APPEND,
		.size	= MTDPART_SIZ_FULL,
	}
};

static struct platform_device *br6104k_devices[] __initdata = {
	&adm5120_flash0_device,
};

static void __init br6104k_setup(void) {
	/* setup data for flash0 device */
	adm5120_flash0_data.nr_parts = ARRAY_SIZE(br6104k_partitions);
	adm5120_flash0_data.parts = br6104k_partitions;

	/* TODO: setup mac addresses, if possible */
}

unsigned char br6104k_vlans[6] = {
	0x41, 0x42, 0x44, 0x48, 0x50, 0x00
};

static struct adm5120_board br6104k_board __initdata = {
	.mach_type	= MACH_ADM5120_BR6104K,
	.name		= "Edimax BR-6104K",
	.board_setup	= br6104k_setup,
	.eth_num_ports	= 5,
	.eth_vlans	= br6104k_vlans,
	.num_devices	= ARRAY_SIZE(br6104k_devices),
	.devices	= br6104k_devices,
};

static int __init register_boards(void)
{
	adm5120_board_register(&br6104k_board);
	return 0;
}

pure_initcall(register_boards);
