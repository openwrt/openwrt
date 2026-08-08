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
#include <qos.h>

#include <dal/dal_mgmts.h>

/* Function Name:
 *      rtksw_qos_init
 * Description:
 *      Configure Qos default settings with queue number assigment to each port.
 * Input:
 *      unit        - Unit ID
 *      queueNum - Queue number of each port.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will initialize related Qos setting with queue number assigment.
 *      The queue number is from 1 to 8.
 */
rtksw_api_ret_t rtksw_qos_init(rtksw_uint32 unit, rtksw_queue_num_t queueNum)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->qos_init)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->qos_init(unit, queueNum);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_qos_priSel_set
 * Description:
 *      Configure the priority order among different priority mechanism.
 * Input:
 *      unit        - Unit ID
 *      index       - Priority decision table index (0~1)
 *      pPriDec     - Priority assign for port, dscp, 802.1p, cvlan, svlan, acl based priority decision.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_QOS_SEL_PRI_SOURCE   - Invalid priority decision source parameter.
 * Note:
 *      ASIC will follow user priority setting of mechanisms to select mapped queue priority for receiving frame.
 *      If two priority mechanisms are the same, the ASIC will chose the highest priority from mechanisms to
 *      assign queue priority to receiving frame.
 *      The priority sources are:
 *      - PRIDEC_PORT
 *      - PRIDEC_ACL
 *      - PRIDEC_DSCP
 *      - PRIDEC_1Q
 *      - PRIDEC_1AD
 *      - PRIDEC_CVLAN
 *      - PRIDEC_DA
 *      - PRIDEC_SA
 */
rtksw_api_ret_t rtksw_qos_priSel_set(rtksw_uint32 unit, rtksw_qos_priDecTbl_t index, rtksw_priority_select_t *pPriDec)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
    
    if (NULL == RT_MAPPER->qos_priSel_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->qos_priSel_set(unit, index, pPriDec);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_qos_priSel_get
 * Description:
 *      Get the priority order configuration among different priority mechanism.
 * Input:
 *      unit        - Unit ID
 *      index       - Priority decision table index (0~1)
 * Output:
 *      pPriDec     - Priority assign for port, dscp, 802.1p, cvlan, svlan, acl based priority decision .
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      ASIC will follow user priority setting of mechanisms to select mapped queue priority for receiving frame.
 *      If two priority mechanisms are the same, the ASIC will chose the highest priority from mechanisms to
 *      assign queue priority to receiving frame.
 *      The priority sources are:
 *      - PRIDEC_PORT,
 *      - PRIDEC_ACL,
 *      - PRIDEC_DSCP,
 *      - PRIDEC_1Q,
 *      - PRIDEC_1AD,
 *      - PRIDEC_CVLAN,
 *      - PRIDEC_DA,
 *      - PRIDEC_SA,
 */
rtksw_api_ret_t rtksw_qos_priSel_get(rtksw_uint32 unit, rtksw_qos_priDecTbl_t index, rtksw_priority_select_t *pPriDec)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
    
    if (NULL == RT_MAPPER->qos_priSel_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->qos_priSel_get(unit, index, pPriDec);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_qos_1pPriRemap_set
 * Description:
 *      Configure 1Q priorities mapping to internal absolute priority.
 * Input:
 *      unit        - Unit ID
 *      dot1p_pri   - 802.1p priority value.
 *      int_pri     - internal priority value.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_INPUT            - Invalid input parameters.
 *      RT_ERR_VLAN_PRIORITY    - Invalid 1p priority.
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority.
 * Note:
 *      Priority of 802.1Q assignment for internal asic priority, and it is used for queue usage and packet scheduling.
 */
rtksw_api_ret_t rtksw_qos_1pPriRemap_set(rtksw_uint32 unit, rtksw_pri_t dot1p_pri, rtksw_pri_t int_pri)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
    
    if (NULL == RT_MAPPER->qos_1pPriRemap_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->qos_1pPriRemap_set(unit, dot1p_pri, int_pri);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_qos_1pPriRemap_get
 * Description:
 *      Get 1Q priorities mapping to internal absolute priority.
 * Input:
 *      unit        - Unit ID
 *      dot1p_pri   - 802.1p priority value .
 * Output:
 *      pInt_pri    - internal priority value.
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_VLAN_PRIORITY    - Invalid priority.
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority.
 * Note:
 *      Priority of 802.1Q assigment for internal asic priority, and it is uesed for queue usage and packet scheduling.
 */
rtksw_api_ret_t rtksw_qos_1pPriRemap_get(rtksw_uint32 unit, rtksw_pri_t dot1p_pri, rtksw_pri_t *pInt_pri)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
    
    if (NULL == RT_MAPPER->qos_1pPriRemap_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->qos_1pPriRemap_get(unit, dot1p_pri, pInt_pri);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_qos_dscpPriRemap_set
 * Description:
 *      Map dscp value to internal priority.
 * Input:
 *      unit        - Unit ID
 *      dscp        - Dscp value of receiving frame
 *      int_pri     - internal priority value .
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_INPUT            - Invalid input parameters.
 *      RT_ERR_QOS_DSCP_VALUE   - Invalid DSCP value.
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority.
 * Note:
 *      The Differentiated Service Code Point is a selector for router's per-hop behaviors. As a selector, there is no implication that a numerically
 *      greater DSCP implies a better network service. As can be seen, the DSCP totally overlaps the old precedence field of TOS. So if values of
 *      DSCP are carefully chosen then backward compatibility can be achieved.
 */
rtksw_api_ret_t rtksw_qos_dscpPriRemap_set(rtksw_uint32 unit, rtksw_dscp_t dscp, rtksw_pri_t int_pri)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
    
    if (NULL == RT_MAPPER->qos_dscpPriRemap_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->qos_dscpPriRemap_set(unit, dscp, int_pri);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_qos_dscpPriRemap_get
 * Description:
 *      Get dscp value to internal priority.
 * Input:
 *      unit        - Unit ID
 *      dscp        - Dscp value of receiving frame
 * Output:
 *      pInt_pri    - internal priority value.
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_QOS_DSCP_VALUE   - Invalid DSCP value.
 * Note:
 *      The Differentiated Service Code Point is a selector for router's per-hop behaviors. As a selector, there is no implication that a numerically
 *      greater DSCP implies a better network service. As can be seen, the DSCP totally overlaps the old precedence field of TOS. So if values of
 *      DSCP are carefully chosen then backward compatibility can be achieved.
 */
rtksw_api_ret_t rtksw_qos_dscpPriRemap_get(rtksw_uint32 unit, rtksw_dscp_t dscp, rtksw_pri_t *pInt_pri)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
    
    if (NULL == RT_MAPPER->qos_dscpPriRemap_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->qos_dscpPriRemap_get(unit, dscp, pInt_pri);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_qos_portPri_set
 * Description:
 *      Configure priority usage to each port.
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
 *      int_pri     - internal priority value.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_ID          - Invalid port number.
 *      RT_ERR_QOS_SEL_PORT_PRI - Invalid port priority.
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority.
 * Note:
 *      The API can set priority of port assignments for queue usage and packet scheduling.
 */
rtksw_api_ret_t rtksw_qos_portPri_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_pri_t int_pri)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
    
    if (NULL == RT_MAPPER->qos_portPri_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->qos_portPri_set(unit, port, int_pri);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_qos_portPri_get
 * Description:
 *      Get priority usage to each port.
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
 * Output:
 *      pInt_pri    - internal priority value.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      The API can get priority of port assignments for queue usage and packet scheduling.
 */
rtksw_api_ret_t rtksw_qos_portPri_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_pri_t *pInt_pri)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
    
    if (NULL == RT_MAPPER->qos_portPri_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->qos_portPri_get(unit, port, pInt_pri);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_qos_queueNum_set
 * Description:
 *      Set output queue number for each port.
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
 *      index       - Mapping queue number (1~8)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 * Note:
 *      The API can set the output queue number of the specified port. The queue number is from 1 to 8.
 */
rtksw_api_ret_t rtksw_qos_queueNum_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_queue_num_t queue_num)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
    
    if (NULL == RT_MAPPER->qos_queueNum_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->qos_queueNum_set(unit, port, queue_num);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_qos_queueNum_get
 * Description:
 *      Get output queue number.
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
 * Output:
 *      pQueue_num  - Mapping queue number
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      The API will return the output queue number of the specified port. The queue number is from 1 to 8.
 */
rtksw_api_ret_t rtksw_qos_queueNum_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_queue_num_t *pQueue_num)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
    
    if (NULL == RT_MAPPER->qos_queueNum_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->qos_queueNum_get(unit, port, pQueue_num);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_qos_priMap_set
 * Description:
 *      Set output queue number for each port.
 * Input:
 *      unit        - Unit ID
 *      queue_num   - Queue number usage.
 *      pPri2qid    - Priority mapping to queue ID.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_INPUT            - Invalid input parameters.
 *      RT_ERR_QUEUE_NUM        - Invalid queue number.
 *      RT_ERR_QUEUE_ID         - Invalid queue id.
 *      RT_ERR_PORT_ID          - Invalid port number.
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority.
 * Note:
 *      ASIC supports priority mapping to queue with different queue number from 1 to 8.
 *      For different queue numbers usage, ASIC supports different internal available queue IDs.
 */
rtksw_api_ret_t rtksw_qos_priMap_set(rtksw_uint32 unit, rtksw_queue_num_t queue_num, rtksw_qos_pri2queue_t *pPri2qid)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
    
    if (NULL == RT_MAPPER->qos_priMap_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->qos_priMap_set(unit, queue_num, pPri2qid);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_qos_priMap_get
 * Description:
 *      Get priority to queue ID mapping table parameters.
 * Input:
 *      unit        - Unit ID
 *      queue_num   - Queue number usage.
 * Output:
 *      pPri2qid    - Priority mapping to queue ID.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 * Note:
 *      The API can return the mapping queue id of the specified priority and queue number.
 *      The queue number is from 1 to 8.
 */
rtksw_api_ret_t rtksw_qos_priMap_get(rtksw_uint32 unit, rtksw_queue_num_t queue_num, rtksw_qos_pri2queue_t *pPri2qid)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
    
    if (NULL == RT_MAPPER->qos_priMap_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->qos_priMap_get(unit, queue_num, pPri2qid);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_qos_schedulingQueue_set
 * Description:
 *      Set weight and type of queues in dedicated port.
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
 *      pQweights   - The array of weights for WRR/WFQ queue (0 for STRICT_PRIORITY queue).
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_ID          - Invalid port number.
 *      RT_ERR_QOS_QUEUE_WEIGHT - Invalid queue weight.
 * Note:
 *      The API can set weight and type, strict priority or weight fair queue (WFQ) for
 *      dedicated port for using queues. If queue id is not included in queue usage,
 *      then its type and weight setting in dummy for setting. There are priorities
 *      as queue id in strict queues. It means strict queue id 5 carrying higher priority
 *      than strict queue id 4. The WFQ queue weight is from 1 to 127, and weight 0 is
 *      for strict priority queue type.
 */
rtksw_api_ret_t rtksw_qos_schedulingQueue_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_qos_queue_weights_t *pQweights)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
    
    if (NULL == RT_MAPPER->qos_schedulingQueue_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->qos_schedulingQueue_set(unit, port, pQweights);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_qos_schedulingQueue_get
 * Description:
 *      Get weight and type of queues in dedicated port.
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
 * Output:
 *      pQweights   - The array of weights for WRR/WFQ queue (0 for STRICT_PRIORITY queue).
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      The API can get weight and type, strict priority or weight fair queue (WFQ) for dedicated port for using queues.
 *      The WFQ queue weight is from 1 to 127, and weight 0 is for strict priority queue type.
 */
rtksw_api_ret_t rtksw_qos_schedulingQueue_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_qos_queue_weights_t *pQweights)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
    
    if (NULL == RT_MAPPER->qos_schedulingQueue_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->qos_schedulingQueue_get(unit, port, pQweights);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_qos_1pRemarkEnable_set
 * Description:
 *      Set 1p Remarking state
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
 *      enable      - State of per-port 1p Remarking
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_ENABLE       - Invalid enable parameter.
 * Note:
 *      The API can enable or disable 802.1p remarking ability for whole system.
 *      The status of 802.1p remark:
 *      - RTKSW_DISABLED
 *      - RTKSW_ENABLED
 */
rtksw_api_ret_t rtksw_qos_1pRemarkEnable_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t enable)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
    
    if (NULL == RT_MAPPER->qos_1pRemarkEnable_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->qos_1pRemarkEnable_set(unit, port, enable);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_qos_1pRemarkEnable_get
 * Description:
 *      Get 802.1p remarking ability.
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
 * Output:
 *      pEnable - Status of 802.1p remark.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      The API can get 802.1p remarking ability.
 *      The status of 802.1p remark:
 *      - RTKSW_DISABLED
 *      - RTKSW_ENABLED
 */
rtksw_api_ret_t rtksw_qos_1pRemarkEnable_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t *pEnable)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
    
    if (NULL == RT_MAPPER->qos_1pRemarkEnable_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->qos_1pRemarkEnable_get(unit, port, pEnable);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_qos_1pRemark_set
 * Description:
 *      Set 802.1p remarking parameter.
 * Input:
 *      unit        - Unit ID
 *      int_pri     - Internal priority value.
 *      dot1p_pri   - 802.1p priority value.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_VLAN_PRIORITY    - Invalid 1p priority.
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority.
 * Note:
 *      The API can set 802.1p parameters source priority and new priority.
 */
rtksw_api_ret_t rtksw_qos_1pRemark_set(rtksw_uint32 unit, rtksw_pri_t int_pri, rtksw_pri_t dot1p_pri)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
    
    if (NULL == RT_MAPPER->qos_1pRemark_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->qos_1pRemark_set(unit, int_pri, dot1p_pri);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_qos_1pRemark_get
 * Description:
 *      Get 802.1p remarking parameter.
 * Input:
 *      unit        - Unit ID
 *      int_pri     - Internal priority value.
 * Output:
 *      pDot1p_pri - 802.1p priority value.
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority.
 * Note:
 *      The API can get 802.1p remarking parameters. It would return new priority of ingress priority.
 */
rtksw_api_ret_t rtksw_qos_1pRemark_get(rtksw_uint32 unit, rtksw_pri_t int_pri, rtksw_pri_t *pDot1p_pri)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
    
    if (NULL == RT_MAPPER->qos_1pRemark_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->qos_1pRemark_get(unit, int_pri, pDot1p_pri);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}


/* Function Name:
 *      rtksw_qos_1pRemarkSrcSel_set
 * Description:
 *      Set remarking source of 802.1p remarking.
 * Input:
 *      unit        - Unit ID
 *      type        - remarking source
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT            - invalid input parameter

 * Note:
 *      The API can configure 802.1p remark functionality to map original 802.1p value or internal
 *      priority to TX DSCP value.
 */
rtksw_api_ret_t rtksw_qos_1pRemarkSrcSel_set(rtksw_uint32 unit, rtksw_qos_1pRmkSrc_t type)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
    
    if (NULL == RT_MAPPER->qos_1pRemarkSrcSel_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->qos_1pRemarkSrcSel_set(unit, type);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_qos_1pRemarkSrcSel_get
 * Description:
 *      Get remarking source of 802.1p remarking.
 * Input:
 *      unit        - Unit ID
 * Output:
 *      pType      - remarking source
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer

 * Note:
 *      None
 */
rtksw_api_ret_t rtksw_qos_1pRemarkSrcSel_get(rtksw_uint32 unit, rtksw_qos_1pRmkSrc_t *pType)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
    
    if (NULL == RT_MAPPER->qos_1pRemarkSrcSel_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->qos_1pRemarkSrcSel_get(unit, pType);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_qos_dscpRemarkEnable_set
 * Description:
 *      Set DSCP remarking ability.
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
 *      enable      - status of DSCP remark.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_ID          - Invalid port number.
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority.
 *      RT_ERR_ENABLE           - Invalid enable parameter.
 * Note:
 *      The API can enable or disable DSCP remarking ability for whole system.
 *      The status of DSCP remark:
 *      - RTKSW_DISABLED
 *      - RTKSW_ENABLED
 */
rtksw_api_ret_t rtksw_qos_dscpRemarkEnable_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t enable)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
    
    if (NULL == RT_MAPPER->qos_dscpRemarkEnable_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->qos_dscpRemarkEnable_set(unit, port, enable);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_qos_dscpRemarkEnable_get
 * Description:
 *      Get DSCP remarking ability.
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
 * Output:
 *      pEnable     - status of DSCP remarking.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      The API can get DSCP remarking ability.
 *      The status of DSCP remark:
 *      - RTKSW_DISABLED
 *      - RTKSW_ENABLED
 */
rtksw_api_ret_t rtksw_qos_dscpRemarkEnable_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t *pEnable)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
    
    if (NULL == RT_MAPPER->qos_dscpRemarkEnable_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->qos_dscpRemarkEnable_get(unit, port, pEnable);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_qos_dscpRemark_set
 * Description:
 *      Set DSCP remarking parameter.
 * Input:
 *      unit        - Unit ID
 *      int_pri     - Internal priority value.
 *      dscp        - DSCP value.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority.
 *      RT_ERR_QOS_DSCP_VALUE   - Invalid DSCP value.
 * Note:
 *      The API can set DSCP value and mapping priority.
 */
rtksw_api_ret_t rtksw_qos_dscpRemark_set(rtksw_uint32 unit, rtksw_pri_t int_pri, rtksw_dscp_t dscp)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
    
    if (NULL == RT_MAPPER->qos_dscpRemark_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->qos_dscpRemark_set(unit, int_pri, dscp);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_qos_dscpRemark_get
 * Description:
 *      Get DSCP remarking parameter.
 * Input:
 *      unit        - Unit ID
 *      int_pri     - Internal priority value.
 * Output:
 *      Dscp - DSCP value.
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority.
 * Note:
 *      The API can get DSCP parameters. It would return DSCP value for mapping priority.
 */
rtksw_api_ret_t rtksw_qos_dscpRemark_get(rtksw_uint32 unit, rtksw_pri_t int_pri, rtksw_dscp_t *pDscp)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
    
    if (NULL == RT_MAPPER->qos_dscpRemark_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->qos_dscpRemark_get(unit, int_pri, pDscp);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_qos_dscpRemarkSrcSel_set
 * Description:
 *      Set remarking source of DSCP remarking.
 * Input:
 *      unit        - Unit ID
 *      type        - remarking source
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT            - invalid input parameter

 * Note:
 *      The API can configure DSCP remark functionality to map original DSCP value or internal
 *      priority to TX DSCP value.
 */
rtksw_api_ret_t rtksw_qos_dscpRemarkSrcSel_set(rtksw_uint32 unit, rtksw_qos_dscpRmkSrc_t type)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
    
    if (NULL == RT_MAPPER->qos_dscpRemarkSrcSel_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->qos_dscpRemarkSrcSel_set(unit, type);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_qos_dscpRemarkSrcSel_get
 * Description:
 *      Get remarking source of DSCP remarking.
 * Input:
 *      unit        - Unit ID
 *      none
 * Output:
 *      pType       - remarking source
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer

 * Note:
 *      None
 */
rtksw_api_ret_t rtksw_qos_dscpRemarkSrcSel_get(rtksw_uint32 unit, rtksw_qos_dscpRmkSrc_t *pType)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
    
    if (NULL == RT_MAPPER->qos_dscpRemarkSrcSel_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->qos_dscpRemarkSrcSel_get(unit, pType);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_qos_dscpRemark2Dscp_set
 * Description:
 *      Set DSCP to remarked DSCP mapping.
 * Input:
 *      unit        - Unit ID
 *      dscp        - DSCP value
 *      rmkDscp     - remarked DSCP value
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QOS_DSCP_VALUE   - Invalid dscp value
 * Note:
 *      dscp parameter can be DSCP value or internal priority according to configuration of API
 *      dal_apollomp_qos_dscpRemarkSrcSel_set(), because DSCP remark functionality can map original DSCP
 *      value or internal priority to TX DSCP value.
 */
rtksw_api_ret_t rtksw_qos_dscpRemark2Dscp_set(rtksw_uint32 unit, rtksw_dscp_t dscp, rtksw_dscp_t rmkDscp)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
    
    if (NULL == RT_MAPPER->qos_dscpRemark2Dscp_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->qos_dscpRemark2Dscp_set(unit, dscp, rmkDscp);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_qos_dscpRemark2Dscp_get
 * Description:
 *      Get DSCP to remarked DSCP mapping.
 * Input:
 *      unit        - Unit ID
 *      dscp        - DSCP value
 * Output:
 *      pDscp       - remarked DSCP value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QOS_DSCP_VALUE   - Invalid dscp value
 *      RT_ERR_NULL_POINTER     - NULL pointer
 * Note:
 *      None.
 */
rtksw_api_ret_t rtksw_qos_dscpRemark2Dscp_get(rtksw_uint32 unit, rtksw_dscp_t dscp, rtksw_dscp_t *pDscp)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
    
    if (NULL == RT_MAPPER->qos_dscpRemark2Dscp_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->qos_dscpRemark2Dscp_get(unit, dscp, pDscp);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_qos_portPriSelIndex_set
 * Description:
 *      Configure priority decision index to each port.
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
 *      index       - priority decision index.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_ID          - Invalid port number.
 *      RT_ERR_ENTRY_INDEX - Invalid entry index.
 * Note:
 *      The API can set priority of port assignments for queue usage and packet scheduling.
 */
rtksw_api_ret_t rtksw_qos_portPriSelIndex_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_qos_priDecTbl_t index)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
    
    if (NULL == RT_MAPPER->qos_portPriSelIndex_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->qos_portPriSelIndex_set(unit, port, index);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_qos_portPriSelIndex_get
 * Description:
 *      Get priority decision index from each port.
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
 * Output:
 *      pIndex      - priority decision index.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      The API can get priority of port assignments for queue usage and packet scheduling.
 */
rtksw_api_ret_t rtksw_qos_portPriSelIndex_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_qos_priDecTbl_t *pIndex)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
    
    if (NULL == RT_MAPPER->qos_portPriSelIndex_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->qos_portPriSelIndex_get(unit, port, pIndex);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_qos_schedulingType_set
 * Description:
 *      Configure type of scheduling.
 * Input:
 *      unit        - Unit ID
 *      queueType   - Scheduling type.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_QOS_SCHE_TYPE    - Invalid QoS scheduling type.
 * Note:
 *      The API can set type of scheduling.
 */
rtksw_api_ret_t rtksw_qos_schedulingType_set(rtksw_uint32 unit, rtksw_qos_scheduling_type_t queueType)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
    
    if (NULL == RT_MAPPER->qos_schedulingType_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->qos_schedulingType_set(unit, queueType);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_qos_schedulingType_get
 * Description:
 *      Get type of scheduling.
 * Input:
 *      unit        - Unit ID
 *      none.
 * Output:
 *      pIndex      - priority decision index.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_NULL_POINTER     - NULL pointer
 * Note:
 *      The API can get type of scheduling.
 */
rtksw_api_ret_t rtksw_qos_schedulingType_get(rtksw_uint32 unit, rtksw_qos_scheduling_type_t *pQueueType)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
    
    if (NULL == RT_MAPPER->qos_schedulingType_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->qos_schedulingType_get(unit, pQueueType);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_qos_schedulingQueueType_set
 * Description:
 *      Set weight and type of queues in dedicated port.
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
 *      pQtype      - The array of queue types for WRR/WFQ or STRICT_PRIORITY.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_ID          - Invalid port number.
 *      RT_ERR_QOS_SCHE_TYPE - Invalid queue weight.
 * Note:
 *      The API can set type, strict priority or weight fair queue (WFQ) for
 *      dedicated port for using queues. If queue id is not included in queue usage,
 *      then its type and weight setting in dummy for setting. There are priorities
 *      as queue id in strict queues. It means strict queue id 5 carrying higher priority
 *      than strict queue id 4.
 */
rtksw_api_ret_t rtksw_qos_schedulingQueueType_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_qos_queue_type_t *pQtype)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
    
    if (NULL == RT_MAPPER->qos_schedulingQueueType_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->qos_schedulingQueueType_set(unit, port, pQtype);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_qos_schedulingQueueType_get
 * Description:
 *      Get type of queues in dedicated port.
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
 * Output:
 *      pQtype - The array of queue types for WRR/WFQ or STRICT_PRIORITY.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      The API can get queue type, strict priority or weight fair queue (WFQ) for dedicated port for using queues.
 */
rtksw_api_ret_t rtksw_qos_schedulingQueueType_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_qos_queue_type_t *pQtype)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
    
    if (NULL == RT_MAPPER->qos_schedulingQueueType_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->qos_schedulingQueueType_get(unit, port, pQtype);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}


