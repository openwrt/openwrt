/*
 * arch/ubicom32/include/asm/switch-dev.h
 *   generic Ethernet switch platform data definitions for Ubicom32 architecture.
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
#ifndef _ASM_UBICOM32_SWITCH_DEV_H
#define _ASM_UBICOM32_SWITCH_DEV_H

#define SWITCH_DEV_FLAG_HW_RESET	0x01
#define SWITCH_DEV_FLAG_SW_RESET	0x02

struct switch_core_platform_data {
	/*
	 * See flags above
	 */
	u32_t		flags;

	/*
	 * GPIO to use for nReset
	 */
	int		pin_reset;

	/*
	 * Name of this switch
	 */
	const char	*name;
};

#endif /* _ASM_UBICOM32_SWITCH_DEV_H */
