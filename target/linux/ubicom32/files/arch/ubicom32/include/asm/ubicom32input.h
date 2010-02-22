/*
 * arch/ubicom32/include/asm/ubicom32input.h
 *   Ubicom32 Input driver, based on gpio-keys
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
 *
 * TODO: add groups for inputs which can be sampled together
 */

#ifndef _ASM_UBICOM32_UBICOM32_INPUT_H
#define _ASM_UBICOM32_UBICOM32_INPUT_H

struct ubicom32input_button {
	/*
	 * Input event code (KEY_*, SW_*, etc)
	 */
	int		code;

	/*
	 * Input event type (EV_KEY, EV_SW, etc)
	 */
	int		type;

	/*
	 * GPIO to poll
	 */
	int		gpio;

	/*
	 * 1 for active low, 0 for active high
	 */
	int		active_low;

	/*
	 * Description, used for reserving GPIOs
	 */
	const char	*desc;
};

struct ubicom32input_platform_data {
	struct ubicom32input_button	*buttons;
	int				nbuttons;

	/*
	 * Optional poll interval, in ms, defaults to 50ms
	 */
	int				poll_interval;

	/*
	 * Option Name of this driver
	 */
	const char			*name;
};

#endif /* _ASM_UBICOM32_UBICOM32_INPUT_H */
