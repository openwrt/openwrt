/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef __RTL960X_DSA_H
#define __RTL960X_DSA_H

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
 * VID 0 is the standalone VLAN shared by every user port outside a bridge
 * and the CPU port (the special VLAN 0 of the rtl83xx and rtl8365mb drivers,
 * ocelot's OCELOT_STANDALONE_PVID).
 */
#define RTL960X_STANDALONE_VID	0

struct rtl960x_dsa {
	struct dsa_switch *ds;
	struct device *dev;
	struct regmap *map;
	u16 pvid[RTL960X_NUM_PORTS];	/* shadow of each port's PVID */
};

#endif /* __RTL960X_DSA_H */
