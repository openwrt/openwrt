/*
 *  Atheros AR71xx SoC specific setup
 *
 *  Copyright (C) 2008 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  Parts of this file are based on Atheros' 2.6.15 BSP
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/pci.h>
#include <linux/serial_8250.h>
#include <linux/bootmem.h>

#include <asm/bootinfo.h>
#include <asm/traps.h>
#include <asm/time.h>		/* for mips_hpt_frequency */
#include <asm/reboot.h>		/* for _machine_{restart,halt} */

#include <asm/mach-ar71xx/ar71xx.h>
#include <asm/mach-ar71xx/pci.h>
#include <asm/mach-ar71xx/platform.h>

#define AR71XX_SYS_TYPE_LEN	64
#define AR71XX_BASE_FREQ	40000000
#define AR91XX_BASE_FREQ	5000000

#define AR71XX_MEM_SIZE_MIN	0x0200000
#define AR71XX_MEM_SIZE_MAX	0x8000000

u32 ar71xx_cpu_freq;
EXPORT_SYMBOL_GPL(ar71xx_cpu_freq);

u32 ar71xx_ahb_freq;
EXPORT_SYMBOL_GPL(ar71xx_ahb_freq);

u32 ar71xx_ddr_freq;
EXPORT_SYMBOL_GPL(ar71xx_ddr_freq);

int (*ar71xx_pci_bios_init)(unsigned nr_irqs,
			     struct ar71xx_pci_irq *map) __initdata;

int (*ar71xx_pci_be_handler)(int is_fixup);

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

static int ar71xx_be_handler(struct pt_regs *regs, int is_fixup)
{
	int err = 0;

	if (ar71xx_pci_be_handler)
		err = ar71xx_pci_be_handler(is_fixup);

	return (is_fixup && !err) ? MIPS_BE_FIXUP : MIPS_BE_FATAL;
}

int __init ar71xx_pci_init(unsigned nr_irqs, struct ar71xx_pci_irq *map)
{
	if (!ar71xx_pci_bios_init)
		return 0;

	return ar71xx_pci_bios_init(nr_irqs, map);
}

static void __init ar71xx_detect_mem_size(void)
{
	volatile u8 *p;
	u8 memsave;
	u32 size;

	p = (volatile u8 *) KSEG1ADDR(0);
	memsave = *p;
	for (size = AR71XX_MEM_SIZE_MIN;
	     size <= (AR71XX_MEM_SIZE_MAX >> 1); size <<= 1) {
		volatile u8 *r;

		r = (p + size);
		*p = 0x55;
		if (*r == 0x55) {
			/* Mirrored data found, try another pattern */
			*p = 0xAA;
			if (*r == 0xAA) {
				/* Mirrored data found again, stop detection */
				break;
			}
		}
	}
	*p = memsave;

	add_memory_region(0, size, BOOT_MEM_RAM);
}

static void __init ar71xx_detect_sys_type(void)
{
	char *chip;
	u32 id;
	u32 rev;

	id = ar71xx_reset_rr(RESET_REG_REV_ID) & REV_ID_MASK;
	rev = (id >> REV_ID_REVISION_SHIFT) & REV_ID_REVISION_MASK;
	switch (id & REV_ID_CHIP_MASK) {
	case REV_ID_CHIP_AR7130:
		chip = "7130";
		break;
	case REV_ID_CHIP_AR7141:
		chip = "7141";
		break;
	case REV_ID_CHIP_AR7161:
		chip = "7161";
		break;
	case REV_ID_CHIP_AR9130:
		chip = "9130";
		break;
	default:
		chip = "71xx";
	}

	sprintf(ar71xx_sys_type, "Atheros AR%s rev %u (id:0x%02x)",
		chip, rev, id);
}

static void __init ar91xx_detect_sys_frequency(void)
{
	u32 pll;
	u32 freq;
	u32 div;

	pll = ar71xx_pll_rr(PLL_REG_CPU_PLL_CFG);

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

	if ((ar71xx_reset_rr(RESET_REG_REV_ID) & REV_ID_MASK) >=
			REV_ID_CHIP_AR9130) {
		return ar91xx_detect_sys_frequency();
	}

	pll = ar71xx_pll_rr(PLL_REG_CPU_PLL_CFG);

	div = ((pll >> AR71XX_PLL_DIV_SHIFT) & AR71XX_PLL_DIV_MASK) + 1;
	freq = div * AR71XX_BASE_FREQ;

	div = ((pll >> AR71XX_CPU_DIV_SHIFT) & AR71XX_CPU_DIV_MASK) + 1;
	ar71xx_cpu_freq = freq / div;

	div = ((pll >> AR71XX_DDR_DIV_SHIFT) & AR71XX_DDR_DIV_MASK) + 1;
	ar71xx_ddr_freq = freq / div;

	div = (((pll >> AR71XX_AHB_DIV_SHIFT) & AR71XX_AHB_DIV_MASK) + 1) * 2;
	ar71xx_ahb_freq = ar71xx_cpu_freq / div;
}

#ifdef CONFIG_AR71XX_EARLY_SERIAL
static void __init ar71xx_early_serial_setup(void)
{
	struct uart_port p;

	memset(&p, 0, sizeof(p));

	p.flags		= UPF_BOOT_AUTOCONF | UPF_SKIP_TEST | UPF_IOREMAP;
	p.iotype	= UPIO_MEM32;
	p.uartclk	= ar71xx_ahb_freq;
	p.irq		= AR71XX_MISC_IRQ_UART;
	p.regshift	= 2;
	p.mapbase	= AR71XX_UART_BASE;

	early_serial_setup(&p);
}
#else
static inline void ar71xx_early_serial_setup(void) {};
#endif /* CONFIG_AR71XX_EARLY_SERIAL */

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
	ar71xx_detect_sys_frequency();

	_machine_restart = ar71xx_restart;
	_machine_halt = ar71xx_halt;
	pm_power_off = ar71xx_halt;

	board_be_handler = ar71xx_be_handler;

	ar71xx_early_serial_setup();
}

void __init plat_time_init(void)
{
	mips_hpt_frequency = ar71xx_cpu_freq / 2;
}
