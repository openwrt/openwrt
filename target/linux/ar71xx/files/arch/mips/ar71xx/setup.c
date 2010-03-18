/*
 *  Atheros AR71xx SoC specific setup
 *
 *  Copyright (C) 2008-2009 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  Parts of this file are based on Atheros' 2.6.15 BSP
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
#define AR71XX_BASE_FREQ	40000000
#define AR91XX_BASE_FREQ	5000000
#define AR724X_BASE_FREQ	5000000

u32 ar71xx_cpu_freq;
EXPORT_SYMBOL_GPL(ar71xx_cpu_freq);

u32 ar71xx_ahb_freq;
EXPORT_SYMBOL_GPL(ar71xx_ahb_freq);

u32 ar71xx_ddr_freq;
EXPORT_SYMBOL_GPL(ar71xx_ddr_freq);

enum ar71xx_soc_type ar71xx_soc;
EXPORT_SYMBOL_GPL(ar71xx_soc);

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
	     size <<= 1 ) {
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

	case REV_ID_MAJOR_AR724X:
		ar71xx_soc = AR71XX_SOC_AR7240;
		chip = "7240";
		rev = (id & AR724X_REV_ID_REVISION_MASK);
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

	default:
		panic("ar71xx: unknown chip id:0x%08x\n", id);
	}

	sprintf(ar71xx_sys_type, "Atheros AR%s rev %u", chip, rev);
}

static void __init ar91xx_detect_sys_frequency(void)
{
	u32 pll;
	u32 freq;
	u32 div;

	pll = ar71xx_pll_rr(AR91XX_PLL_REG_CPU_CONFIG);

	div = ((pll >> AR91XX_PLL_DIV_SHIFT) & AR91XX_PLL_DIV_MASK);
	freq = div * AR91XX_BASE_FREQ;

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

	pll = ar71xx_pll_rr(AR71XX_PLL_REG_CPU_CONFIG);

	div = ((pll >> AR71XX_PLL_DIV_SHIFT) & AR71XX_PLL_DIV_MASK) + 1;
	freq = div * AR71XX_BASE_FREQ;

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

	pll = ar71xx_pll_rr(AR724X_PLL_REG_CPU_CONFIG);

	div = ((pll >> AR724X_PLL_DIV_SHIFT) & AR724X_PLL_DIV_MASK);
	freq = div * AR724X_BASE_FREQ;

	div = ((pll >> AR724X_PLL_REF_DIV_SHIFT) & AR724X_PLL_REF_DIV_MASK);
	freq *= div;

	ar71xx_cpu_freq = freq;

	div = ((pll >> AR724X_DDR_DIV_SHIFT) & AR724X_DDR_DIV_MASK) + 1;
	ar71xx_ddr_freq = freq / div;

	div = (((pll >> AR724X_AHB_DIV_SHIFT) & AR724X_AHB_DIV_MASK) + 1) * 2;
	ar71xx_ahb_freq = ar71xx_cpu_freq / div;
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
		ar724x_detect_sys_frequency();
		break;

	case AR71XX_SOC_AR9130:
	case AR71XX_SOC_AR9132:
		ar91xx_detect_sys_frequency();
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

	printk(KERN_INFO
		"%s, CPU:%u.%03u MHz, AHB:%u.%03u MHz, DDR:%u.%03u MHz\n",
		ar71xx_sys_type,
		ar71xx_cpu_freq / 1000000, (ar71xx_cpu_freq / 1000) % 1000,
		ar71xx_ahb_freq / 1000000, (ar71xx_ahb_freq / 1000) % 1000,
		ar71xx_ddr_freq / 1000000, (ar71xx_ddr_freq / 1000) % 1000);

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
