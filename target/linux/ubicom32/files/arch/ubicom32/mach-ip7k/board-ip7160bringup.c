/*
 * arch/ubicom32/mach-ip7k/board-ip7160bringup.c
 *   Support for the IP7160 bringup board.
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
#include <linux/leds.h>
#include <linux/delay.h>
#include <linux/input.h>

#include <asm/board.h>
#include <asm/machdep.h>
#include <asm/ubicom32input.h>

#ifdef CONFIG_SERIAL_UBI32_SERDES
#include <asm/ubicom32suart.h>
#endif

/*
 * Use ubicom32input driver to monitor the various pushbuttons on this board.
 *
 * WPS			PD5
 * FACT_DEFAULT		PD6
 *
 * TODO: pick some ubicom understood EV_xxx define for WPS and Fact Default
 */
static struct ubicom32input_button ip7160bringup_ubicom32input_buttons[] = {
	{
		.type		= EV_KEY,
		.code		= KEY_FN_F1,
		.gpio		= GPIO_RD_5,
		.desc		= "WPS",
		.active_low	= 1,
	},
	{
		.type		= EV_KEY,
		.code		= KEY_FN_F2,
		.gpio		= GPIO_RD_6,
		.desc		= "Factory Default",
		.active_low	= 1,
	},
};

static struct ubicom32input_platform_data ip7160bringup_ubicom32input_data = {
	.buttons	= ip7160bringup_ubicom32input_buttons,
	.nbuttons	= ARRAY_SIZE(ip7160bringup_ubicom32input_buttons),
};

static struct platform_device ip7160bringup_ubicom32input_device = {
	.name	= "ubicom32input",
	.id	= -1,
	.dev	= {
		.platform_data = &ip7160bringup_ubicom32input_data,
	},
};

#ifdef CONFIG_SERIAL_UBI32_SERDES
static struct resource ip7160bringup_ubicom32_suart_resources[] = {
	{
		.start	= RE,
		.end	= RE,
		.flags	= IORESOURCE_MEM,
	},
	{
		.start	= PORT_OTHER_INT(RE),
		.end	= PORT_OTHER_INT(RE),
		.flags	= IORESOURCE_IRQ,
	},
	{
		.start	= 250000000,
		.end	= 250000000,
		.flags	= UBICOM32_SUART_IORESOURCE_CLOCK,
	},
};

static struct platform_device ip7160bringup_ubicom32_suart_device = {
	.name		= "ubicom32suart",
	.id		= -1,
	.num_resources  = ARRAY_SIZE(ip7160bringup_ubicom32_suart_resources),
	.resource	= ip7160bringup_ubicom32_suart_resources,
};
#endif

/*
 * List of all devices in our system
 */
static struct platform_device *ip7160bringup_devices[] __initdata = {
#ifdef CONFIG_SERIAL_UBI32_SERDES
	&ip7160bringup_ubicom32_suart_device,
#endif
	&ip7160bringup_ubicom32input_device,
};

/*
 * ip7160bringup_init
 *	Called to add the devices which we have on this board
 */
static int __init ip7160bringup_init(void)
{
	board_init();

	ubi_gpio_init();

	printk(KERN_INFO "%s: registering device resources\n", __FUNCTION__);
	platform_add_devices(ip7160bringup_devices, ARRAY_SIZE(ip7160bringup_devices));

	return 0;
}

arch_initcall(ip7160bringup_init);
