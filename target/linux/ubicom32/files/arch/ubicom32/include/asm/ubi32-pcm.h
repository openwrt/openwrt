/*
 * arch/ubicom32/include/asm/ubi32-pcm.h
 *   Ubicom32 architecture PCM driver platform data definitions.
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
#ifndef _ASM_UBICOM32_UBI32_PCM_H
#define _ASM_UBICOM32_UBI32_PCM_H

/*
 * This function is called when the sample rate has changed
 */
typedef int (*ubi32_pcm_set_rate_fn_t)(void *appdata, int rate);

struct ubi32pcm_platform_data {
	/*
	 * Name of the audio node/inst
	 */
	const char		*node_name;
	const char		*inst_name;
	int			inst_num;

	/*
	 * Application specific data provided when calling functions
	 */
	void			*appdata;

	/*
	 * Functions called when various things happen
	 */
	ubi32_pcm_set_rate_fn_t	set_rate;

	/*
	 * Pointer to optional upper layer data (i.e. DAC config, etc)
	 */
	void			*priv_data;
};
#endif /* _ASM_UBICOM32_UBI32_PCM_H */
