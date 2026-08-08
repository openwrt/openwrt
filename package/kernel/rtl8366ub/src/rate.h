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

#ifndef __RTKSW_API_RATE_H__
#define __RTKSW_API_RATE_H__

/*
 * Include Files
 */
//#include <rtk_types.h>

/*
 * Data Type Declaration
 */
typedef enum rtksw_meter_type_e{
    RTKSW_METER_TYPE_KBPS = 0,    /* Kbps */
    RTKSW_METER_TYPE_PPS,         /* Packet per second */
    RTKSW_METER_TYPE_END
}rtksw_meter_type_t;


/*
 * Function Declaration
 */

 /* Rate */
/* Function Name:
 *      rtksw_rate_shareMeter_set
 * Description:
 *      Set meter configuration
 * Input:
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
 *      The rate unit is packets per second and the range is 1 ~ 0x1FFF if type is RTKSW_METER_TYPE_PPS.
 *      The ifg_include parameter is used
 *      for rate calculation with/without inter-frame-gap and preamble.
 */
extern rtksw_api_ret_t rtksw_rate_shareMeter_set(rtksw_uint32 unit, rtksw_meter_id_t index, rtksw_meter_type_t type, rtksw_rate_t rate, rtksw_enable_t ifg_include);

/* Function Name:
 *      rtksw_rate_shareMeter_get
 * Description:
 *      Get meter configuration
 * Input:
 *      unit            - Unit ID
 *      index           - shared meter index
 * Output:
 *      pType           - Meter Type
 *      pRate           - pointer of rate of share meter
 *      pIfg_include    - include IFG or not, ENABLE:include DISABLE:exclude
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_FILTER_METER_ID  - Invalid meter
 * Note:
 *
 */
extern rtksw_api_ret_t rtksw_rate_shareMeter_get(rtksw_uint32 unit, rtksw_meter_id_t index, rtksw_meter_type_t *pType, rtksw_rate_t *pRate, rtksw_enable_t *pIfg_include);

/* Function Name:
 *      rtksw_rate_shareMeterBucket_set
 * Description:
 *      Set meter Bucket Size
 * Input:
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
extern rtksw_api_ret_t rtksw_rate_shareMeterBucket_set(rtksw_uint32 unit, rtksw_meter_id_t index, rtksw_uint32 bucket_size);

/* Function Name:
 *      rtksw_rate_shareMeterBucket_get
 * Description:
 *      Get meter Bucket Size
 * Input:
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
extern rtksw_api_ret_t rtksw_rate_shareMeterBucket_get(rtksw_uint32 unit, rtksw_meter_id_t index, rtksw_uint32 *pBucket_size);

/* Function Name:
 *      rtksw_rate_igrBandwidthCtrlRate_set
 * Description:
 *      Set port ingress bandwidth control
 * Input:
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
extern rtksw_api_ret_t rtksw_rate_igrBandwidthCtrlRate_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_rate_t rate, rtksw_enable_t ifg_include, rtksw_enable_t fc_enable);

/* Function Name:
 *      rtksw_rate_igrBandwidthCtrlRate_get
 * Description:
 *      Get port ingress bandwidth control
 * Input:
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
extern rtksw_api_ret_t rtksw_rate_igrBandwidthCtrlRate_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_rate_t *pRate, rtksw_enable_t *pIfg_include, rtksw_enable_t *pFc_enable);

/* Function Name:
 *      rtksw_rate_egrBandwidthCtrlRate_set
 * Description:
 *      Set port egress bandwidth control
 * Input:
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
extern rtksw_api_ret_t rtksw_rate_egrBandwidthCtrlRate_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_rate_t rate,  rtksw_enable_t ifg_include);

/* Function Name:
 *      rtksw_rate_egrBandwidthCtrlRate_get
 * Description:
 *      Get port egress bandwidth control
 * Input:
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
extern rtksw_api_ret_t rtksw_rate_egrBandwidthCtrlRate_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_rate_t *pRate, rtksw_enable_t *pIfg_include);

/* Function Name:
 *      rtksw_rate_egrQueueBwCtrlEnable_get
 * Description:
 *      Get enable status of egress bandwidth control on specified queue.
 * Input:
 *      unit        - Unit ID
 *      unit        - unit id
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
extern rtksw_api_ret_t rtksw_rate_egrQueueBwCtrlEnable_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_qid_t queue, rtksw_enable_t *pEnable);

/* Function Name:
 *      rtksw_rate_egrQueueBwCtrlEnable_set
 * Description:
 *      Set enable status of egress bandwidth control on specified queue.
 * Input:
 *      unit        - Unit ID
 *      port    - port id
 *      queue   - queue id
 *      enable  - enable status of egress queue bandwidth control
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
extern rtksw_api_ret_t rtksw_rate_egrQueueBwCtrlEnable_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_qid_t queue, rtksw_enable_t enable);

/* Function Name:
 *      rtksw_rate_egrQueueBwCtrlRate_get
 * Description:
 *      Get rate of egress bandwidth control on specified queue.
 * Input:
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
extern rtksw_api_ret_t rtksw_rate_egrQueueBwCtrlRate_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_qid_t queue, rtksw_meter_id_t *pIndex);

/* Function Name:
 *      rtksw_rate_egrQueueBwCtrlRate_set
 * Description:
 *      Set rate of egress bandwidth control on specified queue.
 * Input:
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
extern rtksw_api_ret_t rtksw_rate_egrQueueBwCtrlRate_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_qid_t queue, rtksw_meter_id_t index);

#if defined(CONFIG_COMPATIBLE_V1_4)

#define rtk_rate_shareMeter_set(index, type, rate, ifg_include)                     rtksw_rate_shareMeter_set(0, index, type, rate, ifg_include)
#define rtk_rate_shareMeter_get(index, pType, pRate, pIfg_include)                  rtksw_rate_shareMeter_get(0, index, pType, pRate, pIfg_include)
#define rtk_rate_shareMeterBucket_set(index, bucket_size)                           rtksw_rate_shareMeterBucket_set(0, index, bucket_size)
#define rtk_rate_shareMeterBucket_get(index, pBucket_size)                          rtksw_rate_shareMeterBucket_get(0, index, pBucket_size)
#define rtk_rate_igrBandwidthCtrlRate_set(port, rate, ifg_include, fc_enable)       rtksw_rate_igrBandwidthCtrlRate_set(0, port, rate, ifg_include, fc_enable)
#define rtk_rate_igrBandwidthCtrlRate_get(port, pRate, pIfg_include, pFc_enable)    rtksw_rate_igrBandwidthCtrlRate_get(0, port, pRate, pIfg_include, pFc_enable)
#define rtk_rate_egrBandwidthCtrlRate_set(port, rate,  ifg_include)                 rtksw_rate_egrBandwidthCtrlRate_set(0, port, rate,  ifg_include)
#define rtk_rate_egrBandwidthCtrlRate_get(port, pRate, pIfg_include)                rtksw_rate_egrBandwidthCtrlRate_get(0, port, pRate, pIfg_include)
#define rtk_rate_egrQueueBwCtrlEnable_get(port, queue, pEnable)                     rtksw_rate_egrQueueBwCtrlEnable_get(0, port, queue, pEnable)
#define rtk_rate_egrQueueBwCtrlEnable_set(port, queue, enable)                      rtksw_rate_egrQueueBwCtrlEnable_set(0, port, queue, enable)
#define rtk_rate_egrQueueBwCtrlRate_get(port, queue, pIndex)                        rtksw_rate_egrQueueBwCtrlRate_get(0, port, queue, pIndex)
#define rtk_rate_egrQueueBwCtrlRate_set(port, queue, index)                         rtksw_rate_egrQueueBwCtrlRate_set(0, port, queue, index)

#define METER_TYPE_KBPS    RTKSW_METER_TYPE_KBPS  
#define METER_TYPE_PPS     RTKSW_METER_TYPE_PPS       

#define rtk_meter_type_t        rtksw_meter_type_t

#endif /* CONFIG_COMPATIBLE_V1_4 */

#endif /* __RTKSW_API_RATE_H__ */

