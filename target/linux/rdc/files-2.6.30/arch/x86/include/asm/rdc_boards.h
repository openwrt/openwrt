/*
 *  RDC321x boards
 *
 *  Copyright (C) 2007-2009 OpenWrt.org
 *  Copyright (C) 2007 Florian Fainelli <florian@openwrt.org>
 *  Copyright (C) 2008-2009 Daniel Gimpelevich <daniel@gimpelevich.san-francisco.ca.us>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the
 *  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA  02110-1301, USA.
 *
 */

#ifndef _RDC_BOARDS_H__
#define _RDC_BOARDS_H__

#include <linux/leds.h>
#include <linux/gpio_buttons.h>

struct rdc_platform_data {
	struct gpio_led_platform_data led_data;
	struct gpio_buttons_platform_data button_data;
};

#endif
