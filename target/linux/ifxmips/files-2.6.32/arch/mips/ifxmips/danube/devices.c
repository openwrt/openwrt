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

/* usb */
static struct resource dwc_usb_res[] =
{
	{
		.name = "dwc3884_membase",
		.flags = IORESOURCE_MEM,
		.start = 0x1E101000,
		.end = 0x1E101FFF
	},
	{
		.name = "dwc3884_irq",
		.flags = IORESOURCE_IRQ,
		.start = IFXMIPS_USB_INT,
	}
};

static struct platform_device dwc_usb =
{
	.name = "dwc3884-hcd",
	.resource = dwc_usb_res,
	.num_resources = ARRAY_SIZE(dwc_usb_res),
};

void __init
danube_register_usb(void)
{
	platform_device_register(&dwc_usb);
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
danube_register_ethernet(unsigned char *mac)
{
	if(mac)
		danube_ethernet.dev.platform_data = mac;
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
