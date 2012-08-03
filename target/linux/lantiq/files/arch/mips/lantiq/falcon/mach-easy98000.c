/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 *  Copyright (C) 2011 Thomas Langer <thomas.langer@lantiq.com>
 *  Copyright (C) 2011 John Crispin <blogic@openwrt.org>
 */

#include <linux/platform_device.h>
#include <linux/mtd/partitions.h>
#include <linux/spi/spi.h>
#include <linux/spi/spi_gpio.h>
#include <linux/spi/eeprom.h>

#include "../machtypes.h"

#include "devices.h"

static struct mtd_partition easy98000_nor_partitions[] = {
	{
		.name	= "uboot",
		.offset	= 0x0,
		.size	= 0x40000,
	},
	{
		.name	= "uboot_env",
		.offset	= 0x40000,
		.size	= 0x40000,	/* 2 sectors for redundant env. */
	},
	{
		.name	= "linux",
		.offset	= 0x80000,
		.size	= 0xF80000,	/* map only 16 MiB */
	},
};

struct physmap_flash_data easy98000_nor_flash_data = {
	.nr_parts	= ARRAY_SIZE(easy98000_nor_partitions),
	.parts		= easy98000_nor_partitions,
};

static struct flash_platform_data easy98000_spi_flash_platform_data = {
	.name = "sflash",
	.parts = easy98000_nor_partitions,
	.nr_parts = ARRAY_SIZE(easy98000_nor_partitions)
};

static struct spi_board_info easy98000_spi_flash_data __initdata = {
	.modalias		= "m25p80",
	.bus_num		= 0,
	.chip_select		= 0,
	.max_speed_hz		= 10 * 1000 * 1000,
	.mode			= SPI_MODE_3,
	.platform_data		= &easy98000_spi_flash_platform_data
};

/* setup gpio based spi bus/device for access to the eeprom on the board */
#define SPI_GPIO_MRST		102
#define SPI_GPIO_MTSR		103
#define SPI_GPIO_CLK		104
#define SPI_GPIO_CS0		105
#define SPI_GPIO_CS1		106
#define SPI_GPIO_BUS_NUM	1

static struct spi_gpio_platform_data easy98000_spi_gpio_data = {
	.sck		= SPI_GPIO_CLK,
	.mosi		= SPI_GPIO_MTSR,
	.miso		= SPI_GPIO_MRST,
	.num_chipselect	= 2,
};

static struct platform_device easy98000_spi_gpio_device = {
	.name			= "spi_gpio",
	.id			= SPI_GPIO_BUS_NUM,
	.dev.platform_data	= &easy98000_spi_gpio_data,
};

static struct spi_eeprom at25160n = {
	.byte_len	= 16 * 1024 / 8,
	.name		= "at25160n",
	.page_size	= 32,
	.flags		= EE_ADDR2,
};

static struct spi_board_info easy98000_spi_gpio_devices __initdata = {
	.modalias		= "at25",
	.bus_num		= SPI_GPIO_BUS_NUM,
	.max_speed_hz		= 1000 * 1000,
	.mode			= SPI_MODE_3,
	.chip_select		= 1,
	.controller_data	= (void *) SPI_GPIO_CS1,
	.platform_data		= &at25160n,
};

static void __init
easy98000_init_common(void)
{
	spi_register_board_info(&easy98000_spi_gpio_devices, 1);
	platform_device_register(&easy98000_spi_gpio_device);
	falcon_register_i2c();
}

static void __init
easy98000_init(void)
{
	easy98000_init_common();
	ltq_register_nor(&easy98000_nor_flash_data);
}

static void __init
easy98000sf_init(void)
{
	easy98000_init_common();
	falcon_register_spi_flash(&easy98000_spi_flash_data);
}

static void __init
easy98000nand_init(void)
{
	easy98000_init_common();
	falcon_register_nand();
}

MIPS_MACHINE(LANTIQ_MACH_EASY98000,
			"EASY98000",
			"EASY98000 Eval Board",
			easy98000_init);

MIPS_MACHINE(LANTIQ_MACH_EASY98000SF,
			"EASY98000SF",
			"EASY98000 Eval Board (Serial Flash)",
			easy98000sf_init);

MIPS_MACHINE(LANTIQ_MACH_EASY98000NAND,
			"EASY98000NAND",
			"EASY98000 Eval Board (NAND Flash)",
			easy98000nand_init);
