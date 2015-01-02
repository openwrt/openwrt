/*
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
#include <linux/vmalloc.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/byteorder/generic.h>

#include "mtdsplit.h"

#define IH_MAGIC	0x27051956	/* Image Magic Number		*/
#define IH_NMLEN		32	/* Image Name Length		*/

#define IH_OS_LINUX		5	/* Linux	*/

#define IH_TYPE_KERNEL		2	/* OS Kernel Image		*/
#define IH_TYPE_FILESYSTEM	7	/* Filesystem Image		*/

/*
 * Legacy format image header,
 * all data in network byte order (aka natural aka bigendian).
 */
struct uimage_header {
	uint32_t	ih_magic;	/* Image Header Magic Number	*/
	uint32_t	ih_hcrc;	/* Image Header CRC Checksum	*/
	uint32_t	ih_time;	/* Image Creation Timestamp	*/
	uint32_t	ih_size;	/* Image Data Size		*/
	uint32_t	ih_load;	/* Data	 Load  Address		*/
	uint32_t	ih_ep;		/* Entry Point Address		*/
	uint32_t	ih_dcrc;	/* Image Data CRC Checksum	*/
	uint8_t		ih_os;		/* Operating System		*/
	uint8_t		ih_arch;	/* CPU architecture		*/
	uint8_t		ih_type;	/* Image Type			*/
	uint8_t		ih_comp;	/* Compression Type		*/
	uint8_t		ih_name[IH_NMLEN];	/* Image Name		*/
};

static int
read_uimage_header(struct mtd_info *mtd, size_t offset,
		   struct uimage_header *header)
{
	size_t header_len;
	size_t retlen;
	int ret;

	header_len = sizeof(*header);
	ret = mtd_read(mtd, offset, header_len, &retlen,
		       (unsigned char *) header);
	if (ret) {
		pr_debug("read error in \"%s\"\n", mtd->name);
		return ret;
	}

	if (retlen != header_len) {
		pr_debug("short read in \"%s\"\n", mtd->name);
		return -EIO;
	}

	return 0;
}

static int __mtdsplit_parse_uimage(struct mtd_info *master,
				   struct mtd_partition **pparts,
				   struct mtd_part_parser_data *data,
				   bool (*verify)(struct uimage_header *hdr))
{
	struct mtd_partition *parts;
	struct uimage_header *header;
	int nr_parts;
	size_t offset;
	size_t uimage_offset;
	size_t uimage_size = 0;
	size_t rootfs_offset;
	size_t rootfs_size = 0;
	int uimage_part, rf_part;
	int ret;

	nr_parts = 2;
	parts = kzalloc(nr_parts * sizeof(*parts), GFP_KERNEL);
	if (!parts)
		return -ENOMEM;

	header = vmalloc(sizeof(*header));
	if (!header) {
		ret = -ENOMEM;
		goto err_free_parts;
	}

	/* find uImage on erase block boundaries */
	for (offset = 0; offset < master->size; offset += master->erasesize) {
		uimage_size = 0;

		ret = read_uimage_header(master, offset, header);
		if (ret)
			continue;

		if (!verify(header)) {
			pr_debug("no valid uImage found in \"%s\" at offset %llx\n",
				 master->name, (unsigned long long) offset);
			continue;
		}

		uimage_size = sizeof(*header) + be32_to_cpu(header->ih_size);
		if ((offset + uimage_size) > master->size) {
			pr_debug("uImage exceeds MTD device \"%s\"\n",
				 master->name);
			continue;
		}
		break;
	}

	if (uimage_size == 0) {
		pr_debug("no uImage found in \"%s\"\n", master->name);
		ret = -ENODEV;
		goto err_free_header;
	}

	uimage_offset = offset;

	if (uimage_offset == 0) {
		uimage_part = 0;
		rf_part = 1;

		/* find the roots after the uImage */
		ret = mtd_find_rootfs_from(master,
					   uimage_offset + uimage_size,
					   master->size,
					   &rootfs_offset);
		if (ret) {
			pr_debug("no rootfs after uImage in \"%s\"\n",
				 master->name);
			goto err_free_header;
		}

		rootfs_size = master->size - rootfs_offset;
		uimage_size = rootfs_offset - uimage_offset;
	} else {
		rf_part = 0;
		uimage_part = 1;

		/* check rootfs presence at offset 0 */
		ret = mtd_check_rootfs_magic(master, 0);
		if (ret) {
			pr_debug("no rootfs before uImage in \"%s\"\n",
				 master->name);
			goto err_free_header;
		}

		rootfs_offset = 0;
		rootfs_size = uimage_offset;
	}

	if (rootfs_size == 0) {
		pr_debug("no rootfs found in \"%s\"\n", master->name);
		ret = -ENODEV;
		goto err_free_header;
	}

	parts[uimage_part].name = KERNEL_PART_NAME;
	parts[uimage_part].offset = uimage_offset;
	parts[uimage_part].size = uimage_size;

	parts[rf_part].name = ROOTFS_PART_NAME;
	parts[rf_part].offset = rootfs_offset;
	parts[rf_part].size = rootfs_size;

	vfree(header);

	*pparts = parts;
	return nr_parts;

err_free_header:
	vfree(header);

err_free_parts:
	kfree(parts);
	return ret;
}

static bool uimage_verify_default(struct uimage_header *header)
{
	/* default sanity checks */
	if (be32_to_cpu(header->ih_magic) != IH_MAGIC) {
		pr_debug("invalid uImage magic: %08x\n",
			 be32_to_cpu(header->ih_magic));
		return false;
	}

	if (header->ih_os != IH_OS_LINUX) {
		pr_debug("invalid uImage OS: %08x\n",
			 be32_to_cpu(header->ih_os));
		return false;
	}

	if (header->ih_type != IH_TYPE_KERNEL) {
		pr_debug("invalid uImage type: %08x\n",
			 be32_to_cpu(header->ih_type));
		return false;
	}

	return true;
}

static int
mtdsplit_uimage_parse_generic(struct mtd_info *master,
			      struct mtd_partition **pparts,
			      struct mtd_part_parser_data *data)
{
	return __mtdsplit_parse_uimage(master, pparts, data,
				      uimage_verify_default);
}

static struct mtd_part_parser uimage_generic_parser = {
	.owner = THIS_MODULE,
	.name = "uimage-fw",
	.parse_fn = mtdsplit_uimage_parse_generic,
	.type = MTD_PARSER_TYPE_FIRMWARE,
};

#define FW_MAGIC_WNR2000V3	0x32303033
#define FW_MAGIC_WNR2000V4	0x32303034
#define FW_MAGIC_WNR2200	0x32323030
#define FW_MAGIC_WNR612V2	0x32303631
#define FW_MAGIC_WNDR3700	0x33373030
#define FW_MAGIC_WNDR3700V2	0x33373031

static bool uimage_verify_wndr3700(struct uimage_header *header)
{
	uint8_t expected_type = IH_TYPE_FILESYSTEM;
	switch be32_to_cpu(header->ih_magic) {
	case FW_MAGIC_WNR612V2:
	case FW_MAGIC_WNR2000V3:
	case FW_MAGIC_WNR2200:
	case FW_MAGIC_WNDR3700:
	case FW_MAGIC_WNDR3700V2:
		break;
	case FW_MAGIC_WNR2000V4:
		expected_type = IH_TYPE_KERNEL;
		break;
	default:
		return false;
	}

	if (header->ih_os != IH_OS_LINUX ||
	    header->ih_type != expected_type)
		return false;

	return true;
}

static int
mtdsplit_uimage_parse_netgear(struct mtd_info *master,
			      struct mtd_partition **pparts,
			      struct mtd_part_parser_data *data)
{
	return __mtdsplit_parse_uimage(master, pparts, data,
				      uimage_verify_wndr3700);
}

static struct mtd_part_parser uimage_netgear_parser = {
	.owner = THIS_MODULE,
	.name = "netgear-fw",
	.parse_fn = mtdsplit_uimage_parse_netgear,
	.type = MTD_PARSER_TYPE_FIRMWARE,
};

static int __init mtdsplit_uimage_init(void)
{
	register_mtd_parser(&uimage_generic_parser);
	register_mtd_parser(&uimage_netgear_parser);

	return 0;
}

module_init(mtdsplit_uimage_init);
