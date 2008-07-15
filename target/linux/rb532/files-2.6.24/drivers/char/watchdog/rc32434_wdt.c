/*
 *	RC32434_WDT 0.01: IDT Interprise 79RC32434 watchdog driver 
 *	Copyright (c) Ondrej Zajicek <santiago@crfreenet.org>, 2006
 *
 *	based on
 *
 *	SoftDog	0.05:	A Software Watchdog Device
 *
 *	(c) Copyright 1996 Alan Cox <alan@redhat.com>, All Rights Reserved.
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version
 *	2 of the License, or (at your option) any later version.
 *	
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/miscdevice.h>
#include <linux/watchdog.h>
#include <linux/reboot.h>
#include <linux/smp_lock.h>
#include <linux/init.h>
#include <asm/bootinfo.h>
#include <asm/time.h>
#include <asm/uaccess.h>
#include <asm/rc32434/integ.h>

#define DEFAULT_TIMEOUT	15		/* (secs) Default is 15 seconds */
#define MAX_TIMEOUT	20		
/*
 * (secs) Max is 20 seconds 
 * (max frequency of counter is ~200 MHz, counter is 32-bit unsigned int) 
 */

#define NAME "rc32434_wdt"
#define VERSION "0.1"

static INTEG_t rc_wdt = (INTEG_t) INTEG_VirtualAddress;                                                                     

static int expect_close = 0;
static int access = 0;
static int timeout = 0;

static int nowayout = WATCHDOG_NOWAYOUT;
module_param(nowayout, int, 0);
MODULE_PARM_DESC(nowayout, "Watchdog cannot be stopped once started (default=" __MODULE_STRING(WATCHDOG_NOWAYOUT) ")");
MODULE_LICENSE("GPL");


static inline void start_wdt(void)
{
	rc_wdt -> wtcount = 0;
	rc_wdt -> errcs |= ERRCS_wre_m;
	rc_wdt -> wtc   |= WTC_en_m;
}

static inline void stop_wdt(void)
{
	rc_wdt -> wtc   &= ~WTC_en_m;
	rc_wdt -> errcs &= ~ERRCS_wre_m;
}

static inline void set_wdt(int new_timeout)
{
	u32 cmp = new_timeout * mips_hpt_frequency;
	u32 state;
	
	timeout = new_timeout;
	/*
	 * store and disable WTC
	 */
	state = rc_wdt -> wtc & WTC_en_m;
	rc_wdt -> wtc   &= ~WTC_en_m;

	rc_wdt -> wtcount = 0;
	rc_wdt -> wtcompare = cmp;

	/*
	 * restore WTC
	 */	
	rc_wdt -> wtc |= state;
}

static inline void update_wdt(void)
{
	rc_wdt -> wtcount = 0;
}

/*
 *	Allow only one person to hold it open
 */
 
static int wdt_open(struct inode *inode, struct file *file)
{
	if (access)
		return -EBUSY;
	if (nowayout) {
		__module_get(THIS_MODULE);
	}
	/*
	 *	Activate timer
	 */
	start_wdt();
	printk(KERN_INFO NAME ": enabling watchdog timer\n");
	access = 1;
	return 0;
}

static int wdt_release(struct inode *inode, struct file *file)
{
	/*
	 *	Shut off the timer.
	 * 	Lock it in if it's a module and we set nowayout
	 */
	if (expect_close && nowayout == 0) {
		stop_wdt ();
		printk(KERN_INFO NAME ": disabling watchdog timer\n");
		module_put(THIS_MODULE);
	} else {
		printk (KERN_CRIT NAME ": device closed unexpectedly.  WDT will not stop!\n");
	}
	access = 0;
	return 0;
}

static ssize_t wdt_write(struct file *file, const char *data, size_t len, loff_t *ppos)
{
	/*
	 *	Refresh the timer.
	 */
	if (len) {
		if (!nowayout) {
			size_t i;

			/* In case it was set long ago */
			expect_close = 0;

			for (i = 0; i != len; i++) {
				char c;
				if (get_user(c, data + i))
					return -EFAULT;
				if (c == 'V')
					expect_close = 1;
			}
		}
		update_wdt ();
		return len;
	}
	return 0;
}

static int wdt_ioctl(struct inode *inode, struct file *file,
	unsigned int cmd, unsigned long arg)
{
	int new_timeout;
	static struct watchdog_info ident = {
		.options =		WDIOF_SETTIMEOUT |
					WDIOF_KEEPALIVEPING |
					WDIOF_MAGICCLOSE,
		.firmware_version =	0,
		.identity =		"RC32434_WDT Watchdog",
	};
	switch (cmd) {
		default:
			return -ENOTTY;
		case WDIOC_GETSUPPORT:
			if(copy_to_user((struct watchdog_info *)arg, &ident, sizeof(ident)))
				return -EFAULT;
			return 0;
		case WDIOC_GETSTATUS:
		case WDIOC_GETBOOTSTATUS:
			return put_user(0,(int *)arg);
		case WDIOC_KEEPALIVE:
			update_wdt();
			return 0;
		case WDIOC_SETTIMEOUT:
			if (get_user(new_timeout, (int *)arg))
				return -EFAULT;
			if (new_timeout < 1)
				return -EINVAL;
			if (new_timeout > MAX_TIMEOUT)
				return -EINVAL;
			set_wdt(new_timeout);
			/* Fall */
		case WDIOC_GETTIMEOUT:
			return put_user(timeout, (int *)arg);
	}
}

static struct file_operations wdt_fops = {
	owner:		THIS_MODULE,
	llseek:		no_llseek,
	write:		wdt_write,
	ioctl:		wdt_ioctl,
	open:		wdt_open,
	release:	wdt_release,
};

static struct miscdevice wdt_miscdev = {
	minor:		WATCHDOG_MINOR,
	name:		"watchdog",
	fops:		&wdt_fops,
};

static char banner[] __initdata = KERN_INFO NAME ": Watchdog Timer version " VERSION ", timer margin: %d sec\n";

static int __init watchdog_init(void)
{
	int ret;

	/*
  	 * There should be check for RC32434 SoC
	 */
	if (mips_machgroup != MACH_GROUP_MIKROTIK) return -1;

	ret = misc_register(&wdt_miscdev);

	if (ret)
		return ret;

	stop_wdt();
	set_wdt(DEFAULT_TIMEOUT);

	printk(banner, timeout);

	return 0;
}

static void __exit watchdog_exit(void)
{
	misc_deregister(&wdt_miscdev);
}

module_init(watchdog_init);
module_exit(watchdog_exit);
