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
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the
 *  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA  02110-1301, USA.
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

void __init adm5120_time_init(void)
{
	mips_hpt_frequency = adm5120_speed / 2;
}

void __init plat_timer_setup(struct irqaction *irq)
{
	clear_c0_status(ST0_BEV);

	/* Install ISR for CPU Counter interrupt */
	setup_irq(ADM5120_IRQ_COUNTER, irq);
}
