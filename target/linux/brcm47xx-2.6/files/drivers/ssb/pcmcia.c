/*
 * Sonics Silicon Backplane
 * PCMCIA-Hostbus related functions
 *
 * Copyright 2006 Johannes Berg <johannes@sipsolutions.net>
 * Copyright 2007 Michael Buesch <mb@bu3sch.de>
 *
 * Licensed under the GNU/GPL. See COPYING for details.
 */

#include <linux/ssb/ssb.h>
#include <linux/delay.h>

#include <pcmcia/cs_types.h>
#include <pcmcia/cs.h>
#include <pcmcia/cistpl.h>
#include <pcmcia/ciscode.h>
#include <pcmcia/ds.h>
#include <pcmcia/cisreg.h>

#include "ssb_private.h"


int ssb_pcmcia_switch_coreidx(struct ssb_bus *bus,
			      u8 coreidx)
{
	struct pcmcia_device *pdev = bus->host_pcmcia;
	int err;
	int attempts = 0;
	u32 cur_core;
	conf_reg_t reg;
	u32 addr;
	u32 read_addr;

	addr = (coreidx * SSB_CORE_SIZE) + SSB_ENUM_BASE;
	while (1) {
		reg.Action = CS_WRITE;
		reg.Offset = 0x2E;
		reg.Value = (addr & 0x0000F000) >> 12;
		err = pcmcia_access_configuration_register(pdev, &reg);
		if (err != CS_SUCCESS)
			goto error;
		reg.Offset = 0x30;
		reg.Value = (addr & 0x00FF0000) >> 16;
		err = pcmcia_access_configuration_register(pdev, &reg);
		if (err != CS_SUCCESS)
			goto error;
		reg.Offset = 0x32;
		reg.Value = (addr & 0xFF000000) >> 24;
		err = pcmcia_access_configuration_register(pdev, &reg);
		if (err != CS_SUCCESS)
			goto error;

		read_addr = 0;

		reg.Action = CS_READ;
		reg.Offset = 0x2E;
		err = pcmcia_access_configuration_register(pdev, &reg);
		if (err != CS_SUCCESS)
			goto error;
		read_addr |= (reg.Value & 0xF) << 12;
		reg.Offset = 0x30;
		err = pcmcia_access_configuration_register(pdev, &reg);
		if (err != CS_SUCCESS)
			goto error;
		read_addr |= reg.Value << 16;
		reg.Offset = 0x32;
		err = pcmcia_access_configuration_register(pdev, &reg);
		if (err != CS_SUCCESS)
			goto error;
		read_addr |= reg.Value << 24;

		cur_core = (read_addr - SSB_ENUM_BASE) / SSB_CORE_SIZE;
		if (cur_core == coreidx)
			break;

		if (attempts++ > SSB_BAR0_MAX_RETRIES)
			goto error;
		udelay(10);
	}

	return 0;
error:
	ssb_printk(KERN_ERR PFX "Failed to switch to core %u\n", coreidx);
	return -ENODEV;
}

int ssb_pcmcia_switch_core(struct ssb_bus *bus,
			   struct ssb_device *dev)
{
	int err;
	unsigned long flags;

	ssb_dprintk(KERN_INFO PFX
		    "Switching to %s core, index %d\n",
		    ssb_core_name(dev->id.coreid),
		    dev->core_index);

	spin_lock_irqsave(&bus->bar_lock, flags);
	err = ssb_pcmcia_switch_coreidx(bus, dev->core_index);
	if (!err)
		bus->mapped_device = dev;
	spin_unlock_irqrestore(&bus->bar_lock, flags);

	return err;
}

int ssb_pcmcia_switch_segment(struct ssb_bus *bus, u8 seg)
{
	int attempts = 0;
	unsigned long flags;
	conf_reg_t reg;
	int res, err = 0;

	assert(seg == 0 || seg == 1);
	reg.Offset = 0x34;
	reg.Function = 0;
	spin_lock_irqsave(&bus->bar_lock, flags);
	while (1) {
		reg.Action = CS_WRITE;
		reg.Value = seg;
		res = pcmcia_access_configuration_register(bus->host_pcmcia, &reg);
		if (unlikely(res != CS_SUCCESS))
			goto error;
		reg.Value = 0xFF;
		reg.Action = CS_READ;
		res = pcmcia_access_configuration_register(bus->host_pcmcia, &reg);
		if (unlikely(res != CS_SUCCESS))
			goto error;

		if (reg.Value == seg)
			break;

		if (unlikely(attempts++ > SSB_BAR0_MAX_RETRIES))
			goto error;
		udelay(10);
	}
	bus->mapped_pcmcia_seg = seg;
out_unlock:
	spin_unlock_irqrestore(&bus->bar_lock, flags);
	return err;
error:
	ssb_printk(KERN_ERR PFX "Failed to switch pcmcia segment\n");
	err = -ENODEV;
	goto out_unlock;
}

static inline int do_select_core(struct ssb_bus *bus,
				 struct ssb_device *dev,
				 u16 *offset)
{
	int err;
	u8 need_seg = (*offset >= 0x800) ? 1 : 0;

	if (unlikely(dev != bus->mapped_device)) {
		err = ssb_pcmcia_switch_core(bus, dev);
		if (unlikely(err))
			return err;
	}
	if (unlikely(need_seg != bus->mapped_pcmcia_seg)) {
		err = ssb_pcmcia_switch_segment(bus, need_seg);
		if (unlikely(err))
			return err;
	}
	if (need_seg == 1)
		*offset -= 0x800;

	return 0;
}

static u16 ssb_pcmcia_read16(struct ssb_device *dev, u16 offset)
{
	struct ssb_bus *bus = dev->bus;
	u16 x;

	if (unlikely(do_select_core(bus, dev, &offset)))
		return 0xFFFF;
	x = readw(bus->mmio + offset);
//printk("R16 0x%04X, 0x%04X\n", offset, x);
	return x;
}

static u32 ssb_pcmcia_read32(struct ssb_device *dev, u16 offset)
{
	struct ssb_bus *bus = dev->bus;
	u32 x;

	if (unlikely(do_select_core(bus, dev, &offset)))
		return 0xFFFFFFFF;
	x = readl(bus->mmio + offset);
//printk("R32 0x%04X, 0x%08X\n", offset, x);
	return x;
}

static void ssb_pcmcia_write16(struct ssb_device *dev, u16 offset, u16 value)
{
	struct ssb_bus *bus = dev->bus;

	if (unlikely(do_select_core(bus, dev, &offset)))
		return;
//printk("W16 0x%04X, 0x%04X\n", offset, value);
	writew(value, bus->mmio + offset);
}

static void ssb_pcmcia_write32(struct ssb_device *dev, u16 offset, u32 value)
{
	struct ssb_bus *bus = dev->bus;

	if (unlikely(do_select_core(bus, dev, &offset)))
		return;
//printk("W32 0x%04X, 0x%08X\n", offset, value);
	readw(bus->mmio + offset);
	writew(value >> 16, bus->mmio + offset + 2);
	readw(bus->mmio + offset);
	writew(value, bus->mmio + offset);
}

const struct ssb_bus_ops ssb_pcmcia_ops = {
	.read16		= ssb_pcmcia_read16,
	.read32		= ssb_pcmcia_read32,
	.write16	= ssb_pcmcia_write16,
	.write32	= ssb_pcmcia_write32,
};

int ssb_pcmcia_init(struct ssb_bus *bus)
{
	conf_reg_t reg;
	int err;

	if (bus->bustype != SSB_BUSTYPE_PCMCIA)
		return 0;

	/* Switch segment to a known state and sync
	 * bus->mapped_pcmcia_seg with hardware state. */
	ssb_pcmcia_switch_segment(bus, 0);

	/* Init IRQ routing */
	reg.Action = CS_READ;
	reg.Function = 0;
	if (bus->chip_id == 0x4306)
		reg.Offset = 0x00;
	else
		reg.Offset = 0x80;
	err = pcmcia_access_configuration_register(bus->host_pcmcia, &reg);
	if (err != CS_SUCCESS)
		goto error;
	reg.Action = CS_WRITE;
	reg.Value |= 0x04 | 0x01;
	err = pcmcia_access_configuration_register(bus->host_pcmcia, &reg);
	if (err != CS_SUCCESS)
		goto error;

	return 0;
error:
	return -ENODEV;
}
