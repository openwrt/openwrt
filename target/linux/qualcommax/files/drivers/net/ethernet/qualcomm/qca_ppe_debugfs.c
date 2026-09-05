// SPDX-License-Identifier: GPL-2.0-or-later OR MIT
/* Copyright (c) 2026 Julius Bairaktaris <julius@bairaktaris.de> */
/* Read-only dumps of the PPE's hardware counters.
 *
 * The driver switches most of these on at probe and reads none of them back,
 * so every question about where a frame stopped has so far been answered with
 * devmem. Nothing here writes hardware, not even the tables the hardware lets
 * a host clear.
 */

#include <linux/bitfield.h>
#include <linux/debugfs.h>
#include <linux/regmap.h>
#include <linux/seq_file.h>

#include "qca_ppe.h"

/* Every packet-and-byte counter table below is three words wide. */
#define PPE_CNT_WORDS		3

/* The counter tables share one shape: a 32-bit packet count followed by a
 * 40-bit byte count, both straddling word boundaries.
 */
static void ppe_cnt_get(const u32 *w, u32 off, u64 *pkts, u64 *bytes)
{
	*pkts = ppe_entry_get(w, off, 32);
	*bytes = ppe_entry_get(w, off + 32, 40);
}

/* Frames that reached the L3 stage, per ingress L3 interface. This is what
 * separates "the flow entry never matched" from "the frame never got as far as
 * the lookup", which no other counter answers.
 */
static int ppe_l3_interface_show(struct seq_file *s, void *data)
{
	struct qca_ppe_priv *priv = s->private;
	u64 pkts, bytes, drop_pkts, drop_bytes;
	u32 w[PPE_CNT_DROP_WORDS];
	u32 i;

	seq_puts(s, "l3_if packets bytes drop_packets drop_bytes\n");

	for (i = 0; i < PPE_RT_IF_CNT_ENTRIES; i++) {
		if (regmap_bulk_read(priv->regmap, PPE_RT_IF_CNT_TBL(i), w,
				     ARRAY_SIZE(w)))
			return -EIO;

		ppe_cnt_get(w, 0, &pkts, &bytes);
		ppe_cnt_get(w, PPE_CNT_DROP_OFF, &drop_pkts,
			    &drop_bytes);
		if (!pkts && !drop_pkts)
			continue;

		seq_printf(s, "%-5u %llu %llu %llu %llu\n", i, pkts, bytes,
			   drop_pkts, drop_bytes);
	}

	return 0;
}
DEFINE_SHOW_ATTRIBUTE(ppe_l3_interface);

/* The ingress policer's own view of what it did to a port's traffic. Red is the
 * colour a token-bucket overrun is painted and the packets counted there are
 * the ones it dropped, which until now was inferred from the throughput that
 * came out the other side.
 */
static int ppe_policer_show(struct seq_file *s, void *data)
{
	static const char * const colour[] = { "green", "yellow", "red" };
	struct qca_ppe_priv *priv = s->private;
	u32 w[PPE_CNT_WORDS];
	u64 pkts, bytes;
	int port, c;

	seq_puts(s, "port colour packets bytes\n");

	for (port = 0; port < priv->ds.num_ports; port++) {
		for (c = 0; c < ARRAY_SIZE(colour); c++) {
			if (regmap_bulk_read(priv->regmap,
					     PPE_PORT_METER_CNT(port, c), w,
					     ARRAY_SIZE(w)))
				return -EIO;

			ppe_cnt_get(w, 0, &pkts, &bytes);
			seq_printf(s, "%-4d %-6s %llu %llu\n", port,
				   colour[c], pkts, bytes);
		}
	}

	return 0;
}
DEFINE_SHOW_ATTRIBUTE(ppe_policer);

/* Why frames left the datapath. A CPU code says which exception handed the
 * frame to the host; a drop code says which stage discarded it, and is the
 * only counter that names the stage.
 */
static int ppe_drop_code_show(struct seq_file *s, void *data)
{
	struct qca_ppe_priv *priv = s->private;
	u32 w[PPE_CNT_WORDS];
	u64 pkts, bytes;
	u32 i;

	seq_puts(s, "kind code port packets bytes\n");

	for (i = 0; i < PPE_DROP_CPU_ENTRIES; i++) {
		if (regmap_bulk_read(priv->regmap, PPE_DROP_CPU_CNT_TBL(i), w,
				     ARRAY_SIZE(w)))
			return -EIO;

		ppe_cnt_get(w, 0, &pkts, &bytes);
		if (!pkts)
			continue;

		if (i < PPE_CPU_CODE_ENTRIES)
			seq_printf(s, "cpu  %-4u -    %llu %llu\n", i, pkts,
				   bytes);
		else
			seq_printf(s, "drop %-4u %-4u %llu %llu\n",
				   (i - PPE_CPU_CODE_ENTRIES) /
				   PPE_DROP_CODE_PORTS,
				   (i - PPE_CPU_CODE_ENTRIES) %
				   PPE_DROP_CODE_PORTS, pkts, bytes);
	}

	return 0;
}
DEFINE_SHOW_ATTRIBUTE(ppe_drop_code);

/* What each egress queue has transmitted and what it is holding right now.
 * Depth in buffers is the direct readout of standing queue occupancy, which
 * has so far only been inferred from the latency it causes.
 */
static int ppe_queues_show(struct seq_file *s, void *data)
{
	struct qca_ppe_priv *priv = s->private;
	u32 w[PPE_CNT_WORDS], val;
	u64 pkts, bytes;
	u32 i;

	seq_puts(s, "queue packets bytes buffers\n");

	for (i = 0; i < PPE_L0_QUEUES; i++) {
		if (regmap_bulk_read(priv->regmap, PPE_QUEUE_TX_CNT_TBL(i), w,
				     ARRAY_SIZE(w)))
			return -EIO;

		if (i < PPE_L0_UCAST_QUEUES) {
			regmap_read(priv->regmap, PPE_QM_AC_UNI_CNT(i), &val);
			val = FIELD_GET(PPE_AC_UNI_PEND_CNT, val);
		} else {
			regmap_read(priv->regmap,
				    PPE_QM_AC_MUL_CNT(i - PPE_L0_UCAST_QUEUES),
				    &val);
			val = FIELD_GET(PPE_AC_MUL_PEND_CNT, val);
		}

		ppe_cnt_get(w, 0, &pkts, &bytes);
		if (!pkts && !val)
			continue;

		seq_printf(s, "%-5u %llu %llu %u\n", i, pkts, bytes, val);
	}

	/* One buffer group per value the queues' group field can hold. */
	seq_puts(s, "group buffers prealloc_used\n");
	for (i = 0; i <= FIELD_MAX(PPE_AC_GRP_ID); i++) {
		regmap_read(priv->regmap, PPE_QM_AC_GRP_CNT(i), &val);
		seq_printf(s, "%-5u %lu %lu\n", i,
			   FIELD_GET(PPE_AC_GRP_PEND_CNT, val),
			   FIELD_GET(PPE_AC_GRP_ALLOC_USED, val));
	}

	return 0;
}
DEFINE_SHOW_ATTRIBUTE(ppe_queues);

/* Why a queue discarded a frame, split the way the hardware counts it: WRED
 * against the queue's own limit, or a forced drop when the buffer pool behind
 * it ran out, each per colour.
 */
static int ppe_queue_drops_show(struct seq_file *s, void *data)
{
	static const char * const reason[PPE_UNI_DROP_TYPES] = {
		"wred_green", "wred_yellow", "wred_red",
		"force_green", "force_yellow", "force_red",
	};
	const u32 force = PPE_UNI_DROP_TYPES - PPE_MUL_DROP_TYPES;
	struct qca_ppe_priv *priv = s->private;
	u32 w[PPE_CNT_WORDS];
	u64 pkts, bytes;
	u32 i, t, reg;

	seq_puts(s, "queue reason packets bytes\n");

	for (i = 0; i < PPE_L0_QUEUES; i++) {
		for (t = 0; t < PPE_UNI_DROP_TYPES; t++) {
			if (i < PPE_L0_UCAST_QUEUES) {
				reg = PPE_QM_UNI_DROP_CNT(i, t);
			} else {
				u32 mq = i - PPE_L0_UCAST_QUEUES;
				u32 port = 0;

				/* A multicast queue only has the forced
				 * drops, in a block per port.
				 */
				if (t < force)
					continue;

				if (mq >= PPE_MUL_QUEUES_CPU) {
					mq -= PPE_MUL_QUEUES_CPU;
					port = 1 + mq / PPE_MUL_QUEUES_PORT;
					mq %= PPE_MUL_QUEUES_PORT;
				}

				reg = PPE_QM_MUL_DROP_CNT(port, mq, t - force);
			}

			if (regmap_bulk_read(priv->regmap, reg, w,
					     ARRAY_SIZE(w)))
				return -EIO;

			ppe_cnt_get(w, 0, &pkts, &bytes);
			if (!pkts)
				continue;

			seq_printf(s, "%-5u %-12s %llu %llu\n", i, reason[t],
				   pkts, bytes);
		}
	}

	return 0;
}
DEFINE_SHOW_ATTRIBUTE(ppe_queue_drops);

/* What the buffer manager did to ingress: the buffers each of its ports holds,
 * and the frames it turned away, separated into a plain overload and a drop
 * the flow control caused.
 */
static int ppe_bm_show(struct seq_file *s, void *data)
{
	struct qca_ppe_priv *priv = s->private;
	u64 pkts, bytes, fc_pkts, fc_bytes;
	u32 w[PPE_CNT_WORDS], used, react;
	u32 i;

	seq_puts(s, "port used react drop_packets drop_bytes fc_packets fc_bytes\n");

	for (i = 0; i < PPE_BM_PORTS; i++) {
		regmap_read(priv->regmap, PPE_BM_PORT_CNT(i), &used);
		regmap_read(priv->regmap, PPE_BM_PORT_REACT_CNT(i), &react);

		if (regmap_bulk_read(priv->regmap, PPE_BM_DROP_STAT(i), w,
				     ARRAY_SIZE(w)))
			return -EIO;
		ppe_cnt_get(w, 0, &pkts, &bytes);

		if (regmap_bulk_read(priv->regmap,
				     PPE_BM_DROP_STAT(i + PPE_BM_PORTS), w,
				     ARRAY_SIZE(w)))
			return -EIO;
		ppe_cnt_get(w, 0, &fc_pkts, &fc_bytes);

		seq_printf(s, "%-4u %lu %lu %llu %llu %llu %llu\n", i,
			   FIELD_GET(PPE_BM_PORT_CNT_VAL, used),
			   FIELD_GET(PPE_BM_PORT_REACT_CNT_VAL, react),
			   pkts, bytes, fc_pkts, fc_bytes);
	}

	seq_puts(s, "group buffers\n");
	for (i = 0; i < PPE_BM_SHARED_GROUPS; i++) {
		regmap_read(priv->regmap, PPE_BM_SHARED_GRP_CNT(i), &used);
		seq_printf(s, "%-5u %lu\n", i,
			   FIELD_GET(PPE_BM_SHARED_GRP_CNT_VAL, used));
	}

	return 0;
}
DEFINE_SHOW_ATTRIBUTE(ppe_bm);

/* Where an ingress frame stopped before the lookup: the port receiver's own
 * drop count, the parser's per-port totals, and the L2 stage's per-VSI view.
 */
static int ppe_ingress_show(struct seq_file *s, void *data)
{
	u64 pkts, bytes, vlan_pkts, drop_pkts, drop_bytes;
	struct qca_ppe_priv *priv = s->private;
	u32 w[PPE_CNT_DROP_WORDS];
	u32 drops, parsed, lo, hi;
	int i;

	seq_puts(s, "port prx_drops parsed_packets parsed_bytes\n");

	for (i = 0; i < priv->ds.num_ports; i++) {
		regmap_read(priv->regmap, PPE_PRX_DROP_CNT(i), &drops);
		regmap_read(priv->regmap, PPE_IPR_PKT_CNT(i), &parsed);
		regmap_read(priv->regmap, PPE_IPR_BYTE_LO(i), &lo);
		regmap_read(priv->regmap, PPE_IPR_BYTE_HI(i), &hi);

		seq_printf(s, "%-4d %u %u %llu\n", i, drops, parsed,
			   lo | ((u64)hi << 32));
	}

	seq_puts(s, "vsi vlan_packets l2_packets l2_bytes l2_drops l2_drop_bytes\n");

	for (i = 0; i < PPE_VSI_MAX; i++) {
		if (regmap_bulk_read(priv->regmap, PPE_VLAN_CNT_TBL(i), w,
				     PPE_CNT_WORDS))
			return -EIO;
		ppe_cnt_get(w, 0, &vlan_pkts, &bytes);

		if (regmap_bulk_read(priv->regmap, PPE_PRE_L2_CNT_TBL(i), w,
				     ARRAY_SIZE(w)))
			return -EIO;
		ppe_cnt_get(w, 0, &pkts, &bytes);
		ppe_cnt_get(w, PPE_CNT_DROP_OFF, &drop_pkts, &drop_bytes);

		if (!vlan_pkts && !pkts && !drop_pkts)
			continue;

		seq_printf(s, "%-3d %llu %llu %llu %llu %llu\n", i, vlan_pkts,
			   pkts, bytes, drop_pkts, drop_bytes);
	}

	return 0;
}
DEFINE_SHOW_ATTRIBUTE(ppe_ingress);

/* What the egress editor put out, and what the transmit stage dropped on the
 * way. The stage totals bracket the per-port rows: a frame counted in but not
 * out never reached a port at all.
 */
static int ppe_egress_show(struct seq_file *s, void *data)
{
	struct qca_ppe_priv *priv = s->private;
	u64 pkts, bytes, drop_pkts, drop_bytes;
	u32 w[PPE_CNT_WORDS], val;
	int i;

	regmap_read(priv->regmap, PPE_EPE_DBG_IN_CNT, &val);
	seq_printf(s, "epe_in %u\n", val);
	regmap_read(priv->regmap, PPE_EPE_DBG_OUT_CNT, &val);
	seq_printf(s, "epe_out %u\n", val);

	seq_puts(s, "port packets bytes drop_packets drop_bytes\n");

	for (i = 0; i < priv->ds.num_ports; i++) {
		if (regmap_bulk_read(priv->regmap, PPE_PORT_TX_CNT_TBL(i), w,
				     ARRAY_SIZE(w)))
			return -EIO;
		ppe_cnt_get(w, 0, &pkts, &bytes);

		if (regmap_bulk_read(priv->regmap, PPE_PORT_TX_DROP_CNT(i), w,
				     ARRAY_SIZE(w)))
			return -EIO;
		ppe_cnt_get(w, 0, &drop_pkts, &drop_bytes);

		seq_printf(s, "%-4d %llu %llu %llu %llu\n", i, pkts, bytes,
			   drop_pkts, drop_bytes);
	}

	seq_puts(s, "vsi packets bytes\n");

	for (i = 0; i < PPE_VSI_MAX; i++) {
		if (regmap_bulk_read(priv->regmap, PPE_EG_VSI_CNT_TBL(i), w,
				     ARRAY_SIZE(w)))
			return -EIO;

		ppe_cnt_get(w, 0, &pkts, &bytes);
		if (!pkts)
			continue;

		seq_printf(s, "%-3d %llu %llu\n", i, pkts, bytes);
	}

	seq_puts(s, "vport packets bytes drop_packets drop_bytes\n");

	for (i = 0; i < PPE_MAX_VPORT; i++) {
		if (regmap_bulk_read(priv->regmap, PPE_VP_TX_CNT_TBL(i), w,
				     ARRAY_SIZE(w)))
			return -EIO;
		ppe_cnt_get(w, 0, &pkts, &bytes);

		if (regmap_bulk_read(priv->regmap, PPE_VP_TX_DROP_CNT(i), w,
				     ARRAY_SIZE(w)))
			return -EIO;
		ppe_cnt_get(w, 0, &drop_pkts, &drop_bytes);

		if (!pkts && !drop_pkts)
			continue;

		seq_printf(s, "%-5d %llu %llu %llu %llu\n", i, pkts, bytes,
			   drop_pkts, drop_bytes);
	}

	return 0;
}
DEFINE_SHOW_ATTRIBUTE(ppe_egress);

/* Whether the ACL is doing anything, and which rule. The three global counters
 * run with no enable bit, so a rule that classifies nothing shows up as a miss
 * count that keeps climbing while its own row stays at zero.
 */
static int ppe_acl_show(struct seq_file *s, void *data)
{
	struct qca_ppe_priv *priv = s->private;
	u32 w[PPE_CNT_WORDS], val;
	u64 pkts, bytes;
	int i;

	regmap_read(priv->regmap, PPE_ACL_GLB_HIT_CNT, &val);
	seq_printf(s, "hit %u\n", val);
	regmap_read(priv->regmap, PPE_ACL_GLB_MISS_CNT, &val);
	seq_printf(s, "miss %u\n", val);
	regmap_read(priv->regmap, PPE_ACL_GLB_BYPASS_CNT, &val);
	seq_printf(s, "bypass %u\n", val);

	seq_puts(s, "rule packets bytes\n");

	for (i = 0; i < PPE_ACL_CNT_ENTRIES; i++) {
		if (regmap_bulk_read(priv->regmap, PPE_ACL_CNT(i), w,
				     ARRAY_SIZE(w)))
			return -EIO;

		ppe_cnt_get(w, 0, &pkts, &bytes);
		if (!pkts)
			continue;

		seq_printf(s, "%-4d %llu %llu\n", i, pkts, bytes);
	}

	return 0;
}
DEFINE_SHOW_ATTRIBUTE(ppe_acl);

void ppe_counters_debugfs_init(struct qca_ppe_priv *priv)
{
	debugfs_create_file("l3_interface", 0400, priv->debugfs, priv,
			    &ppe_l3_interface_fops);
	debugfs_create_file("policer", 0400, priv->debugfs, priv,
			    &ppe_policer_fops);
	debugfs_create_file("drop_code", 0400, priv->debugfs, priv,
			    &ppe_drop_code_fops);
	debugfs_create_file("queues", 0400, priv->debugfs, priv,
			    &ppe_queues_fops);
	debugfs_create_file("queue_drops", 0400, priv->debugfs, priv,
			    &ppe_queue_drops_fops);
	debugfs_create_file("bm", 0400, priv->debugfs, priv, &ppe_bm_fops);
	debugfs_create_file("ingress", 0400, priv->debugfs, priv,
			    &ppe_ingress_fops);
	debugfs_create_file("egress", 0400, priv->debugfs, priv,
			    &ppe_egress_fops);
	debugfs_create_file("acl", 0400, priv->debugfs, priv, &ppe_acl_fops);
}
