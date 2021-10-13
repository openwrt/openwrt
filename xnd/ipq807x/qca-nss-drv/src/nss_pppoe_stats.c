/*
 **************************************************************************
 * Copyright (c) 2016-2020, The Linux Foundation. All rights reserved.
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
#include <nss_pppoe.h>
#include "nss_pppoe_stats.h"
#include "nss_pppoe_strings.h"

/*
 * Declare atomic notifier data structure for statistics.
 */
ATOMIC_NOTIFIER_HEAD(nss_pppoe_stats_notifier);

/*
 * Lock used for PPPoE statistics
 */
static DEFINE_SPINLOCK(nss_pppoe_stats_lock);

/*
 * PPPoE session stats structure for debug interface
 */
struct nss_pppoe_stats_session_stats {
	uint64_t stats[NSS_PPPOE_STATS_SESSION_MAX];
						/* stats for the session */
	int32_t if_index;	/* net device index for the session */
	uint32_t if_num;	/* nss interface number */
	bool valid;			/* dynamic interface valid flag */
};

/*
 * PPPoE interface stats structure for base node and sessions
 */
struct nss_pppoe_stats {
	uint64_t base_stats[NSS_PPPOE_STATS_BASE_MAX];
						/* Base node stats */
	struct nss_pppoe_stats_session_stats session_stats[NSS_MAX_PPPOE_DYNAMIC_INTERFACES];
					/* Per session stats */
};

/*
 * Global PPPoE stats decleration.
 */
static struct nss_pppoe_stats pppoe_stats;

/*
 * nss_pppoe_stats_read()
 *	Read pppoe statistics
 */
static ssize_t nss_pppoe_stats_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{

	uint32_t max_output_lines = 2 /* header & footer for session stats */
					+ NSS_MAX_PPPOE_DYNAMIC_INTERFACES * (NSS_PPPOE_STATS_SESSION_MAX + 2) /*session stats */
					+ 2 + NSS_PPPOE_STATS_BASE_MAX + 2;
	size_t size_al = NSS_STATS_MAX_STR_LENGTH * max_output_lines;
	size_t size_wr = 0;
	ssize_t bytes_read = 0;
	struct net_device *dev;
	int id;

	char *lbuf = kzalloc(size_al, GFP_KERNEL);
	if (unlikely(lbuf == NULL)) {
		nss_warning("Could not allocate memory for local statistics buffer");
		return 0;
	}

	/*
	 * Base node stats
	 */
	size_wr += nss_stats_print("pppoe", "pppoe base node stats start"
					, NSS_STATS_SINGLE_INSTANCE
					, nss_pppoe_strings_base_stats
					, pppoe_stats.base_stats
					, NSS_PPPOE_STATS_BASE_MAX
					, lbuf, size_wr, size_al);

	/*
	 * Session stats
	 */
	for (id = 0; id < NSS_MAX_PPPOE_DYNAMIC_INTERFACES; id++) {
		if (!pppoe_stats.session_stats[id].valid) {
			continue;
		}

		dev = dev_get_by_index(&init_net, pppoe_stats.session_stats[id].if_index);
		if (unlikely(!dev)) {
			continue;
		}

		size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "%d. nss interface id=%d, netdevice=%s\n", id,
				pppoe_stats.session_stats[id].if_num, dev->name);
		dev_put(dev);

		size_wr += nss_stats_print("pppoe", "pppoe session node stats"
						, id
						, nss_pppoe_strings_session_stats
						, pppoe_stats.session_stats[id].stats
						, NSS_PPPOE_STATS_SESSION_MAX
						, lbuf, size_wr, size_al);
	}

	bytes_read = simple_read_from_buffer(ubuf, sz, ppos, lbuf, size_wr);

	kfree(lbuf);
	return bytes_read;
}

/*
 * nss_pppoe_stats_pppoe_session_init()
 *	Initialize the session statistics.
 */
bool nss_pppoe_stats_pppoe_session_init(uint32_t if_num, struct net_device *dev)
{
	int i;

	spin_lock_bh(&nss_pppoe_stats_lock);
	for (i = 0; i < NSS_MAX_PPPOE_DYNAMIC_INTERFACES; i++) {
		if (!pppoe_stats.session_stats[i].valid) {
			pppoe_stats.session_stats[i].valid = true;
			pppoe_stats.session_stats[i].if_num = if_num;
			pppoe_stats.session_stats[i].if_index = dev->ifindex;
			spin_unlock_bh(&nss_pppoe_stats_lock);
			return true;
		}
	}
	spin_unlock_bh(&nss_pppoe_stats_lock);

	return false;
}

/*
 * nss_pppoe_stats_pppoe_session_deinit()
 *	De-initialize the session's stats.
 */
void nss_pppoe_stats_pppoe_session_deinit(uint32_t if_num)
{
	int i;

	spin_lock_bh(&nss_pppoe_stats_lock);
	for (i = 0; i < NSS_MAX_PPPOE_DYNAMIC_INTERFACES; i++) {
		if (pppoe_stats.session_stats[i].if_num == if_num) {
			memset(&pppoe_stats.session_stats[i], 0, sizeof(pppoe_stats.session_stats[i]));
		}
	}
	spin_unlock_bh(&nss_pppoe_stats_lock);
}

/*
 * nss_pppoe_stats_sync
 *	Per session debug stats for pppoe
 */
void nss_pppoe_stats_sync(struct nss_ctx_instance *nss_ctx, struct nss_pppoe_sync_stats_msg *stats_msg, uint16_t if_num)
{
	int i;
	spin_lock_bh(&nss_pppoe_stats_lock);
	for (i = 0; i < NSS_MAX_PPPOE_DYNAMIC_INTERFACES; i++) {
		if (pppoe_stats.session_stats[i].if_num == if_num) {
			int j;

			/*
			 * Sync PPPoE session stats.
			 */
			pppoe_stats.session_stats[i].stats[NSS_PPPOE_STATS_SESSION_RX_PACKETS] += stats_msg->session_stats.node.rx_packets;
			pppoe_stats.session_stats[i].stats[NSS_PPPOE_STATS_SESSION_RX_BYTES] += stats_msg->session_stats.node.rx_bytes;
			pppoe_stats.session_stats[i].stats[NSS_PPPOE_STATS_SESSION_TX_PACKETS] += stats_msg->session_stats.node.tx_packets;
			pppoe_stats.session_stats[i].stats[NSS_PPPOE_STATS_SESSION_TX_BYTES] += stats_msg->session_stats.node.tx_bytes;
			pppoe_stats.session_stats[i].stats[NSS_PPPOE_STATS_SESSION_WRONG_VERSION_OR_TYPE] += stats_msg->session_stats.exception[NSS_PPPOE_SESSION_EXCEPTION_EVENT_WRONG_VERSION_OR_TYPE];
			pppoe_stats.session_stats[i].stats[NSS_PPPOE_STATS_SESSION_WRONG_CODE] += stats_msg->session_stats.exception[NSS_PPPOE_SESSION_EXCEPTION_EVENT_WRONG_CODE];
			pppoe_stats.session_stats[i].stats[NSS_PPPOE_STATS_SESSION_UNSUPPORTED_PPP_PROTOCOL] += stats_msg->session_stats.exception[NSS_PPPOE_SESSION_EXCEPTION_EVENT_UNSUPPORTED_PPP_PROTOCOL];

			/*
			 * Sync PPPoE base node stats coming with this session's stats.
			 */
			pppoe_stats.base_stats[NSS_PPPOE_STATS_BASE_RX_PACKETS] += stats_msg->base_stats.node.rx_packets;
			pppoe_stats.base_stats[NSS_PPPOE_STATS_BASE_RX_BYTES] += stats_msg->base_stats.node.rx_bytes;
			pppoe_stats.base_stats[NSS_PPPOE_STATS_BASE_TX_PACKETS] += stats_msg->base_stats.node.tx_packets;
			pppoe_stats.base_stats[NSS_PPPOE_STATS_BASE_TX_BYTES] += stats_msg->base_stats.node.tx_bytes;

			for (j = 0; j < NSS_MAX_NUM_PRI; j++) {
				pppoe_stats.base_stats[NSS_PPPOE_STATS_BASE_RX_QUEUE_0_DROPPED + j] += stats_msg->base_stats.node.rx_dropped[j];
			}

			/*
			 * Sync PPPoE base exception stats coming with this session's stats.
			 */
			pppoe_stats.base_stats[NSS_PPPOE_STATS_BASE_SHORT_PPPOE_HDR_LENGTH] += stats_msg->base_stats.exception[NSS_PPPOE_BASE_EXCEPTION_EVENT_SHORT_PPPOE_HDR_LENGTH];
			pppoe_stats.base_stats[NSS_PPPOE_STATS_BASE_SHORT_PACKET_LENGTH] += stats_msg->base_stats.exception[NSS_PPPOE_BASE_EXCEPTION_EVENT_SHORT_PACKET_LENGTH];
			pppoe_stats.base_stats[NSS_PPPOE_STATS_BASE_WRONG_VERSION_OR_TYPE] += stats_msg->base_stats.exception[NSS_PPPOE_BASE_EXCEPTION_EVENT_WRONG_VERSION_OR_TYPE];
			pppoe_stats.base_stats[NSS_PPPOE_STATS_BASE_WRONG_CODE] += stats_msg->base_stats.exception[NSS_PPPOE_BASE_EXCEPTION_EVENT_WRONG_CODE];
			pppoe_stats.base_stats[NSS_PPPOE_STATS_BASE_UNSUPPORTED_PPP_PROTOCOL] += stats_msg->base_stats.exception[NSS_PPPOE_BASE_EXCEPTION_EVENT_UNSUPPORTED_PPP_PROTOCOL];
			pppoe_stats.base_stats[NSS_PPPOE_STATS_BASE_DISABLED_BRIDGE_PACKET] += stats_msg->base_stats.exception[NSS_PPPOE_BASE_EXCEPTION_EVENT_DISABLED_BRIDGE_PACKET];
			break;
		}
	}
	spin_unlock_bh(&nss_pppoe_stats_lock);
}

/*
 * nss_pppoe_stats_ops
 */
NSS_STATS_DECLARE_FILE_OPERATIONS(pppoe);

/*
 * nss_pppoe_stats_dentry_create()
 *	Create PPPoE node statistics debug entry.
 */
void nss_pppoe_stats_dentry_create(void)
{
	nss_stats_create_dentry("pppoe", &nss_pppoe_stats_ops);
}

/*
 * nss_pppoe_stats_notify()
 *	Sends notifications to the registered modules.
 *
 * Leverage NSS-FW statistics timing to update Netlink.
 */
void nss_pppoe_stats_notify(struct nss_ctx_instance *nss_ctx, uint32_t if_num)
{
	struct nss_pppoe_stats_notification nss_pppoe_stats;
	int id;

	for (id = 0; id < NSS_MAX_PPPOE_DYNAMIC_INTERFACES; id++) {
		if (pppoe_stats.session_stats[id].if_num == if_num) {
			memcpy(&nss_pppoe_stats.session_stats, &pppoe_stats.session_stats[id].stats, sizeof(nss_pppoe_stats.session_stats));
		}
	}
	memcpy(&nss_pppoe_stats.base_stats, &pppoe_stats.base_stats, sizeof(nss_pppoe_stats.base_stats));
	nss_pppoe_stats.core_id = nss_ctx->id;
	nss_pppoe_stats.if_num = if_num;
	atomic_notifier_call_chain(&nss_pppoe_stats_notifier, NSS_STATS_EVENT_NOTIFY, (void *)&nss_pppoe_stats);
}

/*
 * nss_pppoe_stats_register_notifier()
 *	Registers statistics notifier.
 */
int nss_pppoe_stats_register_notifier(struct notifier_block *nb)
{
	return atomic_notifier_chain_register(&nss_pppoe_stats_notifier, nb);
}
EXPORT_SYMBOL(nss_pppoe_stats_register_notifier);

/*
 * nss_pppoe_stats_unregister_notifier()
 *	Deregisters statistics notifier.
 */
int nss_pppoe_stats_unregister_notifier(struct notifier_block *nb)
{
	return atomic_notifier_chain_unregister(&nss_pppoe_stats_notifier, nb);
}
EXPORT_SYMBOL(nss_pppoe_stats_unregister_notifier);
