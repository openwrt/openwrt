/*
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * Copyright (c) 2025 Realtek Semiconductor Corp. All rights reserved.
 *
 * This software is a confidential and proprietary property of Realtek
 * Semiconductor Corp. Disclosure, reproduction, redistribution, in
 * whole or in part, of this work and its derivatives without express
 * permission is prohibited.
 *
 * Realtek Semiconductor Corp. reserves the right to update, modify, or
 * discontinue this software at any time without notice. This software is
 * provided as is and any express or implied warranties, including, but
 * not limited to, the implied warranties of merchantability and fitness for
 * a particular purpose are disclaimed. In no event shall Realtek
 * Semiconductor Corp. be liable for any direct, indirect, incidental,
 * special, exemplary, or consequential damages (including, but not limited
 * to, procurement of substitute goods or services; loss of use, data, or
 * profits; or business interruption) however caused and on any theory of
 * liability, whether in contract, strict liability, or tort (including
 * negligence or otherwise) arising in any way out of the use of this software,
 * even if advised of the possibility of such damage.
 */

#ifndef __DAL_RTL8371C_SWITCH_H__
#define __DAL_RTL8371C_SWITCH_H__

/*
 * Include Files
 */
#include <rtk_types.h>
#include <rtk_switch.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */

/*
 * Function Declaration
 */

/* Module Name    : Switch     */
/* Sub-module Name: Switch parameter settings */

/* Function Name:
 *      dal_rtl8371c_switch_init
 * Description:
 *      Initialize switch module of the specified device.
 *  Input:
 *      unit        - Unit ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Module must be initialized before using all of APIs in this module
 */
extern rtksw_api_ret_t dal_rtl8371c_switch_init(rtksw_uint32 unit);

/* Function Name:
 *      dal_rtl8371c_switch_mac_set
 * Description:
 *      Set switch MAC address.
 *  Input:
 *      unit        - Unit ID
 *      pMac        - Switch MAC
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Error Input
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      This API configure the MAC address of switch function.
 */
extern rtksw_api_ret_t dal_rtl8371c_switch_mac_set(rtksw_uint32 unit, rtksw_mac_t *pMac);

/* Function Name:
 *      dal_rtl8371c_switch_mac_get
 * Description:
 *      Get switch MAC address
 *  Input:
 *      unit        - Unit ID
 *      pMac        - switch mac
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - Success
 *      RT_ERR_SMI      - SMI access error
 * Note:
 *      This API get the MAC address of switch function.
 */
extern rtksw_api_ret_t dal_rtl8371c_switch_mac_get(rtksw_uint32 unit, rtksw_mac_t *pMac);

/* Function Name:
 *      dal_rtl8371c_switch_reset
 * Description:
 *      Reset chip 
 * Input:
 *      unit    - Unit ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      The API can reset entire chip.
 */
extern rtksw_api_ret_t dal_rtl8371c_switch_reset(rtksw_uint32 unit);

#endif /* __DAL_RTL8371C_SWITCH_H__ */

