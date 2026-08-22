// SPDX-License-Identifier: GPL-2.0-or-later OR MIT
/* Copyright (c) 2026 Julius Bairaktaris <julius@bairaktaris.de> */
/* Routed and NAT flow offload for the Qualcomm PPE.
 *
 * The PPE matches a packet's 5-tuple against a hashed flow table and, on a hit,
 * forwards it in hardware. The key is split across two tables: the flow entry
 * carries the destination address, the L4 ports and the protocol, while the
 * source address lives in the host table and the flow entry only references its
 * index. Which of the two addresses goes where when a packet is looked up is
 * a per-direction hardware choice (PPE_FLOW_KEY_SEL); every entry is staged
 * the same way here, and the one direction group that looks up with the
 * opposite orientation - WAN-to-LAN, where tunnel-terminated ingress lands -
 * gets its KEY_SEL flipped to match in ppe_flow_init().
 *
 * Entries are placed by the hardware hash rather than by the driver: an add
 * stages the entry in the op registers, and the hardware picks the slot, writes
 * the host index into the entry itself, and reports both back.
 */

#include <linux/bitfield.h>
#include <linux/clk.h>
#include <linux/debugfs.h>
#include <linux/regmap.h>
#include <linux/seq_file.h>

#include "qca_ppe.h"

#define PPE_FLOW_OP_RETRIES	1000

/* The result register reports how many results are queued and a read pops one,
 * so the value has to be captured on the poll that sees it, not re-read after.
 */
static int ppe_flow_op_wait(struct qca_ppe_priv *priv, u32 rslt_reg,
			    u32 cmd_id, u32 *rslt)
{
	u32 val;
	int i;

	for (i = 0; i < PPE_FLOW_OP_RETRIES; i++) {
		regmap_read(priv->regmap, rslt_reg, &val);
		if (FIELD_GET(PPE_FLOW_RSLT_VALID_CNT, val) &&
		    FIELD_GET(PPE_FLOW_RSLT_CMD_ID, val) == cmd_id) {
			*rslt = val;
			return 0;
		}
		udelay(1);
	}

	return -ETIMEDOUT;
}

static u32 ppe_flow_next_cmd_id(struct qca_ppe_priv *priv)
{
	priv->flow_cmd_id = (priv->flow_cmd_id + 1) & PPE_FLOW_RSLT_CMD_ID;

	return priv->flow_cmd_id;
}

/* Add, delete or look up one flow entry by key. @host carries the source
 * address half of the key and is staged alongside, so that an add creates both
 * halves and binds them in one command. On success @index and @host_index name
 * the slots the hardware chose.
 */
int ppe_flow_op(struct qca_ppe_priv *priv, u32 op_type,
		const u32 *entry, int nentry, const u32 *host, int nhost,
		u32 *index, u32 *host_index)
{
	u32 cmd_id, rslt, val;
	int ret, i;

	lockdep_assert_held(&priv->flow_lock);

	for (i = 0; i < nhost; i++)
		regmap_write(priv->regmap, PPE_FLOW_HOST_TBL_OP_DATA(i),
			     host[i]);
	regmap_write(priv->regmap, PPE_FLOW_HOST_TBL_OP,
		     FIELD_PREP(PPE_FLOW_HOST_OP_HASH_BLOCK,
				PPE_FLOW_HASH_BLOCKS));

	for (i = 0; i < nentry; i++)
		regmap_write(priv->regmap, PPE_FLOW_TBL_OP_DATA(i), entry[i]);

	cmd_id = ppe_flow_next_cmd_id(priv);
	regmap_write(priv->regmap, PPE_FLOW_TBL_OP,
		     FIELD_PREP(PPE_FLOW_OP_CMD_ID, cmd_id) |
		     FIELD_PREP(PPE_FLOW_OP_TYPE, op_type) |
		     FIELD_PREP(PPE_FLOW_OP_HASH_BLOCK, PPE_FLOW_HASH_BLOCKS) |
		     PPE_FLOW_OP_HOST_EN);

	ret = ppe_flow_op_wait(priv, PPE_FLOW_TBL_OP_RSLT, cmd_id, &rslt);
	if (ret)
		return ret;

	if (rslt & PPE_FLOW_RSLT_FAIL)
		return -ENOENT;

	if (index)
		*index = FIELD_GET(PPE_FLOW_RSLT_ENTRY_IDX, rslt);

	/* The host result register is a queue that fills on its own schedule,
	 * so it may not yet - or no longer - hold this command's result. What
	 * binds the flow to its host entry is the host index the hardware
	 * wrote into the flow entry itself; read it from there.
	 */
	if (index && host_index) {
		regmap_read(priv->regmap, PPE_IN_FLOW_TBL(*index), &val);
		*host_index = ppe_entry_get(&val, PPE_FLOW_E_HOST_IDX_OFF,
					    PPE_FLOW_E_HOST_IDX_LEN);
	}

	return 0;
}

/* Read one entry back by index. An IPv6 entry spans two of the flat table's
 * slots and the hardware distributes its words across them, so the flat mapping
 * cannot be used to read an entry of either family with one layout — the op
 * engine returns the words as they were staged.
 */
int ppe_flow_entry_read(struct qca_ppe_priv *priv, u32 index, u32 *words,
			int nwords)
{
	u32 cmd_id, rslt;
	int ret, i;

	lockdep_assert_held(&priv->flow_lock);

	cmd_id = ppe_flow_next_cmd_id(priv);
	regmap_write(priv->regmap, PPE_FLOW_TBL_RD_OP,
		     FIELD_PREP(PPE_FLOW_OP_CMD_ID, cmd_id) |
		     FIELD_PREP(PPE_FLOW_OP_TYPE, PPE_TBL_OP_GET) |
		     FIELD_PREP(PPE_FLOW_OP_HASH_BLOCK, PPE_FLOW_HASH_BLOCKS) |
		     PPE_FLOW_OP_INDEX_MODE |
		     FIELD_PREP(PPE_FLOW_OP_ENTRY_IDX, index));

	ret = ppe_flow_op_wait(priv, PPE_FLOW_TBL_RD_OP_RSLT, cmd_id, &rslt);
	if (ret)
		return ret;

	if (rslt & PPE_FLOW_RSLT_FAIL)
		return -ENOENT;

	for (i = 0; i < nwords; i++)
		regmap_read(priv->regmap, PPE_FLOW_TBL_RD_RSLT_DATA(i),
			    &words[i]);

	return 0;
}

/* Delete one flow entry by the index the hardware assigned it, so the key does
 * not have to be kept and restaged just to remove it.
 */
int ppe_flow_entry_delete(struct qca_ppe_priv *priv, u32 index)
{
	u32 cmd_id, rslt;
	int ret;

	lockdep_assert_held(&priv->flow_lock);

	cmd_id = ppe_flow_next_cmd_id(priv);
	regmap_write(priv->regmap, PPE_FLOW_TBL_OP,
		     FIELD_PREP(PPE_FLOW_OP_CMD_ID, cmd_id) |
		     FIELD_PREP(PPE_FLOW_OP_TYPE, PPE_TBL_OP_DEL) |
		     FIELD_PREP(PPE_FLOW_OP_HASH_BLOCK, PPE_FLOW_HASH_BLOCKS) |
		     PPE_FLOW_OP_INDEX_MODE |
		     FIELD_PREP(PPE_FLOW_OP_ENTRY_IDX, index));

	ret = ppe_flow_op_wait(priv, PPE_FLOW_TBL_OP_RSLT, cmd_id, &rslt);
	if (ret)
		return ret;

	return (rslt & PPE_FLOW_RSLT_FAIL) ? -ENOENT : 0;
}

/* A host entry is visible in the flat mapping but a valid one cannot be cleared
 * by writing zeroes there, so removal goes through the host op engine.
 */
int ppe_host_del(struct qca_ppe_priv *priv, u32 index)
{
	u32 cmd_id, val;
	int i;

	lockdep_assert_held(&priv->flow_lock);

	cmd_id = ppe_flow_next_cmd_id(priv);
	regmap_write(priv->regmap, PPE_HOST_TBL_OP,
		     FIELD_PREP(PPE_HOST_OP_CMD_ID, cmd_id) |
		     FIELD_PREP(PPE_HOST_OP_TYPE, PPE_TBL_OP_DEL) |
		     FIELD_PREP(PPE_HOST_OP_HASH_BLOCK, PPE_FLOW_HASH_BLOCKS) |
		     PPE_HOST_OP_INDEX_MODE |
		     FIELD_PREP(PPE_HOST_OP_ENTRY_IDX, index));

	for (i = 0; i < PPE_FLOW_OP_RETRIES; i++) {
		regmap_read(priv->regmap, PPE_HOST_TBL_OP_RSLT, &val);
		if (FIELD_GET(PPE_HOST_RSLT_VALID_CNT, val) &&
		    FIELD_GET(PPE_HOST_RSLT_CMD_ID, val) == cmd_id)
			return (val & PPE_HOST_RSLT_FAIL) ? -ENOENT : 0;
		udelay(1);
	}

	return -ETIMEDOUT;
}

void ppe_flow_counter_read(struct qca_ppe_priv *priv, u32 index, u64 *packets,
			   u64 *bytes)
{
	u32 lo, hi, hi2;

	regmap_read(priv->regmap, PPE_IN_FLOW_CNT_TBL(index), &lo);
	*packets = lo;

	/* 40-bit byte counter. Re-read on a carry between the two halves so a
	 * count crossing the low word's boundary is not torn by 1 << 32.
	 */
	regmap_read(priv->regmap, PPE_IN_FLOW_CNT_TBL(index) + 8, &hi);
	do {
		hi2 = hi;
		regmap_read(priv->regmap, PPE_IN_FLOW_CNT_TBL(index) + 4, &lo);
		regmap_read(priv->regmap, PPE_IN_FLOW_CNT_TBL(index) + 8, &hi);
	} while (hi != hi2);
	*bytes = lo | ((u64)FIELD_GET(PPE_FLOW_CNT_BYTES_HI, hi) << 32);
}

/* Deleting an entry does not reset its counter, so a slot has to be cleared
 * when it is handed to a new flow rather than when the old one goes away.
 */
void ppe_flow_counter_clear(struct qca_ppe_priv *priv, u32 index)
{
	int i;

	for (i = 0; i < PPE_FLOW_CNT_WORDS; i++)
		regmap_write(priv->regmap, PPE_IN_FLOW_CNT_TBL(index) + i * 4,
			     0);
}

static int ppe_flows_show(struct seq_file *s, void *data)
{
	struct qca_ppe_priv *priv = s->private;
	u32 w[PPE_FLOW_ENTRY_WORDS_V6];
	u64 packets, bytes;
	u32 i;

	seq_puts(s, "index type proto fwd age host  packets bytes\n");

	guard(mutex)(&priv->flow_lock);

	for (i = 0; i < priv->data->num_flow_entries; i++) {
		if (ppe_flow_entry_read(priv, i, w, ARRAY_SIZE(w)))
			continue;
		if (!ppe_entry_get(w, PPE_FLOW_E_VALID_OFF,
				   PPE_FLOW_E_VALID_LEN))
			continue;

		ppe_flow_counter_read(priv, i, &packets, &bytes);

		seq_printf(s, "%-5u %-4s %-5llu %-3llu %-3llu %-5llu %llu %llu\n",
			   i, (w[0] & PPE_FLOW_E_TYPE_IPV6) ? "ipv6" : "ipv4",
			   ppe_entry_get(w, PPE_FLOW_E_PROTO_OFF,
					 PPE_FLOW_E_PROTO_LEN),
			   ppe_entry_get(w, PPE_FLOW_E_FWD_TYPE_OFF,
					 PPE_FLOW_E_FWD_TYPE_LEN),
			   ppe_entry_get(w, PPE_FLOW_E_AGE_OFF,
					 PPE_FLOW_E_AGE_LEN),
			   ppe_entry_get(w, PPE_FLOW_E_HOST_IDX_OFF,
					 PPE_FLOW_E_HOST_IDX_LEN),
			   packets, bytes);

		/* An IPv6 entry occupies two slots and reads back identically
		 * through either, so it would otherwise be listed twice.
		 */
		if (w[0] & PPE_FLOW_E_TYPE_IPV6)
			i++;
	}

	return 0;
}
DEFINE_SHOW_ATTRIBUTE(ppe_flows);

static const char * const ppe_flow_reject_name[] = {
	[PPE_REJECT_INGRESS_PORT]	= "ingress_not_switch_port",
	[PPE_REJECT_INGRESS_VLAN]	= "ingress_vlan_domain",
	[PPE_REJECT_KEY]		= "unsupported_match",
	[PPE_REJECT_PROTO]		= "unsupported_protocol",
	[PPE_REJECT_ACTION]		= "unsupported_action",
	[PPE_REJECT_L2]			= "no_egress_l2_header",
	[PPE_REJECT_EGRESS_PORT]	= "egress_not_switch_port",
	[PPE_REJECT_HAIRPIN]		= "egress_is_ingress",
	[PPE_REJECT_NAT_BOTH]		= "snat_and_dnat",
	[PPE_REJECT_NAT_IPV6]		= "ipv6_nat",
	[PPE_REJECT_RESOURCE]		= "table_full",
	[PPE_REJECT_HW_OP]		= "hardware_op_failed",
};

static int ppe_offload_show(struct seq_file *s, void *data)
{
	struct qca_ppe_priv *priv = s->private;
	int i;

	guard(mutex)(&priv->flow_lock);

	seq_printf(s, "%-24s %u\n", "offloaded", priv->flow_offloaded);
	seq_printf(s, "%-24s %u\n", "reinstalled", priv->flow_reinstalled);
	seq_printf(s, "%-24s %u\n", "destroy_miss", priv->flow_destroy_miss);
	seq_printf(s, "%-24s %u\n", "stale", priv->flow_stale);
	seq_printf(s, "%-24s %u\n", "live_entries",
		   atomic_read(&priv->flow_table.nelems));
	for (i = 0; i < PPE_REJECT_MAX; i++)
		seq_printf(s, "%-24s %u\n", ppe_flow_reject_name[i],
			   priv->flow_reject[i]);

	return 0;
}
DEFINE_SHOW_ATTRIBUTE(ppe_offload);

void ppe_flow_debugfs_init(struct qca_ppe_priv *priv)
{
	priv->debugfs = debugfs_create_dir(dev_name(priv->ds.dev), NULL);
	debugfs_create_file("flows", 0400, priv->debugfs, priv,
			    &ppe_flows_fops);
	debugfs_create_file("offload", 0400, priv->debugfs, priv,
			    &ppe_offload_fops);
}

void ppe_flow_debugfs_exit(struct qca_ppe_priv *priv)
{
	debugfs_remove_recursive(priv->debugfs);
}

/* Every period the PPE derives from its own clock - the flow age step below,
 * the shaper and policer refresh - is wrong by whatever the board clocks the
 * block at, so read the rate rather than assuming one.
 */
unsigned long ppe_clk_rate(struct qca_ppe_priv *priv)
{
	int i;

	for (i = 0; i < priv->num_clks; i++)
		if (!strcmp(priv->clks[i].id, "nss_ppe_clk"))
			return clk_get_rate(priv->clks[i].clk);

	return 0;
}

/* The entry's two-bit age field counts down one step per age period, so an
 * untouched entry survives two to three periods. The hardware turns its own
 * clock into that period using the rate it is told about here.
 */
static void ppe_flow_age_timer_set(struct qca_ppe_priv *priv, u32 *ctrl)
{
	unsigned long rate = ppe_clk_rate(priv);

	if (!rate)
		return;

	*ctrl |= FIELD_PREP(PPE_FLOW_CLK_FREQ_MHZ, rate / HZ_PER_MHZ) |
		 FIELD_PREP(PPE_FLOW_AGE_TIMER, PPE_FLOW_AGE_SECS) |
		 FIELD_PREP(PPE_FLOW_AGE_TIMER_UNIT, PPE_FLOW_AGE_UNIT_SEC);
}

void ppe_flow_init(struct qca_ppe_priv *priv)
{
	u32 ctrl;
	int type, dir;

	mutex_init(&priv->flow_lock);
	mutex_init(&priv->vlan_lock);

	/* A miss has to forward: with the lookup enabled and no entry matching,
	 * any other action would black-hole traffic the driver never saw.
	 *
	 * Fragments bypass the lookup: only the first fragment carries the L4
	 * ports, so matching it in hardware while the rest miss to the CPU
	 * would leave conntrack's reassembly waiting forever.
	 *
	 * The neighbouring TCP_SPECIAL bypass stays off: on this generation it
	 * takes every TCP packet out of the lookup, not just the flagged ones.
	 * A connection's FIN and RST are therefore forwarded in hardware and its
	 * entry goes on the flowtable's idle timeout, as it does on every driver
	 * whose hardware never shows it the teardown.
	 */
	for (type = 0; type < PPE_FLOW_PKT_TYPES; type++) {
		u32 val = 0;

		for (dir = 0; dir < PPE_FLOW_CTRL1_DIRS; dir++)
			val |= (FIELD_PREP(PPE_FLOW_MISS_ACTION,
					   PPE_FLOW_MISS_FORWARD) |
				PPE_FLOW_FRAG_BYPASS)
			       << (dir * PPE_FLOW_CTRL1_DIR_BITS);

		/* Tunnel-terminated ingress (a de-encapsulated PPPoE frame)
		 * classifies as WAN-to-LAN regardless of the host entries,
		 * and that direction group builds its lookup key with the
		 * opposite orientation: without its KEY_SEL flipped, such
		 * frames never find the entries this driver installs. All
		 * other groups match the orientation entries are staged in.
		 */
		val |= PPE_FLOW_KEY_SEL
		       << (PPE_FLOW_DIR_WAN_TO_LAN * PPE_FLOW_CTRL1_DIR_BITS);

		regmap_write(priv->regmap, PPE_FLOW_CTRL1(type), val);
	}

	/* Hash mode 1 is the bucket function the entries here are placed by, set
	 * explicitly rather than inherited from whatever ran before.
	 */
	ctrl = PPE_FLOW_EN | FIELD_PREP(PPE_FLOW_HASH_MODE1, 1);
	ppe_flow_age_timer_set(priv, &ctrl);
	regmap_write(priv->regmap, PPE_FLOW_CTRL0, ctrl);
}
