/*
 *  Ralink SoC specific prom routines
 *
 *  Copyright (C) 2009 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/string.h>

#include <asm/bootinfo.h>
#include <asm/addrspace.h>

#include <asm/mach-ralink/common.h>
#include <asm/mach-ralink/machine.h>
#include <ralink_soc.h>

struct board_rec {
	char			*name;
	enum ramips_mach_type	mach_type;
};

static int ramips_prom_argc __initdata;
static char **ramips_prom_argv __initdata;
static char **ramips_prom_envp __initdata;

static struct board_rec boards[] __initdata = {
	{
		.name		= "RT-N15",
		.mach_type	= RAMIPS_MACH_RT_N15,
	}, {
		.name		= "V22RW-2X2",
		.mach_type	= RAMIPS_MACH_V22RW_2X2,
	}, {
		.name		= "WHR-G300N",
		.mach_type	= RAMIPS_MACH_WHR_G300N,
	}
};

static inline void *to_ram_addr(void *addr)
{
	u32 base;

	base = KSEG0ADDR(RALINK_SOC_SDRAM_BASE);
	if (((u32) addr > base) &&
	    ((u32) addr < (base + RALINK_SOC_MEM_SIZE_MAX)))
		return addr;

	base = KSEG1ADDR(RALINK_SOC_SDRAM_BASE);
	if (((u32) addr > base) &&
	    ((u32) addr < (base + RALINK_SOC_MEM_SIZE_MAX)))
		return addr;

	/* some U-Boot variants uses physical addresses */
	base = RALINK_SOC_SDRAM_BASE;
	if (((u32) addr > base) &&
	    ((u32) addr < (base + RALINK_SOC_MEM_SIZE_MAX)))
		return (void *)KSEG0ADDR(addr);

	return NULL;
}

static __init char *ramips_prom_getargv(const char *name)
{
	int len = strlen(name);
	int i;

	if (!ramips_prom_argv) {
		printk(KERN_DEBUG "argv=%p is invalid, skipping\n",
		       ramips_prom_argv);
		return NULL;
	}

	for (i = 0; i < ramips_prom_argc; i++) {
		char *argv = to_ram_addr(ramips_prom_argv[i]);

		if (!argv) {
			printk(KERN_DEBUG
			       "argv[%d]=%p is invalid, skipping\n",
			       i, ramips_prom_argv[i]);
			continue;
		}

		printk(KERN_DEBUG "argv[%d]: %s\n", i, argv);
		if (strncmp(name, argv, len) == 0 && (argv)[len] == '=')
			return argv + len + 1;
	}

	return NULL;
}

static __init char *ramips_prom_getenv(const char *envname)
{
#define PROM_MAX_ENVS	256
	int len = strlen(envname);
	char **env;
	int i;

	env = ramips_prom_envp;
	if (!env) {
		printk(KERN_DEBUG "envp=%p is not in RAM, skipping\n",
		       ramips_prom_envp);
		return NULL;
	}

	for (i = 0; i < PROM_MAX_ENVS; i++) {
		char *p = to_ram_addr(env[i]);

		if (!p)
			break;

		printk(KERN_DEBUG "env[%d]: %s\n", i, p);
		if (strncmp(envname, p, len) == 0 && p[len] == '=')
			return p + len + 1;
	}

	return NULL;
#undef PROM_MAX_ENVS
}

static __init void find_board_byname(char *name)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(boards); i++)
		if (strcmp(name, boards[i].name) == 0) {
			ramips_mach = boards[i].mach_type;
			break;
		}
}

void __init prom_init(void)
{
	char *p;

	printk(KERN_DEBUG
	       "prom: fw_arg0=%08x, fw_arg1=%08x, fw_arg2=%08x, fw_arg3=%08x\n",
	       (unsigned int)fw_arg0, (unsigned int)fw_arg1,
	       (unsigned int)fw_arg2, (unsigned int)fw_arg3);

	ramips_prom_argc = fw_arg0;
	ramips_prom_argv = to_ram_addr((void *)fw_arg1);
	ramips_prom_envp = to_ram_addr((void *)fw_arg2);

	p = ramips_prom_getargv("board");
	if (!p)
		p = ramips_prom_getenv("board");
	if (p)
		find_board_byname(p);
}

void __init prom_free_prom_memory(void)
{
	/* We do not have to prom memory to free */
}
