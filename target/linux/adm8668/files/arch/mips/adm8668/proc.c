/*
 * Copyright (C) 2010 Scott Nicholas <neutronscott@scottn.us>
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <adm8668.h>

int adm8668_sesled_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char buf[8];
	int num;

	num = (count < 8) ? count : 8;

	if (copy_from_user(buf, buffer, num))
	{
		printk("copy_from_user failed");
                return -EFAULT;
	}
	num = simple_strtoul(buf, NULL, 16);
	switch (num)
	{
		case 0:
			GPIO_SET_LOW(0);
			CRGPIO_SET_LOW(2);
			break;
		case 1:
			GPIO_SET_LOW(0);
			CRGPIO_SET_HI(2);
			break;
		case 2:
			GPIO_SET_HI(0);
			CRGPIO_SET_HI(2);
			break;
		default:
			break;
	}

	return count;
}

int adm8668_sesled_read_proc(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
	int len = 0;
	int led_state = 0;

	led_state = (ADM8668_CONFIG_REG(CRGPIO_REG) & 0x100) ? 1 : 0;
	led_state += (ADM8668_WLAN_REG(GPIO_REG) & 0x40) ? 2 : 0;
	len += sprintf(buf+len, "%s\n",
		(led_state&1) ?
			((led_state&2) ? "ORANGE" : "GREEN") : "OFF");

	return len;
}

int adm8668_button_read_proc(char *buf, char **start, off_t offset,
	int count, int *eof, void *data)
{
	int len = 0;
	int read_once = ADM8668_CONFIG_REG(CRGPIO_REG);
	int button_flip = (read_once >> 20) & 0x3;
	int button_state = read_once & 0x3;

	len += sprintf(buf+len, "SES: %s %s\nRESET: %s %s\n",
		(button_state&2) ? "UP" : "DOWN",
		(button_flip&2) ? "FLIP" : "",
		(button_state&1) ? "UP" : "DOWN",
		(button_flip&1) ? "FLIP" : "");

	return len;
}

int __init adm8668_init_proc(void)
{
	struct proc_dir_entry *adm8668_proc_dir = NULL;
	struct proc_dir_entry *sesled = NULL;
	int __maybe_unused bogus;

	/* these are known to be lights. rest are input...? */
	ADM8668_CONFIG_REG(CRGPIO_REG)	= GPIO2_OUTPUT_ENABLE;
	ADM8668_WLAN_REG(GPIO_REG)	= GPIO0_OUTPUT_ENABLE;

	/* inital read off of the flipper switcher on the button thingie */
	bogus = ADM8668_CONFIG_REG(CRGPIO_REG);

	adm8668_proc_dir = proc_mkdir("adm8668", 0);
	if (adm8668_proc_dir == NULL) {
		printk(KERN_ERR "ADM8668 proc: unable to create proc dir.\n");
		return 0;
	}
	create_proc_read_entry("buttons", 0444, adm8668_proc_dir,
					adm8668_button_read_proc, NULL);
	sesled = create_proc_entry("sesled", S_IRUGO|S_IWUGO, adm8668_proc_dir);
	if (sesled) {
		sesled->read_proc = adm8668_sesled_read_proc;
		sesled->write_proc = adm8668_sesled_write_proc;
	}

	return 0;
}

module_init(adm8668_init_proc);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Scott Nicholas <neutronscott@scottn.us>");
MODULE_DESCRIPTION("ADM8668 ghetto button driver");
