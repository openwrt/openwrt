// SPDX-License-Identifier: GPL-2.0-only
/*
 * EN75xx setup code
 *
 * Copyright (C) 2025 Caleb James DeLisle <cjd@cjdns.fr>
 */

#include <linux/init.h>
#include <linux/clk-provider.h>
#include <linux/clk.h>
#include <linux/of_fdt.h>
#include <linux/irqchip.h>

#include <asm/addrspace.h>
#include <asm/io.h>
#include <asm/bootinfo.h>
#include <asm/time.h>
#include <asm/prom.h>
#include <asm/smp-ops.h>
#include <asm/reboot.h>

#define CR_AHB_RSTCR	((void __iomem *)CKSEG1ADDR(0x1fb00040))
#define RESET		BIT(31)

#define	UART_BASE		0x1fbf0003
#define UART_REG_SHIFT		2

static void hw_reset(char *command)
{
	iowrite32(RESET, CR_AHB_RSTCR);
}

/* 1. Bring up early printk. */
void __init prom_init(void)
{
	setup_8250_early_printk_port(
		CKSEG1ADDR(UART_BASE),
		UART_REG_SHIFT,
		0
	);

	_machine_restart = hw_reset;
}

/* 2. Parse the DT and find memory */
void __init plat_mem_setup(void)
{
	void *dtb;

	set_io_port_base(KSEG1);

	dtb = get_fdt();
	if (!dtb)
		panic("no dtb found");

	__dt_setup_arch(dtb);

	early_init_dt_scan_memory();
}

/* 3. Overload __weak device_tree_init() */
void __init device_tree_init(void)
{
	unflatten_and_copy_device_tree();

	register_up_smp_ops();
}

const char *get_system_type(void)
{
	return "Generic-EN75xx";
}

/* 4. Initialize the IRQ subsystem */
void __init arch_init_irq(void)
{
	irqchip_init();
}

/* 5. Timers */
void __init plat_time_init(void)
{
	struct device_node *np;
	struct clk *clk;

	of_clk_init(NULL);

	np = of_get_cpu_node(0, NULL);
	if (!np)
		panic("Failed to get CPU node from DT\n");
	clk = of_clk_get(np, 0);
	if (IS_ERR(clk))
		panic("Failed to get CPU clock from DT %ld\n", PTR_ERR(clk));
	pr_info("CPU frequency: %luMHz", clk_get_rate(clk) / 1000000);
	mips_hpt_frequency = clk_get_rate(clk) / 2;
	clk_put(clk);
	of_node_put(np);

	timer_probe();
}
