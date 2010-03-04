/*
 * RDC321x watchdog driver
 *
 * Copyright (C) 2007-2010 Florian Fainelli <florian@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <linux/miscdevice.h>
#include <linux/platform_device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <linux/timer.h>
#include <linux/watchdog.h>
#include <linux/uaccess.h>
#include <linux/pci.h>

#include <asm/rdc321x_defs.h>

extern int rdc321x_pci_write(int reg, u32 val);
extern int rdc321x_pci_read(int reg, u32 *val);

#define RDC321X_WDT_REG	0x00000044

#define RDC_WDT_EN	0x00800000 /* Enable bit */
#define RDC_WDT_WDTIRQ	0x00400000 /* Create WDT IRQ before CPU reset */
#define RDC_WDT_NMIIRQ	0x00200000 /* Create NMI IRQ before CPU reset */
#define RDC_WDT_RST	0x00100000 /* Reset wdt */
#define RDC_WDT_NIF	0x00080000 /* NMI interrupt occured */
#define RDC_WDT_WIF	0x00040000 /* WDT interrupt occured */
#define RDC_WDT_IRT	0x00000700 /* IRQ Routing table */
#define RDC_WDT_CNT	0x0000007F /* WDT count */

/* default counter value (2.34 s) */
#define RDC_WDT_DFLT_CNT 0x00000040

#define RDC_WDT_SETUP (RDC_WDT_EN | RDC_WDT_NMIIRQ | RDC_WDT_RST | RDC_WDT_DFLT_CNT)

/* some device data */
static struct {
	struct timer_list timer;
	int seconds_left;
	int total_seconds;
	bool inuse;
	bool running;
	bool close_expected;
} rdc321x_wdt_dev;

static struct watchdog_info ident = {
	.options = WDIOF_SETTIMEOUT | WDIOF_MAGICCLOSE,
	.identity = "RDC321x WDT",
};

/* generic helper functions */
static void rdc321x_wdt_timer(unsigned long unused)
{
	if (!rdc321x_wdt_dev.running) {
		rdc321x_pci_write(RDC321X_WDT_REG, 0);
		return;
	}

	rdc321x_wdt_dev.seconds_left--;

	if (rdc321x_wdt_dev.seconds_left < 1)
		return;

	rdc321x_pci_write(RDC321X_WDT_REG, RDC_WDT_SETUP);

	mod_timer(&rdc321x_wdt_dev.timer, HZ * 2 + jiffies);
}

static void rdc321x_wdt_reset(void)
{
	rdc321x_wdt_dev.seconds_left = rdc321x_wdt_dev.total_seconds;
}

static void rdc321x_wdt_start(void)
{
	if (rdc321x_wdt_dev.running)
		return;

	rdc321x_wdt_dev.seconds_left = rdc321x_wdt_dev.total_seconds;

	rdc321x_wdt_dev.running = true;

	rdc321x_wdt_timer(0);

	return;
}

static int rdc321x_wdt_stop(void)
{
	if (WATCHDOG_NOWAYOUT)
		return -ENOSYS;

	rdc321x_wdt_dev.running = false;

	return 0;
}

/* filesystem operations */
static int rdc321x_wdt_open(struct inode *inode, struct file *file)
{
	if (xchg(&rdc321x_wdt_dev.inuse, true))
		return -EBUSY;

	return nonseekable_open(inode, file);
}

static int rdc321x_wdt_release(struct inode *inode, struct file *file)
{
	if (rdc321x_wdt_dev.close_expected)
		rdc321x_wdt_stop();

	rdc321x_wdt_dev.inuse = false;

	return 0;
}

static long rdc321x_wdt_ioctl(struct file *file, unsigned int cmd,
				unsigned long arg)
{
	void __user *argp = (void __user *)arg;
	int value;

	switch (cmd) {
		case WDIOC_KEEPALIVE:
			rdc321x_wdt_reset();
			break;
		case WDIOC_GETSUPPORT:
			if (copy_to_user(argp, &ident, sizeof(ident)))
				return -EFAULT;
			break;
		case WDIOC_SETTIMEOUT:
			if (copy_from_user(&rdc321x_wdt_dev.total_seconds, argp, sizeof(int)))
				return -EFAULT;
			rdc321x_wdt_dev.seconds_left = rdc321x_wdt_dev.total_seconds;
			break;
		case WDIOC_GETTIMEOUT:
			if (copy_to_user(argp, &rdc321x_wdt_dev.total_seconds, sizeof(int)))
				return -EFAULT;
			break;
		case WDIOC_GETTIMELEFT:
			if (copy_to_user(argp, &rdc321x_wdt_dev.seconds_left, sizeof(int)))
				return -EFAULT;
			break;
		case WDIOC_SETOPTIONS:
			if (copy_from_user(&value, argp, sizeof(int)))
				return -EFAULT;
			switch (value) {
				case WDIOS_ENABLECARD:
					rdc321x_wdt_start();
					break;
				case WDIOS_DISABLECARD:
					return rdc321x_wdt_stop();
			default:
				return -EINVAL;
		}
		break;
	default:
		return -EINVAL;
	}
	return 0;
}

static ssize_t rdc321x_wdt_write(struct file *file, const char __user *buf,
				size_t count, loff_t *ppos)
{
	size_t i;
 
	if (!count)
		return -EIO;

	rdc321x_wdt_dev.close_expected = false;

	for (i = 0; i != count; i++) {
		char c;

		if (get_user(c, buf + i))
			return -EFAULT;

		if (c == 'V') {
			rdc321x_wdt_dev.close_expected = true;
			break;
		}
	}

	rdc321x_wdt_reset();

	return count;
}

static const struct file_operations rdc321x_wdt_fops = {
	.llseek		= no_llseek,
	.unlocked_ioctl	= rdc321x_wdt_ioctl,
	.open		= rdc321x_wdt_open,
	.write		= rdc321x_wdt_write,
	.release	= rdc321x_wdt_release,
};

static struct miscdevice rdc321x_wdt_misc = {
	.minor	= WATCHDOG_MINOR,
	.name	= "watchdog",
	.fops	= &rdc321x_wdt_fops,
};

static int __init rdc321x_wdt_probe(struct platform_device *pdev)
{
	int err;

	err = rdc321x_pci_write(RDC321X_WDT_REG, 0);
	if (err)
		return err;

	rdc321x_wdt_dev.running = false;
	rdc321x_wdt_dev.close_expected = false;
	rdc321x_wdt_dev.inuse = 0;
	setup_timer(&rdc321x_wdt_dev.timer, rdc321x_wdt_timer, 0);
	rdc321x_wdt_dev.total_seconds = 100;

	err = misc_register(&rdc321x_wdt_misc);
	if (err < 0) {
		printk(KERN_ERR PFX "watchdog: misc_register failed\n");
		return err;
	}

	panic_on_unrecovered_nmi = 1;
	dev_info(&pdev->dev, "watchdog inig success\n");

	return 0;
}

static int __devexit rdc321x_wdt_remove(struct platform_device *pdev)
{
	if (rdc321x_wdt_dev.inuse)
		rdc321x_wdt_dev.inuse = 0;
	misc_deregister(&rdc321x_wdt_misc);
	return 0;
}

static struct platform_driver rdc321x_wdt_driver = {
	.driver.name	= "rdc321x-wdt",
	.driver.owner	= THIS_MODULE,
	.probe		= rdc321x_wdt_probe,
	.remove		= __devexit_p(rdc321x_wdt_remove),
};

static int __init rdc321x_wdt_init(void)
{
	return platform_driver_register(&rdc321x_wdt_driver);
}

static void __exit rdc321x_wdt_exit(void)
{
	platform_driver_unregister(&rdc321x_wdt_driver);
}

module_init(rdc321x_wdt_init);
module_exit(rdc321x_wdt_exit);

MODULE_AUTHOR("Florian Fainelli <florian@openwrt.org>");
MODULE_DESCRIPTION("RDC321x Watchdog driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:rdc321x-wdt");
