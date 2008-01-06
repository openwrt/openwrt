/*
 * GPIO char driver
 *
 * Copyright 2005, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id$
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>

#include <typedefs.h>
#include <osl.h>
#include <sbutils.h>
#include <bcmdevs.h>

static sb_t *gpio_sbh;
static int gpio_major;
static devfs_handle_t gpio_dir;
static struct {
	char *name;
	devfs_handle_t handle;
} gpio_file[] = {
	{ "in", NULL },
	{ "out", NULL },
	{ "outen", NULL },
	{ "control", NULL }
};

static int
gpio_open(struct inode *inode, struct file * file)
{
	if (MINOR(inode->i_rdev) > ARRAYSIZE(gpio_file))
		return -ENODEV;

	MOD_INC_USE_COUNT;
	return 0;
}

static int
gpio_release(struct inode *inode, struct file * file)
{
	MOD_DEC_USE_COUNT;
	return 0;
}

static ssize_t
gpio_read(struct file *file, char *buf, size_t count, loff_t *ppos)
{
	u32 val;

	switch (MINOR(file->f_dentry->d_inode->i_rdev)) {
	case 0:
		val = sb_gpioin(gpio_sbh);
		break;
	case 1:
		val = sb_gpioout(gpio_sbh, 0, 0, GPIO_DRV_PRIORITY);
		break;
	case 2:
		val = sb_gpioouten(gpio_sbh, 0, 0, GPIO_DRV_PRIORITY);
		break;
	case 3:
		val = sb_gpiocontrol(gpio_sbh, 0, 0, GPIO_DRV_PRIORITY);
		break;
	default:
		return -ENODEV;
	}

	if (put_user(val, (u32 *) buf))
		return -EFAULT;

	return sizeof(val);
}

static ssize_t
gpio_write(struct file *file, const char *buf, size_t count, loff_t *ppos)
{
	u32 val;

	if (get_user(val, (u32 *) buf))
		return -EFAULT;

	switch (MINOR(file->f_dentry->d_inode->i_rdev)) {
	case 0:
		return -EACCES;
	case 1:
		sb_gpioout(gpio_sbh, ~0, val, GPIO_DRV_PRIORITY);
		break;
	case 2:
		sb_gpioouten(gpio_sbh, ~0, val, GPIO_DRV_PRIORITY);
		break;
	case 3:
		sb_gpiocontrol(gpio_sbh, ~0, val, GPIO_DRV_PRIORITY);
		break;
	default:
		return -ENODEV;
	}

	return sizeof(val);
}

static struct file_operations gpio_fops = {
	owner:		THIS_MODULE,
	open:		gpio_open,
	release:	gpio_release,
	read:		gpio_read,
	write:		gpio_write,
};

static int __init
gpio_init(void)
{
	int i;

	if (!(gpio_sbh = sb_kattach(SB_OSH)))
		return -ENODEV;

	sb_gpiosetcore(gpio_sbh);

	if ((gpio_major = devfs_register_chrdev(0, "gpio", &gpio_fops)) < 0)
		return gpio_major;

	gpio_dir = devfs_mk_dir(NULL, "gpio", NULL);

	for (i = 0; i < ARRAYSIZE(gpio_file); i++) {
		gpio_file[i].handle = devfs_register(gpio_dir,
						     gpio_file[i].name,
						     DEVFS_FL_DEFAULT, gpio_major, i,
						     S_IFCHR | S_IRUGO | S_IWUGO,
						     &gpio_fops, NULL);
	}

	return 0;
}

static void __exit
gpio_exit(void)
{
	int i;

	for (i = 0; i < ARRAYSIZE(gpio_file); i++)
		devfs_unregister(gpio_file[i].handle);
	devfs_unregister(gpio_dir);
	devfs_unregister_chrdev(gpio_major, "gpio");
	sb_detach(gpio_sbh);
}

module_init(gpio_init);
module_exit(gpio_exit);
