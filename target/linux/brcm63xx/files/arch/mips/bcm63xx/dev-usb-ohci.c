/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2008 Maxime Bizon <mbizon@freebox.fr>
 */

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <bcm63xx_cpu.h>
#include <bcm63xx_dev_usb_ohci.h>

static struct resource ohci_resources[] = {
	{
		.start		= -1, /* filled at runtime */
		.end		= -1, /* filled at runtime */
		.flags		= IORESOURCE_MEM,
	},
	{
		.start		= -1, /* filled at runtime */
		.flags		= IORESOURCE_IRQ,
	},
};

static u64 ohci_dmamask = ~(u32)0;

static struct platform_device bcm63xx_ohci_device = {
	.name		= "bcm63xx_ohci",
	.id		= 0,
	.num_resources	= ARRAY_SIZE(ohci_resources),
	.resource	= ohci_resources,
	.dev		= {
		.dma_mask		= &ohci_dmamask,
		.coherent_dma_mask	= 0xffffffff,
	},
};

int __init bcm63xx_ohci_register(void)
{
	if (!BCMCPU_IS_6348() && !BCMCPU_IS_6358())
		return 0;

	ohci_resources[0].start = bcm63xx_regset_address(RSET_OHCI0);
	ohci_resources[0].end = ohci_resources[0].start;
	ohci_resources[0].end += RSET_OHCI_SIZE - 1;
	ohci_resources[1].start = bcm63xx_get_irq_number(IRQ_OHCI0);
	return platform_device_register(&bcm63xx_ohci_device);
}
