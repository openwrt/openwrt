/*
 *  Copyright (C) 2013 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2017 Weijie Gao <hackpascal@gmail.com>
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
#include <linux/sizes.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/byteorder/generic.h>

#include "mtdsplit.h"

#define CVIMG_BLOCK_SIZE	SZ_64K
#define CVIMG_SEARCH_LIMIT	SZ_2M

#define CVIMG_MIN_ROOTFS_OFFSET	0xa0000
#define CVIMG_ROOTFS_ALIGNMENT	SZ_4K

#define SIGNATURE_LEN		4

#define FAKE_ROOTFS_IDENT	"FAKE"

#define FAKE_ROOTFS_IDENT_SIZE	4
#define FAKE_ROOTFS_IDENT_OFS	4
#define FAKE_ROOTFS_SIZE_OFS	16

#define CVIMG_NR_PARTS		2

enum data_type {
	DATA_UNKNOWN,
	DATA_KERNEL,
	DATA_KERNEL_WITH_ROOTFS
};

struct signature {
	enum data_type	type;
	const char	*sig;
};

const static struct signature sig_known[] = {
	{
		.type	= DATA_KERNEL,
		.sig	= "cs6b"
	}, {
		.type	= DATA_KERNEL,
		.sig	= "cs6c"
	}, {
		.type	= DATA_KERNEL,
		.sig	= "csys"
	}, {
		.type	= DATA_KERNEL_WITH_ROOTFS,
		.sig	= "cr6b"
	}, {
		.type	= DATA_KERNEL_WITH_ROOTFS,
		.sig	= "cr6c"
	}, {
		.type	= DATA_KERNEL_WITH_ROOTFS,
		.sig	= "csro"
	},
};

struct img_header {
    unsigned char signature[SIGNATURE_LEN];
    unsigned int start_addr;
    unsigned int burn_addr;
    unsigned int len;
};

static enum data_type signature_type(const char *sig)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(sig_known); i++) {
		if (!memcmp(sig, sig_known[i].sig, SIGNATURE_LEN))
			return sig_known[i].type;
	}

	return DATA_UNKNOWN;
}

static int mtd_find_cvimg_rootfs_from(struct mtd_info *mtd,
			 size_t from,
			 size_t limit,
			 size_t *ret_offset,
			 enum mtdsplit_part_type *type)
{
	size_t offset;
	int err;

	for (offset = from; offset < limit;
	     offset = ALIGN(offset + CVIMG_ROOTFS_ALIGNMENT,
			    CVIMG_ROOTFS_ALIGNMENT)) {
		err = mtd_check_rootfs_magic(mtd, offset, type);
		if (err)
			continue;

		*ret_offset = offset;
		return 0;
	}

	return -ENODEV;
}

static int mtdsplit_parse_cvimg(struct mtd_info *master,
				const struct mtd_partition **pparts,
				struct mtd_part_parser_data *data)
{
	struct img_header hdr;
	size_t hdr_len, retlen;
	size_t temp_offset, rootfs_offset;
	struct mtd_partition *parts;
	char ident[FAKE_ROOTFS_IDENT_SIZE];
	u32 fake_rootfs_size;
	int err;

	hdr_len = sizeof(hdr);
	err = mtd_read(master, 0, hdr_len, &retlen, (void *) &hdr);
	if (err)
		return err;

	if (be32_to_cpu(hdr.len) > master->size)
		return -EINVAL;

	temp_offset = ALIGN(sizeof(hdr) +
			     be32_to_cpu(hdr.len), sizeof (uint32_t));

	switch (signature_type(hdr.signature)) {
	case DATA_KERNEL:
	case DATA_KERNEL_WITH_ROOTFS:
		err = mtd_check_rootfs_magic(master, temp_offset, NULL);
		if (!err) {
			rootfs_offset = temp_offset;
			break;
		}

		err = mtd_find_cvimg_rootfs_from(master, CVIMG_MIN_ROOTFS_OFFSET,
					   master->size, &rootfs_offset, NULL);
		if (err)
			return err;

		err = mtd_read(master, rootfs_offset + FAKE_ROOTFS_IDENT_OFS,
			       FAKE_ROOTFS_IDENT_SIZE, &retlen, ident);
		if (err)
			return err;

		if (!memcmp(ident, FAKE_ROOTFS_IDENT, FAKE_ROOTFS_IDENT_SIZE)) {
			err = mtd_read(master,
				rootfs_offset + FAKE_ROOTFS_SIZE_OFS,
				sizeof (fake_rootfs_size), &retlen,
				(void *) &fake_rootfs_size);
			if (err)
				return err;

			fake_rootfs_size = ntohl(fake_rootfs_size);

			err = mtd_check_rootfs_magic(master,
				rootfs_offset + fake_rootfs_size, NULL);
			if (!err) {
				rootfs_offset += fake_rootfs_size;
				break;
			}

			err = mtd_find_cvimg_rootfs_from(master,
				ALIGN(rootfs_offset + fake_rootfs_size,
					CVIMG_ROOTFS_ALIGNMENT),
					master->size, &rootfs_offset, NULL);
			if (err)
				return err;
		}

		break;
	default:
		return -EINVAL;
	}

	if (rootfs_offset > master->size)
		return -EINVAL;

	parts = kzalloc(CVIMG_NR_PARTS * sizeof(*parts), GFP_KERNEL);
	if (!parts)
		return -ENOMEM;

	parts[0].name = KERNEL_PART_NAME;
	parts[0].offset = 0;
	parts[0].size = rootfs_offset;

	parts[1].name = ROOTFS_PART_NAME;
	parts[1].offset = rootfs_offset;
	parts[1].size = master->size - rootfs_offset;

	*pparts = parts;
	return CVIMG_NR_PARTS;
}

static struct mtd_part_parser mtdsplit_cvimg_parser = {
	.owner = THIS_MODULE,
	.name = "cvimg-fw",
	.parse_fn = mtdsplit_parse_cvimg,
	.type = MTD_PARSER_TYPE_FIRMWARE,
};

static int __init mtdsplit_cvimg_init(void)
{
	register_mtd_parser(&mtdsplit_cvimg_parser);

	return 0;
}

subsys_initcall(mtdsplit_cvimg_init);
