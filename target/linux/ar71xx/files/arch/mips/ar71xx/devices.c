/*
 *  Atheros AR71xx SoC platform devices
 *
 *  Copyright (C) 2010-2011 Jaiganesh Narayanan <jnarayanan@atheros.com>
 *  Copyright (C) 2008-2009 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  Parts of this file are based on Atheros 2.6.15 BSP
 *  Parts of this file are based on Atheros 2.6.31 BSP
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/etherdevice.h>
#include <linux/platform_device.h>
#include <linux/serial_8250.h>

#include <asm/mach-ar71xx/ar71xx.h>
#include <asm/mach-ar71xx/ar933x_uart_platform.h>

#include "devices.h"

unsigned char ar71xx_mac_base[ETH_ALEN] __initdata;

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

static struct resource ar933x_uart_resources[] = {
	{
		.start  = AR933X_UART_BASE,
		.end    = AR933X_UART_BASE + AR71XX_UART_SIZE - 1,
		.flags  = IORESOURCE_MEM,
	},
	{
		.start  = AR71XX_MISC_IRQ_UART,
		.end    = AR71XX_MISC_IRQ_UART,
		.flags  = IORESOURCE_IRQ,
	},
};

static struct ar933x_uart_platform_data ar933x_uart_data;
static struct platform_device ar933x_uart_device = {
	.name           = "ar933x-uart",
	.id             = -1,
	.resource       = ar933x_uart_resources,
	.num_resources  = ARRAY_SIZE(ar933x_uart_resources),
	.dev = {
		.platform_data  = &ar933x_uart_data,
	},
};

void __init ar71xx_add_device_uart(void)
{
	struct platform_device *pdev;

	switch (ar71xx_soc) {
	case AR71XX_SOC_AR7130:
	case AR71XX_SOC_AR7141:
	case AR71XX_SOC_AR7161:
	case AR71XX_SOC_AR7240:
	case AR71XX_SOC_AR7241:
	case AR71XX_SOC_AR7242:
	case AR71XX_SOC_AR9130:
	case AR71XX_SOC_AR9132:
		pdev = &ar71xx_uart_device;
		ar71xx_uart_data[0].uartclk = ar71xx_ahb_freq;
		break;

	case AR71XX_SOC_AR9330:
	case AR71XX_SOC_AR9331:
		pdev = &ar933x_uart_device;
		ar933x_uart_data.uartclk = ar71xx_ref_freq;
		break;

	case AR71XX_SOC_AR9341:
	case AR71XX_SOC_AR9342:
	case AR71XX_SOC_AR9344:
		pdev = &ar71xx_uart_device;
		ar71xx_uart_data[0].uartclk = ar71xx_ref_freq;
		break;

	default:
		BUG();
	}

	platform_device_register(pdev);
}

static struct resource ar71xx_mdio0_resources[] = {
	{
		.name	= "mdio_base",
		.flags	= IORESOURCE_MEM,
		.start	= AR71XX_GE0_BASE,
		.end	= AR71XX_GE0_BASE + 0x200 - 1,
	}
};

static struct ag71xx_mdio_platform_data ar71xx_mdio0_data;

struct platform_device ar71xx_mdio0_device = {
	.name		= "ag71xx-mdio",
	.id		= 0,
	.resource	= ar71xx_mdio0_resources,
	.num_resources	= ARRAY_SIZE(ar71xx_mdio0_resources),
	.dev = {
		.platform_data = &ar71xx_mdio0_data,
	},
};

static struct resource ar71xx_mdio1_resources[] = {
	{
		.name	= "mdio_base",
		.flags	= IORESOURCE_MEM,
		.start	= AR71XX_GE1_BASE,
		.end	= AR71XX_GE1_BASE + 0x200 - 1,
	}
};

static struct ag71xx_mdio_platform_data ar71xx_mdio1_data;

struct platform_device ar71xx_mdio1_device = {
	.name		= "ag71xx-mdio",
	.id		= 1,
	.resource	= ar71xx_mdio1_resources,
	.num_resources	= ARRAY_SIZE(ar71xx_mdio1_resources),
	.dev = {
		.platform_data = &ar71xx_mdio1_data,
	},
};

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

static void __init ar71xx_mii_ctrl_set_if(unsigned int reg,
					  unsigned int mii_if)
{
	void __iomem *base;
	u32 t;

	base = ioremap(AR71XX_MII_BASE, AR71XX_MII_SIZE);

	t = __raw_readl(base + reg);
	t &= ~(MII_CTRL_IF_MASK);
	t |= (mii_if & MII_CTRL_IF_MASK);
	__raw_writel(t, base + reg);

	iounmap(base);
}

static void ar71xx_mii_ctrl_set_speed(unsigned int reg, unsigned int speed)
{
	void __iomem *base;
	unsigned int mii_speed;
	u32 t;

	switch (speed) {
	case SPEED_10:
		mii_speed =  MII_CTRL_SPEED_10;
		break;
	case SPEED_100:
		mii_speed =  MII_CTRL_SPEED_100;
		break;
	case SPEED_1000:
		mii_speed =  MII_CTRL_SPEED_1000;
		break;
	default:
		BUG();
	}

	base = ioremap(AR71XX_MII_BASE, AR71XX_MII_SIZE);

	t = __raw_readl(base + reg);
	t &= ~(MII_CTRL_SPEED_MASK << MII_CTRL_SPEED_SHIFT);
	t |= mii_speed  << MII_CTRL_SPEED_SHIFT;
	__raw_writel(t, base + reg);

	iounmap(base);
}

void __init ar71xx_add_device_mdio(unsigned int id, u32 phy_mask)
{
	struct platform_device *mdio_dev;
	struct ag71xx_mdio_platform_data *mdio_data;
	unsigned int max_id;

	if (ar71xx_soc == AR71XX_SOC_AR9341 ||
	    ar71xx_soc == AR71XX_SOC_AR9342 ||
	    ar71xx_soc == AR71XX_SOC_AR9344)
		max_id = 1;
	else
		max_id = 0;

	if (id > max_id) {
		printk(KERN_ERR "ar71xx: invalid MDIO id %u\n", id);
		return;
	}

	switch (ar71xx_soc) {
	case AR71XX_SOC_AR7241:
	case AR71XX_SOC_AR9330:
	case AR71XX_SOC_AR9331:
		mdio_dev = &ar71xx_mdio1_device;
		mdio_data = &ar71xx_mdio1_data;
		break;

	case AR71XX_SOC_AR9341:
	case AR71XX_SOC_AR9342:
	case AR71XX_SOC_AR9344:
		if (id == 0) {
			mdio_dev = &ar71xx_mdio0_device;
			mdio_data = &ar71xx_mdio0_data;
		} else {
			mdio_dev = &ar71xx_mdio1_device;
			mdio_data = &ar71xx_mdio1_data;
		}
		break;

	case AR71XX_SOC_AR7242:
		ar71xx_set_pll(AR71XX_PLL_REG_SEC_CONFIG,
			       AR7242_PLL_REG_ETH0_INT_CLOCK, 0x62000000,
			       AR71XX_ETH0_PLL_SHIFT);
		/* fall through */
	default:
		mdio_dev = &ar71xx_mdio0_device;
		mdio_data = &ar71xx_mdio0_data;
		break;
	}

	mdio_data->phy_mask = phy_mask;

	switch (ar71xx_soc) {
	case AR71XX_SOC_AR7240:
	case AR71XX_SOC_AR7241:
	case AR71XX_SOC_AR9330:
	case AR71XX_SOC_AR9331:
		mdio_data->is_ar7240 = 1;
		break;

	case AR71XX_SOC_AR9341:
	case AR71XX_SOC_AR9342:
	case AR71XX_SOC_AR9344:
		if (id == 1)
			mdio_data->is_ar7240 = 1;
		break;

	default:
		break;
	}

	platform_device_register(mdio_dev);
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

static void ar71xx_set_speed_ge0(int speed)
{
	u32 val = ar71xx_get_eth_pll(0, speed);

	ar71xx_set_pll(AR71XX_PLL_REG_SEC_CONFIG, AR71XX_PLL_REG_ETH0_INT_CLOCK,
			val, AR71XX_ETH0_PLL_SHIFT);
	ar71xx_mii_ctrl_set_speed(MII_REG_MII0_CTRL, speed);
}

static void ar71xx_set_speed_ge1(int speed)
{
	u32 val = ar71xx_get_eth_pll(1, speed);

	ar71xx_set_pll(AR71XX_PLL_REG_SEC_CONFIG, AR71XX_PLL_REG_ETH1_INT_CLOCK,
			 val, AR71XX_ETH1_PLL_SHIFT);
	ar71xx_mii_ctrl_set_speed(MII_REG_MII1_CTRL, speed);
}

static void ar724x_set_speed_ge0(int speed)
{
	/* TODO */
}

static void ar724x_set_speed_ge1(int speed)
{
	/* TODO */
}

static void ar7242_set_speed_ge0(int speed)
{
	u32 val = ar71xx_get_eth_pll(0, speed);
	void __iomem *base;

	base = ioremap_nocache(AR71XX_PLL_BASE, AR71XX_PLL_SIZE);
	__raw_writel(val, base + AR7242_PLL_REG_ETH0_INT_CLOCK);
	iounmap(base);
}

static void ar91xx_set_speed_ge0(int speed)
{
	u32 val = ar71xx_get_eth_pll(0, speed);

	ar71xx_set_pll(AR91XX_PLL_REG_ETH_CONFIG, AR91XX_PLL_REG_ETH0_INT_CLOCK,
			 val, AR91XX_ETH0_PLL_SHIFT);
	ar71xx_mii_ctrl_set_speed(MII_REG_MII0_CTRL, speed);
}

static void ar91xx_set_speed_ge1(int speed)
{
	u32 val = ar71xx_get_eth_pll(1, speed);

	ar71xx_set_pll(AR91XX_PLL_REG_ETH_CONFIG, AR91XX_PLL_REG_ETH1_INT_CLOCK,
			 val, AR91XX_ETH1_PLL_SHIFT);
	ar71xx_mii_ctrl_set_speed(MII_REG_MII1_CTRL, speed);
}

static void ar933x_set_speed_ge0(int speed)
{
	/* TODO */
}

static void ar933x_set_speed_ge1(int speed)
{
	/* TODO */
}

static void ar934x_set_speed_ge0(int speed)
{
	/* TODO */
}

static void ar934x_set_speed_ge1(int speed)
{
	/* TODO */
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

static void ar933x_ddr_flush_ge0(void)
{
	ar71xx_ddr_flush(AR933X_DDR_REG_FLUSH_GE0);
}

static void ar933x_ddr_flush_ge1(void)
{
	ar71xx_ddr_flush(AR933X_DDR_REG_FLUSH_GE1);
}

static void ar934x_ddr_flush_ge0(void)
{
	ar71xx_ddr_flush(AR934X_DDR_REG_FLUSH_GE0);
}

static void ar934x_ddr_flush_ge1(void)
{
	ar71xx_ddr_flush(AR934X_DDR_REG_FLUSH_GE1);
}

static struct resource ar71xx_eth0_resources[] = {
	{
		.name	= "mac_base",
		.flags	= IORESOURCE_MEM,
		.start	= AR71XX_GE0_BASE,
		.end	= AR71XX_GE0_BASE + 0x200 - 1,
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

struct platform_device ar71xx_eth0_device = {
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
		.name	= "mac_irq",
		.flags	= IORESOURCE_IRQ,
		.start	= AR71XX_CPU_IRQ_GE1,
		.end	= AR71XX_CPU_IRQ_GE1,
	},
};

struct ag71xx_platform_data ar71xx_eth1_data = {
	.reset_bit	= RESET_MODULE_GE1_MAC,
};

struct platform_device ar71xx_eth1_device = {
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

#define AR7242_PLL_VAL_1000	0x16000000
#define AR7242_PLL_VAL_100	0x00000101
#define AR7242_PLL_VAL_10	0x00001616

#define AR91XX_PLL_VAL_1000	0x1a000000
#define AR91XX_PLL_VAL_100	0x13000a44
#define AR91XX_PLL_VAL_10	0x00441099

#define AR933X_PLL_VAL_1000	0x00110000
#define AR933X_PLL_VAL_100	0x00001099
#define AR933X_PLL_VAL_10	0x00991099

#define AR934X_PLL_VAL_1000	0x00110000
#define AR934X_PLL_VAL_100	0x00001099
#define AR934X_PLL_VAL_10	0x00991099

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
	case AR71XX_SOC_AR7241:
		pll_10 = AR724X_PLL_VAL_10;
		pll_100 = AR724X_PLL_VAL_100;
		pll_1000 = AR724X_PLL_VAL_1000;
		break;

	case AR71XX_SOC_AR7242:
		pll_10 = AR7242_PLL_VAL_10;
		pll_100 = AR7242_PLL_VAL_100;
		pll_1000 = AR7242_PLL_VAL_1000;
		break;

	case AR71XX_SOC_AR9130:
	case AR71XX_SOC_AR9132:
		pll_10 = AR91XX_PLL_VAL_10;
		pll_100 = AR91XX_PLL_VAL_100;
		pll_1000 = AR91XX_PLL_VAL_1000;
		break;

	case AR71XX_SOC_AR9330:
	case AR71XX_SOC_AR9331:
		pll_10 = AR933X_PLL_VAL_10;
		pll_100 = AR933X_PLL_VAL_100;
		pll_1000 = AR933X_PLL_VAL_1000;
		break;

	case AR71XX_SOC_AR9341:
	case AR71XX_SOC_AR9342:
	case AR71XX_SOC_AR9344:
		pll_10 = AR934X_PLL_VAL_10;
		pll_100 = AR934X_PLL_VAL_100;
		pll_1000 = AR934X_PLL_VAL_1000;
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

static int __init ar71xx_setup_phy_if_mode(unsigned int id,
					   struct ag71xx_platform_data *pdata)
{
	unsigned int mii_if;

	switch (id) {
	case 0:
		switch (ar71xx_soc) {
		case AR71XX_SOC_AR7130:
		case AR71XX_SOC_AR7141:
		case AR71XX_SOC_AR7161:
		case AR71XX_SOC_AR9130:
		case AR71XX_SOC_AR9132:
			switch (pdata->phy_if_mode) {
			case PHY_INTERFACE_MODE_MII:
				mii_if = MII0_CTRL_IF_MII;
				break;
			case PHY_INTERFACE_MODE_GMII:
				mii_if = MII0_CTRL_IF_GMII;
				break;
			case PHY_INTERFACE_MODE_RGMII:
				mii_if = MII0_CTRL_IF_RGMII;
				break;
			case PHY_INTERFACE_MODE_RMII:
				mii_if = MII0_CTRL_IF_RMII;
				break;
			default:
				return -EINVAL;
			}
			ar71xx_mii_ctrl_set_if(MII_REG_MII0_CTRL, mii_if);
			break;

		case AR71XX_SOC_AR7240:
		case AR71XX_SOC_AR7241:
		case AR71XX_SOC_AR9330:
		case AR71XX_SOC_AR9331:
			pdata->phy_if_mode = PHY_INTERFACE_MODE_MII;
			break;

		case AR71XX_SOC_AR7242:
			/* FIXME */

		case AR71XX_SOC_AR9341:
		case AR71XX_SOC_AR9342:
		case AR71XX_SOC_AR9344:
			switch (pdata->phy_if_mode) {
			case PHY_INTERFACE_MODE_MII:
			case PHY_INTERFACE_MODE_GMII:
			case PHY_INTERFACE_MODE_RGMII:
			case PHY_INTERFACE_MODE_RMII:
				break;
			default:
				return -EINVAL;
			}
			break;

		default:
			BUG();
		}
		break;
	case 1:
		switch (ar71xx_soc) {
		case AR71XX_SOC_AR7130:
		case AR71XX_SOC_AR7141:
		case AR71XX_SOC_AR7161:
		case AR71XX_SOC_AR9130:
		case AR71XX_SOC_AR9132:
			switch (pdata->phy_if_mode) {
			case PHY_INTERFACE_MODE_RMII:
				mii_if = MII1_CTRL_IF_RMII;
				break;
			case PHY_INTERFACE_MODE_RGMII:
				mii_if = MII1_CTRL_IF_RGMII;
				break;
			default:
				return -EINVAL;
			}
			ar71xx_mii_ctrl_set_if(MII_REG_MII1_CTRL, mii_if);
			break;

		case AR71XX_SOC_AR7240:
		case AR71XX_SOC_AR7241:
		case AR71XX_SOC_AR9330:
		case AR71XX_SOC_AR9331:
			pdata->phy_if_mode = PHY_INTERFACE_MODE_GMII;
			break;

		case AR71XX_SOC_AR7242:
			/* FIXME */

		case AR71XX_SOC_AR9341:
		case AR71XX_SOC_AR9342:
		case AR71XX_SOC_AR9344:
			switch (pdata->phy_if_mode) {
			case PHY_INTERFACE_MODE_MII:
			case PHY_INTERFACE_MODE_GMII:
				break;
			default:
				return -EINVAL;
			}
			break;

		default:
			BUG();
		}
		break;
	}

	return 0;
}

static int ar71xx_eth_instance __initdata;
void __init ar71xx_add_device_eth(unsigned int id)
{
	struct platform_device *pdev;
	struct ag71xx_platform_data *pdata;
	int err;

	if (id > 1) {
		printk(KERN_ERR "ar71xx: invalid ethernet id %d\n", id);
		return;
	}

	ar71xx_init_eth_pll_data(id);

	if (id == 0)
		pdev = &ar71xx_eth0_device;
	else
		pdev = &ar71xx_eth1_device;

	pdata = pdev->dev.platform_data;

	err = ar71xx_setup_phy_if_mode(id, pdata);
	if (err) {
		printk(KERN_ERR
		       "ar71xx: invalid PHY interface mode for GE%u\n", id);
		return;
	}

	switch (ar71xx_soc) {
	case AR71XX_SOC_AR7130:
		if (id == 0) {
			pdata->ddr_flush = ar71xx_ddr_flush_ge0;
			pdata->set_speed = ar71xx_set_speed_ge0;
		} else {
			pdata->ddr_flush = ar71xx_ddr_flush_ge1;
			pdata->set_speed = ar71xx_set_speed_ge1;
		}
		break;

	case AR71XX_SOC_AR7141:
	case AR71XX_SOC_AR7161:
		if (id == 0) {
			pdata->ddr_flush = ar71xx_ddr_flush_ge0;
			pdata->set_speed = ar71xx_set_speed_ge0;
		} else {
			pdata->ddr_flush = ar71xx_ddr_flush_ge1;
			pdata->set_speed = ar71xx_set_speed_ge1;
		}
		pdata->has_gbit = 1;
		break;

	case AR71XX_SOC_AR7242:
		if (id == 0) {
			pdata->reset_bit |= AR724X_RESET_GE0_MDIO |
					    RESET_MODULE_GE0_PHY;
			pdata->ddr_flush = ar724x_ddr_flush_ge0;
			pdata->set_speed = ar7242_set_speed_ge0;
		} else {
			pdata->reset_bit |= AR724X_RESET_GE1_MDIO |
					    RESET_MODULE_GE1_PHY;
			pdata->ddr_flush = ar724x_ddr_flush_ge1;
			pdata->set_speed = ar724x_set_speed_ge1;
		}
		pdata->has_gbit = 1;
		pdata->is_ar724x = 1;

		if (!pdata->fifo_cfg1)
			pdata->fifo_cfg1 = 0x0010ffff;
		if (!pdata->fifo_cfg2)
			pdata->fifo_cfg2 = 0x015500aa;
		if (!pdata->fifo_cfg3)
			pdata->fifo_cfg3 = 0x01f00140;
		break;

	case AR71XX_SOC_AR7241:
		if (id == 0)
			pdata->reset_bit |= AR724X_RESET_GE0_MDIO;
		else
			pdata->reset_bit |= AR724X_RESET_GE1_MDIO;
		/* fall through */
	case AR71XX_SOC_AR7240:
		if (id == 0) {
			pdata->reset_bit |= RESET_MODULE_GE0_PHY;
			pdata->ddr_flush = ar724x_ddr_flush_ge0;
			pdata->set_speed = ar724x_set_speed_ge0;

			pdata->phy_mask = BIT(4);
		} else {
			pdata->reset_bit |= RESET_MODULE_GE1_PHY;
			pdata->ddr_flush = ar724x_ddr_flush_ge1;
			pdata->set_speed = ar724x_set_speed_ge1;

			pdata->speed = SPEED_1000;
			pdata->duplex = DUPLEX_FULL;
			pdata->has_ar7240_switch = 1;
		}
		pdata->has_gbit = 1;
		pdata->is_ar724x = 1;
		if (ar71xx_soc == AR71XX_SOC_AR7240)
			pdata->is_ar7240 = 1;

		if (!pdata->fifo_cfg1)
			pdata->fifo_cfg1 = 0x0010ffff;
		if (!pdata->fifo_cfg2)
			pdata->fifo_cfg2 = 0x015500aa;
		if (!pdata->fifo_cfg3)
			pdata->fifo_cfg3 = 0x01f00140;
		break;

	case AR71XX_SOC_AR9130:
		if (id == 0) {
			pdata->ddr_flush = ar91xx_ddr_flush_ge0;
			pdata->set_speed = ar91xx_set_speed_ge0;
		} else {
			pdata->ddr_flush = ar91xx_ddr_flush_ge1;
			pdata->set_speed = ar91xx_set_speed_ge1;
		}
		pdata->is_ar91xx = 1;
		break;

	case AR71XX_SOC_AR9132:
		if (id == 0) {
			pdata->ddr_flush = ar91xx_ddr_flush_ge0;
			pdata->set_speed = ar91xx_set_speed_ge0;
		} else {
			pdata->ddr_flush = ar91xx_ddr_flush_ge1;
			pdata->set_speed = ar91xx_set_speed_ge1;
		}
		pdata->is_ar91xx = 1;
		pdata->has_gbit = 1;
		break;

	case AR71XX_SOC_AR9330:
	case AR71XX_SOC_AR9331:
		if (id == 0) {
			pdata->reset_bit = AR933X_RESET_GE0_MAC |
					   AR933X_RESET_GE0_MDIO;
			pdata->ddr_flush = ar933x_ddr_flush_ge0;
			pdata->set_speed = ar933x_set_speed_ge0;
		} else {
			pdata->reset_bit = AR933X_RESET_GE1_MAC |
					   AR933X_RESET_GE1_MDIO;
			pdata->ddr_flush = ar933x_ddr_flush_ge1;
			pdata->set_speed = ar933x_set_speed_ge1;

			pdata->speed = SPEED_1000;
			pdata->duplex = DUPLEX_FULL;
			pdata->has_ar7240_switch = 1;
		}

		pdata->has_gbit = 1;
		pdata->is_ar724x = 1;

		if (!pdata->fifo_cfg1)
			pdata->fifo_cfg1 = 0x0010ffff;
		if (!pdata->fifo_cfg2)
			pdata->fifo_cfg2 = 0x015500aa;
		if (!pdata->fifo_cfg3)
			pdata->fifo_cfg3 = 0x01f00140;
		break;

	case AR71XX_SOC_AR9341:
	case AR71XX_SOC_AR9342:
	case AR71XX_SOC_AR9344:
		if (id == 0) {
			pdata->reset_bit = AR934X_RESET_GE0_MAC |
					   AR934X_RESET_GE0_MDIO;
			pdata->ddr_flush =ar934x_ddr_flush_ge0;
			pdata->set_speed = ar934x_set_speed_ge0;
		} else {
			pdata->reset_bit = AR934X_RESET_GE1_MAC |
					   AR934X_RESET_GE1_MDIO;
			pdata->ddr_flush = ar934x_ddr_flush_ge1;
			pdata->set_speed = ar934x_set_speed_ge1;
		}

		pdata->has_gbit = 1;
		pdata->is_ar724x = 1;

		if (!pdata->fifo_cfg1)
			pdata->fifo_cfg1 = 0x0010ffff;
		if (!pdata->fifo_cfg2)
			pdata->fifo_cfg2 = 0x015500aa;
		if (!pdata->fifo_cfg3)
			pdata->fifo_cfg3 = 0x01f00140;
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

	if (!is_valid_ether_addr(pdata->mac_addr)) {
		random_ether_addr(pdata->mac_addr);
		printk(KERN_DEBUG
			"ar71xx: using random MAC address for eth%d\n",
			ar71xx_eth_instance);
	}

	if (pdata->mii_bus_dev == NULL) {
		switch (ar71xx_soc) {
		case AR71XX_SOC_AR9341:
		case AR71XX_SOC_AR9342:
		case AR71XX_SOC_AR9344:
			if (id == 0)
				pdata->mii_bus_dev = &ar71xx_mdio0_device.dev;
			else
				pdata->mii_bus_dev = &ar71xx_mdio1_device.dev;
			break;

		case AR71XX_SOC_AR7241:
		case AR71XX_SOC_AR9330:
		case AR71XX_SOC_AR9331:
			pdata->mii_bus_dev = &ar71xx_mdio1_device.dev;
			break;

		default:
			pdata->mii_bus_dev = &ar71xx_mdio0_device.dev;
			break;
		}
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

static int __init ar71xx_ethaddr_setup(char *str)
{
	ar71xx_parse_mac_addr(str);
	return 1;
}
__setup("ethaddr=", ar71xx_ethaddr_setup);

static int __init ar71xx_kmac_setup(char *str)
{
	ar71xx_parse_mac_addr(str);
	return 1;
}
__setup("kmac=", ar71xx_kmac_setup);

void __init ar71xx_init_mac(unsigned char *dst, const unsigned char *src,
			    unsigned offset)
{
	u32 t;

	if (!is_valid_ether_addr(src)) {
		memset(dst, '\0', ETH_ALEN);
		return;
	}

	t = (((u32) src[3]) << 16) + (((u32) src[4]) << 8) + ((u32) src[5]);
	t += offset;

	dst[0] = src[0];
	dst[1] = src[1];
	dst[2] = src[2];
	dst[3] = (t >> 16) & 0xff;
	dst[4] = (t >> 8) & 0xff;
	dst[5] = t & 0xff;
}
