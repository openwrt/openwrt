/*
 *  Definitions for the GPIO buttons interface driver
 *
 *  Copyright (C) 2007,2008 Gabor Juhos <juhosg at openwrt.org>
 *
 *  This file was based on: /include/linux/gpio_keys.h
 *	The original gpio_keys.h seems not to have a license.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 */

#ifndef _GPIO_BUTTONS_H_
#define _GPIO_BUTTONS_H_

struct gpio_button {
	int	gpio;		/* GPIO line number */
	int	active_low;
	char	*desc;		/* button description */
	int	type;		/* input event type (EV_KEY, EV_SW) */
	int	code;		/* input event code (KEY_*, SW_*) */
	int	count;
	int	threshold;	/* count threshold */
};

struct gpio_buttons_platform_data {
	struct gpio_button *buttons;
	int	nbuttons;		/* number of buttons */
	int	poll_interval;		/* polling interval */
};

#endif /* _GPIO_BUTTONS_H_ */

