/*
 *  RouterBoard 500 Platform devices
 *
 *  Copyright (C) 2006 Felix Fietkau <nbd@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  $Id$
 */
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/ctype.h>
#include <linux/string.h>
#include <linux/platform_device.h>
#include <asm/unaligned.h>
#include <asm/io.h>

#include <asm/rc32434/rc32434.h>
#include <asm/rc32434/dma.h>
#include <asm/rc32434/dma_v.h>
#include <asm/rc32434/eth.h>
#include <asm/rc32434/rb.h>

#define ETH0_DMA_RX_IRQ   	GROUP1_IRQ_BASE + 0
#define ETH0_DMA_TX_IRQ   	GROUP1_IRQ_BASE + 1 
#define ETH0_RX_OVR_IRQ   	GROUP3_IRQ_BASE + 9
#define ETH0_TX_UND_IRQ   	GROUP3_IRQ_BASE + 10

#define ETH0_RX_DMA_ADDR  (DMA0_PhysicalAddress + 0*DMA_CHAN_OFFSET)
#define ETH0_TX_DMA_ADDR  (DMA0_PhysicalAddress + 1*DMA_CHAN_OFFSET)

static struct resource korina_dev0_res[] = {
	{
		.name  = "korina_regs",
		.start = ETH0_PhysicalAddress,
		.end   = ETH0_PhysicalAddress + sizeof(ETH_t),
		.flags = IORESOURCE_MEM,
	},
	{
		.name  = "korina_rx",
		.start = ETH0_DMA_RX_IRQ,
		.end   = ETH0_DMA_RX_IRQ,
		.flags = IORESOURCE_IRQ
	},
	{
		.name  = "korina_tx",
		.start = ETH0_DMA_TX_IRQ,
		.end   = ETH0_DMA_TX_IRQ,
		.flags = IORESOURCE_IRQ
	},
	{
		.name  = "korina_ovr",
		.start = ETH0_RX_OVR_IRQ,
		.end   = ETH0_RX_OVR_IRQ,
		.flags = IORESOURCE_IRQ
	},
	{
		.name  = "korina_und",
		.start = ETH0_TX_UND_IRQ,
		.end   = ETH0_TX_UND_IRQ,
		.flags = IORESOURCE_IRQ
	},
	{
		.name  = "korina_dma_rx",
		.start = ETH0_RX_DMA_ADDR,
		.end   = ETH0_RX_DMA_ADDR + DMA_CHAN_OFFSET - 1,
		.flags = IORESOURCE_MEM,
	},
	{
		.name  = "korina_dma_tx",
		.start = ETH0_TX_DMA_ADDR,
		.end   = ETH0_TX_DMA_ADDR + DMA_CHAN_OFFSET - 1,
		.flags = IORESOURCE_MEM,
	}
};

static struct korina_device korina_dev0_data = {
	.name = "korina0",
	.mac = { 0xde, 0xca, 0xff, 0xc0, 0xff, 0xee }
};

static struct platform_device korina_dev0 = {
	.id = 0,
	.name = "korina",
	.dev.platform_data = &korina_dev0_data,
	.resource = korina_dev0_res,
	.num_resources = ARRAY_SIZE(korina_dev0_res),
};


#define CF_GPIO_NUM 13

static struct resource cf_slot0_res[] = {
	{
		.name  = "cf_membase",
		.flags = IORESOURCE_MEM
	},
	{
		.name  = "cf_irq",
		.start = (8 + 4 * 32 + CF_GPIO_NUM),  /* 149 */
		.end   = (8 + 4 * 32 + CF_GPIO_NUM),
		.flags = IORESOURCE_IRQ
	}
};

static struct cf_device cf_slot0_data = {
	.gpio_pin = 13
};

static struct platform_device cf_slot0 = {
	.id = 0,
	.name = "rb500-cf",
	.dev.platform_data = &cf_slot0_data,
	.resource = cf_slot0_res,
	.num_resources = ARRAY_SIZE(cf_slot0_res),
};

/* Resources and device for NAND.  There is no data needed and no irqs, so just define the memory used. */
static struct resource nand_slot0_res[] = {
	{
		.name = "nand_membase",
		.flags = IORESOURCE_MEM	
	}
};
 
static struct platform_device nand_slot0 = {
	.id = 0,
	.name = "rb500-nand",
	.resource = nand_slot0_res,
	.num_resources = ARRAY_SIZE(nand_slot0_res),
};


static struct platform_device *rb500_devs[] = {
	&korina_dev0,
	&nand_slot0,
	&cf_slot0
};

static void __init parse_mac_addr(char* macstr)
{
	int i, j;
	unsigned char result, value;
	
	for (i=0; i<6; i++) {
		result = 0;
		if (i != 5 && *(macstr+2) != ':') {
			return;
		}				
		for (j=0; j<2; j++) {
			if (isxdigit(*macstr) && (value = isdigit(*macstr) ? *macstr-'0' : 
						  toupper(*macstr)-'A'+10) < 16) {
				result = result*16 + value;
				macstr++;
			} 
			else return;
		}
		
		macstr++; 
		korina_dev0_data.mac[i] = result;
	}
}


/* DEVICE CONTROLLER 1 */
#define CFG_DC_DEV1 (void*)0xb8010010
#define CFG_DC_DEV2 (void*)0xb8010020
#define CFG_DC_DEVBASE    0x0
#define CFG_DC_DEVMASK    0x4
#define CFG_DC_DEVC       0x8
#define CFG_DC_DEVTC      0xC


static int __init plat_setup_devices(void)
{
	/* Look for the CF card reader */
	if (!readl(CFG_DC_DEV1 + CFG_DC_DEVMASK))
		rb500_devs[1] = NULL;
	else {
		cf_slot0_res[0].start = readl(CFG_DC_DEV1 + CFG_DC_DEVBASE);
		cf_slot0_res[0].end = cf_slot0_res[0].start + 0x1000;
	}
	
	/* There is always a NAND device */
	nand_slot0_res[0].start = readl( CFG_DC_DEV2 + CFG_DC_DEVBASE);
	nand_slot0_res[0].end = nand_slot0_res[0].start + 0x1000;
		
	return platform_add_devices(rb500_devs, ARRAY_SIZE(rb500_devs));
}

static int __init setup_kmac(char *s)
{
    printk("korina mac = %s\n",s);
	parse_mac_addr(s);
    return 0;
}

__setup("kmac=", setup_kmac);
arch_initcall(plat_setup_devices);


