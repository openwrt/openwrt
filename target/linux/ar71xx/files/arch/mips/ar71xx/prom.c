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
#include <asm/fw/myloader/myloader.h>

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
	}, {
		.name		= "493",
		.mach_type	= MACH_AR71XX_RB_493,
	}
};

static __init char *ar71xx_prom_getargv(const char *name)
{
	int len = strlen(name);
	int i;

	if (!ar71xx_prom_argv)
		return NULL;

	for (i = 0; i < ar71xx_prom_argc; i++) {
		char *argv = ar71xx_prom_argv[i];
		if (strncmp(name, argv, len) == 0 && (argv)[len] == '=')
			return argv + len + 1;
	}

	return NULL;
}

static __init char *ar71xx_prom_getenv(const char *envname)
{
	int len = strlen(envname);
	char **env;

	if (!ar71xx_prom_envp)
		return NULL;

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

static int ar71xx_prom_init_myloader(void)
{
	struct myloader_info *mylo;

	mylo = myloader_get_info();
	if (!mylo)
		return 0;

	switch (mylo->did) {
	case DEVID_COMPEX_WP543:
		mips_machtype = MACH_AR71XX_WP543;
		break;
	default:
		printk(KERN_WARNING "prom: unknown device id: %x\n",
				mylo->did);
	}
	ar71xx_set_mac_base(mylo->macs[0]);

	return 1;
}

static void ar71xx_prom_init_generic(void)
{
	char *p;

	ar71xx_prom_argc = fw_arg0;
	ar71xx_prom_argv = (char **)fw_arg1;
	ar71xx_prom_envp = (char **)fw_arg2;

	p = ar71xx_prom_getenv("board");
	if (!p)
		p = ar71xx_prom_getargv("board");
	if (p)
		mips_machtype = find_board_byname(p);

	p = ar71xx_prom_getenv("ethaddr");
	if (!p)
		p = ar71xx_prom_getargv("kmac");
	if (p)
		ar71xx_parse_mac_addr(p);
}

void __init prom_init(void)
{
	printk(KERN_DEBUG "prom: fw_arg0=%08x, fw_arg1=%08x, "
			"fw_arg2=%08x, fw_arg3=%08x\n",
			(unsigned int)fw_arg0, (unsigned int)fw_arg1,
			(unsigned int)fw_arg2, (unsigned int)fw_arg3);

	mips_machtype = MACH_AR71XX_GENERIC;

	if (ar71xx_prom_init_myloader())
		return;

	ar71xx_prom_init_generic();
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
