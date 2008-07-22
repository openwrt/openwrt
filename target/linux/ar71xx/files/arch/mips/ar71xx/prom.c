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
#include <asm/mach-ar71xx/platform.h>

struct board_rec {
	char		*name;
	unsigned long	mach_type;
};

static int ar71xx_prom_argc __initdata;
static char **ar71xx_prom_argv __initdata;
static char **ar71xx_prom_envp __initdata;

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

static __init void routerboot_printargs(void)
{
	int i;

	for (i = 0; i < ar71xx_prom_argc; i++)
		printk(KERN_DEBUG "prom: routerboot envp[%d]: %s\n",
				i, ar71xx_prom_argv[i]);
}

static __init char *routerboot_getenv(const char *envname)
{
	int len = strlen(envname);
	int i;

	for (i = 0; i < ar71xx_prom_argc; i++) {
		char *env = ar71xx_prom_argv[i];
		if (strncmp(envname, env, len) == 0 && (env)[len] == '=')
			return env + len + 1;
	}

	return NULL;
}

static __init char *redboot_getenv(const char *envname)
{
	int len = strlen(envname);
	char **env;

	for (env = ar71xx_prom_envp; *env != NULL; env++)
		if (strncmp(envname, *env, len) == 0 && (*env)[len] == '=')
			return *env + len + 1;

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
	char *board = NULL;
	char *mac = NULL;

	printk(KERN_DEBUG "prom: fw_arg0=%08x, fw_arg1=%08x, "
			"fw_arg2=%08x, fw_arg3=%08x\n",
			(unsigned int)fw_arg0, (unsigned int)fw_arg1,
			(unsigned int)fw_arg2, (unsigned int)fw_arg3);

	if ((fw_arg0 == 7) && (fw_arg2 == 0) && (fw_arg3 == 0)) {
		 /* assume RouterBOOT */
		ar71xx_prom_argc = fw_arg0;
		ar71xx_prom_argv = (char **)fw_arg1;
		routerboot_printargs();
		board = routerboot_getenv("board");
		mac = routerboot_getenv("kmac");
	} else {
		/* assume Redboot */
		ar71xx_prom_argc = fw_arg0;
		ar71xx_prom_argv = (char **)fw_arg1;
		ar71xx_prom_envp = (char **)fw_arg2;
		mac = redboot_getenv("ethaddr");
	}

	if (board)
		mips_machtype = find_board_byname(board);
	else
		mips_machtype = MACH_AR71XX_GENERIC;

	if (mac)
		ar71xx_set_mac_base(mac);

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
