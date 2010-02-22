/*
 * arch/ubicom32/mach-ip5k/board-ip5160dev.c
 *   Platform initialization for ip5160dev board.
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
#ifdef CONFIG_SERIAL_UBI32_SERDES
#include <asm/ubicom32suart.h>
#endif

/*
 * Factory Default Button on the board at PXn
 * TODO: This is just a placeholder and it needs to include proper header files
 */
struct ubicom32fdb_platform_data {
	int		fdb_gpio;
	bool		fdb_polarity;
};

static struct ubicom32fdb_platform_data ip5160dev_fdb_data = {
	.fdb_gpio		= 0,
	.fdb_polarity		= true,
};

static struct platform_device ip5160dev_fdb_device = {
	.name	= "ubicom32fdb",
	.id	= -1,
	.dev	= {
		.platform_data = &ip5160dev_fdb_data,
	},
};

#ifdef CONFIG_SERIAL_UBI32_SERDES
static struct resource ip5160dev_ubicom32_suart_resources[] = {
        {
		.start	= RD,
		.end	= RD,
		.flags	= IORESOURCE_MEM,
        },
        {
		.start	= PORT_OTHER_INT(RD),
		.end	= PORT_OTHER_INT(RD),
		.flags	= IORESOURCE_IRQ,
        },
        {
		.start	= 240000000,
		.end	= 240000000,
		.flags	= UBICOM32_SUART_IORESOURCE_CLOCK,
        },
};

static struct platform_device ip5160dev_ubicom32_suart_device = {
	.name		= "ubicom32suart",
	.id		= -1,
        .num_resources  = ARRAY_SIZE(ip5160dev_ubicom32_suart_resources),
        .resource       = ip5160dev_ubicom32_suart_resources,
};
#endif

/*
 * List of all devices in our system
 */
static struct platform_device *ip5160dev_devices[] __initdata = {
#ifdef CONFIG_SERIAL_UBI32_SERDES
	&ip5160dev_ubicom32_suart_device,
#endif
	&ip5160dev_fdb_device,
};

/*
 * ip5160dev_init
 *	Called to add the devices which we have on this board
 */
static int __init ip5160dev_init(void)
{
	ubi_gpio_init();
	printk(KERN_INFO "%s: registering device resources\n", __FUNCTION__);
	platform_add_devices(ip5160dev_devices, ARRAY_SIZE(ip5160dev_devices));
	return 0;
}

arch_initcall(ip5160dev_init);
