// SPDX-License-Identifier: GPL-2.0-or-later OR MIT

#include <linux/bitmap.h>
#include <linux/if_bridge.h>

#include "qca_ppe.h"

static int ppe_xlt_idx_alloc(struct qca_ppe_priv *priv)
{
	int idx;

	idx = find_first_zero_bit(priv->xlt_bitmap, PPE_XLT_TBL_NUM);
	if (idx >= PPE_XLT_TBL_NUM)
		return -ENOSPC;

	set_bit(idx, priv->xlt_bitmap);
	return idx;
}

static void ppe_xlt_rule_set(struct qca_ppe_priv *priv, int idx,
			     u8 port_bmp, u16 vid, bool untagged)
{
	u32 w0, w1;

	w0 = PPE_XLT_VALID | FIELD_PREP(PPE_XLT_PORT_BMP, port_bmp);
	w1 = 0;

	if (untagged) {
		w0 |= PPE_XLT_CKEY_FMT_0;
	} else {
		w1 |= FIELD_PREP(PPE_XLT_CKEY_FMT_1,
				  PPE_XLT_CKEY_TAGGED >> 1);
		w1 |= PPE_XLT_CKEY_VID_INCL;
		w1 |= FIELD_PREP(PPE_XLT_CKEY_VID, vid);
	}

	regmap_write(priv->regmap, PPE_XLT_RULE_TBL(idx), w0);
	regmap_write(priv->regmap, PPE_XLT_RULE_W1(idx), w1);
	regmap_write(priv->regmap, PPE_XLT_RULE_TBL(idx) + 8, 0);
}

static void ppe_xlt_action_set(struct qca_ppe_priv *priv, int idx,
			       u32 vsi, bool strip_ctag)
{
	u32 w0 = 0, w1;

	if (strip_ctag)
		w0 = FIELD_PREP(PPE_XLT_CVID_CMD, PPE_XLT_CVID_DEL);

	w1 = PPE_XLT_VSI_CMD | FIELD_PREP(PPE_XLT_VSI, vsi);

	regmap_write(priv->regmap, PPE_XLT_ACTION_TBL(idx), w0);
	regmap_write(priv->regmap, PPE_XLT_ACTION_W1(idx), w1);
}

static void ppe_xlt_clear(struct qca_ppe_priv *priv, int idx)
{
	regmap_write(priv->regmap, PPE_XLT_RULE_TBL(idx), 0);
	regmap_write(priv->regmap, PPE_XLT_RULE_W1(idx), 0);
	regmap_write(priv->regmap, PPE_XLT_RULE_TBL(idx) + 8, 0);
	regmap_write(priv->regmap, PPE_XLT_ACTION_TBL(idx), 0);
	regmap_write(priv->regmap, PPE_XLT_ACTION_W1(idx), 0);
}

static void ppe_xlt_idx_free(struct qca_ppe_priv *priv, int *idx)
{
	ppe_xlt_clear(priv, *idx);
	clear_bit(*idx, priv->xlt_bitmap);
	*idx = -1;
}

static void ppe_eg_vsi_tag_port_set(struct qca_ppe_priv *priv,
				    u32 vsi, int port, u32 mode)
{
	regmap_update_bits(priv->regmap, PPE_EG_VSI_TAG(vsi),
			   0x3 << (port * 2), (mode & 0x3) << (port * 2));
}

static void ppe_port_def_cvid_set(struct qca_ppe_priv *priv,
				  int port, u16 vid, bool enable)
{
	regmap_update_bits(priv->regmap, PPE_PORT_DEF_VID(port),
			   PPE_PORT_DEF_CVID | PPE_PORT_DEF_CVID_EN,
			   enable ? FIELD_PREP(PPE_PORT_DEF_CVID, vid) |
			   PPE_PORT_DEF_CVID_EN : 0);
}

static struct qca_ppe_vlan_entry *
ppe_vlan_find(struct qca_ppe_priv *priv, struct net_device *br_dev,
	      u16 vid)
{
	int i;

	for (i = 0; i < PPE_VSI_MAX; i++)
		if (priv->vlans[i].br_dev == br_dev &&
		    priv->vlans[i].vid == vid)
			return &priv->vlans[i];

	return NULL;
}

static struct qca_ppe_vlan_entry *
ppe_vlan_alloc(struct qca_ppe_priv *priv, struct net_device *br_dev,
	       u16 vid)
{
	struct qca_ppe_vlan_entry *entry;
	int vsi, i;

	vsi = ppe_vsi_alloc(priv);
	if (vsi < 0)
		return NULL;

	for (i = 0; i < PPE_VSI_MAX; i++) {
		if (priv->vlans[i].br_dev)
			continue;

		entry = &priv->vlans[i];
		entry->br_dev = br_dev;
		entry->vid = vid;
		entry->vsi = vsi;
		entry->ports = 0;
		entry->pvid_ports = 0;
		entry->xlt_idx = -1;
		entry->xlt_pvid_idx = -1;
		return entry;
	}

	ppe_vsi_free(priv, vsi);
	return NULL;
}

static void ppe_vlan_free(struct qca_ppe_priv *priv,
			  struct qca_ppe_vlan_entry *entry)
{
	if (entry->xlt_idx >= 0)
		ppe_xlt_idx_free(priv, &entry->xlt_idx);
	if (entry->xlt_pvid_idx >= 0)
		ppe_xlt_idx_free(priv, &entry->xlt_pvid_idx);
	ppe_vsi_free(priv, entry->vsi);
	entry->br_dev = NULL;
}

static void ppe_vlan_members_update(struct qca_ppe_priv *priv,
				    struct qca_ppe_vlan_entry *entry)
{
	ppe_vsi_member_set(priv, entry->vsi,
			   entry->ports | BIT(QCA_PPE_CPU_PORT));
}

static void ppe_vlan_pvid_update(struct qca_ppe_priv *priv,
				 struct qca_ppe_vlan_entry *entry)
{
	if (!entry->pvid_ports && entry->xlt_pvid_idx >= 0) {
		ppe_xlt_idx_free(priv, &entry->xlt_pvid_idx);
		return;
	}

	if (entry->pvid_ports)
		ppe_xlt_rule_set(priv, entry->xlt_pvid_idx,
				 entry->pvid_ports, 0, true);
}

int qca_ppe_vlan_setup(struct dsa_switch *ds)
{
	struct qca_ppe_priv *priv = ds_to_priv(ds);
	int i;

	for (i = 0; i < ds->num_ports; i++) {
		u32 mode = dsa_is_user_port(ds, i) ?
			   PPE_EG_UNMODIFIED : PPE_EG_UNTOUCHED;

		regmap_update_bits(priv->regmap, PPE_PORT_EG_VLAN(i),
				   PPE_PORT_EG_VLAN_CTAG_MODE |
				   PPE_PORT_EG_VLAN_STAG_MODE,
				   FIELD_PREP(PPE_PORT_EG_VLAN_CTAG_MODE, mode) |
				   FIELD_PREP(PPE_PORT_EG_VLAN_STAG_MODE, mode));
	}

	for (i = 0; i < PPE_VSI_MAX; i++) {
		regmap_write(priv->regmap, PPE_EG_VSI_TAG(i),
			     PPE_EG_VSI_TAG_UNMODIFIED);
		priv->vlans[i].xlt_idx = -1;
		priv->vlans[i].xlt_pvid_idx = -1;
	}

	regmap_update_bits(priv->regmap, PPE_EG_BRIDGE_CONFIG,
			   PPE_EG_L2_EDIT_EN, PPE_EG_L2_EDIT_EN);

	ds->configure_vlan_while_not_filtering = false;

	return 0;
}

int qca_ppe_port_vlan_filtering(struct dsa_switch *ds, int port,
				bool vlan_filtering,
				struct netlink_ext_ack *extack)
{
	struct qca_ppe_priv *priv = ds_to_priv(ds);

	regmap_update_bits(priv->regmap, PPE_PORT_EG_VLAN(port),
			   PPE_PORT_EG_VSI_TAG_EN,
			   vlan_filtering ? PPE_PORT_EG_VSI_TAG_EN : 0);

	regmap_update_bits(priv->regmap, PPE_PORT_VLAN_CFG(port),
			   PPE_VLAN_XLT_MISS_FWD,
			   vlan_filtering ?
			   FIELD_PREP(PPE_VLAN_XLT_MISS_FWD, PPE_XLT_MISS_FWD_DROP) : 0);

	return 0;
}

int qca_ppe_port_vlan_add(struct dsa_switch *ds, int port,
			  const struct switchdev_obj_port_vlan *vlan,
			  struct netlink_ext_ack *extack)
{
	struct qca_ppe_priv *priv = ds_to_priv(ds);
	struct net_device *br_dev = priv->port_br_dev[port];
	bool untagged = vlan->flags & BRIDGE_VLAN_INFO_UNTAGGED;
	bool pvid = vlan->flags & BRIDGE_VLAN_INFO_PVID;
	struct qca_ppe_vlan_entry *entry;
	u16 vid = vlan->vid;
	int idx;

	if (!br_dev)
		return 0;

	entry = ppe_vlan_find(priv, br_dev, vid);
	if (!entry) {
		entry = ppe_vlan_alloc(priv, br_dev, vid);
		if (!entry)
			return -ENOSPC;
	}

	entry->ports |= BIT(port);

	if (entry->xlt_idx < 0) {
		idx = ppe_xlt_idx_alloc(priv);
		if (idx < 0) {
			entry->ports &= ~BIT(port);
			if (!entry->ports)
				ppe_vlan_free(priv, entry);
			return -ENOSPC;
		}
		entry->xlt_idx = idx;
	}

	ppe_xlt_rule_set(priv, entry->xlt_idx,
			 entry->ports | BIT(QCA_PPE_CPU_PORT), vid, false);
	ppe_xlt_action_set(priv, entry->xlt_idx, entry->vsi, true);

	ppe_eg_vsi_tag_port_set(priv, entry->vsi, port,
				untagged ? PPE_EG_UNTAGGED : PPE_EG_TAGGED);

	if (pvid) {
		ppe_port_def_cvid_set(priv, port, vid, true);
		priv->port_pvid[port] = vid;
		entry->pvid_ports |= BIT(port);

		if (entry->xlt_pvid_idx < 0) {
			idx = ppe_xlt_idx_alloc(priv);
			if (idx < 0)
				return -ENOSPC;
			entry->xlt_pvid_idx = idx;
		}
		ppe_xlt_rule_set(priv, entry->xlt_pvid_idx,
				 entry->pvid_ports, 0, true);
		ppe_xlt_action_set(priv, entry->xlt_pvid_idx, entry->vsi,
				   false);
	} else if (priv->port_pvid[port] == vid) {
		ppe_port_def_cvid_set(priv, port, 0, false);
		priv->port_pvid[port] = 0;
		entry->pvid_ports &= ~BIT(port);

		ppe_vlan_pvid_update(priv, entry);
	}

	ppe_vlan_members_update(priv, entry);

	return 0;
}

int qca_ppe_port_vlan_del(struct dsa_switch *ds, int port,
			  const struct switchdev_obj_port_vlan *vlan)
{
	struct qca_ppe_priv *priv = ds_to_priv(ds);
	struct net_device *br_dev = priv->port_br_dev[port];
	struct qca_ppe_vlan_entry *entry;
	u16 vid = vlan->vid;

	if (!br_dev)
		return 0;

	entry = ppe_vlan_find(priv, br_dev, vid);
	if (!entry)
		return 0;

	entry->ports &= ~BIT(port);
	ppe_eg_vsi_tag_port_set(priv, entry->vsi, port, PPE_EG_UNMODIFIED);

	if (priv->port_pvid[port] == vid) {
		ppe_port_def_cvid_set(priv, port, 0, false);
		priv->port_pvid[port] = 0;
		entry->pvid_ports &= ~BIT(port);
	}

	if (!entry->ports) {
		ppe_vlan_free(priv, entry);
		return 0;
	}

	ppe_xlt_rule_set(priv, entry->xlt_idx,
			 entry->ports | BIT(QCA_PPE_CPU_PORT), vid, false);

	ppe_vlan_pvid_update(priv, entry);

	ppe_vlan_members_update(priv, entry);

	return 0;
}
