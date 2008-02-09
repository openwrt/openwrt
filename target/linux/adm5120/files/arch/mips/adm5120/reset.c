/*
 *  $Id$
 *
 *  ADM5120 specific reset routines
 *
 *  Copyright (C) 2007 OpenWrt.org
 *  Copyright (C) 2007 Gabor Juhos <juhosg at openwrt.org>
 *
 *  This code was based on the ADM5120 specific port of the Linux 2.6.10 kernel
 *  done by Jeroen Vreeken
 *	Copyright (C) 2005 Jeroen Vreeken (pe1rxq@amsat.org)
 *
 *  Jeroen's code was based on the Linux 2.4.xx source codes found in various
 *  tarballs released by Edimax for it's ADM5120 based devices.
 *	Copyright (C) ADMtek Incorporated
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 */

#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/irq.h>
#include <linux/io.h>

#include <asm/addrspace.h>

#include <adm5120_info.h>
#include <adm5120_defs.h>
#include <adm5120_switch.h>

void (*adm5120_board_reset)(void);

void adm5120_restart(char *command)
{
	/* TODO: stop switch before reset */

	if (adm5120_board_reset)
		adm5120_board_reset();

	SW_WRITE_REG(SWITCH_REG_SOFT_RESET, 1);
}

void adm5120_halt(void)
{
	local_irq_disable();

	while (1) {
		if (cpu_wait)
			cpu_wait();
	};
}
