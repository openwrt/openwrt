/*
 * arch/ubicom32/include/asm/sd_tio.h
 *   SD TIO definitions
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
#ifndef _ASM_UBICOM32_SD_TIO_H
#define _ASM_UBICOM32_SD_TIO_H

#include <asm/devtree.h>

/*
 * Devtree node for SD
 */
struct sd_tio_node {
	struct devtree_node	dn;
	void			*regs;
};

#endif /* _ASM_UBICOM32_SD_TIO_H */
