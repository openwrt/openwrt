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
#include <asm-mips/danube/danube_wdt.h>
#include <asm-mips/danube/danube.h>


// TODO remove magic numbers and weirdo macros

extern unsigned int danube_get_fpi_hz (void);

static int danube_wdt_inuse = 0;
static int danube_wdt_major = 0;

int
danube_wdt_enable (unsigned int timeout)
{
	unsigned int wdt_cr = 0;
	unsigned int wdt_reload = 0;
	unsigned int wdt_clkdiv, wdt_pwl, ffpi;
	int retval = 0;

	/* clock divider & prewarning limit */
	wdt_clkdiv = 1 << (7 * DANUBE_BIU_WDT_CR_CLKDIV_GET(readl(DANUBE_BIU_WDT_CR)));
	wdt_pwl = 0x8000 >> DANUBE_BIU_WDT_CR_PWL_GET(readl(DANUBE_BIU_WDT_CR));

	//TODO
	printk("WARNING FUNCTION CALL MISSING!!!");
	//ffpi = cgu_get_io_region_clock();
	printk("cpu clock = %d\n", ffpi);

	/* caculate reload value */
	wdt_reload = (timeout * (ffpi / wdt_clkdiv)) + wdt_pwl;

	printk("wdt_pwl=0x%x, wdt_clkdiv=%d, ffpi=%d, wdt_reload = 0x%x\n",
		wdt_pwl, wdt_clkdiv, ffpi, wdt_reload);

	if (wdt_reload > 0xFFFF)
	{
		printk ("timeout too large %d\n", timeout);
		retval = -EINVAL;
		goto out;
	}

	/* Write first part of password access */
	writel(DANUBE_BIU_WDT_CR_PW_SET(DANUBE_WDT_PW1), DANUBE_BIU_WDT_CR);

	wdt_cr = readl(DANUBE_BIU_WDT_CR);
	wdt_cr &= (!DANUBE_BIU_WDT_CR_PW_SET(0xff) &
		   !DANUBE_BIU_WDT_CR_PWL_SET(0x3) &
		   !DANUBE_BIU_WDT_CR_CLKDIV_SET(0x3) &
		   !DANUBE_BIU_WDT_CR_RELOAD_SET(0xffff));

	wdt_cr |= (DANUBE_BIU_WDT_CR_PW_SET(DANUBE_WDT_PW2) |
		   DANUBE_BIU_WDT_CR_PWL_SET(DANUBE_BIU_WDT_CR_PWL_GET(readl(DANUBE_BIU_WDT_CR))) |
		   DANUBE_BIU_WDT_CR_CLKDIV_SET(DANUBE_BIU_WDT_CR_CLKDIV_GET(readl(DANUBE_BIU_WDT_CR))) |
		   DANUBE_BIU_WDT_CR_RELOAD_SET(wdt_reload) |
		   DANUBE_BIU_WDT_CR_GEN);

	writel(wdt_cr, DANUBE_BIU_WDT_CR);

	printk("watchdog enabled\n");

out:
	return retval;
}

void
danube_wdt_disable (void)
{
	writel(DANUBE_BIU_WDT_CR_PW_SET(DANUBE_WDT_PW1), DANUBE_BIU_WDT_CR);
	writel(DANUBE_BIU_WDT_CR_PW_SET(DANUBE_WDT_PW2), DANUBE_BIU_WDT_CR);

	printk("watchdog disabled\n");
}

/* passed LPEN or DSEN */
void
danube_wdt_enable_feature (int en, int type)
{
	unsigned int wdt_cr = 0;

	writel(DANUBE_BIU_WDT_CR_PW_SET(DANUBE_WDT_PW1), DANUBE_BIU_WDT_CR);

	wdt_cr = readl(DANUBE_BIU_WDT_CR);

	if (en)
	{
		wdt_cr &= (~DANUBE_BIU_WDT_CR_PW_SET(0xff));
		wdt_cr |= (DANUBE_BIU_WDT_CR_PW_SET(DANUBE_WDT_PW2) | type);
	} else {
		wdt_cr &= (~DANUBE_BIU_WDT_CR_PW_SET(0xff) & ~type);
		wdt_cr |= DANUBE_BIU_WDT_CR_PW_SET(DANUBE_WDT_PW2);
	}

	writel(wdt_cr, DANUBE_BIU_WDT_CR);
}

void
danube_wdt_prewarning_limit (int pwl)
{
	unsigned int wdt_cr = 0;

	wdt_cr = readl(DANUBE_BIU_WDT_CR);
	writel(DANUBE_BIU_WDT_CR_PW_SET(DANUBE_WDT_PW1), DANUBE_BIU_WDT_CR);

	wdt_cr &= 0xf300ffff;
	wdt_cr |= (DANUBE_BIU_WDT_CR_PW_SET(DANUBE_WDT_PW2) | DANUBE_BIU_WDT_CR_PWL_SET(pwl));

	/* Set reload value in second password access */
	writel(wdt_cr, DANUBE_BIU_WDT_CR);
}

void
danube_wdt_set_clkdiv (int clkdiv)
{
	unsigned int wdt_cr = 0;

	wdt_cr = readl(DANUBE_BIU_WDT_CR);
	writel(DANUBE_BIU_WDT_CR_PW_SET(DANUBE_WDT_PW1), DANUBE_BIU_WDT_CR);

	wdt_cr &= 0xfc00ffff;
	wdt_cr |= (DANUBE_BIU_WDT_CR_PW_SET(DANUBE_WDT_PW2) | DANUBE_BIU_WDT_CR_CLKDIV_SET(clkdiv));

	/* Set reload value in second password access */
	writel(wdt_cr, DANUBE_BIU_WDT_CR);
}

static int
danube_wdt_ioctl (struct inode *inode, struct file *file, unsigned int cmd,
	   unsigned long arg)
{
	int result = 0;
	static int timeout = -1;
	unsigned int user_arg;

	if ((cmd != DANUBE_WDT_IOC_STOP) && (cmd != DANUBE_WDT_IOC_PING) && (cmd != DANUBE_WDT_IOC_GET_STATUS))
	{
		if (copy_from_user((void *) &user_arg, (void *) arg, sizeof (int))){
			result = -EINVAL;
			goto out;
		}
	}

	switch (cmd)
	{
	case DANUBE_WDT_IOC_START:
		if ((result = danube_wdt_enable(user_arg)) < 0)
			timeout = -1;
		else
			timeout = user_arg;
		break;

	case DANUBE_WDT_IOC_STOP:
		printk("disable watch dog timer\n");
		danube_wdt_disable();
		break;

	case DANUBE_WDT_IOC_PING:
		if (timeout < 0)
			result = -EIO;
		else
			result = danube_wdt_enable(timeout);
		break;

	case DANUBE_WDT_IOC_GET_STATUS:
		user_arg = readl(DANUBE_BIU_WDT_SR);
		copy_to_user((int*)arg, (int*)&user_arg, sizeof(int));
		break;

	case DANUBE_WDT_IOC_SET_PWL:
		danube_wdt_prewarning_limit(user_arg);
		break;

	case DANUBE_WDT_IOC_SET_DSEN:
		danube_wdt_enable_feature(user_arg, DANUBE_BIU_WDT_CR_DSEN);
		break;

	case DANUBE_WDT_IOC_SET_LPEN:
		danube_wdt_enable_feature(user_arg, DANUBE_BIU_WDT_CR_LPEN);
		break;

	case DANUBE_WDT_IOC_SET_CLKDIV:
		danube_wdt_set_clkdiv(user_arg);
		break;

	default:
		printk("unknown watchdog iotcl\n");
	}

out:
	return result;
}

static int
danube_wdt_open (struct inode *inode, struct file *file)
{
	if (danube_wdt_inuse)
		return -EBUSY;

	danube_wdt_inuse = 1;

	return 0;
}

static int
danube_wdt_release (struct inode *inode, struct file *file)
{
	danube_wdt_inuse = 0;

	return 0;
}

int
danube_wdt_register_proc_read (char *buf, char **start, off_t offset, int count,
			int *eof, void *data)
{
	int len = 0;

	len += sprintf (buf + len, "DANUBE_BIU_WDT_PROC_READ\n");
	len += sprintf (buf + len, "DANUBE_BIU_WDT_CR(0x%08x)	: 0x%08x\n",
			(unsigned int)DANUBE_BIU_WDT_CR, readl(DANUBE_BIU_WDT_CR));
	len += sprintf (buf + len, "DANUBE_BIU_WDT_SR(0x%08x)	: 0x%08x\n",
			(unsigned int)DANUBE_BIU_WDT_SR, readl(DANUBE_BIU_WDT_SR));

	*eof = 1;

	return len;
}

static struct file_operations wdt_fops = {
      .owner = THIS_MODULE,
      .ioctl = danube_wdt_ioctl,
      .open = danube_wdt_open,
      .release = danube_wdt_release,
};

int __init
danube_wdt_init_module (void)
{
	danube_wdt_major = register_chrdev(0, "wdt", &wdt_fops);

	if (danube_wdt_major < 0)
	{
		printk("cannot register watchdog device\n");

		return -EINVAL;
	}

	create_proc_read_entry("danube_wdt", 0, NULL, danube_wdt_register_proc_read, NULL);

	printk("danube watchdog loaded\n");

	return 0;
}

void
danube_wdt_cleanup_module (void)
{
	unregister_chrdev(danube_wdt_major, "wdt");
	remove_proc_entry("danube_wdt", NULL);
}

module_init(danube_wdt_init_module);
module_exit(danube_wdt_cleanup_module);
