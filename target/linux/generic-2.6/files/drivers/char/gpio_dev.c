/*
 * character device wrapper for generic gpio layer
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA02111-1307USA
 *
 * Feedback, Bugs...  blogic@openwrt.org
 *
 * dpg 20100106
 */

#include <linux/module.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/gpio.h>
#include <asm/atomic.h>
#include <linux/init.h>
#include <linux/genhd.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/gpio_dev.h>

#define DRVNAME		"gpiodev"
#define DEVNAME		"gpio"

static int dev_major;
static struct class *gpiodev_class;


/* third argument of user space ioctl ('arg' here) contains the <pin> */
static int
gpio_ioctl(struct inode * inode, struct file * file, unsigned int cmd,
	   unsigned long arg)
{
	int retval = 0;

	switch (cmd)
	{
	case GPIO_GET:
		retval = gpio_get_value(arg);
		break;
	case GPIO_SET:
		gpio_set_value(arg, 1);
		break;
	case GPIO_CLEAR:
		gpio_set_value(arg, 0);
		break;
	case GPIO_DIR_IN:
		retval = gpio_direction_input(arg);
		break;
	case GPIO_DIR_OUT:
		retval = gpio_direction_output(arg, 0);
		break;
	case GPIO_DIR_HIGH:
		retval = gpio_direction_output(arg, 1);
		break;
	case GPIO_REQUEST:
		/* should be first ioctl operation on <pin> */
		retval = gpio_request(arg, DRVNAME);
		break;
	case GPIO_FREE:
		/* should be last ioctl operation on <pin> */
		/* may be needed first if previous user missed this ioctl */
		gpio_free(arg);
		break;
	case GPIO_CAN_SLEEP:
		retval = gpio_cansleep(arg);
		break;
	default:
		retval = -EINVAL;
		/* = -ENOTTY; // correct return but ... */
		break;
	}
	return retval;
}

/* Allow co-incident opens */
static int
gpio_open(struct inode *inode, struct file *file)
{
	int result = 0;
	unsigned int dev_minor = MINOR(inode->i_rdev);

	if (dev_minor != 0)
	{
		printk(KERN_ERR DRVNAME ": trying to access unknown minor device -> %d\n", dev_minor);
		result = -ENODEV;
		goto out;
	}
out:
	return result;
}

static int
gpio_close(struct inode * inode, struct file * file)
{
	/* could track all <pin>s requested by this fd and gpio_free()
         * them here
	 */
	return 0;
}

struct file_operations gpio_fops = {
	ioctl:		gpio_ioctl,
	open:		gpio_open,
	release:	gpio_close
};

static int
gpio_probe(struct platform_device *dev)
{
	int result = 0;

	dev_major = register_chrdev(0, DEVNAME, &gpio_fops);
	if (!dev_major)
	{
		printk(KERN_ERR DRVNAME ": Error whilst opening %s \n", DEVNAME);
		result = -ENODEV;
		goto out;
	}
	gpiodev_class = class_create(THIS_MODULE, DRVNAME);
	device_create(gpiodev_class, NULL, MKDEV(dev_major, 0), dev, DEVNAME);
	printk(KERN_INFO DRVNAME ": gpio device registered with major %d\n", dev_major);
out:
	return result;
}

static int
gpio_remove(struct platform_device *dev)
{
	unregister_chrdev(dev_major, DEVNAME);
	return 0;
}

static struct
platform_driver gpio_driver = {
	.probe = gpio_probe,
	.remove = gpio_remove,
	.driver = {
		.name = "GPIODEV",
		.owner = THIS_MODULE,
	},
};

static int __init
gpio_mod_init(void)
{
	int ret = platform_driver_register(&gpio_driver);
	if (ret)
		printk(KERN_INFO DRVNAME ": Error registering platfom driver!\n");

	return ret;
}

static void __exit
gpio_mod_exit(void)
{
	platform_driver_unregister(&gpio_driver);
}

module_init (gpio_mod_init);
module_exit (gpio_mod_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("John Crispin / OpenWrt +");
MODULE_DESCRIPTION("Character device for for generic gpio api");
