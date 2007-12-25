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
#include <linux/platform_device.h>
#include <asm/uaccess.h>
#include <asm/unistd.h>
#include <linux/errno.h>
#include <asm/ifxmips/ifxmips.h>
#include <asm/ifxmips/ifxmips_gpio.h>
#include <asm/ifxmips/ifxmips_pmu.h>

#define DRVNAME							"ifxmips_led" 

#define IFXMIPS_LED_CLK_EDGE			IFXMIPS_LED_FALLING
//#define IFXMIPS_LED_CLK_EDGE			IFXMIPS_LED_RISING

#define IFXMIPS_LED_SPEED				IFXMIPS_LED_8HZ

#define IFXMIPS_LED_GPIO_PORT			0

static int ifxmips_led_major;

void
ifxmips_led_set (unsigned int led)
{
	led &= 0xffffff;
	writel(readl(IFXMIPS_LED_CPU0) | led, IFXMIPS_LED_CPU0);
}
EXPORT_SYMBOL(ifxmips_led_set);

void
ifxmips_led_clear (unsigned int led)
{
	led = ~(led & 0xffffff);
	writel(readl(IFXMIPS_LED_CPU0) & led, IFXMIPS_LED_CPU0);
}
EXPORT_SYMBOL(ifxmips_led_clear);

void
ifxmips_led_blink_set (unsigned int led)
{
	led &= 0xffffff;
	writel(readl(IFXMIPS_LED_CON0) | led, IFXMIPS_LED_CON0);
}
EXPORT_SYMBOL(ifxmips_led_blink_set);

void
ifxmips_led_blink_clear (unsigned int led)
{
	led = ~(led & 0xffffff);
	writel(readl(IFXMIPS_LED_CON0) & led, IFXMIPS_LED_CON0);
}
EXPORT_SYMBOL(ifxmips_led_blink_clear);

void
ifxmips_led_setup_gpio (void)
{
	int i = 0;

	/* we need to setup pins SH,D,ST (4,5,6) */
	for (i = 4; i < 7; i++)
	{
		ifxmips_port_set_altsel0(IFXMIPS_LED_GPIO_PORT, i);
		ifxmips_port_clear_altsel1(IFXMIPS_LED_GPIO_PORT, i);
		ifxmips_port_set_dir_out(IFXMIPS_LED_GPIO_PORT, i);
		ifxmips_port_set_open_drain(IFXMIPS_LED_GPIO_PORT, i);
	}
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

static struct file_operations ifxmips_led_fops = {
	.owner = THIS_MODULE,
	.ioctl = led_ioctl,
	.open = led_open,
	.release = led_release
};

static int
ifxmips_led_probe(struct platform_device *dev)
{
	int ret = 0;

	ifxmips_led_setup_gpio();

	writel(0, IFXMIPS_LED_AR);
	writel(0, IFXMIPS_LED_CPU0);
	writel(0, IFXMIPS_LED_CPU1);
	writel(LED_CON0_SWU, IFXMIPS_LED_CON0);
	writel(0, IFXMIPS_LED_CON1);

	/* setup the clock edge that the shift register is triggered on */
	writel(readl(IFXMIPS_LED_CON0) & ~IFXMIPS_LED_EDGE_MASK, IFXMIPS_LED_CON0);
	writel(readl(IFXMIPS_LED_CON0) | IFXMIPS_LED_CLK_EDGE, IFXMIPS_LED_CON0);

	/* per default leds 15-0 are set */
	writel(IFXMIPS_LED_GROUP1 | IFXMIPS_LED_GROUP0, IFXMIPS_LED_CON1);

	/* leds are update periodically by the FPID */
	writel(readl(IFXMIPS_LED_CON1) & ~IFXMIPS_LED_UPD_MASK, IFXMIPS_LED_CON1);
	writel(readl(IFXMIPS_LED_CON1) | IFXMIPS_LED_UPD_SRC_FPI, IFXMIPS_LED_CON1);

	/* set led update speed */
	writel(readl(IFXMIPS_LED_CON1) & ~IFXMIPS_LED_MASK, IFXMIPS_LED_CON1);
	writel(readl(IFXMIPS_LED_CON1) | IFXMIPS_LED_SPEED, IFXMIPS_LED_CON1);

	/* adsl 0 and 1 leds are updated by the arc */
	writel(readl(IFXMIPS_LED_CON0) | IFXMIPS_LED_ADSL_SRC, IFXMIPS_LED_CON0);

	/* per default, the leds are turned on */
	ifxmips_pmu_enable(IFXMIPS_PMU_PWDCR_LED);

	ifxmips_led_major = register_chrdev(0, "ifxmips_led", &ifxmips_led_fops);

	if (!ifxmips_led_major)
	{
		printk("ifxmips_led: Error! Could not register device. %d\n", ifxmips_led_major);
		ret = -EINVAL;

		goto out;
	}

	printk(KERN_INFO "ifxmips_led: device successfully initialized #%d.\n", ifxmips_led_major);

out:
	return ret;
}

static int
ifxmips_led_remove(struct platform_device *pdev)
{
	unregister_chrdev(ifxmips_led_major, "ifxmips_led");
	return 0;
}

static struct
platform_driver ifxmips_led_driver = {
	.probe = ifxmips_led_probe,
	.remove = ifxmips_led_remove,
	.driver = {
		.name = DRVNAME,
		.owner = THIS_MODULE,
	},
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
ifxmips_led_init (void)
{
	int ret = platform_driver_register(&ifxmips_led_driver);
	if (ret)
		printk(KERN_INFO "ifxmips_led: Error registering platfom driver!");

	return ret;
}

void __exit
ifxmips_led_exit (void)
{
	platform_driver_unregister(&ifxmips_led_driver);
}

module_init(ifxmips_led_init);
module_exit(ifxmips_led_exit);
