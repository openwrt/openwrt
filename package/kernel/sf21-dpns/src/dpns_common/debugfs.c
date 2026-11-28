// SPDX-License-Identifier: GPL-2.0
/*
 * Raw access to DPNS registers and search engine tables, for debugging:
 *
 *   /sys/kernel/debug/dpns/regs   write "ADDR" or "ADDR VALUE", read returns "ADDR: VALUE"
 *   /sys/kernel/debug/dpns/table  write "r ID ADDR [WORDS]" or "w ID ADDR W0 [W1 ...]",
 *                                 read returns the words of the last access
 *
 * Numbers accept 0x prefixes. Register addresses are offsets in the DPNS block.
 */
#include <linux/debugfs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include "ops.h"

#define DBG_TBL_WORDS_MAX	16
#define DPNS_REG_SPACE		0x1000000

static COMMON_t *dbg_priv;
static struct dentry *dbg_dir;
static DEFINE_MUTEX(dbg_lock);
static u32 dbg_reg;
static u32 dbg_tbl_id, dbg_tbl_addr, dbg_tbl_words = 4;
static u32 dbg_tbl_data[DBG_TBL_WORDS_MAX];

static int dbg_parse(const char __user *ubuf, size_t len, char *buf, size_t sz,
		     char **argv, int argv_max)
{
	char *p;
	int argc = 0;

	if (len >= sz)
		return -EINVAL;
	if (copy_from_user(buf, ubuf, len))
		return -EFAULT;
	buf[len] = '\0';
	p = buf;
	while (argc < argv_max) {
		char *tok = strsep(&p, " \t\n");

		if (!tok)
			break;
		if (*tok)
			argv[argc++] = tok;
	}
	return argc;
}

static int regs_show(struct seq_file *s, void *v)
{
	seq_printf(s, "0x%06x: 0x%08x\n", dbg_reg, sf_readl(dbg_priv, dbg_reg));
	return 0;
}

static ssize_t regs_write(struct file *file, const char __user *ubuf, size_t len, loff_t *off)
{
	char buf[64], *argv[2];
	u32 addr, val;
	int argc;

	argc = dbg_parse(ubuf, len, buf, sizeof(buf), argv, 2);
	if (argc < 1)
		return argc < 0 ? argc : -EINVAL;
	if (kstrtou32(argv[0], 0, &addr) || addr >= DPNS_REG_SPACE || (addr & 3))
		return -EINVAL;
	dbg_reg = addr;
	if (argc == 2) {
		if (kstrtou32(argv[1], 0, &val))
			return -EINVAL;
		sf_writel(dbg_priv, addr, val);
	}
	return len;
}

static int regs_open(struct inode *inode, struct file *file)
{
	return single_open(file, regs_show, NULL);
}

static const struct file_operations regs_fops = {
	.owner = THIS_MODULE,
	.open = regs_open,
	.read = seq_read,
	.write = regs_write,
	.llseek = seq_lseek,
	.release = single_release,
};

static int table_show(struct seq_file *s, void *v)
{
	u32 i;

	mutex_lock(&dbg_lock);
	seq_printf(s, "id %u addr 0x%x:", dbg_tbl_id, dbg_tbl_addr);
	for (i = 0; i < dbg_tbl_words; i++)
		seq_printf(s, " 0x%08x", dbg_tbl_data[i]);
	seq_putc(s, '\n');
	mutex_unlock(&dbg_lock);
	return 0;
}

static ssize_t table_write(struct file *file, const char __user *ubuf, size_t len, loff_t *off)
{
	char buf[256], *argv[3 + DBG_TBL_WORDS_MAX];
	u32 id, addr, n, i;
	int argc;

	argc = dbg_parse(ubuf, len, buf, sizeof(buf), argv, ARRAY_SIZE(argv));
	if (argc < 3)
		return argc < 0 ? argc : -EINVAL;
	if (kstrtou32(argv[1], 0, &id) || id > 0xff || kstrtou32(argv[2], 0, &addr) || addr > 0xffff)
		return -EINVAL;

	mutex_lock(&dbg_lock);
	if (argv[0][0] == 'r') {
		n = 4;
		if (argc > 3 && (kstrtou32(argv[3], 0, &n) || !n || n > DBG_TBL_WORDS_MAX))
			goto inval;
		memset(dbg_tbl_data, 0, sizeof(dbg_tbl_data));
		dbg_priv->table_read(dbg_priv, id, addr, dbg_tbl_data, n * sizeof(u32));
	} else if (argv[0][0] == 'w' && argc > 3) {
		n = argc - 3;
		for (i = 0; i < n; i++)
			if (kstrtou32(argv[3 + i], 0, &dbg_tbl_data[i]))
				goto inval;
		dbg_priv->table_write(dbg_priv, id, addr, dbg_tbl_data, n * sizeof(u32));
	} else {
		goto inval;
	}
	dbg_tbl_id = id;
	dbg_tbl_addr = addr;
	dbg_tbl_words = n;
	mutex_unlock(&dbg_lock);
	return len;
inval:
	mutex_unlock(&dbg_lock);
	return -EINVAL;
}

static int table_open(struct inode *inode, struct file *file)
{
	return single_open(file, table_show, NULL);
}

static const struct file_operations table_fops = {
	.owner = THIS_MODULE,
	.open = table_open,
	.read = seq_read,
	.write = table_write,
	.llseek = seq_lseek,
	.release = single_release,
};

void dpns_common_debugfs_init(COMMON_t *priv)
{
	dbg_priv = priv;
	dbg_dir = debugfs_create_dir("dpns", NULL);
	debugfs_create_file("regs", 0600, dbg_dir, NULL, &regs_fops);
	debugfs_create_file("table", 0600, dbg_dir, NULL, &table_fops);
}

void dpns_common_debugfs_exit(void)
{
	debugfs_remove_recursive(dbg_dir);
	dbg_dir = NULL;
}
