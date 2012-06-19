/*
 * arch/arm/mach-mcs814x/common.c
 *
 * Core functions for Moschip MCS814x SoCs
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2.  This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/gpio.h>

#include <asm/setup.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <mach/hardware.h>
#include <mach/cpu.h>
#include <asm/pgtable.h>
#include <asm/mach/map.h>

static struct map_desc mcs814x_io_desc[] __initdata = {
	{
		.virtual	= MCS814X_IO_BASE,
		.pfn		= __phys_to_pfn(MCS814X_IO_START),
		.length		= MCS814X_IO_SIZE,
		.type		= MT_DEVICE
	},
};

#define SYSDBG_BS2	0x04
#define CPU_MODE_SHIFT	23
#define CPU_MODE_MASK	0x03

struct cpu_mode {
	const char *name;
	int gpio_start;
	int gpio_end;
};

static const struct cpu_mode cpu_modes[] = {
	{
		.name		= "I2S",
		.gpio_start	= 4,
		.gpio_end	= 8,
	},
	{
		.name		= "UART",
		.gpio_start	= 4,
		.gpio_end	= 9,
	},
	{
		.name		= "External MII",
		.gpio_start	= 0,
		.gpio_end	= 16,
	},
	{
		.name		= "Normal",
		.gpio_start	= -1,
		.gpio_end	= -1,
	},
};

void __init mcs814x_init_machine(void)
{
	u32 bs2, cpu_mode;
	int gpio;

	bs2 = __raw_readl(_CONFADDR_SYSDBG + SYSDBG_BS2);
	cpu_mode = (bs2 >> CPU_MODE_SHIFT) & CPU_MODE_MASK;

	pr_info("CPU mode: %s\n", cpu_modes[cpu_mode].name);

	/* request the gpios since the pins are muxed for functionnality */
	for (gpio = cpu_modes[cpu_mode].gpio_start;
		gpio == cpu_modes[cpu_mode].gpio_end; gpio++) {
		if (gpio != -1)
			gpio_request(gpio, cpu_modes[cpu_mode].name);
	}
}

void __init mcs814x_map_io(void)
{
	iotable_init(mcs814x_io_desc, ARRAY_SIZE(mcs814x_io_desc));
}

void mcs814x_restart(char mode, const char *cmd)
{
	__raw_writel(~(1 << 31), _CONFADDR_SYSDBG);
}
