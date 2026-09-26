/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef __RTL960X_DSA_H
#define __RTL960X_DSA_H

#include <linux/mutex.h>
#include <linux/regmap.h>
#include <net/dsa.h>

/*
 * RTL9607C family switch-core port map (11 ports, 0..10):
 *   0..3  internal GPHY ports
 *   4     reserved / unused
 *   5     SGMII or PON                          (WAN-capable)
 *   6     HSGMII, muxed with USB 3.0            (WAN-capable)
 *   7     2nd HSGMII, muxed with PCIE1 or GMAC2
 *   8     reserved / unused
 *   9,10  CPU ports (GMAC0 / GMAC1)
 */

#define RTL960X_NUM_PORTS	11
#define RTL960X_NUM_VLANS	4096
#define RTL960X_NUM_MSTI	4

/*
 * Driver-internal VIDs. VID 0 is the standalone VLAN shared by every user
 * port outside a bridge and the CPU port (the special VLAN 0 of the rtl83xx
 * and rtl8365mb drivers, ocelot's OCELOT_STANDALONE_PVID). One VID per
 * offloaded bridge is reserved at the top of the table, indexed by DSA's
 * 1-based bridge number, for the VLAN-unaware PVID (ocelot's per-bridge
 * unaware PVID). VID 4095 stays unused as 802.1Q reserves it.
 */
#define RTL960X_STANDALONE_VID	0
#define RTL960X_NUM_BRIDGE_VIDS	RTL960X_NUM_PORTS
#define RTL960X_BRIDGE_VID(num)	(RTL960X_NUM_VLANS - 1 - (num))
#define RTL960X_BRIDGE_VID_MIN	RTL960X_BRIDGE_VID(RTL960X_NUM_BRIDGE_VIDS)

struct rtl960x_dsa {
	struct dsa_switch *ds;
	struct device *dev;
	struct regmap *map;
	/*
	 * table_lock serialises the indirect table-access engine, which the
	 * VLAN and L2 tables share, and the L2 flush engine.
	 */
	struct mutex table_lock;
	/*
	 * l2_lock keeps a whole FDB dump walk atomic against the FDB add/del
	 * work items, which run from DSA's deferred workqueue.
	 */
	struct mutex l2_lock;
	u16 pvid[RTL960X_NUM_PORTS];	/* shadow of each port's PVID */
	u16 bridge_pvid[RTL960X_NUM_PORTS]; /* PVID set by the bridge, 0 if none */
	/* Bridges whose ports are programmed VLAN-unaware, by bridge.num - 1. */
	DECLARE_BITMAP(unaware, RTL960X_NUM_BRIDGE_VIDS);
	u8 vlan_owner[RTL960X_BRIDGE_VID_MIN]; /* bridge.num using the VID */
};

#endif /* __RTL960X_DSA_H */
