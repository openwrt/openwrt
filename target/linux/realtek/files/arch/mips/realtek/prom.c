/*
 *  Realtek RLX based SoC specific prom routines
 *
 *  Copyright (C) 2017 Weijie Gao <hackpascal@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/string.h>
#include <linux/initrd.h>

#include <asm/bootinfo.h>
#include <asm/addrspace.h>
#include <asm/fw/fw.h>

#include "common.h"

void __init prom_init(void)
{
	fw_init_cmdline();
}

void __init prom_free_prom_memory(void)
{

}
