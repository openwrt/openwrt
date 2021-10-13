/*
 **************************************************************************
 * Copyright (c) 2017,2019-2020, The Linux Foundation. All rights reserved.
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
#include "nss_l2tpv2_stats.h"
#include "nss_l2tpv2_strings.h"

/*
 * Declare atomic notifier data structure for statistics.
 */
ATOMIC_NOTIFIER_HEAD(nss_l2tpv2_stats_notifier);

/*
 * nss_l2tpv2_stats_read()
 *	Read l2tpv2 statistics.
 */
static ssize_t nss_l2tpv2_stats_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	/*
	 * Max output lines = #stats * NSS_MAX_CORES  +
	 * Few output lines for banner printing + Number of Extra outputlines for future reference to add new stats.
	 */
	uint32_t max_output_lines = NSS_MAX_L2TPV2_DYNAMIC_INTERFACES * (NSS_L2TPV2_STATS_SESSION_MAX + 2) /*session stats */
					+ NSS_STATS_EXTRA_OUTPUT_LINES;
	size_t size_al = NSS_STATS_MAX_STR_LENGTH * max_output_lines ;
	size_t size_wr = 0;
	ssize_t bytes_read = 0;
	struct net_device *dev;
	struct nss_l2tpv2_stats_session_debug l2tpv2_session_stats[NSS_MAX_L2TPV2_DYNAMIC_INTERFACES];
	int id;

	char *lbuf = kzalloc(size_al, GFP_KERNEL);
	if (unlikely(lbuf == NULL)) {
		nss_warning("Could not allocate memory for local statistics buffer");
		return 0;
	}

	memset(&l2tpv2_session_stats, 0, sizeof(struct nss_l2tpv2_stats_session_debug) * NSS_MAX_L2TPV2_DYNAMIC_INTERFACES);
	size_wr += nss_stats_banner(lbuf, size_wr, size_al, "l2tpv2", NSS_STATS_SINGLE_CORE);

	/*
	 * Get all stats
	 */
	nss_l2tpv2_session_debug_stats_get((void *)&l2tpv2_session_stats);

	/*
	 * Session stats
	 */
	size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "\nl2tp v2 session stats start:\n\n");
	for (id = 0; id < NSS_MAX_L2TPV2_DYNAMIC_INTERFACES; id++) {

			if (!l2tpv2_session_stats[id].valid) {
				break;
			}

			dev = dev_get_by_index(&init_net, l2tpv2_session_stats[id].if_index);
			if (likely(dev)) {

				size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "%d. nss interface id=%d, netdevice=%s\n", id,
						l2tpv2_session_stats[id].if_num, dev->name);
				dev_put(dev);
			} else {
				size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "%d. nss interface id=%d\n", id,
						l2tpv2_session_stats[id].if_num);
			}

			size_wr += nss_stats_print("l2tpv2", "l2tp v2 session stats"
							, id
							, nss_l2tpv2_strings_session_stats
							, l2tpv2_session_stats[id].stats
							, NSS_L2TPV2_STATS_SESSION_MAX
							, lbuf, size_wr, size_al);
			size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "\n");
	}

	bytes_read = simple_read_from_buffer(ubuf, sz, ppos, lbuf, size_wr);

	kfree(lbuf);
	return bytes_read;
}

/*
 * nss_l2tpv2_stats_ops
 */
NSS_STATS_DECLARE_FILE_OPERATIONS(l2tpv2);

/*
 * nss_l2tpv2_stats_dentry_create()
 *	Create l2tpv2 statistics debug entry.
 */
void nss_l2tpv2_stats_dentry_create(void)
{
	nss_stats_create_dentry("l2tpv2", &nss_l2tpv2_stats_ops);
}

/*
 * nss_l2tpv2_stats_notify()
 *	Sends notifications to all the registered modules.
 *
 * Leverage NSS-FW statistics timing to update Netlink.
 */
void nss_l2tpv2_stats_notify(struct nss_ctx_instance *nss_ctx, uint32_t if_num)
{
	struct nss_l2tpv2_stats_notification l2tpv2_stats;
	struct nss_l2tpv2_stats_session_debug l2tpv2_session_stats[NSS_MAX_L2TPV2_DYNAMIC_INTERFACES];
	int id;

	memset(&l2tpv2_session_stats, 0, sizeof(l2tpv2_session_stats));

	/*
	 * Get all stats
	 */
	nss_l2tpv2_session_debug_stats_get((void *)&l2tpv2_session_stats);

	for (id = 0; id < NSS_MAX_L2TPV2_DYNAMIC_INTERFACES; id++) {
		if (l2tpv2_session_stats[id].if_num == if_num) {
			memcpy(&l2tpv2_stats.stats, &l2tpv2_session_stats[id].stats, sizeof(l2tpv2_stats.stats));
		}
	}
	l2tpv2_stats.core_id = nss_ctx->id;
	l2tpv2_stats.if_num = if_num;
	atomic_notifier_call_chain(&nss_l2tpv2_stats_notifier, NSS_STATS_EVENT_NOTIFY, (void *)&l2tpv2_stats);
}

/*
 * nss_l2tpv2_stats_register_notifier()
 *	Registers statistics notifier.
 */
int nss_l2tpv2_stats_register_notifier(struct notifier_block *nb)
{
	return atomic_notifier_chain_register(&nss_l2tpv2_stats_notifier, nb);
}
EXPORT_SYMBOL(nss_l2tpv2_stats_register_notifier);

/*
 * nss_l2tpv2_stats_unregister_notifier()
 *	Deregisters statistics notifier.
 */
int nss_l2tpv2_stats_unregister_notifier(struct notifier_block *nb)
{
	return atomic_notifier_chain_unregister(&nss_l2tpv2_stats_notifier, nb);
}
EXPORT_SYMBOL(nss_l2tpv2_stats_unregister_notifier);
