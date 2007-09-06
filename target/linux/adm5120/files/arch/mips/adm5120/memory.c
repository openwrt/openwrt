/*
 *  $Id$
 *
 *  Copyright (C) 2007 OpenWrt.org
 *  Copyright (C) 2007 Gabor Juhos <juhosg at openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the
 *  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA  02110-1301, USA.
 *
 */

#include <linux/init.h>
#include <linux/types.h>
#include <linux/kernel.h>

#include <asm/bootinfo.h>
#include <asm/addrspace.h>

#include <asm/mach-adm5120/adm5120_info.h>
#include <asm/mach-adm5120/adm5120_defs.h>
#include <asm/mach-adm5120/adm5120_switch.h>
#include <asm/mach-adm5120/adm5120_mpmc.h>

#define SWITCH_READ(r) *(u32 *)(KSEG1ADDR(ADM5120_SWITCH_BASE)+(r))
#define SWITCH_WRITE(r,v) *(u32 *)(KSEG1ADDR(ADM5120_SWITCH_BASE)+(r))=(v)

#define MPMC_READ(r) *(u32 *)(KSEG1ADDR(ADM5120_SWITCH_BASE)+(r))
#define MPMC_WRITE(r,v) *(u32 *)(KSEG1ADDR(ADM5120_SWITCH_BASE)+(r))=(v)

#if 1
#  define mem_dbg(f, a...)	printk("mem_detect: " f, ## a)
#else
#  define mem_dbg(f, a...)
#endif

#define MEM_WR_DELAY	10000 /* 0.01 usec */

unsigned long adm5120_memsize;

static int __init mem_check_pattern(u8 *addr, unsigned long offs)
{
	volatile u32 *p1 = (volatile u32 *)addr;
	volatile u32 *p2 = (volatile u32 *)(addr+offs);
	u32 t,u,v;

	/* save original value */
	t = *p1;
	u = *p2;

	if (t != u)
		return 0;

	v = 0x55555555;
	if (u == v)
		v = 0xAAAAAAAA;

	mem_dbg("write 0x%08X to 0x%08lX\n", v, (unsigned long)p1);

	*p1 = v;
	mem_dbg("delay %d ns\n", MEM_WR_DELAY);
	adm5120_ndelay(MEM_WR_DELAY);
	u = *p2;

	mem_dbg("pattern at 0x%08lX is 0x%08X\n", (unsigned long)p2, u);

	/* restore original value */
	*p1 = t;

	return (v == u);
}

static void __init adm5120_detect_memsize(void)
{
	u32	memctrl;
	u32	size, maxsize;
	u8	*p;

	memctrl = SWITCH_READ(SWITCH_REG_MEMCTRL);
	switch (memctrl & MEMCTRL_SDRS_MASK) {
	case MEMCTRL_SDRS_4M:
		maxsize = 4 << 20;
		break;
	case MEMCTRL_SDRS_8M:
		maxsize = 8 << 20;
		break;
	case MEMCTRL_SDRS_16M:
		maxsize = 16 << 20;
		break;
	default:
		maxsize = 64 << 20;
		break;
	}

	/* disable buffers for both SDRAM banks */
	mem_dbg("disable buffers for both banks\n");
	MPMC_WRITE(MPMC_REG_DC0, MPMC_READ(MPMC_REG_DC0) & ~DC_BE);
	MPMC_WRITE(MPMC_REG_DC1, MPMC_READ(MPMC_REG_DC1) & ~DC_BE);

	mem_dbg("checking for %uMB chip in 1st bank\n", maxsize >> 20);

	/* detect size of the 1st SDRAM bank */
	p = (u8 *)KSEG1ADDR(0);
	for (size = 2<<20; size <= (maxsize >> 1); size <<= 1) {
		if (mem_check_pattern(p, size)) {
			/* mirrored address */
			mem_dbg("mirrored data found at offset 0x%08X\n", size);
			break;
		}
	}

	mem_dbg("chip size in 1st bank is %uMB\n", size >> 20);
	adm5120_memsize = size;

	if (size != maxsize)
		/* 2nd bank is not supported */
		goto out;

	if ((memctrl & MEMCTRL_SDR1_ENABLE) == 0)
		/* 2nd bank is disabled */
		goto out;

	/*
	 * some bootloaders enable 2nd bank, even if the 2nd SDRAM chip
	 * are missing.
	 */
	mem_dbg("check presence of 2nd bank\n");

	p = (u8 *)KSEG1ADDR(maxsize+size-4);
	if (mem_check_pattern(p, 0)) {
		adm5120_memsize += size;
	}

	if (maxsize != size) {
		/* adjusting MECTRL register */
		memctrl &= ~(MEMCTRL_SDRS_MASK);
		switch (size>>20) {
		case 4:
			memctrl |= MEMCTRL_SDRS_4M;
			break;
		case 8:
			memctrl |= MEMCTRL_SDRS_8M;
			break;
		case 16:
			memctrl |= MEMCTRL_SDRS_16M;
			break;
		default:
			memctrl |= MEMCTRL_SDRS_64M;
			break;
		}
		SWITCH_WRITE(SWITCH_REG_MEMCTRL, memctrl);
	}

out:
	/* reenable buffer for both SDRAM banks */
	mem_dbg("enable buffers for both banks\n");
	MPMC_WRITE(MPMC_REG_DC0, MPMC_READ(MPMC_REG_DC0) | DC_BE);
	MPMC_WRITE(MPMC_REG_DC1, MPMC_READ(MPMC_REG_DC1) | DC_BE);

	mem_dbg("%dx%uMB memory found\n", (adm5120_memsize == size) ? 1 : 2 ,
		size >>20);
}

void __init adm5120_mem_init(void)
{
	adm5120_detect_memsize();
	add_memory_region(0, adm5120_memsize, BOOT_MEM_RAM);
}
