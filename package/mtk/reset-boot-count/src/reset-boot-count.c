// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2024 MediaTek Inc. All rights reserved.
 *
 * Helper for resetting boot count of A/B boot systems
 *
 * Author: Weijie Gao <weijie.gao@mediatek.com>
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/arm-smccc.h>

#define RBC "reset_boot_count"

#define MTK_SIP_READ_NONRST_REG			0xC2000550
#define MTK_SIP_WRITE_NONRST_REG		0xC2000551

static struct proc_dir_entry *rbc_entry;

static bool dual_boot_get_boot_count(u32 *retslot, u32 *retcnt)
{
	struct arm_smccc_res res = {0};
	u32 val, slot;
	s8 neg, pos;

	arm_smccc_smc(MTK_SIP_READ_NONRST_REG, 0, 0, 0, 0, 0, 0, 0, &res);

	val = (u32)res.a0;

	/* slot: val[31..24] = -slot, val[23..16] = slot */
	pos = (val >> 16) & 0xff;
	neg = (val >> 24) & 0xff;

	if (!(pos >= 0 && neg <= 0 && pos + neg == 0)) {
		pr_debug("slot of boot count is invalid\n");
		goto err;
	}

	slot = pos;

	/* count: val[15..8] = -count, val[7..0] = count */
	pos = val & 0xff;
	neg = (val >> 8) & 0xff;

	if (!(pos >= 0 && neg <= 0 && pos + neg == 0)) {
		pr_debug("count of boot count is invalid\n");
		goto err;
	}

	if (retslot)
		*retslot = slot;

	if (retcnt)
		*retcnt = pos;

	return true;

err:
	if (retslot)
		*retslot = 0;

	if (retcnt)
		*retcnt = 0;

	return false;
}

static void dual_boot_set_boot_count(u32 slot, u32 count)
{
	struct arm_smccc_res res = {0};
	u32 val;
	s32 neg;

	if (slot > 127 || count > 127)
		return;

	neg = -count;
	val = count | ((neg << 8) & 0xff00);

	neg = -slot;
	val = val | ((uint32_t)slot << 16) | ((neg << 24) & 0xff000000);

	arm_smccc_smc(MTK_SIP_WRITE_NONRST_REG, 0, val, 0, 0, 0, 0, 0, &res);
}

static int rbc_display(struct seq_file *seq, void *v)
{
	return 0;
}

static int rbc_open(struct inode *inode, struct file *file)
{
	return single_open(file, rbc_display, inode->i_private);
}

static ssize_t rbc_write(struct file *file, const char __user *buffer,
			 size_t count, loff_t *pos)
{
	u32 slot;

	dual_boot_get_boot_count(&slot, NULL);
	dual_boot_set_boot_count(slot, 0);

	pr_info("Boot count reset\n");

	return count;
}

static const struct proc_ops rbc_fops = {
	.proc_open  = rbc_open,
	.proc_read = seq_read,
	.proc_write = rbc_write,
	.proc_lseek  = seq_lseek,
	.proc_release = single_release,
};

static int __init rbc_init(void)
{
	rbc_entry = proc_create(RBC, 0200, NULL, &rbc_fops);

	if (!rbc_entry)
		pr_err("failed to create proc entry " RBC);

	return 0;
}

static void __exit rbc_exit(void)
{
	remove_proc_entry(RBC, NULL);
}

module_init(rbc_init);
module_exit(rbc_exit);

MODULE_AUTHOR("Weijie Gao <weijie.gao@mediatek.com>");
MODULE_DESCRIPTION("Kernel module for resetting boot count of A/B boot systems");
MODULE_LICENSE("GPL");
