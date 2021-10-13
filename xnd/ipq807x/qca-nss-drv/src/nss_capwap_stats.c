/*
 **************************************************************************
 * Copyright (c) 2017-2020, The Linux Foundation. All rights reserved.
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

#include "nss_drv_stats.h"
#include "nss_core.h"
#include "nss_capwap.h"
#include "nss_capwap_stats.h"
#include "nss_capwap_strings.h"

/*
 * Declare atomic notifier data structure for statistics.
 */
ATOMIC_NOTIFIER_HEAD(nss_capwap_stats_notifier);

/*
 * nss_capwap_stats_encap()
 *	Make a row for CAPWAP encap stats.
 */
static ssize_t nss_capwap_stats_encap(char *line, int len, int i, struct nss_capwap_tunnel_stats *s)
{
	uint64_t tcnt = 0;

	switch (i) {
	case 0:
		tcnt = s->pnode_stats.tx_packets;
		break;
	case 1:
		tcnt = s->pnode_stats.tx_bytes;
		break;
	case 2:
		tcnt = s->tx_segments;
		break;
	case 3:
		tcnt = s->tx_dropped_sg_ref;
		break;
	case 4:
		tcnt = s->tx_dropped_ver_mis;
		break;
	case 5:
		tcnt = s->tx_dropped_inner;
		break;
	case 6:
		tcnt = s->tx_dropped_hroom;
		break;
	case 7:
		tcnt = s->tx_dropped_dtls;
		break;
	case 8:
		tcnt = s->tx_dropped_nwireless;
		break;
	case 9:
		tcnt = s->tx_queue_full_drops;
		break;
	case 10:
		tcnt = s->tx_mem_failure_drops;
		break;
	case 11:
		tcnt = s->fast_mem;
		break;
	default:
		return 0;
	}

	return snprintf(line, len, "%s = %llu\n", nss_capwap_strings_encap_stats[i].stats_name, tcnt);
}

/*
 * nss_capwap_stats_decap()
 *	Make a row for CAPWAP decap stats.
 */
static ssize_t nss_capwap_stats_decap(char *line, int len, int i, struct nss_capwap_tunnel_stats *s)
{
	uint64_t tcnt = 0;

	switch (i) {
	case 0:
		tcnt = s->pnode_stats.rx_packets;
		break;
	case 1:
		tcnt = s->pnode_stats.rx_bytes;
		break;
	case 2:
		tcnt = s->dtls_pkts;
		break;
	case 3:
		tcnt = s->rx_segments;
		break;
	case 4:
		tcnt = s->pnode_stats.rx_dropped;
		break;
	case 5:
		tcnt = s->rx_oversize_drops;
		break;
	case 6:
		tcnt = s->rx_frag_timeout_drops;
		break;
	case 7:
		tcnt = s->rx_dup_frag;
		break;
	case 8:
		tcnt = s->rx_frag_gap_drops;
		break;
	case 9:
		tcnt = s->rx_n2h_drops;
		return snprintf(line, len, "%s = %llu (n2h = %llu)\n", nss_capwap_strings_decap_stats[i].stats_name, tcnt, s->rx_n2h_queue_full_drops);
	case 10:
		tcnt = s->rx_n2h_queue_full_drops;
		break;
	case 11:
		tcnt = s->rx_mem_failure_drops;
		break;
	case 12:
		tcnt = s->rx_csum_drops;
		break;
	case 13:
		tcnt = s->rx_malformed;
		break;
	case 14:
		tcnt = s->fast_mem;
		break;
	default:
		return 0;
	}

	return snprintf(line, len, "%s = %llu\n", nss_capwap_strings_decap_stats[i].stats_name, tcnt);
}

/*
 * nss_capwap_stats_read()
 *	Read CAPWAP stats
 */
static ssize_t nss_capwap_stats_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos, uint16_t type)
{
	struct nss_stats_data *data = fp->private_data;
	ssize_t bytes_read = 0;
	struct nss_capwap_tunnel_stats stats;
	size_t bytes;
	char line[80];
	int start;
	uint32_t if_num = NSS_DYNAMIC_IF_START;
	uint32_t max_if_num = NSS_DYNAMIC_IF_START + NSS_MAX_DYNAMIC_INTERFACES;

	if (data) {
		if_num = data->if_num;
	}

	/*
	 * If we are done accomodating all the CAPWAP tunnels.
	 */
	if (if_num > max_if_num) {
		return 0;
	}

	for (; if_num <= max_if_num; if_num++) {
		bool isthere;
		enum nss_dynamic_interface_type dtype;

		if (nss_is_dynamic_interface(if_num) == false) {
			continue;
		}

		dtype = nss_dynamic_interface_get_type(nss_capwap_get_ctx(), if_num);

		/*
		 * Read encap stats from inner node and decap stats from outer node.
		 */
		if ((type == 1) && (dtype != NSS_DYNAMIC_INTERFACE_TYPE_CAPWAP_HOST_INNER)) {
			continue;
		}

		if ((type == 0) && (dtype != NSS_DYNAMIC_INTERFACE_TYPE_CAPWAP_OUTER)) {
			continue;
		}

		/*
		 * If CAPWAP tunnel does not exists, then isthere will be false.
		 */
		isthere = nss_capwap_get_stats(if_num, &stats);
		if (!isthere) {
			continue;
		}

		bytes = snprintf(line, sizeof(line), "----if_num : %2d----\n", if_num);
		if ((bytes_read + bytes) > sz) {
			break;
		}

		if (copy_to_user(ubuf + bytes_read, line, bytes) != 0) {
			bytes_read = -EFAULT;
			goto fail;
		}
		bytes_read += bytes;
		start = 0;
		while (bytes_read < sz) {
			if (type == 1) {
				bytes = nss_capwap_stats_encap(line, sizeof(line), start, &stats);
			} else {
				bytes = nss_capwap_stats_decap(line, sizeof(line), start, &stats);
			}

			/*
			 * If we don't have any more lines in decap/encap.
			 */
			if (bytes == 0) {
				break;
			}

			if ((bytes_read + bytes) > sz)
				break;

			if (copy_to_user(ubuf + bytes_read, line, bytes) != 0) {
				bytes_read = -EFAULT;
				goto fail;
			}

			bytes_read += bytes;
			start++;
		}
	}

	if (bytes_read > 0) {
		*ppos = bytes_read;
	}

	if (data) {
		data->if_num = if_num;
	}
fail:
	return bytes_read;
}

/*
 * nss_capwap_decap_stats_read()
 *	Read CAPWAP decap stats
 */
static ssize_t nss_capwap_decap_stats_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_capwap_stats_read(fp, ubuf, sz, ppos, 0);
}

/*
 * nss_capwap_encap_stats_read()
 *	Read CAPWAP encap stats
 */
static ssize_t nss_capwap_encap_stats_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_capwap_stats_read(fp, ubuf, sz, ppos, 1);
}

/*
 * nss_capwap_stats_ops
 */
NSS_STATS_DECLARE_FILE_OPERATIONS(capwap_encap);
NSS_STATS_DECLARE_FILE_OPERATIONS(capwap_decap);

/*
 * nss_capwap_stats_dentry_create()
 *	Create CAPWAP statistics debug entry
 */
void nss_capwap_stats_dentry_create(void)
{
	nss_stats_create_dentry("capwap_encap", &nss_capwap_encap_stats_ops);
	nss_stats_create_dentry("capwap_decap", &nss_capwap_decap_stats_ops);
}

/*
 * nss_capwap_stats_notify()
 *	Sends notifications to the registered modules.
 *
 * Leverage NSS-FW statistics timing to update Netlink.
 */
void nss_capwap_stats_notify(uint32_t if_num, uint32_t core_id)
{
	struct nss_capwap_stats_notification capwap_stats;

	capwap_stats.core_id = core_id;
	capwap_stats.if_num = if_num;
	nss_capwap_get_stats(if_num, &capwap_stats.stats);
	atomic_notifier_call_chain(&nss_capwap_stats_notifier, NSS_STATS_EVENT_NOTIFY, (void *)&capwap_stats);
}

/*
 * nss_capwap_stats_register_notifier()
 *	Registers statistics notifier.
 */
int nss_capwap_stats_register_notifier(struct notifier_block *nb)
{
	return atomic_notifier_chain_register(&nss_capwap_stats_notifier, nb);
}
EXPORT_SYMBOL(nss_capwap_stats_register_notifier);

/*
 * nss_capwap_stats_unregister_notifier()
 *	Deregisters statistics notifier.
 */
int nss_capwap_stats_unregister_notifier(struct notifier_block *nb)
{
	return atomic_notifier_chain_unregister(&nss_capwap_stats_notifier, nb);
}
EXPORT_SYMBOL(nss_capwap_stats_unregister_notifier);
