/*
 * MTX-1 platform devices registration
 *
 * Copyright (C) 2007, Florian Fainelli <florian@openwrt.org>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *  
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <linux/autoconf.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/platform_device.h>
#include <linux/leds.h>

#include <asm/gpio.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,23)
static struct gpio_led default_leds[] = {
        { .name = "mtx1:green", .gpio = 211, .active_low = 1, },
	{ .name = "mtx1:red", gpio = 212, .active_low = 1, },
}

static struct gpio_led_platform_data mtx1_led_data;

static struct platform_device mtx1_gpio_leds = {
        .name = "leds-gpio",
        .id = -1,
        .dev = {
                .platform_data = &mtx1_led_data,
        }
};
#endif

static int __init mtx1_register_devices(void)
{
	int res;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,23)
	mtx1_led_data.num_leds = ARRAY_SIZE(default_leds);
        mtx1_led_data.leds = default_leds;
	res = platform_device_register(&mtx1_gpio_leds);
#endif
	return res;
}

arch_initall(mtx1_register_devices);

