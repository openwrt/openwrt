/*
 *  Realtek RLX based SoC PCI host controller driver
 *
 *  Copyright (C) 2017 Weijie Gao <hackpascal@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/pci.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/platform_device.h>

#include <asm/c-lexra.h>

#include <asm/mach-realtek/realtek.h>
#include <asm/mach-realtek/platform.h>

struct realtek_pci_controller {
	void __iomem *rc_cfg_base;
	void __iomem *dev_cfg_base;

	int link_up;

	struct pci_controller pci_controller;
	struct resource io_res;
	struct resource mem_res;
};

static inline struct realtek_pci_controller *
pci_bus_to_realtek_pci_controller(struct pci_bus *bus)
{
	struct pci_controller *hose;

	hose = (struct pci_controller *) bus->sysdata;
	return container_of(hose, struct realtek_pci_controller, pci_controller);
}

static __IRAM int realtek_pci_raw_read(void __iomem *base, int where, int size, u32 *value)
{
	u32 data;
	int s;

	data = __raw_readl(base + (where & ~3));

	switch (size) {
	case 1:
		s = ((where & 3) * 8);
		data >>= s;
		data &= 0xff;
		break;
	case 2:
		s = ((where & 2) * 8);
		data >>= s;
		data &= 0xffff;
		break;
	case 4:
		break;
	default:
		return PCIBIOS_BAD_REGISTER_NUMBER;
	}

	if (value)
		*value = data;

	return PCIBIOS_SUCCESSFUL;
}

static __IRAM int realtek_pci_raw_write(void __iomem *base, int where, int size, u32 value)
{
	u32 data;
	int s;

	data = __raw_readl(base + (where & ~3));

	switch (size) {
	case 1:
		s = ((where & 3) * 8);
		data &= ~(0xff << s);
		data |= ((value & 0xff) << s);
		break;
	case 2:
		s = ((where & 2) * 8);
		data &= ~(0xffff << s);
		data |= ((value & 0xffff) << s);
		break;
	case 4:
		data = value;
		break;
	default:
		return PCIBIOS_BAD_REGISTER_NUMBER;
	}

	__raw_writel(data, base + (where & ~3));

	return PCIBIOS_SUCCESSFUL;
}

static int realtek_pci_local_read(struct realtek_pci_controller *rpc,
				  int where, int size, u32 *value)
{
	return realtek_pci_raw_read(rpc->rc_cfg_base, where, size, value);
}

static int realtek_pci_local_write(struct realtek_pci_controller *rpc,
				  int where, int size, u32 value)
{
	return realtek_pci_raw_write(rpc->rc_cfg_base, where, size, value);
}

static __IRAM int realtek_pci_read(struct pci_bus *bus, unsigned int devfn, int where,
			    int size, uint32_t *value)
{
	struct realtek_pci_controller *rpc;

	rpc = pci_bus_to_realtek_pci_controller(bus);
	if (!rpc->link_up)
		return PCIBIOS_DEVICE_NOT_FOUND;

	if (devfn)
		return PCIBIOS_DEVICE_NOT_FOUND;

	return realtek_pci_raw_read(rpc->dev_cfg_base, where, size, value);
}

static __IRAM int realtek_pci_write(struct pci_bus *bus, unsigned int devfn, int where,
			     int size, uint32_t value)
{
	struct realtek_pci_controller *rpc;

	rpc = pci_bus_to_realtek_pci_controller(bus);
	if (!rpc->link_up)
		return PCIBIOS_DEVICE_NOT_FOUND;

	if (devfn)
		return PCIBIOS_DEVICE_NOT_FOUND;

	return realtek_pci_raw_write(rpc->dev_cfg_base, where, size, value);
}

static struct pci_ops realtek_pci_ops = {
	.read	= realtek_pci_read,
	.write	= realtek_pci_write,
};


static int realtek_pcie_check_link(struct realtek_pci_controller *rpc)
{
	int i = 20;
	u32 val = 0;

	do
	{
		mdelay(50);
		realtek_pci_local_read(rpc, 0x728, 4, &val);

		if((val & 0x1f) == 0x11)
			return 1;
	} while (i--);

	return 0;
}

static int realtek_pci_probe(struct platform_device *pdev)
{
	struct realtek_pci_controller *rpc;
	struct resource *res;
	int id;
	u32 val;

	id = pdev->id;
	if (id == -1)
		id = 0;

	rpc = devm_kzalloc(&pdev->dev, sizeof(struct realtek_pci_controller),
			    GFP_KERNEL);
	if (!rpc)
		return -ENOMEM;

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "rc_cfg_base");
	rpc->rc_cfg_base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(rpc->rc_cfg_base))
		return PTR_ERR(rpc->rc_cfg_base);

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "dev_cfg_base");
	rpc->dev_cfg_base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(rpc->dev_cfg_base))
		return PTR_ERR(rpc->dev_cfg_base);

	res = platform_get_resource_byname(pdev, IORESOURCE_IO, "io_base");
	if (!res)
		return -EINVAL;

	rpc->io_res.parent = res;
	rpc->io_res.name = "PCI IO space";
	rpc->io_res.start = res->start;
	rpc->io_res.end = res->end;
	rpc->io_res.flags = IORESOURCE_IO;

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "mem_base");
	if (!res)
		return -EINVAL;

	rpc->mem_res.parent = res;
	rpc->mem_res.name = "PCI memory space";
	rpc->mem_res.start = res->start;
	rpc->mem_res.end = res->end;
	rpc->mem_res.flags = IORESOURCE_MEM;

	rpc->pci_controller.pci_ops = &realtek_pci_ops;
	rpc->pci_controller.io_resource = &rpc->io_res;
	rpc->pci_controller.mem_resource = &rpc->mem_res;

	/* rpc->pci_controller.io_map_base = rpc->io_res.start; */

	realtek_pci_local_write(rpc, PCI_COMMAND, 4, 0x00100007);

	realtek_pci_local_read(rpc, 0x78, 1, &val);
	val &= ~0xE0;
	realtek_pci_local_write(rpc, 0x78, 1, val);

	rpc->link_up = realtek_pcie_check_link(rpc);
	if (!rpc->link_up)
		dev_warn(&pdev->dev, "PCIe link is down\n");

	register_pci_controller(&rpc->pci_controller);

	return 0;
}

static struct platform_driver realtek_pci_driver = {
	.probe = realtek_pci_probe,
	.driver = {
		.name = "realtek-pci",
	},
};

static int __init realtek_pci_init(void)
{
	return platform_driver_register(&realtek_pci_driver);
}

postcore_initcall(realtek_pci_init);
