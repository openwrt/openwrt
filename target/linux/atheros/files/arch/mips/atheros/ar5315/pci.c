/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <linux/types.h>
#include <linux/pci.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/spinlock.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <asm/bootinfo.h>
#include <asm/paccess.h>
#include <asm/irq_cpu.h>
#include <asm/io.h>
#include "ar531x.h"

#define AR531X_MEM_BASE    0x80800000UL
#define AR531X_MEM_SIZE    0x00ffffffUL
#define AR531X_IO_SIZE     0x00007fffUL
#define IDSEL_SHIFT	13

static spinlock_t ar531x_pci_lock = SPIN_LOCK_UNLOCKED;
static u32 cfgaddr;

static int config_access(int busno, int dev, int func, int where, int size, u32 ptr, int write)
{
	u32 address;      /* Address to read from */
	u32 reg;
	unsigned long flags;
	int ret = -1;
	if ((busno != 0) || ((dev != 0) && (dev != 3)) || (func > 2))
		return ret;

	spin_lock_irqsave(&ar531x_pci_lock, flags);

	/* Select Configuration access */
	reg = sysRegRead(AR5315_PCI_MISC_CONFIG);
	reg |= AR5315_PCIMISC_CFG_SEL;
	sysRegWrite(AR5315_PCI_MISC_CONFIG, reg);
	(void)sysRegRead(AR5315_PCI_MISC_CONFIG);

	address = (u32)cfgaddr + (1 << (IDSEL_SHIFT + dev)) + (func << 8) + where;

	if (size == 1)
		address ^= 0x3;
	else if (size == 2)
		address ^= 0x2;

	if (write) {
		if (size == 1)
			ret = put_dbe(ptr, (u8 *) address);
		else if (size == 2)
			ret = put_dbe(ptr, (u16 *) address);
		else if (size == 4)
			ret = put_dbe(ptr, (u32 *) address);
	} else {
		if (size == 1)
			ret = get_dbe(*((u32 *)ptr), (u8 *) address);
		else if (size == 2)
			ret = get_dbe(*((u32 *)ptr), (u16 *) address);
		else if (size == 4)
			ret = get_dbe(*((u32 *)ptr), (u32 *) address);
	}

	/* Select Memory access */
	reg = sysRegRead(AR5315_PCI_MISC_CONFIG);
	reg &= ~AR5315_PCIMISC_CFG_SEL;
	sysRegWrite(AR5315_PCI_MISC_CONFIG, reg);
	(void)sysRegRead(AR5315_PCI_MISC_CONFIG);

	spin_unlock_irqrestore(&ar531x_pci_lock, flags);

	if (ret) {
		*((u32 *)ptr) = 0xffffffff;
		return PCIBIOS_DEVICE_NOT_FOUND;
	}

	return PCIBIOS_SUCCESSFUL;
}

static int ar531x_pci_read(struct pci_bus *bus, unsigned int devfn, int where, int size, u32 * value)
{
	return config_access(bus->number, PCI_SLOT(devfn), PCI_FUNC(devfn), where, size, (u32) value, 0);
}

static int ar531x_pci_write(struct pci_bus *bus, unsigned int devfn, int where, int size, u32 value)
{
	return config_access(bus->number, PCI_SLOT(devfn), PCI_FUNC(devfn), where, size, value, 1);
}

struct pci_ops ar531x_pci_ops = {
	.read	= ar531x_pci_read,
	.write	= ar531x_pci_write,
};

static struct resource ar531x_mem_resource = {
	.name	= "AR531x PCI MEM",
	.start	= AR531X_MEM_BASE,
	.end	= AR531X_MEM_BASE + AR531X_MEM_SIZE - AR531X_IO_SIZE - 1 + 0x4000000,
	.flags	= IORESOURCE_MEM,
};

static struct resource ar531x_io_resource = {
	.name	= "AR531x PCI I/O",
	.start	= AR531X_MEM_BASE + AR531X_MEM_SIZE - AR531X_IO_SIZE,
	.end	= AR531X_MEM_BASE + AR531X_MEM_SIZE - 1,
	.flags	= IORESOURCE_IO,
};

struct pci_controller ar531x_pci_controller = {
	.pci_ops		= &ar531x_pci_ops,
	.mem_resource	= &ar531x_mem_resource,
	.io_resource	= &ar531x_io_resource,
	.mem_offset     = 0x00000000UL,
	.io_offset      = 0x00000000UL,
};

int pcibios_map_irq(const struct pci_dev *dev, u8 slot, u8 pin)
{
	return AR5315_IRQ_LCBUS_PCI;
}

int pcibios_plat_dev_init(struct pci_dev *dev)
{
	u32 reg;

	pci_write_config_byte(dev, PCI_INTERRUPT_LINE, 5);
	pci_write_config_word(dev, 0x40, 0);

	/* Clear any pending Abort or external Interrupts 
	 * and enable interrupt processing */
	reg = sysRegRead(AR5315_PCI_INTEN_REG);
	reg &= ~AR5315_PCI_INT_ENABLE;
	sysRegWrite(AR5315_PCI_INTEN_REG, reg);

	reg = sysRegRead(AR5315_PCI_INT_STATUS);
	reg |= (AR5315_PCI_ABORT_INT | AR5315_PCI_EXT_INT);
	sysRegWrite(AR5315_PCI_INT_STATUS, reg);

	reg = sysRegRead(AR5315_PCI_INT_MASK);
	reg |= (AR5315_PCI_EXT_INT | AR5315_PCI_ABORT_INT);
	sysRegWrite(AR5315_PCI_INT_MASK, reg);

	reg = sysRegRead(AR5315_PCI_INTEN_REG);
	reg |= AR5315_PCI_INT_ENABLE;
	sysRegWrite(AR5315_PCI_INTEN_REG, reg);

	return 0;
}

static void ar5315_pci_fixup(struct pci_dev *dev)
{
	struct pci_bus *bus = dev->bus;

	if ((PCI_SLOT(dev->devfn) != 3) || (PCI_FUNC(dev->devfn) != 0) || (bus->number != 0))
		return;

#define _DEV	bus->number, PCI_SLOT(dev->devfn), PCI_FUNC(dev->devfn)
	printk("PCI: fixing up device %d,%d,%d\n", _DEV);
	/* fix up mbars */
	config_access(_DEV, PCI_BASE_ADDRESS_0, 4, HOST_PCI_MBAR0, 1);
	config_access(_DEV, PCI_BASE_ADDRESS_1, 4, HOST_PCI_MBAR1, 1);
	config_access(_DEV, PCI_BASE_ADDRESS_2, 4, HOST_PCI_MBAR2, 1);
	config_access(_DEV, PCI_COMMAND, 4,
		PCI_COMMAND_MEMORY|PCI_COMMAND_MASTER|PCI_COMMAND_SPECIAL|
		PCI_COMMAND_INVALIDATE|PCI_COMMAND_PARITY|PCI_COMMAND_SERR|
		PCI_COMMAND_FAST_BACK, 1);
#undef _DEV
}
DECLARE_PCI_FIXUP_HEADER(PCI_ANY_ID, PCI_ANY_ID, ar5315_pci_fixup);

int __init ar5315_pci_init(void)
{
	u32 reg;

	if (mips_machtype != MACH_ATHEROS_AR2315)
		return -ENODEV;

	printk("AR531x PCI init... \n");

	cfgaddr = (u32) ioremap_nocache(0x80000000, 1*1024*1024); /* Remap PCI config space */
	ar531x_pci_controller.io_map_base =
		(unsigned long) ioremap_nocache(AR531X_MEM_BASE + AR531X_MEM_SIZE, AR531X_IO_SIZE);
	set_io_port_base(ar531x_pci_controller.io_map_base); /* PCI I/O space */

	reg = sysRegRead(AR5315_RESET);
	sysRegWrite(AR5315_RESET, reg | AR5315_RESET_PCIDMA);

	udelay(10*1000);

	sysRegWrite(AR5315_RESET, reg & ~AR5315_RESET_PCIDMA);
	sysRegRead(AR5315_RESET);     /* read after */

	udelay(10*1000);

	reg = sysRegRead(AR5315_ENDIAN_CTL);
	reg |= AR5315_CONFIG_PCIAHB | AR5315_CONFIG_PCIAHB_BRIDGE;

	sysRegWrite(AR5315_ENDIAN_CTL, reg);

	reg = sysRegRead(AR5315_PCICLK);
	reg = 4;
	sysRegWrite(AR5315_PCICLK, reg);

	reg = sysRegRead(AR5315_AHB_ARB_CTL);
	reg |= (ARB_PCI);
	sysRegWrite(AR5315_AHB_ARB_CTL, reg);

	reg = sysRegRead(AR5315_IF_CTL);
	reg &= ~(IF_PCI_CLK_MASK | IF_MASK);
	reg |= (IF_PCI | IF_PCI_HOST | IF_PCI_INTR | (IF_PCI_CLK_OUTPUT_CLK << IF_PCI_CLK_SHIFT));

	sysRegWrite(AR5315_IF_CTL, reg);

	/* Reset the PCI bus by setting bits 5-4 in PCI_MCFG */
	reg = sysRegRead(AR5315_PCI_MISC_CONFIG);
	reg &= ~(AR5315_PCIMISC_RST_MODE);
	reg |= AR5315_PCIRST_LOW;
	sysRegWrite(AR5315_PCI_MISC_CONFIG, reg);

	/* wait for 100 ms */
	udelay(100*1000);

	/* Bring the PCI out of reset */
	reg = sysRegRead(AR5315_PCI_MISC_CONFIG);
	reg &= ~(AR5315_PCIMISC_RST_MODE);
	reg |= (AR5315_PCIRST_HIGH | AR5315_PCICACHE_DIS | 0x8);
	sysRegWrite(AR5315_PCI_MISC_CONFIG, reg);

	sysRegWrite(AR5315_PCI_UNCACHE_CFG,
			0x1E | /* 1GB uncached */
			(1 << 5) | /* Enable uncached */
			(0x2 << 30) /* Base: 0x80000000 */
	);
	(void)sysRegRead(AR5315_PCI_UNCACHE_CFG); /* flush */

	udelay(500*1000);

	/* dirty hack - anyone with a datasheet that knows the memory map ? */
	ioport_resource.start = 0x10000000;
	ioport_resource.end = 0xffffffff;
	iomem_resource.start = 0x10000000;
	iomem_resource.end = 0xffffffff;

	register_pci_controller(&ar531x_pci_controller);

	printk("done\n");
	return 0;
}

arch_initcall(ar5315_pci_init);
