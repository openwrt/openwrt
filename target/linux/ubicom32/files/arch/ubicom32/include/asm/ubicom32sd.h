/*
 * arch/ubicom32/include/asm/ubicom32sd.h
 *   Ubicom32SD public include file
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
#ifndef _ASM_UBICOM32_UBICOM32_SD_H
#define _ASM_UBICOM32_UBICOM32_SD_H

struct ubicom32sd_card {
	/*
	 * GPIOs of PWR, WP and CD lines.
	 * Polarity is 1 for active high and 0 for active low
	 */
	int				pin_pwr;
	bool				pwr_polarity;
	int				pin_wp;
	bool				wp_polarity;
	int				pin_cd;
	bool				cd_polarity;
};

struct ubicom32sd_platform_data {
	int			ncards;

	struct ubicom32sd_card	*cards;
};

#endif /* _ASM_UBICOM32_UBICOM32_SD_H */
