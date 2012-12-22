/*
 *  Ralink RT3662/RT3883 SoC platform device registration
 *
 *  Copyright (C) 2011-2012 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/physmap.h>
#include <linux/mtd/partitions.h>
#include <linux/dma-mapping.h>
#include <linux/spi/spi.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/clk.h>
#include <linux/rt2x00_platform.h>
#include <linux/usb/ehci_pdriver.h>
#include <linux/usb/ohci_pdriver.h>

#include <asm/addrspace.h>

#include <asm/mach-ralink/rt3883.h>
#include <asm/mach-ralink/rt3883_regs.h>
#include <asm/mach-ralink/ramips_nand_platform.h>
#include "devices.h"

#include <ramips_eth_platform.h>

static struct resource rt3883_flash0_resources[] = {
	{
		.flags	= IORESOURCE_MEM,
		.start	= RT3883_BOOT_BASE,
		.end	= RT3883_BOOT_BASE + RT3883_BOOT_SIZE - 1,
	},
};

struct physmap_flash_data rt3883_flash0_data;
static struct platform_device rt3883_flash0_device = {
	.name		= "physmap-flash",
	.resource	= rt3883_flash0_resources,
	.num_resources	= ARRAY_SIZE(rt3883_flash0_resources),
	.dev = {
		.platform_data = &rt3883_flash0_data,
	},
};

static struct resource rt3883_flash1_resources[] = {
	{
		.flags	= IORESOURCE_MEM,
		.start	= RT3883_SRAM_BASE,
		.end	= RT3883_SRAM_BASE + RT3883_SRAM_SIZE - 1,
	},
};

struct physmap_flash_data rt3883_flash1_data;
static struct platform_device rt3883_flash1_device = {
	.name		= "physmap-flash",
	.resource	= rt3883_flash1_resources,
	.num_resources	= ARRAY_SIZE(rt3883_flash1_resources),
	.dev = {
		.platform_data = &rt3883_flash1_data,
	},
};

static int rt3883_flash_instance __initdata;
void __init rt3883_register_pflash(unsigned int id)
{
	struct platform_device *pdev;
	struct physmap_flash_data *pdata;
	void __iomem *fscc_base;
	u32 t;
	int reg;

	switch (id) {
	case 0:
		pdev = &rt3883_flash0_device;
		reg = RT3883_FSCC_REG_FLASH_CFG0;
		break;
	case 1:
		pdev = &rt3883_flash1_device;
		reg = RT3883_FSCC_REG_FLASH_CFG1;
		break;
	default:
		return;
	}

	pdata = pdev->dev.platform_data;

	fscc_base = ioremap(RT3883_FSCC_BASE, RT3883_FSCC_SIZE);
	if (!fscc_base)
		panic("RT3883: ioremap failed for FSCC");

	t = __raw_readl(fscc_base + reg);
	iounmap(fscc_base);

	t = (t >> RT3883_FLASH_CFG_WIDTH_SHIFT) & RT3883_FLASH_CFG_WIDTH_MASK;
	switch (t) {
	case RT3883_FLASH_CFG_WIDTH_8BIT:
		pdata->width = 1;
		break;
	case RT3883_FLASH_CFG_WIDTH_16BIT:
		pdata->width = 2;
		break;
	case RT3883_FLASH_CFG_WIDTH_32BIT:
		if (id == 1) {
			pdata->width = 4;
			break;
		}
		/* fallthrough */
	default:
		pr_warn("RT3883: flash bank%d: invalid width detected\n", id);
		return;
	}

	pdev->id = rt3883_flash_instance;

	platform_device_register(pdev);
	rt3883_flash_instance++;
}

static atomic_t rt3883_usb_pwr_ref = ATOMIC_INIT(0);

static int rt3883_usb_power_on(struct platform_device *pdev)
{

	if (atomic_inc_return(&rt3883_usb_pwr_ref) == 1) {
		u32 t;

		t = rt3883_sysc_rr(RT3883_SYSC_REG_USB_PS);

		/* enable clock for port0's and port1's phys */
		t = rt3883_sysc_rr(RT3883_SYSC_REG_CLKCFG1);
		t |= RT3883_CLKCFG1_UPHY0_CLK_EN | RT3883_CLKCFG1_UPHY1_CLK_EN;
		rt3883_sysc_wr(t, RT3883_SYSC_REG_CLKCFG1);
		mdelay(500);

		/* pull USBHOST and USBDEV out from reset */
		t = rt3883_sysc_rr(RT3883_SYSC_REG_RSTCTRL);
		t &= ~(RT3883_RSTCTRL_UHST | RT3883_RSTCTRL_UDEV);
		rt3883_sysc_wr(t, RT3883_SYSC_REG_RSTCTRL);
		mdelay(500);

		/* enable host mode */
		t = rt3883_sysc_rr(RT3883_SYSC_REG_SYSCFG1);
		t |= RT3883_SYSCFG1_USB0_HOST_MODE;
		rt3883_sysc_wr(t, RT3883_SYSC_REG_SYSCFG1);

		t = rt3883_sysc_rr(RT3883_SYSC_REG_USB_PS);
	}

	return 0;
}

static void rt3883_usb_power_off(struct platform_device *pdev)
{
	if (atomic_dec_return(&rt3883_usb_pwr_ref) == 0) {
		u32 t;

		/* put USBHOST and USBDEV into reset */
		t = rt3883_sysc_rr(RT3883_SYSC_REG_RSTCTRL);
		t |= RT3883_RSTCTRL_UHST | RT3883_RSTCTRL_UDEV;
		rt3883_sysc_wr(t, RT3883_SYSC_REG_RSTCTRL);
		udelay(10000);

		/* disable clock for port0's and port1's phys*/
		t = rt3883_sysc_rr(RT3883_SYSC_REG_CLKCFG1);
		t &= ~(RT3883_CLKCFG1_UPHY0_CLK_EN |
		       RT3883_CLKCFG1_UPHY1_CLK_EN);
		rt3883_sysc_wr(t, RT3883_SYSC_REG_CLKCFG1);
		udelay(10000);
	}
}

static struct usb_ehci_pdata rt3883_ehci_data = {
	.port_power_off	= 1,
	.power_on	= rt3883_usb_power_on,
	.power_off	= rt3883_usb_power_off,
};

static struct resource rt3883_ehci_resources[] = {
	{
		.start	= RT3883_EHCI_BASE,
		.end	= RT3883_EHCI_BASE + PAGE_SIZE - 1,
		.flags	= IORESOURCE_MEM,
	}, {
		.start	= RT3883_INTC_IRQ_UHST,
		.end	= RT3883_INTC_IRQ_UHST,
		.flags	= IORESOURCE_IRQ,
	},
};

static u64 rt3883_ehci_dmamask = DMA_BIT_MASK(32);
static struct platform_device rt3883_ehci_device = {
	.name		= "ehci-platform",
	.id		= -1,
	.resource	= rt3883_ehci_resources,
	.num_resources	= ARRAY_SIZE(rt3883_ehci_resources),
	.dev            = {
		.dma_mask		= &rt3883_ehci_dmamask,
		.coherent_dma_mask	= DMA_BIT_MASK(32),
		.platform_data		= &rt3883_ehci_data,
	},
};

static struct resource rt3883_ohci_resources[] = {
	{
		.start	= RT3883_OHCI_BASE,
		.end	= RT3883_OHCI_BASE + PAGE_SIZE - 1,
		.flags	= IORESOURCE_MEM,
	}, {
		.start	= RT3883_INTC_IRQ_UHST,
		.end	= RT3883_INTC_IRQ_UHST,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct usb_ohci_pdata rt3883_ohci_data = {
	.power_on	= rt3883_usb_power_on,
	.power_off	= rt3883_usb_power_off,
};

static u64 rt3883_ohci_dmamask = DMA_BIT_MASK(32);
static struct platform_device rt3883_ohci_device = {
	.name		= "ohci-platform",
	.id		= -1,
	.resource	= rt3883_ohci_resources,
	.num_resources	= ARRAY_SIZE(rt3883_ohci_resources),
	.dev            = {
		.dma_mask		= &rt3883_ohci_dmamask,
		.coherent_dma_mask	= DMA_BIT_MASK(32),
		.platform_data		= &rt3883_ohci_data,
	},
};

void __init rt3883_register_usbhost(void)
{
	platform_device_register(&rt3883_ehci_device);
	platform_device_register(&rt3883_ohci_device);
}

static void rt3883_fe_reset(void)
{
	u32 t;

	t = rt3883_sysc_rr(RT3883_SYSC_REG_RSTCTRL);
	t |= RT3883_RSTCTRL_FE;
	rt3883_sysc_wr(t , RT3883_SYSC_REG_RSTCTRL);

	t &= ~RT3883_RSTCTRL_FE;
	rt3883_sysc_wr(t, RT3883_SYSC_REG_RSTCTRL);
}

static struct resource rt3883_eth_resources[] = {
	{
		.start	= RT3883_FE_BASE,
		.end	= RT3883_FE_BASE + PAGE_SIZE - 1,
		.flags	= IORESOURCE_MEM,
	}, {
		.start	= RT3883_CPU_IRQ_FE,
		.end	= RT3883_CPU_IRQ_FE,
		.flags	= IORESOURCE_IRQ,
	},
};

struct ramips_eth_platform_data rt3883_eth_data = {
	.mac		= { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55 },
	.reset_fe	= rt3883_fe_reset,
	.min_pkt_len	= 64,
};

static struct platform_device rt3883_eth_device = {
	.name		= "ramips_eth",
	.resource	= rt3883_eth_resources,
	.num_resources	= ARRAY_SIZE(rt3883_eth_resources),
	.dev = {
		.platform_data = &rt3883_eth_data,
	}
};

void __init rt3883_register_ethernet(void)
{
	struct clk *clk;

	clk = clk_get(NULL, "sys");
	if (IS_ERR(clk))
		panic("unable to get SYS clock, err=%ld", PTR_ERR(clk));

	rt3883_eth_data.sys_freq = clk_get_rate(clk);

	platform_device_register(&rt3883_eth_device);
}

static struct resource rt3883_wlan_resources[] = {
	{
		.start	= RT3883_WLAN_BASE,
		.end	= RT3883_WLAN_BASE + 0x3FFFF,
		.flags	= IORESOURCE_MEM,
	}, {
		.start	= RT3883_CPU_IRQ_WLAN,
		.end	= RT3883_CPU_IRQ_WLAN,
		.flags	= IORESOURCE_IRQ,
	},
};

struct rt2x00_platform_data rt3883_wlan_data;
static struct platform_device rt3883_wlan_device = {
	.name		= "rt2800_wmac",
	.resource	= rt3883_wlan_resources,
	.num_resources	= ARRAY_SIZE(rt3883_wlan_resources),
	.dev = {
		.platform_data = &rt3883_wlan_data,
	}
};

void __init rt3883_register_wlan(void)
{
	rt3883_wlan_data.eeprom_file_name = "soc_wmac.eeprom",
	platform_device_register(&rt3883_wlan_device);
}

static struct resource rt3883_wdt_resources[] = {
	{
		.start	= RT3883_TIMER_BASE,
		.end	= RT3883_TIMER_BASE + RT3883_TIMER_SIZE - 1,
		.flags	= IORESOURCE_MEM,
	},
};

static struct platform_device rt3883_wdt_device = {
	.name		= "ramips-wdt",
	.id		= -1,
	.resource	= rt3883_wdt_resources,
	.num_resources	= ARRAY_SIZE(rt3883_wdt_resources),
};

void __init rt3883_register_wdt(bool enable_reset)
{
	if (enable_reset) {
		u32 t;

		/* enable WDT reset output on GPIO 2 */
		t = rt3883_sysc_rr(RT3883_SYSC_REG_SYSCFG1);
		t |= RT3883_SYSCFG1_GPIO2_AS_WDT_OUT;
		rt3883_sysc_wr(t, RT3883_SYSC_REG_SYSCFG1);
	}

	platform_device_register(&rt3883_wdt_device);
}

static struct resource rt3883_nand_resources[] = {
	{
		.flags	= IORESOURCE_MEM,
		.start	= RT3883_NANDC_BASE,
		.end	= RT3883_NANDC_BASE + RT3883_NANDC_SIZE - 1,
	},
};

struct ramips_nand_platform_data rt3883_nand_data;
static struct platform_device rt3883_nand_device = {
	.name		= RAMIPS_NAND_DRIVER_NAME,
	.id		= -1,
	.resource	= rt3883_nand_resources,
	.num_resources	= ARRAY_SIZE(rt3883_nand_resources),
	.dev	= {
		.platform_data = &rt3883_nand_data,
	},
};

void __init rt3883_register_nand(void)
{
	platform_device_register(&rt3883_nand_device);
}

static struct resource rt3883_spi_resources[] = {
	{
		.flags	= IORESOURCE_MEM,
		.start	= RT3883_SPI_BASE,
		.end	= RT3883_SPI_BASE + RT3883_SPI_SIZE - 1,
	},
};

static struct platform_device rt3883_spi_device = {
	.name		= "ramips-spi",
	.id		= 0,
	.resource	= rt3883_spi_resources,
	.num_resources	= ARRAY_SIZE(rt3883_spi_resources),
};

void __init rt3883_register_spi(struct spi_board_info *info, int n)
{
	spi_register_board_info(info, n);
	platform_device_register(&rt3883_spi_device);
}

