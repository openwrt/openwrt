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
#include <dal/rtl8371c/dal_rtl8371c_rate.h>
#include <dal/reg.h>
#include <dal/rtl8371c/rtk_rtl8371c_reg_struct.h>

#include <dal/dal_mgmts.h>

/* Function Name:
 *      dal_rtl8371c_rate_shareMeter_set
 * Description:
 *      Set meter configuration
 *  Input:
 *      unit        - Unit ID
 *      index       - shared meter index
 *      type        - shared meter type
 *      rate        - rate of share meter
 *      ifg_include - include IFG or not, ENABLE:include DISABLE:exclude
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_FILTER_METER_ID  - Invalid meter
 *      RT_ERR_RATE             - Invalid rate
 *      RT_ERR_INPUT            - Invalid input parameters
 * Note:
 *      The API can set shared meter rate and ifg include for each meter.
 *      The rate unit is 1 kbps and the range is from 8k to 1048568k if type is RTKSW_METER_TYPE_KBPS and
 *      the granularity of rate is 8 kbps.
 *      The rate unit is packets per second and the range is 1 ~ 0x7FFFF if type is RTKSW_METER_TYPE_PPS.
 *      The ifg_include parameter is used
 *      for rate calculation with/without inter-frame-gap and preamble.
 */
rtksw_api_ret_t dal_rtl8371c_rate_shareMeter_set(rtksw_uint32 unit, rtksw_meter_id_t index, rtksw_meter_type_t type, rtksw_rate_t rate, rtksw_enable_t ifg_include)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 rateLsb;
    rtksw_uint32 rateMsb;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (index > RTKSW_MAX_METER_ID)
        return RT_ERR_FILTER_METER_ID;

    if (type >= RTKSW_METER_TYPE_END)
        return RT_ERR_INPUT;

    if (ifg_include >= RTKSW_ENABLE_END)
        return RT_ERR_INPUT;

    switch (type)
    {
        case RTKSW_METER_TYPE_KBPS:
            if (rate > RTL8371C_QOS_RATE_INPUT_MAX_10G)
                return RT_ERR_RATE ;

            rateLsb = ((rate >> 3) & 0xFFFF);
            rateMsb = ((rate >> 19) & 0xFFFF);

            break;
        case RTKSW_METER_TYPE_PPS:
            if (rate > RTL8371C_QOS_PPS_INPUT_MAX)
                return RT_ERR_RATE ;

            rateLsb = (rate & 0xFFFF);
            rateMsb = ((rate >> 16) & 0xFFFF);

            break;
        default:
            return RT_ERR_INPUT;
    }

    if ((retVal = reg16_array_field_write(unit, RTL8371C_CFG_METER_RATEr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)index, RTL8371C_RATE_15_0f, &rateLsb)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_CFG_METER_RATEr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)index, RTL8371C_RATE_20_16f, &rateMsb)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_CFG_METER_RATEr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)index, RTL8371C_METER_IPGf, (rtksw_uint32 *)&ifg_include)) != RT_ERR_OK)
        return retVal;

    /* Set Type */
    if ((retVal = reg16_array_field_write(unit, RTL8371C_CFG_METER_RATEr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)index, RTL8371C_MODEf, (rtksw_uint32 *)&type)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_rate_shareMeter_get
 * Description:
 *      Get meter configuration
 *  Input:
 *      unit        - Unit ID
 *      index       - shared meter index
 * Output:
 *      pType        - Meter Type
 *      pRate        - pointer of rate of share meter
 *      pIfg_include - include IFG or not, ENABLE:include DISABLE:exclude
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_FILTER_METER_ID  - Invalid meter
 * Note:
 *
 */
rtksw_api_ret_t dal_rtl8371c_rate_shareMeter_get(rtksw_uint32 unit, rtksw_meter_id_t index, rtksw_meter_type_t *pType, rtksw_rate_t *pRate, rtksw_enable_t *pIfg_include)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 rateLsb;
    rtksw_uint32 rateMsb;
    rtksw_uint32 regData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (index > RTKSW_MAX_METER_ID)
        return RT_ERR_FILTER_METER_ID;

    if(NULL == pType)
        return RT_ERR_NULL_POINTER;

    if(NULL == pRate)
        return RT_ERR_NULL_POINTER;

    if(NULL == pIfg_include)
        return RT_ERR_NULL_POINTER;

    /* 21-bits Rate */
    if ((retVal = reg16_array_field_read(unit, RTL8371C_CFG_METER_RATEr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)index, RTL8371C_RATE_15_0f, &rateLsb)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_array_field_read(unit, RTL8371C_CFG_METER_RATEr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)index, RTL8371C_RATE_20_16f, &rateMsb)) != RT_ERR_OK)
        return retVal;

    *pRate = ((rateMsb << 16) & 0x1f0000) | rateLsb;

    /* IFG */
    if ((retVal = reg16_array_field_read(unit, RTL8371C_CFG_METER_RATEr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)index, RTL8371C_METER_IPGf, &regData)) != RT_ERR_OK)
        return retVal;

    *pIfg_include = (regData == 1) ? RTKSW_ENABLED : RTKSW_DISABLED;

    /* Type */
    if ((retVal = reg16_array_field_read(unit, RTL8371C_CFG_METER_RATEr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)index, RTL8371C_MODEf, &regData)) != RT_ERR_OK)
        return retVal;

    *pType = (regData == 0) ? RTKSW_METER_TYPE_KBPS : RTKSW_METER_TYPE_PPS;

    if(*pType == RTKSW_METER_TYPE_KBPS)
        *pRate = *pRate << 3;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_rate_shareMeterBucket_set
 * Description:
 *      Set meter Bucket Size
 *  Input:
 *      unit        - Unit ID
 *      index       - shared meter index
 *      bucket_size - Bucket Size
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_INPUT            - Error Input
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_FILTER_METER_ID  - Invalid meter
 * Note:
 *      The API can set shared meter bucket size.
 */
rtksw_api_ret_t dal_rtl8371c_rate_shareMeterBucket_set(rtksw_uint32 unit, rtksw_meter_id_t index, rtksw_uint32 bucket_size)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (index > RTKSW_MAX_METER_ID)
        return RT_ERR_FILTER_METER_ID;

    if(bucket_size > RTL8371C_METERBUCKETSIZEMAX)
        return RT_ERR_INPUT;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_CFG_METER_BUCKETr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)index, RTL8371C_BUCKET_SIZEf, (rtksw_uint32 *)&bucket_size)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_rate_shareMeterBucket_get
 * Description:
 *      Get meter Bucket Size
 *  Input:
 *      unit        - Unit ID
 *      index       - shared meter index
 * Output:
 *      pBucket_size - Bucket Size
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_FILTER_METER_ID  - Invalid meter
 * Note:
 *      The API can get shared meter bucket size.
 */
rtksw_api_ret_t dal_rtl8371c_rate_shareMeterBucket_get(rtksw_uint32 unit, rtksw_meter_id_t index, rtksw_uint32 *pBucket_size)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (index > RTKSW_MAX_METER_ID)
        return RT_ERR_FILTER_METER_ID;

    if(NULL == pBucket_size)
        return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_array_field_read(unit, RTL8371C_CFG_METER_BUCKETr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)index, RTL8371C_BUCKET_SIZEf, pBucket_size)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_rate_igrBandwidthCtrlRate_set
 * Description:
 *      Set port ingress bandwidth control
 *  Input:
 *      unit        - Unit ID
 *      port        - Port id
 *      rate        - Rate of share meter
 *      ifg_include - include IFG or not, ENABLE:include DISABLE:exclude
 *      fc_enable   - enable flow control or not, ENABLE:use flow control DISABLE:drop
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_ENABLE       - Invalid IFG parameter.
 *      RT_ERR_INBW_RATE    - Invalid ingress rate parameter.
 * Note:
 *      The rate unit is 1 kbps and the range is from 8k to 1048568k. The granularity of rate is 8 kbps.
 *      The ifg_include parameter is used for rate calculation with/without inter-frame-gap and preamble.
 */
rtksw_api_ret_t dal_rtl8371c_rate_igrBandwidthCtrlRate_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_rate_t rate, rtksw_enable_t ifg_include, rtksw_enable_t fc_enable)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 rateLsb;
    rtksw_uint32 rateMsb;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if(ifg_include >= RTKSW_ENABLE_END)
        return RT_ERR_INPUT;

    if(fc_enable >= RTKSW_ENABLE_END)
        return RT_ERR_INPUT;

    if (rtksw_switch_isUsxgPort(unit, port) == RT_ERR_OK)
    {
        if (rate > RTL8371C_QOS_RATE_INPUT_MAX_10G)
            return RT_ERR_QOS_EBW_RATE;
    }
    else if ((rtksw_switch_isHsgPort(unit, port) == RT_ERR_OK) || (rtksw_switch_isUtp2p5gPort(unit, port) == RT_ERR_OK))
    {
        if (rate > RTL8371C_QOS_RATE_INPUT_MAX_HSG)
            return RT_ERR_QOS_EBW_RATE;
    }
    else
    {
        if (rate > RTL8371C_QOS_RATE_INPUT_MAX)
            return RT_ERR_QOS_EBW_RATE ;
    }

    rateLsb = (rate >> 3) & 0xFFFF;
    rateMsb = (rate >> 19) & 0xFFFF;

    /* Rate */
    if ((retVal = reg16_array_field_write(unit, RTL8371C_IGBW_PORT_CTRLr, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_INGRESSBW_RATE_Lf, &rateLsb)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_IGBW_PORT_CTRLr, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_INGRESSBW_RATE_Hf, &rateMsb)) != RT_ERR_OK)
        return retVal;

    /* IFG */
    if ((retVal = reg16_array_field_write(unit, RTL8371C_IGBW_PORT_FC_CTRLr, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_INGRESSBW_IFGf, (rtksw_uint32 *)&ifg_include)) != RT_ERR_OK)
        return retVal;

    /* FC enable */
    if ((retVal = reg16_array_field_write(unit, RTL8371C_IGBW_PORT_FC_CTRLr, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_INGRESSBW_FLOWCTRLf, (rtksw_uint32 *)&fc_enable)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_rate_igrBandwidthCtrlRate_get
 * Description:
 *      Get port ingress bandwidth control
 *  Input:
 *      unit        - Unit ID
 *      port        - Port id
 * Output:
 *      pRate           - Rate of share meter
 *      pIfg_include    - Rate's calculation including IFG, ENABLE:include DISABLE:exclude
 *      pFc_enable      - enable flow control or not, ENABLE:use flow control DISABLE:drop
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *     The rate unit is 1 kbps and the range is from 8k to 1048568k. The granularity of rate is 8 kbps.
 *     The ifg_include parameter is used for rate calculation with/without inter-frame-gap and preamble.
 */
rtksw_api_ret_t dal_rtl8371c_rate_igrBandwidthCtrlRate_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_rate_t *pRate, rtksw_enable_t *pIfg_include, rtksw_enable_t *pFc_enable)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 rateLsb;
    rtksw_uint32 rateMsb;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if(NULL == pRate)
        return RT_ERR_NULL_POINTER;

    if(NULL == pIfg_include)
        return RT_ERR_NULL_POINTER;

    if(NULL == pFc_enable)
        return RT_ERR_NULL_POINTER;

    /* Rate */
    if ((retVal = reg16_array_field_read(unit, RTL8371C_IGBW_PORT_CTRLr, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_INGRESSBW_RATE_Lf, &rateLsb)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_array_field_read(unit, RTL8371C_IGBW_PORT_CTRLr, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_INGRESSBW_RATE_Hf, &rateMsb)) != RT_ERR_OK)
        return retVal;

    *pRate = (((rateMsb << 16) | (rateLsb & 0xFFFF)) << 3);

    /* IFG */
    if ((retVal = reg16_array_field_read(unit, RTL8371C_IGBW_PORT_FC_CTRLr, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_INGRESSBW_IFGf, (rtksw_uint32 *)pIfg_include)) != RT_ERR_OK)
        return retVal;

    /* FC enable */
    if ((retVal = reg16_array_field_read(unit, RTL8371C_IGBW_PORT_FC_CTRLr, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_INGRESSBW_FLOWCTRLf, (rtksw_uint32 *)pFc_enable)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_rate_egrBandwidthCtrlRate_set
 * Description:
 *      Set port egress bandwidth control
 *  Input:
 *      unit        - Unit ID
 *      port        - Port id
 *      rate        - Rate of egress bandwidth
 *      ifg_include - include IFG or not, ENABLE:include DISABLE:exclude
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_QOS_EBW_RATE - Invalid egress bandwidth/rate
 * Note:
 *     The rate unit is 1 kbps and the range is from 8k to 1048568k. The granularity of rate is 8 kbps.
 *     The ifg_include parameter is used for rate calculation with/without inter-frame-gap and preamble.
 */
rtksw_api_ret_t dal_rtl8371c_rate_egrBandwidthCtrlRate_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_rate_t rate,  rtksw_enable_t ifg_include)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 rateLsb;
    rtksw_uint32 rateMsb;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if(ifg_include >= RTKSW_ENABLE_END)
        return RT_ERR_INPUT;

    if (rtksw_switch_isUsxgPort(unit, port) == RT_ERR_OK)
    {
        if (rate > RTL8371C_QOS_RATE_INPUT_MAX_10G)
            return RT_ERR_QOS_EBW_RATE;
    }
    else if ((rtksw_switch_isHsgPort(unit, port) == RT_ERR_OK) || (rtksw_switch_isUtp2p5gPort(unit, port) == RT_ERR_OK))
    {
        if (rate > RTL8371C_QOS_RATE_INPUT_MAX_HSG)
            return RT_ERR_QOS_EBW_RATE;
    }
    else
    {
        if (rate > RTL8371C_QOS_RATE_INPUT_MAX)
            return RT_ERR_QOS_EBW_RATE ;
    }

    rateLsb = (rate >> 3) & 0xFFFF;
    rateMsb = (rate >> 19) & 0xFFFF;

    /* Rate */
    if ((retVal = reg16_array_field_write(unit, RTL8371C_EGRESS_RATEr, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_RATE_Lf, &rateLsb)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_EGRESS_RATEr, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_RATE_Hf, &rateMsb)) != RT_ERR_OK)
        return retVal;

    /* IFG */
    if ((retVal = reg16_field_write(unit, RTL8371C_WFQ_CFGr, RTL8371C_WFQ_IFGf, (rtksw_uint32 *)&ifg_include)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_rate_egrBandwidthCtrlRate_get
 * Description:
 *      Get port egress bandwidth control
 *  Input:
 *      unit        - Unit ID
 *      port        - Port id
 * Output:
 *      pRate           - Rate of egress bandwidth
 *      pIfg_include    - Rate's calculation including IFG, ENABLE:include DISABLE:exclude
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *     The rate unit is 1 kbps and the range is from 8k to 1048568k. The granularity of rate is 8 kbps.
 *     The ifg_include parameter is used for rate calculation with/without inter-frame-gap and preamble.
 */
rtksw_api_ret_t dal_rtl8371c_rate_egrBandwidthCtrlRate_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_rate_t *pRate, rtksw_enable_t *pIfg_include)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 rateLsb;
    rtksw_uint32 rateMsb;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if(NULL == pRate)
        return RT_ERR_NULL_POINTER;

    if(NULL == pIfg_include)
        return RT_ERR_NULL_POINTER;

    /* Rate */
    if ((retVal = reg16_array_field_read(unit, RTL8371C_EGRESS_RATEr, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_RATE_Lf, &rateLsb)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_array_field_read(unit, RTL8371C_EGRESS_RATEr, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_RATE_Hf, &rateMsb)) != RT_ERR_OK)
        return retVal;

    *pRate = (((rateMsb << 16) | (rateLsb & 0xFFFF)) << 3);

    /* IFG */
    if ((retVal = reg16_field_write(unit, RTL8371C_WFQ_CFGr, RTL8371C_WFQ_IFGf, (rtksw_uint32 *)pIfg_include)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_rate_egrQueueBwCtrlEnable_set
 * Description:
 *      Set enable status of egress bandwidth control on specified queue.
 *  Input:
 *      unit        - Unit ID
 *      port        - port id
 *      queue       - queue id
 *      enable      - enable status of egress queue bandwidth control
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_QUEUE_ID         - invalid queue id
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
rtksw_api_ret_t dal_rtl8371c_rate_egrQueueBwCtrlEnable_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_qid_t queue, rtksw_enable_t enable)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    /*for whole port function, the queue value should be 0xFF*/
    if (queue != RTKSW_WHOLE_SYSTEM)
        return RT_ERR_QUEUE_ID;

    if (enable >= RTKSW_ENABLE_END)
        return RT_ERR_INPUT;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_APRENr, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_PORT_APR_ENABLEf, (rtksw_uint32 *)&enable)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_rate_egrQueueBwCtrlEnable_get
 * Description:
 *      Get enable status of egress bandwidth control on specified queue.
 *  Input:
 *      unit        - Unit ID
 *      port        - port id
 *      queue       - queue id
 * Output:
 *      pEnable - Pointer to enable status of egress queue bandwidth control
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_QUEUE_ID         - invalid queue id
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
rtksw_api_ret_t dal_rtl8371c_rate_egrQueueBwCtrlEnable_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_qid_t queue, rtksw_enable_t *pEnable)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    /*for whole port function, the queue value should be 0xFF*/
    if (queue != RTKSW_WHOLE_SYSTEM)
        return RT_ERR_QUEUE_ID;

    if(NULL == pEnable)
        return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_array_field_read(unit, RTL8371C_APRENr, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_PORT_APR_ENABLEf, (rtksw_uint32 *)pEnable)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_rate_egrQueueBwCtrlRate_set
 * Description:
 *      Set rate of egress bandwidth control on specified queue.
 *  Input:
 *      unit        - Unit ID
 *      port        - port id
 *      queue       - queue id
 *      index       - shared meter index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_QUEUE_ID         - invalid queue id
 *      RT_ERR_FILTER_METER_ID  - Invalid meter id
 * Note:
 *    The actual rate control is set in shared meters.
 *    The unit of granularity is 8Kbps.
 */
rtksw_api_ret_t dal_rtl8371c_rate_egrQueueBwCtrlRate_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_qid_t queue, rtksw_meter_id_t index)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 offset_idx;
    rtksw_uint32 phy_port;
    rtksw_uint32 field;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if (queue >= RTL8371C_MAX_NUM_OF_QUEUE)
        return RT_ERR_QUEUE_ID;

    if (index > RTKSW_MAX_METER_ID)
        return RT_ERR_FILTER_METER_ID;

    phy_port = rtksw_switch_port_L2P_get(unit, port);

    if (rtksw_switch_isUtpPort(unit, port) == RT_ERR_OK)
    {
        if (index < (phy_port * 8) ||  index > (7 + (phy_port * 8)))
            return RT_ERR_FILTER_METER_ID;

        offset_idx = index - (phy_port * 8);
    }
    else
    {
        if (index < ((phy_port % 4) * 8) ||  index > (7 + ((phy_port % 4) * 8)))
            return RT_ERR_FILTER_METER_ID;

        offset_idx = index - ((phy_port % 4) * 8);
    }

    if (queue == 0)
        field = RTL8371C_QUEUE0_APR_METERf;
    else if (queue == 1)
        field = RTL8371C_QUEUE1_APR_METERf;
    else if (queue == 2)
        field = RTL8371C_QUEUE2_APR_METERf;
    else if (queue == 3)
        field = RTL8371C_QUEUE3_APR_METERf;
    else if (queue == 4)
        field = RTL8371C_QUEUE4_APR_METERf;
    else if (queue == 5)
        field = RTL8371C_QUEUE5_APR_METERf;
    else if (queue == 6)
        field = RTL8371C_QUEUE6_APR_METERf;
    else if (queue == 7)
        field = RTL8371C_QUEUE7_APR_METERf;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_APR_METERr, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, field, (rtksw_uint32 *)&offset_idx)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_rate_egrQueueBwCtrlRate_get
 * Description:
 *      Get rate of egress bandwidth control on specified queue.
 *  Input:
 *      unit        - Unit ID
 *      port        - port id
 *      queue       - queue id
 *      pIndex      - shared meter index
 * Output:
 *      pRate - pointer to rate of egress queue bandwidth control
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_QUEUE_ID         - invalid queue id
 *      RT_ERR_FILTER_METER_ID  - Invalid meter id
 * Note:
 *    The actual rate control is set in shared meters.
 *    The unit of granularity is 8Kbps.
 */
rtksw_api_ret_t dal_rtl8371c_rate_egrQueueBwCtrlRate_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_qid_t queue, rtksw_meter_id_t *pIndex)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 apridx;
    rtksw_uint32 phy_port;
    rtksw_uint32 field;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if (queue >= RTL8371C_MAX_NUM_OF_QUEUE)
        return RT_ERR_QUEUE_ID;

    if(NULL == pIndex)
        return RT_ERR_NULL_POINTER;

    if (queue == 0)
        field = RTL8371C_QUEUE0_APR_METERf;
    else if (queue == 1)
        field = RTL8371C_QUEUE1_APR_METERf;
    else if (queue == 2)
        field = RTL8371C_QUEUE2_APR_METERf;
    else if (queue == 3)
        field = RTL8371C_QUEUE3_APR_METERf;
    else if (queue == 4)
        field = RTL8371C_QUEUE4_APR_METERf;
    else if (queue == 5)
        field = RTL8371C_QUEUE5_APR_METERf;
    else if (queue == 6)
        field = RTL8371C_QUEUE6_APR_METERf;
    else if (queue == 7)
        field = RTL8371C_QUEUE7_APR_METERf;

    if ((retVal = reg16_array_field_read(unit, RTL8371C_APR_METERr, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, field, (rtksw_uint32 *)&apridx)) != RT_ERR_OK)
        return retVal;

    phy_port = rtksw_switch_port_L2P_get(unit, port);
    if (rtksw_switch_isUtpPort(unit, port) == RT_ERR_OK)
        *pIndex = apridx + (phy_port * 8);
    else
        *pIndex = apridx + ((phy_port % 4) * 8);

    return RT_ERR_OK;
}