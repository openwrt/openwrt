/*
 * Copyright (C) 2009	Henk Vergonet <Henk.Vergonet@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <bcm63xx_cpu.h>

static struct resource udc_resources[] = {
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

static u64 udc_dmamask = ~(u32)0;

static struct platform_device bcm63xx_udc_device = {
	.name		= "bcm63xx-udc",
	.id		= 0,
	.num_resources	= ARRAY_SIZE(udc_resources),
	.resource	= udc_resources,
	.dev		= {
		.dma_mask		= &udc_dmamask,
		.coherent_dma_mask	= 0xffffffff,
	},
};

int __init bcm63xx_udc_register(void)
{
	if (!BCMCPU_IS_6338() && !BCMCPU_IS_6345() && !BCMCPU_IS_6348())
		return 0;

	udc_resources[0].start = bcm63xx_regset_address(RSET_UDC0);
	udc_resources[0].end = udc_resources[0].start;
	udc_resources[0].end += RSET_UDC_SIZE - 1;
	udc_resources[1].start = bcm63xx_get_irq_number(IRQ_UDC0);
	return platform_device_register(&bcm63xx_udc_device);
}
