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
#include <dal/rtl8371c/dal_rtl8371c_svlan.h>
#include <dal/rtl8371c/dal_rtl8371c_vlan.h>
#include <dal/rtl8371c/dal_rtl8371c_cpu.h>
#include <dal/reg.h>
#include <dal/rtl8371c/rtk_rtl8371c_reg_struct.h>
#include <dal/rtl8371c/rtk_rtl8371c_table_struct.h>

#include <dal/dal_mgmts.h>
#include <osal/lib.h>

/* Function Name:
 *      dal_rtl8371c_svlaninit
 * Description:
 *      Initialize SVLAN Configuration
 *  Input:
 *      unit        - Unit ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      Ether type of S-tag in 802.1ad is 0x88a8 and there are existed ether type 0x9100 and 0x9200 for Q-in-Q SLAN design.
 *      User can set mathced ether type as service provider supported protocol.
 */
rtksw_api_ret_t dal_rtl8371c_svlaninit(rtksw_uint32 unit)
{
    rtksw_uint32 i;
    rtksw_uint32 fieldData;
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /*default use C-priority*/
    if ((retVal = dal_rtl8371c_svlanpriorityRef_set(unit, REF_CTAG_PRI)) != RT_ERR_OK)
        return retVal;

    /*Drop SVLAN untag frame*/
    if ((retVal = dal_rtl8371c_svlanuntag_action_set(unit, UNTAG_DROP, 0)) != RT_ERR_OK)
        return retVal;

    /*Set TPID to 0x88a8*/
    if ((retVal = dal_rtl8371c_svlantpidEntry_set(unit, 0x88a8)) != RT_ERR_OK)
        return retVal;

    /*Clean Uplink Port Mask to none*/
    fieldData = 0;
    if ((retVal = reg16_field_read(unit, RTL8371C_VS_UPLINK_PORTr, RTL8371C_MSKf, &fieldData)) != RT_ERR_OK)
        return retVal;

    /*Clean C2S Configuration*/
    for (i=0; i<= RTL8371C_C2SIDXMAX; i++)
    {
        fieldData = 0;
        if ((retVal = reg16_array_field_write(unit, RTL8371C_VLAN_C2S_ENTRYr, REG_ARRAY_INDEX_NONE, i, RTL8371C_SVID_ASSIGNf, &fieldData)) != RT_ERR_OK)
            return retVal;

        if ((retVal = reg16_array_field_write(unit, RTL8371C_VLAN_C2S_ENTRYr, REG_ARRAY_INDEX_NONE, i, RTL8371C_PMSK_ENf, &fieldData)) != RT_ERR_OK)
            return retVal;

        if ((retVal = reg16_array_field_write(unit, RTL8371C_VLAN_C2S_ENTRYr, REG_ARRAY_INDEX_NONE, i, RTL8371C_CVIDf, &fieldData)) != RT_ERR_OK)
            return retVal;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rtl8371c_svlanservicePort_add
 * Description:
 *      Add one service port in the specified device
 *  Input:
 *      unit        - Unit ID
 *      port        - Port id.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API is setting which port is connected to provider switch. All frames receiving from this port must
 *      contain accept SVID in S-tag field.
 */
rtksw_api_ret_t dal_rtl8371c_svlanservicePort_add(rtksw_uint32 unit, rtksw_port_t port)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 pmsk;
    rtksw_uint32 phyPort;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* check port valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    phyPort = rtksw_switch_port_L2P_get(unit, port);
    if (phyPort == UNDEFINE_PHY_PORT)
        return RT_ERR_PORT_ID;

    if ((retVal = reg16_field_read(unit, RTL8371C_VS_UPLINK_PORTr, RTL8371C_MSKf, &pmsk)) != RT_ERR_OK)
        return retVal;

    pmsk = pmsk | (1<<phyPort);

    if ((retVal = reg16_field_write(unit, RTL8371C_VS_UPLINK_PORTr, RTL8371C_MSKf, &pmsk)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_svlanservicePort_get
 * Description:
 *      Get service ports in the specified device.
 *  Input:
 *      unit        - Unit ID
 * Output:
 *      pSvlan_portmask - pointer buffer of svlan ports.
 * Return:
 *      RT_ERR_OK          - OK
 *      RT_ERR_FAILED      - Failed
 *      RT_ERR_SMI         - SMI access error
 * Note:
 *      This API is setting which port is connected to provider switch. All frames receiving from this port must
 *      contain accept SVID in S-tag field.
 */
rtksw_api_ret_t dal_rtl8371c_svlanservicePort_get(rtksw_uint32 unit, rtksw_portmask_t *pSvlan_portmask)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 pmsk;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(NULL == pSvlan_portmask)
        return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_field_read(unit, RTL8371C_VS_UPLINK_PORTr, RTL8371C_MSKf, &pmsk)) != RT_ERR_OK)
        return retVal;

    if(rtksw_switch_portmask_P2L_get(unit, pmsk, pSvlan_portmask) != RT_ERR_OK)
        return RT_ERR_FAILED;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_svlanservicePort_del
 * Description:
 *      Delete one service port in the specified device
 *  Input:
 *      unit        - Unit ID
 *      port        - Port id.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      This API is removing SVLAN service port in the specified device.
 */
rtksw_api_ret_t dal_rtl8371c_svlanservicePort_del(rtksw_uint32 unit, rtksw_port_t port)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 pmsk;
    rtksw_uint32 phyPort;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* check port valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    phyPort = rtksw_switch_port_L2P_get(unit, port);
    if (phyPort == UNDEFINE_PHY_PORT)
        return RT_ERR_PORT_ID;

    if ((retVal = reg16_field_read(unit, RTL8371C_VS_UPLINK_PORTr, RTL8371C_MSKf, &pmsk)) != RT_ERR_OK)
        return retVal;

    pmsk = pmsk & ~(1<<phyPort);

    if ((retVal = reg16_field_write(unit, RTL8371C_VS_UPLINK_PORTr, RTL8371C_MSKf, &pmsk)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_svlantpidEntry_set
 * Description:
 *      Configure accepted S-VLAN ether type.
 *  Input:
 *      unit            - Unit ID
 *      svlan_tag_id    - Ether type of S-tag frame parsing in uplink ports.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameter.
 * Note:
 *      Ether type of S-tag in 802.1ad is 0x88a8 and there are existed ether type 0x9100 and 0x9200 for Q-in-Q SLAN design.
 *      User can set mathced ether type as service provider supported protocol.
 */
rtksw_api_ret_t dal_rtl8371c_svlantpidEntry_set(rtksw_uint32 unit, rtksw_uint32 svlan_tag_id)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (svlan_tag_id>RTKSW_MAX_NUM_OF_PROTO_TYPE)
        return RT_ERR_INPUT;

    if ((retVal = reg16_field_write(unit, RTL8371C_RG02r, RTL8371C_VS_TPIDf, (rtksw_uint32 *)&svlan_tag_id)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_svlantpidEntry_get
 * Description:
 *      Get accepted S-VLAN ether type setting.
 *  Input:
 *      unit        - Unit ID
 * Output:
 *      pSvlan_tag_id -  Ether type of S-tag frame parsing in uplink ports.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      This API is setting which port is connected to provider switch. All frames receiving from this port must
 *      contain accept SVID in S-tag field.
 */
rtksw_api_ret_t dal_rtl8371c_svlantpidEntry_get(rtksw_uint32 unit, rtksw_uint32 *pSvlan_tag_id)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(NULL == pSvlan_tag_id)
        return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_field_read(unit, RTL8371C_RG02r, RTL8371C_VS_TPIDf, pSvlan_tag_id)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_svlanpriorityRef_set
 * Description:
 *      Set S-VLAN upstream priority reference setting.
 *  Input:
 *      unit        - Unit ID
 *      ref         - reference selection parameter.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameter.
 * Note:
 *      The API can set the upstream SVLAN tag priority reference source. The related priority
 *      sources are as following:
 *      - REF_INTERNAL_PRI,
 *      - REF_CTAG_PRI,
 *      - REF_PB_PRI.
 */
rtksw_api_ret_t dal_rtl8371c_svlanpriorityRef_set(rtksw_uint32 unit, rtksw_svlan_pri_ref_t ref)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if ((ref >= REF_PRI_END) || (ref == REF_SVLAN_PRI))
        return RT_ERR_INPUT;

    if ((retVal = reg16_field_write(unit, RTL8371C_VS_CFGr, RTL8371C_VS_SPRISELf, (rtksw_uint32 *)&ref)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_svlanpriorityRef_get
 * Description:
 *      Get S-VLAN upstream priority reference setting.
 *  Input:
 *      unit        - Unit ID
 * Output:
 *      pRef - reference selection parameter.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      The API can get the upstream SVLAN tag priority reference source. The related priority
 *      sources are as following:
 *      - REF_INTERNAL_PRI,
 *      - REF_CTAG_PRI,
 *      - REF_SVLAN_PRI,
 *      - REF_PB_PRI
 */
rtksw_api_ret_t dal_rtl8371c_svlanpriorityRef_get(rtksw_uint32 unit, rtksw_svlan_pri_ref_t *pRef)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(NULL == pRef)
        return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_field_read(unit, RTL8371C_VS_CFGr, RTL8371C_VS_SPRISELf, (rtksw_uint32 *)pRef)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rtl8371c_svlanmemberPortEntry_set
 * Description:
 *      Configure system SVLAN member content
 *  Input:
 *      unit        - Unit ID
 *      svid        - SVLAN id
 *      psvlan_cfg  - SVLAN member configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_INPUT            - Invalid input parameter.
 *      RT_ERR_SVLAN_VID        - Invalid SVLAN VID parameter.
 *      RT_ERR_PORT_MASK        - Invalid portmask.
 *      RT_ERR_SVLAN_TABLE_FULL - SVLAN configuration is full.
 * Note:
 *      The API can set system 64 accepted s-tag frame format. Only 64 SVID S-tag frame will be accpeted
 *      to receiving from uplink ports. Other SVID S-tag frame or S-untagged frame will be droped by default setup.
 *      - rtksw_svlan_memberCfg_t->svid is SVID of SVLAN member configuration.
 *      - rtksw_svlan_memberCfg_t->memberport is member port mask of SVLAN member configuration.
 *      - rtksw_svlan_memberCfg_t->fid is filtering database of SVLAN member configuration.
 *      - rtksw_svlan_memberCfg_t->priority is priority of SVLAN member configuration.
 */
rtksw_api_ret_t dal_rtl8371c_svlanmemberPortEntry_set(rtksw_uint32 unit, rtksw_vlan_t svid, rtksw_svlan_memberCfg_t *pSvlan_cfg)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 phyMbrPmask, phyUntagPmask;
    rtksw_uint32 fieldData;
    rtl8371c_vlan_entry_t vlanEntry;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(NULL == pSvlan_cfg)
        return RT_ERR_NULL_POINTER;

    if(svid > RTL8371C_VIDMAX)
        return RT_ERR_SVLAN_VID;

    RTKSW_CHK_PORTMASK_VALID(unit, &(pSvlan_cfg->memberport));

    RTKSW_CHK_PORTMASK_VALID(unit, &(pSvlan_cfg->untagport));

    if (pSvlan_cfg->fid > RTL8371C_FIDMAX)
        return RT_ERR_L2_FID;

    if (pSvlan_cfg->chk_ivl_svl> RTKSW_ENABLED)
        return RT_ERR_INPUT;

    if (pSvlan_cfg->ivl_svl> RTKSW_ENABLED)
        return RT_ERR_INPUT;

    if (pSvlan_cfg->fiden !=0)
        return RT_ERR_CHIP_NOT_SUPPORTED;

    if (pSvlan_cfg->priority != 0)
        return RT_ERR_CHIP_NOT_SUPPORTED;

    if (pSvlan_cfg->efiden != 0)
        return RT_ERR_CHIP_NOT_SUPPORTED;

    if (pSvlan_cfg->efid != 0)
        return RT_ERR_CHIP_NOT_SUPPORTED;

    /* Get physical port mask */
    if(rtksw_switch_portmask_L2P_get(unit, &(pSvlan_cfg->memberport), &phyMbrPmask) != RT_ERR_OK)
        return RT_ERR_FAILED;
    if(rtksw_switch_portmask_L2P_get(unit, &(pSvlan_cfg->untagport), &phyUntagPmask) != RT_ERR_OK)
        return RT_ERR_FAILED;

    osal_memset(&vlanEntry, 0, sizeof(rtl8371c_vlan_entry_t));

    /* MBR */
    if ((retVal = table16_field_set(unit, RTL8371C_VLAN_TBLt, RTL8371C_VLAN_TBL_MBRtf, &phyMbrPmask, (rtksw_uint32 *)&vlanEntry)) != RT_ERR_OK)
        return retVal;

    /* UNTAG */
    if ((retVal = table16_field_set(unit, RTL8371C_VLAN_TBLt, RTL8371C_VLAN_TBL_UNTAGSETtf, &phyUntagPmask, (rtksw_uint32 *)&vlanEntry)) != RT_ERR_OK)
        return retVal;

    /* SVLAN CHK SVL_IVL */
    fieldData = pSvlan_cfg->chk_ivl_svl;
    if ((retVal = table16_field_set(unit, RTL8371C_VLAN_TBLt, RTL8371C_VLAN_TBL_SVLAN_CHK_IVL_SVLtf, &fieldData, (rtksw_uint32 *)&vlanEntry)) != RT_ERR_OK)
        return retVal;

    /* SVL_IVL */
    fieldData = pSvlan_cfg->ivl_svl;
    if ((retVal = table16_field_set(unit, RTL8371C_VLAN_TBLt, RTL8371C_VLAN_TBL_IVL_SVLtf, &fieldData, (rtksw_uint32 *)&vlanEntry)) != RT_ERR_OK)
        return retVal;

    /* FID */
    fieldData = pSvlan_cfg->fid;
    if ((retVal = table16_field_set(unit, RTL8371C_VLAN_TBLt, RTL8371C_VLAN_TBL_FID_MSTItf, &fieldData, (rtksw_uint32 *)&vlanEntry)) != RT_ERR_OK)
        return retVal;

    if ((retVal = table16_write(unit, RTL8371C_VLAN_TBLt, svid, (rtksw_uint32 *)&vlanEntry)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_svlanmemberPortEntry_get
 * Description:
 *      Get SVLAN member Configure.
 *  Input:
 *      unit        - Unit ID
 *      svid        - SVLAN id
 * Output:
 *      pSvlan_cfg - SVLAN member configuration
 * Return:
 *      RT_ERR_OK                       - OK
 *      RT_ERR_FAILED                   - Failed
 *      RT_ERR_SMI                      - SMI access error
 *      RT_ERR_SVLAN_ENTRY_NOT_FOUND    - specified svlan entry not found.
 *      RT_ERR_INPUT                    - Invalid input parameters.
 * Note:
 *      The API can get system 64 accepted s-tag frame format. Only 64 SVID S-tag frame will be accpeted
 *      to receiving from uplink ports. Other SVID S-tag frame or S-untagged frame will be droped.
 */
rtksw_api_ret_t dal_rtl8371c_svlanmemberPortEntry_get(rtksw_uint32 unit, rtksw_vlan_t svid, rtksw_svlan_memberCfg_t *pSvlan_cfg)
{
    rtksw_api_ret_t retVal;
    rtl8371c_vlan_entry_t vlanEntry;
    rtksw_uint32 fieldData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(NULL == pSvlan_cfg)
        return RT_ERR_NULL_POINTER;

    if (svid > RTL8371C_VIDMAX)
        return RT_ERR_SVLAN_VID;

    osal_memset(pSvlan_cfg, 0, sizeof(rtksw_svlan_memberCfg_t));

    osal_memset(&vlanEntry, 0, sizeof(rtl8371c_vlan_entry_t));
    if ((retVal = table16_read(unit, RTL8371C_VLAN_TBLt, svid, (rtksw_uint32 *)&vlanEntry)) != RT_ERR_OK)
        return retVal;

    /* MBR */
    if ((retVal = table16_field_get(unit, RTL8371C_VLAN_TBLt, RTL8371C_VLAN_TBL_MBRtf, &fieldData, (rtksw_uint32 *)&vlanEntry)) != RT_ERR_OK)
        return retVal;
    if ((retVal = rtksw_switch_portmask_P2L_get(unit, fieldData, &(pSvlan_cfg->memberport))) != RT_ERR_OK)
        return RT_ERR_FAILED;

    /* UNTAG */
    if ((retVal = table16_field_get(unit, RTL8371C_VLAN_TBLt, RTL8371C_VLAN_TBL_UNTAGSETtf, &fieldData, (rtksw_uint32 *)&vlanEntry)) != RT_ERR_OK)
        return retVal;
    if ((retVal = rtksw_switch_portmask_P2L_get(unit, fieldData, &(pSvlan_cfg->untagport))) != RT_ERR_OK)
        return RT_ERR_FAILED;

    /* SVLAN CHK SVL_IVL */
    if ((retVal = table16_field_get(unit, RTL8371C_VLAN_TBLt, RTL8371C_VLAN_TBL_SVLAN_CHK_IVL_SVLtf, &fieldData, (rtksw_uint32 *)&vlanEntry)) != RT_ERR_OK)
        return retVal;
    
    pSvlan_cfg->chk_ivl_svl = fieldData;

    /* SVL_IVL */
    if ((retVal = table16_field_get(unit, RTL8371C_VLAN_TBLt, RTL8371C_VLAN_TBL_IVL_SVLtf, &fieldData, (rtksw_uint32 *)&vlanEntry)) != RT_ERR_OK)
        return retVal;

    pSvlan_cfg->ivl_svl = fieldData;

    /* FID */
    if ((retVal = table16_field_get(unit, RTL8371C_VLAN_TBLt, RTL8371C_VLAN_TBL_FID_MSTItf, &fieldData, (rtksw_uint32 *)&vlanEntry)) != RT_ERR_OK)
        return retVal;

    pSvlan_cfg->fid = fieldData;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_svlandefaultSvlan_set
 * Description:
 *      Configure default egress SVLAN.
 *  Input:
 *      unit        - Unit ID
 *      port        - Source port
 *      svid        - SVLAN id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                       - OK
 *      RT_ERR_FAILED                   - Failed
 *      RT_ERR_SMI                      - SMI access error
 *      RT_ERR_INPUT                    - Invalid input parameter.
 *      RT_ERR_SVLAN_VID                - Invalid SVLAN VID parameter.
 *      RT_ERR_SVLAN_ENTRY_NOT_FOUND    - specified svlan entry not found.
 * Note:
 *      The API can set port n S-tag format index while receiving frame from port n
 *      is transmit through uplink port with s-tag field
 */
rtksw_api_ret_t dal_rtl8371c_svlandefaultSvlan_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_vlan_t svid)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    /* svid must be 0~4095 */
    if (svid > RTL8371C_VIDMAX)
        return RT_ERR_SVLAN_VID;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_VS_PORT_DFLT_SVIDr, rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_PORT_DFLT_SVIDf, (rtksw_uint32 *)&svid)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_svlandefaultSvlan_get
 * Description:
 *      Get the configure default egress SVLAN.
 *  Input:
 *      unit        - Unit ID
 *      port        - Source port
 * Output:
 *      pSvid - SVLAN VID
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      The API can get port n S-tag format index while receiving frame from port n
 *      is transmit through uplink port with s-tag field
 */
rtksw_api_ret_t dal_rtl8371c_svlandefaultSvlan_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_vlan_t *pSvid)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(NULL == pSvid)
        return RT_ERR_NULL_POINTER;

    /* Check port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if ((retVal = reg16_array_field_read(unit, RTL8371C_VS_PORT_DFLT_SVIDr, rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_PORT_DFLT_SVIDf, (rtksw_uint32 *)pSvid)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_svlanc2s_add
 * Description:
 *      Configure SVLAN C2S table
 *  Input:
 *      unit        - Unit ID
 *      vid         - VLAN ID
 *      src_port    - Ingress Port
 *      svid        - SVLAN VID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port ID.
 *      RT_ERR_SVLAN_VID    - Invalid SVLAN VID parameter.
 *      RT_ERR_VLAN_VID     - Invalid VID parameter.
 *      RT_ERR_OUT_OF_RANGE - input out of range.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      The API can set system C2S configuration. ASIC will check upstream's VID and assign related
 *      SVID to mathed packet. There are 128 SVLAN C2S configurations.
 */
rtksw_api_ret_t dal_rtl8371c_svlanc2s_add(rtksw_uint32 unit, rtksw_vlan_t vid, rtksw_port_t src_port, rtksw_vlan_t svid)
{
    rtksw_api_ret_t retVal, i;
    rtksw_uint32 empty_idx;
    rtksw_port_t phyPort;
    rtksw_uint16 doneFlag;
    rtksw_uint32 idx_svid, idx_pmsk, idx_cvid;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (vid > RTL8371C_VIDMAX)
        return RT_ERR_VLAN_VID;

    if (svid > RTL8371C_VIDMAX)
        return RT_ERR_SVLAN_VID;

    /* Check port Valid */
    RTKSW_CHK_PORT_VALID(unit, src_port);

    phyPort = rtksw_switch_port_L2P_get(unit, src_port);
    if (phyPort == UNDEFINE_PHY_PORT)
        return RT_ERR_PORT_ID;

    empty_idx = 0xFFFF;
    doneFlag = FALSE;

    for (i = RTL8371C_C2SIDXMAX; i >= 0; i--)
    {
        if ((retVal = reg16_array_field_read(unit, RTL8371C_VLAN_C2S_ENTRYr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)i, RTL8371C_SVID_ASSIGNf, &idx_svid)) != RT_ERR_OK)
            return retVal;

        if ((retVal = reg16_array_field_read(unit, RTL8371C_VLAN_C2S_ENTRYr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)i, RTL8371C_PMSK_ENf, &idx_pmsk)) != RT_ERR_OK)
            return retVal;

        if ((retVal = reg16_array_field_read(unit, RTL8371C_VLAN_C2S_ENTRYr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)i, RTL8371C_CVIDf, &idx_cvid)) != RT_ERR_OK)
            return retVal;

        if (idx_pmsk != 0) /* Valid entry */
        {
            if (idx_cvid == vid)
            {
                /* Check Src_port */
                if(idx_pmsk & (1 << phyPort))
                {
                    /* Check SVIDX */
                    if(idx_svid == svid)
                    {
                        /* All the same, do nothing */
                        return RT_ERR_OK;
                    }
                    else
                    {
                        /* New svidx, remove src_port and find a new slot to add a new enrty */
                        idx_pmsk = idx_pmsk & ~(1 << phyPort);
                        if(idx_pmsk == 0)
                        {
                            idx_svid = 0;
                            idx_cvid = 0;
                        }

                        if ((retVal = reg16_array_field_write(unit, RTL8371C_VLAN_C2S_ENTRYr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)i, RTL8371C_SVID_ASSIGNf, &idx_svid)) != RT_ERR_OK)
                            return retVal;

                        if ((retVal = reg16_array_field_write(unit, RTL8371C_VLAN_C2S_ENTRYr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)i, RTL8371C_PMSK_ENf, &idx_pmsk)) != RT_ERR_OK)
                            return retVal;

                        if ((retVal = reg16_array_field_write(unit, RTL8371C_VLAN_C2S_ENTRYr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)i, RTL8371C_CVIDf, &idx_cvid)) != RT_ERR_OK)
                            return retVal;
                    }
                }
                else
                {
                    if(idx_svid == svid && doneFlag == FALSE)
                    {
                        idx_pmsk = idx_pmsk | (1 << phyPort);
                        if ((retVal = reg16_array_field_write(unit, RTL8371C_VLAN_C2S_ENTRYr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)i, RTL8371C_PMSK_ENf, &idx_pmsk)) != RT_ERR_OK)
                            return retVal;

                        doneFlag = TRUE;
                    }
                }
            }
        }
        else /* Empty entry */
        {
            empty_idx = i;
        }
    }

    if (0xFFFF != empty_idx && doneFlag ==FALSE)
    {
        if ((retVal = reg16_array_field_write(unit, RTL8371C_VLAN_C2S_ENTRYr, REG_ARRAY_INDEX_NONE, empty_idx, RTL8371C_SVID_ASSIGNf, (rtksw_uint32 *)&svid)) != RT_ERR_OK)
            return retVal;

        idx_pmsk = 1 << phyPort;
        if ((retVal = reg16_array_field_write(unit, RTL8371C_VLAN_C2S_ENTRYr, REG_ARRAY_INDEX_NONE, empty_idx, RTL8371C_PMSK_ENf, &idx_pmsk)) != RT_ERR_OK)
            return retVal;

        if ((retVal = reg16_array_field_write(unit, RTL8371C_VLAN_C2S_ENTRYr, REG_ARRAY_INDEX_NONE, empty_idx, RTL8371C_CVIDf, (rtksw_uint32 *)&vid)) != RT_ERR_OK)
            return retVal;

       return RT_ERR_OK;
    }
    else if(doneFlag == TRUE)
    {
        return RT_ERR_OK;
    }

    return RT_ERR_OUT_OF_RANGE;
}

/* Function Name:
 *      dal_rtl8371c_svlanc2s_del
 * Description:
 *      Delete one C2S entry
 *  Input:
 *      unit        - Unit ID
 *      vid         - VLAN ID
 *      src_port    - Ingress Port
 *      svid        - SVLAN VID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_VLAN_VID         - Invalid VID parameter.
 *      RT_ERR_PORT_ID          - Invalid port ID.
 *      RT_ERR_OUT_OF_RANGE     - input out of range.
 * Note:
 *      The API can delete system C2S configuration. There are 128 SVLAN C2S configurations.
 */
rtksw_api_ret_t dal_rtl8371c_svlanc2s_del(rtksw_uint32 unit, rtksw_vlan_t vid, rtksw_port_t src_port)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 i;
    rtksw_port_t phyPort;
    rtksw_uint32 idx_svid, idx_pmsk, idx_cvid;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (vid > RTL8371C_VIDMAX)
        return RT_ERR_VLAN_VID;

    /* Check port Valid */
    RTKSW_CHK_PORT_VALID(unit, src_port);
    phyPort = rtksw_switch_port_L2P_get(unit, src_port);
    if (phyPort == UNDEFINE_PHY_PORT)
        return RT_ERR_PORT_ID;

    for (i = 0; i <= RTL8371C_C2SIDXMAX; i++)
    {
        if ((retVal = reg16_array_field_read(unit, RTL8371C_VLAN_C2S_ENTRYr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)i, RTL8371C_SVID_ASSIGNf, &idx_svid)) != RT_ERR_OK)
            return retVal;

        if ((retVal = reg16_array_field_read(unit, RTL8371C_VLAN_C2S_ENTRYr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)i, RTL8371C_PMSK_ENf, &idx_pmsk)) != RT_ERR_OK)
            return retVal;

        if ((retVal = reg16_array_field_read(unit, RTL8371C_VLAN_C2S_ENTRYr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)i, RTL8371C_CVIDf, &idx_cvid)) != RT_ERR_OK)
            return retVal;

        if (idx_cvid == vid)
        {
            if(idx_pmsk & (1 << phyPort))
            {
                idx_pmsk = idx_pmsk & ~(1 << phyPort);
                if(idx_pmsk == 0)
                {
                    idx_cvid = 0;
                    idx_svid = 0;
                }

                if ((retVal = reg16_array_field_write(unit, RTL8371C_VLAN_C2S_ENTRYr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)i, RTL8371C_SVID_ASSIGNf, &idx_svid)) != RT_ERR_OK)
                    return retVal;

                if ((retVal = reg16_array_field_write(unit, RTL8371C_VLAN_C2S_ENTRYr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)i, RTL8371C_PMSK_ENf, &idx_pmsk)) != RT_ERR_OK)
                    return retVal;

                if ((retVal = reg16_array_field_write(unit, RTL8371C_VLAN_C2S_ENTRYr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)i, RTL8371C_CVIDf, &idx_cvid)) != RT_ERR_OK)
                    return retVal;

                return RT_ERR_OK;
            }
        }
    }

    return RT_ERR_OUT_OF_RANGE;
}

/* Function Name:
 *      dal_rtl8371c_svlanc2s_get
 * Description:
 *      Get configure SVLAN C2S table
 *  Input:
 *      unit        - Unit ID
 *      vid         - VLAN ID
 *      src_port    - Ingress Port
 * Output:
 *      pSvid - SVLAN ID
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_PORT_ID      - Invalid port ID.
 *      RT_ERR_OUT_OF_RANGE - input out of range.
 * Note:
 *     The API can get system C2S configuration. There are 128 SVLAN C2S configurations.
 */
rtksw_api_ret_t dal_rtl8371c_svlanc2s_get(rtksw_uint32 unit, rtksw_vlan_t vid, rtksw_port_t src_port, rtksw_vlan_t *pSvid)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 i;
    rtksw_uint32 idx_svid, idx_pmsk, idx_cvid;
    rtksw_port_t phyPort;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(NULL == pSvid)
        return RT_ERR_NULL_POINTER;

    if (vid > RTL8371C_VIDMAX)
        return RT_ERR_VLAN_VID;

    /* Check port Valid */
    RTKSW_CHK_PORT_VALID(unit, src_port);
    phyPort = rtksw_switch_port_L2P_get(unit, src_port);
    if (phyPort == UNDEFINE_PHY_PORT)
        return RT_ERR_PORT_ID;

    for (i = 0; i <= RTL8371C_C2SIDXMAX; i++)
    {
        if ((retVal = reg16_array_field_read(unit, RTL8371C_VLAN_C2S_ENTRYr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)i, RTL8371C_SVID_ASSIGNf, &idx_svid)) != RT_ERR_OK)
            return retVal;

        if ((retVal = reg16_array_field_read(unit, RTL8371C_VLAN_C2S_ENTRYr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)i, RTL8371C_PMSK_ENf, &idx_pmsk)) != RT_ERR_OK)
            return retVal;

        if ((retVal = reg16_array_field_read(unit, RTL8371C_VLAN_C2S_ENTRYr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)i, RTL8371C_CVIDf, &idx_cvid)) != RT_ERR_OK)
            return retVal;

        if (idx_cvid == vid)
        {
            if(idx_pmsk & (1 << phyPort))
            {
                *pSvid = idx_svid;
                return RT_ERR_OK;
            }
        }
    }

    return RT_ERR_OUT_OF_RANGE;
}

/* Function Name:
 *      dal_rtl8371c_svlan_sp2c_add
 * Description:
 *      Add system SP2C configuration
 *  Input:
 *      unit        - Unit ID
 *      cvid        - VLAN ID
 *      dst_port    - Destination port of SVLAN to CVLAN configuration
 *      svid        - SVLAN VID
 *
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_SVLAN_VID    - Invalid SVLAN VID parameter.
 *      RT_ERR_VLAN_VID     - Invalid VID parameter.
 *      RT_ERR_OUT_OF_RANGE - input out of range.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      The API can add SVID & Destination Port to CVLAN configuration. The downstream frames with assigned
 *      SVID will be add C-tag with assigned CVID if the output port is the assigned destination port.
 *      There are 128 SP2C configurations.
 */
rtksw_api_ret_t dal_rtl8371c_svlan_sp2c_add(rtksw_uint32 unit, rtksw_vlan_t svid, rtksw_port_t dst_port, rtksw_vlan_t cvid)
{
    rtksw_api_ret_t retVal, i;
    rtksw_uint32 empty_idx;
    rtksw_port_t port;
    rtksw_uint32 idx_svid, idx_port;
    rtksw_uint32 valid_flag;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (svid > RTL8371C_VIDMAX)
        return RT_ERR_SVLAN_VID;

    if (cvid > RTL8371C_VIDMAX)
        return RT_ERR_VLAN_VID;

    /* Check port Valid */
    RTKSW_CHK_PORT_VALID(unit, dst_port);
    port = rtksw_switch_port_L2P_get(unit, dst_port);
    empty_idx = 0xFFFF;

    for (i = RTL8371C_SP2CMAX; i>=0; i--)
    {
        if ((retVal = reg16_array_field_read(unit, RTL8371C_VS_SP2Cr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)i, RTL8371C_SVIDf, &idx_svid)) != RT_ERR_OK)
            return retVal;

        if ((retVal = reg16_array_field_read(unit, RTL8371C_VS_SP2Cr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)i, RTL8371C_DST_PORTf, &idx_port)) != RT_ERR_OK)
            return retVal;

        if ((retVal = reg16_array_field_read(unit, RTL8371C_VS_SP2Cr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)i, RTL8371C_VALIDf, &valid_flag)) != RT_ERR_OK)
            return retVal;

        if ( (idx_svid == svid) && (idx_port == port) && (valid_flag == 1))
        {
            empty_idx = i;
            break;
        }
        else if (valid_flag == 0)
        {
            empty_idx = i;
        }
    }

    if (empty_idx!=0xFFFF)
    {
        valid_flag = 1;

        if ((retVal = reg16_array_field_write(unit, RTL8371C_VS_SP2Cr, REG_ARRAY_INDEX_NONE, empty_idx, RTL8371C_SVIDf, (rtksw_uint32 *)&svid)) != RT_ERR_OK)
            return retVal;

        if ((retVal = reg16_array_field_write(unit, RTL8371C_VS_SP2Cr, REG_ARRAY_INDEX_NONE, empty_idx, RTL8371C_DST_PORTf, (rtksw_uint32 *)&port)) != RT_ERR_OK)
            return retVal;

        if ((retVal = reg16_array_field_write(unit, RTL8371C_VS_SP2Cr, REG_ARRAY_INDEX_NONE, empty_idx, RTL8371C_VIDf, (rtksw_uint32 *)&cvid)) != RT_ERR_OK)
            return retVal;

        if ((retVal = reg16_array_field_write(unit, RTL8371C_VS_SP2Cr, REG_ARRAY_INDEX_NONE, empty_idx, RTL8371C_VALIDf, (rtksw_uint32 *)&valid_flag)) != RT_ERR_OK)
            return retVal;

        return RT_ERR_OK;
    }

    return RT_ERR_OUT_OF_RANGE;

}

/* Function Name:
 *      dal_rtl8371c_svlan_sp2c_get
 * Description:
 *      Get configure system SP2C content
 *  Input:
 *      unit        - Unit ID
 *      svid        - SVLAN VID
 *      dst_port    - Destination port of SVLAN to CVLAN configuration
 * Output:
 *      pCvid - VLAN ID
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_OUT_OF_RANGE - input out of range.
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_SVLAN_VID    - Invalid SVLAN VID parameter.
 * Note:
 *     The API can get SVID & Destination Port to CVLAN configuration. There are 128 SP2C configurations.
 */
rtksw_api_ret_t dal_rtl8371c_svlan_sp2c_get(rtksw_uint32 unit, rtksw_vlan_t svid, rtksw_port_t dst_port, rtksw_vlan_t *pCvid)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 i;
    rtksw_uint32 idx_svid, idx_port;
    rtksw_uint32 valid_flag;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(NULL == pCvid)
        return RT_ERR_NULL_POINTER;

    if (svid > RTL8371C_VIDMAX)
        return RT_ERR_SVLAN_VID;

    /* Check port Valid */
    RTKSW_CHK_PORT_VALID(unit, dst_port);
    dst_port = rtksw_switch_port_L2P_get(unit, dst_port);

    for (i = 0; i <= RTL8371C_SP2CMAX; i++)
    {
        if ((retVal = reg16_array_field_read(unit, RTL8371C_VS_SP2Cr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)i, RTL8371C_SVIDf, &idx_svid)) != RT_ERR_OK)
            return retVal;

        if ((retVal = reg16_array_field_read(unit, RTL8371C_VS_SP2Cr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)i, RTL8371C_DST_PORTf, &idx_port)) != RT_ERR_OK)
            return retVal;

        if ((retVal = reg16_array_field_read(unit, RTL8371C_VS_SP2Cr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)i, RTL8371C_VALIDf, &valid_flag)) != RT_ERR_OK)
            return retVal;

        if ( (idx_svid == svid) && (idx_port == dst_port) && (valid_flag == 1))
        {
             if ((retVal = reg16_array_field_read(unit, RTL8371C_VS_SP2Cr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)i, RTL8371C_VIDf, (rtksw_uint32 *)pCvid)) != RT_ERR_OK)
                return retVal;

            return RT_ERR_OK;
        }
    }

    return RT_ERR_OUT_OF_RANGE;
}

/* Function Name:
 *      dal_rtl8371c_svlan_sp2c_del
 * Description:
 *      Delete system SP2C configuration
 *  Input:
 *      unit        - Unit ID
 *      svid        - SVLAN VID
 *      dst_port    - Destination port of SVLAN to CVLAN configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_SVLAN_VID    - Invalid SVLAN VID parameter.
 *      RT_ERR_OUT_OF_RANGE - input out of range.
 * Note:
 *      The API can delete SVID & Destination Port to CVLAN configuration. There are 128 SP2C configurations.
 */
rtksw_api_ret_t dal_rtl8371c_svlan_sp2c_del(rtksw_uint32 unit, rtksw_vlan_t svid, rtksw_port_t dst_port)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 i;
    rtksw_uint32 idx_svid, idx_port;
    rtksw_uint32 valid_flag, idx_cvid;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (svid > RTL8371C_VIDMAX)
        return RT_ERR_SVLAN_VID;

    /* Check port Valid */
    RTKSW_CHK_PORT_VALID(unit, dst_port);
    dst_port = rtksw_switch_port_L2P_get(unit, dst_port);

    for (i = 0; i <= RTL8371C_SP2CMAX; i++)
    {
        if ((retVal = reg16_array_field_read(unit, RTL8371C_VS_SP2Cr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)i, RTL8371C_SVIDf, &idx_svid)) != RT_ERR_OK)
            return retVal;

        if ((retVal = reg16_array_field_read(unit, RTL8371C_VS_SP2Cr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)i, RTL8371C_DST_PORTf, &idx_port)) != RT_ERR_OK)
            return retVal;

        if ((retVal = reg16_array_field_read(unit, RTL8371C_VS_SP2Cr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)i, RTL8371C_VALIDf, &valid_flag)) != RT_ERR_OK)
            return retVal;

        if ( (idx_svid == svid) && (idx_port == dst_port) && (valid_flag == 1))
        {
            idx_svid = 0;
            idx_port = 0;
            idx_cvid = 0;
            valid_flag = 0;
            if ((retVal = reg16_array_field_write(unit, RTL8371C_VS_SP2Cr, REG_ARRAY_INDEX_NONE, i, RTL8371C_VALIDf, &valid_flag)) != RT_ERR_OK)
                return retVal;

            if ((retVal = reg16_array_field_write(unit, RTL8371C_VS_SP2Cr, REG_ARRAY_INDEX_NONE, i, RTL8371C_SVIDf, &idx_svid)) != RT_ERR_OK)
                return retVal;

            if ((retVal = reg16_array_field_write(unit, RTL8371C_VS_SP2Cr, REG_ARRAY_INDEX_NONE, i, RTL8371C_DST_PORTf, &idx_port)) != RT_ERR_OK)
                return retVal;

            if ((retVal = reg16_array_field_write(unit, RTL8371C_VS_SP2Cr, REG_ARRAY_INDEX_NONE, i, RTL8371C_VIDf, &idx_cvid)) != RT_ERR_OK)
                return retVal;

            return RT_ERR_OK;
        }
    }

    return RT_ERR_OUT_OF_RANGE;
}

/* Function Name:
 *      dal_rtl8371c_svlanuntag_action_set
 * Description:
 *      Configure Action of downstream Un-Stag packet
 *  Input:
 *      unit        - Unit ID
 *      action      - Action for UnStag
 *      svid        - The SVID assigned to UnStag packet
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                       - OK
 *      RT_ERR_FAILED                   - Failed
 *      RT_ERR_SMI                      - SMI access error
 *      RT_ERR_SVLAN_VID                - Invalid SVLAN VID parameter.
 *      RT_ERR_SVLAN_ENTRY_NOT_FOUND    - specified svlan entry not found.
 *      RT_ERR_OUT_OF_RANGE             - input out of range.
 *      RT_ERR_INPUT                    - Invalid input parameters.
 * Note:
 *      The API can configure action of downstream Un-Stag packet. A SVID assigned
 *      to the un-stag is also supported by this API. The parameter of svid is
 *      only referenced when the action is set to UNTAG_ASSIGN
 */
rtksw_api_ret_t dal_rtl8371c_svlanuntag_action_set(rtksw_uint32 unit, rtksw_svlan_untag_action_t action, rtksw_vlan_t svid)
{
    rtksw_api_ret_t   retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (action >= UNTAG_END)
        return RT_ERR_OUT_OF_RANGE;

    if(action == UNTAG_ASSIGN)
    {
        if (svid > RTL8371C_VIDMAX)
            return RT_ERR_SVLAN_VID;
    }

    if ((retVal = reg16_field_write(unit, RTL8371C_VS_CFGr, RTL8371C_VS_UNTAGf, (rtksw_uint32 *)&action)) != RT_ERR_OK)
        return retVal;

    if(action == UNTAG_ASSIGN)
    {
        if ((retVal = reg16_field_write(unit, RTL8371C_VS_UN_IDXr, RTL8371C_VS_UNTAG_SVIDf, (rtksw_uint32 *)&svid)) != RT_ERR_OK)
            return retVal;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_svlanuntag_action_get
 * Description:
 *      Get Action of downstream Un-Stag packet
 *  Input:
 *      unit        - Unit ID
 * Output:
 *      pAction  - Action for UnStag
 *      pSvid    - The SVID assigned to UnStag packet
 * Return:
 *      RT_ERR_OK                       - OK
 *      RT_ERR_FAILED                   - Failed
 *      RT_ERR_SMI                      - SMI access error
 *      RT_ERR_SVLAN_VID                - Invalid SVLAN VID parameter.
 *      RT_ERR_SVLAN_ENTRY_NOT_FOUND    - specified svlan entry not found.
 *      RT_ERR_OUT_OF_RANGE             - input out of range.
 *      RT_ERR_INPUT                    - Invalid input parameters.
 * Note:
 *      The API can Get action of downstream Un-Stag packet. A SVID assigned
 *      to the un-stag is also retrieved by this API. The parameter pSvid is
 *      only refernced when the action is UNTAG_ASSIGN
 */
rtksw_api_ret_t dal_rtl8371c_svlanuntag_action_get(rtksw_uint32 unit, rtksw_svlan_untag_action_t *pAction, rtksw_vlan_t *pSvid)
{
    rtksw_api_ret_t   retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(NULL == pAction || NULL == pSvid)
        return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_field_read(unit, RTL8371C_VS_CFGr, RTL8371C_VS_UNTAGf, (rtksw_uint32 *)pAction)) != RT_ERR_OK)
        return retVal;

    if(*pAction == UNTAG_ASSIGN)
    {
        if ((retVal = reg16_field_write(unit, RTL8371C_VS_UN_IDXr, RTL8371C_VS_UNTAG_SVIDf, (rtksw_uint32 *)pSvid)) != RT_ERR_OK)
            return retVal;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_svlantrapPri_set
 * Description:
 *      Set svlan trap priority
 *  Input:
 *      unit        - Unit ID
 *      priority    - priority for trap packets
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QOS_INT_PRIORITY
 * Note:
 *      None
 */
rtksw_api_ret_t dal_rtl8371c_svlantrapPri_set(rtksw_uint32 unit, rtksw_pri_t priority)
{
    rtksw_api_ret_t   retVal;

    RTKSW_CHK_INIT_STATE(unit);

    if(priority > RTL8371C_PRIMAX)
        return RT_ERR_OUT_OF_RANGE;

    if ((retVal = reg16_field_write(unit, RTL8371C_PRI_TRAPr, RTL8371C_SVLAN_PRIOIRTYf, (rtksw_uint32 *)&priority)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_svlantrapPri_get
 * Description:
 *      Get svlan trap priority
 *  Input:
 *      unit        - Unit ID
 * Output:
 *      pPriority - priority for trap packets
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
rtksw_api_ret_t dal_rtl8371c_svlantrapPri_get(rtksw_uint32 unit, rtksw_pri_t *pPriority)
{
    rtksw_api_ret_t   retVal;

    RTKSW_CHK_INIT_STATE(unit);

    if(NULL == pPriority)
        return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_field_read(unit, RTL8371C_PRI_TRAPr, RTL8371C_SVLAN_PRIOIRTYf, (rtksw_uint32 *)pPriority)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}   /* end of rtksw_svlan_trapPri_get */

/* Function Name:
 *      dal_rtl8371c_svlanunassign_action_set
 * Description:
 *      Configure Action of upstream without svid assign action
 *  Input:
 *      unit        - Unit ID
 *      action      - Action for Un-assign
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                       - OK
 *      RT_ERR_FAILED                   - Failed
 *      RT_ERR_OUT_OF_RANGE             - input out of range.
 *      RT_ERR_INPUT                    - Invalid input parameters.
 * Note:
 *      The API can configure action of upstream Un-assign svid packet. If action is not
 *      trap to CPU, the port-based SVID sure be assign as system need
 */
rtksw_api_ret_t dal_rtl8371c_svlanunassign_action_set(rtksw_uint32 unit, rtksw_svlan_unassign_action_t action)
{
    rtksw_api_ret_t   retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (action >= UNASSIGN_END)
        return RT_ERR_OUT_OF_RANGE;

    if ((retVal = reg16_field_write(unit, RTL8371C_VS_CFGr, RTL8371C_VS_UIFSEGf, (rtksw_uint32 *)&action)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_svlanunassign_action_get
 * Description:
 *      Get action of upstream without svid assignment
 *  Input:
 *      unit        - Unit ID
 * Output:
 *      pAction  - Action for Un-assign
 * Return:
 *      RT_ERR_OK                       - OK
 *      RT_ERR_FAILED                   - Failed
 * Note:
 *      None
 */
rtksw_api_ret_t dal_rtl8371c_svlanunassign_action_get(rtksw_uint32 unit, rtksw_svlan_unassign_action_t *pAction)
{
    rtksw_api_ret_t   retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(NULL == pAction)
        return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_field_read(unit, RTL8371C_VS_CFGr, RTL8371C_VS_UIFSEGf, (rtksw_uint32 *)pAction)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}
