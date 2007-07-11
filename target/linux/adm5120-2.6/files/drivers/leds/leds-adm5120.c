/*
 *  $Id$
 *
 *  ADM5120 GPIO LED devices
 *
 *  Copyright (C) 2007 OpenWrt.org
 *  Copyright (C) 2007 Gabor Juhos <juhosg@freemail.hu>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the
 *  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA  02110-1301, USA.
 *
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/leds.h>
#include <linux/err.h>

#include <linux/gpio_leds.h>

#include <asm/bootinfo.h>
#include <asm/io.h>
#include <asm/gpio.h>

#include <asm/mach-adm5120/adm5120_info.h>

#define NUM_LEDS_MAX	23

#define ADM5120_GPIO_xxxx	0x100	/* an unknown pin */

struct mach_data {
	unsigned long machtype;
	unsigned count;
	struct gpio_led_platform_data *data;
};

struct adm5120_leddev {
	struct platform_device pdev;
	struct gpio_led_platform_data pdata;
};

static int led_count = 0;
static struct adm5120_leddev *led_devs[NUM_LEDS_MAX];

#define LED_ARRAY(n)				\
static struct gpio_led_platform_data		\
n ## _leds [] __initdata =

#define LED_DATA(n,t,g,off,on) {		\
	.name = (n),				\
	.trigger = (t),				\
	.gpio = (g),				\
	.value_off = (off),			\
	.value_on = (on)			\
	}

#define LED_STD(g,n,t)	LED_DATA((n),(t),(g), 0, 1)
#define LED_INV(g,n,t)	LED_DATA((n),(t),(g), 1, 0)

/*
 * ZyXEL boards
 */
#if defined(CONFIG_LEDS_ADM5120_EXPERIMENTAL)
LED_ARRAY(p334) { /* FIXME: untested */
	LED_INV(ADM5120_GPIO_xxxx, "power",	NULL ),
	LED_INV(ADM5120_GPIO_xxxx, "lan1",	NULL ),
	LED_INV(ADM5120_GPIO_xxxx, "lan2",	NULL ),
	LED_INV(ADM5120_GPIO_xxxx, "lan3",	NULL ),
	LED_INV(ADM5120_GPIO_xxxx, "lan4",	NULL ),
	LED_INV(ADM5120_GPIO_xxxx, "wan",	NULL ),
};
#endif

LED_ARRAY(p334wt) {
	LED_INV(ADM5120_GPIO_PIN2, "power",	NULL ),
	LED_INV(ADM5120_GPIO_P3L0, "lan1",	NULL ),
	LED_INV(ADM5120_GPIO_P2L0, "lan2",	NULL ),
	LED_INV(ADM5120_GPIO_P1L0, "lan3",	NULL ),
	LED_INV(ADM5120_GPIO_P0L0, "lan4",	NULL ),
	LED_INV(ADM5120_GPIO_P4L0, "wan",	NULL ),
	LED_INV(ADM5120_GPIO_P4L2, "wlan",	NULL ),
	LED_INV(ADM5120_GPIO_P2L2, "otist",	NULL ),
	LED_INV(ADM5120_GPIO_P1L2, "hidden",	NULL ),
};

#if defined(CONFIG_LEDS_ADM5120_EXPERIMENTAL)
LED_ARRAY(p335) { /* FIXME: untested */
	LED_INV(ADM5120_GPIO_PIN2, "power",	NULL ),
	LED_INV(ADM5120_GPIO_P3L0, "lan1",	NULL ),
	LED_INV(ADM5120_GPIO_P2L0, "lan2",	NULL ),
	LED_INV(ADM5120_GPIO_P1L0, "lan3",	NULL ),
	LED_INV(ADM5120_GPIO_P0L0, "lan4",	NULL ),
	LED_INV(ADM5120_GPIO_P4L0, "wan",	NULL ),
	LED_INV(ADM5120_GPIO_P4L2, "wlan",	NULL ),
	LED_INV(ADM5120_GPIO_P2L2, "otist",	NULL ),
	LED_INV(ADM5120_GPIO_xxxx, "usb",	NULL ),
};
#endif

/*
 * Mikrotik boards
 */
#if defined(CONFIG_LEDS_ADM5120_EXPERIMENTAL)
LED_ARRAY(rb100) { /* FIXME: untested */
	LED_STD(ADM5120_GPIO_PIN6, "power",	NULL ),
	LED_STD(ADM5120_GPIO_PIN3, "user",	NULL ),
};
#endif

/*
 * Compex boards
 */
#if defined(CONFIG_LEDS_ADM5120_EXPERIMENTAL)
LED_ARRAY(np27g) { /* FIXME: untested */
	LED_STD(ADM5120_GPIO_xxxx, "lan1",	NULL ),
	LED_STD(ADM5120_GPIO_xxxx, "lan2",	NULL ),
	LED_STD(ADM5120_GPIO_xxxx, "lan3",	NULL ),
	LED_STD(ADM5120_GPIO_xxxx, "lan4",	NULL ),
	LED_STD(ADM5120_GPIO_xxxx, "wan_cond",	NULL ),
	LED_STD(ADM5120_GPIO_xxxx, "wlan",	NULL ),
	LED_STD(ADM5120_GPIO_xxxx, "wan_act",	NULL ),
	LED_STD(ADM5120_GPIO_xxxx, "usb1",	NULL ),
	LED_STD(ADM5120_GPIO_xxxx, "usb2",	NULL ),
	LED_INV(ADM5120_GPIO_PIN2, "power",	NULL ),
	LED_STD(ADM5120_GPIO_xxxx, "diag",	NULL ),
};
#endif

#if defined(CONFIG_LEDS_ADM5120_EXPERIMENTAL)
LED_ARRAY(np28g) { /* FIXME: untested */
	LED_STD(ADM5120_GPIO_xxxx, "lan1",	NULL ),
	LED_STD(ADM5120_GPIO_xxxx, "lan2",	NULL ),
	LED_STD(ADM5120_GPIO_xxxx, "lan3",	NULL ),
	LED_STD(ADM5120_GPIO_xxxx, "wan",	NULL ),
	LED_STD(ADM5120_GPIO_xxxx, "wlan",	NULL ),
	LED_STD(ADM5120_GPIO_xxxx, "usb1",	NULL ),
	LED_STD(ADM5120_GPIO_xxxx, "usb2",	NULL ),
	LED_STD(ADM5120_GPIO_xxxx, "usb3",	NULL ),
	LED_STD(ADM5120_GPIO_xxxx, "usb4",	NULL ),
	LED_INV(ADM5120_GPIO_PIN2, "power",	NULL ),
	LED_STD(ADM5120_GPIO_xxxx, "diag",	NULL ),
};
#endif

LED_ARRAY(wp54g) {
	LED_INV(ADM5120_GPIO_PIN2, "diag",	NULL ),
	LED_INV(ADM5120_GPIO_PIN6, "wlan",	NULL ),
	LED_INV(ADM5120_GPIO_PIN7, "wan",	NULL ),
	LED_INV(ADM5120_GPIO_P0L0, "lan1",	NULL ),
	LED_INV(ADM5120_GPIO_P1L0, "lan2",	NULL ),
};

LED_ARRAY(generic) {
#if defined(CONFIG_LEDS_ADM5120_DIAG)
	LED_STD(ADM5120_GPIO_PIN0, "gpio0",	NULL ),
	LED_STD(ADM5120_GPIO_PIN1, "gpio1",	NULL ),
	LED_STD(ADM5120_GPIO_PIN2, "gpio2",	NULL ),
	LED_STD(ADM5120_GPIO_PIN3, "gpio3",	NULL ),
	LED_STD(ADM5120_GPIO_PIN4, "gpio4",	NULL ),
	LED_STD(ADM5120_GPIO_PIN5, "gpio5",	NULL ),
	LED_STD(ADM5120_GPIO_PIN6, "gpio6",	NULL ),
	LED_STD(ADM5120_GPIO_PIN7, "gpio7",	NULL ),
	LED_STD(ADM5120_GPIO_P0L0, "port0led0",	NULL ),
	LED_STD(ADM5120_GPIO_P0L1, "port0led1",	NULL ),
	LED_STD(ADM5120_GPIO_P0L2, "port0led2",	NULL ),
	LED_STD(ADM5120_GPIO_P1L0, "port1led0",	NULL ),
	LED_STD(ADM5120_GPIO_P1L1, "port1led1",	NULL ),
	LED_STD(ADM5120_GPIO_P1L2, "port1led2",	NULL ),
	LED_STD(ADM5120_GPIO_P2L0, "port2led0",	NULL ),
	LED_STD(ADM5120_GPIO_P2L1, "port2led1",	NULL ),
	LED_STD(ADM5120_GPIO_P2L2, "port2led2",	NULL ),
	LED_STD(ADM5120_GPIO_P3L0, "port3led0",	NULL ),
	LED_STD(ADM5120_GPIO_P3L1, "port3led1",	NULL ),
	LED_STD(ADM5120_GPIO_P3L2, "port3led2",	NULL ),
	LED_STD(ADM5120_GPIO_P4L0, "port4led0",	NULL ),
	LED_STD(ADM5120_GPIO_P4L1, "port4led1",	NULL ),
	LED_STD(ADM5120_GPIO_P4L2, "port4led2",	NULL ),
#endif
};

#define MACH_DATA(m, n) {				\
	.machtype	= (m),				\
	.count		= ARRAY_SIZE(n ## _leds),	\
	.data		= n ## _leds			\
}

static struct mach_data machines[] __initdata = {
	MACH_DATA(MACH_ADM5120_GENERIC, generic),
	MACH_DATA(MACH_ADM5120_P334WT,	p334wt),
	MACH_DATA(MACH_ADM5120_WP54AG,	wp54g),
	MACH_DATA(MACH_ADM5120_WP54G,	wp54g),
	MACH_DATA(MACH_ADM5120_WP54G_WRT, wp54g),
	MACH_DATA(MACH_ADM5120_WPP54AG,	wp54g),
	MACH_DATA(MACH_ADM5120_WPP54G,	wp54g),
#if defined(CONFIG_LEDS_ADM5120_EXPERIMENTAL)
	MACH_DATA(MACH_ADM5120_P334,	p334),
	MACH_DATA(MACH_ADM5120_P335,	p335),
	MACH_DATA(MACH_ADM5120_RB_111,	rb100),
	MACH_DATA(MACH_ADM5120_RB_112,	rb100),
	MACH_DATA(MACH_ADM5120_NP27G,	np27g),
	MACH_DATA(MACH_ADM5120_NP28G,	np28g),
	MACH_DATA(MACH_ADM5120_NP28GHS,	np28g),
#endif
};

static struct adm5120_leddev * __init
create_leddev(struct gpio_led_platform_data *data)
{
	struct adm5120_leddev *p;

	p = kzalloc(sizeof(*p), GFP_KERNEL);
	if (p == NULL)
		return NULL;

	memcpy(&p->pdata, data, sizeof(p->pdata));
	p->pdev.dev.platform_data = &p->pdata;

	return p;
}

static void
destroy_leddev(struct adm5120_leddev *led)
{
	if (led)
		kfree(led);
}

static struct mach_data * __init
adm5120_leds_findmach(unsigned long machtype)
{
	struct mach_data *mach;
	int i;

	mach = NULL;
	for (i=0; i<ARRAY_SIZE(machines); i++) {
		if (machines[i].machtype == machtype) {
			mach = &machines[i];
			break;
		}
	};

#if defined(CONFIG_LEDS_ADM5120_DIAG)
	if (mach == NULL)
		mach = machines;
#endif

	return mach;
}

static int __init
adm5120_leds_init(void)
{
	struct mach_data *mach;
	int i, ret;

	if (mips_machgroup != MACH_GROUP_ADM5120) {
		ret = -EINVAL;
		goto err;
	}

	mach = adm5120_leds_findmach(mips_machtype);
	if (mach == NULL) {
		/* the board is not yet supported */
		ret = -EINVAL;
		goto err;
	}

	for (i=0; i < mach->count; i++) {
		led_devs[i] = create_leddev(&mach->data[i]);
		if (led_devs[i] == NULL) {
			ret = -ENOMEM;
			goto err_destroy;
		}
		led_devs[i]->pdev.name = "gpio-led";
		led_devs[i]->pdev.id = i;
	}

	for (i=0; i < mach->count; i++) {
		ret = platform_device_register(&led_devs[i]->pdev);
		if (ret)
			goto err_unregister;
	}

	led_count = mach->count;
	return 0;

err_unregister:
	for (i--; i>=0; i--)
		platform_device_unregister(&led_devs[i]->pdev);

err_destroy:
	for (i=0; i<led_count; i++)
		destroy_leddev(led_devs[i]);
err:
	return ret;
}

static void __exit
adm5120_leds_exit(void)
{
	int i;

	for (i=0; i < led_count; i++) {
		platform_device_unregister(&led_devs[i]->pdev);
		destroy_leddev(led_devs[i]);
	}
}

module_init(adm5120_leds_init);
module_exit(adm5120_leds_exit);

MODULE_AUTHOR("Gabor Juhos <juhosg@freemail.hu>");
MODULE_DESCRIPTION(DRV_DESC);
MODULE_LICENSE("GPL");

