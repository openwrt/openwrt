/* CF-mips driver
   This is a block driver for the direct (mmaped) interface to the CF-slot,
   found in Routerboard.com's RB532 board
   See SDK provided from routerboard.com.
   
   Module adapted By P.Christeas <p_christeas@yahoo.com>, 2005-6.
   Cleaned up and adapted to platform_device by Felix Fietkau <nbd@openwrt.org>

   This work is redistributed under the terms of the GNU General Public License.
*/

#include <linux/kernel.h>	/* printk() */
#include <linux/module.h>	/* module to be loadable */
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/pci.h>
#include <linux/ioport.h>	/* request_mem_region() */

#include <asm/gpio.h>
#include <asm/unaligned.h>		/* ioremap() */
#include <asm/io.h>		/* ioremap() */
#include <asm/rc32434/rb.h>

#include "ata.h"

#define REQUEST_MEM_REGION 0
#define DEBUG 1

#if DEBUG
#define DEBUGP printk
#else
#define DEBUGP(format, args...)
#endif

#define SECS	1000000		/* unit for wait_not_busy() is 1us */

unsigned cf_head = 0;
unsigned cf_cyl = 0;
unsigned cf_spt = 0;
unsigned cf_sectors = 0;
static unsigned cf_block_size = 1;
static void *baddr = 0;

#define DBUF32 ((volatile u32 *)((unsigned long)dev->baddr | ATA_DBUF_OFFSET))


static void cf_do_tasklet(unsigned long dev_l);


static inline void wareg(u8 val, unsigned reg, struct cf_mips_dev* dev)
{
	writeb(val, dev->baddr + ATA_REG_OFFSET + reg);
}

static inline u8 rareg(unsigned reg, struct cf_mips_dev* dev)
{
	return readb(dev->baddr + ATA_REG_OFFSET + reg);
}

static inline int cfrdy(struct cf_mips_dev *dev)
{
	return gpio_get_value(dev->pin);
}

static inline void prepare_cf_irq(struct cf_mips_dev *dev)
{
	rb500_gpio_set_int_level(1, dev->pin);	/* interrupt on cf ready (not busy) */
	rb500_gpio_set_int_status(0, dev->pin); 	/* clear interrupt status */
}

static inline int cf_present(struct cf_mips_dev* dev)
{
	/* TODO: read and configure CIS into memory mapped mode
	 * TODO:   parse CISTPL_CONFIG on CF+ cards to get base address (0x200)
	 * TODO:   maybe adjust power saving setting for Hitachi Microdrive
	 */
	int i;

	/* setup CFRDY GPIO as input */
	rb500_gpio_set_func(dev->pin, 0);
	gpio_direction_input(dev->pin);

	for (i = 0; i < 0x10; ++i) {
		if (rareg(i,dev) != 0xff)
			return 1;
	}
	return 0;
}

static inline int is_busy(struct cf_mips_dev *dev)
{
	return !cfrdy(dev);
}

static int wait_not_busy(int to_us, int wait_for_busy,struct cf_mips_dev *dev)
{
	int us_passed = 0;
	if (wait_for_busy && !is_busy(dev)) {
		/* busy must appear within 400ns,
		 * but it may dissapear before we see it
		 *  => must not wait for busy in a loop
		 */
		ndelay(400);
	}

	do {
		if (us_passed)
			udelay(1);	/* never reached in async mode */
		if (!is_busy(dev)) {
			if (us_passed > 1 * SECS) {
				printk(KERN_WARNING "cf-mips:   not busy ok (after %dus)"
				       ", status 0x%02x\n", us_passed, (unsigned) rareg(ATA_REG_ST,dev));
			}
			return CF_TRANS_OK;
		}
		if (us_passed == 1 * SECS) {
			printk(KERN_WARNING "cf-mips: wait not busy %dus..\n", to_us);
		}
		if (dev->async_mode) {
			dev->to_timer.expires = jiffies + (to_us * HZ / SECS);
			dev->irq_enable_time = jiffies;
			prepare_cf_irq(dev);
			if (is_busy(dev)) {
				add_timer(&dev->to_timer);
				enable_irq(dev->irq);
				return CF_TRANS_IN_PROGRESS;
			}
			continue;
		}
		++us_passed;
	} while (us_passed < to_us);

	printk(KERN_ERR "cf-mips:  wait not busy timeout (%dus)"
	       ", status 0x%02x, state %d\n",
	       to_us, (unsigned) rareg(ATA_REG_ST,dev), dev->tstate);
	return CF_TRANS_FAILED;
}

static irqreturn_t cf_irq_handler(int irq, void *dev_id)
{
	/* While tasklet has not disabled irq, irq will be retried all the time
	 * because of ILEVEL matching GPIO pin status => deadlock.
	 * To avoid this, we change ILEVEL to 0.
	 */
	struct cf_mips_dev *dev=dev_id;

	rb500_gpio_set_int_level(0, dev->pin);
	rb500_gpio_set_int_status(0, dev->pin);
	
	del_timer(&dev->to_timer);
	tasklet_schedule(&dev->tasklet);
	return IRQ_HANDLED;
}

static int do_reset(struct cf_mips_dev *dev)
{
	printk(KERN_INFO "cf-mips: resetting..\n");

	wareg(ATA_REG_DC_SRST, ATA_REG_DC,dev);
	udelay(1);		/* FIXME: how long should we wait here? */
	wareg(0, ATA_REG_DC,dev);

	return wait_not_busy(30 * SECS, 1,dev);
}

static int set_multiple(struct cf_mips_dev *dev)
{
	if (dev->block_size <= 1)
		return CF_TRANS_OK;

	wareg(dev->block_size, ATA_REG_SC,dev);
	wareg(ATA_REG_DH_BASE | ATA_REG_DH_LBA, ATA_REG_DH,dev);
	wareg(ATA_CMD_SET_MULTIPLE, ATA_REG_CMD,dev);

	return wait_not_busy(10 * SECS, 1,dev);
}

static int set_cmd(struct cf_mips_dev *dev)
{
	//DEBUGP(KERN_INFO "cf-mips: ata cmd 0x%02x\n", dev->tcmd);
	// sector_count should be <=24 bits..
	BUG_ON(dev->tsect_start>=0x10000000);
	// This way, it addresses 2^24 * 512 = 128G

	if (dev->tsector_count) {
		wareg(dev->tsector_count & 0xff, ATA_REG_SC,dev);
		wareg(dev->tsect_start & 0xff, ATA_REG_SN,dev);
		wareg((dev->tsect_start >> 8) & 0xff, ATA_REG_CL,dev);
		wareg((dev->tsect_start >> 16) & 0xff, ATA_REG_CH,dev);
	}
	wareg(((dev->tsect_start >> 24) & 0x0f) | ATA_REG_DH_BASE | ATA_REG_DH_LBA,
	      ATA_REG_DH,dev);	/* select drive on all commands */
	wareg(dev->tcmd, ATA_REG_CMD,dev);
	return wait_not_busy(10 * SECS, 1,dev);
}

static int do_trans(struct cf_mips_dev *dev)
{
	int res;
	unsigned st;
	int transfered;
	
	//printk("do_trans: %d sectors left\n",dev->tsectors_left);
	while (dev->tsectors_left) {
		transfered = 0;

		st = rareg(ATA_REG_ST,dev);
		if (!(st & ATA_REG_ST_DRQ)) {
			printk(KERN_ERR "cf-mips: do_trans without DRQ (status 0x%x)!\n", st);
			if (st & ATA_REG_ST_ERR) {
				int errId = rareg(ATA_REG_ERR,dev);
				printk(KERN_ERR "cf-mips: %s error, status 0x%x, errid 0x%x\n",
				       (dev->tread ? "read" : "write"), st, errId);
			}
			return CF_TRANS_FAILED;
		}
		do { /* Fill/read the buffer one block */
			u32 *qbuf, *qend;
			qbuf = (u32 *)dev->tbuf;
			qend = qbuf + CF_SECT_SIZE / sizeof(u32);
			if (dev->tread) {
			    while (qbuf!=qend)
				put_unaligned(*DBUF32,qbuf++);
				//*(qbuf++) = *DBUF32;
			}
			else {
			    while(qbuf!=qend)
				*DBUF32 = get_unaligned(qbuf++);
			}

			dev->tsectors_left--;
			dev->tbuf += CF_SECT_SIZE;
			dev->tbuf_size -= CF_SECT_SIZE;
			transfered++;
		} while (transfered != dev->block_size && dev->tsectors_left > 0);

		res = wait_not_busy(10 * SECS, 1,dev);
		if (res != CF_TRANS_OK)
			return res;
	};

	st = rareg(ATA_REG_ST,dev);
	if (st & (ATA_REG_ST_DRQ | ATA_REG_ST_DWF | ATA_REG_ST_ERR)) {
		if (st & ATA_REG_ST_DRQ) {
			printk(KERN_ERR "cf-mips: DRQ after all %d sectors are %s"
			       ", status 0x%x\n", dev->tsector_count, (dev->tread ? "read" : "written"), st);
		} else if (st & ATA_REG_ST_DWF) {
			printk(KERN_ERR "cf-mips: write fault, status 0x%x\n", st);
		} else {
			int errId = rareg(ATA_REG_ERR,dev);
			printk(KERN_ERR "cf-mips: %s error, status 0x%x, errid 0x%x\n",
			       (dev->tread ? "read" : "write"), st, errId);
		}
		return CF_TRANS_FAILED;
	}
	return CF_TRANS_OK;
}

static int cf_do_state(struct cf_mips_dev *dev)
{
	int res;
	switch (dev->tstate) {	/* fall through everywhere */
	case TS_IDLE:
		dev->tstate = TS_READY;
		if (is_busy(dev)) {
			dev->tstate = TS_AFTER_RESET;
			res = do_reset(dev);
			if (res != CF_TRANS_OK)
				break;
		}
	case TS_AFTER_RESET:
		if (dev->tstate == TS_AFTER_RESET) {
			dev->tstate = TS_READY;
			res = set_multiple(dev);
			if (res != CF_TRANS_OK)
				break;
		}
	case TS_READY:
		dev->tstate = TS_CMD;
		res = set_cmd(dev);
		if (res != CF_TRANS_OK)
			break;;
	case TS_CMD:
		dev->tstate = TS_TRANS;
	case TS_TRANS:
		res = do_trans(dev);
		break;
	default:
		printk(KERN_ERR "cf-mips: BUG: unknown tstate %d\n", dev->tstate);
		return CF_TRANS_FAILED;
	}
	if (res != CF_TRANS_IN_PROGRESS)
		dev->tstate = TS_IDLE;
	return res;
}

static void cf_do_tasklet(unsigned long dev_l)
{
	struct cf_mips_dev* dev=(struct cf_mips_dev*) dev_l;
	int res;

	disable_irq(dev->irq);

	if (dev->tstate == TS_IDLE)
		return;		/* can happen when irq is first registered */

#if 0
	DEBUGP(KERN_WARNING "cf-mips:   not busy ok (tasklet)  status 0x%02x\n",
	       (unsigned) rareg(ATA_REG_ST,dev));
#endif

	res = cf_do_state(dev);
	if (res == CF_TRANS_IN_PROGRESS)
		return;
	cf_async_trans_done(dev,res);
}

static void cf_async_timeout(unsigned long dev_l)
{
	struct cf_mips_dev* dev=(struct cf_mips_dev*) dev_l;
	disable_irq(dev->irq);
	/* Perhaps send abort to the device? */
	printk(KERN_ERR "cf-mips:  wait not busy timeout (%lus)"
	       ", status 0x%02x, state %d\n",
	       jiffies - dev->irq_enable_time, (unsigned) rareg(ATA_REG_ST,dev), dev->tstate);
	dev->tstate = TS_IDLE;
	cf_async_trans_done(dev,CF_TRANS_FAILED);
}

int cf_do_transfer(struct cf_mips_dev* dev,sector_t sector, unsigned long nsect, 
	char* buffer, int is_write)
{
	BUG_ON(dev->tstate!=TS_IDLE);
	if (nsect > ATA_MAX_SECT_PER_CMD) {
		printk(KERN_WARNING "cf-mips: sector count %lu out of range\n",nsect);
		return CF_TRANS_FAILED;
	}
	if (sector + nsect > dev->sectors) {
		printk(KERN_WARNING "cf-mips: sector %lu out of range\n",sector);
		return CF_TRANS_FAILED;
	}
	dev->tbuf = buffer;
	dev->tbuf_size = nsect*512;
	dev->tsect_start = sector;
	dev->tsector_count = nsect;
	dev->tsectors_left = dev->tsector_count;
	dev->tread = (is_write)?0:1;
	
	dev->tcmd = (dev->block_size == 1 ?
		(is_write ? ATA_CMD_WRITE_SECTORS : ATA_CMD_READ_SECTORS) :
		(is_write ? ATA_CMD_WRITE_MULTIPLE : ATA_CMD_READ_MULTIPLE));

	return cf_do_state(dev);
}

static int do_identify(struct cf_mips_dev *dev)
{
	u16 sbuf[CF_SECT_SIZE >> 1];
 	int res;
	char tstr[17]; //serial
	char tmp;
	int i;
	BUG_ON(dev->tstate!=TS_IDLE);
	dev->tbuf = (char *) sbuf;
	dev->tbuf_size = CF_SECT_SIZE;
	dev->tsect_start = 0;
	dev->tsector_count = 0;
	dev->tsectors_left = 1;
	dev->tread = 1;
	dev->tcmd = ATA_CMD_IDENTIFY_DRIVE;

	DEBUGP(KERN_INFO "cf-mips: identify drive..\n");
	res = cf_do_state(dev);
	if (res == CF_TRANS_IN_PROGRESS) {
		printk(KERN_ERR "cf-mips: BUG: async identify cmd\n");
		return CF_TRANS_FAILED;
	}
	if (res != CF_TRANS_OK)
		return 0;

	dev->head = sbuf[3];
	dev->cyl = sbuf[1];
	dev->spt = sbuf[6];
	dev->sectors = ((unsigned long) sbuf[7] << 16) | sbuf[8];
	dev->dtype=sbuf[0];
	memcpy(tstr, &sbuf[12], 16);
	tstr[16] = '\0';

	/* Byte-swap the serial number */
	for (i = 0; i<8; i++) {
		tmp = tstr[i * 2];
		tstr[i * 2] = tstr[i * 2 +1];
		tstr[i * 2 + 1] = tmp;
	}

	printk(KERN_INFO "cf-mips: %s detected, C/H/S=%d/%d/%d sectors=%u (%uMB) Serial=%s\n",
	       (sbuf[0] == 0x848A ? "CF card" : "ATA drive"), dev->cyl, dev->head,
	       dev->spt, dev->sectors, dev->sectors >> 11, tstr);
	return 1;
}

static void init_multiple(struct cf_mips_dev * dev)
{
	int res;
	DEBUGP(KERN_INFO "cf-mips: detecting block size\n");

	dev->block_size = 128;	/* max block size = 128 sectors (64KB) */
	do {
		wareg(dev->block_size, ATA_REG_SC,dev);
		wareg(ATA_REG_DH_BASE | ATA_REG_DH_LBA, ATA_REG_DH,dev);
		wareg(ATA_CMD_SET_MULTIPLE, ATA_REG_CMD,dev);

		res = wait_not_busy(10 * SECS, 1,dev);
		if (res != CF_TRANS_OK) {
			printk(KERN_ERR "cf-mips: failed to detect block size: busy!\n");
			dev->block_size = 1;
			return;
		}
		if ((rareg(ATA_REG_ST,dev) & ATA_REG_ST_ERR) == 0)
			break;
		dev->block_size /= 2;
	} while (dev->block_size > 1);

	printk(KERN_INFO "cf-mips: multiple sectors = %d\n", dev->block_size);
}

int cf_init(struct cf_mips_dev *dev)
{
	tasklet_init(&dev->tasklet,cf_do_tasklet,(unsigned long)dev);
	dev->baddr = ioremap_nocache((unsigned long)dev->base, CFDEV_BUF_SIZE);
	if (!dev->baddr) {
		printk(KERN_ERR "cf-mips: cf_init: ioremap for (%lx,%x) failed\n",
		       (unsigned long) dev->base, CFDEV_BUF_SIZE);
		return -EBUSY;
	}

	if (!cf_present(dev)) {
		printk(KERN_WARNING "cf-mips: cf card not present\n");
		iounmap(dev->baddr);
		return -ENODEV;
	}

	if (do_reset(dev) != CF_TRANS_OK) {
		printk(KERN_ERR "cf-mips: cf reset failed\n");
		iounmap(dev->baddr);
		return -EBUSY;
	}

	if (!do_identify(dev)) {
		printk(KERN_ERR "cf-mips: cf identify failed\n");
		iounmap(dev->baddr);
		return -EBUSY;
	}

/*	set_apm_level(ATA_APM_WITH_STANDBY); */
	init_multiple(dev);

	init_timer(&dev->to_timer);
	dev->to_timer.function = cf_async_timeout;
	dev->to_timer.data = (unsigned long)dev;

	prepare_cf_irq(dev);
	if (request_irq(dev->irq, cf_irq_handler, 0, "CF Mips", dev)) {
		printk(KERN_ERR "cf-mips: failed to get irq\n");
		iounmap(dev->baddr);
		return -EBUSY;
	}
	/* Disable below would be odd, because request will enable, and the tasklet
	  will disable it itself */
	//disable_irq(dev->irq);
	
	dev->async_mode = 1;

	return 0;
}

void cf_cleanup(struct cf_mips_dev *dev)
{
	iounmap(dev->baddr);
	free_irq(dev->irq, NULL);
#if REQUEST_MEM_REGION
	release_mem_region((unsigned long)dev->base, CFDEV_BUF_SIZE);
#endif
}


/*eof*/
