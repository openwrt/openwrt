/*
 * HND SiliconBackplane Gigabit Ethernet core registers
 *
 * Copyright 2007, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id: sbgige.h,v 1.5 2007/06/01 05:58:20 michael Exp $
 */

#ifndef	_sbgige_h_
#define	_sbgige_h_

#include <typedefs.h>
#include <sbconfig.h>
#include <pcicfg.h>

/* cpp contortions to concatenate w/arg prescan */
#ifndef PAD
#define	_PADLINE(line)	pad ## line
#define	_XSTR(line)	_PADLINE(line)
#define	PAD		_XSTR(__LINE__)
#endif	/* PAD */

/* PCI to OCP shim registers */
typedef volatile struct {
	uint32 FlushStatusControl;
	uint32 FlushReadAddr;
	uint32 FlushTimeoutCntr;
	uint32 BarrierReg;
	uint32 MaocpSIControl;
	uint32 SiocpMaControl;
	uint8 PAD[0x02E8];
} sbgige_pcishim_t;

/* SB core registers */
typedef volatile struct {
	/* PCI I/O Read/Write registers */
	uint8 pciio[0x0400];

	/* Reserved */
	uint8 reserved[0x0400];

	/* PCI configuration registers */
	pci_config_regs pcicfg;
	uint8 PAD[0x0300];

	/* PCI to OCP shim registers */
	sbgige_pcishim_t pcishim;

	/* Sonics SiliconBackplane registers */
	sbconfig_t sbconfig;
} sbgige_t;

#endif	/* _sbgige_h_ */
