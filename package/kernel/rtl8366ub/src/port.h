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

#ifndef __RTKSW_API_PORT_H__
#define __RTKSW_API_PORT_H__

/*
 * Data Type Declaration
 */
#define PHY_CONTROL_REG                             0
#define PHY_STATUS_REG                              1
#define PHY_AN_ADVERTISEMENT_REG                    4
#define PHY_AN_LINKPARTNER_REG                      5
#define PHY_1000_BASET_CONTROL_REG                  9
#define PHY_1000_BASET_STATUS_REG                   10
#define PHY_RESOLVED_REG                            26

typedef enum rtksw_mode_ext_e
{
    RTKSW_MODE_EXT_DISABLE = 0,
    RTKSW_MODE_EXT_RGMII,
    RTKSW_MODE_EXT_MII_MAC,
    RTKSW_MODE_EXT_MII_PHY,
    RTKSW_MODE_EXT_TMII_MAC,
    RTKSW_MODE_EXT_TMII_PHY,
    RTKSW_MODE_EXT_GMII,
    RTKSW_MODE_EXT_RMII_MAC,
    RTKSW_MODE_EXT_RMII_PHY,
    RTKSW_MODE_EXT_SGMII,
    RTKSW_MODE_EXT_HSGMII,
    RTKSW_MODE_EXT_1000X_100FX,
    RTKSW_MODE_EXT_1000X,
    RTKSW_MODE_EXT_100FX,
    RTKSW_MODE_EXT_RGMII_2,
    RTKSW_MODE_EXT_MII_MAC_2,
    RTKSW_MODE_EXT_MII_PHY_2,
    RTKSW_MODE_EXT_TMII_MAC_2,
    RTKSW_MODE_EXT_TMII_PHY_2,
    RTKSW_MODE_EXT_RMII_MAC_2,
    RTKSW_MODE_EXT_RMII_PHY_2,
    RTKSW_MODE_EXT_FIBER_2P5G,
    RTKSW_MODE_EXT_5GR,
    RTKSW_MODE_EXT_10GR,
    RTKSW_MODE_EXT_USXGMII,
    RTKSW_MODE_EXT_END
} rtksw_mode_ext_t;

typedef enum rtksw_port_duplex_e
{
    RTKSW_PORT_HALF_DUPLEX = 0,
    RTKSW_PORT_FULL_DUPLEX,
    RTKSW_PORT_DUPLEX_END
} rtksw_port_duplex_t;

typedef enum rtksw_port_macMode_e
{
    PORT_MAC_NORMAL = 0,
    PORT_MAC_FORCE,
    PORT_MAC_END
}rtksw_port_macMode_t;

typedef enum rtksw_port_linkStatus_e
{
    RTKSW_PORT_LINKDOWN = 0,
    RTKSW_PORT_LINKUP,
    RTKSW_PORT_LINKSTATUS_END
} rtksw_port_linkStatus_t;

typedef struct  rtksw_port_mac_ability_s
{
    rtksw_uint32 forcemode;
    rtksw_uint32 speed;
    rtksw_uint32 duplex;
    rtksw_uint32 link;
    rtksw_uint32 nway;
    rtksw_uint32 txpause;
    rtksw_uint32 rxpause;
}rtksw_port_mac_ability_t;

typedef struct rtksw_port_phy_ability_s
{
    rtksw_uint32    AutoNegotiation;  /*PHY register 0.12 setting for auto-negotiation process*/
    rtksw_uint32    Half_10;          /*PHY register 4.5 setting for 10BASE-TX half duplex capable*/
    rtksw_uint32    Full_10;          /*PHY register 4.6 setting for 10BASE-TX full duplex capable*/
    rtksw_uint32    Half_100;         /*PHY register 4.7 setting for 100BASE-TX half duplex capable*/
    rtksw_uint32    Full_100;         /*PHY register 4.8 setting for 100BASE-TX full duplex capable*/
    rtksw_uint32    Full_1000;        /*PHY register 9.9 setting for 1000BASE-T full duplex capable*/
    rtksw_uint32    Full_2P5G;        /*PHY register 2.5G setting for 2500BASE-T full duplex capable*/
    rtksw_uint32    FC;               /*PHY register 4.10 setting for flow control capability*/
    rtksw_uint32    AsyFC;            /*PHY register 4.11 setting for asymmetric flow control capability*/
} rtksw_port_phy_ability_t;

typedef struct rtksw_port_fiber_ability_s
{
    rtksw_uint32    AutoNegotiation;
    rtksw_uint32    Full_100;
    rtksw_uint32    Full_1000;
    rtksw_uint32    Full_2P5G;
    rtksw_uint32    Full_5G;
    rtksw_uint32    Full_10G;
    rtksw_uint32    FC;
    rtksw_uint32    AsyFC;
} rtksw_port_fiber_ability_t;

typedef rtksw_uint32  rtksw_port_phy_data_t;     /* phy data  */
typedef rtksw_uint32  rtksw_port_phy_page_t;     /* phy page  */

typedef enum rtksw_port_phy_reg_e
{
    RTKSW_PHY_REG_CONTROL             = 0,
    RTKSW_PHY_REG_STATUS,
    RTKSW_PHY_REG_IDENTIFIER_1,
    RTKSW_PHY_REG_IDENTIFIER_2,
    RTKSW_PHY_REG_AN_ADVERTISEMENT,
    RTKSW_PHY_REG_AN_LINKPARTNER,
    RTKSW_PHY_REG_1000_BASET_CONTROL  = 9,
    RTKSW_PHY_REG_1000_BASET_STATUS,
    RTKSW_PHY_REG_END                 = 32
} rtksw_port_phy_reg_t;

typedef enum rtksw_port_speed_e
{
    RTKSW_PORT_SPEED_10M = 0,
    RTKSW_PORT_SPEED_100M,
    RTKSW_PORT_SPEED_1000M,
    RTKSW_PORT_SPEED_500M,
    RTKSW_PORT_SPEED_2500M,
    RTKSW_PORT_SPEED_5G,
    RTKSW_PORT_SPEED_10G,
    RTKSW_PORT_SPEED_END
} rtksw_port_speed_t;

typedef enum rtksw_port_media_e
{
    RTKSW_PORT_MEDIA_COPPER = 0,
    RTKSW_PORT_MEDIA_FIBER,
    RTKSW_PORT_MEDIA_AUTO,
    RTKSW_PORT_MEDIA_RGMII,
    RTKSW_PORT_MEDIA_END
}rtksw_port_media_t;

typedef enum rtksw_port_phy_type_e
{
    PHY_TYPE_10G_FIBER = 0,
    PHY_TYPE_10G_2500M_PHY,
    PHY_TYPE_FEPHY,
    PHY_TYPE_GPHY,
    PHY_TYPE_END
} rtksw_port_phy_type_t;

typedef struct rtksw_port_extPhyAutoPollingCfg_e
{
    rtksw_enable_t state;
    rtksw_port_phy_type_t extPhyType;
    rtksw_uint32 extPhyID;
} rtksw_port_extPhyAutoPollingCfg_t;

typedef struct rtksw_rtctResult_s
{
    rtksw_port_speed_t    linkType;
    union
    {
        struct rtksw_fe_result_s
        {
            rtksw_uint32      isRxShort;
            rtksw_uint32      isTxShort;
            rtksw_uint32      isRxOpen;
            rtksw_uint32      isTxOpen;
            rtksw_uint32      isRxMismatch;
            rtksw_uint32      isTxMismatch;
            rtksw_uint32      isRxLinedriver;
            rtksw_uint32      isTxLinedriver;
            rtksw_uint32      rxLen;
            rtksw_uint32      txLen;
        } fe_result;

        struct rtksw_ge_result_s
        {
            rtksw_uint32      channelAShort;
            rtksw_uint32      channelBShort;
            rtksw_uint32      channelCShort;
            rtksw_uint32      channelDShort;

            rtksw_uint32      channelAOpen;
            rtksw_uint32      channelBOpen;
            rtksw_uint32      channelCOpen;
            rtksw_uint32      channelDOpen;

            rtksw_uint32      channelAMismatch;
            rtksw_uint32      channelBMismatch;
            rtksw_uint32      channelCMismatch;
            rtksw_uint32      channelDMismatch;

            rtksw_uint32      channelALinedriver;
            rtksw_uint32      channelBLinedriver;
            rtksw_uint32      channelCLinedriver;
            rtksw_uint32      channelDLinedriver;

            rtksw_uint32      channelALen;
            rtksw_uint32      channelBLen;
            rtksw_uint32      channelCLen;
            rtksw_uint32      channelDLen;
        } ge_result;
    }result;
} rtksw_rtctResult_t;

typedef enum rtksw_port_autoDosType_e
{
    AUTODOS_DAEQSA = 0,
    AUTODOS_LANDATTACKS,
    AUTODOS_BLATATTACKS,
    AUTODOS_SYNFINSCAN,
    AUTODOS_XMASCAN,
    AUTODOS_NULLSCAN,
    AUTODOS_SYN1024,
    AUTODOS_TCPSHORTHDR,
    AUTODOS_TCPFRAGERROR,
    AUTODOS_ICMPFRAGMENT,
    AUTODOS_END,

} rtksw_port_autoDosType_t;

typedef enum rtksw_port_phy_mdix_mode_e
{
    PHY_AUTO_CROSSOVER_MODE= 0,
    PHY_FORCE_MDI_MODE,
    PHY_FORCE_MDIX_MODE,
    PHY_FORCE_MODE_END
} rtksw_port_phy_mdix_mode_t;

typedef enum rtksw_port_phy_mdix_status_e
{
    PHY_STATUS_AUTO_MDI_MODE= 0,
    PHY_STATUS_AUTO_MDIX_MODE,
    PHY_STATUS_FORCE_MDI_MODE,
    PHY_STATUS_FORCE_MDIX_MODE,
    PHY_STATUS_FORCE_MODE_END
} rtksw_port_phy_mdix_status_t;

typedef enum rtksw_port_phy_test_mode_e
{
    RTKSW_PHY_TEST_MODE_NORMAL= 0,
    RTKSW_PHY_TEST_MODE_1,
    RTKSW_PHY_TEST_MODE_2,
    RTKSW_PHY_TEST_MODE_3,
    RTKSW_PHY_TEST_MODE_4,
    RTKSW_PHY_2P5G_TEST_MODE_1,
    RTKSW_PHY_2P5G_TEST_MODE_2,
    RTKSW_PHY_2P5G_TEST_MODE_3,
    RTKSW_PHY_2P5G_TEST_MODE_4_TONE_1,
    RTKSW_PHY_2P5G_TEST_MODE_4_TONE_2,
    RTKSW_PHY_2P5G_TEST_MODE_4_TONE_3,
    RTKSW_PHY_2P5G_TEST_MODE_4_TONE_4,
    RTKSW_PHY_2P5G_TEST_MODE_4_TONE_5,
    RTKSW_PHY_2P5G_TEST_MODE_5,
    RTKSW_PHY_2P5G_TEST_MODE_6,
    RTKSW_PHY_2P5G_TEST_MODE_7,
    RTKSW_PHY_TEST_MODE_END
} rtksw_port_phy_test_mode_t;

typedef enum rtksw_port_sdsPolarity_e
{
    SDS_POLARITY_NORMAL = 0,
    SDS_POLARITY_REVERSE,
    SDS_POLARITY_END
} rtksw_port_sdsPolarity_t;

/* Function Name:
 *      rtksw_port_phyAutoNegoAbility_set
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
extern rtksw_api_ret_t rtksw_port_phyAutoNegoAbility_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_phy_ability_t *pAbility);

/* Function Name:
 *      rtksw_port_phyAutoNegoAbility_get
 * Description:
 *      Get PHY ability through PHY registers.
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
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
extern rtksw_api_ret_t rtksw_port_phyAutoNegoAbility_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_phy_ability_t *pAbility);

/* Function Name:
 *      rtksw_port_phyForceModeAbility_set
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
 *      This API can be used to configure combo port in fiber mode.
 *      The possible parameters in fiber mode are Full_1000 and Full 100.
 *      All the other fields in rtksw_port_phy_ability_t will be ignored in fiber port.
 */
extern rtksw_api_ret_t rtksw_port_phyForceModeAbility_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_phy_ability_t *pAbility);

/* Function Name:
 *      rtksw_port_phyForceModeAbility_get
 * Description:
 *      Get PHY ability through PHY registers.
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
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
extern rtksw_api_ret_t rtksw_port_phyForceModeAbility_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_phy_ability_t *pAbility);

/* Function Name:
 *      rtksw_port_phyStatus_get
 * Description:
 *      Get ethernet PHY linking status
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
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
extern rtksw_api_ret_t rtksw_port_phyStatus_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_linkStatus_t *pLinkStatus, rtksw_port_speed_t *pSpeed, rtksw_port_duplex_t *pDuplex);

/* Function Name:
 *      rtksw_port_macForceLink_set
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
extern rtksw_api_ret_t rtksw_port_macForceLink_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_mac_ability_t *pPortability);

/* Function Name:
 *      rtksw_port_macForceLink_get
 * Description:
 *      Get port force linking configuration.
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
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
extern rtksw_api_ret_t rtksw_port_macForceLink_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_mac_ability_t *pPortability);

/* Function Name:
 *      rtksw_port_macForceLinkExt_set
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
 */
extern rtksw_api_ret_t rtksw_port_macForceLinkExt_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_mode_ext_t mode, rtksw_port_mac_ability_t *pPortability);

/* Function Name:
 *      rtksw_port_macForceLinkExt_get
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
extern rtksw_api_ret_t rtksw_port_macForceLinkExt_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_mode_ext_t *pMode, rtksw_port_mac_ability_t *pPortability);

/* Function Name:
 *      rtksw_port_macStatus_get
 * Description:
 *      Get port link status.
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
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
extern rtksw_api_ret_t rtksw_port_macStatus_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_mac_ability_t *pPortstatus);

/* Function Name:
 *      rtksw_port_macLocalLoopbackEnable_set
 * Description:
 *      Set Port Local Loopback. (Redirect TX to RX.)
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
 *      enable      - Loopback state, 0:disable, 1:enable
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
extern rtksw_api_ret_t rtksw_port_macLocalLoopbackEnable_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t enable);

/* Function Name:
 *      rtksw_port_macLocalLoopbackEnable_get
 * Description:
 *      Get Port Local Loopback. (Redirect TX to RX.)
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
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
extern rtksw_api_ret_t rtksw_port_macLocalLoopbackEnable_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t *pEnable);

/* Function Name:
 *      rtksw_port_phyReg_set
 * Description:
 *      Set PHY register data of the specific port.
 * Input:
 *      unit    - Unit ID
 *      port    - port id.
 *      reg     - Register id
 *      regData - Register data
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
extern rtksw_api_ret_t rtksw_port_phyReg_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_phy_reg_t reg, rtksw_port_phy_data_t regData);

/* Function Name:
 *      rtksw_port_phyReg_get
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
extern rtksw_api_ret_t rtksw_port_phyReg_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_phy_reg_t reg, rtksw_port_phy_data_t *pData);

/* Function Name:
 *      rtksw_port_phyOCPReg_set
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
extern rtksw_api_ret_t rtksw_port_phyOCPReg_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_uint32 ocpAddr, rtksw_uint32 ocpData);

/* Function Name:
 *      rtksw_port_phyOCPReg_get
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
extern rtksw_api_ret_t rtksw_port_phyOCPReg_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_uint32 ocpAddr, rtksw_uint32 *pRegData);

/* Function Name:
 *      rtksw_port_backpressureEnable_set
 * Description:
 *      Set the half duplex backpressure enable status of the specific port.
 * Input:
 *      unit        - Unit ID
 *      port        - port id.
 *      enable      - Back pressure status.
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
 *      - DISABLE(Defer)
 *      - ENABLE (Backpressure)
 */
extern rtksw_api_ret_t rtksw_port_backpressureEnable_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t enable);

/* Function Name:
 *      rtksw_port_backpressureEnable_get
 * Description:
 *      Get the half duplex backpressure enable status of the specific port.
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
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
 *      - DISABLE(Defer)
 *      - ENABLE (Backpressure)
 */
extern rtksw_api_ret_t rtksw_port_backpressureEnable_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t *pEnable);

/* Function Name:
 *      rtksw_port_adminEnable_set
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
extern rtksw_api_ret_t rtksw_port_adminEnable_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t enable);

/* Function Name:
 *      rtksw_port_adminEnable_get
 * Description:
 *      Get port admin configurationof the specific port.
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
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
extern rtksw_api_ret_t rtksw_port_adminEnable_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t *pEnable);

/* Function Name:
 *      rtksw_port_isolation_set
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
extern rtksw_api_ret_t rtksw_port_isolation_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_portmask_t *pPortmask);

/* Function Name:
 *      rtksw_port_isolation_get
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
extern rtksw_api_ret_t rtksw_port_isolation_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_portmask_t *pPortmask);

/* Function Name:
 *      rtksw_port_rgmiiDelayExt_set
 * Description:
 *      Set RGMII interface delay value for TX and RX.
 * Input:
 *      unit        - Unit ID
 *      txDelay     - TX delay value, 1 for delay 2ns and 0 for no-delay
 *      rxDelay     - RX delay value, 0~7 for delay setup.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API can set external interface 2 RGMII delay.
 *      In TX delay, there are 2 selection: no-delay and 2ns delay.
 *      In RX dekay, there are 8 steps for delay tunning. 0 for no-delay, and 7 for maximum delay.
 *      Note. This API should be called before rtksw_port_macForceLinkExt_set().
 */
extern rtksw_api_ret_t rtksw_port_rgmiiDelayExt_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_data_t txDelay, rtksw_data_t rxDelay);

/* Function Name:
 *      rtksw_port_rgmiiDelayExt_get
 * Description:
 *      Get RGMII interface delay value for TX and RX.
 * Input:
 *      unit        - Unit ID
 * Output:
 *      pTxDelay    - TX delay value
 *      pRxDelay    - RX delay value
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API can set external interface 2 RGMII delay.
 *      In TX delay, there are 2 selection: no-delay and 2ns delay.
 *      In RX dekay, there are 8 steps for delay tunning. 0 for n0-delay, and 7 for maximum delay.
 */
extern rtksw_api_ret_t rtksw_port_rgmiiDelayExt_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_data_t *pTxDelay, rtksw_data_t *pRxDelay);

/* Function Name:
 *      rtksw_port_phyEnableAll_set
 * Description:
 *      Set all PHY enable status.
 * Input:
 *      unit        - Unit ID
 *      enable      - PHY Enable State.
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
extern rtksw_api_ret_t rtksw_port_phyEnableAll_set(rtksw_uint32 unit, rtksw_enable_t enable);

/* Function Name:
 *      rtksw_port_phyEnableAll_get
 * Description:
 *      Get all PHY enable status.
 * Input:
 *      unit        - Unit ID
 * Output:
 *      pEnable     - PHY Enable State.
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
extern rtksw_api_ret_t rtksw_port_phyEnableAll_get(rtksw_uint32 unit, rtksw_enable_t *pEnable);

/* Function Name:
 *      rtksw_port_efid_set
 * Description:
 *      Set port-based enhanced filtering database
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 *      efid    - Specified enhanced filtering database.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_L2_FID - Invalid fid.
 *      RT_ERR_INPUT - Invalid input parameter.
 *      RT_ERR_PORT_ID - Invalid port ID.
 * Note:
 *      The API can set port-based enhanced filtering database.
 */
extern rtksw_api_ret_t rtksw_port_efid_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_data_t efid);

/* Function Name:
 *      rtksw_port_efid_get
 * Description:
 *      Get port-based enhanced filtering database
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
 * Output:
 *      pEfid       - Specified enhanced filtering database.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_INPUT - Invalid input parameters.
 *      RT_ERR_PORT_ID - Invalid port ID.
 * Note:
 *      The API can get port-based enhanced filtering database status.
 */
extern rtksw_api_ret_t rtksw_port_efid_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_data_t *pEfid);

/* Function Name:
 *      rtksw_port_phyComboPortMedia_set
 * Description:
 *      Set Combo port media type
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
 *      media       - Media (COPPER or FIBER or AUTO)
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
extern rtksw_api_ret_t rtksw_port_phyComboPortMedia_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_media_t media);

/* Function Name:
 *      rtksw_port_phyComboPortMedia_get
 * Description:
 *      Get Combo port media type
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
 * Output:
 *      pMedia      - Media (COPPER or FIBER or AUTO)
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_INPUT            - Invalid input parameters.
 *      RT_ERR_PORT_ID          - Invalid port ID.
 * Note:
 *      The API can Set Combo port media type.
 */
extern rtksw_api_ret_t rtksw_port_phyComboPortMedia_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_media_t *pMedia);

/* Function Name:
 *      rtksw_port_rtctEnable_set
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
extern rtksw_api_ret_t rtksw_port_rtctEnable_set(rtksw_uint32 unit, rtksw_portmask_t *pPortmask);

/* Function Name:
 *      rtksw_port_rtctDisable_set
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
extern rtksw_api_ret_t rtksw_port_rtctDisable_set(rtksw_uint32 unit, rtksw_portmask_t *pPortmask);


/* Function Name:
 *      rtksw_port_rtctResult_get
 * Description:
 *      Get the result of RTCT test
 * Input:
 *      unit        - Unit ID
 *      port        - Port ID
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
extern rtksw_api_ret_t rtksw_port_rtctResult_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_rtctResult_t *pRtctResult);

/* Function Name:
 *      rtksw_port_sds_reset
 * Description:
 *      Reset Serdes
 * Input:
 *      unit        - Unit ID
 *      port        - Port ID
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_PORT_ID              - Invalid port ID.
 * Note:
 *      The API can reset Serdes
 */
extern rtksw_api_ret_t rtksw_port_sds_reset(rtksw_uint32 unit, rtksw_port_t port);

/* Function Name:
 *      rtksw_port_sgmiiLinkStatus_get
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
extern rtksw_api_ret_t rtksw_port_sgmiiLinkStatus_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_data_t *pSignalDetect, rtksw_data_t *pSync, rtksw_port_linkStatus_t *pLink);

/* Function Name:
 *      rtksw_port_sgmiiNway_set
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
extern rtksw_api_ret_t rtksw_port_sgmiiNway_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t state);

/* Function Name:
 *      rtksw_port_sgmiiNway_get
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
extern rtksw_api_ret_t rtksw_port_sgmiiNway_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t *pState);

/* Function Name:
 *      rtksw_port_fiberAbilityExt_set
 * Description:
 *      Get SGMII/HSGMII port Nway state
 * Input:
 *      unit        - Unit ID
 *      port        - Port ID
 *      pause       - pause state
 *      asypause    - asypause state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_PORT_ID              - Invalid port ID.
 * Note:
 *      The API can get SGMII/HSGMII port Nway state
 */
extern rtksw_api_ret_t rtksw_port_fiberAbilityExt_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_uint32 pause, rtksw_uint32 asypause);


/* Function Name:
 *      rtksw_port_fiberAbilityExt_get
 * Description:
 *      Get SGMII/HSGMII port Nway state
 * Input:
 *      unit        - Unit ID
 *      port        - Port ID
 * Output:
 *      pPause      - pause state
 *      pAsypause   - asypause state
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_PORT_ID              - Invalid port ID.
 * Note:
 *      The API can get SGMII/HSGMII port Nway state
 */
extern rtksw_api_ret_t rtksw_port_fiberAbilityExt_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_uint32* pPause, rtksw_uint32* pAsypause);


/* Function Name:
 *      rtksw_port_autoDos_set
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
extern rtksw_api_ret_t rtksw_port_autoDos_set(rtksw_uint32 unit, rtksw_port_autoDosType_t type, rtksw_enable_t state);

/* Function Name:
 *      rtksw_port_autoDos_get
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
extern rtksw_api_ret_t rtksw_port_autoDos_get(rtksw_uint32 unit, rtksw_port_autoDosType_t type, rtksw_enable_t *pState);

/* Function Name:
 *      rtksw_port_fiberAbility_set
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
extern rtksw_api_ret_t rtksw_port_fiberAbility_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_fiber_ability_t *pAbility);


/* Function Name:
 *      rtksw_port_fiberAbility_get
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
extern rtksw_api_ret_t rtksw_port_fiberAbility_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_fiber_ability_t *pAbility);

/* Function Name:
 *      rtksw_port_phyMdx_set
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
extern rtksw_api_ret_t rtksw_port_phyMdx_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_phy_mdix_mode_t mode);

/* Function Name:
 *      rtksw_port_phyMdx_get
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
extern rtksw_api_ret_t rtksw_port_phyMdx_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_phy_mdix_mode_t *pMode);

/* Function Name:
 *      rtksw_port_phyMdxStatus_get
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
extern rtksw_api_ret_t rtksw_port_phyMdxStatus_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_phy_mdix_status_t *pStatus);

/* Function Name:
 *      rtksw_port_phyTestMode_set
 * Description:
 *      Set PHY in test mode.
 * Input:
 *      unit        - Unit ID
 *      port        - port id.
 *      mode        - PHY test mode 0:normal 1:test mode 1 2:test mode 2 3: test mode 3 4:test mode 4 5~7:reserved
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
 *      Set PHY in test mode and only one PHY can be in test mode at the same time.
 *      It means API will return FAILED if other PHY is in test mode.
 *      This API only provide test mode 1 & 4 setup.
 */
extern rtksw_api_ret_t rtksw_port_phyTestMode_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_phy_test_mode_t mode);

/* Function Name:
 *      rtksw_port_phyTestMode_get
 * Description:
 *      Get PHY in which test mode.
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
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
 *      Get test mode of PHY from register setting 9.15 to 9.13.
 */
extern rtksw_api_ret_t rtksw_port_phyTestMode_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_phy_test_mode_t *pMode);

/* Function Name:
 *      rtksw_port_maxPacketLength_set
 * Description:
 *      Set Max packet length per port
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
 *      length      - Max packet length.
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
extern rtksw_api_ret_t rtksw_port_maxPacketLength_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_uint32 length);

/* Function Name:
 *      rtksw_port_maxPacketLength_get
 * Description:
 *      Get Max packet length per port
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
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
extern rtksw_api_ret_t rtksw_port_maxPacketLength_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_uint32 *pLength);

/* Function Name:
 *      rtksw_port_phyGreenEthernet_set
 * Description:
 *      Set Ports Green Ethernet state.
 * Input:
 *      unit    - Unit ID
 *      port    - port ID
 *      state   - Green Ethernet state.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - OK
 *      RT_ERR_FAILED   - Failed
 *      RT_ERR_SMI      - SMI access error
 *      RT_ERR_ENABLE   - Invalid enable input.
 * Note:
 *      This API can set Port Green Ethernet state.
 *      The configuration is as following:
 *      - DISABLE
 *      - ENABLE
 */
extern rtksw_api_ret_t rtksw_port_phyGreenEthernet_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t state);

/* Function Name:
 *      rtksw_port_phyGreenEthernet_get
 * Description:
 *      Get Ports Green Ethernet state.
 * Input:
 *      unit    - Unit ID
 *      port    - port ID
 * Output:
 *      pState  - Green Ethernet state.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 * Note:
 *      This API can get Green Ethernet state.
 */
extern rtksw_api_ret_t rtksw_port_phyGreenEthernet_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t *pState);

/* Function Name:
 *      rtksw_port_phyLinkDownPowerSaving_set
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
extern rtksw_api_ret_t rtksw_port_phyLinkDownPowerSaving_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t state);

/* Function Name:
 *      rtksw_port_phyLinkDownPowerSaving_get
 * Description:
 *      Get Ports Link Down Power Saving state.
 * Input:
 *      unit        - Unit ID
 *      port        - port ID
 * Output:
 *      pState      - Link Down Power Saving state.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 * Note:
 *      This API can get Link Down Power Saving state.
 */
extern rtksw_api_ret_t rtksw_port_phyLinkDownPowerSaving_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t *pState);

/* Function Name:
 *      rtksw_port_serdesReg_set
 * Description:
 *      Set Serdes register data of the specific port.
 * Input:
 *      unit    - Unit ID
 *      port    - port id.
 *      page    - Page id.
 *      reg     - Register id
 *      regData - Register data
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
extern rtksw_api_ret_t rtksw_port_serdesReg_set(rtksw_uint32 unit, rtksw_uint32 sdsID, rtksw_uint32 page, rtksw_uint32 reg, rtksw_uint32 regData);

/* Function Name:
 *      rtksw_port_serdesReg_get
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
extern rtksw_api_ret_t rtksw_port_serdesReg_get(rtksw_uint32 unit, rtksw_uint32 sdsID, rtksw_uint32 page, rtksw_uint32 reg, rtksw_uint32 *pData);

/* Function Name:
 *      rtksw_port_serdesPolarity_set
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
extern rtksw_api_ret_t rtksw_port_serdesPolarity_set(rtksw_uint32 unit, rtksw_uint32 sdsID, rtksw_port_sdsPolarity_t inputPolarity, rtksw_port_sdsPolarity_t outputPolarity);

/* Function Name:
 *      rtksw_port_serdesPolarity_get
 * Description:
 *      Get Serdes polarity.
 * Input:
 *      unit            - Unit ID
 *      sdsID           - Serdes id.
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
extern rtksw_api_ret_t rtksw_port_serdesPolarity_get(rtksw_uint32 unit, rtksw_uint32 sdsID, rtksw_port_sdsPolarity_t *pInputPolarity, rtksw_port_sdsPolarity_t *pOutputPolarity);

/* Function Name:
 *      rtksw_port_extPhyAutoPollingCfg_set
 * Description:
 *      Set External PHY auto polling function.
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
 *      pCfg        - Auto polling configuration
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_INPUT            - Error input
 *      RT_ERR_NULL_POINTER     - Null pointer
 * Note:
 *      This API can set External PHY auto polling function.
 */
extern rtksw_api_ret_t rtksw_port_extPhyAutoPollingCfg_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_extPhyAutoPollingCfg_t *pCfg);

/* Function Name:
 *      rtksw_port_extPhyAutoPollingCfg_get
 * Description:
 *      Get External PHY auto polling function.
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
 * Output:
 *      pCfg        - Auto polling configuration
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_INPUT            - Error input
 *      RT_ERR_NULL_POINTER     - Null pointer
 * Note:
 *      This API can set External PHY auto polling function.
 */
extern rtksw_api_ret_t rtksw_port_extPhyAutoPollingCfg_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_extPhyAutoPollingCfg_t *pCfg);

/* Function Name:
 *      rtksw_port_extPhySmiState_set
 * Description:
 *      Enable/Disable SMI master for accessing external MDC/MDIO slave device.
 * Input:
 *      unit        - Unit ID
 *      state       - SMI master state.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_ENABLE               - Error input
 *      RT_ERR_CHIP_NOT_SUPPORTED   - Function not supported by this chip model 
 * Note:
 *      This API can set the state of SMI master.
 */
extern rtksw_api_ret_t rtksw_port_extPhySmiState_set(rtksw_uint32 unit, rtksw_enable_t state);

/* Function Name:
 *      rtksw_port_extPhySmiState_get
 * Description:
 *      Get state of SMI master for accessing external MDC/MDIO slave device.
 * Input:
 *      unit        - Unit ID
 * Output:
 *      pState      - SMI master state.
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_NULL_POINTER         - Null pointer
 *      RT_ERR_CHIP_NOT_SUPPORTED   - Function not supported by this chip model
 * Note:
 *      This API can set the state of SMI master.
 */
extern rtksw_api_ret_t rtksw_port_extPhySmiState_get(rtksw_uint32 unit, rtksw_enable_t *pState);

/* Function Name:
 *      rtksw_port_extPhyReg_set
 * Description:
 *      Set register of external MDC/MDIO slave device.
 * Input:
 *      unit        - Unit ID
 *      phyID       - External device ID
 *      page        - Page. For accessing stardard register 0-15, keep this parameter as 0.
 *      reg         - Register of external device
 *      data        - The data value which is written into register
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_CHIP_NOT_SUPPORTED   - Function not supported by this chip model
 * Note:
 *      This API can set register of external MDC/MDIO slave device.
 */
extern rtksw_api_ret_t rtksw_port_extPhyReg_set(rtksw_uint32 unit, rtksw_uint32 phyID, rtksw_port_phy_page_t page, rtksw_port_phy_reg_t reg, rtksw_port_phy_data_t data);

/* Function Name:
 *      rtksw_port_extPhyReg_get
 * Description:
 *      Get register of external MDC/MDIO slave device.
 * Input:
 *      unit        - Unit ID
 *      phyID       - External device ID
 *      page        - Page. For accessing stardard register 0-15, keep this parameter as 0.
 *      reg         - Register of external device
 * Output:
 *      pData       - The data value which is read from register
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_NULL_POINTER         - Null pointer
 *      RT_ERR_CHIP_NOT_SUPPORTED   - Function not supported by this chip model
 * Note:
 *      This API can set register of external MDC/MDIO slave device.
 */
extern rtksw_api_ret_t rtksw_port_extPhyReg_get(rtksw_uint32 unit, rtksw_uint32 phyID, rtksw_port_phy_page_t page, rtksw_port_phy_reg_t reg, rtksw_port_phy_data_t *pData);

/* Function Name:
 *      rtksw_port_extC45PhyReg_set
 * Description:
 *      Set register of external C45 MDC/MDIO slave device.
 * Input:
 *      unit        - Unit ID
 *      phyID       - External device ID
 *      mmd         - MMD.
 *      reg         - Register of external device
 *      data        - The data value which is written into register
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_CHIP_NOT_SUPPORTED   - Function not supported by this chip model
 * Note:
 *      This API can set register of external C45 MDC/MDIO slave device.
 */
extern rtksw_api_ret_t rtksw_port_extC45PhyReg_set(rtksw_uint32 unit, rtksw_uint32 phyID, rtksw_uint32 mmd, rtksw_port_phy_reg_t reg, rtksw_port_phy_data_t data);

/* Function Name:
 *      rtksw_port_extC45PhyReg_get
 * Description:
 *      Get register of external C45 MDC/MDIO slave device.
 * Input:
 *      unit        - Unit ID
 *      phyID       - External device ID
 *      mmd         - MMD.
 *      reg         - Register of external device
 * Output:
 *      pData       - The data value which is read from register
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_NULL_POINTER         - Null pointer
 *      RT_ERR_CHIP_NOT_SUPPORTED   - Function not supported by this chip model
 * Note:
 *      This API can set register of external C45 MDC/MDIO slave device.
 */
extern rtksw_api_ret_t rtksw_port_extC45PhyReg_get(rtksw_uint32 unit, rtksw_uint32 phyID, rtksw_uint32 mmd, rtksw_port_phy_reg_t reg, rtksw_port_phy_data_t *pData);

#if defined(CONFIG_COMPATIBLE_V1_4)

#define rtk_port_phyAutoNegoAbility_set(port, pAbility)                         rtksw_port_phyAutoNegoAbility_set(0, port, pAbility)
#define rtk_port_phyAutoNegoAbility_get(port, pAbility)                         rtksw_port_phyAutoNegoAbility_get(0, port, pAbility)
#define rtk_port_phyForceModeAbility_set(port, pAbility)                        rtksw_port_phyForceModeAbility_set(0, port, pAbility)
#define rtk_port_phyForceModeAbility_get(port, pAbility)                        rtksw_port_phyForceModeAbility_get(0, port, pAbility)
#define rtk_port_phyStatus_get(port, pLinkStatus, pSpeed, pDuplex)              rtksw_port_phyStatus_get(0, port, pLinkStatus, pSpeed, pDuplex)
#define rtk_port_macForceLink_set(port, pPortability)                           rtksw_port_macForceLink_set(0, port, pPortability)
#define rtk_port_macForceLink_get(port, pPortability)                           rtksw_port_macForceLink_get(0, port, pPortability)
#define rtk_port_macForceLinkExt_set(port, mode, pPortability)                  rtksw_port_macForceLinkExt_set(0, port, mode, pPortability)
#define rtk_port_macForceLinkExt_get(port, pMode, pPortability)                 rtksw_port_macForceLinkExt_get(0, port, pMode, pPortability)
#define rtk_port_macStatus_get(port, pPortstatus)                               rtksw_port_macStatus_get(0, port, pPortstatus)
#define rtk_port_macLocalLoopbackEnable_set(port, enable)                       rtksw_port_macLocalLoopbackEnable_set(0, port, enable)
#define rtk_port_macLocalLoopbackEnable_get(port, pEnable)                      rtksw_port_macLocalLoopbackEnable_get(0, port, pEnable)
#define rtk_port_phyReg_set(port, reg, regData)                                 rtksw_port_phyReg_set(0, port, reg, regData)
#define rtk_port_phyReg_get(port, reg, pData)                                   rtksw_port_phyReg_get(0, port, reg, pData)
#define rtk_port_phyOCPReg_set(port, ocpAddr, ocpData)                          rtksw_port_phyOCPReg_set(0, port, ocpAddr, ocpData)
#define rtk_port_phyOCPReg_get(port, ocpAddr, pRegData)                         rtksw_port_phyOCPReg_get(0, port, ocpAddr, pRegData)
#define rtk_port_set_disable(port, ocpData)                                     rtksw_port_phyOCPReg_set(0, port, 0xa610, ocpData)
#define rtk_port_get_disable(port, pRegData)                                    rtksw_port_phyOCPReg_get(0, port, 0xa610, pRegData)
#define rtk_port_backpressureEnable_set(port, enable)                           rtksw_port_backpressureEnable_set(0, port, enable)
#define rtk_port_backpressureEnable_get(port, pEnable)                          rtksw_port_backpressureEnable_get(0, port, pEnable)
#define rtk_port_adminEnable_set(port, enable)                                  rtksw_port_adminEnable_set(0, port, enable)
#define rtk_port_adminEnable_get(port, pEnable)                                 rtksw_port_adminEnable_get(0, port, pEnable)
#define rtk_port_isolation_set(port, pPortmask)                                 rtksw_port_isolation_set(0, port, pPortmask)
#define rtk_port_isolation_get(port, pPortmask)                                 rtksw_port_isolation_get(0, port, pPortmask)
#define rtk_port_rgmiiDelayExt_set(port, txDelay, rxDelay)                      rtksw_port_rgmiiDelayExt_set(0, port, txDelay, rxDelay)
#define rtk_port_rgmiiDelayExt_get(port, pTxDelay, pRxDelay)                    rtksw_port_rgmiiDelayExt_get(0, port, pTxDelay, pRxDelay)
#define rtk_port_phyEnableAll_set(enable)                                       rtksw_port_phyEnableAll_set(0, enable)
#define rtk_port_phyEnableAll_get(pEnable)                                      rtksw_port_phyEnableAll_get(0, pEnable)
#define rtk_port_efid_set(port, efid)                                           rtksw_port_efid_set(0, port, efid)
#define rtk_port_efid_get(port, pEfid)                                          rtksw_port_efid_get(0, port, pEfid)
#define rtk_port_phyComboPortMedia_set(port, media)                             rtksw_port_phyComboPortMedia_set(0, port, media)
#define rtk_port_phyComboPortMedia_get(port, pMedia)                            rtksw_port_phyComboPortMedia_get(0, port, pMedia)
#define rtk_port_rtctEnable_set(pPortmask)                                      rtksw_port_rtctEnable_set(0, pPortmask)
#define rtk_port_rtctDisable_set(pPortmask)                                     rtksw_port_rtctDisable_set(0, pPortmask)
#define rtk_port_rtctResult_get(port, pRtctResult)                              rtksw_port_rtctResult_get(0, port, pRtctResult)
#define rtk_port_sds_reset(port)                                                rtksw_port_sds_reset(0, port)
#define rtk_port_sgmiiLinkStatus_get(port, pSignalDetect, pSync, pLink)         rtksw_port_sgmiiLinkStatus_get(0, port, pSignalDetect, pSync, pLink)
#define rtk_port_sgmiiNway_set(port, state)                                     rtksw_port_sgmiiNway_set(0, port, state)
#define rtk_port_sgmiiNway_get(port, pState)                                    rtksw_port_sgmiiNway_get(0, port, pState)
#define rtk_port_fiberAbilityExt_set(port, pause, asypause)                     rtksw_port_fiberAbilityExt_set(0, port, pause, asypause)
#define rtk_port_fiberAbilityExt_get(port, pPause, pAsypause)                   rtksw_port_fiberAbilityExt_get(0, port, pPause, pAsypause)
#define rtk_port_autoDos_set(type, state)                                       rtksw_port_autoDos_set(0, type, state)
#define rtk_port_autoDos_get(type, pState)                                      rtksw_port_autoDos_get(0, type, pState)
#define rtk_port_fiberAbility_set(port, pAbility)                               rtksw_port_fiberAbility_set(0, port, pAbility)
#define rtk_port_fiberAbility_get(port, pAbility)                               rtksw_port_fiberAbility_get(0, port, pAbility)
#define rtk_port_phyMdx_set(port, mode)                                         rtksw_port_phyMdx_set(0, port, mode)
#define rtk_port_phyMdx_get(port, pMode)                                        rtksw_port_phyMdx_get(0, port, pMode)
#define rtk_port_phyMdxStatus_get(port, pStatus)                                rtksw_port_phyMdxStatus_get(0, port, pStatus)
#define rtk_port_phyTestMode_set(port, mode)                                    rtksw_port_phyTestMode_set(0, port, mode)
#define rtk_port_phyTestMode_get(port, pMode)                                   rtksw_port_phyTestMode_get(0, port, pMode)
#define rtk_port_maxPacketLength_set(port, length)                              rtksw_port_maxPacketLength_set(0, port, length)
#define rtk_port_maxPacketLength_get(port, pLength)                             rtksw_port_maxPacketLength_get(0, port, pLength)
#define rtk_port_phyGreenEthernet_set(port, state)                              rtksw_port_phyGreenEthernet_set(0, port, state)
#define rtk_port_phyGreenEthernet_get(port, pState)                             rtksw_port_phyGreenEthernet_get(0, port, pState)
#define rtk_port_phyLinkDownPowerSaving_set(port, state)                        rtksw_port_phyLinkDownPowerSaving_set(0, port, state)
#define rtk_port_phyLinkDownPowerSaving_get( port, pState)                      rtksw_port_phyLinkDownPowerSaving_get(0, port, pState)
#define rtk_port_serdesReg_set(sdsID, page, reg, regData)                       rtksw_port_serdesReg_set(0, sdsID, page, reg, regData)
#define rtk_port_serdesReg_get(sdsID, page, reg, pData)                         rtksw_port_serdesReg_get(0, sdsID, page, reg, pData)
#define rtk_port_serdesPolarity_set(sdsID, inputPolarity, outputPolarity)       rtksw_port_serdesPolarity_set(0, sdsID, inputPolarity, outputPolarity)
#define rtk_port_serdesPolarity_get(sdsID, pInputPolarity, pOutputPolarity)     rtksw_port_serdesPolarity_get(0, sdsID, pInputPolarity, pOutputPolarity)
#define rtk_port_extPhyAutoPollingCfg_set(port, pCfg)                           rtksw_port_extPhyAutoPollingCfg_set(0, port, pCfg)
#define rtk_port_extPhyAutoPollingCfg_get(port, pCfg)                           rtksw_port_extPhyAutoPollingCfg_get(0, port, pCfg)
#define rtk_port_extPhySmiState_set(state)                                      rtksw_port_extPhySmiState_set(0, state)
#define rtk_port_extPhySmiState_get(pState)                                     rtksw_port_extPhySmiState_get(0, pState)
#define rtk_port_extPhyReg_set(phyID, page, reg, data)                          rtksw_port_extPhyReg_set(0, phyID, page, reg, data)
#define rtk_port_extPhyReg_get(phyID, page, reg, pData)                         rtksw_port_extPhyReg_get(0, phyID, page, reg, pData)
#define rtk_port_extC45PhyReg_set(phyID, mmd, reg, data)                        rtksw_port_extC45PhyReg_set(0, phyID, mmd, reg, data)
#define rtk_port_extC45PhyReg_get(phyID, mmd, reg, pData)                       rtksw_port_extC45PhyReg_get(0, phyID, mmd, reg, pData)

#define rtk_mode_ext_t                  rtksw_mode_ext_t
#define rtk_port_duplex_t               rtksw_port_duplex_t
#define rtk_port_macMode_t              rtksw_port_macMode_t
#define rtk_port_linkStatus_t           rtksw_port_linkStatus_t
#define rtk_port_mac_ability_t          rtksw_port_mac_ability_t
#define rtk_port_phy_ability_t          rtksw_port_phy_ability_t
#define rtk_port_fiber_ability_t        rtksw_port_fiber_ability_t
#define rtk_port_phy_data_t             rtksw_port_phy_data_t
#define rtk_port_phy_page_t             rtksw_port_phy_page_t
#define rtk_port_phy_reg_t              rtksw_port_phy_reg_t
#define rtk_port_speed_t                rtksw_port_speed_t
#define rtk_port_media_t                rtksw_port_media_t
#define rtk_port_phy_type_t             rtksw_port_phy_type_t
#define rtk_port_extPhyAutoPollingCfg_t rtksw_port_extPhyAutoPollingCfg_t
#define rtk_rtctResult_t                rtksw_rtctResult_t
#define rtk_port_autoDosType_t          rtksw_port_autoDosType_t
#define rtk_port_phy_mdix_mode_t        rtksw_port_phy_mdix_mode_t
#define rtk_port_phy_mdix_status_t      rtksw_port_phy_mdix_status_t
#define rtk_port_phy_test_mode_t        rtksw_port_phy_test_mode_t
#define rtk_port_sdsPolarity_t          rtksw_port_sdsPolarity_t

#define RTK_PORT_HALF_DUPLEX      RTKSW_PORT_HALF_DUPLEX 
#define RTK_PORT_FULL_DUPLEX      RTKSW_PORT_FULL_DUPLEX

#define RTK_PORT_LINKDOWN         RTKSW_PORT_LINKDOWN
#define RTK_PORT_LINKUP           RTKSW_PORT_LINKUP

#define RTK_PHY_REG_CONTROL             RTKSW_PHY_REG_CONTROL           
#define RTK_PHY_REG_STATUS              RTKSW_PHY_REG_STATUS
#define RTK_PHY_REG_IDENTIFIER_1        RTKSW_PHY_REG_IDENTIFIER_1
#define RTK_PHY_REG_IDENTIFIER_2        RTKSW_PHY_REG_IDENTIFIER_2
#define RTK_PHY_REG_AN_ADVERTISEMENT    RTKSW_PHY_REG_AN_ADVERTISEMENT
#define RTK_PHY_REG_AN_LINKPARTNER      RTKSW_PHY_REG_AN_LINKPARTNER
#define RTK_PHY_REG_1000_BASET_CONTROL  RTKSW_PHY_REG_1000_BASET_CONTROL
#define RTK_PHY_REG_1000_BASET_STATUS   RTKSW_PHY_REG_1000_BASET_STATUS

#define RTK_PORT_SPEED_10M          RTKSW_PORT_SPEED_10M
#define RTK_PORT_SPEED_100M         RTKSW_PORT_SPEED_100M
#define RTK_PORT_SPEED_1000M        RTKSW_PORT_SPEED_1000M
#define RTK_PORT_SPEED_500M         RTKSW_PORT_SPEED_500M
#define RTK_PORT_SPEED_2500M        RTKSW_PORT_SPEED_2500M
#define RTK_PORT_SPEED_10G          RTKSW_PORT_SPEED_10G

#define RTK_PORT_MEDIA_COPPER       RTKSW_PORT_MEDIA_COPPER
#define RTK_PORT_MEDIA_FIBER        RTKSW_PORT_MEDIA_FIBER
#define RTK_PORT_MEDIA_AUTO         RTKSW_PORT_MEDIA_AUTO
#define RTK_PORT_MEDIA_RGMII        RTKSW_PORT_MEDIA_RGMII

#define RTK_PHY_TEST_MODE_NORMAL    RTKSW_PHY_TEST_MODE_NORMAL
#define RTK_PHY_TEST_MODE_1         RTKSW_PHY_TEST_MODE_1
#define RTK_PHY_TEST_MODE_2         RTKSW_PHY_TEST_MODE_2
#define RTK_PHY_TEST_MODE_3         RTKSW_PHY_TEST_MODE_3
#define RTK_PHY_TEST_MODE_4         RTKSW_PHY_TEST_MODE_4

#define PORT_SPEED_10M              RTKSW_PORT_SPEED_10M
#define PORT_SPEED_100M             RTKSW_PORT_SPEED_100M
#define PORT_SPEED_1000M            RTKSW_PORT_SPEED_1000M
#define PORT_SPEED_500M             RTKSW_PORT_SPEED_500M
#define PORT_SPEED_2500M            RTKSW_PORT_SPEED_2500M
#define PORT_SPEED_10G              RTKSW_PORT_SPEED_10G
#define PORT_SPEED_END              RTKSW_PORT_SPEED_END

#define PORT_MEDIA_COPPER           RTKSW_PORT_MEDIA_COPPER
#define PORT_MEDIA_FIBER            RTKSW_PORT_MEDIA_FIBER
#define PORT_MEDIA_AUTO             RTKSW_PORT_MEDIA_AUTO
#define PORT_MEDIA_RGMII            RTKSW_PORT_MEDIA_RGMII
#define PORT_MEDIA_END              RTKSW_PORT_MEDIA_END

#define MODE_EXT_DISABLE            RTKSW_MODE_EXT_DISABLE
#define MODE_EXT_RGMII              RTKSW_MODE_EXT_RGMII
#define MODE_EXT_MII_MAC            RTKSW_MODE_EXT_MII_MAC
#define MODE_EXT_MII_PHY            RTKSW_MODE_EXT_MII_PHY
#define MODE_EXT_TMII_MAC           RTKSW_MODE_EXT_TMII_MAC
#define MODE_EXT_TMII_PHY           RTKSW_MODE_EXT_TMII_PHY
#define MODE_EXT_GMII               RTKSW_MODE_EXT_GMII
#define MODE_EXT_RMII_MAC           RTKSW_MODE_EXT_RMII_MAC
#define MODE_EXT_RMII_PHY           RTKSW_MODE_EXT_RMII_PHY
#define MODE_EXT_SGMII              RTKSW_MODE_EXT_SGMII
#define MODE_EXT_HSGMII             RTKSW_MODE_EXT_HSGMII
#define MODE_EXT_1000X_100FX        RTKSW_MODE_EXT_1000X_100FX
#define MODE_EXT_1000X              RTKSW_MODE_EXT_1000X
#define MODE_EXT_100FX              RTKSW_MODE_EXT_100FX
#define MODE_EXT_RGMII_2            RTKSW_MODE_EXT_RGMII_2
#define MODE_EXT_MII_MAC_2          RTKSW_MODE_EXT_MII_MAC_2
#define MODE_EXT_MII_PHY_2          RTKSW_MODE_EXT_MII_PHY_2
#define MODE_EXT_TMII_MAC_2         RTKSW_MODE_EXT_TMII_MAC_2
#define MODE_EXT_TMII_PHY_2         RTKSW_MODE_EXT_TMII_PHY_2
#define MODE_EXT_RMII_MAC_2         RTKSW_MODE_EXT_RMII_MAC_2
#define MODE_EXT_RMII_PHY_2         RTKSW_MODE_EXT_RMII_PHY_2
#define MODE_EXT_FIBER_2P5G         RTKSW_MODE_EXT_FIBER_2P5G
#define MODE_EXT_5GR                RTKSW_MODE_EXT_5GR
#define MODE_EXT_10GR               RTKSW_MODE_EXT_10GR
#define MODE_EXT_USXGMII            RTKSW_MODE_EXT_USXGMII

#endif /* CONFIG_COMPATIBLE_V1_4 */

#endif /* __RTKSW_API_PORT_H__ */



