/*
 * diag.c - GPIO interface driver for Broadcom boards
 *
 * Copyright (C) 2006 Mike Baker <mbm@openwrt.org>,
 * Copyright (C) 2006-2007 Felix Fietkau <nbd@openwrt.org>
 * Copyright (C) 2008 Andy Boyett <agb@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/kmod.h>
#include <linux/proc_fs.h>
#include <linux/timer.h>
#include <linux/version.h>
#include <asm/uaccess.h>
#include <linux/workqueue.h>
#include <linux/skbuff.h>
#include <linux/netlink.h>
#include <linux/kobject.h>
#include <net/sock.h>
#include <bcm47xx_board.h>
extern u64 uevent_next_seqnum(void);

#include "gpio.h"
#include "diag.h"
#define getvar(str) (nvram_get(str)?:"")

static inline int startswith (char *source, char *cmp) { return !strncmp(source,cmp,strlen(cmp)); }
static int fill_event(struct event_t *);
static unsigned int gpiomask = 0;
module_param(gpiomask, int, 0644);

extern char *nvram_get(char *str);
static void led_flash(unsigned long dummy);

static struct platform_t platform;

static struct timer_list led_timer = TIMER_INITIALIZER(&led_flash, 0, 0);

static struct proc_dir_entry *diag, *leds;

#if LINUX_VERSION_CODE < KERNEL_VERSION(3,9,0)
static inline struct inode *file_inode(struct file *f)
{
	return f->f_path.dentry->d_inode;
}
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(3,10,0)
static inline void *PDE_DATA(const struct inode *inode)
{
	return PDE(inode)->data;
}
#endif


enum {
	/* Linksys */
	WAP54GV1,
	WAP54GV2,
	WAP54GV3,
	WRT54GV1,
	WRT54G,
	WRTSL54GS,
	WRT54G3G,
	WRT54G3GV2_VF,
	WRT150NV1,
	WRT150NV11,
	WRT160NV1,
	WRT160NV3,
	WRT300NV11,
	WRT350N,
	WRT600N,
	WRT600NV11,
	WRT610N,
	WRT610NV2,
	E1000V1,
	E3000V1,
	E3200V1,
	E4200V1,

	/* ASUS */
	WLHDD,
	WL300G,
	WL320GE,
	WL330GE,
	WL500G,
	WL500GD,
	WL500GP,
	WL500GPV2,
	WL500W,
	WL520GC,
	WL520GU,
	ASUS_4702,
	WL700GE,
	RTN12,
	RTN16,
	RTN66U,

	/* Buffalo */
	WBR2_G54,
	WHR_G54S,
	WHR_HP_G54,
	WHR_G125,
	WHR2_A54G54,
	WLA2_G54L,
	WZR_G300N,
	WZR_RS_G54,
	WZR_RS_G54HP,
	BUFFALO_UNKNOWN_4710,

	/* Siemens */
	SE505V1,
	SE505V2,

	/* US Robotics */
	USR5461,

	/* Dell */
	TM2300,
	TM2300V2,

	/* Motorola */
	WE800G,
	WR850GV1,
	WR850GV2V3,
	WR850GP,

	/* Belkin */
	BELKIN_UNKNOWN,
	BELKIN_F7D4301,

	/* Netgear */
	WGT634U,
	WNR834BV1,
	WNR834BV2,
	WNDR3400V1,
	WNDR3700V3,

	/* Trendware */
	TEW411BRPP,

	/* SimpleTech */
	STI_NAS,

	/* D-Link */
	DIR130,
	DIR320,
	DIR330,
	DWL3150,

	/* Sitecom */
	WL105B,

	/* Western Digital */
	WDNetCenter,

	/* Askey */
	RT210W,

	/* OvisLink */
	WL1600GL,

	/* Microsoft */
	MN700,

	/* Edimax */
	PS1208MFG,
};

static void __init bcm4780_init(void) {
		int pin = 1 << 3;

		/* Enables GPIO 3 that controls HDD and led power on ASUS WL-700gE */
		printk(MODULE_NAME ": Spinning up HDD and enabling leds\n");
		bcm47xx_gpio_outen(pin, pin);
		bcm47xx_gpio_control(pin, 0);
		bcm47xx_gpio_out(pin, pin);

		/* Wait 5s, so the HDD can spin up */
		set_current_state(TASK_INTERRUPTIBLE);
		schedule_timeout(HZ * 5);
}

static void __init NetCenter_init(void) {
		/* unset pin 6 (+12V) */
		int pin = 1 << 6;
		bcm47xx_gpio_outen(pin, pin);
		bcm47xx_gpio_control(pin, 0);
		bcm47xx_gpio_out(pin, pin);
		/* unset pin 1 (turn off red led, blue will light alone if +5V comes up) */
		pin = 1 << 1;
		bcm47xx_gpio_outen(pin, pin);
		bcm47xx_gpio_control(pin, 0);
		bcm47xx_gpio_out(pin, pin);
		/* unset pin 3 (+5V) and wait 5 seconds (harddisk spin up) */
		bcm4780_init();
}

static void __init bcm57xx_init(void) {
	int pin = 1 << 2;

	/* FIXME: switch comes up, but port mappings/vlans not right */
	bcm47xx_gpio_outen(pin, pin);
	bcm47xx_gpio_control(pin, 0);
	bcm47xx_gpio_out(pin, pin);
}

static struct platform_t __initdata platforms[] = {
	/* Linksys */
	[WAP54GV1] = {
		.name		= "Linksys WAP54G V1",
		.buttons	= {
			{ .name = "reset",	.gpio = 1 << 0 },
		},
		.leds		= {
			{ .name = "diag",	.gpio = 1 << 3 },
			{ .name = "wlan",	.gpio = 1 << 4 },
		},
	},
	[WAP54GV2] = {
		.name		= "Linksys WAP54G V2",
		.buttons	= {
			{ .name = "reset",	.gpio = 1 << 0 },
		},
		.leds		= {
			{ .name = "wlan",	.gpio = 1 << 5, .polarity = REVERSE },
			/* GPIO 6 is b44 (eth0, LAN) PHY power */
		},
	},
	[WAP54GV3] = {
		.name		= "Linksys WAP54G V3",
		.buttons	= {
			/* FIXME: verify this */
			{ .name = "reset",	.gpio = 1 << 7 },
			{ .name = "ses",	.gpio = 1 << 0 },
		},
		.leds		= {
			/* FIXME: diag? */
			{ .name = "ses",	.gpio = 1 << 1 },
		},
	},
	[WRT54GV1] = {
		.name		= "Linksys WRT54G V1.x",
		.buttons	= {
			{ .name = "reset",	.gpio = 1 << 6 },
		},
		.leds		= {
			{ .name = "diag",	.gpio = 0x13 | GPIO_TYPE_EXTIF, .polarity = NORMAL },
			{ .name = "dmz",	.gpio = 0x12 | GPIO_TYPE_EXTIF, .polarity = NORMAL },
		},
	},
	[WRT54G] = {
		.name		= "Linksys WRT54G/GS/GL",
		.buttons	= {
			{ .name = "reset",	.gpio = 1 << 6 },
			{ .name = "ses",	.gpio = 1 << 4 },
		},
		.leds		= {
			{ .name = "power",	.gpio = 1 << 1, .polarity = NORMAL },
			{ .name = "dmz",	.gpio = 1 << 7, .polarity = REVERSE },
			{ .name = "ses_white",	.gpio = 1 << 2, .polarity = REVERSE },
			{ .name = "ses_orange",	.gpio = 1 << 3, .polarity = REVERSE },
			{ .name = "wlan",	.gpio = 1 << 0, .polarity = REVERSE },
		},
	},
	[WRTSL54GS] = {
		.name		= "Linksys WRTSL54GS",
		.buttons	= {
			{ .name = "reset",	.gpio = 1 << 6 },
			{ .name = "ses",	.gpio = 1 << 4 },
		},
		.leds		= {
			{ .name = "power",	.gpio = 1 << 1, .polarity = NORMAL },
			{ .name = "dmz",	.gpio = 1 << 0, .polarity = REVERSE },
			{ .name = "ses_white",	.gpio = 1 << 5, .polarity = REVERSE },
			{ .name = "ses_orange",	.gpio = 1 << 7, .polarity = REVERSE },
		},
	},
	[WRT54G3G] = {
		.name		= "Linksys WRT54G3G",
		.buttons	= {
			{ .name = "reset",	.gpio = 1 << 6 },
			{ .name = "3g",		.gpio = 1 << 4 },
		},
		.leds		= {
			{ .name = "power",	.gpio = 1 << 1, .polarity = NORMAL },
			{ .name = "dmz",	.gpio = 1 << 7, .polarity = REVERSE },
			{ .name = "3g_green",	.gpio = 1 << 2, .polarity = NORMAL },
			{ .name = "3g_blue",	.gpio = 1 << 3, .polarity = NORMAL },
			{ .name = "3g_blink",	.gpio = 1 << 5, .polarity = NORMAL },
		},
	},
	[WRT54G3GV2_VF] = {
		.name		= "Linksys WRT54G3GV2-VF",
		.buttons	= {
			{ .name = "reset",	.gpio = 1 << 6 },
			{ .name = "3g",		.gpio = 1 << 5 },
		},
		.leds		= {
			{ .name = "power",	.gpio = 1 << 1, .polarity = NORMAL },
			{ .name = "3g_green",	.gpio = 1 << 2, .polarity = NORMAL },
			{ .name = "3g_blue",	.gpio = 1 << 3, .polarity = NORMAL },
		},
	},
	[WRT150NV1] = {
		.name		= "Linksys WRT150N V1",
		.buttons	= {
			{ .name = "reset",	.gpio = 1 << 6 },
			{ .name = "ses",	.gpio = 1 << 4 },
		},
		.leds		= {
			{ .name = "power",	.gpio = 1 << 1, .polarity = NORMAL },
			{ .name = "ses_green",	.gpio = 1 << 5, .polarity = REVERSE },
			{ .name = "ses_amber", .gpio = 1 << 3, .polarity = REVERSE },
		},
	},
	[WRT150NV11] = {
		.name		= "Linksys WRT150N V1.1",
		.buttons	= {
			{ .name = "reset",	.gpio = 1 << 6 },
			{ .name = "ses",	.gpio = 1 << 4 },
		},
		.leds		= {
			{ .name = "power",	.gpio = 1 << 1, .polarity = NORMAL },
			{ .name = "ses_green",	.gpio = 1 << 5, .polarity = REVERSE },
			{ .name = "ses_amber", .gpio = 1 << 3, .polarity = REVERSE },
		},
	},
	[WRT160NV1] = {
		.name		= "Linksys WRT160N v1.x",
		.buttons	= {
			{ .name = "reset",	.gpio = 1 << 6 },
			{ .name = "ses",	.gpio = 1 << 4 },
		},
		.leds		= {
			{ .name = "power",	.gpio = 1 << 1, .polarity = NORMAL },
			{ .name = "ses_blue",	.gpio = 1 << 5, .polarity = REVERSE },
			{ .name = "ses_orange", .gpio = 1 << 3, .polarity = REVERSE },
		},
	},
	[WRT160NV3] = {
		.name		= "Linksys WRT160N V3",
		.buttons	= {
			{ .name = "reset",	.gpio = 1 << 6 },
			{ .name = "ses",	.gpio = 1 << 5 },
		},
		.leds		= {
			{ .name = "power",	.gpio = 1 << 1, .polarity = NORMAL },
			{ .name = "ses_blue",	.gpio = 1 << 4, .polarity = REVERSE },
			{ .name = "ses_orange", .gpio = 1 << 2, .polarity = REVERSE },
		},
	},
	[WRT300NV11] = {
		.name           = "Linksys WRT300N V1.1",
		.buttons        = {
			{ .name = "reset",     .gpio = 1 << 6 }, // "Reset" on back panel
			{ .name = "ses",       .gpio = 1 << 4 }, // "Reserved" on top panel
		},
		.leds           = {
			{ .name = "power",     .gpio = 1 << 1, .polarity = NORMAL  }, // "Power"
			{ .name = "ses_amber", .gpio = 1 << 3, .polarity = REVERSE }, // "Security" Amber
			{ .name = "ses_green", .gpio = 1 << 5, .polarity = REVERSE }, // "Security" Green
		},
		.platform_init = bcm57xx_init,
	},
	[WRT350N] = {
		.name		= "Linksys WRT350N",
		.buttons	= {
			{ .name = "reset",	.gpio = 1 << 6 },
			{ .name = "ses",	.gpio = 1 << 8 },
		},
		.leds		= {
			{ .name = "power",	.gpio = 1 << 1, .polarity = NORMAL },
			{ .name = "ses_amber",	.gpio = 1 << 3, .polarity = REVERSE },
			{ .name = "ses_green",	.gpio = 1 << 9, .polarity = REVERSE },
			{ .name = "usb_blink",	.gpio = 1 << 10, .polarity = REVERSE },
			{ .name = "usb",	.gpio = 1 << 11, .polarity = REVERSE },
		},
		.platform_init = bcm57xx_init,
	},
	[WRT600N] = {
		.name           = "Linksys WRT600N",
		.buttons        = {
			{ .name = "reset",      .gpio = 1 << 6 },
			{ .name = "ses",        .gpio = 1 << 7 },
		},
		.leds           = {
			{ .name = "power",              .gpio = 1 << 2,  .polarity = REVERSE }, // Power LED
			{ .name = "usb",                .gpio = 1 << 3,  .polarity = REVERSE }, // USB LED
			{ .name = "wl0_ses_amber",      .gpio = 1 << 8,  .polarity = REVERSE }, // 2.4Ghz LED Amber
			{ .name = "wl0_ses_green",      .gpio = 1 << 9,  .polarity = REVERSE }, // 2.4Ghz LED Green
			{ .name = "wl1_ses_amber",      .gpio = 1 << 10, .polarity = REVERSE }, // 5.6Ghz LED Amber
			{ .name = "wl1_ses_green",      .gpio = 1 << 11, .polarity = REVERSE }, // 5.6Ghz LED Green
		},
		.platform_init = bcm57xx_init,
	},
	[WRT600NV11] = {
		.name           = "Linksys WRT600N V1.1",
		.buttons        = {
			{ .name = "reset",      .gpio = 1 << 6 },
			{ .name = "ses",        .gpio = 1 << 7 },
		},
		.leds           = {
			{ .name = "power",             .gpio = 1 << 2,  .polarity = REVERSE }, // Power LED
			{ .name = "usb",                .gpio = 1 << 3,  .polarity = REVERSE }, // USB LED
			{ .name = "wl0_ses_amber",      .gpio = 1 << 8,  .polarity = REVERSE }, // 2.4Ghz LED Amber
			{ .name = "wl0_ses_green",     .gpio = 1 << 9,  .polarity = REVERSE }, // 2.4Ghz LED Green
			{ .name = "wl1_ses_amber",      .gpio = 1 << 10, .polarity = REVERSE }, // 5.6Ghz LED Amber
			{ .name = "wl1_ses_green",      .gpio = 1 << 11, .polarity = REVERSE }, // 5.6Ghz LED Green
		},
		.platform_init = bcm57xx_init,
	},
	[WRT610N] = {
		.name           = "Linksys WRT610N",
		.buttons        = {
			{ .name = "reset",      .gpio = 1 << 6 },
			{ .name = "ses",        .gpio = 1 << 8 },
		},
		.leds           = {
			{ .name = "power",      .gpio = 1 << 1,  .polarity = NORMAL }, // Power LED
			{ .name = "usb",        .gpio = 1 << 0,  .polarity = REVERSE }, // USB LED
			{ .name = "ses_amber",  .gpio = 1 << 3,  .polarity = REVERSE }, // WiFi protected setup LED amber
			{ .name = "ses_blue",   .gpio = 1 << 9,  .polarity = REVERSE }, // WiFi protected setup LED blue
		},
	},
	[WRT610NV2] = {
		.name		= "Linksys WRT610N V2",
		.buttons	= {
			{ .name = "reset",	.gpio = 1 << 6 },
			{ .name = "ses",	.gpio = 1 << 4 },
		},
		.leds		= {
			{ .name = "power",	.gpio = 1 << 5,	.polarity = NORMAL },	// Power LED
			{ .name = "usb",	.gpio = 1 << 7,	.polarity = NORMAL },	// USB LED
			{ .name = "ses_amber",	.gpio = 1 << 0,	.polarity = REVERSE },	// WiFi protected setup LED amber
			{ .name = "ses_blue",	.gpio = 1 << 3,	.polarity = REVERSE },	// WiFi protected setup LED blue
			{ .name = "wlan",	.gpio = 1 << 1,	.polarity = NORMAL },	// Wireless LED
		},
	},
	/* same hardware as WRT160NV3 and Cisco Valet M10V1, but different board detection, combine? */
	[E1000V1] = {
		.name           = "Linksys E1000 V1",
		.buttons        = {
			{ .name = "reset",      .gpio = 1 << 6 },
			{ .name = "wps",        .gpio = 1 << 5 }, /* nvram get gpio5=wps_button */
		},
		.leds           = {
			/** turns on leds for all ethernet ports (wan too)
			 *  this also disconnects some, or maybe all, ethernet ports 
			 *  1: leds work normally
			 *  0: all lit all the time */
			/* nvram get gpio3=robo_reset */
			{ .name = "wlan",       .gpio = 1 << 0, .polarity = NORMAL },
			{ .name = "power",      .gpio = 1 << 1, .polarity = NORMAL },
			{ .name = "ses_blue",   .gpio = 1 << 4, .polarity = REVERSE }, /* nvram get gpio4=wps_led */
			{ .name = "ses_orange", .gpio = 1 << 2, .polarity = REVERSE }, /* nvram get gpio2=wps_status_led */
		},
	},
	[E3000V1] = {
		.name		= "Linksys E3000 V1",
		.buttons	= {
			{ .name = "reset",	.gpio = 1 << 6 },
			{ .name = "ses",	.gpio = 1 << 4 },
		},
		.leds		= {
			{ .name = "power",	.gpio = 1 << 5,	.polarity = NORMAL },	// Power LED
			{ .name = "usb",	.gpio = 1 << 7,	.polarity = NORMAL },	// USB LED
			{ .name = "ses_amber",	.gpio = 1 << 0,	.polarity = REVERSE },	// WiFi protected setup LED amber
			{ .name = "ses_blue",	.gpio = 1 << 3,	.polarity = REVERSE },	// WiFi protected setup LED blue
			{ .name = "wlan",	.gpio = 1 << 1,	.polarity = NORMAL },	// Wireless LED
		},
	},
	[E3200V1] = {
		.name		= "Linksys E3200 V1",
		.buttons	= {
			/* { .name = "switch",	.gpio = 1 << 4 },*/	/* nvram get gpio4=robo_reset */
			{ .name = "reset",	.gpio = 1 << 5 },	/* nvram get reset_gpio=5 */
			{ .name = "wps",	.gpio = 1 << 8 },	/* nvram get gpio8=wps_button */
			/* { .name = "wombo",	.gpio = 1 << 23 },*/	/* nvram get gpio23=wombo_reset - wireless on motherboard */
		},
		.leds	= {
			{ .name = "power",	.gpio = 1 << 3, .polarity = REVERSE },	/* Power LED */
		},
	},
	[E4200V1] = {
		.name		= "Linksys E4200 V1",
		.buttons	= {
			{ .name = "reset",	.gpio = 1 << 6 },
			{ .name = "wps",	.gpio = 1 << 4 },
		},
		.leds	= {
			{ .name = "power",	.gpio = 1 << 5, .polarity = REVERSE },
		},
	},
	/* Asus */
	[WLHDD] = {
		.name		= "ASUS WL-HDD",
		.buttons	= {
			{ .name = "reset",	.gpio = 1 << 6 },
		},
		.leds		= {
			{ .name = "power",	.gpio = 1 << 0, .polarity = REVERSE },
			{ .name = "usb",	.gpio = 1 << 2, .polarity = REVERSE },
		},
	},
	[WL300G] = {
		.name		= "ASUS WL-300g",
		.buttons	= {
			{ .name = "reset",	.gpio = 1 << 6 },
		},
		.leds		= {
			{ .name = "power",	.gpio = 1 << 0, .polarity = REVERSE },
		},
	},
	[WL320GE] = {
		.name		= "ASUS WL-320gE/WL-320gP",
		.buttons	= {
			{ .name = "reset",	.gpio = 1 << 6 },
		},
		.leds		= {
			{ .name = "wlan",	.gpio = 1 << 0, .polarity = REVERSE },
			{ .name = "power",	.gpio = 1 << 2, .polarity = REVERSE },
			{ .name = "link",	.gpio = 1 << 11, .polarity = REVERSE },
		},
	},
	[WL330GE] = {
		.name		= "ASUS WL-330gE",
		.buttons	= {
			{ .name = "reset",	.gpio = 1 << 2 },
		},
		.leds		= {
			{ .name = "power",	.gpio = 1 << 0, .polarity = REVERSE },
		},
	},
	[WL500G] = {
		.name		= "ASUS WL-500g",
		.buttons	= {
			{ .name = "reset",	.gpio = 1 << 6 },
		},
		.leds		= {
			{ .name = "power",	.gpio = 1 << 0, .polarity = REVERSE },
		},
	},
	[WL500GD] = {
		.name		= "ASUS WL-500g Deluxe",
		.buttons	= {
			{ .name = "reset",	.gpio = 1 << 6 },
		},
		.leds		= {
			{ .name = "power",	.gpio = 1 << 0, .polarity = REVERSE },
		},
	},
	[WL500GP] = {
		.name		= "ASUS WL-500g Premium",
		.buttons	= {
			{ .name = "reset",	.gpio = 1 << 0 },
			{ .name = "ses",	.gpio = 1 << 4 },
		},
		.leds		= {
			{ .name = "power",	.gpio = 1 << 1, .polarity = REVERSE },
		},
	},
	[WL500GPV2] = {
		.name		= "ASUS WL-500g Premium V2",
		.buttons	= {
			{ .name = "reset",	.gpio = 1 << 2 },
			{ .name = "ses",	.gpio = 1 << 3 },
		},
		.leds		= {
			{ .name = "power",	.gpio = 1 << 0, .polarity = REVERSE },
			{ .name = "wlan",	.gpio = 1 << 1, .polarity = REVERSE },
		},
	},
	[WL500W] = {
		.name		= "ASUS WL-500W",
		.buttons	= {
			{ .name = "reset",	.gpio = 1 << 6 },
			{ .name = "ses",	.gpio = 1 << 7 },
		},
		.leds		= {
			{ .name = "power",	.gpio = 1 << 5, .polarity = REVERSE },
		},
	},
	[WL520GC] = {
		.name		= "ASUS WL-520GC",
		.buttons	= {
			{ .name = "reset",	.gpio = 1 << 2 },
			{ .name = "ses",	.gpio = 1 << 3 },
		},
		.leds		= {
		{ .name = "power",	.gpio = 1 << 0, .polarity = REVERSE },
			{ .name = "wlan",	.gpio = 1 << 1, .polarity = REVERSE },
		},
	},
	[WL520GU] = {
		.name		= "ASUS WL-520gU",
		.buttons	= {
			{ .name = "reset",	.gpio = 1 << 2 },
			{ .name = "ses",	.gpio = 1 << 3 },
		},
		.leds		= {
			{ .name = "power",	.gpio = 1 << 0, .polarity = REVERSE },
			{ .name = "wlan",	.gpio = 1 << 1, .polarity = REVERSE },
		},
	},
	[ASUS_4702] = {
		.name		= "ASUS (unknown, BCM4702)",
		.buttons	= {
			{ .name = "reset",	.gpio = 1 << 6 },
		},
		.leds		= {
			{ .name = "power",	.gpio = 1 << 0, .polarity = REVERSE },
		},
	},
	[WL700GE] = {
		.name		= "ASUS WL-700gE",
		.buttons	= {
			{ .name = "reset",	.gpio = 1 << 7 }, // on back, hardwired, always resets device regardless OS state
			{ .name = "ses",	.gpio = 1 << 4 }, // on back, actual name ezsetup
			{ .name = "power",	.gpio = 1 << 0 }, // on front
			{ .name = "copy",	.gpio = 1 << 6 }, // on front
		},
		.leds		= {
#if 0
			// GPIO that controls power led also enables/disables some essential functions
			// - power to HDD
			// - switch leds
			{ .name = "power",	.gpio = 1 << 3, .polarity = NORMAL },  // actual name power
#endif
			{ .name = "diag",	.gpio = 1 << 1, .polarity = REVERSE }, // actual name ready
		},
		.platform_init = bcm4780_init,
	},
	[RTN12] = {
		.name		= "ASUS RT-N12",
		.buttons	= {
			{ .name = "wps",	.gpio = 1 << 0 },
			{ .name = "reset",	.gpio = 1 << 1 },
			// this is the router/repeater/ap switch
			{ .name = "sw1",	.gpio = 1 << 4 },
			{ .name = "sw2",	.gpio = 1 << 5 },
			{ .name = "sw3",	.gpio = 1 << 6 },
		},
		.leds		= {
			{ .name = "power",	.gpio = 1 << 2, .polarity = REVERSE },
			{ .name = "wlan",	.gpio = 1 << 7, .polarity = NORMAL },
			// gpio3 forces WAN and LAN1-4 all on
			//{ .name = "eth",	.gpio = 1 << 3, .polarity = REVERSE },
		},
	},
	[RTN16] = {
		.name		= "ASUS RT-N16",
		.buttons	= {
			{ .name = "reset",	.gpio = 1 << 8 },
			{ .name = "ses",	.gpio = 1 << 5 },
		},
		.leds		= {
			{ .name = "power",	.gpio = 1 << 1, .polarity = REVERSE },
			{ .name = "wlan",	.gpio = 1 << 7, .polarity = NORMAL },
		},
	},
	[RTN66U] = {
		.name		= "ASUS RT-N66U",
		.buttons	= {
			{ .name = "reset",	.gpio = 1 << 9 },
			{ .name = "wps",	.gpio = 1 << 4 },
		},
		.leds		= {
			{ .name = "power",	.gpio = 1 << 12, .polarity = REVERSE },
			{ .name = "usb",	.gpio = 1 << 15, .polarity = REVERSE },
		},
	},
	/* Buffalo */
	[WHR_G54S] = {
		.name		= "Buffalo WHR-G54S",
		.buttons	= {
			{ .name = "reset",	.gpio = 1 << 4 },
			{ .name = "bridge",	.gpio = 1 << 5 },
			{ .name = "ses",	.gpio = 1 << 0 },
		},
		.leds		= {
			{ .name = "diag",	.gpio = 1 << 7, .polarity = REVERSE },
			{ .name = "internal",	.gpio = 1 << 3, .polarity = REVERSE },
			{ .name = "ses",	.gpio = 1 << 6, .polarity = REVERSE },
			{ .name = "bridge",	.gpio = 1 << 1, .polarity = REVERSE },
			{ .name = "wlan",	.gpio = 1 << 2, .polarity = REVERSE },
		},
	},
	[WBR2_G54] = {
		.name		= "Buffalo WBR2-G54",
		/* FIXME: verify */
		.buttons	= {
			{ .name = "reset",	.gpio = 1 << 7 },
		},
		.leds		= {
			{ .name = "diag",	.gpio = 1 << 1, .polarity = REVERSE },
		},
	},
	[WHR_HP_G54] = {
		.name		= "Buffalo WHR-HP-G54",
		.buttons	= {
			{ .name = "reset",	.gpio = 1 << 4 },
			{ .name = "bridge",	.gpio = 1 << 5 },
			{ .name = "ses",	.gpio = 1 << 0 },
		},
		.leds		= {
			{ .name = "diag",	.gpio = 1 << 7, .polarity = REVERSE },
			{ .name = "internal",	.gpio = 1 << 3, .polarity = REVERSE },
			{ .name = "bridge",	.gpio = 1 << 1, .polarity = REVERSE },
			{ .name = "ses",	.gpio = 1 << 6, .polarity = REVERSE },
			{ .name = "wlan",	.gpio = 1 << 2, .polarity = REVERSE },
		},
	},
	[WHR_G125] = {
		.name		= "Buffalo WHR-G125",
		.buttons	= {
			{ .name = "reset",	.gpio = 1 << 4 },
			{ .name = "bridge",	.gpio = 1 << 5 },
			{ .name = "ses",	.gpio = 1 << 0 },
		},
		.leds		= {
			{ .name = "diag",	.gpio = 1 << 7, .polarity = REVERSE },
			{ .name = "internal",	.gpio = 1 << 3, .polarity = REVERSE },
			{ .name = "bridge",	.gpio = 1 << 1, .polarity = REVERSE },
			{ .name = "ses",	.gpio = 1 << 6, .polarity = REVERSE },
			{ .name = "wlan",	.gpio = 1 << 2, .polarity = REVERSE },
		},
	},
	[WHR2_A54G54] = {
		.name		= "Buffalo WHR2-A54G54",
		.buttons	= {
			{ .name = "reset",	.gpio = 1 << 4 },
		},
		.leds		= {
			{ .name = "diag",	.gpio = 1 << 7, .polarity = REVERSE },
		},
	},
	[WLA2_G54L] = {
		.name		= "Buffalo WLA2-G54L",
		/* FIXME: verify */
		.buttons	= {
			{ .name = "reset",	.gpio = 1 << 7 },
		},
		.leds		= {
			{ .name = "diag",	.gpio = 1 << 1, .polarity = REVERSE },
		},
	},
	[WZR_G300N] = {
		.name		= "Buffalo WZR-G300N",
		.buttons	= {
			{ .name = "reset",	.gpio = 1 << 4 },
		},
		.leds		= {
			{ .name = "diag",	.gpio = 1 << 7, .polarity = REVERSE },
			{ .name = "bridge",	.gpio = 1 << 1, .polarity = REVERSE },
			{ .name = "ses",	.gpio = 1 << 6, .polarity = REVERSE },
		},
	},
	[WZR_RS_G54] = {
		.name		= "Buffalo WZR-RS-G54",
		.buttons	= {
			{ .name = "ses",	.gpio = 1 << 0 },
			{ .name = "reset",	.gpio = 1 << 4 },
		},
		.leds		= {
			{ .name = "diag",	.gpio = 1 << 7, .polarity = REVERSE },
			{ .name = "ses",	.gpio = 1 << 6, .polarity = REVERSE },
			{ .name = "vpn",	.gpio = 1 << 1, .polarity = REVERSE },
		},
	},
	[WZR_RS_G54HP] = {
		.name		= "Buffalo WZR-RS-G54HP",
		.buttons	= {
			{ .name = "ses",	.gpio = 1 << 0 },
			{ .name = "reset",	.gpio = 1 << 4 },
		},
		.leds		= {
			{ .name = "diag",	.gpio = 1 << 7, .polarity = REVERSE },
			{ .name = "ses",	.gpio = 1 << 6, .polarity = REVERSE },
			{ .name = "vpn",	.gpio = 1 << 1, .polarity = REVERSE },
		},
	},
	[BUFFALO_UNKNOWN_4710] = {
		.name		= "Buffalo (unknown, BCM4710)",
		.buttons	= {
			{ .name = "reset",	.gpio = 1 << 4 },
		},
		.leds		= {
			{ .name = "diag",	.gpio = 1 << 1, .polarity = REVERSE },
		},
	},
	/* Siemens */
	[SE505V1] = {
		.name		= "Siemens SE505 V1",
		.buttons	= {
			/* No usable buttons */
		},
		.leds		= {
//			{ .name = "power",	.gpio = 1 << 0  .polarity = REVERSE },	// Usable when retrofitting D26 (?)
			{ .name = "dmz",	.gpio = 1 << 4, .polarity = REVERSE },	// actual name WWW
			{ .name = "wlan",	.gpio = 1 << 3, .polarity = REVERSE },
		},
	},
	[SE505V2] = {
		.name		= "Siemens SE505 V2",
		.buttons	= {
			/* No usable buttons */
		},
		.leds		= {
			{ .name = "power",	.gpio = 1 << 5, .polarity = REVERSE },
			{ .name = "dmz",	.gpio = 1 << 0, .polarity = REVERSE },	// actual name WWW
			{ .name = "wlan",	.gpio = 1 << 3, .polarity = REVERSE },
		},
	},
	/* US Robotics */
	[USR5461] = {
		.name		= "U.S. Robotics USR5461",
		.buttons	= {
			/* No usable buttons */
		},
		.leds		= {
			{ .name = "wlan",	.gpio = 1 << 0, .polarity = REVERSE },
			{ .name = "printer",	.gpio = 1 << 1, .polarity = REVERSE },
		},
	},
	/* Dell */
	[TM2300] = {
		.name		= "Dell TrueMobile 2300",
		.buttons	= {
			{ .name = "reset",	.gpio = 1 << 0 },
		},
		.leds		= {
			{ .name = "wlan",	.gpio = 1 << 6, .polarity = REVERSE },
			{ .name = "power",	.gpio = 1 << 7, .polarity = REVERSE },
		},
	},
	[TM2300V2] = {
		.name		= "Dell TrueMobile 2300 v2",
		.buttons	= {
			{ .name = "reset",	.gpio = 1 << 0 },
		},
		.leds		= {
			{ .name = "wlan",	.gpio = 1 << 6, .polarity = REVERSE },
			{ .name = "power",	.gpio = 1 << 7, .polarity = REVERSE },
		},
	},
	/* Motorola */
	[WE800G] = {
		.name		= "Motorola WE800G",
		.buttons	= {
			{ .name = "reset",	.gpio = 1 << 0 },
		},
		.leds		= {
			{ .name = "power",	.gpio = 1 << 4, .polarity = NORMAL },
			{ .name = "diag",	.gpio = 1 << 2, .polarity = REVERSE },
			{ .name = "wlan_amber",	.gpio = 1 << 1, .polarity = NORMAL },
		},
	},
	[WR850GV1] = {
		.name		= "Motorola WR850G V1",
		.buttons	= {
			{ .name = "reset",	.gpio = 1 << 0 },
		},
		.leds		= {
			{ .name = "power",	.gpio = 1 << 4, .polarity = NORMAL },
			{ .name = "diag",	.gpio = 1 << 3, .polarity = REVERSE },
			{ .name = "dmz",	.gpio = 1 << 6, .polarity = NORMAL },
			{ .name = "wlan_red",	.gpio = 1 << 5, .polarity = REVERSE },
			{ .name = "wlan_green",	.gpio = 1 << 7, .polarity = REVERSE },
		},
	},
	[WR850GV2V3] = {
		.name		= "Motorola WR850G V2/V3",
		.buttons	= {
			{ .name = "reset",	.gpio = 1 << 5 },
		},
		.leds		= {
			{ .name = "power",	.gpio = 1 << 1, .polarity = NORMAL },
			{ .name = "wlan",	.gpio = 1 << 0, .polarity = REVERSE },
			{ .name = "wan",	.gpio = 1 << 6, .polarity = INPUT },
			{ .name = "diag",	.gpio = 1 << 7, .polarity = REVERSE },
		},
	},
	[WR850GP] = {
		.name		= "Motorola WR850GP",
		.buttons	= {
			{ .name = "reset",	.gpio = 1 << 5 },
		},
		.leds		= {
			{ .name = "power",	.gpio = 1 << 1, .polarity = NORMAL },
			{ .name = "wlan",	.gpio = 1 << 0, .polarity = REVERSE },
			{ .name = "dmz",	.gpio = 1 << 6, .polarity = REVERSE },
			{ .name = "diag",	.gpio = 1 << 7, .polarity = REVERSE },
		},
	},

	/* Belkin */
	[BELKIN_UNKNOWN] = {
		.name		= "Belkin (unknown)",
		/* FIXME: verify & add detection */
		.buttons	= {
			{ .name = "reset",	.gpio = 1 << 7 },
		},
		.leds		= {
			{ .name = "power",	.gpio = 1 << 5, .polarity = NORMAL },
			{ .name = "wlan",	.gpio = 1 << 3, .polarity = NORMAL },
			{ .name = "connected",	.gpio = 1 << 0, .polarity = NORMAL },
		},
	},
	[BELKIN_F7D4301] = {
		.name		= "Belkin PlayMax F7D4301",
		.buttons	= {
			{ .name = "reset",	.gpio = 1 << 6 },
			{ .name = "wps",	.gpio = 1 << 8 },
		},
		.leds		= {
			{ .name = "power",	.gpio = 1 << 11, .polarity = REVERSE },
			{ .name = "wlan",	.gpio = 1 << 13, .polarity = REVERSE },
			{ .name = "led0",	.gpio = 1 << 14, .polarity = REVERSE },
			{ .name = "led1",	.gpio = 1 << 15, .polarity = REVERSE },
		},
	},
	/* Netgear */
	[WGT634U] = {
		.name		= "Netgear WGT634U",
		.buttons	= {
			{ .name = "reset",	.gpio = 1 << 2 },
		},
		.leds		= {
			{ .name = "power",	.gpio = 1 << 3, .polarity = NORMAL },
		},
	},
	/* Netgear */
	[WNR834BV1] = {
		.name		= "Netgear WNR834B V1",
		.buttons	= { /* TODO: add reset button and confirm LEDs - GPIO from dd-wrt */ },
		.leds		= {
			{ .name = "power",	.gpio = 1 << 4, .polarity = REVERSE },
			{ .name = "diag",	.gpio = 1 << 5, .polarity = REVERSE },
			{ .name = "wlan",	.gpio = 1 << 6, .polarity = REVERSE },
		},
	},
	/* Netgear */
	[WNR834BV2] = {
		.name	 	= "Netgear WNR834B V2",
		.buttons	= {
			{ .name = "reset",	.gpio = 1 << 6 },
		},
		.leds		= {
			{ .name = "power",	.gpio = 1 << 2, .polarity = NORMAL },
			{ .name = "diag",	.gpio = 1 << 3, .polarity = NORMAL },
			{ .name = "connected",	.gpio = 1 << 7, .polarity = NORMAL },
		},
	},
	[WNDR3400V1] = {
		.name		= "Netgear WNDR3400 V1",
		.buttons	= {
			/* nvram get gpio5=robo_reset */
			{ .name = "reset",	.gpio = 1 << 4 },
			{ .name = "wps",	.gpio = 1 << 6 },
			{ .name = "wlan",	.gpio = 1 << 8 },
		},
		.leds		= {
			{ .name = "wlan",	.gpio = 0 << 0, .polarity = NORMAL },
			{ .name = "connected",	.gpio = 1 << 0, .polarity = NORMAL },
			{ .name = "power",	.gpio = 1 << 3, .polarity = NORMAL },
			{ .name = "diag",	.gpio = 1 << 7, .polarity = NORMAL },
			{ .name = "usb",	.gpio = 1 << 2, .polarity = REVERSE },
		},
	},
	[WNDR3700V3] = {
		.name		= "Netgear WNDR3700 V3",
		.buttons	= {
			/* { .name = "usb",	.gpio = 1 << 1 }, */ /* this button doesn't seem to exist. */
			{ .name = "wlan",	.gpio = 1 << 2 },
			{ .name = "reset",	.gpio = 1 << 3 },
			{ .name = "wps",	.gpio = 1 << 4 },
			/* { .name = "switch",	.gpio = 1 << 5 },*/	/* nvram get gpio5=robo_reset */
		},
		.leds		= {
			{ .name = "power",	.gpio = (1 << 0) | GPIO_TYPE_SHIFT, .polarity = REVERSE },
			{ .name = "diag",	.gpio = (1 << 1) | GPIO_TYPE_SHIFT, .polarity = REVERSE },
			/* WAN LED doesn't respond to GPIO control. The switch is probably driving it.
			 * { .name = "wan",	.gpio = (1 << 2) | GPIO_TYPE_SHIFT, .polarity = REVERSE },
			 */
			{ .name = "wlan2g",	.gpio = (1 << 3) | GPIO_TYPE_SHIFT, .polarity = REVERSE },
			{ .name = "wlan5g",	.gpio = (1 << 4) | GPIO_TYPE_SHIFT, .polarity = REVERSE },
			{ .name = "usb",	.gpio = (1 << 5) | GPIO_TYPE_SHIFT, .polarity = REVERSE },
			{ .name = "wps",	.gpio = (1 << 6) | GPIO_TYPE_SHIFT, .polarity = REVERSE },
			{ .name = "wlan",	.gpio = (1 << 7) | GPIO_TYPE_SHIFT, .polarity = REVERSE },
		},
	},
	/* Trendware */
	[TEW411BRPP] = {
		.name           = "Trendware TEW411BRP+",
		.buttons        = {
			{ /* No usable buttons */ },
		},
		.leds           = {
			{ .name = "power",      .gpio = 1 << 7, .polarity = NORMAL },
			{ .name = "wlan",       .gpio = 1 << 1, .polarity = NORMAL },
			{ .name = "bridge",     .gpio = 1 << 6, .polarity = NORMAL },
		},
	},
	/* SimpleTech */
	[STI_NAS] = {
		.name	   = "SimpleTech SimpleShare NAS",
		.buttons	= {
			{ .name = "reset",      .gpio = 1 << 0 }, // Power button on back, named reset to enable failsafe.
		},
		.leds	   = {
			{ .name = "diag",       .gpio = 1 << 1, .polarity = REVERSE }, // actual name ready
		},
		.platform_init = bcm4780_init,
	},
	/* D-Link */
	[DIR130] = {
		.name	  = "D-Link DIR-130",
		.buttons	= {
			{ .name = "reset",	.gpio = 1 << 3},
			{ .name = "reserved",	.gpio = 1 << 7},
		},
		.leds	   = {
			{ .name = "diag",	.gpio = 1 << 0},
			{ .name = "blue",	.gpio = 1 << 6},
		},
	},
	[DIR320] = {
		.name	  = "D-Link DIR-320",
		.buttons	= {
			{ .name = "reserved",	.gpio = 1 << 6},
			{ .name = "reset",	.gpio = 1 << 7},
		},
		.leds	   = {
			{ .name = "wlan",	.gpio = 1 << 0, .polarity = NORMAL },
			{ .name = "diag",	.gpio = 1 << 1, .polarity = NORMAL }, /* "status led */
			{ .name = "red",	.gpio = 1 << 3, .polarity = REVERSE },
			{ .name = "blue",	.gpio = 1 << 4, .polarity = REVERSE },
			{ .name = "usb",	.gpio = 1 << 5, .polarity = NORMAL },
		},
	},
	[DIR330] = {
		.name	  = "D-Link DIR-330",
		.buttons	= {
			{ .name = "reset",	.gpio = 1 << 3},
			{ .name = "reserved",	.gpio = 1 << 7},
		},
		.leds	   = {
			{ .name = "diag",	.gpio = 1 << 0},
			{ .name = "usb",	.gpio = 1 << 4},
			{ .name = "blue",	.gpio = 1 << 6},
		},
	},
	[DWL3150] = {
		.name	= "D-Link DWL-3150",
		.buttons	= {
			{ .name = "reset",	.gpio = 1 << 7},
		},
		.leds	  = {
			{ .name = "diag",	.gpio = 1 << 2},
			{ .name = "status",	.gpio = 1 << 1},
		},
	},
	/* Double check */
	[WL105B] = {
		.name	= "Sitecom WL-105b",
		.buttons	= {
			{ .name = "reset",	.gpio = 1 << 10},
		},
		.leds	  = {
			{ .name = "wlan",	.gpio = 1 << 4},
			{ .name = "power",	.gpio = 1 << 3},
		},
	},
	/* Western Digital Net Center */
	[WDNetCenter] = {
		.name   = "Western Digital NetCenter",
		.buttons        = {
			{ .name = "power",	.gpio = 1 << 0},
			{ .name = "reset",	.gpio = 1 << 7},
		},
		.platform_init = NetCenter_init,
	},
	/* Askey (and clones) */
	[RT210W] = {
		.name		= "Askey RT210W",
		.buttons	= {
			/* Power button is hard-wired to hardware reset */
			/* but is also connected to GPIO 7 (probably for bootloader recovery)  */
			{ .name = "power",	.gpio = 1 << 7},
		},
		.leds		= {
		 	/* These were verified and named based on Belkin F5D4230-4 v1112 */
			{ .name = "connected",	.gpio = 1 << 0, .polarity = REVERSE },
			{ .name = "wlan",	.gpio = 1 << 3, .polarity = REVERSE },
			{ .name = "power",	.gpio = 1 << 5, .polarity = REVERSE },
		},
	},
	[WL1600GL] = {
		.name		= "OvisLink WL-1600GL",
		.buttons	= {
			{ .name = "reset",	.gpio = 1 << 3 },
			{ .name = "ses",	.gpio = 1 << 4 },
		},
		.leds		= {
			{ .name = "power",	.gpio = 1 << 5, .polarity = REVERSE },
			{ .name = "wps",	.gpio = 1 << 2, .polarity = REVERSE },
			{ .name = "wlan",	.gpio = 1 << 1, .polarity = REVERSE },
			{ .name = "connected",	.gpio = 1 << 0, .polarity = REVERSE },
		},
	},
	/* Microsoft */
	[MN700] = {
		.name   = "Microsoft MN-700",
		.buttons        = {
			{ .name = "reset",	.gpio = 1 << 7 },
		},
		.leds     = {
			{ .name = "power",	.gpio = 1 << 6, .polarity = NORMAL },
		},
	},
	/* Edimax */
	[PS1208MFG] = {
		.name   = "Edimax PS-1208MFG",
		.buttons        = {
			{ .name = "reset",	.gpio = 1 << 4 },
		},
		.leds     = {
			{ .name = "status",	.gpio = 1 << 1, .polarity = NORMAL },
			{ .name = "wlan",	.gpio = 1 << 0, .polarity = NORMAL },
		},
	},
};

static struct platform_t __init *platform_detect_legacy(void)
{
	char *boardnum, *boardtype;

	if (strcmp(getvar("nvram_type"), "cfe") == 0)
		return &platforms[WGT634U];


	/* no easy model number, attempt to guess */
	boardnum = getvar("boardnum");
	boardtype = getvar("boardtype");

	if (!strcmp(boardnum, "20070615")) { /* Linksys WRT600N  v1/V1.1 */
		if (!strcmp(boardtype, "0x478") && !strcmp(getvar("cardbus"), "0") && !strcmp(getvar("switch_type"),"BCM5395"))
			return &platforms[WRT600NV11];

	if (!strcmp(boardtype, "0x478") && !strcmp(getvar("cardbus"), "0"))
			return &platforms[WRT600N];
	}

	if (startswith(getvar("pmon_ver"), "CFE")) {
		/* CFE based - newer hardware */
		if (!strcmp(boardnum, "42")) { /* Linksys */
			if (!strcmp(boardtype, "0x478") && !strcmp(getvar("cardbus"), "1"))
				return &platforms[WRT350N];

			if (!strcmp(boardtype, "0x0101") && !strcmp(getvar("boot_ver"), "v3.6"))
				return &platforms[WRT54G3G];

			/* default to WRT54G if no boot_hw_model is set */
			if (nvram_get("boot_hw_model") == NULL)
				return &platforms[WRT54G];
		}
		if (!strcmp(boardnum, "1024") && !strcmp(boardtype, "0x0446"))
			return &platforms[WAP54GV2];

		if (!strcmp(boardnum, "8") && !strcmp(boardtype, "0x048e"))
			return &platforms[WL1600GL];


		if (!strcmp(boardnum, "44") || !strcmp(boardnum, "44\r")) {
			if (!strcmp(boardtype,"0x0101") || !strcmp(boardtype, "0x0101\r"))
				return &platforms[TM2300V2]; /* Dell TrueMobile 2300 v2 */
		}

		if (!strcmp(boardnum, "10496"))
			return &platforms[USR5461];

		if (!strcmp(getvar("boardtype"), "0x0101") && !strcmp(getvar("boardrev"), "0x10")) /* SE505V2 With Modified CFE */
			return &platforms[SE505V2];

		if (!strcmp(boardtype, "0x048e") && !strcmp(getvar("boardrev"),"0x35") &&
				!strcmp(getvar("boardflags"), "0x750")) /* D-Link DIR-320 */
			return &platforms[DIR320];

		if (!strncmp(boardnum, "TH",2) && !strcmp(boardtype,"0x042f")) {
			return &platforms[WDNetCenter];
		}

		if (!strcmp(boardtype,"0x0472") && !strcmp(getvar("cardbus"), "1")) { /* Netgear WNR834B  V1 and V2*/
			if (!strcmp(boardnum, "08") || !strcmp(boardnum, "8"))
				return &platforms[WNR834BV1];
			if (!strcmp(boardnum, "01") || !strcmp(boardnum, "1"))
				return &platforms[WNR834BV2];
		}

	} else { /* PMON based - old stuff */
		if ((simple_strtoul(getvar("GemtekPmonVer"), NULL, 0) == 9) &&
			(simple_strtoul(getvar("et0phyaddr"), NULL, 0) == 30)) {
			return &platforms[WR850GV1];
		}
		if (startswith(boardtype, "bcm94710dev")) {
			if (!strcmp(boardnum, "42"))
				return &platforms[WRT54GV1];
			if (simple_strtoul(boardnum, NULL, 0) == 2)
				return &platforms[WAP54GV1];
		}
		/* MN-700 has also hardware_version 'WL500-...', so use boardnum */
		if (startswith(getvar("hardware_version"), "WL500-")) {
			if (!strcmp(getvar("boardnum"), "mn700"))
				return &platforms[MN700];
			else
				return &platforms[WL500G];
		}
		/* Sitecom WL-105b */
		if (startswith(boardnum, "2") && simple_strtoul(getvar("GemtekPmonVer"), NULL, 0) == 1)
			return &platforms[WL105B];

		/* unknown asus stuff, probably bcm4702 */
		if (startswith(boardnum, "asusX"))
			return &platforms[ASUS_4702];

		/* bcm4702 based Askey RT210W clones, Including:
		 * Askey RT210W (duh?)
		 * Siemens SE505v1
		 * Belkin F5D7230-4 before version v1444 (MiniPCI slot, not integrated)
		 */
		if (!strcmp(boardtype,"bcm94710r4")
		 && !strcmp(boardnum ,"100")
		 && !strcmp(getvar("pmon_ver"),"v1.03.12.bk")
		   ){
			return &platforms[RT210W];
		}
	}

	if (boardnum || !strcmp(boardnum, "00")) {/* probably buffalo */
		if (startswith(boardtype, "bcm94710ap"))
			return &platforms[BUFFALO_UNKNOWN_4710];
	}

	if (startswith(getvar("CFEver"), "MotoWRv2") ||
		startswith(getvar("CFEver"), "MotoWRv3") ||
		!strcmp(getvar("MOTO_BOARD_TYPE"), "WR_FEM1")) {

		return &platforms[WR850GV2V3];
	}

	if (!strcmp(boardnum, "44") && !strcmp(getvar("boardflags"),"0x0388")) {  /* Trendware TEW-411BRP+ */
		return &platforms[TEW411BRPP];
	}

	if (startswith(boardnum, "04FN")) /* SimpleTech SimpleShare */
		return &platforms[STI_NAS];

	if (!strcmp(boardnum, "10") && !strcmp(getvar("boardrev"), "0x13")) /* D-Link DWL-3150 */
		return &platforms[DWL3150];

	if (!strcmp(boardnum, "01") && !strcmp(boardtype, "0x048e") && /* Edimax PS1208MFG */
		!strcmp(getvar("status_gpio"), "1")) /* gpio based detection */
		return &platforms[PS1208MFG];

	/* not found */
	return NULL;
}

static struct platform_t __init *platform_detect(void)
{
	enum bcm47xx_board board;
	const char *board_name;


	board = bcm47xx_board_get();
	board_name = bcm47xx_board_get_name();
	if (board != BCM47XX_BOARD_UNKNOWN && board != BCM47XX_BOARD_NON)
		printk(MODULE_NAME ": kernel found a \"%s\"\n", board_name);

	switch(board) {
	case BCM47XX_BOARD_ASUS_RTN12:
		return &platforms[RTN12];
	case BCM47XX_BOARD_ASUS_RTN16:
		return &platforms[RTN16];
	case BCM47XX_BOARD_ASUS_RTN66U:
		return &platforms[RTN66U];
	case BCM47XX_BOARD_ASUS_WL300G:
		return &platforms[WL300G];
	case BCM47XX_BOARD_ASUS_WL320GE:
		return &platforms[WL320GE];
	case BCM47XX_BOARD_ASUS_WL330GE:
		return &platforms[WL330GE];
	case BCM47XX_BOARD_ASUS_WL500GD:
		return &platforms[WL500GD];
	case BCM47XX_BOARD_ASUS_WL500GPV1:
		return &platforms[WL500GP];
	case BCM47XX_BOARD_ASUS_WL500GPV2:
		return &platforms[WL500GPV2];
	case BCM47XX_BOARD_ASUS_WL500W:
		return &platforms[WL500W];
	case BCM47XX_BOARD_ASUS_WL520GC:
		return &platforms[WL520GC];
	case BCM47XX_BOARD_ASUS_WL520GU:
		return &platforms[WL520GU];
	case BCM47XX_BOARD_ASUS_WL700GE:
		return &platforms[WL700GE];
	case BCM47XX_BOARD_ASUS_WLHDD:
		return &platforms[WLHDD];
	case BCM47XX_BOARD_BELKIN_F7D4301:
		return &platforms[BELKIN_F7D4301];
	case BCM47XX_BOARD_BUFFALO_WBR2_G54:
		return &platforms[WBR2_G54];
	case BCM47XX_BOARD_BUFFALO_WHR2_A54G54:
		return &platforms[WHR2_A54G54];
	case BCM47XX_BOARD_BUFFALO_WHR_G125:
		return &platforms[WHR_G125];
	case BCM47XX_BOARD_BUFFALO_WHR_G54S:
		return &platforms[WHR_G54S];
	case BCM47XX_BOARD_BUFFALO_WHR_HP_G54:
		return &platforms[WHR_HP_G54];
	case BCM47XX_BOARD_BUFFALO_WLA2_G54L:
		return &platforms[WLA2_G54L];
	case BCM47XX_BOARD_BUFFALO_WZR_G300N:
		return &platforms[WZR_G300N];
	case BCM47XX_BOARD_BUFFALO_WZR_RS_G54:
		return &platforms[WZR_RS_G54];
	case BCM47XX_BOARD_BUFFALO_WZR_RS_G54HP:
		return &platforms[WZR_RS_G54HP];
	case BCM47XX_BOARD_DELL_TM2300:
		return &platforms[TM2300];
	case BCM47XX_BOARD_DLINK_DIR130:
		return &platforms[DIR130];
	case BCM47XX_BOARD_DLINK_DIR330:
		return &platforms[DIR330];
	case BCM47XX_BOARD_LINKSYS_E1000V1:
		return &platforms[E1000V1];
	case BCM47XX_BOARD_LINKSYS_E3000V1:
		return &platforms[E3000V1];
	case BCM47XX_BOARD_LINKSYS_E3200V1:
		return &platforms[E3200V1];
	case BCM47XX_BOARD_LINKSYS_E4200V1:
		return &platforms[E4200V1];
	case BCM47XX_BOARD_LINKSYS_WRT150NV1:
		return &platforms[WRT150NV1];
	case BCM47XX_BOARD_LINKSYS_WRT150NV11:
		return &platforms[WRT150NV11];
	case BCM47XX_BOARD_LINKSYS_WRT160NV1:
		return &platforms[WRT160NV1];
	case BCM47XX_BOARD_LINKSYS_WRT160NV3:
		return &platforms[WRT160NV3];
	case BCM47XX_BOARD_LINKSYS_WRT300NV11:
		return &platforms[WRT300NV11];
	case BCM47XX_BOARD_LINKSYS_WRT54G3GV2:
		return &platforms[WRT54G3GV2_VF];
	case BCM47XX_BOARD_LINKSYS_WRT610NV1:
		return &platforms[WRT610N];
	case BCM47XX_BOARD_LINKSYS_WRT610NV2:
		return &platforms[WRT610NV2];
	case BCM47XX_BOARD_LINKSYS_WRTSL54GS:
		return &platforms[WRTSL54GS];
	case BCM47XX_BOARD_MOTOROLA_WE800G:
		return &platforms[WE800G];
	case BCM47XX_BOARD_MOTOROLA_WR850GP:
		return &platforms[WR850GP];
	case BCM47XX_BOARD_MOTOROLA_WR850GV2V3:
		return &platforms[WR850GV2V3];
	case BCM47XX_BOARD_NETGEAR_WNDR3400V1:
		return &platforms[WNDR3400V1];
	case BCM47XX_BOARD_NETGEAR_WNDR3700V3:
		return &platforms[WNDR3700V3];
	case BCM47XX_BOARD_UNKNOWN:
	case BCM47XX_BOARD_NON:
		printk(MODULE_NAME ": unknown board found, try legacy detect\n");
		printk(MODULE_NAME ": please open a ticket at https://dev.openwrt.org and attach the complete nvram\n");
		return platform_detect_legacy();
	default:
		printk(MODULE_NAME ": board was detected as \"%s\", but not gpio configuration available\n", board_name);
		printk(MODULE_NAME ": now trying legacy detect\n");
		return platform_detect_legacy();
	}
}

static inline void ssb_maskset32(struct ssb_device *dev,
				  u16 offset, u32 mask, u32 set)
{
	ssb_write32(dev, offset, (ssb_read32(dev, offset) & mask) | set);
}

static void gpio_set_irqenable(int enabled, irqreturn_t (*handler)(int, void *))
{
	int irq;
	int err;

	irq = gpio_to_irq(0);
	if (irq < 0) {
		pr_err("no irq for gpio available\n");
		return;
	}
	
	if (enabled) {
		err = request_irq(irq, handler, IRQF_SHARED, "gpio", handler);
		if (err) {
			pr_err("can not reqeust irq\n");
			return;
		}
	} else {
		free_irq(irq, handler);
	}

	switch (bcm47xx_bus_type) {
#ifdef CONFIG_BCM47XX_SSB
	case BCM47XX_BUS_TYPE_SSB:
		if (bcm47xx_bus.ssb.chipco.dev)
			ssb_maskset32(bcm47xx_bus.ssb.chipco.dev, SSB_CHIPCO_IRQMASK, ~SSB_CHIPCO_IRQ_GPIO, (enabled ? SSB_CHIPCO_IRQ_GPIO : 0));
		break;
#endif
#ifdef CONFIG_BCM47XX_BCMA
	case BCM47XX_BUS_TYPE_BCMA:
		if (bcm47xx_bus.bcma.bus.drv_cc.core)
			bcma_maskset32(bcm47xx_bus.bcma.bus.drv_cc.core, BCMA_CC_IRQMASK, ~BCMA_CC_IRQ_GPIO, (enabled ? BCMA_CC_IRQ_GPIO : 0));
		break;
#endif
	}
}

static void hotplug_button(struct work_struct *work)
{
	struct event_t *event = container_of(work, struct event_t, wq);
	char *s;

	event->skb = alloc_skb(2048, GFP_KERNEL);

	s = skb_put(event->skb, strlen(event->action) + 2);
	sprintf(s, "%s@", event->action);
	fill_event(event);

	NETLINK_CB(event->skb).dst_group = 1;
	broadcast_uevent(event->skb, 0, 1, GFP_KERNEL);

	kfree(event);
}

static irqreturn_t button_handler(int irq, void *dev_id)
{
	struct button_t *b;
	u32 in, changed;

	in = bcm47xx_gpio_in(~0) & platform.button_mask;
	bcm47xx_gpio_polarity(platform.button_mask, in);
	changed = platform.button_polarity ^ in;
	platform.button_polarity = in;

	changed &= ~bcm47xx_gpio_outen(0, 0);

	for (b = platform.buttons; b->name; b++) {
		struct event_t *event;

		if (!(b->gpio & changed)) continue;

		b->pressed ^= 1;

		if ((event = (struct event_t *)kzalloc (sizeof(struct event_t), GFP_ATOMIC))) {
			event->seen = (jiffies - b->seen)/HZ;
			event->name = b->name;
			event->action = b->pressed ? "pressed" : "released";
			INIT_WORK(&event->wq, (void *)(void *)hotplug_button);
			schedule_work(&event->wq);
		}

		b->seen = jiffies;
	}
	return IRQ_HANDLED;
}

static void register_buttons(struct button_t *b)
{
	for (; b->name; b++)
		platform.button_mask |= b->gpio;

	platform.button_mask &= ~gpiomask;

	bcm47xx_gpio_outen(platform.button_mask, 0);
	bcm47xx_gpio_control(platform.button_mask, 0);
	platform.button_polarity = bcm47xx_gpio_in(~0) & platform.button_mask;
	bcm47xx_gpio_polarity(platform.button_mask, platform.button_polarity);
	bcm47xx_gpio_intmask(platform.button_mask, platform.button_mask);

	gpio_set_irqenable(1, button_handler);
}

static void unregister_buttons(struct button_t *b)
{
	bcm47xx_gpio_intmask(platform.button_mask, 0);

	gpio_set_irqenable(0, button_handler);
}


static void add_msg(struct event_t *event, char *msg, int argv)
{
	char *s;

	if (argv)
		return;

	s = skb_put(event->skb, strlen(msg) + 1);
	strcpy(s, msg);
}

static int fill_event (struct event_t *event)
{
	static char buf[128];

	add_msg(event, "HOME=/", 0);
	add_msg(event, "PATH=/sbin:/bin:/usr/sbin:/usr/bin", 0);
	add_msg(event, "SUBSYSTEM=button", 0);
	snprintf(buf, 128, "ACTION=%s", event->action);
	add_msg(event, buf, 0);
	snprintf(buf, 128, "BUTTON=%s", event->name);
	add_msg(event, buf, 0);
	snprintf(buf, 128, "SEEN=%ld", event->seen);
	add_msg(event, buf, 0);
	snprintf(buf, 128, "SEQNUM=%llu", uevent_next_seqnum());
	add_msg(event, buf, 0);

	return 0;
}

/*
 * This should be extended to allow the platform to specify the pins and width
 * of the shift register. They're hardcoded for now because only the WNDR3700v3
 * uses it.
 */
static void shiftreg_output(unsigned int val)
{
	unsigned int mask;

	bcm47xx_gpio_out(SHIFTREG_DATA, SHIFTREG_DATA); /* init off, pull high */
	bcm47xx_gpio_out(SHIFTREG_CLK, 0); /* init reset */

	/* shift 8 times */
	for(mask = 1 << (SHIFTREG_MAX_BITS-1); mask; mask >>= 1)
	{
		bcm47xx_gpio_out(SHIFTREG_DATA, (val & mask) ? SHIFTREG_DATA : 0);
		bcm47xx_gpio_out(SHIFTREG_CLK, SHIFTREG_CLK); /* pull high to trigger */
		bcm47xx_gpio_out(SHIFTREG_CLK, 0); /* reset to low */
	}
}

static void set_led_shift(struct led_t *led)
{
	static u32	shiftreg = 0;
	u32			old = shiftreg;
	u32			pin = (led->gpio & ~GPIO_TYPE_MASK);

	if (led->state) {
		shiftreg |= pin;
	} else {
		shiftreg &= ~pin;
	}

	/* Clock the bits out. */
	if (shiftreg != old) {
		shiftreg_output(shiftreg);
	}
}

static void set_led_extif(struct led_t *led)
{
	volatile u8 *addr = (volatile u8 *) KSEG1ADDR(EXTIF_UART) + (led->gpio & ~GPIO_TYPE_MASK);
	if (led->state)
		*addr = 0xFF;
	else
		*addr;
}


static void led_flash(unsigned long dummy) {
	struct led_t *l;
	u32 mask = 0;
	u8 extif_blink = 0;

	for (l = platform.leds; l->name; l++) {
		if (!l->flash) continue;
		switch (l->gpio & GPIO_TYPE_MASK) {
		case GPIO_TYPE_EXTIF:
			extif_blink = 1;
			l->state = !l->state;
			set_led_extif(l);
			break;
		case GPIO_TYPE_SHIFT:
			extif_blink = 1;
			l->state = !l->state;
			set_led_shift(l);
			break;
		case GPIO_TYPE_NORMAL:
		default:
			mask |= l->gpio;
			break;
		}
	}

	mask &= ~gpiomask;
	if (mask) {
		u32 val = ~bcm47xx_gpio_in(~0);

		bcm47xx_gpio_outen(mask, mask);
		bcm47xx_gpio_control(mask, 0);
		bcm47xx_gpio_out(mask, val);
	}
	if (mask || extif_blink) {
		mod_timer(&led_timer, jiffies + FLASH_TIME);
	}
}

static int diag_led_show(struct seq_file *m, void *v)
{
	struct led_t * led = m->private;

	u8 p = (led->polarity == NORMAL ? 0 : 1);
	if (led->flash) {
		return seq_printf(m, "f\n");
	} else if ((led->gpio & GPIO_TYPE_MASK) != GPIO_TYPE_NORMAL) {
		return seq_printf(m, "%d\n", ((led->state ^ p) ? 1 : 0));
	} else {
		u32 in = (bcm47xx_gpio_in(~0) & led->gpio ? 1 : 0);
		return seq_printf(m, "%d\n", ((in ^ p) ? 1 : 0));
	}
}

static int diag_led_open(struct inode *inode, struct file *file)
{
	return single_open(file, diag_led_show, PDE_DATA(inode));
}

static ssize_t diag_led_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	struct led_t *led = PDE_DATA(file_inode(file));
	char cmd[5];
	size_t len;
	int p;

	len = min(count, sizeof(cmd) - 1);
	if (copy_from_user(cmd, buf, len))
		return -EFAULT;

	cmd[len] = 0;

	p = (led->polarity == NORMAL ? 0 : 1);
	if (cmd[0] == 'f') {
		led->flash = 1;
		led_flash(0);
	} else {
		led->flash = 0;
		if ((led->gpio & GPIO_TYPE_MASK) == GPIO_TYPE_EXTIF) {
			led->state = p ^ ((cmd[0] == '1') ? 1 : 0);
			set_led_extif(led);
		} else if ((led->gpio & GPIO_TYPE_MASK) == GPIO_TYPE_SHIFT) {
			led->state = p ^ ((cmd[0] == '1') ? 1 : 0);
			set_led_shift(led);
		} else {
			bcm47xx_gpio_outen(led->gpio, led->gpio);
			bcm47xx_gpio_control(led->gpio, 0);
			bcm47xx_gpio_out(led->gpio, ((p ^ (cmd[0] == '1')) ? led->gpio : 0));
		}
	}
	return count;
}

static const struct file_operations diag_led_fops = {
	.open = diag_led_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.write = diag_led_write
};

static void register_leds(struct led_t *l)
{
	struct proc_dir_entry *p;
	u32 mask = 0;
	u32 oe_mask = 0;
	u32 val = 0;

	leds = proc_mkdir("led", diag);
	if (!leds)
		return;

	for(; l->name; l++) {
		if (l->gpio & gpiomask)
			continue;

		switch (l->gpio & GPIO_TYPE_MASK) {
		case GPIO_TYPE_EXTIF:
			l->state = 0;
			set_led_extif(l);
			break;
		case GPIO_TYPE_SHIFT:
			mask |= (SHIFTREG_DATA | SHIFTREG_CLK);
			oe_mask |= (SHIFTREG_DATA | SHIFTREG_CLK);
			l->state = (l->polarity != NORMAL);
			set_led_shift(l);
			break;
		case GPIO_TYPE_NORMAL:
		default:
			if (l->polarity != INPUT) oe_mask |= l->gpio;
			mask |= l->gpio;
			val |= (l->polarity == NORMAL)?0:l->gpio;
			break;
		}

		if (l->polarity == INPUT) continue;

		p = proc_create_data(l->name, S_IRUSR, leds, &diag_led_fops, l);
	}

	bcm47xx_gpio_outen(mask, oe_mask);
	bcm47xx_gpio_control(mask, 0);
	bcm47xx_gpio_out(mask, val);
	bcm47xx_gpio_intmask(mask, 0);
}

static void unregister_leds(struct led_t *l)
{
	for(; l->name; l++)
		remove_proc_entry(l->name, leds);

	remove_proc_entry("led", diag);
}

static int diag_model_show(struct seq_file *m, void *v)
{
	return seq_printf(m, "%s\n", platform.name);
}

static int diag_model_open(struct inode *inode, struct file *file)
{
	return single_open(file, diag_model_show, PDE_DATA(inode));
}

static const struct file_operations diag_model_fops = {
	.open = diag_model_open,
	.read = seq_read,
	.llseek = seq_lseek
};

static int diag_gpiomask_show(struct seq_file *m, void *v)
{
	return seq_printf(m, "0x%04x\n", gpiomask);
}

static int diag_gpiomask_open(struct inode *inode, struct file *file)
{
	return single_open(file, diag_gpiomask_show, PDE_DATA(inode));
}

static ssize_t diag_gpiomask_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	int err = kstrtouint_from_user(buf, count, 0, &gpiomask);
	if (err)
		return err;

	if (platform.buttons) {
		unregister_buttons(platform.buttons);
		register_buttons(platform.buttons);
	}

	if (platform.leds) {
		unregister_leds(platform.leds);
		register_leds(platform.leds);
	}

	return count;
}

static const struct file_operations diag_gpiomask_fops = {
	.open = diag_gpiomask_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.write = diag_gpiomask_write
};

static int __init diag_init(void)
{
	static struct proc_dir_entry *p;
	static struct platform_t *detected;

	detected = platform_detect();
	if (!detected) {
		printk(MODULE_NAME ": Router model not detected.\n");
		return -ENODEV;
	}
	memcpy(&platform, detected, sizeof(struct platform_t));

	printk(MODULE_NAME ": Detected '%s'\n", platform.name);
	if (platform.platform_init != NULL) {
		platform.platform_init();
	}

	if (!(diag = proc_mkdir("diag", NULL))) {
		printk(MODULE_NAME ": proc_mkdir on /proc/diag failed\n");
		return -EINVAL;
	}

	p = proc_create("model", S_IRUSR, diag, &diag_model_fops);
	if (!p) {
		remove_proc_entry("diag", NULL);
		return -EINVAL;
	}

	p = proc_create("gpiomask", S_IRUSR | S_IWUSR, diag, &diag_gpiomask_fops);
	if (!p) {
		remove_proc_entry("model", diag);
		remove_proc_entry("diag", NULL);
		return -EINVAL;
	}

	if (platform.buttons)
		register_buttons(platform.buttons);

	if (platform.leds)
		register_leds(platform.leds);

	return 0;
}

static void __exit diag_exit(void)
{
	del_timer(&led_timer);

	if (platform.buttons)
		unregister_buttons(platform.buttons);

	if (platform.leds)
		unregister_leds(platform.leds);

	remove_proc_entry("model", diag);
	remove_proc_entry("gpiomask", diag);
	remove_proc_entry("diag", NULL);
}

module_init(diag_init);
module_exit(diag_exit);

MODULE_AUTHOR("Mike Baker, Felix Fietkau / OpenWrt.org");
MODULE_LICENSE("GPL");
