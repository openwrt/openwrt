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
#include <asm/danube.h>

ulong ifx_get_ddr_hz(void)
{
	static const ulong ddr_freq[] = {166666667,133333333,111111111,83333333};
	return ddr_freq[((*DANUBE_CGU_SYS) & 0x3)];
}

ulong ifx_get_cpuclk(void)
{
#ifdef CONFIG_USE_EMULATOR
	return EMULATOR_CPU_SPEED;
#else //NOT CONFIG_USE_EMULATOR
	unsigned int ddr_clock=ifx_get_ddr_hz();
	switch((*DANUBE_CGU_SYS) & 0xc){
		case 0:
		default:
			return 323333333;
		case 4:
			return ddr_clock;
		case 8:
			return ddr_clock << 1;
	}
#endif
}

ulong get_bus_freq(ulong dummy)
{
#ifdef CONFIG_USE_EMULATOR
	unsigned int  clkCPU;
	clkCPU = ifx_get_cpuclk();
	return clkCPU >> 2;
#else //NOT CONFIG_USE_EMULATOR
	unsigned int ddr_clock=ifx_get_ddr_hz();
	if ((*DANUBE_CGU_SYS) & 0x40){
		return ddr_clock >> 1;
	}
	return ddr_clock;
#endif
}

