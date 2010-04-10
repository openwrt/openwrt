#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/leds.h>
#include <linux/gpio.h>
#include <linux/gpio_buttons.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/physmap.h>
#include <linux/input.h>

#include <machine.h>
#include <ifxmips_prom.h>

#include "arcaydian.h"
#include "devices.h"

#define ARV452_EBU_GPIO_START	0x14000000
#define ARV452_EBU_GPIO_SIZE	0x00001000

#define ARV452_GPIO_BUTTON_RESET		14
#define ARV452_BUTTONS_POLL_INTERVAL	20

#define ARV452_LATCH_SWITCH				(1 << 10)

#ifdef CONFIG_MTD_PARTITIONS
static struct mtd_partition arv452_partitions[] =
{
	{
		.name	= "uboot",
		.offset	= 0x0,
		.size	= 0x20000,
	},
	{
		.name	= "uboot_env",
		.offset	= 0x20000,
		.size	= 0x0,
	},
	{
		.name	= "kernel",
		.offset	= 0x0,
		.size	= 0x0,
	},
	{
		.name	= "rootfs",
		.offset	= 0x0,
		.size	= 0x0,
	},
	{
		.name	= "board_config",
		.offset	= 0x3f0000,
		.size	= 0x10000,
	},
	{
		.name	= "openwrt",
		.offset	= 0x0,
		.size	= 0x0,
	},
};
#endif

static struct physmap_flash_data arv452_flash_data = {
#ifdef CONFIG_MTD_PARTITIONS
	.nr_parts	= ARRAY_SIZE(arv452_partitions),
	.parts		= arv452_partitions,
#endif
};

static struct gpio_led
arv452_leds_gpio[] __initdata = {
/*
	{ .name = "ifx0", .gpio = 0, .active_low = 1, },
	{ .name = "ifx1", .gpio = 1, .active_low = 1, },
	{ .name = "ifx2", .gpio = 2, .active_low = 1, },
*/
	{ .name = "ifx:blue:power", .gpio = 3, .active_low = 1, },
	{ .name = "ifx:blue:adsl", .gpio = 4, .active_low = 1, },
	{ .name = "ifx:blue:internet", .gpio = 5, .active_low = 1, },
	{ .name = "ifx:red:power", .gpio = 6, .active_low = 1, },
	{ .name = "ifx:yello:wps", .gpio = 7, .active_low = 1, },
	{ .name = "ifx:red:wps", .gpio = 9, .active_low = 1, },
/*
	{ .name = "ifx10", .gpio = 10, .active_low = 1, },
	{ .name = "ifx11", .gpio = 11, .active_low = 1, },
	{ .name = "ifx12", .gpio = 12, .active_low = 1, },
	{ .name = "ifx13", .gpio = 13, .active_low = 1, },
	{ .name = "ifx14", .gpio = 14, .active_low = 1, },
	{ .name = "ifx15", .gpio = 15, .active_low = 1, },
	{ .name = "ifx16", .gpio = 16, .active_low = 1, },
	{ .name = "ifx17", .gpio = 17, .active_low = 1, },
	{ .name = "ifx18", .gpio = 18, .active_low = 1, },
	{ .name = "ifx19", .gpio = 19, .active_low = 1, },
	{ .name = "ifx20", .gpio = 20, .active_low = 1, },
	{ .name = "ifx21", .gpio = 21, .active_low = 1, },
	{ .name = "ifx22", .gpio = 22, .active_low = 1, },
	{ .name = "ifx23", .gpio = 23, .active_low = 1, },
	{ .name = "ifx24", .gpio = 24, .active_low = 1, },
	{ .name = "ifx25", .gpio = 25, .active_low = 1, },
	{ .name = "ifx26", .gpio = 26, .active_low = 1, },
	{ .name = "ifx27", .gpio = 27, .active_low = 1, },
	{ .name = "ifx28", .gpio = 28, .active_low = 1, },
	{ .name = "ifx29", .gpio = 29, .active_low = 1, },
	{ .name = "ifx30", .gpio = 30, .active_low = 1, },
	{ .name = "ifx31", .gpio = 31, .active_low = 1, },
*/
	{ .name = "ifx:blue:voip", .gpio = 32, .active_low = 1, },
	{ .name = "ifx:blue:fxs1", .gpio = 33, .active_low = 1, },
	{ .name = "ifx:blue:fxs2", .gpio = 34, .active_low = 1, },
	{ .name = "ifx:blue:fxo", .gpio = 35, .active_low = 1, },
	{ .name = "ifx:blue:voice", .gpio = 36, .active_low = 1, },
	{ .name = "ifx:blue:usb", .gpio = 37, .active_low = 1, },
	{ .name = "ifx:blue:wlan", .gpio = 38, .active_low = 1, },
/*	{ .name = "ifx39", .gpio = 39, .active_low = 1, },
	{ .name = "ifx40", .gpio = 40, .active_low = 1, },
	{ .name = "ifx41", .gpio = 41, .active_low = 1, },
	{ .name = "ifx42", .gpio = 42, .active_low = 1, },
	{ .name = "ifx43", .gpio = 43, .active_low = 1, },
	{ .name = "ifx44", .gpio = 44, .active_low = 1, },
	{ .name = "ifx45", .gpio = 45, .active_low = 1, },
	{ .name = "ifx46", .gpio = 46, .active_low = 1, },
	{ .name = "ifx47", .gpio = 47, .active_low = 1, },
*/
};

static struct gpio_button
arv452_gpio_buttons[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= BTN_0,
		.threshold	= 3,
		.gpio		= ARV452_GPIO_BUTTON_RESET,
		.active_low	= 1,
	}
};

static struct resource arv452_ebu_resource =
{
	.name	= "ebu-gpio",
	.start	= ARV452_EBU_GPIO_START,
	.end	= ARV452_EBU_GPIO_START + ARV452_EBU_GPIO_SIZE - 1,
	.flags	= IORESOURCE_MEM,
};

static void __init
arv452_init(void)
{
	unsigned char mac[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	ifxmips_find_brn_mac(mac);

	ifxmips_register_gpio();

	danube_register_ebu_gpio(&arv452_ebu_resource, ARV452_LATCH_SWITCH);

	ifxmips_register_mtd(&arv452_flash_data);

	danube_register_pci(PCI_CLOCK_EXT, 0);

	ifxmips_register_wdt();

	ifxmips_register_gpio_leds(arv452_leds_gpio, ARRAY_SIZE(arv452_leds_gpio));

	danube_register_ethernet(mac);

	danube_register_usb();
}

MIPS_MACHINE(IFXMIPS_MACH_ARV452,
			"ARV452",
			"Airties WAV-281, Arcor A800",
		     arv452_init);
