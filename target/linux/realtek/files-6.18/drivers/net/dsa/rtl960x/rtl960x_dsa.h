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
#define RTL960X_NUM_MSTI	4
/* RTL960X supports 4k vlans (vid<=4095) */
#define RTL960X_MAX_4K_VID			0x0fff /* 4095 */

struct rtl960x_dsa {
	struct dsa_switch *ds;
	struct device *dev;
	struct regmap *map;
	/* vlan_lock protects against concurrent Read-Modify-Write operations
	 * on the global VLAN 4K tables, such as when adding or deleting port
	 * PVID configurations.
	 */
	struct mutex		vlan_lock;
	/*
	 * l2_lock keeps a whole FDB dump walk atomic against the FDB add/del
	 * work items, which run from DSA's deferred workqueue, and serialises
	 * the L2 flush engine.
	 */
	struct mutex l2_lock;
	/* PVID requested by the bridge, 0 if none. Only programmed
     * while the port filters VLANs. Protected by vlan_lock.
	 */
	u16 bridge_pvid[RTL960X_NUM_PORTS];
	u16 mbr_mask[RTL960X_MAX_4K_VID];
	u16 untag_mask[RTL960X_MAX_4K_VID];
	/* User ports that do not filter VLANs. Protected by vlan_lock. */
	u16 transparent_mask;
};

#endif /* __RTL960X_DSA_H */
