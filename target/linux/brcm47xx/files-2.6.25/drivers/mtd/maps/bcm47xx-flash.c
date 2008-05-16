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
 *  Copyright 2001-2003, Broadcom Corporation
 *  All Rights Reserved.
 * 
 *  THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 *  KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 *  SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 *  FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 *  Flash mapping for BCM947XX boards
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/map.h>
#ifdef CONFIG_MTD_PARTITIONS
#include <linux/mtd/partitions.h>
#endif
#include <linux/crc32.h>
#ifdef CONFIG_SSB
#include <linux/ssb/ssb.h>
#endif
#include <asm/io.h>


#define TRX_MAGIC	0x30524448	/* "HDR0" */
#define TRX_VERSION	1
#define TRX_MAX_LEN	0x3A0000
#define TRX_NO_HEADER	1		/* Do not write TRX header */	
#define TRX_GZ_FILES	0x2     /* Contains up to TRX_MAX_OFFSET individual gzip files */
#define TRX_MAX_OFFSET	3

struct trx_header {
	u32 magic;		/* "HDR0" */
	u32 len;		/* Length of file including header */
	u32 crc32;		/* 32-bit CRC from flag_version to end of file */
	u32 flag_version;	/* 0:15 flags, 16:31 version */
	u32 offsets[TRX_MAX_OFFSET];	/* Offsets of partitions from start of header */
};

#define ROUNDUP(x, y) ((((x)+((y)-1))/(y))*(y))
#define NVRAM_SPACE 0x8000
#define WINDOW_ADDR 0x1fc00000
#define WINDOW_SIZE 0x400000
#define BUSWIDTH 2

#ifdef CONFIG_SSB
extern struct ssb_bus ssb_bcm47xx;
#endif
static struct mtd_info *bcm47xx_mtd;

static void bcm47xx_map_copy_from(struct map_info *map, void *to, unsigned long from, ssize_t len)
{
	if (len==1) {
		memcpy_fromio(to, map->virt + from, len);
	} else {
		int i;
		u16 *dest = (u16 *) to;
		u16 *src  = (u16 *) (map->virt + from);
		for (i = 0; i < (len / 2); i++) {
			dest[i] = src[i];
		}
		if (len & 1)
			*((u8 *)dest+len-1) = src[i] & 0xff;
	}
}

static struct map_info bcm47xx_map = {
	name: "Physically mapped flash",
	size: WINDOW_SIZE,
	bankwidth: BUSWIDTH,
	phys: WINDOW_ADDR,
};

#ifdef CONFIG_MTD_PARTITIONS

static struct mtd_partition bcm47xx_parts[] = {
	{ name: "cfe",	offset: 0, size: 0, mask_flags: MTD_WRITEABLE, },
	{ name: "linux", offset: 0, size: 0, },
	{ name: "rootfs", offset: 0, size: 0, },
	{ name: "nvram", offset: 0, size: 0, },
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
		if (mtd->read(mtd, off, sizeof(buf), &len, buf) ||
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

	ret = mtd->erase(mtd, &erase);
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

	ret = mtd->write (mtd, pos, len, &retlen, buf);
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
		if (mtd->read(mtd, off, sizeof(trx), &len, (char *) &trx) ||
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
	
	if (mtd->read(mtd, part->offset, sizeof(buf), &len, buf) || len != sizeof(buf))
		return 0;

	/* Move the fs outside of the trx */
	part->size = 0;

	if (trx.len != part->offset + part->size - off) {
		/* Update the trx offsets and length */
		trx.len = part->offset + part->size - off;
	
		/* Update the trx crc32 */
		for (i = (u32) &(((struct trx_header *)NULL)->flag_version); i <= trx.len; i += sizeof(buf)) {
			if (mtd->read(mtd, off + i, sizeof(buf), &len, buf) || len != sizeof(buf))
				return 0;
			crc = crc32_le(crc, buf, min(sizeof(buf), trx.len - i));
		}
		trx.crc32 = crc;

		/* read first eraseblock from the trx */
		block = kmalloc(mtd->erasesize, GFP_KERNEL);
		trx2 = (struct trx_header *) block;
		if (mtd->read(mtd, off, mtd->erasesize, &len, block) || len != mtd->erasesize) {
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
	bcm47xx_parts[0].offset = 0;
	bcm47xx_parts[0].size   = cfe_size;

	/* nvram */
	if (cfe_size != 384 * 1024) {
		bcm47xx_parts[3].offset = size - ROUNDUP(NVRAM_SPACE, mtd->erasesize);
		bcm47xx_parts[3].size   = ROUNDUP(NVRAM_SPACE, mtd->erasesize);
	} else {
		/* nvram (old 128kb config partition on netgear wgt634u) */
		bcm47xx_parts[3].offset = bcm47xx_parts[0].size;
		bcm47xx_parts[3].size   = ROUNDUP(NVRAM_SPACE, mtd->erasesize);
	}

	/* linux (kernel and rootfs) */
	if (cfe_size != 384 * 1024) {
		bcm47xx_parts[1].offset = bcm47xx_parts[0].size;
		bcm47xx_parts[1].size   = bcm47xx_parts[3].offset - 
			bcm47xx_parts[1].offset;
	} else {
		/* do not count the elf loader, which is on one block */
		bcm47xx_parts[1].offset = bcm47xx_parts[0].size + 
			bcm47xx_parts[3].size + mtd->erasesize;
		bcm47xx_parts[1].size   = size - 
			bcm47xx_parts[0].size - 
			(2*bcm47xx_parts[3].size) - 
			mtd->erasesize;
	}

	/* find and size rootfs */
	find_root(mtd,size,&bcm47xx_parts[2]);
	bcm47xx_parts[2].size = size - bcm47xx_parts[2].offset - bcm47xx_parts[3].size;

	return bcm47xx_parts;
}
#endif

int __init init_bcm47xx_map(void)
{
#ifdef CONFIG_SSB
	struct ssb_mipscore *mcore = &ssb_bcm47xx.mipscore;
#endif
	size_t size;
	int ret = 0;
#ifdef CONFIG_MTD_PARTITIONS
	struct mtd_partition *parts;
	int i;
#endif

#ifdef CONFIG_SSB
	u32 window = mcore->flash_window;
	u32 window_size = mcore->flash_window_size;

	printk("flash init: 0x%08x 0x%08x\n", window, window_size);
	bcm47xx_map.phys = window;
	bcm47xx_map.size = window_size;
	bcm47xx_map.virt = ioremap_nocache(window, window_size);
#else
	printk("flash init: 0x%08x 0x%08x\n", WINDOW_ADDR, WINDOW_SIZE);
	bcm47xx_map.virt = ioremap_nocache(WINDOW_ADDR, WINDOW_SIZE);
#endif

	if (!bcm47xx_map.virt) {
		printk("Failed to ioremap\n");
		return -EIO;
	}

	simple_map_init(&bcm47xx_map);
	
	if (!(bcm47xx_mtd = do_map_probe("cfi_probe", &bcm47xx_map))) {
		printk("Failed to do_map_probe\n");
		iounmap((void *)bcm47xx_map.virt);
		return -ENXIO;
	}

	/* override copy_from routine */
 	bcm47xx_map.copy_from = bcm47xx_map_copy_from;

	bcm47xx_mtd->owner = THIS_MODULE;

	size = bcm47xx_mtd->size;

	printk(KERN_NOTICE "Flash device: 0x%x at 0x%x\n", size, WINDOW_ADDR);

#ifdef CONFIG_MTD_PARTITIONS
	parts = init_mtd_partitions(bcm47xx_mtd, size);
	for (i = 0; parts[i].name; i++);
	ret = add_mtd_partitions(bcm47xx_mtd, parts, i);
	if (ret) {
		printk(KERN_ERR "Flash: add_mtd_partitions failed\n");
		goto fail;
	}
#endif
	return 0;

 fail:
	if (bcm47xx_mtd)
		map_destroy(bcm47xx_mtd);
	if (bcm47xx_map.virt)
		iounmap((void *)bcm47xx_map.virt);
	bcm47xx_map.virt = 0;
	return ret;
}

void __exit cleanup_bcm47xx_map(void)
{
#ifdef CONFIG_MTD_PARTITIONS
	del_mtd_partitions(bcm47xx_mtd);
#endif
	map_destroy(bcm47xx_mtd);
	iounmap((void *)bcm47xx_map.virt);
}

module_init(init_bcm47xx_map);
module_exit(cleanup_bcm47xx_map);
