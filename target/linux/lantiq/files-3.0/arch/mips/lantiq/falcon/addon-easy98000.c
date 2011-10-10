/*
 *  EASY98000 CPLD Addon driver
 *
 *  Copyright (C) 2011 Thomas Langer <thomas.langer@lantiq.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2  as published
 *  by the Free Software Foundation.
 *
 */

#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

struct easy98000_reg_cpld {
	u16 cmdreg1;		/* 0x1 */
	u16 cmdreg0;		/* 0x0 */
	u16 idreg0;		/* 0x3 */
	u16 resreg;		/* 0x2 */
	u16 intreg;		/* 0x5 */
	u16 idreg1;		/* 0x4 */
	u16 ledreg;		/* 0x7 */
	u16 pcmconconfig;	/* 0x6 */
	u16 res0;		/* 0x9 */
	u16 ethledreg;		/* 0x8 */
	u16 res1[4];		/* 0xa-0xd */
	u16 cpld1v;		/* 0xf */
	u16 cpld2v;		/* 0xe */
};
static struct easy98000_reg_cpld * const cpld =
	(struct easy98000_reg_cpld *)(KSEG1 | 0x17c00000);
#define cpld_r8(reg) (__raw_readw(&cpld->reg) & 0xFF)
#define cpld_w8(val, reg) __raw_writew((val) & 0xFF, &cpld->reg)

int easy98000_addon_has_dm9000(void)
{
	if ((cpld_r8(idreg0) & 0xF) == 1)
		return 1;
	return 0;
}

#if defined(CONFIG_PROC_FS)
typedef void (*cpld_dump) (struct seq_file *s);
struct proc_entry {
	char *name;
	void *callback;
};

static int cpld_proc_show ( struct seq_file *s, void *p )
{
	cpld_dump dump = s->private;

	if ( dump != NULL )
		dump(s);

	return 0;
}

static int cpld_proc_open ( struct inode *inode, struct file *file )
{
	return single_open ( file, cpld_proc_show, PDE(inode)->data );
}

static void cpld_versions_get ( struct seq_file *s )
{
	seq_printf(s, "CPLD1: V%d\n", cpld_r8(cpld1v));
	seq_printf(s, "CPLD2: V%d\n", cpld_r8(cpld2v));
}

static void cpld_ebu_module_get ( struct seq_file *s )
{
	u8 addon_id;

	addon_id = cpld_r8(idreg0) & 0xF;
	switch (addon_id) {
	case 0xF: /* nothing connected */
		break;
	case 1:
		seq_printf(s, "Ethernet Controller module (dm9000)\n");
		break;
	default:
		seq_printf(s, "Unknown EBU module (EBU_ID=0x%02X)\n", addon_id);
		break;
	}
}

static void cpld_xmii_module_get ( struct seq_file *s )
{
	u8 addon_id;
	char *mod = NULL;

	addon_id = cpld_r8(idreg1) & 0xF;
	switch (addon_id) {
	case 0xF:
		mod = "no module";
		break;
	case 0x1:
		mod = "RGMII module";
		break;
	case 0x4:
		mod = "GMII MAC Mode (XWAY TANTOS-3G)";
		break;
	case 0x6:
		mod = "TMII MAC Mode (XWAY TANTOS-3G)";
		break;
	case 0x8:
		mod = "GMII PHY module";
		break;
	case 0x9:
		mod = "MII PHY module";
		break;
	case 0xA:
		mod = "RMII PHY module";
		break;
	default:
		break;
	}
	if (mod)
		seq_printf(s, "%s\n", mod);
	else
		seq_printf(s, "unknown xMII module (xMII_ID=0x%02X)\n", addon_id);
}

static struct proc_entry proc_entries[] = {
	{"versions",	cpld_versions_get},
	{"ebu",		cpld_ebu_module_get},
	{"xmii",	cpld_xmii_module_get},
};

static struct file_operations ops = {
	.owner   = THIS_MODULE,
	.open    = cpld_proc_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = single_release,
};

static void cpld_proc_entry_create(struct proc_dir_entry *parent_node,
				   struct proc_entry *proc_entry)
{
	proc_create_data ( proc_entry->name, (S_IFREG | S_IRUGO), parent_node,
			   &ops, proc_entry->callback);
}

static int cpld_proc_install(void)
{
	struct proc_dir_entry *driver_proc_node;

	driver_proc_node = proc_mkdir("cpld", NULL);
	if (driver_proc_node != NULL) {
		int i;
		for (i = 0; i < ARRAY_SIZE(proc_entries); i++)
			cpld_proc_entry_create(driver_proc_node,
					      &proc_entries[i]);
	} else {
		printk("cannot create proc entry");
		return -1;
	}
	return 0;
}
#else
static inline int cpld_proc_install(void) {}
#endif

static int easy98000_addon_probe(struct platform_device *pdev)
{
	return cpld_proc_install();
}

static int easy98000_addon_remove(struct platform_device *pdev)
{
#if defined(CONFIG_PROC_FS)
	char buf[64];
	int i;

	for (i = 0; i < sizeof(proc_entries) / sizeof(proc_entries[0]); i++) {
		sprintf(buf, "cpld/%s", proc_entries[i].name);
		remove_proc_entry(buf, 0);
	}
	remove_proc_entry("cpld", 0);
#endif
	return 0;
}

static struct platform_driver easy98000_addon_driver = {
	.probe = easy98000_addon_probe,
	.remove = __devexit_p(easy98000_addon_remove),
	.driver = {
		.name = "easy98000_addon",
		.owner = THIS_MODULE,
	},
};

int __init easy98000_addon_init(void)
{
	return platform_driver_register(&easy98000_addon_driver);
}

void __exit easy98000_addon_exit(void)
{
	platform_driver_unregister(&easy98000_addon_driver);
}

module_init(easy98000_addon_init);
module_exit(easy98000_addon_exit);
