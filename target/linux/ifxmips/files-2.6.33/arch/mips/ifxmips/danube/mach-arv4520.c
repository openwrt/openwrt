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
#include <ifxmips.h>
#include <ifxmips_prom.h>

#include "arcaydian.h"
#include "devices.h"

#define ARV452_EBU_GPIO_START	0x14000000
#define ARV452_EBU_GPIO_SIZE	0x00001000

#define ARV452_GPIO_BUTTON_RESET		14
#define ARV452_BUTTONS_POLL_INTERVAL	20

#define ARV452_LATCH_SWITCH				(1 << 10)

#ifdef CONFIG_MTD_PARTITIONS
static struct mtd_partition arv4520_partitions[] =
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

static struct physmap_flash_data arv4520_flash_data = {
#ifdef CONFIG_MTD_PARTITIONS
	.nr_parts	= ARRAY_SIZE(arv4520_partitions),
	.parts		= arv4520_partitions,
#endif
};

static struct gpio_led
arv4520_leds_gpio[] __initdata = {
	{ .name = "ifx:blue:power", .gpio = 3, .active_low = 1, },
	{ .name = "ifx:blue:adsl", .gpio = 4, .active_low = 1, },
	{ .name = "ifx:blue:internet", .gpio = 5, .active_low = 1, },
	{ .name = "ifx:red:power", .gpio = 6, .active_low = 1, },
	{ .name = "ifx:yello:wps", .gpio = 7, .active_low = 1, },
	{ .name = "ifx:red:wps", .gpio = 9, .active_low = 1, },
	{ .name = "ifx:blue:voip", .gpio = 32, .active_low = 1, },
	{ .name = "ifx:blue:fxs1", .gpio = 33, .active_low = 1, },
	{ .name = "ifx:blue:fxs2", .gpio = 34, .active_low = 1, },
	{ .name = "ifx:blue:fxo", .gpio = 35, .active_low = 1, },
	{ .name = "ifx:blue:voice", .gpio = 36, .active_low = 1, },
	{ .name = "ifx:blue:usb", .gpio = 37, .active_low = 1, },
	{ .name = "ifx:blue:wlan", .gpio = 38, .active_low = 1, },
};

static struct gpio_button
arv4520_gpio_buttons[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= BTN_0,
		.threshold	= 3,
		.gpio		= ARV452_GPIO_BUTTON_RESET,
		.active_low	= 1,
	}
};

static struct resource arv4520_ebu_resource =
{
	.name	= "ebu-gpio",
	.start	= ARV452_EBU_GPIO_START,
	.end	= ARV452_EBU_GPIO_START + ARV452_EBU_GPIO_SIZE - 1,
	.flags	= IORESOURCE_MEM,
};

static void __init
arv4520_init(void)
{
	unsigned char mac[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

	ifxmix_detect_brn_block();

	ifxmips_find_brn_mac(mac);

	ifxmips_register_gpio();

	danube_register_ebu_gpio(&arv4520_ebu_resource, ARV452_LATCH_SWITCH);

	ifxmips_register_mtd(&arv4520_flash_data);

	danube_register_pci(PCI_CLOCK_EXT, 0);

	ifxmips_register_wdt();

	ifxmips_register_gpio_leds(arv4520_leds_gpio, ARRAY_SIZE(arv4520_leds_gpio));

	danube_register_ethernet(mac, REV_MII_MODE);

	danube_register_usb();
}

MIPS_MACHINE(IFXMIPS_MACH_ARV4520,
			"ARV4520",
			"Airties WAV-281, Arcor A800",
			arv4520_init);
