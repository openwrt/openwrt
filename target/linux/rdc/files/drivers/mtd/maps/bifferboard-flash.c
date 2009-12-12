/* Flash mapping for Bifferboard, (c) bifferos@yahoo.co.uk 
 * Works with 1, 4 and 8MB flash versions
 */

#include <linux/delay.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/map.h>
#include <linux/mtd/partitions.h>

#define DRV "bifferboard-flash: "

static struct mtd_info *bb_mtd = NULL;

#define SIZE_BIFFBOOT 0x10000
#define SIZE_SECTOR   0x10000
#define SIZE_CONFIG   0x02000
#define SIZE_1MB 0x00100000
#define SIZE_4MB 0x00400000
#define SIZE_8MB 0x00800000

#define BASE_1MB 0xfff00000
#define BASE_4MB 0xffc00000
#define BASE_8MB 0xff800000

#define OFFS_KERNEL 0x6000
#define OFFS_CONFIG 0x4000

/*
 * Flash detection code.  This is needed because the cfi_probe doesn't probe
 * for the size correctly.  You actually have to know the flash size before you
 * call it, or so it seems.  It does work correctly if you map the entire flash
 * chip, but unsure of the implications for mapping more mem than exists.
 */

static unsigned char ReadFlash(void* base, u32 addr)
{
	unsigned char val = *(volatile unsigned char *)(base+addr);  
	udelay(1);
	return val;
}

static void WriteFlash(void* base, u32 addr, unsigned short data)
{
	*(volatile unsigned short *)(base+addr) = data;
}

static DEFINE_SPINLOCK(flash_lock);

static u32 bb_detect(void)
{
	u32 ret = 0;  
	ulong flags;
  
	/* for detection, map in just the 1MB device, 1st 64k is enough */
	void* base = ioremap_nocache(BASE_1MB, 0x10000);
    
	if (!base)
	{
		pr_err(DRV "Failed to map flash for probing\n");
		return 0;
	}
  
	spin_lock_irqsave(&flash_lock, flags);  
  
	/* put flash in auto-detect mode */
	WriteFlash(base, 0xaaaa, 0xaaaa);
	WriteFlash(base, 0x5554, 0x5555);
	WriteFlash(base, 0xaaaa, 0x9090);

	/* Read the auto-config data - 4 values in total */
	ret = ReadFlash(base, 0x0000);  ret <<= 8;
	ret |= ReadFlash(base, 0x0200);  ret <<= 8;
	ret |= ReadFlash(base, 0x0003);  ret <<= 8;
	ret |= ReadFlash(base, 0x0002);
  
	/* exit the autodetect state */
	WriteFlash(base, 0x0000, 0xf0f0);
  
	spin_unlock_irqrestore(&flash_lock, flags);
  
	/* unmap it, it'll be re-mapped based on the detection */
	iounmap(base);
  
	return ret;
}


static struct map_info bb_map;

/* Update the map, using the detected flash signature. */
static int bb_adjust_map(unsigned long sig, struct map_info* m)
{
	m->bankwidth = 2;
	switch (sig)
	{
		case 0x7f1c225b :
			m->name = "ENLV800B";
			m->phys = BASE_1MB;
			m->size = SIZE_1MB;
			break;
		case 0x7f1c22f9 :
			m->name = "ENLV320B";
			m->phys = BASE_4MB;
			m->size = SIZE_4MB;
			break;
		case 0x7f1c22cb :
			m->name = "ENLV640B";
			m->phys = BASE_8MB;
			m->size = SIZE_8MB;
			break;
		default:
			return -ENXIO;
	}
	return 0;
}


/* adjust partition sizes, prior to adding the partition
 * Values not specified by defines here are subject to replacement based on 
 * the real flash size. (0x00000000 being the exception, of course) */

static struct mtd_partition bb_parts[] = 
{
/* 0 */	{ name: "kernel",   offset: 0x00000000,         size: 0x000fa000 },
/* 1 */	{ name: "rootfs",   offset: MTDPART_OFS_APPEND, size: 0x002F0000 },
/* 2 */	{ name: "biffboot", offset: MTDPART_OFS_APPEND, size: MTDPART_SIZ_FULL, mask_flags: MTD_WRITEABLE },
};


/* returns the count of partitions to be used */
static ulong bb_adjust_partitions(struct map_info* m)
{
	/* Read the kernel offset value, 1036 bytes into the config block. */
	u16 km_units = *((u16*)(m->virt + OFFS_CONFIG + 1036));
	u32 kernelmax = 0x200000;  /* Biffboot 2.7 or earlier default */
	
	/* special-case 1MB devices, because there are fewer partitions */
	if (m->size == SIZE_1MB)
	{
		bb_parts[0].size   = SIZE_1MB - SIZE_BIFFBOOT;	/* kernel */
		bb_parts[1].name   = "biffboot";		/* biffboot */
		bb_parts[1].offset = MTDPART_OFS_APPEND;	/* biffboot */
		bb_parts[1].size   = MTDPART_SIZ_FULL;		/* biffboot */
		bb_parts[1].mask_flags = MTD_WRITEABLE;
		return 2;   /* because there's no rootfs now */
	}
	
	/* sanity check */
	if (km_units > ((m->size-SIZE_BIFFBOOT)/SIZE_SECTOR))
	{
		pr_err(DRV "config block has invalid kernelmax\n");
		return 0;
	}
	
	kernelmax = km_units * SIZE_SECTOR;
	
	/* Kernel */
	bb_parts[0].size = kernelmax;
	
	/* rootfs */
	bb_parts[1].size   = m->size - kernelmax - SIZE_BIFFBOOT;
		
	return 3;  /* 3 partitions */
}


static int __init init_bb_map(void)
{
	int ret;
	ulong signature = bb_detect();
	ulong part_len;
	
	if (!signature)
	{
		pr_err(DRV "Undetected flash chip");
		return -ENXIO;
	}
	
	ret = bb_adjust_map(signature, &bb_map);
	
	if (ret)
	{
		pr_err(DRV "Unrecognised flash chip (signature: 0x%lx)\n", signature);
		return ret;
	}
	
	bb_map.virt = ioremap_nocache(bb_map.phys, bb_map.size);
	if (!bb_map.virt) 
	{
		pr_err(DRV "ioremap\n");
		return -EIO;
	}
	
	bb_mtd = do_map_probe("cfi_probe", &bb_map);
	if (!bb_mtd)
	{
		/* cfi_probe fails here for 1MB devices */
		pr_err(DRV "cfi_probe\n");
		ret = -ENXIO;
		goto err;
	}

	part_len = bb_adjust_partitions(&bb_map);
	if (!part_len)
	{
		pr_err(DRV "no partitions established");
		ret = -ENXIO;
		goto err2;
	}
	
	bb_mtd->owner = THIS_MODULE;
	ret = add_mtd_partitions(bb_mtd, bb_parts, part_len);
	if (ret) 
	{
		pr_err(DRV "add_mtd_partitions\n");
		ret = -ENXIO;
		goto err2;
	}
	
	return 0;
	
err2:
	map_destroy(bb_mtd);
err:
	iounmap(bb_map.virt);

	return ret;
}

static void __exit exit_bb_map(void)
{
	del_mtd_partitions(bb_mtd);
	map_destroy(bb_mtd);	
	iounmap(bb_map.virt);
}

module_init(init_bb_map);
module_exit(exit_bb_map);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bifferos <bifferos@yahoo.co.uk>");
MODULE_DESCRIPTION("MTD map driver for Bifferboard");

