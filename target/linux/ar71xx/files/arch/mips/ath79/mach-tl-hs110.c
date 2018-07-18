/*
 *  TP-LINK HS110 board support
 *
 *  Copyright (C) 2017 Arvid E. Picciani <aep@exys.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/gpio.h>
#include <linux/printk.h>

#include <asm/mach-ath79/ath79.h>
#include <asm/mach-ath79/ar71xx_regs.h>

#include "common.h"
#include "dev-eth.h"
#include "dev-m25p80.h"
#include "dev-usb.h"
#include "dev-wmac.h"
#include "machtypes.h"

#define MAC0_OFFSET		0x0000
#define MAC1_OFFSET		0x0006

static void __init hs110_setup(void)
{
	u8 *art = (u8 *) KSEG1ADDR(0x1f000000);

	ath79_register_m25p80(NULL);
	ath79_init_mac(ath79_eth1_data.mac_addr,
			art + MAC0_OFFSET, 0);
	ath79_init_mac(ath79_eth0_data.mac_addr,
			art + MAC1_OFFSET, 0);

	ath79_register_mdio(0, 0x0);

	ath79_register_eth(1);
	ath79_register_eth(0);

	art += 0x3f1000;

	ath79_register_wmac(art, NULL);
	ath79_register_usb();
}

MIPS_MACHINE(ATH79_MACH_TL_HS110, "TL-HS110", "TP-LINK HS110",
	     hs110_setup);
