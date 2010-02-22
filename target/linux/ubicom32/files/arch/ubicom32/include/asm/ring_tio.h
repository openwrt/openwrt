/*
 * arch/ubicom32/include/asm/ring_tio.h
 *   Ubicom32 architecture Ring TIO definitions.
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
#ifndef _ASM_UBICOM32_RING_TIO_H
#define _ASM_UBICOM32_RING_TIO_H

#include <asm/devtree.h>

#define RING_TIO_NODE_VERSION	2

/*
 * Devtree node for ring
 */
struct ring_tio_node {
	struct devtree_node	dn;

	u32_t			version;
	void			*regs;
};

extern void ring_tio_init(const char *node_name);

#endif /* _ASM_UBICOM32_RING_TIO_H */
