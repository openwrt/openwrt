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

#ifndef __DAL_RTL8371C_VLAN_H__
#define __DAL_RTL8371C_VLAN_H__

#include <vlan.h>

#define RTL8371C_VIDMAX                     0xFFF
#define RTL8371C_FIDMAX                     7
#define RTL8371C_MSTIMAX                    7

#define RTL8371C_VLAN_4KTABLE_LEN           2
#define RTL8371C_VLAN_BUSY_CHECK_NO         10

#define RTL8371C_PORTMASK                   0xFF

typedef struct rtl8371c_vlan_entry_s
{
    rtksw_uint32 entry_data[RTL8371C_VLAN_4KTABLE_LEN];
}rtl8371c_vlan_entry_t;


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
extern rtksw_api_ret_t dal_rtl8371c_vlan_init(rtksw_uint32 unit);

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
extern rtksw_api_ret_t dal_rtl8371c_vlan_set(rtksw_uint32 unit, rtksw_vlan_t vid, rtksw_vlan_cfg_t *pVlanCfg);

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
extern rtksw_api_ret_t dal_rtl8371c_vlan_get(rtksw_uint32 unit, rtksw_vlan_t vid, rtksw_vlan_cfg_t *pVlanCfg);

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
extern rtksw_api_ret_t dal_rtl8371c_vlan_egrFilterEnable_set(rtksw_uint32 unit, rtksw_enable_t egrFilter);

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
extern rtksw_api_ret_t dal_rtl8371c_vlan_egrFilterEnable_get(rtksw_uint32 unit, rtksw_enable_t *pEgrFilter);

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
extern rtksw_api_ret_t dal_rtl8371c_vlan_portPvid_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_vlan_t pvid, rtksw_pri_t priority);

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
extern rtksw_api_ret_t dal_rtl8371c_vlan_portPvid_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_vlan_t *pPvid, rtksw_pri_t *pPriority);

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
extern rtksw_api_ret_t dal_rtl8371c_vlan_portIgrFilterEnable_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t igr_filter);

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
extern rtksw_api_ret_t dal_rtl8371c_vlan_portIgrFilterEnable_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t *pIgr_filter);

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
extern rtksw_api_ret_t dal_rtl8371c_vlan_portAcceptFrameType_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_vlan_acceptFrameType_t accept_frame_type);

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
extern rtksw_api_ret_t dal_rtl8371c_vlan_portAcceptFrameType_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_vlan_acceptFrameType_t *pAccept_frame_type);

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
extern rtksw_api_ret_t dal_rtl8371c_vlan_tagMode_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_vlan_tagMode_t tag_mode);

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
extern rtksw_api_ret_t dal_rtl8371c_vlan_tagMode_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_vlan_tagMode_t *pTag_mode);

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
extern rtksw_api_ret_t dal_rtl8371c_vlan_transparent_set(rtksw_uint32 unit, rtksw_port_t egr_port, rtksw_portmask_t *pIgr_pmask);

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
extern rtksw_api_ret_t dal_rtl8371c_vlan_transparent_get(rtksw_uint32 unit, rtksw_port_t egr_port, rtksw_portmask_t *pIgr_pmask);

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
extern rtksw_api_ret_t dal_rtl8371c_vlan_keep_set(rtksw_uint32 unit, rtksw_port_t egr_port, rtksw_portmask_t *pIgr_pmask);

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
extern rtksw_api_ret_t dal_rtl8371c_vlan_keep_get(rtksw_uint32 unit, rtksw_port_t egr_port, rtksw_portmask_t *pIgr_pmask);

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
extern rtksw_api_ret_t dal_rtl8371c_vlan_portFid_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t enable, rtksw_fid_t fid);

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
extern rtksw_api_ret_t dal_rtl8371c_vlan_portFid_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t *pEnable, rtksw_fid_t *pFid);


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
extern rtksw_api_ret_t dal_rtl8371c_stp_mstpState_set(rtksw_uint32 unit, rtksw_stp_msti_id_t msti, rtksw_port_t port, rtksw_stp_state_t stp_state);

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
extern rtksw_api_ret_t dal_rtl8371c_stp_mstpState_get(rtksw_uint32 unit, rtksw_stp_msti_id_t msti, rtksw_port_t port, rtksw_stp_state_t *pStp_state);

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
extern rtksw_api_ret_t dal_rtl8371c_vlan_reservedVidAction_set(rtksw_uint32 unit, rtksw_vlan_resVidAction_t action_vid0, rtksw_vlan_resVidAction_t action_vid4095);

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
extern rtksw_api_ret_t dal_rtl8371c_vlan_reservedVidAction_get(rtksw_uint32 unit, rtksw_vlan_resVidAction_t *pAction_vid0, rtksw_vlan_resVidAction_t *pAction_vid4095);

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
extern rtksw_api_ret_t dal_rtl8371c_vlan_realKeepRemarkEnable_set(rtksw_uint32 unit, rtksw_enable_t enabled);

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
extern rtksw_api_ret_t dal_rtl8371c_vlan_realKeepRemarkEnable_get(rtksw_uint32 unit, rtksw_enable_t *pEnabled);

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
extern rtksw_api_ret_t dal_rtl8371c_vlan_reset(rtksw_uint32 unit);

#endif /* __DAL_RTL8371C_VLAN_H__ */
