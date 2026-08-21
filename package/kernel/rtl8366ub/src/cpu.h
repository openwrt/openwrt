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

#ifndef __RTKSW_API_CPU_H__
#define __RTKSW_API_CPU_H__


/*
 * Data Type Declaration
 */
typedef enum rtksw_cpu_insert_e
{
    CPU_INSERT_TO_ALL = 0,
    CPU_INSERT_TO_TRAPPING,
    CPU_INSERT_TO_NONE,
    CPU_INSERT_END
}rtksw_cpu_insert_t;

typedef enum rtksw_cpu_position_e
{
    CPU_POS_AFTER_SA = 0,
    CPU_POS_BEFORE_CRC,
    CPU_POS_END
}rtksw_cpu_position_t;

typedef enum rtksw_cpu_tag_length_e
{
    CPU_LEN_8BYTES = 0,
    CPU_LEN_4BYTES,
    CPU_LEN_4BYTES_PRIORITY,
    CPU_LEN_END
}rtksw_cpu_tag_length_t;


typedef enum rtksw_cpu_rx_length_e
{
    CPU_RX_72BYTES = 0,
    CPU_RX_64BYTES,
    CPU_RX_END
}rtksw_cpu_rx_length_t;


/* Function Name:
 *      rtksw_cpu_enable_set
 * Description:
 *      Set CPU port function enable/disable.
 * Input:
 *      unit    - Unit ID
 *      enable  - CPU port function enable
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameter.
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      The API can set CPU port function enable/disable.
 */
extern rtksw_api_ret_t rtksw_cpu_enable_set(rtksw_uint32 unit, rtksw_enable_t enable);

/* Function Name:
 *      rtksw_cpu_enable_get
 * Description:
 *      Get CPU port and its setting.
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pEnable - CPU port function enable
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_INPUT            - Invalid input parameters.
 *      RT_ERR_L2_NO_CPU_PORT   - CPU port is not exist
 * Note:
 *      The API can get CPU port function enable/disable.
 */
extern rtksw_api_ret_t rtksw_cpu_enable_get(rtksw_uint32 unit, rtksw_enable_t *pEnable);

/* Function Name:
 *      rtksw_cpu_tagPort_set
 * Description:
 *      Set CPU port and CPU tag insert mode.
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 *      mode    - CPU tag insert for packets egress from CPU port.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameter.
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      The API can set CPU port and inserting proprietary CPU tag mode (Length/Type 0x8899)
 *      to the frame that transmitting to CPU port.
 *      The inset cpu tag mode is as following:
 *      - CPU_INSERT_TO_ALL
 *      - CPU_INSERT_TO_TRAPPING
 *      - CPU_INSERT_TO_NONE
 */
extern rtksw_api_ret_t rtksw_cpu_tagPort_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_cpu_insert_t mode);

/* Function Name:
 *      rtksw_cpu_tagPort_get
 * Description:
 *      Get CPU port and CPU tag insert mode.
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pPort   - Port id.
 *      pMode   - CPU tag insert for packets egress from CPU port, 0:all insert 1:Only for trapped packets 2:no insert.
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_INPUT            - Invalid input parameters.
 *      RT_ERR_L2_NO_CPU_PORT   - CPU port is not exist
 * Note:
 *      The API can get configured CPU port and its setting.
 *      The inset cpu tag mode is as following:
 *      - CPU_INSERT_TO_ALL
 *      - CPU_INSERT_TO_TRAPPING
 *      - CPU_INSERT_TO_NONE
 */
extern rtksw_api_ret_t rtksw_cpu_tagPort_get(rtksw_uint32 unit, rtksw_port_t *pPort, rtksw_cpu_insert_t *pMode);

/* Function Name:
 *      rtksw_cpu_awarePort_set
 * Description:
 *      Set CPU aware port mask.
 * Input:
 *      unit        - Unit ID
 *      portmask    - Port mask.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_MASK      - Invalid port mask.
 * Note:
 *      The API can set configured CPU aware port mask.
 */
extern rtksw_api_ret_t rtksw_cpu_awarePort_set(rtksw_uint32 unit, rtksw_portmask_t *pPortmask);


/* Function Name:
 *      rtksw_cpu_awarePort_get
 * Description:
 *      Get CPU aware port mask.
 * Input:
 *      unit        - Unit ID
 * Output:
 *      pPortmask   - Port mask.
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 * Note:
 *      The API can get configured CPU aware port mask.
 */
extern rtksw_api_ret_t rtksw_cpu_awarePort_get(rtksw_uint32 unit, rtksw_portmask_t *pPortmask);

/* Function Name:
 *      rtksw_cpu_tagPosition_set
 * Description:
 *      Set CPU tag position.
 * Input:
 *      unit        - Unit ID
 *      position    - CPU tag position.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT      - Invalid input.
 * Note:
 *      The API can set CPU tag position.
 */
extern rtksw_api_ret_t rtksw_cpu_tagPosition_set(rtksw_uint32 unit, rtksw_cpu_position_t position);

/* Function Name:
 *      rtksw_cpu_tagPosition_get
 * Description:
 *      Get CPU tag position.
 * Input:
 *      unit        - Unit ID
 * Output:
 *      pPosition   - CPU tag position.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT      - Invalid input.
 * Note:
 *      The API can get CPU tag position.
 */
extern rtksw_api_ret_t rtksw_cpu_tagPosition_get(rtksw_uint32 unit, rtksw_cpu_position_t *pPosition);

/* Function Name:
 *      rtksw_cpu_tagLength_set
 * Description:
 *      Set CPU tag length.
 * Input:
 *      unit    - Unit ID
 *      length  - CPU tag length.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT      - Invalid input.
 * Note:
 *      The API can set CPU tag length.
 */
extern rtksw_api_ret_t rtksw_cpu_tagLength_set(rtksw_uint32 unit, rtksw_cpu_tag_length_t length);

/* Function Name:
 *      rtksw_cpu_tagLength_get
 * Description:
 *      Get CPU tag length.
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pLength - CPU tag length.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT      - Invalid input.
 * Note:
 *      The API can get CPU tag length.
 */
extern rtksw_api_ret_t rtksw_cpu_tagLength_get(rtksw_uint32 unit, rtksw_cpu_tag_length_t *pLength);

/* Function Name:
 *      rtksw_cpu_acceptLength_set
 * Description:
 *      Set CPU accept  length.
 * Input:
 *      unit    - Unit ID
 *      length  - CPU tag length.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT      - Invalid input.
 * Note:
 *      The API can set CPU accept length.
 */
extern rtksw_api_ret_t rtksw_cpu_acceptLength_set(rtksw_uint32 unit, rtksw_cpu_rx_length_t length);

/* Function Name:
 *      rtksw_cpu_acceptLength_get
 * Description:
 *      Get CPU accept length.
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pLength - CPU tag length.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT      - Invalid input.
 * Note:
 *      The API can get CPU accept length.
 */
extern rtksw_api_ret_t rtksw_cpu_acceptLength_get(rtksw_uint32 unit, rtksw_cpu_rx_length_t *pLength);

/* Function Name:
 *      rtksw_cpu_priRemap_set
 * Description:
 *      Configure CPU priorities mapping to internal absolute priority.
 * Input:
 *      unit        - Unit ID
 *      int_pri     - internal priority value.
 *      new_pri     - new internal priority value.
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
 *      Priority of CPU tag assignment for internal asic priority, and it is used for queue usage and packet scheduling.
 */
extern rtksw_api_ret_t rtksw_cpu_priRemap_set(rtksw_uint32 unit, rtksw_pri_t int_pri, rtksw_pri_t new_pri);

/* Function Name:
 *      rtksw_cpu_priRemap_get
 * Description:
 *      Configure CPU priorities mapping to internal absolute priority.
 * Input:
 *      unit        - Unit ID
 *      int_pri     - internal priority value.
 * Output:
 *      pNew_pri    - new internal priority value.
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_INPUT            - Invalid input parameters.
 *      RT_ERR_VLAN_PRIORITY    - Invalid 1p priority.
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority.
 * Note:
 *      Priority of CPU tag assignment for internal asic priority, and it is used for queue usage and packet scheduling.
 */
extern rtksw_api_ret_t rtksw_cpu_priRemap_get(rtksw_uint32 unit, rtksw_pri_t int_pri, rtksw_pri_t *pNew_pri);

#if defined(CONFIG_COMPATIBLE_V1_4)

#define rtk_cpu_enable_set(enable)              rtksw_cpu_enable_set(0, enable)
#define rtk_cpu_enable_get(pEnable)             rtksw_cpu_enable_get(0, pEnable)
#define rtk_cpu_tagPort_set(port, mode)         rtksw_cpu_tagPort_set(0, port, mode)
#define rtk_cpu_tagPort_get(pPort, pMode)       rtksw_cpu_tagPort_get(0, pPort, pMode)
#define rtk_cpu_awarePort_set(pPortmask)        rtksw_cpu_awarePort_set(0, pPortmask)
#define rtk_cpu_awarePort_get(pPortmask)        rtksw_cpu_awarePort_get(0, pPortmask)
#define rtk_cpu_tagPosition_set(position)       rtksw_cpu_tagPosition_set(0, position)
#define rtk_cpu_tagPosition_get(pPosition)      rtksw_cpu_tagPosition_get(0, pPosition)
#define rtk_cpu_tagLength_set(length)           rtksw_cpu_tagLength_set(0, length)
#define rtk_cpu_tagLength_get(pLength)          rtksw_cpu_tagLength_get(0, pLength)
#define rtk_cpu_acceptLength_set(length)        rtksw_cpu_acceptLength_set(0, length)
#define rtk_cpu_acceptLength_get(pLength)       rtksw_cpu_acceptLength_get(0, pLength)
#define rtk_cpu_priRemap_set(int_pri, new_pri)  rtksw_cpu_priRemap_set(0, int_pri, new_pri)
#define rtk_cpu_priRemap_get(int_pri, pNew_pri) rtksw_cpu_priRemap_get(0, int_pri, pNew_pri)

#define rtk_cpu_insert_t        rtksw_cpu_insert_t
#define rtk_cpu_position_t      rtksw_cpu_position_t
#define rtk_cpu_tag_length_t    rtksw_cpu_tag_length_t
#define rtk_cpu_rx_length_t     rtksw_cpu_rx_length_t

#endif /* CONFIG_COMPATIBLE_V1_4 */

#endif /* __RTKSW_API_CPU_H__ */
