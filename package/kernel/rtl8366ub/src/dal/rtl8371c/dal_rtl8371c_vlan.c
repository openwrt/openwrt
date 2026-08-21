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
#include <dal/rtl8371c/dal_rtl8371c_vlan.h>
#include <dal/reg.h>
#include <dal/rtl8371c/rtk_rtl8371c_reg_struct.h>
#include <dal/rtl8371c/rtk_rtl8371c_table_struct.h>

#include <dal/dal_mgmts.h>
#include <osal/lib.h>

#if defined(CONFIG_VIRTUAL_REG_ASICDRV_TEST)
rtksw_vlan_cfg_t Rtl8371cVirtualVlanTable[RTL8371C_VIDMAX + 1];
#endif

#if 0
static void _dal_rtl8371c_Vlan4kStUser2Smi(dal_rtl8371c_user_vlan4kentry *pUserVlan4kEntry, rtksw_uint16 *pSmiVlan4kEntry)
{
    pSmiVlan4kEntry[0] |= (pUserVlan4kEntry->mbr & 0x00FF);
    pSmiVlan4kEntry[0] |= (pUserVlan4kEntry->untag & 0x00FF) << 8;

    pSmiVlan4kEntry[1] |= (pUserVlan4kEntry->fid_msti & 0x0007);
    pSmiVlan4kEntry[1] |= (pUserVlan4kEntry->svlan_chk_ivl_svl & 0x0001) << 3;
    pSmiVlan4kEntry[1] |= (pUserVlan4kEntry->ivl_svl & 0x0001) << 4;
}


static void _dal_rtl8371c_Vlan4kStSmi2User(rtksw_uint16 *pSmiVlan4kEntry, dal_rtl8371c_user_vlan4kentry *pUserVlan4kEntry)
{
    pUserVlan4kEntry->mbr               = (pSmiVlan4kEntry[0] & 0x00FF);
    pUserVlan4kEntry->untag             = (pSmiVlan4kEntry[0] & 0xFF00) >> 8;

    pUserVlan4kEntry->fid_msti          = (pSmiVlan4kEntry[1] & 0x0007);
    pUserVlan4kEntry->svlan_chk_ivl_svl = (pSmiVlan4kEntry[1] & 0x0008) >> 3;
    pUserVlan4kEntry->ivl_svl           = (pSmiVlan4kEntry[1] & 0x0010) >> 4;
}
#endif

static rtksw_api_ret_t _dal_rtl8371c_setAsicVlan4kEntry(rtksw_uint32 unit, rtksw_vlan_t vid, rtksw_vlan_cfg_t *pVlanCfg)
{
    rtksw_api_ret_t retVal;
    rtl8371c_vlan_entry_t vlanEntry;
    rtksw_uint32 phyMbrPmask;
    rtksw_uint32 phyUntagPmask;
    rtksw_uint32 fieldData;

    osal_memset(&vlanEntry, 0, sizeof(rtl8371c_vlan_entry_t));

    /* MBR */
    if(rtksw_switch_portmask_L2P_get(unit, &(pVlanCfg->mbr), &phyMbrPmask) != RT_ERR_OK)
        return RT_ERR_FAILED;
    
    if ((retVal = table16_field_set(unit, RTL8371C_VLAN_TBLt, RTL8371C_VLAN_TBL_MBRtf, &phyMbrPmask, (rtksw_uint32 *)&vlanEntry)) != RT_ERR_OK)
        return retVal;

    /* UNTAG */
    if(rtksw_switch_portmask_L2P_get(unit, &(pVlanCfg->untag), &phyUntagPmask) != RT_ERR_OK)
        return RT_ERR_FAILED;

    if ((retVal = table16_field_set(unit, RTL8371C_VLAN_TBLt, RTL8371C_VLAN_TBL_UNTAGSETtf, &phyUntagPmask, (rtksw_uint32 *)&vlanEntry)) != RT_ERR_OK)
        return retVal;

    /* SVL_IVL */
    fieldData = pVlanCfg->ivl_en;
    if ((retVal = table16_field_set(unit, RTL8371C_VLAN_TBLt, RTL8371C_VLAN_TBL_IVL_SVLtf, &fieldData, (rtksw_uint32 *)&vlanEntry)) != RT_ERR_OK)
        return retVal;

    /* FID */
    fieldData = pVlanCfg->fid_msti;
    if ((retVal = table16_field_set(unit, RTL8371C_VLAN_TBLt, RTL8371C_VLAN_TBL_FID_MSTItf, &fieldData, (rtksw_uint32 *)&vlanEntry)) != RT_ERR_OK)
        return retVal;

    if ((retVal = table16_write(unit, RTL8371C_VLAN_TBLt, (rtksw_uint32)vid, (rtksw_uint32 *)&vlanEntry)) != RT_ERR_OK)
        return retVal;

#if defined(CONFIG_VIRTUAL_REG_ASICDRV_TEST)
    osal_memcpy(&Rtl8371cVirtualVlanTable[vid], pVlanCfg, sizeof(rtksw_vlan_cfg_t));
#endif

    return RT_ERR_OK;
}


static rtksw_api_ret_t _dal_rtl8371c_getAsicVlan4kEntry(rtksw_uint32 unit, rtksw_vlan_t vid, rtksw_vlan_cfg_t *pVlanCfg)
{
    rtksw_api_ret_t retVal;
    rtl8371c_vlan_entry_t vlanEntry;
    rtksw_uint32 fieldData;

    osal_memset(&vlanEntry, 0x00, sizeof(rtl8371c_vlan_entry_t));
    if ((retVal = table16_read(unit, RTL8371C_VLAN_TBLt, (rtksw_uint32)vid, (rtksw_uint32 *)&vlanEntry)) != RT_ERR_OK)
        return retVal;

    osal_memset(pVlanCfg, 0x00, sizeof(rtksw_vlan_cfg_t));

    /* MBR */
    if ((retVal = table16_field_get(unit, RTL8371C_VLAN_TBLt, RTL8371C_VLAN_TBL_MBRtf, &fieldData, (rtksw_uint32 *)&vlanEntry)) != RT_ERR_OK)
        return retVal;
    if ((retVal = rtksw_switch_portmask_P2L_get(unit, fieldData, &(pVlanCfg->mbr))) != RT_ERR_OK)
        return RT_ERR_FAILED;

    /* UNTAG */
    if ((retVal = table16_field_get(unit, RTL8371C_VLAN_TBLt, RTL8371C_VLAN_TBL_UNTAGSETtf, &fieldData, (rtksw_uint32 *)&vlanEntry)) != RT_ERR_OK)
        return retVal;
    if ((retVal = rtksw_switch_portmask_P2L_get(unit, fieldData, &(pVlanCfg->untag))) != RT_ERR_OK)
        return RT_ERR_FAILED;

    /* SVL_IVL */
    if ((retVal = table16_field_get(unit, RTL8371C_VLAN_TBLt, RTL8371C_VLAN_TBL_IVL_SVLtf, &fieldData, (rtksw_uint32 *)&vlanEntry)) != RT_ERR_OK)
        return retVal;

    pVlanCfg->ivl_en = fieldData;

    /* FID */
    if ((retVal = table16_field_get(unit, RTL8371C_VLAN_TBLt, RTL8371C_VLAN_TBL_FID_MSTItf, &fieldData, (rtksw_uint32 *)&vlanEntry)) != RT_ERR_OK)
        return retVal;

    pVlanCfg->fid_msti = fieldData;

#if defined(CONFIG_VIRTUAL_REG_ASICDRV_TEST)
    osal_memcpy(pVlanCfg, &Rtl8371cVirtualVlanTable[vid], sizeof(rtksw_vlan_cfg_t));
#endif

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_vlan_init
 * Description:
 *      Initialize VLAN.
 *  Input:
 *      unit        - Unit ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      VLAN is disabled by default. User has to call this API to enable VLAN before
 *      using it. And It will set a default VLAN(vid 1) including all ports and set
 *      all ports PVID to the default VLAN.
 */
rtksw_api_ret_t dal_rtl8371c_vlan_init(rtksw_uint32 unit)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 i;
    rtksw_vlan_cfg_t vlanCfg;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Set a default VLAN with vid 1 to 4K table for all ports */
    osal_memset(&vlanCfg, 0, sizeof(rtksw_vlan_cfg_t));
    RTKSW_PORTMASK_ALLPORT_SET(unit, vlanCfg.mbr);
    RTKSW_PORTMASK_ALLPORT_SET(unit, vlanCfg.untag);
    if ((retVal = _dal_rtl8371c_setAsicVlan4kEntry(unit, 1, &vlanCfg)) != RT_ERR_OK)
        return retVal;

    /* Set all ports PVID to default VLAN and tag-mode to original */
    RTKSW_SCAN_ALL_LOG_PORT(unit, i)
    {
        if ((retVal = dal_rtl8371c_vlan_portPvid_set(unit, i, 1, 0)) != RT_ERR_OK)
            return retVal;
        if ((retVal = dal_rtl8371c_vlan_tagMode_set(unit, i, RTKSW_VLAN_TAG_MODE_ORIGINAL)) != RT_ERR_OK)
            return retVal;
    }

    /* Enable Ingress filter */
    RTKSW_SCAN_ALL_LOG_PORT(unit, i)
    {
        if ((retVal = dal_rtl8371c_vlan_portIgrFilterEnable_set(unit, i, RTKSW_ENABLED)) != RT_ERR_OK)
            return retVal;
    }

    /* enable VLAN */
    if ((retVal = dal_rtl8371c_vlan_egrFilterEnable_set(unit, RTKSW_ENABLED)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_vlan_set
 * Description:
 *      Set a VLAN entry.
 *  Input:
 *      unit        - Unit ID
 *      vid         - VLAN ID to configure.
 *      pVlanCfg    - VLAN Configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_INPUT                - Invalid input parameters.
 *      RT_ERR_L2_FID               - Invalid FID.
 *      RT_ERR_VLAN_PORT_MBR_EXIST  - Invalid member port mask.
 *      RT_ERR_VLAN_VID             - Invalid VID parameter.
 * Note:
 *
 */
rtksw_api_ret_t dal_rtl8371c_vlan_set(rtksw_uint32 unit, rtksw_vlan_t vid, rtksw_vlan_cfg_t *pVlanCfg)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* vid must be 0~4095 */
    if (vid > RTL8371C_VIDMAX)
        return RT_ERR_VLAN_VID;

    /* Null pointer check */
    if(NULL == pVlanCfg)
        return RT_ERR_NULL_POINTER;

    /* Check port mask valid */
    RTKSW_CHK_PORTMASK_VALID(unit, &(pVlanCfg->mbr));

    /* Check untag port mask valid */
    RTKSW_CHK_PORTMASK_VALID(unit, &(pVlanCfg->untag));

    /* IVL_EN */
    if(pVlanCfg->ivl_en >= RTKSW_ENABLE_END)
        return RT_ERR_ENABLE;

    /* fid must be 0~3 */
    if(pVlanCfg->fid_msti > RTL8371C_FIDMAX)
        return RT_ERR_L2_FID;

    /* Policing,  Meter ID ,  VLAN based priority ar not supported in RTL8371C*/
    if ((pVlanCfg->envlanpol != 0) || (pVlanCfg->meteridx != 0) || (pVlanCfg->vbpen != 0) || (pVlanCfg->vbpri != 0))
        return RT_ERR_INPUT;

    if ((retVal = _dal_rtl8371c_setAsicVlan4kEntry(unit, vid, pVlanCfg)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_vlan_get
 * Description:
 *      Get a VLAN entry.
 *  Input:
 *      unit        - Unit ID
 *      vid         - VLAN ID to configure.
 * Output:
 *      pVlanCfg - VLAN Configuration
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_VLAN_VID     - Invalid VID parameter.
 * Note:
 *
 */
rtksw_api_ret_t dal_rtl8371c_vlan_get(rtksw_uint32 unit, rtksw_vlan_t vid, rtksw_vlan_cfg_t *pVlanCfg)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* vid must be 0~4095 */
    if (vid > RTL8371C_VIDMAX)
        return RT_ERR_VLAN_VID;

    /* Null pointer check */
    if(NULL == pVlanCfg)
        return RT_ERR_NULL_POINTER;

    if ((retVal = _dal_rtl8371c_getAsicVlan4kEntry(unit, (rtksw_uint32)vid, pVlanCfg)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_vlan_egrFilterEnable_set
 * Description:
 *      Set VLAN egress filter.
 *  Input:
 *      unit        - Unit ID
 *      egrFilter   - Egress filtering
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_ENABLE       - Invalid input parameters.
 * Note:
 *
 */
rtksw_api_ret_t dal_rtl8371c_vlan_egrFilterEnable_set(rtksw_uint32 unit, rtksw_enable_t egrFilter)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(egrFilter >= RTKSW_ENABLE_END)
        return RT_ERR_ENABLE;

    /* enable VLAN */
    if ((retVal = reg16_field_write(unit, RTL8371C_VLAN_CTRLr, RTL8371C_VLAN_FILTERINGf, (rtksw_uint32 *)&egrFilter)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_vlan_egrFilterEnable_get
 * Description:
 *      Get VLAN egress filter.
 *  Input:
 *      unit        - Unit ID
 *      pEgrFilter  - Egress filtering
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_NULL_POINTER - NULL Pointer.
 * Note:
 *
 */
rtksw_api_ret_t dal_rtl8371c_vlan_egrFilterEnable_get(rtksw_uint32 unit, rtksw_enable_t *pEgrFilter)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 state;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(NULL == pEgrFilter)
        return RT_ERR_NULL_POINTER;

    /* enable VLAN */
    if ((retVal = reg16_field_read(unit, RTL8371C_VLAN_CTRLr, RTL8371C_VLAN_FILTERINGf, (rtksw_uint32 *)&state)) != RT_ERR_OK)
        return retVal;

    *pEgrFilter = (rtksw_enable_t)state;
    return RT_ERR_OK;
}


/* Function Name:
 *     dal_rtl8371c_vlan_portPvid_set
 * Description:
 *      Set port to specified VLAN ID(PVID).
 *  Input:
 *      unit        - Unit ID
 *      port        - Port id.
 *      pvid        - Specified VLAN ID.
 *      priority    - 802.1p priority for the PVID.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_PORT_ID              - Invalid port number.
 *      RT_ERR_VLAN_PRIORITY        - Invalid priority.
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - VLAN entry not found.
 *      RT_ERR_VLAN_VID             - Invalid VID parameter.
 * Note:
 *       The API is used for Port-based VLAN. The untagged frame received from the
 *       port will be classified to the specified VLAN and assigned to the specified priority.
 */
rtksw_api_ret_t dal_rtl8371c_vlan_portPvid_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_vlan_t pvid, rtksw_pri_t priority)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    /* vid must be 0~8191 */
    if (pvid > RTL8371C_VIDMAX)
        return RT_ERR_VLAN_VID;

    /* priority is not supported in RTL8367D */
    if (priority > 0)
        return RT_ERR_INPUT;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_VLAN_PORT_PB_VLANr, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_PVIDf, (rtksw_uint32 *)&pvid)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_vlan_portPvid_get
 * Description:
 *      Get VLAN ID(PVID) on specified port.
 *  Input:
 *      unit        - Unit ID
 *      port        - Port id.
 * Output:
 *      pPvid - Specified VLAN ID.
 *      pPriority - 802.1p priority for the PVID.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *     The API can get the PVID and 802.1p priority for the PVID of Port-based VLAN.
 */
rtksw_api_ret_t dal_rtl8371c_vlan_portPvid_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_vlan_t *pPvid, rtksw_pri_t *pPriority)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if(NULL == pPvid)
        return RT_ERR_NULL_POINTER;

    if(NULL == pPriority)
        return RT_ERR_NULL_POINTER;

    *pPriority = 0;

    if ((retVal = reg16_array_field_read(unit, RTL8371C_VLAN_PORT_PB_VLANr, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_PVIDf, (rtksw_uint32 *)pPvid)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_vlan_portIgrFilterEnable_set
 * Description:
 *      Set VLAN ingress for each port.
 *  Input:
 *      unit        - Unit ID
 *      port        - Port id.
 *      igr_filter  - VLAN ingress function enable status.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number
 *      RT_ERR_ENABLE       - Invalid enable input
 * Note:
 *      The status of vlan ingress filter is as following:
 *      - RTKSW_DISABLED
 *      - RTKSW_ENABLED
 *      While VLAN function is enabled, ASIC will decide VLAN ID for each received frame and get belonged member
 *      ports from VLAN table. If received port is not belonged to VLAN member ports, ASIC will drop received frame if VLAN ingress function is enabled.
 */
rtksw_api_ret_t dal_rtl8371c_vlan_portIgrFilterEnable_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t igr_filter)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if (igr_filter >= RTKSW_ENABLE_END)
        return RT_ERR_ENABLE;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_CVLAN_INGRESS_FILTERr, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_VLAN_INGRESSf, (rtksw_uint32 *)&igr_filter)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_vlan_portIgrFilterEnable_get
 * Description:
 *      Get VLAN Ingress Filter
 *  Input:
 *      unit        - Unit ID
 *      port        - Port id.
 * Output:
 *      pIgr_filter - VLAN ingress function enable status.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *     The API can Get the VLAN ingress filter status.
 *     The status of vlan ingress filter is as following:
 *     - RTKSW_DISABLED
 *     - RTKSW_ENABLED
 */
rtksw_api_ret_t dal_rtl8371c_vlan_portIgrFilterEnable_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t *pIgr_filter)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if(NULL == pIgr_filter)
        return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_array_field_read(unit, RTL8371C_CVLAN_INGRESS_FILTERr, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_VLAN_INGRESSf, (rtksw_uint32 *)pIgr_filter)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_vlan_portAcceptFrameType_set
 * Description:
 *      Set VLAN accept_frame_type
 *  Input:
 *      unit                - Unit ID
 *      port                - Port id.
 *      accept_frame_type   - accept frame type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                       - OK
 *      RT_ERR_FAILED                   - Failed
 *      RT_ERR_SMI                      - SMI access error
 *      RT_ERR_PORT_ID                  - Invalid port number.
 *      RT_ERR_VLAN_ACCEPT_FRAME_TYPE   - Invalid frame type.
 * Note:
 *      The API is used for checking 802.1Q tagged frames.
 *      The accept frame type as following:
 *      - RTKSW_ACCEPT_FRAME_TYPE_ALL
 *      - RTKSW_ACCEPT_FRAME_TYPE_TAG_ONLY
 *      - RTKSW_ACCEPT_FRAME_TYPE_UNTAG_ONLY
 */
rtksw_api_ret_t dal_rtl8371c_vlan_portAcceptFrameType_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_vlan_acceptFrameType_t accept_frame_type)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if (accept_frame_type >= RTKSW_ACCEPT_FRAME_TYPE_END)
        return RT_ERR_VLAN_ACCEPT_FRAME_TYPE;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_VLAN_PORT_AFTr, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_CTAG_ACCEPT_TYPEf, (rtksw_uint32 *)&accept_frame_type)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_vlan_portAcceptFrameType_get
 * Description:
 *      Get VLAN accept_frame_type
 *  Input:
 *      unit        - Unit ID
 *      port        - Port id.
 * Output:
 *      pAccept_frame_type - accept frame type
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *     The API can Get the VLAN ingress filter.
 *     The accept frame type as following:
 *     - RTKSW_ACCEPT_FRAME_TYPE_ALL
 *     - RTKSW_ACCEPT_FRAME_TYPE_TAG_ONLY
 *     - RTKSW_ACCEPT_FRAME_TYPE_UNTAG_ONLY
 */
rtksw_api_ret_t dal_rtl8371c_vlan_portAcceptFrameType_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_vlan_acceptFrameType_t *pAccept_frame_type)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 type;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if(NULL == pAccept_frame_type)
        return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_array_field_read(unit, RTL8371C_VLAN_PORT_AFTr, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_CTAG_ACCEPT_TYPEf, (rtksw_uint32 *)&type)) != RT_ERR_OK)
        return retVal;

    *pAccept_frame_type = (rtksw_vlan_acceptFrameType_t)type;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_vlan_tagMode_set
 * Description:
 *      Set CVLAN egress tag mode
 *  Input:
 *      unit        - Unit ID
 *      port        - Port id.
 *      tag_mode    - The egress tag mode.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_INPUT        - Invalid input parameter.
 *      RT_ERR_ENABLE       - Invalid enable input.
 * Note:
 *      The API can set Egress tag mode. There are 4 mode for egress tag:
 *      - RTKSW_VLAN_TAG_MODE_ORIGINAL,
 *      - RTKSW_VLAN_TAG_MODE_KEEP_FORMAT,
 *      - RTKSW_VLAN_TAG_MODE_PRI.
 *      - RTKSW_VLAN_TAG_MODE_REAL_KEEP_FORMAT,
 */
rtksw_api_ret_t dal_rtl8371c_vlan_tagMode_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_vlan_tagMode_t tag_mode)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if (tag_mode >= RTKSW_VLAN_TAG_MODE_END)
        return RT_ERR_PORT_ID;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_VLAN_PORT_EGR_TAGr, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_VLAN_EGRESS_MODEf, (rtksw_uint32 *)&tag_mode)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_vlan_tagMode_get
 * Description:
 *      Get CVLAN egress tag mode
 *  Input:
 *      unit        - Unit ID
 *      port        - Port id.
 * Output:
 *      pTag_mode - The egress tag mode.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      The API can get Egress tag mode. There are 4 mode for egress tag:
 *      - RTKSW_VLAN_TAG_MODE_ORIGINAL,
 *      - RTKSW_VLAN_TAG_MODE_KEEP_FORMAT,
 *      - RTKSW_VLAN_TAG_MODE_PRI.
 *      - RTKSW_VLAN_TAG_MODE_REAL_KEEP_FORMAT,
 */
rtksw_api_ret_t dal_rtl8371c_vlan_tagMode_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_vlan_tagMode_t *pTag_mode)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32  mode;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if(NULL == pTag_mode)
        return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_array_field_read(unit, RTL8371C_VLAN_PORT_EGR_TAGr, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_VLAN_EGRESS_MODEf, (rtksw_uint32 *)&mode)) != RT_ERR_OK)
        return retVal;

    *pTag_mode = (rtksw_vlan_tagMode_t)mode;
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_vlan_transparent_set
 * Description:
 *      Set VLAN transparent mode
 *  Input:
 *      unit            - Unit ID
 *      egr_port        - Egress Port id.
 *      pIgr_pmask      - Ingress Port Mask.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      None.
 */
rtksw_api_ret_t dal_rtl8371c_vlan_transparent_set(rtksw_uint32 unit, rtksw_port_t egr_port, rtksw_portmask_t *pIgr_pmask)
{
     rtksw_api_ret_t retVal;
     rtksw_uint32    pmask;

     /* Check initialization state */
     RTKSW_CHK_INIT_STATE(unit);

     /* Check Port Valid */
     RTKSW_CHK_PORT_VALID(unit, egr_port);

     if(NULL == pIgr_pmask)
        return RT_ERR_NULL_POINTER;

     RTKSW_CHK_PORTMASK_VALID(unit, pIgr_pmask);

     if(rtksw_switch_portmask_L2P_get(unit, pIgr_pmask, &pmask) != RT_ERR_OK)
        return RT_ERR_FAILED;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_VLAN_PORT_EGR_TRANSr, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, egr_port), REG_ARRAY_INDEX_NONE, RTL8371C_PMSKf, (rtksw_uint32 *)&pmask)) != RT_ERR_OK)
        return retVal;

     return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_vlan_transparent_get
 * Description:
 *      Get VLAN transparent mode
 *  Input:
 *      unit            - Unit ID
 *      egr_port        - Egress Port id.
 * Output:
 *      pIgr_pmask      - Ingress Port Mask
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      None.
 */
rtksw_api_ret_t dal_rtl8371c_vlan_transparent_get(rtksw_uint32 unit, rtksw_port_t egr_port, rtksw_portmask_t *pIgr_pmask)
{
     rtksw_api_ret_t retVal;
     rtksw_uint32    pmask;

     /* Check initialization state */
     RTKSW_CHK_INIT_STATE(unit);

     /* Check Port Valid */
     RTKSW_CHK_PORT_VALID(unit, egr_port);

     if(NULL == pIgr_pmask)
        return RT_ERR_NULL_POINTER;

     if ((retVal = reg16_array_field_read(unit, RTL8371C_VLAN_PORT_EGR_TRANSr, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, egr_port), REG_ARRAY_INDEX_NONE, RTL8371C_PMSKf, (rtksw_uint32 *)&pmask)) != RT_ERR_OK)
        return retVal;

     if(rtksw_switch_portmask_P2L_get(unit, pmask, pIgr_pmask) != RT_ERR_OK)
        return RT_ERR_FAILED;

     return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_vlan_keep_set
 * Description:
 *      Set VLAN egress keep mode
 *  Input:
 *      unit            - Unit ID
 *      egr_port        - Egress Port id.
 *      pIgr_pmask      - Ingress Port Mask.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      None.
 */
rtksw_api_ret_t dal_rtl8371c_vlan_keep_set(rtksw_uint32 unit, rtksw_port_t egr_port, rtksw_portmask_t *pIgr_pmask)
{
     rtksw_api_ret_t retVal;
     rtksw_uint32    pmask;

     /* Check initialization state */
     RTKSW_CHK_INIT_STATE(unit);

     /* Check Port Valid */
     RTKSW_CHK_PORT_VALID(unit, egr_port);

     if(NULL == pIgr_pmask)
        return RT_ERR_NULL_POINTER;

     RTKSW_CHK_PORTMASK_VALID(unit, pIgr_pmask);

     if(rtksw_switch_portmask_L2P_get(unit, pIgr_pmask, &pmask) != RT_ERR_OK)
        return RT_ERR_FAILED;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_VLAN_PORT_EGR_KEEPr, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, egr_port), REG_ARRAY_INDEX_NONE, RTL8371C_PMSKf, (rtksw_uint32 *)&pmask)) != RT_ERR_OK)
        return retVal;

     return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_vlan_keep_get
 * Description:
 *      Get VLAN egress keep mode
 *  Input:
 *      unit            - Unit ID
 *      egr_port        - Egress Port id.
 * Output:
 *      pIgr_pmask      - Ingress Port Mask
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      None.
 */
rtksw_api_ret_t dal_rtl8371c_vlan_keep_get(rtksw_uint32 unit, rtksw_port_t egr_port, rtksw_portmask_t *pIgr_pmask)
{
     rtksw_api_ret_t retVal;
     rtksw_uint32    pmask;

     /* Check initialization state */
     RTKSW_CHK_INIT_STATE(unit);

     /* Check Port Valid */
     RTKSW_CHK_PORT_VALID(unit, egr_port);

     if(NULL == pIgr_pmask)
        return RT_ERR_NULL_POINTER;

     if ((retVal = reg16_array_field_read(unit, RTL8371C_VLAN_PORT_EGR_KEEPr, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, egr_port), REG_ARRAY_INDEX_NONE, RTL8371C_PMSKf, (rtksw_uint32 *)&pmask)) != RT_ERR_OK)
        return retVal;

     if(rtksw_switch_portmask_P2L_get(unit, pmask, pIgr_pmask) != RT_ERR_OK)
        return RT_ERR_FAILED;

     return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_vlan_portFid_set
 * Description:
 *      Set port-based filtering database
 *  Input:
 *      unit        - Unit ID
 *      port        - Port id.
 *      enable      - ebable port-based FID
 *      fid         - Specified filtering database.
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
 *      The API can set port-based filtering database. If the function is enabled, all input
 *      packets will be assigned to the port-based fid regardless vlan tag.
 */
rtksw_api_ret_t dal_rtl8371c_vlan_portFid_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t enable, rtksw_fid_t fid)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if (enable>=RTKSW_ENABLE_END)
        return RT_ERR_ENABLE;

    /* fid must be 0~3 */
    if (fid > RTL8371C_FIDMAX)
        return RT_ERR_L2_FID;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_PORT_BASED_FIDr, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_FIDf, (rtksw_uint32 *)&fid)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_PORT_BASED_FID_ENr, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_ENABLEDf, (rtksw_uint32 *)&enable)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_vlan_portFid_get
 * Description:
 *      Get port-based filtering database
 *  Input:
 *      unit        - Unit ID
 *      port        - Port id.
 * Output:
 *      pEnable - ebable port-based FID
 *      pFid - Specified filtering database.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_INPUT - Invalid input parameters.
 *      RT_ERR_PORT_ID - Invalid port ID.
 * Note:
 *      The API can get port-based filtering database status. If the function is enabled, all input
 *      packets will be assigned to the port-based fid regardless vlan tag.
 */
rtksw_api_ret_t dal_rtl8371c_vlan_portFid_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t *pEnable, rtksw_fid_t *pFid)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if(NULL == pEnable)
        return RT_ERR_NULL_POINTER;

    if(NULL == pFid)
        return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_array_field_read(unit, RTL8371C_PORT_BASED_FIDr, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_FIDf, (rtksw_uint32 *)pFid)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_array_field_read(unit, RTL8371C_PORT_BASED_FID_ENr, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_ENABLEDf, (rtksw_uint32 *)pEnable)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/*Spanning Tree*/
/* Function Name:
 *      dal_rtl8371c_stp_mstpState_set
 * Description:
 *      Configure spanning tree state per each port.
 *  Input:
 *      unit        - Unit ID
 *      port        - Port id
 *      msti        - Multiple spanning tree instance.
 *      stp_state   - Spanning tree state for msti
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_MSTI         - Invalid msti parameter.
 *      RT_ERR_MSTP_STATE   - Invalid STP state.
 * Note:
 *      System supports per-port multiple spanning tree state for each msti.
 *      There are four states supported by ASIC.
 *      - RTKSW_STP_STATE_DISABLED
 *      - RTKSW_STP_STATE_BLOCKING
 *      - RTKSW_STP_STATE_LEARNING
 *      - RTKSW_STP_STATE_FORWARDING
 */
rtksw_api_ret_t dal_rtl8371c_stp_mstpState_set(rtksw_uint32 unit, rtksw_stp_msti_id_t msti, rtksw_port_t port, rtksw_stp_state_t stp_state)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 field;
    rtksw_uint32 phyPort;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if (msti > RTL8371C_MSTIMAX)
        return RT_ERR_MSTI;

    if (stp_state >= RTKSW_STP_STATE_END)
        return RT_ERR_MSTP_STATE;

    phyPort = rtksw_switch_port_L2P_get(unit, port);
    if (phyPort == UNDEFINE_PHY_PORT)
        return RT_ERR_PORT_ID;

    if (phyPort == 0)
        field = RTL8371C_PORT0_STATEf;
    else if (phyPort == 1)
        field = RTL8371C_PORT1_STATEf;
    else if (phyPort == 2)
        field = RTL8371C_PORT2_STATEf;
    else if (phyPort == 3)
        field = RTL8371C_PORT3_STATEf;
    else if (phyPort == 4)
        field = RTL8371C_PORT4_STATEf;
    else if (phyPort == 5)
        field = RTL8371C_PORT5_STATEf;
    else if (phyPort == 6)
        field = RTL8371C_PORT6_STATEf;
    else if (phyPort == 7)
        field = RTL8371C_PORT7_STATEf;
    else
		return RT_ERR_PORT_ID;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_MSTP_STATEr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)msti, field, (rtksw_uint32 *)&stp_state)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_stp_mstpState_get
 * Description:
 *      Get spanning tree state per each port.
 *  Input:
 *      unit        - Unit ID
 *      port        - Port id.
 *      msti        - Multiple spanning tree instance.
 * Output:
 *      pStp_state - Spanning tree state for msti
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_MSTI         - Invalid msti parameter.
 * Note:
 *      System supports per-port multiple spanning tree state for each msti.
 *      There are four states supported by ASIC.
 *      - RTKSW_STP_STATE_DISABLED
 *      - RTKSW_STP_STATE_BLOCKING
 *      - RTKSW_STP_STATE_LEARNING
 *      - RTKSW_STP_STATE_FORWARDING
 */
rtksw_api_ret_t dal_rtl8371c_stp_mstpState_get(rtksw_uint32 unit, rtksw_stp_msti_id_t msti, rtksw_port_t port, rtksw_stp_state_t *pStp_state)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 field;
    rtksw_uint32 phyPort;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if (msti > RTL8371C_MSTIMAX)
        return RT_ERR_MSTI;

    if(NULL == pStp_state)
        return RT_ERR_NULL_POINTER;

    phyPort = rtksw_switch_port_L2P_get(unit, port);
    if (phyPort == UNDEFINE_PHY_PORT)
        return RT_ERR_PORT_ID;

    if (phyPort == 0)
        field = RTL8371C_PORT0_STATEf;
    else if (phyPort == 1)
        field = RTL8371C_PORT1_STATEf;
    else if (phyPort == 2)
        field = RTL8371C_PORT2_STATEf;
    else if (phyPort == 3)
        field = RTL8371C_PORT3_STATEf;
    else if (phyPort == 4)
        field = RTL8371C_PORT4_STATEf;
    else if (phyPort == 5)
        field = RTL8371C_PORT5_STATEf;
    else if (phyPort == 6)
        field = RTL8371C_PORT6_STATEf;
    else if (phyPort == 7)
        field = RTL8371C_PORT7_STATEf;
    else
		return RT_ERR_PORT_ID;

    if ((retVal = reg16_array_field_read(unit, RTL8371C_MSTP_STATEr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)msti, field, (rtksw_uint32 *)pStp_state)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_vlan_reservedVidAction_set
 * Description:
 *      Set Action of VLAN ID = 0 & 4095 tagged packet
 *  Input:
 *      unit            - Unit ID
 *      action_vid0     - Action for VID 0.
 *      action_vid4095  - Action for VID 4095.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Error Input
 * Note:
 *
 */
rtksw_api_ret_t dal_rtl8371c_vlan_reservedVidAction_set(rtksw_uint32 unit, rtksw_vlan_resVidAction_t action_vid0, rtksw_vlan_resVidAction_t action_vid4095)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(action_vid0 >= RTKSW_RESVID_ACTION_END)
        return RT_ERR_INPUT;

    if(action_vid4095 >= RTKSW_RESVID_ACTION_END)
        return RT_ERR_INPUT;

    if ((retVal = reg16_read(unit, RTL8371C_VLAN_CTRLr, &regData)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_field_set(unit, RTL8371C_VLAN_CTRLr, RTL8371C_VLAN_VID0_TYPEf, (rtksw_uint32 *)&action_vid0, &regData)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_field_set(unit, RTL8371C_VLAN_CTRLr, RTL8371C_VLAN_VID4095_TYPEf, (rtksw_uint32 *)&action_vid4095, &regData)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_write(unit, RTL8371C_VLAN_CTRLr, &regData)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_vlan_reservedVidAction_get
 * Description:
 *      Get Action of VLAN ID = 0 & 4095 tagged packet
 *  Input:
 *      unit            - Unit ID
 *      pAction_vid0    - Action for VID 0.
 *      pAction_vid4095 - Action for VID 4095.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_NULL_POINTER - NULL Pointer
 * Note:
 *
 */
rtksw_api_ret_t dal_rtl8371c_vlan_reservedVidAction_get(rtksw_uint32 unit, rtksw_vlan_resVidAction_t *pAction_vid0, rtksw_vlan_resVidAction_t *pAction_vid4095)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(pAction_vid0 == NULL)
        return RT_ERR_NULL_POINTER;

    if(pAction_vid4095 == NULL)
        return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_read(unit, RTL8371C_VLAN_CTRLr, &regData)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_field_get(unit, RTL8371C_VLAN_CTRLr, RTL8371C_VLAN_VID0_TYPEf, (rtksw_uint32 *)pAction_vid0, &regData)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_field_get(unit, RTL8371C_VLAN_CTRLr, RTL8371C_VLAN_VID4095_TYPEf, (rtksw_uint32 *)pAction_vid4095, &regData)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_vlan_realKeepRemarkEnable_set
 * Description:
 *      Set Real keep 1p remarking feature
 *  Input:
 *      unit        - Unit ID
 *      enabled     - State of 1p remarking at real keep packet
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Error Input
 * Note:
 *
 */
rtksw_api_ret_t dal_rtl8371c_vlan_realKeepRemarkEnable_set(rtksw_uint32 unit, rtksw_enable_t enabled)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(enabled >= RTKSW_ENABLE_END)
        return RT_ERR_INPUT;

    if ((retVal = reg16_field_write(unit, RTL8371C_VLAN_TAG_PRI_CFGr, RTL8371C_RMK1P_BYPASS_REALKEEPf, (rtksw_uint32 *)&enabled)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_vlan_realKeepRemarkEnable_get
 * Description:
 *      Get Real keep 1p remarking feature
 *  Input:
 *      unit        - Unit ID
 * Output:
 *      pEnabled     - State of 1p remarking at real keep packet
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Error Input
 * Note:
 *
 */
rtksw_api_ret_t dal_rtl8371c_vlan_realKeepRemarkEnable_get(rtksw_uint32 unit, rtksw_enable_t *pEnabled)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(NULL == pEnabled)
        return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_field_read(unit, RTL8371C_VLAN_TAG_PRI_CFGr, RTL8371C_RMK1P_BYPASS_REALKEEPf, (rtksw_uint32 *)pEnabled)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_vlan_reset
 * Description:
 *      Reset VLAN
 *  Input:
 *      unit        - Unit ID
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Error Input
 * Note:
 *
 */
rtksw_api_ret_t dal_rtl8371c_vlan_reset(rtksw_uint32 unit)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData = 1;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if ((retVal = reg16_field_write(unit, RTL8371C_VLAN_RESET_TABLEr, RTL8371C_RESET_VLANf, &regData)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

