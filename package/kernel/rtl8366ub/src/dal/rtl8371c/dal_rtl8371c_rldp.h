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

#ifndef __DAL_RTL8371C_RLDP_H__
#define __DAL_RTL8371C_RLDP_H__


/*
 * Include Files
 */

#include <rldp.h>

/*
 * Function Declaration
 */

/* Module Name : RLDP */


/* Function Name:
 *      dal_rtl8371c_rldp_config_set
 * Description:
 *      Set RLDP module configuration
 *  Input:
 *      unit        - Unit ID
 *      pConfig     - configuration structure of RLDP
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern rtksw_api_ret_t dal_rtl8371c_rldp_config_set(rtksw_uint32 unit, rtksw_rldp_config_t *pConfig);


/* Function Name:
 *      dal_rtl8371c_rldp_config_get
 * Description:
 *      Get RLDP module configuration
 *  Input:
 *      unit        - Unit ID
 * Output:
 *      pConfig - configuration structure of RLDP
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern rtksw_api_ret_t dal_rtl8371c_rldp_config_get(rtksw_uint32 unit, rtksw_rldp_config_t *pConfig);


/* Function Name:
 *      dal_rtl8371c_rldp_portConfig_set
 * Description:
 *      Set per port RLDP module configuration
 *  Input:
 *      unit        - Unit ID
 *      port        - port number to be configured
 *      pPortConfig - per port configuration structure of RLDP
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern rtksw_api_ret_t dal_rtl8371c_rldp_portConfig_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_rldp_portConfig_t *pPortConfig);


/* Function Name:
 *      dal_rtl8371c_rldp_portConfig_get
 * Description:
 *      Get per port RLDP module configuration
 *  Input:
 *      unit        - Unit ID
 *      port        - port number to be get
 * Output:
 *      pPortConfig - per port configuration structure of RLDP
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern rtksw_api_ret_t dal_rtl8371c_rldp_portConfig_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_rldp_portConfig_t *pPortConfig);


/* Function Name:
 *      dal_rtl8371c_rldp_status_get
 * Description:
 *      Get RLDP module status
 *  Input:
 *      unit        - Unit ID
 * Output:
 *      pStatus - status structure of RLDP
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern rtksw_api_ret_t dal_rtl8371c_rldp_status_get(rtksw_uint32 unit, rtksw_rldp_status_t *pStatus);


/* Function Name:
 *      dal_rtl8371c_rldp_portStatus_get
 * Description:
 *      Get RLDP module status
 *  Input:
 *      unit        - Unit ID
 *      port        - port number to be get
 * Output:
 *      pPortStatus - per port status structure of RLDP
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern rtksw_api_ret_t dal_rtl8371c_rldp_portStatus_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_rldp_portStatus_t *pPortStatus);


/* Function Name:
 *      dal_rtl8371c_rldp_portStatus_clear
 * Description:
 *      Clear RLDP module status
 *  Input:
 *      unit        - Unit ID
 *      port        - port number to be clear
 *      pPortStatus - per port status structure of RLDP
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      Clear operation effect loop_enter and loop_leave only, other field in
 *      the structure are don't care
 */
extern rtksw_api_ret_t dal_rtl8371c_rldp_portStatus_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_rldp_portStatus_t *pPortStatus);


/* Function Name:
 *      dal_rtl8371c_rldp_portLoopPair_get
 * Description:
 *      Get RLDP port loop pairs
 *  Input:
 *      unit        - Unit ID
 *      port        - port number to be get
 * Output:
 *      pPortmask - per port related loop ports
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern rtksw_api_ret_t dal_rtl8371c_rldp_portLoopPair_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_portmask_t *pPortmask);

#endif /* __DAL_RTL8371C_RLDP_H__ */

