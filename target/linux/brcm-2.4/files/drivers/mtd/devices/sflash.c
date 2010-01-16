/*
 * Broadcom SiliconBackplane chipcommon serial flash interface
 *
 * Copyright 2006, Broadcom Corporation      
 * All Rights Reserved.      
 *       
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY      
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM      
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS      
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.      
 *
 * $Id$
 */

#include <linux/config.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/ioport.h>
#include <linux/mtd/compatmac.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/errno.h>
#include <linux/pci.h>
#include <linux/delay.h>
#include <asm/io.h>

#include <typedefs.h>
#include <osl.h>
// #include <bcmutils.h>
#include <bcmdevs.h>
#include <bcmnvram.h>
#include <sbutils.h>
#include <sbconfig.h>
#include <sbchipc.h>
#include <sflash.h>

#ifdef CONFIG_MTD_PARTITIONS
extern struct mtd_partition * init_mtd_partitions(struct mtd_info *mtd, size_t size);
#endif

struct sflash_mtd {
	sb_t *sbh;
	chipcregs_t *cc;
	struct semaphore lock;
	struct mtd_info mtd;
	struct mtd_erase_region_info region;
};

/* Private global state */
static struct sflash_mtd sflash;

static int
sflash_mtd_poll(struct sflash_mtd *sflash, unsigned int offset, int timeout)
{
	int now = jiffies;
	int ret = 0;

	for (;;) {
		if (!sflash_poll(sflash->sbh, sflash->cc, offset)) {
			ret = 0;
			break;
		}
		if (time_after(jiffies, now + timeout)) {
			printk(KERN_ERR "sflash: timeout\n");
			ret = -ETIMEDOUT;
			break;
		}
		if (current->need_resched) {
			set_current_state(TASK_UNINTERRUPTIBLE);
			schedule_timeout(timeout / 10);
		} else
			udelay(1);
	}

	return ret;
}

static int
sflash_mtd_read(struct mtd_info *mtd, loff_t from, size_t len, size_t *retlen, u_char *buf)
{
	struct sflash_mtd *sflash = (struct sflash_mtd *) mtd->priv;
	int bytes, ret = 0;

	/* Check address range */
	if (len == 0){
	 *retlen = 0;
		return 0;
 }
	if (!len)
		return 0;
	if ((from + len) > mtd->size)
		return -EINVAL;
	
	down(&sflash->lock);

	*retlen = 0;
	while (len) {
		if ((bytes = sflash_read(sflash->sbh, sflash->cc, (uint) from, len, buf)) < 0) {
			ret = bytes;
			break;
		}
		from += (loff_t) bytes;
		len -= bytes;
		buf += bytes;
		*retlen += bytes;
	}

	up(&sflash->lock);

	return ret;
}

static int
sflash_mtd_write(struct mtd_info *mtd, loff_t to, size_t len, size_t *retlen, const u_char *buf)
{
	struct sflash_mtd *sflash = (struct sflash_mtd *) mtd->priv;
	int bytes, ret = 0;

	/* Check address range */
	if (len == 0){
	 *retlen = 0;
		return 0;
 }
	if (!len)
		return 0;
	if ((to + len) > mtd->size)
		return -EINVAL;

	down(&sflash->lock);

	*retlen = 0;
	while (len) {
		if ((bytes = sflash_write(sflash->sbh, sflash->cc, (uint)to, (uint)len, buf)) < 0) {
			ret = bytes;
			break;
		}
		if ((ret = sflash_mtd_poll(sflash, (unsigned int) to, HZ / 10)))
			break;
		to += (loff_t) bytes;
		len -= bytes;
		buf += bytes;
		*retlen += bytes;
	}

	up(&sflash->lock);

	return ret;
}

static int
sflash_mtd_erase(struct mtd_info *mtd, struct erase_info *erase)
{
	struct sflash_mtd *sflash = (struct sflash_mtd *) mtd->priv;
	int i, j, ret = 0;
	unsigned int addr, len;

	/* Check address range */
	if (!erase->len)
		return 0;
	if ((erase->addr + erase->len) > mtd->size)
		return -EINVAL;

	addr = erase->addr;
	len = erase->len;

	down(&sflash->lock);

	/* Ensure that requested region is aligned */
	for (i = 0; i < mtd->numeraseregions; i++) {
		for (j = 0; j < mtd->eraseregions[i].numblocks; j++) {
			if (addr == mtd->eraseregions[i].offset + mtd->eraseregions[i].erasesize * j &&
			    len >= mtd->eraseregions[i].erasesize) {
				if ((ret = sflash_erase(sflash->sbh, sflash->cc, addr)) < 0)
					break;
				if ((ret = sflash_mtd_poll(sflash, addr, 10 * HZ)))
					break;
				addr += mtd->eraseregions[i].erasesize;
				len -= mtd->eraseregions[i].erasesize;
			}
		}
		if (ret)
			break;
	}

	up(&sflash->lock);

	/* Set erase status */
	if (ret)
		erase->state = MTD_ERASE_FAILED;
	else 
		erase->state = MTD_ERASE_DONE;

	/* Call erase callback */
	if (erase->callback)
		erase->callback(erase);

	return ret;
}

#if LINUX_VERSION_CODE < 0x20212 && defined(MODULE)
#define sflash_mtd_init init_module
#define sflash_mtd_exit cleanup_module
#endif

mod_init_t
sflash_mtd_init(void)
{
	struct pci_dev *pdev;
	int ret = 0;
	struct sflash *info;
	uint i;
#ifdef CONFIG_MTD_PARTITIONS
	struct mtd_partition *parts;
#endif

	if (!(pdev = pci_find_device(VENDOR_BROADCOM, SB_CC, NULL))) {
		printk(KERN_ERR "sflash: chipcommon not found\n");
		return -ENODEV;
	}

	memset(&sflash, 0, sizeof(struct sflash_mtd));
	init_MUTEX(&sflash.lock);

	/* attach to the backplane */
	if (!(sflash.sbh = sb_kattach(SB_OSH))) {
		printk(KERN_ERR "sflash: error attaching to backplane\n");
		ret = -EIO;
		goto fail;
	}

	/* Map registers and flash base */
	if (!(sflash.cc = ioremap_nocache(pci_resource_start(pdev, 0),
					  pci_resource_len(pdev, 0)))) {
		printk(KERN_ERR "sflash: error mapping registers\n");
		ret = -EIO;
		goto fail;
	}

	/* Initialize serial flash access */
	if (!(info = sflash_init(sflash.sbh, sflash.cc))) {
		printk(KERN_ERR "sflash: found no supported devices\n");
		ret = -ENODEV;
		goto fail;
	}

	printk(KERN_INFO "sflash: found serial flash; blocksize=%dKB, numblocks=%d, size=%dKB\n",info->blocksize/1024,info->numblocks,info->size/1024);

	/* Setup region info */
	sflash.region.offset = 0;
	sflash.region.erasesize = info->blocksize;
	sflash.region.numblocks = info->numblocks;
	if (sflash.region.erasesize > sflash.mtd.erasesize)
		sflash.mtd.erasesize = sflash.region.erasesize;
	sflash.mtd.size = info->size;
	sflash.mtd.numeraseregions = 1;

	/* Register with MTD */
	sflash.mtd.name = "sflash";
	sflash.mtd.type = MTD_NORFLASH;
	sflash.mtd.flags = MTD_CAP_NORFLASH;
	sflash.mtd.eraseregions = &sflash.region;
	sflash.mtd.module = THIS_MODULE;
	sflash.mtd.erase = sflash_mtd_erase;
	sflash.mtd.read = sflash_mtd_read;
	sflash.mtd.write = sflash_mtd_write;
	sflash.mtd.priv = &sflash;

#ifdef CONFIG_MTD_PARTITIONS
	parts = init_mtd_partitions(&sflash.mtd, sflash.mtd.size);
	for (i = 0; parts[i].name; i++);
	ret = add_mtd_partitions(&sflash.mtd, parts, i);
#else
	ret = add_mtd_device(&sflash.mtd);
#endif
	if (ret) {
		printk(KERN_ERR "sflash: add_mtd failed\n");
		goto fail;
	}

	return 0;

 fail:
	if (sflash.cc)
		iounmap((void *) sflash.cc);
	if (sflash.sbh)
		sb_detach(sflash.sbh);
	return ret;
}

mod_exit_t
sflash_mtd_exit(void)
{
#ifdef CONFIG_MTD_PARTITIONS
	del_mtd_partitions(&sflash.mtd);
#else
	del_mtd_device(&sflash.mtd);
#endif
	iounmap((void *) sflash.cc);
	sb_detach(sflash.sbh);
}

module_init(sflash_mtd_init);
module_exit(sflash_mtd_exit);
