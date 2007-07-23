/*
 * Infineon AP DC COM  Amazon WDT driver
 * Copyright 2004 Wu Qi Ming <gokimi@msn.com>
 * All rights reserved
 */
#if defined(MODVERSIONS)
#include <linux/modversions.h>
#endif

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
#include <asm/amazon/amazon.h>
#include <asm/amazon/amazon_wdt.h>

#define AMAZON_WDT_EMSG(fmt, args...) printk( "%s: " fmt, __FUNCTION__ , ##args)

extern unsigned int amazon_get_fpi_hz(void);

/* forward declarations for _fops */
static ssize_t wdt_read(struct file *file, char *buf, size_t count, loff_t *offset);
static ssize_t wdt_write(struct file *file, const char *buf, size_t count, loff_t *offset);
static int wdt_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg);
static int wdt_open(struct inode *inode, struct file *file);
static int wdt_release(struct inode *inode, struct file *file);
static int wdt_proc_read(char *buf, char **start, off_t offset,int count, int *eof, void *data);


static struct file_operations wdt_fops = {
	read:wdt_read,
	write:wdt_write,
	ioctl:wdt_ioctl,
	open:wdt_open,
	release:wdt_release,	
};

/* data */
static struct wdt_dev *amazon_wdt_dev;
static struct proc_dir_entry* amazon_wdt_dir;
static int occupied=0;


/* Brief: enable WDT
 * Parameter:
 	timeout: time interval for WDT
 * Return:
 	0	OK
		EINVAL
 * Describes:
 	1. Password Access
	2. Modify Access (change ENDINIT => 0)
	3. Change WDT_CON1 (enable WDT)
	4. Password Access again
	5. Modify Access (change ENDINIT => 1)
 */
int wdt_enable(int timeout)
{
  	u32 hard_psw,ffpi;
  	int reload_value, divider=0;
  
	ffpi = amazon_get_fpi_hz();
	
  	divider = 1;
  	if((reload_value=65536-timeout*ffpi/256)<0){
        	divider = 0;
        	reload_value=65536-timeout*ffpi/16384;
  	}
	if (reload_value < 0){
		AMAZON_WDT_EMSG("timeout too large %d\n", timeout);
		return -EINVAL;
	}
	
	AMAZON_WDT_EMSG("timeout:%d reload_value: %8x\n", timeout, reload_value);
  	
	hard_psw=(AMAZON_WDT_REG32(AMAZON_WDT_CON0)&0xffffff01)+(AMAZON_WDT_REG32(AMAZON_WDT_CON1)&0xc)+ 0xf0;  
  	AMAZON_WDT_REG32(AMAZON_WDT_CON0)=hard_psw;
  	wmb();
	
  	AMAZON_WDT_REG32(AMAZON_WDT_CON0)=(hard_psw&0xff00)+(reload_value<<16)+0xf2;
  	wmb();

  	AMAZON_WDT_REG32(AMAZON_WDT_CON1)=divider<<2;  
	wmb();
	
  	hard_psw=(AMAZON_WDT_REG32(AMAZON_WDT_CON0)&0xffffff01)+(AMAZON_WDT_REG32(AMAZON_WDT_CON1)&0xc)+ 0xf0;
  	AMAZON_WDT_REG32(AMAZON_WDT_CON0)=hard_psw;
  	
	wmb();
  	AMAZON_WDT_REG32(AMAZON_WDT_CON0)=(AMAZON_WDT_REG32(AMAZON_WDT_CON0)&0xffffff00)+0xf3;
	wmb();
	return 0;
}

/*	Brief:	Disable/stop WDT
 */
void wdt_disable(void)
{
     	u32 hard_psw=0;
	
     	hard_psw=(AMAZON_WDT_REG32(AMAZON_WDT_CON0)&0xffffff01)+(AMAZON_WDT_REG32(AMAZON_WDT_CON1)&0xc)+ 0xf0;  
     	AMAZON_WDT_REG32(AMAZON_WDT_CON0)=hard_psw;
     	wmb();

     	AMAZON_WDT_REG32(AMAZON_WDT_CON0)=(AMAZON_WDT_REG32(AMAZON_WDT_CON0)&0xffffff00)+0xf2;
	wmb();
	
     	AMAZON_WDT_REG32(AMAZON_WDT_CON1)|=8;  
	wmb();
	
     	hard_psw=(AMAZON_WDT_REG32(AMAZON_WDT_CON0)&0xffffff01)+(AMAZON_WDT_REG32(AMAZON_WDT_CON1)&0xc)+ 0xf0;  
     	AMAZON_WDT_REG32(AMAZON_WDT_CON0)=hard_psw;
     	wmb();

     	AMAZON_WDT_REG32(AMAZON_WDT_CON0)=(AMAZON_WDT_REG32(AMAZON_WDT_CON0)&0xffffff00)+0xf3;
	wmb();
	
   	return;
}

static int wdt_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
        int result=0;
	static int timeout=-1;
        
        switch(cmd){
         case AMAZON_WDT_IOC_START:
          	AMAZON_WDT_DMSG("enable watch dog timer!\n");
		if ( copy_from_user((void*)&timeout, (void*)arg, sizeof (int)) ){
			AMAZON_WDT_EMSG("invalid argument\n");
			result=-EINVAL;
		}else{
			if ((result = wdt_enable(timeout)) < 0){
				timeout = -1;
			}
		}
          	break;
        case AMAZON_WDT_IOC_STOP:
          	AMAZON_WDT_DMSG("disable watch dog timer\n");
		timeout = -1;
          	wdt_disable();
		
         	break;
	case AMAZON_WDT_IOC_PING:
		if (timeout <0 ){
			result = -EIO;
		}else{
			result = wdt_enable(timeout); 
		}		
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
        AMAZON_WDT_DMSG("wdt_open\n");
	
	if (occupied == 1) return -EBUSY;
	occupied = 1;
	
	return 0;
}

static int wdt_release(struct inode *inode, struct file *file)
{
        AMAZON_WDT_DMSG("wdt_release\n"); 
	
	occupied = 0;
	return 0;
}


int wdt_register_proc_read(char *buf, char **start, off_t offset,
                         int count, int *eof, void *data)
{
   	int len=0;
   	printk("wdt_registers:\n");
   	len+=sprintf(buf+len,"NMISR:    0x%08x\n",AMAZON_WDT_REG32(AMAZON_WDT_NMISR));
   	len+=sprintf(buf+len,"RST_REQ:  0x%08x\n",AMAZON_WDT_REG32(AMAZON_RST_REQ));
   	len+=sprintf(buf+len,"RST_SR:   0x%08x\n",AMAZON_WDT_REG32(AMAZON_RST_SR));
   	len+=sprintf(buf+len,"WDT_CON0: 0x%08x\n",AMAZON_WDT_REG32(AMAZON_WDT_CON0));
   	len+=sprintf(buf+len,"WDT_CON1: 0x%08x\n",AMAZON_WDT_REG32(AMAZON_WDT_CON1));
   	len+=sprintf(buf+len,"WDT_SR:   0x%08x\n",AMAZON_WDT_REG32(AMAZON_WDT_SR));
   	*eof = 1;
   	return len;
}


int __init amazon_wdt_init_module(void)
{
	int result=0;

   	amazon_wdt_dev = (wdt_dev*)kmalloc(sizeof(wdt_dev),GFP_KERNEL);
   	if (amazon_wdt_dev == NULL){
   		return -ENOMEM;
   	}
	memset(amazon_wdt_dev,0,sizeof(wdt_dev));
	
      	amazon_wdt_dev->major=result;
	strcpy(amazon_wdt_dev->name,"wdt");
	
	result = register_chrdev(0,amazon_wdt_dev->name,&wdt_fops);
   	if (result < 0) {
        	AMAZON_WDT_EMSG("cannot register device\n");
		kfree(amazon_wdt_dev);
       	 	return result;
   	}

   	amazon_wdt_dir=proc_mkdir("amazon_wdt",NULL);
   	create_proc_read_entry("wdt_register",
                          	0,
                          	amazon_wdt_dir,
                          	wdt_register_proc_read,
                          	NULL);  
   
	occupied=0;
   	return 0;
}

void amazon_wdt_cleanup_module(void)
{
	unregister_chrdev(amazon_wdt_dev->major,amazon_wdt_dev->name);
        kfree(amazon_wdt_dev);	
    	remove_proc_entry("wdt_register",amazon_wdt_dir);
	remove_proc_entry("amazon_wdt",NULL);
        AMAZON_WDT_DMSG("unloaded\n");
	return;
}

MODULE_LICENSE ("GPL");
MODULE_AUTHOR("Infineon IFAP DC COM");
MODULE_DESCRIPTION("AMAZON WDT driver");

module_init(amazon_wdt_init_module);
module_exit(amazon_wdt_cleanup_module);

