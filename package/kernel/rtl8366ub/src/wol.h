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

#ifndef __RTKSW_API_WOL_H__
#define __RTKSW_API_WOL_H__

/* Function Name:
 *      rtksw_wol_init
 * Description:
 *      Initial WoL function.
 * Input:
 *      unit        - Unit ID
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 * Note:
 *      This API initialize the WoL function.
 *
 *      After initialization, rtksw_wol_enable_set can be used to turn on or turn off WoL function.
 */
extern rtksw_api_ret_t rtksw_wol_init(rtksw_uint32 unit);

/* Function Name:
 *      rtksw_wol_state_set
 * Description:
 *      Enable/Disable WoL function.
 * Input:
 *      unit        - Unit ID
 *      state       - The state of WoL function
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 * Note:
 *      This API enable/disable the WoL function.
 */
extern rtksw_api_ret_t rtksw_wol_state_set(rtksw_uint32 unit, rtksw_enable_t state);

/* Function Name:
 *      rtksw_wol_systemMac_set
 * Description:
 *      Set WOL system MAC address.
 * Input:
 *      unit        - Unit ID
 *      pMac        - System MAC
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Error Input
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      This API configure the MAC address of WoL function.
 */
extern rtksw_api_ret_t rtksw_wol_systemMac_set(rtksw_uint32 unit, rtksw_mac_t *pMac);

#endif /* __RTKSW_API_WOL_H__ */