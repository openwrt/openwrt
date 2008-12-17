/*
 *  Broadcom BCM63xx SoC watchdog driver
 *
 *  Copyright (C) 2007, Miguel Gaio <miguel.gaio@efixo.com>
 *  Copyright (C) 2008, Florian Fainelli <florian@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version
 *  2 of the License, or (at your option) any later version.
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
#include <linux/platform_device.h>
#include <linux/uaccess.h>
#include <linux/timer.h>

#include <bcm63xx_cpu.h>
#include <bcm63xx_io.h>
#include <bcm63xx_regs.h>

#define PFX KBUILD_MODNAME

#define WDT_HZ		50000000 /* Fclk */
#define WDT_INTERVAL	(40)    /* in seconds */

static struct {
	void __iomem *regs;
	struct completion stop;
	int running;
	struct timer_list timer;
	int queue;
	int default_ticks;
	unsigned long inuse;
} bcm63xx_wdt_device;

static int ticks = 100 * WDT_HZ;

static int expect_close;
static int timeout;

static int nowayout = WATCHDOG_NOWAYOUT;
module_param(nowayout, int, 0);
MODULE_PARM_DESC(nowayout, "Watchdog cannot be stopped once started (default="
	__MODULE_STRING(WATCHDOG_NOWAYOUT) ")");


static void bcm63xx_wdt_toggle(void)
{
	bcm_writel(WDT_START_1, bcm63xx_wdt_device.regs + WDT_CTL_REG);
	bcm_writel(WDT_START_2, bcm63xx_wdt_device.regs + WDT_CTL_REG);
}

static void bcm63xx_wdt_start(void)
{
	if (!bcm63xx_wdt_device.inuse) {
		bcm63xx_wdt_toggle();
		mod_timer(&bcm63xx_wdt_device.timer, jiffies + WDT_INTERVAL);
	}

	bcm63xx_wdt_device.running++;
}

static void bcm63xx_wdt_stop(void)
{
	if (bcm63xx_wdt_device.running) {
		bcm_writel(WDT_STOP_1, bcm63xx_wdt_device.regs + WDT_CTL_REG);
		bcm_writel(WDT_STOP_2, bcm63xx_wdt_device.regs + WDT_CTL_REG);

		bcm63xx_wdt_device.running = 0;
	}
}

static void bcm63xx_wdt_set(int new_timeout)
{
	new_timeout *= WDT_HZ;
	bcm_writel(new_timeout, bcm63xx_wdt_device.regs + WDT_DEFVAL_REG);
}

static void bcm63xx_wdt_reset(void)
{
	ticks = bcm63xx_wdt_device.default_ticks;
}

static void bcm63xx_wdt_update(unsigned long unused)
{
	if (bcm63xx_wdt_device.running)
		ticks--;

	bcm63xx_wdt_toggle();

	if (bcm63xx_wdt_device.queue && ticks)
		mod_timer(&bcm63xx_wdt_device.timer,
			jiffies + WDT_INTERVAL);
	else
		complete(&bcm63xx_wdt_device.stop);
}

static int bcm63xx_wdt_open(struct inode *inode, struct file *file)
{
	if (test_and_set_bit(0, &bcm63xx_wdt_device.inuse))
		return -EBUSY;

	if (nowayout)
		__module_get(THIS_MODULE);

	return nonseekable_open(inode, file);
}

static int bcm63xx_wdt_release(struct inode *inode, struct file *file)
{
	if (expect_close && nowayout == 0) {
		bcm63xx_wdt_stop();
		printk(KERN_INFO PFX ": disabling watchdog timer\n");
		module_put(THIS_MODULE);
	} else
		printk(KERN_CRIT PFX
			": device closed unexpectedly. WDT will not stop !\n");

	clear_bit(0, &bcm63xx_wdt_device.inuse);
	return 0;
}

static ssize_t bcm63xx_wdt_write(struct file *file, const char *data,
				size_t len, loff_t *ppos)
{
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
		bcm63xx_wdt_update(0);
		return len;
	}
	return 0;
}

static long bcm63xx_wdt_ioctl(struct file *file, unsigned int cmd,
				unsigned long arg)
{
	void __user *argp = (void __user *)arg;
	int new_timeout;
	unsigned int value;
	static struct watchdog_info ident = {
		.options =		WDIOF_SETTIMEOUT |
					WDIOF_KEEPALIVEPING |
					WDIOF_MAGICCLOSE,
		.identity =		"BCM63xx Watchdog",
	};
	switch (cmd) {
	case WDIOC_KEEPALIVE:
		bcm63xx_wdt_reset();
		break;
	case WDIOC_GETSTATUS:
	case WDIOC_GETBOOTSTATUS:
		value = bcm_readl(bcm63xx_wdt_device.regs + WDT_DEFVAL_REG);
		if (copy_to_user(argp, &value, sizeof(int)))
			return -EFAULT;
		break;
	case WDIOC_GETSUPPORT:
		if (copy_to_user(argp, &ident, sizeof(ident)))
			return -EFAULT;
		break;
	case WDIOC_SETOPTIONS:
		if (copy_from_user(&value, argp, sizeof(int)))
			return -EFAULT;
		switch (value) {
		case WDIOS_ENABLECARD:
			bcm63xx_wdt_start();
			break;
		case WDIOS_DISABLECARD:
			bcm63xx_wdt_stop();
		default:
			return -EINVAL;
		}
		break;
	case WDIOC_SETTIMEOUT:
		if (copy_from_user(&new_timeout, argp, sizeof(int)))
			return -EFAULT;
		if (new_timeout < 5)
			return -EINVAL;
		if (new_timeout > 40)
			return -EINVAL;
		bcm63xx_wdt_set(new_timeout);
		bcm63xx_wdt_toggle();
	case WDIOC_GETTIMEOUT:
		return copy_to_user(argp, &timeout, sizeof(int));
	default:
		return -ENOTTY;
	}

	return 0;
}

static struct file_operations bcm63xx_wdt_fops = {
	.owner		= THIS_MODULE,
	.llseek		= no_llseek,
	.write		= bcm63xx_wdt_write,
	.unlocked_ioctl	= bcm63xx_wdt_ioctl,
	.open		= bcm63xx_wdt_open,
	.release	= bcm63xx_wdt_release,
};

static struct miscdevice bcm63xx_wdt_miscdev = {
	.minor	= WATCHDOG_MINOR,
	.name	= "watchdog",
	.fops	= &bcm63xx_wdt_fops,
};

static int bcm63xx_wdt_probe(struct platform_device *pdev)
{
	int ret;
	struct resource *r;

	r = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!r) {
		printk(KERN_ERR PFX 
			"failed to retrieve resources\n");
		return -ENODEV;
	}

	bcm63xx_wdt_device.regs = ioremap_nocache(r->start, r->end - r->start);
	if (!bcm63xx_wdt_device.regs) {
		printk(KERN_ERR PFX
			"failed to remap I/O resources\n");
		return -ENXIO;
	}

	ret = misc_register(&bcm63xx_wdt_miscdev);
	if (ret < 0) {
		printk(KERN_ERR PFX
			"failed to register watchdog device\n");
		goto unmap;
	}

	init_completion(&bcm63xx_wdt_device.stop);
	bcm63xx_wdt_device.queue = 0;

	clear_bit(0, &bcm63xx_wdt_device.inuse);

	setup_timer(&bcm63xx_wdt_device.timer, bcm63xx_wdt_update, 0L);

	bcm63xx_wdt_device.default_ticks = ticks;
	bcm63xx_wdt_set(ticks);
	bcm63xx_wdt_start();
	
	printk(KERN_INFO PFX " started, timer margin: %d sec\n", WDT_INTERVAL);

	return 0;

unmap:
	iounmap(bcm63xx_wdt_device.regs);
	return ret;
}

static int bcm63xx_wdt_remove(struct platform_device *pdev)
{
	if (bcm63xx_wdt_device.queue) {
		bcm63xx_wdt_device.queue = 0;
		wait_for_completion(&bcm63xx_wdt_device.stop);
	}

	misc_deregister(&bcm63xx_wdt_miscdev);

	iounmap(bcm63xx_wdt_device.regs);

	return 0;
}

static struct platform_driver bcm63xx_wdt = {
	.probe	= bcm63xx_wdt_probe,
	.remove = bcm63xx_wdt_remove,
	.driver = {
		.name = "bcm63xx-wdt",
	}
};

static int __init bcm63xx_wdt_init(void)
{
	return platform_driver_register(&bcm63xx_wdt);
}

static void __exit bcm63xx_wdt_exit(void)
{
	platform_driver_unregister(&bcm63xx_wdt);
}

module_init(bcm63xx_wdt_init);
module_exit(bcm63xx_wdt_exit);

MODULE_AUTHOR("Miguel Gaio <miguel.gaio@efixo.com>");
MODULE_AUTHOR("Florian Fainelli <florian@openwrt.org>");
MODULE_DESCRIPTION("Driver for the Broadcom BCM63xx SoC watchdog");
MODULE_LICENSE("GPL");
MODULE_ALIAS_MISCDEV(WATCHDOG_MINOR);
MODULE_ALIAS("platform:bcm63xx-wdt");
