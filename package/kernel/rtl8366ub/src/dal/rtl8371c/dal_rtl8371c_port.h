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

#ifndef __DAL_RTL8371C_PORT_H__
#define __DAL_RTL8371C_PORT_H__

#include <port.h>

/*
 * Data Type Declaration
 */
#define RTL8371C_PHY_REGNOMAX           0x1F

#define RTL8371C_MAX_PACKET_LENGTH      0x3FEF

#define RTL8371C_MAX_SDS_ID             1
#define RTL8371C_MAX_SDS_PAGE           63
#define RTL8371C_MAX_SDS_REGISTER       31

/* Function Name:
 *      dal_rtl8371c_port_phyAutoNegoAbility_set
 * Description:
 *      Set ethernet PHY auto-negotiation desired ability.
 * Input:
 *      unit        - Unit ID
 *      port        - port id.
 *      pAbility    - Ability structure
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_ID          - Invalid port number.
 *      RT_ERR_PHY_REG_ID       - Invalid PHY address
 *      RT_ERR_INPUT            - Invalid input parameters.
 *      RT_ERR_BUSYWAIT_TIMEOUT - PHY access busy
 * Note:
 *      If Full_1000 bit is set to 1, the AutoNegotiation will be automatic set to 1. While both AutoNegotiation and Full_1000 are set to 0, the PHY speed and duplex selection will
 *      be set as following 100F > 100H > 10F > 10H priority sequence.
 */
extern rtksw_api_ret_t dal_rtl8371c_port_phyAutoNegoAbility_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_phy_ability_t *pAbility);

/* Function Name:
 *      dal_rtl8371c_port_phyAutoNegoAbility_get
 * Description:
 *      Get PHY ability through PHY registers.
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 * Output:
 *      pAbility - Ability structure
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_ID          - Invalid port number.
 *      RT_ERR_PHY_REG_ID       - Invalid PHY address
 *      RT_ERR_INPUT            - Invalid input parameters.
 *      RT_ERR_BUSYWAIT_TIMEOUT - PHY access busy
 * Note:
 *      Get the capablity of specified PHY.
 */
extern rtksw_api_ret_t dal_rtl8371c_port_phyAutoNegoAbility_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_phy_ability_t *pAbility);

/* Function Name:
 *      dal_rtl8371c_port_phyForceModeAbility_set
 * Description:
 *      Set the port speed/duplex mode/pause/asy_pause in the PHY force mode.
 * Input:
 *      unit        - Unit ID
 *      port        - port id.
 *      pAbility    - Ability structure
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_ID          - Invalid port number.
 *      RT_ERR_PHY_REG_ID       - Invalid PHY address
 *      RT_ERR_INPUT            - Invalid input parameters.
 *      RT_ERR_BUSYWAIT_TIMEOUT - PHY access busy
 * Note:
 *      While both AutoNegotiation and Full_1000 are set to 0, the PHY speed and duplex selection will
 *      be set as following 100F > 100H > 10F > 10H priority sequence.
 */
extern rtksw_api_ret_t dal_rtl8371c_port_phyForceModeAbility_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_phy_ability_t *pAbility);

/* Function Name:
 *      dal_rtl8371c_port_phyForceModeAbility_get
 * Description:
 *      Get PHY ability through PHY registers.
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 * Output:
 *      pAbility - Ability structure
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_ID          - Invalid port number.
 *      RT_ERR_PHY_REG_ID       - Invalid PHY address
 *      RT_ERR_INPUT            - Invalid input parameters.
 *      RT_ERR_BUSYWAIT_TIMEOUT - PHY access busy
 * Note:
 *      Get the capablity of specified PHY.
 */
extern rtksw_api_ret_t dal_rtl8371c_port_phyForceModeAbility_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_phy_ability_t *pAbility);

/* Function Name:
 *      dal_rtl8371c_port_phyStatus_get
 * Description:
 *      Get ethernet PHY linking status
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 * Output:
 *      linkStatus  - PHY link status
 *      speed       - PHY link speed
 *      duplex      - PHY duplex mode
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_ID          - Invalid port number.
 *      RT_ERR_PHY_REG_ID       - Invalid PHY address
 *      RT_ERR_INPUT            - Invalid input parameters.
 *      RT_ERR_BUSYWAIT_TIMEOUT - PHY access busy
 * Note:
 *      API will return auto negotiation status of phy.
 */
extern rtksw_api_ret_t dal_rtl8371c_port_phyStatus_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_linkStatus_t *pLinkStatus, rtksw_port_speed_t *pSpeed, rtksw_port_duplex_t *pDuplex);

/* Function Name:
 *      dal_rtl8371c_port_macForceLink_set
 * Description:
 *      Set port force linking configuration.
 * Input:
 *      unit            - Unit ID
 *      port            - port id.
 *      pPortability    - port ability configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      This API can set Port/MAC force mode properties.
 */
extern rtksw_api_ret_t dal_rtl8371c_port_macForceLink_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_mac_ability_t *pPortability);

/* Function Name:
 *      dal_rtl8371c_port_macForceLink_get
 * Description:
 *      Get port force linking configuration.
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 * Output:
 *      pPortability - port ability configuration
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API can get Port/MAC force mode properties.
 */
extern rtksw_api_ret_t dal_rtl8371c_port_macForceLink_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_mac_ability_t *pPortability);

/* Function Name:
 *      dal_rtl8371c_port_macForceLinkExt_set
 * Description:
 *      Set external interface force linking configuration.
 * Input:
 *      unit            - Unit ID
 *      port            - external port ID
 *      mode            - external interface mode
 *      pPortability    - port ability configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API can set external interface force mode properties.
 *      The external interface can be set to:
 *      - RTKSW_MODE_EXT_DISABLE,
 *      - RTKSW_MODE_EXT_RGMII,
 *      - RTKSW_MODE_EXT_MII_MAC,
 *      - RTKSW_MODE_EXT_MII_PHY,
 *      - RTKSW_MODE_EXT_TMII_MAC,
 *      - RTKSW_MODE_EXT_TMII_PHY,
 *      - RTKSW_MODE_EXT_GMII,
 *      - RTKSW_MODE_EXT_RMII_MAC,
 *      - RTKSW_MODE_EXT_RMII_PHY,
 *      - RTKSW_MODE_EXT_SGMII,
 *      - RTKSW_MODE_EXT_HSGMII,
 */
extern rtksw_api_ret_t dal_rtl8371c_port_macForceLinkExt_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_mode_ext_t mode, rtksw_port_mac_ability_t *pPortability);

/* Function Name:
 *      dal_rtl8371c_port_macForceLinkExt_get
 * Description:
 *      Set external interface force linking configuration.
 * Input:
 *      unit            - Unit ID
 *      port            - external port ID
 * Output:
 *      pMode           - external interface mode
 *      pPortability    - port ability configuration
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API can get external interface force mode properties.
 */
extern rtksw_api_ret_t dal_rtl8371c_port_macForceLinkExt_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_mode_ext_t *pMode, rtksw_port_mac_ability_t *pPortability);

/* Function Name:
 *      dal_rtl8371c_port_macStatus_get
 * Description:
 *      Get port link status.
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 * Output:
 *      pPortstatus - port ability configuration
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      This API can get Port/PHY properties.
 */
extern rtksw_api_ret_t dal_rtl8371c_port_macStatus_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_mac_ability_t *pPortstatus);

/* Function Name:
 *      dal_rtl8371c_port_macLocalLoopbackEnable_set
 * Description:
 *      Set Port Local Loopback. (Redirect TX to RX.)
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 *      enable  - Loopback state, 0:disable, 1:enable
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      This API can enable/disable Local loopback in MAC.
 *      For UTP port, This API will also enable the digital
 *      loopback bit in PHY register for sync of speed between
 *      PHY and MAC. For EXT port, users need to force the
 *      link state by themself.
 */
extern rtksw_api_ret_t dal_rtl8371c_port_macLocalLoopbackEnable_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t enable);

/* Function Name:
 *      dal_rtl8371c_port_macLocalLoopbackEnable_get
 * Description:
 *      Get Port Local Loopback. (Redirect TX to RX.)
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 * Output:
 *      pEnable  - Loopback state, 0:disable, 1:enable
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      None.
 */
extern rtksw_api_ret_t dal_rtl8371c_port_macLocalLoopbackEnable_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t *pEnable);

/* Function Name:
 *      dal_rtl8371c_port_phyReg_set
 * Description:
 *      Set PHY register data of the specific port.
 * Input:
 *      unit    - Unit ID
 *      port    - port id.
 *      reg     - Register id
 *      value   - Register data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_ID          - Invalid port number.
 *      RT_ERR_PHY_REG_ID       - Invalid PHY address
 *      RT_ERR_BUSYWAIT_TIMEOUT - PHY access busy
 * Note:
 *      This API can set PHY register data of the specific port.
 */
extern rtksw_api_ret_t dal_rtl8371c_port_phyReg_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_phy_reg_t reg, rtksw_port_phy_data_t value);

/* Function Name:
 *      dal_rtl8371c_port_phyReg_get
 * Description:
 *      Get PHY register data of the specific port.
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 *      reg     - Register id
 * Output:
 *      pData   - Register data
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_ID          - Invalid port number.
 *      RT_ERR_PHY_REG_ID       - Invalid PHY address
 *      RT_ERR_BUSYWAIT_TIMEOUT - PHY access busy
 * Note:
 *      This API can get PHY register data of the specific port.
 */
extern rtksw_api_ret_t dal_rtl8371c_port_phyReg_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_phy_reg_t reg, rtksw_port_phy_data_t *pData);

/* Function Name:
 *      dal_rtl8371c_port_phyOCPReg_set
 * Description:
 *      Set PHY OCP register
 * Input:
 *      unit        - Unit ID
 *      port        - PHY ID
 *      ocpAddr     - OCP register address
 *      ocpData     - OCP Data.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK                               - OK
 *      RT_ERR_FAILED                           - Failed
 *      RT_ERR_BUSYWAIT_TIMEOUT                 - Timeout
 * Note:
 *      None.
 */
extern rtksw_api_ret_t dal_rtl8371c_port_phyOCPReg_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_uint32 ocpAddr, rtksw_uint32 ocpData);

/* Function Name:
 *      dal_rtl8371c_port_phyOCPReg_get
 * Description:
 *      Set PHY OCP register
 * Input:
 *      unit        - Unit ID
 *      phyNo       - PHY ID
 *      ocpAddr     - OCP register address
 * Output:
 *      pRegData    - OCP data.
 * Return:
 *      RT_ERR_OK                               - OK
 *      RT_ERR_FAILED                           - Failed
 *      RT_ERR_NULL_POINTER                     - Null pointer
 * Note:
 *      None.
 */
extern rtksw_api_ret_t dal_rtl8371c_port_phyOCPReg_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_uint32 ocpAddr, rtksw_uint32 *pRegData);

/* Function Name:
 *      dal_rtl8371c_port_backpressureEnable_set
 * Description:
 *      Set the half duplex backpressure enable status of the specific port.
 * Input:
 *      unit    - Unit ID
 *      port    - port id.
 *      enable  - Back pressure status.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_ENABLE       - Invalid enable input.
 * Note:
 *      This API can set the half duplex backpressure enable status of the specific port.
 *      The half duplex backpressure enable status of the port is as following:
 *      - DISABLE
 *      - ENABLE
 */
extern rtksw_api_ret_t dal_rtl8371c_port_backpressureEnable_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t enable);

/* Function Name:
 *      dal_rtl8371c_port_backpressureEnable_get
 * Description:
 *      Get the half duplex backpressure enable status of the specific port.
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 * Output:
 *      pEnable - Back pressure status.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      This API can get the half duplex backpressure enable status of the specific port.
 *      The half duplex backpressure enable status of the port is as following:
 *      - DISABLE
 *      - ENABLE
 */
extern rtksw_api_ret_t dal_rtl8371c_port_backpressureEnable_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t *pEnable);

/* Function Name:
 *      dal_rtl8371c_port_adminEnable_set
 * Description:
 *      Set port admin configuration of the specific port.
 * Input:
 *      unit    - Unit ID
 *      port    - port id.
 *      enable  - Back pressure status.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_ENABLE       - Invalid enable input.
 * Note:
 *      This API can set port admin configuration of the specific port.
 *      The port admin configuration of the port is as following:
 *      - DISABLE
 *      - ENABLE
 */
extern rtksw_api_ret_t dal_rtl8371c_port_adminEnable_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t enable);

/* Function Name:
 *      dal_rtl8371c_port_adminEnable_get
 * Description:
 *      Get port admin configurationof the specific port.
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 * Output:
 *      pEnable - Back pressure status.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      This API can get port admin configuration of the specific port.
 *      The port admin configuration of the port is as following:
 *      - DISABLE
 *      - ENABLE
 */
extern rtksw_api_ret_t dal_rtl8371c_port_adminEnable_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t *pEnable);

/* Function Name:
 *      dal_rtl8371c_port_isolation_set
 * Description:
 *      Set permitted port isolation portmask
 * Input:
 *      unit        - Unit ID
 *      port        - port id.
 *      pPortmask   - Permit port mask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_PORT_MASK    - Invalid portmask.
 * Note:
 *      This API set the port mask that a port can trasmit packet to of each port
 *      A port can only transmit packet to ports included in permitted portmask
 */
extern rtksw_api_ret_t dal_rtl8371c_port_isolation_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_portmask_t *pPortmask);

/* Function Name:
 *      dal_rtl8371c_port_isolation_get
 * Description:
 *      Get permitted port isolation portmask
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 * Output:
 *      pPortmask - Permit port mask
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      This API get the port mask that a port can trasmit packet to of each port
 *      A port can only transmit packet to ports included in permitted portmask
 */
extern rtksw_api_ret_t dal_rtl8371c_port_isolation_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_portmask_t *pPortmask);

/* Function Name:
 *      dal_rtl8371c_port_phyEnableAll_set
 * Description:
 *      Set all PHY enable status.
 * Input:
 *      unit    - Unit ID
 *      enable  - PHY Enable State.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_ENABLE       - Invalid enable input.
 * Note:
 *      This API can set all PHY status.
 *      The configuration of all PHY is as following:
 *      - DISABLE
 *      - ENABLE
 */
extern rtksw_api_ret_t dal_rtl8371c_port_phyEnableAll_set(rtksw_uint32 unit, rtksw_enable_t enable);

/* Function Name:
 *      dal_rtl8371c_port_phyEnableAll_get
 * Description:
 *      Get all PHY enable status.
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pEnable - PHY Enable State.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      This API can set all PHY status.
 *      The configuration of all PHY is as following:
 *      - DISABLE
 *      - ENABLE
 */
extern rtksw_api_ret_t dal_rtl8371c_port_phyEnableAll_get(rtksw_uint32 unit, rtksw_enable_t *pEnable);

/* Function Name:
 *      dal_rtl8371c_port_phyComboPortMedia_set
 * Description:
 *      Set Combo port media type
 * Input:
 *      unit    - Unit ID
 *      port    - Port id. (Should be Port 4)
 *      media   - Media (COPPER or FIBER or AUTO)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_INPUT            - Invalid input parameters.
 *      RT_ERR_PORT_ID          - Invalid port ID.
 * Note:
 *      The API can Set Combo port media type.
 */
extern rtksw_api_ret_t dal_rtl8371c_port_phyComboPortMedia_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_media_t media);

/* Function Name:
 *      dal_rtl8371c_port_phyComboPortMedia_get
 * Description:
 *      Get Combo port media type
 * Input:
 *      unit    - Unit ID
 *      port    - Port id. (Should be Port 4)
 * Output:
 *      pMedia  - Media (COPPER or FIBER or AUTO)
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_INPUT            - Invalid input parameters.
 *      RT_ERR_PORT_ID          - Invalid port ID.
 * Note:
 *      The API can Set Combo port media type.
 */
extern rtksw_api_ret_t dal_rtl8371c_port_phyComboPortMedia_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_media_t *pMedia);

/* Function Name:
 *      dal_rtl8371c_port_rtctEnable_set
 * Description:
 *      Enable RTCT test
 * Input:
 *      unit        - Unit ID
 *      pPortmask   - Port mask of RTCT enabled port
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_MASK        - Invalid port mask.
 * Note:
 *      The API can enable RTCT Test
 */
extern rtksw_api_ret_t dal_rtl8371c_port_rtctEnable_set(rtksw_uint32 unit, rtksw_portmask_t *pPortmask);

/* Function Name:
 *      dal_rtl8371c_port_rtctDisable_set
 * Description:
 *      Disable RTCT test
 * Input:
 *      unit        - Unit ID
 *      pPortmask   - Port mask of RTCT disabled port
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_MASK        - Invalid port mask.
 * Note:
 *      The API can disable RTCT Test
 */
extern rtksw_api_ret_t dal_rtl8371c_port_rtctDisable_set(rtksw_uint32 unit, rtksw_portmask_t *pPortmask);

/* Function Name:
 *      dal_rtl8371c_port_rtctResult_get
 * Description:
 *      Get the result of RTCT test
 * Input:
 *      unit    - Unit ID
 *      port    - Port ID
 * Output:
 *      pRtctResult - The result of RTCT result
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_PORT_ID              - Invalid port ID.
 *      RT_ERR_PHY_RTCT_NOT_FINISH  - Testing does not finish.
 * Note:
 *      The API can get RTCT test result.
 *      RTCT test may takes 4.8 seconds to finish its test at most.
 *      Thus, if this API return RT_ERR_PHY_RTCT_NOT_FINISH or
 *      other error code, the result can not be referenced and
 *      user should call this API again until this API returns
 *      a RT_ERR_OK.
 *      The result is stored at pRtctResult->ge_result
 *      pRtctResult->linkType is unused.
 *      The unit of channel length is 2.5cm. Ex. 300 means 300 * 2.5 = 750cm = 7.5M
 */
extern rtksw_api_ret_t dal_rtl8371c_port_rtctResult_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_rtctResult_t *pRtctResult);

/* Function Name:
 *      dal_rtl8371c_port_sgmiiLinkStatus_get
 * Description:
 *      Get SGMII status
 * Input:
 *      unit        - Unit ID
 *      port        - Port ID
 * Output:
 *      pSignalDetect   - Signal detect
 *      pSync           - Sync
 *      pLink           - Link
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_PORT_ID              - Invalid port ID.
 * Note:
 *      The API can reset Serdes
 */
extern rtksw_api_ret_t dal_rtl8371c_port_sgmiiLinkStatus_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_data_t *pSignalDetect, rtksw_data_t *pSync, rtksw_port_linkStatus_t *pLink);

/* Function Name:
 *      dal_rtl8371c_port_sgmiiNway_set
 * Description:
 *      Configure SGMII/HSGMII port Nway state
 * Input:
 *      unit        - Unit ID
 *      port        - Port ID
 *      state       - Nway state
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_PORT_ID              - Invalid port ID.
 * Note:
 *      The API configure SGMII/HSGMII port Nway state
 */
extern rtksw_api_ret_t dal_rtl8371c_port_sgmiiNway_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t state);

/* Function Name:
 *      dal_rtl8371c_port_sgmiiNway_get
 * Description:
 *      Get SGMII/HSGMII port Nway state
 * Input:
 *      unit        - Unit ID
 *      port        - Port ID
 * Output:
 *      pState      - Nway state
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_PORT_ID              - Invalid port ID.
 * Note:
 *      The API can get SGMII/HSGMII port Nway state
 */
extern rtksw_api_ret_t dal_rtl8371c_port_sgmiiNway_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t *pState);

/* Function Name:
 *      dal_rtl8371c_port_autoDos_set
 * Description:
 *      Set Auto Dos state
 * Input:
 *      unit        - Unit ID
 *      type        - Auto DoS type
 *      state       - 1: Eanble(Drop), 0: Disable(Forward)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 * Note:
 *      The API can set Auto Dos state
 */
extern rtksw_api_ret_t dal_rtl8371c_port_autoDos_set(rtksw_uint32 unit, rtksw_port_autoDosType_t type, rtksw_enable_t state);

/* Function Name:
 *      dal_rtl8371c_port_autoDos_get
 * Description:
 *      Get Auto Dos state
 * Input:
 *      unit        - Unit ID
 *      type        - Auto DoS type
 * Output:
 *      pState      - 1: Eanble(Drop), 0: Disable(Forward)
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_NULL_POINTER         - Null Pointer
 * Note:
 *      The API can get Auto Dos state
 */
extern rtksw_api_ret_t dal_rtl8371c_port_autoDos_get(rtksw_uint32 unit, rtksw_port_autoDosType_t type, rtksw_enable_t *pState);

/* Function Name:
 *      dal_rtl8371c_port_fiberAbility_set
 * Description:
 *      Configure fiber port ability
 * Input:
 *      unit        - Unit ID
 *      port        - Port ID
 *      pAbility    - Fiber port ability
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_PORT_ID              - Invalid port ID.
 * Note:
 *      The API can configure fiber port ability
 */
extern rtksw_api_ret_t dal_rtl8371c_port_fiberAbility_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_fiber_ability_t *pAbility);

/* Function Name:
 *      dal_rtl8371c_port_fiberAbility_get
 * Description:
 *      Get fiber port ability
 * Input:
 *      unit        - Unit ID
 *      port        - Port ID
 * Output:
 *      pAbility    - Fiber port ability
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_PORT_ID              - Invalid port ID.
 * Note:
 *      The API can get fiber port ability
 */
extern rtksw_api_ret_t dal_rtl8371c_port_fiberAbility_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_fiber_ability_t *pAbility);

/* Function Name:
 *      dal_rtl8371c_port_phyMdx_set
 * Description:
 *      Set PHY MDI/MDIX state
 * Input:
 *      unit        - Unit ID
 *      port        - port ID
 *      mode        - PHY MDI/MDIX mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 * Note:
 *      The API can set PHY MDI/MDIX state
 */
extern rtksw_api_ret_t dal_rtl8371c_port_phyMdx_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_phy_mdix_mode_t mode);

/* Function Name:
 *      dal_rtl8371c_port_phyMdx_get
 * Description:
 *      Get PHY MDI/MDIX state
 * Input:
 *      unit        - Unit ID
 *      port        - port ID
 * Output:
 *      pMode       - PHY MDI/MDIX mode
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 * Note:
 *      The API can get PHY MDI/MDIX state
 */
extern rtksw_api_ret_t dal_rtl8371c_port_phyMdx_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_phy_mdix_mode_t *pMode);

/* Function Name:
 *      dal_rtl8371c_port_phyMdxStatus_get
 * Description:
 *      Get PHY MDI/MDIX status
 * Input:
 *      unit        - Unit ID
 *      port        - port ID
 * Output:
 *      pStatus     - PHY MDI/MDIX status
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 * Note:
 *      The API can get PHY MDI/MDIX status
 */
extern rtksw_api_ret_t dal_rtl8371c_port_phyMdxStatus_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_phy_mdix_status_t *pStatus);

/* Function Name:
 *      dal_rtl8371c_port_phyTestMode_set
 * Description:
 *      Set PHY in test mode.
 * Input:
 *      unit    - Unit ID
 *      port    - port id.
 *      mode    - PHY test mode 0:normal 1:test mode 1 2:test mode 2 3: test mode 3 4:test mode 4 5~7:reserved
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_ID          - Invalid port number.
 *      RT_ERR_BUSYWAIT_TIMEOUT - PHY access busy
 *      RT_ERR_NOT_ALLOWED      - The Setting is not allowed, caused by set more than 1 port in Test mode.
 * Note:
 * 
 */
extern rtksw_api_ret_t dal_rtl8371c_port_phyTestMode_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_phy_test_mode_t mode);

/* Function Name:
 *      dal_rtl8371c_port_phyTestMode_get
 * Description:
 *      Get PHY in which test mode.
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 * Output:
 *      mode - PHY test mode 0:normal 1:test mode 1 2:test mode 2 3: test mode 3 4:test mode 4 5~7:reserved
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_ID          - Invalid port number.
 *      RT_ERR_INPUT            - Invalid input parameters.
 *      RT_ERR_BUSYWAIT_TIMEOUT - PHY access busy
 * Note:
 * 
 */
extern rtksw_api_ret_t dal_rtl8371c_port_phyTestMode_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_phy_test_mode_t *pMode);

/* Function Name:
 *      dal_rtl8371c_port_maxPacketLength_set
 * Description:
 *      Set Max packet length per port
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 *      length  - Max packet length.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_OUT_OF_RANGE    - The parameter is out of range
 *      RT_ERR_INPUT           - Input error
 * Note:
 *      None.
 */
extern rtksw_api_ret_t dal_rtl8371c_port_maxPacketLength_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_uint32 length);

/* Function Name:
 *      dal_rtl8371c_port_maxPacketLength_get
 * Description:
 *      Get Max packet length per port
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 * Output:
 *      pLength - Pointer of Max packet length.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_OUT_OF_RANGE    - The parameter is out of range
 *      RT_ERR_INPUT           - Input error
 * Note:
 *      None.
 */
extern rtksw_api_ret_t dal_rtl8371c_port_maxPacketLength_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_uint32 *pLength);

/* Function Name:
 *      dal_rtl8371c_port_phyLinkDownPowerSaving_set
 * Description:
 *      Set Ports Link Down Power Saving state.
 * Input:
 *      unit    - Unit ID
 *      port    - port ID
 *      state   - Link Down Power Saving state.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - OK
 *      RT_ERR_FAILED   - Failed
 *      RT_ERR_SMI      - SMI access error
 *      RT_ERR_ENABLE   - Invalid enable input.
 * Note:
 *      This API can set Port Link Down Power Saving state.
 *      The configuration is as following:
 *      - DISABLE
 *      - ENABLE
 */
extern rtksw_api_ret_t dal_rtl8371c_port_phyLinkDownPowerSaving_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t state);

/* Function Name:
 *      dal_rtl8371c_port_phyLinkDownPowerSaving_get
 * Description:
 *      Get Ports Link Down Power Saving state.
 * Input:
 *      unit    - Unit ID
 *      port    - port ID
 * Output:
 *      pState  - Link Down Power Saving state.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 * Note:
 *      This API can get Link Down Power Saving state.
 */
extern rtksw_api_ret_t dal_rtl8371c_port_phyLinkDownPowerSaving_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t *pState);

/* Function Name:
 *      dal_rtl8371c_port_serdesReg_set
 * Description:
 *      Set Serdes register data of the specific port.
 * Input:
 *      unit    - Unit ID
 *      port    - port id.
 *      page    - Page id.
 *      reg     - Register id
 *      data    - Register data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_INPUT            - Error input
 *      RT_ERR_BUSYWAIT_TIMEOUT - Access busy
 * Note:
 *      This API can set serdes register data of the specific sdsID.
 */
extern rtksw_api_ret_t dal_rtl8371c_port_serdesReg_set(rtksw_uint32 unit, rtksw_uint32 sdsID, rtksw_uint32 page, rtksw_uint32 reg, rtksw_uint32 data);

/* Function Name:
 *      dal_rtl8371c_port_serdesReg_get
 * Description:
 *      Get Serdes register data of the specific port.
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 *      page    - Page id.
 *      reg     - Register id
 * Output:
 *      pData   - Register data
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_INPUT            - Error input
 *      RT_ERR_NULL_POINTER     - NULL pointer
 *      RT_ERR_BUSYWAIT_TIMEOUT - Access busy
 * Note:
 *      This API can get Serdes register data of the specific sdsID.
 */
extern rtksw_api_ret_t dal_rtl8371c_port_serdesReg_get(rtksw_uint32 unit, rtksw_uint32 sdsID, rtksw_uint32 page, rtksw_uint32 reg, rtksw_uint32 *pData);

/* Function Name:
 *      dal_rtl8371c_port_serdesPolarity_set
 * Description:
 *      Set Serdes polarity.
 * Input:
 *      unit            - Unit ID
 *      sdsID           - Serdes id.
 *      inputPolarity   - Input Polarity.
 *      outputPolarity  - Output Polarity
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_INPUT            - Error input
 * Note:
 *      This API can set serdes polarity of the specific sdsID.
 */
extern rtksw_api_ret_t dal_rtl8371c_port_serdesPolarity_set(rtksw_uint32 unit, rtksw_uint32 sdsID, rtksw_port_sdsPolarity_t inputPolarity, rtksw_port_sdsPolarity_t outputPolarity);

/* Function Name:
 *      dal_rtl8371c_port_serdesPolarity_get
 * Description:
 *      Get Serdes polarity.
 * Input:
 *      unit        - Unit ID
 *      sdsID       - Serdes id.
 * Output:
 *      pInputPolarity  - Input Polarity.
 *      pOutputPolarity - Output Polarity
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_INPUT            - Error input
 *      RT_ERR_NULL_POINTER     - Null pointer
 * Note:
 *      This API can set serdes polarity of the specific sdsID.
 */
extern rtksw_api_ret_t dal_rtl8371c_port_serdesPolarity_get(rtksw_uint32 unit, rtksw_uint32 sdsID, rtksw_port_sdsPolarity_t *pInputPolarity, rtksw_port_sdsPolarity_t *pOutputPolarity);

#endif /* __DAL_RTL8371C_PORT_H__ */



