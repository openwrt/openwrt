/*
 *  $Id$
 *
 *  Infineon boards
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

static void switch_bank_gpio3(unsigned bank)
{
	switch (bank) {
	case 0:
		gpio_set_value(ADM5120_GPIO_PIN3, 0);
		break;
	case 1:
		gpio_set_value(ADM5120_GPIO_PIN3, 1);
		break;
	}
}

static struct mtd_partition easy83000_partitions[] = {
	{
		.name	= "admboot",
		.offset	= 0,
		.size	= 64*1024,
		.mask_flags = MTD_WRITEABLE,
	} , {
		.name	= "boardcfg",
		.offset	= MTDPART_OFS_APPEND,
		.size	= 64*1024,
	} , {
		.name	= "firmware",
		.offset	= MTDPART_OFS_APPEND,
		.size	= MTDPART_SIZ_FULL,
	}
};

static struct platform_device *easy83000_devices[] __initdata = {
	&adm5120_flash0_device,
};

static void __init easy83000_setup(void)
{
	gpio_request(ADM5120_GPIO_PIN3, NULL); /* for flash A20 line */
	gpio_direction_output(ADM5120_GPIO_PIN3, 0);

	/* setup data for flash0 device */
	adm5120_flash0_data.switch_bank = switch_bank_gpio3;
	adm5120_flash0_data.nr_parts = ARRAY_SIZE(easy83000_partitions);
	adm5120_flash0_data.parts = easy83000_partitions;

	/* TODO: setup mac addresses */
}

static struct adm5120_board easy83000_board __initdata = {
	.mach_type	= MACH_ADM5120_EASY83000,
	.name		= "Infineon EASY-83000",
	.board_setup	= easy83000_setup,
	.num_eth_ports	= 6,
	.num_devices	= ARRAY_SIZE(easy83000_devices),
	.devices	= easy83000_devices,
};

static int __init register_boards(void)
{
	adm5120_board_register(&easy83000_board);
	return 0;
}

pure_initcall(register_boards);
