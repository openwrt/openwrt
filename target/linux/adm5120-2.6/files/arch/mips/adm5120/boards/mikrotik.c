/*
 *  $Id$
 *
 *  Mikrotik RouterBOARDs 111/112/133/133C/150/153
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

static struct platform_device *rb150_devices[] __initdata = {
	&adm5120_flash0_device,
	/* TODO: nand device is not yet supported */
};

static void __init rb1xx_setup(void)
{
	/* setup data for flash0 device */
	adm5120_flash0_data.nr_parts = ARRAY_SIZE(rb1xx_partitions);
	adm5120_flash0_data.parts = rb1xx_partitions;

	/* TODO: setup mac address */
}

#if 0
/*
 * RB1xx boards have bad network performance with the default VLAN matrixes.
 * Disable it while the ethernet driver gets fixed.
 */
static unsigned char rb11x_vlans[6] __initdata = {
	/* FIXME: untested */
	0x41, 0x00, 0x00, 0x00, 0x00, 0x00
};

static unsigned char rb133_vlans[6] __initdata = {
	/* FIXME: untested */
	0x44, 0x42, 0x41, 0x00, 0x00, 0x00
};

static unsigned char rb133c_vlans[6] __initdata = {
	/* FIXME: untested */
	0x44, 0x00, 0x00, 0x00, 0x00, 0x00
};

static unsigned char rb15x_vlans[6] __initdata = {
	/* FIXME: untested */
	0x41, 0x42, 0x44, 0x48, 0x50, 0x00
};
#else
static unsigned char rb_vlans[6] __initdata = {
	0x7F, 0x00, 0x00, 0x00, 0x00, 0x00
};
#define rb11x_vlans	rb_vlans
#define rb133_vlans	rb_vlans
#define rb133c_vlans	rb_vlans
#define rb15x_vlans	rb_vlans
#endif

static struct adm5120_board rb111_board __initdata = {
	.mach_type	= MACH_ADM5120_RB_111,
	.name		= "Mikrotik RouterBOARD 111",
	.board_setup	= rb1xx_setup,
	.eth_num_ports	= 1,
	.eth_vlans	= rb11x_vlans,
	.num_devices	= ARRAY_SIZE(rb1xx_devices),
	.devices	= rb1xx_devices,
};

static struct adm5120_board rb112_board __initdata = {
	.mach_type	= MACH_ADM5120_RB_112,
	.name		= "Mikrotik RouterBOARD 112",
	.board_setup	= rb1xx_setup,
	.eth_num_ports	= 1,
	.eth_vlans	= rb11x_vlans,
	.num_devices	= ARRAY_SIZE(rb1xx_devices),
	.devices	= rb1xx_devices,
};

static struct adm5120_board rb133_board __initdata = {
	.mach_type	= MACH_ADM5120_RB_133,
	.name		= "Mikrotik RouterBOARD 133",
	.board_setup	= rb1xx_setup,
	.eth_num_ports	= 3,
	.eth_vlans	= rb133_vlans,
	.num_devices	= ARRAY_SIZE(rb1xx_devices),
	.devices	= rb1xx_devices,
};

static struct adm5120_board rb133c_board __initdata = {
	.mach_type	= MACH_ADM5120_RB_133C,
	.name		= "Mikrotik RouterBOARD 133C",
	.board_setup	= rb1xx_setup,
	.eth_num_ports	= 1,
	.eth_vlans	= rb133c_vlans,
	.num_devices	= ARRAY_SIZE(rb1xx_devices),
	.devices	= rb1xx_devices,
};

static struct adm5120_board rb150_board __initdata = {
	.mach_type	= MACH_ADM5120_RB_150,
	.name		= "Mikrotik RouterBOARD 150",
	.board_setup	= rb1xx_setup,
	.eth_num_ports	= 5,
	.eth_vlans	= rb15x_vlans,
	.num_devices	= ARRAY_SIZE(rb150_devices),
	.devices	= rb150_devices,
};

static struct adm5120_board rb153_board __initdata = {
	.mach_type	= MACH_ADM5120_RB_153,
	.name		= "Mikrotik RouterBOARD 153",
	.board_setup	= rb1xx_setup,
	.eth_num_ports	= 5,
	.eth_vlans	= rb15x_vlans,
	.num_devices	= ARRAY_SIZE(rb1xx_devices),
	.devices	= rb1xx_devices,
};

static int __init register_boards(void)
{
	adm5120_board_register(&rb111_board);
	adm5120_board_register(&rb112_board);
	adm5120_board_register(&rb133_board);
	adm5120_board_register(&rb133c_board);
	adm5120_board_register(&rb150_board);
	adm5120_board_register(&rb153_board);
	return 0;
}

pure_initcall(register_boards);
