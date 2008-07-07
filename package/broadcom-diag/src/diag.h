/*
 * diag.h - GPIO interface driver for Broadcom boards
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
 * $Id:$
 */

#include <linux/irq.h>
#define MODULE_NAME "diag"

#define MAX_GPIO 16
#define FLASH_TIME HZ/6

enum polarity_t {
	REVERSE = 0,
	NORMAL = 1,
	INPUT = 2,
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
	u32 gpio;
	unsigned long seen;
	u8 pressed;
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
	u32 button_mask;
	u32 button_polarity;
	void (*platform_init)(void);

	struct led_t leds[MAX_GPIO];
};

struct event_t {
	struct work_struct wq;
	unsigned long seen;
	char *name, *action;
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,0)
	struct sk_buff *skb;
#else
	char *scratch;
	char *argv[4];
	char *envp[7];
	u8 enr, anr;
#endif
};

extern char *nvram_get(char *str);

static struct platform_t platform;

/* buttons */

static void register_buttons(struct button_t *b);
static void unregister_buttons(struct button_t *b);

#ifndef LINUX_2_4
static void hotplug_button(struct work_struct *work);
static irqreturn_t button_handler(int irq, void *dev_id);
#else
static void hotplug_button(struct event_t *event);
static irqreturn_t button_handler(int irq, void *dev_id, struct pt_regs *regs);
#endif

/* leds */

static void register_leds(struct led_t *l);
static void unregister_leds(struct led_t *l);

static void set_led_extif(struct led_t *led);
static void led_flash(unsigned long dummy);

/* 2.4 compatibility */
#ifndef TIMER_INITIALIZER
#define TIMER_INITIALIZER(_function, _expires, _data) \
	{ \
		/* _expires and _data currently unused */ \
		function: _function \
	}
#endif

static struct timer_list led_timer = TIMER_INITIALIZER(&led_flash, 0, 0);

/* proc */

static struct proc_dir_entry *diag, *leds;

static ssize_t diag_proc_read(struct file *file, char *buf, size_t count, loff_t *ppos);
static ssize_t diag_proc_write(struct file *file, const char *buf, size_t count, loff_t *ppos);

static struct file_operations diag_proc_fops = {
	read: diag_proc_read,
	write: diag_proc_write
};

static struct prochandler_t proc_model = { .type = PROC_MODEL };
static struct prochandler_t proc_gpiomask = { .type = PROC_GPIOMASK };

