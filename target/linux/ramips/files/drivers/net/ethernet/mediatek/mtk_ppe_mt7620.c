// SPDX-License-Identifier: GPL-2.0-only
/*
 * Hardware flow offload for the packet processing engine in MT7620 SoCs.
 *
 * MT7620 contains the HNAT v2 PPE, but its FOE format and integrated-switch
 * port 7 topology differ from the newer MediaTek NETSYS PPE supported by
 * drivers/net/ethernet/mediatek.
 */

#include <linux/bitfield.h>
#include <linux/bitmap.h>
#include <linux/dma-mapping.h>
#include <linux/etherdevice.h>
#include <linux/if_ether.h>
#include <linux/if_vlan.h>
#include <linux/iopoll.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/rhashtable.h>
#include <linux/reset.h>
#include <linux/tcp.h>
#include <linux/unaligned.h>
#include <linux/udp.h>
#include <linux/workqueue.h>
#include <net/dsa.h>
#include <net/flow_offload.h>
#include <net/ipv6.h>
#include <net/pkt_cls.h>

#include <asm/mach-ralink/ralink_regs.h>
#include <asm/mach-ralink/mt7620.h>

#include <linux/soc/mediatek/mt7620-gsw.h>

#include "mtk_ppe_mt7620.h"
#include "mtk_eth_soc.h"

#define MT7620_PPE_NUM_ENTRIES		4096
#define MT7620_PPE_HASH_MASK		0x7ff
#define MT7620_PPE_WAYS			2

/* The PPE block starts at FE + 0xc00; HNAT v2 control starts at +0x200. */
#define MT7620_PPE_BASE			0x0c00
#define MT7620_PPE_GDM2_FWD_CFG		(MT7620_PPE_BASE + 0x100)
#define MT7620_PPE_GLO_CFG		(MT7620_PPE_BASE + 0x200)
#define MT7620_PPE_FLOW_SET		(MT7620_PPE_BASE + 0x204)
#define MT7620_PPE_IP_PROT_CHK		(MT7620_PPE_BASE + 0x208)
#define MT7620_PPE_TB_CFG		(MT7620_PPE_BASE + 0x21c)
#define MT7620_PPE_TB_BASE		(MT7620_PPE_BASE + 0x220)
#define MT7620_PPE_BNDR			(MT7620_PPE_BASE + 0x228)
#define MT7620_PPE_BIND_LMT0		(MT7620_PPE_BASE + 0x22c)
#define MT7620_PPE_BIND_LMT1		(MT7620_PPE_BASE + 0x230)
#define MT7620_PPE_KA			(MT7620_PPE_BASE + 0x234)
#define MT7620_PPE_UNB_AGE		(MT7620_PPE_BASE + 0x238)
#define MT7620_PPE_BND_AGE0		(MT7620_PPE_BASE + 0x23c)
#define MT7620_PPE_BND_AGE1		(MT7620_PPE_BASE + 0x240)
#define MT7620_PPE_HASH_SEED		(MT7620_PPE_BASE + 0x244)
#define MT7620_PPE_FP_BMAP0		(MT7620_PPE_BASE + 0x248)
#define MT7620_PPE_FP_BMAP1		(MT7620_PPE_BASE + 0x24c)
#define MT7620_PPE_FP_BMAP2		(MT7620_PPE_BASE + 0x250)
#define MT7620_PPE_FP_BMAP3		(MT7620_PPE_BASE + 0x254)
#define MT7620_PPE_FP_BMAP4		(MT7620_PPE_BASE + 0x258)
#define MT7620_PPE_VLAN_MTU0		(MT7620_PPE_BASE + 0x30c)
#define MT7620_PPE_VLAN_MTU1		(MT7620_PPE_BASE + 0x310)
#define MT7620_PPE_VLAN_MTU2		(MT7620_PPE_BASE + 0x314)
#define MT7620_PPE_CACHE_CTL		(MT7620_PPE_BASE + 0x320)
#define MT7620_PPE_UDP_CTL		(MT7620_PPE_BASE + 0x380)

#define MT7620_PPE_FLOW_IPV4_NAT	BIT(12)
#define MT7620_PPE_FLOW_IPV4_NAPT	BIT(13)
#define MT7620_PPE_FLOW_IPV6_3T		BIT(8)
#define MT7620_PPE_FLOW_IPV6_5T		BIT(9)

#define MT7620_PPE_GDM2_FWD_MASK	0x7777
#define MT7620_PPE_GLO_CFG_BUSY		BIT(31)

#define MT7620_PPE_TB_SIZE		GENMASK(2, 0)
#define MT7620_PPE_TB_ENTRY_80B		BIT(3)
#define MT7620_PPE_TB_MISS_ACTION	GENMASK(5, 4)
#define MT7620_PPE_TB_KEEPALIVE		GENMASK(13, 12)
#define MT7620_PPE_TB_HASH_MODE		GENMASK(15, 14)

#define MT7620_PPE_MISS_ONLY_CPU	2
#define MT7620_PPE_TABLE_4K		2
#define MT7620_PPE_HASH_MODE_1		1
#define MT7620_PPE_HASH_SEED_VALUE	0x12345678

#define MT7620_RX_DMA_FOE_ENTRY		GENMASK(13, 0)
#define MT7620_RX_DMA_CPU_REASON		GENMASK(18, 14)

#define MT7620_PPE_VLAN_MTU_NONE	GENMASK(13, 0)
#define MT7620_PPE_VLAN_MTU_1TAG	GENMASK(29, 16)
#define MT7620_PPE_VLAN_MTU_2TAG	GENMASK(13, 0)
#define MT7620_PPE_VLAN_MTU_3TAG	GENMASK(29, 16)
#define MT7620_PPE_VLAN_MTU_4TAG	GENMASK(13, 0)

#define MT7620_PPE_IB1_TIMESTAMP	GENMASK(14, 0)
#define MT7620_PPE_IB1_VLAN_LAYER	GENMASK(18, 16)
#define MT7620_PPE_IB1_PPPOE		BIT(19)
#define MT7620_PPE_IB1_KEEP_VPRI	BIT(20)
#define MT7620_PPE_IB1_KEEP_DSCP	BIT(21)
#define MT7620_PPE_IB1_CACHEABLE	BIT(22)
#define MT7620_PPE_IB1_TTL		BIT(24)
#define MT7620_PPE_IB1_PACKET_TYPE	GENMASK(27, 25)
#define MT7620_PPE_IB1_STATE		GENMASK(29, 28)
#define MT7620_PPE_IB1_UDP		BIT(30)
#define MT7620_PPE_IB1_STATIC		BIT(31)

#define MT7620_PPE_STATE_INVALID	0
#define MT7620_PPE_STATE_UNBIND		1
#define MT7620_PPE_STATE_BIND		2
#define MT7620_PPE_PACKET_IPV4_HNAPT	0
#define MT7620_PPE_PACKET_IPV6_5T	5

#define MT7620_PPE_MAX_VLANS		2

#define MT7620_PPE_IB2_FORCE_PORT	GENMASK(3, 0)
#define MT7620_PPE_IB2_PORT_METER	GENMASK(17, 12)
#define MT7620_PPE_IB2_PORT_ACCOUNT	GENMASK(23, 18)

#define MT7620_GSW_PSC(port)		(0x200c + ((port) * 0x100))
#define MT7620_GSW_PMCR(port)		(0x3000 + ((port) * 0x100))
#define MT7620_GSW_PSC_SA_DISABLE	BIT(4)
#define MT7620_GSW_PPE_PORT		7
#define MT7620_GSW_PPE_PMCR		0x0005e33b

struct mt7620_foe_entry {
	u32 data[20];
};

static_assert(sizeof(struct mt7620_foe_entry) == 80);

struct mt7620_ppe_flow_data {
	struct ethhdr eth;
	__be32 src_addr;
	__be32 dst_addr;
	struct in6_addr src_addr6;
	struct in6_addr dst_addr6;
	__be16 src_port;
	__be16 dst_port;
	u16 vlan_id[MT7620_PPE_MAX_VLANS];
	u16 pppoe_sid;
	u8 l4proto;
	u8 dsa_port;
	u8 vlan_count;
	bool ipv6;
	bool pppoe;
};

struct mt7620_ppe_flow {
	struct rhash_head node;
	struct list_head list;
	struct mt7620_ppe_flow_data key;
	struct mt7620_foe_entry foe;
	unsigned long cookie;
	unsigned long lastused;
	u16 hash;
};

struct mt7620_ppe {
	struct mtk_eth *eth;
	struct mt7620_gsw *gsw;
	struct mt7620_foe_entry *foe_table;
	dma_addr_t foe_phys;
	struct mt7620_ppe_flow **slots;
	struct rhashtable flows;
	struct list_head flow_list;
	struct list_head block_cb_list;
	/* Protects the flow state, FOE table and PPE lifecycle. */
	struct mutex lock;
	/* Serializes RX reason-15 requests with the process-context worker. */
	spinlock_t bind_lock;
	struct work_struct bind_work;
	DECLARE_BITMAP(bind_pending, MT7620_PPE_NUM_ENTRIES);
	bool bind_worker_active;
	bool running;
	bool udp_offload;
	u32 saved_pfc;
	u32 saved_gdm2_fwd_cfg;
	u32 saved_glo_cfg;
	u32 saved_udp_ctl;
	u32 saved_tpf[MT7620_GSW_LAST_USER_PORT + 1];
	u32 saved_psc7;
	u32 saved_pmcr7;
};

static const struct rhashtable_params mt7620_ppe_flow_ht_params = {
	.head_offset = offsetof(struct mt7620_ppe_flow, node),
	.key_offset = offsetof(struct mt7620_ppe_flow, cookie),
	.key_len = sizeof(unsigned long),
	.automatic_shrinking = true,
};

static u32 mt7620_ppe_r32(struct mt7620_ppe *ppe, u32 reg)
{
	return mtk_r32(ppe->eth, reg);
}

static void mt7620_ppe_w32(struct mt7620_ppe *ppe, u32 val, u32 reg)
{
	mtk_w32(ppe->eth, val, reg);
}

static void mt7620_ppe_m32(struct mt7620_ppe *ppe, u32 clear, u32 set,
			   u32 reg)
{
	mtk_m32(ppe->eth, clear, set, reg);
}

static u32 mt7620_ppe_hash(const struct mt7620_ppe_flow_data *data)
{
	u32 ports = be16_to_cpu(data->src_port) << 16 |
		    be16_to_cpu(data->dst_port);
	u32 hv1, hv2, hv3;
	u32 hash, low, high;

	if (data->ipv6) {
		hv1 = be32_to_cpu(data->src_addr6.s6_addr32[3]) ^
		      be32_to_cpu(data->dst_addr6.s6_addr32[3]) ^ ports;
		hv2 = be32_to_cpu(data->src_addr6.s6_addr32[2]) ^
		      be32_to_cpu(data->dst_addr6.s6_addr32[2]) ^
		      be32_to_cpu(data->dst_addr6.s6_addr32[0]);
		hv3 = be32_to_cpu(data->src_addr6.s6_addr32[1]) ^
		      be32_to_cpu(data->dst_addr6.s6_addr32[1]) ^
		      be32_to_cpu(data->src_addr6.s6_addr32[0]);
	} else {
		hv1 = ports;
		hv2 = be32_to_cpu(data->dst_addr);
		hv3 = be32_to_cpu(data->src_addr);
	}

	hash = (hv1 & hv2) | (~hv1 & hv3);
	low = hash & 0x00ffffff;
	high = hash & 0xff000000;
	hash = hv1 ^ hv2 ^ hv3 ^ (low << 8 | high >> 24);
	hash = (hash >> 16) ^ (hash & 0x000fffff);

	return (hash & MT7620_PPE_HASH_MASK) * MT7620_PPE_WAYS;
}

static bool
mt7620_ppe_flow_key_equal(const struct mt7620_ppe_flow_data *a,
			  const struct mt7620_ppe_flow_data *b)
{
	return a->src_port == b->src_port &&
		a->dst_port == b->dst_port &&
		a->l4proto == b->l4proto &&
		a->ipv6 == b->ipv6 &&
		(a->ipv6 ?
		 ipv6_addr_equal(&a->src_addr6, &b->src_addr6) &&
		 ipv6_addr_equal(&a->dst_addr6, &b->dst_addr6) :
		 a->src_addr == b->src_addr && a->dst_addr == b->dst_addr);
}

static void mt7620_ppe_cache_clear(struct mt7620_ppe *ppe)
{
	mt7620_ppe_m32(ppe, 0, BIT(9), MT7620_PPE_CACHE_CTL);
	mt7620_ppe_m32(ppe, BIT(9), 0, MT7620_PPE_CACHE_CTL);
}

static int mt7620_ppe_busy_wait(struct mt7620_ppe *ppe)
{
	struct mtk_eth *eth = ppe->eth;
	u32 val;
	int err;

	err = read_poll_timeout(mt7620_ppe_r32, val,
				!(val & MT7620_PPE_GLO_CFG_BUSY),
				20, USEC_PER_SEC, false,
				ppe, MT7620_PPE_GLO_CFG);
	if (!err)
		return 0;

	netdev_err(eth->netdev[0], "PPE busy timeout\n");
	if (!eth->rst_ppe)
		return err;

	err = reset_control_assert(eth->rst_ppe);
	if (err) {
		netdev_err(eth->netdev[0], "failed to assert PPE reset: %d\n",
			   err);
		return err;
	}
	usleep_range(60, 120);

	err = reset_control_deassert(eth->rst_ppe);
	if (err) {
		netdev_err(eth->netdev[0], "failed to deassert PPE reset: %d\n",
			   err);
		return err;
	}
	usleep_range(1000, 1200);

	netdev_warn(eth->netdev[0], "reset PPE after busy timeout\n");
	err = read_poll_timeout(mt7620_ppe_r32, val,
				!(val & MT7620_PPE_GLO_CFG_BUSY),
				20, USEC_PER_SEC, false,
				ppe, MT7620_PPE_GLO_CFG);
	if (err)
		netdev_err(eth->netdev[0],
			   "PPE remains busy after reset\n");

	return err;
}

static void mt7620_ppe_entry_clear(struct mt7620_ppe *ppe, u16 hash)
{
	WRITE_ONCE(ppe->foe_table[hash].data[0], 0);
	dma_wmb();
	mt7620_ppe_cache_clear(ppe);
}

static bool mt7620_ppe_entry_usable(struct mt7620_ppe *ppe, u16 hash)
{
	return !ppe->slots[hash] &&
	       !(READ_ONCE(ppe->foe_table[hash].data[0]) &
		 MT7620_PPE_IB1_STATIC);
}

static bool
mt7620_ppe_unbound_key_matches(const struct mt7620_foe_entry *entry,
			       const struct mt7620_ppe_flow *flow)
{
	u32 info = READ_ONCE(entry->data[0]);
	u32 expected = flow->foe.data[0];
	int last_word = flow->key.ipv6 ? 9 : 3;
	int i;

	if (FIELD_GET(MT7620_PPE_IB1_STATE, info) != MT7620_PPE_STATE_UNBIND ||
	    (info & (MT7620_PPE_IB1_PACKET_TYPE | MT7620_PPE_IB1_UDP)) !=
	    (expected & (MT7620_PPE_IB1_PACKET_TYPE | MT7620_PPE_IB1_UDP)))
		return false;

	dma_rmb();
	for (i = 1; i <= last_word; i++)
		if (READ_ONCE(entry->data[i]) != flow->foe.data[i])
			return false;

	return true;
}

static void mt7620_ppe_bind_work(struct work_struct *work)
{
	struct mt7620_ppe *ppe = container_of(work, struct mt7620_ppe,
					      bind_work);
	unsigned long flags;
	unsigned int hash;
	bool cache_clear = false;

	for (;;) {
		spin_lock_irqsave(&ppe->bind_lock, flags);
		hash = find_first_bit(ppe->bind_pending,
				      MT7620_PPE_NUM_ENTRIES);
		if (hash >= MT7620_PPE_NUM_ENTRIES) {
			ppe->bind_worker_active = false;
			spin_unlock_irqrestore(&ppe->bind_lock, flags);
			break;
		}
		__clear_bit(hash, ppe->bind_pending);
		spin_unlock_irqrestore(&ppe->bind_lock, flags);

		mutex_lock(&ppe->lock);
		if (ppe->running && ppe->slots[hash] &&
		    mt7620_ppe_unbound_key_matches(&ppe->foe_table[hash],
						   ppe->slots[hash])) {
			u32 info = ppe->slots[hash]->foe.data[0];

			info &= ~MT7620_PPE_IB1_TIMESTAMP;
			info |= READ_ONCE(ppe->foe_table[hash].data[0]) &
				MT7620_PPE_IB1_TIMESTAMP;
			WRITE_ONCE(ppe->foe_table[hash].data[0], info);
			dma_wmb();
			cache_clear = true;
		}
		if (cache_clear) {
			mt7620_ppe_cache_clear(ppe);
			cache_clear = false;
		}
		mutex_unlock(&ppe->lock);
	}
}

void mt7620_ppe_rx_process(struct mtk_eth *eth, u32 rxd4)
{
	struct mt7620_ppe *ppe = READ_ONCE(eth->mt7620_ppe);
	unsigned long flags;
	u16 hash;
	bool schedule = false;

	if (!ppe || FIELD_GET(MT7620_RX_DMA_CPU_REASON, rxd4) != 15)
		return;

	hash = FIELD_GET(MT7620_RX_DMA_FOE_ENTRY, rxd4);
	if (hash >= MT7620_PPE_NUM_ENTRIES)
		return;

	spin_lock_irqsave(&ppe->bind_lock, flags);
	if (!READ_ONCE(ppe->slots[hash])) {
		spin_unlock_irqrestore(&ppe->bind_lock, flags);
		return;
	}
	__set_bit(hash, ppe->bind_pending);
	if (!ppe->bind_worker_active) {
		ppe->bind_worker_active = true;
		schedule = true;
	}
	spin_unlock_irqrestore(&ppe->bind_lock, flags);

	if (schedule)
		schedule_work(&ppe->bind_work);
}

static void mt7620_ppe_table_reset(struct mt7620_ppe *ppe)
{
	static const u8 boundary_offsets[] = {
		12, 25, 38, 51, 76, 89, 102, 115,
	};
	unsigned long flags;
	int base, i;

	memset(ppe->foe_table, 0,
	       sizeof(*ppe->foe_table) * MT7620_PPE_NUM_ENTRIES);
	for (base = 0; base < MT7620_PPE_NUM_ENTRIES; base += 128)
		for (i = 0; i < ARRAY_SIZE(boundary_offsets); i++)
			WRITE_ONCE(ppe->foe_table[base + boundary_offsets[i]].data[0],
				   MT7620_PPE_IB1_STATIC);
	spin_lock_irqsave(&ppe->bind_lock, flags);
	bitmap_zero(ppe->bind_pending, MT7620_PPE_NUM_ENTRIES);
	spin_unlock_irqrestore(&ppe->bind_lock, flags);
	dma_wmb();
}

static void
mt7620_ppe_entry_commit(struct mt7620_ppe *ppe, u16 hash,
			const struct mt7620_foe_entry *entry)
{
	u32 info = entry->data[0];

	WRITE_ONCE(ppe->foe_table[hash].data[0], 0);
	memcpy(&ppe->foe_table[hash].data[1], &entry->data[1],
	       sizeof(entry->data) - sizeof(entry->data[0]));
	dma_wmb();
	WRITE_ONCE(ppe->foe_table[hash].data[0], info);
	dma_wmb();
	mt7620_ppe_cache_clear(ppe);
}

static void mt7620_ppe_set_mac(struct mt7620_foe_entry *entry, int word,
			       const u8 *src, const u8 *dst)
{
	entry->data[word] = get_unaligned_be32(dst);
	entry->data[word + 1] = (u32)get_unaligned_be16(dst + 4) << 16;
	entry->data[word + 2] = get_unaligned_be32(src);
	entry->data[word + 3] = (u32)get_unaligned_be16(src + 4) << 16;
}

static void
mt7620_ppe_set_l2(struct mt7620_foe_entry *entry, int word, __be16 proto,
		  const struct mt7620_ppe_flow_data *out)
{
	u16 etype = be16_to_cpu(proto);

	if (out->vlan_count)
		etype = ETH_P_8021Q;
	else if (out->pppoe)
		etype = ETH_P_PPP_SES;

	entry->data[word] = (u32)etype << 16;
	if (out->vlan_count)
		entry->data[word] |= out->vlan_id[0];

	mt7620_ppe_set_mac(entry, word + 1, out->eth.h_source,
			   out->eth.h_dest);
	if (out->vlan_count > 1)
		entry->data[word + 2] |= out->vlan_id[1];
	if (out->pppoe)
		entry->data[word + 4] |= out->pppoe_sid;
}

static void
mt7620_ppe_prepare_entry(struct mt7620_ppe *ppe,
			 struct mt7620_foe_entry *entry,
			 const struct mt7620_ppe_flow_data *key,
			 const struct mt7620_ppe_flow_data *out)
{
	int i;
	u32 timestamp;

	memset(entry, 0, sizeof(*entry));
	/*
	 * The free-running FOE timestamp is at FE + 0x10 on MT7620. This is
	 * also the FE interrupt-status offset in the older generic register
	 * map, so use the literal hardware offset rather than the enum.
	 */
	timestamp = mt7620_ppe_r32(ppe, 0x0010) &
		    MT7620_PPE_IB1_TIMESTAMP;
	entry->data[0] = FIELD_PREP(MT7620_PPE_IB1_TIMESTAMP, timestamp) |
			 MT7620_PPE_IB1_KEEP_DSCP |
			 MT7620_PPE_IB1_CACHEABLE |
			 MT7620_PPE_IB1_TTL |
			 FIELD_PREP(MT7620_PPE_IB1_PACKET_TYPE,
				    key->ipv6 ? MT7620_PPE_PACKET_IPV6_5T :
						MT7620_PPE_PACKET_IPV4_HNAPT) |
			 FIELD_PREP(MT7620_PPE_IB1_STATE,
				    MT7620_PPE_STATE_BIND);
	if (key->l4proto == IPPROTO_UDP)
		entry->data[0] |= MT7620_PPE_IB1_UDP;
	if (out->vlan_count)
		entry->data[0] |=
			FIELD_PREP(MT7620_PPE_IB1_VLAN_LAYER,
				   out->vlan_count) |
			MT7620_PPE_IB1_KEEP_VPRI;
	if (out->pppoe)
		entry->data[0] |= MT7620_PPE_IB1_PPPOE;

	if (key->ipv6) {
		for (i = 0; i < 4; i++) {
			entry->data[1 + i] =
				be32_to_cpu(key->src_addr6.s6_addr32[i]);
			entry->data[5 + i] =
				be32_to_cpu(key->dst_addr6.s6_addr32[i]);
		}
		entry->data[9] = be16_to_cpu(key->dst_port) |
				 be16_to_cpu(key->src_port) << 16;
		entry->data[14] =
			FIELD_PREP(MT7620_PPE_IB2_FORCE_PORT, out->dsa_port) |
			FIELD_PREP(MT7620_PPE_IB2_PORT_METER, 0x3f) |
			FIELD_PREP(MT7620_PPE_IB2_PORT_ACCOUNT, 0x3f);
		mt7620_ppe_set_l2(entry, 15, htons(ETH_P_IPV6), out);
	} else {
		entry->data[1] = be32_to_cpu(key->src_addr);
		entry->data[2] = be32_to_cpu(key->dst_addr);
		entry->data[3] = be16_to_cpu(key->dst_port) |
				 be16_to_cpu(key->src_port) << 16;
		entry->data[4] =
			FIELD_PREP(MT7620_PPE_IB2_FORCE_PORT, out->dsa_port) |
			FIELD_PREP(MT7620_PPE_IB2_PORT_METER, 0x3f) |
			FIELD_PREP(MT7620_PPE_IB2_PORT_ACCOUNT, 0x3f);
		entry->data[5] = be32_to_cpu(out->src_addr);
		entry->data[6] = be32_to_cpu(out->dst_addr);
		entry->data[7] = be16_to_cpu(out->dst_port) |
				 be16_to_cpu(out->src_port) << 16;
		mt7620_ppe_set_l2(entry, 11, htons(ETH_P_IP), out);
	}
}

static int
mt7620_ppe_path_to_conduit(struct mt7620_ppe *ppe, struct net_device *dev,
			   const u8 *dest,
			   struct mt7620_ppe_flow_data *out)
{
	struct net_device_path_stack stack;
	bool synth_pppoe = out && !out->pppoe;
	bool synth_vlan = out && !out->vlan_count;
	bool ralink_dsa = false;
	int err, i;

	if (dev == ppe->eth->netdev[0] && !out)
		return 0;

	rcu_read_lock();
	err = dev_fill_forward_path(dev, dest, &stack);
	rcu_read_unlock();
	if (err)
		return -EOPNOTSUPP;

	for (i = 0; i < stack.num_paths; i++) {
		switch (stack.path[i].type) {
		case DEV_PATH_VLAN:
			/*
			 * An indirect flowtable bound to a VLAN netdev receives
			 * packets after the VLAN layer has stripped the tag.  In
			 * that case netfilter deliberately emits no VLAN_PUSH action;
			 * reconstruct the egress tag from the forwarding path.
			 * A table bound below the VLAN layer already supplies the
			 * action, so do not duplicate it.
			 */
			if (!synth_vlan)
				break;
			if (stack.path[i].encap.proto != htons(ETH_P_8021Q) ||
			    out->vlan_count == MT7620_PPE_MAX_VLANS)
				return -EOPNOTSUPP;
			out->vlan_id[out->vlan_count++] =
				stack.path[i].encap.id;
			break;
		case DEV_PATH_PPPOE:
			if (!synth_pppoe)
				break;
			if (stack.path[i].encap.proto != htons(ETH_P_PPP_SES))
				return -EOPNOTSUPP;
			out->pppoe_sid = stack.path[i].encap.id;
			out->pppoe = true;
			synth_pppoe = false;
			break;
		case DEV_PATH_BRIDGE:
			/*
			 * Mirror nft_dev_path_info() when reconstructing VLAN actions.
			 * A bridge may add a tag, leave it unchanged, or cancel the
			 * preceding VLAN encapsulation for an untagged DSA port.
			 */
			if (!synth_vlan)
				break;
			switch (stack.path[i].bridge.vlan_mode) {
			case DEV_PATH_BR_VLAN_TAG:
				if (stack.path[i].bridge.vlan_proto !=
				    htons(ETH_P_8021Q) ||
				    out->vlan_count == MT7620_PPE_MAX_VLANS)
					return -EOPNOTSUPP;
				out->vlan_id[out->vlan_count++] =
					stack.path[i].bridge.vlan_id;
				break;
			case DEV_PATH_BR_VLAN_UNTAG:
			case DEV_PATH_BR_VLAN_UNTAG_HW:
				if (!out->vlan_count)
					return -EOPNOTSUPP;
				out->vlan_id[--out->vlan_count] = 0;
				break;
			case DEV_PATH_BR_VLAN_KEEP:
				break;
			default:
				return -EOPNOTSUPP;
			}
			break;
		case DEV_PATH_DSA:
			if (stack.path[i].dsa.proto != DSA_TAG_PROTO_RALINK)
				return -EOPNOTSUPP;
			if (stack.path[i].dsa.port < 0 ||
			    stack.path[i].dsa.port >
			    MT7620_GSW_LAST_USER_PORT)
				return -EOPNOTSUPP;
			if (out)
				out->dsa_port = stack.path[i].dsa.port;
			ralink_dsa = true;
			break;
		case DEV_PATH_ETHERNET:
			if (stack.path[i].dev != ppe->eth->netdev[0])
				return -EOPNOTSUPP;
			break;
		default:
			return -EOPNOTSUPP;
		}
	}

	return ralink_dsa ? 0 : -EOPNOTSUPP;
}

static void
mt7620_ppe_mangle_eth(const struct flow_action_entry *act, struct ethhdr *eth)
{
	void *dest = (u8 *)eth + act->mangle.offset;
	const void *src = &act->mangle.val;

	if (act->mangle.offset > 8)
		return;

	if (act->mangle.mask == 0xffff) {
		src += 2;
		dest += 2;
	}

	memcpy(dest, src, act->mangle.mask ? 2 : 4);
}

static int
mt7620_ppe_mangle_ports(const struct flow_action_entry *act,
			struct mt7620_ppe_flow_data *data)
{
	u32 val = ntohl(act->mangle.val);

	switch (act->mangle.offset) {
	case 0:
		if (act->mangle.mask == ~htonl(0xffff))
			data->dst_port = cpu_to_be16(val);
		else
			data->src_port = cpu_to_be16(val >> 16);
		break;
	case 2:
		data->dst_port = cpu_to_be16(val);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static int
mt7620_ppe_mangle_ipv4(const struct flow_action_entry *act,
		       struct mt7620_ppe_flow_data *data)
{
	__be32 *dest;

	switch (act->mangle.offset) {
	case offsetof(struct iphdr, saddr):
		dest = &data->src_addr;
		break;
	case offsetof(struct iphdr, daddr):
		dest = &data->dst_addr;
		break;
	default:
		return -EINVAL;
	}

	memcpy(dest, &act->mangle.val, sizeof(*dest));
	return 0;
}

static int
mt7620_ppe_flow_replace(struct mt7620_ppe *ppe, struct flow_cls_offload *f)
{
	struct flow_rule *rule = flow_cls_offload_flow_rule(f);
	struct mt7620_ppe_flow_data key = {}, out;
	struct mt7620_foe_entry foe;
	struct mt7620_ppe_flow *flow;
	struct flow_action_entry *act;
	struct net_device *idev = NULL;
	struct net_device *odev = NULL;
	u16 hash;
	int err, i;

	if (!ppe->running)
		return -ENETDOWN;

	if (rhashtable_lookup_fast(&ppe->flows, &f->cookie,
				   mt7620_ppe_flow_ht_params))
		return -EEXIST;

	if (!flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_META) ||
	    !flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_CONTROL) ||
	    !flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_BASIC) ||
	    !flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_PORTS))
		return -EOPNOTSUPP;

	{
		struct flow_match_meta match;

		flow_rule_match_meta(rule, &match);
		idev = dev_get_by_index(dev_net(ppe->eth->netdev[0]),
					match.key->ingress_ifindex);
		if (!idev)
			return -ENODEV;
		err = mt7620_ppe_path_to_conduit(ppe, idev, idev->dev_addr,
						 NULL);
		dev_put(idev);
		if (err)
			return err;
	}

	{
		struct flow_match_control match;

		flow_rule_match_control(rule, &match);
		if (match.key->addr_type != FLOW_DISSECTOR_KEY_IPV4_ADDRS &&
		    match.key->addr_type != FLOW_DISSECTOR_KEY_IPV6_ADDRS)
			return -EOPNOTSUPP;
		key.ipv6 = match.key->addr_type ==
			   FLOW_DISSECTOR_KEY_IPV6_ADDRS;
		if (flow_rule_has_control_flags(match.mask->flags, f->common.extack))
			return -EOPNOTSUPP;
	}

	{
		struct flow_match_basic match;

		flow_rule_match_basic(rule, &match);
		if (match.key->n_proto !=
		    htons(key.ipv6 ? ETH_P_IPV6 : ETH_P_IP) ||
		    (match.key->ip_proto != IPPROTO_TCP &&
		     match.key->ip_proto != IPPROTO_UDP))
			return -EOPNOTSUPP;
		if (match.key->ip_proto == IPPROTO_UDP && !ppe->udp_offload)
			return -EOPNOTSUPP;
		key.l4proto = match.key->ip_proto;
	}

	if (key.ipv6) {
		struct flow_match_ipv6_addrs match;

		if (!flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_IPV6_ADDRS))
			return -EOPNOTSUPP;
		flow_rule_match_ipv6_addrs(rule, &match);
		key.src_addr6 = match.key->src;
		key.dst_addr6 = match.key->dst;
	} else {
		struct flow_match_ipv4_addrs match;

		if (!flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_IPV4_ADDRS))
			return -EOPNOTSUPP;
		flow_rule_match_ipv4_addrs(rule, &match);
		key.src_addr = match.key->src;
		key.dst_addr = match.key->dst;
	}

	{
		struct flow_match_ports match;

		flow_rule_match_ports(rule, &match);
		key.src_port = match.key->src;
		key.dst_port = match.key->dst;
	}

	out = key;
	flow_action_for_each(i, act, &rule->action) {
		switch (act->id) {
		case FLOW_ACTION_MANGLE:
			switch (act->mangle.htype) {
			case FLOW_ACT_MANGLE_HDR_TYPE_ETH:
				mt7620_ppe_mangle_eth(act, &out.eth);
				break;
			case FLOW_ACT_MANGLE_HDR_TYPE_IP4:
				if (key.ipv6)
					return -EOPNOTSUPP;
				err = mt7620_ppe_mangle_ipv4(act, &out);
				if (err)
					return err;
				break;
			case FLOW_ACT_MANGLE_HDR_TYPE_TCP:
			case FLOW_ACT_MANGLE_HDR_TYPE_UDP:
				err = mt7620_ppe_mangle_ports(act, &out);
				if (err)
					return err;
				break;
			default:
				return -EOPNOTSUPP;
			}
			break;
		case FLOW_ACTION_REDIRECT:
			odev = act->dev;
			break;
		case FLOW_ACTION_CSUM:
			break;
		case FLOW_ACTION_VLAN_PUSH:
			if (out.vlan_count == MT7620_PPE_MAX_VLANS ||
			    act->vlan.proto != htons(ETH_P_8021Q))
				return -EOPNOTSUPP;
			out.vlan_id[out.vlan_count++] = act->vlan.vid;
			break;
		case FLOW_ACTION_VLAN_POP:
			break;
		case FLOW_ACTION_PPPOE_PUSH:
			if (out.pppoe)
				return -EOPNOTSUPP;
			out.pppoe_sid = act->pppoe.sid;
			out.pppoe = true;
			break;
		default:
			return -EOPNOTSUPP;
		}
	}

	if (out.vlan_count + out.pppoe > MT7620_PPE_MAX_VLANS)
		return -EOPNOTSUPP;

	if (!odev || !is_valid_ether_addr(out.eth.h_source) ||
	    !is_valid_ether_addr(out.eth.h_dest))
		return -EINVAL;

	err = mt7620_ppe_path_to_conduit(ppe, odev, out.eth.h_dest, &out);
	if (err)
		return err;

	hash = mt7620_ppe_hash(&key);
	for (i = 0; i < MT7620_PPE_WAYS; i++)
		if (ppe->slots[hash + i] &&
		    mt7620_ppe_flow_key_equal(&ppe->slots[hash + i]->key,
					      &key))
			return -EEXIST;
	for (i = 0; i < MT7620_PPE_WAYS; i++)
		if (mt7620_ppe_entry_usable(ppe, hash + i))
			break;
	if (i == MT7620_PPE_WAYS)
		return -ENOSPC;
	hash += i;

	flow = kzalloc(sizeof(*flow), GFP_KERNEL);
	if (!flow)
		return -ENOMEM;

	flow->cookie = f->cookie;
	flow->key = key;
	flow->hash = hash;
	flow->lastused = jiffies;
	err = rhashtable_insert_fast(&ppe->flows, &flow->node,
				     mt7620_ppe_flow_ht_params);
	if (err) {
		kfree(flow);
		return err;
	}

	WRITE_ONCE(ppe->slots[hash], flow);
	list_add_tail(&flow->list, &ppe->flow_list);
	mt7620_ppe_prepare_entry(ppe, &foe, &key, &out);
	flow->foe = foe;
	foe.data[0] &= ~MT7620_PPE_IB1_STATE;
	foe.data[0] |= FIELD_PREP(MT7620_PPE_IB1_STATE,
				  MT7620_PPE_STATE_UNBIND);
	mt7620_ppe_entry_commit(ppe, hash, &foe);
	netif_dbg(ppe->eth, hw, ppe->eth->netdev[0],
		  "PPE flow %lx: IPv%c hash %u, port %u\n",
		  flow->cookie, key.ipv6 ? '6' : '4', hash, out.dsa_port);

	return 0;
}

static int
mt7620_ppe_flow_destroy(struct mt7620_ppe *ppe, struct flow_cls_offload *f)
{
	struct mt7620_ppe_flow *flow;
	unsigned long flags;

	flow = rhashtable_lookup_fast(&ppe->flows, &f->cookie,
				      mt7620_ppe_flow_ht_params);
	if (!flow)
		return -ENOENT;

	netif_dbg(ppe->eth, hw, ppe->eth->netdev[0],
		  "remove PPE flow %lx, hash %u\n", flow->cookie, flow->hash);
	mt7620_ppe_entry_clear(ppe, flow->hash);
	WRITE_ONCE(ppe->slots[flow->hash], NULL);
	spin_lock_irqsave(&ppe->bind_lock, flags);
	__clear_bit(flow->hash, ppe->bind_pending);
	spin_unlock_irqrestore(&ppe->bind_lock, flags);
	list_del(&flow->list);
	rhashtable_remove_fast(&ppe->flows, &flow->node,
			       mt7620_ppe_flow_ht_params);
	kfree(flow);

	return 0;
}

static int
mt7620_ppe_flow_stats(struct mt7620_ppe *ppe, struct flow_cls_offload *f)
{
	struct mt7620_ppe_flow *flow;
	u32 info, now, timestamp, idle;

	flow = rhashtable_lookup_fast(&ppe->flows, &f->cookie,
				      mt7620_ppe_flow_ht_params);
	if (!flow)
		return -ENOENT;

	info = READ_ONCE(ppe->foe_table[flow->hash].data[0]);
	if (FIELD_GET(MT7620_PPE_IB1_STATE, info) == MT7620_PPE_STATE_BIND) {
		now = mt7620_ppe_r32(ppe, 0x0010) &
		      MT7620_PPE_IB1_TIMESTAMP;
		timestamp = FIELD_GET(MT7620_PPE_IB1_TIMESTAMP, info);
		idle = (now - timestamp) & MT7620_PPE_IB1_TIMESTAMP;
		flow->lastused = jiffies - min_t(u32, idle, 300) * HZ;
	}
	f->stats.lastused = flow->lastused;

	return 0;
}

static int
mt7620_ppe_flow_cmd(struct mt7620_ppe *ppe, struct flow_cls_offload *f)
{
	int err;

	mutex_lock(&ppe->lock);
	switch (f->command) {
	case FLOW_CLS_REPLACE:
		err = mt7620_ppe_flow_replace(ppe, f);
		break;
	case FLOW_CLS_DESTROY:
		err = mt7620_ppe_flow_destroy(ppe, f);
		break;
	case FLOW_CLS_STATS:
		err = mt7620_ppe_flow_stats(ppe, f);
		break;
	default:
		err = -EOPNOTSUPP;
		break;
	}
	mutex_unlock(&ppe->lock);

	return err;
}

static int
mt7620_ppe_setup_block_cb(enum tc_setup_type type, void *type_data,
			  void *cb_priv)
{
	struct mt7620_ppe *ppe = cb_priv;
	struct net_device *dev = ppe->eth->netdev[0];

	if (!tc_can_offload(dev) || !READ_ONCE(ppe->eth->mt7620_ppe))
		return -EOPNOTSUPP;
	if (type != TC_SETUP_CLSFLOWER)
		return -EOPNOTSUPP;

	return mt7620_ppe_flow_cmd(ppe, type_data);
}

static void mt7620_ppe_flush_locked(struct mt7620_ppe *ppe);

static int
mt7620_ppe_setup_block(struct net_device *dev, struct flow_block_offload *f)
{
	struct mtk_mac *mac = netdev_priv(dev);
	struct mt7620_ppe *ppe = mac->hw->mt7620_ppe;
	struct flow_block_cb *block_cb;
	flow_setup_cb_t *cb = mt7620_ppe_setup_block_cb;

	if (!ppe)
		return -EOPNOTSUPP;

	if (f->binder_type != FLOW_BLOCK_BINDER_TYPE_CLSACT_INGRESS)
		return -EOPNOTSUPP;

	f->driver_block_list = &ppe->block_cb_list;
	switch (f->command) {
	case FLOW_BLOCK_BIND:
		block_cb = flow_block_cb_lookup(f->block, cb, dev);
		if (block_cb) {
			flow_block_cb_incref(block_cb);
			return 0;
		}

		block_cb = flow_block_cb_alloc(cb, dev, ppe, NULL);
		if (IS_ERR(block_cb))
			return PTR_ERR(block_cb);

		flow_block_cb_incref(block_cb);
		flow_block_cb_add(block_cb, f);
		list_add_tail(&block_cb->driver_list, &ppe->block_cb_list);
		return 0;
	case FLOW_BLOCK_UNBIND:
		block_cb = flow_block_cb_lookup(f->block, cb, dev);
		if (!block_cb)
			return -ENOENT;

		if (!flow_block_cb_decref(block_cb)) {
			flow_block_cb_remove(block_cb, f);
			list_del(&block_cb->driver_list);
			if (list_empty(&ppe->block_cb_list) &&
			    mac->hw->mt7620_ppe) {
				mutex_lock(&ppe->lock);
				mt7620_ppe_flush_locked(ppe);
				mutex_unlock(&ppe->lock);
			}
		}
		return 0;
	default:
		return -EOPNOTSUPP;
	}
}

static int
mt7620_ppe_setup_indr_block(struct mt7620_ppe *ppe, struct net_device *dev,
			    struct Qdisc *sch, struct flow_block_offload *f,
			    void *data,
			    void (*cleanup)(struct flow_block_cb *block_cb))
{
	struct flow_block_cb *block_cb;
	flow_setup_cb_t *cb = mt7620_ppe_setup_block_cb;

	if (f->binder_type != FLOW_BLOCK_BINDER_TYPE_CLSACT_INGRESS)
		return -EOPNOTSUPP;

	f->driver_block_list = &ppe->block_cb_list;
	switch (f->command) {
	case FLOW_BLOCK_BIND:
		if (flow_block_cb_lookup(f->block, cb, dev))
			return 0;

		block_cb = flow_indr_block_cb_alloc(cb, dev, ppe, NULL, f,
						    dev, sch, data, ppe,
						    cleanup);
		if (IS_ERR(block_cb))
			return PTR_ERR(block_cb);

		flow_block_cb_add(block_cb, f);
		list_add_tail(&block_cb->driver_list, &ppe->block_cb_list);
		return 0;
	case FLOW_BLOCK_UNBIND:
		block_cb = flow_block_cb_lookup(f->block, cb, dev);
		if (!block_cb)
			return -ENOENT;

		flow_indr_block_cb_remove(block_cb, f);
		list_del(&block_cb->driver_list);
		if (list_empty(&ppe->block_cb_list) &&
		    READ_ONCE(ppe->eth->mt7620_ppe)) {
			mutex_lock(&ppe->lock);
			mt7620_ppe_flush_locked(ppe);
			mutex_unlock(&ppe->lock);
		}
		return 0;
	default:
		return -EOPNOTSUPP;
	}
}

static int
mt7620_ppe_indr_setup_tc_cb(struct net_device *dev, struct Qdisc *sch,
			    void *cb_priv, enum tc_setup_type type,
			    void *type_data, void *data,
			    void (*cleanup)(struct flow_block_cb *block_cb))
{
	struct mt7620_ppe *ppe = cb_priv;

	if (!dev || type != TC_SETUP_FT)
		return -EOPNOTSUPP;

	/* Only claim upper devices which resolve to this MT7620 DSA tree. */
	if (mt7620_ppe_path_to_conduit(ppe, dev, dev->dev_addr, NULL))
		return -EOPNOTSUPP;

	return mt7620_ppe_setup_indr_block(ppe, dev, sch, type_data, data,
					   cleanup);
}

int mt7620_ppe_setup_tc(struct net_device *dev, enum tc_setup_type type,
			void *type_data)
{
	if (type != TC_SETUP_FT)
		return -EOPNOTSUPP;

	return mt7620_ppe_setup_block(dev, type_data);
}

static void mt7620_ppe_flush_locked(struct mt7620_ppe *ppe)
{
	struct mt7620_ppe_flow *flow, *tmp;

	list_for_each_entry_safe(flow, tmp, &ppe->flow_list, list) {
		WRITE_ONCE(ppe->slots[flow->hash], NULL);
		list_del(&flow->list);
		rhashtable_remove_fast(&ppe->flows, &flow->node,
				       mt7620_ppe_flow_ht_params);
		kfree(flow);
	}
	mt7620_ppe_table_reset(ppe);
	mt7620_ppe_cache_clear(ppe);
}

static void mt7620_ppe_switch_start(struct mt7620_ppe *ppe)
{
	struct mt7620_gsw *gsw = ppe->gsw;
	u32 val;
	int port;

	mt7620_gsw_reg_lock(gsw);
	ppe->saved_pfc = mt7620_gsw_reg_read(gsw, MT7620_GSW_PFC);
	for (port = 0; port < ARRAY_SIZE(ppe->saved_tpf); port++)
		ppe->saved_tpf[port] =
			mt7620_gsw_reg_read(gsw, MT7620_GSW_TPF(port));
	ppe->saved_psc7 =
		mt7620_gsw_reg_read(gsw,
				    MT7620_GSW_PSC(MT7620_GSW_PPE_PORT));
	ppe->saved_pmcr7 =
		mt7620_gsw_reg_read(gsw,
				    MT7620_GSW_PMCR(MT7620_GSW_PPE_PORT));

	val = ppe->saved_pfc &
	      ~(MT7620_GSW_PFC_PPE_PORT | MT7620_GSW_PFC_PPE_ENABLE);
	val |= FIELD_PREP(MT7620_GSW_PFC_PPE_PORT, MT7620_GSW_PPE_PORT) |
	       MT7620_GSW_PFC_PPE_ENABLE;
	mt7620_gsw_reg_write(gsw, val, MT7620_GSW_PFC);

	mt7620_gsw_reg_write(gsw,
			     ppe->saved_psc7 | MT7620_GSW_PSC_SA_DISABLE,
			     MT7620_GSW_PSC(MT7620_GSW_PPE_PORT));
	mt7620_gsw_reg_write(gsw, MT7620_GSW_PPE_PMCR,
			     MT7620_GSW_PMCR(MT7620_GSW_PPE_PORT));
	mt7620_gsw_reg_unlock(gsw);

	for (port = 0; port < ARRAY_SIZE(ppe->saved_tpf); port++)
		mt7620_gsw_ppe_port_set(gsw, port, true);
}

static void mt7620_ppe_switch_stop(struct mt7620_ppe *ppe)
{
	struct mt7620_gsw *gsw = ppe->gsw;
	int port;

	mt7620_gsw_reg_lock(gsw);
	mt7620_gsw_reg_write(gsw, ppe->saved_pfc, MT7620_GSW_PFC);
	for (port = 0; port < ARRAY_SIZE(ppe->saved_tpf); port++)
		mt7620_gsw_reg_write(gsw, ppe->saved_tpf[port], MT7620_GSW_TPF(port));
	mt7620_gsw_reg_write(gsw, ppe->saved_psc7,
			     MT7620_GSW_PSC(MT7620_GSW_PPE_PORT));
	mt7620_gsw_reg_write(gsw, ppe->saved_pmcr7,
			     MT7620_GSW_PMCR(MT7620_GSW_PPE_PORT));
	mt7620_gsw_reg_unlock(gsw);
}

static void mt7620_ppe_program_mtu(struct mt7620_ppe *ppe)
{
	u32 base = ETH_HLEN + ppe->eth->netdev[0]->mtu;
	u32 val;

	val = FIELD_PREP(MT7620_PPE_VLAN_MTU_NONE, base) |
	      FIELD_PREP(MT7620_PPE_VLAN_MTU_1TAG, base + VLAN_HLEN);
	mt7620_ppe_w32(ppe, val, MT7620_PPE_VLAN_MTU0);

	val = FIELD_PREP(MT7620_PPE_VLAN_MTU_2TAG,
			 base + 2 * VLAN_HLEN) |
	      FIELD_PREP(MT7620_PPE_VLAN_MTU_3TAG,
			 base + 3 * VLAN_HLEN);
	mt7620_ppe_w32(ppe, val, MT7620_PPE_VLAN_MTU1);

	val = FIELD_PREP(MT7620_PPE_VLAN_MTU_4TAG, base + 4 * VLAN_HLEN);
	mt7620_ppe_w32(ppe, val, MT7620_PPE_VLAN_MTU2);
}

void mt7620_ppe_update_mtu(struct mtk_eth *eth)
{
	struct mt7620_ppe *ppe = eth->mt7620_ppe;

	if (!ppe)
		return;

	mutex_lock(&ppe->lock);
	if (ppe->running)
		mt7620_ppe_program_mtu(ppe);
	mutex_unlock(&ppe->lock);
}

static void mt7620_ppe_enable_locked(struct mt7620_ppe *ppe)
{
	struct mtk_eth *eth = ppe->eth;
	u32 cfg;

	if (ppe->running)
		return;
	if (mt7620_ppe_busy_wait(ppe))
		return;

	mt7620_ppe_table_reset(ppe);

	mt7620_ppe_w32(ppe, lower_32_bits(ppe->foe_phys), MT7620_PPE_TB_BASE);
	cfg = FIELD_PREP(MT7620_PPE_TB_SIZE, MT7620_PPE_TABLE_4K) |
	      MT7620_PPE_TB_ENTRY_80B |
	      FIELD_PREP(MT7620_PPE_TB_MISS_ACTION,
			 MT7620_PPE_MISS_ONLY_CPU) |
	      FIELD_PREP(MT7620_PPE_TB_KEEPALIVE, 0) |
	      FIELD_PREP(MT7620_PPE_TB_HASH_MODE,
			 MT7620_PPE_HASH_MODE_1);
	/* IPv4 and IPv6 share the global 80-byte HNAT v2 FOE table. */
	mt7620_ppe_w32(ppe, cfg, MT7620_PPE_TB_CFG);
	mt7620_ppe_w32(ppe, MT7620_PPE_HASH_SEED_VALUE, MT7620_PPE_HASH_SEED);
	mt7620_ppe_w32(ppe, 0xffffffff, MT7620_PPE_IP_PROT_CHK);

	mt7620_ppe_w32(ppe, 0x00020001, MT7620_PPE_FP_BMAP0);
	mt7620_ppe_w32(ppe, 0x00080004, MT7620_PPE_FP_BMAP1);
	mt7620_ppe_w32(ppe, 0x00200010, MT7620_PPE_FP_BMAP2);
	mt7620_ppe_w32(ppe, 0x00800040, MT7620_PPE_FP_BMAP3);
	mt7620_ppe_w32(ppe, 0x003f0000, MT7620_PPE_FP_BMAP4);
	mt7620_ppe_program_mtu(ppe);

	mt7620_ppe_w32(ppe, 100 | (50 << 16), MT7620_PPE_BIND_LMT0);
	mt7620_ppe_w32(ppe, 25, MT7620_PPE_BIND_LMT1);
	mt7620_ppe_w32(ppe, 30, MT7620_PPE_BNDR);
	mt7620_ppe_w32(ppe, (1000 << 16) | 3, MT7620_PPE_UNB_AGE);
	mt7620_ppe_w32(ppe, (5 << 16) | 5, MT7620_PPE_BND_AGE0);
	mt7620_ppe_w32(ppe, (5 << 16) | 5, MT7620_PPE_BND_AGE1);
	mt7620_ppe_w32(ppe, 0, MT7620_PPE_KA);

	mt7620_ppe_cache_clear(ppe);
	mt7620_ppe_m32(ppe, 0, BIT(0), MT7620_PPE_CACHE_CTL);
	ppe->saved_glo_cfg = mt7620_ppe_r32(ppe, MT7620_PPE_GLO_CFG) &
			     BIT(4);
	/* Preserve the CPU path for TTL expiry and ICMP Time Exceeded. */
	mt7620_ppe_m32(ppe, BIT(4), 0, MT7620_PPE_GLO_CFG);
	if (ppe->udp_offload) {
		ppe->saved_udp_ctl =
			mt7620_ppe_r32(ppe, MT7620_PPE_UDP_CTL) & BIT(30);
		mt7620_ppe_m32(ppe, BIT(30), 0, MT7620_PPE_UDP_CTL);
	}
	mt7620_ppe_w32(ppe, MT7620_PPE_FLOW_IPV4_NAT |
			   MT7620_PPE_FLOW_IPV4_NAPT |
			   MT7620_PPE_FLOW_IPV6_3T |
			   MT7620_PPE_FLOW_IPV6_5T,
			   MT7620_PPE_FLOW_SET);

	ppe->saved_gdm2_fwd_cfg =
		mt7620_ppe_r32(ppe, MT7620_PPE_GDM2_FWD_CFG);
	mt7620_ppe_m32(ppe, MT7620_PPE_GDM2_FWD_MASK, 0, MT7620_PPE_GDM2_FWD_CFG);
	mt7620_ppe_switch_start(ppe);
	ppe->running = true;
	netdev_info(eth->netdev[0],
		    "MT7620 PPE hardware flow offload enabled\n");
}

static void mt7620_ppe_disable_locked(struct mt7620_ppe *ppe)
{
	if (!ppe->running)
		return;

	mt7620_ppe_switch_stop(ppe);
	mt7620_ppe_w32(ppe, ppe->saved_gdm2_fwd_cfg, MT7620_PPE_GDM2_FWD_CFG);
	mt7620_ppe_busy_wait(ppe);
	mt7620_ppe_w32(ppe, 0, MT7620_PPE_FLOW_SET);
	mt7620_ppe_flush_locked(ppe);
	mt7620_ppe_m32(ppe, BIT(0), 0, MT7620_PPE_CACHE_CTL);
	mt7620_ppe_w32(ppe, 0, MT7620_PPE_TB_BASE);
	mt7620_ppe_m32(ppe, BIT(4), ppe->saved_glo_cfg, MT7620_PPE_GLO_CFG);
	if (ppe->udp_offload)
		mt7620_ppe_m32(ppe, BIT(30), ppe->saved_udp_ctl, MT7620_PPE_UDP_CTL);
	mt7620_ppe_busy_wait(ppe);
	ppe->running = false;
	netdev_info(ppe->eth->netdev[0],
		    "MT7620 PPE hardware flow offload disabled\n");
}

void mt7620_ppe_start(struct mtk_eth *eth)
{
	struct mt7620_ppe *ppe = eth->mt7620_ppe;

	if (!ppe)
		return;

	mutex_lock(&ppe->lock);
	mt7620_ppe_enable_locked(ppe);
	mutex_unlock(&ppe->lock);
}

void mt7620_ppe_stop(struct mtk_eth *eth)
{
	struct mt7620_ppe *ppe = eth->mt7620_ppe;

	if (!ppe)
		return;

	mutex_lock(&ppe->lock);
	mt7620_ppe_disable_locked(ppe);
	mutex_unlock(&ppe->lock);
}

void mt7620_ppe_deinit(struct mtk_eth *eth)
{
	struct mt7620_ppe *ppe = eth->mt7620_ppe;

	if (!ppe)
		return;

	flow_indr_dev_unregister(mt7620_ppe_indr_setup_tc_cb, ppe, NULL);
	mt7620_ppe_stop(eth);
	WRITE_ONCE(eth->mt7620_ppe, NULL);
	cancel_work_sync(&ppe->bind_work);
	rhashtable_destroy(&ppe->flows);
	mt7620_gsw_upstream_put(ppe->gsw);
}

int mt7620_ppe_init(struct mtk_eth *eth)
{
	struct mt7620_ppe *ppe;
	size_t size = sizeof(*ppe->foe_table) * MT7620_PPE_NUM_ENTRIES;
	int err;

	if (!eth->netdev[0])
		return 0;

	ppe = devm_kzalloc(eth->dev, sizeof(*ppe), GFP_KERNEL);
	if (!ppe)
		return -ENOMEM;

	ppe->gsw = mt7620_gsw_upstream_get(eth->dev);
	if (IS_ERR(ppe->gsw))
		return PTR_ERR(ppe->gsw);

	ppe->foe_table = dmam_alloc_coherent(eth->dev, size, &ppe->foe_phys,
					     GFP_KERNEL);
	if (!ppe->foe_table) {
		err = -ENOMEM;
		goto err_put_gsw;
	}

	ppe->slots = devm_kcalloc(eth->dev, MT7620_PPE_NUM_ENTRIES,
				  sizeof(*ppe->slots), GFP_KERNEL);
	if (!ppe->slots) {
		err = -ENOMEM;
		goto err_put_gsw;
	}

	ppe->eth = eth;
	ppe->udp_offload = mt7620_get_eco() >= 5;
	mutex_init(&ppe->lock);
	spin_lock_init(&ppe->bind_lock);
	INIT_WORK(&ppe->bind_work, mt7620_ppe_bind_work);
	INIT_LIST_HEAD(&ppe->flow_list);
	INIT_LIST_HEAD(&ppe->block_cb_list);
	err = rhashtable_init(&ppe->flows, &mt7620_ppe_flow_ht_params);
	if (err)
		goto err_put_gsw;

	WRITE_ONCE(eth->mt7620_ppe, ppe);
	err = flow_indr_dev_register(mt7620_ppe_indr_setup_tc_cb, ppe);
	if (err) {
		WRITE_ONCE(eth->mt7620_ppe, NULL);
		rhashtable_destroy(&ppe->flows);
		goto err_put_gsw;
	}

	dev_info(eth->dev, "allocated %zu KiB MT7620 PPE FOE table at %pad\n",
		 size / 1024, &ppe->foe_phys);
	if (!ppe->udp_offload)
		dev_info(eth->dev,
			 "UDP hardware flow offload disabled on ECO < 5\n");
	return 0;

err_put_gsw:
	mt7620_gsw_upstream_put(ppe->gsw);
	return err;
}
