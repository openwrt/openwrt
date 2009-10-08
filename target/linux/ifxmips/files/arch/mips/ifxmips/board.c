/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 *
 *   Copyright (C) 2007 John Crispin <blogic@openwrt.org>
 */

#include <linux/autoconf.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/string.h>
#include <linux/mtd/physmap.h>
#include <linux/kernel.h>
#include <linux/reboot.h>
#include <linux/platform_device.h>
#include <linux/leds.h>
#include <linux/etherdevice.h>
#include <linux/reboot.h>
#include <linux/time.h>
#include <linux/io.h>
#include <linux/gpio.h>
#include <asm/bootinfo.h>
#include <asm/irq.h>
#include <asm/ifxmips/ifxmips.h>
#include <asm/ifxmips/ifxmips_irq.h>

#define MAX_BOARD_NAME_LEN		32
#define MAX_IFXMIPS_DEVS		9

#define SYSTEM_DANUBE			"Danube"
#define SYSTEM_DANUBE_CHIPID1	0x00129083
#define SYSTEM_DANUBE_CHIPID2	0x0012B083

#define SYSTEM_TWINPASS			"Twinpass"
#define SYSTEM_TWINPASS_CHIPID	0x0012D083

enum {
	EASY50712,
	EASY4010,
	ARV4519,
};

extern int ifxmips_pci_external_clock;

static unsigned int chiprev;
static int cmdline_mac;
char board_name[MAX_BOARD_NAME_LEN + 1] = { 0 };

struct ifxmips_board {
	int type;
	char name[32];
	unsigned int system_type;
	struct platform_device **devs;
	struct resource reset_resource;
	struct resource gpiodev_resource;
	struct gpio_led *ifxmips_leds;
	struct gpio_led *gpio_leds;
	int pci_external_clock;
	int num_devs;
};

DEFINE_SPINLOCK(ebu_lock);
EXPORT_SYMBOL_GPL(ebu_lock);

static unsigned char ifxmips_ethaddr[6];
static int ifxmips_brn;

static struct gpio_led_platform_data ifxmips_led_data;

static struct platform_device ifxmips_led = {
	.id = 0,
	.name = "ifxmips_led",
	.dev = {
		.platform_data = (void *) &ifxmips_led_data,
	}
};

static struct platform_device ifxmips_gpio = {
	.id = 0,
	.name = "ifxmips_gpio",
	.num_resources = 1,
};

static struct platform_device ifxmips_mii = {
	.id = 0,
	.name = "ifxmips_mii0",
	.dev = {
		.platform_data = ifxmips_ethaddr,
	}
};

static struct platform_device ifxmips_wdt = {
	.id = 0,
	.name = "ifxmips_wdt",
};

static struct resource ifxmips_mtd_resource = {
	.start  = IFXMIPS_FLASH_START,
	.end    = IFXMIPS_FLASH_START + IFXMIPS_FLASH_MAX - 1,
	.flags  = IORESOURCE_MEM,
};

static struct platform_device ifxmips_mtd = {
	.id = 0,
	.name = "ifxmips_mtd",
	.num_resources  = 1,
	.resource   = &ifxmips_mtd_resource,
};

static struct platform_device ifxmips_gpio_dev = {
	.name     = "GPIODEV",
	.id     = -1,
	.num_resources    =	1,
};

#ifdef CONFIG_LEDS_GPIO
static struct gpio_led arv4519_gpio_leds[] = {
	{ .name = "ifx:green:power", .gpio = 3, .active_low = 1, },
	{ .name = "ifx:red:power", .gpio = 7, .active_low = 1, },
	{ .name = "ifx:green:adsl", .gpio = 4, .active_low = 1, },
	{ .name = "ifx:green:internet", .gpio = 5, .active_low = 1, },
	{ .name = "ifx:red:internet", .gpio = 8, .active_low = 1, },
	{ .name = "ifx:green:wlan", .gpio = 6, .active_low = 1, },
	{ .name = "ifx:green:usbpwr", .gpio = 14, .active_low = 1, },
	{ .name = "ifx:green:usb", .gpio = 19, .active_low = 1, },
};

static struct gpio_led_platform_data ifxmips_gpio_led_data;

static struct platform_device ifxmips_gpio_leds = {
	.name = "leds-gpio",
	.id = -1,
	.dev = {
		.platform_data = (void *) &ifxmips_gpio_led_data,
	}
};
#endif

static struct resource dwc_usb_res[] = {
	{
		.name = "dwc_usb_membase",
		.flags = IORESOURCE_MEM,
		.start = 0x1E101000,
		.end = 0x1E101FFF
	},
	{
		.name = "dwc_usb_irq",
		.flags = IORESOURCE_IRQ,
		.start = IFXMIPS_USB_INT,
	}
};

static struct platform_device dwc_usb =
{
	.id = 0,
	.name = "dwc_usb",
	.resource = dwc_usb_res,
	.num_resources = ARRAY_SIZE(dwc_usb_res),
};

struct platform_device *easy50712_devs[] = {
	&ifxmips_led, &ifxmips_gpio, &ifxmips_mii,
	&ifxmips_mtd, &ifxmips_wdt, &ifxmips_gpio_dev, &dwc_usb
};

struct platform_device *easy4010_devs[] = {
	&ifxmips_led, &ifxmips_gpio, &ifxmips_mii,
	&ifxmips_mtd, &ifxmips_wdt, &ifxmips_gpio_dev, &dwc_usb
};

struct platform_device *arv5419_devs[] = {
	&ifxmips_gpio, &ifxmips_mii, &ifxmips_mtd,
	&ifxmips_gpio_dev, &ifxmips_wdt, &dwc_usb,
#ifdef CONFIG_LEDS_GPIO
	&ifxmips_gpio_leds,
#endif
};

static struct gpio_led easy50712_leds[] = {
	{ .name = "ifx:green:test0", .gpio = 0,},
	{ .name = "ifx:green:test1", .gpio = 1,},
	{ .name = "ifx:green:test2", .gpio = 2,},
	{ .name = "ifx:green:test3", .gpio = 3,},
};

static struct gpio_led easy4010_leds[] = {
	{ .name = "ifx:green:test0", .gpio = 0,},
	{ .name = "ifx:green:test1", .gpio = 1,},
	{ .name = "ifx:green:test2", .gpio = 2,},
	{ .name = "ifx:green:test3", .gpio = 3,},
};

static struct ifxmips_board boards[] = {
	{
		/* infineon eval kit */
		.type = EASY50712,
		.name = "EASY50712",
		.system_type = SYSTEM_DANUBE_CHIPID1,
		.devs = easy50712_devs,
		.reset_resource = {.name = "reset", .start = 1, .end = 15,},
		.gpiodev_resource = { .name = "gpio",
			.start = (1 << 0) | (1 << 1),
			.end = (1 << 0) | (1 << 1)},
		.ifxmips_leds = easy50712_leds,
	}, {
		/* infineon eval kit */
		.type = EASY4010,
		.name = "EASY4010",
		.system_type = SYSTEM_TWINPASS_CHIPID,
		.devs = easy4010_devs,
		.reset_resource = {.name = "reset", .start = 1, .end = 15},
		.gpiodev_resource = { .name = "gpio",
			.start = (1 << 0) | (1 << 1),
			.end = (1 << 0) | (1 << 1)},
		.ifxmips_leds = easy4010_leds,
	}, {
		/* arcaydian annex-a board used by thompson, airties, ... */
		.type = ARV4519,
		.name = "ARV4519",
		.system_type = SYSTEM_DANUBE_CHIPID2,
		.devs = arv5419_devs,
		.reset_resource = {.name = "reset", .start = 1, .end = 14},
		.pci_external_clock = 1,
		.gpio_leds = arv4519_gpio_leds,
	},
};

const char *get_system_type(void)
{
	chiprev = (ifxmips_r32(IFXMIPS_MPS_CHIPID) & 0x0FFFFFFF);

	switch (chiprev) {
	case SYSTEM_DANUBE_CHIPID1:
	case SYSTEM_DANUBE_CHIPID2:
		return SYSTEM_DANUBE;

	case SYSTEM_TWINPASS_CHIPID:
		return SYSTEM_TWINPASS;
	}

	return BOARD_SYSTEM_TYPE;
}

static int __init ifxmips_set_board_type(char *str)
{
	str = strchr(str, '=');
	if (!str)
		goto out;
	str++;
	if (strlen(str) > MAX_BOARD_NAME_LEN)
		goto out;
	strncpy(board_name, str, MAX_BOARD_NAME_LEN);
	printk(KERN_INFO "bootloader told us, that this is a %s board\n",
		board_name);
out:
	return 1;
}
__setup("ifxmips_board", ifxmips_set_board_type);

static int __init ifxmips_set_ethaddr(char *str)
{
#define IS_HEX(x) \
	(((x >= '0' && x <= '9') || (x >= 'a' && x <= 'f') \
		|| (x >= 'A' && x <= 'F')) ? (1) : (0))
	int i;
	str = strchr(str, '=');
	if (!str)
		goto out;
	str++;
	if (strlen(str) != 17)
		goto out;
	for (i = 0; i < 6; i++) {
		if (!IS_HEX(str[3 * i]) || !IS_HEX(str[(3 * i) + 1]))
			goto out;
		if ((i != 5) && (str[(3 * i) + 2] != ':'))
			goto out;
		ifxmips_ethaddr[i] = simple_strtoul(&str[3 * i], NULL, 16);
	}
	if (is_valid_ether_addr(ifxmips_ethaddr))
		cmdline_mac = 1;
out:
	return 1;
}
__setup("ethaddr", ifxmips_set_ethaddr);

int ifxmips_find_brn_block(void)
{
	unsigned char temp[8];
	memcpy_fromio(temp,
		(void *)KSEG1ADDR(IFXMIPS_FLASH_START + 0x800000 - 0x10000), 8);
	if (memcmp(temp, "BRN-BOOT", 8) == 0) {
		if (!cmdline_mac)
			memcpy_fromio(ifxmips_ethaddr,
				(void *)KSEG1ADDR(IFXMIPS_FLASH_START +
					0x800000 - 0x10000 + 0x16), 6);
		if (is_valid_ether_addr(ifxmips_ethaddr))
			cmdline_mac = 1;
		return 1;
	} else {
		return 0;
	}
}

int ifxmips_has_brn_block(void)
{
	return ifxmips_brn;
}
EXPORT_SYMBOL(ifxmips_has_brn_block);

struct ifxmips_board *ifxmips_find_board(void)
{
	int i;
	if (!*board_name)
		return 0;
	for (i = 0; i < ARRAY_SIZE(boards); i++)
		if ((boards[i].system_type == chiprev) &&
		    (!strcmp(boards[i].name, board_name)))
			return &boards[i];
	return 0;
}

int __init ifxmips_init_devices(void)
{
	struct ifxmips_board *board = ifxmips_find_board();

	chiprev = (ifxmips_r32(IFXMIPS_MPS_CHIPID) & 0x0FFFFFFF);
	ifxmips_brn = ifxmips_find_brn_block();

	if (!cmdline_mac)
		random_ether_addr(ifxmips_ethaddr);

	if (!board) {
		switch (chiprev) {
		case SYSTEM_DANUBE_CHIPID1:
		case SYSTEM_DANUBE_CHIPID2:
		default:
			board = &boards[0];
			break;
		case SYSTEM_TWINPASS_CHIPID:
			board = &boards[1];
			break;
		}
	}

	switch (board->type) {
	case EASY50712:
		board->num_devs = ARRAY_SIZE(easy50712_devs);
		ifxmips_led_data.num_leds = ARRAY_SIZE(easy50712_leds);
		break;
	case EASY4010:
		board->num_devs = ARRAY_SIZE(easy4010_devs);
		ifxmips_led_data.num_leds = ARRAY_SIZE(easy4010_leds);
		break;
	case ARV4519:
		/* set some sane defaults for the gpios */
		gpio_set_value(3, 0);
		gpio_set_value(4, 0);
		gpio_set_value(5, 0);
		gpio_set_value(6, 0);
		gpio_set_value(7, 1);
		gpio_set_value(8, 1);
		gpio_set_value(19, 0);
		board->num_devs = ARRAY_SIZE(arv5419_devs);
#ifdef CONFIG_LEDS_GPIO
		ifxmips_gpio_led_data.num_leds = ARRAY_SIZE(arv4519_gpio_leds);
#endif
		break;
	}
#ifdef CONFIG_LEDS_GPIO
	ifxmips_gpio_led_data.leds = board->gpio_leds;
#endif
	ifxmips_led_data.leds = board->ifxmips_leds;

	printk(KERN_INFO "%s: adding %d devs\n",
		__func__, board->num_devs);

	ifxmips_gpio.resource = &board->reset_resource;
	ifxmips_gpio_dev.resource = &board->gpiodev_resource;
	if (board->pci_external_clock)
		ifxmips_pci_external_clock = 1;
	printk(KERN_INFO "using board definition %s\n", board->name);
	return platform_add_devices(board->devs, board->num_devs);
}

arch_initcall(ifxmips_init_devices);
