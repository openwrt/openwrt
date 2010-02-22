/*
 * arch/ubicom32/mach-ip5k/board-ip5160rgw.c
 *   Platform initialization for ip5160rgw board.
 *
 * (C) Copyright 2009, Ubicom, Inc.
 *
 * This file is part of the Ubicom32 Linux Kernel Port.
 *
 * The Ubicom32 Linux Kernel Port is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 *
 * The Ubicom32 Linux Kernel Port is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Ubicom32 Linux Kernel Port.  If not,
 * see <http://www.gnu.org/licenses/>.
 *
 * Ubicom32 implementation derived from (with many thanks):
 *   arch/m68knommu
 *   arch/blackfin
 *   arch/parisc
 */
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <asm/board.h>
#include <asm/machdep.h>

/*
 * Factory Default Button on the board at PXn
 * TODO: This is just a placeholder and it needs to include proper header files
 */
struct ubicom32fdb_platform_data {
	int		fdb_gpio;
	bool		fdb_polarity;
};

static struct ubicom32fdb_platform_data ip5160rgw_fdb_data = {
	.fdb_gpio		= 0,
	.fdb_polarity		= true,
};

static struct platform_device ip5160rgw_fdb_device = {
	.name	= "ubicom32fdb",
	.id	= -1,
	.dev	= {
		.platform_data = &ip5160rgw_fdb_data,
	},
};

/*
 * List of all devices in our system
 */
static struct platform_device *ip5160rgw_devices[] __initdata = {
	&ip5160rgw_fdb_device,
};

/*
 * ip5160rgw_init
 *	Called to add the devices which we have on this board
 */
static int __init ip5160rgw_init(void)
{
	ubi_gpio_init();
	printk(KERN_INFO "%s: registering device resources\n", __FUNCTION__);
	platform_add_devices(ip5160rgw_devices, ARRAY_SIZE(ip5160rgw_devices));
	return 0;
}

arch_initcall(ip5160rgw_init);
