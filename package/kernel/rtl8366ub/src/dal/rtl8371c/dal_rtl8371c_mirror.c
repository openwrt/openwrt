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
#include <dal/rtl8371c/dal_rtl8371c_mirror.h>
#include <dal/reg.h>
#include <dal/rtl8371c/rtk_rtl8371c_reg_struct.h>

#include <dal/dal_mgmts.h>

/* Function Name:
 *      dal_rtl8371c_mirror_portBased_set
 * Description:
 *      Set port mirror function.
 * Input:
 *      unit                    - Unit ID
 *      mirroring_port          - Monitor port.
 *      pMirrored_rx_portmask   - Rx mirror port mask.
 *      pMirrored_tx_portmask   - Tx mirror port mask.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number
 *      RT_ERR_PORT_MASK    - Invalid portmask.
 * Note:
 *      The API is to set mirror function of source port and mirror port.
 *      The mirror port can only be set to one port and the TX and RX mirror ports
 *      should be identical.
 */
rtksw_api_ret_t dal_rtl8371c_mirror_portBased_set(rtksw_uint32 unit, rtksw_port_t mirroring_port, rtksw_portmask_t *pMirrored_rx_portmask, rtksw_portmask_t *pMirrored_tx_portmask)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 rxPmask;
    rtksw_uint32 txPmask;
    rtksw_uint32 monitor_port;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check port valid */
    RTKSW_CHK_PORT_VALID(unit, mirroring_port);

    if(NULL == pMirrored_rx_portmask)
        return RT_ERR_NULL_POINTER;

    if(NULL == pMirrored_tx_portmask)
        return RT_ERR_NULL_POINTER;

    RTKSW_CHK_PORTMASK_VALID(unit, pMirrored_rx_portmask);

    RTKSW_CHK_PORTMASK_VALID(unit, pMirrored_tx_portmask);

    /*mirror port != source port*/
    if(RTKSW_PORTMASK_IS_PORT_SET((*pMirrored_tx_portmask), mirroring_port) || RTKSW_PORTMASK_IS_PORT_SET((*pMirrored_rx_portmask), mirroring_port))
        return RT_ERR_PORT_MASK;

    /* Configure source portmask */
    if ((retVal = rtksw_switch_portmask_L2P_get(unit, pMirrored_rx_portmask, &rxPmask)) != RT_ERR_OK)
        return retVal;

    if ((retVal = rtksw_switch_portmask_L2P_get(unit, pMirrored_tx_portmask, &txPmask)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_field_write(unit, RTL8371C_MIRROR_SRC_PMASKr, RTL8371C_MIRROR_TX_PMSKf, &txPmask)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_field_write(unit, RTL8371C_MIRROR_SRC_PMASKr, RTL8371C_MIRROR_RX_PMSKf, &rxPmask)) != RT_ERR_OK)
        return retVal;

    /* Configure monitor(destination) port */
    monitor_port = rtksw_switch_port_L2P_get(unit, mirroring_port);
    if ((retVal = reg16_field_write(unit, RTL8371C_MIRROR_SET_CTRLr, RTL8371C_MIRROR_MONITOR_PORTf, &monitor_port)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rtl8371c_mirror_portBased_get
 * Description:
 *      Get port mirror function.
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pMirroring_port         - Monitor port.
 *      pMirrored_rx_portmask   - Rx mirror port mask.
 *      pMirrored_tx_portmask   - Tx mirror port mask.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      The API is to get mirror function of source port and mirror port.
 */
rtksw_api_ret_t dal_rtl8371c_mirror_portBased_get(rtksw_uint32 unit, rtksw_port_t *pMirroring_port, rtksw_portmask_t *pMirrored_rx_portmask, rtksw_portmask_t *pMirrored_tx_portmask)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 rxPmask;
    rtksw_uint32 txPmask;
    rtksw_uint32 mport;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(NULL == pMirrored_rx_portmask)
        return RT_ERR_NULL_POINTER;

    if(NULL == pMirrored_tx_portmask)
        return RT_ERR_NULL_POINTER;

    if(NULL == pMirroring_port)
        return RT_ERR_NULL_POINTER;

    /* Get source portmask */
    if ((retVal = reg16_field_read(unit, RTL8371C_MIRROR_SRC_PMASKr, RTL8371C_MIRROR_TX_PMSKf, &txPmask)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_field_read(unit, RTL8371C_MIRROR_SRC_PMASKr, RTL8371C_MIRROR_RX_PMSKf, &rxPmask)) != RT_ERR_OK)
        return retVal;

    if ((retVal = rtksw_switch_portmask_P2L_get(unit, txPmask, pMirrored_tx_portmask)) != RT_ERR_OK)
            return retVal;

    if ((retVal = rtksw_switch_portmask_P2L_get(unit, rxPmask, pMirrored_rx_portmask)) != RT_ERR_OK)
            return retVal;

    /* Get monitor(destination) port */
    if ((retVal = reg16_field_read(unit, RTL8371C_MIRROR_SET_CTRLr, RTL8371C_MIRROR_MONITOR_PORTf, &mport)) != RT_ERR_OK)
        return retVal;

    *pMirroring_port = rtksw_switch_port_P2L_get(unit, mport);
    return RT_ERR_OK;

}

/* Function Name:
 *      dal_rtl8371c_mirror_portIso_set
 * Description:
 *      Set mirror port isolation.
 * Input:
 *      unit    - Unit ID
 *      enable  - Mirror isolation status.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_ENABLE       - Invalid enable input
 * Note:
 *      The API is to set mirror isolation function that prevent normal forwarding packets to miror port.
 */
rtksw_api_ret_t dal_rtl8371c_mirror_portIso_set(rtksw_uint32 unit, rtksw_enable_t enable)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 isoEn;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (enable >= RTKSW_ENABLE_END)
        return RT_ERR_ENABLE;

    isoEn = (enable == RTKSW_ENABLED) ? 1 : 0;
    if ((retVal = reg16_field_write(unit, RTL8371C_MIRROR_SET_CTRLr, RTL8371C_MIRROR_ISOf, &isoEn)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_mirror_portIso_get
 * Description:
 *      Get mirror port isolation.
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pEnable - Mirror isolation status.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      The API is to get mirror isolation status.
 */
rtksw_api_ret_t dal_rtl8371c_mirror_portIso_get(rtksw_uint32 unit, rtksw_enable_t *pEnable)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 isoEn;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(NULL == pEnable)
        return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_field_read(unit, RTL8371C_MIRROR_SET_CTRLr, RTL8371C_MIRROR_ISOf, &isoEn)) != RT_ERR_OK)
        return retVal;

    *pEnable = (isoEn == 1) ? RTKSW_ENABLED : RTKSW_DISABLED;
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_mirror_vlanLeaky_set
 * Description:
 *      Set mirror VLAN leaky.
 * Input:
 *      unit        - Unit ID
 *      txenable    - TX leaky enable.
 *      rxenable    - RX leaky enable.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_ENABLE       - Invalid enable input
 * Note:
 *      The API is to set mirror VLAN leaky function forwarding packets to miror port.
 */
rtksw_api_ret_t dal_rtl8371c_mirror_vlanLeaky_set(rtksw_uint32 unit, rtksw_enable_t txenable, rtksw_enable_t rxenable)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 txEn, rxEn;
    rtksw_uint32 regData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if ((txenable >= RTKSW_ENABLE_END) ||(rxenable >= RTKSW_ENABLE_END))
        return RT_ERR_ENABLE;

    txEn = (txenable == RTKSW_ENABLED) ? 1 : 0;
    rxEn = (rxenable == RTKSW_ENABLED) ? 1 : 0;
    if ((retVal = reg16_read(unit, RTL8371C_MIRROR_CTRLr, &regData)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_field_set(unit, RTL8371C_MIRROR_CTRLr, RTL8371C_MIRROR_TX_VLAN_LEAKYf, &txEn, &regData)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_field_set(unit, RTL8371C_MIRROR_CTRLr, RTL8371C_MIRROR_RX_VLAN_LEAKYf, &rxEn, &regData)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_write(unit, RTL8371C_MIRROR_CTRLr, &regData)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_mirror_vlanLeaky_get
 * Description:
 *      Get mirror VLAN leaky.
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pTxenable - TX leaky enable.
 *      pRxenable - RX leaky enable.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      The API is to get mirror VLAN leaky status.
 */
rtksw_api_ret_t dal_rtl8371c_mirror_vlanLeaky_get(rtksw_uint32 unit, rtksw_enable_t *pTxenable, rtksw_enable_t *pRxenable)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 txEn, rxEn;
    rtksw_uint32 regData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if( (NULL == pTxenable) || (NULL == pRxenable) )
        return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_read(unit, RTL8371C_MIRROR_CTRLr, &regData)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_field_get(unit, RTL8371C_MIRROR_CTRLr, RTL8371C_MIRROR_TX_VLAN_LEAKYf, &txEn, &regData)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_field_get(unit, RTL8371C_MIRROR_CTRLr, RTL8371C_MIRROR_RX_VLAN_LEAKYf, &rxEn, &regData)) != RT_ERR_OK)
        return retVal;

    *pTxenable = (txEn == 1) ? RTKSW_ENABLED : RTKSW_DISABLED;
    *pRxenable = (rxEn == 1) ? RTKSW_ENABLED : RTKSW_DISABLED;
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_mirror_isolationLeaky_set
 * Description:
 *      Set mirror Isolation leaky.
 * Input:
 *      unit        - Unit ID
 *      txenable    - TX leaky enable.
 *      rxenable    - RX leaky enable.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_ENABLE       - Invalid enable input
 * Note:
 *      The API is to set mirror VLAN leaky function forwarding packets to miror port.
 */
rtksw_api_ret_t dal_rtl8371c_mirror_isolationLeaky_set(rtksw_uint32 unit, rtksw_enable_t txenable, rtksw_enable_t rxenable)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 txEn, rxEn;
    rtksw_uint32 regData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if ((txenable >= RTKSW_ENABLE_END) ||(rxenable >= RTKSW_ENABLE_END))
        return RT_ERR_ENABLE;

    txEn = (txenable == RTKSW_ENABLED) ? 1 : 0;
    rxEn = (rxenable == RTKSW_ENABLED) ? 1 : 0;
    if ((retVal = reg16_read(unit, RTL8371C_MIRROR_CTRLr, &regData)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_field_set(unit, RTL8371C_MIRROR_CTRLr, RTL8371C_MIRROR_TX_ISOLATION_LEAKYf, &txEn, &regData)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_field_set(unit, RTL8371C_MIRROR_CTRLr, RTL8371C_MIRROR_RX_ISOLATION_LEAKYf, &rxEn, &regData)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_write(unit, RTL8371C_MIRROR_CTRLr, &regData)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_mirror_isolationLeaky_get
 * Description:
 *      Get mirror isolation leaky.
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pTxenable - TX leaky enable.
 *      pRxenable - RX leaky enable.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      The API is to get mirror isolation leaky status.
 */
rtksw_api_ret_t dal_rtl8371c_mirror_isolationLeaky_get(rtksw_uint32 unit, rtksw_enable_t *pTxenable, rtksw_enable_t *pRxenable)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 txEn, rxEn;
    rtksw_uint32 regData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if( (NULL == pTxenable) || (NULL == pRxenable) )
        return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_read(unit, RTL8371C_MIRROR_CTRLr, &regData)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_field_get(unit, RTL8371C_MIRROR_CTRLr, RTL8371C_MIRROR_TX_ISOLATION_LEAKYf, &txEn, &regData)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_field_get(unit, RTL8371C_MIRROR_CTRLr, RTL8371C_MIRROR_RX_ISOLATION_LEAKYf, &rxEn, &regData)) != RT_ERR_OK)
        return retVal;

    *pTxenable = (txEn == 1) ? RTKSW_ENABLED : RTKSW_DISABLED;
    *pRxenable = (rxEn == 1) ? RTKSW_ENABLED : RTKSW_DISABLED;
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_mirror_keep_set
 * Description:
 *      Set mirror packet format keep.
 * Input:
 *      unit    - Unit ID
 *      mode    - mirror keep mode.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_ENABLE       - Invalid enable input
 * Note:
 *      The API is to set  -mirror keep mode.
 *      The mirror keep mode is as following:
 *      - MIRROR_FOLLOW_VLAN
 *      - MIRROR_KEEP_ORIGINAL
 *      - MIRROR_KEEP_END
 */
rtksw_api_ret_t dal_rtl8371c_mirror_keep_set(rtksw_uint32 unit, rtksw_mirror_keep_t mode)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 keepMode;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (mode >= MIRROR_KEEP_END)
        return RT_ERR_ENABLE;

    keepMode = (mode == MIRROR_FOLLOW_VLAN) ? 0 : 1;
    if ((retVal = reg16_field_write(unit, RTL8371C_MIRROR_CTRLr, RTL8371C_MIRROR_REALKEEP_ENf, &keepMode)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_mirror_keep_get
 * Description:
 *      Get mirror packet format keep.
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pMode - mirror keep mode.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      The API is to get mirror keep mode.
 *      The mirror keep mode is as following:
 *      - MIRROR_FOLLOW_VLAN
 *      - MIRROR_KEEP_ORIGINAL
 *      - MIRROR_KEEP_END
 */
rtksw_api_ret_t dal_rtl8371c_mirror_keep_get(rtksw_uint32 unit, rtksw_mirror_keep_t *pMode)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 keepMode;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(NULL == pMode)
        return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_field_read(unit, RTL8371C_MIRROR_CTRLr, RTL8371C_MIRROR_REALKEEP_ENf, &keepMode)) != RT_ERR_OK)
        return retVal;

    *pMode = (keepMode == 0) ? MIRROR_FOLLOW_VLAN : MIRROR_KEEP_ORIGINAL;
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_mirror_override_set
 * Description:
 *      Set port mirror override function.
 * Input:
 *      unit            - Unit ID
 *      rxMirror        - 1: output mirrored packet, 0: output normal forward packet
 *      txMirror        - 1: output mirrored packet, 0: output normal forward packet
 *      aclMirror       - 1: output mirrored packet, 0: output normal forward packet
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      The API is to set mirror override function.
 *      This function control the output format when a port output
 *      normal forward & mirrored packet at the same time.
 */
rtksw_api_ret_t dal_rtl8371c_mirror_override_set(rtksw_uint32 unit, rtksw_enable_t rxMirror, rtksw_enable_t txMirror, rtksw_enable_t aclMirror)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 rxOverwrite;
    rtksw_uint32 txOverwrite;
    rtksw_uint32 aclOverwrite;
    rtksw_uint32 regData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if( (rxMirror >= RTKSW_ENABLE_END) || (txMirror >= RTKSW_ENABLE_END) || (aclMirror >= RTKSW_ENABLE_END))
        return RT_ERR_ENABLE;

    rxOverwrite = (rxMirror == RTKSW_ENABLED) ? 1 : 0;
    txOverwrite = (txMirror == RTKSW_ENABLED) ? 1 : 0;
    aclOverwrite = (aclMirror == RTKSW_ENABLED) ? 1 : 0;

    if ((retVal = reg16_read(unit, RTL8371C_MIRROR_CTRLr, &regData)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_field_set(unit, RTL8371C_MIRROR_CTRLr, RTL8371C_MIRROR_RX_OVERRIDE_ENf, &rxOverwrite, &regData)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_field_set(unit, RTL8371C_MIRROR_CTRLr, RTL8371C_MIRROR_TX_OVERRIDE_ENf, &txOverwrite, &regData)) != RT_ERR_OK)
        return retVal;
    
    if ((retVal = reg16_field_set(unit, RTL8371C_MIRROR_CTRLr, RTL8371C_MIRROR_ACL_OVERRIDE_ENf, &aclOverwrite, &regData)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_write(unit, RTL8371C_MIRROR_CTRLr, &regData)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_mirror_override_get
 * Description:
 *      Get port mirror override function.
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pRxMirror       - 1: output mirrored packet, 0: output normal forward packet
 *      pTxMirror       - 1: output mirrored packet, 0: output normal forward packet
 *      pAclMirror      - 1: output mirrored packet, 0: output normal forward packet
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_NULL_POINTER - Null Pointer
 * Note:
 *      The API is to Get mirror override function.
 *      This function control the output format when a port output
 *      normal forward & mirrored packet at the same time.
 */
rtksw_api_ret_t dal_rtl8371c_mirror_override_get(rtksw_uint32 unit, rtksw_enable_t *pRxMirror, rtksw_enable_t *pTxMirror, rtksw_enable_t *pAclMirror)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 rxOverwrite;
    rtksw_uint32 txOverwrite;
    rtksw_uint32 aclOverwrite;
    rtksw_uint32 regData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if( (pRxMirror == NULL) || (pTxMirror == NULL) || (pAclMirror == NULL))
        return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_read(unit, RTL8371C_MIRROR_CTRLr, &regData)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_field_get(unit, RTL8371C_MIRROR_CTRLr, RTL8371C_MIRROR_RX_OVERRIDE_ENf, &rxOverwrite, &regData)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_field_get(unit, RTL8371C_MIRROR_CTRLr, RTL8371C_MIRROR_TX_OVERRIDE_ENf, &txOverwrite, &regData)) != RT_ERR_OK)
        return retVal;
    
    if ((retVal = reg16_field_get(unit, RTL8371C_MIRROR_CTRLr, RTL8371C_MIRROR_ACL_OVERRIDE_ENf, &aclOverwrite, &regData)) != RT_ERR_OK)
        return retVal;

    *pRxMirror = (rxOverwrite == 1) ? RTKSW_ENABLED : RTKSW_DISABLED;
    *pTxMirror = (txOverwrite == 1) ? RTKSW_ENABLED : RTKSW_DISABLED;
    *pAclMirror = (aclOverwrite == 1) ? RTKSW_ENABLED : RTKSW_DISABLED;
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_mirror_sampling_set
 * Description:
 *      Set port mirror sampling function.
 * Input:
 *      unit            - Unit ID
 *      state           - Enable/Disable mirror sampling
 *      sampleNumber    - TX one mirrored packet after sampleNumber packets
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      The API is used to set mirror sampling function.
 */
extern rtksw_api_ret_t dal_rtl8371c_mirror_sampling_set(rtksw_uint32 unit, rtksw_enable_t state, rtksw_uint32 sampleNumber)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (state >= RTKSW_ENABLE_END)
        return RT_ERR_ENABLE;

    if (sampleNumber > 0xFFFF)
        return RT_ERR_INPUT;

    if(state == RTKSW_ENABLED)
        regData = sampleNumber;
    else
        regData = 0;

    if ((retVal = reg16_field_write(unit, RTL8371C_MIRROR_SAMPLE_CFGr, RTL8371C_SAMPLE_RATEf, &regData)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_mirror_sampling_get
 * Description:
 *      get port mirror sampling function.
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pState          - Enable/Disable mirror sampling
 *      pSampleNumber   - TX one mirrored packet after sampleNumber packets
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      The API is used to get mirror sampling function.
 */
extern rtksw_api_ret_t dal_rtl8371c_mirror_sampling_get(rtksw_uint32 unit, rtksw_enable_t *pState, rtksw_uint32 *pSampleNumber)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if ((pState == NULL) || (pSampleNumber == NULL))
        return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_field_read(unit, RTL8371C_MIRROR_SAMPLE_CFGr, RTL8371C_SAMPLE_RATEf, &regData)) != RT_ERR_OK)
        return retVal;

    if (regData == 0)
    {
        *pState = RTKSW_DISABLED;
        *pSampleNumber = 0;
    }
    else
    {
        *pState = RTKSW_ENABLED;
        *pSampleNumber = regData;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_mirror_samplingCounter_get
 * Description:
 *      get port mirror sampling counter function.
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pState          - Enable/Disable mirror sampling
 *      pSampleNumber   - TX one mirrored packet after sampleNumber packets
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      The API is used to get mirror sampling counter function.
 */
extern rtksw_api_ret_t dal_rtl8371c_mirror_samplingCounter_get(rtksw_uint32 unit, rtksw_uint32 *pMirroredCounter, rtksw_uint32 *pSampleCounter)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if ((pMirroredCounter == NULL) || (pSampleCounter == NULL))
        return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_field_read(unit, RTL8371C_MIRROR_SAMPLE_CNT0r, RTL8371C_MIR_PKT_CNTf, &regData)) != RT_ERR_OK)
        return retVal;

    *pMirroredCounter = regData;

    if ((retVal = reg16_field_read(unit, RTL8371C_MIRROR_SAMPLE_CNT1r, RTL8371C_SAMPKE_PKT_CNTf, &regData)) != RT_ERR_OK)
        return retVal;

    *pSampleCounter = regData;

    return RT_ERR_OK;
}
