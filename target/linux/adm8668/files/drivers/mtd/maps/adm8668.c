/*
 * Infineon/ADMTek ADM8668 (WildPass) partition parser support
 *
 * Copyright (C) 2010 Scott Nicholas <neutronscott@scottn.us>
 * Copyright (C) 2012 Florian Fainelli <florian@openwrt.org>
 *
 * original functions for finding root filesystem from Mike Baker
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 * THIS  SOFTWARE  IS PROVIDED   ``AS  IS'' AND   ANY  EXPRESS OR IMPLIED
 * WARRANTIES,   INCLUDING, BUT NOT  LIMITED  TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
 * NO  EVENT  SHALL   THE AUTHOR  BE    LIABLE FOR ANY   DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED   TO, PROCUREMENT OF  SUBSTITUTE GOODS  OR SERVICES; LOSS OF
 * USE, DATA,  OR PROFITS; OR  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN  CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * You should have received a copy of the  GNU General Public License along
 * with this program; if not, write  to the Free Software Foundation, Inc.,
 * 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 * Copyright 2004, Broadcom Corporation
 * All Rights Reserved.
 *
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/vmalloc.h>
#include <linux/slab.h>
#include <linux/crc32.h>
#include <linux/magic.h>

#define PFX	"adm8668-part: "

/* first a little bit about the headers i need.. */

/* just interested in part of the full struct */
struct squashfs_super_block {
        __le32  s_magic;
        __le32  pad0[9];        /* it's not really padding */
        __le64  bytes_used;
};

#define IH_MAGIC	0x56190527	/* Image Magic Number		*/
struct uboot_header {
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
	char		ih_name[32];	/* image name */
};

/* in case i wanna change stuff later, and to clarify the math section... */
#define	PART_LINUX	0
#define	PART_ROOTFS	1
#define PART_UBOOT_ENV	2
#define	NR_PARTS	3

static int adm8668_parse_partitions(struct mtd_info *master,
				    struct mtd_partition **pparts,
				    struct mtd_part_parser_data *data)
{
	int ret;
	struct uboot_header uhdr;
	int off, blocksize;
	size_t len, linux_len;
	struct squashfs_super_block shdr;
	struct erase_info erase_info;
	struct mtd_partition *adm8668_parts;

	memset(&erase_info, 0, sizeof(erase_info));

	blocksize = master->erasesize;

	if (blocksize < 0x10000)
		blocksize = 0x10000;

	adm8668_parts = kzalloc(sizeof(*adm8668_parts) * NR_PARTS, GFP_KERNEL);
	if (!adm8668_parts)
		return -ENOMEM;

	adm8668_parts[PART_LINUX].name = kstrdup("linux", GFP_KERNEL);
	adm8668_parts[PART_LINUX].offset = 0x40000;
	adm8668_parts[PART_LINUX].size = master->size - 0x40000;
	adm8668_parts[PART_ROOTFS].name = kstrdup("rootfs", GFP_KERNEL);
	adm8668_parts[PART_ROOTFS].offset = 0xe0000;
	adm8668_parts[PART_ROOTFS].size = 0x140000;
	adm8668_parts[PART_UBOOT_ENV].name = kstrdup("uboot_env", GFP_KERNEL);
	adm8668_parts[PART_UBOOT_ENV].offset = 0x20000;
	adm8668_parts[PART_UBOOT_ENV].size = 0x20000;

	/* now find squashfs */
	memset(&shdr, 0xe5, sizeof(shdr));

	for (off = 0x40000; off < master->size; off += blocksize) {
		/*
		 * Read into buffer
		 */
		if (mtd_read(master, off, sizeof(shdr), &len, (char *)&shdr) ||
		    len != sizeof(shdr))
			continue;

		if (shdr.s_magic == SQUASHFS_MAGIC) {
			uint32_t fs_size = (uint32_t)shdr.bytes_used;

			pr_info(PFX "filesystem type: squashfs, size=%dkB\n",
				fs_size >> 10);

			/*
			 * Update rootfs based on the superblock info, and
			 * stretch to end of MTD. rootfs_split will split it
			 */
			adm8668_parts[PART_ROOTFS].offset = off;
			adm8668_parts[PART_ROOTFS].size = master->size -
				adm8668_parts[PART_ROOTFS].offset;

			/*
			 * kernel ends where rootfs starts
			 * but we'll keep it full-length for upgrades
			 */
			linux_len = adm8668_parts[PART_LINUX + 1].offset -
				adm8668_parts[PART_LINUX].offset;

			adm8668_parts[PART_LINUX].size = master->size -
				adm8668_parts[PART_LINUX].offset;
			goto found;
		}
	}

	pr_err(PFX "could't find root filesystem\n");
	return NR_PARTS;

found:
	if (mtd_read(master, adm8668_parts[PART_LINUX].offset, sizeof(uhdr), &len, (char *)&uhdr) ||
	    len != sizeof(uhdr)) {
		pr_err(PFX "failed to read u-boot header\n");
		return NR_PARTS;
	}

	if (uhdr.ih_magic != IH_MAGIC) {
		pr_info(PFX "invalid u-boot magic detected?!?!\n");
		return NR_PARTS;
	}

	if (be32_to_cpu(uhdr.ih_size) != (linux_len - sizeof(uhdr))) {
		u32 data;
		size_t data_len = 0;
		unsigned char *block;
		unsigned int offset;

		offset = adm8668_parts[PART_LINUX].offset +
			 sizeof(struct uboot_header);

		pr_info(PFX "Updating U-boot image:\n");
		pr_info(PFX "  old: [size: %8d crc32: 0x%08x]\n",
			be32_to_cpu(uhdr.ih_size), be32_to_cpu(uhdr.ih_dcrc));

		if (mtd_read(master, offset, sizeof(data), &data_len, (char *)&data)) {
			pr_err(PFX "failed to read data\n");
			goto out;
		}

		/* Update the data length & crc32 */
		uhdr.ih_size = cpu_to_be32(linux_len - sizeof(uhdr));
		uhdr.ih_dcrc = crc32_le(~0, (char *)&data, linux_len - sizeof(uhdr)) ^ (~0);
		uhdr.ih_dcrc = cpu_to_be32(uhdr.ih_dcrc);

		pr_info(PFX "  new: [size: %8d crc32: 0x%08x]\n",
			be32_to_cpu(uhdr.ih_size), be32_to_cpu(uhdr.ih_dcrc));

		/* update header's crc... */
		uhdr.ih_hcrc = 0;
		uhdr.ih_hcrc = crc32_le(~0, (unsigned char *)&uhdr,
				sizeof(uhdr)) ^ (~0);
		uhdr.ih_hcrc = cpu_to_be32(uhdr.ih_hcrc);

		/* read first eraseblock from the image */
		block = vmalloc(master->erasesize);
		if (!block)
			return -ENOMEM;

		if (mtd_read(master, adm8668_parts[PART_LINUX].offset, master->erasesize, &len, block)
				|| len != master->erasesize) {
			pr_err(PFX "error copying first eraseblock\n");
			return 0;
		}

		/* Write updated header to the flash */
		memcpy(block, &uhdr, sizeof(uhdr));
		if (master->unlock)
			master->unlock(master, off, master->erasesize);

		erase_info.mtd = master;
		erase_info.addr = (uint64_t)adm8668_parts[PART_LINUX].offset;
		erase_info.len = master->erasesize;
		ret = mtd_erase(master, &erase_info);
		if (!ret) {
			if (mtd_write(master, adm8668_parts[PART_LINUX].offset, master->erasesize,
					&len, block))
				pr_err(PFX "write failed");
		} else
			pr_err(PFX "erase failed");

		mtd_sync(master);
out:
		if (block)
			vfree(block);
		pr_info(PFX "done\n");
	}

	*pparts = adm8668_parts;

	return NR_PARTS;
}

static struct mtd_part_parser adm8668_parser = {
	.owner	= THIS_MODULE,
	.parse_fn = adm8668_parse_partitions,
	.name = "adm8668part",
};

static int __init adm8668_parser_init(void)
{
	return register_mtd_parser(&adm8668_parser);
}

static void __exit adm8668_parser_exit(void)
{
	deregister_mtd_parser(&adm8668_parser);
}

module_init(adm8668_parser_init);
module_exit(adm8668_parser_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Scott Nicholas <neutronscott@scottn.us>");
MODULE_AUTHOR("Florian Fainelli <florian@openwrt.org>");
MODULE_DESCRIPTION("MTD partition parser for ADM8668");
