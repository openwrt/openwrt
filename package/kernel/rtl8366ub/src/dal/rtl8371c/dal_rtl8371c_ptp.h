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

#ifndef __DAL_RTL8371C_PTP_H__
#define __DAL_RTL8371C_PTP_H__

#include "ptp.h"

/*
 * Symbol Definition
 */
#define dal_rtl8371c_MAX_NUM_OF_NANO_SECOND                     0x3B9AC9FF
#define dal_rtl8371c_PTP_INTR_MASK                              0xFF
#define dal_rtl8371c_MAX_NUM_OF_TPID                            0xFFFF
#define RTL8371C_MAX_PPS_WIDTH                                  630
#define RTL8371C_MAX_NUM_OF_TX_FIFO                             8



typedef enum RTL8371C_PTP_TIME_CMD_E
{
    PTP_TIME_READ = 0,
    PTP_TIME_WRITE,
    PTP_TIME_ADJUST,
    PTP_TIME_CMD_END
}RTL8371C_PTP_TIME_CMD;


typedef enum RTL8371C_PTP_TIME_ADJ_E
{
    PTP_TIME_ADJ_INC = 0,
    PTP_TIME_ADJ_DEC,
    PTP_TIME_ADJ_END
}RTL8371C_PTP_TIME_ADJ;



typedef struct  rtl8373_ptp_tx_time_stamp_s{
    rtksw_uint32 valid;
    rtksw_uint32 portid;
    rtksw_uint32 msgtype;
    rtksw_ptp_info_t timestamp;
}rtl8373_ptp_tx_time_stamp_t;

typedef struct  rtl8373_ptp_imr_s{
    rtksw_uint32 imr_pps_1;
    rtksw_uint32 imr_tod_done;
    rtksw_uint32 imr_txtime_empty;
}rtl8373_ptp_imr_t;

typedef struct  rtl8373_ptp_isr_s{
    rtksw_uint32 isr_pps_1;
    rtksw_uint32 isr_tod_done;
    rtksw_uint32 isr_txtime_empty;
}rtl8373_ptp_isr_t;


typedef struct  rtl8373_ptp_todframe_s{
    rtksw_uint16 TodDate[16];
}rtl8373_ptp_todframe_t;
#define RTL8371C_PTP_INTR_MASK        0xFF

#define RTL8371C_PTP_PORT_MASK        0x3FF
/*
 * Data Declaration
 */

/*
 * Function Declaration
 */


/* Function Name:
 *      dal_rtl8371c_ptp_init
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
 *      This API is used to initialize EEE status.
 */
extern rtksw_api_ret_t dal_rtl8371c_ptp_init(rtksw_uint32 unit);

/* Function Name:
 *      dal_rtl8371c_ptp_portBypassEnable_set
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
extern rtksw_api_ret_t dal_rtl8371c_ptp_portBypassEnable_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t Enable);

/* Function Name:
 *      dal_rtl8371c_ptp_portBypassEnable_get
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
extern rtksw_api_ret_t dal_rtl8371c_ptp_portBypassEnable_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t *pEnable);

/* Function Name:
 *      dal_rtl8371c_ptp_portTypeEnable_set
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
extern rtksw_api_ret_t dal_rtl8371c_ptp_portTypeEnable_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_ptp_header_t type,rtksw_enable_t Enable);

/* Function Name:
 *      dal_rtl8371c_ptpEn_get
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
extern rtksw_api_ret_t dal_rtl8371c_ptp_portTypeEnable_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_ptp_header_t type,rtksw_enable_t *pEnable);

/* Function Name:
 *      dal_rtl8371c_ptp_systemRefTime_set
 * Description:
 *      Set the reference time of the specified device.
 * Input:
 *      unit        - Unit ID
 *      apply       - state of apply
 *      timeStamp   - reference timestamp value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 * Note:
 *      None
 */
extern rtksw_api_ret_t dal_rtl8371c_ptp_systemRefTime_set(rtksw_uint32 unit, rtksw_time_timeStamp_t timeStamp, rtksw_enable_t apply);

/* Function Name:
 *      dal_rtl8371c_ptp_systemRefTime_get
 * Description:
 *      Get the reference time of the specified device by software.
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pTimeStamp - pointer buffer of the reference time
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
 */
extern rtksw_api_ret_t dal_rtl8371c_ptp_systemRefTime_get(rtksw_uint32 unit, rtksw_time_timeStamp_t *pTimeStamp);

/* Function Name:
 *      dal_rtl8371c_ptp_Oper_triger
 * Description:
 *      Set the PTP time operation configuration of specific port.
 * Input:
 *      unit    - Unit ID
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
extern rtksw_api_ret_t dal_rtl8371c_ptp_oper_triger(rtksw_uint32 unit);

/* Function Name:
 *      dal_rtl8371c_ptp_operCfg_set
 * Description:
 *      Set the PTP time operation configuration of specific port.
 * Input:
 *      unit        - Unit ID
 *      pOperCfg    - pointer to PTP time operation configuraton
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
extern rtksw_api_ret_t dal_rtl8371c_ptp_operCfg_set(rtksw_uint32 unit, rtksw_time_operCfg_t *pOperCfg);

/* Function Name:
 *      dal_rtl8371c_ptp_operCfg_get
 * Description:
 *      Get the PTP time operation configuration of specific port.
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pOperCfg    - pointer to PTP time operation configuraton
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
extern rtksw_api_ret_t dal_rtl8371c_ptp_operCfg_get(rtksw_uint32 unit, rtksw_time_operCfg_t *pOperCfg);

/* Function Name:
 *      dal_rtl8371c_ptp_systemRefTimeAdjust_set
 * Description:
 *      Adjust the reference time.
 * Input:
 *      unit        - Unit ID
 *      sign        - significant
 *      timeStamp   - reference timestamp value
 *      apply       - state of apply
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
extern rtksw_api_ret_t dal_rtl8371c_ptp_systemRefTimeAdjust_set(rtksw_uint32 unit, rtksw_ptp_sys_adjust_t sign, rtksw_time_timeStamp_t timeStamp, rtksw_enable_t apply);

/* Function Name:
 *      dal_rtl8371c_ptp_vlanTpid_set
 * Description:
 *      Set PTP accepted outer or inner tag TPID.
 * Input:
 *      unit    - Unit ID
 *      type    - Type
 *      idx     - Index
 *      Tpid    - Ether type of tag frame parsing in PTP ports.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      None
 */
extern rtksw_api_ret_t dal_rtl8371c_ptp_vlanTpid_set(rtksw_uint32 unit, rtksw_vlanType_t type, rtksw_uint32 idx, rtksw_uint32 Tpid);

/* Function Name:
 *      dal_rtl8371c_ptp_vlanTpid_get
 * Description:
 *      Get PTP accepted outer or inner tag TPID.
 * Input:
 *      unit    - Unit ID
 *      type    - Type
 *      idx     - Index
 * Output:
 *      pTpid - Ether type of tag frame parsing in PTP ports.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      None
 */
extern rtksw_api_ret_t dal_rtl8371c_ptp_vlanTpid_get(rtksw_uint32 unit, rtksw_vlanType_t type, rtksw_uint32 idx, rtksw_uint32 *pTpid);

/* Function Name:
 *      dal_rtl8371c_ptp_latchTime_get
 * Description:
 *      Get the PTP latched time of specific port.
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pLatchTime    - pointer to PTP time operation configuraton
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
extern rtksw_api_ret_t dal_rtl8371c_ptp_latchTime_get(rtksw_uint32 unit, rtksw_time_timeStamp_t *pLatchTime);

/* Function Name:
 *      dal_rtl8371c_ptp_refTimeFreqCfg_set
 * Description:
 *      Set the frequency of reference time of PHY of the specified port.
 * Input:
 *      unit    - Unit ID
 *      freq    - reference time frequency
 *      apply   - state of apply
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
 *      The frequency configuration decides the reference time tick frequency.
 *      The default value is 0x10000000.
 *      If it is configured to 0x8000000, the tick frequency would be half of default.
 *      If it is configured to 0x20000000, the tick frequency would be one and half times of default.
 */
extern rtksw_api_ret_t dal_rtl8371c_ptp_refTimeFreqCfg_set(rtksw_uint32 unit, rtksw_uint32 freq, rtksw_enable_t apply);

/* Function Name:
 *      dal_rtl8371c_ptp_refTimeFreqCfg_get
 * Description:
 *      Set  ptp_RefTimeFreqCfg_get.
 * Input:
 *      unit    - Unit ID
 * Output:
 *      cfgFreq
 *      curFreq
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 */
extern rtksw_api_ret_t dal_rtl8371c_ptp_refTimeFreqCfg_get(rtksw_uint32 unit, rtksw_uint32 *cfgFreq, rtksw_uint32 *curFreq);

/* Function Name:
 *      dal_rtl8371c_ptp_txTimestampFifo_get
 * Description:
 *      Get the top entry from PTP Tx timstamp FIFO on the dedicated port from the specified device.
 * Input:
 *      unit        - Unit ID
 * Output:
 *      pTimeEntry  - pointer buffer of TIME timestamp entry
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
extern rtksw_api_ret_t dal_rtl8371c_ptp_txTimestampFifo_get(rtksw_uint32 unit, rtksw_time_txTimeEntry_t *pTimeEntry);

/* Function Name:
 *      dal_rtl8371c_ptp_1PPSOutput_set
 * Description:
 *      Set 1 PPS output configuration of the specified port.
 * Input:
 *      unit        - Unit ID
 *      pulseWidth  - pointer to 1 PPS pulse width, unit: 10 ms
 *      enable      - enable 1 PPS output
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
 *      None
 */
extern rtksw_api_ret_t dal_rtl8371c_ptp_1PPSOutput_set(rtksw_uint32 unit, rtksw_uint32 pulseWidth, rtksw_enable_t enable);

/* Function Name:
 *      dal_rtl8371c_ptp_1PPSOutput_get
 * Description:
 *      Get 1 PPS output configuration of the specified port.
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pPulseWidth - pointer to 1 PPS pulse width, unit: 10 ms
 *      pEnable     - pointer to 1 PPS output enable status
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
extern rtksw_api_ret_t dal_rtl8371c_ptp_1PPSOutput_get(rtksw_uint32 unit, rtksw_uint32 *pPulseWidth, rtksw_enable_t *pEnable);

/* Function Name:
 *      dal_rtl8371c_ptp_todDelay_set
 * Description:
 *      Set toddelay.
 * Input:
 *      unit        - Unit ID
 *      toddelay    - toddelay
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
 *      None
 */
extern rtksw_api_ret_t dal_rtl8371c_ptp_todDelay_set(rtksw_uint32 unit, rtksw_uint32 toddelay);

/* Function Name:
 *      dal_rtl8371c_ptp_todDelay_get
 * Description:
 *      Set toddelay.
 * Input:
 *      unit        - Unit ID
 * Output:
 *      toddelay  - toddelay
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern rtksw_api_ret_t dal_rtl8371c_ptp_todDelay_get(rtksw_uint32 unit, rtksw_uint32 *ptoddelay);

/* Function Name:
 *      dal_rtl8371c_ptp_clockOutput_set
 * Description:
 *      Set 1 PPS output configuration of the specified port.
 * Input:
 *      unit        - Unit ID
 *      pClkOutput  - pointer to clock output configuration
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
 *      None
 */
extern rtksw_api_ret_t dal_rtl8371c_ptp_clockOutput_set(rtksw_uint32 unit, rtksw_time_clkOutput_t *pClkOutput);

/* Function Name:
 *      dal_rtl8371c_ptp_clockOutput_get
 * Description:
 *      Get clock output configuration of the specified port.
 * Input:
 *      unit        - Unit ID
 * Output:
 *      pClkOutput  - pointer to clock output configuration
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
extern rtksw_api_ret_t dal_rtl8371c_ptp_clockOutput_get(rtksw_uint32 unit, rtksw_time_clkOutput_t *pClkOutput);

/* Function Name:
 *      dal_rtl8371c_ptp_portCtrl_set
 * Description:
 *      Get enable status for PTP transparent clock of the specified port.
 * Input:
 *      unit        - Unit ID
 *      port        - port id
 *      pPortcfg    - port role/udp_en/eth_en/always_ts
 * Output:
 *      
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
extern rtksw_api_ret_t dal_rtl8371c_ptp_portCtrl_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_ptp_port_ctrl_t *pPortcfg);

/* Function Name:
 *      dal_rtl8371c_ptp_TransEnable_get
 * Description:
 *      Get enable status for PTP transparent clock of the specified port.
 * Input:
 *      unit    - Unit ID
 *      port    - port id
 * Output:
 *      portcfg   -port role/udp_en/eth_en/always_ts
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
extern rtksw_api_ret_t dal_rtl8371c_ptp_portCtrl_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_ptp_port_ctrl_t *pPortcfg);

/* Function Name:
 *      dal_rtl8371c_ptp_intControl_set
 * Description:
 *      Set PTP interrupt trigger status configuration.
 * Input:
 *      unit    - Unit ID
 *      type    - Interrupt type
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
 *          PTP_INT_TYPE_1PPS = 0,
 *          PTP_INT_TYPE_TOD_DONE,
 *          PTP_INT_TYPE_TXTIME_EMPTY,
 */
extern  rtksw_api_ret_t dal_rtl8371c_ptp_intControl_set(rtksw_uint32 unit, rtksw_ptp_intrType_t type, rtksw_enable_t enable);
;

/* Function Name:
 *      dal_rtl8371c_ptp_intControl_get
 * Description:
 *      Get PTP interrupt trigger status configuration.
 * Input:
 *      unit    - Unit ID
 *      type    - Interrupt type
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
 *          PTP_INT_TYPE_1PPS = 0,
 *          PTP_INT_TYPE_TOD_DONE,
 *          PTP_INT_TYPE_TXTIME_EMPTY,
 */
extern  rtksw_api_ret_t dal_rtl8371c_ptp_intControl_get(rtksw_uint32 unit, rtksw_ptp_intrType_t type, rtksw_enable_t *pEnable);

/* Function Name:
 *      dal_rtl8371c_ptp_intStatus_set
 * Description:
 *      Set PTP interrupt trigger status configuration.
 * Input:
 *      unit        - Unit ID
 *      statusMask  - Interrupt statue.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_ENABLE       - Invalid enable input.
 * Note:
 *      The API can set PTP interrupt status configuration. It is used to clean interrupt status.
 *      The interrupt trigger status is shown in the following:
 *          PTP_INT_TYPE_1PPS          (Bit0)
 *          PTP_INT_TYPE_TOD_DONE      (Bit1)
 *          PTP_INT_TYPE_TXTIME_EMPTY  (Bit2)
 */
extern rtksw_api_ret_t dal_rtl8371c_ptp_intStatus_set(rtksw_uint32 unit, rtksw_ptp_intStatus_t statusMask);

/* Function Name:
 *      dal_rtl8371c_ptp_intStatus_get
 * Description:
 *      Get PTP interrupt trigger status configuration.
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pStatusMask - Interrupt status.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      The API can get interrupt status configuration.
 *      The interrupt trigger status is shown in the following:
 *          PTP_INT_TYPE_1PPS          (Bit0)
 *          PTP_INT_TYPE_TOD_DONE      (Bit1)
 *          PTP_INT_TYPE_TXTIME_EMPTY  (Bit2)
 */
 extern rtksw_api_ret_t dal_rtl8371c_ptp_intStatus_get(rtksw_uint32 unit, rtksw_ptp_intStatus_t *pStatusMask);

/* Function Name:
 *      dal_rtl8371c_ptp_portPktTrap_set
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
 extern rtksw_api_ret_t dal_rtl8371c_ptp_portPktTrap_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_ptp_pktType_t type, rtksw_enable_t enable);


/* Function Name:
 *      dal_rtl8371c_ptp_portPktTrap_get
 * Description:
 *      Get PTP packet trap of the specified port.
 * Input:
 *      unit    - Unit ID
 *      port    - port id
 *      type    - PTP message type
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
extern rtksw_api_ret_t dal_rtl8371c_ptp_portPktTrap_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_ptp_pktType_t type, rtksw_enable_t *pEnable);

/* Function Name:
 *      dal_rtl8371c_ptp_portPktAction_set
 * Description:
 *      Set PTP packet trap of the specified port.
 * Input:
 *      unit        - Unit ID
 *      port        - port id
 *      type        - Eth or UDP
 *      action      - packet action
 *      priority    - Priority
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
  extern rtksw_api_ret_t dal_rtl8371c_ptp_portPktAction_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_ptp_header_t type, rtksw_trapTarget_t action, rtksw_uint32 priority);

/* Function Name:
 *      dal_rtl8371c_ptp_portPktAction_get
 * Description:
 *      Get PTP packet trap of the specified port.
 * Input:
 *      unit        - Unit ID
 *      port        - port id
 *      type        - Eth or UDP
 *      action      - packet action
 * Output:
 *      *pAction - packet action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT         - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern rtksw_api_ret_t dal_rtl8371c_ptp_portPktAction_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_ptp_header_t type, rtksw_trapTarget_t *pAction, rtksw_uint32 *pPriority);

/* Function Name:
 *      dal_rtl8371c_ptp_timerCompen_set
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
rtksw_api_ret_t dal_rtl8371c_ptp_timerCompen_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_uint32 TxImbal,rtksw_uint32 RxImbal);

/* Function Name:
 *      dal_rtl8371c_ptp_timerCompen_get
 * Description:
 *      Get TX/RX timer value compensation..
 * Input:
 *      unit    - Unit ID
 *      port    - port id
 * Output:
 *      PTxImbal  - TX timer value compensation
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
rtksw_api_ret_t dal_rtl8371c_ptp_timerCompen_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_uint32 *pTxImbal,rtksw_uint32 *pRxImbal);

/* Function Name:
 *      dal_rtl8371c_ptp_PPSLatchTime_get
 * Description:
 *      Set toddelay.
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pLatchTime  - latch time
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      mac mode only
 */
rtksw_api_ret_t dal_rtl8371c_ptp_PPSLatchTime_get(rtksw_uint32 unit, rtksw_time_timeStamp_t *pLatchTime);

/* Function Name:
 *      dal_rtl8371c_ptp_clkSrcCtrl_set
 * Description:
 *      Set PTP time Clock source selection
 * Input:
 *      unit    - Unit ID
 *      clksrc  - clock source
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK   - Success
 *      RT_ERR_SMI  - SMI access error
 * Note:
 * 0: internal clock (Internal PLL, 1GMHz)
 * 1: external clock, refer to cfg_ext_clk_src)
 */
rtksw_api_ret_t dal_rtl8371c_ptp_clkSrcCtrl_set(rtksw_uint32 unit, rtksw_enable_t clksrc);

/* Function Name:
 *      dal_rtl8371c_ptp_clkSrcCtrl_get
 * Description:
 *      Get PTP time Clock source selection
 * Input:
 *      unit    - Unit ID
 * Output:
 *      clksrc  - Clock source
 * Return:
 *      RT_ERR_OK   - Success
 *      RT_ERR_SMI  - SMI access error
 * Note:
 * 0: internal clock (Internal PLL, 1GMHz)
 * 1: external clock, refer to cfg_ext_clk_src)
 */
rtksw_api_ret_t dal_rtl8371c_ptp_clkSrcCtrl_get(rtksw_uint32 unit, rtksw_enable_t *clksrc);

#endif /* __DAL_RTL8371C_PTP_H__ */


