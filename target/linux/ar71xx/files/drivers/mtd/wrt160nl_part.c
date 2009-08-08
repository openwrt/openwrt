/*
 * Copyright © 2009 Christian Daniel <cd@maintech.de>
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

#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/bootmem.h>
#include <linux/magic.h>

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

static struct mtd_partition trx_parts[TRX_PARTS];

static int create_mtd_partitions(struct mtd_info *master,
				 struct mtd_partition **pparts,
				 unsigned long origin)
{
	uint8_t buf[512];
	int len;
	struct trx_header *header;
	struct uimage_header *uheader;
	unsigned int kernel_len;

	master->read(master, 4 * master->erasesize, sizeof(buf), &len, buf);
	if (strncmp(buf, "NL16", 4) == 0) {
		printk(KERN_INFO "TRX on WRT160NL detected\n");

		header = (struct trx_header *)(buf + 32);
		if (le32_to_cpu(header->magic) != TRX_MAGIC) {
			printk(KERN_WARNING "TRX messed up\n");
			return 0;
		}

		uheader = (struct uimage_header *)(buf + 60);
		if (uheader->ih_magic != IH_MAGIC) {
			printk(KERN_WARNING "uImage messed up\n");
			return 0;
		}

		kernel_len = uheader->ih_size / master->erasesize;
		if (uheader->ih_size % master->erasesize)
			kernel_len++;

		kernel_len++;
		kernel_len *= master->erasesize;

		trx_parts[0].name = "u-boot";
		trx_parts[0].offset = 0;
		trx_parts[0].size = 4 * master->erasesize;
		trx_parts[0].mask_flags = MTD_WRITEABLE;

		trx_parts[1].name = "kernel";
		trx_parts[1].offset = trx_parts[0].offset + trx_parts[0].size;
		trx_parts[1].size = kernel_len;
		trx_parts[1].mask_flags = 0;

		trx_parts[2].name = "rootfs";
		trx_parts[2].offset = trx_parts[1].offset + trx_parts[1].size;
		trx_parts[2].size = master->size - 6 * master->erasesize - trx_parts[1].size;
		trx_parts[2].mask_flags = 0;

		trx_parts[3].name = "nvram";
		trx_parts[3].offset = master->size - 2 * master->erasesize;
		trx_parts[3].size = master->erasesize;
		trx_parts[3].mask_flags = MTD_WRITEABLE;

		trx_parts[4].name = "art";
		trx_parts[4].offset = master->size - master->erasesize;
		trx_parts[4].size = master->erasesize;
		trx_parts[4].mask_flags = MTD_WRITEABLE;

		trx_parts[5].name = "firmware";
		trx_parts[5].offset = 4 * master->erasesize;
		trx_parts[5].size = master->size - 6 * master->erasesize;
		trx_parts[5].mask_flags = 0;

		*pparts = trx_parts;

		return TRX_PARTS;
	} else {
		return 0;
	}
}

static struct mtd_part_parser trx_parser = {
	.owner		= THIS_MODULE,
	.parse_fn	= create_mtd_partitions,
	.name		= "wrt160nl",
};

static int __init trx_parser_init(void)
{
	return register_mtd_parser(&trx_parser);
}

module_init(trx_parser_init);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Christian Daniel <cd@maintech.de>");
