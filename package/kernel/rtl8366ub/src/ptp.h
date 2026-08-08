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

#ifndef __RTKSW_API_PTP_H__
#define __RTKSW_API_PTP_H__


/*
 * Symbol Definition
 */
#define RTKSW_MAX_NUM_OF_NANO_SECOND                     0x3B9AC9FF
#define RTKSW_PTP_INTR_MASK                              0xFF
#define RTKSW_MAX_NUM_OF_TPID                            0xFFFF

/* Message Type */
typedef enum rtksw_ptp_msgType_e
{
    RTKSW_PTP_MSG_TYPE_TX_SYNC = 0,
    RTKSW_PTP_MSG_TYPE_TX_DELAY_REQ,
    RTKSW_PTP_MSG_TYPE_TX_PDELAY_REQ,
    RTKSW_PTP_MSG_TYPE_TX_PDELAY_RESP,
    RTKSW_PTP_MSG_TYPE_RX_SYNC,
    RTKSW_PTP_MSG_TYPE_RX_DELAY_REQ,
    RTKSW_PTP_MSG_TYPE_RX_PDELAY_REQ,
    RTKSW_PTP_MSG_TYPE_RX_PDELAY_RESP,
    RTKSW_PTP_MSG_TYPE_END
} rtksw_ptp_msgType_t;

typedef enum rtksw_ptp_intType_e
{
    RTKSW_PTP_INT_TYPE_TX_SYNC = 0,
    RTKSW_PTP_INT_TYPE_TX_DELAY_REQ,
    RTKSW_PTP_INT_TYPE_TX_PDELAY_REQ,
    RTKSW_PTP_INT_TYPE_TX_PDELAY_RESP,
    RTKSW_PTP_INT_TYPE_RX_SYNC,
    RTKSW_PTP_INT_TYPE_RX_DELAY_REQ,
    RTKSW_PTP_INT_TYPE_RX_PDELAY_REQ,
    RTKSW_PTP_INT_TYPE_RX_PDELAY_RESP,
    RTKSW_PTP_INT_TYPE_ALL,
    RTKSW_PTP_INT_TYPE_END
}rtksw_ptp_intType_t;

typedef enum rtksw_ptp_sys_adjust_e
{
    RTKSW_PTP_SYS_ADJUST_PLUS = 0,
    RTKSW_PTP_SYS_ADJUST_MINUS,
    RTKSW_PTP_SYS_ADJUST_END
} rtksw_ptp_sys_adjust_t;


/* Reference Time */
typedef struct rtksw_ptp_timeStamp_s
{
    rtksw_uint32 sec;
    rtksw_uint32 nsec;
} rtksw_ptp_timeStamp_t;

/* Reference Time 64bit */
typedef struct rtksw_time_timeStamp_s
{
    rtksw_uint64 sec;
    rtksw_uint32 nsec;
} rtksw_time_timeStamp_t;

typedef struct rtksw_ptp_info_s
{
    rtksw_uint32 sequenceId;
    rtksw_ptp_timeStamp_t   timeStamp;
} rtksw_ptp_info_t;

typedef rtksw_uint32 rtksw_ptp_tpid_t;

typedef rtksw_uint32  rtksw_ptp_intStatus_t;     /* interrupt status mask  */

typedef enum rtksw_ptp_intrType_e
{
    RTKSW_PTP_INTR_TYPE_1PPS = 0,
    RTKSW_PTP_INTR_TYPE_TOD_DONE,
    RTKSW_PTP_INTR_TYPE_TXTIME_EMPTY,
    RTKSW_PTP_INTR_TYPE_ALL,    
    RTKSW_PTP_INTR_TYPE_END
}rtksw_ptp_intrType_t;

typedef enum rtksw_ptp_header_e
{
    RTKSW_PTP_ETH = 0,
    RTKSW_PTP_UDP ,
    RTKSW_PTP_END
} rtksw_ptp_header_t;

typedef enum rtksw_ptp_port_role_e
{
    RTKSW_PTP_PORT_NONE= 0,
    RTKSW_PTP_PORT_BCOC,
    RTKSW_PTP_PORT_E2ETC,
    RTKSW_PTP_PORT_P2PTC,
    RTKSW_PTP_PORT_ROLE_ALL,    
    RTKSW_PTP_PORT_END,
}  rtksw_ptp_port_role_t;

typedef struct 
{
    rtksw_ptp_port_role_t portrole;
    rtksw_enable_t utp_en;
    rtksw_enable_t eth_en;
    rtksw_enable_t always_ts_en;
    rtksw_uint32 link_delay;    
} rtksw_ptp_port_ctrl_t;

typedef struct  rtksw_ptp_porttrap_ctrl_s{
    rtksw_uint32 utp_en;
    rtksw_uint32 eth_en;
    rtksw_uint32 ptp_delay_en;
    rtksw_uint32 ptp_pdelay_en;
    rtksw_uint32 ptp_pasm_en;
}rtksw_ptp_porttrap_ctrl_t;


typedef enum rtksw_vlanType_e
{
    RTKSW_PTP_INNER_VLAN = 0,
    RTKSW_PTP_OUTER_VLAN,
    RTKSW_PTP_VLAN_TYPE_END
} rtksw_vlanType_t;

/* Message Type */
typedef enum rtksw_time_ptpMsgType_e
{
    RTKSW_PTP_MSG_TYPE_SYNC = 0,
    RTKSW_PTP_MSG_TYPE_DELAY_REQ = 1,
    RTKSW_PTP_MSG_TYPE_PDELAY_REQ = 2,
    RTKSW_PTP_MSG_TYPE_PDELAY_RESP = 3,
} rtksw_time_ptpMsgType_t;

/* trap packet target */
typedef enum rtksw_trapTarget_e
{
    RTKSW_FORWARD,
    RTKSW_TRAP,
    RTKSW_DROP,
    RTKSW_TRAP_END,
} rtksw_trapTarget_t;

/* TIME packet identifier */
typedef struct rtksw_time_ptpIdentifier_s
{
    rtksw_time_ptpMsgType_t   msgType;
    rtksw_uint32                  sequenceId;
} rtksw_time_ptpIdentifier_t;

typedef enum rtksw_time_oper_e
{
    RTKSW_PTP_TIME_OPER_START = 0,
    RTKSW_PTP_TIME_OPER_STOP,
    RTKSW_PTP_TIME_OPER_LATCH,
    RTKSW_PTP_TIME_OPER_CMD_EXEC,
    RTKSW_PTP_TIME_OPER_FREQ_APPLY,
    RTKSW_PTP_TIME_OPER_END
} rtksw_time_oper_t;


typedef enum rtksw_time_opertriger_e
{
    RTKSW_PTP_TIME_FALL_TRI= 1,
    RTKSW_PTP_TIME_RISE_TRI,
    RTKSW_PTP_TIME_BOTH_TRI,
    RTKSW_TIME_TRI_END
} rtksw_time_opertriger_t;

typedef struct rtksw_time_operCfg_s
{
    rtksw_time_oper_t oper;
    rtksw_enable_t rise_tri;
    rtksw_enable_t fall_tri;
    rtksw_enable_t tri_apply;
} rtksw_time_operCfg_t;

typedef enum rtksw_time_clkOutMode_e
{
    RTKSW_PTP_CLK_OUT_REPEAT = 0,
    RTKSW_PTP_CLK_OUT_PULSE = 1,
    RTKSW_PTP_CLK_OUT_END
} rtksw_time_clkOutMode_t;

typedef enum rtksw_time_outSigSel_e
{
    RTKSW_PTP_OUT_SIG_SEL_CLOCK = 0,
    RTKSW_PTP_OUT_SIG_SEL_1PPS = 1,
    RTKSW_PTP_OUT_SIG_SEL_END
} rtksw_time_outSigSel_t;

/*
 * Data Declaration
 */
typedef struct rtksw_time_txTimeEntry_s
{
    rtksw_uint8 valid;
    rtksw_port_t port;
    rtksw_time_ptpMsgType_t msg_type;
    rtksw_uint32 seqId;
    rtksw_time_timeStamp_t txTime;
} rtksw_time_txTimeEntry_t;

typedef struct rtksw_time_clkOutput_s
{
    rtksw_time_clkOutMode_t mode;
    rtksw_time_timeStamp_t startTime;
    rtksw_uint32 halfPeriodNsec;
    rtksw_enable_t enable;
    rtksw_uint8 runing; //Only valid for get API
} rtksw_time_clkOutput_t;


/* Message Type */
typedef enum rtksw_ptp_pktType_e
{
    RTKSW_PTP_PKT_TYPE_DELAY_CARE,
    RTKSW_PTP_PKT_TYPE_PDELAY_CARE,
    RTKSW_PTP_PKT_TYPE_ASM_CARE,    
    RTKSW_RTK_PTP_PKT_TYPE_END
} rtksw_ptp_pktType_t;


/*
 * Data Declaration
 */

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
extern rtksw_api_ret_t rtksw_ptp_init(rtksw_uint32 unit);

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
extern rtksw_api_ret_t rtksw_ptp_mac_set(rtksw_uint32 unit, rtksw_mac_t mac);

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
extern rtksw_api_ret_t rtksw_ptp_mac_get(rtksw_uint32 unit, rtksw_mac_t *pMac);

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
extern rtksw_api_ret_t rtksw_ptp_tpid_set(rtksw_uint32 unit, rtksw_ptp_tpid_t outerId, rtksw_ptp_tpid_t innerId);

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
extern rtksw_api_ret_t rtksw_ptp_tpid_get(rtksw_uint32 unit, rtksw_ptp_tpid_t *pOuterId, rtksw_ptp_tpid_t *pInnerId);

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
extern rtksw_api_ret_t rtksw_ptp_refTime_set(rtksw_uint32 unit, rtksw_ptp_timeStamp_t timeStamp);

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
extern rtksw_api_ret_t rtksw_ptp_refTime_get(rtksw_uint32 unit, rtksw_ptp_timeStamp_t *pTimeStamp);

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
extern rtksw_api_ret_t rtksw_ptp_refTimeAdjust_set(rtksw_uint32 unit, rtksw_ptp_sys_adjust_t sign, rtksw_ptp_timeStamp_t timeStamp);

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
extern rtksw_api_ret_t rtksw_ptp_refTimeEnable_set(rtksw_uint32 unit, rtksw_enable_t enable);

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
extern rtksw_api_ret_t rtksw_ptp_refTimeEnable_get(rtksw_uint32 unit, rtksw_enable_t *pEnable);

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
extern rtksw_api_ret_t rtksw_ptp_portEnable_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t enable);

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
extern rtksw_api_ret_t rtksw_ptp_portEnable_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t *pEnable);

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
extern rtksw_api_ret_t rtksw_ptp_portTimestamp_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_ptp_msgType_t type, rtksw_ptp_info_t *pInfo);

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
extern rtksw_api_ret_t rtksw_ptp_intControl_set(rtksw_uint32 unit, rtksw_ptp_intType_t type, rtksw_enable_t enable);

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
extern rtksw_api_ret_t rtksw_ptp_intControl_get(rtksw_uint32 unit, rtksw_ptp_intType_t type, rtksw_enable_t *pEnable);

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
extern rtksw_api_ret_t rtksw_ptp_intStatus_get(rtksw_uint32 unit, rtksw_ptp_intStatus_t *pStatusMask);

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
extern rtksw_api_ret_t rtksw_ptp_portIntStatus_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_ptp_intStatus_t statusMask);

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
extern rtksw_api_ret_t rtksw_ptp_portIntStatus_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_ptp_intStatus_t *pStatusMask);

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
extern rtksw_api_ret_t rtksw_ptp_portTrap_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t enable);

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
extern rtksw_api_ret_t rtksw_ptp_portTrap_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t *pEnable);

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
extern rtksw_api_ret_t rtksw_ptp_portBypassEnable_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t enable);

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
extern rtksw_api_ret_t rtksw_ptp_portBypassEnable_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t *pEnable);

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
ret_t rtksw_ptp_portTypeEnable_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_ptp_header_t type,rtksw_enable_t Enable);


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
ret_t rtksw_ptp_portTypeEnable_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_ptp_header_t type,rtksw_enable_t *pEnable);

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
extern rtksw_api_ret_t rtksw_ptp_systemRefTime_set(rtksw_uint32 unit, rtksw_time_timeStamp_t timeStamp,rtksw_enable_t apply);

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
extern rtksw_api_ret_t rtksw_ptp_systemRefTime_get(rtksw_uint32 unit, rtksw_time_timeStamp_t *pTimeStamp);

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
extern rtksw_api_ret_t rtksw_ptp_oper_triger(rtksw_uint32 unit);

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
extern rtksw_api_ret_t rtksw_ptp_systemRefTimeAdjust_set(rtksw_uint32 unit, rtksw_ptp_sys_adjust_t sign, rtksw_time_timeStamp_t timeStamp, rtksw_enable_t apply);

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
extern rtksw_api_ret_t rtksw_ptp_vlanTpid_set(rtksw_uint32 unit, rtksw_vlanType_t type, rtksw_uint32 idx, rtksw_uint32 tpid);

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
extern rtksw_api_ret_t rtksw_ptp_vlanTpid_get(rtksw_uint32 unit, rtksw_vlanType_t type, rtksw_uint32 idx, rtksw_uint32 *pTpid);

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
extern rtksw_api_ret_t rtksw_ptp_operCfg_set(rtksw_uint32 unit, rtksw_time_operCfg_t *pOperCfg);

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
extern rtksw_api_ret_t rtksw_ptp_operCfg_get(rtksw_uint32 unit, rtksw_time_operCfg_t *pOperCfg);

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
extern rtksw_api_ret_t rtksw_ptp_latchTime_get(rtksw_uint32 unit, rtksw_time_timeStamp_t *pLatchTime);

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
extern rtksw_api_ret_t rtksw_ptp_refTimeFreqCfg_set(rtksw_uint32 unit, rtksw_uint32 freq, rtksw_enable_t apply);

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
extern rtksw_api_ret_t rtksw_ptp_refTimeFreqCfg_get(rtksw_uint32 unit, rtksw_uint32 *pFreqCfg, rtksw_uint32 *pFreqCur);

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
extern rtksw_api_ret_t rtksw_ptp_txTimestampFifo_get(rtksw_uint32 unit, rtksw_time_txTimeEntry_t *pTimeEntry);

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
extern rtksw_api_ret_t rtksw_ptp_1PPSOutput_set(rtksw_uint32 unit, rtksw_uint32 pulseWidth, rtksw_enable_t enable);

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
extern rtksw_api_ret_t rtksw_ptp_1PPSOutput_get(rtksw_uint32 unit, rtksw_uint32 *pPulseWidth, rtksw_enable_t *pEnable);

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
extern rtksw_api_ret_t rtksw_ptp_todDelay_set(rtksw_uint32 unit, rtksw_uint32 toddelay);


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
extern rtksw_api_ret_t rtksw_ptp_todDelay_get(rtksw_uint32 unit, rtksw_uint32* toddelay);

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
extern rtksw_api_ret_t rtksw_ptp_clockOutput_set(rtksw_uint32 unit, rtksw_time_clkOutput_t *pClkOutput);

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
extern rtksw_api_ret_t rtksw_ptp_clockOutput_get(rtksw_uint32 unit, rtksw_time_clkOutput_t *pClkOutput);

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
extern rtksw_api_ret_t rtksw_ptp_portCtrl_set(rtksw_uint32 unit, rtksw_port_t port,rtksw_ptp_port_ctrl_t *pPortcfg);


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
extern rtksw_api_ret_t rtksw_ptp_portCtrl_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_ptp_port_ctrl_t *pPortcfg);

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
extern rtksw_api_ret_t rtksw_ptp_interruptCtrl_set(rtksw_uint32 unit, rtksw_ptp_intrType_t type, rtksw_enable_t enable);

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
extern rtksw_api_ret_t rtksw_ptp_interruptCtrl_get(rtksw_uint32 unit, rtksw_ptp_intrType_t type, rtksw_enable_t *pEnable);

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
extern rtksw_api_ret_t rtksw_ptp_InterruptStatus_set(rtksw_uint32 unit, rtksw_ptp_intStatus_t statusMask);


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
extern rtksw_api_ret_t rtksw_ptp_InterruptStatus_get(rtksw_uint32 unit, rtksw_ptp_intStatus_t *pStatusMask);

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
extern rtksw_api_ret_t rtksw_ptp_portPktTrap_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_ptp_pktType_t type, rtksw_enable_t enable);

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
extern rtksw_api_ret_t rtksw_ptp_portPktTrap_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_ptp_pktType_t type, rtksw_enable_t *pEnable);

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
extern rtksw_api_ret_t rtksw_ptp_portPktAction_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_ptp_header_t type, rtksw_trapTarget_t action, rtksw_uint32 priority);

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
extern rtksw_api_ret_t rtksw_ptp_portPktAction_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_ptp_header_t type, rtksw_trapTarget_t *pAction, rtksw_uint32 *pPriority);

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
extern rtksw_api_ret_t rtksw_ptp_clkSrcCtrl_set(rtksw_uint32 unit, rtksw_enable_t clksrc);

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
extern rtksw_api_ret_t rtksw_ptp_clkSrcCtrl_get(rtksw_uint32 unit, rtksw_enable_t *clksrc);


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
extern rtksw_api_ret_t rtksw_ptp_timerCompen_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_uint32 TxImbal,rtksw_uint32 RxImbal);

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
extern rtksw_api_ret_t rtksw_ptp_timerCompen_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_uint32 *pTxImbal,rtksw_uint32 *pRxImbal);

#if defined(CONFIG_COMPATIBLE_V1_4)

#define rtk_ptp_init(void)                                          rtksw_ptp_init(0)
#define rtk_ptp_mac_set(mac)                                        rtksw_ptp_mac_set(0, mac)
#define rtk_ptp_mac_get(pMac)                                       rtksw_ptp_mac_get(0, pMac)
#define rtk_ptp_tpid_set(outerId, innerId)                          rtksw_ptp_tpid_set(0, outerId, innerId)
#define rtk_ptp_tpid_get(pOuterId, pInnerId)                        rtksw_ptp_tpid_get(0, pOuterId, pInnerId)
#define rtk_ptp_refTime_set(timeStamp)                              rtksw_ptp_refTime_set(0, timeStamp)
#define rtk_ptp_refTime_get(pTimeStamp)                             rtksw_ptp_refTime_get(0, pTimeStamp)
#define rtk_ptp_refTimeAdjust_set(sign, timeStamp)                  rtksw_ptp_refTimeAdjust_set(0, sign, timeStamp)
#define rtk_ptp_refTimeEnable_set(enable)                           rtksw_ptp_refTimeEnable_set(0, enable)
#define rtk_ptp_refTimeEnable_get(pEnable)                          rtksw_ptp_refTimeEnable_get(0, pEnable)
#define rtk_ptp_portEnable_set(port, enable)                        rtksw_ptp_portEnable_set(0, port, enable)
#define rtk_ptp_portEnable_get(port, pEnable)                       rtksw_ptp_portEnable_get(0, port, pEnable)
#define rtk_ptp_portTimestamp_get(port, type, pInfo)                rtksw_ptp_portTimestamp_get(0, port, type, pInfo)
#define rtk_ptp_intControl_set(type, enable)                        rtksw_ptp_intControl_set(0, type, enable)
#define rtk_ptp_intControl_get(type, pEnable)                       rtksw_ptp_intControl_get(0, type, pEnable)
#define rtk_ptp_intStatus_get(pStatusMask)                          rtksw_ptp_intStatus_get(0, pStatusMask)
#define rtk_ptp_portIntStatus_set(port, statusMask)                 rtksw_ptp_portIntStatus_set(0, port, statusMask)
#define rtk_ptp_portIntStatus_get(port, pStatusMask)                rtksw_ptp_portIntStatus_get(0, port, pStatusMask)
#define rtk_ptp_portTrap_set(port, enable)                          rtksw_ptp_portTrap_set(0, port, enable)
#define rtk_ptp_portTrap_get(port, pEnable)                         rtksw_ptp_portTrap_get(0, port, pEnable)
#define rtk_ptp_portBypassEnable_set(port, enable)                  rtksw_ptp_portBypassEnable_set(0, port, enable)
#define rtk_ptp_portBypassEnable_get(port, pEnable)                 rtksw_ptp_portBypassEnable_get(0, port, pEnable)
#define rtk_ptp_systemRefTime_set(timeStamp,apply)                  rtksw_ptp_systemRefTime_set(0, timeStamp,apply)
#define rtk_ptp_systemRefTime_get(pTimeStamp)                       rtksw_ptp_systemRefTime_get(0, pTimeStamp)
#define rtk_ptp_oper_triger(void)                                   rtksw_ptp_oper_triger(0)
#define rtk_ptp_systemRefTimeAdjust_set(sign, timeStamp, apply)     rtksw_ptp_systemRefTimeAdjust_set(0, sign, timeStamp, apply)
#define rtk_ptp_vlanTpid_set(type, idx, tpid)                       rtksw_ptp_vlanTpid_set(0, type, idx, tpid)
#define rtk_ptp_vlanTpid_get(type, idx, pTpid)                      rtksw_ptp_vlanTpid_get(0, type, idx, pTpid)
#define rtk_ptp_operCfg_set(pOperCfg)                               rtksw_ptp_operCfg_set(0, pOperCfg)
#define rtk_ptp_operCfg_get(pOperCfg)                               rtksw_ptp_operCfg_get(0, pOperCfg)
#define rtk_ptp_latchTime_get(pLatchTime)                           rtksw_ptp_latchTime_get(0, pLatchTime)
#define rtk_ptp_refTimeFreqCfg_set(freq, apply)                     rtksw_ptp_refTimeFreqCfg_set(0, freq, apply)
#define rtk_ptp_refTimeFreqCfg_get(pFreqCfg, pFreqCur)              rtksw_ptp_refTimeFreqCfg_get(0, pFreqCfg, pFreqCur)
#define rtk_ptp_txTimestampFifo_get(pTimeEntry)                     rtksw_ptp_txTimestampFifo_get(0, pTimeEntry)
#define rtk_ptp_1PPSOutput_set(pulseWidth, enable)                  rtksw_ptp_1PPSOutput_set(0, pulseWidth, enable)
#define rtk_ptp_1PPSOutput_get(pPulseWidth, pEnable)                rtksw_ptp_1PPSOutput_get(0, pPulseWidth, pEnable)
#define rtk_ptp_todDelay_set(toddelay)                              rtksw_ptp_todDelay_set(0, toddelay)
#define rtk_ptp_todDelay_get(toddelay)                              rtksw_ptp_todDelay_get(0, toddelay)
#define rtk_ptp_clockOutput_set(pClkOutput)                         rtksw_ptp_clockOutput_set(0, pClkOutput)
#define rtk_ptp_clockOutput_get(pClkOutput)                         rtksw_ptp_clockOutput_get(0, pClkOutput)
#define rtk_ptp_portCtrl_set(port,pPortcfg)                         rtksw_ptp_portCtrl_set(0, port,pPortcfg)
#define rtk_ptp_portCtrl_get(port, pPortcfg)                        rtksw_ptp_portCtrl_get(0, port, pPortcfg)
#define rtk_ptp_interruptCtrl_set(type, enable)                     rtksw_ptp_interruptCtrl_set(0, type, enable)
#define rtk_ptp_interruptCtrl_get(type, pEnable)                    rtksw_ptp_interruptCtrl_get(0, type, pEnable)
#define rtk_ptp_InterruptStatus_set(statusMask)                     rtksw_ptp_InterruptStatus_set(0, statusMask)
#define rtk_ptp_InterruptStatus_get(pStatusMask)                    rtksw_ptp_InterruptStatus_get(0, pStatusMask)
#define rtk_ptp_portPktTrap_set(port, type, enable)                 rtksw_ptp_portPktTrap_set(0, port, type, enable)
#define rtk_ptp_portPktTrap_get(port, type, pEnable)                rtksw_ptp_portPktTrap_get(0, port, type, pEnable)
#define rtk_ptp_portPktAction_set(port, type, action, priority)     rtksw_ptp_portPktAction_set(0, port, type, action, priority)
#define rtk_ptp_portPktAction_get(port, type, pAction, pPriority)   rtksw_ptp_portPktAction_get(0, port, type, pAction, pPriority)
#define rtk_ptp_clkSrcCtrl_set(clksrc)                              rtksw_ptp_clkSrcCtrl_set(0, clksrc)
#define rtk_ptp_clkSrcCtrl_get(clksrc)                              rtksw_ptp_clkSrcCtrl_get(0, clksrc)
#define rtk_ptp_timerCompen_set(port, TxImbal, RxImbal)             rtksw_ptp_timerCompen_set(0, port, TxImbal, RxImbal)
#define rtk_ptp_timerCompen_get(port, pTxImbal, pRxImbal)           rtksw_ptp_timerCompen_get(0, port, pTxImbal, pRxImbal)

#define RTK_FORWARD   RTKSW_FORWARD
#define RTK_TRAP      RTKSW_TRAP
#define RTK_DROP      RTKSW_DROP

#define RTK_MAX_NUM_OF_NANO_SECOND    RTKSW_MAX_NUM_OF_NANO_SECOND 
#define RTK_PTP_INTR_MASK             RTKSW_PTP_INTR_MASK          
#define RTK_MAX_NUM_OF_TPID           RTKSW_MAX_NUM_OF_TPID        

#define rtk_ptp_msgType_t         rtksw_ptp_msgType_t
#define rtk_ptp_intType_t         rtksw_ptp_intType_t
#define rtk_ptp_sys_adjust_t      rtksw_ptp_sys_adjust_t
#define rtk_ptp_timeStamp_t       rtksw_ptp_timeStamp_t
#define rtk_time_timeStamp_t      rtksw_time_timeStamp_t
#define rtk_ptp_info_t            rtksw_ptp_info_t
#define rtk_ptp_tpid_t            rtksw_ptp_tpid_t
#define rtk_ptp_intStatus_t       rtksw_ptp_intStatus_t
#define rtk_ptp_intrType_t        rtksw_ptp_intrType_t
#define rtk_ptp_header_t          rtksw_ptp_header_t
#define rtk_ptp_port_role_t       rtksw_ptp_port_role_t
#define rtk_ptp_port_ctrl_t       rtksw_ptp_port_ctrl_t
#define rtk_ptp_porttrap_ctrl_t   rtksw_ptp_porttrap_ctrl_t
#define rtk_vlanType_t            rtksw_vlanType_t
#define rtk_time_ptpMsgType_t     rtksw_time_ptpMsgType_t
#define rtk_trapTarget_t          rtksw_trapTarget_t
#define rtk_time_ptpIdentifier_t  rtksw_time_ptpIdentifier_t
#define rtk_time_oper_t           rtksw_time_oper_t
#define rtk_time_opertriger_t     rtksw_time_opertriger_t
#define rtk_time_operCfg_t        rtksw_time_operCfg_t
#define rtk_time_clkOutMode_t     rtksw_time_clkOutMode_t
#define rtk_time_outSigSel_t      rtksw_time_outSigSel_t

#define rtk_time_txTimeEntry_t    rtksw_time_txTimeEntry_t
#define rtk_time_clkOutput_t      rtksw_time_clkOutput_t
#define rtk_ptp_pktType_t         rtksw_ptp_pktType_t

#endif /* CONFIG_COMPATIBLE_V1_4 */

#endif /* __RTKSW_API_PTP_H__ */

