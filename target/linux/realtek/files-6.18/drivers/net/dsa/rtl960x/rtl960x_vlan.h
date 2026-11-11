/* SPDX-License-Identifier: GPL-2.0-only */
/* VLAN table and port VLAN configuration of the RTL960x switch core */

#ifndef __RTL960X_VLAN_H
#define __RTL960X_VLAN_H

#include <linux/types.h>
#include <net/dsa.h>

#include "rtl960x_dsa.h"

enum rtl960x_frame_ingress {
	RTL960X_FRAME_TYPE_ANY_FRAME = 0,
	RTL960X_FRAME_TYPE_TAGGED_ONLY,
	RTL960X_FRAME_TYPE_UNTAGGED_ONLY,
};

int rtl960x_vlan_port_get_pvid(struct rtl960x_dsa *priv, int port, u16 *pvid);
int rtl960x_vlan_port_set_pvid(struct rtl960x_dsa *priv, int port, u16 pvid);
int rtl960x_vlan_port_get_framefilter(struct rtl960x_dsa *priv, int port,
				      enum rtl960x_frame_ingress *frame_type);
int rtl960x_vlan_port_set_framefilter(struct rtl960x_dsa *priv, int port,
				      enum rtl960x_frame_ingress frame_type);
int rtl960x_vlan_4k_set(struct rtl960x_dsa *priv, u16 vid, u16 member,
			u16 untag);
int rtl960x_vlan_pvid_port_commit(struct dsa_switch *ds, int port, u16 vid,
				  bool vlan_filtering,
				  struct netlink_ext_ack *extack);

#endif /* __RTL960X_VLAN_H */
