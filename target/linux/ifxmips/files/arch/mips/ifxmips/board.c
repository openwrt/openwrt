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
#include <asm/bootinfo.h>
#include <asm/reboot.h>
#include <asm/time.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <linux/etherdevice.h>
#include <asm/ifxmips/ifxmips.h>

#define MAX_BOARD_NAME_LEN		32
#define MAX_IFXMIPS_DEVS		9

#define SYSTEM_DANUBE			"Danube"
#define SYSTEM_DANUBE_CHIPID1	0x10129083
#define SYSTEM_DANUBE_CHIPID2	0x3012B083

#define SYSTEM_TWINPASS			"Twinpass"
#define SYSTEM_TWINPASS_CHIPID	0x3012D083

extern int ifxmips_pci_external_clock;

static unsigned int chiprev;
static int cmdline_mac = 0;
char board_name[MAX_BOARD_NAME_LEN + 1] = { 0 };

struct ifxmips_board {
	char name[32];
	unsigned int system_type;
	struct platform_device *devs[MAX_IFXMIPS_DEVS];
	struct resource reset_resource;
	struct resource gpiodev_resource;
	int pci_external_clock;
	int num_devs;
};

spinlock_t ebu_lock = SPIN_LOCK_UNLOCKED;
EXPORT_SYMBOL_GPL(ebu_lock);

static unsigned char ifxmips_mii_mac[6];
static int ifxmips_brn = 0;

static struct platform_device
ifxmips_led =
{
	.id = 0,
	.name = "ifxmips_led",
};

static struct platform_device
ifxmips_gpio =
{
	.id = 0,
	.name = "ifxmips_gpio",
	.num_resources = 1,
};

static struct platform_device
ifxmips_mii =
{
	.id = 0,
	.name = "ifxmips_mii0",
	.dev = {
		.platform_data = ifxmips_mii_mac,
	}
};

static struct platform_device
ifxmips_wdt =
{
	.id = 0,
	.name = "ifxmips_wdt",
};

static struct resource
ifxmips_mtd_resource = {
	.start  = IFXMIPS_FLASH_START,
	.end    = IFXMIPS_FLASH_START + IFXMIPS_FLASH_MAX - 1,
	.flags  = IORESOURCE_MEM,
};

static struct platform_device
ifxmips_mtd =
{
	.id = 0,
	.name = "ifxmips_mtd",
	.num_resources  = 1,
	.resource   = &ifxmips_mtd_resource,
};

static struct platform_device
ifxmips_gpio_dev = {
	.name     = "GPIODEV",
	.id     = -1,
	.num_resources    =	1,
};

const char*
get_system_type(void)
{
	chiprev = ifxmips_r32(IFXMIPS_MPS_CHIPID);
	switch(chiprev)
	{
	case SYSTEM_DANUBE_CHIPID1:
	case SYSTEM_DANUBE_CHIPID2:
		return SYSTEM_DANUBE;

	case SYSTEM_TWINPASS_CHIPID:
		return SYSTEM_TWINPASS;
	}

	return BOARD_SYSTEM_TYPE;
}

static int __init
ifxmips_set_board_type(char *str)
{
	str = strchr(str, '=');
	if(!str)
		goto out;
	str++;
	if(strlen(str) > MAX_BOARD_NAME_LEN)
		goto out;
	strncpy(board_name, str, MAX_BOARD_NAME_LEN);
	printk("bootloader told us, that this is a %s board\n", board_name);
out:
	return 1;
}
__setup("ifxmips_board", ifxmips_set_board_type);

#define IS_HEX(x) \
	(((x >='0' && x <= '9') || (x >='a' && x <= 'f') || (x >='A' && x <= 'F'))?(1):(0))

static int __init
ifxmips_set_mii0_mac(char *str)
{
	int i;
	str = strchr(str, '=');
	if(!str)
		goto out;
	str++;
	if(strlen(str) != 17)
		goto out;
	for(i = 0; i < 6; i++)
	{
		if(!IS_HEX(str[3 * i]) || !IS_HEX(str[(3 * i) + 1]))
			goto out;
		if((i != 5) && (str[(3 * i) + 2] != ':'))
			goto out;
		ifxmips_mii_mac[i] = simple_strtoul(&str[3 * i], NULL, 16);
	}
	if(is_valid_ether_addr(ifxmips_mii_mac))
		cmdline_mac = 1;
out:
	return 1;
}
__setup("mii0_mac", ifxmips_set_mii0_mac);


static struct ifxmips_board boards[] =
{
	{
		.name = "EASY50712",
		.system_type = SYSTEM_DANUBE_CHIPID1,
		.devs =
		{
			&ifxmips_led, &ifxmips_gpio, &ifxmips_mii,
			&ifxmips_mtd, &ifxmips_wdt, &ifxmips_gpio_dev,
		},
		.reset_resource =
		{
			.name = "reset",
			.start = 1,
			.end = 15,
		},
		.gpiodev_resource =
		{
			.name = "gpio",
			.start = (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3) |
					(1 << 4) | (1 << 5) | (1 << 8) | (1 << 9) | (1 << 12),
			.end = (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3) |
					(1 << 4) | (1 << 5) | (1 << 8) | (1 << 9) | (1 << 12),
		},
		.num_devs = 6,
	}, {
		.name = "EASY4010",
		.system_type = SYSTEM_TWINPASS_CHIPID,
		.devs =
		{
			&ifxmips_led, &ifxmips_gpio, &ifxmips_mii,
			&ifxmips_mtd, &ifxmips_wdt, &ifxmips_gpio_dev,
		},
		.reset_resource =
		{
			.name = "reset",
			.start = 1,
			.end = 15,
		},
		.gpiodev_resource =
		{
			.name = "gpio",
			.start = (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3) |
					(1 << 4) | (1 << 5) | (1 << 8) | (1 << 9) | (1 << 12),
			.end = (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3) |
					(1 << 4) | (1 << 5) | (1 << 8) | (1 << 9) | (1 << 12),
		},
		.num_devs = 6,
	}, {
		.name = "ARV4519",
		.system_type = SYSTEM_DANUBE_CHIPID2,
		.devs =
		{
			&ifxmips_led, &ifxmips_gpio, &ifxmips_mii,
			&ifxmips_mtd, &ifxmips_wdt, &ifxmips_gpio_dev,
		},
		.reset_resource =
		{
			.name = "reset",
			.start = 1,
			.end = 12,
		},
		.gpiodev_resource =
		{
			.name = "gpio",
			.start = (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3) |
					(1 << 4) | (1 << 5) | (1 << 8) | (1 << 9) | (1 << 12),
			.end = (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3) |
					(1 << 4) | (1 << 5) | (1 << 8) | (1 << 9) | (1 << 12),
		},
		.pci_external_clock = 1,
		.num_devs = 6,
	},
};

int
ifxmips_find_brn_block(void){
	unsigned char temp[0];
	memcpy_fromio(temp, (void*)KSEG1ADDR(IFXMIPS_FLASH_START + 0x800000 - 0x10000), 8);
	if(memcmp(temp, "BRN-BOOT", 8) == 0)
		return 1;
	else
		return 0;
}

int
ifxmips_has_brn_block(void)
{
	return ifxmips_brn;
}

struct ifxmips_board*
ifxmips_find_board(void)
{
	int i;
	if(!*board_name)
		return 0;
	for(i = 0; i < ARRAY_SIZE(boards); i++)
		if((boards[i].system_type == chiprev) && (!strcmp(boards[i].name, board_name)))
			return &boards[i];
	return 0;
}

int __init
ifxmips_init_devices(void)
{
	struct ifxmips_board *board = ifxmips_find_board();

	chiprev = ifxmips_r32(IFXMIPS_MPS_CHIPID);
	ifxmips_brn = ifxmips_find_brn_block();

	if(!cmdline_mac)
		random_ether_addr(ifxmips_mii_mac);

	if(!board)
	{
		switch(chiprev)
		{
		case SYSTEM_DANUBE_CHIPID1:
		case SYSTEM_DANUBE_CHIPID2:
			board = &boards[0];
			break;
		case SYSTEM_TWINPASS_CHIPID:
			board = &boards[1];
			break;
		}
	}
	ifxmips_gpio.resource = &board->reset_resource;
	ifxmips_gpio_dev.resource = &board->gpiodev_resource;
	if(board->pci_external_clock)
		ifxmips_pci_external_clock = 1;
	printk("using board definition %s\n", board->name);
	return platform_add_devices(board->devs, board->num_devs);
}

arch_initcall(ifxmips_init_devices);
