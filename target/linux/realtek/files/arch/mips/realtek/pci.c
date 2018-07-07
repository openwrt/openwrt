/*
 *  Realtek RLX based SoC PCI bus controller initialization
 *
 *  Copyright (C) 2017 Weijie Gao <hackpascal@gmail.com>
 *
 *  Parts of this file are based on Linux kernel of Realtek RSDK 1.3.6
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/delay.h>
#include <linux/platform_device.h>

#include <asm/mach-realtek/realtek.h>
#include <asm/mach-realtek/platform.h>
#include "common.h"
#include "irq.h"

static struct resource realtek_pcie0_resource[] = {
	{
		.name	= "rc_cfg_base",
		.flags	= IORESOURCE_MEM,
		.start	= REALTEK_PCIE0_RC_CFG_BASE,
		.end	= REALTEK_PCIE0_RC_CFG_BASE + REALTEK_PCIE0_RC_CFG_SIZE - 1,
	},
	{
		.name	= "dev_cfg_base",
		.flags	= IORESOURCE_MEM,
		.start	= REALTEK_PCIE0_EP_CFG_BASE,
		.end	= REALTEK_PCIE0_EP_CFG_BASE + REALTEK_PCIE0_EP_CFG_SIZE - 1,
	},
	{
		.name	= "io_base",
		.flags	= IORESOURCE_IO,
		.start	= REALTEK_PCIE0_IO_BASE,
		.end	= REALTEK_PCIE0_IO_BASE + REALTEK_PCIE0_IO_SIZE - 1,
	},
	{
		.name	= "mem_base",
		.flags	= IORESOURCE_MEM,
		.start	= REALTEK_PCIE0_MEM_BASE,
		.end	= REALTEK_PCIE0_MEM_BASE + REALTEK_PCIE0_MEM_SIZE - 1,
	}
};

static struct resource realtek_pcie1_resource[] = {
	{
		.name	= "rc_cfg_base",
		.flags	= IORESOURCE_MEM,
		.start	= REALTEK_PCIE1_RC_CFG_BASE,
		.end	= REALTEK_PCIE1_RC_CFG_BASE + REALTEK_PCIE1_RC_CFG_SIZE - 1,
	},
	{
		.name	= "dev_cfg_base",
		.flags	= IORESOURCE_MEM,
		.start	= REALTEK_PCIE1_EP_CFG_BASE,
		.end	= REALTEK_PCIE1_EP_CFG_BASE + REALTEK_PCIE1_EP_CFG_SIZE - 1,
	},
	{
		.name	= "io_base",
		.flags	= IORESOURCE_IO,
		.start	= REALTEK_PCIE1_IO_BASE,
		.end	= REALTEK_PCIE1_IO_BASE + REALTEK_PCIE1_IO_SIZE - 1,
	},
	{
		.name	= "mem_base",
		.flags	= IORESOURCE_MEM,
		.start	= REALTEK_PCIE1_MEM_BASE,
		.end	= REALTEK_PCIE1_MEM_BASE + REALTEK_PCIE1_MEM_SIZE - 1,
	}
};

struct realtek_pcie_reset_controller {
	void __iomem *rc_ext_base;
	u32 rc_phy_reg;
};

static void __init realtek_pcie_mdio_write(struct realtek_pcie_reset_controller *rprc, u32 reg, u32 data)
{
	__raw_writel(
		((reg & REALTEK_PCIE_RC_EXT_MDIO_REGADDR_MASK) << REALTEK_PCIE_RC_EXT_MDIO_REGADDR_SHIFT) |
		((data & REALTEK_PCIE_RC_EXT_MDIO_DATA_MASK) << REALTEK_PCIE_RC_EXT_MDIO_DATA_SHIFT) |
		REALTEK_PCIE_RC_EXT_MDIO_WR,
		rprc->rc_ext_base + REALTEK_PCIE_RC_EXT_REG_MDIO);

	mdelay(1);
}

static void __init realtek_pcie_mdio_reset(struct realtek_pcie_reset_controller *rprc)
{
	realtek_sys_write(rprc->rc_phy_reg, REALTEK_SYS_PCIE_PHY_UNK3);
	realtek_sys_write(rprc->rc_phy_reg, REALTEK_SYS_PCIE_PHY_UNK3 | REALTEK_SYS_PCIE_PHY_RESET_L);
	realtek_sys_write(rprc->rc_phy_reg, REALTEK_SYS_PCIE_PHY_UNK3 | REALTEK_SYS_PCIE_PHY_LOAD_DONE | REALTEK_SYS_PCIE_PHY_RESET_L);
}

static void __init realtek_pcie_phy_reset(struct realtek_pcie_reset_controller *rprc)
{
	__raw_writel(REALTEK_PCIE_RC_EXT_PWR_APP_LTSSM_EN, rprc->rc_ext_base + REALTEK_PCIE_RC_EXT_REG_PWR_CR);
	__raw_writel(REALTEK_PCIE_RC_EXT_PWR_PHY_SRST_L | REALTEK_PCIE_RC_EXT_PWR_APP_LTSSM_EN, rprc->rc_ext_base + REALTEK_PCIE_RC_EXT_REG_PWR_CR);
}

static void __init rtl8196c_pcie_reset(struct realtek_pcie_reset_controller *rprc, int pcie_xtal_40mhz)
{
	u32 val;

	/* Enable PCIe controller */
	val = realtek_sys_read(REALTEK_SYS_REG_CLK_MANAGE);
	val |= RTL8196C_SYS_CLK_PCIE0_EN;
	realtek_sys_write(REALTEK_SYS_REG_CLK_MANAGE, val);

#if 0
	/* Is it OK not to set this? */
	__raw_writel((1 << REALTEK_PCIE_RC_EXT_DEV_NUM_SHIFT), rprc->rc_ext_base + REALTEK_PCIE_RC_EXT_REG_IP_CFG);
#endif

	realtek_pcie_mdio_reset(rprc);
	realtek_pcie_phy_reset(rprc);

	realtek_pcie_mdio_write(rprc, 0x00, 0xD087);
	realtek_pcie_mdio_write(rprc, 0x01, 0x0003);
	realtek_pcie_mdio_write(rprc, 0x02, 0x4d18);

	if (pcie_xtal_40mhz) {
		realtek_pcie_mdio_write(rprc, 0x05, 0x0BCB);
		realtek_pcie_mdio_write(rprc, 0x06, 0xF148);
	} else {
		realtek_pcie_mdio_write(rprc, 0x06, 0xf848);
	}

	realtek_pcie_mdio_write(rprc, 0x07, 0x31ff);
	realtek_pcie_mdio_write(rprc, 0x08, 0x18d7);
	realtek_pcie_mdio_write(rprc, 0x09, 0x539c);
	realtek_pcie_mdio_write(rprc, 0x0a, 0x20eb);
	realtek_pcie_mdio_write(rprc, 0x0d, 0x1764);
	realtek_pcie_mdio_write(rprc, 0x0b, 0x0511);
	realtek_pcie_mdio_write(rprc, 0x0f, 0x0a00);
	realtek_pcie_mdio_write(rprc, 0x19, 0xFCE0); 
	realtek_pcie_mdio_write(rprc, 0x1e, 0xC280);

	/* Reset PCIe device */
	val = realtek_sys_read(REALTEK_SYS_REG_CLK_MANAGE);
	val &= ~RTL8196C_SYS_CLK_PCIE0_DEV_RST_L;
	realtek_sys_write(REALTEK_SYS_REG_CLK_MANAGE, val);
	mdelay(1);

	val |= RTL8196C_SYS_CLK_PCIE0_DEV_RST_L;
	realtek_sys_write(REALTEK_SYS_REG_CLK_MANAGE, val);
	mdelay(1);

	realtek_pcie_phy_reset(rprc);
}

int pcibios_map_irq(const struct pci_dev *dev, u8 slot, u8 pin)
{
	switch (dev->bus->number) {
	case 0:
		if (soc_is_rtl8196c())
			return REALTEK_SOC_IRQ(5);
		break;
	}

	return -1;
}

int pcibios_plat_dev_init(struct pci_dev *dev)
{
	return 0;
}

static void __init __realtek_pci_register(int n)
{
	struct platform_device *pdev;

	switch (n) {
	case 0:
		pdev = platform_device_register_simple("realtek-pci", 0,
					       realtek_pcie0_resource, ARRAY_SIZE(realtek_pcie0_resource));
		break;
	case 1:
		pdev = platform_device_register_simple("realtek-pci", 1,
					       realtek_pcie1_resource, ARRAY_SIZE(realtek_pcie1_resource));
		break;
	default:
		return;
	}
}

void __init realtek_register_pci(void)
{
	struct realtek_pcie_reset_controller rprc0, rprc1;

	/* Reference: https://www.linux-mips.org/wiki/PCI_Subsystem#I.2FO_ports_in_PCI */
	set_io_port_base(KSEG1);
	ioport_resource.end = 0xffffffff;

	rprc0.rc_ext_base = ioremap_nocache(REALTEK_PCIE0_RC_EXT_BASE, REALTEK_PCIE0_RC_EXT_SIZE);
	rprc0.rc_phy_reg = REALTEK_SYS_REG_PCIE0_PHY;

	rprc1.rc_ext_base = ioremap_nocache(REALTEK_PCIE1_RC_EXT_BASE, REALTEK_PCIE1_RC_EXT_SIZE);
	rprc1.rc_phy_reg = REALTEK_SYS_REG_PCIE1_PHY;

	if (soc_is_rtl8196c()) {
		rtl8196c_pcie_reset(&rprc0, 1);
		__realtek_pci_register(0);
	} else
		BUG();

	iounmap(rprc0.rc_ext_base);
	iounmap(rprc1.rc_ext_base);
}
