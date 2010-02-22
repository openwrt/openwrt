/*
 * arch/ubicom32/mach-common/vdc_tio.c
 *   Generic initialization for VDC
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

#include <linux/platform_device.h>
#include <linux/types.h>

#include <asm/devtree.h>
#include <asm/vdc_tio.h>

/*
 * Resources that this driver uses
 */
static struct resource vdc_tio_resources[] = {
	/*
	 * Send IRQ
	 */
	[0] = {
		/*
		 * The init routine will query the devtree and fill this in
		 */
		.flags	= IORESOURCE_IRQ,
	},

	/*
	 * Receive IRQ (optional)
	 */
	[1] = {
		/*
		 * The init routine will query the devtree and fill this in
		 */
		.flags	= IORESOURCE_IRQ,
	},

	/*
	 * Memory Mapped Registers
	 */
	[2] = {
		/*
		 * The init routine will query the devtree and fill this in
		 */
		.flags	= IORESOURCE_MEM,
	},
};

/*
 * The platform_device structure which is passed to the driver
 */
static struct platform_device vdc_tio_platform_device = {
	.name		= "ubicom32fb",
	.id		= -1,
	.resource	= vdc_tio_resources,
	.num_resources	= ARRAY_SIZE(vdc_tio_resources),
};

/*
 * vdc_tio_init
 *	Checks the device tree and instantiates the driver if found
 */
void __init vdc_tio_init(void)
{
	/*
	 * Check the device tree for the vdc_tio
	 */
	struct vdc_tio_node *vdc_node =
		(struct vdc_tio_node *)devtree_find_node("vdctio");
	if (!vdc_node) {
		printk(KERN_WARNING "No vdc_tio found\n");
		return;
	}

	/*
	 * Fill in the resources and platform data from devtree information
	 */
	vdc_tio_resources[0].start = vdc_node->dn.sendirq;
	vdc_tio_resources[1].start = vdc_node->dn.recvirq;
	vdc_tio_resources[2].start = (u32_t)vdc_node->regs;
	vdc_tio_resources[2].end = (u32_t)vdc_node->regs +
		sizeof(struct vdc_tio_vp_regs);

	/*
	 * Try to get the device registered
	 */
	if (platform_device_register(&vdc_tio_platform_device) < 0) {
		printk(KERN_WARNING "VDC failed to register\n");
	}
}
