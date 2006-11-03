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
#define sbh bcm947xx_sbh
#define sbh_lock bcm947xx_sbh_lock
#endif

#include <typedefs.h>
#include <osl.h>
#include <bcmdevs.h>
#include <sbutils.h>
#include <sbconfig.h>
#include <sbchipc.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
#include <sbmips.h>
#else
#include <hndcpu.h>
#endif

#define MODULE_NAME "diag"

#define MAX_GPIO 8
#define FLASH_TIME HZ/6

#define EXTIF_ADDR 0x1f000000
#define EXTIF_UART (EXTIF_ADDR + 0x00800000)

/* For LEDs */
#define GPIO_TYPE_NORMAL	(0x0 << 24)
#define GPIO_TYPE_EXTIF 	(0x1 << 24)
#define GPIO_TYPE_MASK  	(0xf << 24)

static unsigned int gpiomask = 0;
module_param(gpiomask, int, 0644);

enum polarity_t {
	REVERSE = 0,
	NORMAL = 1,
};

enum {
	PROC_BUTTON,
	PROC_LED,
	PROC_MODEL,
	PROC_GPIOMASK
};

struct prochandler_t {
	int type;
	void *ptr;
};

struct button_t {
	struct prochandler_t proc;
	char *name;
	u16 gpio;
	u8 polarity;
	u8 pressed;
	unsigned long seen;
};

struct led_t {
	struct prochandler_t proc;
	char *name;
	u32 gpio;
	u8 polarity;
	u8 flash;
	u8 state;
};

struct platform_t {
	char *name;
	struct button_t buttons[MAX_GPIO];
	struct led_t leds[MAX_GPIO];
};

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
	WR850GV2,

	/* Belkin */
	BELKIN_UNKNOWN,
};

static struct platform_t platforms[] = {
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
		.name		= "Linksys WRT54G*",
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
			{ .name = "ses",	.gpio = 1 << 0 },
		},
		.leds		= {
			{ .name = "diag",	.gpio = 1 << 1, .polarity = REVERSE },
			{ .name = "internal",	.gpio = 1 << 3, .polarity = REVERSE },
			{ .name = "ses",	.gpio = 1 << 6, .polarity = REVERSE },
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
			{ .name = "modem",	.gpio = 1 << 6, .polarity = NORMAL },
			{ .name = "wlan_red",	.gpio = 1 << 5, .polarity = REVERSE },
			{ .name = "wlan_green",	.gpio = 1 << 7, .polarity = REVERSE },
		},
	},
	[WR850GV2] = {
		.name		= "Motorola WR850G V2",
		.buttons	= {
			{ .name = "reset",	.gpio = 1 << 5 },
		},
		.leds		= {
			{ .name = "diag",	.gpio = 1 << 1, .polarity = REVERSE },
			{ .name = "wlan",	.gpio = 1 << 0, .polarity = NORMAL },
			{ .name = "modem_green",.gpio = 1 << 6, .polarity = REVERSE },
			{ .name = "modem_red",	.gpio = 1 << 7, .polarity = REVERSE },
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

extern void *sbh;
extern spinlock_t sbh_lock;
extern char *nvram_get(char *str);

static struct proc_dir_entry *diag, *leds;
static struct platform_t platform;
static void led_flash(unsigned long dummy);

#define getvar(str) (nvram_get(str)?:"")

static void set_led_extif(struct led_t *led)
{
	volatile u8 *addr = (volatile u8 *) KSEG1ADDR(EXTIF_UART) + (led->gpio & ~GPIO_TYPE_MASK);
	if (led->state)
		*addr = 0xFF;
	else
		*addr;
}

static struct platform_t __init *platform_detect(void)
{
	char *boardnum, *boardtype, *buf;

	boardnum = getvar("boardnum");
	boardtype = getvar("boardtype");
	if (strncmp(getvar("pmon_ver"), "CFE", 3) == 0) {
		/* CFE based - newer hardware */
		if (!strcmp(boardnum, "42")) { /* Linksys */
			if (!strcmp(boardtype, "0x0101"))
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
		if (!strncmp(boardtype, "bcm94710dev", 11)) {
			if (!strcmp(boardnum, "42"))
				return &platforms[WRT54GV1];
			if (simple_strtoul(boardnum, NULL, 9) == 2)
				return &platforms[WAP54GV1];
		}
		if (!strncmp(getvar("hardware_version"), "WL500-", 6))
			return &platforms[WL500G];
		if (!strncmp(getvar("hardware_version"), "WL300-", 6)) {
			/* Either WL-300g or WL-HDD, do more extensive checks */
			if ((simple_strtoul(getvar("et0phyaddr"), NULL, 0) == 0) &&
				(simple_strtoul(getvar("et1phyaddr"), NULL, 9) == 1))
				return &platforms[WLHDD];
			if ((simple_strtoul(getvar("et0phyaddr"), NULL, 0) == 0) &&
				(simple_strtoul(getvar("et1phyaddr"), NULL, 9) == 10))
				return &platforms[WL300G];
		}

		/* unknown asus stuff, probably bcm4702 */
		if (!strncmp(boardnum, "asusX", 5))
			return &platforms[ASUS_4702];

		if ((simple_strtoul(getvar("GemtekPmonVer"), NULL, 0) == 9) &&
			(simple_strtoul(getvar("et0phyaddr"), NULL, 0) == 30)) {
			if (!strncmp(getvar("ModelId"),"WE800G", 6))
				return &platforms[WE800G];
			else
				return &platforms[WR850GV1];
		}
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

	if (!strcmp(getvar("CFEver"), "MotoWRv203"))
		return &platforms[WR850GV2];

	/* not found */
	return NULL;
}

static ssize_t diag_proc_read(struct file *file, char *buf, size_t count, loff_t *ppos);
static ssize_t diag_proc_write(struct file *file, const char *buf, size_t count, void *data);
static struct file_operations diag_proc_fops = {
	read: diag_proc_read,
	write: diag_proc_write
};



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
						int in = (sb_gpioin(sbh) & led->gpio ? 1 : 0);
						int p = (led->polarity == NORMAL ? 0 : 1);
						len = sprintf(page, "%d\n", ((in ^ p) ? 1 : 0));
					}
				}
				break;
			}
			case PROC_MODEL:
				len = sprintf(page, "%s\n", platform.name);
				break;
			case PROC_GPIOMASK:
				len = sprintf(page, "%d\n", gpiomask);
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


static ssize_t diag_proc_write(struct file *file, const char *buf, size_t count, void *data)
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
				
				if (!(led->gpio & GPIO_TYPE_EXTIF) && (led->gpio & gpiomask))
					break;

				if (page[0] == 'f') {
					led->flash = 1;
					led_flash(0);
				} else {
					led->flash = 0;
					if (led->gpio & GPIO_TYPE_EXTIF) {
						led->state = p ^ ((page[0] == '1') ? 1 : 0);
						set_led_extif(led);
					} else {
						sb_gpioouten(sbh, led->gpio, led->gpio, GPIO_DRV_PRIORITY);
						sb_gpiocontrol(sbh, led->gpio, 0, GPIO_DRV_PRIORITY);
						sb_gpioout(sbh, led->gpio, ((p ^ (page[0] == '1')) ? led->gpio : 0), GPIO_DRV_PRIORITY);
					}
				}
				break;
			}
			case PROC_GPIOMASK:
				gpiomask = simple_strtoul(page, NULL, 16);
				break;
		}
		ret = count;
	}

	kfree(page);
	return ret;
}

struct event_t {
	struct work_struct tq;
	char buf[256];
	char *argv[3];
	char *envp[6];
};

static void hotplug_button(struct event_t *event)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
	call_usermodehelper (event->argv[0], event->argv, event->envp, 1);
#else
	call_usermodehelper (event->argv[0], event->argv, event->envp);
#endif
	kfree(event);
}

static void set_irqenable(int enabled)
{
	unsigned int coreidx;
	unsigned long flags;
	chipcregs_t *cc;

	spin_lock_irqsave(sbh_lock, flags);
	coreidx = sb_coreidx(sbh);
	if ((cc = sb_setcore(sbh, SB_CC, 0))) {
		int intmask;

		intmask = readl(&cc->intmask);
		if (enabled)
			intmask |= CI_GPIO;
		else
			intmask &= ~CI_GPIO;
		writel(intmask, &cc->intmask);
	}
	sb_setcoreidx(sbh, coreidx);
	spin_unlock_irqrestore(sbh_lock, flags);
}


static irqreturn_t button_handler(int irq, void *dev_id, struct pt_regs *regs)
{
	struct button_t *b;
	int in = sb_gpioin(sbh);
	struct event_t *event;

	set_irqenable(0);
	for (b = platform.buttons; b->name; b++) { 
		if (b->gpio & gpiomask)
			continue;

		if (b->polarity != (in & b->gpio)) {
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

				INIT_WORK(&event->tq, (void *)(void *)hotplug_button, (void *)event);
				schedule_work(&event->tq);
			}

			b->seen = jiffies;
			b->polarity ^= b->gpio;
			sb_gpiointpolarity(sbh, b->gpio, b->polarity, GPIO_DRV_PRIORITY);
		}
	}
	set_irqenable(1);
	return IRQ_HANDLED;
}

static struct timer_list led_timer = {
	function: &led_flash
};

static void led_flash(unsigned long dummy) {
	struct led_t *l;
	unsigned mask = 0;
	unsigned extif_blink = 0;

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
		unsigned val;

		val = ~sb_gpioin(sbh);
		val &= mask;

		sb_gpioouten(sbh, mask, mask, GPIO_DRV_PRIORITY);
		sb_gpiocontrol(sbh, mask, 0, GPIO_DRV_PRIORITY);
		sb_gpioout(sbh, mask, val, GPIO_DRV_PRIORITY);
	}
	if (mask || extif_blink) {
		mod_timer(&led_timer, jiffies + FLASH_TIME);
	}
}

static void __init register_buttons(struct button_t *b)
{
	int irq = sb_irq(sbh) + 2;

	request_irq(irq, button_handler, SA_SHIRQ | SA_SAMPLE_RANDOM, "gpio", button_handler);

	for (; b->name; b++) {
		if (b->gpio & gpiomask)
			continue;

		sb_gpioouten(sbh, b->gpio,0, GPIO_DRV_PRIORITY);
		sb_gpiocontrol(sbh, b->gpio,0, GPIO_DRV_PRIORITY);
		b->polarity = sb_gpioin(sbh) & b->gpio;
		sb_gpiointpolarity(sbh, b->gpio, b->polarity, GPIO_DRV_PRIORITY);
		sb_gpiointmask(sbh, b->gpio, b->gpio, GPIO_DRV_PRIORITY);
	}
	set_irqenable(1);
}

static void __exit unregister_buttons(struct button_t *b)
{
	int irq = sb_irq(sbh) + 2;

	for (; b->name; b++)
		sb_gpiointmask(sbh, b->gpio, 0, GPIO_DRV_PRIORITY);

	free_irq(irq, button_handler);
}

static void __init register_leds(struct led_t *l)
{
	struct proc_dir_entry *p;

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
			sb_gpioouten(sbh, l->gpio, l->gpio, GPIO_DRV_PRIORITY);
			sb_gpiocontrol(sbh, l->gpio, 0, GPIO_DRV_PRIORITY);
			sb_gpioout(sbh, l->gpio, (l->polarity == NORMAL)?0:l->gpio, GPIO_DRV_PRIORITY);
		}

		if ((p = create_proc_entry(l->name, S_IRUSR, leds))) {
			l->proc.type = PROC_LED;
			l->proc.ptr = l;
			p->data = (void *) &l->proc;
			p->proc_fops = &diag_proc_fops;
		}
	}
}

static void __exit unregister_leds(struct led_t *l)
{
	for(; l->name; l++)
		remove_proc_entry(l->name, leds);

	remove_proc_entry("led", diag);
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

static struct prochandler_t proc_model = { .type = PROC_MODEL };
static struct prochandler_t proc_gpiomask = { .type = PROC_GPIOMASK };

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

module_init(diag_init);
module_exit(diag_exit);

MODULE_AUTHOR("Mike Baker, Felix Fietkau / OpenWrt.org");
MODULE_LICENSE("GPL");
