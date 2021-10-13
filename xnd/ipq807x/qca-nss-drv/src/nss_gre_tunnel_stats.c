/*
 ****************************************************************************
 * Copyright (c) 2017, 2020-2021, The Linux Foundation. All rights reserved.
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
 ****************************************************************************
 */

#include "nss_tx_rx_common.h"
#include "nss_gre_tunnel.h"
#include "nss_gre_tunnel_stats.h"
#include "nss_gre_tunnel_strings.h"

/*
 * Declare atomic notifier data structure for statistics.
 */
ATOMIC_NOTIFIER_HEAD(nss_gre_tunnel_stats_notifier);

/*
 * Spinlock to protect gre tunnel statistics update/read
 */
DEFINE_SPINLOCK(nss_gre_tunnel_stats_lock);

struct nss_gre_tunnel_stats_session session_stats[NSS_MAX_GRE_TUNNEL_SESSIONS];

/*
 * nss_gre_tunnel_stats_session_sync()
 *	Sync function for GRE Tunnel statistics
 */
void nss_gre_tunnel_stats_session_sync(struct nss_ctx_instance *nss_ctx, struct nss_gre_tunnel_stats *stats_msg,
					uint16_t if_num)
{
	int i;
	struct nss_gre_tunnel_stats_session *s = NULL;

	NSS_VERIFY_CTX_MAGIC(nss_ctx);

	spin_lock_bh(&nss_gre_tunnel_stats_lock);
	for (i = 0; i < NSS_MAX_GRE_TUNNEL_SESSIONS; i++) {
		if (session_stats[i].if_num == if_num) {
			s = &session_stats[i];
			break;
		}
	}

	if (!s) {
		spin_unlock_bh(&nss_gre_tunnel_stats_lock);
		nss_warning("%px: Session not found: %u", nss_ctx, if_num);
		return;
	}

	s->stats[NSS_GRE_TUNNEL_STATS_SESSION_RX_PKTS] += stats_msg->node_stats.rx_packets;
	s->stats[NSS_GRE_TUNNEL_STATS_SESSION_TX_PKTS] += stats_msg->node_stats.tx_packets;
	for (i = 0; i < NSS_MAX_NUM_PRI; i++) {
		s->stats[NSS_GRE_TUNNEL_STATS_SESSION_RX_QUEUE_0_DROPPED + i] += stats_msg->node_stats.rx_dropped[i];
	}
	s->stats[NSS_GRE_TUNNEL_STATS_SESSION_RX_MALFORMED] += stats_msg->rx_malformed;
	s->stats[NSS_GRE_TUNNEL_STATS_SESSION_RX_INVALID_PROT] += stats_msg->rx_invalid_prot;
	s->stats[NSS_GRE_TUNNEL_STATS_SESSION_DECAP_QUEUE_FULL] += stats_msg->decap_queue_full;
	s->stats[NSS_GRE_TUNNEL_STATS_SESSION_RX_SINGLE_REC_DGRAM] += stats_msg->rx_single_rec_dgram;
	s->stats[NSS_GRE_TUNNEL_STATS_SESSION_RX_INVALID_REC_DGRAM] += stats_msg->rx_invalid_rec_dgram;
	s->stats[NSS_GRE_TUNNEL_STATS_SESSION_BUFFER_ALLOC_FAIL] += stats_msg->buffer_alloc_fail;
	s->stats[NSS_GRE_TUNNEL_STATS_SESSION_BUFFER_COPY_FAIL] += stats_msg->buffer_copy_fail;
	s->stats[NSS_GRE_TUNNEL_STATS_SESSION_OUTFLOW_QUEUE_FULL] += stats_msg->outflow_queue_full;
	s->stats[NSS_GRE_TUNNEL_STATS_SESSION_RX_DROPPED_HROOM] += stats_msg->rx_dropped_hroom;
	s->stats[NSS_GRE_TUNNEL_STATS_SESSION_RX_CBUFFER_ALLOC_FAIL] += stats_msg->rx_cbuf_alloc_fail;
	s->stats[NSS_GRE_TUNNEL_STATS_SESSION_RX_CENQUEUE_FAIL] += stats_msg->rx_cenqueue_fail;
	s->stats[NSS_GRE_TUNNEL_STATS_SESSION_RX_DECRYPT_DONE] += stats_msg->rx_decrypt_done;
	s->stats[NSS_GRE_TUNNEL_STATS_SESSION_RX_FORWARD_ENQUEUE_FAIL] += stats_msg->rx_forward_enqueue_fail;
	s->stats[NSS_GRE_TUNNEL_STATS_SESSION_TX_CBUFFER_ALLOC_FAIL] += stats_msg->tx_cbuf_alloc_fail;
	s->stats[NSS_GRE_TUNNEL_STATS_SESSION_TX_CENQUEUE_FAIL] += stats_msg->tx_cenqueue_fail;
	s->stats[NSS_GRE_TUNNEL_STATS_SESSION_RX_DROPPED_TROOM] += stats_msg->rx_dropped_troom;
	s->stats[NSS_GRE_TUNNEL_STATS_SESSION_TX_FORWARD_ENQUEUE_FAIL] += stats_msg->tx_forward_enqueue_fail;
	s->stats[NSS_GRE_TUNNEL_STATS_SESSION_TX_CIPHER_DONE] += stats_msg->tx_cipher_done;
	s->stats[NSS_GRE_TUNNEL_STATS_SESSION_CRYPTO_NOSUPP] += stats_msg->crypto_nosupp;
	s->stats[NSS_GRE_TUNNEL_STATS_SESSION_RX_DROPPED_MH_VERSION] += stats_msg->rx_dropped_mh_ver;
	s->stats[NSS_GRE_TUNNEL_STATS_SESSION_RX_UNALIGNED_PKT] += stats_msg->rx_unaligned_pkt;

	/*
	 * Copy crypto resp err stats.
	 */
	for (i = 0; i < NSS_CRYPTO_CMN_RESP_ERROR_MAX; i++) {
#if defined(NSS_HAL_IPQ807x_SUPPORT)
		s->stats[NSS_GRE_TUNNEL_STATS_SESSION_MAX + i] += stats_msg->crypto_resp_error[i];
#else
		s->stats[NSS_GRE_TUNNEL_STATS_SESSION_MAX + i] = 0;
#endif
	}

	spin_unlock_bh(&nss_gre_tunnel_stats_lock);
}

/*
 * nss_gre_tunnel_stats_session_get()
 *	Get session GRE Tunnel statitics.
 */
static void nss_gre_tunnel_stats_session_get(struct nss_gre_tunnel_stats_session *stats)
{
	int i;

	if (!stats) {
		nss_warning("No memory to copy gre_tunnel session stats");
		return;
	}

	spin_lock_bh(&nss_gre_tunnel_stats_lock);
	for (i = 0; i < NSS_MAX_GRE_TUNNEL_SESSIONS; i++) {
		if (session_stats[i].valid) {
			memcpy(stats, &session_stats[i],
				sizeof(struct nss_gre_tunnel_stats_session));
			stats++;
		}
	}
	spin_unlock_bh(&nss_gre_tunnel_stats_lock);
}

/*
 * nss_gre_tunnel_stats_read()
 *	Read GRE Tunnel session statistics
 */
static ssize_t nss_gre_tunnel_stats_read(struct file *fp, char __user *ubuf,
					size_t sz, loff_t *ppos)
{
	uint32_t max_output_lines = 2 + (NSS_MAX_GRE_TUNNEL_SESSIONS
					* (NSS_GRE_TUNNEL_STATS_SESSION_MAX + 2)) + 2;
	size_t size_al = NSS_STATS_MAX_STR_LENGTH * max_output_lines;
	size_t size_wr = 0;
	ssize_t bytes_read = 0;
	uint64_t *stats_shadow;
	struct net_device *dev;
	int id, i;
	struct nss_gre_tunnel_stats_session *gre_tunnel_session_stats = NULL;

	char *lbuf = kzalloc(size_al, GFP_KERNEL);
	if (unlikely(lbuf == NULL)) {
		nss_warning("Could not allocate memory for local statistics buffer");
		return 0;
	}

	stats_shadow = kzalloc(NSS_CRYPTO_CMN_RESP_ERROR_MAX * 8, GFP_KERNEL);
	if (unlikely(!stats_shadow)) {
		nss_warning("Could not allocate memory for local shadow buffer");
		kfree(lbuf);
		return 0;
	}

	gre_tunnel_session_stats = kzalloc((sizeof(struct nss_gre_tunnel_stats_session)
						* NSS_MAX_GRE_TUNNEL_SESSIONS), GFP_KERNEL);
	if (unlikely(gre_tunnel_session_stats == NULL)) {
		nss_warning("Could not allocate memory for populating GRE Tunnel stats");
		kfree(lbuf);
		kfree(stats_shadow);
		return 0;
	}

	/*
	 * Get all stats
	 */
	nss_gre_tunnel_stats_session_get(gre_tunnel_session_stats);

	/*
	 * Session stats
	 */
	size_wr += nss_stats_banner(lbuf, size_wr, size_al, "GRE tunnel stats", NSS_STATS_SINGLE_CORE);

	for (id = 0; id < NSS_MAX_GRE_TUNNEL_SESSIONS; id++) {
		if (!gre_tunnel_session_stats[id].valid)
			break;

		dev = dev_get_by_index(&init_net, gre_tunnel_session_stats[id].if_index);
		if (likely(dev)) {
			size_wr += scnprintf(lbuf + size_wr, size_al - size_wr,
						"%d. nss interface id=%d, netdevice=%s\n",
						id, gre_tunnel_session_stats[id].if_num,
						dev->name);
			dev_put(dev);
		} else {
			size_wr += scnprintf(lbuf + size_wr, size_al - size_wr,
						"%d. nss interface id=%d\n", id,
						gre_tunnel_session_stats[id].if_num);
		}

		size_wr += nss_stats_print("gre_tunnel", NULL, NSS_STATS_SINGLE_INSTANCE,
					nss_gre_tunnel_strings_stats, gre_tunnel_session_stats[id].stats,
					NSS_GRE_TUNNEL_STATS_SESSION_MAX, lbuf, size_wr, size_al);

		/*
		 * Print crypto resp err stats.
		 * TODO: We are not printing with the right enum string for crypto. This
		 * is intentional since we atleast want to see some stats for now.
		 */
		spin_lock_bh(&nss_gre_tunnel_stats_lock);
		for (i = 0; i < NSS_CRYPTO_CMN_RESP_ERROR_MAX; i++) {
			stats_shadow[i] = gre_tunnel_session_stats[id].stats[NSS_GRE_TUNNEL_STATS_SESSION_MAX + i];
		}

		spin_unlock_bh(&nss_gre_tunnel_stats_lock);
		size_wr += nss_stats_print("gre_tunnel", NULL, NSS_STATS_SINGLE_INSTANCE,
					nss_gre_tunnel_strings_stats, stats_shadow,
					NSS_CRYPTO_CMN_RESP_ERROR_MAX, lbuf, size_wr, size_al);

		size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "\n");
	}

	bytes_read = simple_read_from_buffer(ubuf, sz, ppos, lbuf, size_wr);

	kfree(gre_tunnel_session_stats);
	kfree(lbuf);
	kfree(stats_shadow);
	return bytes_read;
}

/*
 * nss_gre_tunnel_stats_ops
 */
NSS_STATS_DECLARE_FILE_OPERATIONS(gre_tunnel)

/*
 * nss_gre_tunnel_stats_dentry_create()
 *	Create gre tunnel statistics debug entry.
 */
void nss_gre_tunnel_stats_dentry_create(void)
{
	nss_stats_create_dentry("gre_tunnel", &nss_gre_tunnel_stats_ops);
}

/*
 * nss_gre_tunnel_stats_notify()
 *	Sends notifications to all the registered modules.
 *
 * Leverage NSS-FW statistics timing to update Netlink.
 */
void nss_gre_tunnel_stats_notify(struct nss_ctx_instance *nss_ctx, uint32_t if_num)
{
	struct nss_gre_tunnel_stats_notification gre_tunnel_stats;
	struct nss_gre_tunnel_stats_session *s = NULL;
	int i;

	spin_lock_bh(&nss_gre_tunnel_stats_lock);
	for (i = 0; i < NSS_MAX_GRE_TUNNEL_SESSIONS; i++) {
		if (session_stats[i].if_num != if_num) {
			continue;
		}

		s = &session_stats[i];
		gre_tunnel_stats.core_id = nss_ctx->id;
		gre_tunnel_stats.if_num = if_num;
		memcpy(gre_tunnel_stats.stats_ctx, s->stats, sizeof(gre_tunnel_stats.stats_ctx));
		spin_unlock_bh(&nss_gre_tunnel_stats_lock);
		atomic_notifier_call_chain(&nss_gre_tunnel_stats_notifier, NSS_STATS_EVENT_NOTIFY, &gre_tunnel_stats);
		return;
	}
	spin_unlock_bh(&nss_gre_tunnel_stats_lock);
}

/*
 * nss_gre_tunnel_stats_unregister_notifier()
 *	Deregisters statistics notifier.
 */
int nss_gre_tunnel_stats_unregister_notifier(struct notifier_block *nb)
{
	return atomic_notifier_chain_unregister(&nss_gre_tunnel_stats_notifier, nb);
}
EXPORT_SYMBOL(nss_gre_tunnel_stats_unregister_notifier);

/*
 * nss_gre_tunnel_stats_register_notifier()
 *	Registers statistics notifier.
 */
int nss_gre_tunnel_stats_register_notifier(struct notifier_block *nb)
{
	return atomic_notifier_chain_register(&nss_gre_tunnel_stats_notifier, nb);
}
EXPORT_SYMBOL(nss_gre_tunnel_stats_register_notifier);
