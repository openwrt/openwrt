/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _NET_DSA_RTL83XX_H
#define _NET_DSA_RTL83XX_H

#include <net/dsa.h>
#include "rtl838x.h"


#define RTL8380_VERSION_A 'A'
#define RTL8390_VERSION_A 'A'
#define RTL8380_VERSION_B 'B'

struct fdb_update_work {
	struct work_struct work;
	struct net_device *ndev;
	u64 macs[];
};

#define MIB_DESC(_size, _offset, _name) {.size = _size, .offset = _offset, .name = _name}
struct rtl83xx_mib_desc {
	unsigned int size;
	unsigned int offset;
	const char *name;
};

void __init rtl83xx_storm_control_init(struct rtl838x_switch_priv *priv);

/* RTL838x-specific */
u32 rtl838x_hash(struct rtl838x_switch_priv *priv, u64 seed);
irqreturn_t rtl838x_switch_irq(int irq, void *dev_id);
void rtl8380_get_version(struct rtl838x_switch_priv *priv);
void rtl838x_vlan_profile_dump(int index);
int rtl83xx_dsa_phy_read(struct dsa_switch *ds, int phy_addr, int phy_reg);

/* RTL839x-specific */
u32 rtl839x_hash(struct rtl838x_switch_priv *priv, u64 seed);
irqreturn_t rtl839x_switch_irq(int irq, void *dev_id);
void rtl8390_get_version(struct rtl838x_switch_priv *priv);
void rtl839x_vlan_profile_dump(int index);
int rtl83xx_dsa_phy_write(struct dsa_switch *ds, int phy_addr, int phy_reg, u16 val);

#endif /* _NET_DSA_RTL83XX_H */

