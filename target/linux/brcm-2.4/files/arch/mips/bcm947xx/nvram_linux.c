/*
 * NVRAM variable manipulation (Linux kernel half)
 *
 * Copyright 2006, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id: nvram_linux.c,v 1.19 2006/04/08 07:12:42 honor Exp $
 */

#include <linux/config.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/interrupt.h>
#include <linux/spinlock.h>
#include <linux/slab.h>
#include <linux/bootmem.h>
#include <linux/wrapper.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/mtd/mtd.h>
#include <asm/addrspace.h>
#include <asm/io.h>
#include <asm/uaccess.h>

#include <typedefs.h>
#include <osl.h>
#include <bcmendian.h>
#include <bcmnvram.h>
#include <sbconfig.h>
#include <sbchipc.h>
#include <sbutils.h>
#include <hndmips.h>
#include <sflash.h>

/* In BSS to minimize text size and page aligned so it can be mmap()-ed */
static char nvram_buf[NVRAM_SPACE] __attribute__((aligned(PAGE_SIZE)));

#ifdef MODULE

#define early_nvram_get(name) nvram_get(name)

#else /* !MODULE */

/* Global SB handle */
extern void *bcm947xx_sbh;
extern spinlock_t bcm947xx_sbh_lock;

static int cfe_env;
extern char *cfe_env_get(char *nv_buf, const char *name);

/* Convenience */
#define sbh bcm947xx_sbh
#define sbh_lock bcm947xx_sbh_lock
#define KB * 1024
#define MB * 1024 * 1024

/* Probe for NVRAM header */
static void __init
early_nvram_init(void)
{
	struct nvram_header *header;
	chipcregs_t *cc;
	struct sflash *info = NULL;
	int i;
	uint32 base, off, lim;
	u32 *src, *dst;

	if ((cc = sb_setcore(sbh, SB_CC, 0)) != NULL) {
		base = KSEG1ADDR(SB_FLASH2);
		switch (readl(&cc->capabilities) & CC_CAP_FLASH_MASK) {
		case PFLASH:
			lim = SB_FLASH2_SZ;
			break;

		case SFLASH_ST:
		case SFLASH_AT:
			if ((info = sflash_init(sbh,cc)) == NULL)
				return;
			lim = info->size;
			break;

		case FLASH_NONE:
		default:
			return;
		}
	} else {
		/* extif assumed, Stop at 4 MB */
		base = KSEG1ADDR(SB_FLASH1);
		lim = SB_FLASH1_SZ;
	}

	/* XXX: hack for supporting the CFE environment stuff on WGT634U */
	src = (u32 *) KSEG1ADDR(base + 8 * 1024 * 1024 - 0x2000);
	dst = (u32 *) nvram_buf;
	if ((lim == 0x02000000) && ((*src & 0xff00ff) == 0x000001)) {
		printk("early_nvram_init: WGT634U NVRAM found.\n");

		for (i = 0; i < 0x1ff0; i++) {
			if (*src == 0xFFFFFFFF)
				break;
			*dst++ = *src++;
		}
		cfe_env = 1;
		return;
	}

	off = FLASH_MIN;
	while (off <= lim) {
		/* Windowed flash access */
		header = (struct nvram_header *) KSEG1ADDR(base + off - NVRAM_SPACE);
		if (header->magic == NVRAM_MAGIC)
			goto found;
		off <<= 1;
	}

	/* Try embedded NVRAM at 4 KB and 1 KB as last resorts */
	header = (struct nvram_header *) KSEG1ADDR(base + 4 KB);
	if (header->magic == NVRAM_MAGIC)
		goto found;
	
	header = (struct nvram_header *) KSEG1ADDR(base + 1 KB);
	if (header->magic == NVRAM_MAGIC)
		goto found;
	
	printk("early_nvram_init: NVRAM not found\n");
	return;

found:
	src = (u32 *) header;
	dst = (u32 *) nvram_buf;
	for (i = 0; i < sizeof(struct nvram_header); i += 4)
		*dst++ = *src++;
	for (; i < header->len && i < NVRAM_SPACE; i += 4)
		*dst++ = ltoh32(*src++);
}

/* Early (before mm or mtd) read-only access to NVRAM */
static char * __init
early_nvram_get(const char *name)
{
	char *var, *value, *end, *eq;

	if (!name)
		return NULL;

	/* Too early? */
	if (sbh == NULL)
		return NULL;

	if (!nvram_buf[0])
		early_nvram_init();

	if (cfe_env)
		return cfe_env_get(nvram_buf, name);

	/* Look for name=value and return value */
	var = &nvram_buf[sizeof(struct nvram_header)];
	end = nvram_buf + sizeof(nvram_buf) - 2;
	end[0] = end[1] = '\0';
	for (; *var; var = value + strlen(value) + 1) {
		if (!(eq = strchr(var, '=')))
			break;
		value = eq + 1;
		if ((eq - var) == strlen(name) && strncmp(var, name, (eq - var)) == 0)
			return value;
	}

	return NULL;
}

static int __init
early_nvram_getall(char *buf, int count)
{
	char *var, *end;
	int len = 0;
	
	/* Too early? */
	if (sbh == NULL)
		return -1;

	if (!nvram_buf[0])
		early_nvram_init();

	bzero(buf, count);

	/* Write name=value\0 ... \0\0 */
	var = &nvram_buf[sizeof(struct nvram_header)];
	end = nvram_buf + sizeof(nvram_buf) - 2;
	end[0] = end[1] = '\0';
	for (; *var; var += strlen(var) + 1) {
		if ((count - len) <= (strlen(var) + 1))
			break;
		len += sprintf(buf + len, "%s", var) + 1;
	}

	return 0;
}
#endif /* !MODULE */

extern char * _nvram_get(const char *name);
extern int _nvram_set(const char *name, const char *value);
extern int _nvram_unset(const char *name);
extern int _nvram_getall(char *buf, int count);
extern int _nvram_commit(struct nvram_header *header);
extern int _nvram_init(void *sbh);
extern void _nvram_exit(void);

/* Globals */
static spinlock_t nvram_lock = SPIN_LOCK_UNLOCKED;
static struct semaphore nvram_sem;
static unsigned long nvram_offset = 0;
static int nvram_major = -1;
static devfs_handle_t nvram_handle = NULL;
static struct mtd_info *nvram_mtd = NULL;

int
_nvram_read(char *buf)
{
	struct nvram_header *header = (struct nvram_header *) buf;
	size_t len;

	if (!nvram_mtd ||
	    MTD_READ(nvram_mtd, nvram_mtd->size - NVRAM_SPACE, NVRAM_SPACE, &len, buf) ||
	    len != NVRAM_SPACE ||
	    header->magic != NVRAM_MAGIC) {
		/* Maybe we can recover some data from early initialization */
		memcpy(buf, nvram_buf, NVRAM_SPACE);
	}

	return 0;
}

struct nvram_tuple *
_nvram_realloc(struct nvram_tuple *t, const char *name, const char *value)
{
	if ((nvram_offset + strlen(value) + 1) > NVRAM_SPACE)
		return NULL;

	if (!t) {
		if (!(t = kmalloc(sizeof(struct nvram_tuple) + strlen(name) + 1, GFP_ATOMIC)))
			return NULL;

		/* Copy name */
		t->name = (char *) &t[1];
		strcpy(t->name, name);

		t->value = NULL;
	}

	/* Copy value */
	if (!t->value || strcmp(t->value, value)) {
		t->value = &nvram_buf[nvram_offset];
		strcpy(t->value, value);
		nvram_offset += strlen(value) + 1;
	}

	return t;
}

void
_nvram_free(struct nvram_tuple *t)
{
	if (!t)
		nvram_offset = 0;
	else
		kfree(t);
}

int
nvram_set(const char *name, const char *value)
{
	unsigned long flags;
	int ret;
	struct nvram_header *header;

	spin_lock_irqsave(&nvram_lock, flags);
	if ((ret = _nvram_set(name, value))) {
		/* Consolidate space and try again */
		if ((header = kmalloc(NVRAM_SPACE, GFP_ATOMIC))) {
			if (_nvram_commit(header) == 0)
				ret = _nvram_set(name, value);
			kfree(header);
		}
	}
	spin_unlock_irqrestore(&nvram_lock, flags);

	return ret;
}

char *
real_nvram_get(const char *name)
{
	unsigned long flags;
	char *value;

	spin_lock_irqsave(&nvram_lock, flags);
	value = _nvram_get(name);
	spin_unlock_irqrestore(&nvram_lock, flags);

	return value;
}

char *
nvram_get(const char *name)
{
	if (nvram_major >= 0)
		return real_nvram_get(name);
	else
		return early_nvram_get(name);
}

int
nvram_unset(const char *name)
{
	unsigned long flags;
	int ret;

	spin_lock_irqsave(&nvram_lock, flags);
	ret = _nvram_unset(name);
	spin_unlock_irqrestore(&nvram_lock, flags);

	return ret;
}

static void
erase_callback(struct erase_info *done)
{
	wait_queue_head_t *wait_q = (wait_queue_head_t *) done->priv;
	wake_up(wait_q);
}

int
nvram_commit(void)
{
	char *buf;
	size_t erasesize, len, magic_len;
	unsigned int i;
	int ret;
	struct nvram_header *header;
	unsigned long flags;
	u_int32_t offset;
	DECLARE_WAITQUEUE(wait, current);
	wait_queue_head_t wait_q;
	struct erase_info erase;
	u_int32_t magic_offset = 0; /* Offset for writing MAGIC # */

	if (!nvram_mtd) {
		printk("nvram_commit: NVRAM not found\n");
		return -ENODEV;
	}

	if (in_interrupt()) {
		printk("nvram_commit: not committing in interrupt\n");
		return -EINVAL;
	}

	/* Backup sector blocks to be erased */
	erasesize = ROUNDUP(NVRAM_SPACE, nvram_mtd->erasesize);
	if (!(buf = kmalloc(erasesize, GFP_KERNEL))) {
		printk("nvram_commit: out of memory\n");
		return -ENOMEM;
	}

	down(&nvram_sem);

	if ((i = erasesize - NVRAM_SPACE) > 0) {
		offset = nvram_mtd->size - erasesize;
		len = 0;
		ret = MTD_READ(nvram_mtd, offset, i, &len, buf);
		if (ret || len != i) {
			printk("nvram_commit: read error ret = %d, len = %d/%d\n", ret, len, i);
			ret = -EIO;
			goto done;
		}
		header = (struct nvram_header *)(buf + i);
		magic_offset = i + ((void *)&header->magic - (void *)header);
	} else {
		offset = nvram_mtd->size - NVRAM_SPACE;
		magic_offset = ((void *)&header->magic - (void *)header);
		header = (struct nvram_header *)buf;
	}

	/* clear the existing magic # to mark the NVRAM as unusable 
		 we can pull MAGIC bits low without erase	*/
	header->magic = NVRAM_CLEAR_MAGIC; /* All zeros magic */

	/* Unlock sector blocks (for Intel 28F320C3B flash) , 20060309 */
	if(nvram_mtd->unlock)
		nvram_mtd->unlock(nvram_mtd, offset, nvram_mtd->erasesize);

	ret = MTD_WRITE(nvram_mtd, offset + magic_offset, sizeof(header->magic), 
									&magic_len, (char *)&header->magic);
	if (ret || magic_len != sizeof(header->magic)) {
		printk("nvram_commit: clear MAGIC error\n");
		ret = -EIO;
		goto done;
	}

	header->magic = NVRAM_MAGIC; /* reset MAGIC before we regenerate the NVRAM,
																otherwise we'll have an incorrect CRC */
	/* Regenerate NVRAM */
	spin_lock_irqsave(&nvram_lock, flags);
	ret = _nvram_commit(header);
	spin_unlock_irqrestore(&nvram_lock, flags);
	if (ret)
		goto done;

	/* Erase sector blocks */
	init_waitqueue_head(&wait_q);
	for (; offset < nvram_mtd->size - NVRAM_SPACE + header->len; offset += nvram_mtd->erasesize) {
		erase.mtd = nvram_mtd;
		erase.addr = offset;
		erase.len = nvram_mtd->erasesize;
		erase.callback = erase_callback;
		erase.priv = (u_long) &wait_q;

		set_current_state(TASK_INTERRUPTIBLE);
		add_wait_queue(&wait_q, &wait);

		/* Unlock sector blocks */
		if (nvram_mtd->unlock)
			nvram_mtd->unlock(nvram_mtd, offset, nvram_mtd->erasesize);

		if ((ret = MTD_ERASE(nvram_mtd, &erase))) {
			set_current_state(TASK_RUNNING);
			remove_wait_queue(&wait_q, &wait);
			printk("nvram_commit: erase error\n");
			goto done;
		}

		/* Wait for erase to finish */
		schedule();
		remove_wait_queue(&wait_q, &wait);
	}

	/* Write partition up to end of data area */
	header->magic = NVRAM_INVALID_MAGIC; /* All ones magic */
	offset = nvram_mtd->size - erasesize;
	i = erasesize - NVRAM_SPACE + header->len;
	ret = MTD_WRITE(nvram_mtd, offset, i, &len, buf);
	if (ret || len != i) {
		printk("nvram_commit: write error\n");
		ret = -EIO;
		goto done;
	}

	/* Now mark the NVRAM in flash as "valid" by setting the correct
		 MAGIC # */
	header->magic = NVRAM_MAGIC;
	ret = MTD_WRITE(nvram_mtd, offset + magic_offset, sizeof(header->magic), 
									&magic_len, (char *)&header->magic);
	if (ret || magic_len != sizeof(header->magic)) {
		printk("nvram_commit: write MAGIC error\n");
		ret = -EIO;
		goto done;
	}

	/*
	 * Reading a few bytes back here will put the device
	 * back to the correct mode on certain flashes */
	offset = nvram_mtd->size - erasesize;
	ret = MTD_READ(nvram_mtd, offset, 4, &len, buf);

 done:
	up(&nvram_sem);
	kfree(buf);

	return ret;
}

int
nvram_getall(char *buf, int count)
{
	unsigned long flags;
	int ret;

	spin_lock_irqsave(&nvram_lock, flags);
	if (nvram_major >= 0)
		ret = _nvram_getall(buf, count);
	else
		ret = early_nvram_getall(buf, count);
	spin_unlock_irqrestore(&nvram_lock, flags);

	return ret;
}







/* User mode interface below */

static ssize_t
dev_nvram_read(struct file *file, char *buf, size_t count, loff_t *ppos)
{
	char tmp[100], *name = tmp, *value;
	ssize_t ret;
	unsigned long off;

	if (count > sizeof(tmp)) {
		if (!(name = kmalloc(count, GFP_KERNEL)))
			return -ENOMEM;
	}

	if (copy_from_user(name, buf, count)) {
		ret = -EFAULT;
		goto done;
	}

	if (*name == '\0') {
		/* Get all variables */
		ret = nvram_getall(name, count);
		if (ret == 0) {
			if (copy_to_user(buf, name, count)) {
				ret = -EFAULT;
				goto done;
			}
			ret = count;
		}
	} else {
		if (!(value = nvram_get(name))) {
			ret = 0;
			goto done;
		}

		/* Provide the offset into mmap() space */
		off = (unsigned long) value - (unsigned long) nvram_buf;

		if (put_user(off, (unsigned long *) buf)) {
			ret = -EFAULT;
			goto done;
		}

		ret = sizeof(unsigned long);
	}

	flush_cache_all();	
 
done:
	if (name != tmp)
		kfree(name);

	return ret;
}

static ssize_t
dev_nvram_write(struct file *file, const char *buf, size_t count, loff_t *ppos)
{
	char tmp[100], *name = tmp, *value;
	ssize_t ret;

	if (count > sizeof(tmp)) {
		if (!(name = kmalloc(count, GFP_KERNEL)))
			return -ENOMEM;
	}

	if (copy_from_user(name, buf, count)) {
		ret = -EFAULT;
		goto done;
	}

	value = name;
	name = strsep(&value, "=");
	if (value)
		ret = nvram_set(name, value) ? : count;
	else
		ret = nvram_unset(name) ? : count;

 done:
	if (name != tmp)
		kfree(name);

	return ret;
}	

static int
dev_nvram_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
	if (cmd != NVRAM_MAGIC)
		return -EINVAL;

	return nvram_commit();
}

static int
dev_nvram_mmap(struct file *file, struct vm_area_struct *vma)
{
	unsigned long offset = virt_to_phys(nvram_buf);

	if (remap_page_range(vma->vm_start, offset, vma->vm_end-vma->vm_start,
			     vma->vm_page_prot))
		return -EAGAIN;

	return 0;
}

static int
dev_nvram_open(struct inode *inode, struct file * file)
{
	MOD_INC_USE_COUNT;
	return 0;
}

static int
dev_nvram_release(struct inode *inode, struct file * file)
{
	MOD_DEC_USE_COUNT;
	return 0;
}

static struct file_operations dev_nvram_fops = {
	owner:		THIS_MODULE,
	open:		dev_nvram_open,
	release:	dev_nvram_release,
	read:		dev_nvram_read,
	write:		dev_nvram_write,
	ioctl:		dev_nvram_ioctl,
	mmap:		dev_nvram_mmap,
};

static void
dev_nvram_exit(void)
{
	int order = 0;
	struct page *page, *end;

	if (nvram_handle)
		devfs_unregister(nvram_handle);

	if (nvram_major >= 0)
		devfs_unregister_chrdev(nvram_major, "nvram");

	if (nvram_mtd)
		put_mtd_device(nvram_mtd);

	while ((PAGE_SIZE << order) < NVRAM_SPACE)
		order++;
	end = virt_to_page(nvram_buf + (PAGE_SIZE << order) - 1);
	for (page = virt_to_page(nvram_buf); page <= end; page++)
		mem_map_unreserve(page);

	_nvram_exit();
}

static int __init
dev_nvram_init(void)
{
	int order = 0, ret = 0;
	struct page *page, *end;
	unsigned int i;

	/* Allocate and reserve memory to mmap() */
	while ((PAGE_SIZE << order) < NVRAM_SPACE)
		order++;
	end = virt_to_page(nvram_buf + (PAGE_SIZE << order) - 1);
	for (page = virt_to_page(nvram_buf); page <= end; page++)
		mem_map_reserve(page);

#ifdef CONFIG_MTD
	/* Find associated MTD device */
	for (i = 0; i < MAX_MTD_DEVICES; i++) {
		nvram_mtd = get_mtd_device(NULL, i);
		if (nvram_mtd) {
			if (!strcmp(nvram_mtd->name, "nvram") &&
			    nvram_mtd->size >= NVRAM_SPACE)
				break;
			put_mtd_device(nvram_mtd);
		}
	}
	if (i >= MAX_MTD_DEVICES)
		nvram_mtd = NULL;
#endif

	/* Initialize hash table lock */
	spin_lock_init(&nvram_lock);

	/* Initialize commit semaphore */
	init_MUTEX(&nvram_sem);

	/* Register char device */
	if ((nvram_major = devfs_register_chrdev(0, "nvram", &dev_nvram_fops)) < 0) {
		ret = nvram_major;
		goto err;
	}

	/* Initialize hash table */
	_nvram_init(sbh);

	/* Create /dev/nvram handle */
	nvram_handle = devfs_register(NULL, "nvram", DEVFS_FL_NONE, nvram_major, 0,
				      S_IFCHR | S_IRUSR | S_IWUSR | S_IRGRP, &dev_nvram_fops, NULL);

	/* Set the SDRAM NCDL value into NVRAM if not already done */
	if (getintvar(NULL, "sdram_ncdl") == 0) {
		unsigned int ncdl;
		char buf[] = "0x00000000";

		if ((ncdl = sb_memc_get_ncdl(sbh))) {
			sprintf(buf, "0x%08x", ncdl);
			nvram_set("sdram_ncdl", buf);
			nvram_commit();
		}
	}

	return 0;

 err:
	dev_nvram_exit();
	return ret;
}

module_init(dev_nvram_init);
module_exit(dev_nvram_exit);
