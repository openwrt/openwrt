/*
 *  Copyright (C) 2013 Felix Fietkau <nbd@nbd.name>
 *  Copyright (C) 2013 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 */

#define pr_fmt(fmt)	KBUILD_MODNAME ": " fmt

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/magic.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/byteorder/generic.h>

#include "mtdsplit.h"

static const struct mtd_erase_region_info* mtd_erase_region_by_offset(uint64_t offset, const struct mtd_info *mtd)
{
	const struct mtd_erase_region_info* result = mtd->eraseregions;
	int i = 0;

	// Look for the region containing our offset
	while ((i < mtd->numeraseregions) && (result->offset <= offset)) {
		i++;
		result++;
	}

	// Actually we've found the next region
	if (i > 0) {
	    result--;
	}

	return result;
}

static uint64_t mtd_roundup_to_eb_in_region(uint64_t offset, uint64_t value, struct mtd_info *mtd)
{
	const struct mtd_erase_region_info* erase_region;
	uint64_t erase_size = 0;
	uint64_t erase_size_mask = 0;

	if (mtd->numeraseregions == 0)
		return mtd_roundup_to_eb(value, mtd);

	erase_region = mtd_erase_region_by_offset(offset, mtd);
	erase_size = erase_region->erasesize; // to convert to uint64_t
	erase_size_mask = erase_size - 1;

	return (value + erase_size_mask) & ~erase_size_mask;
}

static uint64_t mtd_rounddown_to_eb_in_region(uint64_t offset, uint64_t value, struct mtd_info *mtd)
{
	const struct mtd_erase_region_info* erase_region;
	uint64_t erase_size = 0;
	uint64_t erase_size_mask = 0;

	if (mtd->numeraseregions == 0)
		return mtd_rounddown_to_eb(value, mtd);

	erase_region = mtd_erase_region_by_offset(offset, mtd);
	erase_size = erase_region->erasesize; // to convert to uint64_t
	erase_size_mask = erase_size - 1;

	return value & ~erase_size_mask;
}


static int
mtdsplit_parse_squashfs(struct mtd_info *master,
			const struct mtd_partition **pparts,
			struct mtd_part_parser_data *data)
{
	struct mtd_partition *part;
	struct mtd_info *parent_mtd;
	size_t part_offset;
	size_t absolute_offset;
	size_t squashfs_len;
	int err;

	err = mtd_get_squashfs_len(master, 0, &squashfs_len);
	if (err)
		return err;

	parent_mtd = mtd_get_master(master);
	part_offset = mtdpart_get_offset(master);

	part = kzalloc(sizeof(*part), GFP_KERNEL);
	if (!part) {
		pr_alert("unable to allocate memory for \"%s\" partition\n",
			 ROOTFS_SPLIT_NAME);
		return -ENOMEM;
	}

	part->name = ROOTFS_SPLIT_NAME;
	absolute_offset = mtd_roundup_to_eb_in_region(part_offset, part_offset + squashfs_len, parent_mtd);
	part->offset = absolute_offset - part_offset;
	part->size = mtd_rounddown_to_eb_in_region(absolute_offset, master->size - part->offset, parent_mtd);

	*pparts = part;
	return 1;
}

static struct mtd_part_parser mtdsplit_squashfs_parser = {
	.owner = THIS_MODULE,
	.name = "squashfs-split",
	.parse_fn = mtdsplit_parse_squashfs,
	.type = MTD_PARSER_TYPE_ROOTFS,
};

static int __init mtdsplit_squashfs_init(void)
{
	register_mtd_parser(&mtdsplit_squashfs_parser);

	return 0;
}

subsys_initcall(mtdsplit_squashfs_init);
