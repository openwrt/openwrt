#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/string.h>
#include <linux/mtd/physmap.h>
#include <linux/kernel.h>
#include <linux/reboot.h>
#include <linux/platform_device.h>
#include <linux/leds.h>
#include <linux/etherdevice.h>
#include <linux/reboot.h>
#include <linux/time.h>
#include <linux/io.h>
#include <linux/gpio.h>
#include <linux/leds.h>

#include <asm/bootinfo.h>
#include <asm/irq.h>

#include <ifxmips.h>
#include <ifxmips_irq.h>
#include <ifxmips_pmu.h>
#include <ifxmips_led.h>

#include "devices.h"

/* asc ports */
static struct resource danube_asc0_resources[] =
{
	[0] = {
		.start  = (IFXMIPS_ASC_BASE_ADDR & ~KSEG1),
		.end    = (IFXMIPS_ASC_BASE_ADDR & ~KSEG1) + 0x100 - 1,
		.flags  = IORESOURCE_MEM,
	},
	[1] = {
		.start	= IFXMIPSASC_TIR(0),
		.end	= IFXMIPSASC_TIR(0)+3,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct resource danube_asc1_resources[] =
{
	[0] = {
		.start  = (IFXMIPS_ASC_BASE_ADDR & ~KSEG1) + IFXMIPS_ASC_BASE_DIFF,
		.end    = (IFXMIPS_ASC_BASE_ADDR & ~KSEG1) + IFXMIPS_ASC_BASE_DIFF + 0x100 - 1,
		.flags  = IORESOURCE_MEM,
	},
	[1] = {
		.start	= IFXMIPSASC_TIR(1),
		.end	= IFXMIPSASC_TIR(1)+3,
		.flags	= IORESOURCE_IRQ,
	},
};

void __init danube_register_asc(int port)
{
	switch (port) {
	case 0:
		platform_device_register_simple("ifxmips_asc", 0,
			danube_asc0_resources, ARRAY_SIZE(danube_asc0_resources));
		break;
	case 1:
		platform_device_register_simple("ifxmips_asc", 1,
			danube_asc1_resources, ARRAY_SIZE(danube_asc1_resources));
		break;
	default:
		break;
	}
}

/* ebu gpio */
static struct platform_device ifxmips_ebu_gpio =
{
	.name = "ifxmips_ebu",
	.num_resources = 1,
};

void __init
danube_register_ebu_gpio(struct resource *resource, u32 value)
{
	ifxmips_ebu_gpio.resource = resource;
	ifxmips_ebu_gpio.dev.platform_data = (void*)value;
	platform_device_register(&ifxmips_ebu_gpio);
}

/* ethernet */
unsigned char ifxmips_ethaddr[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static struct resource danube_ethernet_resources =
{
	.start  = IFXMIPS_PPE32_BASE_ADDR,
	.end    = IFXMIPS_PPE32_BASE_ADDR + IFXMIPS_PPE32_SIZE - 1,
	.flags  = IORESOURCE_MEM,
};

static struct platform_device danube_ethernet =
{
	.name			= "ifxmips_mii0",
	.resource		= &danube_ethernet_resources,
	.num_resources	= 1,
	.dev = {
		.platform_data = ifxmips_ethaddr,
	}
};

void __init
danube_register_ethernet(unsigned char *mac, int mii_mode)
{
	struct ifxmips_eth_data *eth = kmalloc(sizeof(struct ifxmips_eth_data), GFP_KERNEL);
	memset(eth, 0, sizeof(struct ifxmips_eth_data));
	if(mac)
		eth->mac = mac;
	else
		eth->mac = ifxmips_ethaddr;
	eth->mii_mode = mii_mode;
	danube_ethernet.dev.platform_data = eth;
	platform_device_register(&danube_ethernet);
}

/* pci */
extern int ifxmips_pci_external_clock;
extern int ifxmips_pci_req_mask;

void __init
danube_register_pci(int clock, int irq_mask)
{
	ifxmips_pci_external_clock = clock;
	if(irq_mask)
		ifxmips_pci_req_mask = irq_mask;
}

/* tapi */
static struct resource mps_resources[] = {
	{
		.name = "mem",
		.flags = IORESOURCE_MEM,
		.start = 0x1f107000,
		.end =   0x1f1073ff,
	},
	{
		.name = "mailbox",
		.flags = IORESOURCE_MEM,
		.start = 0x1f200000,
		.end =   0x1f2007ff,
	},
};

static struct platform_device mps_device = {
	.name = "mps",
	.resource = mps_resources,
	.num_resources = ARRAY_SIZE(mps_resources),
};

static struct platform_device vmmc_device = {
	.name = "vmmc",
	.dev = {
		.parent = &mps_device.dev,
	},
};

void __init
danube_register_tapi(void)
{
#define CP1_SIZE	(1 << 20)
	dma_addr_t dma;
	mps_device.dev.platform_data = CPHYSADDR(dma_alloc_coherent(NULL, CP1_SIZE, &dma, GFP_ATOMIC));
	platform_device_register(&mps_device);
	platform_device_register(&vmmc_device);
}
