/*******************************************************************
 * Simple Flash mapping for RDC3210                                *
 *                                                                 *
 *                                                     2005.03.23  *
 *                              Dante Su (dante_su@gemtek.com.tw)  *
 *                          Copyright (C) 2005 Gemtek Corporation  *
 *******************************************************************/

#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <asm/io.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/map.h>
#include <linux/mtd/partitions.h>
#include <linux/config.h>

#ifndef RDC3210_STATIC_MAP
#define RDC3210_STATIC_MAP	0
#endif
#ifndef RDC3210_FACTORY_DFLT
#define RDC3210_FACTORY_DFLT	0
#endif
#ifndef RDC3210_USING_JFFS2
#define RDC3210_USING_JFFS2	1
#endif

#define WINDOW_ADDR		0xFFC00000
#define WINDOW_SIZE		0x00400000

#define BUSWIDTH 2

/* Dante: linked from linux-2.4.x/drivers/mtd/chips/flashdrv.c */
extern int flashdrv_get_size(void);
extern int flashdrv_get_sector(int addr);
extern int flashdrv_get_sector_addr(int sector);
extern int flashdrv_get_sector_size(int sector);

static struct mtd_info		*rdc3210_mtd;

__u8  rdc3210_map_read8(struct map_info *map, unsigned long ofs)
{
	return *(__u8 *)(map->map_priv_1 + ofs);
}

__u16 rdc3210_map_read16(struct map_info *map, unsigned long ofs)
{
	return *(__u16 *)(map->map_priv_1 + ofs);
}

__u32 rdc3210_map_read32(struct map_info *map, unsigned long ofs)
{
	return *(__u32 *)(map->map_priv_1 + ofs);
}

void rdc3210_map_write8(struct map_info *map, __u8 d, unsigned long adr)
{
	*(__u8 *)(map->map_priv_1 + adr) = d;
}

void rdc3210_map_write16(struct map_info *map, __u16 d, unsigned long adr)
{
	*(__u16 *)(map->map_priv_1 + adr) = d;
}

void rdc3210_map_write32(struct map_info *map, __u32 d, unsigned long adr)
{
	*(__u32 *)(map->map_priv_1 + adr) = d;
}

void rdc3210_map_copy_from(struct map_info *map, void *to, unsigned long from, ssize_t len)
{
	int	i;
	u16	*dst = (u16 *)(to);
	u16	*src = (u16 *)(map->map_priv_1 + from);

	for(i = 0; i < (len / 2); ++i)
		dst[i] = src[i];

	if(len & 1)
	{
		printk("# WARNNING!!! rdc3210_map_copy_from has odd length\n");
		//dst[len - 1] = B0(src[i]);
	}
}

void rdc3210_map_copy_to(struct map_info *map, void *to, unsigned long from, ssize_t len)
{
	int	i;
	u16	*dst = (u16 *)(map->map_priv_1 + to);
	u16	*src = (u16 *)(from);
	
	for(i = 0; i < (len / 2); ++i)
		dst[i] = src[i];

	if(len & 1)
	{
		printk("# WARNNING!!! rdc3210_map_copy_from has odd length\n");
		//dst[len - 1] = B0(src[i]);
	}
}

struct map_info rdc3210_map = 
{
	name:		"RDC3210 Flash",
	size:		WINDOW_SIZE,
	buswidth:	BUSWIDTH,
	read8:		rdc3210_map_read8,
	read16:		rdc3210_map_read16,
	read32:		rdc3210_map_read32,
	copy_from:	rdc3210_map_copy_from,
	write8:		rdc3210_map_write8,
	write16:	rdc3210_map_write16,
	write32:	rdc3210_map_write32,
	copy_to:	rdc3210_map_copy_to,
};

/* Dante: This is the default static mapping, however this is nothing but a hint. (Say dynamic mapping) */
static struct mtd_partition rdc3210_parts[] = 
{
	{ name: "linux",   offset:  0,          size: 0x003C0000 },	/* 3840 KB = (Kernel + ROMFS) = (768 KB + 3072 KB) */
	{ name: "romfs",   offset:  0x000C0000, size: 0x00300000 },	/* 3072 KB */
	{ name: "nvram",   offset:  0x003C0000, size: 0x00010000 },	/*   64 KB */
#if RDC3210_STATIC_MAP || RDC3210_FACTORY_DFLT
	{ name: "factory", offset:  0x003D0000, size: 0x00010000 },	/*   64 KB */
#endif
	{ name: "bootldr", offset:  0x003E0000, size: 0x00020000 },	/*  128 KB */
};

#if LINUX_VERSION_CODE < 0x20212 && defined(MODULE)
#define init_rdc3210_map		init_module
#define cleanup_rdc3210_map		cleanup_module
#endif

mod_init_t init_rdc3210_map(void)
{
       	printk(KERN_NOTICE "flash device: %x at %x\n", WINDOW_SIZE, WINDOW_ADDR);
       	
	rdc3210_map.map_priv_1 = ioremap(WINDOW_ADDR, WINDOW_SIZE);

	if (!rdc3210_map.map_priv_1) 
	{
		printk("Failed to ioremap\n");
		return -EIO;
	}
	rdc3210_mtd = do_map_probe("cfi_probe", &rdc3210_map);
#if RDC3210_STATIC_MAP	/* Dante: This is for fixed map */
	if (rdc3210_mtd) 
	{
		rdc3210_mtd->module = THIS_MODULE;
		add_mtd_partitions(rdc3210_mtd, rdc3210_parts, sizeof(rdc3210_parts)/sizeof(rdc3210_parts[0]));
		return 0;
	}
#else	/* Dante: This is for dynamic mapping */

#include <gemtek/sysdep.h>
#include <gemtek/imghdr.h>

	if (rdc3210_mtd) 
	{	// Dante
#if RDC3210_USING_JFFS2
		unsigned int	tmp, tmp2 = rdc3210_mtd->erasesize;
#else
		unsigned int	tmp, tmp2 = 32;
#endif
		gt_imghdr_t	*hdr;
		
		hdr = (gt_imghdr_t *)(rdc3210_map.map_priv_1);
		
		if(memcmp(hdr->magic, GTIMG_MAGIC, 4))
		{
			printk("Invalid MAGIC for Firmware Image!!!\n");
			return -EIO;
		}

#if RDC3210_FACTORY_DFLT
		/* 1. Adjust Redboot */
		tmp = flashdrv_get_size() - rdc3210_parts[4].size;
		rdc3210_parts[4].offset = flashdrv_get_sector_addr(flashdrv_get_sector(tmp));
		rdc3210_parts[4].size   = flashdrv_get_size() - rdc3210_parts[4].offset;
		
		/* 2. Adjust NVRAM */
		tmp -= rdc3210_parts[3].size;
		rdc3210_parts[3].offset = flashdrv_get_sector_addr(flashdrv_get_sector(tmp));
		rdc3210_parts[3].size   = rdc3210_parts[4].offset - rdc3210_parts[3].offset;
		
		/* 3. Adjust Factory Default */
		tmp -= rdc3210_parts[2].size;
		rdc3210_parts[2].offset = flashdrv_get_sector_addr(flashdrv_get_sector(tmp));
		rdc3210_parts[2].size   = rdc3210_parts[3].offset - rdc3210_parts[2].offset;
		
		/* 4. Adjust Linux (Kernel + ROMFS) */
		rdc3210_parts[0].size   = rdc3210_parts[2].offset - rdc3210_parts[0].offset;

		/* 5. Adjust ROMFS */
		tmp = hdr->kernelsz + sizeof(gt_imghdr_t);
		rdc3210_parts[1].offset = rdc3210_parts[0].offset + (((tmp / tmp2) + ((tmp % tmp2) ? 1 : 0)) * tmp2);
		rdc3210_parts[1].size   = rdc3210_parts[2].offset - rdc3210_parts[1].offset;
#else
		/* 1. Adjust Redboot */
		tmp = flashdrv_get_size() - rdc3210_parts[3].size;
		rdc3210_parts[3].offset = flashdrv_get_sector_addr(flashdrv_get_sector(tmp));
		rdc3210_parts[3].size   = flashdrv_get_size() - rdc3210_parts[3].offset;
		
		/* 2. Adjust NVRAM */
		tmp -= rdc3210_parts[2].size;
		rdc3210_parts[2].offset = flashdrv_get_sector_addr(flashdrv_get_sector(tmp));
		rdc3210_parts[2].size   = rdc3210_parts[3].offset - rdc3210_parts[2].offset;
				
		/* 4. Adjust Linux (Kernel + ROMFS) */
		rdc3210_parts[0].size   = rdc3210_parts[2].offset - rdc3210_parts[0].offset;

		/* 5. Adjust ROMFS */
		tmp = hdr->kernelsz + sizeof(gt_imghdr_t);
		rdc3210_parts[1].offset = rdc3210_parts[0].offset + (((tmp / tmp2) + ((tmp % tmp2) ? 1 : 0)) * tmp2);
		rdc3210_parts[1].size   = rdc3210_parts[2].offset - rdc3210_parts[1].offset;
#endif
		
		rdc3210_mtd->module = THIS_MODULE;
		add_mtd_partitions(rdc3210_mtd, rdc3210_parts, sizeof(rdc3210_parts)/sizeof(rdc3210_parts[0]));
		return 0;
	}
#endif

	iounmap((void *)rdc3210_map.map_priv_1);
	return -ENXIO;
}

mod_exit_t cleanup_rdc3210_map(void)
{
	if (rdc3210_mtd) 
	{
		del_mtd_partitions(rdc3210_mtd);
		map_destroy(rdc3210_mtd);
	}
	
	if (rdc3210_map.map_priv_1) 
	{
		iounmap((void *)rdc3210_map.map_priv_1);
		rdc3210_map.map_priv_1 = NULL;
	}
}

module_init(init_rdc3210_map);
module_exit(cleanup_rdc3210_map);
