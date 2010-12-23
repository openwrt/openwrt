/*
 *	Driver for ADC on Atmel AT91 SoC Family
 *
 *	Copyright (C) 2010 Claudio Mignanti - c.mignanti@gmail.com
 *	Based on http://www.at91.com/forum/viewtopic.php/p,9409/#p9409
 *
 *	Copyright (C) 2010 Stefano Barbato - stefano@codesink.org
 *
 *	2010/05/18 Antonio Galea
 *		Sysfs device model, different drivers integration
 *
 *	WISHLIST:
 *	- concurrent access control
 *	- add support for dynamic reconfiguration
 *	- hardware triggers
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as	published by
 *	the Free Software Foundation.
 *
 * ---------------------------------------------------------------------------
*/

#include <linux/cdev.h>
#include <linux/clk.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/kernel.h>
#include <linux/smp_lock.h>

#include <asm/io.h>

#include "at91_adc.h"

#define DRV_CLASS	"at91_adc"

#define ADC_REQUEST 		1	//un-used atm
#define ADC_READ		2
#define ADC_FREE		3

/* Device functions */
#define at91_adc_read(reg)				ioread32(at91_adc_base + (reg))
#define at91_adc_write(reg, val)	iowrite32((val), at91_adc_base + (reg))
#define AT91_DEFAULT_CONFIG			 AT91_ADC_SHTIM	 | \
																	AT91_ADC_STARTUP | \
																	AT91_ADC_PRESCAL | \
																	AT91_ADC_SLEEP

static void at91_adc_device_release(struct device *dev) {}

struct platform_device at91_adc_device = {
	.name					= "at91_adc",
	.id						= -1,
	.dev.release	 = at91_adc_device_release,
};

struct clk				*at91_adc_clk;
void __iomem				*at91_adc_base;
void __iomem				*at91_pioc_base;
static struct cdev			*at91_adc_cdev	= NULL;
static dev_t				at91_adc_devno 	= 0;
static struct class			*at91_adc_class	= NULL;

static int at91_adc_read_chan(int chan){
	int val, sr;

	if(chan<0 || chan>3){
		return -EINVAL;
	}
	/* disable pull-up resistor */
	iowrite32(1 << chan, at91_pioc_base + 0x60);

	at91_adc_write(AT91_ADC_CHER,AT91_ADC_CH(chan));	// Enable Channel
	at91_adc_write(AT91_ADC_CR,AT91_ADC_START);		//Start the ADC
	
	for(sr=0; !(sr & AT91_ADC_EOC(chan)); sr=at91_adc_read(AT91_ADC_SR))
		cpu_relax();

	val=at91_adc_read(AT91_ADC_CHR(chan)) & AT91_ADC_DATA; //Read up to 10 bits

	return val;
}

/* 	PC0 ->  AD0
	PC1 -> 	AD1
	PC2 -> 	AD2
	PC3 -> 	AD3 */
static int mux_chan (int chan, int operation) {

	int pin_chan;

	if(chan<0 || chan>3){
		return -EINVAL;
	}

	switch (chan) { 
		case 0:
			pin_chan=AT91_PIN_PC0;
			break;
		case 1:
			pin_chan=AT91_PIN_PC1;
			break;
		case 2:
			pin_chan=AT91_PIN_PC2;
			break;
		case 3:
			pin_chan=AT91_PIN_PC3;
			break;
		default:
			return -EINVAL;
	}

	if (operation == 1)		//request_chan
		at91_set_A_periph(pin_chan, 0);				//Mux PIN to GPIO
	else					//free_chan
		at91_set_B_periph(pin_chan, 0);				//Mux PIN to GPIO

	return 0;
}

static int at91_adc_config(int requested_config){
	int actual_config;

	at91_adc_write(AT91_ADC_CR,AT91_ADC_SWRST);	 //Reset the ADC
	at91_adc_write(AT91_ADC_MR,requested_config); //Mode setup
	actual_config = at91_adc_read(AT91_ADC_MR);	 //Read it back

	return (requested_config==actual_config? 0: -EINVAL);
}

/* Sysfs interface */
static ssize_t at91_adc_chanX_show(
	struct device *dev, struct device_attribute *attr, char *buf ){

	ssize_t status = 0;
	int		 chan = -1;
	int		 value;

	if(strlen(attr->attr.name)==5 && strncmp(attr->attr.name,"chan",4)==0){
		chan = attr->attr.name[4]-'0';
	}

	if(chan<0 || chan>3){
		return -EIO;
	}

	value	= at91_adc_read_chan(chan);
	status = sprintf(buf, "%d\n", value);

	return status;
}

static DEVICE_ATTR(chan0, 0444, at91_adc_chanX_show, NULL);
static DEVICE_ATTR(chan1, 0444, at91_adc_chanX_show, NULL);
static DEVICE_ATTR(chan2, 0444, at91_adc_chanX_show, NULL);
static DEVICE_ATTR(chan3, 0444, at91_adc_chanX_show, NULL);

static const struct attribute *at91_adc_dev_attrs[] = {
	&dev_attr_chan0.attr,
	&dev_attr_chan1.attr,
	&dev_attr_chan2.attr,
	&dev_attr_chan3.attr,
	NULL,
};

static const struct attribute_group at91_adc_dev_attr_group = {
	.attrs = (struct attribute **) at91_adc_dev_attrs,
};

/* IOCTL interface */
#ifdef HAVE_UNLOCKED_IOCTL
static long at91_adc_unlocked_ioctl(
	struct file *file, unsigned int cmd, unsigned long arg){
#else
static int at91_adc_ioctl(
	struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg){
#endif

	long retval = 0;

#ifdef HAVE_UNLOCKED_IOCTL
  lock_kernel();
#endif

	switch (cmd) {
		case ADC_REQUEST:
			retval = mux_chan ((int)arg, 1);
			break;

		case ADC_READ:
			retval = at91_adc_read_chan((int)arg);
			break;

		case ADC_FREE:
			retval = mux_chan ((int)arg, 0);
			break;

		default:
			retval = -EINVAL;
	}

#ifdef HAVE_UNLOCKED_IOCTL
  unlock_kernel();
#endif

	return retval;
}

struct file_operations at91_adc_fops = {
	.owner = THIS_MODULE,
#ifdef HAVE_UNLOCKED_IOCTL
	.unlocked_ioctl = at91_adc_unlocked_ioctl,
#else
	.ioctl = at91_adc_ioctl,
#endif
};

static void at91_adc_cdev_teardown(void){
	if(at91_adc_class){
		device_destroy(at91_adc_class, at91_adc_devno);
		class_destroy(at91_adc_class);
	}

	if(at91_adc_devno){
		unregister_chrdev_region(at91_adc_devno,1);
		if(at91_adc_cdev){ cdev_del(at91_adc_cdev); }
	}

	at91_adc_devno = 0;
	at91_adc_cdev	= NULL;
	at91_adc_class = NULL;
	return;
}

static int at91_adc_cdev_setup(void){

	int status;
	/* alloc a new device number (major: dynamic, minor: 0) */
	status = alloc_chrdev_region(&at91_adc_devno,0,1,at91_adc_device.name);

	if(status){
		goto err;
	}

	/* create a new char device */
	at91_adc_cdev = cdev_alloc();
	if(at91_adc_cdev == NULL){ status=-ENOMEM; goto err; }
	at91_adc_cdev->owner = THIS_MODULE;
	at91_adc_cdev->ops	 = &at91_adc_fops;
	status = cdev_add(at91_adc_cdev,at91_adc_devno,1);
	if(status){
		goto err;
	}
	
	/* register the class */
	at91_adc_class = class_create(THIS_MODULE, DRV_CLASS);
	if(IS_ERR(at91_adc_class)){ status=-EFAULT; goto err; }
	device_create(at91_adc_class, NULL, at91_adc_devno, NULL, at91_adc_device.name);
	printk(KERN_INFO "Major: %u; minor: %u\n", \
		MAJOR(at91_adc_devno), MINOR(at91_adc_devno) \
	);

	return 0;

err:
	at91_adc_cdev_teardown();
	return status;
}

/* Module init/exit */
static int __init at91_adc_init(void){

	int status;

	at91_adc_clk = clk_get(NULL,"adc_clk");
	clk_enable(at91_adc_clk);

	at91_adc_base = ioremap(AT91SAM9260_BASE_ADC,SZ_256);
	if(!at91_adc_base){
		status=-ENODEV;
		goto fail_no_iomem_adc;
	}

	at91_pioc_base = ioremap(AT91_BASE_SYS + AT91_PIOC,SZ_512);
	if(!at91_pioc_base){
		status=-ENODEV;
		goto fail_no_iomem_pioc;
	}

	status = platform_device_register(&at91_adc_device);
	if(status){
		goto fail_no_dev;
	}

	status = at91_adc_config(AT91_DEFAULT_CONFIG);
	if(status){
		goto fail_no_config;
	}

	status = sysfs_create_group(
		&(at91_adc_device.dev.kobj), &at91_adc_dev_attr_group
	);

	if(status){
		goto fail_no_sysfs;
	}

	status = at91_adc_cdev_setup();
	if(status){
		goto fail_no_cdev;
	}

	printk(KERN_INFO "Registered device at91_adc.\n");
	return 0;

fail_no_cdev:
fail_no_sysfs:
	// nothing to undo
fail_no_config:
	platform_device_unregister(&at91_adc_device);
fail_no_dev:
	iounmap(at91_adc_base);
fail_no_iomem_pioc:
	iounmap(at91_pioc_base);
fail_no_iomem_adc:
	clk_disable(at91_adc_clk);
	clk_put(at91_adc_clk);
	return status;
}

static void __exit at91_adc_exit(void){

	at91_adc_cdev_teardown();
	platform_device_unregister(&at91_adc_device);
	iounmap(at91_adc_base);
	iounmap(at91_pioc_base);

	clk_disable(at91_adc_clk);
	clk_put(at91_adc_clk);

	printk(KERN_INFO "Unregistered device at91_adc.\n");
}

module_init(at91_adc_init);
module_exit(at91_adc_exit);

MODULE_AUTHOR("Paul Kavan");
MODULE_AUTHOR("Claudio Mignanti");
MODULE_AUTHOR("Antonio Galea");
MODULE_AUTHOR("Stefano Barbato");
MODULE_DESCRIPTION("ADC Driver for the AT91SAM9G20");
MODULE_LICENSE("GPL");
