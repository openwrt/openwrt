/*
 * diag.c - GPIO interface driver for Broadcom boards
 *
 * Copyright (C) 2006 Mike Baker <mbm@openwrt.org>,
 *                    Felix Fietkau <nbd@openwrt.org>
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
 * $Id$
 */
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/kmod.h>
#include <linux/proc_fs.h>
#include <linux/timer.h>
#include <linux/version.h>
#include <asm/uaccess.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
#include <linux/kobject.h>
#include <linux/workqueue.h>
#define hotplug_path uevent_helper
#else
#include <linux/tqueue.h>
#define INIT_WORK INIT_TQUEUE
#define schedule_work schedule_task
#define work_struct tq_struct
#endif

#include "gpio.h"
#include "diag.h"
#define getvar(str) (nvram_get(str)?:"")

static unsigned int gpiomask = 0;
module_param(gpiomask, int, 0644);

enum {
	/* Linksys */
	WAP54GV1,
	WAP54GV3,
	WRT54GV1,
	WRT54G,
	WRTSL54GS,
	WRT54G3G,
	
	/* ASUS */
	WLHDD,
	WL300G,
	WL500G,
	WL500GD,
	WL500GP,
	ASUS_4702,
	
	/* Buffalo */
	WBR2_G54,
	WHR_G54S,
	WHR_HP_G54,
	WHR2_A54G54,
	WLA2_G54L,
	BUFFALO_UNKNOWN,
	BUFFALO_UNKNOWN_4710,

	/* Siemens */
	SE505V1,
	SE505V2,
	
	/* US Robotics */
	USR5461,

	/* Dell */
	TM2300,

	/* Motorola */
	WE800G,
	WR850GV1,
	WR850GV2V3,

	/* Belkin */
	BELKIN_UNKNOWN,
};

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
			{ .name = "dmz",	.gpio = 1 << 7, .polarity = REVERSE },
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
	/* Asus */
	[WLHDD] = {
		.name		= "ASUS WL-HDD",
		.buttons	= {
			{ .name = "reset",	.gpio = 1 << 6 },
		},
		.leds		= {
			{ .name = "power",	.gpio = 1 << 0, .polarity = REVERSE },
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
			{ .name = "bridge",	.gpio = 1 << 1, .polarity = REVERSE },
			{ .name = "ses",	.gpio = 1 << 6, .polarity = REVERSE },
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
	[BUFFALO_UNKNOWN] = {
		.name		= "Buffalo (unknown)",
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
			{ .name = "dmz",	.gpio = 1 << 4, .polarity = REVERSE },
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
			{ .name = "dmz",	.gpio = 1 << 0, .polarity = REVERSE },
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
			{ .name = "diag",	.gpio = 1 << 7, .polarity = REVERSE },
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
};

static struct platform_t __init *platform_detect(void)
{
	char *boardnum, *boardtype, *buf;

	boardnum = getvar("boardnum");
	boardtype = getvar("boardtype");
	if (strncmp(getvar("pmon_ver"), "CFE", 3) == 0) {
		/* CFE based - newer hardware */
		if (!strcmp(boardnum, "42")) { /* Linksys */
			if (!strcmp(boardtype, "0x0101") && !strcmp(getvar("boot_ver"), "v3.6"))
				return &platforms[WRT54G3G];

			if (!strcmp(getvar("et1phyaddr"),"5") && !strcmp(getvar("et1mdcport"), "1"))
				return &platforms[WRTSL54GS];
			
			/* default to WRT54G */
			return &platforms[WRT54G];
		}
		
		if (!strcmp(boardnum, "45")) { /* ASUS */
			if (!strcmp(boardtype,"0x042f"))
				return &platforms[WL500GP];
			else
				return &platforms[WL500GD];
		}
		
		if (!strcmp(boardnum, "10496"))
			return &platforms[USR5461];
	} else { /* PMON based - old stuff */
		if ((simple_strtoul(getvar("GemtekPmonVer"), NULL, 0) == 9) &&
			(simple_strtoul(getvar("et0phyaddr"), NULL, 0) == 30)) {
			if (!strncmp(getvar("ModelId"),"WE800G", 6))
				return &platforms[WE800G];
			else
				return &platforms[WR850GV1];
		}
		if (!strncmp(boardtype, "bcm94710dev", 11)) {
			if (!strcmp(boardnum, "42"))
				return &platforms[WRT54GV1];
			if (simple_strtoul(boardnum, NULL, 0) == 2)
				return &platforms[WAP54GV1];
		}
		if (!strncmp(getvar("hardware_version"), "WL500-", 6))
			return &platforms[WL500G];
		if (!strncmp(getvar("hardware_version"), "WL300-", 6)) {
			/* Either WL-300g or WL-HDD, do more extensive checks */
			if ((simple_strtoul(getvar("et0phyaddr"), NULL, 0) == 0) &&
				(simple_strtoul(getvar("et1phyaddr"), NULL, 0) == 1))
				return &platforms[WLHDD];
			if ((simple_strtoul(getvar("et0phyaddr"), NULL, 0) == 0) &&
				(simple_strtoul(getvar("et1phyaddr"), NULL, 0) == 10))
				return &platforms[WL300G];
		}

		/* unknown asus stuff, probably bcm4702 */
		if (!strncmp(boardnum, "asusX", 5))
			return &platforms[ASUS_4702];
	}

	if ((buf = (nvram_get("melco_id") ?: nvram_get("buffalo_id")))) {
		/* Buffalo hardware, check id for specific hardware matches */
		if (!strcmp(buf, "29bb0332"))
			return &platforms[WBR2_G54];
		if (!strcmp(buf, "29129"))
			return &platforms[WLA2_G54L];
		if (!strcmp(buf, "30189"))
			return &platforms[WHR_HP_G54];
		if (!strcmp(buf, "30182"))
			return &platforms[WHR_G54S];
		if (!strcmp(buf, "290441dd"))
			return &platforms[WHR2_A54G54];
	}

	if (buf || !strcmp(boardnum, "00")) {/* probably buffalo */
		if (!strncmp(boardtype, "bcm94710ap", 10))
			return &platforms[BUFFALO_UNKNOWN_4710];
		else
			return &platforms[BUFFALO_UNKNOWN];
	}


	if (!strcmp(getvar("CFEver"), "MotoWRv203") ||
		!strcmp(getvar("MOTO_BOARD_TYPE"), "WR_FEM1")) {

		return &platforms[WR850GV2V3];
	}

	/* not found */
	return NULL;
}

static void register_buttons(struct button_t *b)
{
	for (; b->name; b++)
		platform.button_mask |= b->gpio;

	platform.button_mask &= ~gpiomask;

	gpio_outen(platform.button_mask, 0);
	gpio_control(platform.button_mask, 0);
	platform.button_polarity = gpio_in() & platform.button_mask;
	gpio_intpolarity(platform.button_mask, platform.button_polarity);
	gpio_intmask(platform.button_mask, platform.button_mask);

	gpio_set_irqenable(1, button_handler);
}

static void unregister_buttons(struct button_t *b)
{
	gpio_intmask(platform.button_mask, 0);

	gpio_set_irqenable(0, button_handler);
}

static void hotplug_button(struct event_t *event)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
	call_usermodehelper (event->argv[0], event->argv, event->envp, 1);
#else
	call_usermodehelper (event->argv[0], event->argv, event->envp);
#endif
	kfree(event);
}

static irqreturn_t button_handler(int irq, void *dev_id, struct pt_regs *regs)
{
	struct button_t *b;
	u32 in, changed;

	in = gpio_in() & platform.button_mask;
	gpio_intpolarity(platform.button_mask, in);
	changed = platform.button_polarity ^ in;
	platform.button_polarity = in;

	for (b = platform.buttons; b->name; b++) { 
		struct event_t *event;

		if (!(b->gpio & changed)) continue;

		b->pressed ^= 1;

		if ((event = (struct event_t *)kmalloc (sizeof(struct event_t), GFP_ATOMIC))) {
			int i;
			char *scratch = event->buf;

			i = 0;
			event->argv[i++] = hotplug_path;
			event->argv[i++] = "button";
			event->argv[i] = 0;

			i = 0;
			event->envp[i++] = "HOME=/";
			event->envp[i++] = "PATH=/sbin:/bin:/usr/sbin:/usr/bin";
			event->envp[i++] = scratch;
			scratch += sprintf (scratch, "ACTION=%s", b->pressed?"pressed":"released") + 1;
			event->envp[i++] = scratch;
			scratch += sprintf (scratch, "BUTTON=%s", b->name) + 1;
			event->envp[i++] = scratch;
			scratch += sprintf (scratch, "SEEN=%ld", (jiffies - b->seen)/HZ) + 1;
			event->envp[i] = 0;

			INIT_WORK(&event->wq, (void *)(void *)hotplug_button, (void *)event);
			schedule_work(&event->wq);
		}

		b->seen = jiffies;
	}
	return IRQ_HANDLED;
}

static void register_leds(struct led_t *l)
{
	struct proc_dir_entry *p;
	u32 mask = 0;
	u32 val = 0;

	leds = proc_mkdir("led", diag);
	if (!leds) 
		return;

	for(; l->name; l++) {
		if (l->gpio & gpiomask)
			continue;
	
		if (l->gpio & GPIO_TYPE_EXTIF) {
			l->state = 0;
			set_led_extif(l);
		} else {
			mask |= l->gpio;
			val |= (l->polarity == NORMAL)?0:l->gpio;
		}

		if ((p = create_proc_entry(l->name, S_IRUSR, leds))) {
			l->proc.type = PROC_LED;
			l->proc.ptr = l;
			p->data = (void *) &l->proc;
			p->proc_fops = &diag_proc_fops;
		}
	}

	gpio_outen(mask, mask);
	gpio_control(mask, 0);
	gpio_out(mask, val);
}

static void unregister_leds(struct led_t *l)
{
	for(; l->name; l++)
		remove_proc_entry(l->name, leds);

	remove_proc_entry("led", diag);
}

static void set_led_extif(struct led_t *led)
{
	gpio_set_extif(led->gpio, led->state);
}

static void led_flash(unsigned long dummy) {
	struct led_t *l;
	u32 mask = 0;
	u8 extif_blink = 0;

	for (l = platform.leds; l->name; l++) {
		if (l->flash) {
			if (l->gpio & GPIO_TYPE_EXTIF) {
				extif_blink = 1;
				l->state = !l->state;
				set_led_extif(l);
			} else {
				mask |= l->gpio;
			}
		}
	}

	mask &= ~gpiomask;
	if (mask) {
		u32 val = ~gpio_in();

		gpio_outen(mask, mask);
		gpio_control(mask, 0);
		gpio_out(mask, val);
	}
	if (mask || extif_blink) {
		mod_timer(&led_timer, jiffies + FLASH_TIME);
	}
}

static ssize_t diag_proc_read(struct file *file, char *buf, size_t count, loff_t *ppos)
{
#ifdef LINUX_2_4
	struct inode *inode = file->f_dentry->d_inode;
	struct proc_dir_entry *dent = inode->u.generic_ip;
#else
	struct proc_dir_entry *dent = PDE(file->f_dentry->d_inode);
#endif
	char *page;
	int len = 0;
	
	if ((page = kmalloc(1024, GFP_KERNEL)) == NULL)
		return -ENOBUFS;
	
	if (dent->data != NULL) {
		struct prochandler_t *handler = (struct prochandler_t *) dent->data;
		switch (handler->type) {
			case PROC_LED: {
				struct led_t * led = (struct led_t *) handler->ptr;
				if (led->flash) {
					len = sprintf(page, "f\n");
				} else {
					if (led->gpio & GPIO_TYPE_EXTIF) {
						len = sprintf(page, "%d\n", led->state);
					} else {
						u32 in = (gpio_in() & led->gpio ? 1 : 0);
						u8 p = (led->polarity == NORMAL ? 0 : 1);
						len = sprintf(page, "%d\n", ((in ^ p) ? 1 : 0));
					}
				}
				break;
			}
			case PROC_MODEL:
				len = sprintf(page, "%s\n", platform.name);
				break;
			case PROC_GPIOMASK:
				len = sprintf(page, "0x%04x\n", gpiomask);
				break;
		}
	}
	len += 1;

	if (*ppos < len) {
		len = min_t(int, len - *ppos, count);
		if (copy_to_user(buf, (page + *ppos), len)) {
			kfree(page);
			return -EFAULT;
		}
		*ppos += len;
	} else {
		len = 0;
	}

	return len;
}


static ssize_t diag_proc_write(struct file *file, const char *buf, size_t count, loff_t *ppos)
{
#ifdef LINUX_2_4
	struct inode *inode = file->f_dentry->d_inode;
	struct proc_dir_entry *dent = inode->u.generic_ip;
#else
	struct proc_dir_entry *dent = PDE(file->f_dentry->d_inode);
#endif
	char *page;
	int ret = -EINVAL;

	if ((page = kmalloc(count + 1, GFP_KERNEL)) == NULL)
		return -ENOBUFS;

	if (copy_from_user(page, buf, count)) {
		kfree(page);
		return -EINVAL;
	}
	page[count] = 0;
	
	if (dent->data != NULL) {
		struct prochandler_t *handler = (struct prochandler_t *) dent->data;
		switch (handler->type) {
			case PROC_LED: {
				struct led_t *led = (struct led_t *) handler->ptr;
				int p = (led->polarity == NORMAL ? 0 : 1);
				
				if (page[0] == 'f') {
					led->flash = 1;
					led_flash(0);
				} else {
					led->flash = 0;
					if (led->gpio & GPIO_TYPE_EXTIF) {
						led->state = p ^ ((page[0] == '1') ? 1 : 0);
						set_led_extif(led);
					} else {
						gpio_outen(led->gpio, led->gpio);
						gpio_control(led->gpio, 0);
						gpio_out(led->gpio, ((p ^ (page[0] == '1')) ? led->gpio : 0));
					}
				}
				break;
			}
			case PROC_GPIOMASK:
				gpiomask = simple_strtoul(page, NULL, 0);

				if (platform.buttons) {
					unregister_buttons(platform.buttons);
					register_buttons(platform.buttons);
				}

				if (platform.leds) {
					unregister_leds(platform.leds);
					register_leds(platform.leds);
				}
				break;
		}
		ret = count;
	}

	kfree(page);
	return ret;
}

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

	if (!(diag = proc_mkdir("diag", NULL))) {
		printk(MODULE_NAME ": proc_mkdir on /proc/diag failed\n");
		return -EINVAL;
	}

	if ((p = create_proc_entry("model", S_IRUSR, diag))) {
		p->data = (void *) &proc_model;
		p->proc_fops = &diag_proc_fops;
	}

	if ((p = create_proc_entry("gpiomask", S_IRUSR | S_IWUSR, diag))) {
		p->data = (void *) &proc_gpiomask;
		p->proc_fops = &diag_proc_fops;
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
