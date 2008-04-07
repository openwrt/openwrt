/* CF-mips driver
   This is a block driver for the direct (mmaped) interface to the CF-slot,
   found in Routerboard.com's RB532 board
   See SDK provided from routerboard.com.
   
   Module adapted By P.Christeas <p_christeas@yahoo.com>, 2005-6.
   Cleaned up and adapted to platform_device by Felix Fietkau <nbd@openwrt.org>

   This work is redistributed under the terms of the GNU General Public License.
*/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/time.h>
#include <linux/wait.h>
#include <linux/fs.h>
#include <linux/genhd.h>
#include <linux/blkdev.h>
#include <linux/blkpg.h>
#include <linux/hdreg.h>
#include <linux/platform_device.h>

#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/gpio.h>

#include <asm/rc32434/rb.h>

#ifdef DEBUG
#define DEBUGP printk
#define DLEVEL 1
#else
#define DEBUGP(format, args...)
#define DLEVEL 0
#endif

#define CF_MIPS_MAJOR 13
#define MAJOR_NR	CF_MIPS_MAJOR
#define CF_MAX_PART	16		/* max 15 partitions */

#include "ata.h"

//extern struct block_device_operations cf_bdops;

// static struct hd_struct cf_parts[CF_MAX_PART];
// static int cf_part_sizes[CF_MAX_PART];
// static int cf_hsect_sizes[CF_MAX_PART];
// static int cf_max_sectors[CF_MAX_PART];
// static int cf_blksize_sizes[CF_MAX_PART];

// static spinlock_t lock = SPIN_LOCK_UNLOCKED;

// volatile int cf_busy = 0;

static struct request *active_req = NULL;

static int cf_open (struct inode *, struct file *);
static int cf_release (struct inode *, struct file *);
static int cf_ioctl (struct inode *, struct file *, unsigned, unsigned long);

static void cf_request(request_queue_t * q);
static int cf_transfer(const struct request *req);

/*long (*unlocked_ioctl) (struct file *, unsigned, unsigned long);
long (*compat_ioctl) (struct file *, unsigned, unsigned long);*/
// int (*direct_access) (struct block_device *, sector_t, unsigned long *);
// int (*media_changed) (struct gendisk *);
// int (*revalidate_disk) (struct gendisk *);

static struct block_device_operations cf_bdops = {
      .owner = THIS_MODULE,
      .open = cf_open,
      .release = cf_release,
      .ioctl = cf_ioctl,
      .media_changed = NULL,
      .unlocked_ioctl = NULL,
      .revalidate_disk = NULL,
      .compat_ioctl = NULL,
      .direct_access = NULL
};


int cf_mips_probe(struct platform_device *pdev)
{
	struct gendisk* cf_gendisk=NULL;
	struct cf_device *cdev = (struct cf_device *) pdev->dev.platform_data;
	struct cf_mips_dev *dev;
	struct resource *r;
	int reg_result;

	reg_result = register_blkdev(MAJOR_NR, "cf-mips");
	if (reg_result < 0) {
		printk(KERN_WARNING "cf-mips: can't get major %d\n", MAJOR_NR);
		return reg_result;
	}

	dev = (struct cf_mips_dev *)kmalloc(sizeof(struct cf_mips_dev),GFP_KERNEL);
	if (!dev)
		goto out_err;
	memset(dev, 0, sizeof(struct cf_mips_dev));
	cdev->dev = dev;
	
	dev->pin = cdev->gpio_pin;
	dev->irq = platform_get_irq_byname(pdev, "cf_irq");
	r = platform_get_resource_byname(pdev, IORESOURCE_MEM, "cf_membase");
	dev->base = (void *) r->start;
	
	if (cf_init(dev)) goto out_err;
	printk("init done");
	
	spin_lock_init(&dev->lock);
	dev->queue = blk_init_queue(cf_request,&dev->lock);
	if (!dev->queue){
		printk(KERN_ERR "cf-mips: no mem for queue\n");
		goto out_err;
	}
	blk_queue_max_sectors(dev->queue,ATA_MAX_SECT_PER_CMD);

	/* For memory devices, it is always better to avoid crossing segments
	inside the same request. */
/*	if (dev->dtype==0x848A){
		printk(KERN_INFO "Setting boundary for cf to 0x%x",(dev->block_size*512)-1);
		blk_queue_segment_boundary(dev->queue, (dev->block_size*512)-1);
	}*/

	dev->gd = alloc_disk(CF_MAX_PART);
	cf_gendisk = dev->gd;
	cdev->gd = dev->gd;
	if (!cf_gendisk) goto out_err; /* Last of these goto's */
	
	cf_gendisk->major = MAJOR_NR;
	cf_gendisk->first_minor = 0;
	cf_gendisk->queue=dev->queue;
	BUG_ON(cf_gendisk->minors != CF_MAX_PART);
	strcpy(cf_gendisk->disk_name,"cfa");
	cf_gendisk->fops = &cf_bdops;
	cf_gendisk->flags = 0 ; /* is not yet GENHD_FL_REMOVABLE */
	cf_gendisk->private_data=dev;
	
	set_capacity(cf_gendisk,dev->sectors * CF_KERNEL_MUL);
	
	/* Let the disk go live */
	add_disk(cf_gendisk);
#if 0
	result = cf_init();
	
	/* default cfg for all partitions */
	memset(cf_parts, 0, sizeof (cf_parts[0]) * CF_MAX_PART);
	memset(cf_part_sizes, 0, sizeof (cf_part_sizes[0]) * CF_MAX_PART);
	for (i = 0; i < CF_MAX_PART; ++i) {
		cf_hsect_sizes[i] = CF_SECT_SIZE;
		cf_max_sectors[i] = ATA_MAX_SECT_PER_CMD;
		cf_blksize_sizes[i] = BLOCK_SIZE;
	}

	/* setup info for whole disk (partition 0) */
	cf_part_sizes[0] = cf_sectors / 2;
	cf_parts[0].nr_sects = cf_sectors;

	blk_size[MAJOR_NR] = cf_part_sizes;
	blksize_size[MAJOR_NR] = cf_blksize_sizes;
	max_sectors[MAJOR_NR] = cf_max_sectors;
	hardsect_size[MAJOR_NR] = cf_hsect_sizes;
	read_ahead[MAJOR_NR] = 8;	/* (4kB) */

	blk_init_queue(BLK_DEFAULT_QUEUE(MAJOR_NR), DEVICE_REQUEST);

	add_gendisk(&cf_gendisk);
#endif
// 	printk(KERN_INFO "cf-mips partition check: \n");
// 	register_disk(cf_gendisk, MKDEV(MAJOR_NR, 0), CF_MAX_PART,
// 		      &cf_bdops, dev->sectors);
	return 0;

out_err:
	if (dev->queue){
		blk_cleanup_queue(dev->queue);
	}
	if (reg_result) {
		unregister_blkdev(MAJOR_NR, "cf-mips");
		return reg_result;
	}
	if (dev){
		cf_cleanup(dev);
		kfree(dev);
	}
	return 1;
}

static int
cf_mips_remove(struct platform_device *pdev)
{
	struct cf_device *cdev = (struct cf_device *) pdev->dev.platform_data;
	struct cf_mips_dev *dev = (struct cf_mips_dev *) cdev->dev;
	
	unregister_blkdev(MAJOR_NR, "cf-mips");
	blk_cleanup_queue(dev->queue);

	del_gendisk(dev->gd);
	cf_cleanup(dev);
	return 0;
}


static struct platform_driver cf_driver = {
	.driver.name = "rb500-cf",
	.probe = cf_mips_probe,
	.remove = cf_mips_remove,
};

static int __init cf_mips_init(void)
{
	printk(KERN_INFO "cf-mips module loaded\n");
	return platform_driver_register(&cf_driver);
}

static void cf_mips_cleanup(void)
{
	platform_driver_unregister(&cf_driver);
	printk(KERN_INFO "cf-mips module removed\n");
}

module_init(cf_mips_init);
module_exit(cf_mips_cleanup);

MODULE_LICENSE("GPL");
MODULE_ALIAS_BLOCKDEV_MAJOR(CF_MIPS_MAJOR);


static int cf_open(struct inode *inode, struct file *filp)
{
	struct cf_mips_dev  *dev=inode->i_bdev->bd_disk->private_data;
	int minor = MINOR(inode->i_rdev);
	
	if (minor >= CF_MAX_PART)
		return -ENODEV;
	//DEBUGP(KERN_INFO "cf-mips module opened, minor %d\n", minor);
	spin_lock(&dev->lock);
	dev->users++;
	spin_unlock(&dev->lock);
	filp->private_data=dev;
	
	/* dirty workaround to set CFRDY GPIO as an input when some other
	   program sets it as an output */
	gpio_set_value(dev->pin, 0);
	return 0;		/* success */
}

static int cf_release(struct inode *inode, struct file *filp)
{
	int minor = MINOR(inode->i_rdev);
	struct cf_mips_dev  *dev=inode->i_bdev->bd_disk->private_data;
	spin_lock(&dev->lock);
	dev->users--;
	spin_unlock(&dev->lock);
	return 0;
}

static int cf_ioctl(struct inode *inode, struct file *filp,
	 unsigned int cmd, unsigned long arg)
{
	unsigned minor = MINOR(inode->i_rdev);
	struct cf_mips_dev  *dev=inode->i_bdev->bd_disk->private_data;

	DEBUGP(KERN_INFO "cf_ioctl cmd %u\n", cmd);
	switch (cmd) {
	case BLKRRPART:	/* re-read partition table */
		if (!capable(CAP_SYS_ADMIN))
			return -EACCES;
		printk(KERN_INFO "cf-mips partition check: \n");
		register_disk(dev->gd);
		return 0;

	case HDIO_GETGEO:
		{
			struct hd_geometry geo;
			geo.cylinders = dev->cyl;
			geo.heads = dev->head;
			geo.sectors = dev->spt;
			geo.start = (*dev->gd->part)[minor].start_sect;
			if (copy_to_user((void *) arg, &geo, sizeof (geo)))
				return -EFAULT;
		}
		return 0;
	}

	return -EINVAL;		/* unknown command */
}

static void cf_request(request_queue_t * q)
{
	struct cf_mips_dev* dev;
	
	struct request * req;
	int status;

	/* We could have q->queuedata = dev , but haven't yet. */
	if (active_req)
		return;

	while ((req=elv_next_request(q))!=NULL){
		dev=req->rq_disk->private_data;
		status=cf_transfer(req);
		if (status==CF_TRANS_IN_PROGRESS){
			active_req=req;
			return;
		}
		end_request(req,status);
	}
}

static int cf_transfer(const struct request *req)
{
	struct cf_mips_dev* dev=req->rq_disk->private_data;

	if (!blk_fs_request(req)){	
		if (printk_ratelimit())
			printk(KERN_WARNING "cf-mips: skipping non-fs request 0x%x\n",req->cmd[0]);
		return CF_TRANS_FAILED;
	}
	
	return cf_do_transfer(dev,req->sector,req->current_nr_sectors,req->buffer,rq_data_dir(req));
}

void cf_async_trans_done(struct cf_mips_dev * dev,int result)
{
	struct request *req;
	
	spin_lock(&dev->lock);
	req=active_req;
	active_req=NULL;
	end_request(req,result);
	spin_unlock(&dev->lock);

	spin_lock(&dev->lock);
	cf_request(dev->queue);
	spin_unlock(&dev->lock);
}

