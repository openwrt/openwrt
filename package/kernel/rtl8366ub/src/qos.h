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

#ifndef __RTKSW_API_QOS_H__
#define __RTKSW_API_QOS_H__

#include <rtk_types.h>

/*
 * Data Type Declaration
 */
#define QOS_DEFAULT_TICK_PERIOD                     (19-1)
#define QOS_DEFAULT_BYTE_PER_TOKEN                  34
#define QOS_DEFAULT_LK_THRESHOLD                    (34*3) /* Why use 0x400? */


#define QOS_DEFAULT_INGRESS_BANDWIDTH               0x3FFF /* 0x3FFF => unlimit */
#define QOS_DEFAULT_EGRESS_BANDWIDTH                0x3D08 /*( 0x3D08 + 1) * 64Kbps => 1Gbps*/
#define QOS_DEFAULT_PREIFP                          1
#define QOS_DEFAULT_PACKET_USED_PAGES_FC            0x60
#define QOS_DEFAULT_PACKET_USED_FC_EN               0
#define QOS_DEFAULT_QUEUE_BASED_FC_EN               1

#define QOS_DEFAULT_PRIORITY_SELECT_PORT            8
#define QOS_DEFAULT_PRIORITY_SELECT_1Q              0
#define QOS_DEFAULT_PRIORITY_SELECT_ACL             0
#define QOS_DEFAULT_PRIORITY_SELECT_DSCP            0

#define QOS_DEFAULT_DSCP_MAPPING_PRIORITY           0

#define QOS_DEFAULT_1Q_REMARKING_ABILITY            0
#define QOS_DEFAULT_DSCP_REMARKING_ABILITY          0
#define QOS_DEFAULT_QUEUE_GAP                       20
#define QOS_DEFAULT_QUEUE_NO_MAX                    6
#define QOS_DEFAULT_AVERAGE_PACKET_RATE             0x3FFF
#define QOS_DEFAULT_BURST_SIZE_IN_APR               0x3F
#define QOS_DEFAULT_PEAK_PACKET_RATE                2
#define QOS_DEFAULT_SCHEDULER_ABILITY_APR           1     /*disable*/
#define QOS_DEFAULT_SCHEDULER_ABILITY_PPR           1    /*disable*/
#define QOS_DEFAULT_SCHEDULER_ABILITY_WFQ           1    /*disable*/

#define QOS_WEIGHT_MAX                              127

#define RTKSW_MAX_NUM_OF_PRIORITY                     8
#define RTKSW_MAX_NUM_OF_QUEUE                        8

#define RTKSW_PRIMAX                                             7
#define RTKSW_QIDMAX                                             7
#define RTKSW_DSCPMAX                                         63


/* enum Priority Selection Index */
typedef enum rtksw_qos_priDecTbl_e
{
    PRIDECTBL_IDX0 = 0,
    PRIDECTBL_IDX1,
    PRIDECTBL_END,
}rtksw_qos_priDecTbl_t;


/* Types of 802.1p remarking source */
typedef enum rtksw_qos_1pRmkSrc_e
{
    DOT1P_RMK_SRC_USER_PRI,
    DOT1P_RMK_SRC_TAG_PRI,
    DOT1P_RMK_SRC_END
} rtksw_qos_1pRmkSrc_t;


/* Types of DSCP remarking source */
typedef enum rtksw_qos_dscpRmkSrc_e
{
    DSCP_RMK_SRC_INT_PRI,
    DSCP_RMK_SRC_DSCP,
    DSCP_RMK_SRC_USER_PRI,
    DSCP_RMK_SRC_END
} rtksw_qos_dscpRmkSrc_t;

typedef struct rtksw_priority_select_s
{
    rtksw_uint32 port_pri;
    rtksw_uint32 dot1q_pri;
    rtksw_uint32 acl_pri;
    rtksw_uint32 dscp_pri;
    rtksw_uint32 cvlan_pri;
    rtksw_uint32 svlan_pri;
    rtksw_uint32 dmac_pri;
    rtksw_uint32 smac_pri;
} rtksw_priority_select_t;

typedef struct rtksw_qos_pri2queue_s
{
    rtksw_uint32 pri2queue[RTKSW_MAX_NUM_OF_PRIORITY];
} rtksw_qos_pri2queue_t;

typedef enum rtksw_qos_schedulingQueue_type_e
{
    RTKSW_QOS_QUEUE_TYPE_STRICT = 0, 
    RTKSW_QOS_QUEUE_TYPE_WEIGHTED, 
    RTKSW_QOS_QUEUE_TYPE_END
} rtksw_qos_schedulingQueue_type_t;

typedef struct rtksw_qos_queue_type_s
{
    rtksw_qos_schedulingQueue_type_t  queueType[RTKSW_MAX_NUM_OF_QUEUE];
} rtksw_qos_queue_type_t;

typedef struct rtksw_qos_queue_weights_s
{
    rtksw_uint32 weights[RTKSW_MAX_NUM_OF_QUEUE];
} rtksw_qos_queue_weights_t;

typedef enum rtksw_qos_scheduling_type_e
{
    RTKSW_QOS_WFQ = 0,    /* Weighted-Fair-Queue */
    RTKSW_QOS_WRR,        /* Weighted-Round-Robin */
    SCHEDULING_TYPE_END
} rtksw_qos_scheduling_type_t;

typedef rtksw_uint32  rtksw_queue_num_t;    /* queue number*/

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
extern rtksw_api_ret_t rtksw_qos_init(rtksw_uint32 unit, rtksw_queue_num_t queueNum);

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
extern rtksw_api_ret_t rtksw_qos_priSel_set(rtksw_uint32 unit, rtksw_qos_priDecTbl_t index, rtksw_priority_select_t *pPriDec);

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
extern rtksw_api_ret_t rtksw_qos_priSel_get(rtksw_uint32 unit, rtksw_qos_priDecTbl_t index, rtksw_priority_select_t *pPriDec);

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
extern rtksw_api_ret_t rtksw_qos_1pPriRemap_set(rtksw_uint32 unit, rtksw_pri_t dot1p_pri, rtksw_pri_t int_pri);

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
extern rtksw_api_ret_t rtksw_qos_1pPriRemap_get(rtksw_uint32 unit, rtksw_pri_t dot1p_pri, rtksw_pri_t *pInt_pri);

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
extern rtksw_api_ret_t rtksw_qos_dscpPriRemap_set(rtksw_uint32 unit, rtksw_dscp_t dscp, rtksw_pri_t int_pri);

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
extern rtksw_api_ret_t rtksw_qos_dscpPriRemap_get(rtksw_uint32 unit, rtksw_dscp_t dscp, rtksw_pri_t *pInt_pri);

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
extern rtksw_api_ret_t rtksw_qos_portPri_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_pri_t int_pri);

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
extern rtksw_api_ret_t rtksw_qos_portPri_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_pri_t *pInt_pri);

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
extern rtksw_api_ret_t rtksw_qos_queueNum_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_queue_num_t queue_num);

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
extern rtksw_api_ret_t rtksw_qos_queueNum_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_queue_num_t *pQueue_num);

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
extern rtksw_api_ret_t rtksw_qos_priMap_set(rtksw_uint32 unit, rtksw_queue_num_t queue_num, rtksw_qos_pri2queue_t *pPri2qid);

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
extern rtksw_api_ret_t rtksw_qos_priMap_get(rtksw_uint32 unit, rtksw_queue_num_t queue_num, rtksw_qos_pri2queue_t *pPri2qid);

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
extern rtksw_api_ret_t rtksw_qos_schedulingQueue_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_qos_queue_weights_t *pQweights);

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
extern rtksw_api_ret_t rtksw_qos_schedulingQueue_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_qos_queue_weights_t *pQweights);

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
extern rtksw_api_ret_t rtksw_qos_1pRemarkEnable_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t enable);

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
extern rtksw_api_ret_t rtksw_qos_1pRemarkEnable_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t *pEnable);

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
extern rtksw_api_ret_t rtksw_qos_1pRemark_set(rtksw_uint32 unit, rtksw_pri_t int_pri, rtksw_pri_t dot1p_pri);

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
extern rtksw_api_ret_t rtksw_qos_1pRemark_get(rtksw_uint32 unit, rtksw_pri_t int_pri, rtksw_pri_t *pDot1p_pri);


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
extern rtksw_api_ret_t rtksw_qos_1pRemarkSrcSel_set(rtksw_uint32 unit, rtksw_qos_1pRmkSrc_t type);

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
extern rtksw_api_ret_t rtksw_qos_1pRemarkSrcSel_get(rtksw_uint32 unit, rtksw_qos_1pRmkSrc_t *pType);

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
extern rtksw_api_ret_t rtksw_qos_dscpRemarkEnable_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t enable);

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
extern rtksw_api_ret_t rtksw_qos_dscpRemarkEnable_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t *pEnable);

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
extern rtksw_api_ret_t rtksw_qos_dscpRemark_set(rtksw_uint32 unit, rtksw_pri_t int_pri, rtksw_dscp_t dscp);

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
extern rtksw_api_ret_t rtksw_qos_dscpRemark_get(rtksw_uint32 unit, rtksw_pri_t int_pri, rtksw_dscp_t *pDscp);

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
extern rtksw_api_ret_t rtksw_qos_dscpRemarkSrcSel_set(rtksw_uint32 unit, rtksw_qos_dscpRmkSrc_t type);

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
extern rtksw_api_ret_t rtksw_qos_dscpRemarkSrcSel_get(rtksw_uint32 unit, rtksw_qos_dscpRmkSrc_t *pType);

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
extern rtksw_api_ret_t rtksw_qos_dscpRemark2Dscp_set(rtksw_uint32 unit, rtksw_dscp_t dscp, rtksw_dscp_t rmkDscp);

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
extern rtksw_api_ret_t rtksw_qos_dscpRemark2Dscp_get(rtksw_uint32 unit, rtksw_dscp_t dscp, rtksw_dscp_t *pDscp);

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
extern rtksw_api_ret_t rtksw_qos_portPriSelIndex_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_qos_priDecTbl_t index);

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
extern rtksw_api_ret_t rtksw_qos_portPriSelIndex_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_qos_priDecTbl_t *pIndex);

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
extern rtksw_api_ret_t rtksw_qos_schedulingType_set(rtksw_uint32 unit, rtksw_qos_scheduling_type_t queueType);

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
extern rtksw_api_ret_t rtksw_qos_schedulingType_get(rtksw_uint32 unit, rtksw_qos_scheduling_type_t *pQueueType);

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
extern rtksw_api_ret_t rtksw_qos_schedulingQueueType_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_qos_queue_type_t *pQtype);

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
extern rtksw_api_ret_t rtksw_qos_schedulingQueueType_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_qos_queue_type_t *pQtype);

#if defined(CONFIG_COMPATIBLE_V1_4)

#define rtk_qos_init(queueNum)                              rtksw_qos_init(0, queueNum)
#define rtk_qos_priSel_set(index, pPriDec)                  rtksw_qos_priSel_set(0, index, pPriDec)
#define rtk_qos_priSel_get(index, pPriDec)                  rtksw_qos_priSel_get(0, index, pPriDec)
#define rtk_qos_1pPriRemap_set(dot1p_pri, int_pri)          rtksw_qos_1pPriRemap_set(0, dot1p_pri, int_pri)
#define rtk_qos_1pPriRemap_get(dot1p_pri, pInt_pri)         rtksw_qos_1pPriRemap_get(0, dot1p_pri, pInt_pri)
#define rtk_qos_dscpPriRemap_set(dscp, int_pri)             rtksw_qos_dscpPriRemap_set(0, dscp, int_pri)
#define rtk_qos_dscpPriRemap_get(dscp, pInt_pri)            rtksw_qos_dscpPriRemap_get(0, dscp, pInt_pri)
#define rtk_qos_portPri_set(port, int_pri)                  rtksw_qos_portPri_set(0, port, int_pri)
#define rtk_qos_portPri_get(port, pInt_pri)                 rtksw_qos_portPri_get(0, port, pInt_pri)
#define rtk_qos_queueNum_set(port, queue_num)               rtksw_qos_queueNum_set(0, port, queue_num)
#define rtk_qos_queueNum_get(port, pQueue_num)              rtksw_qos_queueNum_get(0, port, pQueue_num)
#define rtk_qos_priMap_set(queue_num, pPri2qid)             rtksw_qos_priMap_set(0, queue_num, pPri2qid)
#define rtk_qos_priMap_get(queue_num, pPri2qid)             rtksw_qos_priMap_get(0, queue_num, pPri2qid)
#define rtk_qos_schedulingQueue_set(port, pQweights)        rtksw_qos_schedulingQueue_set(0, port, pQweights)
#define rtk_qos_schedulingQueue_get(port, pQweights)        rtksw_qos_schedulingQueue_get(0, port, pQweights)
#define rtk_qos_1pRemarkEnable_set(port, enable)            rtksw_qos_1pRemarkEnable_set(0, port, enable)
#define rtk_qos_1pRemarkEnable_get(port, pEnable)           rtksw_qos_1pRemarkEnable_get(0, port, pEnable)
#define rtk_qos_1pRemark_set(int_pri, dot1p_pri)            rtksw_qos_1pRemark_set(0, int_pri, dot1p_pri)
#define rtk_qos_1pRemark_get(int_pri, pDot1p_pri)           rtksw_qos_1pRemark_get(0, int_pri, pDot1p_pri)
#define rtk_qos_1pRemarkSrcSel_set(type)                    rtksw_qos_1pRemarkSrcSel_set(0, type)
#define rtk_qos_1pRemarkSrcSel_get(pType)                   rtksw_qos_1pRemarkSrcSel_get(0, pType)
#define rtk_qos_dscpRemarkEnable_set(port, enable)          rtksw_qos_dscpRemarkEnable_set(0, port, enable)
#define rtk_qos_dscpRemarkEnable_get(port, pEnable)         rtksw_qos_dscpRemarkEnable_get(0, port, pEnable)
#define rtk_qos_dscpRemark_set(int_pri, dscp)               rtksw_qos_dscpRemark_set(0, int_pri, dscp)
#define rtk_qos_dscpRemark_get(int_pri, pDscp)              rtksw_qos_dscpRemark_get(0, int_pri, pDscp)
#define rtk_qos_dscpRemarkSrcSel_set(type)                  rtksw_qos_dscpRemarkSrcSel_set(0,  type)
#define rtk_qos_dscpRemarkSrcSel_get(pType)                 rtksw_qos_dscpRemarkSrcSel_get(0,  pType)
#define rtk_qos_dscpRemark2Dscp_set(dscp, rmkDscp)          rtksw_qos_dscpRemark2Dscp_set(0, dscp, rmkDscp)
#define rtk_qos_dscpRemark2Dscp_get(dscp, pDscp)            rtksw_qos_dscpRemark2Dscp_get(0, dscp, pDscp)
#define rtk_qos_portPriSelIndex_set(port, index)            rtksw_qos_portPriSelIndex_set(0, port, index)
#define rtk_qos_portPriSelIndex_get(port, pIndex)           rtksw_qos_portPriSelIndex_get(0, port, pIndex)
#define rtk_qos_schedulingType_set(queueType)               rtksw_qos_schedulingType_set(0, queueType)
#define rtk_qos_schedulingType_get(pQueueType)              rtksw_qos_schedulingType_get(0, pQueueType)
#define rtk_qos_schedulingQueueType_set(port, pQtype)       rtksw_qos_schedulingQueueType_set(0, port, pQtype)
#define rtk_qos_schedulingQueueType_get(port, pQtype)       rtksw_qos_schedulingQueueType_get(0, port, pQtype)

#define RTK_MAX_NUM_OF_PRIORITY     RTKSW_MAX_NUM_OF_PRIORITY 
#define RTK_MAX_NUM_OF_QUEUE        RTKSW_MAX_NUM_OF_QUEUE    
#define RTK_PRIMAX                  RTKSW_PRIMAX 
#define RTK_QIDMAX                  RTKSW_QIDMAX 
#define RTK_DSCPMAX                 RTKSW_DSCPMAX

#define RTK_QOS_QUEUE_TYPE_STRICT   RTKSW_QOS_QUEUE_TYPE_STRICT
#define RTK_QOS_QUEUE_TYPE_WEIGHTED RTKSW_QOS_QUEUE_TYPE_WEIGHTED

#define RTK_QOS_WFQ     RTKSW_QOS_WFQ
#define RTK_QOS_WRR     RTKSW_QOS_WRR

#define rtk_qos_priDecTbl_t             rtksw_qos_priDecTbl_t
#define rtk_qos_1pRmkSrc_t              rtksw_qos_1pRmkSrc_t
#define rtk_qos_dscpRmkSrc_t            rtksw_qos_dscpRmkSrc_t
#define rtk_priority_select_t           rtksw_priority_select_t
#define rtk_qos_pri2queue_t             rtksw_qos_pri2queue_t
#define rtk_qos_schedulingQueue_type_t  rtksw_qos_schedulingQueue_type_t
#define rtk_qos_queue_type_t            rtksw_qos_queue_type_t
#define rtk_qos_queue_weights_t         rtksw_qos_queue_weights_t
#define rtk_qos_scheduling_type_t       rtksw_qos_scheduling_type_t
#define rtk_queue_num_t                 rtksw_queue_num_t

#endif /* CONFIG_COMPATIBLE_V1_4 */

#endif /* __RTKSW_API_QOS_H__ */
