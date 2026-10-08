// SPDX-License-Identifier: GPL-2.0-only
/*
 * VLAN offload for the RTL960x switch core.
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
#define   RTL960X_VLAN_EGR_TAG_MODE_ORI		0
#define   RTL960X_VLAN_EGR_TAG_MODE_KEEP	1	/* keep the ingress format */

/*
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

static int rtl960x_vlan_4k_write(struct rtl960x_dsa *priv,
				 const struct rtl960x_vlan4k *vlan4k)
{
	u32 data[RTL960X_VLAN_ENTRY_SIZE];

	/* Pack table entry */
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
				 vid << RTL960X_VLAN_PB_VID_SHIFT(port));
	if (ret)
		return ret;

	priv->pvid[port] = vid;

	return 0;
}

/**
 * rtl960x_vlan_setup() - bring up the VLAN engine with every port standalone
 * @ds: DSA switch instance
 *
 * All user ports and the CPU port become untagged members of VLAN 0 with
 * VID 0 as PVID and ingress filtering off, so until DSA programs real VLANs
 * the switch forwards exactly as in the VLAN-unaware case (forwarding is
 * still gated by the PISO isolation masks).
 *
 * Context: Can sleep.
 * Return: 0 on success, or a negative error code on failure.
 */
int rtl960x_vlan_setup(struct dsa_switch *ds)
{
	struct rtl960x_dsa *priv = ds->priv;
	struct rtl960x_vlan4k vlan = {};
	u32 all = 0;
	int p, ret, v;

	for (p = 0; p < ds->num_ports; p++)
		if (dsa_is_user_port(ds, p) || dsa_is_cpu_port(ds, p))
			all |= BIT(p);

	/*
	 * Unconfigured VLAN entries power on with every port as a member (flat
	 * forwarding). Clear them so a VID only reaches ports DSA explicitly
	 * adds; otherwise port_vlan_add's read-modify-write would keep the
	 * all-ones default and never isolate VLANs.
	 */
	for (v = 1; v < RTL960X_NUM_VLANS; v++) {
		vlan.vid = v;
		ret = rtl960x_vlan_4k_write(priv, &vlan);
		if (ret)
			return ret;
	}

	vlan.vid = RTL960X_STANDALONE_VID;
	vlan.member = all;
	vlan.untag = all;
	ret = rtl960x_vlan_4k_write(priv, &vlan);
	if (ret)
		return ret;

	/*
	 * No ingress filtering yet. Egress on user ports follows the per-VID
	 * untag mask. Reset VID_0_TYPE=0 treats priority-tagged ingress as
	 * untagged and classifies it with the port PVID.
	 */
	ret = regmap_write(priv->map, RTL960X_VLAN_INGRESS_REG, 0);
	if (ret)
		return ret;
	/*
	 * The CPU port keeps each frame's ingress tag format, so CPU-bound
	 * tagging ignores the VLAN untag mask. VLAN-unaware frames stay
	 * untagged and VLAN-aware frames stay tagged, including when a VID has
	 * mixed tagging, so the bridge does not misclassify them using its PVID.
	 * This per-egress-port mode does not affect CPU-to-user traffic.
	 */
	for (p = 0; p < ds->num_ports; p++) {
		if (!dsa_is_user_port(ds, p) && !dsa_is_cpu_port(ds, p))
			continue;
		ret = regmap_write(priv->map, RTL960X_VLAN_EGR_TAG_REG(p),
				   dsa_is_cpu_port(ds, p) ?
				   RTL960X_VLAN_EGR_TAG_MODE_KEEP :
				   RTL960X_VLAN_EGR_TAG_MODE_ORI);
		if (ret)
			return ret;
		ret = rtl960x_vlan_set_pvid(priv, p, RTL960X_STANDALONE_VID);
		if (ret)
			return ret;
	}

	return regmap_set_bits(priv->map, RTL960X_VLAN_CTRL_REG,
			       RTL960X_VLAN_CTRL_FILTER_EN_MSK);
}
