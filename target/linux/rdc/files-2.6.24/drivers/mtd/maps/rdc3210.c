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
#include <linux/autoconf.h>
#include <linux/sched.h>
#include <linux/squashfs_fs.h>

static struct mtd_info		*rdc3210_mtd;

struct map_info rdc3210_map = 
{
	.name =		"RDC3210 Flash",
	.size =		CONFIG_MTD_RDC3210_SIZE,
	.bankwidth =	CONFIG_MTD_RDC3210_BUSWIDTH,
};

/* Dante: This is the default static mapping, however this is nothing but a hint. (Say dynamic mapping) */
static struct mtd_partition rdc3210_parts[] = 
{
#if CONFIG_MTD_RDC3210_SIZE == 0x400000
	{ name: "linux",   offset:  0,          size: 0x003C0000 },	/* 3840 KB = (Kernel + ROMFS) = (768 KB + 3072 KB) */
	{ name: "romfs",   offset:  0x000C0000, size: 0x00300000 },	/* 3072 KB */
	{ name: "nvram",   offset:  0x003C0000, size: 0x00010000 },	/*   64 KB */
#ifdef CONFIG_MTD_RDC3210_FACTORY_PRESENT
	{ name: "factory", offset:  0x003D0000, size: 0x00010000 },	/*   64 KB */
#endif
	{ name: "bootldr", offset:  0x003E0000, size: 0x00020000 },	/*  128 KB */
#elif CONFIG_MTD_RDC3210_SIZE == 0x200000
	{ name: "linux",   offset:  0x00008000, size: 0x001E8000 },
	{ name: "romfs",   offset:  0x000C8000, size: 0x00128000 },
	{ name: "nvram",   offset:  0x00000000, size: 0x00008000 },	/*   64 KB */
#ifdef CONFIG_MTD_RDC3210_FACTORY_PRESENT
#error Unsupported configuration!
#endif
	{ name: "bootldr", offset:  0x001F0000, size: 0x00010000 },

#elif CONFIG_MTD_RDC3210_SIZE == 0x800000
        { name: "linux",   offset:  0,          size: 0x001F0000 },     /* 1984 KB */
        { name: "config",  offset:  0x001F0000, size: 0x00010000 },     /*   64 KB */
        { name: "romfs",   offset:  0x00200000, size: 0x005D0000 },     /* 5952 KB */
#ifdef CONFIG_MTD_RDC3210_FACTORY_PRESENT
        { name: "factory", offset:  0x007D0000, size: 0x00010000 },     /*   64 KB */
#endif
        { name: "bootldr", offset:  0x007E0000, size: 0x00010000 },     /*   64 KB */
#else
#error Unsupported configuration!
#endif
};

static __u32 crctab[257] = {
	0x00000000, 0x77073096, 0xee0e612c, 0x990951ba,
	0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
	0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
	0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
	0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de,
	0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
	0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,
	0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
	0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
	0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
	0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940,
	0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
	0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116,
	0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
	0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
	0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
	0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a,
	0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
	0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818,
	0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
	0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
	0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
	0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c,
	0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
	0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2,
	0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
	0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
	0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
	0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086,
	0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
	0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4,
	0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
	0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
	0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
	0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8,
	0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
	0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe,
	0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
	0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
	0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
	0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252,
	0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
	0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60,
	0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
	0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
	0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
	0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04,
	0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
	0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a,
	0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
	0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
	0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
	0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e,
	0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
	0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c,
	0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
	0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
	0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
	0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0,
	0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
	0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6,
	0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
	0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
	0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d,
	0
};

static __u32 crc32(__u8 * buf, __u32 len)
{
	register int i;
	__u32 sum;
	register __u32 s0;
	s0 = ~0;
	for (i = 0; i < len; i++) {
		s0 = (s0 >> 8) ^ crctab[(__u8) (s0 & 0xFF) ^ buf[i]];
	}
	sum = ~s0;
	return sum;
}

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

static int __init init_rdc3210_map(void)
{
	rdc3210_map.phys = -rdc3210_map.size;
       	printk(KERN_NOTICE "flash device: %x at %x\n", rdc3210_map.size, rdc3210_map.phys);

#if CONFIG_MTD_RDC3210_SIZE == 0x800000
	simple_map_init(&rdc3210_map);
#endif
       	
	rdc3210_map.map_priv_1 = (unsigned long)(rdc3210_map.virt = ioremap_nocache(rdc3210_map.phys, rdc3210_map.size));

	if (!rdc3210_map.map_priv_1) 
	{
		printk("Failed to ioremap\n");
		return -EIO;
	}
	rdc3210_mtd = do_map_probe("cfi_probe", &rdc3210_map);
#ifdef CONFIG_MTD_RDC3210_STATIC_MAP	/* Dante: This is for fixed map */
	if (rdc3210_mtd) 
	{
		rdc3210_mtd->owner = THIS_MODULE;
		add_mtd_partitions(rdc3210_mtd, rdc3210_parts, sizeof(rdc3210_parts)/sizeof(rdc3210_parts[0]));
		return 0;
	}
#else	/* Dante: This is for dynamic mapping */

#include "imghdr.h"

	typedef struct {
		u8	magic[4];
		u32	kernelsz, ramdisksz;
		u8	magic2[4];
		u32	sz2;
	}sc_imghdr_t;

	if (rdc3210_mtd) 
	{	// Dante
		sc_imghdr_t	*hdr2= (sc_imghdr_t *)(rdc3210_map.map_priv_1);
		gt_imghdr_t	*hdr = (gt_imghdr_t *)hdr2
#ifdef CONFIG_MTD_RDC3210_ALLOW_JFFS2
			, *ptmp
#endif
			;
		int	len, tmp, tmp2, tmp3, tmp4, hdr_type = 0;
		
		if(!memcmp(hdr->magic, GTIMG_MAGIC, 4))
		{
			hdr_type = 1;
			tmp = hdr->kernelsz + sizeof(gt_imghdr_t);
			tmp2 = rdc3210_mtd->erasesize;
			tmp3 = ((tmp / 32) + ((tmp % 32) ? 1 : 0)) * 32;
			tmp4 = ((tmp / tmp2) + ((tmp % tmp2) ? 1 : 0)) * tmp2;
		}
#ifndef CONFIG_MTD_RDC3210_ALLOW_JFFS2
		else if (!memcmp(hdr2->magic, "CSYS", 4))
		{
			hdr_type = 2;
			tmp = hdr2->ramdisksz + hdr2->kernelsz + sizeof(sc_imghdr_t);
			tmp2 = rdc3210_mtd->erasesize;
			tmp3 = ((tmp / 32) + ((tmp % 32) ? 1 : 0)) * 32;
			tmp4 = ((tmp / tmp2) + ((tmp % tmp2) ? 1 : 0)) * tmp2;
		}
#endif
		else
		{
			iounmap((void *)rdc3210_map.map_priv_1);
			rdc3210_map.map_priv_1 = 0L;
			rdc3210_map.virt = NULL;
			printk("Invalid MAGIC for Firmware Image!!!\n");
			return -EIO;
		}
#ifdef CONFIG_MTD_RDC3210_ALLOW_JFFS2
		tmp = (tmp3 == tmp4) ? tmp4 + tmp2 : tmp4;
		if ((ptmp = (gt_imghdr_t *)vmalloc(tmp)) == NULL)
		{
			iounmap((void *)rdc3210_map.map_priv_1);
			rdc3210_map.map_priv_1 = 0L;
			rdc3210_map.virt = NULL;
			printk("Can't allocate 0x%08x for flash-reading buffer!\n", tmp);
			return -ENOMEM;
		}
		if (rdc3210_mtd->read(rdc3210_mtd, 0, tmp, &len, (__u8 *)ptmp) || len != tmp)
		{
			vfree(ptmp);
			iounmap((void *)rdc3210_map.map_priv_1);
			rdc3210_map.map_priv_1 = 0L;
			rdc3210_map.virt = NULL;
			printk("Can't read that much flash! Read 0x%08x of it.\n", len);
			return -EIO;
		}
#endif
#ifdef CONFIG_MTD_RDC3210_FACTORY_PRESENT
		/* 1. Adjust Redboot */
		tmp = rdc3210_mtd->size - rdc3210_parts[4].size;
		rdc3210_parts[4].offset = tmp - (tmp % tmp2);
		rdc3210_parts[4].size   = rdc3210_mtd->size - rdc3210_parts[4].offset;
		
		/* 2. Adjust Factory Default */
		tmp -= rdc3210_parts[3].size;
		rdc3210_parts[3].offset = tmp - (tmp % tmp2);
		rdc3210_parts[3].size   = rdc3210_parts[4].offset - rdc3210_parts[3].offset;
#else
		/* 1. Adjust Redboot */
		tmp = rdc3210_mtd->size - rdc3210_parts[3].size;
		rdc3210_parts[3].offset = tmp - (tmp % tmp2);
		rdc3210_parts[3].size   = rdc3210_mtd->size - rdc3210_parts[3].offset;
#endif
		if (hdr_type == 1) {
		/* 3. Adjust NVRAM */
#ifdef CONFIG_MTD_RDC3210_ALLOW_JFFS2
		if (*(__u32 *)(((unsigned char *)ptmp)+tmp3) == SQUASHFS_MAGIC)
		{
			len = 1;
			tmp4 = tmp3;
			tmp = hdr->imagesz;
		rdc3210_parts[2].name   = "rootfs_data";
		rdc3210_parts[2].offset = rdc3210_parts[0].offset + (((tmp / tmp2) + ((tmp % tmp2) ? 1 : 0)) * tmp2);
		}
		else
#else
			tmp4 = tmp3;
#endif
		{
			len = 0;
		tmp -= rdc3210_parts[2].size;
		rdc3210_parts[2].offset = tmp - (tmp % tmp2);
		}
		rdc3210_parts[2].size   = rdc3210_parts[3].offset - rdc3210_parts[2].offset;
		}
		else if (hdr_type == 2)
		{
			len = 0;
			tmp4 = tmp3;
		}
		
		/* 4. Adjust Linux (Kernel + ROMFS) */
		rdc3210_parts[0].size   = rdc3210_parts[len + hdr_type + 1].offset - rdc3210_parts[0].offset;

		/* 5. Adjust ROMFS */
		rdc3210_parts[1].offset = rdc3210_parts[0].offset + tmp4;
		rdc3210_parts[1].size   = rdc3210_parts[hdr_type + 1].offset - rdc3210_parts[1].offset;
#ifdef CONFIG_MTD_RDC3210_ALLOW_JFFS2
		if (!(hdr->reserved || len))
		{
			__u8	buf[1024];
			ptmp->reserved = hdr->imagesz;
			ptmp->imagesz  = tmp4;
			ptmp->checksum = ptmp->fastcksum = 0;
			memcpy(buf, ptmp, 0x100);
			memcpy(buf + 0x100, ((__u8 *)ptmp) + ((tmp4 >> 1) - ((tmp4 & 0x6) >> 1)), 0x100);
			memcpy(buf + 0x200, ((__u8 *)ptmp) + (tmp4 - 0x200), 0x200);
			ptmp->fastcksum = crc32(buf, sizeof(buf));
			ptmp->checksum = crc32((__u8 *)ptmp, tmp4);
			if (rdc3210_mtd->unlock) rdc3210_mtd->unlock(rdc3210_mtd, 0, tmp2);
			if ((len = erase_write(rdc3210_mtd, 0, tmp2, (char *)ptmp)))
			{
				vfree(ptmp);
				iounmap((void *)rdc3210_map.map_priv_1);
				rdc3210_map.map_priv_1 = 0L;
				rdc3210_map.virt = NULL;
				printk("Couldn't erase! Got %d.\n", len);
				return len;
			}
			if (rdc3210_mtd->sync) rdc3210_mtd->sync(rdc3210_mtd);
		}
		vfree(ptmp);
#endif
		rdc3210_mtd->owner = THIS_MODULE;
		add_mtd_partitions(rdc3210_mtd, rdc3210_parts, sizeof(rdc3210_parts)/sizeof(rdc3210_parts[0]));
		return 0;
	}
#endif
	iounmap((void *)rdc3210_map.map_priv_1);
	rdc3210_map.map_priv_1 = 0L;
	rdc3210_map.virt = NULL;
	return -ENXIO;
}

static void __exit cleanup_rdc3210_map(void)
{
	if (rdc3210_mtd) 
	{
		del_mtd_partitions(rdc3210_mtd);
		map_destroy(rdc3210_mtd);
	}
	
	if (rdc3210_map.map_priv_1) 
	{
		iounmap((void *)rdc3210_map.map_priv_1);
		rdc3210_map.map_priv_1 = 0L;
		rdc3210_map.virt = NULL;
	}
}

module_init(init_rdc3210_map);
module_exit(cleanup_rdc3210_map);
