/*
 *  Ralink RT305x SoC platform device registration
 *
 *  Copyright (C) 2009-2010 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/err.h>
#include <linux/clk.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/physmap.h>
#include <linux/spi/spi.h>
#include <linux/rt2x00_platform.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/usb/ehci_pdriver.h>
#include <linux/usb/ohci_pdriver.h>

#include <asm/addrspace.h>

#include <asm/mach-ralink/rt305x.h>
#include <asm/mach-ralink/rt305x_regs.h>
#include "devices.h"

#include <ramips_eth_platform.h>
#include <rt305x_esw_platform.h>

static struct resource rt305x_flash0_resources[] = {
	{
		.flags	= IORESOURCE_MEM,
		.start	= KSEG1ADDR(RT305X_FLASH0_BASE),
		.end	= KSEG1ADDR(RT305X_FLASH0_BASE) +
			  RT305X_FLASH0_SIZE - 1,
	},
};

struct physmap_flash_data rt305x_flash0_data;
static struct platform_device rt305x_flash0_device = {
	.name		= "physmap-flash",
	.resource	= rt305x_flash0_resources,
	.num_resources	= ARRAY_SIZE(rt305x_flash0_resources),
	.dev = {
		.platform_data = &rt305x_flash0_data,
	},
};

static struct resource rt305x_flash1_resources[] = {
	{
		.flags	= IORESOURCE_MEM,
		.start	= KSEG1ADDR(RT305X_FLASH1_BASE),
		.end	= KSEG1ADDR(RT305X_FLASH1_BASE) +
			  RT305X_FLASH1_SIZE - 1,
	},
};

struct physmap_flash_data rt305x_flash1_data;
static struct platform_device rt305x_flash1_device = {
	.name		= "physmap-flash",
	.resource	= rt305x_flash1_resources,
	.num_resources	= ARRAY_SIZE(rt305x_flash1_resources),
	.dev = {
		.platform_data = &rt305x_flash1_data,
	},
};

static int rt305x_flash_instance __initdata;
void __init rt305x_register_flash(unsigned int id)
{
	struct platform_device *pdev;
	struct physmap_flash_data *pdata;
	u32 t;
	int reg;

	switch (id) {
	case 0:
		pdev = &rt305x_flash0_device;
		reg = MEMC_REG_FLASH_CFG0;
		break;
	case 1:
		pdev = &rt305x_flash1_device;
		reg = MEMC_REG_FLASH_CFG1;
		break;
	default:
		return;
	}

	t = rt305x_memc_rr(reg);
	t = (t >> FLASH_CFG_WIDTH_SHIFT) & FLASH_CFG_WIDTH_MASK;

	pdata = pdev->dev.platform_data;
	switch (t) {
	case FLASH_CFG_WIDTH_8BIT:
		pdata->width = 1;
		break;
	case FLASH_CFG_WIDTH_16BIT:
		pdata->width = 2;
		break;
	case FLASH_CFG_WIDTH_32BIT:
		pdata->width = 4;
		break;
	default:
		printk(KERN_ERR "RT305x: flash bank%u witdh is invalid\n", id);
		return;
	}

	pdev->id = rt305x_flash_instance;

	platform_device_register(pdev);
	rt305x_flash_instance++;
}

static void rt305x_fe_reset(void)
{
	rt305x_sysc_wr(RT305X_RESET_FE, SYSC_REG_RESET_CTRL);
	rt305x_sysc_wr(0, SYSC_REG_RESET_CTRL);
}

static struct resource rt305x_eth_resources[] = {
	{
		.start	= RT305X_FE_BASE,
		.end	= RT305X_FE_BASE + PAGE_SIZE - 1,
		.flags	= IORESOURCE_MEM,
	}, {
		.start	= RT305X_CPU_IRQ_FE,
		.end	= RT305X_CPU_IRQ_FE,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct ramips_eth_platform_data ramips_eth_data = {
	.mac = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55 },
	.reset_fe = rt305x_fe_reset,
	.min_pkt_len = 64,
};

static struct platform_device rt305x_eth_device = {
	.name		= "ramips_eth",
	.resource	= rt305x_eth_resources,
	.num_resources	= ARRAY_SIZE(rt305x_eth_resources),
	.dev = {
		.platform_data = &ramips_eth_data,
	}
};

static struct resource rt305x_esw_resources[] = {
	{
		.start	= RT305X_SWITCH_BASE,
		.end	= RT305X_SWITCH_BASE + PAGE_SIZE - 1,
		.flags	= IORESOURCE_MEM,
	},
};

struct rt305x_esw_platform_data rt305x_esw_data = {
	/* All ports are LAN ports. */
	.vlan_config		= RT305X_ESW_VLAN_CONFIG_NONE,
	.reg_initval_fct2	= 0x00d6500c,
	/*
	 * ext phy base addr 31, enable port 5 polling, rx/tx clock skew 1,
	 * turbo mii off, rgmi 3.3v off
	 * port5: disabled
	 * port6: enabled, gige, full-duplex, rx/tx-flow-control
	 */
	.reg_initval_fpa2	= 0x3f502b28,
};

static struct platform_device rt305x_esw_device = {
	.name		= "rt305x-esw",
	.resource	= rt305x_esw_resources,
	.num_resources	= ARRAY_SIZE(rt305x_esw_resources),
	.dev = {
		.platform_data = &rt305x_esw_data,
	}
};

void __init rt305x_register_ethernet(void)
{
	struct clk *clk;

	clk = clk_get(NULL, "sys");
	if (IS_ERR(clk))
		panic("unable to get SYS clock, err=%ld", PTR_ERR(clk));

	ramips_eth_data.sys_freq = clk_get_rate(clk);

	platform_device_register(&rt305x_esw_device);
	platform_device_register(&rt305x_eth_device);
}

static struct resource rt305x_wifi_resources[] = {
	{
		.start	= RT305X_WMAC_BASE,
		.end	= RT305X_WMAC_BASE + 0x3FFFF,
		.flags	= IORESOURCE_MEM,
	}, {
		.start	= RT305X_CPU_IRQ_WNIC,
		.end	= RT305X_CPU_IRQ_WNIC,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct rt2x00_platform_data rt305x_wifi_data;
static struct platform_device rt305x_wifi_device = {
	.name			= "rt2800_wmac",
	.resource		= rt305x_wifi_resources,
	.num_resources	= ARRAY_SIZE(rt305x_wifi_resources),
	.dev = {
		.platform_data = &rt305x_wifi_data,
	}
};

void __init rt305x_register_wifi(void)
{
	u32 t;

	rt305x_wifi_data.eeprom_file_name = "soc_wmac.eeprom";

	if (soc_is_rt3352() || soc_is_rt5350()) {
		t = rt305x_sysc_rr(SYSC_REG_SYSTEM_CONFIG);
		t &= RT3352_SYSCFG0_XTAL_SEL;
		if (!t)
			rt305x_wifi_data.clk_is_20mhz = 1;
	}
	platform_device_register(&rt305x_wifi_device);
}

static struct resource rt305x_wdt_resources[] = {
	{
		.start	= RT305X_TIMER_BASE,
		.end	= RT305X_TIMER_BASE + RT305X_TIMER_SIZE - 1,
		.flags	= IORESOURCE_MEM,
	},
};

static struct platform_device rt305x_wdt_device = {
	.name		= "ramips-wdt",
	.id		= -1,
	.resource	= rt305x_wdt_resources,
	.num_resources	= ARRAY_SIZE(rt305x_wdt_resources),
};

void __init rt305x_register_wdt(void)
{
	u32 t;

	/* enable WDT reset output on pin SRAM_CS_N */
	t = rt305x_sysc_rr(SYSC_REG_SYSTEM_CONFIG);
	t |= RT305X_SYSCFG_SRAM_CS0_MODE_WDT <<
	     RT305X_SYSCFG_SRAM_CS0_MODE_SHIFT;
	rt305x_sysc_wr(t, SYSC_REG_SYSTEM_CONFIG);

	platform_device_register(&rt305x_wdt_device);
}

static struct resource rt305x_spi_resources[] = {
	{
		.flags	= IORESOURCE_MEM,
		.start	= RT305X_SPI_BASE,
		.end	= RT305X_SPI_BASE + RT305X_SPI_SIZE - 1,
	},
};

static struct platform_device rt305x_spi_device = {
	.name		= "ramips-spi",
	.id		= 0,
	.resource	= rt305x_spi_resources,
	.num_resources	= ARRAY_SIZE(rt305x_spi_resources),
};

void __init rt305x_register_spi(struct spi_board_info *info, int n)
{
	spi_register_board_info(info, n);
	platform_device_register(&rt305x_spi_device);
}

static struct resource rt305x_dwc_otg_resources[] = {
	{
		.start	= RT305X_OTG_BASE,
		.end	= RT305X_OTG_BASE + 0x3FFFF,
		.flags	= IORESOURCE_MEM,
	}, {
		.start	= RT305X_INTC_IRQ_OTG,
		.end	= RT305X_INTC_IRQ_OTG,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device rt305x_dwc_otg_device = {
	.name			= "dwc_otg",
	.resource		= rt305x_dwc_otg_resources,
	.num_resources	= ARRAY_SIZE(rt305x_dwc_otg_resources),
	.dev = {
		.platform_data = NULL,
	}
};

static atomic_t rt3352_usb_pwr_ref = ATOMIC_INIT(0);

static int rt3352_usb_power_on(struct platform_device *pdev)
{

	if (atomic_inc_return(&rt3352_usb_pwr_ref) == 1) {
		u32 t;

		t = rt305x_sysc_rr(RT3352_SYSC_REG_USB_PS);

		/* enable clock for port0's and port1's phys */
		t = rt305x_sysc_rr(RT3352_SYSC_REG_CLKCFG1);
		t |= RT3352_CLKCFG1_UPHY0_CLK_EN | RT3352_CLKCFG1_UPHY1_CLK_EN;
		rt305x_sysc_wr(t, RT3352_SYSC_REG_CLKCFG1);
		mdelay(500);

		/* pull USBHOST and USBDEV out from reset */
		t = rt305x_sysc_rr(RT3352_SYSC_REG_RSTCTRL);
		t &= ~(RT3352_RSTCTRL_UHST | RT3352_RSTCTRL_UDEV);
		rt305x_sysc_wr(t, RT3352_SYSC_REG_RSTCTRL);
		mdelay(500);

		/* enable host mode */
		t = rt305x_sysc_rr(RT3352_SYSC_REG_SYSCFG1);
		t |= RT3352_SYSCFG1_USB0_HOST_MODE;
		rt305x_sysc_wr(t, RT3352_SYSC_REG_SYSCFG1);

		t = rt305x_sysc_rr(RT3352_SYSC_REG_USB_PS);
	}

	return 0;
}

static void rt3352_usb_power_off(struct platform_device *pdev)
{

	if (atomic_dec_return(&rt3352_usb_pwr_ref) == 0) {
		u32 t;

		/* put USBHOST and USBDEV into reset */
		t = rt305x_sysc_rr(RT3352_SYSC_REG_RSTCTRL);
		t |= RT3352_RSTCTRL_UHST | RT3352_RSTCTRL_UDEV;
		rt305x_sysc_wr(t, RT3352_SYSC_REG_RSTCTRL);
		udelay(10000);

		/* disable clock for port0's and port1's phys*/
		t = rt305x_sysc_rr(RT3352_SYSC_REG_CLKCFG1);
		t &= ~(RT3352_CLKCFG1_UPHY0_CLK_EN | RT3352_CLKCFG1_UPHY1_CLK_EN);
		rt305x_sysc_wr(t, RT3352_SYSC_REG_CLKCFG1);
		udelay(10000);
	}
}

static struct usb_ehci_pdata rt3352_ehci_data = {
	.port_power_off	= 1,
	.power_on	= rt3352_usb_power_on,
	.power_off	= rt3352_usb_power_off,
};

static struct resource rt3352_ehci_resources[] = {
	{
		.start	= RT3352_EHCI_BASE,
		.end	= RT3352_EHCI_BASE + RT3352_EHCI_SIZE - 1,
		.flags	= IORESOURCE_MEM,
	}, {
		.start	= RT305X_INTC_IRQ_OTG,
		.end	= RT305X_INTC_IRQ_OTG,
		.flags	= IORESOURCE_IRQ,
	},
};

static u64 rt3352_ehci_dmamask = DMA_BIT_MASK(32);
static struct platform_device rt3352_ehci_device = {
	.name		= "ehci-platform",
	.id		= -1,
	.resource	= rt3352_ehci_resources,
	.num_resources	= ARRAY_SIZE(rt3352_ehci_resources),
	.dev            = {
		.dma_mask		= &rt3352_ehci_dmamask,
		.coherent_dma_mask	= DMA_BIT_MASK(32),
		.platform_data		= &rt3352_ehci_data,
	},
};

static struct resource rt3352_ohci_resources[] = {
	{
		.start	= RT3352_OHCI_BASE,
		.end	= RT3352_OHCI_BASE + RT3352_OHCI_SIZE - 1,
		.flags	= IORESOURCE_MEM,
	}, {
		.start	= RT305X_INTC_IRQ_OTG,
		.end	= RT305X_INTC_IRQ_OTG,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct usb_ohci_pdata rt3352_ohci_data = {
	.power_on	= rt3352_usb_power_on,
	.power_off	= rt3352_usb_power_off,
};

static u64 rt3352_ohci_dmamask = DMA_BIT_MASK(32);
static struct platform_device rt3352_ohci_device = {
	.name		= "ohci-platform",
	.id		= -1,
	.resource	= rt3352_ohci_resources,
	.num_resources	= ARRAY_SIZE(rt3352_ohci_resources),
	.dev            = {
		.dma_mask		= &rt3352_ohci_dmamask,
		.coherent_dma_mask	= DMA_BIT_MASK(32),
		.platform_data		= &rt3352_ohci_data,
	},
};

void __init rt305x_register_usb(void)
{
	if (soc_is_rt305x() || soc_is_rt3350()) {
		platform_device_register(&rt305x_dwc_otg_device);
	} else if (soc_is_rt3352() || soc_is_rt5350()) {
		platform_device_register(&rt3352_ehci_device);
		platform_device_register(&rt3352_ohci_device);
	} else {
		BUG();
	}
}
