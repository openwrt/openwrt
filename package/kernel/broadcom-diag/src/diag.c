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
	WRT54G3G,
	WRT350N,
	WRT600N,
	WRT600NV11,

	/* ASUS */
	WL500G,
	ASUS_4702,

	/* Buffalo */
	WBR2_G54,
	WLA2_G54L,
	BUFFALO_UNKNOWN_4710,

	/* Siemens */
	SE505V1,
	SE505V2,

	/* US Robotics */
	USR5461,

	/* Dell */
	TM2300V2,

	/* Motorola */
	WR850GV1,

	/* Netgear */
	WGT634U,
	WNR834BV1,
	WNDR3700V3,

	/* Trendware */
	TEW411BRPP,

	/* SimpleTech */
	STI_NAS,

	/* D-Link */
	DIR320,
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

	/* Huawei */
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
	},
	/* Asus */
	[WL500G] = {
		.name		= "ASUS WL-500g",
		.buttons	= {
			{ .name = "reset",	.gpio = 1 << 6 },
		},
		.leds		= {
			{ .name = "power",	.gpio = 1 << 0, .polarity = REVERSE },
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
	/* Buffalo */
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
	/* Huawei */
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

	switch(board) {
	case BCM47XX_BOARD_BUFFALO_WBR2_G54:
		return &platforms[WBR2_G54];
	case BCM47XX_BOARD_BUFFALO_WLA2_G54L:
		return &platforms[WLA2_G54L];
	case BCM47XX_BOARD_LINKSYS_WRT54G:
		return &platforms[WRT54G];
	case BCM47XX_BOARD_NETGEAR_WNDR3700V3:
		return &platforms[WNDR3700V3];
	case BCM47XX_BOARD_UNKNOWN:
	case BCM47XX_BOARD_NO:
		printk(MODULE_NAME ": unknown board found, try legacy detect\n");
		printk(MODULE_NAME ": please open a ticket at https://dev.openwrt.org and attach the complete nvram\n");
		return platform_detect_legacy();
	default:
		printk(MODULE_NAME ": board was detected as \"%s\", and kernel should handle it\n", board_name);
		return NULL;
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
