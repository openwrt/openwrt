// SPDX-License-Identifier: GPL-2.0-only
/*
 * VLAN offload for the RTL960x switch core.
 *
 * The chip has no per-port "ignore the tag" mode: the VLAN engine is global,
 * every frame is classified by its VID and egress is gated by that VID's
 * member mask. A user port that is not VLAN-filtering is therefore made a
 * member of VIDs 1-4094 and keeps the ingress tag format on egress
 * ("transparent"), so tagged frames pass through it unmodified; the address
 * lookup and the isolation masks decide where they go.
 *
 * VLAN 0 is the standalone VLAN: every user port outside a bridge is an
 * untagged member together with the CPU port and uses VID 0 as PVID, so
 * untagged standalone traffic is exchanged with the CPU (the isolation masks
 * keep standalone ports apart from each other). The CPU port also uses VID 0
 * as its PVID.
 */

#include <linux/bitfield.h>
#include <linux/regmap.h>
#include <linux/soc/realtek/otto_table.h>

#include "rtl960x_vlan.h"

#define RTL960X_VLAN_ENTRY_SIZE			1	/* 32-bit words */

/* VLAN table entry, one 32-bit word */
#define RTL960X_VLAN_ENTRY_MBR_MSK		GENMASK(10, 0)
#define RTL960X_VLAN_ENTRY_UNTAG_MSK		GENMASK(21, 11)
#define RTL960X_VLAN_ENTRY_FID_MSTI_MSK		GENMASK(23, 22)
/* Force the SVLAN decision to honour the IVL/SVL setting */
#define RTL960X_VLAN_ENTRY_SVLAN_CHK_MSK	BIT(24)
#define RTL960X_VLAN_ENTRY_IVL_MSK		BIT(25)
/* Index into the 32-entry extension port member table (MAC7/9/10) */
#define RTL960X_VLAN_ENTRY_EXT_MASKIDX_MSK	GENMASK(30, 26)

/* Per-port ingress VLAN filtering, one bit per port */
#define RTL960X_VLAN_INGRESS_REG		0x13004
/* Global VLAN engine enable */
#define RTL960X_VLAN_CTRL_REG			0x13008
#define   RTL960X_VLAN_CTRL_FILTER_EN_MSK	BIT(0)
/* Port-based VID, two 12-bit fields per register */
#define RTL960X_VLAN_PB_VID_REG(_p)		(0x1300c + ((_p) / 2) * 4)
#define   RTL960X_VLAN_PB_VID_SHIFT(_p)		(((_p) % 2) * 12)
#define   RTL960X_VLAN_PB_VID_MSK(_p)		(GENMASK(11, 0) << \
						 RTL960X_VLAN_PB_VID_SHIFT(_p))
/* Per-port egress tag handling */
#define RTL960X_VLAN_EGR_TAG_REG(_p)		(0x2a000 + (_p) * 4)
#define   RTL960X_VLAN_EGR_TAG_MODE_ORI		0	/* follow the untag mask */
#define   RTL960X_VLAN_EGR_TAG_MODE_KEEP	1	/* keep the ingress format */

/**
 * struct rtl960x_vlan4k - VLAN table entry
 * @vid: VLAN ID, the table index
 * @member: port mask of ports in this VLAN
 * @untag: port mask of ports that strip the tag on egress
 * @fid: filtering database for SVL; the same field selects the MSTI
 * @ivl: independent VLAN learning (key by CVID) instead of shared (key by FID)
 * @svlan_chk: force the SVLAN decision to honour @ivl (unused)
 * @ext_maskidx: extension port member table index (unused)
 */
struct rtl960x_vlan4k {
	u16 vid;
	u16 member;
	u16 untag;
	u8 fid;
	bool ivl;
	bool svlan_chk;
	u8 ext_maskidx;
};

static int rtl960x_vlan_4k_write(const struct rtl960x_vlan4k *vlan4k)
{
	u32 data[RTL960X_VLAN_ENTRY_SIZE];

	data[0] = FIELD_PREP(RTL960X_VLAN_ENTRY_MBR_MSK, vlan4k->member) |
		  FIELD_PREP(RTL960X_VLAN_ENTRY_UNTAG_MSK, vlan4k->untag) |
		  FIELD_PREP(RTL960X_VLAN_ENTRY_FID_MSTI_MSK, vlan4k->fid) |
		  FIELD_PREP(RTL960X_VLAN_ENTRY_SVLAN_CHK_MSK, vlan4k->svlan_chk) |
		  FIELD_PREP(RTL960X_VLAN_ENTRY_IVL_MSK, vlan4k->ivl) |
		  FIELD_PREP(RTL960X_VLAN_ENTRY_EXT_MASKIDX_MSK,
			     vlan4k->ext_maskidx);

	return otto_table_write(RTL9607C_TBL_VLAN, vlan4k->vid, &data);
}

static int rtl960x_vlan_set_pvid(struct rtl960x_dsa *priv, int port, u16 vid)
{
	int ret;

	ret = regmap_update_bits(priv->map, RTL960X_VLAN_PB_VID_REG(port),
				 RTL960X_VLAN_PB_VID_MSK(port),
				 field_prep(RTL960X_VLAN_PB_VID_MSK(port), vid));
	if (ret)
		return ret;

	priv->pvid[port] = vid;

	return 0;
}

/*
 * Write the table entry of a regular VID (1 .. RTL960X_NUM_VLANS - 2): the
 * transparent user ports plus the CPU ports, learning per VID (IVL).
 */
static int rtl960x_vlan_4k_sync(struct rtl960x_dsa *priv, u16 vid)
{
	struct rtl960x_vlan4k vlan = { .vid = vid };

	if (priv->transparent) {
		vlan.member = priv->transparent | dsa_cpu_ports(priv->ds);
		vlan.ivl = true;
	}

	return rtl960x_vlan_4k_write(&vlan);
}

static int rtl960x_vlan_4k_sync_all(struct rtl960x_dsa *priv)
{
	int ret, vid;

	for (vid = 1; vid < RTL960X_NUM_VLANS - 1; vid++) {
		ret = rtl960x_vlan_4k_sync(priv, vid);
		if (ret)
			return ret;
	}

	return 0;
}

static int rtl960x_vlan_set_egress_mode(struct rtl960x_dsa *priv, int port,
					bool keep)
{
	return regmap_write(priv->map, RTL960X_VLAN_EGR_TAG_REG(port),
			    keep ? RTL960X_VLAN_EGR_TAG_MODE_KEEP :
				   RTL960X_VLAN_EGR_TAG_MODE_ORI);
}

/**
 * rtl960x_vlan_setup() - bring up the VLAN engine with every port standalone
 * @ds: DSA switch instance
 *
 * All user ports and the CPU port become untagged members of VLAN 0 with
 * VID 0 as PVID and ingress filtering off. Every user port is transparent:
 * a member of VIDs 1-4094, keeping the ingress tag format on egress, so
 * frames forwarded to the CPU retain their ingress tag format and
 * forwarding is decided by the address lookup and the isolation masks. VID
 * 4095 is reserved by 802.1Q and stays empty.
 *
 * Context: Can sleep.
 * Return: 0 on success, or a negative error code on failure.
 */
int rtl960x_vlan_setup(struct dsa_switch *ds)
{
	struct rtl960x_dsa *priv = ds->priv;
	struct rtl960x_vlan4k vlan = {};
	struct dsa_port *dp;
	u16 user, all;
	int ret;

	user = dsa_user_ports(ds);
	all = user | dsa_cpu_ports(ds);

	vlan.vid = RTL960X_STANDALONE_VID;
	vlan.member = all;
	vlan.untag = all;
	ret = rtl960x_vlan_4k_write(&vlan);
	if (ret)
		return ret;

	/*
	 * Unconfigured VLAN entries power on with every port as a member, FID
	 * 0 and shared learning. Rewrite them as IVL with the user ports as
	 * transparent members, so tagged traffic learns per VID instead of in
	 * the standalone database.
	 */
	priv->transparent = user;
	ret = rtl960x_vlan_4k_sync_all(priv);
	if (ret)
		return ret;

	vlan = (struct rtl960x_vlan4k){ .vid = RTL960X_NUM_VLANS - 1 };
	ret = rtl960x_vlan_4k_write(&vlan);
	if (ret)
		return ret;

	/*
	 * No ingress filtering yet. Reset VID_0_TYPE=0 treats priority-tagged
	 * ingress as untagged and classifies it with the port PVID.
	 */
	ret = regmap_write(priv->map, RTL960X_VLAN_INGRESS_REG, 0);
	if (ret)
		return ret;
	/*
	 * Every port keeps each frame's ingress tag format on egress. For the
	 * CPU port this means CPU-bound tagging ignores the VLAN untag mask:
	 * VLAN-unaware frames stay untagged and VLAN-aware frames stay tagged,
	 * including when a VID has mixed tagging, so the bridge does not
	 * misclassify them using its PVID.
	 */
	dsa_switch_for_each_available_port(dp, ds) {
		ret = rtl960x_vlan_set_egress_mode(priv, dp->index, true);
		if (ret)
			return ret;
		ret = rtl960x_vlan_set_pvid(priv, dp->index,
					    RTL960X_STANDALONE_VID);
		if (ret)
			return ret;
	}

	return regmap_set_bits(priv->map, RTL960X_VLAN_CTRL_REG,
			       RTL960X_VLAN_CTRL_FILTER_EN_MSK);
}
