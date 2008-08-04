/*
 *  $Id$
 *
 *  ADM5120 GPIO LED devices
 *
 *  Copyright (C) 2007,2008 OpenWrt.org
 *  Copyright (C) 2007,2008 Gabor Juhos <juhosg at openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/leds.h>
#include <linux/err.h>

#include <linux/io.h>
#include <linux/leds.h>

#include <asm/bootinfo.h>
#include <asm/gpio.h>

#include <adm5120_info.h>

#define DRV_DESC	"LED driver for the ADM5120 based boards"
#define DRV_VERSION	"0.2.1"

#define NUM_LEDS_MAX	23
#define ADM5120_GPIO_xxxx	0x100	/* an unknown pin */

struct mach_data {
	unsigned long machtype;
	unsigned nr_leds;
	struct gpio_led *leds;
};

struct adm5120_leddev {
	struct platform_device pdev;
	struct gpio_led led;
	struct gpio_led_platform_data pdata;
};

static int led_count;
static struct adm5120_leddev *led_devs[NUM_LEDS_MAX];

#define LED_ARRAY(n)				\
static struct gpio_led n ## _leds [] __initdata =

#define LED_DATA(n, t, g, a) {	\
	.name = (n),		\
	.default_trigger = (t), \
	.gpio = (g),		\
	.active_low = (a)	\
}

#define LED_STD(g, n, t)	LED_DATA((n), (t), (g), 0)
#define LED_INV(g, n, t)	LED_DATA((n), (t), (g), 1)

/*
 * Cellvision boards
 */

LED_ARRAY(cas771) {
	LED_STD(ADM5120_GPIO_PIN0, "cam_flash",	NULL),
	/* GPIO PIN3 is the reset */
	LED_STD(ADM5120_GPIO_PIN6, "access",	NULL),
	LED_STD(ADM5120_GPIO_P0L1, "status",	NULL),
	LED_STD(ADM5120_GPIO_P0L2, "diag",	NULL),
};

/*
 * Compex boards
 */
#if defined(CONFIG_LEDS_ADM5120_EXPERIMENTAL)
LED_ARRAY(np27g) { /* FIXME: untested */
	LED_STD(ADM5120_GPIO_xxxx, "lan1",	NULL),
	LED_STD(ADM5120_GPIO_xxxx, "lan2",	NULL),
	LED_STD(ADM5120_GPIO_xxxx, "lan3",	NULL),
	LED_STD(ADM5120_GPIO_xxxx, "lan4",	NULL),
	LED_STD(ADM5120_GPIO_xxxx, "wan_cond",	NULL),
	LED_STD(ADM5120_GPIO_xxxx, "wlan",	NULL),
	LED_STD(ADM5120_GPIO_xxxx, "wan_act",	NULL),
	LED_STD(ADM5120_GPIO_xxxx, "usb1",	NULL),
	LED_STD(ADM5120_GPIO_xxxx, "usb2",	NULL),
	LED_INV(ADM5120_GPIO_PIN2, "power",	NULL),
	LED_STD(ADM5120_GPIO_xxxx, "diag",	NULL),
};
#endif

LED_ARRAY(np28g) {
	LED_INV(ADM5120_GPIO_PIN2, "diag",	NULL),
	LED_INV(ADM5120_GPIO_PIN3, "power",	NULL),
	LED_INV(ADM5120_GPIO_PIN6, "wan_cond",	NULL),
	LED_INV(ADM5120_GPIO_PIN7, "wifi",	NULL),
	LED_INV(ADM5120_GPIO_P0L2, "usb1",	NULL),
	LED_INV(ADM5120_GPIO_P1L0, "lan1",	NULL),
	LED_INV(ADM5120_GPIO_P1L2, "usb2",	NULL),
	LED_INV(ADM5120_GPIO_P2L0, "lan2",	NULL),
	LED_INV(ADM5120_GPIO_P2L2, "usb3",	NULL),
	LED_INV(ADM5120_GPIO_P3L0, "lan3",	NULL),
	LED_INV(ADM5120_GPIO_P3L2, "usb4",	NULL),
	LED_INV(ADM5120_GPIO_P4L0, "wan",	NULL),
};

LED_ARRAY(wp54g) {
	LED_INV(ADM5120_GPIO_PIN2, "diag",	NULL),
	LED_INV(ADM5120_GPIO_PIN6, "wlan",	NULL),
	LED_INV(ADM5120_GPIO_PIN7, "wan",	NULL),
	LED_INV(ADM5120_GPIO_P0L0, "lan1",	NULL),
	LED_INV(ADM5120_GPIO_P1L0, "lan2",	NULL),
};

/*
 * Edimax boards
 */
LED_ARRAY(br6104k) {
	LED_STD(ADM5120_GPIO_PIN0, "power",		NULL),
	LED_INV(ADM5120_GPIO_P0L1, "wan_speed",		NULL),
	LED_INV(ADM5120_GPIO_P0L0, "wan_lnkact",	NULL),
	LED_INV(ADM5120_GPIO_P1L1, "lan1_speed",	NULL),
	LED_INV(ADM5120_GPIO_P1L0, "lan1_lnkact",	NULL),
	LED_INV(ADM5120_GPIO_P2L1, "lan2_speed",	NULL),
	LED_INV(ADM5120_GPIO_P2L0, "lan2_lnkact",	NULL),
	LED_INV(ADM5120_GPIO_P3L1, "lan3_speed",	NULL),
	LED_INV(ADM5120_GPIO_P3L0, "lan3_lnkact",	NULL),
	LED_INV(ADM5120_GPIO_P4L1, "lan4_speed",	NULL),
	LED_INV(ADM5120_GPIO_P4L0, "lan4_lnkact",	NULL),
};

LED_ARRAY(br61x4wg) {
	LED_STD(ADM5120_GPIO_PIN0, "power",		NULL),
	LED_STD(ADM5120_GPIO_PIN5, "wlan",		NULL),
	LED_INV(ADM5120_GPIO_P0L1, "wan_speed",		NULL),
	LED_INV(ADM5120_GPIO_P0L0, "wan_lnkact",	NULL),
	LED_INV(ADM5120_GPIO_P1L1, "lan1_speed",	NULL),
	LED_INV(ADM5120_GPIO_P1L0, "lan1_lnkact",	NULL),
	LED_INV(ADM5120_GPIO_P2L1, "lan2_speed",	NULL),
	LED_INV(ADM5120_GPIO_P2L0, "lan2_lnkact",	NULL),
	LED_INV(ADM5120_GPIO_P3L1, "lan3_speed",	NULL),
	LED_INV(ADM5120_GPIO_P3L0, "lan3_lnkact",	NULL),
	LED_INV(ADM5120_GPIO_P4L1, "lan4_speed",	NULL),
	LED_INV(ADM5120_GPIO_P4L0, "lan4_lnkact",	NULL),
};

/*
 * Infineon boards
 */
LED_ARRAY(easy5120rt) {
	LED_INV(ADM5120_GPIO_PIN6, "user",		NULL),
	LED_INV(ADM5120_GPIO_P0L0, "lan0_led1",		NULL),
	LED_INV(ADM5120_GPIO_P0L1, "lan0_led2",		NULL),
	LED_INV(ADM5120_GPIO_P1L0, "lan1_led1",		NULL),
	LED_INV(ADM5120_GPIO_P1L1, "lan1_led2",		NULL),
	LED_INV(ADM5120_GPIO_P2L0, "lan2_led1",		NULL),
	LED_INV(ADM5120_GPIO_P2L1, "lan2_led2",		NULL),
	LED_INV(ADM5120_GPIO_P3L0, "lan3_led1",		NULL),
	LED_INV(ADM5120_GPIO_P3L1, "lan3_led2",		NULL),
	LED_INV(ADM5120_GPIO_P4L0, "wan",		NULL),
};

/*
 * Mikrotik boards
 */
LED_ARRAY(rb100) {
	LED_STD(ADM5120_GPIO_PIN3, "user",		NULL),
        LED_INV(ADM5120_GPIO_P0L1, "lan_speed", 	NULL),
        LED_INV(ADM5120_GPIO_P0L0, "lan_lnkact", 	NULL),
};

LED_ARRAY(rb133) {
	LED_STD(ADM5120_GPIO_PIN6, "power",		NULL),
	LED_STD(ADM5120_GPIO_PIN5, "user",		NULL),
        LED_INV(ADM5120_GPIO_P2L1, "lan1_speed", 	NULL), /* untested */
        LED_INV(ADM5120_GPIO_P2L0, "lan1_lnkact", 	NULL), /* untested */
        LED_INV(ADM5120_GPIO_P1L1, "lan2_speed", 	NULL), /* untested */
        LED_INV(ADM5120_GPIO_P1L0, "lan2_lnkact", 	NULL), /* untested */
        LED_INV(ADM5120_GPIO_P0L1, "lan3_speed", 	NULL), /* untested */
        LED_INV(ADM5120_GPIO_P0L0, "lan3_lnkact", 	NULL), /* untested */
};

LED_ARRAY(rb133c) {
	LED_STD(ADM5120_GPIO_PIN6, "power",		NULL),
	LED_STD(ADM5120_GPIO_PIN5, "user",		NULL),
        LED_INV(ADM5120_GPIO_P2L1, "lan1_speed", 	NULL), /* untested */
        LED_INV(ADM5120_GPIO_P2L0, "lan1_lnkact", 	NULL), /* untested */
};

LED_ARRAY(rb150) {
	LED_STD(ADM5120_GPIO_P0L2, "user",		NULL),
	LED_INV(ADM5120_GPIO_P0L1, "lan1_led1",		NULL), /* untested */
	LED_INV(ADM5120_GPIO_P0L0, "lan1_led2",		NULL), /* untested */
	LED_INV(ADM5120_GPIO_P1L1, "lan5_led1",		NULL), /* untested */
	LED_INV(ADM5120_GPIO_P1L0, "lan5_led2",		NULL), /* untested */
	LED_INV(ADM5120_GPIO_P2L1, "lan4_led1",		NULL), /* untested */
	LED_INV(ADM5120_GPIO_P2L0, "lan4_led2",		NULL), /* untested */
	LED_INV(ADM5120_GPIO_P3L1, "lan3_led1",		NULL), /* untested */
	LED_INV(ADM5120_GPIO_P3L0, "lan3_led2",		NULL), /* untested */
	LED_INV(ADM5120_GPIO_P4L1, "lan2_led1",		NULL), /* untested */
	LED_INV(ADM5120_GPIO_P4L0, "lan2_led2",		NULL), /* untested */
};

LED_ARRAY(rb153) {
	LED_STD(ADM5120_GPIO_PIN5, "user",		NULL),
	LED_INV(ADM5120_GPIO_P0L1, "lan1_speed",	NULL),
	LED_INV(ADM5120_GPIO_P0L0, "lan1_lnkact",	NULL),
	LED_INV(ADM5120_GPIO_P1L1, "lan5_speed",	NULL),
	LED_INV(ADM5120_GPIO_P1L0, "lan5_lnkact",	NULL),
	LED_INV(ADM5120_GPIO_P2L1, "lan4_speed",	NULL),
	LED_INV(ADM5120_GPIO_P2L0, "lan4_lnkact",	NULL),
	LED_INV(ADM5120_GPIO_P3L1, "lan3_speed",	NULL),
	LED_INV(ADM5120_GPIO_P3L0, "lan3_lnkact",	NULL),
	LED_INV(ADM5120_GPIO_P4L1, "lan2_speed",	NULL),
	LED_INV(ADM5120_GPIO_P4L0, "lan2_lnkact",	NULL),
};

/*
 * ZyXEL boards
 */
#if defined(CONFIG_LEDS_ADM5120_EXPERIMENTAL)
LED_ARRAY(p334) { /* FIXME: untested */
	LED_INV(ADM5120_GPIO_xxxx, "power",	NULL),
	LED_INV(ADM5120_GPIO_xxxx, "lan1",	NULL),
	LED_INV(ADM5120_GPIO_xxxx, "lan2",	NULL),
	LED_INV(ADM5120_GPIO_xxxx, "lan3",	NULL),
	LED_INV(ADM5120_GPIO_xxxx, "lan4",	NULL),
	LED_INV(ADM5120_GPIO_xxxx, "wan",	NULL),
};
#endif

LED_ARRAY(p334wt) {
	LED_INV(ADM5120_GPIO_PIN2, "power",	NULL),
	LED_INV(ADM5120_GPIO_P3L0, "lan1",	NULL),
	LED_INV(ADM5120_GPIO_P2L0, "lan2",	NULL),
	LED_INV(ADM5120_GPIO_P1L0, "lan3",	NULL),
	LED_INV(ADM5120_GPIO_P0L0, "lan4",	NULL),
	LED_INV(ADM5120_GPIO_P4L0, "wan",	NULL),
	LED_INV(ADM5120_GPIO_P4L2, "wlan",	NULL),
	LED_INV(ADM5120_GPIO_P2L2, "otist",	NULL),
	LED_INV(ADM5120_GPIO_P1L2, "hidden",	NULL),
};

#if defined(CONFIG_LEDS_ADM5120_EXPERIMENTAL)
LED_ARRAY(p335) { /* FIXME: untested */
	LED_INV(ADM5120_GPIO_PIN2, "power",	NULL),
	LED_INV(ADM5120_GPIO_P3L0, "lan1",	NULL),
	LED_INV(ADM5120_GPIO_P2L0, "lan2",	NULL),
	LED_INV(ADM5120_GPIO_P1L0, "lan3",	NULL),
	LED_INV(ADM5120_GPIO_P0L0, "lan4",	NULL),
	LED_INV(ADM5120_GPIO_P4L0, "wan",	NULL),
	LED_INV(ADM5120_GPIO_P4L2, "wlan",	NULL),
	LED_INV(ADM5120_GPIO_P2L2, "otist",	NULL),
	LED_INV(ADM5120_GPIO_xxxx, "usb",	NULL),
};
#endif

/*
 * Generic board
 */
LED_ARRAY(generic) {
#if defined(CONFIG_LEDS_ADM5120_DIAG)
	LED_STD(ADM5120_GPIO_PIN0, "gpio0",	NULL),
	LED_STD(ADM5120_GPIO_PIN1, "gpio1",	NULL),
	LED_STD(ADM5120_GPIO_PIN2, "gpio2",	NULL),
	LED_STD(ADM5120_GPIO_PIN3, "gpio3",	NULL),
	LED_STD(ADM5120_GPIO_PIN4, "gpio4",	NULL),
	LED_STD(ADM5120_GPIO_PIN5, "gpio5",	NULL),
	LED_STD(ADM5120_GPIO_PIN6, "gpio6",	NULL),
	LED_STD(ADM5120_GPIO_PIN7, "gpio7",	NULL),
	LED_STD(ADM5120_GPIO_P0L0, "port0led0",	NULL),
	LED_STD(ADM5120_GPIO_P0L1, "port0led1",	NULL),
	LED_STD(ADM5120_GPIO_P0L2, "port0led2",	NULL),
	LED_STD(ADM5120_GPIO_P1L0, "port1led0",	NULL),
	LED_STD(ADM5120_GPIO_P1L1, "port1led1",	NULL),
	LED_STD(ADM5120_GPIO_P1L2, "port1led2",	NULL),
	LED_STD(ADM5120_GPIO_P2L0, "port2led0",	NULL),
	LED_STD(ADM5120_GPIO_P2L1, "port2led1",	NULL),
	LED_STD(ADM5120_GPIO_P2L2, "port2led2",	NULL),
	LED_STD(ADM5120_GPIO_P3L0, "port3led0",	NULL),
	LED_STD(ADM5120_GPIO_P3L1, "port3led1",	NULL),
	LED_STD(ADM5120_GPIO_P3L2, "port3led2",	NULL),
	LED_STD(ADM5120_GPIO_P4L0, "port4led0",	NULL),
	LED_STD(ADM5120_GPIO_P4L1, "port4led1",	NULL),
	LED_STD(ADM5120_GPIO_P4L2, "port4led2",	NULL),
#endif
};

#define MACH_DATA(m, n) { 				\
	.machtype	= (m),				\
	.nr_leds	= ARRAY_SIZE(n ## _leds),	\
	.leds		= n ## _leds 			\
}

static struct mach_data machines[] __initdata = {
	MACH_DATA(MACH_ADM5120_GENERIC, generic),
	/* Cellvision */
	MACH_DATA(MACH_ADM5120_CAS771, cas771),
	/* Compex */
	MACH_DATA(MACH_ADM5120_NP28G,	np28g),
	MACH_DATA(MACH_ADM5120_NP28GHS,	np28g),
	MACH_DATA(MACH_ADM5120_WP54AG,	wp54g),
	MACH_DATA(MACH_ADM5120_WP54G,	wp54g),
	MACH_DATA(MACH_ADM5120_WP54G_WRT, wp54g),
	MACH_DATA(MACH_ADM5120_WPP54AG,	wp54g),
	MACH_DATA(MACH_ADM5120_WPP54G,	wp54g),
	/* Edimax */
	MACH_DATA(MACH_ADM5120_BR6104K,	br6104k),
	MACH_DATA(MACH_ADM5120_BR61x4WG, br61x4wg),
	/* Infineon */
	MACH_DATA(MACH_ADM5120_EASY5120RT, easy5120rt),
	/* Mikrotik */
	MACH_DATA(MACH_ADM5120_RB_111,	rb100),
	MACH_DATA(MACH_ADM5120_RB_112,	rb100),
	MACH_DATA(MACH_ADM5120_RB_133,	rb133),
	MACH_DATA(MACH_ADM5120_RB_133C,	rb133c),
	MACH_DATA(MACH_ADM5120_RB_150,	rb150),
	MACH_DATA(MACH_ADM5120_RB_153,	rb153),
	/* ZyXEL */
	MACH_DATA(MACH_ADM5120_P334WT,	p334wt),
#if defined(CONFIG_LEDS_ADM5120_EXPERIMENTAL)
	/* untested */
	MACH_DATA(MACH_ADM5120_P334,	p334),
	MACH_DATA(MACH_ADM5120_P335,	p335),
	MACH_DATA(MACH_ADM5120_NP27G,	np27g),
#endif
};

static struct adm5120_leddev * __init
create_leddev(int id, struct gpio_led *led)
{
	struct adm5120_leddev *p;

	p = kzalloc(sizeof(*p), GFP_KERNEL);
	if (p == NULL)
		return NULL;

	memcpy(&p->led, led, sizeof(p->led));
	p->pdev.name = "leds-gpio";
	p->pdev.id = id;
	p->pdev.dev.platform_data = &p->pdata;
	p->pdata.num_leds = 1;
	p->pdata.leds = &p->led;

	return p;
}

static struct mach_data * __init
adm5120_leds_findmach(unsigned long machtype)
{
	struct mach_data *mach;
	int i;

	mach = NULL;
	for (i = 0; i < ARRAY_SIZE(machines); i++) {
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

	mach = adm5120_leds_findmach(mips_machtype);
	if (mach == NULL) {
		printk(KERN_ERR "leds-adm5120: unsupported board\n");
		ret = -EINVAL;
		goto err;
	}

	for (i = 0; i < mach->nr_leds; i++) {
		led_devs[i] = create_leddev(i, &mach->leds[i]);
		if (led_devs[i] == NULL) {
			ret = -ENOMEM;
			goto err_destroy;
		}
	}

	for (i = 0; i < mach->nr_leds; i++) {
		ret = platform_device_register(&led_devs[i]->pdev);
		if (ret)
			goto err_unregister;
	}

	led_count = mach->nr_leds;
	return 0;

err_unregister:
	for (i--; i >= 0; i--)
		platform_device_unregister(&led_devs[i]->pdev);

err_destroy:
	for (i = 0; i < led_count; i++)
		kfree(led_devs[i]);
err:
	return ret;
}

static void __exit
adm5120_leds_exit(void)
{
	int i;

	for (i = 0; i < led_count; i++) {
		platform_device_unregister(&led_devs[i]->pdev);
		kfree(led_devs[i]);
	}
}

module_init(adm5120_leds_init);
module_exit(adm5120_leds_exit);

MODULE_AUTHOR("Gabor Juhos <juhosg at openwrt.org>");
MODULE_DESCRIPTION(DRV_DESC);
MODULE_LICENSE("GPL v2");
MODULE_VERSION(DRV_VERSION);

