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
