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

#ifndef __RTKSW_RLDP_H__
#define __RTKSW_RLDP_H__


/*
 * Include Files
 */


/*
 * Symbol Definition
 */
typedef enum rtksw_rldp_trigger_e
{
    RTKSW_RLDP_TRIGGER_SAMOVING = 0,
    RTKSW_RLDP_TRIGGER_PERIOD,
    RTKSW_RLDP_TRIGGER_END
} rtksw_rldp_trigger_t;

typedef enum rtksw_rldp_cmpType_e
{
    RTKSW_RLDP_CMPTYPE_MAGIC = 0,     /* Compare the RLDP with magic only */
    RTKSW_RLDP_CMPTYPE_MAGIC_ID,      /* Compare the RLDP with both magic + ID */
    RTKSW_RLDP_CMPTYPE_END
} rtksw_rldp_cmpType_t;

typedef enum rtksw_rldp_loopStatus_e
{
    RTKSW_RLDP_LOOPSTS_NONE = 0,
    RTKSW_RLDP_LOOPSTS_LOOPING,
    RTKSW_RLDP_LOOPSTS_END
} rtksw_rldp_loopStatus_t;

typedef enum rtksw_rlpp_trapType_e
{
    RTKSW_RLPP_TRAPTYPE_NONE = 0,
    RTKSW_RLPP_TRAPTYPE_CPU,
    RTKSW_RLPP_TRAPTYPE_END
} rtksw_rlpp_trapType_t;

typedef struct rtksw_rldp_config_s
{
    rtksw_enable_t        rldp_enable;
    rtksw_rldp_trigger_t trigger_mode;
    rtksw_mac_t           magic;
    rtksw_rldp_cmpType_t  compare_type;
    rtksw_uint32              interval_check; /* Checking interval for check state */
    rtksw_uint32              num_check;      /* Checking number for check state */
    rtksw_uint32              interval_loop;  /* Checking interval for loop state */
    rtksw_uint32              num_loop;       /* Checking number for loop state */
} rtksw_rldp_config_t;

typedef struct rtksw_rldp_portConfig_s
{
    rtksw_enable_t        tx_enable;
} rtksw_rldp_portConfig_t;

typedef struct rtksw_rldp_status_s
{
    rtksw_mac_t           id;
} rtksw_rldp_status_t;

typedef struct rtksw_rldp_portStatus_s
{
    rtksw_rldp_loopStatus_t   loop_status;
    rtksw_rldp_loopStatus_t   loop_enter;
    rtksw_rldp_loopStatus_t   loop_leave;
} rtksw_rldp_portStatus_t;

/*
 * Data Declaration
 */


/*
 * Macro Declaration
 */

#define RTKSW_RLDP_INTERVAL_MAX  0xffff
#define RTKSW_RLDP_NUM_MAX       0xff


/*
 * Function Declaration
 */

/* Module Name : RLDP */

/* Function Name:
 *      rtksw_rldp_config_set
 * Description:
 *      Set RLDP module configuration
 * Input:
 *      unit        - Unit ID
 *      pConfig     - configuration structure of RLDP
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern rtksw_api_ret_t rtksw_rldp_config_set(rtksw_uint32 unit, rtksw_rldp_config_t *pConfig);

/* Function Name:
 *      rtksw_rldp_config_get
 * Description:
 *      Get RLDP module configuration
 * Input:
 *      unit        - Unit ID
 * Output:
 *      pConfig - configuration structure of RLDP
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern rtksw_api_ret_t rtksw_rldp_config_get(rtksw_uint32 unit, rtksw_rldp_config_t *pConfig);

/* Function Name:
 *      rtksw_rldp_portConfig_set
 * Description:
 *      Set per port RLDP module configuration
 * Input:
 *      unit        - Unit ID
 *      port        - port number to be configured
 *      pPortConfig - per port configuration structure of RLDP
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern rtksw_api_ret_t rtksw_rldp_portConfig_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_rldp_portConfig_t *pPortConfig);


/* Function Name:
 *      rtksw_rldp_portConfig_get
 * Description:
 *      Get per port RLDP module configuration
 * Input:
 *      unit        - Unit ID
 *      port        - port number to be get
 * Output:
 *      pPortConfig - per port configuration structure of RLDP
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern rtksw_api_ret_t rtksw_rldp_portConfig_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_rldp_portConfig_t *pPortConfig);


/* Function Name:
 *      rtksw_rldp_status_get
 * Description:
 *      Get RLDP module status
 * Input:
 *      unit        - Unit ID
 * Output:
 *      pStatus - status structure of RLDP
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern rtksw_api_ret_t rtksw_rldp_status_get(rtksw_uint32 unit, rtksw_rldp_status_t *pStatus);


/* Function Name:
 *      rtksw_rldp_portStatus_get
 * Description:
 *      Get RLDP module status
 * Input:
 *      unit        - Unit ID
 *      port        - port number to be get
 * Output:
 *      pPortStatus - per port status structure of RLDP
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern rtksw_api_ret_t rtksw_rldp_portStatus_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_rldp_portStatus_t *pPortStatus);

/* Function Name:
 *      rtksw_rldp_portStatus_set
 * Description:
 *      Clear RLDP module status
 * Input:
 *      unit        - Unit ID
 *      port        - port number to be clear
 *      pPortStatus - per port status structure of RLDP
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      Clear operation effect loop_enter and loop_leave only, other field in
 *      the structure are don't care. Loop status cab't be clean.
 */
extern rtksw_api_ret_t rtksw_rldp_portStatus_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_rldp_portStatus_t *pPortStatus);

/* Function Name:
 *      rtksw_rldp_portLoopPair_get
 * Description:
 *      Get RLDP port loop pairs
 * Input:
 *      port    - port number to be get
 * Output:
 *      pPortmask - per port related loop ports
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern rtksw_api_ret_t rtksw_rldp_portLoopPair_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_portmask_t *pPortmask);

#if defined(CONFIG_COMPATIBLE_V1_4)

#define rtk_rldp_config_set(pConfig)                    rtksw_rldp_config_set(0, pConfig)
#define rtk_rldp_config_get(pConfig)                    rtksw_rldp_config_get(0, pConfig)
#define rtk_rldp_portConfig_set(port, pPortConfig)      rtksw_rldp_portConfig_set(0, port, pPortConfig)
#define rtk_rldp_portConfig_get(port, pPortConfig)      rtksw_rldp_portConfig_get(0, port, pPortConfig)
#define rtk_rldp_status_get(pStatus)                    rtksw_rldp_status_get(0, pStatus)
#define rtk_rldp_portStatus_get(port, pPortStatus)      rtksw_rldp_portStatus_get(0, port, pPortStatus)
#define rtk_rldp_portStatus_set(port, pPortStatus)      rtksw_rldp_portStatus_set(0, port, pPortStatus)
#define rtk_rldp_portLoopPair_get(port, pPortmask)      rtksw_rldp_portLoopPair_get(0, port, pPortmask)

#define RTK_RLDP_TRIGGER_SAMOVING   RTKSW_RLDP_TRIGGER_SAMOVING
#define RTK_RLDP_TRIGGER_PERIOD     RTKSW_RLDP_TRIGGER_PERIOD

#define RTK_RLDP_CMPTYPE_MAGIC      RTKSW_RLDP_CMPTYPE_MAGIC
#define RTK_RLDP_CMPTYPE_MAGIC_ID   RTKSW_RLDP_CMPTYPE_MAGIC_ID

#define RTK_RLDP_LOOPSTS_NONE       RTKSW_RLDP_LOOPSTS_NONE
#define RTK_RLDP_LOOPSTS_LOOPING    RTKSW_RLDP_LOOPSTS_LOOPING

#define RTK_RLPP_TRAPTYPE_NONE      RTKSW_RLPP_TRAPTYPE_NONE
#define RTK_RLPP_TRAPTYPE_CPU       RTKSW_RLPP_TRAPTYPE_CPU

#define rtk_rldp_trigger_t          rtksw_rldp_trigger_t
#define rtk_rldp_cmpType_t          rtksw_rldp_cmpType_t
#define rtk_rldp_loopStatus_t       rtksw_rldp_loopStatus_t
#define rtk_rlpp_trapType_t         rtksw_rlpp_trapType_t
#define rtk_rldp_config_t           rtksw_rldp_config_t
#define rtk_rldp_portConfig_t       rtksw_rldp_portConfig_t
#define rtk_rldp_status_t           rtksw_rldp_status_t
#define rtk_rldp_portStatus_t       rtksw_rldp_portStatus_t

#endif /* CONFIG_COMPATIBLE_V1_4 */

#endif /* __RTKSW_RLDP_H__ */

