// SPDX-License-Identifier: GPL-2.0-only
/* First-generation Ralink Packet Processing Engine ("HNAT v1") of RT305x,
 * RT3352 and RT3883
 */

#include <linux/bitfield.h>
#include <linux/debugfs.h>
#include <linux/dma-mapping.h>
#include <linux/etherdevice.h>
#include <linux/if_pppox.h>
#include <linux/if_vlan.h>
#include <linux/ip.h>
#include <linux/log2.h>
#include <linux/ppp_defs.h>
#include <linux/rhashtable.h>
#include <linux/seq_file.h>
#include <linux/udp.h>

#include "mtk_eth_soc.h"
#include "ppe_v1_offload.h"

/* packets per second at which a learned flow is bound (vendor default) */
#define PPE_V1_BIND_RATE	30

static const struct rhashtable_params ppe_v1_tuple_ht_params = {
	.head_offset = offsetof(struct ppe_v1_flow, tuple_node),
	.key_offset = offsetof(struct ppe_v1_flow, key),
	.key_len = sizeof(struct ppe_v1_tuple),
	.automatic_shrinking = true,
};

static u16 ppe_v1_timestamp(void)
{
	return fe_r32(PPE_REG_FOE_TS) & 0xffff;
}

/* FIN is a bound entry that has seen a TCP FIN */
static bool ppe_v1_entry_bound(u32 ib1)
{
	u32 state = FIELD_GET(PPE_IB1_STATE, ib1);

	return state == PPE_STATE_BIND || state == PPE_STATE_FIN;
}

static bool ppe_v1_entry_matches(const struct ppe_v1_foe_entry *hwe, u32 ib1,
				 const struct ppe_v1_tuple *key)
{
	return !!(ib1 & PPE_IB1_UDP) == (key->proto == IPPROTO_UDP) &&
	       READ_ONCE(hwe->ipv4.sip) == key->sip &&
	       READ_ONCE(hwe->ipv4.dip) == key->dip &&
	       READ_ONCE(hwe->data[3]) == ((u32)key->sport << 16 | key->dport);
}

static void ppe_v1_write_steering(bool on)
{
	u32 val;

	/* unicast to the PPE; broadcast and multicast always go to the CPU */
	val = fe_r32(FE_GDMA1_FWD_CFG) & ~PPE_GDMA_FWD_MASK;
	if (on)
		val |= FIELD_PREP(PPE_GDMA_FWD_UC_MYMAC, PPE_PSE_PORT_PPE) |
		       FIELD_PREP(PPE_GDMA_FWD_UC_OTHER, PPE_PSE_PORT_PPE);
	fe_w32(val, FE_GDMA1_FWD_CFG);
}

/* Steer GDMA1 RX through the PPE while a flow block is bound. While the
 * device is down, fe_hw_config() and fe_change_mtu() own GDMA1_FWD_CFG, so
 * leave it alone; ppe_v1_start() applies the state.
 */
void ppe_v1_set_steering(struct ppe_v1 *ppe)
{
	lockdep_assert_held(&ppe->mutex);

	if (ppe->running)
		ppe_v1_write_steering(!list_empty(&ppe->block_cb_list));
}

/* Called from ndo_open, i.e. after every frame engine reset, which may
 * clear the PPE configuration.
 */
void ppe_v1_start(struct ppe_v1 *ppe)
{
	u32 val;

	mutex_lock(&ppe->mutex);

	/* NAPI is off and ppe->running is false: nothing else uses the table */
	memset(ppe->foe, 0, ppe->entries * sizeof(*ppe->foe));
	/* the cleared table must reach memory before the PPE is enabled */
	wmb();

	fe_w32(ppe->foe_phys, PPE_REG_FOE_BASE);

	val = FIELD_PREP(PPE_FOE_CFG_TBL_SIZE,
			 ilog2(ppe->entries / PPE_FOE_ENTRIES_MIN)) |
	      PPE_FOE_CFG_HASH_MODE |
	      FIELD_PREP(PPE_FOE_CFG_SMA, PPE_FOE_SMA_FWD_CPU_BUILD) |
	      PPE_FOE_CFG_UNB_AGE | PPE_FOE_CFG_TCP_AGE |
	      PPE_FOE_CFG_UDP_AGE | PPE_FOE_CFG_FIN_AGE;
	fe_w32(val, PPE_REG_FOE_CFG);

	fe_w32(FIELD_PREP(PPE_FOE_UNB_AGE_MNP, 1000) |
	       FIELD_PREP(PPE_FOE_UNB_AGE_DLTA, 3), PPE_REG_FOE_UNB_AGE);
	fe_w32(FIELD_PREP(PPE_FOE_BND_AGE0_UDP, 5), PPE_REG_FOE_BND_AGE0);
	fe_w32(FIELD_PREP(PPE_FOE_BND_AGE1_FIN, 5) |
	       FIELD_PREP(PPE_FOE_BND_AGE1_TCP, 5), PPE_REG_FOE_BND_AGE1);
	fe_w32(FIELD_PREP(PPE_FOE_KA_T, 1) | FIELD_PREP(PPE_FOE_KA_TCP, 1) |
	       FIELD_PREP(PPE_FOE_KA_UDP, 1), PPE_REG_FOE_KA);

	fe_w32(FIELD_PREP(PPE_FOE_BNDR_RATE, PPE_V1_BIND_RATE),
	       PPE_REG_FOE_BNDR);
	fe_w32(FIELD_PREP(PPE_FOE_LMT1_QURT, 0x3fff) |
	       FIELD_PREP(PPE_FOE_LMT1_HALF, 0x3fff), PPE_REG_FOE_LMT1);
	fe_w32(FIELD_PREP(PPE_FOE_LMT2_FULL, 0x3fff), PPE_REG_FOE_LMT2);

	fe_w32(PPE_FLOW_SET_IPV4_NAPT | PPE_FLOW_SET_IPV4_NAT |
	       PPE_FLOW_SET_UC_FOE, PPE_REG_FLOW_SET);
	fe_w32(0, PPE_REG_POL_CFG);

	fe_w32(PPE_GLO_CFG_EN | PPE_GLO_CFG_TTL0_DRP, PPE_REG_GLO_CFG);

	ppe->running = true;
	ppe_v1_set_steering(ppe);

	mutex_unlock(&ppe->mutex);
}

void ppe_v1_stop(struct ppe_v1 *ppe)
{
	mutex_lock(&ppe->mutex);

	ppe_v1_write_steering(false);
	ppe->running = false;

	fe_w32(0, PPE_REG_GLO_CFG);
	fe_w32(0, PPE_REG_FLOW_SET);
	fe_w32(fe_r32(PPE_REG_FOE_CFG) & ~(PPE_FOE_CFG_UNB_AGE |
	       PPE_FOE_CFG_TCP_AGE | PPE_FOE_CFG_UDP_AGE |
	       PPE_FOE_CFG_FIN_AGE), PPE_REG_FOE_CFG);
	fe_w32(0, PPE_REG_FOE_BASE);

	mutex_unlock(&ppe->mutex);
}

/* ib1 of the entry @flow is bound to, or 0 if the hardware aged it out */
static u32 ppe_v1_flow_ib1(struct ppe_v1 *ppe, struct ppe_v1_flow *flow)
{
	struct ppe_v1_foe_entry *hwe;
	u32 ib1;

	lockdep_assert_held(&ppe->lock);

	if (flow->hash < 0)
		return 0;

	hwe = &ppe->foe[flow->hash];
	ib1 = READ_ONCE(hwe->ipv4.ib1);
	if (ppe_v1_entry_bound(ib1) && ppe_v1_entry_matches(hwe, ib1, &flow->key))
		return ib1;

	/* it will be learned and bound again */
	flow->hash = -1;
	return 0;
}

static void ppe_v1_flow_unbind(struct ppe_v1 *ppe, struct ppe_v1_flow *flow)
{
	if (ppe_v1_flow_ib1(ppe, flow)) {
		WRITE_ONCE(ppe->foe[flow->hash].ipv4.ib1,
			   FIELD_PREP(PPE_IB1_STATE, PPE_STATE_INVALID) |
			   FIELD_PREP(PPE_IB1_UNB_TIMESTAMP,
				      ppe_v1_timestamp()));
		/* push the state change out before the flow can go away */
		wmb();
	}
	flow->hash = -1;
}

/* Makes @flow visible to the RX path, which may bind it right away */
int ppe_v1_flow_commit(struct ppe_v1 *ppe, struct ppe_v1_flow *flow)
{
	return rhashtable_lookup_insert_fast(&ppe->tuple_ht, &flow->tuple_node,
					     ppe_v1_tuple_ht_params);
}

/* Unbinds @flow and hides it from the RX path, so that it can be freed */
void ppe_v1_flow_clear(struct ppe_v1 *ppe, struct ppe_v1_flow *flow)
{
	spin_lock_bh(&ppe->lock);
	ppe_v1_flow_unbind(ppe, flow);
	rhashtable_remove_fast(&ppe->tuple_ht, &flow->tuple_node,
			       ppe_v1_tuple_ht_params);
	spin_unlock_bh(&ppe->lock);
}

/* Seconds since the bound entry last forwarded a packet, or -1 if the flow
 * is not bound
 */
int ppe_v1_flow_idle_time(struct ppe_v1 *ppe, struct ppe_v1_flow *flow)
{
	int idle = -1;
	u32 ib1;

	spin_lock_bh(&ppe->lock);
	ib1 = ppe_v1_flow_ib1(ppe, flow);
	if (ib1)
		idle = (u16)(ppe_v1_timestamp() -
			     FIELD_GET(PPE_IB1_BIND_TIMESTAMP, ib1));
	spin_unlock_bh(&ppe->lock);

	return idle;
}

/* The PPE corrupts UDP packets without checksum; leave those to the CPU.
 * skb->data points behind the Ethernet header, at up to two VLAN tags and a
 * PPPoE session header.
 */
static bool ppe_v1_udp_nocsum(const struct sk_buff *skb)
{
	__be16 proto = skb->protocol;
	const struct udphdr *uh;
	const struct iphdr *iph;
	unsigned int off = 0;
	struct udphdr _uh;
	struct iphdr _iph;
	int i;

	for (i = 0; i < 2 && proto == htons(ETH_P_8021Q); i++) {
		const struct vlan_hdr *vh;
		struct vlan_hdr _vh;

		vh = skb_header_pointer(skb, off, sizeof(_vh), &_vh);
		if (!vh)
			return true;
		proto = vh->h_vlan_encapsulated_proto;
		off += VLAN_HLEN;
	}

	if (proto == htons(ETH_P_PPP_SES)) {
		const __be16 *ppp;
		__be16 _ppp;

		ppp = skb_header_pointer(skb, off + sizeof(struct pppoe_hdr),
					 sizeof(_ppp), &_ppp);
		if (!ppp || *ppp != htons(PPP_IP))
			return true;
		off += PPPOE_SES_HLEN;
	} else if (proto != htons(ETH_P_IP)) {
		return true;
	}

	iph = skb_header_pointer(skb, off, sizeof(_iph), &_iph);
	if (!iph || iph->ihl != 5 || iph->protocol != IPPROTO_UDP)
		return true;

	uh = skb_header_pointer(skb, off + sizeof(_iph), sizeof(_uh), &_uh);

	return !uh || !uh->check;
}

static void ppe_v1_bind(struct ppe_v1 *ppe, const struct sk_buff *skb,
			u32 idx)
{
	struct ppe_v1_foe_entry *hwe = &ppe->foe[idx];
	struct ppe_v1_tuple key = {};
	struct ppe_v1_flow *flow;
	u32 ib1, ports;
	int i;

	ib1 = READ_ONCE(hwe->ipv4.ib1);
	if (FIELD_GET(PPE_IB1_STATE, ib1) != PPE_STATE_UNBIND ||
	    FIELD_GET(PPE_IB1_PKT_TYPE, ib1) != PPE_PKT_TYPE_IPV4_HNAPT)
		return;

	ports = READ_ONCE(hwe->data[3]);
	key.sip = READ_ONCE(hwe->ipv4.sip);
	key.dip = READ_ONCE(hwe->ipv4.dip);
	key.sport = upper_16_bits(ports);
	key.dport = lower_16_bits(ports);
	key.proto = (ib1 & PPE_IB1_UDP) ? IPPROTO_UDP : IPPROTO_TCP;

	spin_lock(&ppe->lock);
	flow = rhashtable_lookup_fast(&ppe->tuple_ht, &key,
				      ppe_v1_tuple_ht_params);
	if (!flow) {
		ppe->stats.bind_no_flow++;
		goto unlock;
	}

	if (key.proto == IPPROTO_UDP && ppe_v1_udp_nocsum(skb)) {
		ppe->stats.bind_udp_nocsum++;
		goto unlock;
	}

	/* re-learned at another index after the old entry aged out */
	if (flow->hash != idx)
		ppe_v1_flow_unbind(ppe, flow);

	for (i = 4; i < ARRAY_SIZE(hwe->data); i++)
		WRITE_ONCE(hwe->data[i], flow->foe.data[i]);
	/* the PPE must see the complete entry before the BIND state */
	wmb();

	/* the hardware may have recycled the entry meanwhile */
	ib1 = READ_ONCE(hwe->ipv4.ib1);
	if (FIELD_GET(PPE_IB1_STATE, ib1) != PPE_STATE_UNBIND ||
	    !ppe_v1_entry_matches(hwe, ib1, &key)) {
		ppe->stats.bind_race++;
		goto unlock;
	}

	ib1 &= PPE_IB1_PKT_TYPE | PPE_IB1_UDP;
	ib1 |= flow->foe.ipv4.ib1 |
	       FIELD_PREP(PPE_IB1_BIND_TIMESTAMP, ppe_v1_timestamp()) |
	       FIELD_PREP(PPE_IB1_STATE, PPE_STATE_BIND);
	WRITE_ONCE(hwe->ipv4.ib1, ib1);

	flow->hash = idx;
	ppe->stats.bind++;
unlock:
	spin_unlock(&ppe->lock);
}

/* RX hook, called from NAPI after eth_type_trans(). NAPI only runs between
 * ppe_v1_start() and ppe_v1_stop().
 */
void ppe_v1_rx(struct ppe_v1 *ppe, struct sk_buff *skb, u32 rxd4)
{
	u32 idx = FIELD_GET(PPE_RXD4_FOE_ENTRY, rxd4);

	if (!ppe || !(rxd4 & PPE_RXD4_FVLD) || idx >= ppe->entries)
		return;

	if (FIELD_GET(PPE_RXD4_AI, rxd4) == PPE_CPU_REASON_HIT_UNBIND_RATE_REACH)
		ppe_v1_bind(ppe, skb, idx);
}

static const char * const ppe_v1_state_str[] = {
	"INV", "UNB", "BND", "FIN",
};

static int ppe_v1_entries_show(struct seq_file *m, void *private)
{
	struct ppe_v1 *ppe = m->private;
	u32 i;

	seq_printf(m, "foe_ts %u\n", ppe_v1_timestamp());
	for (i = 0; i < ppe->entries; i++) {
		struct ppe_v1_foe_entry e = ppe->foe[i];
		struct ppe_v1_foe_ipv4 *p = &e.ipv4;
		u32 state = FIELD_GET(PPE_IB1_STATE, p->ib1);

		if (state == PPE_STATE_INVALID && !p->sip && !p->dip)
			continue;

		seq_printf(m, "%05u %s %s %pI4h:%u -> %pI4h:%u",
			   i, ppe_v1_state_str[state],
			   (p->ib1 & PPE_IB1_UDP) ? "udp" : "tcp",
			   &p->sip, p->sport, &p->dip, p->dport);
		if (ppe_v1_entry_bound(p->ib1))
			seq_printf(m, " => %pI4h:%u -> %pI4h:%u dp %lu vlan %u/%u ts %lu",
				   &p->new_sip, p->new_sport,
				   &p->new_dip, p->new_dport,
				   FIELD_GET(PPE_IB2_DP, p->ib2),
				   p->vlan1, p->vlan2,
				   FIELD_GET(PPE_IB1_BIND_TIMESTAMP, p->ib1));
		else
			seq_printf(m, " pcnt %lu ts %lu",
				   FIELD_GET(PPE_IB1_UNB_PCNT, p->ib1),
				   FIELD_GET(PPE_IB1_UNB_TIMESTAMP, p->ib1));
		seq_printf(m, " ib1 %08x ib2 %08x\n", p->ib1, p->ib2);
	}

	return 0;
}
DEFINE_SHOW_ATTRIBUTE(ppe_v1_entries);

static int ppe_v1_status_show(struct seq_file *m, void *private)
{
	static const struct {
		const char *name;
		u32 reg;
	} regs[] = {
		{ "GDMA1_FWD_CFG", FE_GDMA1_FWD_CFG },
		{ "FOE_TS", PPE_REG_FOE_TS },
		{ "GLO_CFG", PPE_REG_GLO_CFG },
		{ "FLOW_SET", PPE_REG_FLOW_SET },
		{ "POL_CFG", PPE_REG_POL_CFG },
		{ "FOE_CFG", PPE_REG_FOE_CFG },
		{ "FOE_BASE", PPE_REG_FOE_BASE },
		{ "FOE_USE", PPE_REG_FOE_USE },
		{ "FOE_BNDR", PPE_REG_FOE_BNDR },
		{ "FOE_LMT1", PPE_REG_FOE_LMT1 },
		{ "FOE_LMT2", PPE_REG_FOE_LMT2 },
		{ "FOE_KA", PPE_REG_FOE_KA },
		{ "FOE_UNB_AGE", PPE_REG_FOE_UNB_AGE },
		{ "FOE_BND_AGE0", PPE_REG_FOE_BND_AGE0 },
		{ "FOE_BND_AGE1", PPE_REG_FOE_BND_AGE1 },
	};
	struct ppe_v1 *ppe = m->private;
	struct ppe_v1_stats *s = &ppe->stats;
	u32 count[4] = {};
	int i;

	for (i = 0; i < ARRAY_SIZE(regs); i++)
		seq_printf(m, "%-14s %08x\n", regs[i].name, fe_r32(regs[i].reg));

	for (i = 0; i < ppe->entries; i++)
		count[FIELD_GET(PPE_IB1_STATE, READ_ONCE(ppe->foe[i].ipv4.ib1))]++;

	mutex_lock(&ppe->mutex);
	seq_printf(m, "running %d blocks %zu entries %u\n", ppe->running,
		   list_count_nodes(&ppe->block_cb_list), ppe->entries);
	mutex_unlock(&ppe->mutex);
	seq_printf(m, "state: invalid %u unbind %u bind %u fin %u\n",
		   count[0], count[1], count[2], count[3]);
	seq_printf(m, "bind %u no_flow %u udp_nocsum %u race %u\n",
		   s->bind, s->bind_no_flow, s->bind_udp_nocsum, s->bind_race);

	return 0;
}
DEFINE_SHOW_ATTRIBUTE(ppe_v1_status);

/* runs after the netdev is unregistered, so no flow block is bound */
static void ppe_v1_destroy(void *data)
{
	struct ppe_v1 *ppe = data;

	debugfs_remove(ppe->debugfs);
	ppe_v1_offload_cleanup(ppe);
	rhashtable_destroy(&ppe->tuple_ht);
}

/* must be called before the netdev is registered (devm ordering) */
struct ppe_v1 *ppe_v1_create(struct fe_priv *priv, struct net_device *netdev)
{
	u32 entries = priv->soc->ppe_entries;
	struct device *dev = priv->dev;
	struct ppe_v1 *ppe;

	if (!entries)
		return NULL;

	if (WARN_ON(!is_power_of_2(entries) || entries < PPE_FOE_ENTRIES_MIN ||
		    entries > PPE_FOE_ENTRIES_MAX))
		return NULL;

	ppe = devm_kzalloc(dev, sizeof(*ppe), GFP_KERNEL);
	if (!ppe)
		return NULL;

	ppe->netdev = netdev;
	ppe->entries = entries;
	spin_lock_init(&ppe->lock);
	mutex_init(&ppe->mutex);
	INIT_LIST_HEAD(&ppe->flows);
	INIT_LIST_HEAD(&ppe->block_cb_list);

	ppe->foe = dmam_alloc_coherent(dev, ppe->entries * sizeof(*ppe->foe),
				       &ppe->foe_phys, GFP_KERNEL);
	if (!ppe->foe) {
		dev_warn(dev, "no memory for the PPE table, flow offload disabled\n");
		return NULL;
	}

	if (rhashtable_init(&ppe->tuple_ht, &ppe_v1_tuple_ht_params))
		return NULL;
	if (ppe_v1_offload_init(ppe)) {
		rhashtable_destroy(&ppe->tuple_ht);
		return NULL;
	}
	if (devm_add_action_or_reset(dev, ppe_v1_destroy, ppe))
		return NULL;

	ppe->debugfs = debugfs_create_dir("ralink_ppe", NULL);
	debugfs_create_file("entries", 0400, ppe->debugfs, ppe,
			    &ppe_v1_entries_fops);
	debugfs_create_file("status", 0400, ppe->debugfs, ppe,
			    &ppe_v1_status_fops);

	return ppe;
}
