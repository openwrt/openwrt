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

#include <dal/reg.h>
#include <dal/rtl8371c/rtk_rtl8371c_reg_struct.h>

#include <dal_rtl8371c_wol.h>
#include <rtk_error.h>

#include <dal/dal_mgmts.h>

/* Function Name:
 *      dal_rtl8371c_wol_init
 * Description:
 *      Initial WoL function.
 * Input:
 *      None
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
rtksw_api_ret_t dal_rtl8371c_wol_init(rtksw_uint32 unit)
{
    /* Skip Warning */
    if (unit) {}

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_wol_state_set
 * Description:
 *      Enable/Disable WoL function.
 * Input:
 *      state   - The state of WoL function
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 * Note:
 *      This API enable/disable the WoL function.
 */
rtksw_api_ret_t dal_rtl8371c_wol_state_set(rtksw_uint32 unit, rtksw_enable_t state)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;

    if(state >= RTKSW_ENABLE_END)
        return RT_ERR_INPUT;

    regData = (state == RTKSW_ENABLED) ? 1 : 0;
    if ((retVal = reg16_field_write(unit, RTL8371C_WOL_CTRLr, RTL8371C_WOL_ENf, &regData)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_wol_systemMac_set
 * Description:
 *      Set WOL system MAC address.
 * Input:
 *      pMac   - System MAC
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
rtksw_api_ret_t dal_rtl8371c_wol_systemMac_set(rtksw_uint32 unit, rtksw_mac_t *pMac)
{
    rtksw_uint32      regData;
    rtksw_api_ret_t   retVal;

    if(pMac == NULL)
        return RT_ERR_INPUT;

    regData = ((pMac->octet[0] << 8) | pMac->octet[1]);
    if ((retVal = reg16_field_write(unit, RTL8371C_WOL_MACr, RTL8371C_MAC_47_32f, &regData)) != RT_ERR_OK)
        return retVal;

    regData = ((pMac->octet[2] << 8) | pMac->octet[3]);
    if ((retVal = reg16_field_write(unit, RTL8371C_WOL_MACr, RTL8371C_MAC_31_16f, &regData)) != RT_ERR_OK)
        return retVal;

    regData = ((pMac->octet[4] << 8) | pMac->octet[5]);
    if ((retVal = reg16_field_write(unit, RTL8371C_WOL_MACr, RTL8371C_MAC_15_0f, &regData)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

