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
#include <bcm63xx_dev_usb_ehci.h>

static struct resource ehci_resources[] = {
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

static u64 ehci_dmamask = ~(u32)0;

static struct platform_device bcm63xx_ehci_device = {
	.name		= "bcm63xx_ehci",
	.id		= 0,
	.num_resources	= ARRAY_SIZE(ehci_resources),
	.resource	= ehci_resources,
	.dev		= {
		.dma_mask		= &ehci_dmamask,
		.coherent_dma_mask	= 0xffffffff,
	},
};

int __init bcm63xx_ehci_register(void)
{
	if (!BCMCPU_IS_6358())
		return 0;

	ehci_resources[0].start = bcm63xx_regset_address(RSET_EHCI0);
	ehci_resources[0].end = ehci_resources[0].start;
	ehci_resources[0].end += RSET_EHCI_SIZE - 1;
	ehci_resources[1].start = bcm63xx_get_irq_number(IRQ_EHCI0);
	return platform_device_register(&bcm63xx_ehci_device);
}
