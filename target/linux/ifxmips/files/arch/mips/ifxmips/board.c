/*
 *   arch/mips/ifxmips/board.c
 *
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
 *
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
#include <asm/ifxmips/ifxmips.h>

#define MAX_IFXMIPS_DEVS		7

#define BOARD_DANUBE			"Danube"
#define BOARD_DANUBE_CHIPID		0x10129083

#define BOARD_TWINPASS			"Twinpass"
#define BOARD_TWINPASS_CHIPID	0x3012D083

#define BOARD_DANUBE			"Danube"

static unsigned int chiprev;

static struct platform_device *ifxmips_devs[MAX_IFXMIPS_DEVS];

static struct platform_device ifxmips_led[] =
{
	{
		.id = 0,
		.name = "ifxmips_led",
	},
};

static struct platform_device ifxmips_gpio[] =
{
	{
		.id = 0,
		.name = "ifxmips_gpio",
	},
};

static struct platform_device ifxmips_mii[] =
{
	{
		.id = 0,
		.name = "ifxmips_mii0",
	},
};

static struct platform_device ifxmips_wdt[] =
{
	{
		.id = 0,
		.name = "ifxmips_wdt",
	},
};

static struct physmap_flash_data ifxmips_mtd_data = {
	.width    = 2,
};

static struct resource ifxmips_mtd_resource = {
	.start  = IFXMIPS_FLASH_START,
	.end    = IFXMIPS_FLASH_START + IFXMIPS_FLASH_MAX - 1,
	.flags  = IORESOURCE_MEM,
};

static struct platform_device ifxmips_mtd[] =
{
	{
		.id = 0,
		.name = "ifxmips_mtd",
		.dev = {
			.platform_data = &ifxmips_mtd_data,
		},
		.num_resources  = 1,
		.resource   = &ifxmips_mtd_resource,
	},
};

#ifdef CONFIG_GPIO_DEVICE
static struct resource ifxmips_gpio_dev_resources[] = {
	{
		.name = "gpio",
		.flags  = 0,
		.start = (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5) | (1 << 8) | (1 << 9) | (1 << 12),
		.end = (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5) | (1 << 8) | (1 << 9) | (1 << 12),
	},
};

static struct platform_device ifxmips_gpio_dev[] = {
	{
		.name     = "GPIODEV",
		.id     = -1,
		.num_resources    = ARRAY_SIZE(ifxmips_gpio_dev_resources),
		.resource   = ifxmips_gpio_dev_resources,
	}
};
#endif

const char*
get_system_type (void)
{
	chiprev = ifxmips_r32(IFXMIPS_MPS_CHIPID);
	switch(chiprev)
	{
	case BOARD_DANUBE_CHIPID:
		return BOARD_DANUBE;

	case BOARD_TWINPASS_CHIPID:
		return BOARD_TWINPASS;
	}

	return BOARD_SYSTEM_TYPE;
}

int __init ifxmips_init_devices(void)
{
	/*
	*	This is where we detect what chip we are running on
	*	Currently we support 3 chips
	*	1.) Danube
	*	2.) Twinpass (Danube without dsl phy)
	*/

	int dev = 0;

	/* the following devices are generic for all targets */
	ifxmips_devs[dev++] = ifxmips_led;
	ifxmips_devs[dev++] = ifxmips_gpio;
	ifxmips_devs[dev++] = ifxmips_mii;
	ifxmips_devs[dev++] = ifxmips_mtd;
	ifxmips_devs[dev++] = ifxmips_wdt;
#ifdef CONFIG_GPIO_DEVICE
	ifxmips_devs[dev++] = ifxmips_gpio_dev;
#endif
	return platform_add_devices(ifxmips_devs, dev);
}

arch_initcall(ifxmips_init_devices);
