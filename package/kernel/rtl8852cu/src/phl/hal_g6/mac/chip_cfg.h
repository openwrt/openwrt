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

#ifndef __cplusplus /* for win/linux driver */

/* Modify MakeFile to reduce code size (chip & interface) */
#ifdef CONFIG_RTL8852A
#define MAC_AX_8852A_SUPPORT	1
#define MAC_8852A_SUPPORT
#else
#define MAC_AX_8852A_SUPPORT	0
#endif

#if defined(CONFIG_RTL8852B) || defined(CONFIG_RTL8852BP)
#define MAC_AX_8852B_SUPPORT	1
#define MAC_8852B_SUPPORT
#else
#define MAC_AX_8852B_SUPPORT	0
#endif

#ifdef CONFIG_RTL8852C
#define MAC_AX_8852C_SUPPORT	1
#define MAC_8852C_SUPPORT
#else
#define MAC_AX_8852C_SUPPORT	0
#endif

#if defined(CONFIG_RTL8192XB) || defined(CONFIG_RTL8832BR)
#ifndef CONFIG_RTL8192XB
#define CONFIG_RTL8192XB
#endif
#endif

#ifdef CONFIG_RTL8192XB
#define MAC_AX_8192XB_SUPPORT	1
#define MAC_AX_8832BR_SUPPORT	1
#define MAC_8192XB_SUPPORT
#else
#define MAC_AX_8192XB_SUPPORT	0
#define MAC_AX_8832BR_SUPPORT	0
#endif

#ifdef CONFIG_RTL8851B
#define MAC_AX_8851B_SUPPORT	1
#define MAC_8851B_SUPPORT
#else
#define MAC_AX_8851B_SUPPORT	0
#endif

#ifdef CONFIG_RTL8851E
#define MAC_AX_8851E_SUPPORT	1
#define MAC_8851E_SUPPORT
#else
#define MAC_AX_8851E_SUPPORT	0
#endif

#ifdef CONFIG_RTL8852D
#define MAC_AX_8852D_SUPPORT	1
#define MAC_8852D_SUPPORT
#else
#define MAC_AX_8852D_SUPPORT	0
#endif

#ifdef CONFIG_RTL1115E
#define MAC_AX_1115E_SUPPORT	1
#define MAC_BE_1115E_SUPPORT	1
#define MAC_BE_8922A_SUPPORT	1
#define MAC_BE_8952A_SUPPORT	1

#define MAC_1115E_SUPPORT
#define MAC_8922A_SUPPORT
#define MAC_8952A_SUPPORT
#else
#define MAC_AX_1115E_SUPPORT	0
#define MAC_BE_1115E_SUPPORT	0
#define MAC_BE_8922A_SUPPORT	0
#define MAC_BE_8952A_SUPPORT	0
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

#else /* for WD1 test program */

/* Modify MakeFile to reduce code size (chip & interface) */

#if WIFI_HAL_G6
#define MAC_AX_8852A_SUPPORT	1
#define MAC_AX_8852B_SUPPORT	1
#define MAC_AX_8852C_SUPPORT	1
#define MAC_AX_8192XB_SUPPORT	1
#define MAC_AX_8832BR_SUPPORT	1
#define MAC_AX_8851B_SUPPORT	1
#define MAC_AX_8851E_SUPPORT	1
#define MAC_AX_8852D_SUPPORT	1
//for FW_PUB
#define MAC_8852A_SUPPORT
#define MAC_8852B_SUPPORT
#define MAC_8852C_SUPPORT
#define MAC_8192XB_SUPPORT
#define MAC_8851B_SUPPORT
#define MAC_8851E_SUPPORT
#define MAC_8852D_SUPPORT
#else
#define MAC_AX_8852A_SUPPORT	0
#define MAC_AX_8852B_SUPPORT	0
#define MAC_AX_8852C_SUPPORT	0
#define MAC_AX_8192XB_SUPPORT	0
#define MAC_AX_8832BR_SUPPORT	0
#define MAC_AX_8851B_SUPPORT	0
#define MAC_AX_8851E_SUPPORT	0
#define MAC_AX_8852D_SUPPORT	0
#endif

#if WIFI_HAL_G7
#define MAC_BE_1115E_SUPPORT	1
#define MAC_BE_8922A_SUPPORT	1
#define MAC_BE_8952A_SUPPORT	1
//for FW_PUB
#define MAC_1115E_SUPPORT
#define MAC_8922A_SUPPORT
#define MAC_8952A_SUPPORT
#else
#define MAC_BE_1115E_SUPPORT	0
#define MAC_BE_8922A_SUPPORT	0
#define MAC_BE_8952A_SUPPORT	0
#endif

#if MAC_AX_8852A_SUPPORT
#define MAC_FW_8852A_U1		0
#define MAC_FW_8852A_U2		1
#define MAC_FW_8852A_U3		1
#define MAC_FW_8852A_U4		0
#endif

#if MAC_AX_8852B_SUPPORT
#define MAC_FW_8852B_U1		0
#define MAC_FW_8852B_U2		1
#endif

#if MAC_AX_8852C_SUPPORT
#define MAC_FW_8852C_U1		1
#endif

#if MAC_AX_8192XB_SUPPORT
#define MAC_FW_8192XB_U1	1
#endif

#if MAC_AX_8851B_SUPPORT
#define MAC_FW_8851B_U1		1
#endif

#if MAC_AX_8851E_SUPPORT
#define MAC_FW_8851E_U1		1
#endif

#if MAC_AX_8852D_SUPPORT
#define MAC_FW_8852D_U1		1
#endif

/* Interface support */
#define MAC_AX_SDIO_SUPPORT	1
#define MAC_AX_USB_SUPPORT	1
#define MAC_AX_PCIE_SUPPORT	1

#endif // #else /* for WD1 test program */
#endif

