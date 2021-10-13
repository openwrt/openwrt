/*
 **************************************************************************
 * Copyright (c) 2019, The Linux Foundation. All rights reserved.
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 **************************************************************************
 */

#include "nss_core.h"
#include "nss_igs_stats.h"

/*
 * nss_igs_stats
 *	IGS debug statistics.
 */
enum nss_igs_stats {
	NSS_IGS_STATS_TX_DROP,
	NSS_IGS_STATS_SHAPER_DROP,
	NSS_IGS_STATS_IPV4_PARSE_FAIL,
	NSS_IGS_STATS_IPV4_UNKNOWN_GRE_TYPE,
	NSS_IGS_STATS_IPV4_UNKNOWN_L4,
	NSS_IGS_STATS_IPV4_NO_CME,
	NSS_IGS_STATS_IPV4_FRAG_INITIAL,
	NSS_IGS_STATS_IPV4_FRAG_NON_INITIAL,
	NSS_IGS_STATS_IPV4_MALFORMED_UDP,
	NSS_IGS_STATS_IPV4_MALFORMED_TCP,
	NSS_IGS_STATS_IPV4_MALFORMED_UDPL,
	NSS_IGS_STATS_IPV4_MALFORMED_GRE,
	NSS_IGS_STATS_IPV6_PARSE_FAIL,
	NSS_IGS_STATS_IPV6_UNKNOWN_L4,
	NSS_IGS_STATS_IPV6_NO_CME,
	NSS_IGS_STATS_IPV6_FRAG_INITIAL,
	NSS_IGS_STATS_IPV6_FRAG_NON_INITIAL,
	NSS_IGS_STATS_IPV6_MALFORMED_UDP,
	NSS_IGS_STATS_IPV6_MALFORMED_TCP,
	NSS_IGS_STATS_IPV6_MALFORMED_UDPL,
	NSS_IGS_STATS_IPV6_MALFORMED_FRAG,
	NSS_IGS_STATS_EVENT_NO_SI,
	NSS_IGS_STATS_ETH_PARSE_FAIL,
	NSS_IGS_STATS_ETH_UNKNOWN_TYPE,
	NSS_IGS_STATS_PPPOE_NON_IP,
	NSS_IGS_STATS_PPPOE_MALFORMED,
	NSS_IGS_STATS_MAX
};

/*
 * nss_igs_stats_debug_instance
 *	Stucture for H2N/N2H IGS debug stats
 */
static struct nss_igs_stats_debug_instance {
	uint64_t stats[NSS_IGS_STATS_MAX];	/* IGS statistics for each instance. */
	int32_t if_index;			/* IFB instance netdev index. */
	uint32_t if_num;			/* IFB instance NSS interface number */
	bool valid;				/* IFB statistics valid bit. */
} nss_igs_stats_debug[NSS_MAX_IGS_DYNAMIC_INTERFACES];

/*
 * Data structures to store IGS interface stats.
 */
static DEFINE_SPINLOCK(nss_igs_stats_debug_lock);

/*
 * nss_igs_stats_str
 *	IGS statistics strings for nss session stats
 */
struct nss_stats_info nss_igs_stats_str[NSS_IGS_STATS_MAX] = {
	{"IGS_SHAPER_TX_DROP"				, NSS_STATS_TYPE_DROP},
	{"IGS_SHAPER_DROP"				, NSS_STATS_TYPE_DROP},
	{"IGS_EXCEPTION_IPV4_PARSE_FAIL"		, NSS_STATS_TYPE_EXCEPTION},
	{"IGS_EXCEPTION_IPV4_UNKNOWN_GRE_TYPE"		, NSS_STATS_TYPE_EXCEPTION},
	{"IGS_EXCEPTION_IPV4_UNKNOWN_L4"		, NSS_STATS_TYPE_EXCEPTION},
	{"IGS_EXCEPTION_IPV4_NO_CME"			, NSS_STATS_TYPE_EXCEPTION},
	{"IGS_EXCEPTION_IPV4_FRAG_INITIAL"		, NSS_STATS_TYPE_EXCEPTION},
	{"IGS_EXCEPTION_IPV4_FRAG_NON_INITIAL"		, NSS_STATS_TYPE_EXCEPTION},
	{"IGS_EXCEPTION_IPV4_MALFORMED_UDP"		, NSS_STATS_TYPE_EXCEPTION},
	{"IGS_EXCEPTION_IPV4_MALFORMED_TCP"		, NSS_STATS_TYPE_EXCEPTION},
	{"IGS_EXCEPTION_IPV4_MALFORMED_UDPL"		, NSS_STATS_TYPE_EXCEPTION},
	{"IGS_EXCEPTION_IPV4_MALFORMED_GRE"		, NSS_STATS_TYPE_EXCEPTION},
	{"IGS_EXCEPTION_IPV6_PARSE_FAIL"		, NSS_STATS_TYPE_EXCEPTION},
	{"IGS_EXCEPTION_IPV6_UNKNOWN_L4"		, NSS_STATS_TYPE_EXCEPTION},
	{"IGS_EXCEPTION_IPV6_NO_CME"			, NSS_STATS_TYPE_EXCEPTION},
	{"IGS_EXCEPTION_IPV6_FRAG_INITIAL"		, NSS_STATS_TYPE_EXCEPTION},
	{"IGS_EXCEPTION_IPV6_FRAG_NON_INITIAL"		, NSS_STATS_TYPE_EXCEPTION},
	{"IGS_EXCEPTION_IPV6_MALFORMED_UDP"		, NSS_STATS_TYPE_EXCEPTION},
	{"IGS_EXCEPTION_IPV6_MALFORMED_TCP"		, NSS_STATS_TYPE_EXCEPTION},
	{"IGS_EXCEPTION_IPV6_MALFORMED_UDPL"		, NSS_STATS_TYPE_EXCEPTION},
	{"IGS_EXCEPTION_IPV6_MALFORMED_FRAG"		, NSS_STATS_TYPE_EXCEPTION},
	{"IGS_EXCEPTION_EVENT_NO_SI"			, NSS_STATS_TYPE_EXCEPTION},
	{"IGS_EXCEPTION_ETH_PARSE_FAIL"			, NSS_STATS_TYPE_EXCEPTION},
	{"IGS_EXCEPTION_ETH_UNKNOWN_TYPE"		, NSS_STATS_TYPE_EXCEPTION},
	{"IGS_EXCEPTION_PPPOE_NON_IP"			, NSS_STATS_TYPE_EXCEPTION},
	{"IGS_EXCEPTION_PPPOE_MALFORMED"		, NSS_STATS_TYPE_EXCEPTION}
};

/*
 * nss_igs_stats_get()
 *	Get IGS statistics.
 */
static void nss_igs_stats_get(void *stats_mem)
{
	struct nss_igs_stats_debug_instance *stats = (struct nss_igs_stats_debug_instance *)stats_mem;
	int i;

	if (!stats) {
		nss_warning("No memory to copy IGS stats");
		return;
	}

	spin_lock_bh(&nss_igs_stats_debug_lock);
	for (i = 0; i < NSS_MAX_IGS_DYNAMIC_INTERFACES; i++) {
		if (nss_igs_stats_debug[i].valid) {
			memcpy(stats, &nss_igs_stats_debug[i], sizeof(struct nss_igs_stats_debug_instance));
			stats++;
		}
	}
	spin_unlock_bh(&nss_igs_stats_debug_lock);
}

/*
 * nss_igs_stats_read()
 *	Read IGS statistics
 */
static ssize_t nss_igs_stats_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{

	uint32_t max_output_lines = 2 /* header & footer for instance stats */
					+ NSS_MAX_IGS_DYNAMIC_INTERFACES *
					 ((NSS_STATS_NODE_MAX + 3 ) + (NSS_IGS_STATS_MAX + 3)) /*instance stats */
					 + 2;
	size_t size_al = NSS_STATS_MAX_STR_LENGTH * max_output_lines;
	size_t size_wr = 0;
	ssize_t bytes_read = 0;
	struct net_device *dev;
	struct nss_igs_stats_debug_instance *igs_shadow_stats;
	int id;

	char *lbuf = kzalloc(size_al, GFP_KERNEL);
	if (unlikely(!lbuf)) {
		nss_warning("Could not allocate memory for local statistics buffer");
		return 0;
	}

	igs_shadow_stats = kzalloc(sizeof(struct nss_igs_stats_debug_instance) *
			 NSS_MAX_IGS_DYNAMIC_INTERFACES, GFP_KERNEL);
	if (unlikely(!igs_shadow_stats)) {
		nss_warning("Could not allocate memory for base debug statistics buffer");
		kfree(lbuf);
		return 0;
	}

	/*
	 * Get all stats
	 */
	nss_igs_stats_get((void *)igs_shadow_stats);
	size_wr += nss_stats_banner(lbuf, size_wr, size_al, "igs", NSS_STATS_SINGLE_CORE);

	/*
	 * Session stats
	 */
	for (id = 0; id < NSS_MAX_IGS_DYNAMIC_INTERFACES; id++) {

			if (!igs_shadow_stats[id].valid) {
				continue;
			}

			dev = dev_get_by_index(&init_net, igs_shadow_stats[id].if_index);
			if (likely(dev)) {

				size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "%d. nss interface id=%d, netdevice=%s\n", id,
						igs_shadow_stats[id].if_num, dev->name);
				dev_put(dev);
			} else {
				size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "%d. nss interface id=%d\n", id,
						igs_shadow_stats[id].if_num);
			}
			size_wr += nss_stats_fill_common_stats(igs_shadow_stats[id].if_num, id, lbuf, size_wr, size_al, "igs");

			/*
			 * IGS exception stats.
			 */
			size_wr += nss_stats_print("igs", "igs exception stats start"
							, id
							, nss_igs_stats_str
							, igs_shadow_stats[id].stats
							, NSS_IGS_STATS_MAX
							, lbuf, size_wr, size_al);
			size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "\n");
	}

	bytes_read = simple_read_from_buffer(ubuf, sz, ppos, lbuf, size_wr);

	kfree(igs_shadow_stats);
	kfree(lbuf);
	return bytes_read;
}

/*
 * nss_igs_stats_sync
 *	API to sync statistics for IGS
 */
void nss_igs_stats_sync(struct nss_ctx_instance *nss_ctx,
		 struct nss_cmn_msg *ncm, uint16_t if_num)
{
	uint8_t i, j;
	struct nss_top_instance *nss_top = nss_ctx->nss_top;
	struct nss_igs_msg *nim = (struct nss_igs_msg *)ncm;
	struct nss_igs_stats_sync_msg *stats_msg = &nim->msg.stats;
	struct nss_cmn_node_stats *node_stats_ptr = &stats_msg->node_stats;
	uint32_t *igs_stats_ptr = (uint32_t *)&stats_msg->igs_stats;

	spin_lock_bh(&nss_igs_stats_debug_lock);
	for (i = 0; i < NSS_MAX_IGS_DYNAMIC_INTERFACES; i++) {
		if (nss_igs_stats_debug[i].if_num != if_num) {
			continue;
		}

		for (j = 0; j < NSS_IGS_STATS_MAX; j++) {
			/*
			 * sync stats.
			 */
			nss_igs_stats_debug[i].stats[j] += igs_stats_ptr[j];
		}
		spin_unlock_bh(&nss_igs_stats_debug_lock);
		goto sync_cmn_stats;
	}

	spin_unlock_bh(&nss_igs_stats_debug_lock);
	return;

sync_cmn_stats:
	spin_lock_bh(&nss_top->stats_lock);

	/*
	 * sync common stats.
	 */
	nss_top->stats_node[if_num][NSS_STATS_NODE_RX_PKTS] += node_stats_ptr->rx_packets;
	nss_top->stats_node[if_num][NSS_STATS_NODE_RX_BYTES] += node_stats_ptr->rx_bytes;
	nss_top->stats_node[if_num][NSS_STATS_NODE_TX_PKTS] += node_stats_ptr->tx_packets;
	nss_top->stats_node[if_num][NSS_STATS_NODE_TX_BYTES] += node_stats_ptr->tx_bytes;

	for (i = 0; i < NSS_MAX_NUM_PRI; i++) {
		nss_top->stats_node[if_num][NSS_STATS_NODE_RX_QUEUE_0_DROPPED + i] +=
			node_stats_ptr->rx_dropped[i];
	}

	spin_unlock_bh(&nss_top->stats_lock);
}

/*
 * nss_igs_stats_reset()
 *	API to reset the IGS stats.
 */
void nss_igs_stats_reset(uint32_t if_num)
{
	uint8_t i;

	spin_lock_bh(&nss_igs_stats_debug_lock);
	for (i = 0; i < NSS_MAX_IGS_DYNAMIC_INTERFACES; i++) {
		if (nss_igs_stats_debug[i].if_num == if_num) {
			memset(&nss_igs_stats_debug[i], 0, sizeof(struct nss_igs_stats_debug_instance));
			break;
		}
	}
	spin_unlock_bh(&nss_igs_stats_debug_lock);
}

/*
 * nss_igs_stats_init()
 *	API to initialize IGS debug instance statistics.
 */
void nss_igs_stats_init(uint32_t if_num, struct net_device *netdev)
{
	uint8_t i;

	spin_lock_bh(&nss_igs_stats_debug_lock);
	for (i = 0; i < NSS_MAX_IGS_DYNAMIC_INTERFACES; i++) {
		if (!nss_igs_stats_debug[i].valid) {
			nss_igs_stats_debug[i].valid = true;
			nss_igs_stats_debug[i].if_num = if_num;
			nss_igs_stats_debug[i].if_index = netdev->ifindex;
			break;
		}
	}
	spin_unlock_bh(&nss_igs_stats_debug_lock);
}

/*
 * nss_igs_stats_ops
 */
NSS_STATS_DECLARE_FILE_OPERATIONS(igs)

/*
 * nss_igs_stats_dentry_create()
 *	Create igs statistics debug entry.
 */
void nss_igs_stats_dentry_create(void)
{
	nss_stats_create_dentry("igs", &nss_igs_stats_ops);
}
