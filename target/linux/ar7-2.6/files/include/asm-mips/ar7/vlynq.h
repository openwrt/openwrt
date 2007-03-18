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


#ifndef __VLYNQ_H__
#define __VLYNQ_H__

struct vlynq_mapping {
	u32 size;
	u32 offset;
} __attribute__ ((packed));

struct vlynq_device_id {
	u32 id;
};

struct vlynq_regs;
struct vlynq_device {
	u32 id;
	int irq;
	int local_irq;
	int remote_irq;
	int clock_div;
	u32 regs_start, regs_end;
	u32 mem_start, mem_end;
	u32 irq_start, irq_end;
	void *priv;
	struct vlynq_regs *local;
	struct vlynq_regs *remote;
	struct device dev;
};

struct vlynq_driver {
	char *name;
	int (*probe)(struct vlynq_device *dev);
	int (*remove)(struct vlynq_device *dev);
	struct device_driver driver;
};

#define to_vlynq_driver(drv) container_of(drv, struct vlynq_driver, driver)

struct plat_vlynq_ops {
	int (*on)(struct vlynq_device *dev);
	void (*off)(struct vlynq_device *dev);
};

#define to_vlynq_device(device) container_of(device, struct vlynq_device, dev)

extern struct bus_type vlynq_bus_type;

extern int __vlynq_register_driver(struct vlynq_driver *driver, 
				   struct module *owner);

static inline int vlynq_register_driver(struct vlynq_driver *driver)
{
	return __vlynq_register_driver(driver, THIS_MODULE);
}

extern void vlynq_unregister_driver(struct vlynq_driver *driver);
extern int vlynq_device_enable(struct vlynq_device *dev);
extern void vlynq_device_disable(struct vlynq_device *dev);
extern u32 vlynq_local_id(struct vlynq_device *dev);
extern u32 vlynq_remote_id(struct vlynq_device *dev);
extern void vlynq_set_local_mapping(struct vlynq_device *dev,
				    u32 tx_offset,
				    struct vlynq_mapping *mapping);
extern void vlynq_set_remote_mapping(struct vlynq_device *dev, 
				     u32 tx_offset, 
				     struct vlynq_mapping *mapping);
extern int vlynq_virq_to_irq(struct vlynq_device *dev, int virq);
extern int vlynq_irq_to_virq(struct vlynq_device *dev, int irq);
extern int vlynq_set_local_irq(struct vlynq_device *dev, int virq);
extern int vlynq_set_remote_irq(struct vlynq_device *dev, int virq);

#endif
