/*
 * linux/drivers/char/ar7_wdt.c
 * 
 * Copyright (C) 2007 OpenWrt.org
 * Copyright (c) 2005 Enrik Berkhan <Enrik.Berkhan@akk.org>
 *
 * Some code taken from:
 * National Semiconductor SCx200 Watchdog support
 * Copyright (c) 2001,2002 Christer Weinigel <wingel@nano-system.com>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/watchdog.h>
#include <linux/notifier.h>
#include <linux/reboot.h>
#include <linux/fs.h>
#include <linux/ioport.h>

#include <asm/addrspace.h>
#include <asm/io.h>
#include <asm/uaccess.h>

#include <asm/ar7/ar7.h>

#define DRVNAME "ar7_wdt"
#define LONGNAME "TI AR7 Watchdog Timer"

MODULE_AUTHOR("Nicolas Thill <nico@openwrt.org>");
MODULE_DESCRIPTION(LONGNAME);
MODULE_LICENSE("GPL");
MODULE_ALIAS_MISCDEV(WATCHDOG_MINOR);

static int margin = 60;
module_param(margin, int, 0);
MODULE_PARM_DESC(margin, "Watchdog margin in seconds");

static int nowayout = WATCHDOG_NOWAYOUT;
module_param(nowayout, int, 0);
MODULE_PARM_DESC(nowayout, "Disable watchdog shutdown on close");

typedef struct {
	u32 kick_lock;
	u32 kick;
	u32 change_lock;
	u32 change ;
	u32 disable_lock;
	u32 disable;
	u32 prescale_lock;
	u32 prescale;
} ar7_wdt_t;

static struct semaphore open_semaphore;
static unsigned expect_close;

/* XXX correct? assumed to be sysfreq/2. get this dynamically ... */
#define vbus_freq (ar7_bus_freq() / 2)

/* XXX currently fixed, allows max margin ~68.72 secs */
#define prescale_value 0xFFFF

static void ar7_wdt_kick(u32 value)
{
	volatile ar7_wdt_t *ar7_wdt = (ar7_wdt_t *)ioremap(AR7_REGS_WDT, sizeof(ar7_wdt_t));

	ar7_wdt->kick_lock = 0x5555;
	if ((ar7_wdt->kick_lock & 3) == 1) {
		ar7_wdt->kick_lock = 0xAAAA;
		if ((ar7_wdt->kick_lock & 3) == 3) {
			ar7_wdt->kick = value;
			return;
		}
 	}
 	printk(KERN_ERR DRVNAME ": failed to unlock WDT kick reg\n");
}

static void ar7_wdt_prescale(u32 value)
{
	volatile ar7_wdt_t *ar7_wdt = (ar7_wdt_t *)ioremap(AR7_REGS_WDT, sizeof(ar7_wdt_t));

	ar7_wdt->prescale_lock = 0x5A5A;
	if ((ar7_wdt->prescale_lock & 3) == 1) {
		ar7_wdt->prescale_lock = 0xA5A5;
		if ((ar7_wdt->prescale_lock & 3) == 3) {
			ar7_wdt->prescale = value;
			return;
		}
	}
	printk(KERN_ERR DRVNAME ": failed to unlock WDT prescale reg\n");
}

static void ar7_wdt_change(u32 value)
{
	volatile ar7_wdt_t *ar7_wdt = (ar7_wdt_t *)ioremap(AR7_REGS_WDT, sizeof(ar7_wdt_t));

	ar7_wdt->change_lock = 0x6666;
	if ((ar7_wdt->change_lock & 3) == 1) {
		ar7_wdt->change_lock = 0xBBBB;
		if ((ar7_wdt->change_lock & 3) == 3) {
			ar7_wdt->change = value;
			return;
		}
	}
	printk(KERN_ERR DRVNAME ": failed to unlock WDT change reg\n");
}

static void ar7_wdt_disable(u32 value)
{
	volatile ar7_wdt_t *ar7_wdt = (ar7_wdt_t *)ioremap(AR7_REGS_WDT, sizeof(ar7_wdt_t));

	ar7_wdt->disable_lock = 0x7777;
	if ((ar7_wdt->disable_lock & 3) == 1) {
		ar7_wdt->disable_lock = 0xCCCC;
		if ((ar7_wdt->disable_lock & 3) == 2) {
			ar7_wdt->disable_lock = 0xDDDD; 
			if ((ar7_wdt->disable_lock & 3) == 3) {
				ar7_wdt->disable = value;
				return;
			}
		}
	}
	printk(KERN_ERR DRVNAME ": failed to unlock WDT disable reg\n");
}

static void ar7_wdt_update_margin(int new_margin)
{
	u32 change;

	change = new_margin * (vbus_freq / prescale_value);
	if (change < 1) change = 1;
	if (change > 0xFFFF) change = 0xFFFF;
	ar7_wdt_change(change);
	margin = change * prescale_value / vbus_freq;
	printk(KERN_INFO DRVNAME
	       ": timer margin %d seconds (prescale %d, change %d, freq %d)\n",
	       margin, prescale_value, change, vbus_freq);
}

static void ar7_wdt_enable_wdt(void)
{
	printk(KERN_DEBUG DRVNAME ": enabling watchdog timer\n");
	ar7_wdt_disable(1);
	ar7_wdt_kick(1);
}

static void ar7_wdt_disable_wdt(void)
{
	printk(KERN_DEBUG DRVNAME ": disabling watchdog timer\n");
	ar7_wdt_disable(0);
}

static int ar7_wdt_open(struct inode *inode, struct file *file)
{
	/* only allow one at a time */
	if (down_trylock(&open_semaphore))
		return -EBUSY;
	ar7_wdt_enable_wdt();
	expect_close = 0;

	return 0;
}

static int ar7_wdt_release(struct inode *inode, struct file *file)
{
	if (!expect_close) {
		printk(KERN_WARNING DRVNAME ": watchdog device closed unexpectedly, will not disable the watchdog timer\n");
	} else if (!nowayout) {
		ar7_wdt_disable_wdt();
	}
        up(&open_semaphore);

	return 0;
}

static int ar7_wdt_notify_sys(struct notifier_block *this, 
			      unsigned long code, void *unused)
{
	if (code == SYS_HALT || code == SYS_POWER_OFF)
		if (!nowayout)
			ar7_wdt_disable_wdt();

        return NOTIFY_DONE;
}

static struct notifier_block ar7_wdt_notifier =
{
	.notifier_call = ar7_wdt_notify_sys
};

static ssize_t ar7_wdt_write(struct file *file, const char *data, 
			     size_t len, loff_t *ppos)
{
	if (ppos != &file->f_pos)
		return -ESPIPE;

	/* check for a magic close character */
	if (len) {
		size_t i;

		ar7_wdt_kick(1);

		expect_close = 0;
		for (i = 0; i < len; ++i) {
			char c;
			if (get_user(c, data+i))
				return -EFAULT;
			if (c == 'V')
				expect_close = 1;
		}

	}
	return len;
}

static int ar7_wdt_ioctl(struct inode *inode, struct file *file, 
			 unsigned int cmd, unsigned long arg)
{
	static struct watchdog_info ident = {
		.identity = LONGNAME,
		.firmware_version = 1, 
		.options = (WDIOF_SETTIMEOUT | WDIOF_KEEPALIVEPING),
	};
	int new_margin;
	
	switch (cmd) {
	default:
		return -ENOTTY;
	case WDIOC_GETSUPPORT:
		if(copy_to_user((struct watchdog_info *)arg, &ident, 
				sizeof(ident)))
			return -EFAULT;
		return 0;
	case WDIOC_GETSTATUS:
	case WDIOC_GETBOOTSTATUS:
		if (put_user(0, (int *)arg))
			return -EFAULT;
		return 0;
	case WDIOC_KEEPALIVE:
		ar7_wdt_kick(1);
		return 0;
	case WDIOC_SETTIMEOUT:
		if (get_user(new_margin, (int *)arg))
			return -EFAULT;
		if (new_margin < 1)
			return -EINVAL;

		ar7_wdt_update_margin(new_margin);
		ar7_wdt_kick(1);

	case WDIOC_GETTIMEOUT:
		if (put_user(margin, (int *)arg))
			return -EFAULT;
		return 0;
	}
}

static struct file_operations ar7_wdt_fops = {
	.owner	 = THIS_MODULE,
	.write   = ar7_wdt_write,
	.ioctl   = ar7_wdt_ioctl,
	.open    = ar7_wdt_open,
	.release = ar7_wdt_release,
};

static struct miscdevice ar7_wdt_miscdev = {
	.minor = WATCHDOG_MINOR,
	.name  = "watchdog",
	.fops  = &ar7_wdt_fops,
};

static int __init ar7_wdt_init(void)
{
	int rc;

	if (!request_mem_region(AR7_REGS_WDT, sizeof(ar7_wdt_t), LONGNAME)) {
		printk(KERN_WARNING DRVNAME ": watchdog I/O region busy\n");
		return -EBUSY;
	}

	ar7_wdt_disable_wdt();
	ar7_wdt_prescale(prescale_value);
	ar7_wdt_update_margin(margin);

	sema_init(&open_semaphore, 1);

	rc = misc_register(&ar7_wdt_miscdev);
	if (rc) {
                printk(KERN_ERR DRVNAME ": unable to register misc device\n");
		goto out_alloc;
	}

	rc = register_reboot_notifier(&ar7_wdt_notifier);
        if (rc) {
                printk(KERN_ERR DRVNAME ": unable to register reboot notifier\n");
		goto out_register;
        }
	goto out;

out_register:
	misc_deregister(&ar7_wdt_miscdev);
out_alloc:
	release_mem_region(AR7_REGS_WDT, sizeof(ar7_wdt_t));
out:
	return rc;
}

static void __exit ar7_wdt_cleanup(void)
{
        unregister_reboot_notifier(&ar7_wdt_notifier);
	misc_deregister(&ar7_wdt_miscdev);
	release_mem_region(AR7_REGS_WDT, sizeof(ar7_wdt_t));
}

module_init(ar7_wdt_init);
module_exit(ar7_wdt_cleanup);
