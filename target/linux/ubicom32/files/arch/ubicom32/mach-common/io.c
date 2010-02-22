/*
 * arch/ubicom32/mach-common/io.c
 *   PCI I/O memory read/write support functions.
 *
 * (C) Copyright 2009, Ubicom, Inc.
 *
 * This file is part of the Ubicom32 Linux Kernel Port.
 *
 * The Ubicom32 Linux Kernel Port is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 *
 * The Ubicom32 Linux Kernel Port is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Ubicom32 Linux Kernel Port.  If not,
 * see <http://www.gnu.org/licenses/>.
 *
 * Ubicom32 implementation derived from (with many thanks):
 *   arch/m68knommu
 *   arch/blackfin
 *   arch/parisc
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/io.h>

#ifdef CONFIG_PCI
unsigned char  ioread8(void __iomem *addr)
{
	if (IS_PCI_ADDRESS(addr))
		return ubi32_pci_read_u8(addr);
	else
		return (unsigned char)(*(volatile unsigned char *)addr);
}
EXPORT_SYMBOL(ioread8);

unsigned short  ioread16(void __iomem *addr)
{
	if (IS_PCI_ADDRESS(addr))
		return ubi32_pci_read_u16(addr);
	else
		return (unsigned short)(*(volatile unsigned short *)addr);
}
EXPORT_SYMBOL(ioread16);

unsigned int  ioread32(void __iomem *addr)
{
	if (IS_PCI_ADDRESS(addr))
		return ubi32_pci_read_u32(addr);
	else
		return (unsigned int)(*(volatile unsigned int *)addr);
}
EXPORT_SYMBOL(ioread32);

void iowrite32(unsigned int val, void __iomem *addr)
{
	if (IS_PCI_ADDRESS(addr))
		ubi32_pci_write_u32(val, addr);
	else
		*(volatile unsigned int *)addr = val;
}
EXPORT_SYMBOL(iowrite32);

void iowrite16(unsigned short val, void __iomem *addr)
{
	if (IS_PCI_ADDRESS(addr))
		ubi32_pci_write_u16(val, addr);
	else
		*(volatile unsigned short *)addr = val;
}
EXPORT_SYMBOL(iowrite16);

void iowrite8(unsigned char val, void __iomem *addr)
{
	if (IS_PCI_ADDRESS(addr))
		ubi32_pci_write_u8(val, addr);
	else
		*(volatile unsigned char *)addr = val;
}
EXPORT_SYMBOL(iowrite8);

void memcpy_fromio(void *to, const volatile void __iomem *from, unsigned len)
{
	if (IS_PCI_ADDRESS(from)) {
		if ((((u32_t)from & 0x3) == 0) && (((u32_t)to & 0x3) == 0)) {
			while ((int)len >= 4) {
				*(u32_t *)to = ubi32_pci_read_u32(from);
				to += 4;
				from += 4;
				len -= 4;
			}
		} else if ((((u32_t)from & 0x1) == 0) &&
			   (((u32_t)to & 0x1) == 0)) {
			while ((int)len >= 2) {
				 *(u16_t *)to = ubi32_pci_read_u16(from);
				 to += 2;
				 from += 2;
				 len -= 2;
			}
		}

		while (len) {
			*(u8_t *)to = ubi32_pci_read_u8(from);
			to++;
			from++;
			len--;
		}
	} else
		memcpy(to, (void *)from, len);
}
EXPORT_SYMBOL(memcpy_fromio);

void memcpy_toio(volatile void __iomem *to, const void *from, unsigned len)
{
	if (IS_PCI_ADDRESS(to)) {
		if ((((u32_t)from & 0x3) == 0) && (((u32_t)to & 0x3) == 0)) {
			while ((int)len >= 4) {
				ubi32_pci_write_u32(*(u32_t *)from, to);
				to += 4;
				from += 4;
				len -= 4;
			}
		} else if ((((u32_t)from & 0x1) == 0) &&
			   (((u32_t)to & 0x1) == 0)) {
			while ((int)len >= 2) {
				ubi32_pci_write_u16(*(u16_t *)from, to);
				to += 2;
				from += 2;
				len -= 2;
			}
		}

		while (len) {
			ubi32_pci_write_u8(*(u8_t *)from, to);
			from++;
			to++;
			len--;
		}
	} else
		memcpy((void *)to, from, len);

}
EXPORT_SYMBOL(memcpy_toio);

void memset_io(volatile void __iomem *addr, int val, size_t len)
{
	if (IS_PCI_ADDRESS(addr)) {
		while (len) {
			ubi32_pci_write_u8((unsigned char)val, addr);
			addr++;
			len--;
		}
	} else
		memset((void *)addr, val, len);

}
EXPORT_SYMBOL(memset_io);

void ioread8_rep(void __iomem *port, void *buf, unsigned long count)
{
	if (IS_PCI_ADDRESS(port)) {
		while (count) {
			*(u8_t *)buf = ioread8(port);
			buf++;
			count--;
		}
	} else {
		insb((unsigned int)port, buf, count);
	}

}
EXPORT_SYMBOL(ioread8_rep);

void ioread16_rep(void __iomem *port, void *buf, unsigned long count)
{
	if (IS_PCI_ADDRESS(port)) {
		while (count) {
			*(u16_t *)buf = ioread16(port);
			buf += 2;
			count--;
		}
	} else {
		insw((unsigned int)port, buf, count);
	}
}
EXPORT_SYMBOL(ioread16_rep);

void ioread32_rep(void __iomem *port, void *buf, unsigned long count)
{
	 if (IS_PCI_ADDRESS(port)) {
		while (count) {
			*(u32_t *)buf = ioread32(port);
			buf += 4;
			count--;
		}
	} else {
		insl((unsigned int)port, buf, count);
	}
}
EXPORT_SYMBOL(ioread32_rep);

void  iowrite8_rep(void __iomem *port, const void *buf, unsigned long count)
{
	  if (IS_PCI_ADDRESS(port)) {
		while (count) {
			iowrite8(*(u8_t *)buf, port);
			buf++;
			count--;
		}
	} else {
		outsb((unsigned int)port, buf, count);
	}

}
EXPORT_SYMBOL(iowrite8_rep);

void  iowrite16_rep(void __iomem *port, const void *buf, unsigned long count)
{
	if (IS_PCI_ADDRESS(port)) {
		while (count) {
			iowrite16(*(u16_t *)buf, port);
			buf += 2;
			count--;
		}
	} else {
		outsw((unsigned int)port, buf, count);
	}
}
EXPORT_SYMBOL(iowrite16_rep);

void  iowrite32_rep(void __iomem *port, const void *buf, unsigned long count)
{
	if (IS_PCI_ADDRESS(port)) {
		while (count) {
			iowrite32(*(u32_t *)buf, port);
			buf += 4;
			count--;
		}
	} else {
		outsl((unsigned int)port, buf, count);
	}
}
EXPORT_SYMBOL(iowrite32_rep);

#endif /* CONFIG_PCI */
