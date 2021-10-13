/*
 **************************************************************************
 * Copyright (c) 2020, The Linux Foundation. All rights reserved.
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

/*
 * nss_strings.c
 *	NSS driver strings APIs.
 */

#include "nss_strings.h"
#include "nss_core.h"
#include "nss_drv_strings.h"

/*
 * common stats
 */
struct nss_stats_info nss_strings_stats_node[NSS_STATS_NODE_MAX] = {
	{"rx_pkts"              , NSS_STATS_TYPE_COMMON},
	{"rx_byts"              , NSS_STATS_TYPE_COMMON},
	{"tx_pkts"              , NSS_STATS_TYPE_COMMON},
	{"tx_byts"              , NSS_STATS_TYPE_COMMON},
	{"rx_queue[0]_drops"    , NSS_STATS_TYPE_DROP},
	{"rx_queue[1]_drops"    , NSS_STATS_TYPE_DROP},
	{"rx_queue[2]_drops"    , NSS_STATS_TYPE_DROP},
	{"rx_queue[3]_drops"    , NSS_STATS_TYPE_DROP}
};

/*
 * nss_strings_print()
 *	Helper API to print stats names
 */
size_t nss_strings_print(char __user *ubuf, size_t sz, loff_t *ppos, struct nss_stats_info *stats_info, uint16_t max)
{
	int32_t i;
	size_t size_al = (NSS_STATS_MAX_STR_LENGTH + 12) * max;
	size_t size_wr = 0;
	ssize_t bytes_read = 0;

	char *lbuf = kzalloc(size_al, GFP_KERNEL);
	if (!lbuf) {
		nss_warning("Could not allocate memory for local statistics buffer");
		return 0;
	}

	for (i = 0; i < max; i++) {
		/*
		 * Print what we have but don't exceed the buffer.
		 */
		if (size_wr >= size_al) {
			nss_info_always("Buffer overflowed.\n");
			break;
		}
		size_wr += scnprintf(lbuf + size_wr, size_al - size_wr,
				"\t%d , %s\n", stats_info[i].stats_type, stats_info[i].stats_name);
	}

	bytes_read = simple_read_from_buffer(ubuf, sz, ppos, lbuf, strlen(lbuf));
	kfree(lbuf);

	return bytes_read;
}

/*
 * nss_strings_create_dentry()
 *	Create strings debug entry for subsystem.
 */
void nss_strings_create_dentry(char *name, const struct file_operations *ops)
{
	if (!nss_top_main.strings_dentry || !debugfs_create_file(name, 0400, nss_top_main.strings_dentry, &nss_top_main, ops)) {
		nss_warning("Failed to create debug entry for subsystem %s\n", name);
	}
}

/*
 * nss_strings_open()
 */
int nss_strings_open(struct inode *inode, struct file *filp)
{
	struct nss_strings_data *data = NULL;

	data = kzalloc(sizeof(struct nss_strings_data), GFP_KERNEL);
	if (!data) {
		return -ENOMEM;
	}
	data->if_num = NSS_DYNAMIC_IF_START;
	data->nss_ctx = (struct nss_ctx_instance *)(inode->i_private);
	filp->private_data = data;

	return 0;
}

/*
 * nss_strings_release()
 */
int nss_strings_release(struct inode *inode, struct file *filp)
{
	struct nss_strings_data *data = filp->private_data;

	if (data) {
		kfree(data);
	}

	return 0;
}

/*
 * nss_common_node_stats_strings_read()
 *	Read common node statistics names.
 */
static ssize_t nss_common_node_stats_strings_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	return nss_strings_print(ubuf, sz, ppos, nss_strings_stats_node, NSS_STATS_NODE_MAX);
}

/*
 * nss_common_node_stats_strings_ops
 */
NSS_STRINGS_DECLARE_FILE_OPERATIONS(common_node_stats);

/*
 * nss_strings_init()
 *	Enable NSS statistics
 */
void nss_strings_init(void)
{
	nss_top_main.strings_dentry = debugfs_create_dir("strings", nss_top_main.top_dentry);
	if (unlikely(nss_top_main.strings_dentry == NULL)) {
		nss_warning("Failed to create strings directory in debugfs/qca-nss-drv");
		return;
	}

	/*
	 * Common node statistics
	 */
	nss_strings_create_dentry("common_node_stats", &nss_common_node_stats_strings_ops);
}
