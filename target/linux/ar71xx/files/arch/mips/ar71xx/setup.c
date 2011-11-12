/*
 *  Atheros AR71xx SoC specific setup
 *
 *  Copyright (C) 2010-2011 Jaiganesh Narayanan <jnarayanan@atheros.com>
 *  Copyright (C) 2008-2009 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  Parts of this file are based on Atheros 2.6.15 BSP
 *  Parts of this file are based on Atheros 2.6.31 BSP
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/bootmem.h>

#include <asm/bootinfo.h>
#include <asm/time.h>		/* for mips_hpt_frequency */
#include <asm/reboot.h>		/* for _machine_{restart,halt} */
#include <asm/mips_machine.h>

#include <asm/mach-ar71xx/ar71xx.h>

#include "machtype.h"
#include "devices.h"

#define AR71XX_SYS_TYPE_LEN	64

u32 ar71xx_cpu_freq;
EXPORT_SYMBOL_GPL(ar71xx_cpu_freq);

u32 ar71xx_ahb_freq;
EXPORT_SYMBOL_GPL(ar71xx_ahb_freq);

u32 ar71xx_ddr_freq;
EXPORT_SYMBOL_GPL(ar71xx_ddr_freq);

u32 ar71xx_ref_freq;
EXPORT_SYMBOL_GPL(ar71xx_ref_freq);

enum ar71xx_soc_type ar71xx_soc;
EXPORT_SYMBOL_GPL(ar71xx_soc);

u32 ar71xx_soc_rev;
EXPORT_SYMBOL_GPL(ar71xx_soc_rev);

static char ar71xx_sys_type[AR71XX_SYS_TYPE_LEN];

static void ar71xx_restart(char *command)
{
	ar71xx_device_stop(RESET_MODULE_FULL_CHIP);
	for (;;)
		if (cpu_wait)
			cpu_wait();
}

static void ar71xx_halt(void)
{
	while (1)
		cpu_wait();
}

static void __init ar71xx_detect_mem_size(void)
{
	unsigned long size;

	for (size = AR71XX_MEM_SIZE_MIN; size < AR71XX_MEM_SIZE_MAX;
	     size <<= 1) {
		if (!memcmp(ar71xx_detect_mem_size,
			    ar71xx_detect_mem_size + size, 1024))
			break;
	}

	add_memory_region(0, size, BOOT_MEM_RAM);
}

static void __init ar71xx_detect_sys_type(void)
{
	char *chip = "????";
	u32 id;
	u32 major;
	u32 minor;
	u32 rev = 0;

	id = ar71xx_reset_rr(AR71XX_RESET_REG_REV_ID);
	major = id & REV_ID_MAJOR_MASK;

	switch (major) {
	case REV_ID_MAJOR_AR71XX:
		minor = id & AR71XX_REV_ID_MINOR_MASK;
		rev = id >> AR71XX_REV_ID_REVISION_SHIFT;
		rev &= AR71XX_REV_ID_REVISION_MASK;
		switch (minor) {
		case AR71XX_REV_ID_MINOR_AR7130:
			ar71xx_soc = AR71XX_SOC_AR7130;
			chip = "7130";
			break;

		case AR71XX_REV_ID_MINOR_AR7141:
			ar71xx_soc = AR71XX_SOC_AR7141;
			chip = "7141";
			break;

		case AR71XX_REV_ID_MINOR_AR7161:
			ar71xx_soc = AR71XX_SOC_AR7161;
			chip = "7161";
			break;
		}
		break;

	case REV_ID_MAJOR_AR7240:
		ar71xx_soc = AR71XX_SOC_AR7240;
		chip = "7240";
		rev = id & AR724X_REV_ID_REVISION_MASK;
		break;

	case REV_ID_MAJOR_AR7241:
		ar71xx_soc = AR71XX_SOC_AR7241;
		chip = "7241";
		rev = id & AR724X_REV_ID_REVISION_MASK;
		break;

	case REV_ID_MAJOR_AR7242:
		ar71xx_soc = AR71XX_SOC_AR7242;
		chip = "7242";
		rev = id & AR724X_REV_ID_REVISION_MASK;
		break;

	case REV_ID_MAJOR_AR913X:
		minor = id & AR91XX_REV_ID_MINOR_MASK;
		rev = id >> AR91XX_REV_ID_REVISION_SHIFT;
		rev &= AR91XX_REV_ID_REVISION_MASK;
		switch (minor) {
		case AR91XX_REV_ID_MINOR_AR9130:
			ar71xx_soc = AR71XX_SOC_AR9130;
			chip = "9130";
			break;

		case AR91XX_REV_ID_MINOR_AR9132:
			ar71xx_soc = AR71XX_SOC_AR9132;
			chip = "9132";
			break;
		}
		break;

	case REV_ID_MAJOR_AR9330:
		ar71xx_soc = AR71XX_SOC_AR9330;
		chip = "9330";
		rev = id & AR933X_REV_ID_REVISION_MASK;
		break;

	case REV_ID_MAJOR_AR9331:
		ar71xx_soc = AR71XX_SOC_AR9331;
		chip = "9331";
		rev = id & AR933X_REV_ID_REVISION_MASK;
		break;

	case REV_ID_MAJOR_AR9342:
		ar71xx_soc = AR71XX_SOC_AR9342;
		chip = "9342";
		rev = id & AR934X_REV_ID_REVISION_MASK;
		break;

	case REV_ID_MAJOR_AR9344:
		ar71xx_soc = AR71XX_SOC_AR9344;
		chip = "9344";
		rev = id & AR934X_REV_ID_REVISION_MASK;
		break;

	default:
		panic("ar71xx: unknown chip id:0x%08x\n", id);
	}

	ar71xx_soc_rev = rev;

	sprintf(ar71xx_sys_type, "Atheros AR%s rev %u", chip, rev);
	pr_info("SoC: %s\n", ar71xx_sys_type);
}

static void __init ar934x_detect_sys_frequency(void)
{
	u32 pll, out_div, ref_div, nint, frac, clk_ctrl, postdiv;
	u32 cpu_pll, ddr_pll;

	if (ar71xx_reset_rr(AR934X_RESET_REG_BOOTSTRAP) & AR934X_REF_CLK_40)
		ar71xx_ref_freq = 40 * 1000 * 1000;
	else
		ar71xx_ref_freq = 25 * 1000 * 1000;

	pll = ar71xx_pll_rr(AR934X_PLL_REG_CPU_CONFIG);
	out_div	= AR934X_CPU_PLL_CFG_OUTDIV_GET(pll);
	ref_div	= AR934X_CPU_PLL_CFG_REFDIV_GET(pll);
	nint	= AR934X_CPU_PLL_CFG_NINT_GET(pll);
	frac	= AR934X_CPU_PLL_CFG_NFRAC_GET(pll);

	cpu_pll = nint * ar71xx_ref_freq / ref_div;
	cpu_pll += frac * ar71xx_ref_freq / (ref_div * (2 << 6));
	cpu_pll /= (1 << out_div);

	pll = ar71xx_pll_rr(AR934X_PLL_REG_DDR_CONFIG);
	out_div	= AR934X_DDR_PLL_CFG_OUTDIV_GET(pll);
	ref_div	= AR934X_DDR_PLL_CFG_REFDIV_GET(pll);
	nint	= AR934X_DDR_PLL_CFG_NINT_GET(pll);
	frac	= AR934X_DDR_PLL_CFG_NFRAC_GET(pll);

	ddr_pll = nint * ar71xx_ref_freq / ref_div;
	ddr_pll += frac * ar71xx_ref_freq / (ref_div * (2 << 10));
	ddr_pll /= (1 << out_div);

	clk_ctrl = ar71xx_pll_rr(AR934X_PLL_REG_DDR_CTRL_CLOCK);

	if (clk_ctrl & AR934X_CPU_DDR_CLK_CTRL_CPU_PLL_BYPASS) {
		ar71xx_cpu_freq = ar71xx_ref_freq;
	} else {
		postdiv = AR934X_CPU_DDR_CLK_CTRL_CPU_POST_DIV_GET(clk_ctrl);

		if (clk_ctrl & AR934X_CPU_DDR_CLK_CTRL_CPUCLK_FROM_CPUPLL)
			ar71xx_cpu_freq = cpu_pll / (postdiv + 1);
		else
			ar71xx_cpu_freq = ddr_pll / (postdiv + 1);
	}

	if (clk_ctrl & AR934X_CPU_DDR_CLK_CTRL_DDR_PLL_BYPASS) {
		ar71xx_ddr_freq = ar71xx_ref_freq;
	} else {
		postdiv = AR934X_CPU_DDR_CLK_CTRL_DDR_POST_DIV_GET(clk_ctrl);

		if (clk_ctrl & AR934X_CPU_DDR_CLK_CTRL_DDRCLK_FROM_DDRPLL)
			ar71xx_ddr_freq = ddr_pll / (postdiv + 1);
		else
			ar71xx_ddr_freq = cpu_pll / (postdiv + 1);
	}

	if (clk_ctrl & AR934X_CPU_DDR_CLK_CTRL_AHB_PLL_BYPASS) {
		ar71xx_ahb_freq = ar71xx_ref_freq;
	} else {
		postdiv = AR934X_CPU_DDR_CLK_CTRL_AHB_POST_DIV_GET(clk_ctrl);

		if (clk_ctrl & AR934X_CPU_DDR_CLK_CTRL_AHBCLK_FROM_DDRPLL)
			ar71xx_ahb_freq = ddr_pll / (postdiv + 1);
		else
			ar71xx_ahb_freq = cpu_pll / (postdiv + 1);
	}
}

static void __init ar91xx_detect_sys_frequency(void)
{
	u32 pll;
	u32 freq;
	u32 div;

	ar71xx_ref_freq = 5 * 1000 * 1000;

	pll = ar71xx_pll_rr(AR91XX_PLL_REG_CPU_CONFIG);

	div = ((pll >> AR91XX_PLL_DIV_SHIFT) & AR91XX_PLL_DIV_MASK);
	freq = div * ar71xx_ref_freq;

	ar71xx_cpu_freq = freq;

	div = ((pll >> AR91XX_DDR_DIV_SHIFT) & AR91XX_DDR_DIV_MASK) + 1;
	ar71xx_ddr_freq = freq / div;

	div = (((pll >> AR91XX_AHB_DIV_SHIFT) & AR91XX_AHB_DIV_MASK) + 1) * 2;
	ar71xx_ahb_freq = ar71xx_cpu_freq / div;
}

static void __init ar71xx_detect_sys_frequency(void)
{
	u32 pll;
	u32 freq;
	u32 div;

	ar71xx_ref_freq = 40 * 1000 * 1000;

	pll = ar71xx_pll_rr(AR71XX_PLL_REG_CPU_CONFIG);

	div = ((pll >> AR71XX_PLL_DIV_SHIFT) & AR71XX_PLL_DIV_MASK) + 1;
	freq = div * ar71xx_ref_freq;

	div = ((pll >> AR71XX_CPU_DIV_SHIFT) & AR71XX_CPU_DIV_MASK) + 1;
	ar71xx_cpu_freq = freq / div;

	div = ((pll >> AR71XX_DDR_DIV_SHIFT) & AR71XX_DDR_DIV_MASK) + 1;
	ar71xx_ddr_freq = freq / div;

	div = (((pll >> AR71XX_AHB_DIV_SHIFT) & AR71XX_AHB_DIV_MASK) + 1) * 2;
	ar71xx_ahb_freq = ar71xx_cpu_freq / div;
}

static void __init ar724x_detect_sys_frequency(void)
{
	u32 pll;
	u32 freq;
	u32 div;

	ar71xx_ref_freq = 5 * 1000 * 1000;

	pll = ar71xx_pll_rr(AR724X_PLL_REG_CPU_CONFIG);

	div = ((pll >> AR724X_PLL_DIV_SHIFT) & AR724X_PLL_DIV_MASK);
	freq = div * ar71xx_ref_freq;

	div = ((pll >> AR724X_PLL_REF_DIV_SHIFT) & AR724X_PLL_REF_DIV_MASK);
	freq *= div;

	ar71xx_cpu_freq = freq;

	div = ((pll >> AR724X_DDR_DIV_SHIFT) & AR724X_DDR_DIV_MASK) + 1;
	ar71xx_ddr_freq = freq / div;

	div = (((pll >> AR724X_AHB_DIV_SHIFT) & AR724X_AHB_DIV_MASK) + 1) * 2;
	ar71xx_ahb_freq = ar71xx_cpu_freq / div;
}

static void __init ar933x_detect_sys_frequency(void)
{
	u32 clock_ctrl;
	u32 cpu_config;
	u32 freq;
	u32 t;

	t = ar71xx_reset_rr(AR933X_RESET_REG_BOOTSTRAP);
	if (t & AR933X_BOOTSTRAP_REF_CLK_40)
		ar71xx_ref_freq = (40 * 1000 * 1000);
	else
		ar71xx_ref_freq = (25 * 1000 * 1000);

	clock_ctrl = ar71xx_pll_rr(AR933X_PLL_CLOCK_CTRL_REG);
	if (clock_ctrl & AR933X_PLL_CLOCK_CTRL_BYPASS) {
		ar71xx_cpu_freq = ar71xx_ref_freq;
		ar71xx_ahb_freq = ar71xx_ref_freq;
		ar71xx_ddr_freq = ar71xx_ref_freq;
	} else {
		cpu_config = ar71xx_pll_rr(AR933X_PLL_CPU_CONFIG_REG);

		t = (cpu_config >> AR933X_PLL_CPU_CONFIG_REFDIV_SHIFT) &
		    AR933X_PLL_CPU_CONFIG_REFDIV_MASK;
		freq = ar71xx_ref_freq / t;

		t = (cpu_config >> AR933X_PLL_CPU_CONFIG_NINT_SHIFT) &
		    AR933X_PLL_CPU_CONFIG_NINT_MASK;
		freq *= t;

		t = (cpu_config >> AR933X_PLL_CPU_CONFIG_OUTDIV_SHIFT) &
		    AR933X_PLL_CPU_CONFIG_OUTDIV_MASK;
		if (t == 0)
			t = 1;

		freq >>= t;

		t = ((clock_ctrl >> AR933X_PLL_CLOCK_CTRL_CPU_DIV_SHIFT) &
		     AR933X_PLL_CLOCK_CTRL_CPU_DIV_MASK) + 1;
		ar71xx_cpu_freq = freq / t;

		t = ((clock_ctrl >> AR933X_PLL_CLOCK_CTRL_DDR_DIV_SHIFT) &
		      AR933X_PLL_CLOCK_CTRL_DDR_DIV_MASK) + 1;
		ar71xx_ddr_freq = freq / t;

		t = ((clock_ctrl >> AR933X_PLL_CLOCK_CTRL_AHB_DIV_SHIFT) &
		     AR933X_PLL_CLOCK_CTRL_AHB_DIV_MASK) + 1;
		ar71xx_ahb_freq = freq / t;
	}
}

static void __init detect_sys_frequency(void)
{
	switch (ar71xx_soc) {
	case AR71XX_SOC_AR7130:
	case AR71XX_SOC_AR7141:
	case AR71XX_SOC_AR7161:
		ar71xx_detect_sys_frequency();
		break;

	case AR71XX_SOC_AR7240:
	case AR71XX_SOC_AR7241:
	case AR71XX_SOC_AR7242:
		ar724x_detect_sys_frequency();
		break;

	case AR71XX_SOC_AR9130:
	case AR71XX_SOC_AR9132:
		ar91xx_detect_sys_frequency();
		break;

	case AR71XX_SOC_AR9330:
	case AR71XX_SOC_AR9331:
		ar933x_detect_sys_frequency();
		break;

	case AR71XX_SOC_AR9341:
	case AR71XX_SOC_AR9342:
	case AR71XX_SOC_AR9344:
		ar934x_detect_sys_frequency();
		break;
	default:
		BUG();
	}
}

const char *get_system_type(void)
{
	return ar71xx_sys_type;
}

unsigned int __cpuinit get_c0_compare_irq(void)
{
	return CP0_LEGACY_COMPARE_IRQ;
}

void __init plat_mem_setup(void)
{
	set_io_port_base(KSEG1);

	ar71xx_ddr_base = ioremap_nocache(AR71XX_DDR_CTRL_BASE,
						AR71XX_DDR_CTRL_SIZE);

	ar71xx_pll_base = ioremap_nocache(AR71XX_PLL_BASE,
						AR71XX_PLL_SIZE);

	ar71xx_reset_base = ioremap_nocache(AR71XX_RESET_BASE,
						AR71XX_RESET_SIZE);

	ar71xx_gpio_base = ioremap_nocache(AR71XX_GPIO_BASE, AR71XX_GPIO_SIZE);

	ar71xx_usb_ctrl_base = ioremap_nocache(AR71XX_USB_CTRL_BASE,
						AR71XX_USB_CTRL_SIZE);

	ar71xx_detect_mem_size();
	ar71xx_detect_sys_type();
	detect_sys_frequency();

	pr_info("Clocks: CPU:%u.%03uMHz, DDR:%u.%03uMHz, AHB:%u.%03uMHz, "
		"Ref:%u.%03uMHz",
		ar71xx_cpu_freq / 1000000, (ar71xx_cpu_freq / 1000) % 1000,
		ar71xx_ddr_freq / 1000000, (ar71xx_ddr_freq / 1000) % 1000,
		ar71xx_ahb_freq / 1000000, (ar71xx_ahb_freq / 1000) % 1000,
		ar71xx_ref_freq / 1000000, (ar71xx_ref_freq / 1000) % 1000);

	_machine_restart = ar71xx_restart;
	_machine_halt = ar71xx_halt;
	pm_power_off = ar71xx_halt;
}

void __init plat_time_init(void)
{
	mips_hpt_frequency = ar71xx_cpu_freq / 2;
}

__setup("board=", mips_machtype_setup);

static int __init ar71xx_machine_setup(void)
{
	ar71xx_gpio_init();

	ar71xx_add_device_uart();
	ar71xx_add_device_wdt();

	mips_machine_setup();
	return 0;
}

arch_initcall(ar71xx_machine_setup);

static void __init ar71xx_generic_init(void)
{
	/* Nothing to do */
}

MIPS_MACHINE(AR71XX_MACH_GENERIC, "Generic", "Generic AR71xx board",
	     ar71xx_generic_init);
