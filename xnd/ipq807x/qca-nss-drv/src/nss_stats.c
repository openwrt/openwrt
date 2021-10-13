/*
 **************************************************************************
 * Copyright (c) 2013-2020, The Linux Foundation. All rights reserved.
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
#include "nss_strings.h"
#include "nss_drv_stats.h"

/*
 * Maximum banner length:
 */
#define NSS_STATS_BANNER_MAX_LENGTH 80

/*
 * Maximum number of digits a stats value can have:
 */
#define NSS_STATS_DIGITS_MAX 16

/*
 * Spaces to print core details inside banner
 */
#define NSS_STATS_BANNER_SPACES 12

/*
 * Max characters for a node name.
 */
#define NSS_STATS_NODE_NAME_MAX 24

int nonzero_stats_print = 0;

/*
 * nss_stats_spacing()
 *	Framework to maintain consistent spacing between stats value and stats type.
 */
static size_t nss_stats_spacing(uint64_t stats_val, char *lbuf, size_t size_wr, size_t size_al)
{
	int i;
	int digit_counter = (stats_val == 0 ? 1 : 0);
	while (stats_val != 0) {
		/*
		 * TODO: need to check for (nss_ptr_t)
		 */
		stats_val = (nss_ptr_t)stats_val / 10;
		digit_counter++;
	}

	for (i = 0; i < NSS_STATS_DIGITS_MAX - digit_counter; i++) {
		size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, " ");
	}

	return size_wr;
}

/*
 * nss_stats_nonzero_handler()
 *	Handler to take nonzero stats print configuration.
 */
static int nss_stats_nonzero_handler(struct ctl_table *ctl, int write, void __user *buffer, size_t *lenp, loff_t *ppos)
{
	int ret;
	ret = proc_dointvec(ctl, write, buffer, lenp, ppos);
	return ret;
}

static struct ctl_table nss_stats_table[] = {
	{
		.procname		= "non_zero_stats",
		.data			= &nonzero_stats_print,
		.maxlen			= sizeof(int),
		.mode			= 0644,
		.proc_handler		= &nss_stats_nonzero_handler,
	},
	{ }
};

static struct ctl_table nss_stats_dir[] = {
	{
		.procname		= "stats",
		.mode			= 0555,
		.child			= nss_stats_table,
	},
	{ }
};

static struct ctl_table nss_stats_root_dir[] = {
	{
		.procname		= "nss",
		.mode			= 0555,
		.child			= nss_stats_dir,
	},
	{ }
};

static struct ctl_table nss_stats_root[] = {
	{
		.procname		= "dev",
		.mode			= 0555,
		.child			= nss_stats_root_dir,
	},
	{ }
};
static struct ctl_table_header *nss_stats_header;

/*
 * nss_stats_register_sysctl()
 *	Register a sysctl table for stats.
 */
void nss_stats_register_sysctl(void)
{
	/*
	 * Register sysctl table.
	 */
	nss_stats_header = register_sysctl_table(nss_stats_root);
}

/*
 * nss_stats_open()
 *	Opens stats file.
 */
int nss_stats_open(struct inode *inode, struct file *filp)
{
	struct nss_stats_data *data = NULL;

	data = kzalloc(sizeof(struct nss_stats_data), GFP_KERNEL);
	if (!data) {
		return -ENOMEM;
	}

	memset(data, 0, sizeof (struct nss_stats_data));
	data->if_num = NSS_DYNAMIC_IF_START;
	data->index = 0;
	data->edma_id = (nss_ptr_t)inode->i_private;
	data->nss_ctx = (struct nss_ctx_instance *)(inode->i_private);
	filp->private_data = data;

	return 0;
}

/*
 * nss_stats_release()
 *	Releases stats file.
 */
int nss_stats_release(struct inode *inode, struct file *filp)
{
	struct nss_stats_data *data = filp->private_data;

	if (data) {
		kfree(data);
	}

	return 0;
}

/*
 * nss_stats_clean()
 *	Cleanup NSS statistics files.
 */
void nss_stats_clean(void)
{
	/*
	 * Remove debugfs tree
	 */
	if (likely(nss_top_main.top_dentry != NULL)) {
		debugfs_remove_recursive(nss_top_main.top_dentry);
		nss_top_main.top_dentry = NULL;
	}
}

/*
 * nss_stats_reset_common_stats()
 *	Reset common node statistics.
 */
void nss_stats_reset_common_stats(uint32_t if_num)
{
	if (unlikely(if_num >= NSS_MAX_NET_INTERFACES)) {
		return;
	}

	spin_lock_bh(&nss_top_main.stats_lock);
	memset(nss_top_main.stats_node[if_num], 0, NSS_STATS_NODE_MAX * sizeof(uint64_t));
	spin_unlock_bh(&nss_top_main.stats_lock);
}

/*
 * nss_stats_fill_common_stats()
 *	Fill common node statistics.
 */
size_t nss_stats_fill_common_stats(uint32_t if_num, int instance, char *lbuf, size_t size_wr, size_t size_al, char *node)
{
	uint64_t stats_val[NSS_STATS_NODE_MAX];
	int i;
	size_t orig_size_wr = size_wr;

	spin_lock_bh(&nss_top_main.stats_lock);
	for (i = 0; i < NSS_STATS_NODE_MAX; i++) {
		stats_val[i] = nss_top_main.stats_node[if_num][i];
	}

	spin_unlock_bh(&nss_top_main.stats_lock);
	size_wr += nss_stats_print(node, NULL, instance, nss_strings_stats_node, stats_val, NSS_STATS_NODE_MAX, lbuf, size_wr, size_al);
	return size_wr - orig_size_wr;
}

/*
 * nss_stats_banner()
 *	Printing banner for node.
 */
size_t nss_stats_banner(char *lbuf, size_t size_wr, size_t size_al, char *node, int core)
{
	uint16_t banner_char_length, i;
	size_t orig_size_wr = size_wr;
	char node_upr[NSS_STATS_NODE_NAME_MAX + 1];

	if (strlen(node) > NSS_STATS_NODE_NAME_MAX) {
		nss_warning("Node name %s larger than %d characters\n", node, NSS_STATS_NODE_NAME_MAX);
		return 0;
	}

	size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "\n");
	for (i = 0; i < NSS_STATS_BANNER_MAX_LENGTH ; i++) {
		size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "_");
	}
	if (core > NSS_STATS_SINGLE_CORE) {
		banner_char_length = (uint16_t)((NSS_STATS_BANNER_MAX_LENGTH - (strlen(node) + NSS_STATS_BANNER_SPACES)) / 2);
	} else {
		banner_char_length = (uint16_t)((NSS_STATS_BANNER_MAX_LENGTH - (strlen(node) + 2)) / 2);
	}

	size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "\n\n");
	for (i = 0; i < banner_char_length; i++) {
		size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "<");
	}

	strlcpy(node_upr, node, NSS_STATS_NODE_NAME_MAX);
	for (i = 0; node_upr[i] != '\0' && i < NSS_STATS_NODE_NAME_MAX; i++) {
		node_upr[i] = toupper(node_upr[i]);
	}

	/*
	 * TODO: Enhance so that both core0 and core1 print the same way for a
	 * node that has presence in both cores. i.e. Core0 should have [CORE 0]
	 * and not just Core1.
	 */
	if (core > 1) {
		size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, " %s [CORE %d] ", node_upr, core);
	} else {
		size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, " %s ", node_upr);
	}
	for (i = 0; i < banner_char_length; i++) {
		size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, ">");
	}

	size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "\n");
	for (i = 0; i < NSS_STATS_BANNER_MAX_LENGTH; i++) {
		size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "_");
	}

	size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "\n\n");
	return size_wr - orig_size_wr;
}

/*
 * nss_stats_print()
 *	Helper API to print stats.
 */
size_t nss_stats_print(char *node, char *stat_details, int instance, struct nss_stats_info *stats_info,
				uint64_t *stats_val, uint16_t max, char *lbuf, size_t size_wr, size_t size_al)
{
	uint16_t i, j;
	uint16_t maxlen = 0;
	char stats_string[NSS_STATS_MAX_STR_LENGTH];
	size_t orig_size_wr = size_wr;
	char node_lwr[NSS_STATS_NODE_NAME_MAX + 1];

	if (strlen(node) > NSS_STATS_NODE_NAME_MAX) {
		nss_warning("Node name %s (%u chars) is longer than max chars of %d\n",
				node, (uint32_t)strlen(node), NSS_STATS_NODE_NAME_MAX);
		return 0;
	}

	/*
	 * Calculating the maximum of the array for indentation purposes.
	 */
	for (i = 0; i < max; i++){
		if (strlen(stats_info[i].stats_name) > maxlen) {
			maxlen = strlen(stats_info[i].stats_name);
		}
	}

	if (stat_details != NULL) {
		size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "\n#%s\n\n", stat_details);
	}

	for (i = 0; i < max; i++){
		if (nonzero_stats_print == 1 && stats_val[i] == 0) {
			continue;
		}

		strlcpy(stats_string, stats_info[i].stats_name, NSS_STATS_MAX_STR_LENGTH);

		/*
		 * Converting  uppercase to lower case.
		 */
		for (j = 0; stats_string[j] != '\0' && j < NSS_STATS_MAX_STR_LENGTH; j++) {
			stats_string[j] = tolower(stats_string[j]);
		}

		strlcpy(node_lwr, node, NSS_STATS_NODE_NAME_MAX);
		for (j = 0; node_lwr[j] != '\0' && j < NSS_STATS_NODE_NAME_MAX; j++) {
			node_lwr[j] = tolower(node_lwr[j]);
		}

		/*
		 * Space before %s is needed to avoid printing stat name from start of the line.
		 */
		if (instance < 0) {
			size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "\t%s_%s", node_lwr, stats_string);
		} else {
			size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "\t%s[%d]_%s", node_lwr, instance, stats_string);
		}

		for (j = 0; j < (1 + maxlen - strlen(stats_string)); j++){
			size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, " ");
		}

		size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "= %llu", stats_val[i]);
		size_wr = nss_stats_spacing(stats_val[i], lbuf, size_wr, size_al);

		/*
		 * Switch case will take care of the indentation and spacing details.
		 */
		switch (stats_info[i].stats_type) {
		case NSS_STATS_TYPE_COMMON:
			size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "common\n");
			break;

		case NSS_STATS_TYPE_SPECIAL:
			size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "special\n");
			break;

		case NSS_STATS_TYPE_DROP:
			size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "drop\n");
			break;

		case NSS_STATS_TYPE_ERROR:
			size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "error\n");
			break;

		case NSS_STATS_TYPE_EXCEPTION:
			size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "exception\n");
			break;

		default:
			nss_warning("unknown statistics type");
			break;
		}
	}

	return size_wr - orig_size_wr;
}

/*
 * nss_stats_create_dentry()
 *	Create statistics debug entry for subsystem.
 */
void nss_stats_create_dentry(char *name, const struct file_operations *ops)
{
	if (!debugfs_create_file(name, 0400, nss_top_main.stats_dentry, &nss_top_main, ops)) {
		nss_warning("Failed to create debug entry for subsystem %s\n", name);
	}
}

/*
 * TODO: Move the rest of the code to (nss_wt_stats.c, nss_gmac_stats.c) accordingly.
 */

/*
 * gmac_stats_ops
 */
NSS_STATS_DECLARE_FILE_OPERATIONS(gmac);

/*
 * wt_stats_ops
 */
NSS_STATS_DECLARE_FILE_OPERATIONS(wt);

/*
 * nss_stats_init()
 *	Enable NSS statistics.
 */
void nss_stats_init(void)
{
	struct dentry *core_dentry = NULL;
	struct dentry *wt_dentry = NULL;
	char file_name[10];
	int i;

	/*
	 * NSS driver entry
	 */
	nss_top_main.top_dentry = debugfs_create_dir("qca-nss-drv", NULL);
	if (unlikely(nss_top_main.top_dentry == NULL)) {
		nss_warning("Failed to create qca-nss-drv directory in debugfs");

		/*
		 * Non availability of debugfs directory is not a catastrophy.
		 * We can still go ahead with other initialization.
		 */
		return;
	}

	nss_top_main.stats_dentry = debugfs_create_dir("stats", nss_top_main.top_dentry);
	if (unlikely(nss_top_main.stats_dentry == NULL)) {
		nss_warning("Failed to create qca-nss-drv directory in debugfs");

		/*
		 * Non availability of debugfs directory is not a catastrophy.
		 * We can still go ahead with rest of initialization.
		 */
		return;
	}

	/*
	 * Create files to obtain statistics.
	 */

	/*
	 * drv_stats
	 */
	nss_drv_stats_dentry_create();

	/*
	 * gmac_stats
	 */
	nss_stats_create_dentry("gmac", &nss_gmac_stats_ops);

	/*
	 * Per-project stats
	 */
	nss_top_main.project_dentry = debugfs_create_dir("project",
						nss_top_main.stats_dentry);
	if (unlikely(nss_top_main.project_dentry == NULL)) {
		nss_warning("Failed to create qca-nss-drv/stats/project directory in debugfs");
		return;
	}

	for (i = 0; i < nss_top_main.num_nss; ++i) {
		memset(file_name, 0, sizeof(file_name));
		scnprintf(file_name, sizeof(file_name), "core%d", i);
		core_dentry = debugfs_create_dir(file_name,
						nss_top_main.project_dentry);
		if (unlikely(core_dentry == NULL)) {
			nss_warning("Failed to create qca-nss-drv/stats/project/core%d directory in debugfs", i);
			return;
		}

		wt_dentry = debugfs_create_file("worker_threads",
						0400,
						core_dentry,
						&(nss_top_main.nss[i]),
						&nss_wt_stats_ops);
		if (unlikely(wt_dentry == NULL)) {
			nss_warning("Failed to create qca-nss-drv/stats/project/core%d/worker_threads file in debugfs", i);
			return;
		}
	}

	nss_log_init();
}
