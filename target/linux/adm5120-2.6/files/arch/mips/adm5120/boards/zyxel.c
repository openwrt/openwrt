/*
 *  $Id$
 *
 *  ZyXEL Prestige P-334/P-335 boards
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

static void switch_bank_gpio5(unsigned bank)
{
	switch (bank) {
	case 0:
		gpio_set_value(ADM5120_GPIO_PIN5, 0);
		break;
	case 1:
		gpio_set_value(ADM5120_GPIO_PIN5, 1);
		break;
	}
}

static struct mtd_partition p33x_partitions[] = {
	{
		.name	= "bootbase",
		.offset	= 0,
		.size	= 16*1024,
		.mask_flags = MTD_WRITEABLE,
	} , {
		.name	= "rom",
		.offset	= MTDPART_OFS_APPEND,
		.size	= 16*1024,
	} , {
		.name	= "bootext",
		.offset	= MTDPART_OFS_APPEND,
		.size	= 96*1024,
		.mask_flags = MTD_WRITEABLE,
	} , {
		.name	= "trx",
		.offset	= MTDPART_OFS_APPEND,
		.size	= MTDPART_SIZ_FULL,
	} , {
		.name	= "firmware",
		.offset	= 32*1024,
		.size	= MTDPART_SIZ_FULL,
	}
};

static struct platform_device *p334_devices[] __initdata = {
	&adm5120_flash0_device,
};

static struct platform_device *p335_devices[] __initdata = {
	&adm5120_flash0_device,
	&adm5120_usbc_device,
};

static void __init p33x_setup(void)
{
	gpio_request(ADM5120_GPIO_PIN5, NULL); /* for flash A20 line */
	gpio_direction_output(ADM5120_GPIO_PIN5, 0);

	/* setup data for flash0 device */
	adm5120_flash0_data.switch_bank = switch_bank_gpio5;
	adm5120_flash0_data.nr_parts = ARRAY_SIZE(p33x_partitions);
	adm5120_flash0_data.parts = p33x_partitions;

	/* TODO: setup mac address */
}

unsigned char p33x_vlans[6] __initdata = {
	/* FIXME: untested */
	0x50, 0x48, 0x44, 0x42, 0x41, 0x00
};

static struct adm5120_board p334wt_board __initdata = {
	.mach_type	= MACH_ADM5120_P334WT,
	.name		= "ZyXEL Prestige 334WT",
	.board_setup	= p33x_setup,
	.eth_num_ports	= 5,
	.eth_vlans	= p33x_vlans,
	.num_devices	= ARRAY_SIZE(p334_devices),
	.devices	= p334_devices,
};

static struct adm5120_board p335_board __initdata = {
	.mach_type	= MACH_ADM5120_P335,
	.name		= "ZyXEL Prestige 335/335WT",
	.board_setup	= p33x_setup,
	.eth_num_ports	= 5,
	.eth_vlans	= p33x_vlans,
	.num_devices	= ARRAY_SIZE(p335_devices),
	.devices	= p335_devices,
};

static int __init register_boards(void)
{
	adm5120_board_register(&p334wt_board);
	adm5120_board_register(&p335_board);
	return 0;
}

pure_initcall(register_boards);
