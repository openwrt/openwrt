/*
 ***************************************************************************
 * Copyright (c) 2021, The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 ***************************************************************************
 */

#include "nss_core.h"
#include "nss_ipsec_cmn.h"
#include "nss_ipsec_cmn_stats.h"
#include "nss_ipsec_cmn_strings.h"

/*
 * Declare atomic notifier data structure for statistics.
 */
ATOMIC_NOTIFIER_HEAD(nss_ipsec_cmn_stats_notifier);

/*
 * Spinlock to protect IPsec common statistics update/read
 */
DEFINE_SPINLOCK(nss_ipsec_cmn_stats_lock);

unsigned long *nss_ipsec_cmn_ifmap_get(void);
const char *nss_ipsec_cmn_stats_iface_type(enum nss_dynamic_interface_type type);

/*
 * nss_ipsec_cmn_stats
 *	ipsec common statistics
 */
uint64_t nss_ipsec_cmn_stats[NSS_MAX_NET_INTERFACES][NSS_IPSEC_CMN_STATS_MAX];

/*
 * nss_ipsec_cmn_stats_read()
 *	Read ipsec_cmn node statistics.
 */
static ssize_t nss_ipsec_cmn_stats_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	/*
	 * Max output lines = #stats +
	 * few blank lines for banner printing + Number of Extra outputlines
	 * for future reference to add new stats
	 */
	uint32_t max_output_lines = NSS_IPSEC_CMN_STATS_MAX + NSS_STATS_EXTRA_OUTPUT_LINES;
	size_t size_al = NSS_STATS_MAX_STR_LENGTH * max_output_lines;
	struct nss_ctx_instance *nss_ctx = nss_ipsec_cmn_get_context();
	enum nss_dynamic_interface_type type;
	unsigned long *ifmap;
	uint64_t *stats_shadow;
	ssize_t bytes_read = 0;
	size_t size_wr = 0;
	uint32_t if_num;
	int32_t i;
	int count;
	char *lbuf;

	ifmap = nss_ipsec_cmn_ifmap_get();
	count = bitmap_weight(ifmap, NSS_MAX_NET_INTERFACES);
	if (count) {
		size_al = size_al * count;
	}

	lbuf = vzalloc(size_al);
	if (unlikely(!lbuf)) {
		nss_warning("Could not allocate memory for local statistics buffer");
		return -ENOMEM;
	}

	stats_shadow = vzalloc(NSS_IPSEC_CMN_STATS_MAX * 8);
	if (unlikely(!stats_shadow)) {
		nss_warning("Could not allocate memory for local shadow buffer");
		vfree(lbuf);
		return -ENOMEM;
	}

	/*
	 * Common node stats for each IPSEC dynamic interface.
	 */
	size_wr += nss_stats_banner(lbuf, size_wr, size_al, "ipsec_cmn stats", NSS_STATS_SINGLE_CORE);
	for_each_set_bit(if_num, ifmap, NSS_MAX_NET_INTERFACES) {

		type = nss_dynamic_interface_get_type(nss_ctx, if_num);
		if ((type < NSS_DYNAMIC_INTERFACE_TYPE_IPSEC_CMN_INNER) ||
			(type > NSS_DYNAMIC_INTERFACE_TYPE_IPSEC_CMN_REDIRECT)) {
			continue;
		}

		spin_lock_bh(&nss_ipsec_cmn_stats_lock);
		for (i = 0; i < NSS_IPSEC_CMN_STATS_MAX; i++) {
			stats_shadow[i] = nss_ipsec_cmn_stats[if_num][i];
		}
		spin_unlock_bh(&nss_ipsec_cmn_stats_lock);

		size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "\n%s if_num:%03u\n",
					nss_ipsec_cmn_stats_iface_type(type), if_num);
		size_wr += nss_stats_print("ipsec_cmn", NULL, NSS_STATS_SINGLE_INSTANCE, nss_ipsec_cmn_strings_stats,
							stats_shadow, NSS_IPSEC_CMN_STATS_MAX, lbuf, size_wr, size_al);
	}

	bytes_read = simple_read_from_buffer(ubuf, sz, ppos, lbuf, strlen(lbuf));
	vfree(lbuf);
	vfree(stats_shadow);

	return bytes_read;
}

/*
 * nss_ipsec_cmn_stats_ops.
 */
NSS_STATS_DECLARE_FILE_OPERATIONS(ipsec_cmn);

/*
 * nss_ipsec_cmn_stats_sync()
 *	Update ipsec_cmn node statistics.
 */
void nss_ipsec_cmn_stats_sync(struct nss_ctx_instance *nss_ctx, struct nss_cmn_msg *ncm)
{
	struct nss_ipsec_cmn_msg *nicm = (struct nss_ipsec_cmn_msg *)ncm;
	struct nss_ipsec_cmn_ctx_stats *ndccs = &nicm->msg.ctx_sync.stats;
	uint64_t *ctx_stats;
	uint32_t *msg_stats;
	uint16_t i = 0;

	spin_lock_bh(&nss_ipsec_cmn_stats_lock);

	msg_stats = (uint32_t *)ndccs;
	ctx_stats = nss_ipsec_cmn_stats[ncm->interface];

	for (i = 0; i < NSS_IPSEC_CMN_STATS_MAX; i++, ctx_stats++, msg_stats++) {
		*ctx_stats += *msg_stats;
	}

	spin_unlock_bh(&nss_ipsec_cmn_stats_lock);
}

/*
 * nss_ipsec_cmn_stats_iface_type()
 *	Return a string for each interface type.
 */
const char *nss_ipsec_cmn_stats_iface_type(enum nss_dynamic_interface_type type)
{
	switch (type) {
	case NSS_DYNAMIC_INTERFACE_TYPE_IPSEC_CMN_INNER:
		return "ipsec_cmn_inner";

	case NSS_DYNAMIC_INTERFACE_TYPE_IPSEC_CMN_MDATA_INNER:
		return "ipsec_cmn_mdata_inner";

	case NSS_DYNAMIC_INTERFACE_TYPE_IPSEC_CMN_OUTER:
		return "ipsec_cmn_outer";

	case NSS_DYNAMIC_INTERFACE_TYPE_IPSEC_CMN_MDATA_OUTER:
		return "ipsec_cmn_mdata_outer";

	case NSS_DYNAMIC_INTERFACE_TYPE_IPSEC_CMN_REDIRECT:
		return "ipsec_cmn_redirect";

	default:
		return "invalid_interface";
	}
}

/*
 * nss_ipsec_cmn_stats_notify()
 *	Sends notifications to all the registered modules.
 *
 * Leverage NSS-FW statistics timing to update Netlink.
 */
void nss_ipsec_cmn_stats_notify(struct nss_ctx_instance *nss_ctx, uint32_t if_num)
{
	struct nss_ipsec_cmn_stats_notification ipsec_cmn_stats;

	spin_lock_bh(&nss_ipsec_cmn_stats_lock);
	ipsec_cmn_stats.core_id = nss_ctx->id;
	ipsec_cmn_stats.if_num = if_num;
	memcpy(ipsec_cmn_stats.stats_ctx, nss_ipsec_cmn_stats[if_num], sizeof(ipsec_cmn_stats.stats_ctx));
	spin_unlock_bh(&nss_ipsec_cmn_stats_lock);

	atomic_notifier_call_chain(&nss_ipsec_cmn_stats_notifier, NSS_STATS_EVENT_NOTIFY, &ipsec_cmn_stats);
}

/*
 * nss_ipsec_cmn_stats_unregister_notifier()
 *	Deregisters statistics notifier.
 */
int nss_ipsec_cmn_stats_unregister_notifier(struct notifier_block *nb)
{
	return atomic_notifier_chain_unregister(&nss_ipsec_cmn_stats_notifier, nb);
}
EXPORT_SYMBOL(nss_ipsec_cmn_stats_unregister_notifier);

/*
 * nss_ipsec_cmn_stats_register_notifier()
 *	Registers statistics notifier.
 */
int nss_ipsec_cmn_stats_register_notifier(struct notifier_block *nb)
{
	return atomic_notifier_chain_register(&nss_ipsec_cmn_stats_notifier, nb);
}
EXPORT_SYMBOL(nss_ipsec_cmn_stats_register_notifier);

/*
 * nss_ipsec_cmn_stats_dentry_create()
 *	Create ipsec common statistics debug entry.
 */
void nss_ipsec_cmn_stats_dentry_create(void)
{
	nss_stats_create_dentry("ipsec_cmn", &nss_ipsec_cmn_stats_ops);
}
