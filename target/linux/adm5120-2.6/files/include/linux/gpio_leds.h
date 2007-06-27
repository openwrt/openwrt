/*
 *  $Id$
 *
 *  GPIO LEDs platform data structure
 *
 *  Copyright (C) 2007 OpenWrt.org
 *  Copyright (C) 2007 Gabor Juhos <juhosg@freemail.hu>
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

#ifndef _GPIO_LEDS_H_
#define _GPIO_LEDS_H_

struct gpio_led_platform_data {
	char *name;
	char *trigger;
	unsigned gpio;	/* GPIO line number */
	int value_off;	/* value to turn LED OFF */
	int value_on;	/* value to turn LED ON */
};

#endif /* _GPIO_LEDS_H__ */
