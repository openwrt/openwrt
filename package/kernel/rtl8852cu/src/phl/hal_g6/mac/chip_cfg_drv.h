/** @file */
/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation. All rights reserved.
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
 ******************************************************************************/

#ifndef _MAC_AX_CHIP_CFG_H_
#define _MAC_AX_CHIP_CFG_H_

/* Modify MakeFile to reduce code size (chip & interface) */
#ifdef CONFIG_RTL8852A
#define MAC_AX_8852A_SUPPORT	1
#else
#define MAC_AX_8852A_SUPPORT	0
#endif

#ifdef CONFIG_RTL8852B
#define MAC_AX_8852B_SUPPORT	1
#else
#define MAC_AX_8852B_SUPPORT	0
#endif

#ifdef CONFIG_RTL8852C
#define MAC_AX_8852C_SUPPORT	1
#else
#define MAC_AX_8852C_SUPPORT	0
#endif

#ifdef CONFIG_RTL8192XB
#define MAC_AX_8192XB_SUPPORT	1
#else
#define MAC_AX_8192XB_SUPPORT	0
#endif

#ifdef CONFIG_RTL8851B
#define MAC_AX_8851B_SUPPORT	1
#else
#define MAC_AX_8851B_SUPPORT	0
#endif

#ifdef CONFIG_RTL8851E
#define MAC_AX_8851E_SUPPORT	1
#else
#define MAC_AX_8851E_SUPPORT	0
#endif

#ifdef CONFIG_RTL8852D
#define MAC_AX_8852D_SUPPORT	1
#else
#define MAC_AX_8852D_SUPPORT	0
#endif
/* Interface support */
#ifdef CONFIG_SDIO_HCI
#define MAC_AX_SDIO_SUPPORT	1
#else
#define MAC_AX_SDIO_SUPPORT	0
#endif
#ifdef CONFIG_USB_HCI
#define MAC_AX_USB_SUPPORT	1
#else
#define MAC_AX_USB_SUPPORT	0
#endif
#ifdef CONFIG_PCI_HCI
#define MAC_AX_PCIE_SUPPORT	1
#else
#define MAC_AX_PCIE_SUPPORT	0
#endif
#endif

