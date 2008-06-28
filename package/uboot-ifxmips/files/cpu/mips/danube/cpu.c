/*
 * (C) Copyright 2003
 * Wolfgang Denk, DENX Software Engineering, <wd@denx.de>
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

#include <common.h>
#include <command.h>
#if defined(CONFIG_INCA_IP)
#	include <asm/inca-ip.h>
#elif defined(CONFIG_IFX_MIPS)
#	include <asm/danube.h>
#	include "ifx_cpu.c"
#endif
#include <asm/mipsregs.h>

int do_reset(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
#if defined(CONFIG_INCA_IP)
	*INCA_IP_WDT_RST_REQ = 0x3f;
#elif defined(CONFIG_PURPLE) || defined(CONFIG_TB0229)
	void (*f)(void) = (void *) 0xbfc00000;

	f();
#elif defined(CONFIG_IFX_MIPS)
	IFX_CPU_RESET;
#endif
	fprintf(stderr, "*** reset failed ***\n");
	return 0;
}

void flush_cache (ulong start_addr, ulong size)
{

}

void write_one_tlb( int index, u32 pagemask, u32 hi, u32 low0, u32 low1 ){
	write_32bit_cp0_register(CP0_ENTRYLO0, low0);
	write_32bit_cp0_register(CP0_PAGEMASK, pagemask);
	write_32bit_cp0_register(CP0_ENTRYLO1, low1);
	write_32bit_cp0_register(CP0_ENTRYHI, hi);
	write_32bit_cp0_register(CP0_INDEX, index);
	tlb_write_indexed();
}
