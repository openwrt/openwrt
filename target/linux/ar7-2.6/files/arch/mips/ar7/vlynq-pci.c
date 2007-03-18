/*
 * $Id$
 * 
 * Copyright (C) 2006, 2007 OpenWrt.org
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <linux/types.h>
#include <linux/pci.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/irq.h>
#include <asm/ar7/vlynq.h>

#define VLYNQ_PCI_SLOTS 2

struct vlynq_reg_config {
	u32 offset;
	u32 value;
};

struct vlynq_pci_config {
	u32 chip_id;
	char name[32];
	struct vlynq_mapping rx_mapping[4];
	int irq;
	int irq_type;
	u32 chip;
	u32 class;
	int num_regs;
	struct vlynq_reg_config regs[10];
};

struct vlynq_pci_private {
	u32 latency;
	u32 cache_line;
	u32 command;
	u32 sz_mask;
	struct vlynq_pci_config *config;
};

static struct vlynq_pci_config known_devices[] = {
	{
		.chip_id = 0x00000009, .name = "TI ACX111",
		.rx_mapping = {
			{ .size = 0x22000, .offset = 0xf0000000 },
			{ .size = 0x40000, .offset = 0xc0000000 },
			{ .size = 0x0, .offset = 0x0 },
			{ .size = 0x0, .offset = 0x0 },
		},
		.irq = 0, .chip = 0x9066104c,
		.irq_type = IRQ_TYPE_EDGE_RISING,
		.class = PCI_CLASS_NETWORK_OTHER,
		.num_regs = 5,
		.regs = { 
			{ .offset = 0x790, .value = (0xd0000000 - (ARCH_PFN_OFFSET << PAGE_SHIFT)) },
			{ .offset = 0x794, .value = (0xd0000000 - (ARCH_PFN_OFFSET << PAGE_SHIFT)) },
			{ .offset = 0x740, .value = 0 },
			{ .offset = 0x744, .value = 0x00010000 },
			{ .offset = 0x764, .value = 0x00010000 },
		},
	},
};

static struct vlynq_device *slots[VLYNQ_PCI_SLOTS] = { NULL, };

static struct resource vlynq_io_resource = {
	.start	= 0x00000000,
	.end	= 0x00000000,
	.name	= "pci IO space",
	.flags	= IORESOURCE_IO
};

static struct resource vlynq_mem_resource = {
	.start	= 0x00000000,
	.end	= 0x00000000,
	.name	= "pci memory space",
	.flags	= IORESOURCE_MEM
};

static inline u32 vlynq_get_mapped(struct vlynq_device *dev, int res)
{
	int i;
	struct vlynq_pci_private *priv = dev->priv;
	u32 ret = dev->mem_start;
	if (!priv->config->rx_mapping[res].size) return 0;
	for (i = 0; i < res; i++)
		ret += priv->config->rx_mapping[i].size;

	return ret;
}

static inline u32 vlynq_read(u32 val, int size) {
	switch (size) {
	case 1:
		return *(u8 *)&val;
	case 2:
		return *(u16 *)&val;
	}
	return val;
}

static int vlynq_config_read(struct pci_bus *bus, unsigned int devfn, int where, int size, u32 *val)
{
	struct vlynq_device *dev;
	struct vlynq_pci_private *priv;
	int resno, slot = PCI_SLOT(devfn);

	if ((size == 2) && (where & 1))
		return PCIBIOS_BAD_REGISTER_NUMBER;
	else if ((size == 4) && (where & 3))
		return PCIBIOS_BAD_REGISTER_NUMBER;

	if (slot >= VLYNQ_PCI_SLOTS)
		return PCIBIOS_DEVICE_NOT_FOUND;

	dev = slots[slot];

	if (!dev || (PCI_FUNC(devfn) > 0))
		return PCIBIOS_DEVICE_NOT_FOUND;

	priv = dev->priv;

	switch (where) {
	case PCI_VENDOR_ID:
		*val = vlynq_read(priv->config->chip, size);
		break;
	case PCI_DEVICE_ID:
		*val = priv->config->chip & 0xffff;
	case PCI_COMMAND:
		*val = priv->command;
	case PCI_STATUS:
/*		*val = PCI_STATUS_CAP_LIST;*/
		*val = 0;
		break;
	case PCI_CLASS_REVISION:
		*val = priv->config->class;
		break;
	case PCI_LATENCY_TIMER:
		*val = priv->latency;
		break;
	case PCI_HEADER_TYPE:
		*val = PCI_HEADER_TYPE_NORMAL;
		break;
	case PCI_CACHE_LINE_SIZE:
		*val = priv->cache_line;
		break;
	case PCI_BASE_ADDRESS_0:
	case PCI_BASE_ADDRESS_1:
	case PCI_BASE_ADDRESS_2:
	case PCI_BASE_ADDRESS_3:
		resno = (where - PCI_BASE_ADDRESS_0) >> 2;
		if (priv->sz_mask & (1 << resno)) {
		        priv->sz_mask &= ~(1 << resno);
			*val = priv->config->rx_mapping[resno].size;
		} else {
			*val = vlynq_get_mapped(dev, resno);
		}
		break;
	case PCI_BASE_ADDRESS_4:
	case PCI_BASE_ADDRESS_5:
	case PCI_SUBSYSTEM_VENDOR_ID:
	case PCI_SUBSYSTEM_ID:
	case PCI_ROM_ADDRESS:
	case PCI_INTERRUPT_LINE:
	case PCI_CARDBUS_CIS:
	case PCI_CAPABILITY_LIST:
	case PCI_INTERRUPT_PIN:
		*val = 0;
		break;
	default:
		printk("%s: Read of unknown register 0x%x (size %d)\n", 
		       dev->dev.bus_id, where, size);
		return PCIBIOS_BAD_REGISTER_NUMBER;
	}
	return PCIBIOS_SUCCESSFUL;
}

static int vlynq_config_write(struct pci_bus *bus, unsigned int devfn, int where, int size, u32 val)
{
	struct vlynq_device *dev;
	struct vlynq_pci_private *priv;
	int resno, slot = PCI_SLOT(devfn);

	if ((size == 2) && (where & 1))
		return PCIBIOS_BAD_REGISTER_NUMBER;
	else if ((size == 4) && (where & 3))
		return PCIBIOS_BAD_REGISTER_NUMBER;

	if (slot >= VLYNQ_PCI_SLOTS)
		return PCIBIOS_DEVICE_NOT_FOUND;

	dev = slots[slot];

	if (!dev || (PCI_FUNC(devfn) > 0))
		return PCIBIOS_DEVICE_NOT_FOUND;

	priv = dev->priv;

	switch (where) {
	case PCI_VENDOR_ID:
	case PCI_DEVICE_ID:
	case PCI_STATUS:
	case PCI_CLASS_REVISION:
	case PCI_HEADER_TYPE:
	case PCI_CACHE_LINE_SIZE:
	case PCI_SUBSYSTEM_VENDOR_ID:
	case PCI_SUBSYSTEM_ID:
	case PCI_INTERRUPT_LINE:
	case PCI_INTERRUPT_PIN:
	case PCI_CARDBUS_CIS:
	case PCI_CAPABILITY_LIST:
		return PCIBIOS_FUNC_NOT_SUPPORTED;
	case PCI_COMMAND:
		priv->command = val;
	case PCI_LATENCY_TIMER:
		priv->latency = val;
		break;
	case PCI_BASE_ADDRESS_0:
	case PCI_BASE_ADDRESS_1:
	case PCI_BASE_ADDRESS_2:
	case PCI_BASE_ADDRESS_3:
		if (val == 0xffffffff) {
			resno = (where - PCI_BASE_ADDRESS_0) >> 2;
			priv->sz_mask |= (1 << resno);
			break;
		}
	case PCI_BASE_ADDRESS_4:
	case PCI_BASE_ADDRESS_5:
	case PCI_ROM_ADDRESS:
		break;
	default:
		printk("%s: Write to unknown register 0x%x (size %d) value=0x%x\n", 
		       dev->dev.bus_id, where, size, val);
		return PCIBIOS_BAD_REGISTER_NUMBER;
	}
	return PCIBIOS_SUCCESSFUL;
}

static struct pci_ops vlynq_pci_ops = {
	vlynq_config_read,
	vlynq_config_write
};

static struct pci_controller vlynq_controller = {
	.pci_ops	= &vlynq_pci_ops,
	.io_resource	= &vlynq_io_resource,
	.mem_resource	= &vlynq_mem_resource,
};

static int vlynq_pci_probe(struct vlynq_device *dev)
{
	int result, i;
	u32 chip_id, addr;
	struct vlynq_pci_private *priv;
	struct vlynq_mapping mapping[4] = { { 0, }, };
	struct vlynq_pci_config *config = NULL;

	result = vlynq_set_local_irq(dev, 31);
	if (result)
		return result;

	result = vlynq_set_remote_irq(dev, 30);
	if (result)
		return result;

	result = vlynq_device_enable(dev);
	if (result)
		return result;

	chip_id = vlynq_remote_id(dev);
	for (i = 0; i < ARRAY_SIZE(known_devices); i++)
		if (chip_id == known_devices[i].chip_id)
			config = &known_devices[i];

	if (!config) {
		printk("vlynq-pci: skipping unknown device "
		       "%04x:%04x at %s\n", chip_id >> 16, 
		       chip_id & 0xffff, dev->dev.bus_id);
		result = -ENODEV;
		goto fail;
	}

	printk("vlynq-pci: attaching device %s at %s\n",
	       config->name, dev->dev.bus_id);

	priv = kmalloc(sizeof(struct vlynq_pci_private), GFP_KERNEL);
	if (!priv) {
		printk(KERN_ERR "%s: failed to allocate private data\n",
		       dev->dev.bus_id);
		result = -ENOMEM;
		goto fail;
	}

	memset(priv, 0, sizeof(struct vlynq_pci_private));
	priv->latency = 64;
	priv->cache_line = 32;
	priv->config = config;

	mapping[0].offset = ARCH_PFN_OFFSET << PAGE_SHIFT;
	mapping[0].size = 0x02000000;
	vlynq_set_local_mapping(dev, dev->mem_start, mapping);
	vlynq_set_remote_mapping(dev, 0, config->rx_mapping);

	set_irq_type(vlynq_virq_to_irq(dev, config->irq), config->irq_type);

	addr = (u32)ioremap_nocache(dev->mem_start, 0x10000);
	if (!addr) {
		printk(KERN_ERR "%s: failed to remap io memory\n",
		       dev->dev.bus_id);
		result = -ENXIO;
		goto fail;
	}

	for (i = 0; i < config->num_regs; i++)
		*(volatile u32 *)(addr + config->regs[i].offset) =
			config->regs[i].value;

	dev->priv = priv;
	for (i = 0; i < VLYNQ_PCI_SLOTS; i++) {
		if (!slots[i]) {
			slots[i] = dev;
			break;
		}
	}

	return 0;

fail:
	vlynq_device_disable(dev);

	return result;
}

static int vlynq_pci_remove(struct vlynq_device *dev)
{
	int i;
	struct vlynq_pci_private *priv = dev->priv;

	for (i = 0; i < VLYNQ_PCI_SLOTS; i++)
		if (slots[i] == dev)
			slots[i] = NULL;

	vlynq_device_disable(dev);
	kfree(priv);

	return 0;
}

static struct vlynq_driver vlynq_pci = {
	.name = "PCI over VLYNQ emulation",
	.probe = vlynq_pci_probe,
	.remove = vlynq_pci_remove,
};

int vlynq_pci_init(void)
{
	int res;
	res = vlynq_register_driver(&vlynq_pci);
	if (res) 
		return res;

	register_pci_controller(&vlynq_controller);

	return 0;
}

int pcibios_map_irq(struct pci_dev *pdev, u8 slot, u8 pin)
{
	struct vlynq_device *dev;
	struct vlynq_pci_private *priv;

	dev = slots[slot];

	if (!dev)
		return 0;

	priv = dev->priv;

	return vlynq_virq_to_irq(dev, priv->config->irq);
}
                                                                        
/* Do platform specific device initialization at pci_enable_device() time */
int pcibios_plat_dev_init(struct pci_dev *dev)
{
	return 0;
}
