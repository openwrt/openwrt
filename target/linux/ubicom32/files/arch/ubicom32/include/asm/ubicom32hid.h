/*
 * arch/ubicom32/include/asm/ubicom32hid.h
 *   Ubicom32 architecture HID driver platform data definitions.
 *
 * (C) Copyright 2009, Ubicom, Inc.
 *
 * This file is part of the Ubicom32 Linux Kernel Port.
 *
 * The Ubicom32 Linux Kernel Port is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 *
 * The Ubicom32 Linux Kernel Port is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Ubicom32 Linux Kernel Port.  If not,
 * see <http://www.gnu.org/licenses/>.
 *
 * Ubicom32 implementation derived from (with many thanks):
 *   arch/m68knommu
 *   arch/blackfin
 *   arch/parisc
 */
#ifndef _ASM_UBICOM32_UBICOM32_HID_H
#define _ASM_UBICOM32_UBICOM32_HID_H

enum ubicom32hid_bl_types {
	/*
	 * On or off, using command SET_BL_EN, PB4
	 */
	UBICOM32HID_BL_TYPE_BINARY,

	/*
	 * Dimmable, using command SET_PWM, PB3
	 */
	UBICOM32HID_BL_TYPE_PWM,
};

/*
 * IR code mapping to event code.
 *	If there are no button mappings and no ir mappings
 *	then no input driver will be registered.
 */
struct ubicom32hid_ir {
	/*
	 * Input event code (KEY_*, SW_*, etc)
	 */
	int		code;

	/*
	 * Input event type (EV_KEY, EV_SW, etc)
	 */
	int		type;

	/*
	 * The IR code of this button.
	 */
	uint32_t	ir_code;
};

/*
 * Button mapping to event code.
 *	If there are no button mappings and no ir mappings
 *	then no input driver will be registered.
 */
struct ubicom32hid_button {
	/*
	 * Input event code (KEY_*, SW_*, etc)
	 */
	int		code;

	/*
	 * Input event type (EV_KEY, EV_SW, etc)
	 */
	int		type;

	/*
	 * Bit number of this button.
	 */
	uint8_t		bit;
};

struct ubicom32hid_platform_data {
	/*
	 * Default intensity of the backlight 0-255
	 */
	u8_t				default_intensity;

	/*
	 * GPIO number of the reset line and its polarity.
	 */
	unsigned			gpio_reset;
	int				gpio_reset_polarity;

	/*
	 * TRUE if the backlight sense is active low. (inverted)
	 * FALSE if the backlight sense is active high.
	 */
	bool				invert;

	/*
	 * Type of the backlight we are controlling
	 */
	enum ubicom32hid_bl_types	type;

	/*
	 * Optional polling rate for input, in ms, defaults to 100ms
	 */
	int				poll_interval;

	/*
	 * Optional name to register as input device
	 */
	const char			*input_name;

	/*
	 * Button mapping array
	 */
	const struct ubicom32hid_button	*buttons;
	int				nbuttons;

	/*
	 * IR mapping array
	 */
	const struct ubicom32hid_ir	*ircodes;
	int				nircodes;
};

#endif /* _ASM_UBICOM32_UBICOM32_HID_H */
