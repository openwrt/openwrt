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

#include <rtk_switch.h>
#include <rtk_error.h>
#include <interrupt.h>

#include <dal/dal_mgmts.h>

/* Function Name:
 *      rtksw_int_polarity_set
 * Description:
 *      Set interrupt polarity configuration.
 * Input:
 *      unit    - Unit ID
 *      type    - Interruptpolarity type.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      The API can set interrupt polarity configuration.
 */
rtksw_api_ret_t rtksw_int_polarity_set(rtksw_uint32 unit, rtksw_int_polarity_t type)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->int_polarity_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    
    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->int_polarity_set(unit, type);    
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_int_polarity_get
 * Description:
 *      Get interrupt polarity configuration.
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pType   - Interruptpolarity type.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      The API can get interrupt polarity configuration.
 */
rtksw_api_ret_t rtksw_int_polarity_get(rtksw_uint32 unit, rtksw_int_polarity_t *pType)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->int_polarity_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    
    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->int_polarity_get(unit, pType);    
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_int_control_set
 * Description:
 *      Set interrupt trigger status configuration.
 * Input:
 *      unit    - Unit ID
 *      type    - Interrupt type.
 *      enable  - Interrupt status.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_ENABLE       - Invalid enable input.
 * Note:
 *      The API can set interrupt status configuration.
 *      The interrupt trigger status is shown in the following:
 *      - INT_TYPE_LINK_STATUS
 *      - INT_TYPE_METER_EXCEED
 *      - INT_TYPE_LEARN_LIMIT
 *      - INT_TYPE_LINK_SPEED
 *      - INT_TYPE_CONGEST
 *      - INT_TYPE_GREEN_FEATURE
 *      - INT_TYPE_LOOP_DETECT
 *      - INT_TYPE_8051
 *      - INT_TYPE_CABLE_DIAG
 *      - INT_TYPE_ACL
 *      - INT_TYPE_SLIENT
 *      - INT_TYPE_THERMAL
 *      - INT_TYPE_THERMAL_METER_LOW
 *      - INT_TYPE_THERMAL_METER_HIGH
 */
rtksw_api_ret_t rtksw_int_control_set(rtksw_uint32 unit, rtksw_int_type_t type, rtksw_enable_t enable)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->int_control_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    
    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->int_control_set(unit, type, enable);    
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_int_control_get
 * Description:
 *      Get interrupt trigger status configuration.
 * Input:
 *      unit    - Unit ID
 *      type    - Interrupt type.
 * Output:
 *      pEnable - Interrupt status.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      The API can get interrupt status configuration.
 *      The interrupt trigger status is shown in the following:
 *      - INT_TYPE_LINK_STATUS
 *      - INT_TYPE_METER_EXCEED
 *      - INT_TYPE_LEARN_LIMIT
 *      - INT_TYPE_LINK_SPEED
 *      - INT_TYPE_CONGEST
 *      - INT_TYPE_GREEN_FEATURE
 *      - INT_TYPE_LOOP_DETECT
 *      - INT_TYPE_8051
 *      - INT_TYPE_CABLE_DIAG
 *      - INT_TYPE_ACL
 *      - INT_TYPE_UPS
 *      - INT_TYPE_SLIENT
 *      - INT_TYPE_THERMAL
 *      - INT_TYPE_THERMAL_METER_LOW
 *      - INT_TYPE_THERMAL_METER_HIGH
 */
rtksw_api_ret_t rtksw_int_control_get(rtksw_uint32 unit, rtksw_int_type_t type, rtksw_enable_t *pEnable)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->int_control_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    
    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->int_control_get(unit, type, pEnable);    
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_int_status_set
 * Description:
 *      Set interrupt trigger status to clean.
 * Input:
 *      unit            - Unit ID
 *      pStatusMask     - Interrupt status bit mask.
 * Output:
 *      
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_INPUT - Invalid input parameters.
 * Note:
 *      The API can clean interrupt trigger status when interrupt happened.
 *      The interrupt trigger status is shown in the following:
 *      - INT_TYPE_LINK_STATUS    (value[0] (Bit0))
 *      - INT_TYPE_METER_EXCEED   (value[0] (Bit1))
 *      - INT_TYPE_LEARN_LIMIT    (value[0] (Bit2))
 *      - INT_TYPE_LINK_SPEED     (value[0] (Bit3))
 *      - INT_TYPE_CONGEST        (value[0] (Bit4))
 *      - INT_TYPE_GREEN_FEATURE  (value[0] (Bit5))
 *      - INT_TYPE_LOOP_DETECT    (value[0] (Bit6))
 *      - INT_TYPE_8051           (value[0] (Bit7))
 *      - INT_TYPE_CABLE_DIAG     (value[0] (Bit8))
 *      - INT_TYPE_ACL            (value[0] (Bit9))
 *      - INT_TYPE_SLIENT         (value[0] (Bit11))
 *      The status will be cleared after execute this API.
 */
rtksw_api_ret_t rtksw_int_status_set(rtksw_uint32 unit, rtksw_int_status_t *pStatusMask)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->int_status_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    
    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->int_status_set(unit, pStatusMask);    
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_int_status_get
 * Description:
 *      Get interrupt trigger status.
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pStatusMask - Interrupt status bit mask.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      The API can get interrupt trigger status when interrupt happened.
 *      The interrupt trigger status is shown in the following:
 *      - INT_TYPE_LINK_STATUS    (value[0] (Bit0))
 *      - INT_TYPE_METER_EXCEED   (value[0] (Bit1))
 *      - INT_TYPE_LEARN_LIMIT    (value[0] (Bit2))
 *      - INT_TYPE_LINK_SPEED     (value[0] (Bit3))
 *      - INT_TYPE_CONGEST        (value[0] (Bit4))
 *      - INT_TYPE_GREEN_FEATURE  (value[0] (Bit5))
 *      - INT_TYPE_LOOP_DETECT    (value[0] (Bit6))
 *      - INT_TYPE_8051           (value[0] (Bit7))
 *      - INT_TYPE_CABLE_DIAG     (value[0] (Bit8))
 *      - INT_TYPE_ACL            (value[0] (Bit9))
 *      - INT_TYPE_SLIENT         (value[0] (Bit11))
 *
 */
rtksw_api_ret_t rtksw_int_status_get(rtksw_uint32 unit, rtksw_int_status_t* pStatusMask)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->int_status_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    
    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->int_status_get(unit, pStatusMask);    
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_int_advanceInfo_get
 * Description:
 *      Get interrupt advanced information.
 * Input:
 *      unit        - Unit ID
 *      adv_type    - Advanced interrupt type.
 * Output:
 *      info - Information per type.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API can get advanced information when interrupt happened.
 *      The status will be cleared after execute this API.
 */
rtksw_api_ret_t rtksw_int_advanceInfo_get(rtksw_uint32 unit, rtksw_int_advType_t adv_type, rtksw_int_info_t *pInfo)
{
     rtksw_api_ret_t retVal;

     RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->int_advanceInfo_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    
    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->int_advanceInfo_get(unit, adv_type, pInfo);    
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

