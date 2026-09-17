// SPDX-License-Identifier: GPL-2.0-only
/*
 * Bintec BOSS firmware partition parser
 *
 * Copyright (C) 2017 Keyyo
 * Author: Sebastien Decourriere <sebtx452@gmail.com>
 * Copyright (C) 2026 Simon Wunderlich <sw@simonwunderlich.de>
 *
 * The bootmonitor of the Bintec elmeg RS series boots a gzip-compressed
 * image that sits behind a 52-byte BOSS header. OpenWrt puts only the
 * kernel behind that header, so image_length marks the end of the kernel
 * and the rootfs follows at the next erase block.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/byteorder/generic.h>

#include "mtdsplit.h"

#define BINTEC_NR_PARTS			2
#define BOSS_MAGIC_CLOSEDEYEVISUAL	"TELDAT ClosedEyeVisual"
#define BOSS_MAGIC_CHAOSENDDRAGON	"BINTEC ChaosEndDragon"

struct boss_header {
	char magic[23];
	__be32 fw_version;
	u8 image_type;
	__be32 image_version;
	__be32 image_length;
	__be32 unknown1;
	__be32 unknown2;
	__be32 crc32;
	__be32 unknown3;
} __packed;

static int mtdsplit_parse_bintec(struct mtd_info *master,
				 const struct mtd_partition **pparts,
				 struct mtd_part_parser_data *data)
{
	struct boss_header hdr;
	size_t retlen;
	size_t kernel_size, rootfs_offset;
	u32 image_length;
	struct mtd_partition *parts;
	int err;

	err = mtd_read(master, 0, sizeof(hdr), &retlen, (void *)&hdr);
	if (err)
		return err;

	if (retlen != sizeof(hdr))
		return -EIO;

	if (strncmp(hdr.magic, BOSS_MAGIC_CLOSEDEYEVISUAL, sizeof(hdr.magic)) &&
	    strncmp(hdr.magic, BOSS_MAGIC_CHAOSENDDRAGON, sizeof(hdr.magic)))
		return -EINVAL;

	image_length = be32_to_cpu(hdr.image_length);
	if (image_length > master->size - sizeof(hdr))
		return -EINVAL;

	kernel_size = mtd_roundup_to_eb(sizeof(hdr) + image_length, master);
	if (kernel_size >= master->size)
		return -EINVAL;

	err = mtd_find_rootfs_from(master, kernel_size, master->size,
				   &rootfs_offset, NULL);
	if (err)
		return err;

	parts = kcalloc(BINTEC_NR_PARTS, sizeof(*parts), GFP_KERNEL);
	if (!parts)
		return -ENOMEM;

	parts[0].name = KERNEL_PART_NAME;
	parts[0].offset = 0;
	parts[0].size = rootfs_offset;

	parts[1].name = ROOTFS_PART_NAME;
	parts[1].offset = rootfs_offset;
	parts[1].size = master->size - rootfs_offset;

	*pparts = parts;
	return BINTEC_NR_PARTS;
}

static const struct of_device_id mtdsplit_bintec_of_match_table[] = {
	{ .compatible = "bintec,boss-firmware" },
	{},
};

static struct mtd_part_parser mtdsplit_bintec_parser = {
	.owner = THIS_MODULE,
	.name = "bintec-fw",
	.of_match_table = mtdsplit_bintec_of_match_table,
	.parse_fn = mtdsplit_parse_bintec,
	.type = MTD_PARSER_TYPE_FIRMWARE,
};

module_mtd_part_parser(mtdsplit_bintec_parser);
