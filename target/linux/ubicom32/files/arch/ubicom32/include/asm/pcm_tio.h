/*
 * arch/ubicom32/include/asm/pcm_tio.h
 *   Ubicom32 architecture PCM TIO definitions.
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
#ifndef _ASM_UBICOM32_PCM_TIO_H
#define _ASM_UBICOM32_PCM_TIO_H

#include <asm/devtree.h>

#define PCM_TIO_REGS_VERSION	2
struct pcm_tio_regs {
	/*
	 * set this value to 1 to reload the parameters and restart the HRT
	 */
	u32_t		reload;

	/*
	 * Pointers to the input and output buffers
	 */
	void		*input_buf;
	void		*output_buf;

	/*
	 * Buffer size (see pcm_hrt.S for constraints)
	 */
	u32_t		buffer_size;

	/*
	 * Current cycle.  This variable increases every time half the buffer
	 * is consumed.
	 */
	u32_t		cycle;

	/*
	 * Fields below this line are not accessed by the HRT.  They are purely
	 * informational for the user of this TIO.
	 */

	/*
	 * Version of this structure
	 */
	u32_t		version;

	/*
	 * Number of channels supported
	 */
	u32_t		channels;

	/*
	 * Maximum buffer size
	 */
	u32_t		max_buffer_size;
};

/*
 * Our device node
 */
#define PCM_TIO_NODE_VERSION	1
struct pcm_tio_node {
	struct devtree_node	dn;
	u32_t			version;
	struct pcm_tio_regs	*regs;
};

#endif

