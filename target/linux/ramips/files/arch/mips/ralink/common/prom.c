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

static struct board_rec boards[] __initdata = {
	{
		.name		= "RT-N15",
		.mach_type	= RAMIPS_MACH_RT_N15,
	}, {
		.name		= "DIR-300-revB",
		.mach_type	= RAMIPS_MACH_DIR_300_REVB,
	}, {
		.name		= "V22RW-2X2",
		.mach_type	= RAMIPS_MACH_V22RW_2X2,
	}, {
		.name		= "WHR-G300N",
		.mach_type	= RAMIPS_MACH_WHR_G300N,
	}, {
		.name		= "FONERA20N",
		.mach_type	= RAMIPS_MACH_FONERA20N,
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

static void __init prom_append_cmdline(const char *name,
				       const char *value)
{
	char buf[CL_SIZE];

	snprintf(buf, sizeof(buf), " %s=%s", name, value);
	strlcat(arcs_cmdline, buf, sizeof(arcs_cmdline));
}

#ifdef CONFIG_IMAGE_CMDLINE_HACK
extern char __image_cmdline[];

static int __init use_image_cmdline(void)
{
	char *p = __image_cmdline;
	int replace = 0;

	if (*p == '-') {
		replace = 1;
		p++;
	}

	if (*p == '\0')
		return 0;

	if (replace) {
		strlcpy(arcs_cmdline, p, sizeof(arcs_cmdline));
	} else {
		strlcat(arcs_cmdline, " ", sizeof(arcs_cmdline));
		strlcat(arcs_cmdline, p, sizeof(arcs_cmdline));
	}

	return 1;
}
#else
static int inline use_image_cmdline(void) { return 0; }
#endif

static __init void prom_init_cmdline(int argc, char **argv)
{
	int i;

	if (use_image_cmdline())
		return;

	if (!argv) {
		printk(KERN_DEBUG "argv=%p is invalid, skipping\n",
		       argv);
		return;
	}

	for (i = 0; i < argc; i++) {
		char *p = to_ram_addr(argv[i]);

		if (!p) {
			printk(KERN_DEBUG
			       "argv[%d]=%p is invalid, skipping\n",
			       i, argv[i]);
			continue;
		}

		printk(KERN_DEBUG "argv[%d]: %s\n", i, p);
		strlcat(arcs_cmdline, " ", sizeof(arcs_cmdline));
		strlcat(arcs_cmdline, p, sizeof(arcs_cmdline));
	}
}

static __init char *prom_append_env(char **env, const char *envname)
{
#define PROM_MAX_ENVS	256
	int len = strlen(envname);
	int i;

	if (!env) {
		printk(KERN_DEBUG "env=%p is not in RAM, skipping\n",
		       env);
		return NULL;
	}

	for (i = 0; i < PROM_MAX_ENVS; i++) {
		char *p = to_ram_addr(env[i]);

		if (!p)
			break;

		printk(KERN_DEBUG "env[%d]: %s\n", i, p);
		if (strncmp(envname, p, len) == 0 && p[len] == '=')
			prom_append_cmdline(envname, p + len + 1);
	}

	return NULL;
#undef PROM_MAX_ENVS
}

static __init int ramips_board_setup(char *name)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(boards); i++)
		if (strcmp(name, boards[i].name) == 0) {
			ramips_mach = boards[i].mach_type;
			break;
		}

	return 1;
}
__setup("board=", ramips_board_setup);

void __init prom_init(void)
{
	int argc;
	char **envp;
	char **argv;

	printk(KERN_DEBUG
	       "prom: fw_arg0=%08x, fw_arg1=%08x, fw_arg2=%08x, fw_arg3=%08x\n",
	       (unsigned int)fw_arg0, (unsigned int)fw_arg1,
	       (unsigned int)fw_arg2, (unsigned int)fw_arg3);

	argc = fw_arg0;
	argv = to_ram_addr((void *)fw_arg1);
	prom_init_cmdline(argc, argv);

	envp = to_ram_addr((void *)fw_arg2);
	prom_append_env(envp, "board");
	prom_append_env(envp, "ethaddr");
}

void __init prom_free_prom_memory(void)
{
	/* We do not have to prom memory to free */
}
