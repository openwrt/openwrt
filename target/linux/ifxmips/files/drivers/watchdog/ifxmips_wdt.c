/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 *
 *   Copyright (C) 2006 infineon
 *   Copyright (C) 2007 John Crispin <blogic@openwrt.org> 
 *
 */

#include <asm/uaccess.h>
#include <linux/errno.h>
#include <linux/proc_fs.h>
#include <linux/ioctl.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/watchdog.h>
#include <linux/miscdevice.h>
#include <asm-mips/ifxmips/ifxmips_wdt.h>
#include <asm-mips/ifxmips/ifxmips.h>

#define DRVNAME			"ifxmips_wdt"

// TODO remove magic numbers and weirdo macros
extern unsigned int ifxmips_get_fpi_hz (void);

static int ifxmips_wdt_inuse = 0;
static int ifxmips_wdt_major = 0;

int
ifxmips_wdt_enable (unsigned int timeout)
{
	unsigned int wdt_cr = 0;
	unsigned int wdt_reload = 0;
	unsigned int wdt_clkdiv, wdt_pwl, ffpi;
	int retval = 0;

	/* clock divider & prewarning limit */
	wdt_clkdiv = 1 << (7 * IFXMIPS_BIU_WDT_CR_CLKDIV_GET(readl(IFXMIPS_BIU_WDT_CR)));
	wdt_pwl = 0x8000 >> IFXMIPS_BIU_WDT_CR_PWL_GET(readl(IFXMIPS_BIU_WDT_CR));

	//TODO
	printk("WARNING FUNCTION CALL MISSING!!!");
	//ffpi = cgu_get_io_region_clock();
	printk("cpu clock = %d\n", ffpi);

	/* caculate reload value */
	wdt_reload = (timeout * (ffpi / wdt_clkdiv)) + wdt_pwl;

	printk(KERN_WARNING DRVNAME ": wdt_pwl=0x%x, wdt_clkdiv=%d, ffpi=%d, wdt_reload = 0x%x\n",
		wdt_pwl, wdt_clkdiv, ffpi, wdt_reload);

	if (wdt_reload > 0xFFFF)
	{
		printk(KERN_WARNING DRVNAME ": timeout too large %d\n", timeout);
		retval = -EINVAL;
		goto out;
	}

	/* Write first part of password access */
	writel(IFXMIPS_BIU_WDT_CR_PW_SET(IFXMIPS_WDT_PW1), IFXMIPS_BIU_WDT_CR);

	wdt_cr = readl(IFXMIPS_BIU_WDT_CR);
	wdt_cr &= (!IFXMIPS_BIU_WDT_CR_PW_SET(0xff) &
		   !IFXMIPS_BIU_WDT_CR_PWL_SET(0x3) &
		   !IFXMIPS_BIU_WDT_CR_CLKDIV_SET(0x3) &
		   !IFXMIPS_BIU_WDT_CR_RELOAD_SET(0xffff));

	wdt_cr |= (IFXMIPS_BIU_WDT_CR_PW_SET(IFXMIPS_WDT_PW2) |
		   IFXMIPS_BIU_WDT_CR_PWL_SET(IFXMIPS_BIU_WDT_CR_PWL_GET(readl(IFXMIPS_BIU_WDT_CR))) |
		   IFXMIPS_BIU_WDT_CR_CLKDIV_SET(IFXMIPS_BIU_WDT_CR_CLKDIV_GET(readl(IFXMIPS_BIU_WDT_CR))) |
		   IFXMIPS_BIU_WDT_CR_RELOAD_SET(wdt_reload) |
		   IFXMIPS_BIU_WDT_CR_GEN);

	writel(wdt_cr, IFXMIPS_BIU_WDT_CR);

	printk("watchdog enabled\n");

out:
	return retval;
}

void
ifxmips_wdt_disable (void)
{
	writel(IFXMIPS_BIU_WDT_CR_PW_SET(IFXMIPS_WDT_PW1), IFXMIPS_BIU_WDT_CR);
	writel(IFXMIPS_BIU_WDT_CR_PW_SET(IFXMIPS_WDT_PW2), IFXMIPS_BIU_WDT_CR);

	printk("watchdog disabled\n");
}

/* passed LPEN or DSEN */
void
ifxmips_wdt_enable_feature (int en, int type)
{
	unsigned int wdt_cr = 0;

	writel(IFXMIPS_BIU_WDT_CR_PW_SET(IFXMIPS_WDT_PW1), IFXMIPS_BIU_WDT_CR);

	wdt_cr = readl(IFXMIPS_BIU_WDT_CR);

	if (en)
	{
		wdt_cr &= (~IFXMIPS_BIU_WDT_CR_PW_SET(0xff));
		wdt_cr |= (IFXMIPS_BIU_WDT_CR_PW_SET(IFXMIPS_WDT_PW2) | type);
	} else {
		wdt_cr &= (~IFXMIPS_BIU_WDT_CR_PW_SET(0xff) & ~type);
		wdt_cr |= IFXMIPS_BIU_WDT_CR_PW_SET(IFXMIPS_WDT_PW2);
	}

	writel(wdt_cr, IFXMIPS_BIU_WDT_CR);
}

void
ifxmips_wdt_prewarning_limit (int pwl)
{
	unsigned int wdt_cr = 0;

	wdt_cr = readl(IFXMIPS_BIU_WDT_CR);
	writel(IFXMIPS_BIU_WDT_CR_PW_SET(IFXMIPS_WDT_PW1), IFXMIPS_BIU_WDT_CR);

	wdt_cr &= 0xf300ffff;
	wdt_cr |= (IFXMIPS_BIU_WDT_CR_PW_SET(IFXMIPS_WDT_PW2) | IFXMIPS_BIU_WDT_CR_PWL_SET(pwl));

	/* Set reload value in second password access */
	writel(wdt_cr, IFXMIPS_BIU_WDT_CR);
}

void
ifxmips_wdt_set_clkdiv (int clkdiv)
{
	unsigned int wdt_cr = 0;

	wdt_cr = readl(IFXMIPS_BIU_WDT_CR);
	writel(IFXMIPS_BIU_WDT_CR_PW_SET(IFXMIPS_WDT_PW1), IFXMIPS_BIU_WDT_CR);

	wdt_cr &= 0xfc00ffff;
	wdt_cr |= (IFXMIPS_BIU_WDT_CR_PW_SET(IFXMIPS_WDT_PW2) | IFXMIPS_BIU_WDT_CR_CLKDIV_SET(clkdiv));

	/* Set reload value in second password access */
	writel(wdt_cr, IFXMIPS_BIU_WDT_CR);
}

static int
ifxmips_wdt_ioctl (struct inode *inode, struct file *file, unsigned int cmd,
	   unsigned long arg)
{
	int result = 0;
	static int timeout = -1;
	unsigned int user_arg;

	if ((cmd != IFXMIPS_WDT_IOC_STOP) && (cmd != IFXMIPS_WDT_IOC_PING) && (cmd != IFXMIPS_WDT_IOC_GET_STATUS))
	{
		if (copy_from_user((void *) &user_arg, (void *) arg, sizeof (int))){
			result = -EINVAL;
			goto out;
		}
	}

	switch (cmd)
	{
	case IFXMIPS_WDT_IOC_START:
		if ((result = ifxmips_wdt_enable(user_arg)) < 0)
			timeout = -1;
		else
			timeout = user_arg;
		break;

	case IFXMIPS_WDT_IOC_STOP:
		printk(KERN_INFO DRVNAME ": disable watch dog timer\n");
		ifxmips_wdt_disable();
		break;

	case IFXMIPS_WDT_IOC_PING:
		if (timeout < 0)
			result = -EIO;
		else
			result = ifxmips_wdt_enable(timeout);
		break;

	case IFXMIPS_WDT_IOC_GET_STATUS:
		user_arg = readl(IFXMIPS_BIU_WDT_SR);
		copy_to_user((int*)arg, (int*)&user_arg, sizeof(int));
		break;

	case IFXMIPS_WDT_IOC_SET_PWL:
		ifxmips_wdt_prewarning_limit(user_arg);
		break;

	case IFXMIPS_WDT_IOC_SET_DSEN:
		ifxmips_wdt_enable_feature(user_arg, IFXMIPS_BIU_WDT_CR_DSEN);
		break;

	case IFXMIPS_WDT_IOC_SET_LPEN:
		ifxmips_wdt_enable_feature(user_arg, IFXMIPS_BIU_WDT_CR_LPEN);
		break;

	case IFXMIPS_WDT_IOC_SET_CLKDIV:
		ifxmips_wdt_set_clkdiv(user_arg);
		break;

	default:
		printk(KERN_WARNING DRVNAME ": unknown watchdog iotcl\n");
	}

out:
	return result;
}

static int
ifxmips_wdt_open (struct inode *inode, struct file *file)
{
	if (ifxmips_wdt_inuse)
		return -EBUSY;

	ifxmips_wdt_inuse = 1;

	return 0;
}

static int
ifxmips_wdt_release (struct inode *inode, struct file *file)
{
	ifxmips_wdt_inuse = 0;

	return 0;
}

int
ifxmips_wdt_register_proc_read (char *buf, char **start, off_t offset, int count,
			int *eof, void *data)
{
	int len = 0;

	len += sprintf (buf + len, "IFXMIPS_BIU_WDT_PROC_READ\n");
	len += sprintf (buf + len, "IFXMIPS_BIU_WDT_CR(0x%08x)	: 0x%08x\n",
			(unsigned int)IFXMIPS_BIU_WDT_CR, readl(IFXMIPS_BIU_WDT_CR));
	len += sprintf (buf + len, "IFXMIPS_BIU_WDT_SR(0x%08x)	: 0x%08x\n",
			(unsigned int)IFXMIPS_BIU_WDT_SR, readl(IFXMIPS_BIU_WDT_SR));

	*eof = 1;

	return len;
}

static const struct file_operations ifxmips_wdt_fops = {
	.owner		= THIS_MODULE,
	.llseek		= no_llseek,
	.ioctl		= ifxmips_wdt_ioctl,
	.open		= ifxmips_wdt_open,
	.release	= ifxmips_wdt_release,
//	.write		= at91_wdt_write,
};

static struct miscdevice ifxmips_wdt_miscdev = {
	.minor		= WATCHDOG_MINOR,
	.name		= "ifxmips_wdt",
	.fops		= &ifxmips_wdt_fops,
};


static int
ifxmips_wdt_probe (struct platform_device *pdev)
{
	int ret = misc_register(&ifxmips_wdt_miscdev);
	if (ret)
		return ret;

	create_proc_read_entry(DRVNAME, 0, NULL, ifxmips_wdt_register_proc_read, NULL);

	printk(KERN_INFO DRVNAME ": ifxmips watchdog loaded\n");

	return 0;
}

static int
ifxmips_wdt_remove (struct platform_device *pdev)
{
	misc_deregister(&ifxmips_wdt_miscdev);
	remove_proc_entry(DRVNAME, NULL);
	return 0;
}

static struct
platform_driver ifxmips_wdt_driver = {
	.probe = ifxmips_wdt_probe,
	.remove = ifxmips_wdt_remove,
	.driver = {
		.name = DRVNAME,
		.owner = THIS_MODULE,
	},
};

int __init
ifxmips_wdt_init_module (void)
{
	int ret = platform_driver_register(&ifxmips_wdt_driver);
	if (ret)
		printk(KERN_INFO DRVNAME ": Error registering platfom driver!");
	return ret;
}

void
ifxmips_wdt_cleanup_module (void)
{
	platform_driver_unregister(&ifxmips_wdt_driver);
}

module_init(ifxmips_wdt_init_module);
module_exit(ifxmips_wdt_cleanup_module);

MODULE_AUTHOR("John Crispin <blogic@openwrt.org>");
MODULE_DESCRIPTION("Watchdog driver for infineon ifxmips family");
MODULE_LICENSE("GPL");
MODULE_ALIAS_MISCDEV(WATCHDOG_MINOR);
