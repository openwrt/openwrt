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
#include <asm/mach-bcm47xx/nvram.h>
#include <asm/fw/cfe/cfe_api.h>


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

/* for Edimax Print servers which use an additional header
 * then the firmware on flash looks like :
 * EDIMAX HEADER | TRX HEADER
 * As this header is 12 bytes long we have to handle it
 * and skip it to find the TRX header
 */
#define EDIMAX_PS_HEADER_MAGIC	0x36315350 /*  "PS16"  */
#define EDIMAX_PS_HEADER_LEN	0xc /* 12 bytes long for edimax header */

#define ROUNDUP(x, y) ((((x)+((y)-1))/(y))*(y))
#define NVRAM_SPACE 0x8000
#define WINDOW_ADDR 0x1fc00000
#define WINDOW_SIZE 0x400000
#define BUSWIDTH 2

#define ROUTER_NETGEAR_WGR614L         1
#define ROUTER_NETGEAR_WNR834B         2
#define ROUTER_NETGEAR_WNDR3300        3
#define ROUTER_NETGEAR_WNR3500L        4
#define ROUTER_SIMPLETECH_SIMPLESHARE  5

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
	{ name: NULL, }, /* Used to create custom partitons with the function get_router() */
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

		if (le32_to_cpu(trx->magic) == EDIMAX_PS_HEADER_MAGIC) {
			if (mtd->read(mtd, off + EDIMAX_PS_HEADER_LEN,
			    sizeof(buf), &len, buf) || len != sizeof(buf)) {
				continue;
			} else {
				printk(KERN_NOTICE"Found edimax header\n");
			}
		}

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
	 * Next, write data to flash.
	 */

	ret = mtd->write (mtd, pos, len, &retlen, buf);
	if (ret)
		return ret;
	if (retlen != len)
		return -EIO;
	return 0;
}


static int __init
find_dual_image_off (struct mtd_info *mtd, size_t size)
{
	struct trx_header trx;
	int off, blocksize;
	size_t len;

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
		/* found last TRX header */
		if (le32_to_cpu(trx.magic) == TRX_MAGIC){ 
			if (le32_to_cpu(trx.flag_version >> 16)==2){
				printk("dual image TRX header found\n");
				return size/2;
			} else {
				return 0;
			}
		}
	}
	return 0;
}


static int __init
find_root(struct mtd_info *mtd, size_t size, struct mtd_partition *part)
{
	struct trx_header trx, *trx2;
	unsigned char buf[512], *block;
	int off, blocksize, trxoff = 0;
	u32 i, crc = ~0;
	size_t len;
	bool edimax = false;

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

		/* found an edimax header */
		if (le32_to_cpu(trx.magic) == EDIMAX_PS_HEADER_MAGIC) {
			/* read the correct trx header */
			if (mtd->read(mtd, off + EDIMAX_PS_HEADER_LEN,
			    sizeof(trx), &len, (char *) &trx) ||
			    len != sizeof(trx)) {
				continue;
			} else {
				printk(KERN_NOTICE"Found an edimax ps header\n");
				edimax = true;
			}
		}

		/* found a TRX header */
		if (le32_to_cpu(trx.magic) == TRX_MAGIC) {
			part->offset = le32_to_cpu(trx.offsets[2]) ? : 
				le32_to_cpu(trx.offsets[1]);
			part->size = le32_to_cpu(trx.len); 

			part->size -= part->offset;
			part->offset += off;
			if (edimax) {
				off += EDIMAX_PS_HEADER_LEN;
				trxoff = EDIMAX_PS_HEADER_LEN;
			}

			goto found;
		}
	}

	printk(KERN_NOTICE
	       "%s: Couldn't find root filesystem\n",
	       mtd->name);
	return -1;

 found:
	printk(KERN_NOTICE"TRX offset : %x\n", trxoff);
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
		if (mtd->read(mtd, off - trxoff, mtd->erasesize, &len, block) || len != mtd->erasesize) {
			printk("Error accessing the first trx eraseblock\n");
			return 0;
		}
		
		printk("Updating TRX offsets and length:\n");
		printk("old trx = [0x%08x, 0x%08x, 0x%08x], len=0x%08x crc32=0x%08x\n", trx2->offsets[0], trx2->offsets[1], trx2->offsets[2], trx2->len, trx2->crc32);
		printk("new trx = [0x%08x, 0x%08x, 0x%08x], len=0x%08x crc32=0x%08x\n",   trx.offsets[0],   trx.offsets[1],   trx.offsets[2],   trx.len, trx.crc32);

		/* Write updated trx header to the flash */
		memcpy(block + trxoff, &trx, sizeof(trx));
		if (mtd->unlock)
			mtd->unlock(mtd, off - trxoff, mtd->erasesize);
		erase_write(mtd, off - trxoff, mtd->erasesize, block);
		if (mtd->sync)
			mtd->sync(mtd);
		kfree(block);
		printk("Done\n");
	}
	
	return part->size;
}

static int get_router(void)
{
	char buf[20];
	u32 boardnum = 0;
	u16 boardtype = 0;
	u16 boardrev = 0;
	u32 boardflags = 0;
	u16 sdram_init = 0;
	u16 cardbus = 0;
	u16 strev = 0;

	if (nvram_getenv("boardnum", buf, sizeof(buf)) >= 0)
		boardnum = simple_strtoul(buf, NULL, 0);
	if (nvram_getenv("boardtype", buf, sizeof(buf)) >= 0)
		boardtype = simple_strtoul(buf, NULL, 0);
	if (nvram_getenv("boardrev", buf, sizeof(buf)) >= 0)
		boardrev = simple_strtoul(buf, NULL, 0);
	if (nvram_getenv("boardflags", buf, sizeof(buf)) >= 0)
		boardflags = simple_strtoul(buf, NULL, 0);
	if (nvram_getenv("sdram_init", buf, sizeof(buf)) >= 0)
		sdram_init = simple_strtoul(buf, NULL, 0);
	if (nvram_getenv("cardbus", buf, sizeof(buf)) >= 0)
		cardbus = simple_strtoul(buf, NULL, 0);
	if (nvram_getenv("st_rev", buf, sizeof(buf)) >= 0)
		strev = simple_strtoul(buf, NULL, 0);

	if ((boardnum == 8 || boardnum == 01)
	  && boardtype == 0x0472 && cardbus == 1) {
		/* Netgear WNR834B, Netgear WNR834Bv2 */
		return ROUTER_NETGEAR_WNR834B;
	}

	if (boardnum == 01 && boardtype == 0x0472 && boardrev == 0x23) {
		/* Netgear WNDR-3300 */
		return ROUTER_NETGEAR_WNDR3300;
	}

	if ((boardnum == 83258 || boardnum == 01)
	  && boardtype == 0x048e
	  && (boardrev == 0x11 || boardrev == 0x10)
	  && boardflags == 0x750
	  && sdram_init == 0x000A) {
		/* Netgear WGR614v8/L/WW 16MB ram, cfe v1.3 or v1.5 */
		return ROUTER_NETGEAR_WGR614L;
	}

	if ((boardnum == 1 || boardnum == 3500)
	  && boardtype == 0x04CF
	  && (boardrev == 0x1213 || boardrev == 02)) {
		/* Netgear WNR3500v2/U/L */
		return ROUTER_NETGEAR_WNR3500L;
	}

	if (boardtype == 0x042f
	  && boardrev == 0x10
	  && boardflags == 0 
	  && strev == 0x11) { 
		/* Simpletech Simpleshare */
		return ROUTER_SIMPLETECH_SIMPLESHARE;
	}

	return 0;
}

struct mtd_partition * __init
init_mtd_partitions(struct mtd_info *mtd, size_t size)
{
	int cfe_size;
	int dual_image_offset = 0;
	/* e.g Netgear 0x003e0000-0x003f0000 : "board_data", we exclude this
	 * part from our mapping to prevent overwriting len/checksum on e.g.
	 * Netgear WGR614v8/L/WW
	 */
	int custom_data_size = 0;

	if ((cfe_size = find_cfe_size(mtd,size)) < 0)
		return NULL;

	/* boot loader */
	bcm47xx_parts[0].offset = 0;
	bcm47xx_parts[0].size   = cfe_size;

	/* nvram */
	if (cfe_size != 384 * 1024) {

		switch (get_router()) {
		case ROUTER_NETGEAR_WGR614L:
		case ROUTER_NETGEAR_WNR834B:
		case ROUTER_NETGEAR_WNDR3300:
		case ROUTER_NETGEAR_WNR3500L:
			/* Netgear: checksum is @ 0x003AFFF8 for 4M flash or checksum
			 * is @ 0x007AFFF8 for 8M flash
			 */
			custom_data_size = mtd->erasesize;

			bcm47xx_parts[3].offset = size - ROUNDUP(NVRAM_SPACE, mtd->erasesize);
			bcm47xx_parts[3].size   = ROUNDUP(NVRAM_SPACE, mtd->erasesize);

			/* Place CFE board_data into a partition */
			bcm47xx_parts[4].name = "board_data";
			bcm47xx_parts[4].offset = bcm47xx_parts[3].offset - custom_data_size;
			bcm47xx_parts[4].size   =  custom_data_size;
			break;

		case ROUTER_SIMPLETECH_SIMPLESHARE:
			/* Fixup Simpletech Simple share nvram  */

			printk("Setting up simpletech nvram\n");
			custom_data_size = mtd->erasesize;

			bcm47xx_parts[3].offset = size - ROUNDUP(NVRAM_SPACE, mtd->erasesize) * 2;
			bcm47xx_parts[3].size   = ROUNDUP(NVRAM_SPACE, mtd->erasesize);

			/* Place backup nvram into a partition */
			bcm47xx_parts[4].name = "nvram_copy";
			bcm47xx_parts[4].offset = size - ROUNDUP(NVRAM_SPACE, mtd->erasesize);
			bcm47xx_parts[4].size   = ROUNDUP(NVRAM_SPACE, mtd->erasesize);
			break;

		default:
			bcm47xx_parts[3].offset = size - ROUNDUP(NVRAM_SPACE, mtd->erasesize);
			bcm47xx_parts[3].size   = ROUNDUP(NVRAM_SPACE, mtd->erasesize);
		}

	} else {
		/* nvram (old 128kb config partition on netgear wgt634u) */
		bcm47xx_parts[3].offset = bcm47xx_parts[0].size;
		bcm47xx_parts[3].size   = ROUNDUP(NVRAM_SPACE, mtd->erasesize);
	}

	/* dual image offset*/
	printk("Looking for dual image\n");
	dual_image_offset=find_dual_image_off(mtd,size);
	/* linux (kernel and rootfs) */
	if (cfe_size != 384 * 1024) {
		if (get_router() == ROUTER_SIMPLETECH_SIMPLESHARE) {
			bcm47xx_parts[1].offset = bcm47xx_parts[0].size;
			bcm47xx_parts[1].size   = bcm47xx_parts[4].offset - dual_image_offset -
				bcm47xx_parts[1].offset - custom_data_size;
		} else {
			bcm47xx_parts[1].offset = bcm47xx_parts[0].size;
			bcm47xx_parts[1].size   = bcm47xx_parts[3].offset - dual_image_offset -
				bcm47xx_parts[1].offset - custom_data_size;
		}
	} else {
		/* do not count the elf loader, which is on one block */
		bcm47xx_parts[1].offset = bcm47xx_parts[0].size + 
			bcm47xx_parts[3].size + mtd->erasesize;
		bcm47xx_parts[1].size   = size - 
			bcm47xx_parts[0].size - 
			(2*bcm47xx_parts[3].size) - 
			mtd->erasesize - custom_data_size;
	}

	/* find and size rootfs */
	find_root(mtd,size,&bcm47xx_parts[2]);
	bcm47xx_parts[2].size = size - dual_image_offset -
				bcm47xx_parts[2].offset -
				bcm47xx_parts[3].size - custom_data_size;

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
	bcm47xx_map.bankwidth = mcore->flash_buswidth;
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
