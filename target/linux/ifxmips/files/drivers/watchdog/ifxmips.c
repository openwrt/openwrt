/*
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Copyright (C) 2008 John Crispin <blogic@openwrt.org>
 * Based on EP93xx wdt driver
 */

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/miscdevice.h>
#include <linux/watchdog.h>
#include <linux/platform_device.h>
#include <linux/uaccess.h>

#include <ifxmips.h>
#include <ifxmips_cgu.h>

#define IFXMIPS_WDT_PW1			0x00BE0000
#define IFXMIPS_WDT_PW2			0x00DC0000

#ifndef CONFIG_WATCHDOG_NOWAYOUT
static int wdt_ok_to_close;
#endif

static int wdt_timeout = 30;

int ifxmips_wdt_enable(unsigned int timeout)
{
	u32 fpi;
	fpi = cgu_get_io_region_clock();
	ifxmips_w32(IFXMIPS_WDT_PW1, IFXMIPS_BIU_WDT_CR);
	ifxmips_w32(IFXMIPS_WDT_PW2 |
		(0x3 << 26) | /* PWL */
		(0x3 << 24) | /* CLKDIV */
		(0x1 << 31) | /* enable */
		((timeout * (fpi / 0x40000)) + 0x1000), /* reload */
		IFXMIPS_BIU_WDT_CR);
	return 0;
}

void ifxmips_wdt_disable(void)
{
#ifndef CONFIG_WATCHDOG_NOWAYOUT
	wdt_ok_to_close = 0;
#endif
	ifxmips_w32(IFXMIPS_WDT_PW1, IFXMIPS_BIU_WDT_CR);
	ifxmips_w32(IFXMIPS_WDT_PW2, IFXMIPS_BIU_WDT_CR);
}

static ssize_t ifxmips_wdt_write(struct file *file, const char __user *data,
		size_t len, loff_t *ppos)
{
	size_t i;

	if (!len)
		return 0;

#ifndef CONFIG_WATCHDOG_NOWAYOUT
	for (i = 0; i != len; i++) {
		char c;
		if (get_user(c, data + i))
			return -EFAULT;
		if (c == 'V')
			wdt_ok_to_close = 1;
	}
#endif
	ifxmips_wdt_enable(wdt_timeout);
	return len;
}

static struct watchdog_info ident = {
	.options = WDIOF_MAGICCLOSE,
	.identity = "ifxmips Watchdog",
};

static int ifxmips_wdt_ioctl(struct inode *inode, struct file *file,
		unsigned int cmd, unsigned long arg)
{
	int ret = -ENOTTY;

	switch (cmd) {
	case WDIOC_GETSUPPORT:
		ret = copy_to_user((struct watchdog_info __user *)arg, &ident,
				sizeof(ident)) ? -EFAULT : 0;
		break;

	case WDIOC_GETTIMEOUT:
		ret = put_user(wdt_timeout, (int __user *)arg);
		break;

	case WDIOC_SETTIMEOUT:
		ret = get_user(wdt_timeout, (int __user *)arg);
		break;

	case WDIOC_KEEPALIVE:
		ifxmips_wdt_enable(wdt_timeout);
		ret = 0;
		break;
	}
	return ret;
}

static int ifxmips_wdt_open(struct inode *inode, struct file *file)
{
	ifxmips_wdt_enable(wdt_timeout);
	return nonseekable_open(inode, file);
}

static int ifxmips_wdt_release(struct inode *inode, struct file *file)
{
#ifndef CONFIG_WATCHDOG_NOWAYOUT
	if (wdt_ok_to_close)
		ifxmips_wdt_disable();
	else
#endif
		printk(KERN_ERR "ifxmips_wdt: watchdog closed without warning,"
			" rebooting system\n");
	return 0;
}

static const struct file_operations ifxmips_wdt_fops = {
	.owner		= THIS_MODULE,
	.write		= ifxmips_wdt_write,
	.ioctl		= ifxmips_wdt_ioctl,
	.open		= ifxmips_wdt_open,
	.release	= ifxmips_wdt_release,
};

static struct miscdevice ifxmips_wdt_miscdev = {
	.minor		= WATCHDOG_MINOR,
	.name		= "watchdog",
	.fops		= &ifxmips_wdt_fops,
};

static int ifxmips_wdt_probe(struct platform_device *dev)
{
	int err;
	err = misc_register(&ifxmips_wdt_miscdev);
	if (err)
		printk(KERN_INFO "ifxmips_wdt: error creating device\n");
	else
		printk(KERN_INFO "ifxmips_wdt: loaded\n");
	return err;
}

static int ifxmips_wdt_remove(struct platform_device *dev)
{
	ifxmips_wdt_disable();
	misc_deregister(&ifxmips_wdt_miscdev);
	return 0;
}


static struct platform_driver ifxmips_wdt_driver = {
	.probe = ifxmips_wdt_probe,
	.remove = ifxmips_wdt_remove,
	.driver = {
		.name = "ifxmips_wdt",
		.owner = THIS_MODULE,
	},
};

static int __init init_ifxmips_wdt(void)
{
	int ret = platform_driver_register(&ifxmips_wdt_driver);
	if (ret)
		printk(KERN_INFO "ifxmips_wdt: error registering platfom driver!");
	return ret;
}

static void __exit exit_ifxmips_wdt(void)
{
	platform_driver_unregister(&ifxmips_wdt_driver);
}

module_init(init_ifxmips_wdt);
module_exit(exit_ifxmips_wdt);

MODULE_AUTHOR("John Crispin <blogic@openwrt.org>");
MODULE_DESCRIPTION("ifxmips Watchdog");
MODULE_LICENSE("GPL");
MODULE_ALIAS_MISCDEV(WATCHDOG_MINOR);
