/*
 * Realtek RTL8366 SMI interface driver defines
 *
 * Copyright (C) 2009-2010 Gabor Juhos <juhosg@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#ifndef _RTL8366_SMI_H
#define _RTL8366_SMI_H

#include <linux/phy.h>

struct rtl8366_smi_ops;
struct mii_bus;

struct rtl8366_smi {
	struct device		*parent;
	unsigned int		gpio_sda;
	unsigned int		gpio_sck;
	spinlock_t		lock;
	struct mii_bus		*mii_bus;
	int			mii_irq[PHY_MAX_ADDR];

	struct rtl8366_smi_ops	*ops;
};

struct rtl8366_smi_ops {
	int	(*detect)(struct rtl8366_smi *smi);

	int	(*mii_read)(struct mii_bus *bus, int addr, int reg);
	int	(*mii_write)(struct mii_bus *bus, int addr, int reg, u16 val);
};

struct rtl8366_vlan_mc {
	u16	vid;
	u8	priority;
	u8	untag;
	u8	member;
	u8	fid;
};

struct rtl8366_vlan_4k {
	u16	vid;
	u8	untag;
	u8	member;
	u8	fid;
};

int rtl8366_smi_init(struct rtl8366_smi *smi);
void rtl8366_smi_cleanup(struct rtl8366_smi *smi);
int rtl8366_smi_write_reg(struct rtl8366_smi *smi, u32 addr, u32 data);
int rtl8366_smi_read_reg(struct rtl8366_smi *smi, u32 addr, u32 *data);

#endif /*  _RTL8366_SMI_H */
