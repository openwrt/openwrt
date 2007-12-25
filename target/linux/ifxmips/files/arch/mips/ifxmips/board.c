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

#define MAX_IFXMIPS_DEVS		5

#define BOARD_DANUBE			"Danube"
#define BOARD_DANUBE_CHIPID		0x10129083

#define BOARD_TWINPASS			"Twinpass"

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

const char*
get_system_type (void)
{
	chiprev = readl(IFXMIPS_MPS_CHIPID);
	switch(chiprev)
	{
	case BOARD_DANUBE_CHIPID:
		return BOARD_DANUBE;
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
	*	3.) KDbg74 (used for debuging)
	*/

	int dev = 0;

	/* the following devices are generic for all targets */

	ifxmips_devs[dev++] = ifxmips_led;
	ifxmips_devs[dev++] = ifxmips_gpio;
	ifxmips_devs[dev++] = ifxmips_mii;

	return platform_add_devices(ifxmips_devs, dev);
}

arch_initcall(ifxmips_init_devices);
