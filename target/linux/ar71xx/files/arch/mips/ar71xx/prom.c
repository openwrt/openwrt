/*
 *  Atheros AR71xx SoC specific prom routines
 *
 *  Copyright (C) 2008 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/serial_reg.h>

#include <asm/bootinfo.h>
#include <asm/addrspace.h>

#include <asm/mach-ar71xx/ar71xx.h>

struct board_rec {
	char		*name;
	unsigned long	mach_type;
};

static int prom_argc __initdata;
static char **prom_argv __initdata;
static char **prom_envp __initdata;

static struct board_rec boards[] __initdata = {
	{
		.name		= "411",
		.mach_type	= MACH_AR71XX_RB_411,
	}, {
		.name		= "433",
		.mach_type	= MACH_AR71XX_RB_433,
	}, {
		.name		= "450",
		.mach_type	= MACH_AR71XX_RB_450,
	}
};

char *(*prom_getenv)(const char *envname) __initdata;

static __init char *dummy_getenv(const char *envname)
{
	return NULL;
}

static void __init routerboot_printargs(void)
{
	int i;

	for (i = 0; i < prom_argc; i++)
		printk(KERN_DEBUG "prom: routerboot envp[%d]: %s\n",
				i, prom_envp[i]);
}

static __init char *routerboot_getenv(const char *envname)
{
	char **env;
	int i = strlen(envname);

	for (env = prom_envp; *env != NULL; env++)
		if (strncmp(envname, *env, i) == 0 && (*env)[i] == '=')
			return *env + i + 1;

	return NULL;
}

static __init unsigned long find_board_byname(char *name)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(boards); i++)
		if (strcmp(name, boards[i].name) == 0)
			return boards[i].mach_type;

	return MACH_AR71XX_GENERIC;
}

void __init prom_init(void)
{
	char *board;

	printk(KERN_DEBUG "prom: fw_arg0=%08x, fw_arg1=%08x, "
			"fw_arg2=%08x, fw_arg3=%08x\n",
			(unsigned int)fw_arg0, (unsigned int)fw_arg1,
			(unsigned int)fw_arg2, (unsigned int)fw_arg3);

	prom_getenv = dummy_getenv;

	if ((fw_arg0 == 7) && (fw_arg2 == 0)) {
		prom_argc = fw_arg0;
		prom_envp = (char **)fw_arg1;
		prom_getenv = routerboot_getenv;
		routerboot_printargs();
	}

	board = prom_getenv("board");
	if (board)
		mips_machtype = find_board_byname(board);
	else
		mips_machtype = MACH_AR71XX_GENERIC;

	ar71xx_print_cmdline();
}

void __init prom_free_prom_memory(void)
{
	/* We do not have to prom memory to free */
}

#define UART_READ(r) \
	__raw_readl((void __iomem *)(KSEG1ADDR(AR71XX_UART_BASE) + 4 * (r)))

#define UART_WRITE(r, v) \
	__raw_writel((v), (void __iomem *)(KSEG1ADDR(AR71XX_UART_BASE) + 4*(r)))

void prom_putchar(unsigned char ch)
{
	while (((UART_READ(UART_LSR)) & UART_LSR_THRE) == 0);
	UART_WRITE(UART_TX, ch);
	while (((UART_READ(UART_LSR)) & UART_LSR_THRE) == 0);
}
