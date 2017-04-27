/*
 * Copyright (C) 2011 Gabor Juhos <juhosg@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 *
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/magic.h>

#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>

#define TLT_NUM_PARTS	7
#define TLT_HEADER_V1	0x01000000
#define MD5SUM_LEN		16

#define TLT_UBOOT_LEN	0x20000
#define TLT_CONFIG_LEN	0x10000
#define TLT_ART_LEN		0x10000
#define EVENTLOG_LEN		0x90000

struct tlt_fw_header {
	uint32_t	version;	/* header version */
	char		vendor_name[24];
	char		fw_version[36];
	uint32_t	hw_id;		/* hardware id */
	uint32_t	hw_rev;		/* hardware revision */
	uint32_t	unk1;
	uint8_t		md5sum1[MD5SUM_LEN];
	uint32_t	unk2;
	uint8_t		md5sum2[MD5SUM_LEN];
	uint32_t	unk3;
	uint32_t	kernel_la;	/* kernel load address */
	uint32_t	kernel_ep;	/* kernel entry point */
	uint32_t	fw_length;	/* total length of the firmware */
	uint32_t	kernel_ofs;	/* kernel data offset */
	uint32_t	kernel_len;	/* kernel data length */
	uint32_t	rootfs_ofs;	/* rootfs data offset */
	uint32_t	rootfs_len;	/* rootfs data length */
	uint32_t	boot_ofs;	/* bootloader data offset */
	uint32_t	boot_len;	/* bootloader data length */
	uint8_t		pad[360];
} __attribute__ ((packed));

static struct tlt_fw_header *
tlt_read_header(struct mtd_info *mtd, size_t offset)
{
	struct tlt_fw_header *header;
	size_t header_len;
	size_t retlen;
	int ret;
	u32 t;

	header = vmalloc(sizeof(*header));
	if (!header)
		goto err;

	header_len = sizeof(struct tlt_fw_header);
	ret = mtd_read(mtd, offset, header_len, &retlen,
		       (unsigned char *) header);
	if (ret)
		goto err_free_header;

	if (retlen != header_len)
		goto err_free_header;

	/* sanity checks */
	t = be32_to_cpu(header->version);
	if (t != TLT_HEADER_V1)
		goto err_free_header;

	t = be32_to_cpu(header->kernel_ofs);
	if (t != header_len)
		goto err_free_header;

	return header;

err_free_header:
	vfree(header);
err:
	return NULL;
}

static int tlt_check_rootfs_magic(struct mtd_info *mtd, size_t offset)
{
	u32 magic;
	size_t retlen;
	int ret;

	ret = mtd_read(mtd, offset, sizeof(magic), &retlen,
		       (unsigned char *) &magic);
	if (ret)
		return ret;

	if (retlen != sizeof(magic))
		return -EIO;

	if (le32_to_cpu(magic) != SQUASHFS_MAGIC &&
	    magic != 0x19852003)
		return -EINVAL;

	return 0;
}

static int tlt_parse_partitions(struct mtd_info *master,
				   struct mtd_partition **pparts,
				   struct mtd_part_parser_data *data)
{
	struct mtd_partition *parts;
	struct tlt_fw_header *header;
	int nr_parts;
	size_t kernel_offset;
	size_t rootfs_offset;
	size_t squashfs_offset;
	int ret;

	nr_parts = TLT_NUM_PARTS;
	parts = kzalloc(nr_parts * sizeof(struct mtd_partition), GFP_KERNEL);
	if (!parts) {
		ret = -ENOMEM;
		goto err;
	}

	kernel_offset = TLT_UBOOT_LEN + TLT_CONFIG_LEN + TLT_ART_LEN;

	header = tlt_read_header(master, kernel_offset);
	if (!header) {
		pr_notice("%s: no TP-Link header found\n", master->name);
		ret = -ENODEV;
		goto err_free_parts;
	}

	squashfs_offset = kernel_offset + sizeof(struct tlt_fw_header) +
			  be32_to_cpu(header->kernel_len);

	ret = tlt_check_rootfs_magic(master, squashfs_offset);
	if (ret == 0)
		rootfs_offset = squashfs_offset;
	else
		rootfs_offset = kernel_offset + be32_to_cpu(header->rootfs_ofs);

	vfree(header);

	parts[0].name = "u-boot";
	parts[0].offset = 0;
	parts[0].size = TLT_UBOOT_LEN;
	parts[0].mask_flags = MTD_WRITEABLE;
	
	parts[1].name = "config";
	parts[1].offset = TLT_UBOOT_LEN;
	parts[1].size = TLT_CONFIG_LEN;
	
	parts[2].name = "art";
	parts[2].offset = TLT_UBOOT_LEN + TLT_CONFIG_LEN;
	parts[2].size = TLT_ART_LEN;
	parts[2].mask_flags = MTD_WRITEABLE;

	parts[3].name = "kernel";
	parts[3].offset = kernel_offset;
	parts[3].size = rootfs_offset - kernel_offset;

	parts[4].name = "rootfs";
	parts[4].offset = rootfs_offset;
	parts[4].size = master->size - rootfs_offset - EVENTLOG_LEN;

	parts[5].name = "firmware";
	parts[5].offset = kernel_offset;
	parts[5].size = master->size - kernel_offset - EVENTLOG_LEN;

	parts[6].name = "event-log";
	parts[6].offset = master->size - EVENTLOG_LEN;
	parts[6].size = EVENTLOG_LEN;

	*pparts = parts;
	return nr_parts;

err_free_parts:
	kfree(parts);
err:
	*pparts = NULL;
	return ret;
}

static struct mtd_part_parser tlt_parser = {
	.owner		= THIS_MODULE,
	.parse_fn	= tlt_parse_partitions,
	.name		= "tlt",
};

static int __init tlt_parser_init(void)
{
	register_mtd_parser(&tlt_parser);

	return 0;
}

module_init(tlt_parser_init);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Gabor Juhos <juhosg@openwrt.org>");
