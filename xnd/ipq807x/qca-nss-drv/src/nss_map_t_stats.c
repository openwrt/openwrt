/*
 **************************************************************************
 * Copyright (c) 2017,2019-2021, The Linux Foundation. All rights reserved.
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
#include "nss_map_t_stats.h"
#include "nss_map_t_strings.h"

/*
 * Declare atomic notifier data structure for statistics.
 */
ATOMIC_NOTIFIER_HEAD(nss_map_t_stats_notifier);

/*
 * nss_map_t_stats_read()
 *	Read map_t statistics
 */
static ssize_t nss_map_t_stats_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{

	uint32_t max_output_lines = 2 /* header & footer for instance stats */
					+ NSS_MAX_MAP_T_DYNAMIC_INTERFACES * (NSS_MAP_T_STATS_MAX + 2) /*instance stats */
					+ 2;
	size_t size_al = NSS_STATS_MAX_STR_LENGTH * max_output_lines;
	size_t size_wr = 0;
	ssize_t bytes_read = 0;
	struct net_device *dev;
	struct nss_map_t_stats_instance_debug map_t_instance_stats[NSS_MAX_MAP_T_DYNAMIC_INTERFACES];
	int id, i;

	char *lbuf = kzalloc(size_al, GFP_KERNEL);
	if (unlikely(!lbuf)) {
		nss_warning("Could not allocate memory for local statistics buffer");
		return 0;
	}

	memset(&map_t_instance_stats, 0, sizeof(struct nss_map_t_stats_instance_debug) * NSS_MAX_MAP_T_DYNAMIC_INTERFACES);

	/*
	 * Get all stats
	 */
	nss_map_t_instance_debug_stats_get((void *)&map_t_instance_stats);

	/*
	 * Session stats
	 */
	size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "\nmap_t instance stats start:\n\n");
	for (id = 0; id < NSS_MAX_MAP_T_DYNAMIC_INTERFACES; id++) {

			if (!map_t_instance_stats[id].valid) {
				continue;
			}

			dev = dev_get_by_index(&init_net, map_t_instance_stats[id].if_index);
			if (likely(dev)) {

				size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "%d. nss interface id=%d, netdevice=%s\n", id,
						map_t_instance_stats[id].if_num, dev->name);
				dev_put(dev);
			} else {
				size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "%d. nss interface id=%d\n", id,
						map_t_instance_stats[id].if_num);
			}

			for (i = 0; i < NSS_MAP_T_STATS_MAX; i++) {
				size_wr += scnprintf(lbuf + size_wr, size_al - size_wr,
						     "\t%s = %llu\n", nss_map_t_strings_instance_stats[i].stats_name,
						      map_t_instance_stats[id].stats[i]);
			}
			size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "\n");
	}

	size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "\nmap_t instance stats end\n");
	bytes_read = simple_read_from_buffer(ubuf, sz, ppos, lbuf, size_wr);

	kfree(lbuf);
	return bytes_read;
}

/*
 * nss_map_t_stats_ops
 */
NSS_STATS_DECLARE_FILE_OPERATIONS(map_t);

/*
 * nss_map_t_stats_dentry_create()
 *	Create map_t statistics debug entry.
 */
void nss_map_t_stats_dentry_create(void)
{
	nss_stats_create_dentry("map_t", &nss_map_t_stats_ops);
}

/*
 * nss_map_t_stats_notify()
 *	Sends notifications to the registered modules.
 *
 * Leverage NSS-FW statistics timing to update Netlink.
 */
void nss_map_t_stats_notify(struct nss_ctx_instance *nss_ctx, uint32_t if_num)
{
	struct nss_map_t_stats_notification map_t_stats;
	struct nss_map_t_stats_instance_debug map_t_instance_stats[NSS_MAX_MAP_T_DYNAMIC_INTERFACES];
	int id;

	memset(&map_t_instance_stats, 0, sizeof(map_t_instance_stats));

	/*
	 * Get all stats
	 */
	nss_map_t_instance_debug_stats_get((void *)&map_t_instance_stats);

	for (id = 0; id < NSS_MAX_MAP_T_DYNAMIC_INTERFACES; id++) {
		if (map_t_instance_stats[id].if_num == if_num) {
			memcpy(&map_t_stats.stats, &map_t_instance_stats[id].stats, sizeof(map_t_stats.stats));
		}
	}
	map_t_stats.if_type = nss_dynamic_interface_get_type(nss_ctx, if_num);
	map_t_stats.core_id = nss_ctx->id;
	map_t_stats.if_num = if_num;
	atomic_notifier_call_chain(&nss_map_t_stats_notifier, NSS_STATS_EVENT_NOTIFY, (void *)&map_t_stats);
}

/*
 * nss_map_t_stats_register_notifier()
 *	Registers statistics notifier.
 */
int nss_map_t_stats_register_notifier(struct notifier_block *nb)
{
	return atomic_notifier_chain_register(&nss_map_t_stats_notifier, nb);
}
EXPORT_SYMBOL(nss_map_t_stats_register_notifier);

/*
 * nss_map_t_stats_unregister_notifier()
 *	Deregisters statistics notifier.
 */
int nss_map_t_stats_unregister_notifier(struct notifier_block *nb)
{
	return atomic_notifier_chain_unregister(&nss_map_t_stats_notifier, nb);
}
EXPORT_SYMBOL(nss_map_t_stats_unregister_notifier);
