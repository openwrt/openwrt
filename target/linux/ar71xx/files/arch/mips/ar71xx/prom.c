/*
 *  Atheros AR71xx SoC specific prom routines
 *
 *  Copyright (C) 2008-2009 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>

#include <asm/bootinfo.h>
#include <asm/addrspace.h>
#include <asm/fw/myloader/myloader.h>

#include <asm/mach-ar71xx/ar71xx.h>

#include "devices.h"

struct board_rec {
	char			*name;
	enum ar71xx_mach_type	mach_type;
};

static int ar71xx_prom_argc __initdata;
static char **ar71xx_prom_argv __initdata;
static char **ar71xx_prom_envp __initdata;

static struct board_rec boards[] __initdata = {
	{
		.name		= "411",
		.mach_type	= AR71XX_MACH_RB_411,
	}, {
		.name		= "433",
		.mach_type	= AR71XX_MACH_RB_433,
	}, {
		.name		= "433U",
		.mach_type	= AR71XX_MACH_RB_433U,
	}, {
		.name		= "450",
		.mach_type	= AR71XX_MACH_RB_450,
	}, {
		.name		= "450G",
		.mach_type	= AR71XX_MACH_RB_450G,
	}, {
		.name		= "493",
		.mach_type	= AR71XX_MACH_RB_493,
	}, {
		.name		= "AP81",
		.mach_type	= AR71XX_MACH_AP81,
	}, {
		.name		= "AP83",
		.mach_type	= AR71XX_MACH_AP83,
	}, {
		.name		= "AW-NR580",
		.mach_type	= AR71XX_MACH_AW_NR580,
	}, {
		.name		= "TEW-632BRP",
		.mach_type	= AR71XX_MACH_TEW_632BRP,
	}, {
		.name		= "TL-WR741ND",
		.mach_type	= AR71XX_MACH_TL_WR741ND,
	}, {
		.name		= "TL-WR941ND",
		.mach_type	= AR71XX_MACH_TL_WR941ND,
	}, {
		.name		= "UBNT-RS",
		.mach_type	= AR71XX_MACH_UBNT_RS,
	}, {
		.name		= "UBNT-RSPRO",
		.mach_type	= AR71XX_MACH_UBNT_RSPRO,
	}, {
		.name		= "Ubiquiti AR71xx-based board",
		.mach_type	= AR71XX_MACH_UBNT_RS,
	}, {
		.name		= "UBNT-LS-SR71",
		.mach_type	= AR71XX_MACH_UBNT_LSSR71,
	}, {
		.name		= "UBNT-LSX",
		.mach_type	= AR71XX_MACH_UBNT_LSX,
	}, {
		.name		= "WNR2000",
		.mach_type	= AR71XX_MACH_WNR2000,
	}, {
		.name		= "WRT160NL",
		.mach_type	= AR71XX_MACH_WRT160NL,
	}, {
		.name		= "WRT400N",
		.mach_type	= AR71XX_MACH_WRT400N,
	}, {
		.name		= "PB42",
		.mach_type	= AR71XX_MACH_PB42,
	}, {
		.name		= "PB44",
		.mach_type	= AR71XX_MACH_PB44,
	}, {
		.name		= "MZK-W300NH",
		.mach_type	= AR71XX_MACH_MZK_W300NH,
	}, {
		.name		= "MZK-W04NU",
		.mach_type	= AR71XX_MACH_MZK_W04NU,
	}
};

static inline int is_valid_ram_addr(void *addr)
{
	if (((u32) addr > KSEG0) &&
	    ((u32) addr < (KSEG0 + AR71XX_MEM_SIZE_MAX)))
		return 1;

	if (((u32) addr > KSEG1) &&
	    ((u32) addr < (KSEG1 + AR71XX_MEM_SIZE_MAX)))
		return 1;

	return 0;
}

static __init char *ar71xx_prom_getargv(const char *name)
{
	int len = strlen(name);
	int i;

	if (!is_valid_ram_addr(ar71xx_prom_argv))
		return NULL;

	for (i = 0; i < ar71xx_prom_argc; i++) {
		char *argv = ar71xx_prom_argv[i];

		if (!is_valid_ram_addr(argv))
			continue;

		if (strncmp(name, argv, len) == 0 && (argv)[len] == '=')
			return argv + len + 1;
	}

	return NULL;
}

static __init char *ar71xx_prom_getenv(const char *envname)
{
	int len = strlen(envname);
	char **env;

	if (!is_valid_ram_addr(ar71xx_prom_envp))
		return NULL;

	for (env = ar71xx_prom_envp; is_valid_ram_addr(*env); env++) {
		if (strncmp(envname, *env, len) == 0 && (*env)[len] == '=')
			return *env + len + 1;

		/* RedBoot env comes in pointer pairs - key, value */
		if (strncmp(envname, *env, len) == 0 && (*env)[len] == 0)
			if (is_valid_ram_addr(*(++env)))
				return *env;
	}

	return NULL;
}

static __init unsigned long find_board_byname(char *name)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(boards); i++)
		if (strcmp(name, boards[i].name) == 0)
			return boards[i].mach_type;

	return AR71XX_MACH_GENERIC;
}

static int ar71xx_prom_init_myloader(void)
{
	struct myloader_info *mylo;

	mylo = myloader_get_info();
	if (!mylo)
		return 0;

	switch (mylo->did) {
	case DEVID_COMPEX_WP543:
		ar71xx_mach = AR71XX_MACH_WP543;
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

	p = ar71xx_prom_getargv("board");
	if (!p)
		p = ar71xx_prom_getenv("board");
	if (p)
		ar71xx_mach = find_board_byname(p);

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

	ar71xx_mach = AR71XX_MACH_GENERIC;

	if (ar71xx_prom_init_myloader())
		return;

	ar71xx_prom_init_generic();
}

void __init prom_free_prom_memory(void)
{
	/* We do not have to prom memory to free */
}
