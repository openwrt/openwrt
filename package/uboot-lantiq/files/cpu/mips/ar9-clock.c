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

#include <common.h>
#include <asm/ar9.h>

ulong ifx_get_ddr_hz(void)
{
	switch((*AR9_CGU_SYS) & 0x05) {
		case 0x01: 
		case 0x05: 
		return CLOCK_111M;
	
		case 0x00: 
		case 0x04: 
		return CLOCK_166M;
	}

	return 0;
}

ulong ifx_get_cpuclk(void)
{
	switch((*AR9_CGU_SYS) & 0x05) {
		case 0x00: 
		case 0x01: 
		return CLOCK_333M;

		case 0x04: 
		return CLOCK_166M;

		case 0x05: 
		return CLOCK_111M;
	}

	return 0;
}

ulong get_bus_freq(ulong dummy)
{
	unsigned int ddr_clock=ifx_get_ddr_hz();
	if((*AR9_CGU_SYS) & 0x40){
		return ddr_clock/2;
	} else {
		return ddr_clock;
	}
}
