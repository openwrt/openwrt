/*
 * arch/ubicom32/mach-common/ip5k_usb.c
 *   Ubicom32 architecture usb support.
 *
 * (C) Copyright 2009, Ubicom, Inc.
 * Copyright (C) 2007 MontaVista Software, Inc. <source@mvista.com>
 * Author: Kevin Hilman
 *
 * This file is part of the Ubicom32 Linux Kernel Port.
 *
 * The Ubicom32 Linux Kernel Port is free software: you can
 * redistribute it and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation, either
 * version 2 of the License, or (at your option) any later version.
 *
 * The Ubicom32 Linux Kernel Port is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
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

#include <linux/types.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/dma-mapping.h>
#include <linux/usb/musb.h>
#include <asm/devtree.h>
#include <asm/ip5000.h>
#include "usb_tio.h"

struct usbtionode *unode = NULL;

static struct resource usb_resources[] = {
	[0] = {
		.start	= RJ + 0x800,
		.end	= RJ + 0x1000,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {	/* general IRQ */
		.start	= 1, /* this is a dummy value, the real irq number is passed from kernel_setup_param */
		.flags	= IORESOURCE_IRQ,
	},
};


static struct musb_hdrc_eps_bits musb_eps[] = {
	{ "ep1_tx", 4, },
	{ "ep1_rx", 4, },
	{ "ep2_tx", 10, },
	{ "ep2_rx", 10, },
	{ "ep3_tx", 9, },
	{ "ep3_rx", 9, },
	{ "ep4_tx", 9, },
	{ "ep4_rx", 9, },
	{ "ep5_tx", 6, },
	{ "ep5_rx", 6, },
};

static struct musb_hdrc_config musb_config = {
	.multipoint     = true,
	.dyn_fifo       = false,
	.soft_con       = true,
	.dma            = false,

	.num_eps        = 6,
	.dma_channels   = 0,
	.ram_bits       = 0,
	.eps_bits       = musb_eps,
};

static struct musb_hdrc_platform_data usb_data = {
#ifdef CONFIG_USB_MUSB_OTG
	.mode		= MUSB_OTG,
#else
#ifdef CONFIG_USB_MUSB_HDRC_HCD
	.mode		= MUSB_HOST,
#else
#ifdef CONFIG_USB_GADGET_MUSB_HDRC
	.mode		= MUSB_PERIPHERAL,
#endif
#endif
#endif
	.clock		= NULL,
	.set_clock	= NULL,
	.config		= &musb_config,
};

static struct platform_device musb_device = {
	.name		= "musb_hdrc",
	.id		= 0,
	.dev = {
		.platform_data		= &usb_data,
		.dma_mask		= NULL,
		.coherent_dma_mask	= 0,
	},
	.resource	= usb_resources,
	.num_resources	= ARRAY_SIZE(usb_resources),
};

struct usbtio_node *usb_node = NULL;
void ubi32_usb_init(void)
{
	/*
	 * See if the usbtio is in the device tree.
	 */
	usb_node = (struct usbtio_node *)devtree_find_node("usbtio");
	if (!usb_node) {
		printk(KERN_WARNING "usb init failed\n");
		return;
	}

	usb_resources[1].start = usb_node->dn.recvirq;
	if (platform_device_register(&musb_device) < 0) {
		printk(KERN_ERR "Unable to register HS-USB (MUSB) device\n");
		return;
	}
}

void ubi32_usb_int_clr(void)
{
        UBICOM32_IO_PORT(RJ)->int_clr = (1 << 3);
}
