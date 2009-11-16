/*
 *  Atheros AR71xx SoC platform devices
 *
 *  Copyright (C) 2008-2009 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  Parts of this file are based on Atheros' 2.6.15 BSP
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/etherdevice.h>
#include <linux/platform_device.h>
#include <linux/serial_8250.h>
#include <linux/ath9k_platform.h>

#include <asm/mach-ar71xx/ar71xx.h>

#include "devices.h"

static u8 ar71xx_mac_base[ETH_ALEN] __initdata;

/*
 * OHCI (USB full speed host controller)
 */
static struct resource ar71xx_ohci_resources[] = {
	[0] = {
		.start	= AR71XX_OHCI_BASE,
		.end	= AR71XX_OHCI_BASE + AR71XX_OHCI_SIZE - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= AR71XX_MISC_IRQ_OHCI,
		.end	= AR71XX_MISC_IRQ_OHCI,
		.flags	= IORESOURCE_IRQ,
	},
};

static u64 ar71xx_ohci_dmamask = DMA_BIT_MASK(32);
static struct platform_device ar71xx_ohci_device = {
	.name		= "ar71xx-ohci",
	.id		= -1,
	.resource	= ar71xx_ohci_resources,
	.num_resources	= ARRAY_SIZE(ar71xx_ohci_resources),
	.dev = {
		.dma_mask		= &ar71xx_ohci_dmamask,
		.coherent_dma_mask	= DMA_BIT_MASK(32),
	},
};

/*
 * EHCI (USB full speed host controller)
 */
static struct resource ar71xx_ehci_resources[] = {
	[0] = {
		.start	= AR71XX_EHCI_BASE,
		.end	= AR71XX_EHCI_BASE + AR71XX_EHCI_SIZE - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= AR71XX_CPU_IRQ_USB,
		.end	= AR71XX_CPU_IRQ_USB,
		.flags	= IORESOURCE_IRQ,
	},
};


static u64 ar71xx_ehci_dmamask = DMA_BIT_MASK(32);
static struct ar71xx_ehci_platform_data ar71xx_ehci_data;

static struct platform_device ar71xx_ehci_device = {
	.name		= "ar71xx-ehci",
	.id		= -1,
	.resource	= ar71xx_ehci_resources,
	.num_resources	= ARRAY_SIZE(ar71xx_ehci_resources),
	.dev = {
		.dma_mask		= &ar71xx_ehci_dmamask,
		.coherent_dma_mask	= DMA_BIT_MASK(32),
		.platform_data		= &ar71xx_ehci_data,
	},
};

#define AR71XX_USB_RESET_MASK \
	(RESET_MODULE_USB_HOST | RESET_MODULE_USB_PHY \
	| RESET_MODULE_USB_OHCI_DLL)

static void __init ar71xx_usb_setup(void)
{
	ar71xx_device_stop(AR71XX_USB_RESET_MASK);
	mdelay(1000);
	ar71xx_device_start(AR71XX_USB_RESET_MASK);

	/* Turning on the Buff and Desc swap bits */
	ar71xx_usb_ctrl_wr(USB_CTRL_REG_CONFIG, 0xf0000);

	/* WAR for HW bug. Here it adjusts the duration between two SOFS */
	ar71xx_usb_ctrl_wr(USB_CTRL_REG_FLADJ, 0x20c00);

	mdelay(900);
}

static void __init ar91xx_usb_setup(void)
{
	ar71xx_device_stop(RESET_MODULE_USBSUS_OVERRIDE);
	mdelay(10);

	ar71xx_device_start(RESET_MODULE_USB_HOST);
	mdelay(10);

	ar71xx_device_start(RESET_MODULE_USB_PHY);
	mdelay(10);
}

void __init ar71xx_add_device_usb(void)
{
	switch (ar71xx_soc) {
	case AR71XX_SOC_AR7130:
	case AR71XX_SOC_AR7141:
	case AR71XX_SOC_AR7161:
		ar71xx_usb_setup();
		platform_device_register(&ar71xx_ohci_device);
		platform_device_register(&ar71xx_ehci_device);
		break;

	case AR71XX_SOC_AR9130:
	case AR71XX_SOC_AR9132:
		ar91xx_usb_setup();
		ar71xx_ehci_data.is_ar91xx = 1;
		platform_device_register(&ar71xx_ehci_device);
		break;

	default:
		BUG();
	}
}

static struct resource ar71xx_uart_resources[] = {
	{
		.start	= AR71XX_UART_BASE,
		.end	= AR71XX_UART_BASE + AR71XX_UART_SIZE - 1,
		.flags	= IORESOURCE_MEM,
	},
};

#define AR71XX_UART_FLAGS (UPF_BOOT_AUTOCONF | UPF_SKIP_TEST | UPF_IOREMAP)
static struct plat_serial8250_port ar71xx_uart_data[] = {
	{
		.mapbase	= AR71XX_UART_BASE,
		.irq		= AR71XX_MISC_IRQ_UART,
		.flags		= AR71XX_UART_FLAGS,
		.iotype		= UPIO_MEM32,
		.regshift	= 2,
	}, {
		/* terminating entry */
	}
};

static struct platform_device ar71xx_uart_device = {
	.name		= "serial8250",
	.id		= PLAT8250_DEV_PLATFORM,
	.resource	= ar71xx_uart_resources,
	.num_resources	= ARRAY_SIZE(ar71xx_uart_resources),
	.dev = {
		.platform_data	= ar71xx_uart_data
	},
};

void __init ar71xx_add_device_uart(void)
{
	ar71xx_uart_data[0].uartclk = ar71xx_ahb_freq;
	platform_device_register(&ar71xx_uart_device);
}

static struct resource ar71xx_mdio_resources[] = {
	{
		.name	= "mdio_base",
		.flags	= IORESOURCE_MEM,
		.start	= AR71XX_GE0_BASE,
		.end	= AR71XX_GE0_BASE + 0x200 - 1,
	}
};

static struct ag71xx_mdio_platform_data ar71xx_mdio_data;

static struct platform_device ar71xx_mdio_device = {
	.name		= "ag71xx-mdio",
	.id		= -1,
	.resource	= ar71xx_mdio_resources,
	.num_resources	= ARRAY_SIZE(ar71xx_mdio_resources),
	.dev = {
		.platform_data = &ar71xx_mdio_data,
	},
};

void __init ar71xx_add_device_mdio(u32 phy_mask)
{
	if (ar71xx_soc == AR71XX_SOC_AR7240)
		ar71xx_mdio_data.is_ar7240 = 1;

	ar71xx_mdio_data.phy_mask = phy_mask;

	platform_device_register(&ar71xx_mdio_device);
}

static void ar71xx_set_pll(u32 cfg_reg, u32 pll_reg, u32 pll_val, u32 shift)
{
	void __iomem *base;
	u32 t;

	base = ioremap_nocache(AR71XX_PLL_BASE, AR71XX_PLL_SIZE);

	t = __raw_readl(base + cfg_reg);
	t &= ~(3 << shift);
	t |=  (2 << shift);
	__raw_writel(t, base + cfg_reg);
	udelay(100);

	__raw_writel(pll_val, base + pll_reg);

	t |= (3 << shift);
	__raw_writel(t, base + cfg_reg);
	udelay(100);

	t &= ~(3 << shift);
	__raw_writel(t, base + cfg_reg);
	udelay(100);

	printk(KERN_DEBUG "ar71xx: pll_reg %#x: %#x\n",
		(unsigned int)(base + pll_reg), __raw_readl(base + pll_reg));

	iounmap(base);
}

struct ar71xx_eth_pll_data ar71xx_eth0_pll_data;
struct ar71xx_eth_pll_data ar71xx_eth1_pll_data;

static u32 ar71xx_get_eth_pll(unsigned int mac, int speed)
{
	struct ar71xx_eth_pll_data *pll_data;
	u32 pll_val;

	switch (mac) {
	case 0:
		pll_data = &ar71xx_eth0_pll_data;
		break;
	case 1:
		pll_data = &ar71xx_eth1_pll_data;
		break;
	default:
		BUG();
	}

	switch (speed) {
	case SPEED_10:
		pll_val = pll_data->pll_10;
		break;
	case SPEED_100:
		pll_val = pll_data->pll_100;
		break;
	case SPEED_1000:
		pll_val = pll_data->pll_1000;
		break;
	default:
		BUG();
	}

	return pll_val;
}

static void ar71xx_set_pll_ge0(int speed)
{
	u32 val = ar71xx_get_eth_pll(0, speed);

	ar71xx_set_pll(AR71XX_PLL_REG_SEC_CONFIG, AR71XX_PLL_REG_ETH0_INT_CLOCK,
			val, AR71XX_ETH0_PLL_SHIFT);
}

static void ar71xx_set_pll_ge1(int speed)
{
	u32 val = ar71xx_get_eth_pll(1, speed);

	ar71xx_set_pll(AR71XX_PLL_REG_SEC_CONFIG, AR71XX_PLL_REG_ETH1_INT_CLOCK,
			 val, AR71XX_ETH1_PLL_SHIFT);
}

static void ar724x_set_pll_ge0(int speed)
{
	/* TODO */
}

static void ar724x_set_pll_ge1(int speed)
{
	/* TODO */
}

static void ar91xx_set_pll_ge0(int speed)
{
	u32 val = ar71xx_get_eth_pll(0, speed);

	ar71xx_set_pll(AR91XX_PLL_REG_ETH_CONFIG, AR91XX_PLL_REG_ETH0_INT_CLOCK,
			 val, AR91XX_ETH0_PLL_SHIFT);
}

static void ar91xx_set_pll_ge1(int speed)
{
	u32 val = ar71xx_get_eth_pll(1, speed);

	ar71xx_set_pll(AR91XX_PLL_REG_ETH_CONFIG, AR91XX_PLL_REG_ETH1_INT_CLOCK,
			 val, AR91XX_ETH1_PLL_SHIFT);
}

static void ar71xx_ddr_flush_ge0(void)
{
	ar71xx_ddr_flush(AR71XX_DDR_REG_FLUSH_GE0);
}

static void ar71xx_ddr_flush_ge1(void)
{
	ar71xx_ddr_flush(AR71XX_DDR_REG_FLUSH_GE1);
}

static void ar724x_ddr_flush_ge0(void)
{
	ar71xx_ddr_flush(AR724X_DDR_REG_FLUSH_GE0);
}

static void ar724x_ddr_flush_ge1(void)
{
	ar71xx_ddr_flush(AR724X_DDR_REG_FLUSH_GE1);
}

static void ar91xx_ddr_flush_ge0(void)
{
	ar71xx_ddr_flush(AR91XX_DDR_REG_FLUSH_GE0);
}

static void ar91xx_ddr_flush_ge1(void)
{
	ar71xx_ddr_flush(AR91XX_DDR_REG_FLUSH_GE1);
}

static struct resource ar71xx_eth0_resources[] = {
	{
		.name	= "mac_base",
		.flags	= IORESOURCE_MEM,
		.start	= AR71XX_GE0_BASE,
		.end	= AR71XX_GE0_BASE + 0x200 - 1,
	}, {
		.name	= "mii_ctrl",
		.flags	= IORESOURCE_MEM,
		.start	= AR71XX_MII_BASE + MII_REG_MII0_CTRL,
		.end	= AR71XX_MII_BASE + MII_REG_MII0_CTRL + 3,
	}, {
		.name	= "mac_irq",
		.flags	= IORESOURCE_IRQ,
		.start	= AR71XX_CPU_IRQ_GE0,
		.end	= AR71XX_CPU_IRQ_GE0,
	},
};

struct ag71xx_platform_data ar71xx_eth0_data = {
	.reset_bit	= RESET_MODULE_GE0_MAC,
};

static struct platform_device ar71xx_eth0_device = {
	.name		= "ag71xx",
	.id		= 0,
	.resource	= ar71xx_eth0_resources,
	.num_resources	= ARRAY_SIZE(ar71xx_eth0_resources),
	.dev = {
		.platform_data = &ar71xx_eth0_data,
	},
};

static struct resource ar71xx_eth1_resources[] = {
	{
		.name	= "mac_base",
		.flags	= IORESOURCE_MEM,
		.start	= AR71XX_GE1_BASE,
		.end	= AR71XX_GE1_BASE + 0x200 - 1,
	}, {
		.name	= "mii_ctrl",
		.flags	= IORESOURCE_MEM,
		.start	= AR71XX_MII_BASE + MII_REG_MII1_CTRL,
		.end	= AR71XX_MII_BASE + MII_REG_MII1_CTRL + 3,
	}, {
		.name	= "mac_irq",
		.flags	= IORESOURCE_IRQ,
		.start	= AR71XX_CPU_IRQ_GE1,
		.end	= AR71XX_CPU_IRQ_GE1,
	},
};

struct ag71xx_platform_data ar71xx_eth1_data = {
	.reset_bit	= RESET_MODULE_GE1_MAC,
};

static struct platform_device ar71xx_eth1_device = {
	.name		= "ag71xx",
	.id		= 1,
	.resource	= ar71xx_eth1_resources,
	.num_resources	= ARRAY_SIZE(ar71xx_eth1_resources),
	.dev = {
		.platform_data = &ar71xx_eth1_data,
	},
};

#define AR71XX_PLL_VAL_1000	0x00110000
#define AR71XX_PLL_VAL_100	0x00001099
#define AR71XX_PLL_VAL_10	0x00991099

#define AR724X_PLL_VAL_1000	0x00110000
#define AR724X_PLL_VAL_100	0x00001099
#define AR724X_PLL_VAL_10	0x00991099

#define AR91XX_PLL_VAL_1000	0x1a000000
#define AR91XX_PLL_VAL_100	0x13000a44
#define AR91XX_PLL_VAL_10	0x00441099

static void __init ar71xx_init_eth_pll_data(unsigned int id)
{
	struct ar71xx_eth_pll_data *pll_data;
	u32 pll_10, pll_100, pll_1000;

	switch (id) {
	case 0:
		pll_data = &ar71xx_eth0_pll_data;
		break;
	case 1:
		pll_data = &ar71xx_eth1_pll_data;
		break;
	default:
		BUG();
	}

	switch (ar71xx_soc) {
	case AR71XX_SOC_AR7130:
	case AR71XX_SOC_AR7141:
	case AR71XX_SOC_AR7161:
		pll_10 = AR71XX_PLL_VAL_10;
		pll_100 = AR71XX_PLL_VAL_100;
		pll_1000 = AR71XX_PLL_VAL_1000;
		break;

	case AR71XX_SOC_AR7240:
		pll_10 = AR724X_PLL_VAL_10;
		pll_100 = AR724X_PLL_VAL_100;
		pll_1000 = AR724X_PLL_VAL_1000;
		break;

	case AR71XX_SOC_AR9130:
	case AR71XX_SOC_AR9132:
		pll_10 = AR91XX_PLL_VAL_10;
		pll_100 = AR91XX_PLL_VAL_100;
		pll_1000 = AR91XX_PLL_VAL_1000;
		break;
	default:
		BUG();
	}

	if (!pll_data->pll_10)
		pll_data->pll_10 = pll_10;

	if (!pll_data->pll_100)
		pll_data->pll_100 = pll_100;

	if (!pll_data->pll_1000)
		pll_data->pll_1000 = pll_1000;
}

static int ar71xx_eth_instance __initdata;
void __init ar71xx_add_device_eth(unsigned int id)
{
	struct platform_device *pdev;
	struct ag71xx_platform_data *pdata;

	ar71xx_init_eth_pll_data(id);

	switch (id) {
	case 0:
		switch (ar71xx_eth0_data.phy_if_mode) {
		case PHY_INTERFACE_MODE_MII:
			ar71xx_eth0_data.mii_if = MII0_CTRL_IF_MII;
			break;
		case PHY_INTERFACE_MODE_GMII:
			ar71xx_eth0_data.mii_if = MII0_CTRL_IF_GMII;
			break;
		case PHY_INTERFACE_MODE_RGMII:
			ar71xx_eth0_data.mii_if = MII0_CTRL_IF_RGMII;
			break;
		case PHY_INTERFACE_MODE_RMII:
			ar71xx_eth0_data.mii_if = MII0_CTRL_IF_RMII;
			break;
		default:
			printk(KERN_ERR "ar71xx: invalid PHY interface mode "
					"for eth0\n");
			return;
		}
		pdev = &ar71xx_eth0_device;
		break;
	case 1:
		switch (ar71xx_eth1_data.phy_if_mode) {
		case PHY_INTERFACE_MODE_RMII:
			ar71xx_eth1_data.mii_if = MII1_CTRL_IF_RMII;
			break;
		case PHY_INTERFACE_MODE_RGMII:
			ar71xx_eth1_data.mii_if = MII1_CTRL_IF_RGMII;
			break;
		default:
			printk(KERN_ERR "ar71xx: invalid PHY interface mode "
					"for eth1\n");
			return;
		}
		pdev = &ar71xx_eth1_device;
		break;
	default:
		printk(KERN_ERR "ar71xx: invalid ethernet id %d\n", id);
		return;
	}

	pdata = pdev->dev.platform_data;

	switch (ar71xx_soc) {
	case AR71XX_SOC_AR7130:
		pdata->ddr_flush = id ? ar71xx_ddr_flush_ge1
				      : ar71xx_ddr_flush_ge0;
		pdata->set_pll =  id ? ar71xx_set_pll_ge1
				     : ar71xx_set_pll_ge0;
		break;

	case AR71XX_SOC_AR7141:
	case AR71XX_SOC_AR7161:
		pdata->ddr_flush = id ? ar71xx_ddr_flush_ge1
				      : ar71xx_ddr_flush_ge0;
		pdata->set_pll =  id ? ar71xx_set_pll_ge1
				     : ar71xx_set_pll_ge0;
		pdata->has_gbit = 1;
		break;

	case AR71XX_SOC_AR7240:
		pdata->ddr_flush = id ? ar724x_ddr_flush_ge1
				      : ar724x_ddr_flush_ge0;
		pdata->set_pll =  id ? ar724x_set_pll_ge1
				     : ar724x_set_pll_ge0;
		pdata->is_ar724x = 1;
		break;

	case AR71XX_SOC_AR9130:
		pdata->ddr_flush = id ? ar91xx_ddr_flush_ge1
				      : ar91xx_ddr_flush_ge0;
		pdata->set_pll =  id ? ar91xx_set_pll_ge1
				     : ar91xx_set_pll_ge0;
		pdata->is_ar91xx = 1;
		break;

	case AR71XX_SOC_AR9132:
		pdata->ddr_flush = id ? ar91xx_ddr_flush_ge1
				      : ar91xx_ddr_flush_ge0;
		pdata->set_pll =  id ? ar91xx_set_pll_ge1
				      : ar91xx_set_pll_ge0;
		pdata->is_ar91xx = 1;
		pdata->has_gbit = 1;
		break;

	default:
		BUG();
	}

	switch (pdata->phy_if_mode) {
	case PHY_INTERFACE_MODE_GMII:
	case PHY_INTERFACE_MODE_RGMII:
		if (!pdata->has_gbit) {
			printk(KERN_ERR "ar71xx: no gbit available on eth%d\n",
					id);
			return;
		}
		/* fallthrough */
	default:
		break;
	}

	if (is_valid_ether_addr(ar71xx_mac_base)) {
		memcpy(pdata->mac_addr, ar71xx_mac_base, ETH_ALEN);
		pdata->mac_addr[5] += ar71xx_eth_instance;
	} else {
		random_ether_addr(pdata->mac_addr);
		printk(KERN_DEBUG
			"ar71xx: using random MAC address for eth%d\n",
			ar71xx_eth_instance);
	}

	/* Reset the device */
	ar71xx_device_stop(pdata->reset_bit);
	mdelay(100);

	ar71xx_device_start(pdata->reset_bit);
	mdelay(100);

	platform_device_register(pdev);
	ar71xx_eth_instance++;
}

static struct resource ar71xx_spi_resources[] = {
	[0] = {
		.start	= AR71XX_SPI_BASE,
		.end	= AR71XX_SPI_BASE + AR71XX_SPI_SIZE - 1,
		.flags	= IORESOURCE_MEM,
	},
};

static struct platform_device ar71xx_spi_device = {
	.name		= "ar71xx-spi",
	.id		= -1,
	.resource	= ar71xx_spi_resources,
	.num_resources	= ARRAY_SIZE(ar71xx_spi_resources),
};

void __init ar71xx_add_device_spi(struct ar71xx_spi_platform_data *pdata,
				struct spi_board_info const *info,
				unsigned n)
{
	spi_register_board_info(info, n);
	ar71xx_spi_device.dev.platform_data = pdata;
	platform_device_register(&ar71xx_spi_device);
}

void __init ar71xx_add_device_leds_gpio(int id, unsigned num_leds,
				struct gpio_led *leds)
{
	struct platform_device *pdev;
	struct gpio_led_platform_data pdata;
	struct gpio_led *p;
	int err;

	p = kmalloc(num_leds * sizeof(*p), GFP_KERNEL);
	if (!p)
		return;

	memcpy(p, leds, num_leds * sizeof(*p));

	pdev = platform_device_alloc("leds-gpio", id);
	if (!pdev)
		goto err_free_leds;

	memset(&pdata, 0, sizeof(pdata));
	pdata.num_leds = num_leds;
	pdata.leds = p;

	err = platform_device_add_data(pdev, &pdata, sizeof(pdata));
	if (err)
		goto err_put_pdev;

	err = platform_device_add(pdev);
	if (err)
		goto err_put_pdev;

	return;

err_put_pdev:
	platform_device_put(pdev);

err_free_leds:
	kfree(p);
}

void __init ar71xx_add_device_gpio_buttons(int id,
					   unsigned poll_interval,
					   unsigned nbuttons,
					   struct gpio_button *buttons)
{
	struct platform_device *pdev;
	struct gpio_buttons_platform_data pdata;
	struct gpio_button *p;
	int err;

	p = kmalloc(nbuttons * sizeof(*p), GFP_KERNEL);
	if (!p)
		return;

	memcpy(p, buttons, nbuttons * sizeof(*p));

	pdev = platform_device_alloc("gpio-buttons", id);
	if (!pdev)
		goto err_free_buttons;

	memset(&pdata, 0, sizeof(pdata));
	pdata.poll_interval = poll_interval;
	pdata.nbuttons = nbuttons;
	pdata.buttons = p;

	err = platform_device_add_data(pdev, &pdata, sizeof(pdata));
	if (err)
		goto err_put_pdev;


	err = platform_device_add(pdev);
	if (err)
		goto err_put_pdev;

	return;

err_put_pdev:
	platform_device_put(pdev);

err_free_buttons:
	kfree(p);
}

void __init ar71xx_add_device_wdt(void)
{
	platform_device_register_simple("ar71xx-wdt", -1, NULL, 0);
}

void __init ar71xx_set_mac_base(unsigned char *mac)
{
	memcpy(ar71xx_mac_base, mac, ETH_ALEN);
}

void __init ar71xx_parse_mac_addr(char *mac_str)
{
	u8 tmp[ETH_ALEN];
	int t;

	t = sscanf(mac_str, "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx",
			&tmp[0], &tmp[1], &tmp[2], &tmp[3], &tmp[4], &tmp[5]);

	if (t != ETH_ALEN)
		t = sscanf(mac_str, "%02hhx.%02hhx.%02hhx.%02hhx.%02hhx.%02hhx",
			&tmp[0], &tmp[1], &tmp[2], &tmp[3], &tmp[4], &tmp[5]);

	if (t == ETH_ALEN)
		ar71xx_set_mac_base(tmp);
	else
		printk(KERN_DEBUG "ar71xx: failed to parse mac address "
				"\"%s\"\n", mac_str);
}

static struct resource ar91xx_wmac_resources[] = {
	{
		.start	= AR91XX_WMAC_BASE,
		.end	= AR91XX_WMAC_BASE + AR91XX_WMAC_SIZE - 1,
		.flags	= IORESOURCE_MEM,
	}, {
		.start	= AR71XX_CPU_IRQ_WMAC,
		.end	= AR71XX_CPU_IRQ_WMAC,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct ath9k_platform_data ar91xx_wmac_data;

static struct platform_device ar91xx_wmac_device = {
	.name		= "ath9k",
	.id		= -1,
	.resource	= ar91xx_wmac_resources,
	.num_resources	= ARRAY_SIZE(ar91xx_wmac_resources),
	.dev = {
		.platform_data = &ar91xx_wmac_data,
	},
};

void __init ar91xx_add_device_wmac(void)
{
	u8 *ee = (u8 *) KSEG1ADDR(0x1fff1000);

	memcpy(ar91xx_wmac_data.eeprom_data, ee,
	       sizeof(ar91xx_wmac_data.eeprom_data));

	ar71xx_device_stop(RESET_MODULE_AMBA2WMAC);
	mdelay(10);

	ar71xx_device_start(RESET_MODULE_AMBA2WMAC);
	mdelay(10);

	platform_device_register(&ar91xx_wmac_device);
}

static struct platform_device ar71xx_dsa_switch_device = {
	.name		= "dsa",
	.id		= 0,
};

void __init ar71xx_add_device_dsa(unsigned int id,
				  struct dsa_platform_data *d)
{
	switch (id) {
	case 0:
		d->netdev = &ar71xx_eth0_device.dev;
		break;
	case 1:
		d->netdev = &ar71xx_eth1_device.dev;
		break;
	default:
		printk(KERN_ERR
			"ar71xx: invalid ethernet id %d for DSA switch\n",
			id);
		return;
	}
	d->mii_bus = &ar71xx_mdio_device.dev;
	ar71xx_dsa_switch_device.dev.platform_data = d;

	platform_device_register(&ar71xx_dsa_switch_device);
}
