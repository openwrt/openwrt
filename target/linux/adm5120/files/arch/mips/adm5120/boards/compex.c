/*
 *  $Id$
 *
 *  Compex boards
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

static void wp54_reset(void)
{
	gpio_set_value(ADM5120_GPIO_PIN3, 0);
}

static struct mtd_partition wp54g_wrt_partitions[] = {
	{
		.name	= "cfe",
		.offset	= 0,
		.size	= 0x050000,
		.mask_flags = MTD_WRITEABLE,
	} , {
		.name	= "trx",
		.offset	= MTDPART_OFS_APPEND,
		.size	= 0x3A0000,
	} , {
		.name	= "nvram",
		.offset	= MTDPART_OFS_APPEND,
		.size	= 0x010000,
	}
};

static struct platform_device *np2xg_devices[] __initdata = {
	&adm5120_flash0_device,
	&adm5120_hcd_device,
};

static struct platform_device *wp54_devices[] __initdata = {
	&adm5120_flash0_device,
};

static void __init np2xg_setup(void)
{
	gpio_request(ADM5120_GPIO_PIN5, NULL); /* for flash A20 line */
	gpio_direction_output(ADM5120_GPIO_PIN5, 0);

	/* setup data for flash0 device */
	adm5120_flash0_data.switch_bank = switch_bank_gpio5;

	/* TODO: setup mac address */
}

static void __init wp54_setup(void)
{
	gpio_request(ADM5120_GPIO_PIN5, NULL); /* for flash A20 line */
	gpio_direction_output(ADM5120_GPIO_PIN5, 0);

	gpio_request(ADM5120_GPIO_PIN3, NULL); /* for system reset */
	gpio_direction_output(ADM5120_GPIO_PIN3, 1);


	/* setup data for flash0 device */
	adm5120_flash0_data.switch_bank = switch_bank_gpio5;

	/* TODO: setup mac address */
}

static void __init wp54_wrt_setup(void)
{
	gpio_request(ADM5120_GPIO_PIN5, NULL); /* for flash A20 line */
	gpio_direction_output(ADM5120_GPIO_PIN5, 0);

	gpio_request(ADM5120_GPIO_PIN3, NULL); /* for system reset */
	gpio_direction_output(ADM5120_GPIO_PIN3, 1);

	/* setup data for flash0 device */
	adm5120_flash0_data.switch_bank = switch_bank_gpio5;
	adm5120_flash0_data.nr_parts = ARRAY_SIZE(wp54g_wrt_partitions);
	adm5120_flash0_data.parts = wp54g_wrt_partitions;

	/* TODO: setup mac address */
}

unsigned char np27g_vlans[6] __initdata = {
	/* FIXME: untested */
	0x41, 0x42, 0x44, 0x48, 0x50, 0x00
};

unsigned char np28g_vlans[6] __initdata = {
	0x50, 0x42, 0x44, 0x48, 0x00, 0x00
};

unsigned char wp54_vlans[6] __initdata = {
	0x41, 0x42, 0x00, 0x00, 0x00, 0x00
};

static struct adm5120_board np27g_board __initdata = {
	.mach_type	= MACH_ADM5120_NP27G,
	.name		= "Compex NetPassage 27G",
	.board_setup	= np2xg_setup,
	.eth_num_ports	= 5,
	.eth_vlans	= np27g_vlans,
	.num_devices	= ARRAY_SIZE(np2xg_devices),
	.devices	= np2xg_devices,
};

static struct adm5120_board np28g_board __initdata = {
	.mach_type	= MACH_ADM5120_NP28G,
	.name		= "Compex NetPassage 28G",
	.board_setup	= np2xg_setup,
	.eth_num_ports	= 4,
	.eth_vlans	= np28g_vlans,
	.num_devices	= ARRAY_SIZE(np2xg_devices),
	.devices	= np2xg_devices,
};

static struct adm5120_board wp54ag_board __initdata = {
	.mach_type	= MACH_ADM5120_WP54AG,
	.name		= "Compex WP54AG",
	.board_setup	= wp54_setup,
	.board_reset	= wp54_reset,
	.eth_num_ports	= 2,
	.eth_vlans	= wp54_vlans,
	.num_devices	= ARRAY_SIZE(wp54_devices),
	.devices	= wp54_devices,
};

static struct adm5120_board wp54g_board __initdata = {
	.mach_type	= MACH_ADM5120_WP54G,
	.name		= "Compex WP54G",
	.board_setup	= wp54_setup,
	.board_reset	= wp54_reset,
	.eth_num_ports	= 2,
	.eth_vlans	= wp54_vlans,
	.num_devices	= ARRAY_SIZE(wp54_devices),
	.devices	= wp54_devices,
};

static struct adm5120_board wp54g_wrt_board __initdata = {
	.mach_type	= MACH_ADM5120_WP54G,
	.name		= "Compex WP54G-WRT",
	.board_setup	= wp54_wrt_setup,
	.board_reset	= wp54_reset,
	.eth_num_ports	= 2,
	.eth_vlans	= wp54_vlans,
	.num_devices	= ARRAY_SIZE(wp54_devices),
	.devices	= wp54_devices,
};

static struct adm5120_board wpp54ag_board __initdata = {
	.mach_type	= MACH_ADM5120_WPP54AG,
	.name		= "Compex WPP54AG",
	.board_setup	= wp54_setup,
	.board_reset	= wp54_reset,
	.eth_num_ports	= 2,
	.eth_vlans	= wp54_vlans,
	.num_devices	= ARRAY_SIZE(wp54_devices),
	.devices	= wp54_devices,
};

static struct adm5120_board wpp54g_board __initdata = {
	.mach_type	= MACH_ADM5120_WPP54G,
	.name		= "Compex WPP54G",
	.board_setup	= wp54_setup,
	.board_reset	= wp54_reset,
	.eth_num_ports	= 2,
	.eth_vlans	= wp54_vlans,
	.num_devices	= ARRAY_SIZE(wp54_devices),
	.devices	= wp54_devices,
};

static int __init register_boards(void)
{
	adm5120_board_register(&np27g_board);
	adm5120_board_register(&np28g_board);
	adm5120_board_register(&wp54ag_board);
	adm5120_board_register(&wp54g_board);
	adm5120_board_register(&wp54g_wrt_board);
	adm5120_board_register(&wpp54ag_board);
	adm5120_board_register(&wpp54g_board);
	return 0;
}

pure_initcall(register_boards);
