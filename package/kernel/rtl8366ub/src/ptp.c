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
#include <ptp.h>

#include <dal/dal_mgmts.h>

/* Function Name:
 *      rtksw_ptp_init
 * Description:
 *      PTP function initialization.
 * Input:
 *      unit    - Unit ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 * Note:
 *      This API is used to initialize PTP status.
 */
rtksw_api_ret_t rtksw_ptp_init(rtksw_uint32 unit)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->ptp_init)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->ptp_init(unit);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_ptp_mac_set
 * Description:
 *      Configure PTP mac address.
 * Input:
 *      unit    - Unit ID
 *      mac     - mac address to parser PTP packets.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameter.
 * Note:
 *      None
 */
rtksw_api_ret_t rtksw_ptp_mac_set(rtksw_uint32 unit, rtksw_mac_t mac)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->ptp_mac_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->ptp_mac_set(unit, mac);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_ptp_mac_get
 * Description:
 *      Get PTP mac address.
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pMac    - mac address to parser PTP packets.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameter.
 * Note:
 *      None
 */
rtksw_api_ret_t rtksw_ptp_mac_get(rtksw_uint32 unit, rtksw_mac_t *pMac)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->ptp_mac_get)
        return RT_ERR_DRIVER_NOT_FOUND;


    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->ptp_mac_get(unit, pMac);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_ptp_tpid_set
 * Description:
 *      Configure PTP accepted outer & inner tag TPID.
 * Input:
 *      unit    - Unit ID
 *      outerId - Ether type of S-tag frame parsing in PTP ports.
 *      innerId - Ether type of C-tag frame parsing in PTP ports.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameter.
 * Note:
 *      None
 */
rtksw_api_ret_t rtksw_ptp_tpid_set(rtksw_uint32 unit, rtksw_ptp_tpid_t outerId, rtksw_ptp_tpid_t innerId)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->ptp_tpid_set)
        return RT_ERR_DRIVER_NOT_FOUND;


    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->ptp_tpid_set(unit, outerId, innerId);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_ptp_tpid_get
 * Description:
 *      Get PTP accepted outer & inner tag TPID.
 * Input:
 *      unit        - Unit ID
 * Output:
 *      pOuterId    - Ether type of S-tag frame parsing in PTP ports.
 *      pInnerId    - Ether type of C-tag frame parsing in PTP ports.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      None
 */
rtksw_api_ret_t rtksw_ptp_tpid_get(rtksw_uint32 unit, rtksw_ptp_tpid_t *pOuterId, rtksw_ptp_tpid_t *pInnerId)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->ptp_tpid_get)
        return RT_ERR_DRIVER_NOT_FOUND;


    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->ptp_tpid_get(unit, pOuterId, pInnerId);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_ptp_refTime_set
 * Description:
 *      Set the reference time of the specified device.
 * Input:
 *      unit        - Unit ID
 *      timeStamp   - reference timestamp value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8390, 8380
 * Note:
 *      None
 */
rtksw_api_ret_t rtksw_ptp_refTime_set(rtksw_uint32 unit, rtksw_ptp_timeStamp_t timeStamp)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->ptp_refTime_set)
        return RT_ERR_DRIVER_NOT_FOUND;


    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->ptp_refTime_set(unit, timeStamp);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_ptp_refTime_get
 * Description:
 *      Get the reference time of the specified device.
 * Input:
 *      unit        - Unit ID
 * Output:
 *      pTimeStamp  - pointer buffer of the reference time
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390, 8380
 * Note:
 *      None
 */
rtksw_api_ret_t rtksw_ptp_refTime_get(rtksw_uint32 unit, rtksw_ptp_timeStamp_t *pTimeStamp)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->ptp_refTime_get)
        return RT_ERR_DRIVER_NOT_FOUND;


    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->ptp_refTime_get(unit, pTimeStamp);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_ptp_refTimeAdjust_set
 * Description:
 *      Adjust the reference time.
 * Input:
 *      unit        - Unit ID
 *      sign        - significant
 *      timeStamp   - reference timestamp value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      sign=0 for positive adjustment, sign=1 for negative adjustment.
 */
rtksw_api_ret_t rtksw_ptp_refTimeAdjust_set(rtksw_uint32 unit, rtksw_ptp_sys_adjust_t sign, rtksw_ptp_timeStamp_t timeStamp)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->ptp_refTimeAdjust_set)
        return RT_ERR_DRIVER_NOT_FOUND;


    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->ptp_refTimeAdjust_set(unit, sign, timeStamp);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_ptp_refTimeEnable_set
 * Description:
 *      Set the enable state of reference time of the specified device.
 * Input:
 *      unit    - Unit ID
 *      enable  - status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
rtksw_api_ret_t rtksw_ptp_refTimeEnable_set(rtksw_uint32 unit, rtksw_enable_t enable)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->ptp_refTimeEnable_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->ptp_refTimeEnable_set(unit, enable);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_ptp_refTimeEnable_get
 * Description:
 *      Get the enable state of reference time of the specified device.
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pEnable - status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390, 8380
 * Note:
 *      None
 */
rtksw_api_ret_t rtksw_ptp_refTimeEnable_get(rtksw_uint32 unit, rtksw_enable_t *pEnable)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->ptp_refTimeEnable_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->ptp_refTimeEnable_get(unit, pEnable);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_ptp_portEnable_set
 * Description:
 *      Set PTP status of the specified port.
 * Input:
 *      unit    - Unit ID
 *      port    - port id
 *      enable  - status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT     - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
rtksw_api_ret_t rtksw_ptp_portEnable_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t enable)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->ptp_portEnable_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->ptp_portEnable_set(unit, port, enable);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_ptp_portEnable_get
 * Description:
 *      Get PTP status of the specified port.
 * Input:
 *      unit    - Unit ID
 *      port    - port id
 * Output:
 *      pEnable - status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT         - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
rtksw_api_ret_t rtksw_ptp_portEnable_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t *pEnable)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->ptp_portEnable_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->ptp_portEnable_get(unit, port, pEnable);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_ptp_portTimestamp_get
 * Description:
 *      Get PTP timstamp according to the PTP identifier on the dedicated port from the specified device.
 * Input:
 *      unit        - Unit ID
 *      port        - port id
 *      type        - PTP message type
 * Output:
 *      pInfo      - pointer buffer of sequence ID and timestamp
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390, 8380
 * Note:
 *      None
 */
rtksw_api_ret_t rtksw_ptp_portTimestamp_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_ptp_msgType_t type, rtksw_ptp_info_t *pInfo)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->ptp_portTimestamp_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->ptp_portTimestamp_get(unit, port, type, pInfo);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_ptp_intControl_set
 * Description:
 *      Set PTP interrupt trigger status configuration.
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
 *      The API can set PTP interrupt status configuration.
 *      The interrupt trigger status is shown in the following:
 *          RTKSW_PTP_INT_TYPE_TX_SYNC = 0,
 *          RTKSW_PTP_INT_TYPE_TX_DELAY_REQ,
 *          RTKSW_PTP_INT_TYPE_TX_PDELAY_REQ,
 *          RTKSW_PTP_INT_TYPE_TX_PDELAY_RESP,
 *          RTKSW_PTP_INT_TYPE_RX_SYNC,
 *          RTKSW_PTP_INT_TYPE_RX_DELAY_REQ,
 *          RTKSW_PTP_INT_TYPE_RX_PDELAY_REQ,
 *          RTKSW_PTP_INT_TYPE_RX_PDELAY_RESP,
 *          RTKSW_PTP_INT_TYPE_ALL,
 */
rtksw_api_ret_t rtksw_ptp_intControl_set(rtksw_uint32 unit, rtksw_ptp_intType_t type, rtksw_enable_t enable)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->ptp_intControl_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->ptp_intControl_set(unit, type, enable);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_ptp_intControl_get
 * Description:
 *      Get PTP interrupt trigger status configuration.
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
 *          RTKSW_PTP_INT_TYPE_TX_SYNC = 0,
 *          RTKSW_PTP_INT_TYPE_TX_DELAY_REQ,
 *          RTKSW_PTP_INT_TYPE_TX_PDELAY_REQ,
 *          RTKSW_PTP_INT_TYPE_TX_PDELAY_RESP,
 *          RTKSW_PTP_INT_TYPE_RX_SYNC,
 *          RTKSW_PTP_INT_TYPE_RX_DELAY_REQ,
 *          RTKSW_PTP_INT_TYPE_RX_PDELAY_REQ,
 *          RTKSW_PTP_INT_TYPE_RX_PDELAY_RESP,
 */
rtksw_api_ret_t rtksw_ptp_intControl_get(rtksw_uint32 unit, rtksw_ptp_intType_t type, rtksw_enable_t *pEnable)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->ptp_intControl_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->ptp_intControl_get(unit, type, pEnable);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_ptp_intStatus_get
 * Description:
 *      Get PTP port interrupt trigger status.
 * Input:
 *      unit    - Unit ID
 *      port    - physical port
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
 *      - PORT 0  INT    (value[0] (Bit0))
 *      - PORT 1  INT    (value[0] (Bit1))
 *      - PORT 2  INT    (value[0] (Bit2))
 *      - PORT 3  INT    (value[0] (Bit3))
 *      - PORT 4  INT   (value[0] (Bit4))

 *
 */
rtksw_api_ret_t rtksw_ptp_intStatus_get(rtksw_uint32 unit, rtksw_ptp_intStatus_t *pStatusMask)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->ptp_intStatus_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->ptp_intStatus_get(unit, pStatusMask);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_ptp_portIntStatus_set
 * Description:
 *      Set PTP port interrupt trigger status to clean.
 * Input:
 *      unit        - Unit ID
 *      port        - physical port
 *      statusMask  - Interrupt status bit mask.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_INPUT - Invalid input parameters.
 * Note:
 *      The API can clean interrupt trigger status when interrupt happened.
 *      The interrupt trigger status is shown in the following:
 *      - RTKSW_PTP_INT_TYPE_TX_SYNC              (value[0] (Bit0))
 *      - RTKSW_PTP_INT_TYPE_TX_DELAY_REQ      (value[0] (Bit1))
 *      - RTKSW_PTP_INT_TYPE_TX_PDELAY_REQ    (value[0] (Bit2))
 *      - RTKSW_PTP_INT_TYPE_TX_PDELAY_RESP   (value[0] (Bit3))
 *      - RTKSW_PTP_INT_TYPE_RX_SYNC              (value[0] (Bit4))
 *      - RTKSW_PTP_INT_TYPE_RX_DELAY_REQ      (value[0] (Bit5))
 *      - RTKSW_PTP_INT_TYPE_RX_PDELAY_REQ    (value[0] (Bit6))
 *      - RTKSW_PTP_INT_TYPE_RX_PDELAY_RESP   (value[0] (Bit7))
 *      The status will be cleared after execute this API.
 */
rtksw_api_ret_t rtksw_ptp_portIntStatus_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_ptp_intStatus_t statusMask)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->ptp_portIntStatus_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->ptp_portIntStatus_set(unit, port, statusMask);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_ptp_portIntStatus_get
 * Description:
 *      Get PTP port interrupt trigger status.
 * Input:
 *      unit        - Unit ID
 *      port        - physical port
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
 *      - RTKSW_PTP_INT_TYPE_TX_SYNC              (value[0] (Bit0))
 *      - RTKSW_PTP_INT_TYPE_TX_DELAY_REQ      (value[0] (Bit1))
 *      - RTKSW_PTP_INT_TYPE_TX_PDELAY_REQ    (value[0] (Bit2))
 *      - RTKSW_PTP_INT_TYPE_TX_PDELAY_RESP   (value[0] (Bit3))
 *      - RTKSW_PTP_INT_TYPE_RX_SYNC              (value[0] (Bit4))
 *      - RTKSW_PTP_INT_TYPE_RX_DELAY_REQ      (value[0] (Bit5))
 *      - RTKSW_PTP_INT_TYPE_RX_PDELAY_REQ    (value[0] (Bit6))
 *      - RTKSW_PTP_INT_TYPE_RX_PDELAY_RESP   (value[0] (Bit7))
 *
 */
rtksw_api_ret_t rtksw_ptp_portIntStatus_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_ptp_intStatus_t *pStatusMask)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->ptp_portIntStatus_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->ptp_portIntStatus_get(unit, port, pStatusMask);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_ptp_portTrap_set
 * Description:
 *      Set PTP packet trap of the specified port.
 * Input:
 *      unit    - Unit ID
 *      port    - port id
 *      enable  - status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT     - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
rtksw_api_ret_t rtksw_ptp_portTrap_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t enable)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->ptp_portTrap_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->ptp_portTrap_set(unit, port, enable);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_ptp_portTrap_get
 * Description:
 *      Get PTP packet trap of the specified port.
 * Input:
 *      unit    - Unit ID
 *      port    - port id
 * Output:
 *      pEnable - status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT         - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
rtksw_api_ret_t rtksw_ptp_portTrap_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t *pEnable)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->ptp_portTrap_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->ptp_portTrap_get(unit, port, pEnable);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}


/* Function Name:
 *      rtksw_ptp_portBypassEnable_set
 * Description:
 *      Set PTP status of the specified port.
 * Input:
 *      unit    - Unit ID
 *      port    - port id
 *      enable  - status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT     - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8371
 * Note:
 *      None
 * Changes:
 *      None
 */
rtksw_api_ret_t rtksw_ptp_portBypassEnable_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t enable)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
    if (NULL == RT_MAPPER->ptp_portBypassEnable_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->ptp_portBypassEnable_set(unit, port, enable);
    RTKSW_API_UNLOCK(unit);
    return retVal;

} /* end of rtksw_ptp_portBypassEnable_set */


/* Function Name:
 *      rtksw_ptp_portBypassEnable_get
 * Description:
 *      Get PTP status of the specified port.
 * Input:
 *      unit    - Unit ID
 *      port    - port id
 * Output:
 *      pEnable - status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT         - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8371
 * Note:
 *      None
 * Changes:
 *      None
 */
rtksw_api_ret_t rtksw_ptp_portBypassEnable_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t *pEnable)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
    if (NULL == RT_MAPPER->ptp_portBypassEnable_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->ptp_portBypassEnable_get(unit, port, pEnable);
    RTKSW_API_UNLOCK(unit);
    return retVal;
} 

/* Function Name:
 *      rtksw_ptp_portTypeEnable_set
 * Description:
 *      Get PTP status of the specified port.
 * Input:
 *      unit    - Unit ID
 *      port    - port id
 *      type    - RTKSW_PTP_ETH or UDP
 *      enable  - status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT         - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      
 * Note:
 *      None
 * Changes:
 *      None
 */
ret_t rtksw_ptp_portTypeEnable_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_ptp_header_t type,rtksw_enable_t Enable)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
    
    if (NULL == RT_MAPPER->ptp_portTypeEnable_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->ptp_portTypeEnable_set(unit, port, type, Enable);
    RTKSW_API_UNLOCK(unit);
    return retVal;
}


/* Function Name:
 *      rtksw_ptp_portTypeEnable_get
 * Description:
 *      Get PTP status of the specified port.
 * Input:
 *      unit    - Unit ID
 *      port    - port id
 *      type    - RTKSW_PTP_ETH or UDP
 *      enable  - status
 * Output:
 *      pEnable - status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT         - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      
 * Note:
 *      None
 * Changes:
 *      None
 */
ret_t rtksw_ptp_portTypeEnable_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_ptp_header_t type,rtksw_enable_t *pEnable)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
    
    if (NULL == RT_MAPPER->ptp_portTypeEnable_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->ptp_portTypeEnable_get(unit, port, type,pEnable);
    RTKSW_API_UNLOCK(unit);
    return retVal;
}

/* Function Name:
 *      rtksw_ptp_systemRefTime_set
 * Description:
 *      Set the reference time of the specified portmask.
 * Input:
 *      unit        - Unit ID
 *      timeStamp   - reference timestamp value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8371
 * Note:
 *      None
 * Changes:
 *          Add portmask parameter.
 */
rtksw_api_ret_t rtksw_ptp_systemRefTime_set(rtksw_uint32 unit, rtksw_time_timeStamp_t timeStamp,rtksw_enable_t apply)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
    
    if (NULL == RT_MAPPER->ptp_systemRefTime_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->ptp_systemRefTime_set(unit, timeStamp, apply);
    RTKSW_API_UNLOCK(unit);
    return retVal;
   
} /* end of rtksw_ptp_systemRefTime_set */

/* Function Name:
 *      rtksw_ptp_systemRefTime_get
 * Description:
 *      Get the reference time of the specified port.
 * Input:
 *      unit    - Unit ID
 *      port    - port id, it should be base port of PHY
 * Output:
 *      pTimeStamp - pointer buffer of the reference time
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8371
 * Note:
 *      None
 * Changes:
 *          Add port parameter.
 */
rtksw_api_ret_t rtksw_ptp_systemRefTime_get(rtksw_uint32 unit, rtksw_time_timeStamp_t *pTimeStamp)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
    
    if (NULL == RT_MAPPER->ptp_systemRefTime_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->ptp_systemRefTime_get(unit, pTimeStamp);
    RTKSW_API_UNLOCK(unit);
    return retVal;
} 

/* Function Name:
 *      rtksw_ptp_oper_triger
 * Description:
 *      Set the PTP time operation triger.
 * Input:
 *      unit    - Unit ID
 *      triType     
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
rtksw_api_ret_t rtksw_ptp_oper_triger(rtksw_uint32 unit)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
    
    if (NULL == RT_MAPPER->ptp_oper_triger)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->ptp_oper_triger(unit);
    RTKSW_API_UNLOCK(unit);
    return retVal;
}

/* Function Name:
 *      rtksw_ptp_systemRefTimeAdjust_set
 * Description:
 *      Adjust the reference time of portmask.
 * Input:
 *      unit        - Unit ID
 *      portmask    - portmask, it should be base ports of PHYs
 *      sign        - significant
 *      timeStamp   - reference timestamp value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8371
 * Note:
 *      sign=0 for positive adjustment, sign=1 for negative adjustment.
 * Changes:
 *      [3.2.0]
 *          Change name from rtksw_time_refTimeAdjust_set.
 *          Add portmask parameter.
 */
rtksw_api_ret_t rtksw_ptp_systemRefTimeAdjust_set(rtksw_uint32 unit, rtksw_ptp_sys_adjust_t sign, rtksw_time_timeStamp_t timeStamp, rtksw_enable_t apply)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
    
    if (NULL == RT_MAPPER->ptp_systemRefTimeAdjust_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->ptp_systemRefTimeAdjust_set(unit, sign, timeStamp, apply);
    RTKSW_API_UNLOCK(unit);
    return retVal;

} /* end of rtksw_ptp_systemRefTimeAdjust_set */

/* Function Name:
 *      rtksw_ptp_vlanTpid_set
 * Description:
 *      Set the VLAN TPID of specific port.
 * Input:
 *      unit    - Unit ID
 *      type    - outer or inner VLAN
 *      idx     - TPID entry index
 *      tpid    - VLAN TPID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8371
 * Note:
 *      None
 * Changes:
 *          New added function.
 */
rtksw_api_ret_t rtksw_ptp_vlanTpid_set(rtksw_uint32 unit, rtksw_vlanType_t type, rtksw_uint32 idx, rtksw_uint32 tpid)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->ptp_vlanTpid_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    /* function body */
    retVal = RT_MAPPER->ptp_vlanTpid_set(unit, type, idx, tpid);
    RTKSW_API_UNLOCK(unit);
    return retVal;

} /* end of rtksw_ptp_vlanTpid_set */

/* Function Name:
 *      rtksw_ptp_vlanTpid_get
 * Description:
 *      Get the VLAN TPID of specific port.
 * Input:
 *      unit    - Unit ID
 *      type    - outer or inner VLAN
 *      idx     - TPID entry index
 * Output:
 *      pTpid  - pointer to TPID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 * Changes:
 *          New added function.
 */
rtksw_api_ret_t rtksw_ptp_vlanTpid_get(rtksw_uint32 unit, rtksw_vlanType_t type, rtksw_uint32 idx, rtksw_uint32 *pTpid)
{
       /* function body */
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->ptp_vlanTpid_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    /* function body */
    retVal = RT_MAPPER->ptp_vlanTpid_get(unit, type, idx, pTpid);
    RTKSW_API_UNLOCK(unit);
    return retVal;
} /* end of rtksw_ptp_vlanTpid_get */

/* Function Name:
 *      rtksw_ptp_operCfg_set
 * Description:
 *      Set the PTP time operation configuration of specific port.
 * Input:
 *      unit    - Unit ID
 *      pOperCfg  - pointer to PTP time operation configuraton
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8371
 * Note:
 *      None
 * Changes:
 *          New added function.
 */
rtksw_api_ret_t rtksw_ptp_operCfg_set(rtksw_uint32 unit, rtksw_time_operCfg_t *pOperCfg)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->ptp_operCfg_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    /* function body */
    retVal = RT_MAPPER->ptp_operCfg_set(unit, pOperCfg);
    RTKSW_API_UNLOCK(unit);
    return retVal;

} /* end of rtksw_ptp_operCfg_set */

/* Function Name:
 *      rtksw_ptp_operCfg_get
 * Description:
 *      Get the PTP time operation configuration of specific port.
 * Input:
 *      unit        - Unit ID
 * Output:
 *      pOperCfg    - pointer to PTP time operation configuraton
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8371
 * Note:
 *      None
 * Changes:
 *          New added function.
 */
rtksw_api_ret_t rtksw_ptp_operCfg_get(rtksw_uint32 unit, rtksw_time_operCfg_t *pOperCfg)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->ptp_operCfg_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    /* function body */
    retVal = RT_MAPPER->ptp_operCfg_get(unit, pOperCfg);
    RTKSW_API_UNLOCK(unit);
    return retVal; 
} /* end of rtksw_ptp_operCfg_get */

/* Function Name:
 *      rtksw_time_portPtpLatchTime_get
 * Description:
 *      Get the PTP latched time of specific port.
 * Input:
 *      unit        - Unit ID
 * Output:
 *      pOperCfg    - pointer to PTP time operation configuraton
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8371
 * Note:
 *      None
 * Changes:
 *          New added function.
 */
rtksw_api_ret_t rtksw_ptp_latchTime_get(rtksw_uint32 unit, rtksw_time_timeStamp_t *pLatchTime)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->ptp_latchTime_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    /* function body */
    retVal = RT_MAPPER->ptp_latchTime_get(unit, pLatchTime);
    RTKSW_API_UNLOCK(unit);
    return retVal;

} /* end of rtksw_time_portPtpLatchTime_get */

/* Function Name:
 *      rtksw_ptp_refTimeFreqCfg_set
 * Description:
 *      Set the frequency of reference time of the specified port.
 * Input:
 *      unit        - Unit ID
 *      freq        - reference time frequency
 *      apply       - if the frequency is applied immediately
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - input parameter out of range
 * Applicable:
 *      phy
 * Note:
 *      None
 * Changes:
 *          New added function.
 */
rtksw_api_ret_t rtksw_ptp_refTimeFreqCfg_set(rtksw_uint32 unit, rtksw_uint32 freq, rtksw_enable_t apply)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->ptp_refTimeFreqCfg_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    /* function body */
    retVal = RT_MAPPER->ptp_refTimeFreqCfg_set(unit, freq, apply);
    RTKSW_API_UNLOCK(unit);
    return retVal;
}

/* Function Name:
 *      rtksw_ptp_refTimeFreqCfg_get
 * Description:
 *      Get the frequency of reference time of the specified port.
 * Input:
 *      unit        - Unit ID
 * Output:
 *      pFreqCfg    - pointer to configured reference time frequency
 *      pFreqCur    - pointer to current reference time frequency
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *  phy
 * Note:
 *      None
 * Changes:
 *          New added function.
 */
rtksw_api_ret_t rtksw_ptp_refTimeFreqCfg_get(rtksw_uint32 unit, rtksw_uint32 *pFreqCfg, rtksw_uint32 *pFreqCur)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->ptp_refTimeFreqCfg_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    /* function body */
    retVal = RT_MAPPER->ptp_refTimeFreqCfg_get(unit, pFreqCfg, pFreqCur);
    RTKSW_API_UNLOCK(unit);
    return retVal;
}

/* Function Name:
 *      rtksw_ptp_txTimestampFifo_get
 * Description:
 *      Get the top entry from PTP Tx timstamp FIFO on the dedicated port from the specified device. of the specified port.
 * Input:
 *      unit        - Unit ID
 * Output:
 *      pTimeEntry  - pointer buffer of TIME timestamp entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      phy
 * Note:
 *      None
 * Changes:
 *      [SDK_NEXT_RELEASE_VERSION]
 *          New added function.
 */
rtksw_api_ret_t rtksw_ptp_txTimestampFifo_get(rtksw_uint32 unit, rtksw_time_txTimeEntry_t *pTimeEntry)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->ptp_txTimestampFifo_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    /* function body */
    retVal = RT_MAPPER->ptp_txTimestampFifo_get(unit, pTimeEntry);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_ptp_1PPSOutput_set
 * Description:
 *      Set the 1 PPS output configuration of the specified port.
 * Input:
 *      unit        - Unit ID
 *      pulseWidth  - pointer to 1 PPS pulse width
 *      enable      - enable 1 PPS output
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - input parameter out of range
 * Applicable:
 *
 * Note:
 *      None
 * Changes:
 *      [SDK_NEXT_RELEASE_VERSION]
 *          New added function.
 */
rtksw_api_ret_t rtksw_ptp_1PPSOutput_set(rtksw_uint32 unit, rtksw_uint32 pulseWidth, rtksw_enable_t enable)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->ptp_1PPSOutput_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    /* function body */
    retVal = RT_MAPPER->ptp_1PPSOutput_set(unit, pulseWidth, enable);
    RTKSW_API_UNLOCK(unit);
    return retVal;

}

/* Function Name:
 *      rtksw_ptp_1PPSOutput_get
 * Description:
 *      Get the 1 PPS output configuration of the specified port.
 * Input:
 *      unit        - Unit ID
 * Output:
 *      pPulseWidth - pointer to 1 PPS pulse width
 *      pEnable     - pointer to 1 PPS output enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      phy
 * Note:
 *      None
 * Changes:
 *          New added function.
 */
rtksw_api_ret_t rtksw_ptp_1PPSOutput_get(rtksw_uint32 unit, rtksw_uint32 *pPulseWidth, rtksw_enable_t *pEnable)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->ptp_1PPSOutput_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    /* function body */
    retVal = RT_MAPPER->ptp_1PPSOutput_get(unit, pPulseWidth, pEnable);
    RTKSW_API_UNLOCK(unit);
    return retVal;

}

/* Function Name:
 *      rtksw_ptp_todDelay_set
 * Description:
 *      
 * Input:
 *      unit    - Unit ID
 *      toddelay
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - input parameter out of range
 * Applicable:
 *
 * Note:
 *      None
 * Changes:
 *          New added function.
 */
rtksw_api_ret_t rtksw_ptp_todDelay_set(rtksw_uint32 unit, rtksw_uint32 toddelay)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->ptp_todDelay_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    /* function body */
    retVal = RT_MAPPER->ptp_todDelay_set(unit, toddelay);
    RTKSW_API_UNLOCK(unit);
    return retVal;

}


/* Function Name:
 *      rtksw_ptp_todDelay_get
 * Description:
 *     
 * Input:
 *      unit        - Unit ID
 *      port        - port id
 * Output:
 *      toddelay
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 * Changes:
 *          New added function.
 */
rtksw_api_ret_t rtksw_ptp_todDelay_get(rtksw_uint32 unit, rtksw_uint32* toddelay)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->ptp_todDelay_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    /* function body */
    retVal = RT_MAPPER->ptp_todDelay_get(unit, toddelay);
    RTKSW_API_UNLOCK(unit);
    return retVal;

}

/* Function Name:
 *      rtksw_ptp_clockOutput_set
 * Description:
 *      Set the clock output configuration.
 * Input:
 *      unit        - Unit ID
 *      pClkOutput  - pointer to clock output configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - input parameter out of range
 * Note:
 *      None
 * Changes:
 *          New added function.
 */
rtksw_api_ret_t rtksw_ptp_clockOutput_set(rtksw_uint32 unit, rtksw_time_clkOutput_t *pClkOutput)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->ptp_clockOutput_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    /* function body */
    retVal = RT_MAPPER->ptp_clockOutput_set(unit, pClkOutput);
    RTKSW_API_UNLOCK(unit);
    
    return retVal;
}

/* Function Name:
 *      rtksw_ptp_clockOutput_get
 * Description:
 *      Get the clock output configuration.
 * Input:
 *      unit        - Unit ID
 * Output:
 *      pClkOutput  - pointer to clock output configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8218E
 * Note:
 *      None
 * Changes:
 *      [SDK_NEXT_RELEASE_VERSION]
 *          New added function.
 */
rtksw_api_ret_t rtksw_ptp_clockOutput_get(rtksw_uint32 unit, rtksw_time_clkOutput_t *pClkOutput)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->ptp_clockOutput_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    /* function body */
    retVal = RT_MAPPER->ptp_clockOutput_get(unit, pClkOutput);
    RTKSW_API_UNLOCK(unit);
    return retVal;

}

/* Function Name:
 *      rtksw_ptp_portCtrl_set
 * Description:
 *      Set the PTP port ability.
 * Input:
 *      unit        - Unit ID
 *      port        - Port ID
 *      portcfg     - portrole,linkdelay,awaysts
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      phy
 * Note:
 *      None
 * Changes:
 *          New added function.
 */
rtksw_api_ret_t rtksw_ptp_portCtrl_set(rtksw_uint32 unit, rtksw_port_t port,rtksw_ptp_port_ctrl_t *pPortcfg)
{
   rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->ptp_portCtrl_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    /* function body */
    retVal = RT_MAPPER->ptp_portCtrl_set(unit, port, pPortcfg);
    RTKSW_API_UNLOCK(unit);
    return retVal;
}


/* Function Name:
  *      Get the PTP rtksw_ptp_portCtrl_get ability.
 * Description:
 *      Get the PTP port ability..
 * Input:
 *      unit        - Unit ID
 *      port        - Port ID
 * Output:
 *      portcfg
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - input parameter out of range
 * Note:
 *      None
 * Changes:
 *          New added function.
 */
rtksw_api_ret_t rtksw_ptp_portCtrl_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_ptp_port_ctrl_t *pPortcfg)
{
   rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->ptp_portCtrl_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    /* function body */
    retVal = RT_MAPPER->ptp_portCtrl_get(unit, port, pPortcfg);
    RTKSW_API_UNLOCK(unit);
    return retVal;
}

/* Function Name:
 *      rtksw_ptp_intControl_set
 * Description:
 *      Set PTP interrupt trigger status configuration.
 * Input:
 *      unit        - Unit ID
 *      enable      - Interrupt status.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_ENABLE       - Invalid enable input.
 * Note:
 *      The API can set PTP interrupt status configuration.
 *      The interrupt trigger status is shown in the following:
 *          PTP_INT_TYPE_1PPS = 0,
 *          PTP_INT_TYPE_TOD_DONE,
 *          PTP_INT_TYPE_TXTIME_EMPTY,
 */
rtksw_api_ret_t rtksw_ptp_interruptCtrl_set(rtksw_uint32 unit, rtksw_ptp_intrType_t type, rtksw_enable_t enable)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->ptp_interruptCtrl_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->ptp_interruptCtrl_set(unit, type, enable);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_ptp_intControl_get
 * Description:
 *      Get PTP interrupt trigger status configuration.
 * Input:
 *      unit        - Unit ID
 * Output:
 *      pEnable     - Interrupt status.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      The API can get interrupt status configuration.
 *      The interrupt trigger status is shown in the following:
 *          PTP_INT_TYPE_1PPS = 0,
 *          PTP_INT_TYPE_TOD_DONE,
 *          PTP_INT_TYPE_TXTIME_EMPTY,
 */
rtksw_api_ret_t rtksw_ptp_interruptCtrl_get(rtksw_uint32 unit, rtksw_ptp_intrType_t type, rtksw_enable_t *pEnable)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->ptp_interruptCtrl_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->ptp_interruptCtrl_get(unit, type,pEnable);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_ptp_InterruptStatus_set
 * Description:
 *      Get PTP port interrupt trigger status.
 * Input:
 *      unit        - Unit ID
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
 *          PTP_INT_TYPE_1PPS          (Bit0)
 *          PTP_INT_TYPE_TOD_DONE      (Bit1)
 *          PTP_INT_TYPE_TXTIME_EMPTY  (Bit2)
 */
rtksw_api_ret_t rtksw_ptp_InterruptStatus_set(rtksw_uint32 unit, rtksw_ptp_intStatus_t statusMask)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->ptp_InterruptStatus_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->ptp_InterruptStatus_set(unit, statusMask);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}


/* Function Name:
 *      rtksw_ptp_InterruptStatus_get
 * Description:
 *      Get PTP port interrupt trigger status.
 * Input:
 *      unit        - Unit ID
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
 *          PTP_INT_TYPE_1PPS          (Bit0)
 *          PTP_INT_TYPE_TOD_DONE      (Bit1)
 *          PTP_INT_TYPE_TXTIME_EMPTY  (Bit2)
 *
 */
rtksw_api_ret_t rtksw_ptp_InterruptStatus_get(rtksw_uint32 unit, rtksw_ptp_intStatus_t *pStatusMask)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->ptp_InterruptStatus_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->ptp_InterruptStatus_get(unit, pStatusMask);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_ptp_portPktTrap_set
 * Description:
 *      Set PTP packet trap of the specified port.
 * Input:
 *      unit    - Unit ID
 *      port    - port id
 *      type    - PTP message type
 *      enable  - enable PTP action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT     - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
rtksw_api_ret_t rtksw_ptp_portPktTrap_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_ptp_pktType_t type, rtksw_enable_t enable)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->ptp_portPktTrap_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->ptp_portPktTrap_set(unit, port, type, enable);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_ptp_portPktTrap_get
 * Description:
 *      Get PTP packet trap of the specified port.
 * Input:
 *      unit        - Unit ID
 *      port        - port id
 *      type        - PTP message type
 * Output:
 *      *pEnable - enable PTP action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT         - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
rtksw_api_ret_t rtksw_ptp_portPktTrap_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_ptp_pktType_t type, rtksw_enable_t *pEnable)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->ptp_portPktTrap_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->ptp_portPktTrap_get(unit, port, type, pEnable);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_ptp_portPktAction_set
 * Description:
 *      Set PTP packet trap of the specified port.
 * Input:
 *      unit        - Unit ID
 *      port        - port id
 *      type        - Eth or UDP
 *      action      - packet action
 *      priority    - trap packet priority
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT     - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
rtksw_api_ret_t rtksw_ptp_portPktAction_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_ptp_header_t type, rtksw_trapTarget_t action, rtksw_uint32 priority)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->ptp_portPktAction_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->ptp_portPktAction_set(unit, port, type, action, priority);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_ptp_portPktAction_get
 * Description:
 *      Get PTP packet trap of the specified port.
 * Input:
 *      unit    - Unit ID
 *      port    - port id
 *      type    - Eth or UDP
 * Output:
 *      *pAction - packet action
 *      *pPriority - trap packet priority
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT         - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
rtksw_api_ret_t rtksw_ptp_portPktAction_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_ptp_header_t type, rtksw_trapTarget_t *pAction, rtksw_uint32 *pPriority)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->ptp_portPktAction_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->ptp_portPktAction_get(unit, port, type, pAction, pPriority);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_ptp_clkSrcCtrl_set
 * Description:
 *      Set PTP time Clock source selection
 * Input:
 *      unit        - Unit ID
 *      clksrc
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK   - Success
 *      RT_ERR_SMI  - SMI access error
 * Note:
 * 0: internal clock (Internal PLL, 1GMHz)
 * 1: external clock, refer to cfg_ext_clk_src)
 */
rtksw_api_ret_t rtksw_ptp_clkSrcCtrl_set(rtksw_uint32 unit, rtksw_enable_t clksrc)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
    
    if (NULL == RT_MAPPER->ptp_clkSrcCtrl_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    
    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->ptp_clkSrcCtrl_set(unit, clksrc);
    RTKSW_API_UNLOCK(unit);
    
    return retVal;
}

/* Function Name:
 *      rtksw_ptp_clkSrcCtrl_get
 * Description:
 *      Get PTP time Clock source selection
 * Input:
 *      unit        - Unit ID
 *      clksrc
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK   - Success
 *      RT_ERR_SMI  - SMI access error
 * Note:
 * 0: internal clock (Internal PLL, 1GMHz)
 * 1: external clock, refer to cfg_ext_clk_src)
 */
rtksw_api_ret_t rtksw_ptp_clkSrcCtrl_get(rtksw_uint32 unit, rtksw_enable_t *clksrc)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
    
    if (NULL == RT_MAPPER->ptp_clkSrcCtrl_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    
    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->ptp_clkSrcCtrl_get(unit, clksrc);
    RTKSW_API_UNLOCK(unit);
    
    return retVal;
}


/* Function Name:
 *      dal_rtl8371c_ptp_TransEnable_set
 * Description:
 *      Set TX/RX timer value compensation..
 * Input:
 *      unit        - Unit ID
 *      port        - port id
 *      TxImbal     - TX timer value compensation
 *      RxImbal     - RX timer value compensation
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      unit: 1 ns
 */
rtksw_api_ret_t rtksw_ptp_timerCompen_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_uint32 TxImbal,rtksw_uint32 RxImbal)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
    
    if (NULL == RT_MAPPER->ptp_timerCompen_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    
    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->ptp_timerCompen_set(unit, port, TxImbal, RxImbal);
    RTKSW_API_UNLOCK(unit);
    
    return retVal;
}

/* Function Name:
 *      rtksw_ptp_timerCompen_get
 * Description:
 *      Get TX/RX timer value compensation..
 * Input:
 *      unit        - Unit ID
 *      port        - port id
 * Output:
 *      pTxImbal  - TX timer value compensation
 *      pRxImbal  - RX timer value compensation

 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      unit: 1 ns
 */
rtksw_api_ret_t rtksw_ptp_timerCompen_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_uint32 *pTxImbal,rtksw_uint32 *pRxImbal)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
    
    if (NULL == RT_MAPPER->ptp_timerCompen_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    
    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->ptp_timerCompen_get(unit, port, pTxImbal, pRxImbal);
    RTKSW_API_UNLOCK(unit);
    
    return retVal;
}
        

