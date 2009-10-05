#include <linux/types.h>
#include <linux/pci.h>
#include <linux/io.h>

#include <asm/mach-ralink/rt288x.h>
#include <asm/mach-ralink/rt288x_pci.h>

#define PCI_ACCESS_READ  0
#define PCI_ACCESS_WRITE 1

static int config_access(unsigned char access_type, struct pci_bus *bus,
	unsigned int devfn, unsigned char where, u32 * data)
{
	unsigned int slot = PCI_SLOT(devfn);
	unsigned int address;
	u8 func = PCI_FUNC(devfn);
	address = (bus->number << 16) | (slot << 11) | (func << 8) | (where& 0xfc) | 0x80000000;
	writel(address, RT2880_PCI_CONFIG_ADDR);
	if (access_type == PCI_ACCESS_WRITE)
		writel(*data, RT2880_PCI_CONFIG_DATA);
	else
		*data = readl(RT2880_PCI_CONFIG_DATA);
	return 0;
}

int
pci_config_read(struct pci_bus *bus, unsigned int devfn, int where, int size, u32 * val)
{
	u32 data = 0;
	if(config_access(PCI_ACCESS_READ, bus, devfn, where, &data))
		return PCIBIOS_DEVICE_NOT_FOUND;
	if(size == 1)
		*val = (data >> ((where & 3) << 3)) & 0xff;
	else if(size == 2)
		*val = (data >> ((where & 3) << 3)) & 0xffff;
	else
		*val = data;
	return PCIBIOS_SUCCESSFUL;
}

int
pci_config_write(struct pci_bus *bus, unsigned int devfn,
	int where, int size, u32 val)
{
	u32 data = 0;
	if(size == 4)
	{
		data = val;
	} else {
		if(config_access(PCI_ACCESS_READ, bus, devfn, where, &data))
			return PCIBIOS_DEVICE_NOT_FOUND;
		if(size == 1)
			data = (data & ~(0xff << ((where & 3) << 3))) |
				(val << ((where & 3) << 3));
		else if(size == 2)
			data = (data & ~(0xffff << ((where & 3) << 3))) |
				(val << ((where & 3) << 3));
	}
	if(config_access(PCI_ACCESS_WRITE, bus, devfn, where, &data))
		return PCIBIOS_DEVICE_NOT_FOUND;
	return PCIBIOS_SUCCESSFUL;
}
