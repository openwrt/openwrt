/*
 * MTD split for Broadcom Whole Flash Image
 *
 * Copyright (C) 2020 Álvaro Fernández Rojas <noltari@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 *
 */

#define je16_to_cpu(x) ((x).v16)
#define je32_to_cpu(x) ((x).v32)

#include <linux/crc32.h>
#include <linux/init.h>
#include <linux/jffs2.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/byteorder/generic.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>

#include "mtdsplit.h"

#define BCM_WFI_PARTS		3

#define CFERAM_NAME		"cferam"
#define CFERAM_NAME_LEN		(sizeof(CFERAM_NAME) - 1)
#define KERNEL_NAME		"vmlinux.lz"
#define KERNEL_NAME_LEN		(sizeof(KERNEL_NAME) - 1)
#define OPENWRT_NAME		"1-openwrt"
#define OPENWRT_NAME_LEN	(sizeof(OPENWRT_NAME) - 1)

#define UBI_MAGIC		0x55424923

static u32 jffs2_dirent_crc(struct jffs2_raw_dirent *node)
{
	return crc32(0, node, sizeof(struct jffs2_raw_dirent) - 8);
}

static bool jffs2_dirent_valid(struct jffs2_raw_dirent *node)
{
	return ((je16_to_cpu(node->magic) == JFFS2_MAGIC_BITMASK) &&
		(je16_to_cpu(node->nodetype) == JFFS2_NODETYPE_DIRENT) &&
		je32_to_cpu(node->ino) &&
		je32_to_cpu(node->node_crc) == jffs2_dirent_crc(node));
}

static int jffs2_find_file(struct mtd_info *master, uint8_t *buf,
			   const char *name, size_t name_len,
			   loff_t *offs)
{
	struct jffs2_raw_dirent *node;
	bool valid = false;
	size_t retlen;
	uint16_t magic;
	int rc;

	for (; *offs < master->size; *offs += master->erasesize) {
		unsigned int block_offs = 0;

		/* Skip CFE erased blocks */
		rc = mtd_read(master, *offs, sizeof(magic), &retlen,
			      (void *) &magic);
		if (rc || retlen != sizeof(magic)) {
			continue;
		}

		/* Skip blocks not starting with JFFS2 magic */
		if (magic != JFFS2_MAGIC_BITMASK)
			continue;

		/* Read full block */
		rc = mtd_read(master, *offs, master->erasesize, &retlen,
			      (void *) buf);
		if (rc)
			return rc;
		if (retlen != master->erasesize)
			return -EINVAL;

		while (block_offs < master->erasesize) {
			node = (struct jffs2_raw_dirent *) &buf[block_offs];

			if (!jffs2_dirent_valid(node)) {
				block_offs += 4;
				continue;
			}

			if (!memcmp(node->name, OPENWRT_NAME,
				    OPENWRT_NAME_LEN))
				valid = true;
			else if (!memcmp(node->name, name, name_len))
				return valid ? 0 : -EINVAL;

			block_offs += je32_to_cpu(node->totlen);
			block_offs = (block_offs + 0x3) & ~0x3;
		}
	}

	return -ENOENT;
}

static int ubifs_find(struct mtd_info *master, loff_t *offs)
{
	uint32_t magic;
	size_t retlen;
	int rc;

	for (; *offs < master->size; *offs += master->erasesize) {
		rc = mtd_read(master, *offs, sizeof(magic), &retlen,
			      (unsigned char *) &magic);
		if (rc || retlen != sizeof(magic))
			continue;

		if (be32_to_cpu(magic) == UBI_MAGIC)
			return 0;
	}

	return -ENOENT;
}

static int mtdsplit_parse_bcm_wfi(struct mtd_info *master,
				  const struct mtd_partition **pparts,
				  struct mtd_part_parser_data *data)
{
	struct mtd_partition *parts;
	struct device_node *mtd_node;
	loff_t cfe_off, kernel_off, rootfs_off;
	bool cfe_part = true;
	unsigned int num_parts = BCM_WFI_PARTS, cur_part = 0;
	uint8_t *buf;
	int ret;

	buf = kzalloc(master->erasesize, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	mtd_node = mtd_get_of_node(master);
	if (!mtd_node)
		return -EINVAL;

	if (of_device_is_compatible(mtd_node, "brcm,wfi-sercomm"))
		cfe_part = false;

	if (cfe_part) {
		num_parts++;
		cfe_off = 0;

		ret = jffs2_find_file(master, buf, CFERAM_NAME,
				      CFERAM_NAME_LEN, &cfe_off);
		if (ret) {
			kfree(buf);
			return ret;
		}

		kernel_off = cfe_off + master->erasesize;
	} else {
		kernel_off = 0;
	}

	ret = jffs2_find_file(master, buf, KERNEL_NAME, KERNEL_NAME_LEN,
			      &kernel_off);
	kfree(buf);
	if (ret)
		return ret;

	rootfs_off = kernel_off + master->erasesize;
	ret = ubifs_find(master, &rootfs_off);
	if (ret)
		return ret;

	parts = kzalloc(num_parts * sizeof(*parts), GFP_KERNEL);
	if (!parts)
		return -ENOMEM;

	if (cfe_part) {
		parts[cur_part].name = "cferam";
		parts[cur_part].mask_flags = MTD_WRITEABLE;
		parts[cur_part].offset = 0;
		parts[cur_part].size = kernel_off;
		cur_part++;
	}

	parts[cur_part].name = "firmware";
	parts[cur_part].offset = kernel_off;
	parts[cur_part].size = master->size - kernel_off;
	cur_part++;

	parts[cur_part].name = KERNEL_PART_NAME;
	parts[cur_part].offset = kernel_off;
	parts[cur_part].size = rootfs_off - kernel_off;
	cur_part++;

	parts[cur_part].name = UBI_PART_NAME;
	parts[cur_part].offset = rootfs_off;
	parts[cur_part].size = master->size - rootfs_off;
	cur_part++;

	*pparts = parts;

	return num_parts;
}

static const struct of_device_id mtdsplit_fit_of_match_table[] = {
	{ .compatible = "brcm,wfi" },
	{ .compatible = "brcm,wfi-sercomm" },
	{ },
};

static struct mtd_part_parser mtdsplit_bcm_wfi_parser = {
	.owner = THIS_MODULE,
	.name = "bcm-wfi-fw",
	.of_match_table = mtdsplit_fit_of_match_table,
	.parse_fn = mtdsplit_parse_bcm_wfi,
	.type = MTD_PARSER_TYPE_FIRMWARE,
};

static int __init mtdsplit_bcm_wfi_init(void)
{
	register_mtd_parser(&mtdsplit_bcm_wfi_parser);

	return 0;
}

module_init(mtdsplit_bcm_wfi_init);
