// SPDX-License-Identifier: GPL-2.0
/*
 * DSA support for the switch embedded in the MediaTek MT7620 SoC.
 */

#include <linux/bitfield.h>
#include <linux/ethtool.h>
#include <linux/if_bridge.h>
#include <linux/if_vlan.h>
#include <linux/interrupt.h>
#include <linux/iopoll.h>
#include <linux/netdevice.h>
#include <linux/of.h>
#include <linux/phy.h>
#include <linux/platform_device.h>
#include <linux/soc/mediatek/mt7620-gsw.h>
#include <linux/soc/mediatek/mt7620-ppe.h>
#include <net/dsa.h>
#include <net/switchdev.h>

#include "gsw_mt7620.h"

#define MT7620_DSA_NUM_PORTS		7
#define MT7620_DSA_CPU_PORT		6
#define MT7620_DSA_USER_PORTS		GENMASK(4, 0)
#define MT7620_DSA_NUM_EPHYS		5

#define MT7620_GSW_SSC(p)		(0x2000 + ((p) * 0x100))
#define MT7620_GSW_PCR(p)		(0x2004 + ((p) * 0x100))
#define MT7620_GSW_PVC(p)		(0x2010 + ((p) * 0x100))
#define MT7620_GSW_PPBV1(p)		(0x2014 + ((p) * 0x100))

#define MT7620_GSW_VTCR			0x0090
#define MT7620_GSW_VAWD1		0x0094
#define MT7620_GSW_VAWD2		0x0098
#define MT7620_GSW_VTIM(slot)		(0x0100 + (((slot) / 2) * 4))

#define MT7620_GSW_ATA1			0x0074
#define MT7620_GSW_ATA2			0x0078
#define MT7620_GSW_ATWD			0x007c
#define MT7620_GSW_ATC			0x0080
#define MT7620_GSW_TSRA1		0x0084
#define MT7620_GSW_TSRA2		0x0088
#define MT7620_GSW_ATRD			0x008c
#define MT7620_GSW_MFC			0x0010
#define MT7620_GSW_AAC			0x00a0
#define MT7620_GSW_MIB_BASE(p)		(0x4000 + ((p) * 0x100))

#define MT7620_MFC_BC_FFP		GENMASK(31, 24)
#define MT7620_MFC_UNM_FFP		GENMASK(23, 16)
#define MT7620_MFC_UNU_FFP		GENMASK(15, 8)
#define MT7620_MFC_MIRROR_ENABLE	BIT(3)
#define MT7620_MFC_MIRROR_DEST		GENMASK(2, 0)

#define MT7620_AAC_AGE_CNT		GENMASK(19, 12)
#define MT7620_AAC_AGE_CNT_MAX		0xff
#define MT7620_AAC_AGE_UNIT		GENMASK(11, 0)
#define MT7620_AAC_AGE_UNIT_MAX		0xfff

#define MT7620_ATA2_IVL			BIT(15)
#define MT7620_ATA2_FID			GENMASK(14, 12)
#define MT7620_ATA2_VID			GENMASK(11, 0)

#define MT7620_ATWD_AGE			GENMASK(31, 24)
#define MT7620_ATWD_MY_MAC		BIT(23)
#define MT7620_ATWD_PORT_MAP		GENMASK(11, 4)
#define MT7620_ATWD_STATUS		GENMASK(3, 2)

#define MT7620_ATC_BUSY			BIT(15)
#define MT7620_ATC_SEARCH_END		BIT(14)
#define MT7620_ATC_SEARCH_HIT		BIT(13)
#define MT7620_ATC_INVALID		BIT(12)
#define MT7620_ATC_MATCH		GENMASK(11, 8)

#define MT7620_FDB_READ			0
#define MT7620_FDB_WRITE		1
#define MT7620_FDB_FLUSH		2
#define MT7620_FDB_START		4
#define MT7620_FDB_NEXT			5

#define MT7620_FDB_EMPTY		0
#define MT7620_FDB_STATIC		3
#define MT7620_FDB_ENTRIES		2048
#define MT7620_FDB_MATCH_ALL_MAC	0
#define MT7620_FDB_MATCH_DYNAMIC_MAC	4

#define MT7620_VTCR_BUSY		BIT(31)
#define MT7620_VTCR_FUNC		GENMASK(15, 12)
#define MT7620_VTCR_INDEX		GENMASK(3, 0)

#define MT7620_VTCR_READ		0
#define MT7620_VTCR_WRITE		1

#define MT7620_VAWD1_IVL_MAC		BIT(30)
#define MT7620_VAWD1_VTAG_EN		BIT(28)
#define MT7620_VAWD1_PORT_MEM		GENMASK(23, 16)
#define MT7620_VAWD1_FID		GENMASK(3, 1)
#define MT7620_VAWD1_VALID		BIT(0)

#define MT7620_VLAN_FID_BRIDGED		1

#define MT7620_VAWD2_EGRESS(p, mode)	((mode) << ((p) * 2))

#define MT7620_VLAN_EGRESS_UNTAG	0
#define MT7620_VLAN_EGRESS_TAG		2

#define MT7620_PCR_MATRIX		GENMASK(23, 16)
#define MT7620_PCR_MIRROR_TX		BIT(9)
#define MT7620_PCR_MIRROR_RX		BIT(8)
#define MT7620_PCR_PORT_VLAN		GENMASK(1, 0)

#define MT7620_PORT_MATRIX_MODE		0
#define MT7620_PORT_FALLBACK_MODE	1
#define MT7620_PORT_SECURITY_MODE	3

#define MT7620_PSC_SA_DIS		BIT(4)

#define MT7620_PVC_EG_TAG		GENMASK(10, 8)
#define MT7620_PVC_VLAN_ATTR		GENMASK(7, 6)
#define MT7620_PVC_ACCEPT_FRAME		GENMASK(1, 0)
#define MT7620_PVC_EG_TAG_CONSISTENT	1
#define MT7620_PVC_EG_TAG_DISABLED	0
#define MT7620_PVC_VLAN_USER		0
#define MT7620_PVC_VLAN_TRANSPARENT	3
#define MT7620_PVC_ACCEPT_ALL		0
#define MT7620_PVC_ACCEPT_TAGGED	1
#define MT7620_PVC_PORT_SPEC_TAG	BIT(5)

#define MT7620_PPBV1_PVID		GENMASK(11, 0)

#define MT7620_STP_DISABLED		0
#define MT7620_STP_BLOCKING		1
#define MT7620_STP_LEARNING		2
#define MT7620_STP_FORWARDING		3

#define MT7620_MAX_FRAME_LEN		2048
#define MT7620_MAX_MTU			(MT7620_MAX_FRAME_LEN - VLAN_ETH_HLEN - \
					 ETH_FCS_LEN)

/*
 * The 16-bit packet counters wrap in about 440 ms at 100BASE-T line rate.
 * Poll with enough margin for scheduling delays while traffic is active.
 */
#define MT7620_MIB_INTERVAL		msecs_to_jiffies(100)

enum mt7620_gsw_mib_id {
	MT7620_MIB_TX_GOOD_PACKETS,
	MT7620_MIB_TX_BAD_PACKETS,
	MT7620_MIB_TX_BAD_BYTES,
	MT7620_MIB_TX_BYTES,
	MT7620_MIB_TX_DROPPED,
	MT7620_MIB_RX_GOOD_PACKETS,
	MT7620_MIB_RX_BAD_PACKETS,
	MT7620_MIB_RX_BAD_BYTES,
	MT7620_MIB_RX_BYTES,
	MT7620_MIB_RX_CTRL_DROPPED,
	MT7620_MIB_RX_INGRESS_DROPPED,
	MT7620_MIB_RX_ARL_DROPPED,
	MT7620_MIB_RX_FILTERED,
};

struct mt7620_gsw_mib_desc {
	u16 offset;
	u8 shift;
	u32 mask;
	const char *name;
};

static const struct mt7620_gsw_mib_desc mt7620_gsw_mibs[] = {
	[MT7620_MIB_TX_GOOD_PACKETS] = {
		0x10, 0, U16_MAX, "tx_good_packets"
	},
	[MT7620_MIB_TX_BAD_PACKETS] = {
		0x10, 16, U16_MAX, "tx_bad_packets"
	},
	[MT7620_MIB_TX_BAD_BYTES] = {
		0x14, 0, U32_MAX, "tx_bad_bytes"
	},
	[MT7620_MIB_TX_BYTES] = {
		0x18, 0, U32_MAX, "tx_good_bytes"
	},
	[MT7620_MIB_TX_DROPPED] = {
		0x1c, 0, U16_MAX, "tx_dropped"
	},
	[MT7620_MIB_RX_GOOD_PACKETS] = {
		0x20, 0, U16_MAX, "rx_good_packets"
	},
	[MT7620_MIB_RX_BAD_PACKETS] = {
		0x20, 16, U16_MAX, "rx_bad_packets"
	},
	[MT7620_MIB_RX_BAD_BYTES] = {
		0x24, 0, U32_MAX, "rx_bad_bytes"
	},
	[MT7620_MIB_RX_BYTES] = {
		0x28, 0, U32_MAX, "rx_good_bytes"
	},
	[MT7620_MIB_RX_CTRL_DROPPED] = {
		0x2c, 16, U16_MAX, "rx_ctrl_dropped"
	},
	[MT7620_MIB_RX_INGRESS_DROPPED] = {
		0x2c, 0, U16_MAX, "rx_ingress_dropped"
	},
	[MT7620_MIB_RX_ARL_DROPPED] = {
		0x30, 16, U16_MAX, "rx_arl_dropped"
	},
	[MT7620_MIB_RX_FILTERED] = {
		0x30, 0, U16_MAX, "rx_filtered"
	},
};

static_assert(ARRAY_SIZE(mt7620_gsw_mibs) == GSW_NUM_MIB_COUNTERS);

struct mt7620_gsw_fdb {
	u8 mac[ETH_ALEN];
	u16 vid;
	u8 port_mask;
	bool noarp;
};

static void mt7620_gsw_mib_update(struct mt7620_gsw *gsw)
{
	u32 raw[GSW_NUM_PORTS][GSW_NUM_MIB_COUNTERS];
	int port, i;

	for (port = 0; port < MT7620_DSA_NUM_PORTS; port++) {
		for (i = 0; i < GSW_NUM_MIB_COUNTERS; i++) {
			const struct mt7620_gsw_mib_desc *mib =
				&mt7620_gsw_mibs[i];

			raw[port][i] = (mtk_switch_r32(gsw,
				MT7620_GSW_MIB_BASE(port) + mib->offset) >>
				mib->shift) & mib->mask;
		}
	}

	spin_lock_bh(&gsw->mib_lock);
	for (port = 0; port < MT7620_DSA_NUM_PORTS; port++) {
		for (i = 0; i < GSW_NUM_MIB_COUNTERS; i++) {
			if (gsw->mib_initialized)
				gsw->mib_stats[port][i] += (raw[port][i] -
					gsw->mib_last[port][i]) &
					mt7620_gsw_mibs[i].mask;
			gsw->mib_last[port][i] = raw[port][i];
		}
	}
	gsw->mib_initialized = true;
	spin_unlock_bh(&gsw->mib_lock);
}

static void mt7620_gsw_mib_work(struct work_struct *work)
{
	struct mt7620_gsw *gsw =
		container_of(to_delayed_work(work), struct mt7620_gsw, mib_work);

	mt7620_gsw_mib_update(gsw);
	if (READ_ONCE(gsw->mib_active_ports))
		schedule_delayed_work(&gsw->mib_work, MT7620_MIB_INTERVAL);
}

static void mt7620_gsw_rmw(struct mt7620_gsw *gsw, u32 reg, u32 mask,
			   u32 set)
{
	mutex_lock(&gsw->reg_mutex);
	mtk_switch_w32(gsw, (mtk_switch_r32(gsw, reg) & ~mask) |
		       (set & mask), reg);
	mutex_unlock(&gsw->reg_mutex);
}

static void mt7620_gsw_rmw_locked(struct mt7620_gsw *gsw, u32 reg, u32 mask,
				  u32 set)
{
	mtk_switch_w32(gsw, (mtk_switch_r32(gsw, reg) & ~mask) |
		       (set & mask), reg);
}

static int mt7620_gsw_poll_busy(struct mt7620_gsw *gsw, u32 reg, u32 busy)
{
	u32 val;

	return read_poll_timeout(mtk_switch_r32, val, !(val & busy), 20, 20000,
				 false, gsw, reg);
}

static int mt7620_gsw_vlan_cmd_locked(struct mt7620_gsw *gsw, u8 cmd,
				      u8 slot)
{
	int ret;

	mtk_switch_w32(gsw, MT7620_VTCR_BUSY |
		       FIELD_PREP(MT7620_VTCR_FUNC, cmd) |
		       FIELD_PREP(MT7620_VTCR_INDEX, slot), MT7620_GSW_VTCR);

	ret = mt7620_gsw_poll_busy(gsw, MT7620_GSW_VTCR,
				   MT7620_VTCR_BUSY);
	if (ret)
		dev_err(gsw->dev, "VLAN table command timed out\n");

	return ret;
}

static void mt7620_gsw_vlan_set_vid_locked(struct mt7620_gsw *gsw, int slot,
					   u16 vid)
{
	u32 mask = GENMASK(11, 0);
	u32 shift = (slot & 1) * 12;

	mt7620_gsw_rmw_locked(gsw, MT7620_GSW_VTIM(slot), mask << shift,
			      (vid & mask) << shift);
}

static int mt7620_gsw_vlan_write_locked(struct mt7620_gsw *gsw, int slot)
{
	struct mt7620_gsw_vlan *vlan = &gsw->vlans[slot];
	u32 vawd1 = 0;
	u32 vawd2 = 0;
	int port;

	if (vlan->valid && vlan->members) {
		vawd1 = MT7620_VAWD1_IVL_MAC | MT7620_VAWD1_VTAG_EN |
			FIELD_PREP(MT7620_VAWD1_PORT_MEM, vlan->members) |
			FIELD_PREP(MT7620_VAWD1_FID,
				   MT7620_VLAN_FID_BRIDGED) |
			MT7620_VAWD1_VALID;

		for (port = 0; port < MT7620_DSA_NUM_PORTS; port++) {
			u32 mode;

			if (!(vlan->members & BIT(port)))
				continue;

			mode = vlan->untagged & BIT(port) ?
			       MT7620_VLAN_EGRESS_UNTAG :
			       MT7620_VLAN_EGRESS_TAG;
			vawd2 |= MT7620_VAWD2_EGRESS(port, mode);
		}

		/* Preserve the VLAN tag if TPF redirects the frame through PPE. */
		vawd2 |= MT7620_VAWD2_EGRESS(MT7620_GSW_PPE_PORT,
					     MT7620_VLAN_EGRESS_TAG);
	}

	mtk_switch_w32(gsw, vawd1, MT7620_GSW_VAWD1);
	mtk_switch_w32(gsw, vawd2, MT7620_GSW_VAWD2);

	return mt7620_gsw_vlan_cmd_locked(gsw, MT7620_VTCR_WRITE, slot);
}

static int mt7620_gsw_vlan_find_locked(struct mt7620_gsw *gsw, u16 vid)
{
	int slot;

	for (slot = 0; slot < GSW_NUM_VLANS; slot++)
		if (gsw->vlans[slot].valid && gsw->vlans[slot].vid == vid)
			return slot;

	return -ENOENT;
}

static int mt7620_gsw_vlan_alloc_locked(struct mt7620_gsw *gsw, u16 vid)
{
	int slot;

	for (slot = 0; slot < GSW_NUM_VLANS; slot++) {
		if (gsw->vlans[slot].valid)
			continue;

		gsw->vlans[slot].valid = true;
		gsw->vlans[slot].vid = vid;
		mt7620_gsw_vlan_set_vid_locked(gsw, slot, vid);

		return slot;
	}

	return -ENOSPC;
}

static int mt7620_gsw_fdb_cmd_match_locked(struct mt7620_gsw *gsw, u8 cmd,
					   u8 match, u32 *response)
{
	u32 val;
	int ret;

	val = MT7620_ATC_BUSY | FIELD_PREP(MT7620_ATC_MATCH, match) | cmd;
	mtk_switch_w32(gsw, val, MT7620_GSW_ATC);

	ret = mt7620_gsw_poll_busy(gsw, MT7620_GSW_ATC, MT7620_ATC_BUSY);
	if (ret) {
		dev_err(gsw->dev, "FDB command timed out\n");
		return ret;
	}

	val = mtk_switch_r32(gsw, MT7620_GSW_ATC);
	if (cmd == MT7620_FDB_READ && val & MT7620_ATC_INVALID)
		return -EINVAL;

	if (response)
		*response = val;

	return 0;
}

static int mt7620_gsw_fdb_cmd_locked(struct mt7620_gsw *gsw, u8 cmd,
				     u32 *response)
{
	return mt7620_gsw_fdb_cmd_match_locked(gsw, cmd,
					       MT7620_FDB_MATCH_ALL_MAC,
					       response);
}

static void mt7620_gsw_fdb_write_locked(struct mt7620_gsw *gsw, u16 vid,
					u8 port_mask, const u8 *mac,
					u8 aging, u8 status, bool my_mac)
{
	u32 ata1;
	u32 ata2;
	u32 atwd;

	ata1 = mac[0] << 24 | mac[1] << 16 | mac[2] << 8 | mac[3];
	ata2 = mac[4] << 24 | mac[5] << 16 | MT7620_ATA2_IVL |
	       FIELD_PREP(MT7620_ATA2_FID, MT7620_VLAN_FID_BRIDGED) |
	       FIELD_PREP(MT7620_ATA2_VID, vid);
	atwd = FIELD_PREP(MT7620_ATWD_AGE, aging) |
	       FIELD_PREP(MT7620_ATWD_PORT_MAP, port_mask) |
	       FIELD_PREP(MT7620_ATWD_STATUS, status);
	if (my_mac)
		atwd |= MT7620_ATWD_MY_MAC;

	mtk_switch_w32(gsw, ata1, MT7620_GSW_ATA1);
	mtk_switch_w32(gsw, ata2, MT7620_GSW_ATA2);
	mtk_switch_w32(gsw, atwd, MT7620_GSW_ATWD);
}

static void mt7620_gsw_fdb_read_locked(struct mt7620_gsw *gsw,
				       struct mt7620_gsw_fdb *fdb)
{
	u32 tsra1 = mtk_switch_r32(gsw, MT7620_GSW_TSRA1);
	u32 tsra2 = mtk_switch_r32(gsw, MT7620_GSW_TSRA2);
	u32 atrd = mtk_switch_r32(gsw, MT7620_GSW_ATRD);

	fdb->mac[0] = tsra1 >> 24;
	fdb->mac[1] = tsra1 >> 16;
	fdb->mac[2] = tsra1 >> 8;
	fdb->mac[3] = tsra1;
	fdb->mac[4] = tsra2 >> 24;
	fdb->mac[5] = tsra2 >> 16;
	fdb->vid = FIELD_GET(MT7620_ATA2_VID, tsra2);
	fdb->port_mask = FIELD_GET(MT7620_ATWD_PORT_MAP, atrd);
	fdb->noarp = FIELD_GET(MT7620_ATWD_STATUS, atrd) ==
		     MT7620_FDB_STATIC;
}

static void mt7620_gsw_set_port_matrix(struct mt7620_gsw *gsw, int port,
				       u8 matrix)
{
	mt7620_gsw_rmw(gsw, MT7620_GSW_PCR(port), MT7620_PCR_MATRIX,
		       FIELD_PREP(MT7620_PCR_MATRIX, matrix));
}

static bool mt7620_gsw_is_forwarding(struct dsa_port *dp, int port, u8 state)
{
	if (port >= 0 && dp->index == (unsigned int)port)
		return state == BR_STATE_FORWARDING;

	return dp->stp_state == BR_STATE_FORWARDING;
}

static void mt7620_gsw_update_matrices(struct dsa_switch *ds, int port,
				       u8 state)
{
	struct mt7620_gsw *gsw = ds->priv;
	struct dsa_port *dp, *other_dp;

	dsa_switch_for_each_user_port(dp, ds) {
		u8 matrix = BIT(MT7620_DSA_CPU_PORT);

		if (mt7620_gsw_is_forwarding(dp, port, state)) {
			dsa_switch_for_each_user_port(other_dp, ds) {
				if (dp == other_dp)
					continue;
				if (!mt7620_gsw_is_forwarding(other_dp, port,
							      state))
					continue;
				if (dsa_port_bridge_same(dp, other_dp))
					matrix |= BIT(other_dp->index);
			}
		}

		mt7620_gsw_set_port_matrix(gsw, dp->index, matrix);
	}
}

static void mt7620_gsw_recalc_matrices(struct dsa_switch *ds)
{
	mt7620_gsw_update_matrices(ds, -1, 0);
}

static int mt7620_gsw_setup(struct dsa_switch *ds)
{
	struct mt7620_gsw *gsw = ds->priv;
	int port, slot;

	ds->mtu_enforcement_ingress = true;
	ds->ageing_time_min = 1000;
	ds->ageing_time_max = 256 * 4096 * 1000;

	for (port = 0; port < ds->num_ports; port++) {
		if (dsa_is_user_port(ds, port))
			WRITE_ONCE(gsw->port_mtu[port], ETH_DATA_LEN);
		mt7620_gsw_rmw(gsw, MT7620_GSW_PCR(port),
			       MT7620_PCR_MATRIX | MT7620_PCR_PORT_VLAN, 0);
		mt7620_gsw_rmw(gsw, MT7620_GSW_PSC(port),
			       MT7620_PSC_SA_DIS, MT7620_PSC_SA_DIS);
		mt7620_gsw_rmw(gsw, MT7620_GSW_PVC(port),
			       MT7620_PVC_EG_TAG | MT7620_PVC_VLAN_ATTR |
			       MT7620_PVC_ACCEPT_FRAME |
			       MT7620_PVC_PORT_SPEC_TAG,
			       FIELD_PREP(MT7620_PVC_EG_TAG,
					  MT7620_PVC_EG_TAG_CONSISTENT) |
			       FIELD_PREP(MT7620_PVC_VLAN_ATTR,
					  MT7620_PVC_VLAN_TRANSPARENT) |
			       FIELD_PREP(MT7620_PVC_ACCEPT_FRAME,
					  MT7620_PVC_ACCEPT_ALL));
		mt7620_gsw_rmw(gsw, MT7620_GSW_PPBV1(port),
			       MT7620_PPBV1_PVID, 0);
	}

	mt7620_gsw_set_port_matrix(gsw, MT7620_DSA_CPU_PORT,
				   MT7620_DSA_USER_PORTS);
	mt7620_gsw_rmw(gsw, MT7620_GSW_PSC(MT7620_DSA_CPU_PORT),
		       MT7620_PSC_SA_DIS, 0);
	mt7620_gsw_rmw(gsw, MT7620_GSW_MFC,
		       MT7620_MFC_BC_FFP | MT7620_MFC_UNM_FFP |
		       MT7620_MFC_UNU_FFP | MT7620_MFC_MIRROR_ENABLE |
		       MT7620_MFC_MIRROR_DEST,
		       FIELD_PREP(MT7620_MFC_BC_FFP,
				  BIT(MT7620_DSA_CPU_PORT)) |
		       FIELD_PREP(MT7620_MFC_UNM_FFP,
				  BIT(MT7620_DSA_CPU_PORT)) |
		       FIELD_PREP(MT7620_MFC_UNU_FFP,
				  BIT(MT7620_DSA_CPU_PORT)));

	mutex_lock(&gsw->reg_mutex);
	memset(gsw->pvid, 0, sizeof(gsw->pvid));
	memset(gsw->vlans, 0, sizeof(gsw->vlans));
	gsw->mirror_rx = 0;
	gsw->mirror_tx = 0;
	gsw->mirror_port = 0;
	for (slot = 0; slot < GSW_NUM_VLANS; slot++) {
		mt7620_gsw_vlan_set_vid_locked(gsw, slot, 0);
		if (mt7620_gsw_vlan_write_locked(gsw, slot)) {
			mutex_unlock(&gsw->reg_mutex);
			return -ETIMEDOUT;
		}
	}
	if (mt7620_gsw_fdb_cmd_locked(gsw, MT7620_FDB_FLUSH, NULL)) {
		mutex_unlock(&gsw->reg_mutex);
		return -ETIMEDOUT;
	}
	mutex_unlock(&gsw->reg_mutex);

	memset(gsw->mib_last, 0, sizeof(gsw->mib_last));
	memset(gsw->mib_stats, 0, sizeof(gsw->mib_stats));
	gsw->mib_initialized = false;
	gsw->mib_active_ports = 0;
	mt7620_gsw_mib_update(gsw);

	return 0;
}

static void mt7620_gsw_teardown(struct dsa_switch *ds)
{
	struct mt7620_gsw *gsw = ds->priv;

	cancel_delayed_work_sync(&gsw->mib_work);
}

static enum dsa_tag_protocol
mt7620_gsw_get_tag_protocol(struct dsa_switch *ds, int port,
			    enum dsa_tag_protocol mp)
{
	return DSA_TAG_PROTO_RALINK;
}

static int mt7620_gsw_port_enable(struct dsa_switch *ds, int port,
				  struct phy_device *phy)
{
	struct mt7620_gsw *gsw = ds->priv;
	bool start_mib;

	if (dsa_is_user_port(ds, port)) {
		mt7620_gsw_set_port_matrix(gsw, port,
					   BIT(MT7620_DSA_CPU_PORT));
		start_mib = !READ_ONCE(gsw->mib_active_ports);
		set_bit(port, &gsw->mib_active_ports);
		if (start_mib)
			schedule_delayed_work(&gsw->mib_work,
					      MT7620_MIB_INTERVAL);

		/* dsa_port_enable_rt() starts phylink after this callback. */
		if (phy && phy->irq == PHY_MAC_INTERRUPT)
			mod_delayed_work(system_wq, &gsw->link_sync_work,
					 msecs_to_jiffies(100));
	}

	return 0;
}

static void mt7620_gsw_port_disable(struct dsa_switch *ds, int port)
{
	struct mt7620_gsw *gsw = ds->priv;

	if (dsa_is_user_port(ds, port)) {
		mt7620_gsw_set_port_matrix(gsw, port, 0);
		clear_bit(port, &gsw->mib_active_ports);
		if (!READ_ONCE(gsw->mib_active_ports)) {
			cancel_delayed_work_sync(&gsw->mib_work);
			mt7620_gsw_mib_update(gsw);
		}
	}
}

static int mt7620_gsw_port_max_mtu(struct dsa_switch *ds, int port)
{
	return MT7620_MAX_MTU;
}

static int mt7620_gsw_port_change_mtu(struct dsa_switch *ds, int port,
				      int new_mtu)
{
	struct mt7620_gsw *gsw = ds->priv;

	/*
	 * mt7620_mac_init() configures the switch for its maximum 2 KiB frame
	 * size. PPE GDM2, however, rejects frames above 1518 bytes. Keep TPF
	 * steering enabled only on user ports whose MTU fits that path.
	 */
	if (dsa_is_user_port(ds, port))
		mt7620_gsw_port_set_mtu(gsw, port, new_mtu);

	return 0;
}

static void mt7620_gsw_port_stp_state_set(struct dsa_switch *ds, int port,
					  u8 state)
{
	struct mt7620_gsw *gsw = ds->priv;
	u32 mask = 0;
	u32 val = 0;
	u8 hw_state;
	int fid;

	switch (state) {
	case BR_STATE_DISABLED:
		hw_state = MT7620_STP_DISABLED;
		break;
	case BR_STATE_BLOCKING:
	case BR_STATE_LISTENING:
		hw_state = MT7620_STP_BLOCKING;
		break;
	case BR_STATE_LEARNING:
		hw_state = MT7620_STP_LEARNING;
		break;
	case BR_STATE_FORWARDING:
		hw_state = MT7620_STP_FORWARDING;
		break;
	default:
		return;
	}

	for (fid = 0; fid < 8; fid++) {
		mask |= GENMASK(1, 0) << (fid * 2);
		val |= hw_state << (fid * 2);
	}

	mt7620_gsw_rmw(gsw, MT7620_GSW_SSC(port), mask, val);
	/* DSA updates dp->stp_state only after this callback returns. */
	mt7620_gsw_update_matrices(ds, port, state);
}

static int
mt7620_gsw_port_pre_bridge_flags(struct dsa_switch *ds, int port,
				 struct switchdev_brport_flags flags,
				 struct netlink_ext_ack *extack)
{
	if (flags.mask & ~(BR_LEARNING | BR_FLOOD | BR_MCAST_FLOOD |
			   BR_BCAST_FLOOD)) {
		NL_SET_ERR_MSG_MOD(extack, "Unsupported bridge port flag");
		return -EOPNOTSUPP;
	}

	return 0;
}

static int
mt7620_gsw_port_bridge_flags(struct dsa_switch *ds, int port,
			     struct switchdev_brport_flags flags,
			     struct netlink_ext_ack *extack)
{
	struct mt7620_gsw *gsw = ds->priv;

	if (flags.mask & BR_LEARNING)
		mt7620_gsw_rmw(gsw, MT7620_GSW_PSC(port),
			       MT7620_PSC_SA_DIS,
			       flags.val & BR_LEARNING ? 0 :
			       MT7620_PSC_SA_DIS);

	if (flags.mask & BR_FLOOD)
		mt7620_gsw_rmw(gsw, MT7620_GSW_MFC,
			       FIELD_PREP(MT7620_MFC_UNU_FFP, BIT(port)),
			       flags.val & BR_FLOOD ?
			       FIELD_PREP(MT7620_MFC_UNU_FFP, BIT(port)) : 0);

	if (flags.mask & BR_MCAST_FLOOD)
		mt7620_gsw_rmw(gsw, MT7620_GSW_MFC,
			       FIELD_PREP(MT7620_MFC_UNM_FFP, BIT(port)),
			       flags.val & BR_MCAST_FLOOD ?
			       FIELD_PREP(MT7620_MFC_UNM_FFP, BIT(port)) : 0);

	if (flags.mask & BR_BCAST_FLOOD)
		mt7620_gsw_rmw(gsw, MT7620_GSW_MFC,
			       FIELD_PREP(MT7620_MFC_BC_FFP, BIT(port)),
			       flags.val & BR_BCAST_FLOOD ?
			       FIELD_PREP(MT7620_MFC_BC_FFP, BIT(port)) : 0);

	return 0;
}

static int mt7620_gsw_port_bridge_join(struct dsa_switch *ds, int port,
				       struct dsa_bridge bridge,
				       bool *tx_fwd_offload,
				       struct netlink_ext_ack *extack)
{
	struct mt7620_gsw *gsw = ds->priv;

	mt7620_gsw_rmw(gsw, MT7620_GSW_PCR(port),
		       MT7620_PCR_PORT_VLAN,
		       FIELD_PREP(MT7620_PCR_PORT_VLAN,
				  MT7620_PORT_FALLBACK_MODE));
	mt7620_gsw_recalc_matrices(ds);

	return 0;
}

static void mt7620_gsw_port_bridge_leave(struct dsa_switch *ds, int port,
					 struct dsa_bridge bridge)
{
	struct mt7620_gsw *gsw = ds->priv;

	mt7620_gsw_recalc_matrices(ds);
	mt7620_gsw_rmw(gsw, MT7620_GSW_PCR(port),
		       MT7620_PCR_PORT_VLAN,
		       FIELD_PREP(MT7620_PCR_PORT_VLAN,
				  MT7620_PORT_MATRIX_MODE));
}

static int
mt7620_gsw_port_vlan_filtering(struct dsa_switch *ds, int port,
			       bool vlan_filtering,
			       struct netlink_ext_ack *extack)
{
	struct mt7620_gsw *gsw = ds->priv;
	struct dsa_port *dp;
	bool cpu_vlan_filtering = vlan_filtering;
	u32 pcr_mode;
	u32 pvc;

	dsa_switch_for_each_user_port(dp, ds) {
		if (dp->index == port)
			continue;
		if (dsa_port_is_vlan_filtering(dp)) {
			cpu_vlan_filtering = true;
			break;
		}
	}

	mutex_lock(&gsw->reg_mutex);

	if (vlan_filtering) {
		pcr_mode = MT7620_PORT_SECURITY_MODE;
		pvc = FIELD_PREP(MT7620_PVC_VLAN_ATTR,
				 MT7620_PVC_VLAN_USER) |
		      FIELD_PREP(MT7620_PVC_EG_TAG,
				 MT7620_PVC_EG_TAG_DISABLED) |
		      FIELD_PREP(MT7620_PVC_ACCEPT_FRAME,
				 gsw->pvid[port] ?
				 MT7620_PVC_ACCEPT_ALL :
				 MT7620_PVC_ACCEPT_TAGGED);

		mt7620_gsw_rmw_locked(gsw, MT7620_GSW_PPBV1(port),
				      MT7620_PPBV1_PVID,
				      FIELD_PREP(MT7620_PPBV1_PVID,
						 gsw->pvid[port]));
	} else {
		pcr_mode = dsa_port_bridge_dev_get(dsa_to_port(ds, port)) ?
			   MT7620_PORT_FALLBACK_MODE :
			   MT7620_PORT_MATRIX_MODE;
		pvc = FIELD_PREP(MT7620_PVC_VLAN_ATTR,
				 MT7620_PVC_VLAN_TRANSPARENT) |
		      FIELD_PREP(MT7620_PVC_EG_TAG,
				 MT7620_PVC_EG_TAG_CONSISTENT) |
		      FIELD_PREP(MT7620_PVC_ACCEPT_FRAME,
				 MT7620_PVC_ACCEPT_ALL);

		mt7620_gsw_rmw_locked(gsw, MT7620_GSW_PPBV1(port),
				      MT7620_PPBV1_PVID, 0);
	}

	mt7620_gsw_rmw_locked(gsw, MT7620_GSW_PCR(port),
			      MT7620_PCR_PORT_VLAN,
			      FIELD_PREP(MT7620_PCR_PORT_VLAN, pcr_mode));
	mt7620_gsw_rmw_locked(gsw, MT7620_GSW_PVC(port),
			      MT7620_PVC_VLAN_ATTR | MT7620_PVC_EG_TAG |
			      MT7620_PVC_ACCEPT_FRAME, pvc);
	mt7620_gsw_rmw_locked(gsw, MT7620_GSW_PVC(MT7620_DSA_CPU_PORT),
			      MT7620_PVC_VLAN_ATTR,
			      FIELD_PREP(MT7620_PVC_VLAN_ATTR,
					 cpu_vlan_filtering ?
					 MT7620_PVC_VLAN_USER :
					 MT7620_PVC_VLAN_TRANSPARENT));

	mutex_unlock(&gsw->reg_mutex);

	return 0;
}

static int
mt7620_gsw_port_vlan_add(struct dsa_switch *ds, int port,
			 const struct switchdev_obj_port_vlan *vlan,
			 struct netlink_ext_ack *extack)
{
	struct mt7620_gsw *gsw = ds->priv;
	struct mt7620_gsw_vlan old_vlan;
	bool allocated = false;
	int slot;
	int ret;

	if (!vlan->vid || vlan->vid >= VLAN_VID_MASK)
		return -EINVAL;

	mutex_lock(&gsw->reg_mutex);

	slot = mt7620_gsw_vlan_find_locked(gsw, vlan->vid);
	if (slot < 0) {
		slot = mt7620_gsw_vlan_alloc_locked(gsw, vlan->vid);
		if (slot < 0) {
			NL_SET_ERR_MSG_MOD(extack,
					   "MT7620 has only 16 VLAN table entries");
			ret = slot;
			goto out;
		}
		allocated = true;
	}

	old_vlan = gsw->vlans[slot];
	gsw->vlans[slot].members |= BIT(port);
	if (dsa_is_cpu_port(ds, port))
		gsw->vlans[slot].untagged &= ~BIT(port);
	else if (vlan->flags & BRIDGE_VLAN_INFO_UNTAGGED)
		gsw->vlans[slot].untagged |= BIT(port);
	else
		gsw->vlans[slot].untagged &= ~BIT(port);

	ret = mt7620_gsw_vlan_write_locked(gsw, slot);
	if (ret) {
		if (allocated) {
			memset(&gsw->vlans[slot], 0,
			       sizeof(gsw->vlans[slot]));
			mt7620_gsw_vlan_set_vid_locked(gsw, slot, 0);
		} else {
			gsw->vlans[slot] = old_vlan;
		}
		goto out;
	}
	if (allocated && gsw->conduit) {
		ret = vlan_vid_add(gsw->conduit, htons(ETH_P_8021Q),
				   vlan->vid);
		if (ret) {
			memset(&gsw->vlans[slot], 0,
			       sizeof(gsw->vlans[slot]));
			mt7620_gsw_vlan_write_locked(gsw, slot);
			mt7620_gsw_vlan_set_vid_locked(gsw, slot, 0);
			goto out;
		}
	}

	if (vlan->flags & BRIDGE_VLAN_INFO_PVID) {
		gsw->pvid[port] = vlan->vid;
		mt7620_gsw_rmw_locked(gsw, MT7620_GSW_PVC(port),
				      MT7620_PVC_ACCEPT_FRAME,
				      FIELD_PREP(MT7620_PVC_ACCEPT_FRAME,
						 MT7620_PVC_ACCEPT_ALL));

		if (dsa_port_is_vlan_filtering(dsa_to_port(ds, port)))
			mt7620_gsw_rmw_locked(gsw, MT7620_GSW_PPBV1(port),
					      MT7620_PPBV1_PVID,
					      FIELD_PREP(MT7620_PPBV1_PVID,
							 vlan->vid));
	} else if (vlan->vid && gsw->pvid[port] == vlan->vid) {
		gsw->pvid[port] = 0;

		if (dsa_port_is_vlan_filtering(dsa_to_port(ds, port)))
			mt7620_gsw_rmw_locked(gsw, MT7620_GSW_PVC(port),
					      MT7620_PVC_ACCEPT_FRAME,
					      FIELD_PREP(MT7620_PVC_ACCEPT_FRAME,
							 MT7620_PVC_ACCEPT_TAGGED));

		mt7620_gsw_rmw_locked(gsw, MT7620_GSW_PPBV1(port),
				      MT7620_PPBV1_PVID, 0);
	}

out:
	mutex_unlock(&gsw->reg_mutex);

	return ret;
}

static int
mt7620_gsw_port_vlan_del(struct dsa_switch *ds, int port,
			 const struct switchdev_obj_port_vlan *vlan)
{
	struct mt7620_gsw *gsw = ds->priv;
	struct mt7620_gsw_vlan old_vlan;
	int slot;
	int ret = 0;

	mutex_lock(&gsw->reg_mutex);

	slot = mt7620_gsw_vlan_find_locked(gsw, vlan->vid);
	if (slot < 0)
		goto out;

	old_vlan = gsw->vlans[slot];
	gsw->vlans[slot].members &= ~BIT(port);
	gsw->vlans[slot].untagged &= ~BIT(port);
	if (!gsw->vlans[slot].members)
		gsw->vlans[slot].valid = false;

	ret = mt7620_gsw_vlan_write_locked(gsw, slot);
	if (ret) {
		gsw->vlans[slot] = old_vlan;
		goto out;
	}

	if (!gsw->vlans[slot].valid) {
		if (gsw->conduit)
			vlan_vid_del(gsw->conduit, htons(ETH_P_8021Q), vlan->vid);
		gsw->vlans[slot].vid = 0;
		mt7620_gsw_vlan_set_vid_locked(gsw, slot, 0);
	}

	if (gsw->pvid[port] == vlan->vid) {
		gsw->pvid[port] = 0;

		if (dsa_port_is_vlan_filtering(dsa_to_port(ds, port)))
			mt7620_gsw_rmw_locked(gsw, MT7620_GSW_PVC(port),
					      MT7620_PVC_ACCEPT_FRAME,
					      FIELD_PREP(MT7620_PVC_ACCEPT_FRAME,
							 MT7620_PVC_ACCEPT_TAGGED));

		mt7620_gsw_rmw_locked(gsw, MT7620_GSW_PPBV1(port),
				      MT7620_PPBV1_PVID, 0);
	}

out:
	mutex_unlock(&gsw->reg_mutex);

	return ret;
}

static int
mt7620_gsw_port_fdb_add(struct dsa_switch *ds, int port,
			const unsigned char *addr, u16 vid,
			struct dsa_db db)
{
	struct mt7620_gsw *gsw = ds->priv;
	u32 response;
	int ret;

	mutex_lock(&gsw->reg_mutex);
	/* Host FDB entries need this attribute for TPF MY_MAC steering. */
	mt7620_gsw_fdb_write_locked(gsw, vid, BIT(port), addr, 0xff,
				    MT7620_FDB_STATIC,
				    port == MT7620_DSA_CPU_PORT);
	ret = mt7620_gsw_fdb_cmd_locked(gsw, MT7620_FDB_WRITE, &response);
	if (!ret && response & MT7620_ATC_INVALID)
		ret = -ENOSPC;
	mutex_unlock(&gsw->reg_mutex);

	return ret;
}

static int
mt7620_gsw_port_fdb_del(struct dsa_switch *ds, int port,
			const unsigned char *addr, u16 vid,
			struct dsa_db db)
{
	struct mt7620_gsw *gsw = ds->priv;
	int ret;

	mutex_lock(&gsw->reg_mutex);
	mt7620_gsw_fdb_write_locked(gsw, vid, BIT(port), addr, 0,
				    MT7620_FDB_EMPTY, false);
	ret = mt7620_gsw_fdb_cmd_locked(gsw, MT7620_FDB_WRITE, NULL);
	mutex_unlock(&gsw->reg_mutex);

	return ret;
}

static int
mt7620_gsw_port_fdb_dump(struct dsa_switch *ds, int port,
			 dsa_fdb_dump_cb_t *cb, void *data)
{
	struct mt7620_gsw_fdb fdb = {};
	struct mt7620_gsw *gsw = ds->priv;
	int count = MT7620_FDB_ENTRIES;
	u32 response;
	int ret;

	mutex_lock(&gsw->reg_mutex);

	ret = mt7620_gsw_fdb_cmd_locked(gsw, MT7620_FDB_START, &response);
	if (ret)
		goto out;

	do {
		if (response & MT7620_ATC_SEARCH_HIT) {
			mt7620_gsw_fdb_read_locked(gsw, &fdb);
			if (fdb.port_mask & BIT(port)) {
				ret = cb(fdb.mac, fdb.vid, fdb.noarp, data);
				if (ret)
					break;
			}
		}

		if (response & MT7620_ATC_SEARCH_END)
			break;

		ret = mt7620_gsw_fdb_cmd_locked(gsw, MT7620_FDB_NEXT,
						&response);
	} while (!ret && --count);

out:
	mutex_unlock(&gsw->reg_mutex);

	return ret;
}

static void mt7620_gsw_port_fast_age(struct dsa_switch *ds, int port)
{
	struct mt7620_gsw *gsw = ds->priv;

	/*
	 * The switch cannot combine its source-port match with its dynamic-MAC
	 * match. Flush all dynamic MAC entries to preserve static entries.
	 */
	mutex_lock(&gsw->reg_mutex);
	mt7620_gsw_fdb_cmd_match_locked(gsw, MT7620_FDB_FLUSH,
					MT7620_FDB_MATCH_DYNAMIC_MAC, NULL);
	mutex_unlock(&gsw->reg_mutex);
}

static int
mt7620_gsw_port_mdb_add(struct dsa_switch *ds, int port,
			const struct switchdev_obj_port_mdb *mdb,
			struct dsa_db db)
{
	struct mt7620_gsw_fdb fdb = {};
	struct mt7620_gsw *gsw = ds->priv;
	u8 port_mask = 0;
	u32 response;
	int ret;

	mutex_lock(&gsw->reg_mutex);

	mt7620_gsw_fdb_write_locked(gsw, mdb->vid, 0, mdb->addr, 0,
				    MT7620_FDB_EMPTY, false);
	if (!mt7620_gsw_fdb_cmd_locked(gsw, MT7620_FDB_READ, NULL)) {
		mt7620_gsw_fdb_read_locked(gsw, &fdb);
		port_mask = fdb.port_mask;
	}

	port_mask |= BIT(port);
	mt7620_gsw_fdb_write_locked(gsw, mdb->vid, port_mask, mdb->addr,
				    0xff, MT7620_FDB_STATIC, false);
	ret = mt7620_gsw_fdb_cmd_locked(gsw, MT7620_FDB_WRITE, &response);
	if (!ret && response & MT7620_ATC_INVALID)
		ret = -ENOSPC;

	mutex_unlock(&gsw->reg_mutex);

	return ret;
}

static int
mt7620_gsw_port_mdb_del(struct dsa_switch *ds, int port,
			const struct switchdev_obj_port_mdb *mdb,
			struct dsa_db db)
{
	struct mt7620_gsw_fdb fdb = {};
	struct mt7620_gsw *gsw = ds->priv;
	u8 port_mask = 0;
	int ret;

	mutex_lock(&gsw->reg_mutex);

	mt7620_gsw_fdb_write_locked(gsw, mdb->vid, 0, mdb->addr, 0,
				    MT7620_FDB_EMPTY, false);
	if (!mt7620_gsw_fdb_cmd_locked(gsw, MT7620_FDB_READ, NULL)) {
		mt7620_gsw_fdb_read_locked(gsw, &fdb);
		port_mask = fdb.port_mask;
	}

	port_mask &= ~BIT(port);
	mt7620_gsw_fdb_write_locked(gsw, mdb->vid, port_mask, mdb->addr,
				    0xff, port_mask ? MT7620_FDB_STATIC :
				    MT7620_FDB_EMPTY, false);
	ret = mt7620_gsw_fdb_cmd_locked(gsw, MT7620_FDB_WRITE, NULL);

	mutex_unlock(&gsw->reg_mutex);

	return ret;
}

static int mt7620_gsw_set_ageing_time(struct dsa_switch *ds,
				      unsigned int msecs)
{
	struct mt7620_gsw *gsw = ds->priv;
	unsigned int secs = DIV_ROUND_CLOSEST(msecs, 1000);
	unsigned int best_error = UINT_MAX;
	unsigned int age_count = 0;
	unsigned int age_unit = 0;
	unsigned int count;

	/*
	 * The hardware period is (AGE_CNT + 1) * (AGE_UNIT + 1) seconds.
	 * Find the closest representation across the two register fields.
	 */
	for (count = 0; count <= MT7620_AAC_AGE_CNT_MAX; count++) {
		unsigned int duration;
		unsigned int error;
		unsigned int unit;

		unit = DIV_ROUND_CLOSEST(secs, count + 1);
		if (!unit || unit > MT7620_AAC_AGE_UNIT_MAX + 1)
			continue;

		duration = (count + 1) * unit;
		error = abs_diff(secs, duration);
		if (error < best_error) {
			best_error = error;
			age_count = count;
			age_unit = unit - 1;
		}

		if (!error)
			break;
	}

	mt7620_gsw_rmw(gsw, MT7620_GSW_AAC,
		       MT7620_AAC_AGE_CNT | MT7620_AAC_AGE_UNIT,
		       FIELD_PREP(MT7620_AAC_AGE_CNT, age_count) |
		       FIELD_PREP(MT7620_AAC_AGE_UNIT, age_unit));

	return 0;
}

static void mt7620_gsw_get_strings(struct dsa_switch *ds, int port,
				   u32 stringset, u8 *data)
{
	int i;

	if (stringset != ETH_SS_STATS)
		return;

	for (i = 0; i < ARRAY_SIZE(mt7620_gsw_mibs); i++)
		ethtool_puts(&data, mt7620_gsw_mibs[i].name);
}

static int mt7620_gsw_get_sset_count(struct dsa_switch *ds, int port,
				     int stringset)
{
	if (stringset != ETH_SS_STATS)
		return 0;

	return ARRAY_SIZE(mt7620_gsw_mibs);
}

static void mt7620_gsw_get_ethtool_stats(struct dsa_switch *ds, int port,
					 u64 *data)
{
	struct mt7620_gsw *gsw = ds->priv;

	spin_lock_bh(&gsw->mib_lock);
	memcpy(data, gsw->mib_stats[port], sizeof(gsw->mib_stats[port]));
	spin_unlock_bh(&gsw->mib_lock);
}

static void mt7620_gsw_get_stats64(struct dsa_switch *ds, int port,
				   struct rtnl_link_stats64 *stats)
{
	struct mt7620_gsw *gsw = ds->priv;

	spin_lock_bh(&gsw->mib_lock);
	stats->tx_packets = gsw->mib_stats[port][MT7620_MIB_TX_GOOD_PACKETS];
	stats->tx_bytes = gsw->mib_stats[port][MT7620_MIB_TX_BYTES] -
			  stats->tx_packets * ETH_FCS_LEN;
	stats->tx_errors = gsw->mib_stats[port][MT7620_MIB_TX_BAD_PACKETS];
	stats->tx_dropped = gsw->mib_stats[port][MT7620_MIB_TX_DROPPED];
	stats->rx_packets = gsw->mib_stats[port][MT7620_MIB_RX_GOOD_PACKETS];
	stats->rx_bytes = gsw->mib_stats[port][MT7620_MIB_RX_BYTES] -
			  stats->rx_packets * ETH_FCS_LEN;
	stats->rx_errors = gsw->mib_stats[port][MT7620_MIB_RX_BAD_PACKETS];
	stats->rx_dropped =
		gsw->mib_stats[port][MT7620_MIB_RX_CTRL_DROPPED] +
		gsw->mib_stats[port][MT7620_MIB_RX_INGRESS_DROPPED] +
		gsw->mib_stats[port][MT7620_MIB_RX_ARL_DROPPED] +
		gsw->mib_stats[port][MT7620_MIB_RX_FILTERED];
	spin_unlock_bh(&gsw->mib_lock);
}

static void mt7620_gsw_mac_config(struct phylink_config *config,
				  unsigned int mode,
				  const struct phylink_link_state *state)
{
}

static void mt7620_gsw_mac_link_down(struct phylink_config *config,
				     unsigned int mode,
				     phy_interface_t interface)
{
	struct dsa_port *dp = dsa_phylink_to_port(config);
	struct mt7620_gsw *gsw = dp->ds->priv;

	if (dsa_is_cpu_port(dp->ds, dp->index))
		return;

	mt7620_gsw_rmw(gsw, GSW_REG_PORT_PMCR(dp->index),
		       PMCR_FORCE | PMCR_LINK, 0);
}

static void mt7620_gsw_mac_link_up(struct phylink_config *config,
				   struct phy_device *phydev,
				   unsigned int mode,
				   phy_interface_t interface,
				   int speed, int duplex,
				   bool tx_pause, bool rx_pause)
{
	struct dsa_port *dp = dsa_phylink_to_port(config);
	struct mt7620_gsw *gsw = dp->ds->priv;
	u32 val;

	if (dsa_is_cpu_port(dp->ds, dp->index))
		return;

	/* Embedded PHY status, including speed and pause, comes from PPSC. */
	val = PMCR_IPG | PMCR_MAC_MODE | PMCR_TX_EN | PMCR_RX_EN |
	      PMCR_BACKOFF | PMCR_BACKPRES;

	mtk_switch_w32(gsw, val, GSW_REG_PORT_PMCR(dp->index));
}

static const struct phylink_mac_ops mt7620_gsw_phylink_mac_ops = {
	.mac_config	= mt7620_gsw_mac_config,
	.mac_link_down	= mt7620_gsw_mac_link_down,
	.mac_link_up	= mt7620_gsw_mac_link_up,
};

static void mt7620_gsw_phylink_get_caps(struct dsa_switch *ds, int port,
					struct phylink_config *config)
{
	bitmap_zero(config->supported_interfaces, PHY_INTERFACE_MODE_MAX);
	config->mac_capabilities = MAC_SYM_PAUSE | MAC_ASYM_PAUSE;

	/*
	 * Port 5 is an external MAC whose pinmux and PHY mode are
	 * board-specific. It matches neither case below and is therefore
	 * left without a supported interface until a DSA device tree
	 * describes it.
	 */
	if (port >= 0 && BIT(port) & MT7620_DSA_USER_PORTS) {
		__set_bit(PHY_INTERFACE_MODE_MII,
			  config->supported_interfaces);
		config->mac_capabilities |= MAC_10HD | MAC_10FD |
					    MAC_100HD | MAC_100FD;
	} else if (port == MT7620_DSA_CPU_PORT) {
		phy_interface_set_rgmii(config->supported_interfaces);
		config->mac_capabilities |= MAC_10HD | MAC_10FD |
					    MAC_100HD | MAC_100FD |
					    MAC_1000FD;
	}
}

static int
mt7620_gsw_port_mirror_add(struct dsa_switch *ds, int port,
			   struct dsa_mall_mirror_tc_entry *mirror,
			   bool ingress, struct netlink_ext_ack *extack)
{
	struct mt7620_gsw *gsw = ds->priv;
	u8 *sources = ingress ? &gsw->mirror_rx : &gsw->mirror_tx;
	u32 mirror_ctrl = ingress ? MT7620_PCR_MIRROR_RX :
				     MT7620_PCR_MIRROR_TX;
	u32 val;

	mutex_lock(&gsw->reg_mutex);
	if (*sources & BIT(port)) {
		mutex_unlock(&gsw->reg_mutex);
		return -EEXIST;
	}

	if ((gsw->mirror_rx || gsw->mirror_tx) &&
	    gsw->mirror_port != mirror->to_local_port) {
		NL_SET_ERR_MSG_MOD(extack,
				   "MT7620 supports only one mirror destination");
		mutex_unlock(&gsw->reg_mutex);
		return -EEXIST;
	}

	gsw->mirror_port = mirror->to_local_port;
	*sources |= BIT(port);
	val = mtk_switch_r32(gsw, MT7620_GSW_MFC);
	val &= ~MT7620_MFC_MIRROR_DEST;
	val |= MT7620_MFC_MIRROR_ENABLE |
	       FIELD_PREP(MT7620_MFC_MIRROR_DEST, gsw->mirror_port);
	mtk_switch_w32(gsw, val, MT7620_GSW_MFC);
	mt7620_gsw_rmw_locked(gsw, MT7620_GSW_PCR(port), mirror_ctrl,
			      mirror_ctrl);
	mutex_unlock(&gsw->reg_mutex);

	return 0;
}

static void
mt7620_gsw_port_mirror_del(struct dsa_switch *ds, int port,
			   struct dsa_mall_mirror_tc_entry *mirror)
{
	struct mt7620_gsw *gsw = ds->priv;
	u8 *sources = mirror->ingress ? &gsw->mirror_rx : &gsw->mirror_tx;
	u32 mirror_ctrl = mirror->ingress ? MT7620_PCR_MIRROR_RX :
					     MT7620_PCR_MIRROR_TX;
	u32 val;

	mutex_lock(&gsw->reg_mutex);
	*sources &= ~BIT(port);
	mt7620_gsw_rmw_locked(gsw, MT7620_GSW_PCR(port), mirror_ctrl, 0);
	if (!gsw->mirror_rx && !gsw->mirror_tx) {
		val = mtk_switch_r32(gsw, MT7620_GSW_MFC);
		val &= ~MT7620_MFC_MIRROR_ENABLE;
		mtk_switch_w32(gsw, val, MT7620_GSW_MFC);
	}
	mutex_unlock(&gsw->reg_mutex);
}

static int
mt7620_gsw_cls_flower(struct dsa_switch *ds, int port,
		      struct flow_cls_offload *cls, bool ingress)
{
	struct dsa_port *dp = dsa_to_port(ds, port);
	struct net_device *conduit;

	if (!ingress || !dp->user)
		return -EOPNOTSUPP;

	conduit = dsa_port_to_conduit(dp);
	return mt7620_ppe_setup_dsa_tc(conduit, dp->user, cls);
}

static const struct dsa_switch_ops mt7620_gsw_dsa_ops = {
	.get_tag_protocol	= mt7620_gsw_get_tag_protocol,
	.setup			= mt7620_gsw_setup,
	.teardown		= mt7620_gsw_teardown,
	.port_enable		= mt7620_gsw_port_enable,
	.port_disable		= mt7620_gsw_port_disable,
	.port_max_mtu		= mt7620_gsw_port_max_mtu,
	.port_change_mtu	= mt7620_gsw_port_change_mtu,
	.port_bridge_join	= mt7620_gsw_port_bridge_join,
	.port_bridge_leave	= mt7620_gsw_port_bridge_leave,
	.port_pre_bridge_flags	= mt7620_gsw_port_pre_bridge_flags,
	.port_bridge_flags	= mt7620_gsw_port_bridge_flags,
	.port_stp_state_set	= mt7620_gsw_port_stp_state_set,
	.port_vlan_filtering	= mt7620_gsw_port_vlan_filtering,
	.port_vlan_add		= mt7620_gsw_port_vlan_add,
	.port_vlan_del		= mt7620_gsw_port_vlan_del,
	.port_fdb_add		= mt7620_gsw_port_fdb_add,
	.port_fdb_del		= mt7620_gsw_port_fdb_del,
	.port_fdb_dump		= mt7620_gsw_port_fdb_dump,
	.port_fast_age		= mt7620_gsw_port_fast_age,
	.port_mdb_add		= mt7620_gsw_port_mdb_add,
	.port_mdb_del		= mt7620_gsw_port_mdb_del,
	.set_ageing_time	= mt7620_gsw_set_ageing_time,
	.get_strings		= mt7620_gsw_get_strings,
	.get_sset_count		= mt7620_gsw_get_sset_count,
	.get_ethtool_stats	= mt7620_gsw_get_ethtool_stats,
	.get_stats64		= mt7620_gsw_get_stats64,
	.port_mirror_add	= mt7620_gsw_port_mirror_add,
	.port_mirror_del	= mt7620_gsw_port_mirror_del,
	.cls_flower_add		= mt7620_gsw_cls_flower,
	.cls_flower_del		= mt7620_gsw_cls_flower,
	.cls_flower_stats	= mt7620_gsw_cls_flower,
	.phylink_get_caps	= mt7620_gsw_phylink_get_caps,
};

static irqreturn_t mt7620_gsw_link_irq(int irq, void *data)
{
	struct mt7620_gsw *gsw = data;
	struct dsa_switch *ds = READ_ONCE(gsw->ds);
	u32 status, link_changed;
	int port;

	status = mtk_switch_r32(gsw, GSW_REG_ISR);
	link_changed = status & PORT_IRQ_ST_CHG;
	if (!link_changed)
		return IRQ_NONE;

	/* Link-change status is write-one-to-clear. */
	mtk_switch_w32(gsw, link_changed, GSW_REG_ISR);

	if (!ds)
		return IRQ_HANDLED;

	for (port = 0; port < MT7620_DSA_NUM_PORTS; port++) {
		struct dsa_port *dp;
		struct phy_device *phydev;

		if (!(link_changed & BIT(port)) ||
		    !dsa_is_user_port(ds, port))
			continue;

		dp = dsa_to_port(ds, port);
		phydev = dp->user->phydev;
		if (phydev)
			phy_mac_interrupt(phydev);
	}

	return IRQ_HANDLED;
}

static void mt7620_gsw_set_phy_irq_mode(struct dsa_switch *ds, int irq)
{
	struct dsa_port *dp;

	dsa_switch_for_each_user_port(dp, ds)
		if (dp->user->phydev)
			dp->user->phydev->irq = irq;
}

static void mt7620_gsw_set_mdio_phy_irq_mode(struct mt7620_gsw *gsw, int irq)
{
	int port;

	/*
	 * The embedded PHY devices exist on the conduit MDIO bus before DSA
	 * attaches its user ports. Select MAC-managed interrupts here so phylib
	 * never starts its polling state machine during DSA registration.
	 */
	if (!gsw->upstream_mii_bus)
		return;

	for (port = 0; port < MT7620_DSA_NUM_EPHYS; port++) {
		struct phy_device *phydev;

		phydev = mdiobus_get_phy(gsw->upstream_mii_bus,
					 gsw->ephy_base + port);
		if (phydev)
			phydev->irq = irq;
	}
}

static bool mt7620_gsw_sync_phy_state(struct dsa_switch *ds)
{
	struct dsa_port *dp;

	/* DSA user-port and netdev state is protected by RTNL. Avoid blocking
	 * teardown, which can cancel this work while holding RTNL.
	 */
	if (!rtnl_trylock())
		return false;

	dsa_switch_for_each_user_port(dp, ds) {
		struct phy_device *phydev = dp->user->phydev;
		int ret;

		if (!phydev || !netif_running(dp->user))
			continue;

		/* Generic PHY resume does not restart MT7620 EPHY autoneg. */
		if (phydev->autoneg == AUTONEG_ENABLE) {
			mutex_lock(&phydev->lock);
			ret = phy_restart_aneg(phydev);
			mutex_unlock(&phydev->lock);
			if (ret)
				netdev_warn(dp->user,
					    "failed to restart PHY autoneg: %d\n",
					    ret);
		}

		phy_mac_interrupt(phydev);
	}

	rtnl_unlock();

	return true;
}

static void mt7620_gsw_link_sync_work(struct work_struct *work)
{
	struct mt7620_gsw *gsw =
		container_of(to_delayed_work(work), struct mt7620_gsw,
			     link_sync_work);
	struct dsa_switch *ds = READ_ONCE(gsw->ds);

	if (ds && !mt7620_gsw_sync_phy_state(ds) &&
	    READ_ONCE(gsw->ds) == ds)
		mod_delayed_work(system_wq, &gsw->link_sync_work,
				 msecs_to_jiffies(20));
}

static int mt7620_gsw_irq_setup(struct platform_device *pdev,
				struct mt7620_gsw *gsw,
				struct dsa_switch *ds)
{
	u32 phy_polling;
	int port;
	int ret;

	ret = devm_request_irq(&pdev->dev, gsw->irq, mt7620_gsw_link_irq, 0,
			       dev_name(&pdev->dev), gsw);
	if (ret)
		return dev_err_probe(&pdev->dev, ret,
				     "failed to request switch IRQ\n");

	/*
	 * Link-change bits are generated from switch port status registers.
	 * Keep them updated after phylib stops polling the PHYs itself.
	 */
	gsw->phy_polling = mtk_switch_r32(gsw, ESW_PHY_POLLING);
	phy_polling = gsw->phy_polling;
	phy_polling &= ~(GENMASK(29, 24) | PHY_END_ADDR | PHY_START_ADDR);
	phy_polling |= PHY_AN_EN | PHY_PRE_EN | PMY_MDC_CONF(5) |
		       PHY_EMB_AN_EN |
		       FIELD_PREP(PHY_END_ADDR,
				  gsw->ephy_base + MT7620_DSA_NUM_EPHYS - 1) |
		       FIELD_PREP(PHY_START_ADDR, gsw->ephy_base);
	mtk_switch_w32(gsw, phy_polling, ESW_PHY_POLLING);
	for (port = 0; port < MT7620_DSA_NUM_PORTS; port++)
		if (dsa_is_user_port(ds, port))
			mt7620_gsw_rmw(gsw, GSW_REG_PORT_PMCR(port),
				       PMCR_FORCE | PMCR_LINK, 0);

	mt7620_gsw_set_phy_irq_mode(ds, PHY_MAC_INTERRUPT);
	WRITE_ONCE(gsw->ds, ds);

	/* Drop stale events before allowing link changes to assert the IRQ. */
	mtk_switch_w32(gsw, PORT_IRQ_ST_CHG, GSW_REG_ISR);
	mtk_switch_w32(gsw, ~PORT_IRQ_ST_CHG, GSW_REG_IMR);

	/* An already-up link may not generate another physical transition. */
	mod_delayed_work(system_wq, &gsw->link_sync_work, 0);

	return 0;
}

static void mt7620_gsw_irq_teardown(struct mt7620_gsw *gsw,
				    struct dsa_switch *ds)
{
	mtk_switch_w32(gsw, ~0, GSW_REG_IMR);
	synchronize_irq(gsw->irq);
	WRITE_ONCE(gsw->ds, NULL);
	cancel_delayed_work_sync(&gsw->link_sync_work);
	mt7620_gsw_set_phy_irq_mode(ds, PHY_POLL);
	mtk_switch_w32(gsw, gsw->phy_polling, ESW_PHY_POLLING);
}

static int mt7620_gsw_dsa_probe(struct platform_device *pdev)
{
	struct mt7620_gsw_platform_data *pdata = dev_get_platdata(&pdev->dev);
	struct mt7620_gsw *gsw;
	struct dsa_switch *ds;
	int ret;

	if (!pdata || !pdata->gsw)
		return -ENODEV;

	gsw = pdata->gsw;

	if (gsw->ds)
		return 0;

	ds = devm_kzalloc(&pdev->dev, sizeof(*ds), GFP_KERNEL);
	if (!ds)
		return -ENOMEM;

	ds->dev = &pdev->dev;
	ds->priv = gsw;
	ds->ops = &mt7620_gsw_dsa_ops;
	ds->phylink_mac_ops = &mt7620_gsw_phylink_mac_ops;
	ds->num_ports = MT7620_DSA_NUM_PORTS;

	spin_lock_init(&gsw->mib_lock);
	INIT_DELAYED_WORK(&gsw->link_sync_work, mt7620_gsw_link_sync_work);
	INIT_DELAYED_WORK(&gsw->mib_work, mt7620_gsw_mib_work);

	mt7620_gsw_set_mdio_phy_irq_mode(gsw, PHY_MAC_INTERRUPT);
	ret = dsa_register_switch(ds);
	if (ret) {
		mt7620_gsw_set_mdio_phy_irq_mode(gsw, PHY_POLL);
		dev_err_probe(gsw->dev, ret, "failed to register DSA switch\n");
		return ret;
	}

	ret = mt7620_gsw_irq_setup(pdev, gsw, ds);
	if (ret) {
		dsa_unregister_switch(ds);
		mt7620_gsw_set_mdio_phy_irq_mode(gsw, PHY_POLL);
		return ret;
	}

	return 0;
}

static void mt7620_gsw_dsa_remove(struct platform_device *pdev)
{
	struct mt7620_gsw_platform_data *pdata = dev_get_platdata(&pdev->dev);
	struct mt7620_gsw *gsw = pdata->gsw;
	struct dsa_switch *ds = gsw->ds;

	if (!ds)
		return;

	mt7620_gsw_irq_teardown(gsw, ds);
	dsa_unregister_switch(ds);
}

static struct platform_driver mt7620_gsw_dsa_driver = {
	.probe = mt7620_gsw_dsa_probe,
	.remove = mt7620_gsw_dsa_remove,
	.driver = {
		.name = "mt7620-dsa",
	},
};
module_platform_driver(mt7620_gsw_dsa_driver);
MODULE_ALIAS("platform:mt7620-dsa");
MODULE_DESCRIPTION("DSA driver for the MediaTek MT7620 integrated switch");
MODULE_LICENSE("GPL");
