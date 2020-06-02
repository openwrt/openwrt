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

#define char_to_num(c)		((c >= '0' && c <= '9') ? (c - '0') : (0))

#define BCM_WFI_PARTS		3

#define CFERAM_NAME		"cferam"
#define CFERAM_NAME_LEN		(sizeof(CFERAM_NAME) - 1)
#define KERNEL_NAME		"vmlinux.lz"
#define KERNEL_NAME_LEN		(sizeof(KERNEL_NAME) - 1)
#define OPENWRT_NAME		"1-openwrt"
#define OPENWRT_NAME_LEN	(sizeof(OPENWRT_NAME) - 1)

#define UBI_MAGIC		0x55424923

#define SERCOMM_MAGIC_PFX	"eRcOmM."
#define SERCOMM_MAGIC_PFX_LEN	(sizeof(SERCOMM_MAGIC_PFX) - 1)
#define SERCOMM_MAGIC		"eRcOmM.000"
#define SERCOMM_MAGIC_LEN	(sizeof(SERCOMM_MAGIC) - 1)

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

static int jffs2_find_file(struct mtd_info *mtd, uint8_t *buf,
			   const char *name, size_t name_len,
			   loff_t *offs, loff_t size)
{
	const loff_t end = *offs + size;
	struct jffs2_raw_dirent *node;
	bool valid = false;
	size_t retlen;
	uint16_t magic;
	int rc;

	for (; *offs < end; *offs += mtd->erasesize) {
		unsigned int block_offs = 0;

		/* Skip CFE erased blocks */
		rc = mtd_read(mtd, *offs, sizeof(magic), &retlen,
			      (void *) &magic);
		if (rc || retlen != sizeof(magic)) {
			continue;
		}

		/* Skip blocks not starting with JFFS2 magic */
		if (magic != JFFS2_MAGIC_BITMASK)
			continue;

		/* Read full block */
		rc = mtd_read(mtd, *offs, mtd->erasesize, &retlen,
			      (void *) buf);
		if (rc)
			return rc;
		if (retlen != mtd->erasesize)
			return -EINVAL;

		while (block_offs < mtd->erasesize) {
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

static int ubifs_find(struct mtd_info *mtd, loff_t *offs, loff_t size)
{
	const loff_t end = *offs + size;
	uint32_t magic;
	size_t retlen;
	int rc;

	for (; *offs < end; *offs += mtd->erasesize) {
		rc = mtd_read(mtd, *offs, sizeof(magic), &retlen,
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

	if (of_property_read_bool(mtd_node, "brcm,no-cferam"))
		cfe_part = false;

	if (cfe_part) {
		num_parts++;
		cfe_off = 0;

		ret = jffs2_find_file(master, buf, CFERAM_NAME,
				      CFERAM_NAME_LEN, &cfe_off,
				      master->size);
		if (ret) {
			kfree(buf);
			return ret;
		}

		kernel_off = cfe_off + master->erasesize;
	} else {
		kernel_off = 0;
	}

	ret = jffs2_find_file(master, buf, KERNEL_NAME, KERNEL_NAME_LEN,
			      &kernel_off, master->size);
	kfree(buf);
	if (ret)
		return ret;

	rootfs_off = kernel_off + master->erasesize;
	ret = ubifs_find(master, &rootfs_off, master->size);
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

static const struct of_device_id mtdsplit_bcm_wfi_of_match[] = {
	{ .compatible = "brcm,wfi" },
	{ },
};

static struct mtd_part_parser mtdsplit_bcm_wfi_parser = {
	.owner = THIS_MODULE,
	.name = "bcm-wfi-fw",
	.of_match_table = mtdsplit_bcm_wfi_of_match,
	.parse_fn = mtdsplit_parse_bcm_wfi,
	.type = MTD_PARSER_TYPE_FIRMWARE,
};

static int sercomm_bootflag_value(struct mtd_info *mtd, uint8_t *buf)
{
	size_t retlen;
	loff_t offs;
	int rc;

	for (offs = 0; offs < mtd->size; offs += mtd->erasesize) {
		rc = mtd_read(mtd, offs, SERCOMM_MAGIC_LEN, &retlen, buf);
		if (rc || retlen != SERCOMM_MAGIC_LEN)
			continue;

		if (memcmp(buf, SERCOMM_MAGIC_PFX, SERCOMM_MAGIC_PFX_LEN))
			continue;

		rc = char_to_num(buf[SERCOMM_MAGIC_PFX_LEN + 0]) * 100;
		rc += char_to_num(buf[SERCOMM_MAGIC_PFX_LEN + 1]) * 10;
		rc += char_to_num(buf[SERCOMM_MAGIC_PFX_LEN + 2]) * 1;

		return rc;
	}

	return -ENOENT;
}

static int mtdsplit_parse_ser_wfi(struct mtd_info *master,
				  const struct mtd_partition **pparts,
				  struct mtd_part_parser_data *data)
{
	struct mtd_info *mtd_bf1, *mtd_bf2;
	struct erase_info bf_erase;
	struct mtd_partition *parts;
	loff_t kernel_off, rootfs_off, img_size;
	loff_t img2_off, img2_size = 0;
	unsigned int num_parts = BCM_WFI_PARTS, cur_part = 0;
	uint8_t *buf;
	int bf1, bf2;
	int ret;

	mtd_bf1 = get_mtd_device_nm("bootflag1");
	if (IS_ERR(mtd_bf1))
		return -ENOENT;

	mtd_bf2 = get_mtd_device_nm("bootflag2");
	if (IS_ERR(mtd_bf2))
		return -ENOENT;

	buf = kzalloc(master->erasesize, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	bf1 = sercomm_bootflag_value(mtd_bf1, buf);
	if (bf1 >= 0)
		printk("sercomm: bootflag1=%d\n", bf1);

	bf2 = sercomm_bootflag_value(mtd_bf2, buf);
	if (bf2 >= 0)
		printk("sercomm: bootflag2=%d\n", bf2);

	if (bf1 == bf2 && bf2 >= 0) {
		bf2 = -ENOENT;
		bf_erase.addr = 0;
		bf_erase.len = mtd_bf2->size;
		mtd_erase(mtd_bf2, &bf_erase);
	}

	if (bf1 >= bf2) {
		kernel_off = 0;
		if (bf2 >= 0) {
			img_size = master->size / 2;

			img2_off = img_size;
			img2_size = master->size - img2_off;
			num_parts++;
		} else {
			img_size = master->size;
		}
	} else {
		kernel_off = master->size / 2;
		img_size = master->size;

		img2_off = 0;
		img2_size = kernel_off;
		num_parts++;
	}

	ret = jffs2_find_file(master, buf, KERNEL_NAME, KERNEL_NAME_LEN,
			      &kernel_off, img_size);
	kfree(buf);
	if (ret)
		return ret;

	rootfs_off = kernel_off + master->erasesize;
	ret = ubifs_find(master, &rootfs_off, img_size);
	if (ret)
		return ret;

	parts = kzalloc(num_parts * sizeof(*parts), GFP_KERNEL);
	if (!parts)
		return -ENOMEM;

	parts[cur_part].name = "firmware";
	parts[cur_part].offset = kernel_off;
	parts[cur_part].size = img_size - kernel_off;
	cur_part++;

	parts[cur_part].name = KERNEL_PART_NAME;
	parts[cur_part].offset = kernel_off;
	parts[cur_part].size = rootfs_off - kernel_off;
	cur_part++;

	parts[cur_part].name = UBI_PART_NAME;
	parts[cur_part].offset = rootfs_off;
	parts[cur_part].size = img_size - rootfs_off;
	cur_part++;

	if (img2_size) {
		parts[cur_part].name = "img2";
		parts[cur_part].offset = img2_off;
		parts[cur_part].size = img2_size;
		cur_part++;
	}

	*pparts = parts;

	return num_parts;
}

static const struct of_device_id mtdsplit_ser_wfi_of_match[] = {
	{ .compatible = "sercomm,wfi" },
	{ },
};

static struct mtd_part_parser mtdsplit_ser_wfi_parser = {
	.owner = THIS_MODULE,
	.name = "ser-wfi-fw",
	.of_match_table = mtdsplit_ser_wfi_of_match,
	.parse_fn = mtdsplit_parse_ser_wfi,
	.type = MTD_PARSER_TYPE_FIRMWARE,
};

static int __init mtdsplit_bcm_wfi_init(void)
{
	register_mtd_parser(&mtdsplit_bcm_wfi_parser);
	register_mtd_parser(&mtdsplit_ser_wfi_parser);

	return 0;
}

module_init(mtdsplit_bcm_wfi_init);
