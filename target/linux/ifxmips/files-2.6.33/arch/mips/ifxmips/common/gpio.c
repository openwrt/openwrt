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
#include <linux/uaccess.h>
#include <linux/semaphore.h>
#include <linux/gpio.h>

#include <net/sock.h>

#include <ifxmips.h>

#define MAX_PORTS			2
#define PINS_PER_PORT		16

#define IFXMIPS_GPIO_SANITY		{if (port > MAX_PORTS || pin > PINS_PER_PORT) return -EINVAL; }

#define GPIO_TO_PORT(x) ((x > 15) ? (1) : (0))
#define GPIO_TO_GPIO(x) ((x > 15) ? (x - 16) : (x))

int
ifxmips_port_reserve_pin(unsigned int port, unsigned int pin)
{
	IFXMIPS_GPIO_SANITY;
	printk(KERN_INFO "%s : call to obseleted function\n", __func__);
	return 0;
}
EXPORT_SYMBOL(ifxmips_port_reserve_pin);

int
ifxmips_port_free_pin(unsigned int port, unsigned int pin)
{
	IFXMIPS_GPIO_SANITY;
	printk(KERN_INFO "%s : call to obseleted function\n", __func__);
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
ifxmips_port_clear_pudsel(unsigned int port, unsigned int pin)
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

static void
ifxmips_gpio_set(struct gpio_chip *chip, unsigned int offset, int value)
{
	int port = GPIO_TO_PORT(offset);
	int gpio = GPIO_TO_GPIO(offset);
	if(value)
		ifxmips_port_set_output(port, gpio);
	else
		ifxmips_port_clear_output(port, gpio);
}

static int
ifxmips_gpio_get(struct gpio_chip *chip, unsigned int offset)
{
	int port = GPIO_TO_PORT(offset);
	int gpio = GPIO_TO_GPIO(offset);
	return ifxmips_port_get_input(port, gpio);
}

static int
ifxmips_gpio_direction_input(struct gpio_chip *chip, unsigned int offset)
{
	int port = GPIO_TO_PORT(offset);
	int gpio = GPIO_TO_GPIO(offset);
	ifxmips_port_set_open_drain(port, gpio);
	ifxmips_port_clear_altsel0(port, gpio);
	ifxmips_port_clear_altsel1(port, gpio);
	ifxmips_port_set_dir_in(port, gpio);
	return 0;
}

static int
ifxmips_gpio_direction_output(struct gpio_chip *chip, unsigned int offset, int value)
{
	int port = GPIO_TO_PORT(offset);
	int gpio = GPIO_TO_GPIO(offset);
	ifxmips_port_clear_open_drain(port, gpio);
	ifxmips_port_clear_altsel0(port, gpio);
	ifxmips_port_clear_altsel1(port, gpio);
	ifxmips_port_set_dir_out(port, gpio);
	ifxmips_gpio_set(chip, offset, value);
	return 0;
}

int
gpio_to_irq(unsigned int gpio)
{
	return -EINVAL;
}
EXPORT_SYMBOL(gpio_to_irq);

struct gpio_chip
ifxmips_gpio_chip =
{
	.label = "ifxmips-gpio",
	.direction_input = ifxmips_gpio_direction_input,
	.direction_output = ifxmips_gpio_direction_output,
	.get = ifxmips_gpio_get,
	.set = ifxmips_gpio_set,
	.base = 0,
	.ngpio = 32,
};

static int
ifxmips_gpio_probe(struct platform_device *dev)
{
	gpiochip_add(&ifxmips_gpio_chip);
	return 0;
}

static int
ifxmips_gpio_remove(struct platform_device *pdev)
{
	gpiochip_remove(&ifxmips_gpio_chip);
	return 0;
}

static struct platform_driver
ifxmips_gpio_driver = {
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
