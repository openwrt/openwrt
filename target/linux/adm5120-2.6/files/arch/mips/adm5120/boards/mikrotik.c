/*
 *  $Id$
 *
 *  Mikrotik RouterBOARDs 111/112/133/133C/153
 *
 *  Copyright (C) 2007 OpenWrt.org
 *  Copyright (C) 2007 Gabor Juhos <juhosg@freemail.hu>
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

static struct mtd_partition rb1xx_partitions[] = {
	{
		.name	= "booter",
		.offset	= 0,
		.size	= 64*1024,
		.mask_flags = MTD_WRITEABLE,
	} , {
		.name	= "firmware",
		.offset	= MTDPART_OFS_APPEND,
		.size	= MTDPART_SIZ_FULL,
	}
};

static struct platform_device *rb1xx_devices[] __initdata = {
	&adm5120_flash0_device,
	&adm5120_nand_device,
};

static void __init rb1xx_setup(void)
{
	/* setup data for flash0 device */
	adm5120_flash0_data.nr_parts = ARRAY_SIZE(rb1xx_partitions);
	adm5120_flash0_data.parts = rb1xx_partitions;

	/* TODO: setup mac address */
}

static struct adm5120_board rb111_board __initdata = {
	.mach_type	= MACH_ADM5120_RB_111,
	.name		= "Mikrotik RouterBOARD 111",
	.board_setup	= rb1xx_setup,
	.num_eth_ports	= 1,
	.num_devices	= ARRAY_SIZE(rb1xx_devices),
	.devices	= rb1xx_devices,
};

static struct adm5120_board rb112_board __initdata = {
	.mach_type	= MACH_ADM5120_RB_112,
	.name		= "Mikrotik RouterBOARD 112",
	.board_setup	= rb1xx_setup,
	.num_eth_ports	= 1,
	.num_devices	= ARRAY_SIZE(rb1xx_devices),
	.devices	= rb1xx_devices,
};

static struct adm5120_board rb133_board __initdata = {
	.mach_type	= MACH_ADM5120_RB_133,
	.name		= "Mikrotik RouterBOARD 133",
	.board_setup	= rb1xx_setup,
	.num_eth_ports	= 3,
	.num_devices	= ARRAY_SIZE(rb1xx_devices),
	.devices	= rb1xx_devices,
};

static struct adm5120_board rb133c_board __initdata = {
	.mach_type	= MACH_ADM5120_RB_133C,
	.name		= "Mikrotik RouterBOARD 133C",
	.board_setup	= rb1xx_setup,
	.num_eth_ports	= 1,
	.num_devices	= ARRAY_SIZE(rb1xx_devices),
	.devices	= rb1xx_devices,
};

static struct adm5120_board rb153_board __initdata = {
	.mach_type	= MACH_ADM5120_RB_153,
	.name		= "Mikrotik RouterBOARD 153",
	.board_setup	= rb1xx_setup,
	.num_eth_ports	= 5,
	.num_devices	= ARRAY_SIZE(rb1xx_devices),
	.devices	= rb1xx_devices,
};

static int __init register_boards(void)
{
	adm5120_board_register(&rb111_board);
	adm5120_board_register(&rb112_board);
	adm5120_board_register(&rb133_board);
	adm5120_board_register(&rb133c_board);
	adm5120_board_register(&rb153_board);
	return 0;
}

pure_initcall(register_boards);
