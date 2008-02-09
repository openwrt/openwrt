/*
 *  $Id$
 *
 *  ADM5120 specific hooks for MIPS CPU Counter/Compare timer
 *
 *  Copyright (C) 2007 OpenWrt.org
 *  Copyright (C) 2007 Gabor Juhos <juhosg at openwrt.org>
 *
 *  This file was based on: arch/mips/gt64120/wrppmc/time.c
 *	Original author: Mark.Zhan
 *	Copyright (C) 1996, 1997, 2004 by Ralf Baechle
 *	Copyright (C) 2006, Wind River System Inc.
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 */

#include <linux/types.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/timex.h>

#include <asm/irq.h>
#include <asm/cpu.h>
#include <asm/time.h>

#include <adm5120_info.h>
#include <adm5120_irq.h>

void __init plat_time_init(void)
{
	mips_hpt_frequency = adm5120_speed / 2;
}
