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
 *   Copyright (C) 2007 Xu Liang, infineon
 *   Copyright (C) 2008 John Crispin <blogic@openwrt.org>
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/unistd.h>
#include <linux/errno.h>

#include <asm/irq.h>
#include <asm/div64.h>

#include <ifxmips.h>
#include <ifxmips_cgu.h>

static unsigned int cgu_get_pll0_fdiv(void);
unsigned int ifxmips_clocks[] = {CLOCK_167M, CLOCK_133M, CLOCK_111M, CLOCK_83M };

#define DDR_HZ ifxmips_clocks[ifxmips_r32(IFXMIPS_CGU_SYS) & 0x3]

static inline unsigned int get_input_clock(int pll)
{
	switch (pll) {
	case 0:
		if (ifxmips_r32(IFXMIPS_CGU_PLL0_CFG) & CGU_PLL0_SRC)
			return BASIS_INPUT_CRYSTAL_USB;
		else if (CGU_PLL0_PHASE_DIVIDER_ENABLE)
			return BASIC_INPUT_CLOCK_FREQUENCY_1;
		else
			return BASIC_INPUT_CLOCK_FREQUENCY_2;
	case 1:
		if (CGU_PLL1_SRC)
			return BASIS_INPUT_CRYSTAL_USB;
		else if (CGU_PLL0_PHASE_DIVIDER_ENABLE)
			return BASIC_INPUT_CLOCK_FREQUENCY_1;
		else
			return BASIC_INPUT_CLOCK_FREQUENCY_2;
	case 2:
		switch (CGU_PLL2_SRC) {
		case 0:
			return cgu_get_pll0_fdiv();
		case 1:
			return CGU_PLL2_PHASE_DIVIDER_ENABLE ?
				BASIC_INPUT_CLOCK_FREQUENCY_1 :
				BASIC_INPUT_CLOCK_FREQUENCY_2;
		case 2:
			return BASIS_INPUT_CRYSTAL_USB;
		}
	default:
		return 0;
	}
}

static inline unsigned int cal_dsm(int pll, unsigned int num, unsigned int den)
{
	u64 res, clock = get_input_clock(pll);

	res = num * clock;
	do_div(res, den);
	return res;
}

static inline unsigned int mash_dsm(int pll, unsigned int M, unsigned int N,
	unsigned int K)
{
	unsigned int num = ((N + 1) << 10) + K;
	unsigned int den = (M + 1) << 10;

	return cal_dsm(pll, num, den);
}

static inline unsigned int ssff_dsm_1(int pll, unsigned int M, unsigned int N,
	unsigned int K)
{
	unsigned int num = ((N + 1) << 11) + K + 512;
	unsigned int den = (M + 1) << 11;

	return cal_dsm(pll, num, den);
}

static inline unsigned int ssff_dsm_2(int pll, unsigned int M, unsigned int N,
	unsigned int K)
{
	unsigned int num = K >= 512 ?
		((N + 1) << 12) + K - 512 : ((N + 1) << 12) + K + 3584;
	unsigned int den = (M + 1) << 12;

	return cal_dsm(pll, num, den);
}

static inline unsigned int dsm(int pll, unsigned int M, unsigned int N,
	unsigned int K, unsigned int dsmsel, unsigned int phase_div_en)
{
	if (!dsmsel)
		return mash_dsm(pll, M, N, K);
	else if (!phase_div_en)
		return mash_dsm(pll, M, N, K);
	else
		return ssff_dsm_2(pll, M, N, K);
}

static inline unsigned int cgu_get_pll0_fosc(void)
{
	if (CGU_PLL0_BYPASS)
		return get_input_clock(0);
	else
		return !CGU_PLL0_CFG_FRAC_EN
			? dsm(0, CGU_PLL0_CFG_PLLM, CGU_PLL0_CFG_PLLN, 0, CGU_PLL0_CFG_DSMSEL,
				CGU_PLL0_PHASE_DIVIDER_ENABLE)
			: dsm(0, CGU_PLL0_CFG_PLLM, CGU_PLL0_CFG_PLLN, CGU_PLL0_CFG_PLLK,
				CGU_PLL0_CFG_DSMSEL, CGU_PLL0_PHASE_DIVIDER_ENABLE);
}

static unsigned int cgu_get_pll0_fdiv(void)
{
	unsigned int div = CGU_PLL2_CFG_INPUT_DIV + 1;
	return (cgu_get_pll0_fosc() + (div >> 1)) / div;
}

unsigned int cgu_get_io_region_clock(void)
{
	unsigned int ret = cgu_get_pll0_fosc();
	switch (ifxmips_r32(IFXMIPS_CGU_PLL2_CFG) & CGU_SYS_DDR_SEL) {
	default:
	case 0:
		return (ret + 1) / 2;
	case 1:
		return (ret * 2 + 2) / 5;
	case 2:
		return (ret + 1) / 3;
	case 3:
		return (ret + 2) / 4;
	}
}

void cgu_setup_pci_clk(int external_clock)
{
	/* set clock to 33Mhz */
	ifxmips_w32(ifxmips_r32(IFXMIPS_CGU_IFCCR) & ~0xf00000,
		IFXMIPS_CGU_IFCCR);
	ifxmips_w32(ifxmips_r32(IFXMIPS_CGU_IFCCR) | 0x800000,
		IFXMIPS_CGU_IFCCR);
	if (external_clock) {
		ifxmips_w32(ifxmips_r32(IFXMIPS_CGU_IFCCR) & ~(1 << 16),
			IFXMIPS_CGU_IFCCR);
		ifxmips_w32((1 << 30), IFXMIPS_CGU_PCICR);
	} else {
		ifxmips_w32(ifxmips_r32(IFXMIPS_CGU_IFCCR) | (1 << 16),
			IFXMIPS_CGU_IFCCR);
		ifxmips_w32((1 << 31) | (1 << 30), IFXMIPS_CGU_PCICR);
	}
}

unsigned int cgu_get_fpi_bus_clock(int fpi)
{
	unsigned int ret = cgu_get_io_region_clock();
	if ((fpi == 2) && (ifxmips_r32(IFXMIPS_CGU_SYS) & CGU_SYS_FPI_SEL))
		ret >>= 1;
	return ret;
}
EXPORT_SYMBOL(cgu_get_fpi_bus_clock);

unsigned int ifxmips_get_cpu_hz(void)
{
	unsigned int ddr_clock = DDR_HZ;
	switch (ifxmips_r32(IFXMIPS_CGU_SYS) & 0xc) {
	case 0:
		return CLOCK_333M;
	case 4:
		return ddr_clock;
	}
	return ddr_clock << 1;
}
EXPORT_SYMBOL(ifxmips_get_cpu_hz);

unsigned int ifxmips_get_fpi_hz(void)
{
	unsigned int ddr_clock = DDR_HZ;
	if (ifxmips_r32(IFXMIPS_CGU_SYS) & 0x40)
		return ddr_clock >> 1;
	return ddr_clock;
}
EXPORT_SYMBOL(ifxmips_get_fpi_hz);
