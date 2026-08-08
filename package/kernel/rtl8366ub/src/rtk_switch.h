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

#ifndef __RTKSW_SWITCH_H__
#define __RTKSW_SWITCH_H__

#include <rtk_types.h>
#include <chip.h>

#if defined(RTK_X86_CLE)
#include <pthread.h>
#else
#include <linux/mutex.h>
#endif

#define MAXPKTLEN_CFG_ID_MAX (1)

#define RTKSW_SWITCH_MAX_PKTLEN (0x3FFF)

#if defined(RTK_X86_CLE)
extern pthread_mutex_t api_mutex;
#define RTKSW_API_LOCK(__unit__)      pthread_mutex_lock(&api_mutex)
#define RTKSW_API_UNLOCK(__unit__)    pthread_mutex_unlock(&api_mutex)
#else
extern struct mutex rtksw_api_mutex;
#define RTKSW_API_LOCK(__unit__)    mutex_lock(&rtksw_api_mutex)
#define RTKSW_API_UNLOCK(__unit__)  mutex_unlock(&rtksw_api_mutex)
#endif

typedef enum rtksw_init_state_e
{
    RTKSW_INIT_NOT_COMPLETED = 0,
    RTKSW_INIT_COMPLETED,
    RTKSW_INIT_STATE_END
} rtksw_init_state_t;

typedef enum rtksw_switch_maxPktLen_linkSpeed_e {
     RTKSW_MAXPKTLEN_LINK_SPEED_FE = 0,
     RTKSW_MAXPKTLEN_LINK_SPEED_GE,
     RTKSW_MAXPKTLEN_LINK_SPEED_END,
} rtksw_switch_maxPktLen_linkSpeed_t;


/* UTIL MACRO */
#define RTKSW_CHK_INIT_STATE(__unit__)                                \
    do                                                      \
    {                                                       \
        if(rtksw_switch_initialState_get(__unit__) != RTKSW_INIT_COMPLETED) \
        {                                                   \
            return RT_ERR_NOT_INIT;                         \
        }                                                   \
    }while(0)

#define RTKSW_CHK_PORT_VALID(__unit__, __port__)                            \
    do                                                          \
    {                                                           \
        if(rtksw_switch_logicalPortCheck(__unit__, __port__) != RT_ERR_OK)  \
        {                                                       \
            return RT_ERR_PORT_ID;                              \
        }                                                       \
    }while(0)

#define RTKSW_CHK_PORT_IS_UTP(__unit__, __port__)                           \
    do                                                          \
    {                                                           \
        if(rtksw_switch_isUtpPort(__unit__, __port__) != RT_ERR_OK)         \
        {                                                       \
            return RT_ERR_PORT_ID;                              \
        }                                                       \
    }while(0)

#define RTKSW_CHK_PORT_IS_EXT(__unit__, __port__)                           \
    do                                                          \
    {                                                           \
        if(rtksw_switch_isExtPort(__unit__, __port__) != RT_ERR_OK)         \
        {                                                       \
            return RT_ERR_PORT_ID;                              \
        }                                                       \
    }while(0)

#define RTKSW_CHK_PORT_IS_COMBO(__unit__, __port__)                         \
    do                                                          \
    {                                                           \
        if(rtksw_switch_isComboPort(__unit__, __port__) != RT_ERR_OK)       \
        {                                                       \
            return RT_ERR_PORT_ID;                              \
        }                                                       \
    }while(0)

#define RTKSW_CHK_PORT_IS_PTP(__unit__, __port__)                           \
    do                                                          \
    {                                                           \
        if(rtksw_switch_isPtpPort(__unit__, __port__) != RT_ERR_OK)         \
        {                                                       \
            return RT_ERR_PORT_ID;                              \
        }                                                       \
    }while(0)

#define RTKSW_CHK_PORTMASK_VALID(__uint__, __portmask__)                        \
    do                                                              \
    {                                                               \
        if(rtksw_switch_isPortMaskValid(__uint__, __portmask__) != RT_ERR_OK)   \
        {                                                           \
            return RT_ERR_PORT_MASK;                                \
        }                                                           \
    }while(0)

#define RTKSW_CHK_PORTMASK_VALID_ONLY_UTP(__unit__, __portmask__)               \
    do                                                              \
    {                                                               \
        if(rtksw_switch_isPortMaskUtp(__unit__, __portmask__) != RT_ERR_OK)     \
        {                                                           \
            return RT_ERR_PORT_MASK;                                \
        }                                                           \
    }while(0)

#define RTKSW_CHK_PORTMASK_VALID_ONLY_EXT(__unit__, __portmask__)               \
    do                                                              \
    {                                                               \
        if(rtksw_switch_isPortMaskExt(__unit__, __portmask__) != RT_ERR_OK)     \
        {                                                           \
            return RT_ERR_PORT_MASK;                                \
        }                                                           \
    }while(0)

#define RTKSW_CHK_TRUNK_GROUP_VALID(__unit__, __grpId__)                        \
    do                                                              \
    {                                                               \
        if(rtksw_switch_isValidTrunkGrpId(__unit__, __grpId__) != RT_ERR_OK)    \
        {                                                           \
            return RT_ERR_LA_TRUNK_ID;                              \
        }                                                           \
    }while(0)

#define RTKSW_CHK_UNIT_ID(__unit__)                        \
    do                                                              \
    {                                                               \
        if(__unit__ >= DAL_MGMT_MAX_UNIT)    \
        {                                                           \
            return RT_ERR_UNIT_ID;                              \
        }                                                           \
    }while(0)   

#define RTKSW_CHK_RT_MAPPER(__unit__)                        \
    do                                                              \
    {                                                               \
        if(RT_MAPPER == NULL)    \
        {                                                           \
            return RT_ERR_NOT_INIT;                              \
        }                                                           \
    }while(0)    

#define RTKSW_PORTMASK_IS_PORT_SET(__portmask__, __port__)    (((__portmask__).bits[0] & (0x00000001 << __port__)) ? 1 : 0)
#define RTKSW_PORTMASK_IS_EMPTY(__portmask__)                 (((__portmask__).bits[0] == 0) ? 1 : 0)
#define RTKSW_PORTMASK_CLEAR(__portmask__)                    ((__portmask__).bits[0] = 0)
#define RTKSW_PORTMASK_PORT_SET(__portmask__, __port__)       ((__portmask__).bits[0] |= (0x00000001 << __port__))
#define RTKSW_PORTMASK_PORT_CLEAR(__portmask__, __port__)     ((__portmask__).bits[0] &= ~(0x00000001 << __port__))
#define RTKSW_PORTMASK_ALLPORT_SET(__UNIT__, __portmask__)    (rtksw_switch_logPortMask_get(__UNIT__, &__portmask__))
#define RTKSW_PORTMASK_SCAN(__portmask__, __port__)           for(__port__ = 0; __port__ < RTKSW_SWITCH_PORT_NUM; __port__++)  if(RTKSW_PORTMASK_IS_PORT_SET(__portmask__, __port__))
#define RTKSW_PORTMASK_COMPARE(__portmask_A__, __portmask_B__)    ((__portmask_A__).bits[0] - (__portmask_B__).bits[0])

#define RTKSW_SCAN_ALL_PHY_PORTMASK(__unit__, __port__)                 for(__port__ = 0; __port__ < RTKSW_SWITCH_PORT_NUM; __port__++)  if( (rtksw_switch_phyPortMask_get(__unit__) & (0x00000001 << __port__)))
#define RTKSW_SCAN_ALL_LOG_PORT(__unit__, __port__)                     for(__port__ = 0; __port__ < RTKSW_SWITCH_PORT_NUM; __port__++)  if( rtksw_switch_logicalPortCheck(__unit__, __port__) == RT_ERR_OK)
#define RTKSW_SCAN_ALL_LOG_PORTMASK(__unit__, __portmask__)             for((__portmask__).bits[0] = 0; (__portmask__).bits[0] < 0x7FFFF; (__portmask__).bits[0]++)  if( rtksw_switch_isPortMaskValid(__unit__, &__portmask__) == RT_ERR_OK)

/* Port mask defination */
#define RTKSW_PHY_PORTMASK_ALL                                (rtksw_switch_phyPortMask_get(unit))

/* Port defination*/
#define RTKSW_MAX_LOGICAL_PORT_ID                             (rtksw_switch_maxLogicalPort_get(unit))

/* Boundary defination */
#define RTKSW_MAX_METER_ID                    (rtksw_switch_maxMeterId_get(unit))
#define RTKSW_MAX_BUF_PAGE_NUM                (rtksw_switch_maxBufferPageNum_get(unit))

/* Function Name:
 *      rtksw_switch_initialState_set
 * Description:
 *      Set initial status
 * Input:
 *      unit    - Unit ID
 *      state   - Initial state;
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - Initialized
 *      RT_ERR_FAILED   - Uninitialized
 * Note:
 *
 */
extern rtksw_api_ret_t rtksw_switch_initialState_set(rtksw_uint32 unit, rtksw_init_state_t state);

/* Function Name:
 *      rtksw_switch_initialState_get
 * Description:
 *      Get initial status
 * Input:
 *      unit    - Unit ID
 * Output:
 *      None
 * Return:
 *      RTKSW_INIT_COMPLETED     - Initialized
 *      RTKSW_INIT_NOT_COMPLETED - Uninitialized
 * Note:
 *
 */
extern rtksw_init_state_t rtksw_switch_initialState_get(rtksw_uint32 unit);

/* Function Name:
 *      rtksw_switch_logicalPortCheck
 * Description:
 *      Check logical port ID.
 * Input:
 *      logicalPort     - logical port ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - Port ID is correct
 *      RT_ERR_FAILED   - Port ID is not correct
 *      RT_ERR_NOT_INIT - Not Initialize
 * Note:
 *
 */
extern rtksw_api_ret_t rtksw_switch_logicalPortCheck(rtksw_uint32 unit, rtksw_port_t logicalPort);

/* Function Name:
 *      rtksw_switch_isUtpPort
 * Description:
 *      Check is logical port a UTP port
 * Input:
 *      logicalPort     - logical port ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - Port ID is a UTP port
 *      RT_ERR_FAILED   - Port ID is not a UTP port
 *      RT_ERR_NOT_INIT - Not Initialize
 * Note:
 *
 */
extern rtksw_api_ret_t rtksw_switch_isUtpPort(rtksw_uint32 unit, rtksw_port_t logicalPort);

/* Function Name:
 *      rtksw_switch_isExtPort
 * Description:
 *      Check is logical port a Extension port
 * Input:
 *      logicalPort     - logical port ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - Port ID is a EXT port
 *      RT_ERR_FAILED   - Port ID is not a EXT port
 *      RT_ERR_NOT_INIT - Not Initialize
 * Note:
 *
 */
extern rtksw_api_ret_t rtksw_switch_isExtPort(rtksw_uint32 unit, rtksw_port_t logicalPort);

/* Function Name:
 *      rtksw_switch_isUsxgPort
 * Description:
 *      Check is logical port a USXG port
 * Input:
 *      logicalPort     - logical port ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - Port ID is a USXG port
 *      RT_ERR_FAILED   - Port ID is not a USXG port
 *      RT_ERR_NOT_INIT - Not Initialize
 * Note:
 *
 */
extern rtksw_api_ret_t rtksw_switch_isUsxgPort(rtksw_uint32 unit, rtksw_port_t logicalPort);

/* Function Name:
 *      rtksw_switch_isHsgPort
 * Description:
 *      Check is logical port a HSG port
 * Input:
 *      logicalPort     - logical port ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - Port ID is a HSG port
 *      RT_ERR_FAILED   - Port ID is not a HSG port
 *      RT_ERR_NOT_INIT - Not Initialize
 * Note:
 *
 */
extern rtksw_api_ret_t rtksw_switch_isHsgPort(rtksw_uint32 unit, rtksw_port_t logicalPort);

/* Function Name:
 *      rtksw_switch_isSgmiiPort
 * Description:
 *      Check is logical port a SGMII port
 * Input:
 *      logicalPort     - logical port ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - Port ID is a SGMII port
 *      RT_ERR_FAILED   - Port ID is not a SGMII port
 *      RT_ERR_NOT_INIT - Not Initialize
 * Note:
 *
 */
extern rtksw_api_ret_t rtksw_switch_isSgmiiPort(rtksw_uint32 unit, rtksw_port_t logicalPort);

/* Function Name:
 *      rtksw_switch_isUtp2p5gPort
 * Description:
 *      Check is logical port a 2.5G UTP port
 * Input:
 *      logicalPort     - logical port ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - Port ID is a 2.5G UTP port
 *      RT_ERR_FAILED   - Port ID is not a 2.5G UTP port
 *      RT_ERR_NOT_INIT - Not Initialize
 * Note:
 *
 */
extern rtksw_api_ret_t rtksw_switch_isUtp2p5gPort(rtksw_uint32 unit, rtksw_port_t logicalPort);

/* Function Name:
 *      rtksw_switch_isCPUPort
 * Description:
 *      Check is logical port a CPU port
 * Input:
 *      logicalPort     - logical port ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - Port ID is a CPU port
 *      RT_ERR_FAILED   - Port ID is not a CPU port
 *      RT_ERR_NOT_INIT - Not Initialize
 * Note:
 *
 */
extern rtksw_api_ret_t rtksw_switch_isCPUPort(rtksw_uint32 unit, rtksw_port_t logicalPort);

/* Function Name:
 *      rtksw_switch_isComboPort
 * Description:
 *      Check is logical port a Combo port
 * Input:
 *      logicalPort     - logical port ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - Port ID is a combo port
 *      RT_ERR_FAILED   - Port ID is not a combo port
 *      RT_ERR_NOT_INIT - Not Initialize
 * Note:
 *
 */
extern rtksw_api_ret_t rtksw_switch_isComboPort(rtksw_uint32 unit, rtksw_port_t logicalPort);

/* Function Name:
 *      rtksw_switch_ComboPort_get
 * Description:
 *      Get Combo port ID
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      Port ID of combo port
 * Note:
 *
 */
extern rtksw_uint32 rtksw_switch_ComboPort_get(rtksw_uint32 unit);

/* Function Name:
 *      rtksw_switch_isPtpPort
 * Description:
 *      Check is logical port a PTP port
 * Input:
 *      logicalPort     - logical port ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - Port ID is a PTP port
 *      RT_ERR_FAILED   - Port ID is not a PTP port
 *      RT_ERR_NOT_INIT - Not Initialize
 * Note:
 *
 */
extern rtksw_api_ret_t rtksw_switch_isPtpPort(rtksw_uint32 unit, rtksw_port_t logicalPort);

/* Function Name:
 *      rtksw_switch_port_L2P_get
 * Description:
 *      Get physical port ID
 * Input:
 *      logicalPort       - logical port ID
 * Output:
 *      None
 * Return:
 *      Physical port ID
 * Note:
 *
 */
extern rtksw_uint32 rtksw_switch_port_L2P_get(rtksw_uint32 unit, rtksw_port_t logicalPort);

/* Function Name:
 *      rtksw_switch_port_P2L_get
 * Description:
 *      Get logical port ID
 * Input:
 *      physicalPort       - physical port ID
 * Output:
 *      None
 * Return:
 *      logical port ID
 * Note:
 *
 */
extern rtksw_port_t rtksw_switch_port_P2L_get(rtksw_uint32 unit, rtksw_uint32 physicalPort);

/* Function Name:
 *      rtksw_switch_isPortMaskValid
 * Description:
 *      Check portmask is valid or not
 * Input:
 *      pPmask       - logical port mask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - port mask is valid
 *      RT_ERR_FAILED       - port mask is not valid
 *      RT_ERR_NOT_INIT     - Not Initialize
 *      RT_ERR_NULL_POINTER - Null pointer
 * Note:
 *
 */
extern rtksw_api_ret_t rtksw_switch_isPortMaskValid(rtksw_uint32 unit, rtksw_portmask_t *pPmask);

/* Function Name:
 *      rtksw_switch_isPortMaskUtp
 * Description:
 *      Check all ports in portmask are only UTP port
 * Input:
 *      pPmask       - logical port mask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - Only UTP port in port mask
 *      RT_ERR_FAILED       - Not only UTP port in port mask
 *      RT_ERR_NOT_INIT     - Not Initialize
 *      RT_ERR_NULL_POINTER - Null pointer
 * Note:
 *
 */
extern rtksw_api_ret_t rtksw_switch_isPortMaskUtp(rtksw_uint32 unit, rtksw_portmask_t *pPmask);

/* Function Name:
 *      rtksw_switch_isPortMaskExt
 * Description:
 *      Check all ports in portmask are only EXT port
 * Input:
 *      pPmask       - logical port mask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - Only EXT port in port mask
 *      RT_ERR_FAILED       - Not only EXT port in port mask
 *      RT_ERR_NOT_INIT     - Not Initialize
 *      RT_ERR_NULL_POINTER - Null pointer
 * Note:
 *
 */
extern rtksw_api_ret_t rtksw_switch_isPortMaskExt(rtksw_uint32 unit, rtksw_portmask_t *pPmask);

/* Function Name:
 *      rtksw_switch_portmask_L2P_get
 * Description:
 *      Get physicl portmask from logical portmask
 * Input:
 *      pLogicalPmask       - logical port mask
 * Output:
 *      pPhysicalPortmask   - physical port mask
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_NOT_INIT     - Not Initialize
 *      RT_ERR_NULL_POINTER - Null pointer
 *      RT_ERR_PORT_MASK    - Error port mask
 * Note:
 *
 */
extern rtksw_api_ret_t rtksw_switch_portmask_L2P_get(rtksw_uint32 unit, rtksw_portmask_t *pLogicalPmask, rtksw_uint32 *pPhysicalPortmask);

/* Function Name:
 *      rtksw_switch_portmask_P2L_get
 * Description:
 *      Get logical portmask from physical portmask
 * Input:
 *      physicalPortmask    - physical port mask
 * Output:
 *      pLogicalPmask       - logical port mask
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_NOT_INIT     - Not Initialize
 *      RT_ERR_NULL_POINTER - Null pointer
 *      RT_ERR_PORT_MASK    - Error port mask
 * Note:
 *
 */
extern rtksw_api_ret_t rtksw_switch_portmask_P2L_get(rtksw_uint32 unit, rtksw_uint32 physicalPortmask, rtksw_portmask_t *pLogicalPmask);

/* Function Name:
 *      rtksw_switch_phyPortMask_get
 * Description:
 *      Get physical portmask
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      0x00                - Not Initialize
 *      Other value         - Physical port mask
 * Note:
 *
 */
rtksw_uint32 rtksw_switch_phyPortMask_get(rtksw_uint32 unit);

/* Function Name:
 *      rtksw_switch_logPortMask_get
 * Description:
 *      Get Logical portmask
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_NOT_INIT     - Not Initialize
 *      RT_ERR_NULL_POINTER - Null pointer
 * Note:
 *
 */
extern rtksw_api_ret_t rtksw_switch_logPortMask_get(rtksw_uint32 unit, rtksw_portmask_t *pPortmask);

/* Function Name:
 *      rtksw_switch_init
 * Description:
 *      Set chip to default configuration enviroment
 * Input:
 *      unit    - Unit ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      The API can set chip registers to default configuration for different release chip model.
 */
extern rtksw_api_ret_t rtksw_switch_init(rtksw_uint32 unit);

/* Function Name:
 *      rtksw_switch_portMaxPktLen_set
 * Description:
 *      Set Max packet length
 * Input:
 *      unit    - Unit ID
 *      port    - Port ID
 *      speed   - Speed
 *      cfgId   - Configuration ID
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Error Input
 * Note:
 */
extern rtksw_api_ret_t rtksw_switch_portMaxPktLen_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_switch_maxPktLen_linkSpeed_t speed, rtksw_uint32 cfgId);

/* Function Name:
 *      rtksw_switch_portMaxPktLen_get
 * Description:
 *      Get Max packet length
 * Input:
 *      unit    - Unit ID
 *      port    - Port ID
 *      speed   - Speed
 * Output:
 *      pCfgId  - Configuration ID
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Error Input
 * Note:
 */
extern rtksw_api_ret_t rtksw_switch_portMaxPktLen_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_switch_maxPktLen_linkSpeed_t speed, rtksw_uint32 *pCfgId);

/* Function Name:
 *      rtksw_switch_maxPktLenCfg_set
 * Description:
 *      Set Max packet length configuration
 * Input:
 *      unit    - Unit ID
 *      cfgId   - Configuration ID
 *      pktLen  - Max packet length
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Error Input
 * Note:
 */
extern rtksw_api_ret_t rtksw_switch_maxPktLenCfg_set(rtksw_uint32 unit, rtksw_uint32 cfgId, rtksw_uint32 pktLen);

/* Function Name:
 *      rtksw_switch_maxPktLenCfg_get
 * Description:
 *      Get Max packet length configuration
 * Input:
 *      unit    - Unit ID
 *      cfgId   - Configuration ID
 *      pPktLen - Max packet length
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Error Input
 * Note:
 */
extern rtksw_api_ret_t rtksw_switch_maxPktLenCfg_get(rtksw_uint32 unit, rtksw_uint32 cfgId, rtksw_uint32 *pPktLen);

/* Function Name:
 *      rtksw_switch_greenEthernet_set
 * Description:
 *      Set all Ports Green Ethernet state.
 * Input:
 *      unit    - Unit ID
 *      enable - Green Ethernet state.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - OK
 *      RT_ERR_FAILED   - Failed
 *      RT_ERR_SMI      - SMI access error
 *      RT_ERR_ENABLE   - Invalid enable input.
 * Note:
 *      This API can set all Ports Green Ethernet state.
 *      The configuration is as following:
 *      - DISABLE
 *      - ENABLE
 */
extern rtksw_api_ret_t rtksw_switch_greenEthernet_set(rtksw_uint32 unit, rtksw_enable_t enable);

/* Function Name:
 *      rtksw_switch_greenEthernet_get
 * Description:
 *      Get all Ports Green Ethernet state.
 * Input:
 *      unit    - Unit ID
 *      None
 * Output:
 *      pEnable - Green Ethernet state.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 * Note:
 *      This API can get Green Ethernet state.
 */
extern rtksw_api_ret_t rtksw_switch_greenEthernet_get(rtksw_uint32 unit, rtksw_enable_t *pEnable);

/* Function Name:
 *      rtksw_switch_maxLogicalPort_get
 * Description:
 *      Get Max logical port ID
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      Max logical port
 * Note:
 *      This API can get max logical port
 */
extern rtksw_port_t rtksw_switch_maxLogicalPort_get(rtksw_uint32 unit);

/* Function Name:
 *      rtksw_switch_maxMeterId_get
 * Description:
 *      Get Max Meter ID
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      0x00                - Not Initialize
 *      Other value         - Max Meter ID
 * Note:
 *
 */
extern rtksw_uint32 rtksw_switch_maxMeterId_get(rtksw_uint32 unit);

/* Function Name:
 *      rtksw_switch_maxLutAddrNumber_get
 * Description:
 *      Get Max LUT Address number
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      0x00                - Not Initialize
 *      Other value         - Max LUT Address number
 * Note:
 *
 */
extern rtksw_uint32 rtksw_switch_maxLutAddrNumber_get(rtksw_uint32 unit);

/* Function Name:
 *      rtksw_switch_isValidTrunkGrpId
 * Description:
 *      Check if trunk group is valid or not
 * Input:
 *      grpId       - Group ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - Trunk Group ID is valid
 *      RT_ERR_LA_TRUNK_ID  - Trunk Group ID is not valid
 * Note:
 *
 */
rtksw_uint32 rtksw_switch_isValidTrunkGrpId(rtksw_uint32 unit, rtksw_uint32 grpId);

/* Function Name:
 *      rtksw_switch_maxBufferPageNum_get
 * Description:
 *      Get number of packet buffer page
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      Number of packet buffer page
 * Note:
 *
 */
rtksw_uint32 rtksw_switch_maxBufferPageNum_get(rtksw_uint32 unit);

/* Function Name:
 *      rtksw_switch_chipType_get
 * Description:
 *      Get switch chip type
 * Input:
 *      unit    - Unit ID
 * Output:
 *      None
 * Return:
 *      CHIP_END    - Unknown chip type
 *      other       - Switch chip type
 * Note:
 *
 */
switch_chip_t rtksw_switch_chipType_get(rtksw_uint32 unit);

/* Function Name:
 *      rtksw_switch_reset
 * Description:
 *      Reset chip.
 * Input:
 *      unit                - Unit ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      The API can reset entire chip.
 */
extern rtksw_api_ret_t rtksw_switch_reset(rtksw_uint32 unit);

#if defined(CONFIG_COMPATIBLE_V1_4)

#define rtk_switch_init(void)                               rtksw_switch_init(0)
#define rtk_switch_portMaxPktLen_set(port, speed, cfgId)    rtksw_switch_portMaxPktLen_set(0, port, speed, cfgId)
#define rtk_switch_portMaxPktLen_get(port, speed, pCfgId)   rtksw_switch_portMaxPktLen_get(0, port, speed, pCfgId)
#define rtk_switch_maxPktLenCfg_set(fgId, pktLen)           rtksw_switch_maxPktLenCfg_set(0, cfgId, pktLen)
#define rtk_switch_maxPktLenCfg_get(cfgId, pPktLen)         rtksw_switch_maxPktLenCfg_get(0, cfgId, pPktLen)
#define rtk_switch_greenEthernet_set(enable)                rtksw_switch_greenEthernet_set(0, enable)
#define rtk_switch_greenEthernet_get(pEnable)               rtksw_switch_greenEthernet_get(0, pEnable)

#define MAXPKTLEN_LINK_SPEED_FE         RTKSW_MAXPKTLEN_LINK_SPEED_FE
#define MAXPKTLEN_LINK_SPEED_GE         RTKSW_MAXPKTLEN_LINK_SPEED_GE

#define RTK_SCAN_ALL_PHY_PORTMASK       RTKSW_SCAN_ALL_PHY_PORTMASK
#define RTK_SCAN_ALL_LOG_PORT           RTKSW_SCAN_ALL_LOG_PORT  
#define RTK_SCAN_ALL_LOG_PORTMASK       RTKSW_SCAN_ALL_LOG_PORTMASK

#define RTK_MAX_METER_ID        RTKSW_MAX_METER_ID
#define RTK_MAX_BUF_PAGE_NUM    RTKSW_MAX_BUF_PAGE_NUM

#define RTK_PORTMASK_IS_PORT_SET    RTKSW_PORTMASK_IS_PORT_SET
#define RTK_PORTMASK_IS_EMPTY       RTKSW_PORTMASK_IS_EMPTY
#define RTK_PORTMASK_CLEAR          RTKSW_PORTMASK_CLEAR
#define RTK_PORTMASK_PORT_SET       RTKSW_PORTMASK_PORT_SET
#define RTK_PORTMASK_PORT_CLEAR     RTKSW_PORTMASK_PORT_CLEAR
#define RTK_PORTMASK_ALLPORT_SET    RTKSW_PORTMASK_ALLPORT_SET
#define RTK_PORTMASK_SCAN           RTKSW_PORTMASK_SCAN
#define RTK_PORTMASK_COMPARE        RTKSW_PORTMASK_COMPARE

#define rtk_switch_maxPktLen_linkSpeed_t  rtksw_switch_maxPktLen_linkSpeed_t

#endif /* CONFIG_COMPATIBLE_V1_4 */

#endif
