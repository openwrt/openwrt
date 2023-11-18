/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation.
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
#ifndef _HAL_CONFIG_H_
#define _HAL_CONFIG_H_

#ifdef CONFIG_PHL_TEST_MP
	#define CONFIG_HAL_TEST_MP
#endif

#ifdef CONFIG_PHL_TEST_FPGA
	#define CONFIG_HAL_TEST_FPGA
#endif

#define MAX_PATH_LEN			256

#define MAX_LINES_HWCONFIG_TXT	2000
#define MAX_BYTES_LINE_HWCONFIG_TXT 	256
#define MAX_HWCONFIG_FILE_CONTENT	MAX_LINES_HWCONFIG_TXT * MAX_BYTES_LINE_HWCONFIG_TXT

/* Disable HALBB Features */
#ifndef CONFIG_PHL_DFS
	#define DRV_BB_DFS_DISABLE
#endif

#ifdef CONFIG_PHL_THERMAL_PROTECT
	#define CONFIG_HAL_THERMAL_PROTECT
#endif

#ifdef CONFIG_BB_TIMER_SUPPORT_DISABLE
	#define DRV_BB_TIMER_SUPPORT_DISABLE
#endif

/* Compile flag for fw for different cut versions.
* This should be wrapped with the compile flags from core, and need to be discussed
*/
#if !defined(CONFIG_FW_SPECIFY_FROM_CORE)
#ifdef PHL_FEATURE_NIC
#define MAC_FW_CATEGORY_NIC
#endif /*PHL_FEATURE_NIC*/

#ifdef PHL_FEATURE_AP
#define MAC_FW_CATEGORY_AP
#endif /*PHL_FEATURE_AP*/

#ifdef CONFIG_WOWLAN
#define MAC_FW_CATEGORY_WOWLAN
#endif /*CONFIG_WOWLAN*/

/*8851B*/
#ifdef CONFIG_RTL8851B
#define MAC_FW_8851B_U1
#endif

/*8852A*/
#ifdef CONFIG_RTL8852A
#define MAC_FW_8852A_U1
#define MAC_FW_8852A_U2
#define MAC_FW_8852A_U3
/*CATEG*/
/*#define MAC_FW_CATEGORY_VRAP*/ /*sd7 only*/
#endif

/*8852B*/
#if defined(CONFIG_RTL8852B) || defined(CONFIG_RTL8852BP)
#define MAC_FW_8852B_U1
#define MAC_FW_8852B_U2
#define MAC_FW_8852B_U3
/*CATEG ,8852B need to set CATEGORY_NIC_BPLUS 0*/
#ifdef CONFIG_RTL8852B
#define MAC_FW_CATEGORY_NICCE
#define MAC_FW_CATEGORY_NIC_PLE
#endif /*CONFIG_RTL8852B*/

#ifdef CONFIG_RTL8852BP
#define MAC_FW_CATEGORY_NIC_BPLUS
#endif /*CONFIG_RTL8852BP*/
#endif  /*(CONFIG_RTL8852B) || defined(CONFIG_RTL8852BP)*/

#ifdef CONFIG_RTL8852C
#define MAC_FW_8852C_U1
#define MAC_FW_8852C_U2
/*CATEG*/
/*#define MAC_FW_CATEGORY_VRAP*/ /*sd7 only*/
#endif /*CONFIG_RTL8852C*/

#if defined(CONFIG_RTL8192XB) || defined(CONFIG_RTL8832BR)
#define MAC_FW_8192XB_U1
#endif
#endif /*CONFIG_FW_SPECIFY_FROM_CORE*/

#ifdef CONFIG_WPP
#define DRV_BB_DBG_TRACE_DISABLE
#endif
/* HALRF Features */

#ifdef RTW_WKARD_PHL_LPS_IQK_TWICE
#define RTW_WKARD_HAL_LPS_IQK_TWICE
#endif

#endif /*_HAL_CONFIG_H_*/
