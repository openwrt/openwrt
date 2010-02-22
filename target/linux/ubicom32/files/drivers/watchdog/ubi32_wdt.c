/*
 * drivers/watchdog/ubi32_wdt.c
 *   Ubicom32 Watchdog Driver
 *
 * Originally based on softdog.c
 * Copyright 2006-2007 Analog Devices Inc.
 * Copyright 2006-2007 Michele d'Amico
 * Copyright 1996 Alan Cox <alan@lxorguk.ukuu.org.uk>
 * (C) Copyright 2009, Ubicom, Inc.
 *
 * This file is part of the Ubicom32 Linux Kernel Port.
 *
 * The Ubicom32 Linux Kernel Port is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 *
 * The Ubicom32 Linux Kernel Port is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Ubicom32 Linux Kernel Port.  If not,
 * see <http://www.gnu.org/licenses/>.
 *
 * Ubicom32 implementation derived from (with many thanks):
 *   arch/m68knommu
 *   arch/blackfin
 *   arch/parisc
 */
#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/types.h>
#include <linux/timer.h>
#include <linux/miscdevice.h>
#include <linux/watchdog.h>
#include <linux/fs.h>
#include <linux/notifier.h>
#include <linux/reboot.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/uaccess.h>
#include <asm/ip5000.h>

#define WATCHDOG_NAME "ubi32-wdt"
#define PFX WATCHDOG_NAME ": "

#define OSC1_FREQ 12000000
#define WATCHDOG_SEC_TO_CYC(x) (OSC1_FREQ * (x))
#define WATCHDOG_MAX_SEC (0xffffffff / OSC1_FREQ)

#define MIN_PROCESSOR_ADDRESS 0x03000000

static DEFINE_SPINLOCK(ubi32_wdt_spinlock);

#define WATCHDOG_TIMEOUT 20

#if defined(CONFIG_WATCHDOG_NOWAYOUT)
#define WATCHDOG_NOWAYOUT 1
#else
#define WATCHDOG_NOWAYOUT 0
#endif

static unsigned int timeout = WATCHDOG_TIMEOUT;
static int nowayout = WATCHDOG_NOWAYOUT;
static struct watchdog_info ubi32_wdt_info;
static unsigned long open_check;
static char expect_close;

#if !defined(CONFIG_SMP)
#define UBI32_WDT_LOCK(lock, flags) local_irq_save(flags)
#define UBI32_WDT_UNLOCK(lock, flags) local_irq_restore(flags)
#define UBI32_WDT_LOCK_CHECK()
#else
#define UBI32_WDT_LOCK(lock, flags) spin_lock_irqsave((lock), (flags));
#define UBI32_WDT_UNLOCK(lock, flags) spin_unlock_irqrestore((lock), (flags));
#define UBI32_WDT_LOCK_CHECK() BUG_ON(!spin_is_locked(&ubi32_wdt_spinlock));
#endif

/*
 * ubi32_wdt_remaining()
 * 	Return the approximate number of seconds remaining
 */
static int ubi32_wdt_remaining(void)
{
	int compare;
	int curr;

	UBI32_WDT_LOCK_CHECK();

	ubicom32_write_reg(&UBICOM32_IO_TIMER->tkey, TIMER_TKEYVAL);
	compare = ubicom32_read_reg(&UBICOM32_IO_TIMER->wdcom);
	curr = ubicom32_read_reg(&UBICOM32_IO_TIMER->mptval);
	ubicom32_write_reg(&UBICOM32_IO_TIMER->tkey, 0);
	return (compare - curr) / OSC1_FREQ;

}

/*
 * ubi32_wdt_keepalive()
 *	Keep the Userspace Watchdog Alive
 *
 * The Userspace watchdog got a KeepAlive: schedule the next timeout.
 */
static int ubi32_wdt_keepalive(void)
{
	UBI32_WDT_LOCK_CHECK();
	ubicom32_write_reg(&UBICOM32_IO_TIMER->tkey, TIMER_TKEYVAL);
	ubicom32_write_reg(&UBICOM32_IO_TIMER->wdcom,
			ubicom32_read_reg(&UBICOM32_IO_TIMER->mptval)
			+ WATCHDOG_SEC_TO_CYC(timeout));
	ubicom32_write_reg(&UBICOM32_IO_TIMER->tkey, 0);
	return 0;
}

/*
 * ubi32_wdt_stop()
 *	Stop the on-chip Watchdog
 */
static int ubi32_wdt_stop(void)
{
	UBI32_WDT_LOCK_CHECK();
	ubicom32_write_reg(&UBICOM32_IO_TIMER->tkey, TIMER_TKEYVAL);
	ubicom32_write_reg(&UBICOM32_IO_TIMER->wdcfg, TIMER_WATCHDOG_DISABLE);
	ubicom32_write_reg(&UBICOM32_IO_TIMER->tkey, 0);
	return 0;
}

/*
 * ubi32_wdt_start()
 *	Start the on-chip Watchdog
 */
static int ubi32_wdt_start(void)
{
	UBI32_WDT_LOCK_CHECK();
	ubicom32_write_reg(&UBICOM32_IO_TIMER->tkey, TIMER_TKEYVAL);
	ubicom32_write_reg(&UBICOM32_IO_TIMER->wdcom,
			ubicom32_read_reg(&UBICOM32_IO_TIMER->mptval)
			+ WATCHDOG_SEC_TO_CYC(timeout));
	ubicom32_write_reg(&UBICOM32_IO_TIMER->wdcfg, ~TIMER_WATCHDOG_DISABLE);
	ubicom32_write_reg(&UBICOM32_IO_TIMER->tkey, 0);
	return 0;
}

/*
 * ubi32_wdt_running()
 * 	Return true if the watchdog is configured
 */
static int ubi32_wdt_running(void)
{
	int enabled;

	UBI32_WDT_LOCK_CHECK();
	ubicom32_write_reg(&UBICOM32_IO_TIMER->tkey, TIMER_TKEYVAL);
	enabled = ubicom32_read_reg(&UBICOM32_IO_TIMER->wdcfg) == ~TIMER_WATCHDOG_DISABLE;
	ubicom32_write_reg(&UBICOM32_IO_TIMER->tkey, 0);
	return enabled;
}

/*
 * ubi32_wdt_set_timeout()
 *	Set the Userspace Watchdog timeout
 *
 * - @t: new timeout value (in seconds)
 */
static int ubi32_wdt_set_timeout(unsigned long t)
{
	UBI32_WDT_LOCK_CHECK();

	if (t > WATCHDOG_MAX_SEC) {
		printk(KERN_WARNING PFX "request to large: %ld [1-%d] sec)\n", t, WATCHDOG_MAX_SEC);
		return -EINVAL;
	}

	/*
	 * If we are running, then reset the time value so
	 * that the new value has an immediate effect.
	 */
	timeout = t;
	if (ubi32_wdt_running()) {
		ubi32_wdt_keepalive();
	}
	return 0;
}

/*
 * ubi32_wdt_open()
 *	Open the Device
 */
static int ubi32_wdt_open(struct inode *inode, struct file *file)
{
	unsigned long flags;

	if (test_and_set_bit(0, &open_check))
		return -EBUSY;

	if (nowayout)
		__module_get(THIS_MODULE);

	spin_lock_irqsave(&ubi32_wdt_spinlock, flags);
	ubi32_wdt_start();
	spin_unlock_irqrestore(&ubi32_wdt_spinlock, flags);

	return nonseekable_open(inode, file);
}

/*
 * ubi32_wdt_close()
 *	Close the Device
 */
static int ubi32_wdt_release(struct inode *inode, struct file *file)
{
	unsigned long flags;

	/*
	 * If we don't expect a close, then the watchdog continues
	 * even though the device is closed.  The caller will have
	 * a full timeout value to reopen the device and continue
	 * stroking it.
	 */
	if (expect_close != 42) {
		printk(KERN_CRIT PFX
			"Unexpected close, not stopping watchdog!\n");
		spin_lock_irqsave(&ubi32_wdt_spinlock, flags);
		ubi32_wdt_keepalive();
		spin_unlock_irqrestore(&ubi32_wdt_spinlock, flags);
	} else {
		spin_lock_irqsave(&ubi32_wdt_spinlock, flags);
		ubi32_wdt_stop();
		spin_unlock_irqrestore(&ubi32_wdt_spinlock, flags);
	}

	expect_close = 0;
	clear_bit(0, &open_check);
	return 0;
}

/*
 * ubi32_wdt_write()
 *	Write to Device
 *
 * If the user writes nothing, nothing happens.
 * If the user writes a V, then we expect a close and allow a release.
 * If the user writes anything else, it is ignored.
 */
static ssize_t ubi32_wdt_write(struct file *file, const char __user *data,
						size_t len, loff_t *ppos)
{
	size_t i;
	unsigned long flags;

	/*
	 * Every write resets the expect_close.  The last write
	 * must be a V to allow shutdown on close.
	 */
	expect_close = 0;

	/*
	 * Empty writes still ping.
	 */
	if (!len) {
		goto ping;
	}

	/*
	 * If nowayout is set, it does not matter if the caller
	 * is trying to send the magic 'V' we will not allow a
	 * close to stop us.
	 */
	if (nowayout) {
		goto ping;
	}

	/*
	 * See if the program wrote a 'V' and if so disable
	 * the watchdog on release.
	 */
	for (i = 0; i < len; i++) {
		char c;
		if (get_user(c, data + i)) {
			return -EFAULT;
		}

		if (c == 'V') {
			expect_close = 42;
		}
	}

ping:
	spin_lock_irqsave(&ubi32_wdt_spinlock, flags);
	ubi32_wdt_keepalive();
	spin_unlock_irqrestore(&ubi32_wdt_spinlock, flags);
	return len;
}

/*
 * ubi32_wdt_ioctl()
 *	Query the watchdog device.
 *
 * Query basic information from the device or ping it, as outlined by the
 * watchdog API.
 */
static long ubi32_wdt_ioctl(struct file *file,
				unsigned int cmd, unsigned long arg)
{
	void __user *argp = (void __user *)arg;
	int __user *p = argp;

	switch (cmd) {
	case WDIOC_GETSUPPORT:
		if (copy_to_user(argp, &ubi32_wdt_info, sizeof(ubi32_wdt_info))) {
			return -EFAULT;
		}
		return 0;

	case WDIOC_GETSTATUS: {
		unsigned long flags;
		int running;

		spin_lock_irqsave(&ubi32_wdt_spinlock, flags);
		running = ubi32_wdt_running();
		spin_unlock_irqrestore(&ubi32_wdt_spinlock, flags);
		return running;
	}

	case WDIOC_GETBOOTSTATUS:
		return ubicom32_get_reset_reason();

	case WDIOC_SETOPTIONS: {
		unsigned long flags;
		int options, ret = -EINVAL;

		/*
		 * The sample application does not pass a pointer
		 * but directly passes a value of 1 or 2; however
		 * all of the implementations (and thus probably
		 * the real applications) pass a pointer to a value.
		 *
		 * It should be noted that  WDIOC_SETOPTIONS is defined as
		 * _IOR(WATCHDOG_IOCTL_BASE, 4, int), which means
		 * that it should be an int and NOT a pointer.
		 *
		 * TODO: Examine this code for future chips.
		 * TODO: Report the sample code defect.
		 */
		if ((int)p < MIN_PROCESSOR_ADDRESS) {
			options = (int)p;
		} else {
			if (get_user(options, p))
			return -EFAULT;
		}

		spin_lock_irqsave(&ubi32_wdt_spinlock, flags);
		if (options & WDIOS_DISABLECARD) {
			ubi32_wdt_stop();
			ret = 0;
		}
		if (options & WDIOS_ENABLECARD) {
			ubi32_wdt_start();
			ret = 0;
		}
		spin_unlock_irqrestore(&ubi32_wdt_spinlock, flags);
		return ret;
	}

	case WDIOC_KEEPALIVE: {
		unsigned long flags;

		spin_lock_irqsave(&ubi32_wdt_spinlock, flags);
		ubi32_wdt_keepalive();
		spin_unlock_irqrestore(&ubi32_wdt_spinlock, flags);
		return 0;
	}

	case WDIOC_SETTIMEOUT: {
		int new_timeout;
		unsigned long flags;
		int ret  = 0;

		if (get_user(new_timeout, p))
			return -EFAULT;

		spin_lock_irqsave(&ubi32_wdt_spinlock, flags);
		ret = ubi32_wdt_set_timeout(new_timeout);
		spin_unlock_irqrestore(&ubi32_wdt_spinlock, flags);
		return ret;

	}

	case WDIOC_GETTIMEOUT:
		return put_user(timeout, p);

	case WDIOC_GETTIMELEFT: {
		unsigned long flags;
		int remaining = 0;

		spin_lock_irqsave(&ubi32_wdt_spinlock, flags);
		remaining = ubi32_wdt_remaining();
		spin_unlock_irqrestore(&ubi32_wdt_spinlock, flags);
		return put_user(remaining, p);
	}

	default:
		return -ENOTTY;
	}
}

/*
 * ubi32_wdt_notify_sys()
 *	Notification callback function for system events.
 *
 * Turn off the watchdog during a SYS_DOWN or SYS_HALT.
 */
static int ubi32_wdt_notify_sys(struct notifier_block *this,
					unsigned long code, void *unused)
{
	if (code == SYS_DOWN || code == SYS_HALT) {
		unsigned long flags;

		spin_lock_irqsave(&ubi32_wdt_spinlock, flags);
		ubi32_wdt_stop();
		spin_unlock_irqrestore(&ubi32_wdt_spinlock, flags);
	}

	return NOTIFY_DONE;
}

#ifdef CONFIG_PM
static int state_before_suspend;

/*
 * ubi32_wdt_suspend()
 * 	suspend the watchdog
 *
 * Remember if the watchdog was running and stop it.
 */
static int ubi32_wdt_suspend(struct platform_device *pdev, pm_message_t state)
{
	unsigned long flags;
	spin_lock_irqsave(&ubi32_wdt_spinlock, flags);
	state_before_suspend = ubi32_wdt_running();
	ubi32_wdt_stop();
	spin_unlock_irqrestore(&ubi32_wdt_spinlock, flags);

	return 0;
}

/*
 * ubi32_wdt_resume()
 * 	Resume the watchdog
 *
 * If the watchdog was running, turn it back on.
 */
static int ubi32_wdt_resume(struct platform_device *pdev)
{
	if (state_before_suspend) {
		unsigned long flags;
		spin_lock_irqsave(&ubi32_wdt_spinlock, flags);
		ubi32_wdt_set_timeout(timeout);
		ubi32_wdt_start();
		spin_unlock_irqrestore(&ubi32_wdt_spinlock, flags);
	}

	return 0;
}
#else
# define ubi32_wdt_suspend NULL
# define ubi32_wdt_resume NULL
#endif

static const struct file_operations ubi32_wdt_fops = {
	.owner		= THIS_MODULE,
	.llseek		= no_llseek,
	.write    	= ubi32_wdt_write,
	.unlocked_ioctl	= ubi32_wdt_ioctl,
	.open		= ubi32_wdt_open,
	.release	= ubi32_wdt_release,
};

static struct miscdevice ubi32_wdt_miscdev = {
	.minor    = WATCHDOG_MINOR,
	.name     = "watchdog",
	.fops     = &ubi32_wdt_fops,
};

static struct watchdog_info ubi32_wdt_info = {
	.identity = "Ubicom32 Watchdog",
	.options  = WDIOF_SETTIMEOUT |
		    WDIOF_KEEPALIVEPING |
		    WDIOF_MAGICCLOSE,
};

static struct notifier_block ubi32_wdt_notifier = {
	.notifier_call = ubi32_wdt_notify_sys,
};

/*
 * ubi32_wdt_probe()
 * 	Probe/register the watchdog module
 *
 * Registers the misc device and notifier handler.  Actual device
 * initialization is handled by ubi32_wdt_open().
 */
static int __devinit ubi32_wdt_probe(struct platform_device *pdev)
{
	int ret;

	ret = register_reboot_notifier(&ubi32_wdt_notifier);
	if (ret) {
		printk(KERN_ERR PFX
			"cannot register reboot notifier (err=%d)\n", ret);
		return ret;
	}

	ret = misc_register(&ubi32_wdt_miscdev);
	if (ret) {
		printk(KERN_ERR PFX
			"cannot register miscdev on minor=%d (err=%d)\n",
				WATCHDOG_MINOR, ret);
		unregister_reboot_notifier(&ubi32_wdt_notifier);
		return ret;
	}

	printk(KERN_INFO PFX "initialized: timeout=%d sec (nowayout=%d)\n",
	       timeout, nowayout);

	return 0;
}

/*
 * ubi32_wdt_remove()
 * 	Uninstall the module
 *
 * Unregisters the misc device and notifier handler.  Actual device
 * deinitialization is handled by ubi32_wdt_close().
 */
static int __devexit ubi32_wdt_remove(struct platform_device *pdev)
{
	misc_deregister(&ubi32_wdt_miscdev);
	unregister_reboot_notifier(&ubi32_wdt_notifier);
	return 0;
}

static struct platform_device *ubi32_wdt_device;

static struct platform_driver ubi32_wdt_driver = {
	.probe     = ubi32_wdt_probe,
	.remove    = __devexit_p(ubi32_wdt_remove),
	.suspend   = ubi32_wdt_suspend,
	.resume    = ubi32_wdt_resume,
	.driver    = {
		.name  = WATCHDOG_NAME,
		.owner = THIS_MODULE,
	},
};

/*
 * ubi32_wdt_init()
 * 	Initialize the watchdog.
 *
 * Checks the module params and registers the platform device & driver.
 * Real work is in the platform probe function.
 */
static int __init ubi32_wdt_init(void)
{
	unsigned long flags;
	int ret;

	/*
	 * Check that the timeout value is within range
	 */
	spin_lock_irqsave(&ubi32_wdt_spinlock, flags);
	ret = ubi32_wdt_set_timeout(timeout);
	spin_unlock_irqrestore(&ubi32_wdt_spinlock, flags);
	if (ret) {
		return ret;
	}

	/*
	 * Since this is an on-chip device and needs no board-specific
	 * resources, we'll handle all the platform device stuff here.
	 */
	ret = platform_driver_register(&ubi32_wdt_driver);
	if (ret) {
		printk(KERN_ERR PFX "unable to register driver\n");
		return ret;
	}

	ubi32_wdt_device = platform_device_register_simple(WATCHDOG_NAME, -1, NULL, 0);
	if (IS_ERR(ubi32_wdt_device)) {
		printk(KERN_ERR PFX "unable to register device\n");
		platform_driver_unregister(&ubi32_wdt_driver);
		return PTR_ERR(ubi32_wdt_device);
	}

	return 0;
}

/*
 * ubi32_wdt_exit()
 * 	Deinitialize module
 *
 * Back out the platform device & driver steps.  Real work is in the
 * platform remove function.
 */
static void __exit ubi32_wdt_exit(void)
{
	platform_device_unregister(ubi32_wdt_device);
	platform_driver_unregister(&ubi32_wdt_driver);
}

module_init(ubi32_wdt_init);
module_exit(ubi32_wdt_exit);

MODULE_AUTHOR("Sol Kavy<sol@ubicom.com>");
MODULE_DESCRIPTION("Ubicom32 Watchdog Device Driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS_MISCDEV(WATCHDOG_MINOR);

module_param(timeout, uint, 0);
MODULE_PARM_DESC(timeout,
	"Watchdog timeout in seconds. (1<=timeout<=((2^32)/SCLK), default="
		__MODULE_STRING(WATCHDOG_TIMEOUT) ")");

module_param(nowayout, int, 0);
MODULE_PARM_DESC(nowayout,
	"Watchdog cannot be stopped once started (default="
		__MODULE_STRING(WATCHDOG_NOWAYOUT) ")");
