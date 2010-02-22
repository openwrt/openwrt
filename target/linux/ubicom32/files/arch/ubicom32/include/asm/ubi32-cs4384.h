/*
 * arch/ubicom32/include/asm/ubi32-cs4384.h
 *   Ubicom32 architecture CS4384 driver platform data definitions.
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
 */
#ifndef _ASM_UBICOM32_UBI32_CS4384_H
#define _ASM_UBICOM32_UBI32_CS4384_H

enum ubi32_cs4384_mclk_source {
	UBI32_CS4384_MCLK_PWM_0,
	UBI32_CS4384_MCLK_PWM_1,
	UBI32_CS4384_MCLK_PWM_2,
	UBI32_CS4384_MCLK_CLKDIV_1,
	UBI32_CS4384_MCLK_OTHER,
};

struct ubi32_cs4384_mclk_entry {
	/*
	 * Rate, in Hz, of this entry
	 */
	int rate;

	/*
	 * The divider to program to get the rate
	 */
	int div;
};

struct ubi32_cs4384_platform_data {
	enum ubi32_cs4384_mclk_source	mclk_src;

	int				n_mclk;
	struct ubi32_cs4384_mclk_entry	*mclk_entries;
};
#endif /* _ASM_UBICOM32_UBI32_CS4384_H */
