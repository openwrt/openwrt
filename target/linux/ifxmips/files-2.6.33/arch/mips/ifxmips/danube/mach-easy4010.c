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

#include "devices.h"

extern unsigned char ifxmips_ethaddr[6];

#ifdef CONFIG_MTD_PARTITIONS
static struct mtd_partition easy4010_partitions[] =
{
	{
		.name	= "uboot",
		.offset	= 0x0,
		.size	= 0x40000,
	},
	{
		.name	= "uboot_env",
		.offset	= 0x40000,
		.size	= 0x10000,
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
	}
};
#endif

static struct physmap_flash_data easy4010_flash_data = {
#ifdef CONFIG_MTD_PARTITIONS
	.nr_parts	= ARRAY_SIZE(easy4010_partitions),
	.parts		= easy4010_partitions,
#endif
};

static struct gpio_led easy4010_leds[] = {
	{ .name = "ifx:green:test0", .gpio = 0,},
	{ .name = "ifx:green:test1", .gpio = 1,},
	{ .name = "ifx:green:test2", .gpio = 2,},
	{ .name = "ifx:green:test3", .gpio = 3,},
};

static void __init
easy4010_init(void)
{
	ifxmips_register_gpio();

	ifxmips_register_gpio_dev();

	ifxmips_register_mtd(&easy4010_flash_data);

	ifxmips_register_leds(easy4010_leds, ARRAY_SIZE(easy4010_leds));

	ifxmips_register_wdt();

	danube_register_ethernet(ifxmips_ethaddr, REV_MII_MODE);

	danube_register_usb();
}

MIPS_MACHINE(IFXMIPS_MACH_EASY4010,
			"EASY4010",
			"Lantiq Twinpass Eval Board",
			easy4010_init);
