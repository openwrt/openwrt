/*
 *	ADM5120 LED (GPIO) driver
 *
 *	Copyright (C) Jeroen Vreeken (pe1rxq@amsat.org), 2005
 */

#include <linux/autoconf.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>

#define LED_MINOR 151
#define GPIO_IO        ((unsigned long *)0xb20000b8)

static ssize_t adm5120_led_write(struct file *file, const char __user *data,
	size_t len, loff_t *ppos)
{
	unsigned char val;

	if (!len || get_user(val, data))
		return -EFAULT;
	*GPIO_IO=(*GPIO_IO & 0x00ffffff) | (val<<24);
	return 1;
}

static struct file_operations adm5120_led_fops = {
	.owner	= THIS_MODULE,
	.write	= adm5120_led_write,
};

static struct miscdevice adm5120_led_device = {
	LED_MINOR,
	"led",
	&adm5120_led_fops,
};

static int __init adm5120_led_init(void)
{
	printk(KERN_INFO "ADM5120 LED & GPIO driver\n");
	if (misc_register(&adm5120_led_device)) {
		printk(KERN_WARNING "Couldn't register device %d\n", LED_MINOR);
		return -EBUSY;
	}
	return 0;
}

static void __exit adm5120_led_exit(void)
{
	misc_deregister(&adm5120_led_device);
}

module_init(adm5120_led_init);
module_exit(adm5120_led_exit);

MODULE_DESCRIPTION("ADM5120 LED and GPIO driver");
MODULE_AUTHOR("Jeroen Vreeken");
MODULE_LICENSE("GPL");
