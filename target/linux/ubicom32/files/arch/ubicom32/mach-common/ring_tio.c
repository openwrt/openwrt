/*
 * arch/ubicom32/mach-common/ring_tio.c
 *   Generic initialization for UIO Ubicom32 Ring
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
 */

#include <linux/platform_device.h>
#include <linux/types.h>

#include <asm/devtree.h>
#include <asm/ring_tio.h>

static const char *ring_tio_driver_name = "uio_ubicom32ring";

/*
 * The number of ring_tio's currently allocated, used for .id
 */
static int __initdata ring_tio_count;

/*
 * The maximum number of resources that the ring_tio will have.
 * Currently 3, a register space, and up to 2 interrupts.
 */
#define RING_TIO_MAX_RESOURCES	3

/*
 * ring_tio_init
 *	Checks the device tree and instantiates the driver if found
 */
void __init ring_tio_init(const char *node_name)
{
	struct platform_device *pdev;
	struct resource *res;
	int resource_idx = 0;
	struct ring_tio_node *ring_node;

	/*
	 * Check the device tree for the ring_tio
	 */
	ring_node = (struct ring_tio_node *)devtree_find_node(node_name);
	if (!ring_node) {
		printk(KERN_WARNING "Ring TIO '%s' not found\n", node_name);
		return;
	}

	if (ring_node->version != RING_TIO_NODE_VERSION) {
		printk(KERN_WARNING "ring_tio not compatible\n");
		return;
	}

	/*
	 * Dynamically create the platform_device structure and resources
	 */
	pdev = kzalloc(sizeof(struct platform_device), GFP_KERNEL);
	if (!pdev) {
		printk(KERN_WARNING "ring_tio could not alloc pdev\n");
		return;
	}

	res = kzalloc(sizeof(struct resource) * RING_TIO_MAX_RESOURCES,
			GFP_KERNEL);
	if (!res) {
		kfree(pdev);
		printk(KERN_WARNING "ring_tio could not alloc res\n");
		return;
	}

	pdev->name = ring_tio_driver_name;
	pdev->id = ring_tio_count++;
	pdev->resource = res;

	/*
	 * Fill in the resources and platform data from devtree information
	 */
	res[resource_idx].start = (u32_t)(ring_node->regs);
	res[resource_idx].end = (u32_t)(ring_node->regs);
	res[resource_idx].flags = IORESOURCE_MEM;
	resource_idx++;

	if (ring_node->dn.sendirq != 0xFF) {
		res[resource_idx].start = ring_node->dn.sendirq;
		res[resource_idx].flags = IORESOURCE_IRQ;
		resource_idx++;
	}

	if (ring_node->dn.recvirq != 0xFF) {
		res[resource_idx].start = ring_node->dn.recvirq;
		res[resource_idx].flags = IORESOURCE_IRQ;
		resource_idx++;
	}
	pdev->num_resources = resource_idx;

	printk(KERN_INFO "RingTIO.%d '%s' found irq=%d/%d regs=%p pdev=%p/%p\n",
		ring_tio_count - 1, node_name, ring_node->dn.sendirq,
		ring_node->dn.recvirq, ring_node->regs, pdev, res);

	/*
	 * Try to get the device registered
	 */
	pdev->dev.platform_data = (void *)node_name;
	if (platform_device_register(pdev) < 0) {
		printk(KERN_WARNING "Ring failed to register\n");
		kfree(pdev);
		kfree(res);
	}
}
