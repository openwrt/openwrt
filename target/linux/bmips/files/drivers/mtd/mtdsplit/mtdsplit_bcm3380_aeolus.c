// SPDX-License-Identifier: GPL-2.0-only
/*
 * MTD splitter for the BCM3380 Aeolus dual-image ProgramStore format.
 *
 * The stock bootloader expects:
 *   outer ProgramStore header, control bit 0x0100 set
 *     inner ProgramStore header
 *       LZMA-compressed kernel payload
 *     eraseblock padding
 *     raw SquashFS payload
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/byteorder/generic.h>
#include <linux/module.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/overflow.h>
#include <linux/slab.h>

#include "mtdsplit.h"

#define AEOLUS_NR_PARTS				2

#define AEOLUS_SIGNATURE			0xa0e3
#define AEOLUS_CONTROL_DUAL_IMAGE		0x0100
#define AEOLUS_CONTROL_LZMA			0x0004

struct aeolus_program_header {
	__be16 signature;
	__be16 control;
	__be16 major_revision;
	__be16 minor_revision;
	__be32 calendar_time;
	__be32 total_compressed_length;
	__be32 load_address;
	char filename[48];
	u8 reserved0[8];
	__be32 compressed_length1;
	__be32 compressed_length2;
	__be16 hcs;
	__be16 reserved1;
	__be32 crc;
} __packed;

static int aeolus_read_header(struct mtd_info *master, size_t offset,
			      struct aeolus_program_header *header)
{
	size_t retlen;
	int ret = mtd_read(master, offset, sizeof(*header), &retlen,
			   (void *)header);

	if (ret)
		return ret;

	if (retlen != sizeof(*header))
		return -EIO;

	if (be16_to_cpu(header->signature) != AEOLUS_SIGNATURE)
		return -EINVAL;

	return 0;
}

static bool aeolus_add_overflows(size_t a, size_t b, size_t *result)
{
	if (check_add_overflow(a, b, result))
		return true;

	return false;
}

static size_t aeolus_next_eb(struct mtd_info *mtd, size_t offset)
{
	return mtd_rounddown_to_eb(offset, mtd) + mtd->erasesize;
}

static int aeolus_find_squashfs(struct mtd_info *master, size_t search_start,
				size_t search_limit, size_t *rootfs_offset)
{
	for (size_t offset = search_start; offset < search_limit;
	     offset = aeolus_next_eb(master, offset)) {
		enum mtdsplit_part_type type;
		int ret = mtd_check_rootfs_magic(master, offset, &type);
		if (!ret && type == MTDSPLIT_PART_TYPE_SQUASHFS) {
			*rootfs_offset = offset;
			return 0;
		}
	}

	return -ENOENT;
}

static int aeolus_make_partitions(struct mtd_info *master,
				  const struct mtd_partition **pparts,
				  size_t kernel_size, size_t rootfs_offset)
{
	struct mtd_partition *parts = kcalloc(AEOLUS_NR_PARTS, sizeof(*parts), GFP_KERNEL);
	if (!parts)
		return -ENOMEM;

	parts[0].name = KERNEL_PART_NAME;
	parts[0].offset = 0;
	parts[0].size = kernel_size;

	parts[1].name = ROOTFS_PART_NAME;
	parts[1].offset = rootfs_offset;
	parts[1].size = master->size - rootfs_offset;

	*pparts = parts;
	return AEOLUS_NR_PARTS;
}

static int mtdsplit_parse_bcm3380_aeolus(struct mtd_info *master,
					 const struct mtd_partition **pparts,
					 struct mtd_part_parser_data *data)
{
	struct aeolus_program_header outer;
	int ret = aeolus_read_header(master, 0, &outer);

	if (ret)
		return ret;

	u16 outer_control = be16_to_cpu(outer.control);

	if (!(outer_control & AEOLUS_CONTROL_DUAL_IMAGE))
		return -EINVAL;

	size_t outer_payload_len = be32_to_cpu(outer.total_compressed_length);
	u32 outer_image1_len = be32_to_cpu(outer.compressed_length1);
	u32 outer_image2_len = be32_to_cpu(outer.compressed_length2);

	if (!outer_payload_len || !outer_image1_len || !outer_image2_len)
		return -EINVAL;

	size_t outer_payload_end;

	if (aeolus_add_overflows(sizeof(outer), outer_payload_len,
				 &outer_payload_end))
		return -EINVAL;

	size_t search_limit = outer_payload_end;

	if (outer_payload_end > master->size) {
		pr_warn("%s: outer image length exceeds partition: payload_len=0x%zx end=0x%zx partition_size=0x%llx\n",
			master->name, outer_payload_len, outer_payload_end,
			(unsigned long long)master->size);
		search_limit = master->size;
	}

	struct aeolus_program_header inner;

	ret = aeolus_read_header(master, sizeof(outer), &inner);
	if (ret) {
		pr_info("%s: dual outer header found, but inner ProgramStore header is missing or invalid\n",
			master->name);
		return ret;
	}

	u16 inner_control = be16_to_cpu(inner.control);
	size_t inner_image_len = sizeof(inner) +
				 be32_to_cpu(inner.total_compressed_length);
	size_t kernel_end;

	if (aeolus_add_overflows(sizeof(outer), inner_image_len, &kernel_end))
		return -EINVAL;

	pr_info("%s: BCM3380 Aeolus dual image: outer_ctrl=0x%04x outer_payload_len=0x%zx image1_len=0x%08x image2_len=0x%08x outer_end=0x%zx\n",
		master->name, outer_control, outer_payload_len, outer_image1_len,
		outer_image2_len, outer_payload_end);
	pr_info("%s: inner image: ctrl=0x%04x load=0x%08x payload_len=0x%08x image_len=0x%zx kernel_end=0x%zx\n",
		master->name, inner_control, be32_to_cpu(inner.load_address),
		be32_to_cpu(inner.total_compressed_length), inner_image_len,
		kernel_end);

	if (!(inner_control & AEOLUS_CONTROL_LZMA))
		pr_warn("%s: inner image is not marked as LZMA compressed: ctrl=0x%04x\n",
			master->name, inner_control);

	if (outer_image1_len != inner_image_len)
		pr_warn("%s: outer image1 length does not match inner header: outer=0x%08x inner=0x%zx\n",
			master->name, outer_image1_len, inner_image_len);

	if (kernel_end >= search_limit) {
		pr_warn("%s: no room for rootfs search: kernel_end=0x%zx search_limit=0x%zx\n",
			master->name, kernel_end, search_limit);
		return -EINVAL;
	}

	pr_info("%s: searching for raw SquashFS from padding start 0x%zx to 0x%zx, eraseblock=0x%x\n",
		master->name, kernel_end, search_limit, master->erasesize);

	size_t rootfs_offset;

	ret = aeolus_find_squashfs(master, kernel_end, search_limit,
				   &rootfs_offset);
	if (ret) {
		pr_warn("%s: no raw SquashFS found after inner kernel image\n",
			master->name);
		return ret;
	}

	pr_info("%s: raw SquashFS found at 0x%zx, kernel partition size 0x%zx, rootfs partition size 0x%llx\n",
		master->name, rootfs_offset, rootfs_offset,
		(unsigned long long)(master->size - rootfs_offset));

	return aeolus_make_partitions(master, pparts, rootfs_offset,
				      rootfs_offset);
}

static const struct of_device_id mtdsplit_bcm3380_aeolus_of_match_table[] = {
	{ .compatible = "brcm,bcm3380-aeolus-firmware" },
	{ .compatible = "brcm,bcm3380-aeolus-fw" },
	{},
};
MODULE_DEVICE_TABLE(of, mtdsplit_bcm3380_aeolus_of_match_table);

static struct mtd_part_parser mtdsplit_bcm3380_aeolus_parser = {
	.owner = THIS_MODULE,
	.name = "bcm3380-aeolus-fw",
	.of_match_table = mtdsplit_bcm3380_aeolus_of_match_table,
	.parse_fn = mtdsplit_parse_bcm3380_aeolus,
	.type = MTD_PARSER_TYPE_FIRMWARE,
};

module_mtd_part_parser(mtdsplit_bcm3380_aeolus_parser);
