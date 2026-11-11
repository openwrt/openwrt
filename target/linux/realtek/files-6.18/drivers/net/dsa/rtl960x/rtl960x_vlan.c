// SPDX-License-Identifier: GPL-2.0-only
/* VLAN configuration interface for the rtl960x switch family */

#include <linux/bitfield.h>
#include <linux/if_bridge.h>
#include <linux/if_vlan.h>
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

/* Port-based VID register */
#define RTL960X_VLAN_PB_VID_REG(_p)		(0x1300c + ((_p) / 2) * 4)
#define   RTL960X_VLAN_PB_VID_SHIFT(_p)		(((_p) % 2) * 12)
#define   RTL960X_VLAN_PB_VID_MSK(_p)		(GENMASK(11, 0) << \
						 RTL960X_VLAN_PB_VID_SHIFT(_p))
/* Frame type filtering registers */
#define RTL960X_VLAN_ACCEPT_FRAME_TYPE_REG	0x13000
#define   RTL960X_VLAN_ACCEPT_FRAME_TYPE_OFFSET(_p)	(_p << 1)
#define   RTL960X_VLAN_ACCEPT_FRAME_TYPE_MASK(_p) \
		(0x3 << RTL960X_VLAN_ACCEPT_FRAME_TYPE_OFFSET(_p))

/**
 * struct rtl960x_vlan4k - VLAN table entry
 * @vid: VLAN ID, the table index
 * @member: port mask of ports in this VLAN
 * @untag: port mask of ports that strip the tag on egress
 * @fid: filtering database for SVL; the same field selects the MSTI
 * @ivl: independent VLAN learning (key by CVID) instead of shared (key by FID)
 * @svlan_chk: force the SVLAN decision to honour @ivl (unused)
 * @ext_maskidx: extension port member table index (unused)
 *
 * This structure is used to get/set entries in the VLAN4k table. The
 * VLAN4k table dictates the VLAN configuration for the switch for the
 * vast majority of features.
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

/* rtl960x_vlan_4k_read was here */

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

/**
 * rtl960x_vlan_4k_set() - Write the VLAN4k entry of a VID
 * @priv: switch private structure
 * @vid: VLAN ID
 * @member: member ports
 * @untag: ports that send the VLAN untagged
 *
 * Every entry uses Independent VLAN Learning.
 *
 * Context: Can sleep. Must be called with &priv->vlan_lock held.
 * Return: 0 on success, or a negative error code on failure.
 */
int rtl960x_vlan_4k_set(struct rtl960x_dsa *priv, u16 vid, u16 member,
			u16 untag)
{
	struct rtl960x_vlan4k vlan4k = {
		.vid = vid,
		.member = member,
		.untag = untag,
		.ivl = true,
	};

	lockdep_assert_held(&priv->vlan_lock);

	return rtl960x_vlan_4k_write(&vlan4k);
}

/**
 * rtl960x_vlan_port_get_pvid - Retrieve the port PVID
 * @priv: realtek switch private structure
 * @port: port index
 * @pvid: pointer to store the retrieved VLAN ID
 *
 * Returns the port PVID if defined or 0 if not.
 *
 * Context: Can sleep.
 * Return: 0 on success or a negative error code on failure.
 */
int rtl960x_vlan_port_get_pvid(struct rtl960x_dsa *priv, int port, u16 *pvid)
{
	u32 data;
	int ret;

	ret = regmap_read(priv->map, RTL960X_VLAN_PB_VID_REG(port), &data);
	if (ret)
		return ret;

	*pvid = (data & RTL960X_VLAN_PB_VID_MSK(port)) >> RTL960X_VLAN_PB_VID_SHIFT(port);

	return 0;
}

int rtl960x_vlan_port_set_pvid(struct rtl960x_dsa *priv, int port, u16 vid)
{
	return regmap_update_bits(priv->map, RTL960X_VLAN_PB_VID_REG(port),
				  RTL960X_VLAN_PB_VID_MSK(port),
				  field_prep(RTL960X_VLAN_PB_VID_MSK(port), vid));
}

/**
 * rtl960x_vlan_port_get_framefilter() - Get the ingress frame filtering mode
 * for a port
 * @priv: realtek switch private structure
 * @port: port index
 * @frame_type: pointer to store the retrieved ingress frame filter type
 *
 * Context: Can sleep.
 * Return: 0 on success, or a negative error code on failure.
 */
int rtl960x_vlan_port_get_framefilter(struct rtl960x_dsa *priv, int port,
				      enum rtl960x_frame_ingress *frame_type)
{
	u32 val;
	int ret;

	/* Even if ACCEPT_FRAME_TYPE_ANY, the switch will still check if the
	 * port is a member of vlan PVID
	 */
	ret = regmap_read(priv->map, RTL960X_VLAN_ACCEPT_FRAME_TYPE_REG, &val);
	if (ret)
		return ret;

	*frame_type = field_get(RTL960X_VLAN_ACCEPT_FRAME_TYPE_MASK(port), val);

	return 0;
}

/**
 * rtl960x_vlan_port_set_framefilter() - Set the ingress frame filtering mode
 * for a port
 * @priv: realtek switch private structure
 * @port: port index
 * @frame_type: the ingress frame filter type to configure
 *
 * Context: Can sleep.
 * Return: 0 on success, or a negative error code on failure.
 */
int rtl960x_vlan_port_set_framefilter(struct rtl960x_dsa *priv, int port,
				      enum rtl960x_frame_ingress frame_type)
{
	u32 val;

	/* Even if ACCEPT_FRAME_TYPE_ANY, the switch will still check if the
	 * port is a member of vlan PVID
	 */
	val = frame_type << RTL960X_VLAN_ACCEPT_FRAME_TYPE_OFFSET(port);

	return regmap_update_bits(priv->map, RTL960X_VLAN_ACCEPT_FRAME_TYPE_REG,
				  RTL960X_VLAN_ACCEPT_FRAME_TYPE_MASK(port), val);
}

/**
 * rtl960x_vlan_pvid_port_commit() - Configure a port's PVID as a raw
 * VID written to its dedicated register, for chip families without a
 * working VLAN MC table
 *
 * @ds: dsa switch instance
 * @port: port index
 * @vid: VID to use as PVID, or 0 for the neutral PVID
 * @vlan_filtering: whether the port filters VLANs
 * @extack: netlink extended ACK for error reporting
 *
 * Reads back the previous PVID and frame filter first so both can be
 * restored if enabling the new PVID fails partway through, matching
 * the rollback behavior of the family-C implementation below.
 *
 * Context: Can sleep. Must be called with &priv->vlan_lock held.
 * Return: 0 on success, or a negative error code on failure.
 */
int rtl960x_vlan_pvid_port_commit(struct dsa_switch *ds, int port, u16 vid,
				  bool vlan_filtering,
				  struct netlink_ext_ack *extack)
{
	enum rtl960x_frame_ingress accepted_frame, prev_accepted_frame;
	struct rtl960x_dsa *priv = ds->priv;
	bool vid_changed;
	u16 prev_pvid;
	int ret;

	lockdep_assert_held(&priv->vlan_lock);

	ret = rtl960x_vlan_port_get_pvid(priv, port, &prev_pvid);
	if (ret)
		return ret;

	ret = rtl960x_vlan_port_get_framefilter(priv, port,
						&prev_accepted_frame);
	if (ret) {
		dev_err(priv->dev, "Failed to get current framefilter\n");
		return ret;
	}

	vid_changed = vid != prev_pvid;

	if (vid_changed) {
		ret = rtl960x_vlan_port_set_pvid(priv, port, vid);
		if (ret) {
			dev_err(priv->dev, "Failed to set port PVID\n");
			return ret;
		}
	}

	/* Changing accept frame is what enables PVID (if not enabled before) */
	if (vlan_filtering && !vid)
		accepted_frame = RTL960X_FRAME_TYPE_TAGGED_ONLY;
	else
		accepted_frame = RTL960X_FRAME_TYPE_ANY_FRAME;

	if (accepted_frame != prev_accepted_frame) {
		ret = rtl960x_vlan_port_set_framefilter(priv, port,
							accepted_frame);
		if (ret) {
			dev_err(priv->dev, "Failed to set port frame filter\n");
			goto undo_vlan_port_set_pvid;
		}
	}

	return 0;

undo_vlan_port_set_pvid:
	if (vid_changed)
		(void)rtl960x_vlan_port_set_pvid(priv, port, prev_pvid);
	return ret;
}
