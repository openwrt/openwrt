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
#include <asm/uaccess.h>
#include <asm/unistd.h>
#include <asm/irq.h>
#include <asm/div64.h>
#include <linux/errno.h>
#include <asm/ifxmips/ifxmips.h>

#define FIX_FOR_36M_CRYSTAL             1
#define BASIC_INPUT_CLOCK_FREQUENCY_1   35328000
#define BASIC_INPUT_CLOCK_FREQUENCY_2   36000000

#define BASIS_INPUT_CRYSTAL_USB         12000000

#define GET_BITS(x, msb, lsb)           (((x) & ((1 << ((msb) + 1)) - 1)) >> (lsb))
#define SET_BITS(x, msb, lsb, value)    (((x) & ~(((1 << ((msb) + 1)) - 1) ^ ((1 << (lsb)) - 1))) | (((value) & ((1 << (1 + (msb) - (lsb))) - 1)) << (lsb)))

#define CGU_PLL0_PHASE_DIVIDER_ENABLE   (*IFXMIPS_CGU_PLL0_CFG & (1 << 31))
#define CGU_PLL0_BYPASS                 (*IFXMIPS_CGU_PLL0_CFG & (1 << 30))
#define CGU_PLL0_SRC                    (*IFXMIPS_CGU_PLL0_CFG & (1 << 29))
#define CGU_PLL0_CFG_DSMSEL             (*IFXMIPS_CGU_PLL0_CFG & (1 << 28))
#define CGU_PLL0_CFG_FRAC_EN            (*IFXMIPS_CGU_PLL0_CFG & (1 << 27))
#define CGU_PLL0_CFG_PLLK               GET_BITS(*IFXMIPS_CGU_PLL0_CFG, 26, 17)
#define CGU_PLL0_CFG_PLLN               GET_BITS(*IFXMIPS_CGU_PLL0_CFG, 12, 6)
#define CGU_PLL0_CFG_PLLM               GET_BITS(*IFXMIPS_CGU_PLL0_CFG, 5, 2)
#define CGU_PLL1_SRC                    (*IFXMIPS_CGU_PLL1_CFG & (1 << 31))
#define CGU_PLL1_BYPASS                 (*IFXMIPS_CGU_PLL1_CFG & (1 << 30))
#define CGU_PLL1_CFG_DSMSEL             (*IFXMIPS_CGU_PLL1_CFG & (1 << 28))
#define CGU_PLL1_CFG_FRAC_EN            (*IFXMIPS_CGU_PLL1_CFG & (1 << 27))
#define CGU_PLL1_CFG_PLLK               GET_BITS(*IFXMIPS_CGU_PLL1_CFG, 26, 17)
#define CGU_PLL1_CFG_PLLN               GET_BITS(*IFXMIPS_CGU_PLL1_CFG, 12, 6)
#define CGU_PLL1_CFG_PLLM               GET_BITS(*IFXMIPS_CGU_PLL1_CFG, 5, 2)
#define CGU_PLL2_PHASE_DIVIDER_ENABLE   (*IFXMIPS_CGU_PLL2_CFG & (1 << 20))
#define CGU_PLL2_BYPASS                 (*IFXMIPS_CGU_PLL2_CFG & (1 << 19))
#define CGU_PLL2_SRC                    GET_BITS(*IFXMIPS_CGU_PLL2_CFG, 18, 17)
#define CGU_PLL2_CFG_INPUT_DIV          GET_BITS(*IFXMIPS_CGU_PLL2_CFG, 16, 13)
#define CGU_PLL2_CFG_PLLN               GET_BITS(*IFXMIPS_CGU_PLL2_CFG, 12, 6)
#define CGU_PLL2_CFG_PLLM               GET_BITS(*IFXMIPS_CGU_PLL2_CFG, 5, 2)
#define CGU_SYS_PPESEL                  GET_BITS(*IFXMIPS_CGU_SYS, 8, 7)
#define CGU_SYS_FPI_SEL                 (*IFXMIPS_CGU_SYS & (1 << 6))
#define CGU_SYS_CPU1SEL                 GET_BITS(*IFXMIPS_CGU_SYS, 5, 4)
#define CGU_SYS_CPU0SEL                 GET_BITS(*IFXMIPS_CGU_SYS, 3, 2)
#define CGU_SYS_DDR_SEL                 GET_BITS(*IFXMIPS_CGU_SYS, 1, 0)
#define CGU_IF_CLK_PCI_CLK              GET_BITS(*IFXMIPS_CGU_IF_CLK, 23, 20)
#define CGU_IF_CLK_USBSEL               GET_BITS(*IFXMIPS_CGU_IF_CLK, 5, 4)
#define CGU_IF_CLK_MIISEL               GET_BITS(*IFXMIPS_CGU_IF_CLK, 1, 0)

static unsigned int cgu_get_pll0_fdiv(void);

static inline unsigned int
get_input_clock(int pll)
{
	switch(pll)
	{
	case 0:
		if(CGU_PLL0_SRC)
			return BASIS_INPUT_CRYSTAL_USB;
		else if(CGU_PLL0_PHASE_DIVIDER_ENABLE)
			return BASIC_INPUT_CLOCK_FREQUENCY_1;
		else
			return BASIC_INPUT_CLOCK_FREQUENCY_2;
	case 1:
		if(CGU_PLL1_SRC)
			return BASIS_INPUT_CRYSTAL_USB;
		else if(CGU_PLL0_PHASE_DIVIDER_ENABLE)
			return BASIC_INPUT_CLOCK_FREQUENCY_1;
		else
			return BASIC_INPUT_CLOCK_FREQUENCY_2;
	case 2:
		switch(CGU_PLL2_SRC)
		{
		case 0:
			return cgu_get_pll0_fdiv();
		case 1:
			return CGU_PLL2_PHASE_DIVIDER_ENABLE ? BASIC_INPUT_CLOCK_FREQUENCY_1 : BASIC_INPUT_CLOCK_FREQUENCY_2;
		case 2:
			return BASIS_INPUT_CRYSTAL_USB;
		}
	default:
		return 0;
	}
}

static inline unsigned int
cal_dsm(int pll, unsigned int num, unsigned int den)
{
	u64 res, clock = get_input_clock(pll);

	res = num * clock;
	do_div(res, den);
	return res;
}

static inline unsigned int
mash_dsm(int pll, unsigned int M, unsigned int N, unsigned int K)
{
	unsigned int num = ((N + 1) << 10) + K;
	unsigned int den = (M + 1) << 10;

	return cal_dsm(pll, num, den);
}

static inline unsigned int
ssff_dsm_1(int pll, unsigned int M,	unsigned int N, unsigned int K)
{
	unsigned int num = ((N + 1) << 11) + K + 512;
	unsigned int den = (M + 1) << 11;

	return cal_dsm(pll, num, den);
}

static inline unsigned int
ssff_dsm_2(int pll, unsigned int M,	unsigned int N, unsigned int K)
{
	unsigned int num = K >= 512 ?
		((N + 1) << 12) + K - 512 : ((N + 1) << 12) + K + 3584;
	unsigned int den = (M + 1) << 12;

	return cal_dsm(pll, num, den);
}

static inline unsigned int
dsm(int pll, unsigned int M, unsigned int N, unsigned int K,
	unsigned int dsmsel, unsigned int phase_div_en)
{
	if(!dsmsel)
		return mash_dsm(pll, M, N, K);
	else if(!phase_div_en)
		return mash_dsm(pll, M, N, K);
	else
		return ssff_dsm_2(pll, M, N, K);
}

static inline unsigned int
cgu_get_pll0_fosc(void)
{
	if(CGU_PLL0_BYPASS)
		return get_input_clock(0);
	else
		return !CGU_PLL0_CFG_FRAC_EN
			? dsm(0, CGU_PLL0_CFG_PLLM, CGU_PLL0_CFG_PLLN, 0, CGU_PLL0_CFG_DSMSEL,
				CGU_PLL0_PHASE_DIVIDER_ENABLE)
			: dsm(0, CGU_PLL0_CFG_PLLM, CGU_PLL0_CFG_PLLN, CGU_PLL0_CFG_PLLK,
				CGU_PLL0_CFG_DSMSEL, CGU_PLL0_PHASE_DIVIDER_ENABLE);
}

static inline unsigned int
cgu_get_pll0_fps(int phase)
{
	register unsigned int fps = cgu_get_pll0_fosc();

	switch(phase)
	{
	case 1:
		/*  1.25    */
		fps = ((fps << 2) + 2) / 5;
		break;
	case 2:
		/*  1.5     */
		fps = ((fps << 1) + 1) / 3;
		break;
	}
	return fps;
}

static unsigned int
cgu_get_pll0_fdiv(void)
{
	register unsigned int div = CGU_PLL2_CFG_INPUT_DIV + 1;

	return (cgu_get_pll0_fosc() + (div >> 1)) / div;
}

static inline unsigned int
cgu_get_pll1_fosc(void)
{
	if(CGU_PLL1_BYPASS)
		return get_input_clock(1);
	else
		return !CGU_PLL1_CFG_FRAC_EN
			? dsm(1, CGU_PLL1_CFG_PLLM, CGU_PLL1_CFG_PLLN, 0, CGU_PLL1_CFG_DSMSEL, 0)
			: dsm(1, CGU_PLL1_CFG_PLLM, CGU_PLL1_CFG_PLLN, CGU_PLL1_CFG_PLLK, CGU_PLL1_CFG_DSMSEL, 0);
}

static inline unsigned int
cgu_get_pll1_fps(void)
{
	register unsigned int fps = cgu_get_pll1_fosc();

	return ((fps << 1) + 1) / 3;
}

static inline unsigned int
cgu_get_pll1_fdiv(void)
{
	return cgu_get_pll1_fosc();
}

static inline unsigned int
cgu_get_pll2_fosc(void)
{
	u64 res, clock = get_input_clock(2);

	if ( CGU_PLL2_BYPASS )
		return get_input_clock(2);

	res = (CGU_PLL2_CFG_PLLN + 1) * clock;
	do_div(res, CGU_PLL2_CFG_PLLM + 1);

	return res;
}

static inline unsigned int
cgu_get_pll2_fps(int phase)
{
	register unsigned int fps = cgu_get_pll2_fosc();

	switch ( phase )
	{
	case 1:
		/*  1.125   */
		fps = ((fps << 2) + 2) / 5; break;
	case 2:
		/*  1.25    */
		fps = ((fps << 3) + 4) / 9;
	}

	return fps;
}

static inline unsigned int
cgu_get_pll2_fdiv(void)
{
	register unsigned int div = CGU_IF_CLK_PCI_CLK + 1;
	return (cgu_get_pll2_fosc() + (div >> 1)) / div;
}

unsigned int
cgu_get_mips_clock(int cpu)
{
	register unsigned int ret = cgu_get_pll0_fosc();
	register unsigned int cpusel = cpu == 0 ? CGU_SYS_CPU0SEL : CGU_SYS_CPU1SEL;

	if(cpusel == 0)
		return ret;
	else if(cpusel == 2)
		ret <<= 1;

	switch(CGU_SYS_DDR_SEL)
	{
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

unsigned int
cgu_get_cpu_clock(void)
{
	return cgu_get_mips_clock(0);
}

unsigned int
cgu_get_io_region_clock(void)
{
	register unsigned int ret = cgu_get_pll0_fosc();

	switch(CGU_SYS_DDR_SEL)
	{
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

unsigned int
cgu_get_fpi_bus_clock(int fpi)
{
	register unsigned int ret = cgu_get_io_region_clock();

	if((fpi == 2) && (CGU_SYS_FPI_SEL))
		ret >>= 1;

	return ret;
}

unsigned int
cgu_get_pp32_clock(void)
{
	switch(CGU_SYS_PPESEL)
	{
	default:
	case 0:
		return cgu_get_pll2_fps(1);
	case 1:
		return cgu_get_pll2_fps(2);
	case 2:
		return (cgu_get_pll2_fps(1) + 1) >> 1;
	case 3:
		return (cgu_get_pll2_fps(2) + 1) >> 1;
	}
}

unsigned int
cgu_get_ethernet_clock(int mii)
{
	switch(CGU_IF_CLK_MIISEL)
	{
	case 0:
		return (cgu_get_pll2_fosc() + 3) / 12;
	case 1:
		return (cgu_get_pll2_fosc() + 3) / 6;
	case 2:
		return 50000000;
	case 3:
		return 25000000;
	}
	return 0;
}

unsigned int
cgu_get_usb_clock(void)
{
	switch(CGU_IF_CLK_USBSEL)
	{
	case 0:
		return (cgu_get_pll2_fosc() + 12) / 25;
	case 1:
		return 12000000;
	case 2:
		return 12000000 / 4;
	case 3:
		return 12000000;
	}
	return 0;
}

unsigned int
cgu_get_clockout(int clkout)
{
	unsigned int fosc1 = cgu_get_pll1_fosc();
	unsigned int fosc2 = cgu_get_pll2_fosc();

	if(clkout > 3 || clkout < 0)
		return 0;

	switch(((unsigned int)clkout << 2) | GET_BITS(*IFXMIPS_CGU_IF_CLK, 15 - clkout * 2, 14 - clkout * 2))
	{
	case 0: /*  32.768KHz   */
	case 15:
		return (fosc1 + 6000) / 12000;
	case 1: /*  1.536MHz    */
		return (fosc1 + 128) / 256;
	case 2: /*  2.5MHz      */
		return (fosc2 + 60) / 120;
	case 3: /*  12MHz       */
	case 5:
	case 12:
		return (fosc2 + 12) / 25;
	case 4: /*  40MHz       */
		return (cgu_get_pll2_fps(2) + 3) / 6;
	case 6: /*  24MHz       */
		return (cgu_get_pll2_fps(2) + 5) / 10;
	case 7: /*  48MHz       */
		return (cgu_get_pll2_fps(2) + 2) / 5;
	case 8: /*  25MHz       */
	case 14:
		return (fosc2 + 6) / 12;
	case 9: /*  50MHz       */
	case 13:
		return (fosc2 + 3) / 6;
	case 10:/*  30MHz       */
		return (fosc2 + 5) / 10;
	case 11:/*  60MHz       */
		return (fosc2 + 2) / 5;
	}
	return 0;
}
