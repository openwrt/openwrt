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
 * Copyright 2004 Wu Qi Ming <gokimi@msn.com>
 * Copyright (C) 2007 John Crispin <blogic@openwrt.org>
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/timer.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/proc_fs.h>
#include <linux/stat.h>
#include <linux/tty.h>
#include <linux/selection.h>
#include <linux/kmod.h>
#include <linux/vmalloc.h>
#include <linux/kdev_t.h>
#include <linux/ioctl.h>
#include <asm/uaccess.h>
#include <asm/system.h>
#include <linux/platform_device.h>
#include <asm/amazon/amazon.h>
#include <asm/amazon/amazon_wdt.h>

#define DRV_NAME "AMAZON WDT:"

#undef AMAZON_WDT_DEBUG 

extern unsigned int amazon_get_fpi_hz(void);
static int amazon_wdt_isopen = 0;

#ifdef AMAZON_WDT_DEBUG
static struct proc_dir_entry* amazon_wdt_dir;
#endif

int wdt_enable(int timeout)
{
  	u32 hard_psw, ffpi;
  	int reload_value, divider = 1;
  
	ffpi = amazon_get_fpi_hz();

	reload_value = 65536 - timeout * ffpi / 256;
  	
	if (reload_value < 0) {
		divider = 0;
		reload_value = 65536 - timeout * ffpi / 16384;
  	}

	if (reload_value < 0){
		printk(KERN_INFO DRV_NAME "timeout too large %d\n", timeout);
		return -EINVAL;
	}
	
	printk(KERN_INFO DRV_NAME "timeout:%d reload_value: %8x\n", timeout, reload_value);
  	
	hard_psw = (amazon_readl(AMAZON_WDT_CON0) & 0xffffff01) +
				(amazon_readl(AMAZON_WDT_CON1) & 0xc) + 0xf0;  
  	amazon_writel(hard_psw, AMAZON_WDT_CON0);
  	wmb();
	
	amazon_writel((hard_psw & 0xff00) + (reload_value << 16) + 0xf2, AMAZON_WDT_CON0);
  	wmb();

  	amazon_writel(divider << 2, AMAZON_WDT_CON1);  
	wmb();
	
  	hard_psw = (amazon_readl(AMAZON_WDT_CON0) & 0xffffff01) +
				(amazon_readl(AMAZON_WDT_CON1) & 0xc) + 0xf0;
  	amazon_writel(hard_psw, AMAZON_WDT_CON0);
	wmb();
  	
	amazon_writel_masked(AMAZON_WDT_CON0, 0xff, 0xf3);
	wmb();
	return 0;
}

void wdt_disable(void)
{
	u32 hard_psw = 0;

	hard_psw = (amazon_readl(AMAZON_WDT_CON0) & 0xffffff01) + 
			(amazon_readl(AMAZON_WDT_CON1) & 0xc) + 0xf0;  
	amazon_writel(hard_psw, AMAZON_WDT_CON0);
	wmb();

	amazon_writel_masked(AMAZON_WDT_CON0, 0xff, 0xf2);
	wmb();

	amazon_writel_masked(AMAZON_WDT_CON1, 0x8, 0x8);  
	wmb();

	hard_psw=(amazon_readl(AMAZON_WDT_CON0) & 0xffffff01) +
			(amazon_readl(AMAZON_WDT_CON1) & 0xc) + 0xf0;  
	amazon_writel(hard_psw, AMAZON_WDT_CON0);
	wmb();

	amazon_writel_masked(AMAZON_WDT_CON0, 0xff, 0xf3);
	wmb();

   	return;
}

static int wdt_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
	int result=0;
	static int timeout=-1;

	switch(cmd){
		case AMAZON_WDT_IOC_START:
			printk(KERN_INFO DRV_NAME "enable watch dog timer!\n");
			if (copy_from_user((void*)&timeout, (void*)arg, sizeof (int))) {
				printk(KERN_INFO DRV_NAME "invalid argument\n");
				result=-EINVAL;
			} else if ((result = wdt_enable(timeout)) < 0) {
				timeout = -1;
			}
			break;
		
		case AMAZON_WDT_IOC_STOP:
			printk(KERN_INFO DRV_NAME "disable watch dog timer\n");
			timeout = -1;
			wdt_disable();
			break;
		
		case AMAZON_WDT_IOC_PING:
			if (timeout < 0) {
				result = -EIO;
			} else {
				result = wdt_enable(timeout); 
			}		
			break;
		
		default:
			result=-EINVAL;
			break;
	}
	return result;
}

static ssize_t wdt_read(struct file *file, char *buf, size_t count, loff_t *offset)
{
	return 0;
}

static ssize_t wdt_write(struct file *file, const char *buf, size_t count, loff_t *offset)
{	
	return count;
}

static int wdt_open(struct inode *inode, struct file *file)
{
	if (amazon_wdt_isopen == 1)
		return -EBUSY;

	amazon_wdt_isopen = 1;
	printk(KERN_INFO DRV_NAME "opened\n");	
	return 0;
}

static int wdt_release(struct inode *inode, struct file *file)
{
	amazon_wdt_isopen = 0;
	printk(KERN_INFO DRV_NAME "closed\n");	
	return 0;
}

#ifdef AMAZON_WDT_DEBUG
int wdt_register_proc_read(char *buf, char **start, off_t offset,
                         int count, int *eof, void *data)
{
   	int len=0;
   	len+=sprintf(buf+len,"NMISR:    0x%08x\n",AMAZON_WDT_REG32(AMAZON_WDT_NMISR));
   	len+=sprintf(buf+len,"RST_REQ:  0x%08x\n",AMAZON_WDT_REG32(AMAZON_RST_REQ));
   	len+=sprintf(buf+len,"RST_SR:   0x%08x\n",AMAZON_WDT_REG32(AMAZON_RST_SR));
   	len+=sprintf(buf+len,"WDT_CON0: 0x%08x\n",AMAZON_WDT_REG32(AMAZON_WDT_CON0));
   	len+=sprintf(buf+len,"WDT_CON1: 0x%08x\n",AMAZON_WDT_REG32(AMAZON_WDT_CON1));
   	len+=sprintf(buf+len,"WDT_SR:   0x%08x\n",AMAZON_WDT_REG32(AMAZON_WDT_SR));
   	*eof = 1;
   	return len;
}
#endif

static struct file_operations wdt_fops = {
	read:		wdt_read,
	write:		wdt_write,
	ioctl:		wdt_ioctl,
	open:		wdt_open,
	release:	wdt_release,	
};

static int __init amazon_wdt_probe(struct platform_device *dev)
{
	int result = result = register_chrdev(0, "watchdog", &wdt_fops);
   	
	if (result < 0) {
        printk(KERN_INFO DRV_NAME "cannot register device\n");
       	return result;
   	}

#ifdef AMAZON_WDT_DEBUG
	amazon_wdt_dir=proc_mkdir("amazon_wdt",NULL);
   	create_proc_read_entry("wdt_register", 0, amazon_wdt_dir,
                          	wdt_register_proc_read, NULL);  
#endif

	amazon_wdt_isopen=0;
	printk(KERN_INFO DRV_NAME "driver loaded but inactive\n");
	return 0;
}

static int amazon_wdt_remove(struct platform_device *dev)
{
	unregister_chrdev(0, "watchdog");
#ifdef AMAZON_WDT_DEBUG
	remove_proc_entry("wdt_register", amazon_wdt_dir);
	remove_proc_entry("amazon_wdt", NULL);
#endif
	printk(KERN_INFO DRV_NAME "unregistered\n");
	return 0;
}

static struct platform_driver amazon_wdt_driver = {
	.probe = amazon_wdt_probe,
	.remove = amazon_wdt_remove,
	.driver = {
		.name = "amazon_wdt",
		.owner = THIS_MODULE,
	},
};

static int __init amazon_wdt_init(void)
{
	int ret = platform_driver_register(&amazon_wdt_driver);
	if (ret)
		printk(KERN_WARNING "amazon_wdt: error registering platfom driver!\n");
	return ret;
}

static void __exit amazon_wdt_exit(void)
{
	platform_driver_unregister(&amazon_wdt_driver);
}

module_init(amazon_wdt_init);
module_exit(amazon_wdt_exit);

MODULE_LICENSE ("GPL");
MODULE_AUTHOR("Infineon / John Crispin <blogic@openwrt.org>");
MODULE_DESCRIPTION("AMAZON WDT driver");

