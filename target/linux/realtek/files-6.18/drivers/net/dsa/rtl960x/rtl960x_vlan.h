/* SPDX-License-Identifier: GPL-2.0-only */
/* VLAN table and port VLAN configuration of the RTL960x switch core */

#ifndef __RTL960X_VLAN_H
#define __RTL960X_VLAN_H

#include <linux/types.h>
#include <net/dsa.h>

#include "rtl960x_dsa.h"

int rtl960x_vlan_setup(struct dsa_switch *ds);
int rtl960x_vlan_port_bridge_join(struct dsa_switch *ds, int port,
				  struct dsa_bridge bridge);
void rtl960x_vlan_port_bridge_leave(struct dsa_switch *ds, int port,
				    struct dsa_bridge bridge);

int rtl960x_port_vlan_filtering(struct dsa_switch *ds, int port,
				bool vlan_filtering,
				struct netlink_ext_ack *extack);
int rtl960x_port_vlan_add(struct dsa_switch *ds, int port,
			  const struct switchdev_obj_port_vlan *vlan,
			  struct netlink_ext_ack *extack);
int rtl960x_port_vlan_del(struct dsa_switch *ds, int port,
			  const struct switchdev_obj_port_vlan *vlan);

#endif /* __RTL960X_VLAN_H */
