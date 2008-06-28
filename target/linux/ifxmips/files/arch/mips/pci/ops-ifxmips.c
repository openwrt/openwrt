#include <linux/types.h>
#include <linux/pci.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/mm.h>
#include <asm/ifxmips/ifxmips.h>
#include <asm/ifxmips/ifxmips_irq.h>
#include <asm/addrspace.h>
#include <linux/vmalloc.h>
#include <asm/ifxmips/ifxmips_ebu.h>

#define IFXMIPS_PCI_CFG_BUSNUM_SHF 16
#define IFXMIPS_PCI_CFG_DEVNUM_SHF 11
#define IFXMIPS_PCI_CFG_FUNNUM_SHF 8

#define PCI_ACCESS_READ  0
#define PCI_ACCESS_WRITE 1

extern u32 ifxmips_pci_mapped_cfg;

static int
ifxmips_pci_config_access(unsigned char access_type,
		struct pci_bus *bus, unsigned int devfn, unsigned int where, u32 *data)
{
	unsigned long cfg_base;
	unsigned long flags;

	u32 temp;

	/* IFXMips support slot from 0 to 15 */
	/* dev_fn 0&0x68 (AD29) is ifxmips itself */
	if ((bus->number != 0) || ((devfn & 0xf8) > 0x78)
			|| ((devfn & 0xf8) == 0) || ((devfn & 0xf8) == 0x68))
		return 1;

	spin_lock_irqsave(&ebu_lock, flags);

	cfg_base = ifxmips_pci_mapped_cfg;
	cfg_base |= (bus->number << IFXMIPS_PCI_CFG_BUSNUM_SHF) | (devfn <<
			IFXMIPS_PCI_CFG_FUNNUM_SHF) | (where & ~0x3);

	/* Perform access */
	if (access_type == PCI_ACCESS_WRITE)
	{
#ifdef CONFIG_SWAP_IO_SPACE
		ifxmips_w32(swab32(*data), ((u32*)cfg_base));
#else
		ifxmips_w32(*data, ((u32*)cfg_base));
#endif
	} else {
		*data = ifxmips_r32(((u32*)(cfg_base)));
#ifdef CONFIG_SWAP_IO_SPACE
		*data = swab32(*data);
#endif
	}
	wmb();

	/* clean possible Master abort */
	cfg_base = (ifxmips_pci_mapped_cfg | (0x0 << IFXMIPS_PCI_CFG_FUNNUM_SHF)) + 4;
	temp = ifxmips_r32(((u32*)(cfg_base)));
#ifdef CONFIG_SWAP_IO_SPACE
	temp = swab32 (temp);
#endif
	cfg_base = (ifxmips_pci_mapped_cfg | (0x68 << IFXMIPS_PCI_CFG_FUNNUM_SHF)) + 4;
	ifxmips_w32(temp, ((u32*)cfg_base));

	spin_unlock_irqrestore(&ebu_lock, flags);

	if (((*data) == 0xffffffff) && (access_type == PCI_ACCESS_READ))
		return 1;

	return 0;
}

int
ifxmips_pci_read_config_dword(struct pci_bus *bus, unsigned int devfn,
		int where, int size, u32 * val)
{
	u32 data = 0;

	if (ifxmips_pci_config_access(PCI_ACCESS_READ, bus, devfn, where, &data))
		return PCIBIOS_DEVICE_NOT_FOUND;

	if (size == 1)
		*val = (data >> ((where & 3) << 3)) & 0xff;
	else if (size == 2)
		*val = (data >> ((where & 3) << 3)) & 0xffff;
	else
		*val = data;

	return PCIBIOS_SUCCESSFUL;
}

int
ifxmips_pci_write_config_dword(struct pci_bus *bus, unsigned int devfn,
		int where, int size, u32 val)
{
	u32 data = 0;

	if (size == 4)
	{
		data = val;
	} else {
		if (ifxmips_pci_config_access(PCI_ACCESS_READ, bus, devfn, where, &data))
			return PCIBIOS_DEVICE_NOT_FOUND;

		if (size == 1)
			data = (data & ~(0xff << ((where & 3) << 3))) |
				(val << ((where & 3) << 3));
		else if (size == 2)
			data = (data & ~(0xffff << ((where & 3) << 3))) |
				(val << ((where & 3) << 3));
	}

	if (ifxmips_pci_config_access(PCI_ACCESS_WRITE, bus, devfn, where, &data))
		return PCIBIOS_DEVICE_NOT_FOUND;

	return PCIBIOS_SUCCESSFUL;
}
