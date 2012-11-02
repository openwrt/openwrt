/******************************************************************************
**
** FILE NAME    : dwc_otg_ifx.c
** PROJECT      : Twinpass/Danube
** MODULES      : DWC OTG USB
**
** DATE         : 12 Auguest 2007
** AUTHOR       : Sung Winder
** DESCRIPTION  : Platform specific initialization.
** COPYRIGHT    :       Copyright (c) 2007
**                      Infineon Technologies AG
**                      2F, No.2, Li-Hsin Rd., Hsinchu Science Park,
**                      Hsin-chu City, 300 Taiwan.
**
**    This program is free software; you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation; either version 2 of the License, or
**    (at your option) any later version.
**
** HISTORY
** $Date             $Author         $Comment
** 12 Auguest 2007   Sung Winder     Initiate Version
*******************************************************************************/
#include "dwc_otg_ifx.h"

#include <linux/platform_device.h>
#include <linux/kernel.h>
#include <linux/ioport.h>
#include <linux/gpio.h>

#include <asm/io.h>
//#include <asm/mach-ifxmips/ifxmips.h>
#include <lantiq_soc.h>

#define IFXMIPS_GPIO_BASE_ADDR  (0xBE100B00)

#define IFXMIPS_GPIO_P0_OUT     ((u32 *)(IFXMIPS_GPIO_BASE_ADDR + 0x0010))
#define IFXMIPS_GPIO_P1_OUT     ((u32 *)(IFXMIPS_GPIO_BASE_ADDR + 0x0040))
#define IFXMIPS_GPIO_P0_IN      ((u32 *)(IFXMIPS_GPIO_BASE_ADDR + 0x0014))
#define IFXMIPS_GPIO_P1_IN      ((u32 *)(IFXMIPS_GPIO_BASE_ADDR + 0x0044))
#define IFXMIPS_GPIO_P0_DIR     ((u32 *)(IFXMIPS_GPIO_BASE_ADDR + 0x0018))
#define IFXMIPS_GPIO_P1_DIR     ((u32 *)(IFXMIPS_GPIO_BASE_ADDR + 0x0048))
#define IFXMIPS_GPIO_P0_ALTSEL0     ((u32 *)(IFXMIPS_GPIO_BASE_ADDR + 0x001C))
#define IFXMIPS_GPIO_P1_ALTSEL0     ((u32 *)(IFXMIPS_GPIO_BASE_ADDR + 0x004C))
#define IFXMIPS_GPIO_P0_ALTSEL1     ((u32 *)(IFXMIPS_GPIO_BASE_ADDR + 0x0020))
#define IFXMIPS_GPIO_P1_ALTSEL1     ((u32 *)(IFXMIPS_GPIO_BASE_ADDR + 0x0050))
#define IFXMIPS_GPIO_P0_OD      ((u32 *)(IFXMIPS_GPIO_BASE_ADDR + 0x0024))
#define IFXMIPS_GPIO_P1_OD      ((u32 *)(IFXMIPS_GPIO_BASE_ADDR + 0x0054))
#define IFXMIPS_GPIO_P0_STOFF       ((u32 *)(IFXMIPS_GPIO_BASE_ADDR + 0x0028))
#define IFXMIPS_GPIO_P1_STOFF       ((u32 *)(IFXMIPS_GPIO_BASE_ADDR + 0x0058))
#define IFXMIPS_GPIO_P0_PUDSEL      ((u32 *)(IFXMIPS_GPIO_BASE_ADDR + 0x002C))
#define IFXMIPS_GPIO_P1_PUDSEL      ((u32 *)(IFXMIPS_GPIO_BASE_ADDR + 0x005C))
#define IFXMIPS_GPIO_P0_PUDEN       ((u32 *)(IFXMIPS_GPIO_BASE_ADDR + 0x0030))
#define IFXMIPS_GPIO_P1_PUDEN       ((u32 *)(IFXMIPS_GPIO_BASE_ADDR + 0x0060))


#define writel ltq_w32
#define readl ltq_r32
void dwc_otg_power_on (void)
{
	// clear power
	writel(readl(DANUBE_PMU_PWDCR) | 0x41, DANUBE_PMU_PWDCR);
	// set clock gating
	if (ltq_is_ase())
		writel(readl(DANUBE_CGU_IFCCR) & ~0x20, DANUBE_CGU_IFCCR);
	else
		writel(readl(DANUBE_CGU_IFCCR) | 0x30, DANUBE_CGU_IFCCR);
	// set power
	writel(readl(DANUBE_PMU_PWDCR) & ~0x1, DANUBE_PMU_PWDCR);
	writel(readl(DANUBE_PMU_PWDCR) & ~0x40, DANUBE_PMU_PWDCR);
	writel(readl(DANUBE_PMU_PWDCR) & ~0x8000, DANUBE_PMU_PWDCR);

#if 1//defined (DWC_HOST_ONLY)
	// make the hardware be a host controller (default)
	//clear_bit (DANUBE_USBCFG_HDSEL_BIT, (volatile unsigned long *)DANUBE_RCU_UBSCFG);
	writel(readl(DANUBE_RCU_UBSCFG) & ~(1<<DANUBE_USBCFG_HDSEL_BIT), DANUBE_RCU_UBSCFG);

	//#elif defined (DWC_DEVICE_ONLY)
	/* set the controller to the device mode */
	//    set_bit (DANUBE_USBCFG_HDSEL_BIT, (volatile unsigned long *)DANUBE_RCU_UBSCFG);
#else
#error  "For Danube/Twinpass, it should be HOST or Device Only."
#endif

	// set the HC's byte-order to big-endian
	//set_bit (DANUBE_USBCFG_HOST_END_BIT, (volatile unsigned long *)DANUBE_RCU_UBSCFG);
	writel(readl(DANUBE_RCU_UBSCFG) | (1<<DANUBE_USBCFG_HOST_END_BIT), DANUBE_RCU_UBSCFG);
	//clear_bit (DANUBE_USBCFG_SLV_END_BIT, (volatile unsigned long *)DANUBE_RCU_UBSCFG);
	writel(readl(DANUBE_RCU_UBSCFG) & ~(1<<DANUBE_USBCFG_SLV_END_BIT), DANUBE_RCU_UBSCFG);
	//writel(0x400, DANUBE_RCU_UBSCFG);

	// PHY configurations.
	writel (0x14014, (volatile unsigned long *)0xbe10103c);
}

int ifx_usb_hc_init(unsigned long base_addr, int irq)
{
	return 0;
}

void ifx_usb_hc_remove(void)
{
}
