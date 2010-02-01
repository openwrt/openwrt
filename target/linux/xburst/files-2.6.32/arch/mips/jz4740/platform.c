/*
 *  Copyright (C) 2009-2010, Lars-Peter Clausen <lars@metafoo.de>
 *  	JZ4740 platform devices
 *
 *  This program is free software; you can redistribute	 it and/or modify it
 *  under  the terms of	 the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the	License, or (at your
 *  option) any later version.
 *
 *  You should have received a copy of the  GNU General Public License along
 *  with this program; if not, write  to the Free Software Foundation, Inc.,
 *  675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/resource.h>

#include <asm/mach-jz4740/platform.h>
#include <asm/mach-jz4740/regs.h>
#include <asm/mach-jz4740/irq.h>

/* OHCI (USB full speed host controller) */
static struct resource jz4740_usb_ohci_resources[] = {
	[0] = {
		.start	= CPHYSADDR(UHC_BASE),
		.end	= CPHYSADDR(UHC_BASE) + 0x10000 - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= JZ_IRQ_UHC,
		.end	= JZ_IRQ_UHC,
		.flags	= IORESOURCE_IRQ,
	},
};

/* The dmamask must be set for OHCI to work */
static u64 ohci_dmamask = ~(u32)0;

struct platform_device jz4740_usb_ohci_device = {
	.name		= "jz-ohci",
	.id		= 0,
	.dev = {
		.dma_mask		= &ohci_dmamask,
		.coherent_dma_mask	= 0xffffffff,
	},
	.num_resources	= ARRAY_SIZE(jz4740_usb_ohci_resources),
	.resource	= jz4740_usb_ohci_resources,
};

/* UDC (USB gadget controller) */
static struct resource jz4740_usb_gdt_resources[] = {
	[0] = {
		.start	= CPHYSADDR(UDC_BASE),
		.end	= CPHYSADDR(UDC_BASE) + 0x10000 - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= JZ_IRQ_UDC,
		.end	= JZ_IRQ_UDC,
		.flags	= IORESOURCE_IRQ,
	},
};

static u64 jz4740_udc_dmamask = ~(u32)0;

struct platform_device jz4740_usb_gdt_device = {
	.name		= "jz-udc",
	.id		= -1,
	.dev = {
		.dma_mask		= &jz4740_udc_dmamask,
		.coherent_dma_mask	= 0xffffffff,
	},
	.num_resources	= ARRAY_SIZE(jz4740_usb_gdt_resources),
	.resource	= jz4740_usb_gdt_resources,
};

/** MMC/SD controller **/
static struct resource jz4740_mmc_resources[] = {
	[0] = {
		.start	= CPHYSADDR(MSC_BASE),
		.end	= CPHYSADDR(MSC_BASE) + 0x10000 - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= JZ_IRQ_MSC,
		.end	= JZ_IRQ_MSC,
		.flags	= IORESOURCE_IRQ,
	}
};

static u64 jz4740_mmc_dmamask =  ~(u32)0;

struct platform_device jz4740_mmc_device = {
	.name = "jz4740-mmc",
	.id = 0,
	.dev = {
		.dma_mask		= &jz4740_mmc_dmamask,
		.coherent_dma_mask	= 0xffffffff,
	},
	.num_resources  = ARRAY_SIZE(jz4740_mmc_resources),
	.resource	= jz4740_mmc_resources,
};

static struct resource jz4740_rtc_resources[] = {
	[0] = {
		.start	= CPHYSADDR(RTC_BASE),
		.end	= CPHYSADDR(RTC_BASE) + 0x38 - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start  = JZ_IRQ_RTC,
		.end	= JZ_IRQ_RTC,
		.flags	= IORESOURCE_IRQ,
	},
};

struct platform_device jz4740_rtc_device = {
	.name	= "jz4740-rtc",
	.id	= -1,
	.num_resources	= ARRAY_SIZE(jz4740_rtc_resources),
	.resource	= jz4740_rtc_resources,
};

/** I2C controller **/
static struct resource jz4740_i2c_resources[] = {
	[0] = {
		.start	= CPHYSADDR(I2C_BASE),
		.end	= CPHYSADDR(I2C_BASE) + 0x10000 - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= JZ_IRQ_I2C,
		.end	= JZ_IRQ_I2C,
		.flags	= IORESOURCE_IRQ,
	}
};

static u64 jz4740_i2c_dmamask =  ~(u32)0;

struct platform_device jz4740_i2c_device = {
	.name = "jz_i2c",
	.id = 0,
	.dev = {
		.dma_mask		= &jz4740_i2c_dmamask,
		.coherent_dma_mask	= 0xffffffff,
	},
	.num_resources  = ARRAY_SIZE(jz4740_i2c_resources),
	.resource	= jz4740_i2c_resources,
};

static struct resource jz4740_nand_resources[] = {
	[0] = {
		.start	= CPHYSADDR(EMC_BASE),
		.end	= CPHYSADDR(EMC_BASE) + 0x10000 - 1,
		.flags	= IORESOURCE_MEM,
	},
};

struct platform_device jz4740_nand_device = {
	.name = "jz4740-nand",
	.num_resources = ARRAY_SIZE(jz4740_nand_resources),
	.resource = jz4740_nand_resources,
};

static struct resource jz4740_framebuffer_resources[] = {
	[0] = {
		.start	= CPHYSADDR(LCD_BASE),
		.end	= CPHYSADDR(LCD_BASE) + 0x10000 - 1,
		.flags	= IORESOURCE_MEM,
	},
};

static u64 jz4740_fb_dmamask = ~(u32)0;

struct platform_device jz4740_framebuffer_device = {
	.name = "jz4740-fb",
	.id = -1,
	.num_resources = ARRAY_SIZE(jz4740_framebuffer_resources),
	.resource = jz4740_framebuffer_resources,
	.dev = {
		.dma_mask = &jz4740_fb_dmamask,
		.coherent_dma_mask = 0xffffffff,
	},
};

static struct resource jz4740_i2s_resources[] = {
	[0] = {
		.start	= CPHYSADDR(AIC_BASE),
		.end	= CPHYSADDR(AIC_BASE) + 0x38 - 1,
		.flags	= IORESOURCE_MEM,
	},
};

struct platform_device jz4740_i2s_device = {
	.name = "jz4740-i2s",
	.id = -1,
	.num_resources = ARRAY_SIZE(jz4740_i2s_resources),
	.resource = jz4740_i2s_resources,
};

static struct resource jz4740_codec_resources[] = {
	[0] = {
		.start	= CPHYSADDR(AIC_BASE) + 0x80,
		.end	= CPHYSADDR(AIC_BASE) + 0x88 - 1,
		.flags	= IORESOURCE_MEM,
	},
};

struct platform_device jz4740_codec_device = {
	.name		= "jz4740-codec",
	.id		= -1,
	.num_resources	= ARRAY_SIZE(jz4740_codec_resources),
	.resource	= jz4740_codec_resources,
};

static struct resource jz4740_adc_resources[] = {
	[0] = {
		.start	= CPHYSADDR(SADC_BASE),
		.end	= CPHYSADDR(SADC_BASE) + 0x30,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= JZ_IRQ_SADC,
		.end	= JZ_IRQ_SADC,
		.flags	= IORESOURCE_IRQ,
	},
};

struct platform_device jz4740_adc_device = {
	.name		= "jz4740-adc",
	.id		= -1,
	.num_resources	= ARRAY_SIZE(jz4740_adc_resources),
	.resource	= jz4740_adc_resources,
};

struct platform_device jz4740_battery_device = {
	.name = "jz4740-battery",
	.id = -1,
	.dev = {
		.parent	= &jz4740_adc_device.dev
	},
};
