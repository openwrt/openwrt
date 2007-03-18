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

#include <linux/init.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/ioport.h>
#include <linux/errno.h>
#include <linux/platform_device.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/device.h>
#include <asm/addrspace.h>
#include <asm/io.h>
#include <asm/ar7/vlynq.h>

#define PER_DEVICE_IRQS 32

#define VLYNQ_CTRL_PM_ENABLE       0x80000000
#define VLYNQ_CTRL_CLOCK_INT       0x00008000
#define VLYNQ_CTRL_CLOCK_DIV(x)    ((x & 7) << 16)
#define VLYNQ_CTRL_INT_LOCAL       0x00004000
#define VLYNQ_CTRL_INT_ENABLE      0x00002000
#define VLYNQ_CTRL_INT_VECTOR(x)   ((x & 0x1f) << 8)
#define VLYNQ_CTRL_INT2CFG         0x00000080
#define VLYNQ_CTRL_RESET           0x00000001

#define VLYNQ_STATUS_RERROR        0x00000100
#define VLYNQ_STATUS_LERROR        0x00000080
#define VLYNQ_STATUS_LINK          0x00000001

#define VINT_ENABLE    0x00000100
#define VINT_TYPE_EDGE 0x00000080
#define VINT_LEVEL_LOW 0x00000040
#define VINT_VECTOR(x) (x & 0x1f)
#define VINT_OFFSET(irq) (8 * ((irq) % 4))

#define VLYNQ_AUTONEGO_V2          0x00010000

struct vlynq_regs {
	volatile u32 revision;
	volatile u32 control;
	volatile u32 status;
	volatile u32 int_prio;
	volatile u32 int_status;
	volatile u32 int_pending;
	volatile u32 int_ptr;
	volatile u32 tx_offset;
	volatile struct vlynq_mapping rx_mapping[4];
	volatile u32 chip;
	volatile u32 autonego;
	volatile u32 unused[6];
	volatile u32 int_device[8];
} __attribute__ ((packed));

#ifdef VLYNQ_DEBUG
static void vlynq_dump_regs(struct vlynq_device *dev)
{
	int i;
	printk("VLYNQ local=%p remote=%p\n", dev->local, dev->remote);
	for (i = 0; i < 32; i++) {
		printk("VLYNQ: local %d: %08x\n", i + 1, ((u32 *)dev->local)[i]);
		printk("VLYNQ: remote %d: %08x\n", i + 1, ((u32 *)dev->remote)[i]);
	}
}

static void vlynq_dump_mem(u32 *base, int count)
{
	int i;
	for (i = 0; i < (count + 3) / 4; i++) {
		if (i % 4 == 0) printk("\nMEM[0x%04x]:", i * 4);
		printk(" 0x%08x", *(base + i));
	}
	printk("\n");
}
#endif

int vlynq_linked(struct vlynq_device *dev)
{
	int i;
	for (i = 0; i < 10; i++)
		if (dev->local->status & VLYNQ_STATUS_LINK) {
			printk("%s: linked\n", dev->dev.bus_id);
			return 1;
		} else {
			mdelay(1);
		}
	return 0;
}

static void vlynq_irq_unmask(unsigned int irq)
{
	volatile u32 val;
	struct vlynq_device *dev = irq_desc[irq].chip_data;
	int virq;

	BUG_ON(!dev);
	virq = irq - dev->irq_start;
	val = dev->remote->int_device[virq >> 2];
	val |= VINT_ENABLE << VINT_OFFSET(virq);
	dev->remote->int_device[virq >> 2] = val;
}

static void vlynq_irq_mask(unsigned int irq)
{
	volatile u32 val;
	struct vlynq_device *dev = irq_desc[irq].chip_data;
	int virq;

	BUG_ON(!dev);
	virq = irq - dev->irq_start;
	val = dev->remote->int_device[virq >> 2];
	val &= ~(VINT_ENABLE << VINT_OFFSET(virq));
	dev->remote->int_device[virq >> 2] = val;
}

static int vlynq_irq_type(unsigned int irq, unsigned int flow_type)
{
	volatile u32 val;
	struct vlynq_device *dev = irq_desc[irq].chip_data;
	int virq;

	BUG_ON(!dev);
	virq = irq - dev->irq_start;
	val = dev->remote->int_device[virq >> 2];
	switch (flow_type & IRQ_TYPE_SENSE_MASK) {
	case IRQ_TYPE_EDGE_RISING:
	case IRQ_TYPE_EDGE_FALLING:
	case IRQ_TYPE_EDGE_BOTH:
		val |= VINT_TYPE_EDGE << VINT_OFFSET(virq);
		val &= ~(VINT_LEVEL_LOW << VINT_OFFSET(virq));
		break;
	case IRQ_TYPE_LEVEL_HIGH:
		val &= ~(VINT_TYPE_EDGE << VINT_OFFSET(virq));
		val &= ~(VINT_LEVEL_LOW << VINT_OFFSET(virq));
		break;
	case IRQ_TYPE_LEVEL_LOW:
		val &= ~(VINT_TYPE_EDGE << VINT_OFFSET(virq));
		val |= VINT_LEVEL_LOW << VINT_OFFSET(virq);
		break;
	default:
		return -EINVAL;
	}
	dev->remote->int_device[virq >> 2] = val;
	return 0;
}

static irqreturn_t vlynq_irq(int irq, void *dev_id)
{
	struct vlynq_device *dev = dev_id;
	u32 status, ack;
	int virq = 0;

	status = dev->local->int_status;
	dev->local->int_status = status;

	if (status & (1 << dev->local_irq)) { /* Local vlynq IRQ. Ack */
		ack = dev->local->status;
		dev->local->status = ack;
	}

	if (status & (1 << dev->remote_irq)) { /* Remote vlynq IRQ. Ack */
		ack = dev->remote->status;
		dev->remote->status = ack;
	} 

	status &= ~((1 << dev->local_irq) | (1 << dev->remote_irq));
	while (status) {
		if (status & 1) /* Remote device IRQ. Pass. */
			do_IRQ(dev->irq_start + virq);
		status >>= 1;
		virq++;
	}

	return IRQ_HANDLED;
}

static struct irq_chip vlynq_irq_chip = {
        .name = "vlynq",
        .unmask = vlynq_irq_unmask,
        .mask = vlynq_irq_mask,
	.set_type = vlynq_irq_type,
};

static int vlynq_setup_irq(struct vlynq_device *dev)
{
	u32 val;
	int i;

	if (dev->local_irq == dev->remote_irq) {
		printk("%s: local vlynq irq should be different from remote\n", 
		       dev->dev.bus_id);
		return -EINVAL;
	}

	val = VLYNQ_CTRL_INT_VECTOR(dev->local_irq);
	val |= VLYNQ_CTRL_INT_ENABLE | VLYNQ_CTRL_INT_LOCAL |
		VLYNQ_CTRL_INT2CFG;
	dev->local->int_ptr = 0x14;
	dev->local->control |= val;

	val = VLYNQ_CTRL_INT_VECTOR(dev->remote_irq);
	val |= VLYNQ_CTRL_INT_ENABLE;
	dev->remote->int_ptr = 0x14;
	dev->remote->control |= val;

	for (i = 0; i < PER_DEVICE_IRQS; i++) {
		if ((i == dev->local_irq) || (i == dev->remote_irq))
			continue;
		irq_desc[dev->irq_start + i].status = IRQ_DISABLED;
		irq_desc[dev->irq_start + i].action = 0;
		irq_desc[dev->irq_start + i].depth = 1;
		irq_desc[dev->irq_start + i].chip = &vlynq_irq_chip;
		irq_desc[dev->irq_start + i].chip_data = dev;
		dev->remote->int_device[i >> 2] = 0;
	}

	if (request_irq(dev->irq, vlynq_irq, SA_SHIRQ, "AR7 VLYNQ", dev)) {
		printk("%s: request_irq failed\n", dev->dev.bus_id);
		return -EAGAIN;
	}

	return 0;
}

static void vlynq_free_irq(struct vlynq_device *dev)
{
	free_irq(dev->irq, dev);
}

static void vlynq_device_release(struct device *dev)
{
	struct vlynq_device *vdev = to_vlynq_device(dev);
	kfree(vdev);
}

static int vlynq_device_probe(struct device *dev)
{
	struct vlynq_driver *drv = to_vlynq_driver(dev->driver);
	if (drv->probe)
		return drv->probe(to_vlynq_device(dev));
	return 0;
}

static int vlynq_device_remove(struct device *dev)
{
	struct vlynq_driver *drv = to_vlynq_driver(dev->driver);
	if (drv->remove)
		return drv->remove(to_vlynq_device(dev));
	return 0;
}

int __vlynq_register_driver(struct vlynq_driver *driver, struct module *owner)
{
	driver->driver.name = driver->name;
	driver->driver.bus = &vlynq_bus_type;
/*	driver->driver.owner = owner;*/
	return driver_register(&driver->driver);
}
EXPORT_SYMBOL(__vlynq_register_driver);

void vlynq_unregister_driver(struct vlynq_driver *driver)
{
	driver_unregister(&driver->driver);
}
EXPORT_SYMBOL(vlynq_unregister_driver);

int vlynq_device_enable(struct vlynq_device *dev)
{
	u32 val;
	int result;
	struct plat_vlynq_ops *ops = dev->dev.platform_data;

	result = ops->on(dev);
	if (result)
		return result;

	dev->local->control = 0;
	dev->remote->control = 0;

	if (vlynq_linked(dev)) 
		return vlynq_setup_irq(dev);

	for (val = 0; val < 8; val++) {
		dev->local->control = VLYNQ_CTRL_CLOCK_DIV(val) |
			VLYNQ_CTRL_CLOCK_INT;
		if (vlynq_linked(dev)) 
			return vlynq_setup_irq(dev);
	}
}

void vlynq_device_disable(struct vlynq_device *dev)
{
	struct plat_vlynq_ops *ops = dev->dev.platform_data;

	vlynq_free_irq(dev);
	ops->off(dev);
}

u32 vlynq_local_id(struct vlynq_device *dev)
{
	return dev->local->chip;
}

u32 vlynq_remote_id(struct vlynq_device *dev)
{
	return dev->remote->chip;
}

void vlynq_set_local_mapping(struct vlynq_device *dev, u32 tx_offset,
			     struct vlynq_mapping *mapping)
{
	int i;

	dev->local->tx_offset = tx_offset;
	for (i = 0; i < 4; i++) {
		dev->local->rx_mapping[i].offset = mapping[i].offset;
		dev->local->rx_mapping[i].size = mapping[i].size;
	}
}

void vlynq_set_remote_mapping(struct vlynq_device *dev, u32 tx_offset,
			      struct vlynq_mapping *mapping)
{
	int i;

	dev->remote->tx_offset = tx_offset;
	for (i = 0; i < 4; i++) {
		dev->remote->rx_mapping[i].offset = mapping[i].offset;
		dev->remote->rx_mapping[i].size = mapping[i].size;
	}
}

int vlynq_virq_to_irq(struct vlynq_device *dev, int virq)
{
	if ((virq < 0) || (virq >= PER_DEVICE_IRQS)) 
		return -EINVAL;

	if ((virq == dev->local_irq) || (virq == dev->remote_irq))
		return -EINVAL;

	return dev->irq_start + virq;
}

int vlynq_irq_to_virq(struct vlynq_device *dev, int irq)
{
	if ((irq < dev->irq_start) || (irq >= dev->irq_start + PER_DEVICE_IRQS)) 
		return -EINVAL;

	return irq - dev->irq_start;
}

int vlynq_set_local_irq(struct vlynq_device *dev, int virq)
{
	if ((virq < 0) || (virq >= PER_DEVICE_IRQS)) 
		return -EINVAL;

	if (virq == dev->remote_irq)
		return -EINVAL;

	dev->local_irq = virq;

	return 0;
}

int vlynq_set_remote_irq(struct vlynq_device *dev, int virq)
{
	if ((virq < 0) || (virq >= PER_DEVICE_IRQS)) 
		return -EINVAL;

	if (virq == dev->local_irq)
		return -EINVAL;

	dev->remote_irq = virq;

	return 0;
}

static int vlynq_probe(struct platform_device *pdev)
{
	struct vlynq_device *dev;
	struct resource *regs_res, *mem_res, *irq_res;
	int len, result;

	if (strcmp(pdev->name, "vlynq"))
		return -ENODEV;

	regs_res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "regs");
	if (!regs_res)
		return -ENODEV;

	mem_res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "mem");
	if (!mem_res)
		return -ENODEV;

	irq_res = platform_get_resource_byname(pdev, IORESOURCE_IRQ, "devirq");
	if (!irq_res)
		return -ENODEV;

	dev = kmalloc(sizeof(struct vlynq_device), GFP_KERNEL);
	if (!dev) {
		printk(KERN_ERR "vlynq: failed to allocate device structure\n");
		return -ENOMEM;
	}

	memset(dev, 0, sizeof(struct vlynq_device));

	dev->id = pdev->id;
	dev->dev.bus = &vlynq_bus_type;
	dev->dev.parent = &pdev->dev;
	snprintf(dev->dev.bus_id, BUS_ID_SIZE, "vlynq%d", dev->id);
	dev->dev.bus_id[BUS_ID_SIZE - 1] = 0;
	dev->dev.platform_data = pdev->dev.platform_data;
	dev->dev.release = vlynq_device_release;

	dev->regs_start = regs_res->start;
	dev->regs_end = regs_res->end;
	dev->mem_start = mem_res->start;
	dev->mem_end = mem_res->end;

	len = regs_res->end - regs_res->start;
	if (!request_mem_region(regs_res->start, len, dev->dev.bus_id)) {
		printk("%s: Can't request vlynq registers\n", dev->dev.bus_id);
		result = -ENXIO;
                goto fail_request;
	}

	dev->local = ioremap_nocache(regs_res->start, len);
        if (!dev->local) {
		printk("%s: Can't remap vlynq registers\n", dev->dev.bus_id);
		result = -ENXIO;
                goto fail_remap;
	}

	dev->remote = (struct vlynq_regs *)((u32)dev->local + 128);

	dev->irq = platform_get_irq_byname(pdev, "irq");
	dev->irq_start = irq_res->start;
	dev->irq_end = irq_res->end;
	dev->local_irq = 31;
	dev->remote_irq = 30;

	if (device_register(&dev->dev))
		goto fail_register;
	platform_set_drvdata(pdev, dev);

	printk("%s: regs 0x%p, irq %d, mem 0x%p\n",
	       dev->dev.bus_id, (void *)dev->regs_start, dev->irq,
	       (void *)dev->mem_start);

	return 0;

fail_register:
fail_remap:
	iounmap(dev->local);
fail_request:
	release_mem_region(regs_res->start, len);
	kfree(dev);
	return result;
}

static int vlynq_remove(struct platform_device *pdev)
{
	struct vlynq_device *dev = platform_get_drvdata(pdev);

	device_unregister(&dev->dev);
	release_mem_region(dev->regs_start, dev->regs_end - dev->regs_start);

	kfree(dev);

	return 0;
}

static struct platform_driver vlynq_driver = {
	.driver.name = "vlynq",
	.probe = vlynq_probe,
	.remove = vlynq_remove,
};

struct bus_type vlynq_bus_type = {
	.name = "vlynq",
	.probe = vlynq_device_probe,
	.remove = vlynq_device_remove,
};
EXPORT_SYMBOL(vlynq_bus_type);

#ifdef CONFIG_PCI
extern void vlynq_pci_init(void);
#endif
int __init vlynq_init(void)
{
	int res = 0;

	res = bus_register(&vlynq_bus_type);
	if (res)
		goto fail_bus;

	res = platform_driver_register(&vlynq_driver);
	if (res)
		goto fail_platform;

#ifdef CONFIG_PCI
	vlynq_pci_init();
#endif

	return 0;

fail_platform:
	bus_unregister(&vlynq_bus_type);
fail_bus:
	return res;
}

/*
void __devexit vlynq_exit(void)
{
	platform_driver_unregister(&vlynq_driver);
	bus_unregister(&vlynq_bus_type);
}
*/


subsys_initcall(vlynq_init);
