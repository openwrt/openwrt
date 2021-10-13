/*
 ***************************************************************************
 * Copyright (c) 2020-2021, The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 ***************************************************************************
 */

/*
 * nss_match_stats.c
 */
#include "nss_core.h"
#include "nss_stats.h"
#include <nss_match.h>
#include "nss_match_stats.h"
#include "nss_match_strings.h"

#define NSS_MATCH_STATS_SIZE_PER_IF (NSS_STATS_MAX_STR_LENGTH * NSS_STATS_NODE_MAX)
                                        /* Total number of statistics per match interface. */

int match_ifnum[NSS_MATCH_INSTANCE_MAX] = {0};
uint64_t nss_match_stats[NSS_MATCH_INSTANCE_MAX][NSS_MATCH_STATS_MAX];
static DEFINE_SPINLOCK(nss_match_stats_lock);

/*
 * Declare atomic notifier data structure for statistics.
 */
ATOMIC_NOTIFIER_HEAD(nss_match_stats_notifier);

/*
 * nss_match_stats_read()
 *	Read match node statiistics.
 */
static ssize_t nss_match_stats_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	/*
	 * Max output lines = #stats +
	 * few blank lines for banner printing + Number of Extra outputlines
	 * for future reference to add new stats
	 */
	uint32_t max_output_lines = NSS_MATCH_STATS_MAX + NSS_STATS_EXTRA_OUTPUT_LINES;
	size_t size_al = NSS_STATS_MAX_STR_LENGTH * max_output_lines * NSS_MATCH_INSTANCE_MAX;
	ssize_t bytes_read = 0;
	size_t size_wr = 0;
	uint32_t if_num;
	int index;

	char *lbuf = kzalloc(size_al, GFP_KERNEL);
	if (unlikely(!lbuf)) {
		nss_warning("Could not allocate memory for local statistics buffer");
		return 0;
	}

	size_wr += nss_stats_banner(lbuf, size_wr, size_al, "match stats", NSS_STATS_SINGLE_CORE);

	/*
	 * Common node stats for each match dynamic interface.
	 */
	for (index = 0; index < NSS_MATCH_INSTANCE_MAX; index++) {

		spin_lock_bh(&nss_match_stats_lock);
		if_num = match_ifnum[index];
		spin_unlock_bh(&nss_match_stats_lock);

		if (if_num) {
			size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "\nMatch node if_num:%03u", if_num);
			size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "\n ---------------------- \n");
			size_wr += nss_stats_print("match", NULL, NSS_STATS_SINGLE_INSTANCE, nss_match_strings_stats,
						nss_match_stats[index], NSS_MATCH_STATS_MAX, lbuf, size_wr, size_al);
			continue;
		}
	}

	bytes_read = simple_read_from_buffer(ubuf, sz, ppos, lbuf, strlen(lbuf));
	kfree(lbuf);
	return bytes_read;
}


/*
 * nss_match_stats_sync()
 *	Update match common node statistics.
 */
void nss_match_stats_sync(struct nss_ctx_instance *nss_ctx, struct nss_match_msg *nmm)
{
	struct nss_match_stats_sync *ndccs = &nmm->msg.stats;
	uint64_t *ctx_stats;
	uint32_t *msg_stats;
	uint32_t if_num;
	uint16_t i = 0;
	int index;

	for (index = 0; index < NSS_MATCH_INSTANCE_MAX; index++) {
		spin_lock_bh(&nss_match_stats_lock);
		if_num = match_ifnum[index];
		spin_unlock_bh(&nss_match_stats_lock);

		if (if_num == nmm->cm.interface) {
			break;
		}
	}

	if (index == NSS_MATCH_INSTANCE_MAX) {
		nss_warning("Invalid Match index\n");
		return;
	}

	spin_lock_bh(&nss_match_stats_lock);
	msg_stats = (uint32_t *)ndccs;
	ctx_stats = nss_match_stats[index];

	for (i = 0; i < NSS_MATCH_STATS_MAX; i++, ctx_stats++, msg_stats++) {
		*ctx_stats += *msg_stats;
	}

	spin_unlock_bh(&nss_match_stats_lock);
}

/*
 * nss_match_stats_ops
 */
NSS_STATS_DECLARE_FILE_OPERATIONS(match)

/*
 * nss_match_ifnum_add()
 *	Add match node interface ID.
 */
bool nss_match_ifnum_add(int if_num)
{
	int index = 0;

	spin_lock_bh(&nss_match_stats_lock);

	for (index = 0; index < NSS_MATCH_INSTANCE_MAX; index++) {
		if (match_ifnum[index]) {
			continue;
		}

		match_ifnum[index] = if_num;

		spin_unlock_bh(&nss_match_stats_lock);
		return true;
	}

	spin_unlock_bh(&nss_match_stats_lock);
	return false;
}

/*
 * nss_match_ifnum_delete()
 *	Delete match node interface ID.
 */
bool nss_match_ifnum_delete(int if_num)
{
	int index = 0;

	spin_lock_bh(&nss_match_stats_lock);

	for (index = 0; index < NSS_MATCH_INSTANCE_MAX; index++) {
		if (match_ifnum[index] != if_num) {
			continue;
		}

		match_ifnum[index] = 0;

		spin_unlock_bh(&nss_match_stats_lock);
		return true;
	}

	spin_unlock_bh(&nss_match_stats_lock);
	return false;
}

/*
 * nss_match_stats_notify()
 *	Sends notifications to all the registered modules.
 *
 * Leverage NSS-FW statistics timing to update Netlink.
 */
void nss_match_stats_notify(struct nss_ctx_instance *nss_ctx, uint32_t if_num)
{
	struct nss_match_stats_notification match_stats;
	uint32_t interface;
	int index;

	match_stats.core_id = nss_ctx->id;
	match_stats.if_num = if_num;

	for (index = 0; index < NSS_MATCH_INSTANCE_MAX; index++) {
		spin_lock_bh(&nss_match_stats_lock);
		interface = match_ifnum[index];
		spin_unlock_bh(&nss_match_stats_lock);

		if (interface == if_num) {
			break;
		}
	}

	if (index == NSS_MATCH_INSTANCE_MAX) {
		nss_warning("Invalid Match index\n");
		return;
	}

	spin_lock_bh(&nss_match_stats_lock);
	memcpy(match_stats.stats_ctx, nss_match_stats[index], sizeof(match_stats.stats_ctx));
	spin_unlock_bh(&nss_match_stats_lock);
	atomic_notifier_call_chain(&nss_match_stats_notifier, NSS_STATS_EVENT_NOTIFY, &match_stats);
}

/*
 * nss_match_stats_unregister_notifier()
 *	Deregisters statistics notifier.
 */
int nss_match_stats_unregister_notifier(struct notifier_block *nb)
{
	return atomic_notifier_chain_unregister(&nss_match_stats_notifier, nb);
}

/*
 * nss_match_stats_register_notifier()
 *	Registers statistics notifier.
 */
int nss_match_stats_register_notifier(struct notifier_block *nb)
{
	return atomic_notifier_chain_register(&nss_match_stats_notifier, nb);
}

/*
 * nss_match_stats_dentry_create()
 *	Create match statistics debug entry.
 */
void nss_match_stats_dentry_create(void)
{
	nss_stats_create_dentry("match", &nss_match_stats_ops);
}
