/******************************************************************************
 *
 * Copyright(c) 2007 - 2019 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/
#ifndef __DRV_TYPES_PCI_H__
#define __DRV_TYPES_PCI_H__

#ifdef PLATFORM_LINUX
	#include <linux/pci.h>
#endif

#define	INTEL_VENDOR_ID				0x8086
#define	SIS_VENDOR_ID					0x1039
#define	ATI_VENDOR_ID					0x1002
#define	ATI_DEVICE_ID					0x7914
#define	AMD_VENDOR_ID					0x1022

#define PCI_VENDER_ID_REALTEK		0x10ec

enum aspm_mode {
	ASPM_MODE_UND,
	ASPM_MODE_PERF,
	ASPM_MODE_PS,
	ASPM_MODE_DEF,
};

struct pci_priv {
	BOOLEAN		pci_clk_req;

	u8	pciehdr_offset;

	u8	linkctrl_reg;
	u8	pcibridge_linkctrlreg;

	u8	amd_l1_patch;

#ifdef CONFIG_PCI_DYNAMIC_ASPM
	u8	aspm_mode;
#endif
};

typedef struct _RT_ISR_CONTENT {
	union {
		u32			IntArray[2];
		u32			IntReg4Byte;
		u16			IntReg2Byte;
	};
} RT_ISR_CONTENT, *PRT_ISR_CONTENT;

typedef struct pci_data {
#ifdef PLATFORM_LINUX
	struct pci_dev *ppcidev;

	/* PCI MEM map */
	unsigned long	pci_mem_end;	/* shared mem end	*/
	unsigned long	pci_mem_start;	/* shared mem start	*/

	/* PCI IO map */
	unsigned long	pci_base_addr;	/* device I/O address	*/

	#ifdef RTK_129X_PLATFORM
	unsigned long	ctrl_start;
	/* PCI MASK addr */
	unsigned long	mask_addr;

	/* PCI TRANSLATE addr */
	unsigned long	tran_addr;

	_lock	io_reg_lock;
	#endif

	/* PciBridge */
	struct pci_priv pcipriv;

	u8 irq_alloc;
	unsigned int irq; /* get from pci_dev.irq, store to net_device.irq */
	u16	irqline;
	u8	irq_enabled;
	RT_ISR_CONTENT	isr_content;
	_lock	irq_th_lock;

	u8	bdma64;
#endif/* PLATFORM_LINUX */
} PCI_DATA, *PPCI_DATA;

#endif
