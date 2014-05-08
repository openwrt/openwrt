/*
 *  Copyright (C) 2013 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/byteorder/generic.h>

#include "mtdsplit.h"

#define SEAMA_MAGIC		0x5EA3A417
#define SEAMA_NR_PARTS		2
#define SEAMA_MIN_ROOTFS_OFFS	0x80000	/* 512KiB */

struct seama_header {
	__be32	magic;		/* should always be SEAMA_MAGIC. */
	__be16	reserved;	/* reserved for  */
	__be16	metasize;	/* size of the META data */
	__be32	size;		/* size of the image */
};

static int mtdsplit_parse_seama(struct mtd_info *master,
				struct mtd_partition **pparts,
				struct mtd_part_parser_data *data)
{
	struct seama_header hdr;
	size_t hdr_len, retlen, kernel_size;
	size_t rootfs_offset;
	struct mtd_partition *parts;
	int err;

	hdr_len = sizeof(hdr);
	err = mtd_read(master, 0, hdr_len, &retlen, (void *) &hdr);
	if (err)
		return err;

	if (retlen != hdr_len)
		return -EIO;

	/* sanity checks */
	if (be32_to_cpu(hdr.magic) != SEAMA_MAGIC)
		return -EINVAL;

	kernel_size = hdr_len + be32_to_cpu(hdr.size) +
		      be16_to_cpu(hdr.metasize);
	if (kernel_size > master->size)
		return -EINVAL;

	/* Find the rootfs after the kernel. */
	err = mtd_check_rootfs_magic(master, kernel_size);
	if (!err) {
		rootfs_offset = kernel_size;
	} else {
		/*
		 * The size in the header might cover the rootfs as well.
		 * Start the search from an arbitrary offset.
		 */
		err = mtd_find_rootfs_from(master, SEAMA_MIN_ROOTFS_OFFS,
					   master->size, &rootfs_offset);
		if (err)
			return err;
	}

	parts = kzalloc(SEAMA_NR_PARTS * sizeof(*parts), GFP_KERNEL);
	if (!parts)
		return -ENOMEM;

	parts[0].name = KERNEL_PART_NAME;
	parts[0].offset = 0;
	parts[0].size = rootfs_offset;

	parts[1].name = ROOTFS_PART_NAME;
	parts[1].offset = rootfs_offset;
	parts[1].size = master->size - rootfs_offset;

	*pparts = parts;
	return SEAMA_NR_PARTS;
}

static struct mtd_part_parser mtdsplit_seama_parser = {
	.owner = THIS_MODULE,
	.name = "seama-fw",
	.parse_fn = mtdsplit_parse_seama,
	.type = MTD_PARSER_TYPE_FIRMWARE,
};

static int __init mtdsplit_seama_init(void)
{
	register_mtd_parser(&mtdsplit_seama_parser);

	return 0;
}

subsys_initcall(mtdsplit_seama_init);
