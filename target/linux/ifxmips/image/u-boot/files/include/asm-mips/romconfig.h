/*
 * (C) Copyright 2003
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * This file contains the configuration parameters for the DANUBE board.
 */

#ifndef __CONFIG_H
#define __CONFIG_H


#define EXCEPTION_BASE	0x200

/*****************************************************************************
 * DANUBE
 *****************************************************************************/
/* lock cache for C program stack */
/* points to ROM */
/* stack size is 16K */
#define LOCK_DCACHE_ADDR       	0x9FC00000
#define LOCK_DCACHE_SIZE       	0x1000
#define CFG_EBU_BOOTWORD	      0x688c688c
                                                                                                                                                             
#define CFG_HZ       (danube_get_cpuclk() / 2)

                                                                                                                                                             
/*
 * Memory layout
 */
//#define CFG_SDRAM_BASE       0x80080000
#define CFG_CACHE_LOCK_SIZE  LOCK_DCACHE_SIZE
#define CFG_INIT_SP_OFFSET   CFG_CACHE_LOCK_SIZE

/*
 * Cache settings
 */
#define CFG_CACHE_SIZE   16384
#define CFG_CACHE_LINES  32
#define CFG_CACHE_WAYS   4
#define CFG_CACHE_SETS   128

#define CFG_ICACHE_SIZE   CFG_CACHE_SIZE
#define CFG_DCACHE_SIZE   CFG_CACHE_SIZE
#define CFG_CACHELINE_SIZE  CFG_CACHE_LINES

#endif	/* __CONFIG_H */
