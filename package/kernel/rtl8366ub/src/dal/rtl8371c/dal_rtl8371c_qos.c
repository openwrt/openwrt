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
#include <dal/rtl8371c/dal_rtl8371c_qos.h>
#include <dal/reg.h>
#include <dal/rtl8371c/rtk_rtl8371c_reg_struct.h>

#include <dal/dal_mgmts.h>
#include <osal/lib.h>

/* Function Name:
 *      dal_rtl8371c_qos_init
 * Description:
 *      Configure Qos default settings with queue number assigment to each port.
 *  Input:
 *      unit        - Unit ID
 *      queueNum    - Queue number of each port.
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
rtksw_api_ret_t dal_rtl8371c_qos_init(rtksw_uint32 unit, rtksw_queue_num_t queueNum)
{
    CONST_T rtksw_uint32 g_prioritytToQid[8][8]= {
            {0, 0,0,0,0,0,0,0},
            {0, 0,0,0,7,7,7,7},
            {0, 0,0,0,1,1,7,7},
            {0, 0,1,1,2,2,7,7},
            {0, 0,1,1,2,3,7,7},
            {0, 0,1,2,3,4,7,7},
            {0, 0,1,2,3,4,5,7},
            {0, 1,2,3,4,5,6,7}
    };

    CONST_T rtksw_uint32 g_priorityDecision[5] = {0x01,0x10,0x04,0x02,0x08};
    CONST_T rtksw_uint32 g_prioritytRemap[8] = {0,1,2,3,4,5,6,7};

    rtksw_api_ret_t retVal;
    rtksw_uint32 priority;
    rtksw_uint32 port;
    rtksw_uint32 dscp;
    rtksw_qos_pri2queue_t pri2qid;
    rtksw_uint32 index, value;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (queueNum <= 0 || queueNum > RTKSW_MAX_NUM_OF_QUEUE)
        return RT_ERR_QUEUE_NUM;

    /*Set Output Queue Number*/
    RTKSW_SCAN_ALL_LOG_PORT(unit, port)
    {
        if ((retVal = dal_rtl8371c_qos_queueNum_set(unit, port, queueNum)) != RT_ERR_OK)
            return retVal;
    }

    /*Set Priority to Qid*/
    for (priority = 0; priority <= RTKSW_PRIMAX; priority++)
    {
        pri2qid.pri2queue[priority] = g_prioritytToQid[queueNum - 1][priority];    
    }
    if ((retVal = dal_rtl8371c_qos_priMap_set(unit, queueNum, &pri2qid)) != RT_ERR_OK)
        return retVal;

    /*Priority Decision Order*/
    for (index = 0; index < PRIDECTBL_END; index++)
    {
        value = g_priorityDecision[RTL8371C_PRIDEC_PORT];
        if ((retVal = reg16_array_field_write(unit, RTL8371C_PRI_WEIGHTr, REG_ARRAY_INDEX_NONE, index, RTL8371C_QOS_PORT_WEIGHTf, &value)) != RT_ERR_OK)
            return retVal;    
        value = g_priorityDecision[RTL8371C_PRIDEC_1Q];    
        if ((retVal = reg16_array_field_write(unit, RTL8371C_PRI_WEIGHTr, REG_ARRAY_INDEX_NONE, index, RTL8371C_QOS_DOT1Q_WEIGHTf, &value)) != RT_ERR_OK)
            return retVal;    
        value = g_priorityDecision[RTL8371C_PRIDEC_ACL];     
        if ((retVal = reg16_array_field_write(unit, RTL8371C_PRI_WEIGHTr, REG_ARRAY_INDEX_NONE, index, RTL8371C_QOS_ACL_WEIGHTf, &value)) != RT_ERR_OK)
            return retVal;
        value = g_priorityDecision[RTL8371C_PRIDEC_DSCP];        
        if ((retVal = reg16_array_field_write(unit, RTL8371C_PRI_WEIGHTr, REG_ARRAY_INDEX_NONE, index, RTL8371C_QOS_DSCP_WEIGHTf, &value)) != RT_ERR_OK)
            return retVal;    
        value = g_priorityDecision[RTL8371C_PRIDEC_1AD];        
        if ((retVal = reg16_array_field_write(unit, RTL8371C_PRI_WEIGHTr, REG_ARRAY_INDEX_NONE, index, RTL8371C_QOS_SVLAN_WEIGHTf, &value)) != RT_ERR_OK)
            return retVal; 
    }

    /*Set Port-based Priority to 0*/
    RTKSW_SCAN_ALL_LOG_PORT(unit, port)
    {
        if ((retVal = dal_rtl8371c_qos_portPri_set(unit, port, 0)) != RT_ERR_OK)
            return retVal;
    }

    RTKSW_SCAN_ALL_LOG_PORT(unit, port)
    {
        /*Disable 1p Remarking*/
        if ((retVal = dal_rtl8371c_qos_1pRemarkEnable_set(unit, port, RTKSW_DISABLED)) != RT_ERR_OK)
            return retVal;
        /*Disable DSCP Remarking*/
        if ((retVal = dal_rtl8371c_qos_dscpRemarkEnable_set(unit, port, RTKSW_DISABLED)) != RT_ERR_OK)
            return retVal;
    }

    /*Set 1p & DSCP  Priority Remapping & Remarking*/
    for (priority = 0; priority <= RTKSW_PRIMAX; priority++)
    {
        if ((retVal = dal_rtl8371c_qos_1pPriRemap_set(unit, priority, g_prioritytRemap[priority])) != RT_ERR_OK)
            return retVal;

        if ((retVal = dal_rtl8371c_qos_1pRemark_set(unit, priority, 0)) != RT_ERR_OK)
            return retVal;

        if ((retVal = dal_rtl8371c_qos_dscpRemark_set(unit, priority, 0)) != RT_ERR_OK)
            return retVal;
    }

    /*Set DSCP Priority*/
    for (dscp = 0; dscp <= 63; dscp++)
    {
        if ((retVal = dal_rtl8371c_qos_dscpPriRemap_set(unit, dscp, 0)) != RT_ERR_OK)
            return retVal;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_qos_priSel_set
 * Description:
 *      Configure the priority order among different priority mechanism.
 *  Input:
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
 *      - RTL8371C_PRIDEC_PORT
 *      - RTL8371C_PRIDEC_ACL
 *      - RTL8371C_PRIDEC_DSCP
 *      - RTL8371C_PRIDEC_1Q
 *      - RTL8371C_PRIDEC_1AD
 */
rtksw_api_ret_t dal_rtl8371c_qos_priSel_set(rtksw_uint32 unit, rtksw_qos_priDecTbl_t index, rtksw_priority_select_t *pPriDec)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 port_pow;
    rtksw_uint32 dot1q_pow;
    rtksw_uint32 dscp_pow;
    rtksw_uint32 acl_pow;
    rtksw_uint32 svlan_pow;
    rtksw_uint32 i;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (index < 0 || index >= PRIDECTBL_END)
        return RT_ERR_ENTRY_INDEX;

    if (pPriDec->port_pri >= RTL8371C_PRIDEC_END || pPriDec->dot1q_pri >= RTL8371C_PRIDEC_END || pPriDec->acl_pri >= RTL8371C_PRIDEC_END || pPriDec->dscp_pri >= RTL8371C_PRIDEC_END ||
       pPriDec->svlan_pri >= RTL8371C_PRIDEC_END)
        return RT_ERR_QOS_SEL_PRI_SOURCE;

    port_pow = 1;
    for (i = pPriDec->port_pri; i > 0; i--)
        port_pow = (port_pow)*2;

    dot1q_pow = 1;
    for (i = pPriDec->dot1q_pri; i > 0; i--)
        dot1q_pow = (dot1q_pow)*2;

    acl_pow = 1;
    for (i = pPriDec->acl_pri; i > 0; i--)
        acl_pow = (acl_pow)*2;

    dscp_pow = 1;
    for (i = pPriDec->dscp_pri; i > 0; i--)
        dscp_pow = (dscp_pow)*2;

    svlan_pow = 1;
    for (i = pPriDec->svlan_pri; i > 0; i--)
        svlan_pow = (svlan_pow)*2;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_PRI_WEIGHTr, REG_ARRAY_INDEX_NONE, index, RTL8371C_QOS_PORT_WEIGHTf, &port_pow)) != RT_ERR_OK)
        return retVal;    

    if ((retVal = reg16_array_field_write(unit, RTL8371C_PRI_WEIGHTr, REG_ARRAY_INDEX_NONE, index, RTL8371C_QOS_DOT1Q_WEIGHTf, &dot1q_pow)) != RT_ERR_OK)
        return retVal;    

    if ((retVal = reg16_array_field_write(unit, RTL8371C_PRI_WEIGHTr, REG_ARRAY_INDEX_NONE, index, RTL8371C_QOS_ACL_WEIGHTf, &acl_pow)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_PRI_WEIGHTr, REG_ARRAY_INDEX_NONE, index, RTL8371C_QOS_DSCP_WEIGHTf, &dscp_pow)) != RT_ERR_OK)
        return retVal;    

    if ((retVal = reg16_array_field_write(unit, RTL8371C_PRI_WEIGHTr, REG_ARRAY_INDEX_NONE, index, RTL8371C_QOS_SVLAN_WEIGHTf, &svlan_pow)) != RT_ERR_OK)
        return retVal;    

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_qos_priSel_get
 * Description:
 *      Get the priority order configuration among different priority mechanism.
 *  Input:
 *      unit        - Unit ID
 *      index       - Priority decision table index (0~1)
 * Output:
 *      pPriDec - Priority assign for port, dscp, 802.1p, cvlan, svlan, acl based priority decision .
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      ASIC will follow user priority setting of mechanisms to select mapped queue priority for receiving frame.
 *      If two priority mechanisms are the same, the ASIC will chose the highest priority from mechanisms to
 *      assign queue priority to receiving frame.
 *      The priority sources are:
 *      - RTL8371C_PRIDEC_PORT,
 *      - RTL8371C_PRIDEC_ACL,
 *      - RTL8371C_PRIDEC_DSCP,
 *      - RTL8371C_PRIDEC_1Q,
 *      - RTL8371C_PRIDEC_1AD,
 */
rtksw_api_ret_t dal_rtl8371c_qos_priSel_get(rtksw_uint32 unit, rtksw_qos_priDecTbl_t index, rtksw_priority_select_t *pPriDec)
{

    rtksw_api_ret_t retVal;
    rtksw_int32 i;
    rtksw_uint32 port_pow;
    rtksw_uint32 dot1q_pow;
    rtksw_uint32 dscp_pow;
    rtksw_uint32 acl_pow;
    rtksw_uint32 svlan_pow;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (index < 0 || index >= PRIDECTBL_END)
        return RT_ERR_ENTRY_INDEX;

    osal_memset(pPriDec, 0x00, sizeof(rtksw_priority_select_t));

    if ((retVal = reg16_array_field_read(unit, RTL8371C_PRI_WEIGHTr, REG_ARRAY_INDEX_NONE, index, RTL8371C_QOS_PORT_WEIGHTf, &port_pow)) != RT_ERR_OK)
        return retVal;    

    if ((retVal = reg16_array_field_read(unit, RTL8371C_PRI_WEIGHTr, REG_ARRAY_INDEX_NONE, index, RTL8371C_QOS_DOT1Q_WEIGHTf, &dot1q_pow)) != RT_ERR_OK)
        return retVal;    

    if ((retVal = reg16_array_field_read(unit, RTL8371C_PRI_WEIGHTr, REG_ARRAY_INDEX_NONE, index, RTL8371C_QOS_ACL_WEIGHTf, &acl_pow)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_array_field_read(unit, RTL8371C_PRI_WEIGHTr, REG_ARRAY_INDEX_NONE, index, RTL8371C_QOS_DSCP_WEIGHTf, &dscp_pow)) != RT_ERR_OK)
        return retVal;    

    if ((retVal = reg16_array_field_read(unit, RTL8371C_PRI_WEIGHTr, REG_ARRAY_INDEX_NONE, index, RTL8371C_QOS_SVLAN_WEIGHTf, &svlan_pow)) != RT_ERR_OK)
        return retVal;    

    for (i = (RTL8371C_PRIDEC_END - 1); i >= 0; i--)
    {
        if (port_pow & (1 << i))
        {
            pPriDec->port_pri = i;
            break;
        }
    }

    for (i = (RTL8371C_PRIDEC_END - 1); i >= 0; i--)
    {
        if (dot1q_pow & (1 << i))
        {
            pPriDec->dot1q_pri = i;
            break;
        }
    }

    for (i = (RTL8371C_PRIDEC_END - 1); i >= 0; i--)
    {
        if (acl_pow & (1 << i))
        {
            pPriDec->acl_pri = i;
            break;
        }
    }

    for (i = (RTL8371C_PRIDEC_END - 1); i >= 0; i--)
    {
        if (dscp_pow & (1 << i))
        {
            pPriDec->dscp_pri = i;
            break;
        }
    }

    for (i = (RTL8371C_PRIDEC_END - 1); i >= 0; i--)
    {
        if (svlan_pow & (1 << i))
        {
            pPriDec->svlan_pri = i;
            break;
        }
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_qos_1pPriRemap_set
 * Description:
 *      Configure 1Q priorities mapping to internal absolute priority.
 *  Input:
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
rtksw_api_ret_t dal_rtl8371c_qos_1pPriRemap_set(rtksw_uint32 unit, rtksw_pri_t dot1p_pri, rtksw_pri_t int_pri)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (dot1p_pri > RTKSW_PRIMAX || int_pri > RTKSW_PRIMAX)
        return  RT_ERR_VLAN_PRIORITY;

    switch (dot1p_pri)
    {
        case 0:
            if ((retVal = reg16_field_write(unit, RTL8371C_DOT1Q_PRI_REMAP_W0r, RTL8371C_PRIORITY0f, &int_pri)) != RT_ERR_OK)
                return retVal;
            break;
        case 1:
            if ((retVal = reg16_field_write(unit, RTL8371C_DOT1Q_PRI_REMAP_W0r, RTL8371C_PRIORITY1f, &int_pri)) != RT_ERR_OK)
                return retVal;
            break;
        case 2:
            if ((retVal = reg16_field_write(unit, RTL8371C_DOT1Q_PRI_REMAP_W0r, RTL8371C_PRIORITY2f, &int_pri)) != RT_ERR_OK)
                return retVal;
            break;
        case 3:
            if ((retVal = reg16_field_write(unit, RTL8371C_DOT1Q_PRI_REMAP_W0r, RTL8371C_PRIORITY3f, &int_pri)) != RT_ERR_OK)
                return retVal;
            break;
        case 4:
            if ((retVal = reg16_field_write(unit, RTL8371C_DOT1Q_PRI_REMAP_W1r, RTL8371C_PRIORITY4f, &int_pri)) != RT_ERR_OK)
                return retVal;
            break;
        case 5:
            if ((retVal = reg16_field_write(unit, RTL8371C_DOT1Q_PRI_REMAP_W1r, RTL8371C_PRIORITY5f, &int_pri)) != RT_ERR_OK)
                return retVal;
            break;
        case 6:
            if ((retVal = reg16_field_write(unit, RTL8371C_DOT1Q_PRI_REMAP_W1r, RTL8371C_PRIORITY6f, &int_pri)) != RT_ERR_OK)
                return retVal;
            break;
        case 7:
            if ((retVal = reg16_field_write(unit, RTL8371C_DOT1Q_PRI_REMAP_W1r, RTL8371C_PRIORITY7f, &int_pri)) != RT_ERR_OK)
                return retVal;
            break;
        default:
            return RT_ERR_QOS_INT_PRIORITY;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_qos_1pPriRemap_get
 * Description:
 *      Get 1Q priorities mapping to internal absolute priority.
 *  Input:
 *      unit        - Unit ID
 *      dot1p_pri   - 802.1p priority value .
 * Output:
 *      pInt_pri - internal priority value.
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_VLAN_PRIORITY    - Invalid priority.
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority.
 * Note:
 *      Priority of 802.1Q assigment for internal asic priority, and it is uesed for queue usage and packet scheduling.
 */
rtksw_api_ret_t dal_rtl8371c_qos_1pPriRemap_get(rtksw_uint32 unit, rtksw_pri_t dot1p_pri, rtksw_pri_t *pInt_pri)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (dot1p_pri > RTKSW_PRIMAX)
        return  RT_ERR_QOS_INT_PRIORITY;

    switch (dot1p_pri)
    {
        case 0:
            if ((retVal = reg16_field_read(unit, RTL8371C_DOT1Q_PRI_REMAP_W0r, RTL8371C_PRIORITY0f, pInt_pri)) != RT_ERR_OK)
                return retVal;
            break;
        case 1:
            if ((retVal = reg16_field_read(unit, RTL8371C_DOT1Q_PRI_REMAP_W0r, RTL8371C_PRIORITY1f, pInt_pri)) != RT_ERR_OK)
                return retVal;
            break;
        case 2:
            if ((retVal = reg16_field_read(unit, RTL8371C_DOT1Q_PRI_REMAP_W0r, RTL8371C_PRIORITY2f, pInt_pri)) != RT_ERR_OK)
                return retVal;
            break;
        case 3:
            if ((retVal = reg16_field_read(unit, RTL8371C_DOT1Q_PRI_REMAP_W0r, RTL8371C_PRIORITY3f, pInt_pri)) != RT_ERR_OK)
                return retVal;
            break;
        case 4:
            if ((retVal = reg16_field_read(unit, RTL8371C_DOT1Q_PRI_REMAP_W1r, RTL8371C_PRIORITY4f, pInt_pri)) != RT_ERR_OK)
                return retVal;
            break;
        case 5:
            if ((retVal = reg16_field_read(unit, RTL8371C_DOT1Q_PRI_REMAP_W1r, RTL8371C_PRIORITY5f, pInt_pri)) != RT_ERR_OK)
                return retVal;
            break;
        case 6:
            if ((retVal = reg16_field_read(unit, RTL8371C_DOT1Q_PRI_REMAP_W1r, RTL8371C_PRIORITY6f, pInt_pri)) != RT_ERR_OK)
                return retVal;
            break;
        case 7:
            if ((retVal = reg16_field_read(unit, RTL8371C_DOT1Q_PRI_REMAP_W1r, RTL8371C_PRIORITY7f, pInt_pri)) != RT_ERR_OK)
                return retVal;
            break;
        default:
            return RT_ERR_QOS_INT_PRIORITY;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_qos_dscpPriRemap_set
 * Description:
 *      Map dscp value to internal priority.
 *  Input:
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
rtksw_api_ret_t dal_rtl8371c_qos_dscpPriRemap_set(rtksw_uint32 unit, rtksw_dscp_t dscp, rtksw_pri_t int_pri)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (int_pri > RTKSW_PRIMAX )
        return RT_ERR_QOS_INT_PRIORITY;

    if (dscp > RTKSW_DSCPMAX)
        return RT_ERR_QOS_DSCP_VALUE;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_PRI_SEL_REMAP_DSCPr, REG_ARRAY_INDEX_NONE, dscp, RTL8371C_INTPRI_DSCPf, &int_pri)) != RT_ERR_OK)
        return retVal;    

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_qos_dscpPriRemap_get
 * Description:
 *      Get dscp value to internal priority.
 *  Input:
 *      unit    - Unit ID
 *      dscp    - Dscp value of receiving frame
 * Output:
 *      pInt_pri - internal priority value.
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
rtksw_api_ret_t dal_rtl8371c_qos_dscpPriRemap_get(rtksw_uint32 unit, rtksw_dscp_t dscp, rtksw_pri_t *pInt_pri)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (dscp > RTKSW_DSCPMAX)
        return RT_ERR_QOS_DSCP_VALUE;

    if ((retVal = reg16_array_field_read(unit, RTL8371C_PRI_SEL_REMAP_DSCPr, REG_ARRAY_INDEX_NONE, dscp, RTL8371C_INTPRI_DSCPf, pInt_pri)) != RT_ERR_OK)
        return retVal;    

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_qos_portPri_set
 * Description:
 *      Configure priority usage to each port.
 *  Input:
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
rtksw_api_ret_t dal_rtl8371c_qos_portPri_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_pri_t int_pri)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 phy_port;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if (int_pri > RTKSW_PRIMAX )
        return RT_ERR_QOS_INT_PRIORITY;

    phy_port = rtksw_switch_port_L2P_get(unit, port);

    if ((retVal = reg16_array_field_write(unit, RTL8371C_PORT_PRIr, phy_port, REG_ARRAY_INDEX_NONE, RTL8371C_PORT_BASE_PRIf, &int_pri)) != RT_ERR_OK)
        return retVal;    

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_qos_portPri_get
 * Description:
 *      Get priority usage to each port.
 *  Input:
 *      unit    - Unit ID
 *      port    - Port id.
 * Output:
 *      pInt_pri - internal priority value.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      The API can get priority of port assignments for queue usage and packet scheduling.
 */
rtksw_api_ret_t dal_rtl8371c_qos_portPri_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_pri_t *pInt_pri)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 phy_port;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    phy_port = rtksw_switch_port_L2P_get(unit, port);

    if ((retVal = reg16_array_field_read(unit, RTL8371C_PORT_PRIr, phy_port, REG_ARRAY_INDEX_NONE, RTL8371C_PORT_BASE_PRIf, pInt_pri)) != RT_ERR_OK)
        return retVal;    

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_qos_queueNum_set
 * Description:
 *      Set output queue number for each port.
 *  Input:
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
rtksw_api_ret_t dal_rtl8371c_qos_queueNum_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_queue_num_t queue_num)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 phy_port;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if ((0 == queue_num) || (queue_num > RTKSW_MAX_NUM_OF_QUEUE))
        return RT_ERR_FAILED;

    if (RTKSW_MAX_NUM_OF_QUEUE == queue_num)
        queue_num = 0;

    phy_port = rtksw_switch_port_L2P_get(unit, port);

    if ((retVal = reg16_array_field_write(unit, RTL8371C_CFG_QNUMr, phy_port, REG_ARRAY_INDEX_NONE, RTL8371C_QUEUE_NUMBERf, &queue_num)) != RT_ERR_OK)
        return retVal;    

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_qos_queueNum_get
 * Description:
 *      Get output queue number.
 *  Input:
 *      unit    - Unit ID
 *      port    - Port id.
 * Output:
 *      pQueue_num - Mapping queue number
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      The API will return the output queue number of the specified port. The queue number is from 1 to 8.
 */
rtksw_api_ret_t dal_rtl8371c_qos_queueNum_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_queue_num_t *pQueue_num)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 qidx;
    rtksw_uint32 phy_port;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    phy_port = rtksw_switch_port_L2P_get(unit, port);

    if ((retVal = reg16_array_field_read(unit, RTL8371C_CFG_QNUMr, phy_port, REG_ARRAY_INDEX_NONE, RTL8371C_QUEUE_NUMBERf, &qidx)) != RT_ERR_OK)
        return retVal;    

    if (0 == qidx)
        *pQueue_num = 8;
    else
        *pQueue_num = qidx;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_qos_priMap_set
 * Description:
 *      Set output queue number for each port.
 *  Input:
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
rtksw_api_ret_t dal_rtl8371c_qos_priMap_set(rtksw_uint32 unit, rtksw_queue_num_t queue_num, rtksw_qos_pri2queue_t *pPri2qid)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 qidx;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if ((0 == queue_num) || (queue_num > RTKSW_MAX_NUM_OF_QUEUE))
        return RT_ERR_QUEUE_NUM;

    qidx = queue_num - 1;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_CFG_QID_ASSIGNr, REG_ARRAY_INDEX_NONE, qidx, RTL8371C_PRIORITY0_TO_QIDf, &pPri2qid->pri2queue[0])) != RT_ERR_OK)
        return retVal;
    
    if ((retVal = reg16_array_field_write(unit, RTL8371C_CFG_QID_ASSIGNr, REG_ARRAY_INDEX_NONE, qidx, RTL8371C_PRIORITY1_TO_QIDf, &pPri2qid->pri2queue[1])) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_CFG_QID_ASSIGNr, REG_ARRAY_INDEX_NONE, qidx, RTL8371C_PRIORITY2_TO_QIDf, &pPri2qid->pri2queue[2])) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_CFG_QID_ASSIGNr, REG_ARRAY_INDEX_NONE, qidx, RTL8371C_PRIORITY3_TO_QIDf, &pPri2qid->pri2queue[3])) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_CFG_QID_ASSIGNr, REG_ARRAY_INDEX_NONE, qidx, RTL8371C_PRIORITY4_TO_QIDf, &pPri2qid->pri2queue[4])) != RT_ERR_OK)
        return retVal;
    
    if ((retVal = reg16_array_field_write(unit, RTL8371C_CFG_QID_ASSIGNr, REG_ARRAY_INDEX_NONE, qidx, RTL8371C_PRIORITY5_TO_QIDf, &pPri2qid->pri2queue[5])) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_CFG_QID_ASSIGNr, REG_ARRAY_INDEX_NONE, qidx, RTL8371C_PRIORITY6_TO_QIDf, &pPri2qid->pri2queue[6])) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_CFG_QID_ASSIGNr, REG_ARRAY_INDEX_NONE, qidx, RTL8371C_PRIORITY7_TO_QIDf, &pPri2qid->pri2queue[7])) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_qos_priMap_get
 * Description:
 *      Get priority to queue ID mapping table parameters.
 *  Input:
 *      unit        - Unit ID
 *      queue_num   - Queue number usage.
 * Output:
 *      pPri2qid - Priority mapping to queue ID.
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
rtksw_api_ret_t dal_rtl8371c_qos_priMap_get(rtksw_uint32 unit, rtksw_queue_num_t queue_num, rtksw_qos_pri2queue_t *pPri2qid)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 qidx;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if ((0 == queue_num) || (queue_num > RTKSW_MAX_NUM_OF_QUEUE))
        return RT_ERR_QUEUE_NUM;

    qidx = queue_num - 1;

    if ((retVal = reg16_array_field_read(unit, RTL8371C_CFG_QID_ASSIGNr, REG_ARRAY_INDEX_NONE, qidx, RTL8371C_PRIORITY0_TO_QIDf, &pPri2qid->pri2queue[0])) != RT_ERR_OK)
        return retVal;
    
    if ((retVal = reg16_array_field_read(unit, RTL8371C_CFG_QID_ASSIGNr, REG_ARRAY_INDEX_NONE, qidx, RTL8371C_PRIORITY1_TO_QIDf, &pPri2qid->pri2queue[1])) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_array_field_read(unit, RTL8371C_CFG_QID_ASSIGNr, REG_ARRAY_INDEX_NONE, qidx, RTL8371C_PRIORITY2_TO_QIDf, &pPri2qid->pri2queue[2])) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_array_field_read(unit, RTL8371C_CFG_QID_ASSIGNr, REG_ARRAY_INDEX_NONE, qidx, RTL8371C_PRIORITY3_TO_QIDf, &pPri2qid->pri2queue[3])) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_array_field_read(unit, RTL8371C_CFG_QID_ASSIGNr, REG_ARRAY_INDEX_NONE, qidx, RTL8371C_PRIORITY4_TO_QIDf, &pPri2qid->pri2queue[4])) != RT_ERR_OK)
        return retVal;
    
    if ((retVal = reg16_array_field_read(unit, RTL8371C_CFG_QID_ASSIGNr, REG_ARRAY_INDEX_NONE, qidx, RTL8371C_PRIORITY5_TO_QIDf, &pPri2qid->pri2queue[5])) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_array_field_read(unit, RTL8371C_CFG_QID_ASSIGNr, REG_ARRAY_INDEX_NONE, qidx, RTL8371C_PRIORITY6_TO_QIDf, &pPri2qid->pri2queue[6])) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_array_field_read(unit, RTL8371C_CFG_QID_ASSIGNr, REG_ARRAY_INDEX_NONE, qidx, RTL8371C_PRIORITY7_TO_QIDf, &pPri2qid->pri2queue[7])) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_qos_schedulingQueueType_set
 * Description:
 *      Set type of queues in dedicated port.
 *  Input:
 *      unit        - Unit ID
 *      port        - Port id.
 *      pQtype      - The array of queue type.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_ID          - Invalid port number.
 *      RT_ERR_QOS_QUEUE_WEIGHT - Invalid queue weight.
 * Note:
 *      The API can set queue type, strict priority or weight fair queue (WFQ) for
 *      dedicated port for using queues. There are priorities as queue id in strict queues. 
 *      It means strict queue id 5 carrying higher priority than strict queue id 4.
 */
rtksw_api_ret_t dal_rtl8371c_qos_schedulingQueueType_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_qos_queue_type_t *pQtype)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 qid, field;
    rtksw_uint32 phy_port;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    phy_port = rtksw_switch_port_L2P_get(unit, port);

    for (qid = 0; qid < RTKSW_MAX_NUM_OF_QUEUE; qid++)
    {
        if (pQtype->queueType[qid] >= RTKSW_QOS_QUEUE_TYPE_END)
            return RT_ERR_QOS_SCHE_TYPE;
    
        if (0 == qid)
            field = RTL8371C_QUEUE0_TYPEf;
        else if (1 == qid)
            field = RTL8371C_QUEUE1_TYPEf;        
        else if (2 == qid)
            field = RTL8371C_QUEUE2_TYPEf;
        else if (3 == qid)
            field = RTL8371C_QUEUE3_TYPEf;    
        else if (4 == qid)
            field = RTL8371C_QUEUE4_TYPEf;
        else if (5 == qid)
            field = RTL8371C_QUEUE5_TYPEf;        
        else if (6 == qid)
            field = RTL8371C_QUEUE6_TYPEf;
        else if (7 == qid)
            field = RTL8371C_QUEUE7_TYPEf;    

        if ((retVal = reg16_array_field_write(unit, RTL8371C_SCHED_PORT_WFQ_CTRL_SETr, phy_port, REG_ARRAY_INDEX_NONE, field, &pQtype->queueType[qid])) != RT_ERR_OK)
            return retVal;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_qos_schedulingQueueType_get
 * Description:
 *      Get type of queues in dedicated port.
 *  Input:
 *      unit        - Unit ID
 *      port        - Port id.
 * Output:
 *      pQtype   - The array of queue type.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      The API can get queue type (strict priority or weight fair queue (WFQ)) for dedicated port 
 *      for using queues.
 */
rtksw_api_ret_t dal_rtl8371c_qos_schedulingQueueType_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_qos_queue_type_t *pQtype)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 qid,phy_port;
    rtksw_uint32 field;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    phy_port = rtksw_switch_port_L2P_get(unit, port);

    for (qid = 0; qid < RTKSW_MAX_NUM_OF_QUEUE; qid++)
    {
        if (0 == qid)
            field = RTL8371C_QUEUE0_TYPEf;
        else if (1 == qid)
            field = RTL8371C_QUEUE1_TYPEf;        
        else if (2 == qid)
            field = RTL8371C_QUEUE2_TYPEf;
        else if (3 == qid)
            field = RTL8371C_QUEUE3_TYPEf;    
        else if (4 == qid)
            field = RTL8371C_QUEUE4_TYPEf;
        else if (5 == qid)
            field = RTL8371C_QUEUE5_TYPEf;        
        else if (6 == qid)
            field = RTL8371C_QUEUE6_TYPEf;
        else if (7 == qid)
            field = RTL8371C_QUEUE7_TYPEf;    

        if ((retVal = reg16_array_field_read(unit, RTL8371C_SCHED_PORT_WFQ_CTRL_SETr, phy_port, REG_ARRAY_INDEX_NONE, field, &pQtype->queueType[qid])) != RT_ERR_OK)
            return retVal;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_qos_schedulingQueue_set
 * Description:
 *      Set weight of queues in dedicated port.
 *  Input:
 *      unit        - Unit ID
 *      port        - Port id.
 *      pQweights   - The array of weights for WRR/WFQ queue.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_ID          - Invalid port number.
 *      RT_ERR_QOS_QUEUE_WEIGHT - Invalid queue weight.
 * Note:
 *      The API can set weight for dedicated port for using queues. If queue id is not 
 *      included in queue usage, then itsweight setting in dummy for setting. 
 *      The WFQ queue weight is from 1 to 127.
 */
rtksw_api_ret_t dal_rtl8371c_qos_schedulingQueue_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_qos_queue_weights_t *pQweights)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 qid;
    rtksw_uint32 phy_port;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    phy_port = rtksw_switch_port_L2P_get(unit, port);

    for (qid = 0; qid < RTKSW_MAX_NUM_OF_QUEUE; qid++)
    {
        if (qid == 0)
        {
            if (pQweights->weights[qid] > RTL8371C_QOS_Q0_WEIGHT_MAX)
                return RT_ERR_QOS_QUEUE_WEIGHT;
        }
        else
        {
            if (pQweights->weights[qid] > RTL8371C_QOS_WEIGHT_MAX)
                return RT_ERR_QOS_QUEUE_WEIGHT;
        }

        if ((retVal = reg16_array_field_write(unit, RTL8371C_SCHED_PORT_Q_CTRL_SETr, phy_port, qid, RTL8371C_WEIGHTf, &pQweights->weights[qid])) != RT_ERR_OK)
            return retVal;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_qos_schedulingQueue_get
 * Description:
 *      Get weight of queues in dedicated port.
 *  Input:
 *      unit        - Unit ID
 *      port        - Port id.
 * Output:
 *      pQweights - The array of weights for WRR/WFQ queue.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      The API can get weight for dedicated port for using queues.
 *      The WFQ queue weight is from 1 to 127.
 */
rtksw_api_ret_t dal_rtl8371c_qos_schedulingQueue_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_qos_queue_weights_t *pQweights)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 qid,phy_port;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    phy_port = rtksw_switch_port_L2P_get(unit, port);

    for (qid = 0; qid < RTKSW_MAX_NUM_OF_QUEUE; qid++)
    {    
        if ((retVal = reg16_array_field_read(unit, RTL8371C_SCHED_PORT_Q_CTRL_SETr, phy_port, qid, RTL8371C_WEIGHTf, &pQweights->weights[qid])) != RT_ERR_OK)
            return retVal;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_qos_1pRemarkEnable_set
 * Description:
 *      Set 1p Remarking state
 *  Input:
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
rtksw_api_ret_t dal_rtl8371c_qos_1pRemarkEnable_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t enable)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 phy_port;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if (enable >= RTKSW_ENABLE_END)
        return RT_ERR_INPUT;

    phy_port = rtksw_switch_port_L2P_get(unit, port);

    if ((retVal = reg16_array_field_write(unit, RTL8371C_RMK_PORT_CTRLr, phy_port, REG_ARRAY_INDEX_NONE, RTL8371C_DOT1Q_REMARK_ENABLEf, &enable)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_qos_1pRemarkEnable_get
 * Description:
 *      Get 802.1p remarking ability.
 *  Input:
 *      unit    - Unit ID
 *      port    - Port id.
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
rtksw_api_ret_t dal_rtl8371c_qos_1pRemarkEnable_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t *pEnable)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 phy_port;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    phy_port = rtksw_switch_port_L2P_get(unit, port);
    
    if ((retVal = reg16_array_field_read(unit, RTL8371C_RMK_PORT_CTRLr, phy_port, REG_ARRAY_INDEX_NONE, RTL8371C_DOT1Q_REMARK_ENABLEf, pEnable)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_qos_1pRemark_set
 * Description:
 *      Set 802.1p remarking parameter.
 *  Input:
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
rtksw_api_ret_t dal_rtl8371c_qos_1pRemark_set(rtksw_uint32 unit, rtksw_pri_t int_pri, rtksw_pri_t dot1p_pri)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 field;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (int_pri > RTKSW_PRIMAX )
        return RT_ERR_QOS_INT_PRIORITY;

    if (dot1p_pri > RTKSW_PRIMAX)
        return RT_ERR_VLAN_PRIORITY;

    if (0 == int_pri)
        field = RTL8371C_IPRI0f;
    else if (1 == int_pri)
        field = RTL8371C_IPRI1f;          
    else if (2 == int_pri)
        field = RTL8371C_IPRI2f;
    else if (3 == int_pri)
        field = RTL8371C_IPRI3f;      
    else if (4 == int_pri)
        field = RTL8371C_IPRI4f;
    else if (5 == int_pri)
        field = RTL8371C_IPRI5f;          
    else if (6 == int_pri)
        field = RTL8371C_IPRI6f;
    else if (7 == int_pri)
        field = RTL8371C_IPRI7f;    
    
    if ((retVal = reg16_field_write(unit, RTL8371C_RMK_INTPRI2IPRI_CTRLr, field, &dot1p_pri)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_qos_1pRemark_get
 * Description:
 *      Get 802.1p remarking parameter.
 *  Input:
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
rtksw_api_ret_t dal_rtl8371c_qos_1pRemark_get(rtksw_uint32 unit, rtksw_pri_t int_pri, rtksw_pri_t *pDot1p_pri)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 field;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (int_pri > RTKSW_PRIMAX )
        return RT_ERR_QOS_INT_PRIORITY;

    if (0 == int_pri)
        field = RTL8371C_IPRI0f;
    else if (1 == int_pri)
        field = RTL8371C_IPRI1f;          
    else if (2 == int_pri)
        field = RTL8371C_IPRI2f;
    else if (3 == int_pri)
        field = RTL8371C_IPRI3f;      
    else if (4 == int_pri)
        field = RTL8371C_IPRI4f;
    else if (5 == int_pri)
        field = RTL8371C_IPRI5f;          
    else if (6 == int_pri)
        field = RTL8371C_IPRI6f;
    else if (7 == int_pri)
        field = RTL8371C_IPRI7f;    
    
    if ((retVal = reg16_field_read(unit, RTL8371C_RMK_INTPRI2IPRI_CTRLr, field, pDot1p_pri)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_qos_1pRemarkSrcSel_set
 * Description:
 *      Set remarking source of 802.1p remarking.
 *  Input:
 *      unit    - Unit ID
 *      type    - remarking source
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
rtksw_api_ret_t dal_rtl8371c_qos_1pRemarkSrcSel_set(rtksw_uint32 unit, rtksw_qos_1pRmkSrc_t type)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (type >= DOT1P_RMK_SRC_END )
        return RT_ERR_QOS_INT_PRIORITY;

    if ((retVal = reg16_field_write(unit, RTL8371C_RMK_CTRLr, RTL8371C_RMK_1Q_CFG_SELf, &type)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_qos_1pRemarkSrcSel_get
 * Description:
 *      Get remarking source of 802.1p remarking.
 *  Input:
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
rtksw_api_ret_t dal_rtl8371c_qos_1pRemarkSrcSel_get(rtksw_uint32 unit, rtksw_qos_1pRmkSrc_t *pType)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if ((retVal = reg16_field_read(unit, RTL8371C_RMK_CTRLr, RTL8371C_RMK_1Q_CFG_SELf, pType)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_qos_dscpRemarkEnable_set
 * Description:
 *      Set DSCP remarking ability.
 *  Input:
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
rtksw_api_ret_t dal_rtl8371c_qos_dscpRemarkEnable_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t enable)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 phy_port;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if (enable >= RTKSW_ENABLE_END)
        return RT_ERR_INPUT;

    phy_port = rtksw_switch_port_L2P_get(unit, port);

    if ((retVal = reg16_array_field_write(unit, RTL8371C_RMK_PORT_CTRLr, phy_port, REG_ARRAY_INDEX_NONE, RTL8371C_DSCP_RMK_ENf, &enable)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_qos_dscpRemarkEnable_get
 * Description:
 *      Get DSCP remarking ability.
 *  Input:
 *      unit        - Unit ID
 *      port        - Port id.
 * Output:
 *      pEnable - status of DSCP remarking.
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
rtksw_api_ret_t dal_rtl8371c_qos_dscpRemarkEnable_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t *pEnable)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 phy_port;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    phy_port = rtksw_switch_port_L2P_get(unit, port);

    if ((retVal = reg16_array_field_read(unit, RTL8371C_RMK_PORT_CTRLr, phy_port, REG_ARRAY_INDEX_NONE, RTL8371C_DSCP_RMK_ENf, pEnable)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_qos_dscpRemark_set
 * Description:
 *      Set DSCP remarking parameter.
 *  Input:
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
rtksw_api_ret_t dal_rtl8371c_qos_dscpRemark_set(rtksw_uint32 unit, rtksw_pri_t int_pri, rtksw_dscp_t dscp)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (int_pri > RTKSW_PRIMAX )
        return RT_ERR_QOS_INT_PRIORITY;

    if (dscp > RTKSW_DSCPMAX)
        return RT_ERR_QOS_DSCP_VALUE;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_RMK_INTPRI2DSCP_CTRLr, REG_ARRAY_INDEX_NONE, int_pri, RTL8371C_DSCPf, &dscp)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_qos_dscpRemark_get
 * Description:
 *      Get DSCP remarking parameter.
 *  Input:
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

rtksw_api_ret_t dal_rtl8371c_qos_dscpRemark_get(rtksw_uint32 unit, rtksw_pri_t int_pri, rtksw_dscp_t *pDscp)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (int_pri > RTKSW_PRIMAX )
        return RT_ERR_QOS_INT_PRIORITY;

    if ((retVal = reg16_array_field_read(unit, RTL8371C_RMK_INTPRI2DSCP_CTRLr, REG_ARRAY_INDEX_NONE, int_pri, RTL8371C_DSCPf, pDscp)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_qos_dscpRemarkSrcSel_set
 * Description:
 *      Set remarking source of DSCP remarking.
 *  Input:
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
rtksw_api_ret_t dal_rtl8371c_qos_dscpRemarkSrcSel_set(rtksw_uint32 unit, rtksw_qos_dscpRmkSrc_t type)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (type >= DSCP_RMK_SRC_END )
        return RT_ERR_QOS_INT_PRIORITY;

    if (type == DSCP_RMK_SRC_DSCP )
        return RT_ERR_QOS_INT_PRIORITY;

    switch (type)
    {
        case DSCP_RMK_SRC_INT_PRI:
            regData = 0;
            break;
        case DSCP_RMK_SRC_USER_PRI:
            regData = 1;
            break;
        default:
            return RT_ERR_QOS_INT_PRIORITY;
    }

    if ((retVal = reg16_field_write(unit, RTL8371C_RMK_CTRLr, RTL8371C_RMK_DSCP_CFG_SELf, &regData)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_qos_dscpRemarkSrcSel_get
 * Description:
 *      Get remarking source of DSCP remarking.
 *  Input:
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
rtksw_api_ret_t dal_rtl8371c_qos_dscpRemarkSrcSel_get(rtksw_uint32 unit, rtksw_qos_dscpRmkSrc_t *pType)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if ((retVal = reg16_field_read(unit, RTL8371C_RMK_CTRLr, RTL8371C_RMK_DSCP_CFG_SELf, &regData)) != RT_ERR_OK)
        return retVal;

    switch (regData)
    {
        case 0:
            *pType = DSCP_RMK_SRC_INT_PRI;
            break;
        case 1:
            *pType = DSCP_RMK_SRC_USER_PRI;
            break;
        default:
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_qos_schedulingType_set
 * Description:
 *      Set scheduling type.
 *  Input:
 *      unit        - Unit ID
 *      type        - scheduling type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter

 * Note:
 *      The API can configure QoS scheduling type.
 */
rtksw_api_ret_t dal_rtl8371c_qos_schedulingType_set(rtksw_uint32 unit, rtksw_qos_scheduling_type_t type)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (type >= SCHEDULING_TYPE_END )
        return RT_ERR_QOS_SCHE_TYPE;

    if ((retVal = reg16_field_write(unit, RTL8371C_SCH_WRR_OPTr, RTL8371C_CFG_WRR_MODEf, &type)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_qos_schedulingType_get
 * Description:
 *      Get type of scheduling.
 *  Input:
 *      unit        - Unit ID
 * Output:
 *      pType      - scheduling type
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer

 * Note:
 *      The API can get QoS scheduling type
 */
rtksw_api_ret_t dal_rtl8371c_qos_schedulingType_get(rtksw_uint32 unit, rtksw_qos_scheduling_type_t *pType)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if ((retVal = reg16_field_read(unit, RTL8371C_SCH_WRR_OPTr, RTL8371C_CFG_WRR_MODEf, pType)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_qos_portPriSelIndex_set
 * Description:
 *      Configure priority decision index to each port.
 *  Input:
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
rtksw_api_ret_t dal_rtl8371c_qos_portPriSelIndex_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_qos_priDecTbl_t index)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 phy_port;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if (index >= PRIDECTBL_END )
        return RT_ERR_ENTRY_INDEX;

    phy_port = rtksw_switch_port_L2P_get(unit, port);

    if ((retVal = reg16_array_field_write(unit, RTL8371C_PORT_WEIGHT_SELr, phy_port, REG_ARRAY_INDEX_NONE, RTL8371C_WEIGHT_SELf, &index)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_qos_portPriSelIndex_get
 * Description:
 *      Get priority decision index from each port.
 *  Input:
 *      unit        - Unit ID
 *      port        - Port id.
 * Output:
 *      pIndex - priority decision index.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      The API can get priority of port assignments for queue usage and packet scheduling.
 */
rtksw_api_ret_t dal_rtl8371c_qos_portPriSelIndex_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_qos_priDecTbl_t *pIndex)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 phy_port;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    phy_port = rtksw_switch_port_L2P_get(unit, port);

    if ((retVal = reg16_array_field_read(unit, RTL8371C_PORT_WEIGHT_SELr, phy_port, REG_ARRAY_INDEX_NONE, RTL8371C_WEIGHT_SELf, pIndex)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}


