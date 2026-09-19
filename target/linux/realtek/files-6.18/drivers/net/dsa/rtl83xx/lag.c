// SPDX-License-Identifier: GPL-2.0-only

#include <linux/bitfield.h>
#include <linux/bitmap.h>
#include <linux/iopoll.h>
#include <linux/string.h>

#include "lag.h"

/* Caller must hold priv->reg_mutex */
int rtldsa_lag_add(struct dsa_switch *ds, int group, int port, struct netdev_lag_upper_info *info)
{
	struct rtl838x_switch_priv *priv = ds->priv;
	int ret;
	int i;

	for (i = 0; i < priv->ds->num_lag_ids; i++) {
		if (priv->lags_port_members[i] & BIT_ULL(port))
			break;
	}
	if (i != priv->ds->num_lag_ids) {
		pr_err("%s: Port %d already member of LAG %d.\n", __func__, port, i);
		return -ENOSPC;
	}

	if (priv->r->lag_setup_algomask) {
		ret = priv->r->lag_setup_algomask(priv, group, info);
		if (ret)
			return ret;
	}

	ret = priv->r->lag_set_port_members(priv, group,
					    priv->lags_port_members[group] | BIT_ULL(port), info);
	if (ret)
		return ret;

	pr_info("%s: Added port %d to LAG %d. Members now %016llx.\n",
		__func__, port, group, priv->lags_port_members[group]);

	return 0;
}

/* Caller must hold priv->reg_mutex */
int rtldsa_lag_del(struct dsa_switch *ds, int group, int port)
{
	struct rtl838x_switch_priv *priv = ds->priv;
	int ret;

	if (group >= priv->ds->num_lag_ids) {
		pr_err("%s: LAG %d invalid.\n", __func__, group);
		return -EINVAL;
	}

	if (!(priv->lags_port_members[group] & BIT_ULL(port))) {
		pr_err("%s: Port %d not member of LAG %d.\n", __func__, port, group);
		return -ENOSPC;
	}

	/* Don't touch hash mask bits, as only the port might be removed from
	 * the LAG group. This means the lag group stays valid with existing
	 * mask algo bits. If there are no lag members left, then
	 * rtl83xx_lag_add will reconfigure hash mask when new LAG group is
	 * created.
	 */
	ret = priv->r->lag_set_port_members(priv, group,
					    priv->lags_port_members[group] & ~BIT_ULL(port),
					    NULL);
	if (ret)
		return ret;

	pr_info("%s: Removed port %d from LAG %d. Members now %016llx.\n",
		__func__, port, group, priv->lags_port_members[group]);

	return 0;
}

int rtldsa_93xx_lag_set_distribution_algorithm(struct rtl838x_switch_priv *priv,
					       int group, int algoidx, u32 algomsk)
{
	bool is_l3 = false;
	u32 newmask = 0;

	if (algomsk & TRUNK_DISTRIBUTION_ALGO_SIP_BIT) {
		is_l3 = true;
		newmask |= TRUNK_DISTRIBUTION_ALGO_L3_SIP_BIT;
	}
	if (algomsk & TRUNK_DISTRIBUTION_ALGO_DIP_BIT) {
		is_l3 = true;
		newmask |= TRUNK_DISTRIBUTION_ALGO_L3_DIP_BIT;
	}
	if (algomsk & TRUNK_DISTRIBUTION_ALGO_SRC_L4PORT_BIT) {
		is_l3 = true;
		newmask |= TRUNK_DISTRIBUTION_ALGO_L3_SRC_L4PORT_BIT;
	}

	if (algomsk & TRUNK_DISTRIBUTION_ALGO_DST_L4PORT_BIT) {
		is_l3 = true;
		newmask |= TRUNK_DISTRIBUTION_ALGO_L3_DST_L4PORT_BIT;
	}

	if (is_l3) {
		if (algomsk & TRUNK_DISTRIBUTION_ALGO_SPA_BIT)
			newmask |= TRUNK_DISTRIBUTION_ALGO_L3_SPA_BIT;
		if (algomsk & TRUNK_DISTRIBUTION_ALGO_SMAC_BIT)
			newmask |= TRUNK_DISTRIBUTION_ALGO_L3_SMAC_BIT;
		if (algomsk & TRUNK_DISTRIBUTION_ALGO_DMAC_BIT)
			newmask |= TRUNK_DISTRIBUTION_ALGO_L3_DMAC_BIT;
		if (algomsk & TRUNK_DISTRIBUTION_ALGO_VLAN_BIT)
			newmask |= TRUNK_DISTRIBUTION_ALGO_L3_VLAN_BIT;
	} else {
		if (algomsk & TRUNK_DISTRIBUTION_ALGO_SPA_BIT)
			newmask |= TRUNK_DISTRIBUTION_ALGO_L2_SPA_BIT;
		if (algomsk & TRUNK_DISTRIBUTION_ALGO_SMAC_BIT)
			newmask |= TRUNK_DISTRIBUTION_ALGO_L2_SMAC_BIT;
		if (algomsk & TRUNK_DISTRIBUTION_ALGO_DMAC_BIT)
			newmask |= TRUNK_DISTRIBUTION_ALGO_L2_DMAC_BIT;
		if (algomsk & TRUNK_DISTRIBUTION_ALGO_VLAN_BIT)
			newmask |= TRUNK_DISTRIBUTION_ALGO_L2_VLAN_BIT;
	}

	sw_w32(newmask, priv->r->trk_hash_ctrl + (algoidx << 2));

	return 0;
}

int rtldsa_83xx_lag_setup_algomask(struct rtl838x_switch_priv *priv, int group,
				   struct netdev_lag_upper_info *info)
{
	u32 algomsk = 0;
	u32 algoidx = 0;

	switch (info->hash_type) {
	case NETDEV_LAG_HASH_L2:
		algomsk |= TRUNK_DISTRIBUTION_ALGO_DMAC_BIT;
		algomsk |= TRUNK_DISTRIBUTION_ALGO_SMAC_BIT;
		break;
	case NETDEV_LAG_HASH_L23:
		algomsk |= TRUNK_DISTRIBUTION_ALGO_DMAC_BIT;
		algomsk |= TRUNK_DISTRIBUTION_ALGO_SMAC_BIT;
		algomsk |= TRUNK_DISTRIBUTION_ALGO_SIP_BIT; /* source ip */
		algomsk |= TRUNK_DISTRIBUTION_ALGO_DIP_BIT; /* dest ip */
		algoidx = 1;
		break;
	case NETDEV_LAG_HASH_L34:
		algomsk |= TRUNK_DISTRIBUTION_ALGO_SRC_L4PORT_BIT; /* sport */
		algomsk |= TRUNK_DISTRIBUTION_ALGO_DST_L4PORT_BIT; /* dport */
		algomsk |= TRUNK_DISTRIBUTION_ALGO_SIP_BIT; /* source ip */
		algomsk |= TRUNK_DISTRIBUTION_ALGO_DIP_BIT; /* dest ip */
		algoidx = 2;
		break;
	default:
		algomsk |= TRUNK_DISTRIBUTION_ALGO_MASKALL;
	}

	return priv->r->lag_set_distribution_algorithm(priv, group, algoidx, algomsk);
}

static int rtldsa_93xx_lag_set_group2ports(struct rtl838x_switch_priv *priv, int group,
					   struct netdev_lag_upper_info *info)
{
	DECLARE_BITMAP(ports, ARRAY_SIZE(priv->ports));
	struct rtldsa_93xx_lag_entry e;
	unsigned int table_pos = 0;
	u8 num_of_lag_ports = 0;
	u8 group_ports[8];
	u32 data[3];
	int i;

	/* Read lag table using Table control register 2 */
	int tbl = priv->r->lag_table();

	__otto_table_read(tbl, group, &data);

	bitmap_clear(ports, 0, ARRAY_SIZE(priv->ports));
	bitmap_from_arr64(ports, &priv->lags_port_members[group],
			  ARRAY_SIZE(priv->ports));

	priv->r->lag_fill_data(data, &e);

	num_of_lag_ports = bitmap_weight(ports, ARRAY_SIZE(priv->ports));
	if (num_of_lag_ports > ARRAY_SIZE(group_ports)) {
		pr_err("%s: Number of LAG ports too high: %u", __func__,
		       num_of_lag_ports);

		otto_table_release(tbl);
		return -ENOSPC;
	}

	memset(group_ports, 0x3f, sizeof(group_ports));

	table_pos = 0;
	for_each_set_bit(i, ports, ARRAY_SIZE(priv->ports)) {
		if (!priv->ports[i].dp->lag_tx_enabled)
			continue;

		group_ports[table_pos] = i;
		table_pos++;
	}

	/* Remove tx disabled ports */
	num_of_lag_ports = table_pos;

	e.trk_dev0 = 0;
	e.trk_port0 = group_ports[0];
	e.trk_dev1 = 0;
	e.trk_port1 = group_ports[1];
	e.trk_dev2 = 0;
	e.trk_port2 = group_ports[2];
	e.trk_dev3 = 0;
	e.trk_port3 = group_ports[3];
	e.trk_dev4 = 0;
	e.trk_port4 = group_ports[4];
	e.trk_dev5 = 0;
	e.trk_port5 = group_ports[5];
	e.trk_dev6 = 0;
	e.trk_port6 = group_ports[6];
	e.trk_dev7 = 0;
	e.trk_port7 = group_ports[7];

	e.num_tx_candi = num_of_lag_ports;

	/* set hash_mask_idx to 0 if we are deleting lag group */
	if (info) {
		if (info->hash_type == NETDEV_LAG_HASH_L2) {
			e.l2_hash_mask_idx = RTL93XX_HASH_MASK_INDEX_L2;
			e.ip4_hash_mask_idx = RTL93XX_HASH_MASK_INDEX_L2;
			e.ip6_hash_mask_idx = RTL93XX_HASH_MASK_INDEX_L2;
		} else if (info->hash_type == NETDEV_LAG_HASH_L23) {
			e.l2_hash_mask_idx = RTL93XX_HASH_MASK_INDEX_L23;
			e.ip4_hash_mask_idx = RTL93XX_HASH_MASK_INDEX_L23;
			e.ip6_hash_mask_idx = RTL93XX_HASH_MASK_INDEX_L23;
		} else {
			otto_table_release(tbl);
			return -EOPNOTSUPP;
		}
	}

	priv->r->lag_write_data(data, &e);

	__otto_table_write(tbl, group, &data);
	otto_table_release(tbl);

	return 0;
}

static inline void rtldsa_93xx_lag_set_local_group2ports(struct rtl838x_switch_priv *priv, int group,
						  u64 ports)
{
	priv->r->set_port_reg_be(ports, priv->r->trk_mbr_ctr(group));
}

int rtldsa_93xx_lag_set_port_members(struct rtl838x_switch_priv *priv, int group,
				     u64 members, struct netdev_lag_upper_info *info)
{
	DECLARE_BITMAP(affected_members, ARRAY_SIZE(priv->ports));
	bool valid_group;
	u64 old_members;
	u64 affected;
	size_t port;
	int ret;

	/* calculate modifications of the LAG group */
	old_members = priv->lags_port_members[group];
	priv->lags_port_members[group] = members;

	affected = old_members | priv->lags_port_members[group];

	bitmap_clear(affected_members, 0, ARRAY_SIZE(priv->ports));
	bitmap_from_arr64(affected_members, &affected, BITS_PER_TYPE(affected));

	valid_group = __sw_hweight64(priv->lags_port_members[group]);

	/* apply global group and port settings */
	ret = rtldsa_93xx_lag_set_group2ports(priv, group, info);
	if (ret)
		return ret;

	for_each_set_bit(port, affected_members, ARRAY_SIZE(priv->ports)) {
		bool valid = priv->lags_port_members[group] & BIT_ULL(port);

		priv->r->lag_set_port2group(group, port, valid);
	}

	/* apply local group and port settings */
	priv->r->lag_set_local_group_id(group, group, valid_group);
	rtldsa_93xx_lag_set_local_group2ports(priv, group, priv->lags_port_members[group]);

	for_each_set_bit(port, affected_members, ARRAY_SIZE(priv->ports)) {
		bool valid = priv->lags_port_members[group] & BIT_ULL(port);

		priv->r->lag_set_local_port2group(group, port, valid);
	}

	/* write lag table (and maybe additional information) to SRAM */
	priv->r->lag_sync_tables();

	return 0;
}

void rtldsa_93xx_lag_switch_init(struct rtl838x_switch_priv *priv)
{
	u32 trk_ctrlmask = 0;
	u32 algomask;

	trk_ctrlmask |= RTL93XX_TRK_CTRL_NON_TMN_TUNNEL_HASH_SEL;
	trk_ctrlmask |= RTL93XX_TRK_CTRL_TRK_STAND_ALONE_MODE;
	trk_ctrlmask |= RTL93XX_TRK_CTRL_LOCAL_FIRST;

	sw_w32(trk_ctrlmask, priv->r->trk_ctrl);

	/* Setup NETDEV_LAG_HASH_L2 on slot 0 */
	algomask = TRUNK_DISTRIBUTION_ALGO_SMAC_BIT |
		   TRUNK_DISTRIBUTION_ALGO_DMAC_BIT;
	priv->r->lag_set_distribution_algorithm(priv, 0, RTL93XX_HASH_MASK_INDEX_L2, algomask);

	/* Setup NETDEV_LAG_HASH_L23 on slot 1 */
	algomask = TRUNK_DISTRIBUTION_ALGO_SMAC_BIT |
		   TRUNK_DISTRIBUTION_ALGO_DMAC_BIT |
		   TRUNK_DISTRIBUTION_ALGO_SIP_BIT |
		   TRUNK_DISTRIBUTION_ALGO_DIP_BIT;
	priv->r->lag_set_distribution_algorithm(priv, 0, RTL93XX_HASH_MASK_INDEX_L23, algomask);
}

int rtl838x_trk_mbr_ctr(int group)
{
	return RTL838X_TRK_MBR_CTR + (group << 2);
}

int rtldsa_838x_set_distribution_algorithm(struct rtl838x_switch_priv *priv,
						  int group, int algoidx, u32 algomsk)
{
	algoidx &= 1; /* RTL838X only supports 2 concurrent algorithms */
	sw_w32_mask(1 << (group % 8), algoidx << (group % 8),
		    RTL838X_TRK_HASH_IDX_CTRL + ((group >> 3) << 2));
	sw_w32(algomsk, RTL838X_TRK_HASH_CTRL + (algoidx << 2));
	return 0;
}

int rtldsa_838x_lag_set_port_members(struct rtl838x_switch_priv *priv, int group,
					    u64 members, struct netdev_lag_upper_info *info)
{
	priv->lags_port_members[group] = members;

	priv->r->set_port_reg_be(priv->lags_port_members[group],
				 priv->r->trk_mbr_ctr(group));

	return 0;
}

int rtl839x_trk_mbr_ctr(int group)
{
	return RTL839X_TRK_MBR_CTR + (group << 3);
}

int rtldsa_839x_set_distribution_algorithm(struct rtl838x_switch_priv *priv,
						  int group, int algoidx, u32 algomsk)
{
	sw_w32_mask(3 << ((group & 0xf) << 1), algoidx << ((group & 0xf) << 1),
		    RTL839X_TRK_HASH_IDX_CTRL + ((group >> 4) << 2));
	sw_w32(algomsk, RTL839X_TRK_HASH_CTRL + (algoidx << 2));

	return 0;
}

int rtldsa_839x_lag_set_port_members(struct rtl838x_switch_priv *priv, int group,
					    u64 members, struct netdev_lag_upper_info *info)
{
	priv->lags_port_members[group] = members;

	priv->r->set_port_reg_be(priv->lags_port_members[group],
				 priv->r->trk_mbr_ctr(group));

	return 0;
}

int rtl930x_trk_mbr_ctr(int group)
{
	return RTL930X_TRK_MBR_CTRL + (group << 2);
}

void rtldsa_930x_lag_set_port2group(int group, int port, bool valid)
{
	int tbl = otto_table_acquire(RTL9300_TBL_SRC_TRK_MAP);
	u32 mask = valid ? RTL930X_SRC_TRK_MAP_TRK_VALID : 0;

	mask |= FIELD_PREP(RTL930X_SRC_TRK_MAP_TRK_ID, group); // Update TRK Field.
	__otto_table_write(tbl, port, &mask);
	otto_table_release(tbl);
}

/* Write data from the data buffer into the lag-entry strucure */
void rtldsa_930x_lag_fill_data(u32 data[], struct rtldsa_93xx_lag_entry *e)
{
	/* 95-64 */
	e->num_tx_candi = FIELD_GET(RTL930X_LAG_NUM_TX_CANDI, data[0]);
	e->l2_hash_mask_idx = FIELD_GET(RTL930X_LAG_L2_HASH_MSK_IDX, data[0]);
	e->ip4_hash_mask_idx = FIELD_GET(RTL930X_LAG_IP4_HASH_MSK_IDX, data[0]);
	e->ip6_hash_mask_idx = FIELD_GET(RTL930X_LAG_IP6_HASH_MSK_IDX, data[0]);
	e->flood_dlf_bcast.sep_dlf_bcast_en = FIELD_GET(RTL930X_LAG_SEP_DLF_BCAST_EN, data[0]);
	e->sep_kwn_mc_en = FIELD_GET(RTL930X_LAG_SEP_KWN_MC_EN, data[0]);
	e->trk_dev7 = FIELD_GET(RTL930X_LAG_TRK_DEV7, data[0]);
	e->trk_port7 = FIELD_GET(RTL930X_LAG_TRK_PORT7, data[0]);
	e->trk_dev6 = FIELD_GET(RTL930X_LAG_TRK_DEV6, data[0]);
	e->trk_port6 = FIELD_GET(RTL930X_LAG_TRK_PORT6, data[0]);

	/* 63-32 */
	e->trk_dev5 = FIELD_GET(RTL930X_LAG_TRK_DEV5, data[1]);
	e->trk_port5 = FIELD_GET(RTL930X_LAG_TRK_PORT5, data[1]);
	e->trk_dev4 = FIELD_GET(RTL930X_LAG_TRK_DEV4, data[1]);
	e->trk_port4 = FIELD_GET(RTL930X_LAG_TRK_PORT4, data[1]);
	e->trk_dev3 = FIELD_GET(RTL930X_LAG_TRK_DEV3, data[1]);
	e->trk_port3 = FIELD_GET(RTL930X_LAG_TRK_PORT3, data[1]);

	/* 31-0 */
	e->trk_dev2 = FIELD_GET(RTL930X_LAG_TRK_DEV2, data[2]);
	e->trk_port2 = FIELD_GET(RTL930X_LAG_TRK_PORT2, data[2]);
	e->trk_dev1 = FIELD_GET(RTL930X_LAG_TRK_DEV1, data[2]);
	e->trk_port1 = FIELD_GET(RTL930X_LAG_TRK_PORT1, data[2]);
	e->trk_dev0 = FIELD_GET(RTL930X_LAG_TRK_DEV0, data[2]);
	e->trk_port0 = FIELD_GET(RTL930X_LAG_TRK_PORT0, data[2]);
}

/* Write lag-entry data into buffer */
void rtldsa_930x_lag_write_data(u32 data[], struct rtldsa_93xx_lag_entry *e)
{
	/* 95-64 */
	data[0] = FIELD_PREP(RTL930X_LAG_NUM_TX_CANDI, e->num_tx_candi);
	data[0] |= FIELD_PREP(RTL930X_LAG_L2_HASH_MSK_IDX, e->l2_hash_mask_idx);
	data[0] |= FIELD_PREP(RTL930X_LAG_IP4_HASH_MSK_IDX, e->ip4_hash_mask_idx);
	data[0] |= FIELD_PREP(RTL930X_LAG_IP6_HASH_MSK_IDX, e->ip6_hash_mask_idx);
	data[0] |= FIELD_PREP(RTL930X_LAG_SEP_DLF_BCAST_EN, e->flood_dlf_bcast.sep_dlf_bcast_en);
	data[0] |= FIELD_PREP(RTL930X_LAG_SEP_KWN_MC_EN, e->sep_kwn_mc_en);
	data[0] |= FIELD_PREP(RTL930X_LAG_TRK_DEV7, e->trk_dev7);
	data[0] |= FIELD_PREP(RTL930X_LAG_TRK_PORT7, e->trk_port7);
	data[0] |= FIELD_PREP(RTL930X_LAG_TRK_DEV6, e->trk_dev6);
	data[0] |= FIELD_PREP(RTL930X_LAG_TRK_PORT6, e->trk_port6);

	/* 63-32 */
	data[1] = FIELD_PREP(RTL930X_LAG_TRK_DEV5, e->trk_dev5);
	data[1] |= FIELD_PREP(RTL930X_LAG_TRK_PORT5, e->trk_port5);
	data[1] |= FIELD_PREP(RTL930X_LAG_TRK_DEV4, e->trk_dev4);
	data[1] |= FIELD_PREP(RTL930X_LAG_TRK_PORT4, e->trk_port4);
	data[1] |= FIELD_PREP(RTL930X_LAG_TRK_DEV3, e->trk_dev3);
	data[1] |= FIELD_PREP(RTL930X_LAG_TRK_PORT3, e->trk_port3);

	/* 31-0 */
	data[2] = FIELD_PREP(RTL930X_LAG_TRK_DEV2, e->trk_dev2);
	data[2] |= FIELD_PREP(RTL930X_LAG_TRK_PORT2, e->trk_port2);
	data[2] |= FIELD_PREP(RTL930X_LAG_TRK_DEV1, e->trk_dev1);
	data[2] |= FIELD_PREP(RTL930X_LAG_TRK_PORT1, e->trk_port1);
	data[2] |= FIELD_PREP(RTL930X_LAG_TRK_DEV0, e->trk_dev0);
	data[2] |= FIELD_PREP(RTL930X_LAG_TRK_PORT0, e->trk_port0);
}

void rtldsa_930x_lag_set_local_group_id(int local_group, int global_group, bool valid)
{
	u32 mask = 0;

	mask |= valid ? RTL930X_TRK_ID_CTRL_TRK_VALID : 0;
	mask |= FIELD_PREP(RTL930X_TRK_ID_CTRL_TRK_ID, global_group);
	sw_w32(mask, RTL930X_TRK_ID_CTRL + (4 * local_group));
}

void rtldsa_930x_lag_set_local_port2group(int group, int port, bool valid)
{
	u32 mask = 0;

	mask |= valid ? RTL930X_LOCAL_PORT_TRK_MAP_IS_TRK_MBR : 0;
	mask |= FIELD_PREP(RTL930X_LOCAL_PORT_TRK_MAP_TRK_ID, group);
	sw_w32(mask, RTL930X_LOCAL_PORT_TRK_MAP + (4 * port));
}

void rtldsa_930x_lag_sync_tables(void)
{
	u32 val;
	int ret;

	sw_w32(BIT(0), RTL930X_TRK_LOCAL_TBL_REFRESH);

	ret = readx_poll_timeout(sw_r32, RTL930X_TRK_LOCAL_TBL_REFRESH, val,
				 !(val & BIT(0)), 20, 10000);
	if (ret)
		pr_err("%s: timeout\n", __func__);
}

int rtldsa_930x_lag_table(void)
{
	return otto_table_acquire(RTL9300_TBL_LAG);
}

int rtldsa_931x_trk_mbr_ctr(int group)
{
	return RTL931X_TRK_MBR_CTRL + (group << 3);
}

void rtldsa_931x_lag_set_port2group(int group, int port, bool valid)
{
	u32 trk_id_valid = valid ? RTL931X_SRC_TRK_MAP_TRK_ID_VALID : 0;
	int tbl = otto_table_acquire(RTL9310_TBL_SRC_TRK_MAP);
	u32 mask = 0;

	mask |= trk_id_valid;
	/* Update TRK Field */
	mask |= FIELD_PREP(RTL931X_SRC_TRK_MAP_TRK_ID, group);

	__otto_table_write(tbl, port, &mask);
	otto_table_release(tbl);
}

/* Write data from the data buffer into the lag-entry strucure */
void rtldsa_931x_lag_fill_data(u32 data[], struct rtldsa_93xx_lag_entry *e)
{
	/* 95-64 */
	e->num_tx_candi = FIELD_GET(RTL931X_LAG_NUM_TX_CANDI, data[0]);
	e->l2_hash_mask_idx = FIELD_GET(RTL931X_LAG_L2_HASH_MSK_IDX, data[0]);
	e->ip4_hash_mask_idx = FIELD_GET(RTL931X_LAG_IP4_HASH_MSK_IDX, data[0]);
	e->ip6_hash_mask_idx = FIELD_GET(RTL931X_LAG_IP6_HASH_MSK_IDX, data[0]);
	e->flood_dlf_bcast.sep_flood_en = FIELD_GET(RTL931X_LAG_SEP_FLOOD_EN, data[0]);
	e->sep_kwn_mc_en = FIELD_GET(RTL931X_LAG_SEP_KWN_MC_EN, data[0]);
	e->trk_dev7 = FIELD_GET(RTL931X_LAG_TRK_DEV7, data[0]);
	e->trk_port7 = FIELD_GET(RTL931X_LAG_TRK_PORT7, data[0]);
	e->trk_dev6 = FIELD_GET(RTL931X_LAG_TRK_DEV6, data[0]);
	e->trk_port6 = FIELD_GET(RTL931X_LAG_TRK_PORT6, data[0]);

	/* 63-32 */
	e->trk_dev5 = FIELD_GET(RTL931X_LAG_TRK_DEV5, data[1]);
	e->trk_port5 = FIELD_GET(RTL931X_LAG_TRK_PORT5, data[1]);
	e->trk_dev4 = FIELD_GET(RTL931X_LAG_TRK_DEV4, data[1]);
	e->trk_port4 = FIELD_GET(RTL931X_LAG_TRK_PORT4, data[1]);
	e->trk_dev3 = FIELD_GET(RTL931X_LAG_TRK_DEV3, data[1]);
	e->trk_port3 = FIELD_GET(RTL931X_LAG_TRK_PORT3, data[1]);

	/* 31-0 */
	e->trk_dev2 = FIELD_GET(RTL931X_LAG_TRK_DEV2, data[2]);
	e->trk_port2 = FIELD_GET(RTL931X_LAG_TRK_PORT2, data[2]);
	e->trk_dev1 = FIELD_GET(RTL931X_LAG_TRK_DEV1, data[2]);
	e->trk_port1 = FIELD_GET(RTL931X_LAG_TRK_PORT1, data[2]);
	e->trk_dev0 = FIELD_GET(RTL931X_LAG_TRK_DEV0, data[2]);
	e->trk_port0 = FIELD_GET(RTL931X_LAG_TRK_PORT0, data[2]);
}

/* Write lag-entry data into buffer */
void rtldsa_931x_lag_write_data(u32 data[], struct rtldsa_93xx_lag_entry *e)
{
	/* 95-64 */
	data[0] = FIELD_PREP(RTL931X_LAG_NUM_TX_CANDI, e->num_tx_candi);
	data[0] |= FIELD_PREP(RTL931X_LAG_L2_HASH_MSK_IDX, e->l2_hash_mask_idx);
	data[0] |= FIELD_PREP(RTL931X_LAG_IP4_HASH_MSK_IDX, e->ip4_hash_mask_idx);
	data[0] |= FIELD_PREP(RTL931X_LAG_IP6_HASH_MSK_IDX, e->ip6_hash_mask_idx);
	data[0] |= FIELD_PREP(RTL931X_LAG_SEP_FLOOD_EN, e->flood_dlf_bcast.sep_flood_en);
	data[0] |= FIELD_PREP(RTL931X_LAG_SEP_KWN_MC_EN, e->sep_kwn_mc_en);
	data[0] |= FIELD_PREP(RTL931X_LAG_TRK_DEV7, e->trk_dev7);
	data[0] |= FIELD_PREP(RTL931X_LAG_TRK_PORT7, e->trk_port7);
	data[0] |= FIELD_PREP(RTL931X_LAG_TRK_DEV6, e->trk_dev6);
	data[0] |= FIELD_PREP(RTL931X_LAG_TRK_PORT6, e->trk_port6);

	/* 63-32 */
	data[1] = FIELD_PREP(RTL931X_LAG_TRK_DEV5, e->trk_dev5);
	data[1] |= FIELD_PREP(RTL931X_LAG_TRK_PORT5, e->trk_port5);
	data[1] |= FIELD_PREP(RTL931X_LAG_TRK_DEV4, e->trk_dev4);
	data[1] |= FIELD_PREP(RTL931X_LAG_TRK_PORT4, e->trk_port4);
	data[1] |= FIELD_PREP(RTL931X_LAG_TRK_DEV3, e->trk_dev3);
	data[1] |= FIELD_PREP(RTL931X_LAG_TRK_PORT3, e->trk_port3);

	/* 31-0 */
	data[2] = FIELD_PREP(RTL931X_LAG_TRK_DEV2, e->trk_dev2);
	data[2] |= FIELD_PREP(RTL931X_LAG_TRK_PORT2, e->trk_port2);
	data[2] |= FIELD_PREP(RTL931X_LAG_TRK_DEV1, e->trk_dev1);
	data[2] |= FIELD_PREP(RTL931X_LAG_TRK_PORT1, e->trk_port1);
	data[2] |= FIELD_PREP(RTL931X_LAG_TRK_DEV0, e->trk_dev0);
	data[2] |= FIELD_PREP(RTL931X_LAG_TRK_PORT0, e->trk_port0);
}

void rtldsa_931x_lag_set_local_group_id(int local_group, int global_group, bool valid)
{
	u32 mask = 0;

	mask |= valid ? RTL931X_TRK_ID_CTRL_TRK_VALID : 0;
	mask |= FIELD_PREP(RLT931X_TRK_ID_CTRL_TRK_ID, global_group);
	sw_w32(mask, RTL931X_TRK_ID_CTRL + (4 * local_group));
}

void rtldsa_931x_lag_set_local_port2group(int group, int port, bool valid)
{
	u32 mask = 0;

	mask |= valid ? RTL931X_LOCAL_PORT_TRK_MAP_IS_TRK_MBR : 0;
	mask |= FIELD_PREP(RTL931X_LOCAL_PORT_TRK_MAP_TRK_ID, group);
	sw_w32(mask, RTL931X_LOCAL_PORT_TRK_MAP + (4 * port));
}

void rtldsa_931x_lag_sync_tables(void)
{
	u32 val;
	int ret;

	sw_w32(BIT(0), RTL931X_TRK_LOCAL_TBL_REFRESH);

	ret = readx_poll_timeout(sw_r32, RTL931X_TRK_LOCAL_TBL_REFRESH, val,
				 !(val & BIT(0)), 20, 10000);
	if (ret)
		pr_err("%s: timeout\n", __func__);
}

int rtldsa_931x_lag_table(void)
{
	return otto_table_acquire(RTL9310_TBL_LAG);
}
