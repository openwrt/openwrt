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
#include "nss_pptp_stats.h"
#include "nss_pptp_strings.h"

/*
 * Declare atomic notifier data structure for statistics.
 */
ATOMIC_NOTIFIER_HEAD(nss_pptp_stats_notifier);

struct nss_pptp_stats_session_debug pptp_session_stats[NSS_MAX_PPTP_DYNAMIC_INTERFACES];

/*
 * nss_pptp_stats_read()
 *	Read pptp statistics
 */
static ssize_t nss_pptp_stats_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{

	uint32_t max_output_lines = 2 /* header & footer for session stats */
					+ NSS_MAX_PPTP_DYNAMIC_INTERFACES * (NSS_PPTP_STATS_SESSION_MAX + 2) /*session stats */
					+ 2;
	size_t size_al = NSS_STATS_MAX_STR_LENGTH * max_output_lines ;
	size_t size_wr = 0;
	ssize_t bytes_read = 0;
	struct net_device *dev;
	int id, i;

	char *lbuf = kzalloc(size_al, GFP_KERNEL);
	if (unlikely(lbuf == NULL)) {
		nss_warning("Could not allocate memory for local statistics buffer");
		return 0;
	}

	memset(&pptp_session_stats, 0, sizeof(struct nss_pptp_stats_session_debug) * NSS_MAX_PPTP_DYNAMIC_INTERFACES);

	/*
	 * Get all stats
	 */
	nss_pptp_session_debug_stats_get((void *)&pptp_session_stats);

	/*
	 * Session stats
	 */
	size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "\npptp session stats start:\n\n");
	for (id = 0; id < NSS_MAX_PPTP_DYNAMIC_INTERFACES; id++) {

			if (!pptp_session_stats[id].valid) {
				break;
			}

			dev = dev_get_by_index(&init_net, pptp_session_stats[id].if_index);
			if (likely(dev)) {

				size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "%d. nss interface id=%d, netdevice=%s\n", id,
						pptp_session_stats[id].if_num, dev->name);
				dev_put(dev);
			} else {
				size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "%d. nss interface id=%d\n", id,
						pptp_session_stats[id].if_num);
			}

			for (i = 0; i < NSS_PPTP_STATS_SESSION_MAX; i++) {
				size_wr += scnprintf(lbuf + size_wr, size_al - size_wr,
						     "\t%s = %llu\n", nss_pptp_strings_session_debug_stats[i].stats_name,
						      pptp_session_stats[id].stats[i]);
			}
			size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "\n");
	}

	size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "\npptp session stats end\n");
	bytes_read = simple_read_from_buffer(ubuf, sz, ppos, lbuf, size_wr);

	kfree(lbuf);
	return bytes_read;
}

/*
 * nss_pptp_stats_ops
 */
NSS_STATS_DECLARE_FILE_OPERATIONS(pptp);

/*
 * nss_pptp_stats_dentry_create()
 *	Create PPTP node statistics debug entry.
 */
void nss_pptp_stats_dentry_create(void)
{
	nss_stats_create_dentry("pptp", &nss_pptp_stats_ops);
}

/*
 * nss_pptp_stats_notify()
 *	Sends notifications to the registered modules.
 *
 * Leverage NSS-FW statistics timing to update Netlink.
 */
void nss_pptp_stats_notify(struct nss_ctx_instance *nss_ctx, uint32_t if_num)
{
	struct nss_pptp_stats_notification pptp_stats;
	int id;

	memset(&pptp_session_stats, 0, sizeof(pptp_session_stats));

	/*
	 * Get all stats
	 */
	nss_pptp_session_debug_stats_get((void *)&pptp_session_stats);

	for (id = 0; id < NSS_MAX_PPTP_DYNAMIC_INTERFACES; id++) {
		if (pptp_session_stats[id].if_num == if_num) {
			memcpy(&pptp_stats.stats, &pptp_session_stats[id].stats, sizeof(pptp_stats.stats));
		}
	}
	pptp_stats.if_type = nss_dynamic_interface_get_type(nss_ctx, if_num);
	pptp_stats.core_id = nss_ctx->id;
	pptp_stats.if_num = if_num;
	atomic_notifier_call_chain(&nss_pptp_stats_notifier, NSS_STATS_EVENT_NOTIFY, (void *)&pptp_stats);
}

/*
 * nss_pptp_stats_register_notifier()
 *	Registers statistics notifier.
 */
int nss_pptp_stats_register_notifier(struct notifier_block *nb)
{
	return atomic_notifier_chain_register(&nss_pptp_stats_notifier, nb);
}
EXPORT_SYMBOL(nss_pptp_stats_register_notifier);

/*
 * nss_pptp_stats_unregister_notifier()
 *	Deregisters statistics notifier.
 */
int nss_pptp_stats_unregister_notifier(struct notifier_block *nb)
{
	return atomic_notifier_chain_unregister(&nss_pptp_stats_notifier, nb);
}
EXPORT_SYMBOL(nss_pptp_stats_unregister_notifier);
