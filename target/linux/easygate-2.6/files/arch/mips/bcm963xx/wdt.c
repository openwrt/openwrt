/*
 * Watchdog driver for the BCM963xx devices
 * 
 * Copyright (C) 2007 OpenWrt.org
 *			Florian Fainelli <florian@openwrt.org>
 * 
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/notifier.h>
#include <linux/watchdog.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/completion.h>
#include <linux/ioport.h>

typedef struct bcm963xx_timer {
	unsigned short unused0;
	unsigned char  timer_mask;
#define TIMER0EN        0x01
#define TIMER1EN        0x02
#define TIMER2EN        0x04
  	unsigned char  timer_ints;
#define TIMER0          0x01
#define TIMER1          0x02
#define TIMER2          0x04
#define WATCHDOG        0x08
 	unsigned long	timer_ctl0;
  	unsigned long	timer_ctl1;
  	unsigned long	timer_ctl2;
#define TIMERENABLE     0x80000000
#define RSTCNTCLR       0x40000000      
  	unsigned long	timer_cnt0;
  	unsigned long	timer_cnt1;
  	unsigned long	timer_cnt2;
  	unsigned long	wdt_def_count;

  	/* Write 0xff00 0x00ff to Start timer
   	* Write 0xee00 0x00ee to Stop and re-load default count
   	* Read from this register returns current watch dog count
   	*/
  	unsigned long	wdt_ctl;

  	/* Number of 40-MHz ticks for WD Reset pulse to last */
  	unsigned long	wdt_rst_count;
} bcm963xx_timer;

static struct bcm963xx_wdt_device {
	struct completion stop;
	volatile int running;
	struct timer_list timer;
	volatile int queue;
	int default_ticks;
	unsigned long inuse;
} bcm963xx_wdt_device;

static int ticks = 1000;

#define WDT_BASE	0xfffe0200
#define WDT 		((volatile bcm963xx_timer * const) WDT_BASE)

#define BCM963XX_INTERVAL        (HZ/10+1)

static void bcm963xx_wdt_trigger(unsigned long unused)
{
	if (bcm963xx_wdt_device.running)
		ticks--;

	/* Load the default ticking value into the reset counter register */	
	WDT->wdt_rst_count = bcm963xx_wdt_device.default_ticks;
	
	if (bcm963xx_wdt_device.queue && ticks) {
		bcm963xx_wdt_device.timer.expires = jiffies + BCM963XX_INTERVAL;
		add_timer(&bcm963xx_wdt_device.timer);
	}
	else {
		complete(&bcm963xx_wdt_device.stop);
	}
}

static void bcm963xx_wdt_reset(void)
{
	ticks = bcm963xx_wdt_device.default_ticks;
}

static void bcm963xx_wdt_start(void)
{
	if (!bcm963xx_wdt_device.queue) {
		bcm963xx_wdt_device.queue;
		/* Enable the watchdog by writing 0xff00 ,then 0x00ff to the control register */
		WDT->wdt_ctl = 0xff00;
		WDT->wdt_ctl = 0x00ff;
		bcm963xx_wdt_device.timer.expires = jiffies + BCM963XX_INTERVAL;
		add_timer(&bcm963xx_wdt_device.timer);
	}
	bcm963xx_wdt_device.running++;
}

static int bcm963xx_wdt_stop(void)
{
	if (bcm963xx_wdt_device.running)
		bcm963xx_wdt_device.running = 0;
	
	ticks = bcm963xx_wdt_device.default_ticks;

	/* Stop the watchdog by writing 0xee00 then 0x00ee to the control register */
	WDT->wdt_ctl = 0xee00;
	WDT->wdt_ctl = 0x00ee;

	return -EIO;
}

static int bcm963xx_wdt_open(struct inode *inode, struct file *file)
{
	if (test_and_set_bit(0, &bcm963xx_wdt_device.inuse))
		return -EBUSY;
	return nonseekable_open(inode, file);
}

static int bcm963xx_wdt_release(struct inode *inode, struct file *file)
{
	clear_bit(0, &bcm963xx_wdt_device.inuse);
	return 0;
}

static int bcm963xx_wdt_ioctl(struct inode *inode, struct file *file,
				unsigned int cmd, unsigned long arg)
{
	void __user *argp = (void __user *)arg;
	unsigned int value;

	static struct watchdog_info ident = {
		.options = WDIOF_CARDRESET,
		.identity = "BCM963xx WDT",
	};

	switch (cmd) {
		case WDIOC_KEEPALIVE:
			bcm963xx_wdt_reset();
			break;
		case WDIOC_GETSTATUS:
			/* Reading from the control register will return the current value */
			value = WDT->wdt_ctl;
			if ( copy_to_user(argp, &value, sizeof(int)) )
				return -EFAULT;
			break;
		case WDIOC_GETSUPPORT:
			if ( copy_to_user(argp, &ident, sizeof(ident)) )
				return -EFAULT;
			break;
		case WDIOC_SETOPTIONS:
			if ( copy_from_user(&value, argp, sizeof(int)) )
				return -EFAULT;
			switch(value) {
				case WDIOS_ENABLECARD:
					bcm963xx_wdt_start();
					break;
				case WDIOS_DISABLECARD:
					bcm963xx_wdt_stop();
					break;
				default:
					return -EINVAL;
			}
			break;
		default:
			return -ENOTTY;
	}
	return 0;		
}

static int bcm963xx_wdt_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	if (!count)
		return -EIO;
	bcm963xx_wdt_reset();
	return count;
}

static const struct file_operations bcm963xx_wdt_fops = {
	.owner 		= THIS_MODULE,
	.llseek		= no_llseek,
	.write		= bcm963xx_wdt_write,
	.ioctl		= bcm963xx_wdt_ioctl,
	.open		= bcm963xx_wdt_open,
	.release 	= bcm963xx_wdt_release,
};	

static struct miscdevice bcm963xx_wdt_miscdev = {
	.minor 	= WATCHDOG_MINOR,
	.name 	= "watchdog",
	.fops	= &bcm963xx_wdt_fops,
};

static void __exit bcm963xx_wdt_exit(void)
{
	if (bcm963xx_wdt_device.queue ){
		bcm963xx_wdt_device.queue = 0;
		wait_for_completion(&bcm963xx_wdt_device.stop);
	}	
	misc_deregister(&bcm963xx_wdt_miscdev);
}

static int __init bcm963xx_wdt_init(void)
{
	int ret = 0;
	
	printk("Broadcom BCM963xx Watchdog timer\n");

	ret = misc_register(&bcm963xx_wdt_miscdev);
	if (ret) {
		printk(KERN_CRIT "Cannot register miscdev on minor=%d (err=%d)\n", WATCHDOG_MINOR, ret);
		return ret;
	}
	init_completion(&bcm963xx_wdt_device.stop);
	bcm963xx_wdt_device.queue = 0;
	
	clear_bit(0, &bcm963xx_wdt_device.inuse);
	
	init_timer(&bcm963xx_wdt_device.timer);
	bcm963xx_wdt_device.timer.function = bcm963xx_wdt_trigger;
	bcm963xx_wdt_device.timer.data = 0;

	bcm963xx_wdt_device.default_ticks = ticks;	
	return ret;
}

	
module_init(bcm963xx_wdt_init);
module_exit(bcm963xx_wdt_exit);

MODULE_AUTHOR("Florian Fainelli <florian@openwrt.org>");
MODULE_DESCRIPTION("Broadcom BCM963xx Watchdog driver");
MODULE_LICENSE("GPL");
