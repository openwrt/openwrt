/*
 * Flash mapping for BCM947XX boards
 *
 * Copyright 2004, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id$
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <asm/io.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/map.h>
#include <linux/mtd/partitions.h>
#include <linux/config.h>

#include <typedefs.h>
#include <bcmnvram.h>
#include <bcmutils.h>
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

#ifdef CONFIG_MTD_PARTITIONS
extern struct mtd_partition * init_mtd_partitions(struct mtd_info *mtd, size_t size);
#endif

#define WINDOW_ADDR 0x1fc00000
#define WINDOW_SIZE 0x400000
#define BUSWIDTH 2

/* e.g., flash=2M or flash=4M */
static int flash = 0;
MODULE_PARM(flash, "i");
static int __init
bcm947xx_setup(char *str)
{
	flash = memparse(str, &str);
	return 1;
}
__setup("flash=", bcm947xx_setup);

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

#if LINUX_VERSION_CODE < 0x20212 && defined(MODULE)
#define init_bcm947xx_map init_module
#define cleanup_bcm947xx_map cleanup_module
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
		fltype = readl(&cc->capabilities) & CAP_FLASH_MASK;
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
		printk(KERN_ERR "pflash: ioremap failed\n");
		ret = -EIO;
		goto fail;
	}

	if (!(bcm947xx_mtd = do_map_probe("cfi_probe", &bcm947xx_map))) {
		printk(KERN_ERR "pflash: cfi_probe failed\n");
		ret = -ENXIO;
		goto fail;
	}

	bcm947xx_mtd->module = THIS_MODULE;

	/* Allow size override for testing */
	size = flash ? : bcm947xx_mtd->size;

	printk(KERN_NOTICE "Flash device: 0x%x at 0x%x\n", size, window_addr);

#ifdef CONFIG_MTD_PARTITIONS
	parts = init_mtd_partitions(bcm947xx_mtd, size);
	for (i = 0; parts[i].name; i++);
	ret = add_mtd_partitions(bcm947xx_mtd, parts, i);
	if (ret) {
		printk(KERN_ERR "pflash: add_mtd_partitions failed\n");
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
