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

#ifndef _IFXMIPS_CGU_H__
#define _IFXMIPS_CGU_H__

#define BASIC_INPUT_CLOCK_FREQUENCY_1   35328000
#define BASIC_INPUT_CLOCK_FREQUENCY_2   36000000

#define BASIS_INPUT_CRYSTAL_USB         12000000

#define GET_BITS(x, msb, lsb)           (((x) & ((1 << ((msb) + 1)) - 1)) >> (lsb))

#define CGU_PLL0_PHASE_DIVIDER_ENABLE   (ifxmips_r32(IFXMIPS_CGU_PLL0_CFG) & (1 << 31))
#define CGU_PLL0_BYPASS                 (ifxmips_r32(IFXMIPS_CGU_PLL0_CFG) & (1 << 30))
#define CGU_PLL0_CFG_DSMSEL             (ifxmips_r32(IFXMIPS_CGU_PLL0_CFG) & (1 << 28))
#define CGU_PLL0_CFG_FRAC_EN            (ifxmips_r32(IFXMIPS_CGU_PLL0_CFG) & (1 << 27))
#define CGU_PLL1_SRC                    (ifxmips_r32(IFXMIPS_CGU_PLL1_CFG) & (1 << 31))
#define CGU_PLL1_BYPASS                 (ifxmips_r32(IFXMIPS_CGU_PLL1_CFG) & (1 << 30))
#define CGU_PLL1_CFG_DSMSEL             (ifxmips_r32(IFXMIPS_CGU_PLL1_CFG) & (1 << 28))
#define CGU_PLL1_CFG_FRAC_EN            (ifxmips_r32(IFXMIPS_CGU_PLL1_CFG) & (1 << 27))
#define CGU_PLL2_PHASE_DIVIDER_ENABLE   (ifxmips_r32(IFXMIPS_CGU_PLL2_CFG) & (1 << 20))
#define CGU_PLL2_BYPASS                 (ifxmips_r32(IFXMIPS_CGU_PLL2_CFG) & (1 << 19))
#define CGU_SYS_FPI_SEL                 (1 << 6)
#define CGU_SYS_DDR_SEL                 0x3
#define CGU_PLL0_SRC                    (1 << 29)

#define CGU_PLL0_CFG_PLLK               GET_BITS(*IFXMIPS_CGU_PLL0_CFG, 26, 17)
#define CGU_PLL0_CFG_PLLN               GET_BITS(*IFXMIPS_CGU_PLL0_CFG, 12, 6)
#define CGU_PLL0_CFG_PLLM               GET_BITS(*IFXMIPS_CGU_PLL0_CFG, 5, 2)
#define CGU_PLL1_CFG_PLLK               GET_BITS(*IFXMIPS_CGU_PLL1_CFG, 26, 17)
#define CGU_PLL1_CFG_PLLN               GET_BITS(*IFXMIPS_CGU_PLL1_CFG, 12, 6)
#define CGU_PLL1_CFG_PLLM               GET_BITS(*IFXMIPS_CGU_PLL1_CFG, 5, 2)
#define CGU_PLL2_SRC                    GET_BITS(*IFXMIPS_CGU_PLL2_CFG, 18, 17)
#define CGU_PLL2_CFG_INPUT_DIV          GET_BITS(*IFXMIPS_CGU_PLL2_CFG, 16, 13)
#define CGU_PLL2_CFG_PLLN               GET_BITS(*IFXMIPS_CGU_PLL2_CFG, 12, 6)
#define CGU_PLL2_CFG_PLLM               GET_BITS(*IFXMIPS_CGU_PLL2_CFG, 5, 2)
#define CGU_IF_CLK_PCI_CLK              GET_BITS(*IFXMIPS_CGU_IF_CLK, 23, 20)


unsigned int cgu_get_mips_clock(int cpu);
unsigned int cgu_get_io_region_clock(void);
unsigned int cgu_get_fpi_bus_clock(int fpi);
void cgu_setup_pci_clk(int internal_clock);
unsigned int ifxmips_get_ddr_hz(void);
unsigned int ifxmips_get_fpi_hz(void);
unsigned int ifxmips_get_cpu_hz(void);

#endif
