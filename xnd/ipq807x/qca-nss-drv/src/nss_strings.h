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
 * nss_strings.h
 *	NSS driver strings header file.
 */

#ifndef __NSS_STRINGS_H
#define __NSS_STRINGS_H

#include <linux/debugfs.h>
#include "nss_stats.h"

#define NSS_STRINGS_DECLARE_FILE_OPERATIONS(name) \
static const struct file_operations nss_##name##_strings_ops = { \
	.open = nss_strings_open, \
	.read = nss_##name##_strings_read, \
	.llseek = generic_file_llseek, \
	.release = nss_strings_release, \
}

/*
 * Private data for every file descriptor
 */
struct nss_strings_data {
	uint32_t if_num;			/**< Interface number for stats */
	struct nss_ctx_instance *nss_ctx;	/**< The core for project stats */
};

extern struct nss_stats_info nss_strings_stats_node[NSS_STATS_NODE_MAX];
void nss_strings_init(void);
int nss_strings_release(struct inode *inode, struct file *filp);
int nss_strings_open(struct inode *inode, struct file *filp);
void nss_strings_create_dentry(char *name, const struct file_operations *ops);
size_t nss_strings_fill_common_stats(char __user *ubuf, size_t sz, loff_t *ppos);
size_t nss_strings_print(char __user *ubuf, size_t sz, loff_t *ppos, struct nss_stats_info *stats_info, uint16_t max);

#endif /* __NSS_STRINGS_H */
