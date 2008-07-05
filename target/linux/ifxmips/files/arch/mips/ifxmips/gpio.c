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
 *   Copyright (C) 2004 btxu Generate from INCA-IP project
 *   Copyright (C) 2005 Jin-Sze.Sow Comments edited
 *   Copyright (C) 2006 Huang Xiaogang Modification & verification on Danube chip
 *   Copyright (C) 2007 John Crispin <blogic@openwrt.org> 
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/proc_fs.h>
#include <linux/init.h>
#include <linux/ioctl.h>
#include <linux/timer.h>
#include <linux/module.h>
#include <linux/timer.h>
#include <linux/interrupt.h>
#include <linux/kobject.h>
#include <linux/workqueue.h>
#include <linux/skbuff.h>
#include <linux/netlink.h>
#include <linux/platform_device.h>
#include <net/sock.h>
#include <asm/uaccess.h>
#include <asm/semaphore.h>
#include <asm/uaccess.h>
#include <asm/ifxmips/ifxmips.h>

#define MAX_PORTS			2
#define PINS_PER_PORT		16

#ifdef CONFIG_IFXMIPS_GPIO_RST_BTN

unsigned int rst_port = 1;
unsigned int rst_pin = 15;
static struct timer_list rst_button_timer;

extern struct sock *uevent_sock;
extern u64 uevent_next_seqnum(void);
static unsigned long seen;
static int pressed = 0;

struct event_t {
	struct work_struct wq;
	int set;
	unsigned long jiffies;
};
#endif

#define IFXMIPS_GPIO_SANITY		{if (port > MAX_PORTS || pin > PINS_PER_PORT) return -EINVAL; }
int
ifxmips_port_reserve_pin(unsigned int port, unsigned int pin)
{
	IFXMIPS_GPIO_SANITY;
	printk("%s : call to obseleted function\n", __func__);
	return 0;
}
EXPORT_SYMBOL(ifxmips_port_reserve_pin);

int
ifxmips_port_free_pin(unsigned int port, unsigned int pin)
{
	IFXMIPS_GPIO_SANITY;
	printk("%s : call to obseleted function\n", __func__);
	return 0;
}
EXPORT_SYMBOL(ifxmips_port_free_pin);

int
ifxmips_port_set_open_drain(unsigned int port, unsigned int pin)
{
	IFXMIPS_GPIO_SANITY;
	ifxmips_w32(ifxmips_r32(IFXMIPS_GPIO_P0_OD + (port * 0xC)) | (1 << pin),
		IFXMIPS_GPIO_P0_OD + (port * 0xC));
	return 0;
}
EXPORT_SYMBOL(ifxmips_port_set_open_drain);

int
ifxmips_port_clear_open_drain(unsigned int port, unsigned int pin)
{
	IFXMIPS_GPIO_SANITY;
	ifxmips_w32(ifxmips_r32(IFXMIPS_GPIO_P0_OD + (port * 0xC)) & ~(1 << pin),
		IFXMIPS_GPIO_P0_OD + (port * 0xC));
	return 0;
}
EXPORT_SYMBOL(ifxmips_port_clear_open_drain);

int
ifxmips_port_set_pudsel(unsigned int port, unsigned int pin)
{
    IFXMIPS_GPIO_SANITY;
	ifxmips_w32(ifxmips_r32(IFXMIPS_GPIO_P0_PUDSEL + (port * 0xC)) | (1 << pin),
		IFXMIPS_GPIO_P0_PUDSEL + (port * 0xC));
	return 0;
}
EXPORT_SYMBOL(ifxmips_port_set_pudsel);

int
ifxmips_port_clear_pudsel (unsigned int port, unsigned int pin)
{
    IFXMIPS_GPIO_SANITY;
	ifxmips_w32(ifxmips_r32(IFXMIPS_GPIO_P0_PUDSEL + (port * 0xC)) & ~(1 << pin),
		IFXMIPS_GPIO_P0_PUDSEL + (port * 0xC));
	return 0;
}
EXPORT_SYMBOL(ifxmips_port_clear_pudsel);

int
ifxmips_port_set_puden(unsigned int port, unsigned int pin)
{
    IFXMIPS_GPIO_SANITY;
	ifxmips_w32(ifxmips_r32(IFXMIPS_GPIO_P0_PUDEN + (port * 0xC)) | (1 << pin),
		IFXMIPS_GPIO_P0_PUDEN + (port * 0xC));
	return 0;
}
EXPORT_SYMBOL(ifxmips_port_set_puden);

int
ifxmips_port_clear_puden(unsigned int port, unsigned int pin)
{
    IFXMIPS_GPIO_SANITY;
	ifxmips_w32(ifxmips_r32(IFXMIPS_GPIO_P0_PUDEN + (port * 0xC)) & ~(1 << pin),
		IFXMIPS_GPIO_P0_PUDEN + (port * 0xC));
	return 0;
}
EXPORT_SYMBOL(ifxmips_port_clear_puden);

int
ifxmips_port_set_stoff(unsigned int port, unsigned int pin)
{
    IFXMIPS_GPIO_SANITY;
	ifxmips_w32(ifxmips_r32(IFXMIPS_GPIO_P0_STOFF + (port * 0xC)) | (1 << pin),
		IFXMIPS_GPIO_P0_STOFF + (port * 0xC));
	return 0;
}
EXPORT_SYMBOL(ifxmips_port_set_stoff);

int
ifxmips_port_clear_stoff(unsigned int port, unsigned int pin)
{
    IFXMIPS_GPIO_SANITY;
	ifxmips_w32(ifxmips_r32(IFXMIPS_GPIO_P0_STOFF + (port * 0xC)) & ~(1 << pin),
		IFXMIPS_GPIO_P0_STOFF + (port * 0xC));
	return 0;
}
EXPORT_SYMBOL(ifxmips_port_clear_stoff);

int
ifxmips_port_set_dir_out(unsigned int port, unsigned int pin)
{
    IFXMIPS_GPIO_SANITY;
	ifxmips_w32(ifxmips_r32(IFXMIPS_GPIO_P0_DIR + (port * 0xC)) | (1 << pin),
		IFXMIPS_GPIO_P0_DIR + (port * 0xC));
	return 0;
}
EXPORT_SYMBOL(ifxmips_port_set_dir_out);

int
ifxmips_port_set_dir_in(unsigned int port, unsigned int pin)
{
    IFXMIPS_GPIO_SANITY;
	ifxmips_w32(ifxmips_r32(IFXMIPS_GPIO_P0_DIR + (port * 0xC)) & ~(1 << pin),
		IFXMIPS_GPIO_P0_DIR + (port * 0xC));
	return 0;
}
EXPORT_SYMBOL(ifxmips_port_set_dir_in);

int
ifxmips_port_set_output(unsigned int port, unsigned int pin)
{
    IFXMIPS_GPIO_SANITY;
	ifxmips_w32(ifxmips_r32(IFXMIPS_GPIO_P0_OUT + (port * 0xC)) | (1 << pin),
		IFXMIPS_GPIO_P0_OUT + (port * 0xC));
	return 0;
}
EXPORT_SYMBOL(ifxmips_port_set_output);

int
ifxmips_port_clear_output(unsigned int port, unsigned int pin)
{
    IFXMIPS_GPIO_SANITY;
	ifxmips_w32(ifxmips_r32(IFXMIPS_GPIO_P0_OUT + (port * 0xC)) & ~(1 << pin),
		IFXMIPS_GPIO_P0_OUT + (port * 0xC));
	return 0;
}
EXPORT_SYMBOL(ifxmips_port_clear_output);

int
ifxmips_port_get_input(unsigned int port, unsigned int pin)
{
    IFXMIPS_GPIO_SANITY;
	if (ifxmips_r32(IFXMIPS_GPIO_P0_IN + (port * 0xC)) & (1 << pin))
		return 0;
	else
		return 1;
}
EXPORT_SYMBOL(ifxmips_port_get_input);

int
ifxmips_port_set_altsel0(unsigned int port, unsigned int pin)
{
    IFXMIPS_GPIO_SANITY;
	ifxmips_w32(ifxmips_r32(IFXMIPS_GPIO_P0_ALTSEL0 + (port * 0xC)) | (1 << pin),
		IFXMIPS_GPIO_P0_ALTSEL0 + (port * 0xC));
	return 0;
}
EXPORT_SYMBOL(ifxmips_port_set_altsel0);

int
ifxmips_port_clear_altsel0(unsigned int port, unsigned int pin)
{
    IFXMIPS_GPIO_SANITY;
	ifxmips_w32(ifxmips_r32(IFXMIPS_GPIO_P0_ALTSEL0 + (port * 0xC)) & ~(1 << pin),
		IFXMIPS_GPIO_P0_ALTSEL0 + (port * 0xC));
	return 0;
}
EXPORT_SYMBOL(ifxmips_port_clear_altsel0);

int
ifxmips_port_set_altsel1(unsigned int port, unsigned int pin)
{
    IFXMIPS_GPIO_SANITY;
	ifxmips_w32(ifxmips_r32(IFXMIPS_GPIO_P0_ALTSEL1 + (port * 0xC)) | (1 << pin),
		IFXMIPS_GPIO_P0_ALTSEL1 + (port * 0xC));
	return 0;
}
EXPORT_SYMBOL(ifxmips_port_set_altsel1);

int
ifxmips_port_clear_altsel1(unsigned int port, unsigned int pin)
{
    IFXMIPS_GPIO_SANITY;
	ifxmips_w32(ifxmips_r32(IFXMIPS_GPIO_P0_ALTSEL1 + (port * 0xC)) & ~(1 << pin),
		IFXMIPS_GPIO_P0_ALTSEL1 + (port * 0xC));
	return 0;
}
EXPORT_SYMBOL(ifxmips_port_clear_altsel1);

#ifdef CONFIG_IFXMIPS_GPIO_RST_BTN
static inline void
add_msg(struct sk_buff *skb, char *msg)
{
	char *scratch;
	scratch = skb_put(skb, strlen(msg) + 1);
	sprintf(scratch, msg);
}

static void
hotplug_button(struct work_struct *wq)
{
	struct sk_buff *skb;
	struct event_t *event;
	size_t len;
	char *scratch, *s;
	char buf[128];

	event = container_of(wq, struct event_t, wq);
	if(!uevent_sock)
		goto done;

	s = event->set ? "pressed" : "released";
	len = strlen(s) + 2;
	skb = alloc_skb(len + 2048, GFP_KERNEL);
	if(!skb)
		goto done;

	scratch = skb_put(skb, len);
	sprintf(scratch, "%s@",s);
	add_msg(skb, "HOME=/");
	add_msg(skb, "PATH=/sbin:/bin:/usr/sbin:/usr/bin");
	add_msg(skb, "SUBSYSTEM=button");
	add_msg(skb, "BUTTON=reset");
	add_msg(skb, (event->set ? "ACTION=pressed" : "ACTION=released"));
	sprintf(buf, "SEEN=%ld", (event->jiffies - seen)/HZ);
	add_msg(skb, buf);
	snprintf(buf, 128, "SEQNUM=%llu", uevent_next_seqnum());
	add_msg(skb, buf);

	NETLINK_CB(skb).dst_group = 1;
	netlink_broadcast(uevent_sock, skb, 0, 1, GFP_KERNEL);
done:
	kfree(event);
}

static void
reset_button_poll(unsigned long unused)
{
	struct event_t *event;

	rst_button_timer.expires = jiffies + (HZ / 4);
	add_timer(&rst_button_timer);

	if (pressed != ifxmips_port_get_input(rst_port, rst_pin))
	{
		if(pressed)
			pressed = 0;
		else
			pressed = 1;
		event = (struct event_t *) kzalloc(sizeof(struct event_t), GFP_ATOMIC);
		if (!event)
		{
			printk("Could not alloc hotplug event\n");
			return;
		}
		event->set = pressed;
		event->jiffies = jiffies;
		INIT_WORK(&event->wq, (void *)(void *)hotplug_button);
		schedule_work(&event->wq);
		seen = jiffies;
	}
}
#endif

static int
ifxmips_gpio_probe(struct platform_device *dev)
{
	int retval = 0;

#ifdef CONFIG_IFXMIPS_GPIO_RST_BTN
	rst_port = dev->resource[0].start;
	rst_pin = dev->resource[0].end;
	ifxmips_port_set_open_drain(rst_port, rst_pin);
	ifxmips_port_clear_altsel0(rst_port, rst_pin);
	ifxmips_port_clear_altsel1(rst_port, rst_pin);
	ifxmips_port_set_dir_in(rst_port, rst_pin);
	seen = jiffies;
	init_timer(&rst_button_timer);
	rst_button_timer.function = reset_button_poll;
	rst_button_timer.expires = jiffies + HZ;
	add_timer(&rst_button_timer);
#endif
	return retval;
}

static int
ifxmips_gpio_remove(struct platform_device *pdev)
{
#ifdef CONFIG_IFXMIPS_GPIO_RST_BTN
	del_timer_sync(&rst_button_timer);
#endif
	return 0;
}

static struct
platform_driver ifxmips_gpio_driver = {
	.probe = ifxmips_gpio_probe,
	.remove = ifxmips_gpio_remove,
	.driver = {
		.name = "ifxmips_gpio",
		.owner = THIS_MODULE,
	},
};

int __init
ifxmips_gpio_init(void)
{
	int ret = platform_driver_register(&ifxmips_gpio_driver);
	if (ret)
		printk(KERN_INFO "ifxmips_gpio : Error registering platfom driver!");
	return ret;
}

void __exit
ifxmips_gpio_exit(void)
{
	platform_driver_unregister(&ifxmips_gpio_driver);
}

module_init(ifxmips_gpio_init);
module_exit(ifxmips_gpio_exit);
