/*
 *  Copyright (C) 2006 Felix Fietkau <nbd@openwrt.org>
 *  Copyright (C) 2005 Waldemar Brodkorb <wbx@openwrt.org>
 *  Copyright (C) 2004 Florian Schirmer (jolt@tuxbox.org)
 *
 *  original functions for finding root filesystem from Mike Baker 
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 *
 *  THIS  SOFTWARE  IS PROVIDED   ``AS  IS'' AND   ANY  EXPRESS OR IMPLIED
 *  WARRANTIES,   INCLUDING, BUT NOT  LIMITED  TO, THE IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
 *  NO  EVENT  SHALL   THE AUTHOR  BE    LIABLE FOR ANY   DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 *  NOT LIMITED   TO, PROCUREMENT OF  SUBSTITUTE GOODS  OR SERVICES; LOSS OF
 *  USE, DATA,  OR PROFITS; OR  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 *  ANY THEORY OF LIABILITY, WHETHER IN  CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  You should have received a copy of the  GNU General Public License along
 *  with this program; if not, write  to the Free Software Foundation, Inc.,
 *  675 Mass Ave, Cambridge, MA 02139, USA.
 * 
 *
 * Copyright 2004, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * Flash mapping for BCM947XX boards
 *
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/wait.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/map.h>
#ifdef CONFIG_MTD_PARTITIONS
#include <linux/mtd/partitions.h>
#endif
#include <linux/config.h>
#include <linux/squashfs_fs.h>
#include <linux/jffs2.h>
#include <linux/crc32.h>
#include <asm/io.h>

#include <typedefs.h>
#include <osl.h>
#include <bcmnvram.h>
#include <sbconfig.h>
#include <sbchipc.h>
#include <sbutils.h>
#include <trxhdr.h>

/* Global SB handle */
extern void *bcm947xx_sbh;
extern spinlock_t bcm947xx_sbh_lock;

/* Convenience */
#define sbh bcm947xx_sbh
#define sbh_lock bcm947xx_sbh_lock

#define WINDOW_ADDR 0x1fc00000
#define WINDOW_SIZE 0x400000
#define BUSWIDTH 2

static struct mtd_info *bcm947xx_mtd;

__u8 bcm947xx_map_read8(struct map_info *map, unsigned long ofs)
{
	if (map->map_priv_2 == 1)
		return __raw_readb(map->map_priv_1 + ofs);

	u16 val = __raw_readw(map->map_priv_1 + (ofs & ~1));
	if (ofs & 1)
		return ((val >> 8) & 0xff);
	else
		return (val & 0xff);
}

__u16 bcm947xx_map_read16(struct map_info *map, unsigned long ofs)
{
	return __raw_readw(map->map_priv_1 + ofs);
}

__u32 bcm947xx_map_read32(struct map_info *map, unsigned long ofs)
{
	return __raw_readl(map->map_priv_1 + ofs);
}

void bcm947xx_map_copy_from(struct map_info *map, void *to, unsigned long from, ssize_t len)
{
	if (len==1) {
		memcpy_fromio(to, map->map_priv_1 + from, len);
	} else {
		int i;
		u16 *dest = (u16 *) to;
		u16 *src  = (u16 *) (map->map_priv_1 + from);
		for (i = 0; i < (len / 2); i++) {
			dest[i] = src[i];
		}
		if (len & 1)
			*((u8 *)dest+len-1) = src[i] & 0xff;
	}
}

void bcm947xx_map_write8(struct map_info *map, __u8 d, unsigned long adr)
{
	__raw_writeb(d, map->map_priv_1 + adr);
	mb();
}

void bcm947xx_map_write16(struct map_info *map, __u16 d, unsigned long adr)
{
	__raw_writew(d, map->map_priv_1 + adr);
	mb();
}

void bcm947xx_map_write32(struct map_info *map, __u32 d, unsigned long adr)
{
	__raw_writel(d, map->map_priv_1 + adr);
	mb();
}

void bcm947xx_map_copy_to(struct map_info *map, unsigned long to, const void *from, ssize_t len)
{
	memcpy_toio(map->map_priv_1 + to, from, len);
}

struct map_info bcm947xx_map = {
	name: "Physically mapped flash",
	size: WINDOW_SIZE,
	buswidth: BUSWIDTH,
	read8: bcm947xx_map_read8,
	read16: bcm947xx_map_read16,
	read32: bcm947xx_map_read32,
	copy_from: bcm947xx_map_copy_from,
	write8: bcm947xx_map_write8,
	write16: bcm947xx_map_write16,
	write32: bcm947xx_map_write32,
	copy_to: bcm947xx_map_copy_to
};

#ifdef CONFIG_MTD_PARTITIONS

static struct mtd_partition bcm947xx_parts[] = {
	{ name: "cfe",	offset: 0, size: 0, mask_flags: MTD_WRITEABLE, },
	{ name: "linux", offset: 0, size: 0, },
	{ name: "rootfs", offset: 0, size: 0, },
	{ name: "nvram", offset: 0, size: 0, },
	{ name: "rootfs_data", offset: 0, size: 0, },
	{ name: NULL, },
};

static int __init
find_cfe_size(struct mtd_info *mtd, size_t size)
{
	struct trx_header *trx;
	unsigned char buf[512];
	int off;
	size_t len;
	int blocksize;

	trx = (struct trx_header *) buf;

	blocksize = mtd->erasesize;
	if (blocksize < 0x10000)
		blocksize = 0x10000;

	for (off = (128*1024); off < size; off += blocksize) {
		memset(buf, 0xe5, sizeof(buf));

		/*
		 * Read into buffer 
		 */
		if (MTD_READ(mtd, off, sizeof(buf), &len, buf) ||
		    len != sizeof(buf))
			continue;

		/* found a TRX header */
		if (le32_to_cpu(trx->magic) == TRX_MAGIC) {
			goto found;
		}
	}

	printk(KERN_NOTICE
	       "%s: Couldn't find bootloader size\n",
	       mtd->name);
	return -1;

 found:
	printk(KERN_NOTICE "bootloader size: %d\n", off);
	return off;

}

/*
 * Copied from mtdblock.c
 *
 * Cache stuff...
 * 
 * Since typical flash erasable sectors are much larger than what Linux's
 * buffer cache can handle, we must implement read-modify-write on flash
 * sectors for each block write requests.  To avoid over-erasing flash sectors
 * and to speed things up, we locally cache a whole flash sector while it is
 * being written to until a different sector is required.
 */

static void erase_callback(struct erase_info *done)
{
	wait_queue_head_t *wait_q = (wait_queue_head_t *)done->priv;
	wake_up(wait_q);
}

static int erase_write (struct mtd_info *mtd, unsigned long pos, 
			int len, const char *buf)
{
	struct erase_info erase;
	DECLARE_WAITQUEUE(wait, current);
	wait_queue_head_t wait_q;
	size_t retlen;
	int ret;

	/*
	 * First, let's erase the flash block.
	 */

	init_waitqueue_head(&wait_q);
	erase.mtd = mtd;
	erase.callback = erase_callback;
	erase.addr = pos;
	erase.len = len;
	erase.priv = (u_long)&wait_q;

	set_current_state(TASK_INTERRUPTIBLE);
	add_wait_queue(&wait_q, &wait);

	ret = MTD_ERASE(mtd, &erase);
	if (ret) {
		set_current_state(TASK_RUNNING);
		remove_wait_queue(&wait_q, &wait);
		printk (KERN_WARNING "erase of region [0x%lx, 0x%x] "
				     "on \"%s\" failed\n",
			pos, len, mtd->name);
		return ret;
	}

	schedule();  /* Wait for erase to finish. */
	remove_wait_queue(&wait_q, &wait);

	/*
	 * Next, writhe data to flash.
	 */

	ret = MTD_WRITE (mtd, pos, len, &retlen, buf);
	if (ret)
		return ret;
	if (retlen != len)
		return -EIO;
	return 0;
}




static int __init
find_root(struct mtd_info *mtd, size_t size, struct mtd_partition *part)
{
	struct trx_header trx, *trx2;
	unsigned char buf[512], *block;
	int off, blocksize;
	u32 i, crc = ~0;
	size_t len;
	struct squashfs_super_block *sb = (struct squashfs_super_block *) buf;

	blocksize = mtd->erasesize;
	if (blocksize < 0x10000)
		blocksize = 0x10000;

	for (off = (128*1024); off < size; off += blocksize) {
		memset(&trx, 0xe5, sizeof(trx));

		/*
		 * Read into buffer 
		 */
		if (MTD_READ(mtd, off, sizeof(trx), &len, (char *) &trx) ||
		    len != sizeof(trx))
			continue;

		/* found a TRX header */
		if (le32_to_cpu(trx.magic) == TRX_MAGIC) {
			part->offset = le32_to_cpu(trx.offsets[2]) ? : 
				le32_to_cpu(trx.offsets[1]);
			part->size = le32_to_cpu(trx.len); 

			part->size -= part->offset;
			part->offset += off;

			goto found;
		}
	}

	printk(KERN_NOTICE
	       "%s: Couldn't find root filesystem\n",
	       mtd->name);
	return -1;

 found:
	if (part->size == 0)
		return 0;
	
	if (MTD_READ(mtd, part->offset, sizeof(buf), &len, buf) || len != sizeof(buf))
		return 0;

	if (*((__u32 *) buf) == SQUASHFS_MAGIC) {
		printk(KERN_INFO "%s: Filesystem type: squashfs, size=0x%x\n", mtd->name, (u32) sb->bytes_used);

		/* Update the squashfs partition size based on the superblock info */
		part->size = sb->bytes_used;
		len = part->offset + part->size;
		len +=  (mtd->erasesize - 1);
		len &= ~(mtd->erasesize - 1);
		part->size = len - part->offset;
	} else if (*((__u16 *) buf) == JFFS2_MAGIC_BITMASK) {
		printk(KERN_INFO "%s: Filesystem type: jffs2\n", mtd->name);

		/* Move the squashfs outside of the trx */
		part->size = 0;
	} else {
		printk(KERN_INFO "%s: Filesystem type: unknown\n", mtd->name);
		return 0;
	}

	if (trx.len != part->offset + part->size - off) {
		/* Update the trx offsets and length */
		trx.len = part->offset + part->size - off;
	
		/* Update the trx crc32 */
		for (i = (u32) &(((struct trx_header *)NULL)->flag_version); i <= trx.len; i += sizeof(buf)) {
			if (MTD_READ(mtd, off + i, sizeof(buf), &len, buf) || len != sizeof(buf))
				return 0;
			crc = crc32_le(crc, buf, min(sizeof(buf), trx.len - i));
		}
		trx.crc32 = crc;

		/* read first eraseblock from the trx */
		trx2 = block = kmalloc(mtd->erasesize, GFP_KERNEL);
		if (MTD_READ(mtd, off, mtd->erasesize, &len, block) || len != mtd->erasesize) {
			printk("Error accessing the first trx eraseblock\n");
			return 0;
		}
		
		printk("Updating TRX offsets and length:\n");
		printk("old trx = [0x%08x, 0x%08x, 0x%08x], len=0x%08x crc32=0x%08x\n", trx2->offsets[0], trx2->offsets[1], trx2->offsets[2], trx2->len, trx2->crc32);
		printk("new trx = [0x%08x, 0x%08x, 0x%08x], len=0x%08x crc32=0x%08x\n",   trx.offsets[0],   trx.offsets[1],   trx.offsets[2],   trx.len, trx.crc32);

		/* Write updated trx header to the flash */
		memcpy(block, &trx, sizeof(trx));
		if (mtd->unlock)
			mtd->unlock(mtd, off, mtd->erasesize);
		erase_write(mtd, off, mtd->erasesize, block);
		if (mtd->sync)
			mtd->sync(mtd);
		kfree(block);
		printk("Done\n");
	}
	
	return part->size;
}

struct mtd_partition * __init
init_mtd_partitions(struct mtd_info *mtd, size_t size)
{
	int cfe_size;

	if ((cfe_size = find_cfe_size(mtd,size)) < 0)
		return NULL;

	/* boot loader */
	bcm947xx_parts[0].offset = 0;
	bcm947xx_parts[0].size   = cfe_size;

	/* nvram */
	if (cfe_size != 384 * 1024) {
		bcm947xx_parts[3].offset = size - ROUNDUP(NVRAM_SPACE, mtd->erasesize);
		bcm947xx_parts[3].size   = ROUNDUP(NVRAM_SPACE, mtd->erasesize);
	} else {
		/* nvram (old 128kb config partition on netgear wgt634u) */
		bcm947xx_parts[3].offset = bcm947xx_parts[0].size;
		bcm947xx_parts[3].size   = ROUNDUP(NVRAM_SPACE, mtd->erasesize);
	}

	/* linux (kernel and rootfs) */
	if (cfe_size != 384 * 1024) {
		bcm947xx_parts[1].offset = bcm947xx_parts[0].size;
		bcm947xx_parts[1].size   = bcm947xx_parts[3].offset - 
			bcm947xx_parts[1].offset;
	} else {
		/* do not count the elf loader, which is on one block */
		bcm947xx_parts[1].offset = bcm947xx_parts[0].size + 
			bcm947xx_parts[3].size + mtd->erasesize;
		bcm947xx_parts[1].size   = size - 
			bcm947xx_parts[0].size - 
			(2*bcm947xx_parts[3].size) - 
			mtd->erasesize;
	}

	/* find and size rootfs */
	if (find_root(mtd,size,&bcm947xx_parts[2])==0) {
		/* entirely jffs2 */
		bcm947xx_parts[4].name = NULL;
		bcm947xx_parts[2].size = size - bcm947xx_parts[2].offset - 
				bcm947xx_parts[3].size;
	} else {
		/* legacy setup */
		/* calculate leftover flash, and assign it to the jffs2 partition */
		if (cfe_size != 384 * 1024) {
			bcm947xx_parts[4].offset = bcm947xx_parts[2].offset + 
				bcm947xx_parts[2].size;
			if ((bcm947xx_parts[4].offset % mtd->erasesize) > 0) {
				bcm947xx_parts[4].offset += mtd->erasesize - 
					(bcm947xx_parts[4].offset % mtd->erasesize);
			}
			bcm947xx_parts[4].size = bcm947xx_parts[3].offset - 
				bcm947xx_parts[4].offset;
		} else {
			bcm947xx_parts[4].offset = bcm947xx_parts[2].offset + 
				bcm947xx_parts[2].size;
			if ((bcm947xx_parts[4].offset % mtd->erasesize) > 0) {
				bcm947xx_parts[4].offset += mtd->erasesize - 
					(bcm947xx_parts[4].offset % mtd->erasesize);
			}
			bcm947xx_parts[4].size = size - bcm947xx_parts[3].size - 
				bcm947xx_parts[4].offset;
		}
	}

	return bcm947xx_parts;
}

#endif


mod_init_t init_bcm947xx_map(void)
{
	ulong flags;
 	uint coreidx;
	chipcregs_t *cc;
	uint32 fltype;
	uint window_addr = 0, window_size = 0;
	size_t size;
	int ret = 0;
#ifdef CONFIG_MTD_PARTITIONS
	struct mtd_partition *parts;
	int i;
#endif

	spin_lock_irqsave(&sbh_lock, flags);
	coreidx = sb_coreidx(sbh);

	/* Check strapping option if chipcommon exists */
	if ((cc = sb_setcore(sbh, SB_CC, 0))) {
		fltype = readl(&cc->capabilities) & CC_CAP_FLASH_MASK;
		if (fltype == PFLASH) {
			bcm947xx_map.map_priv_2 = 1;
			window_addr = 0x1c000000;
			bcm947xx_map.size = window_size = 32 * 1024 * 1024;
			if ((readl(&cc->flash_config) & CC_CFG_DS) == 0)
				bcm947xx_map.buswidth = 1;
		}
	} else {
		fltype = PFLASH;
		bcm947xx_map.map_priv_2 = 0;
		window_addr = WINDOW_ADDR;
		window_size = WINDOW_SIZE;
	}

	sb_setcoreidx(sbh, coreidx);
	spin_unlock_irqrestore(&sbh_lock, flags);

	if (fltype != PFLASH) {
		printk(KERN_ERR "pflash: found no supported devices\n");
		ret = -ENODEV;
		goto fail;
	}

	bcm947xx_map.map_priv_1 = (unsigned long) ioremap(window_addr, window_size);

	if (!bcm947xx_map.map_priv_1) {
		printk(KERN_ERR "Failed to ioremap\n");
		return -EIO;
	}

	if (!(bcm947xx_mtd = do_map_probe("cfi_probe", &bcm947xx_map))) {
		printk(KERN_ERR "pflash: cfi_probe failed\n");
		iounmap((void *)bcm947xx_map.map_priv_1);
		return -ENXIO;
	}

	bcm947xx_mtd->module = THIS_MODULE;

	size = bcm947xx_mtd->size;

	printk(KERN_NOTICE "Flash device: 0x%x at 0x%x\n", size, window_addr);

#ifdef CONFIG_MTD_PARTITIONS
	parts = init_mtd_partitions(bcm947xx_mtd, size);
	for (i = 0; parts[i].name; i++);
	ret = add_mtd_partitions(bcm947xx_mtd, parts, i);
	if (ret) {
		printk(KERN_ERR "Flash: add_mtd_partitions failed\n");
		goto fail;
	}
#endif

	return 0;

 fail:
	if (bcm947xx_mtd)
		map_destroy(bcm947xx_mtd);
	if (bcm947xx_map.map_priv_1)
		iounmap((void *) bcm947xx_map.map_priv_1);
	bcm947xx_map.map_priv_1 = 0;
	return ret;
}

mod_exit_t cleanup_bcm947xx_map(void)
{
#ifdef CONFIG_MTD_PARTITIONS
	del_mtd_partitions(bcm947xx_mtd);
#endif
	map_destroy(bcm947xx_mtd);
	iounmap((void *) bcm947xx_map.map_priv_1);
	bcm947xx_map.map_priv_1 = 0;
}

module_init(init_bcm947xx_map);
module_exit(cleanup_bcm947xx_map);
