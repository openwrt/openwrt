#include <linux/types.h>
#include <linux/pci.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/version.h>
#include <asm/pci.h>
#include <asm/io.h>
#include <linux/init.h>
#include <linux/mod_devicetable.h>

#include <asm/mach-ralink/rt288x.h>
#include <asm/mach-ralink/rt288x_pci.h>

#ifdef CONFIG_PCI

extern int pci_config_read(struct pci_bus *bus, unsigned int devfn, int where, int size, u32 *val);
extern int pci_config_write(struct pci_bus *bus, unsigned int devfn, int where, int size, u32 val);

struct pci_ops rt2880_pci_ops = {
	.read =  pci_config_read,
	.write = pci_config_write,
};

static struct resource pci_io_resource = {
	.name = "pci MEM space",
	.start = 0x20000000,
	.end = 0x2FFFFFFF,
	.flags = IORESOURCE_MEM,
};

static struct resource pci_mem_resource = {
	.name = "pci IO space",
	.start = 0x00460000,
	.end = 0x0046FFFF,
	.flags = IORESOURCE_IO,
};

struct pci_controller rt2880_controller = {
	.pci_ops = &rt2880_pci_ops,
	.mem_resource = &pci_io_resource,
	.io_resource = &pci_mem_resource,
	.mem_offset = 0x00000000UL,
	.io_offset = 0x00000000UL,
};

void inline
read_config(unsigned long bus, unsigned long dev, unsigned long func,
	unsigned long reg, unsigned long *val)
{
	unsigned long address =
		(bus << 16) | (dev << 11) | (func << 8) | (reg & 0xfc) | 0x80000000;
	writel(address, RT2880_PCI_CONFIG_ADDR);
	*val = readl(RT2880_PCI_CONFIG_DATA);
}

void inline
write_config(unsigned long bus, unsigned long dev, unsigned long func,
	unsigned long reg, unsigned long val)
{
	unsigned long address =
		(bus << 16) | (dev << 11) | (func << 8) | (reg & 0xfc) | 0x80000000;
	writel(address, RT2880_PCI_CONFIG_ADDR);
	writel(val, RT2880_PCI_CONFIG_DATA);
}

int __init
pcibios_map_irq(const struct pci_dev *dev, u8 slot, u8 pin)
{
	u16 cmd;
	unsigned long val;
	int irq = -1;
	if (dev->bus->number != 0)
		return 0;

	switch(PCI_SLOT(dev->devfn))
	{
	case 0x00:
		write_config(0, 0, 0, PCI_BASE_ADDRESS_0, 0x08000000);
		read_config(0, 0, 0, PCI_BASE_ADDRESS_0, &val);
		break;
	case 0x11:
		irq = RT288X_CPU_IRQ_PCI;
		break;
	default:
		printk("%s:%s[%d] trying to alloc unknown pci irq\n", __FILE__, __func__, __LINE__);
		BUG();
		break;
	}

	pci_write_config_byte((struct pci_dev*)dev, PCI_CACHE_LINE_SIZE, 0x14);
	pci_write_config_byte((struct pci_dev*)dev, PCI_LATENCY_TIMER, 0xFF);
	pci_read_config_word((struct pci_dev*)dev, PCI_COMMAND, &cmd);
	cmd = cmd | PCI_COMMAND_MASTER | PCI_COMMAND_IO | PCI_COMMAND_MEMORY |
		PCI_COMMAND_INVALIDATE | PCI_COMMAND_FAST_BACK | PCI_COMMAND_SERR |
		PCI_COMMAND_WAIT | PCI_COMMAND_PARITY;
	pci_write_config_word((struct pci_dev*)dev, PCI_COMMAND, cmd);
	pci_write_config_byte((struct pci_dev*)dev, PCI_INTERRUPT_LINE, dev->irq);
	return irq;
}

int
init_rt2880pci(void)
{
	unsigned long val = 0;
	int i;
	writel(0, RT2880_PCI_PCICFG_ADDR);
	for(i = 0; i < 0xfffff; i++) {}
	writel(0x79, RT2880_PCI_ARBCTL);
	writel(0x07FF0001, RT2880_PCI_BAR0SETUP_ADDR);
	writel(RT2880_PCI_SLOT1_BASE, RT2880_PCI_MEMBASE);
	writel(0x00460000, RT2880_PCI_IOBASE);
	writel(0x08000000, RT2880_PCI_IMBASEBAR0_ADDR);
	writel(0x08021814, RT2880_PCI_ID);
	writel(0x00800001, RT2880_PCI_CLASS);
	writel(0x28801814, RT2880_PCI_SUBID);
	writel(0x000c0000, RT2880_PCI_PCIMSK_ADDR);
	write_config(0, 0, 0, PCI_BASE_ADDRESS_0, 0x08000000);
	read_config(0, 0, 0, PCI_BASE_ADDRESS_0, &val);
	register_pci_controller(&rt2880_controller);
	return 0;
}

int
pcibios_plat_dev_init(struct pci_dev *dev)
{
	return 0;
}

struct pci_fixup pcibios_fixups[] = {
	{0}
};

arch_initcall(init_rt2880pci);

#endif
