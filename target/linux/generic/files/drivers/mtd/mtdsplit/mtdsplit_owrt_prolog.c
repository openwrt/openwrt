/*
 * Copyright (C) 2026 Linus Walleij <linusw@kernel.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 *
 * The idea about the "executable prolog" is simple: this is an assembly
 * prolog that OpenWrt adds in front of the kernel to move it around in
 * memory. Since it is 512 bytes, we can also store the kernel file size
 * in the prolog and use it for splitting the partition.
 *
 * This format has been designed to be easy to create an executable prolog
 * with a parseable size from bash scripts:
 *
 * cat executable_prolog > image.bin
 * echo "OPENWRT-PROLOG-512" >> image.bin
 * stat -c %s zImage >> image.image
 * dd if=image.bin of=image.new bs=512
 * mv image.new image.bin
 *
 * will create a 512 bytes executable prolog with the needed tag somewhere
 * in the header area. (The executable_prolog needs to be small.)
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/of.h>

#include "mtdsplit.h"

#define OWRT_PROLOG_SIZE		512
#define OWRT_PROLOG_MAX_OVERHEAD	32
#define OWRT_PROLOG_MAGIC		"OPENWRT-PROLOG-512"
#define OWRT_PROLOG_NR_PARTS		2

static int mtdsplit_parse_owrt_prolog(struct mtd_info *master,
				      const struct mtd_partition **pparts,
				      struct mtd_part_parser_data *data)
{
	struct mtd_partition *parts;
	char buf[OWRT_PROLOG_SIZE];
	size_t retlen;
	unsigned long kernel_size, rootfs_offset;
	int ret;
	int i;

	ret = mtd_read(master, 0, sizeof(buf), &retlen, buf);
	if (ret)
		return ret;

	if (retlen != sizeof(buf))
		return -EIO;

	for (i = 0; i < (OWRT_PROLOG_SIZE - OWRT_PROLOG_MAX_OVERHEAD); i++) {
		if (!strncmp(OWRT_PROLOG_MAGIC, buf + i, strlen(OWRT_PROLOG_MAGIC)))
			break;
	}
	if (i == (OWRT_PROLOG_SIZE - OWRT_PROLOG_MAX_OVERHEAD)) {
		pr_err("%s: no OpenWrt prolog found\n", master->name);
		return -EINVAL;
	}

	pr_debug("%s: OpenWrt prolog found at offset %d\n", master->name, i);

	i += strlen(OWRT_PROLOG_MAGIC);
	i++; /* Skip linefeed after the magic */

	ret = kstrtol(buf + i, 10, &kernel_size);
	if (ret)
		return ret;

	/*
	 * From the MTD point of view, the prolog is part of
	 * the kernel.
	 */
	kernel_size += OWRT_PROLOG_SIZE;
	pr_debug("%s: OpenWrt prolog kernel size %08lx\n",
		 master->name, kernel_size);

	/* rootfs starts at the next 0x20000 (128k) boundary: */
	rootfs_offset = round_up(kernel_size, 0x20000);

	pr_debug("%s: OpenWrt prolog rootfs offset %08lx\n",
		 master->name, rootfs_offset);

	if (rootfs_offset >= master->size)
		return -EINVAL;

	ret = mtd_check_rootfs_magic(master, rootfs_offset, NULL);
	if (ret)
		return ret;

	parts = kzalloc(OWRT_PROLOG_NR_PARTS * sizeof(*parts), GFP_KERNEL);
	if (!parts)
		return -ENOMEM;

	parts[0].name = KERNEL_PART_NAME;
	parts[0].offset = 0;
	parts[0].size = kernel_size;

	parts[1].name = ROOTFS_PART_NAME;
	parts[1].offset = rootfs_offset;
	parts[1].size = master->size - rootfs_offset;

	*pparts = parts;
	return OWRT_PROLOG_NR_PARTS;
}

static const struct of_device_id mtdsplit_owrt_prolog_of_match_table[] = {
	{ .compatible = "openwrt,executable-prolog" },
	{},
};

static struct mtd_part_parser mtdsplit_owrt_prolog_parser = {
	.owner = THIS_MODULE,
	.name = "executable-prolog",
	.of_match_table = mtdsplit_owrt_prolog_of_match_table,
	.parse_fn = mtdsplit_parse_owrt_prolog,
	.type = MTD_PARSER_TYPE_FIRMWARE,
};

static int __init mtdsplit_owrt_prolog_init(void)
{
	register_mtd_parser(&mtdsplit_owrt_prolog_parser);

	return 0;
}

subsys_initcall(mtdsplit_owrt_prolog_init);
