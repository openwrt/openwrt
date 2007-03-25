/*
 * $Id$
 * 
 * Copyright (C) 2007 OpenWrt.org
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <linux/init.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <asm/addrspace.h>
#include <asm/io.h>
#include <asm/ar7/ar7.h>

#define BOOT_PLL_SOURCE_MASK 0x3
#define CPU_PLL_SOURCE_SHIFT 16
#define BUS_PLL_SOURCE_SHIFT 14
#define USB_PLL_SOURCE_SHIFT 18
#define DSP_PLL_SOURCE_SHIFT 22
#define BOOT_PLL_SOURCE_AFE 0
#define BOOT_PLL_SOURCE_BUS 0
#define BOOT_PLL_SOURCE_REF 1
#define BOOT_PLL_SOURCE_XTAL 2
#define BOOT_PLL_SOURCE_CPU 3
#define BOOT_PLL_BYPASS 0x00000020
#define BOOT_PLL_ASYNC_MODE 0x02000000
#define BOOT_PLL_2TO1_MODE 0x00008000

struct tnetd7300_clock {
	u32 ctrl;
#define PREDIV_MASK 0x001f0000
#define PREDIV_SHIFT 16
#define POSTDIV_MASK 0x0000001f
	u32 unused1[3];
	u32 pll;
#define MUL_MASK 0x0000f000
#define MUL_SHIFT 12
#define PLL_MODE_MASK 0x00000001
#define PLL_NDIV 0x00000800
#define PLL_DIV 0x00000002
#define PLL_STATUS 0x00000001
	u32 unused2[3];
} __attribute__ ((packed));

struct tnetd7300_clocks {
	struct tnetd7300_clock bus;
	struct tnetd7300_clock cpu;
	struct tnetd7300_clock usb;
	struct tnetd7300_clock dsp;
} __attribute__ ((packed));

struct tnetd7200_clock {
	u32 ctrl;
	u32 unused1[3];
#define DIVISOR_ENABLE_MASK 0x00008000
	u32 mul;
	u32 prediv;
	u32 postdiv;
	u32 unused2[7];
	u32 cmd;
	u32 status;
	u32 cmden;
	u32 padding[15];
};

struct tnetd7200_clocks {
	struct tnetd7200_clock cpu;
	struct tnetd7200_clock dsp;
	struct tnetd7200_clock usb;
};

int ar7_afe_clock = 35328000;
int ar7_ref_clock = 25000000;
int ar7_xtal_clock = 24000000;

int ar7_cpu_clock = 150000000;
EXPORT_SYMBOL(ar7_cpu_clock);
int ar7_bus_clock = 125000000;
EXPORT_SYMBOL(ar7_bus_clock);
int ar7_dsp_clock = 0;
EXPORT_SYMBOL(ar7_dsp_clock);

static int gcd(int x, int y)
{
	if (x > y)
		return (x % y) ? gcd(y, x % y) : y;
	return (y % x) ? gcd(x, y % x) : x;
}

static inline int ABS(int x)
{
	return (x >= 0) ? x : -x;
}

static void approximate(int base, int target, int *prediv,
			int *postdiv, int *mul)
{
	int i, j, k, freq, res = target;
	for (i = 1; i <= 16; i++) {
		for (j = 1; j <= 32; j++) {
			for (k = 1; k <= 32; k++) {
				freq = ABS(base / j * i / k - target);
				if (freq < res) {
					res = freq;
					*mul = i;
					*prediv = j;
					*postdiv = k;
				}
			}
		}
	}
}

static void calculate(int base, int target, int *prediv, int *postdiv,
		      int *mul)
{
	int tmp_gcd, tmp_base, tmp_freq;

	for (*prediv = 1; *prediv <= 32; (*prediv)++) {
		tmp_base = base / *prediv;
		tmp_gcd = gcd(target, tmp_base);
		*mul = target / tmp_gcd;
		*postdiv = tmp_base / tmp_gcd;
		if ((*mul < 1) || (*mul >= 16))
			continue;
		if ((*postdiv > 0) & (*postdiv <= 32))
			break;
	}

	if (base / (*prediv) * (*mul) / (*postdiv) != target) {
		approximate(base, target, prediv, postdiv, mul);
		tmp_freq = base / (*prediv) * (*mul) / (*postdiv);
		printk(KERN_WARNING
		       "Adjusted requested frequency %d to %d\n",
		       target, tmp_freq);
	}

	printk(KERN_DEBUG "Clocks: prediv: %d, postdiv: %d, mul: %d\n",
	       *prediv, *postdiv, *mul);
}

static int tnetd7300_dsp_clock(void)
{
	u32 didr1, didr2;
	u8 rev = ar7_chip_rev();
	didr1 = readl((void *)KSEG1ADDR(AR7_REGS_GPIO + 0x18));
	didr2 = readl((void *)KSEG1ADDR(AR7_REGS_GPIO + 0x1c));
	if (didr2 & (1 << 23))
		return 0;
	if ((rev >= 0x23) && (rev != 0x57))
		return 250000000;
	if ((((didr2 & 0x1fff) << 10) | ((didr1 & 0xffc00000) >> 22))
	    > 4208000)
		return 250000000;
	return 0;
}

static int tnetd7300_get_clock(u32 shift, struct tnetd7300_clock *clock,
			       u32 *bootcr, u32 bus_clock)
{
	int product;
	int base_clock = ar7_ref_clock;
	int prediv = ((clock->ctrl & PREDIV_MASK) >> PREDIV_SHIFT) + 1;
	int postdiv = (clock->ctrl & POSTDIV_MASK) + 1;
	int divisor = prediv * postdiv;
	int mul = ((clock->pll & MUL_MASK) >> MUL_SHIFT) + 1;

	switch ((*bootcr & (BOOT_PLL_SOURCE_MASK << shift)) >> shift) {
	case BOOT_PLL_SOURCE_BUS:
		base_clock = bus_clock;
		break;
	case BOOT_PLL_SOURCE_REF:
		base_clock = ar7_ref_clock;
		break;
	case BOOT_PLL_SOURCE_XTAL:
		base_clock = ar7_xtal_clock;
		break;
	case BOOT_PLL_SOURCE_CPU:
		base_clock = ar7_cpu_clock;
		break;
	}

	if (*bootcr & BOOT_PLL_BYPASS)
		return base_clock / divisor;

	if ((clock->pll & PLL_MODE_MASK) == 0)
		return (base_clock >> (mul / 16 + 1)) / divisor;

	if ((clock->pll & (PLL_NDIV | PLL_DIV)) == (PLL_NDIV | PLL_DIV)) {
		product = (mul & 1) ? 
			(base_clock * mul) >> 1 :
			(base_clock * (mul - 1)) >> 2;
		return product / divisor;
	}

	if (mul == 16)
		return base_clock / divisor;

	return base_clock * mul / divisor;
}

static void tnetd7300_set_clock(u32 shift, struct tnetd7300_clock *clock,
				u32 *bootcr, u32 frequency)
{
	volatile u32 status;
	int prediv, postdiv, mul;
	int base_clock = ar7_bus_clock;

	switch ((*bootcr & (BOOT_PLL_SOURCE_MASK << shift)) >> shift) {
	case BOOT_PLL_SOURCE_BUS:
		base_clock = ar7_bus_clock;
		break;
	case BOOT_PLL_SOURCE_REF:
		base_clock = ar7_ref_clock;
		break;
	case BOOT_PLL_SOURCE_XTAL:
		base_clock = ar7_xtal_clock;
		break;
	case BOOT_PLL_SOURCE_CPU:
		base_clock = ar7_cpu_clock;
		break;
	}

	calculate(base_clock, frequency, &prediv, &postdiv, &mul);

	clock->ctrl = ((prediv - 1) << PREDIV_SHIFT) | (postdiv - 1);
	mdelay(1);
	clock->pll = 4;
	do {
		status = clock->pll;
	} while (status & PLL_STATUS);
	clock->pll = ((mul - 1) << MUL_SHIFT) | (0xff << 3) | 0x0e;
	mdelay(75);
}

static void __init tnetd7300_init_clocks(void)
{
	u32 *bootcr = (u32 *)ioremap_nocache(AR7_REGS_DCL, 4);
	struct tnetd7300_clocks *clocks = (struct tnetd7300_clocks *)ioremap_nocache(AR7_REGS_POWER + 0x20, sizeof(struct tnetd7300_clocks)); 

	ar7_bus_clock = tnetd7300_get_clock(BUS_PLL_SOURCE_SHIFT, 
					    &clocks->bus, bootcr,
					    ar7_afe_clock);

	if (*bootcr & BOOT_PLL_ASYNC_MODE) {
		ar7_cpu_clock = tnetd7300_get_clock(CPU_PLL_SOURCE_SHIFT, 
						    &clocks->cpu,
						    bootcr, ar7_afe_clock);
	} else {
		ar7_cpu_clock = ar7_bus_clock;
	}

	tnetd7300_set_clock(USB_PLL_SOURCE_SHIFT, &clocks->usb,
			    bootcr, 48000000);

	if (ar7_dsp_clock == 250000000)
		tnetd7300_set_clock(DSP_PLL_SOURCE_SHIFT, &clocks->dsp,
				    bootcr, ar7_dsp_clock);

	iounmap(clocks);
	iounmap(bootcr);
}

static int tnetd7200_get_clock(int base, struct tnetd7200_clock *clock,
			       u32 *bootcr, u32 bus_clock)
{
	int divisor = ((clock->prediv & 0x1f) + 1) * 
		((clock->postdiv & 0x1f) + 1);

	if (*bootcr & BOOT_PLL_BYPASS)
		return base / divisor;

	return base * ((clock->mul & 0xf) + 1) / divisor;
}

static void tnetd7200_set_clock(int base, struct tnetd7200_clock *clock,
				u32 *bootcr, u32 frequency) 
{
	volatile u32 status;
	int prediv, postdiv, mul;

	calculate(base, frequency, &prediv, &postdiv, &mul);

	clock->ctrl = 0;
	clock->prediv = DIVISOR_ENABLE_MASK | prediv;
	clock->mul = mul;
	mdelay(1);
	do {
		status = clock->status;
	} while (status & PLL_STATUS);
	clock->postdiv = DIVISOR_ENABLE_MASK | postdiv;
	clock->cmden = 1;
	clock->cmd = 1;
	do {
		status = clock->status;
	} while (status & PLL_STATUS);
	clock->ctrl = 1;
}

static void __init tnetd7200_init_clocks(void)
{
	u32 *bootcr = (u32 *)ioremap_nocache(AR7_REGS_DCL, 4);
	struct tnetd7200_clocks *clocks = (struct tnetd7200_clocks *)ioremap_nocache(AR7_REGS_POWER + 0x80, sizeof(struct tnetd7200_clocks)); 

	ar7_cpu_clock = tnetd7200_get_clock(ar7_afe_clock,
					    &clocks->cpu,
					    bootcr, ar7_afe_clock);

	if (*bootcr & BOOT_PLL_ASYNC_MODE) {
		ar7_bus_clock = 125000000;
	} else {
		if (*bootcr & BOOT_PLL_2TO1_MODE) {
			ar7_bus_clock = ar7_cpu_clock / 2;
		} else {
			ar7_bus_clock = ar7_cpu_clock;
		}
	}

	tnetd7200_set_clock(ar7_ref_clock * 5, &clocks->usb,
			    bootcr, 48000000);

	if (ar7_dsp_clock == 250000000)
		tnetd7200_set_clock(ar7_ref_clock, &clocks->dsp,
				    bootcr, ar7_dsp_clock);

	iounmap(clocks);
	iounmap(bootcr);
}

void __init ar7_init_clocks(void)
{
	switch (ar7_chip_id()) {
	case AR7_CHIP_7100:
		tnetd7200_init_clocks();
		break;
	case AR7_CHIP_7200:
#warning FIXME: check revision
		ar7_dsp_clock = 250000000;
		tnetd7200_init_clocks();
		break;
	case AR7_CHIP_7300:
		ar7_dsp_clock = tnetd7300_dsp_clock();
		tnetd7300_init_clocks();
		break;
	default:
		break;
	}
}
