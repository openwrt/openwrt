/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 *
 *   Copyright (C) 2007 John Crispin <blogic@openwrt.org> 
 */

#ifndef __ASM_MIPS_MACH_IFXMIPS_WAR_H
#define __ASM_MIPS_MACH_IFXMIPS_WAR_H

#define R4600_V1_INDEX_ICACHEOP_WAR     0
#define R4600_V1_HIT_CACHEOP_WAR        0
#define R4600_V2_HIT_CACHEOP_WAR        0
#define R5432_CP0_INTERRUPT_WAR         0
#define BCM1250_M3_WAR                  0
#define SIBYTE_1956_WAR                 0
#define MIPS4K_ICACHE_REFILL_WAR        0
#define MIPS_CACHE_SYNC_WAR             0
#define TX49XX_ICACHE_INDEX_INV_WAR     0
#define RM9000_CDEX_SMP_WAR             0
#define ICACHE_REFILLS_WORKAROUND_WAR   0
#define R10000_LLSC_WAR                 0
#define MIPS34K_MISSED_ITLB_WAR         0

#endif
