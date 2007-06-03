/*
 *  $Id$
 *
 *  ADM5120 specific PCI operations
 *
 *  Copyright (C) ADMtek Incorporated.
 *  Copyright (C) 2005 Jeroen Vreeken (pe1rxq@amsat.org)
 *  Copyright (C) 2007 Gabor Juhos <juhosg@freemail.hu>
 *  Copyright (C) 2007 OpenWrt.org
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the
 *  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA  02110-1301, USA.
 *
 */

#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/pci.h>

#include <asm/mach-adm5120/adm5120_defs.h>

#define DEBUG	0
#if DEBUG
#define DBG(f, ...) printk(f, ## __VA_ARGS__ )
#else
#define DBG(f, ...)
#endif

#define PCI_ENABLE 0x80000000

static inline void write_cfgaddr(u32 addr)
{
	*(volatile u32*)KSEG1ADDR(ADM5120_PCICFG_ADDR) = (addr | PCI_ENABLE);
}

static inline void write_cfgdata(u32 data)
{
	*(volatile u32*)KSEG1ADDR(ADM5120_PCICFG_DATA) = data;

}

static inline u32 read_cfgdata(void)
{
	return (*(volatile u32*)KSEG1ADDR(ADM5120_PCICFG_DATA));
}

static inline u32 mkaddr(struct pci_bus *bus, unsigned int devfn, int where)
{
	return (((bus->number & 0xFF) << 16) | ((devfn & 0xFF) << 8) | \
		(where & 0xFC));
}

static int pci_config_read(struct pci_bus *bus, unsigned int devfn, int where,
                           int size, u32 *val)
{
	u32 data;

	write_cfgaddr(mkaddr(bus,devfn,where));
	data = read_cfgdata();	

	DBG("PCI: cfg_read  %02u.%02u.%01u/%02X:%01d, cfg:0x%08X",
		bus->number, PCI_SLOT(devfn), PCI_FUNC(devfn), where, size, data);

	switch (size) {
	case 1:
		if (where & 1)
			data >>= 8;
		if (where & 2)
			data >>= 16;
		data &= 0xFF;
		break;
	case 2:
		if (where & 2)
			data >>= 16;
		data &= 0xFFFF;
		break;
	}

	*val = data;
	DBG(", 0x%08X returned\n", data);
	
	return PCIBIOS_SUCCESSFUL;
}

static int pci_config_write(struct pci_bus *bus, unsigned int devfn, int where,
                            int size, u32 val)
{
	u32 data;
	int s;

	write_cfgaddr(mkaddr(bus,devfn,where));
	data = read_cfgdata();

	DBG("PCI: cfg_write %02u.%02u.%01u/%02X:%01d, cfg:0x%08X",
		bus->number, PCI_SLOT(devfn), PCI_FUNC(devfn), where, size, data);
	    
	switch (size) {
	case 1:
		s = ((where & 3) << 3);
		data &= ~(0xFF << s);
		data |= ((val & 0xFF) << s);
		break;
	case 2:
		s = ((where & 2) << 4);
		data &= ~(0xFFFF << s);
		data |= ((val & 0xFFFF) << s);
		break;
	case 4:
		data = val;
		break;
	}

	write_cfgdata(data);
	DBG(", 0x%08X written\n", data);

	return PCIBIOS_SUCCESSFUL;
}

struct pci_ops adm5120_pci_ops = {
	.read	= pci_config_read,
	.write	= pci_config_write,
};
