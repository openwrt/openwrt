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

struct rtl960x_dsa;

/**
 * struct rtl960x_port - private per-port data
 * @priv: pointer to parent rtl960x_dsa data
 * @index: DSA port index, same as dsa_port::index
 * @stats: link statistics populated by rtl960x_stats_poll, ready for atomic
 *         access via rtl960x_get_stats64
 * @stats_lock: protect the stats structure during read/update
 * @mib_work: delayed work for polling MIB counters
 */
struct rtl960x_port {
	struct rtl960x_dsa *priv;
	unsigned int index;
	struct rtnl_link_stats64 stats;
	spinlock_t stats_lock;
	struct delayed_work mib_work;
};

struct rtl960x_dsa {
	struct dsa_switch *ds;
	struct device *dev;
	struct regmap *map;
	/*
	 * l2_lock keeps a whole FDB dump walk atomic against the FDB add/del
	 * work items, which run from DSA's deferred workqueue, and serialises
	 * the L2 flush engine.
	 */
	struct mutex l2_lock;
	u16 pvid[RTL960X_NUM_PORTS];	/* shadow of each port's PVID */
	u16 bridge_pvid[RTL960X_NUM_PORTS]; /* PVID set by the bridge, 0 if none */
	u8 vlan_owner[RTL960X_BRIDGE_VID_MIN]; /* bridge.num using the VID */
	/*
	 * Membership and untagged egress DSA configured per VID. The table
	 * entry also carries the transparent ports: user ports that are not
	 * VLAN-filtering, which are members of every VID and keep the ingress
	 * tag format on egress. A transparent bridge port is also a member of
	 * its bridge's reserved VID and uses it as PVID.
	 */
	u16 vlan_member[RTL960X_BRIDGE_VID_MIN];
	u16 vlan_untag[RTL960X_BRIDGE_VID_MIN];
	u16 transparent;
	struct mutex mib_lock;
	struct rtl960x_port ports[RTL960X_NUM_PORTS];
};

#endif /* __RTL960X_DSA_H */
