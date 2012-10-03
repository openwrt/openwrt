/*
 * Moschip MCS8140 PCI support
 *
 * Copyright (C) 2003 Moschip Semiconductors Ltd.
 * Copyright (C) 2003 Artec Design Ltd.
 * Copyright (C) 2012 Florian Fainelli <florian@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/ptrace.h>
#include <linux/slab.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/spinlock.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/io.h>

#include <asm/irq.h>
#include <asm/system.h>
#include <asm/mach/pci.h>
#include <asm/mach/map.h>
#include <mach/mcs814x.h>
#include <mach/irqs.h>

#define MCS8140_PCI_CONFIG_SIZE		SZ_64M
#define MCS8140_PCI_IOMISC_SIZE		SZ_64M

#define MCS8140_PCI_HOST_BASE           0x80000000
#define MCS8140_PCI_IOMISC_BASE         0x00000000
#define MCS8140_PCI_PRE_BASE            0x10000000
#define MCS8140_PCI_NONPRE_BASE         0x30000000

#define MCS8140_PCI_CFG_BASE		(MCS8140_PCI_HOST_BASE + 0x04000000)
#define MCS8140_PCI_IO_BASE		(MCS8140_PCI_HOST_BASE)

#define MCS8140_PCI_IO_VIRT_BASE	(MCS814X_IO_BASE - MCS8140_PCI_CONFIG_SIZE - \
					 MCS8140_PCI_IOMISC_SIZE)
#define MCS8140_PCI_CFG_VIRT_BASE	(MCS814X_IO_BASE - MCS8140_PCI_CONFIG_SIZE)

#define PCI_FATAL_ERROR			1
#define EXTERNAL_ABORT_NON_LINE_FETCH	8
#define EPRM_DONE			0x80
#define EPRM_SDRAM_FUNC0		0xAC
#define PCI_INTD			4
#define MCS8140_PCI_DEVICE_ID		0xA0009710
#define MCS8140_PCI_CLASS_ID		0x02000011  /* Host-Class id :0x0600 */
#define PCI_IF_CONFIG			0x200

static void __iomem *mcs8140_pci_master_base;
static void __iomem *mcs8140_eeprom_emu_base;

static unsigned long __pci_addr(struct pci_bus *bus,
				unsigned int devfn, int offset)
{
	unsigned int busnr = bus->number;
	unsigned int slot;

	/* we only support bus 0 */
	if (busnr != 0)
		return 0;

	/*
	 * Trap out illegal values
	 */
	BUG_ON(devfn > 255 || busnr > 255 || devfn > 255);

	/* Scan 3 slots */
	slot = PCI_SLOT(devfn);
	switch (slot) {
	case 1:
	case 2:
	case 3:
		if (PCI_FUNC(devfn) >= 4)
			return 0;

		return MCS8140_PCI_CFG_VIRT_BASE | (PCI_SLOT(devfn) << 11) |
			(PCI_FUNC(devfn) << 8) | offset;
	default:
		pr_warn("Ignoring: PCI Slot is %x\n", PCI_SLOT(devfn));
		return 0;
	}
}

static int mcs8140_pci_host_status(void)
{
	u32 host_status;

	host_status = readl_relaxed(mcs8140_pci_master_base + PCI_IF_CONFIG);
	if (host_status & PCI_FATAL_ERROR) {
		writel_relaxed(host_status & 0xfffffff0,
			mcs8140_pci_master_base + PCI_IF_CONFIG);
		/* flush write */
		host_status =
			readl_relaxed(mcs8140_pci_master_base + PCI_IF_CONFIG);
		return 1;
	}

	return 0;
}

static int mcs8140_pci_read_config(struct pci_bus *bus,
					unsigned int devfn, int where,
					int size, u32 *val)
{
	unsigned long v = 0xFFFFFFFF;
	unsigned long addr = __pci_addr(bus, devfn, where);

	if (addr != 0) {
		switch (size) {
		case 1:
			v = readb_relaxed(addr);
			break;
		case 2:
			addr &= ~1;
			v = readw_relaxed(addr);
			break;
		default:
			addr &= ~3;
			v = readl_relaxed(addr);
			break;
		}
	} else
		v = 0xffffffff;

	if (mcs8140_pci_host_status())
		v = 0xffffffff;

	*val = v;

	return PCIBIOS_SUCCESSFUL;
}

static void mcs8140_eeprom_emu_init(void)
{
	writel_relaxed(0x0000000F, mcs8140_eeprom_emu_base + EPRM_SDRAM_FUNC0);
	writel_relaxed(0x08000000, MCS8140_PCI_CFG_VIRT_BASE + 0x10);
	/* Set the DONE bit of the EEPROM emulator */
	writel_relaxed(0x01, mcs8140_eeprom_emu_base + EPRM_DONE);
}

static int mcs8140_pci_write_config(struct pci_bus *bus,
					unsigned int devfn, int where,
					int size, u32 val)
{
	unsigned long addr = __pci_addr(bus, devfn, where);

	if (addr != 0) {
		switch (size) {
		case 1:
			writeb_relaxed((u8)val, addr);
			break;
		case 2:
			writew_relaxed((u16)val, addr);
			break;
		case 4:
			writel_relaxed(val, addr);
			break;
		}
	}

	return PCIBIOS_SUCCESSFUL;
}

static struct pci_ops pci_mcs8140_ops = {
	.read	= mcs8140_pci_read_config,
	.write	= mcs8140_pci_write_config,
};


static struct resource io_mem = {
	.name	= "PCI I/O space",
	.start	= MCS8140_PCI_HOST_BASE + MCS8140_PCI_IOMISC_BASE,
	.end	= MCS8140_PCI_HOST_BASE + MCS8140_PCI_IOMISC_BASE + SZ_64M,
	.flags	= IORESOURCE_IO,
};

static struct resource pre_mem = {
	.name	= "PCI prefetchable",
	.start	= MCS8140_PCI_HOST_BASE + MCS8140_PCI_PRE_BASE,
	.end	= MCS8140_PCI_HOST_BASE + MCS8140_PCI_PRE_BASE + SZ_512M,
	.flags	= IORESOURCE_MEM | IORESOURCE_PREFETCH,
};

static struct resource non_mem = {
	.name	= "PCI non-prefetchable",
	.start	= MCS8140_PCI_HOST_BASE + MCS8140_PCI_NONPRE_BASE,
	.end	= MCS8140_PCI_HOST_BASE + MCS8140_PCI_NONPRE_BASE + SZ_256M,
	.flags	= IORESOURCE_MEM,
};

int __init pci_mcs8140_setup_resources(struct pci_sys_data *sys)
{
	int ret = 0;

	ret = request_resource(&iomem_resource, &io_mem);
	if (ret) {
		pr_err("PCI: unable to allocate I/O "
			"memory region (%d)\n", ret);
		goto out;
	}

	ret = request_resource(&iomem_resource, &non_mem);
	if (ret) {
		pr_err("PCI: unable to allocate non-prefetchable "
			"memory region (%d)\n", ret);
		goto release_io_mem;
	}

	ret = request_resource(&iomem_resource, &pre_mem);
	if (ret) {
		pr_err("PCI: unable to allocate prefetchable "
			"memory region (%d)\n", ret);
		goto release_non_mem;
	}

	mcs8140_eeprom_emu_init();

	pci_add_resource(&sys->resources, &io_mem);
	pci_add_resource(&sys->resources, &non_mem);
	pci_add_resource(&sys->resources, &pre_mem);

	return ret;

release_non_mem:
	release_resource(&non_mem);
release_io_mem:
	release_resource(&io_mem);
out:
	return ret;
}

struct pci_bus *pci_mcs8140_scan_bus(int nr, struct pci_sys_data *sys)
{
	return pci_scan_bus(sys->busnr, &pci_mcs8140_ops, sys);
}


int __init pci_mcs8140_setup(int nr, struct pci_sys_data *sys)
{
	int ret = 0;
	u32 val;

	if (nr > 0)
		return 0;

	sys->mem_offset = MCS8140_PCI_IO_VIRT_BASE - MCS8140_PCI_IO_BASE;
	sys->io_offset = 0;

	ret = pci_mcs8140_setup_resources(sys);
	if (ret < 0) {
		pr_err("unable to setup mcs8140 resources\n");
		goto out;
	}

	val = readl_relaxed(MCS8140_PCI_CFG_VIRT_BASE);
	if (val != MCS8140_PCI_DEVICE_ID) {
		pr_err("cannot find MCS8140 PCI Core: %08x\n", val);
		ret = -EIO;
		goto out;
	}

	pr_info("MCS8140 PCI core found\n");

	val = readl_relaxed(MCS8140_PCI_CFG_VIRT_BASE + PCI_COMMAND);
	/* Added to support wireless cards */
	writel_relaxed(0, MCS8140_PCI_CFG_VIRT_BASE + 0x40);
	writel_relaxed(val | 0x147, MCS8140_PCI_CFG_VIRT_BASE + PCI_COMMAND);
	val = readl_relaxed(MCS8140_PCI_CFG_VIRT_BASE + PCI_COMMAND);
	ret = 1;
out:
	return ret;
}


static int __init mcs8140_map_irq(const struct pci_dev *dev, u8 slot, u8 pin)
{
	int line = IRQ_PCI_INTA;

	if (pin != 0) {
		/* IRQ_PCIA - 22 */
		if (pin == PCI_INTD)
			line = IRQ_PCI_INTA + pin; /* IRQ_PCIA - 22 */
		else
			line = IRQ_PCI_INTA + pin - 1; /* IRQ_PCIA - 22 */
	}

	pr_info("PCI: Map interrupt slot 0x%02x pin 0x%02x line 0x%02x\n",
		slot, pin, line);

	return line;
}

static irqreturn_t mcs8140_pci_abort_interrupt(int irq, void *dummy)
{
	u32 word;

	word = readl_relaxed(mcs8140_pci_master_base + PCI_IF_CONFIG);
	if (!(word & (1 << 24)))
		return IRQ_NONE;

	writel_relaxed(word & 0xfffffff0,
		mcs8140_pci_master_base + PCI_IF_CONFIG);
	/* flush write */
	word = readl_relaxed(mcs8140_pci_master_base + PCI_IF_CONFIG);

	return IRQ_HANDLED;
}

static int mcs8140_pci_abort_irq_init(int irq)
{
	u32 word;

	/* Enable Interrupt in PCI Master Core */
	word = readl_relaxed(mcs8140_pci_master_base + PCI_IF_CONFIG);
	word |= (1 << 24);
	writel_relaxed(word, mcs8140_pci_master_base + PCI_IF_CONFIG);

	/* flush write */
	word = readl_relaxed(mcs8140_pci_master_base + PCI_IF_CONFIG);

	return request_irq(irq, mcs8140_pci_abort_interrupt, 0,
			"PCI abort", NULL);
}

static int mcs8140_pci_host_abort(unsigned long addr,
				unsigned int fsr, struct pt_regs *regs)
{
	pr_warn("PCI Data abort: address = 0x%08lx fsr = 0x%03x"
		"PC = 0x%08lx LR = 0x%08lx\n",
		addr, fsr, regs->ARM_pc, regs->ARM_lr);

	/*
	 * If it was an imprecise abort, then we need to correct the
	 * return address to be _after_ the instruction.
	 */
	if (fsr & (1 << 10) || mcs8140_pci_host_status())
		regs->ARM_pc += 4;

	return 0;
}

static void mcs8140_data_abort_init(void)
{
	hook_fault_code(EXTERNAL_ABORT_NON_LINE_FETCH,
		mcs8140_pci_host_abort, SIGBUS,
		0, "external abort on non-line fetch");
}

static struct hw_pci mcs8140_pci __initdata = {
	.map_irq		= mcs8140_map_irq,
	.nr_controllers		= 1,
	.setup			= pci_mcs8140_setup,
	.scan			= pci_mcs8140_scan_bus,
};

static struct map_desc mcs8140_pci_io_desc[] __initdata = {
	{
		.virtual	= MCS8140_PCI_CFG_VIRT_BASE,
		.pfn		= __phys_to_pfn(MCS8140_PCI_CFG_BASE),
		.length		= MCS8140_PCI_CONFIG_SIZE,
		.type		= MT_DEVICE
	},
	{
		.virtual	= MCS8140_PCI_IO_VIRT_BASE,
		.pfn		= __phys_to_pfn(MCS8140_PCI_IO_BASE),
		.length		= MCS8140_PCI_IOMISC_SIZE,
		.type		= MT_DEVICE
	},
};

static int __devinit mcs8140_pci_probe(struct platform_device *pdev)
{
	struct resource *res;
	int ret, irq;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		dev_err(&pdev->dev, "failed to get mem resource 0\n");
		return -ENODEV;
	}

	mcs8140_pci_master_base = devm_ioremap(&pdev->dev, res->start,
					resource_size(res));
	if (!mcs8140_pci_master_base) {
		dev_err(&pdev->dev, "failed to remap PCI master regs\n");
		return -ENODEV;
	}

	res = platform_get_resource(pdev, IORESOURCE_MEM, 1);
	if (!res) {
		dev_err(&pdev->dev, "failed to get mem resource 1\n");
		return -ENOMEM;
	}

	mcs8140_eeprom_emu_base = devm_ioremap(&pdev->dev, res->start,
					resource_size(res));
	if (!mcs8140_eeprom_emu_base) {
		dev_err(&pdev->dev, "failed to remap EEPROM regs\n");
		return -ENOMEM;
	}

	irq = platform_get_irq(pdev, 0);
	if (irq < 0) {
		dev_err(&pdev->dev, "failed to get pci abort irq\n");
		return -ENODEV;
	}

	/* Setup static mappins for PCI CFG space */
	iotable_init(mcs8140_pci_io_desc, ARRAY_SIZE(mcs8140_pci_io_desc));

	pcibios_min_io = MCS8140_PCI_HOST_BASE;
	pcibios_min_mem = MCS8140_PCI_HOST_BASE + MCS8140_PCI_PRE_BASE;

	mcs8140_data_abort_init();
	ret = mcs8140_pci_abort_irq_init(irq);
	if (ret) {
		dev_err(&pdev->dev, "failed to setup abort irq\n");
		return ret;
	}

	pci_common_init(&mcs8140_pci);

	return 0;
}

static struct of_device_id mcs8140_of_ids[] __devinitdata = {
	{ .compatible = "moschip,mcs8140-pci" },
	{ .compatible = "moschip,mcs814x-pci" },
	{ /* sentinel */ },
};

static struct platform_driver mcs8140_pci_driver = {
	.driver	= {
		.name	= "mcs8140-pci",
		.of_match_table = mcs8140_of_ids,
	},
	.probe	= mcs8140_pci_probe,
};

static int __init mcs8140_pci_init(void)
{
	return platform_driver_register(&mcs8140_pci_driver);
}
subsys_initcall(mcs8140_pci_init);

