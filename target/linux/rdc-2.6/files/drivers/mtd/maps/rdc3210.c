/*
 * $Id$
 * Copyright (C) 2005 Gemtek Corporation, Dante Su (dante_su@gemtek.com.tw)
 * Copyright (C) 2006  Florian Fainelli <florian@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <asm/io.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/mtd/map.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/vmalloc.h>

#define WINDOW_ADDR             0xFFC00000
#define WINDOW_SIZE             0x00400000
#define BUSWIDTH 		2

extern int parse_redboot_partitions(struct mtd_info *master, struct mtd_partition **pparts, unsigned long fis_origin);

static struct mtd_partition *parsed_parts;
static struct mtd_info *rdc3210_mtd_info;

static struct map_info rdc3210_map = {
	.name = "rdc3210",
	.size = WINDOW_SIZE,
	.bankwidth = BUSWIDTH,
	.phys = WINDOW_ADDR,
};

static struct mtd_partition rdc3210_parts[] = {
	{ name: "linux",   offset:  0,          size: 0x003C0000 },     /* 3840 KB = (Kernel + ROMFS) = (768 KB + 3072 KB) */
	{ name: "romfs",   offset:  0x000C0000, size: 0x00300000 },     /* 3072 KB */
	{ name: "nvram",   offset:  0x003C0000, size: 0x00010000 },     		/*   64 KB */
	{ name: "factory", offset:  0x003D0000, size: 0x00010000 },     		/*   64 KB */
	{ name: "bootldr", offset:  0x003E0000, size: 0x00020000, mask_flags: MTD_WRITEABLE },/*  128 KB */
};

static int __init rdc3210_mtd_init(void)
{
	printk(KERN_INFO "rdc3210: 0x%08x at 0x%08x\n", WINDOW_SIZE, WINDOW_ADDR);
	rdc3210_map.virt = ioremap_nocache(WINDOW_ADDR, WINDOW_SIZE);

	if (!rdc3210_map.virt) {
		printk(KERN_ERR "rdc3210: failed to ioremap\n");
		return -EIO;
	}

	simple_map_init(&rdc3210_map);

	rdc3210_mtd_info = do_map_probe("cfi_probe", &rdc3210_map);

	if (rdc3210_mtd_info)
	{
		rdc3210_mtd_info->owner = THIS_MODULE;
		int parsed_nr_parts = 0;
		char * part_type;

#ifdef CONFIG_MTD_REDBOOT_PARTS
		if (parsed_nr_parts == 0)
		{
			int ret = parse_redboot_partitions(rdc3210_mtd_info, &parsed_parts, 0);
			if (ret > 0)
			{
				part_type = "RedBoot";
				parsed_nr_parts = ret;
			} else {
				printk(KERN_ERR "rdc3210: failed to parse RedBoot partitions, using static mapping\n");
				add_mtd_partitions(rdc3210_mtd_info, rdc3210_parts, sizeof(rdc3210_parts)/sizeof(rdc3210_parts[0]));
				return -ENXIO;
			}
		}

		add_mtd_partitions(rdc3210_mtd_info, parsed_parts, parsed_nr_parts);
		return 0;
	}
#endif
	iounmap(rdc3210_map.virt);
	return -ENXIO;
}

static void __exit rdc3210_mtd_cleanup(void)
{
	if (rdc3210_mtd_info)
	{
		del_mtd_partitions(rdc3210_mtd_info);
		map_destroy(rdc3210_mtd_info);
	}

	if (rdc3210_map.virt) {
		iounmap(rdc3210_map.virt);
		rdc3210_map.virt = 0;
	}
}

module_init(rdc3210_mtd_init);
module_exit(rdc3210_mtd_cleanup);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("RDC3210 flash map driver");
MODULE_AUTHOR("Florian Fainelli <florian@openwrt.org>");
