#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/i2c-gpio.h>

#include <dev-gpio-leds.h>

#include "../machtypes.h"
#include "devices.h"

#define BOARD_95C3AM1_GPIO_LED_0 10
#define BOARD_95C3AM1_GPIO_LED_1 11
#define BOARD_95C3AM1_GPIO_LED_2 12
#define BOARD_95C3AM1_GPIO_LED_3 13

static struct mtd_partition board_95C3AM1_partitions[] =
{
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

static struct flash_platform_data board_95C3AM1_flash_platform_data = {
	.name = "sflash",
	.parts = board_95C3AM1_partitions,
	.nr_parts = ARRAY_SIZE(board_95C3AM1_partitions)
};

static struct spi_board_info board_95C3AM1_flash_data __initdata = {
	.modalias		= "m25p80",
	.bus_num		= 0,
	.chip_select		= 0,
	.max_speed_hz		= 10 * 1000 * 1000,
	.mode			= SPI_MODE_3,
	.platform_data		= &board_95C3AM1_flash_platform_data
};

static struct gpio_led board_95C3AM1_gpio_leds[] __initdata = {
	{
		.name		= "power",
		.gpio		= BOARD_95C3AM1_GPIO_LED_0,
		.active_low	= 0,
	}, {
		.name		= "optical",
		.gpio		= BOARD_95C3AM1_GPIO_LED_1,
		.active_low	= 0,
	}, {
		.name		= "lan",
		.gpio		= BOARD_95C3AM1_GPIO_LED_2,
		.active_low	= 0,
	}, {
		.name		= "update",
		.gpio		= BOARD_95C3AM1_GPIO_LED_3,
		.active_low	= 0,
	}
};

static struct i2c_gpio_platform_data board_95C3AM1_i2c_gpio_data = {
	.sda_pin	= 107,
	.scl_pin	= 108,
};

static struct platform_device board_95C3AM1_i2c_gpio_device = {
	.name		= "i2c-gpio",
	.id		= 0,
	.dev = {
		.platform_data	= &board_95C3AM1_i2c_gpio_data,
	}
};

static void __init board_95C3AM1_init(void)
{
	falcon_register_i2c();
	falcon_register_spi_flash(&board_95C3AM1_flash_data);
	platform_device_register(&board_95C3AM1_i2c_gpio_device);
	ltq_add_device_gpio_leds(-1, ARRAY_SIZE(board_95C3AM1_gpio_leds),
						board_95C3AM1_gpio_leds);
}

MIPS_MACHINE(LANTIQ_MACH_95C3AM1,
			"95C3AM1",
			"95C3AM1 Board",
			board_95C3AM1_init);
