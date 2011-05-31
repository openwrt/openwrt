/*
 * Copyright (C) 2009 Christian Daniel <cd@maintech.de>
 * Copyright (C) 2009 Gabor Juhos <juhosg@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * TRX flash partition table.
 * Based on ar7 map by Felix Fietkau <nbd@openwrt.org>
 *
 */

#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>

#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>

struct cybertan_header {
	char	magic[4];
	u8	res1[4];
	char	fw_date[3];
	char	fw_ver[3];
	char	id[4];
	char	hw_ver;
	char	unused;
	u8	flags[2];
	u8	res2[10];
};

#define TRX_PARTS	6
#define TRX_MAGIC	0x30524448
#define TRX_MAX_OFFSET	3

struct trx_header {
	uint32_t magic;           /* "HDR0" */
	uint32_t len;             /* Length of file including header */
	uint32_t crc32;           /* 32-bit CRC from flag_version to end of file */
	uint32_t flag_version;    /* 0:15 flags, 16:31 version */
	uint32_t offsets[TRX_MAX_OFFSET]; /* Offsets of partitions from start of header */
};

#define IH_MAGIC	0x27051956	/* Image Magic Number */
#define IH_NMLEN	32		/* Image Name Length */

struct uimage_header {
	uint32_t	ih_magic;	/* Image Header Magic Number */
	uint32_t	ih_hcrc;	/* Image Header CRC Checksum */
	uint32_t	ih_time;	/* Image Creation Timestamp */
	uint32_t	ih_size;	/* Image Data Size */
	uint32_t	ih_load;	/* Data» Load  Address */
	uint32_t	ih_ep;		/* Entry Point Address */
	uint32_t	ih_dcrc;	/* Image Data CRC Checksum */
	uint8_t		ih_os;		/* Operating System */
	uint8_t		ih_arch;	/* CPU architecture */
	uint8_t		ih_type;	/* Image Type */
	uint8_t		ih_comp;	/* Compression Type */
	uint8_t		ih_name[IH_NMLEN];	/* Image Name */
};

struct wrt160nl_header {
	struct cybertan_header	cybertan;
	struct trx_header	trx;
	struct uimage_header	uimage;
} __attribute__ ((packed));

static struct mtd_partition trx_parts[TRX_PARTS];

#define WRT160NL_UBOOT_LEN	0x40000
#define WRT160NL_ART_LEN	0x10000
#define WRT160NL_NVRAM_LEN	0x10000

static int wrt160nl_parse_partitions(struct mtd_info *master,
				     struct mtd_partition **pparts,
				     unsigned long origin)
{
	struct wrt160nl_header *header;
	struct trx_header *theader;
	struct uimage_header *uheader;
	size_t retlen;
	unsigned int kernel_len;
	unsigned int uboot_len = max(master->erasesize, WRT160NL_UBOOT_LEN);
	unsigned int nvram_len = max(master->erasesize, WRT160NL_NVRAM_LEN);
	unsigned int art_len = max(master->erasesize, WRT160NL_ART_LEN);
	int ret;

	header = vmalloc(sizeof(*header));
	if (!header) {
		return -ENOMEM;
		goto out;
	}

	ret = master->read(master, uboot_len, sizeof(*header),
			   &retlen, (void *) header);
	if (ret)
		goto free_hdr;

	if (retlen != sizeof(*header)) {
		ret = -EIO;
		goto free_hdr;
	}

	if (strncmp(header->cybertan.magic, "NL16", 4) != 0) {
		printk(KERN_NOTICE "%s: no WRT160NL signature found\n",
			master->name);
		goto free_hdr;
	}

	theader = &header->trx;
	if (le32_to_cpu(theader->magic) != TRX_MAGIC) {
		printk(KERN_NOTICE "%s: no TRX header found\n", master->name);
		goto free_hdr;
	}

	uheader = &header->uimage;
	if (uheader->ih_magic != IH_MAGIC) {
		printk(KERN_NOTICE "%s: no uImage found\n", master->name);
		goto free_hdr;
	}

	kernel_len = le32_to_cpu(theader->offsets[1]) +
		sizeof(struct cybertan_header);

	trx_parts[0].name = "u-boot";
	trx_parts[0].offset = 0;
	trx_parts[0].size = uboot_len;
	trx_parts[0].mask_flags = MTD_WRITEABLE;

	trx_parts[1].name = "kernel";
	trx_parts[1].offset = trx_parts[0].offset + trx_parts[0].size;
	trx_parts[1].size = kernel_len;
	trx_parts[1].mask_flags = 0;

	trx_parts[2].name = "rootfs";
	trx_parts[2].offset = trx_parts[1].offset + trx_parts[1].size;
	trx_parts[2].size = master->size - uboot_len - nvram_len - art_len -
		trx_parts[1].size;
	trx_parts[2].mask_flags = 0;

	trx_parts[3].name = "nvram";
	trx_parts[3].offset = master->size - nvram_len - art_len;
	trx_parts[3].size = nvram_len;
	trx_parts[3].mask_flags = MTD_WRITEABLE;

	trx_parts[4].name = "art";
	trx_parts[4].offset = master->size - art_len;
	trx_parts[4].size = art_len;
	trx_parts[4].mask_flags = MTD_WRITEABLE;

	trx_parts[5].name = "firmware";
	trx_parts[5].offset = uboot_len;
	trx_parts[5].size = master->size - uboot_len - nvram_len - art_len;
	trx_parts[5].mask_flags = 0;

	*pparts = trx_parts;
	ret = TRX_PARTS;

free_hdr:
	vfree(header);
out:
	return ret;
}

static struct mtd_part_parser wrt160nl_parser = {
	.owner		= THIS_MODULE,
	.parse_fn	= wrt160nl_parse_partitions,
	.name		= "wrt160nl",
};

static int __init wrt160nl_parser_init(void)
{
	return register_mtd_parser(&wrt160nl_parser);
}

module_init(wrt160nl_parser_init);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Christian Daniel <cd@maintech.de>");
