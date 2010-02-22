/*
 * drivers/usb/musb/ubi32_usb.c
 *   Ubicom32 usb controller driver.
 *
 * (C) Copyright 2009, Ubicom, Inc.
 * Copyright (C) 2005-2006 by Texas Instruments
 *
 * Derived from the Texas Instruments Inventra Controller Driver for Linux.
 *
 * This file is part of the Ubicom32 Linux Kernel Port.
 *
 * The Ubicom32 Linux Kernel Port is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 *
 * The Ubicom32 Linux Kernel Port is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Ubicom32 Linux Kernel Port.  If not,
 * see <http://www.gnu.org/licenses/>.
 *
 * Ubicom32 implementation derived from (with many thanks):
 *   arch/m68knommu
 *   arch/blackfin
 *   arch/parisc
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/clk.h>
#include <linux/io.h>

#include <asm/io.h>
#include <asm/ip5000.h>
#include "musb_core.h"

void musb_platform_enable(struct musb *musb)
{
}
void musb_platform_disable(struct musb *musb)
{
}

int musb_platform_set_mode(struct musb *musb, u8 musb_mode) {
	return 0;
}

static void ip5k_usb_hcd_vbus_power(struct musb *musb, int is_on, int sleeping)
{
}

static void ip5k_usb_hcd_set_vbus(struct musb *musb, int is_on)
{
	u8		devctl;
	/* HDRC controls CPEN, but beware current surges during device
	 * connect.  They can trigger transient overcurrent conditions
	 * that must be ignored.
	 */

	devctl = musb_readb(musb->mregs, MUSB_DEVCTL);

	if (is_on) {
		musb->is_active = 1;
		musb->xceiv.default_a = 1;
		musb->xceiv.state = OTG_STATE_A_WAIT_VRISE;
		devctl |= MUSB_DEVCTL_SESSION;

		MUSB_HST_MODE(musb);
	} else {
		musb->is_active = 0;

		/* NOTE:  we're skipping A_WAIT_VFALL -> A_IDLE and
		 * jumping right to B_IDLE...
		 */

		musb->xceiv.default_a = 0;
		musb->xceiv.state = OTG_STATE_B_IDLE;
		devctl &= ~MUSB_DEVCTL_SESSION;

		MUSB_DEV_MODE(musb);
	}
	musb_writeb(musb->mregs, MUSB_DEVCTL, devctl);

	DBG(1, "VBUS %s, devctl %02x "
		/* otg %3x conf %08x prcm %08x */ "\n",
		otg_state_string(musb),
		musb_readb(musb->mregs, MUSB_DEVCTL));
}
static int ip5k_usb_hcd_set_power(struct otg_transceiver *x, unsigned mA)
{
	return 0;
}

static int musb_platform_resume(struct musb *musb);

int __init musb_platform_init(struct musb *musb)
{

#ifdef CONFIG_UBICOM32_V4
	u32_t chip_id;
	asm volatile (
		      "move.4	%0, CHIP_ID	\n\t"
		      : "=r" (chip_id)
	);
	if (chip_id == 0x30001) {
		*((u32_t *)(GENERAL_CFG_BASE + GEN_USB_PHY_TEST)) &= ~(1 << 30);
		udelay(1);
		*((u32_t *)(GENERAL_CFG_BASE + GEN_USB_PHY_TEST)) &= ~(1 << 31);
	} else {
		*((u32_t *)(GENERAL_CFG_BASE + GEN_USB_PHY_TEST)) &= ~(1 << 17);
		udelay(1);
		*((u32_t *)(GENERAL_CFG_BASE + GEN_USB_PHY_TEST)) &= ~(1 << 14);
	}
#endif

	 *((u32_t *)(GENERAL_CFG_BASE + GEN_USB_PHY_CFG)) |= ((1 << 14) | (1 <<15));

	/* The i-clk is AUTO gated. Hence there is no need
	 * to disable it until the driver is shutdown */

	clk_enable(musb->clock);
	musb_platform_resume(musb);

	ip5k_usb_hcd_vbus_power(musb, musb->board_mode == MUSB_HOST, 1);

	if (is_host_enabled(musb))
		musb->board_set_vbus = ip5k_usb_hcd_set_vbus;
	if (is_peripheral_enabled(musb))
		musb->xceiv.set_power = ip5k_usb_hcd_set_power;

	return 0;
}


int musb_platform_suspend(struct musb *musb)
{
	return 0;
}
int musb_platform_resume(struct musb *musb)
{
	return 0;
}

int musb_platform_exit(struct musb *musb)
{
	ip5k_usb_hcd_vbus_power(musb, 0 /*off*/, 1);
	musb_platform_suspend(musb);
	return 0;
}
