/*
 *  Parse MyLoader-style flash partition tables and produce a Linux partition
 *  array to match.
 *
 *  Copyright (C) 2007 OpenWrt.org
 *  Copyright (C) 2007 Gabor Juhos <juhosg@freemail.hu>
 *
 *  This file was based on drivers/mtd/redboot.c
 *  Author: Red Hat, Inc. - David Woodhouse <dwmw2@cambridge.redhat.com>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the
 *  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA  02110-1301, USA.
 */

#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/vmalloc.h>

#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>

#include <linux/byteorder/generic.h>
#include <asm/mach-adm5120/myloader.h>

#define NAME_LEN_MAX		20
#define NAME_MYLOADER		"MyLoader"
#define NAME_PARTITION_TABLE	"Partition Table"
#define BLOCK_LEN_MIN		0x10000

static int parse_myloader_partitions(struct mtd_info *master,
			struct mtd_partition **pparts,
			unsigned long origin)
{
	struct mylo_partition_table *tab;
	struct mylo_partition *part;
	struct mtd_partition *mtd_parts;
	struct mtd_partition *mtd_part;
	int num_parts;
	int ret, i;
	size_t retlen;
	size_t parts_len;
	char *names;
	unsigned long offset;
	unsigned long blocklen;

	tab = vmalloc(sizeof(*tab));
	if (!tab) {
		return -ENOMEM;
		goto out;
	}

	blocklen = master->erasesize;
	if (blocklen < BLOCK_LEN_MIN)
		blocklen = BLOCK_LEN_MIN;

	/* Partition Table is always located on the second erase block */
	offset = blocklen;
	printk(KERN_NOTICE "Searching for MyLoader partition table "
			"in %s at offset 0x%lx\n", master->name, offset);

	ret = master->read(master, offset, sizeof(*tab), &retlen,
			(void *)tab);

	if (ret)
		goto out;

	if (retlen != sizeof(*tab)) {
		ret = -EIO;
		goto out_free_buf;
	}

	/* Check for Partition Table magic number */
	if (tab->magic != le32_to_cpu(MYLO_MAGIC_PARTITIONS)) {
		printk(KERN_NOTICE "No MyLoader partition table detected "
			"in %s\n", master->name);
		ret = 0;
		goto out_free_buf;
	}

	/* The MyLoader and the Partition Table is always present */
	num_parts = 2;

	/* Detect number of used partitions */
	for (i = 0; i < MYLO_MAX_PARTITIONS; i++) {
		part = &tab->partitions[i];

		if (le16_to_cpu(part->type) == PARTITION_TYPE_FREE)
			continue;

		num_parts++;
	}


	mtd_parts = kzalloc((num_parts*sizeof(*mtd_part) + num_parts*NAME_LEN_MAX),
			 GFP_KERNEL);

	if (!mtd_parts) {
		ret = -ENOMEM;
		goto out_free_buf;
	}

	mtd_part = mtd_parts;
	names = (char *)&mtd_parts[num_parts];

	strcpy(NAME_MYLOADER, names);
	mtd_part->name = names;
	mtd_part->offset = 0;
	mtd_part->size = blocklen;
	mtd_part++;
	names += NAME_LEN_MAX;

	strcpy(NAME_PARTITION_TABLE, names);
	mtd_part->name = names;
	mtd_part->offset = blocklen;
	mtd_part->size = blocklen;
	mtd_part++;
	names += NAME_LEN_MAX;

	for (i = 0; i < MYLO_MAX_PARTITIONS; i++) {
		part = &tab->partitions[i];

		if (le16_to_cpu(part->type) == PARTITION_TYPE_FREE)
			continue;

		sprintf(names, "partition%d", i);
		mtd_part->name = names;
		mtd_part->offset = le32_to_cpu(part->addr);
		mtd_part->size = le32_to_cpu(part->size);
		mtd_part++;
		names += NAME_LEN_MAX;
	}

	*pparts = mtd_parts;
	ret = num_parts;

out_free_buf:
	vfree(tab);
out:
	return ret;
}

static struct mtd_part_parser mylo_mtd_parser = {
	.owner = THIS_MODULE,
	.parse_fn = parse_myloader_partitions,
	.name = NAME_MYLOADER,
};

static int __init mylo_mtd_parser_init(void)
{
	return register_mtd_parser(&mylo_mtd_parser);
}

static void __exit mylo_mtd_parser_exit(void)
{
	deregister_mtd_parser(&mylo_mtd_parser);
}

module_init(mylo_mtd_parser_init);
module_exit(mylo_mtd_parser_exit);

EXPORT_SYMBOL_GPL(parse_myloader_partitions);

MODULE_AUTHOR("Gabor Juhos <juhosg@freemail.hu>");
MODULE_DESCRIPTION("Parsing code for MyLoader partition tables");
MODULE_LICENSE("GPL");
