/******************************************************************************
**
** FILE NAME    : dwc_otg_ifx.h
** PROJECT      : Twinpass/Danube
** MODULES      : DWC OTG USB
**
** DATE         : 12 April 2007
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
** $Date          $Author         $Comment
** 12 April 2007   Sung Winder     Initiate Version
*******************************************************************************/
#if !defined(__DWC_OTG_IFX_H__)
#define __DWC_OTG_IFX_H__

#include <linux/irq.h>
#include <irq.h>

// 20070316, winder added.
#ifndef SZ_256K
#define SZ_256K                         0x00040000
#endif

extern void dwc_otg_power_on (void);

/* FIXME: The current Linux-2.6 do not have these header files, but anyway, we need these. */
// #include <asm/danube/danube.h>
// #include <asm/ifx/irq.h>

/* winder, I used the Danube parameter as default. *
 * We could change this through module param.      */
#define IFX_USB_IOMEM_BASE 0x1e101000
#define IFX_USB_IOMEM_SIZE SZ_256K
#define IFX_USB_IRQ LTQ_USB_INT

/**
 * This function is called to set correct clock gating and power.
 * For Twinpass/Danube board.
 */
#ifndef DANUBE_RCU_BASE_ADDR
#define DANUBE_RCU_BASE_ADDR            (0xBF203000)
#endif

#ifndef DANUBE_CGU
#define DANUBE_CGU                          (0xBF103000)
#endif
#ifndef DANUBE_CGU_IFCCR
/***CGU Interface Clock Control Register***/
#define DANUBE_CGU_IFCCR                        ((volatile u32*)(DANUBE_CGU+ 0x0018))
#endif

#ifndef DANUBE_PMU
#define DANUBE_PMU                              (KSEG1+0x1F102000)
#endif
#ifndef DANUBE_PMU_PWDCR
/* PMU Power down Control Register */
#define DANUBE_PMU_PWDCR                        ((volatile u32*)(DANUBE_PMU+0x001C))
#endif


#define DANUBE_RCU_UBSCFG  ((volatile u32*)(DANUBE_RCU_BASE_ADDR + 0x18))
#define DANUBE_USBCFG_HDSEL_BIT    11	// 0:host, 1:device
#define DANUBE_USBCFG_HOST_END_BIT 10	// 0:little_end, 1:big_end
#define DANUBE_USBCFG_SLV_END_BIT  9	// 0:little_end, 1:big_end

extern void ltq_mask_and_ack_irq(struct irq_data *d);

static void inline mask_and_ack_ifx_irq(int x)
{
	struct irq_data d;
	d.irq = x;
	ltq_mask_and_ack_irq(&d);
}
#endif //__DWC_OTG_IFX_H__
