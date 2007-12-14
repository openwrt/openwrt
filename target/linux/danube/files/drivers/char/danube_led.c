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

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <asm/uaccess.h>
#include <asm/unistd.h>
#include <linux/errno.h>
#include <asm/danube/danube.h>
#include <asm/danube/danube_gpio.h>
#include <asm/delay.h>

#define DANUBE_LED_CLK_EDGE				DANUBE_LED_FALLING
//#define DANUBE_LED_CLK_EDGE			DANUBE_LED_RISING

#define DANUBE_LED_SPEED				DANUBE_LED_8HZ

#define DANUBE_LED_GPIO_PORT	0

static int danube_led_major;

void
danube_led_set (unsigned int led)
{
	led &= 0xffffff;
	writel(readl(DANUBE_LED_CPU0) | led, DANUBE_LED_CPU0);
}
EXPORT_SYMBOL(danube_led_set);

void
danube_led_clear (unsigned int led)
{
	led = ~(led & 0xffffff);
	writel(readl(DANUBE_LED_CPU0) & led, DANUBE_LED_CPU0);
}
EXPORT_SYMBOL(danube_led_clear);

void
danube_led_blink_set (unsigned int led)
{
	led &= 0xffffff;
	writel(readl(DANUBE_LED_CON0) | led, DANUBE_LED_CON0);
}
EXPORT_SYMBOL(danube_led_blink_set);

void
danube_led_blink_clear (unsigned int led)
{
	led = ~(led & 0xffffff);
	writel(readl(DANUBE_LED_CON0) & led, DANUBE_LED_CON0);
}
EXPORT_SYMBOL(danube_led_blink_clear);

void
danube_led_setup_gpio (void)
{
	int i = 0;

	/* we need to setup pins SH,D,ST (4,5,6) */
	for (i = 4; i < 7; i++)
	{
		danube_port_set_altsel0(DANUBE_LED_GPIO_PORT, i);
		danube_port_clear_altsel1(DANUBE_LED_GPIO_PORT, i);
		danube_port_set_dir_out(DANUBE_LED_GPIO_PORT, i);
		danube_port_set_open_drain(DANUBE_LED_GPIO_PORT, i);
	}
}

static void
danube_led_enable (void)
{
	int err = 1000000;

	writel(readl(DANUBE_PMU_PWDCR) & ~DANUBE_PMU_PWDCR_LED, DANUBE_PMU_PWDCR);
	while (--err && (readl(DANUBE_PMU_PWDSR) & DANUBE_PMU_PWDCR_LED)) {}

	if (!err)
		panic("Activating LED in PMU failed!");
}

static inline void
danube_led_disable (void)
{
	writel(readl(DANUBE_PMU_PWDCR) | DANUBE_PMU_PWDCR_LED, DANUBE_PMU_PWDCR);
}

static int
led_ioctl (struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
	int ret = -EINVAL;

	switch ( cmd )
	{
	}

	return ret;
}

static int
led_open (struct inode *inode, struct file *file)
{
	return 0;
}

static int
led_release (struct inode *inode, struct file *file)
{
	return 0;
}

static struct file_operations danube_led_fops = {
	.owner = THIS_MODULE,
	.ioctl = led_ioctl,
	.open = led_open,
	.release = led_release
};


/*
Map for LED on reference board
	WLAN_READ     LED11   OUT1    15
	WARNING       LED12   OUT2    14
	FXS1_LINK     LED13   OUT3    13
	FXS2_LINK     LED14   OUT4    12
	FXO_ACT       LED15   OUT5    11
	USB_LINK      LED16   OUT6    10
	ADSL2_LINK    LED19   OUT7    9
	BT_LINK       LED17   OUT8    8
	SD_LINK       LED20   OUT9    7
	ADSL2_TRAFFIC LED31   OUT16   0
Map for hardware relay on reference board
	USB Power On          OUT11   5
	RELAY                 OUT12   4
*/


int __init
danube_led_init (void)
{
	int ret = 0;

	danube_led_setup_gpio();

	writel(0, DANUBE_LED_AR);
	writel(0, DANUBE_LED_CPU0);
	writel(0, DANUBE_LED_CPU1);
	writel(LED_CON0_SWU, DANUBE_LED_CON0);
	writel(0, DANUBE_LED_CON1);

	/* setup the clock edge that the shift register is triggered on */
	writel(readl(DANUBE_LED_CON0) & ~DANUBE_LED_EDGE_MASK, DANUBE_LED_CON0);
	writel(readl(DANUBE_LED_CON0) | DANUBE_LED_CLK_EDGE, DANUBE_LED_CON0);

	/* per default leds 15-0 are set */
	writel(DANUBE_LED_GROUP1 | DANUBE_LED_GROUP0, DANUBE_LED_CON1);

	/* leds are update periodically by the FPID */
	writel(readl(DANUBE_LED_CON1) & ~DANUBE_LED_UPD_MASK, DANUBE_LED_CON1);
	writel(readl(DANUBE_LED_CON1) | DANUBE_LED_UPD_SRC_FPI, DANUBE_LED_CON1);

	/* set led update speed */
	writel(readl(DANUBE_LED_CON1) & ~DANUBE_LED_MASK, DANUBE_LED_CON1);
	writel(readl(DANUBE_LED_CON1) | DANUBE_LED_SPEED, DANUBE_LED_CON1);

	/* adsl 0 and 1 leds are updated by the arc */
	writel(readl(DANUBE_LED_CON0) | DANUBE_LED_ADSL_SRC, DANUBE_LED_CON0);

	/* per default, the leds are turned on */
	danube_led_enable();

	danube_led_major = register_chrdev(0, "danube_led", &danube_led_fops);

	if (!danube_led_major)
	{
		printk("danube_led: Error! Could not register device. %d\n", danube_led_major);
		ret = -EINVAL;

		goto out;
	}

	printk(KERN_INFO "danube_led : device registered on major %d\n", danube_led_major);

out:
	return ret;
}

void __exit
danube_led_exit (void)
{
	unregister_chrdev(danube_led_major, "danube_led");
}

module_init(danube_led_init);
module_exit(danube_led_exit);
