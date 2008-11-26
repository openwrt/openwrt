/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2008 Maxime Bizon <mbizon@freebox.fr>
 */

#include <linux/types.h>
#include <linux/pci.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/io.h>

#include "pci-bcm63xx.h"

/*
 * swizzle 32bits data to return only the needed part
 */
static int postprocess_read(u32 data, int where, unsigned int size)
{
	u32 ret;

	ret = 0;
	switch (size) {
	case 1:
		ret = (data >> ((where & 3) << 3)) & 0xff;
		break;
	case 2:
		ret = (data >> ((where & 3) << 3)) & 0xffff;
		break;
	case 4:
		ret = data;
		break;
	}
	return ret;
}

static int preprocess_write(u32 orig_data, u32 val, int where,
			    unsigned int size)
{
	u32 ret;

	ret = 0;
	switch (size) {
	case 1:
		ret = (orig_data & ~(0xff << ((where & 3) << 3))) |
			(val << ((where & 3) << 3));
		break;
	case 2:
		ret = (orig_data & ~(0xffff << ((where & 3) << 3))) |
			(val << ((where & 3) << 3));
		break;
	case 4:
		ret = val;
		break;
	}
	return ret;
}

/*
 * setup hardware for a configuration cycle with given parameters
 */
static int bcm63xx_setup_cfg_access(int type, unsigned int busn,
				    unsigned int devfn, int where)
{
	unsigned int slot, func, reg;
	u32 val;

	slot = PCI_SLOT(devfn);
	func = PCI_FUNC(devfn);
	reg = where >> 2;

	/* sanity check */
	if (slot > (MPI_L2PCFG_DEVNUM_MASK >> MPI_L2PCFG_DEVNUM_SHIFT))
		return 1;

	if (func > (MPI_L2PCFG_FUNC_MASK >> MPI_L2PCFG_FUNC_SHIFT))
		return 1;

	if (reg > (MPI_L2PCFG_REG_MASK >> MPI_L2PCFG_REG_SHIFT))
		return 1;

	/* ok, setup config access */
	val = (reg << MPI_L2PCFG_REG_SHIFT);
	val |= (func << MPI_L2PCFG_FUNC_SHIFT);
	val |= (slot << MPI_L2PCFG_DEVNUM_SHIFT);
	val |= MPI_L2PCFG_CFG_USEREG_MASK;
	val |= MPI_L2PCFG_CFG_SEL_MASK;
	/* type 0 cycle for local bus, type 1 cycle for anything else */
	if (type != 0) {
		/* FIXME: how to specify bus ??? */
		val |= (1 << MPI_L2PCFG_CFG_TYPE_SHIFT);
	}
	bcm_mpi_writel(val, MPI_L2PCFG_REG);

	return 0;
}

static int bcm63xx_do_cfg_read(int type, unsigned int busn,
				unsigned int devfn, int where, int size,
				u32 *val)
{
	u32 data;

	/* two phase cycle, first we write address, then read data at
	 * another location, caller already has a spinlock so no need
	 * to add one here  */
	if (bcm63xx_setup_cfg_access(type, busn, devfn, where))
		return PCIBIOS_DEVICE_NOT_FOUND;
	iob();
	data = le32_to_cpu(__raw_readl(pci_iospace_start));
	/* restore IO space normal behaviour */
	bcm_mpi_writel(0, MPI_L2PCFG_REG);

	*val = postprocess_read(data, where, size);

	return PCIBIOS_SUCCESSFUL;
}

static int bcm63xx_do_cfg_write(int type, unsigned int busn,
				 unsigned int devfn, int where, int size,
				 u32 val)
{
	u32 data;

	/* two phase cycle, first we write address, then write data to
	 * another location, caller already has a spinlock so no need
	 * to add one here  */
	if (bcm63xx_setup_cfg_access(type, busn, devfn, where))
		return PCIBIOS_DEVICE_NOT_FOUND;
	iob();

	data = le32_to_cpu(__raw_readl(pci_iospace_start));
	data = preprocess_write(data, val, where, size);

	__raw_writel(cpu_to_le32(data), pci_iospace_start);
	wmb();
	/* no way to know the access is done, we have to wait */
	udelay(500);
	/* restore IO space normal behaviour */
	bcm_mpi_writel(0, MPI_L2PCFG_REG);

	return PCIBIOS_SUCCESSFUL;
}

static int bcm63xx_pci_read(struct pci_bus *bus, unsigned int devfn,
			     int where, int size, u32 *val)
{
	int type;

	type = bus->parent ? 1 : 0;

	if (type == 0 && PCI_SLOT(devfn) == CARDBUS_PCI_IDSEL)
		return PCIBIOS_DEVICE_NOT_FOUND;

	return bcm63xx_do_cfg_read(type, bus->number, devfn,
				    where, size, val);
}

static int bcm63xx_pci_write(struct pci_bus *bus, unsigned int devfn,
			      int where, int size, u32 val)
{
	int type;

	type = bus->parent ? 1 : 0;

	if (type == 0 && PCI_SLOT(devfn) == CARDBUS_PCI_IDSEL)
		return PCIBIOS_DEVICE_NOT_FOUND;

	return bcm63xx_do_cfg_write(type, bus->number, devfn,
				     where, size, val);
}

struct pci_ops bcm63xx_pci_ops = {
	.read   = bcm63xx_pci_read,
	.write  = bcm63xx_pci_write
};
