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
#include <linux/uaccess.h>
#include <linux/unistd.h>
#include <linux/errno.h>
#include <linux/leds.h>
#include <linux/delay.h>

#include <asm/ifxmips/ifxmips.h>
#include <asm/ifxmips/ifxmips_gpio.h>
#include <asm/ifxmips/ifxmips_pmu.h>

#define DRVNAME					"ifxmips_led"

/* might need to be changed depending on shift register used on the pcb */
#if 1
#define IFXMIPS_LED_CLK_EDGE			IFXMIPS_LED_FALLING
#else
#define IFXMIPS_LED_CLK_EDGE			IFXMIPS_LED_RISING
#endif

#define IFXMIPS_LED_SPEED			IFXMIPS_LED_8HZ

#define IFXMIPS_LED_GPIO_PORT			0

#define IFXMIPS_MAX_LED				24

struct ifxmips_led {
	struct led_classdev cdev;
	u8 bit;
};

void ifxmips_led_set(unsigned int led)
{
	led &= 0xffffff;
	ifxmips_w32(ifxmips_r32(IFXMIPS_LED_CPU0) | led, IFXMIPS_LED_CPU0);
}
EXPORT_SYMBOL(ifxmips_led_set);

void ifxmips_led_clear(unsigned int led)
{
	led = ~(led & 0xffffff);
	ifxmips_w32(ifxmips_r32(IFXMIPS_LED_CPU0) & led, IFXMIPS_LED_CPU0);
}
EXPORT_SYMBOL(ifxmips_led_clear);

void ifxmips_led_blink_set(unsigned int led)
{
	led &= 0xffffff;
	ifxmips_w32(ifxmips_r32(IFXMIPS_LED_CON0) | led, IFXMIPS_LED_CON0);
}
EXPORT_SYMBOL(ifxmips_led_blink_set);

void ifxmips_led_blink_clear(unsigned int led)
{
	led = ~(led & 0xffffff);
	ifxmips_w32(ifxmips_r32(IFXMIPS_LED_CON0) & led, IFXMIPS_LED_CON0);
}
EXPORT_SYMBOL(ifxmips_led_blink_clear);

static void ifxmips_ledapi_set(struct led_classdev *led_cdev,
	enum led_brightness value)
{
	struct ifxmips_led *led_dev =
		container_of(led_cdev, struct ifxmips_led, cdev);

	if (value)
		ifxmips_led_set(1 << led_dev->bit);
	else
		ifxmips_led_clear(1 << led_dev->bit);
}

void ifxmips_led_setup_gpio(void)
{
	int i = 0;

	/* leds are controlled via a shift register
	   we need to setup pins SH,D,ST (4,5,6) to make it work */
	for (i = 4; i < 7; i++) {
		ifxmips_port_set_altsel0(IFXMIPS_LED_GPIO_PORT, i);
		ifxmips_port_clear_altsel1(IFXMIPS_LED_GPIO_PORT, i);
		ifxmips_port_set_dir_out(IFXMIPS_LED_GPIO_PORT, i);
		ifxmips_port_set_open_drain(IFXMIPS_LED_GPIO_PORT, i);
	}
}

static int ifxmips_led_probe(struct platform_device *dev)
{
	int i = 0;

	ifxmips_led_setup_gpio();

	ifxmips_w32(0, IFXMIPS_LED_AR);
	ifxmips_w32(0, IFXMIPS_LED_CPU0);
	ifxmips_w32(0, IFXMIPS_LED_CPU1);
	ifxmips_w32(LED_CON0_SWU, IFXMIPS_LED_CON0);
	ifxmips_w32(0, IFXMIPS_LED_CON1);

	/* setup the clock edge that the shift register is triggered on */
	ifxmips_w32(ifxmips_r32(IFXMIPS_LED_CON0) & ~IFXMIPS_LED_EDGE_MASK,
		IFXMIPS_LED_CON0);
	ifxmips_w32(ifxmips_r32(IFXMIPS_LED_CON0) | IFXMIPS_LED_CLK_EDGE,
		IFXMIPS_LED_CON0);

	/* per default leds 15-0 are set */
	ifxmips_w32(IFXMIPS_LED_GROUP1 | IFXMIPS_LED_GROUP0, IFXMIPS_LED_CON1);

	/* leds are update periodically by the FPID */
	ifxmips_w32(ifxmips_r32(IFXMIPS_LED_CON1) & ~IFXMIPS_LED_UPD_MASK,
		IFXMIPS_LED_CON1);
	ifxmips_w32(ifxmips_r32(IFXMIPS_LED_CON1) | IFXMIPS_LED_UPD_SRC_FPI,
		IFXMIPS_LED_CON1);

	/* set led update speed */
	ifxmips_w32(ifxmips_r32(IFXMIPS_LED_CON1) & ~IFXMIPS_LED_MASK,
		IFXMIPS_LED_CON1);
	ifxmips_w32(ifxmips_r32(IFXMIPS_LED_CON1) | IFXMIPS_LED_SPEED,
		IFXMIPS_LED_CON1);

	/* adsl 0 and 1 leds are updated by the arc */
	ifxmips_w32(ifxmips_r32(IFXMIPS_LED_CON0) | IFXMIPS_LED_ADSL_SRC,
		IFXMIPS_LED_CON0);

	/* per default, the leds are turned on */
	ifxmips_pmu_enable(IFXMIPS_PMU_PWDCR_LED);

	for (i = 0; i < IFXMIPS_MAX_LED; i++) {
		struct ifxmips_led *tmp =
			kzalloc(sizeof(struct ifxmips_led), GFP_KERNEL);
		tmp->cdev.brightness_set = ifxmips_ledapi_set;
		tmp->cdev.name = kmalloc(sizeof("ifxmips:led:00"), GFP_KERNEL);
		sprintf((char *)tmp->cdev.name, "ifxmips:led:%02d", i);
		tmp->cdev.default_trigger = NULL;
		tmp->bit = i;
		led_classdev_register(&dev->dev, &tmp->cdev);
	}

	return 0;
}

static int ifxmips_led_remove(struct platform_device *pdev)
{
	return 0;
}

static struct platform_driver ifxmips_led_driver = {
	.probe = ifxmips_led_probe,
	.remove = ifxmips_led_remove,
	.driver = {
		.name = DRVNAME,
		.owner = THIS_MODULE,
	},
};

int __init ifxmips_led_init(void)
{
	int ret = platform_driver_register(&ifxmips_led_driver);
	if (ret)
		printk(KERN_INFO
			"ifxmips_led: Error registering platfom driver!");

	return ret;
}

void __exit ifxmips_led_exit(void)
{
	platform_driver_unregister(&ifxmips_led_driver);
}

module_init(ifxmips_led_init);
module_exit(ifxmips_led_exit);
