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

#ifndef __RTKSW_API_SVLAN_H__
#define __RTKSW_API_SVLAN_H__

typedef rtksw_uint32 rtksw_svlan_index_t;

typedef struct rtksw_svlan_memberCfg_s{
    rtksw_uint32 svid;
    rtksw_portmask_t memberport;
    rtksw_portmask_t untagport;
    rtksw_uint32 fiden;
    rtksw_uint32 fid;
    rtksw_uint32 priority;
    rtksw_uint32 efiden;
    rtksw_uint32 efid;
    rtksw_uint32 chk_ivl_svl;	
    rtksw_uint32 ivl_svl;		
}rtksw_svlan_memberCfg_t;

typedef enum rtksw_svlan_pri_ref_e
{
    REF_INTERNAL_PRI = 0,
    REF_CTAG_PRI,
    REF_SVLAN_PRI,
    REF_PB_PRI,
    REF_PRI_END
} rtksw_svlan_pri_ref_t;


typedef rtksw_uint32 rtksw_svlan_tpid_t;

typedef enum rtksw_svlan_untag_action_e
{
    UNTAG_DROP = 0,
    UNTAG_TRAP,
    UNTAG_ASSIGN,
    UNTAG_END
} rtksw_svlan_untag_action_t;

typedef enum rtksw_svlan_unmatch_action_e
{
    UNMATCH_DROP = 0,
    UNMATCH_TRAP,
    UNMATCH_ASSIGN,
    UNMATCH_END
} rtksw_svlan_unmatch_action_t;

typedef enum rtksw_svlan_unassign_action_e
{
    UNASSIGN_PBSVID = 0,
    UNASSIGN_TRAP,
    UNASSIGN_END
} rtksw_svlan_unassign_action_t;


typedef enum rtksw_svlan_lookupType_e
{
    SVLAN_LOOKUP_S64MBRCGF  = 0,
    SVLAN_LOOKUP_C4KVLAN,
    SVLAN_LOOKUP_END,

} rtksw_svlan_lookupType_t;

/* Function Name:
 *      rtksw_svlan_init
 * Description:
 *      Initialize SVLAN Configuration
 * Input:
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
extern rtksw_api_ret_t rtksw_svlan_init(rtksw_uint32 unit);

/* Function Name:
 *      rtksw_svlan_servicePort_add
 * Description:
 *      Add one service port in the specified device
 * Input:
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
extern rtksw_api_ret_t rtksw_svlan_servicePort_add(rtksw_uint32 unit, rtksw_port_t port);

/* Function Name:
 *      rtksw_svlan_servicePort_get
 * Description:
 *      Get service ports in the specified device.
 * Input:
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
extern rtksw_api_ret_t rtksw_svlan_servicePort_get(rtksw_uint32 unit, rtksw_portmask_t *pSvlan_portmask);

/* Function Name:
 *      rtksw_svlan_servicePort_del
 * Description:
 *      Delete one service port in the specified device
 * Input:
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
extern rtksw_api_ret_t rtksw_svlan_servicePort_del(rtksw_uint32 unit, rtksw_port_t port);

/* Function Name:
 *      rtksw_svlan_tpidEntry_set
 * Description:
 *      Configure accepted S-VLAN ether type.
 * Input:
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
extern rtksw_api_ret_t rtksw_svlan_tpidEntry_set(rtksw_uint32 unit, rtksw_svlan_tpid_t svlan_tag_id);

/* Function Name:
 *      rtksw_svlan_tpidEntry_get
 * Description:
 *      Get accepted S-VLAN ether type setting.
 * Input:
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
extern rtksw_api_ret_t rtksw_svlan_tpidEntry_get(rtksw_uint32 unit, rtksw_svlan_tpid_t *pSvlan_tag_id);

/* Function Name:
 *      rtksw_svlan_priorityRef_set
 * Description:
 *      Set S-VLAN upstream priority reference setting.
 * Input:
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
 *      - REF_SVLAN_PRI,
 *      - REF_PB_PRI.
 */
extern rtksw_api_ret_t rtksw_svlan_priorityRef_set(rtksw_uint32 unit, rtksw_svlan_pri_ref_t ref);

/* Function Name:
 *      rtksw_svlan_priorityRef_get
 * Description:
 *      Get S-VLAN upstream priority reference setting.
 * Input:
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
extern rtksw_api_ret_t rtksw_svlan_priorityRef_get(rtksw_uint32 unit, rtksw_svlan_pri_ref_t *pRef);

/* Function Name:
 *      rtksw_svlan_memberPortEntry_set
 * Description:
 *      Configure system SVLAN member content
 * Input:
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
extern rtksw_api_ret_t rtksw_svlan_memberPortEntry_set(rtksw_uint32 unit, rtksw_vlan_t svid, rtksw_svlan_memberCfg_t *pSvlan_cfg);

/* Function Name:
 *      rtksw_svlan_memberPortEntry_get
 * Description:
 *      Get SVLAN member Configure.
 * Input:
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
extern rtksw_api_ret_t rtksw_svlan_memberPortEntry_get(rtksw_uint32 unit, rtksw_vlan_t svid, rtksw_svlan_memberCfg_t *pSvlan_cfg);

/* Function Name:
 *      rtksw_svlan_memberPortEntry_adv_set
 * Description:
 *      Configure system SVLAN member by index
 * Input:
 *      unit        - Unit ID
 *      idx         - Index (0 ~ 63)
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
 *      The API can set system 64 accepted s-tag frame format by index.
 *      - rtksw_svlan_memberCfg_t->svid is SVID of SVLAN member configuration.
 *      - rtksw_svlan_memberCfg_t->memberport is member port mask of SVLAN member configuration.
 *      - rtksw_svlan_memberCfg_t->fid is filtering database of SVLAN member configuration.
 *      - rtksw_svlan_memberCfg_t->priority is priority of SVLAN member configuration.
 */
extern rtksw_api_ret_t rtksw_svlan_memberPortEntry_adv_set(rtksw_uint32 unit, rtksw_uint32 idx, rtksw_svlan_memberCfg_t *pSvlan_cfg);

/* Function Name:
 *      rtksw_svlan_memberPortEntry_adv_get
 * Description:
 *      Get SVLAN member Configure by index.
 * Input:
 *      unit        - Unit ID
 *      idx         - Index (0 ~ 63)
 * Output:
 *      pSvlan_cfg  - SVLAN member configuration
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
extern rtksw_api_ret_t rtksw_svlan_memberPortEntry_adv_get(rtksw_uint32 unit, rtksw_uint32 idx, rtksw_svlan_memberCfg_t *pSvlan_cfg);

/* Function Name:
 *      rtksw_svlan_defaultSvlan_set
 * Description:
 *      Configure default egress SVLAN.
 * Input:
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
extern rtksw_api_ret_t rtksw_svlan_defaultSvlan_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_vlan_t svid);

/* Function Name:
 *      rtksw_svlan_defaultSvlan_get
 * Description:
 *      Get the configure default egress SVLAN.
 * Input:
 *      unit        - Unit ID
 *      port        - Source port
 * Output:
 *      pSvid       - SVLAN VID
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      The API can get port n S-tag format index while receiving frame from port n
 *      is transmit through uplink port with s-tag field
 */
extern rtksw_api_ret_t rtksw_svlan_defaultSvlan_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_vlan_t *pSvid);

/* Function Name:
 *      rtksw_svlan_c2s_add
 * Description:
 *      Configure SVLAN C2S table
 * Input:
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
extern rtksw_api_ret_t rtksw_svlan_c2s_add(rtksw_uint32 unit, rtksw_vlan_t vid, rtksw_port_t src_port, rtksw_vlan_t svid);

/* Function Name:
 *      rtksw_svlan_c2s_del
 * Description:
 *      Delete one C2S entry
 * Input:
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
extern rtksw_api_ret_t rtksw_svlan_c2s_del(rtksw_uint32 unit, rtksw_vlan_t vid, rtksw_port_t src_port);

/* Function Name:
 *      rtksw_svlan_c2s_get
 * Description:
 *      Get configure SVLAN C2S table
 * Input:
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
extern rtksw_api_ret_t rtksw_svlan_c2s_get(rtksw_uint32 unit, rtksw_vlan_t vid, rtksw_port_t src_port, rtksw_vlan_t *pSvid);

/* Function Name:
 *      rtksw_svlan_untag_action_set
 * Description:
 *      Configure Action of downstream UnStag packet
 * Input:
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
extern rtksw_api_ret_t rtksw_svlan_untag_action_set(rtksw_uint32 unit, rtksw_svlan_untag_action_t action, rtksw_vlan_t svid);

/* Function Name:
 *      rtksw_svlan_untag_action_get
 * Description:
 *      Get Action of downstream UnStag packet
 * Input:
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
extern rtksw_api_ret_t rtksw_svlan_untag_action_get(rtksw_uint32 unit, rtksw_svlan_untag_action_t *pAction, rtksw_vlan_t *pSvid);

/* Function Name:
 *      rtksw_svlan_unmatch_action_set
 * Description:
 *      Configure Action of downstream Unmatch packet
 * Input:
 *      unit        - Unit ID
 *      action      - Action for Unmatch
 *      svid        - The SVID assigned to Unmatch packet
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
 *      The API can configure action of downstream Un-match packet. A SVID assigned
 *      to the un-match is also supported by this API. The parameter od svid is
 *      only refernced when the action is set to UNMATCH_ASSIGN
 */
extern rtksw_api_ret_t rtksw_svlan_unmatch_action_set(rtksw_uint32 unit, rtksw_svlan_unmatch_action_t action, rtksw_vlan_t svid);

/* Function Name:
 *      rtksw_svlan_unmatch_action_get
 * Description:
 *      Get Action of downstream Unmatch packet
 * Input:
 *      unit        - Unit ID
 * Output:
 *      pAction  - Action for Unmatch
 *      pSvid    - The SVID assigned to Unmatch packet
 * Return:
 *      RT_ERR_OK                       - OK
 *      RT_ERR_FAILED                   - Failed
 *      RT_ERR_SMI                      - SMI access error
 *      RT_ERR_SVLAN_VID                - Invalid SVLAN VID parameter.
 *      RT_ERR_SVLAN_ENTRY_NOT_FOUND    - specified svlan entry not found.
 *      RT_ERR_OUT_OF_RANGE             - input out of range.
 *      RT_ERR_INPUT                    - Invalid input parameters.
 * Note:
 *      The API can Get action of downstream Un-match packet. A SVID assigned
 *      to the un-match is also retrieved by this API. The parameter pSvid is
 *      only refernced when the action is UNMATCH_ASSIGN
 */
extern rtksw_api_ret_t rtksw_svlan_unmatch_action_get(rtksw_uint32 unit, rtksw_svlan_unmatch_action_t *pAction, rtksw_vlan_t *pSvid);

/* Function Name:
 *      rtksw_svlan_unassign_action_set
 * Description:
 *      Configure Action of upstream without svid assign action
 * Input:
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
extern rtksw_api_ret_t rtksw_svlan_unassign_action_set(rtksw_uint32 unit, rtksw_svlan_unassign_action_t action);

/* Function Name:
 *      rtksw_svlan_unassign_action_get
 * Description:
 *      Get action of upstream without svid assignment
 * Input:
 *      unit        - Unit ID
 * Output:
 *      pAction  - Action for Un-assign
 * Return:
 *      RT_ERR_OK                       - OK
 *      RT_ERR_FAILED                   - Failed
 * Note:
 *      None
 */
extern rtksw_api_ret_t rtksw_svlan_unassign_action_get(rtksw_uint32 unit, rtksw_svlan_unassign_action_t *pAction);

/* Function Name:
 *      rtksw_svlan_dmac_vidsel_set
 * Description:
 *      Set DMAC CVID selection
 * Input:
 *      unit        - Unit ID
 *      port        - Port
 *      enable      - state of DMAC CVID Selection
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK                       - OK
 *      RT_ERR_FAILED                   - Failed
 *      RT_ERR_SMI                      - SMI access error
 *      RT_ERR_SVLAN_VID                - Invalid SVLAN VID parameter.
 *      RT_ERR_SVLAN_ENTRY_NOT_FOUND    - specified svlan entry not found.
 *      RT_ERR_OUT_OF_RANGE             - input out of range.
 *      RT_ERR_INPUT                    - Invalid input parameters.
 * Note:
 *      This API can set DMAC CVID Selection state
 */
extern rtksw_api_ret_t rtksw_svlan_dmac_vidsel_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t enable);

/* Function Name:
 *      rtksw_svlan_dmac_vidsel_get
 * Description:
 *      Get DMAC CVID selection
 * Input:
 *      unit        - Unit ID
 *      port        - Port
 * Output:
 *      pEnable - state of DMAC CVID Selection
 * Return:
 *      RT_ERR_OK                       - OK
 *      RT_ERR_FAILED                   - Failed
 *      RT_ERR_SMI                      - SMI access error
 *      RT_ERR_SVLAN_VID                - Invalid SVLAN VID parameter.
 *      RT_ERR_SVLAN_ENTRY_NOT_FOUND    - specified svlan entry not found.
 *      RT_ERR_OUT_OF_RANGE             - input out of range.
 *      RT_ERR_INPUT                    - Invalid input parameters.
 * Note:
 *      This API can get DMAC CVID Selection state
 */
extern rtksw_api_ret_t rtksw_svlan_dmac_vidsel_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t *pEnable);

/* Function Name:
 *      rtksw_svlan_ipmc2s_add
 * Description:
 *      add ip multicast address to SVLAN
 * Input:
 *      unit        - Unit ID
 *      svid        - SVLAN VID
 *      ipmc        - ip multicast address
 *      ipmcMsk     - ip multicast mask
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
 *      The API can set IP mutlicast to SVID configuration. If upstream packet is IPv4 multicast
 *      packet and DIP is matched MC2S configuration, ASIC will assign egress SVID to the packet.
 *      There are 32 SVLAN multicast configurations for IP and L2 multicast.
 */
extern rtksw_api_ret_t rtksw_svlan_ipmc2s_add(rtksw_uint32 unit, ipaddr_t ipmc, ipaddr_t ipmcMsk,rtksw_vlan_t svid);

/* Function Name:
 *      rtksw_svlan_ipmc2s_del
 * Description:
 *      delete ip multicast address to SVLAN
 * Input:
 *      unit        - Unit ID
 *      ipmc        - ip multicast address
 *      ipmcMsk     - ip multicast mask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_SVLAN_VID        - Invalid SVLAN VID parameter.
 *      RT_ERR_OUT_OF_RANGE     - input out of range.
 * Note:
 *      The API can delete IP mutlicast to SVID configuration. There are 32 SVLAN multicast configurations for IP and L2 multicast.
 */
extern rtksw_api_ret_t rtksw_svlan_ipmc2s_del(rtksw_uint32 unit, ipaddr_t ipmc, ipaddr_t ipmcMsk);

/* Function Name:
 *      rtksw_svlan_ipmc2s_get
 * Description:
 *      Get ip multicast address to SVLAN
 * Input:
 *      unit        - Unit ID
 *      ipmc        - ip multicast address
 *      ipmcMsk     - ip multicast mask
 * Output:
 *      pSvid - SVLAN VID
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_OUT_OF_RANGE - input out of range.
 * Note:
 *      The API can get IP mutlicast to SVID configuration. There are 32 SVLAN multicast configurations for IP and L2 multicast.
 */
extern rtksw_api_ret_t rtksw_svlan_ipmc2s_get(rtksw_uint32 unit, ipaddr_t ipmc, ipaddr_t ipmcMsk, rtksw_vlan_t *pSvid);

/* Function Name:
 *      rtksw_svlan_l2mc2s_add
 * Description:
 *      Add L2 multicast address to SVLAN
 * Input:
 *      unit        - Unit ID
 *      mac         - L2 multicast address
 *      macMsk      - L2 multicast address mask
 *      svid        - SVLAN VID
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
 *      The API can set L2 Mutlicast to SVID configuration. If upstream packet is L2 multicast
 *      packet and DMAC is matched, ASIC will assign egress SVID to the packet. There are 32
 *      SVLAN multicast configurations for IP and L2 multicast.
 */
extern rtksw_api_ret_t rtksw_svlan_l2mc2s_add(rtksw_uint32 unit, rtksw_mac_t mac, rtksw_mac_t macMsk, rtksw_vlan_t svid);

/* Function Name:
 *      rtksw_svlan_l2mc2s_del
 * Description:
 *      delete L2 multicast address to SVLAN
 * Input:
 *      unit        - Unit ID
 *      mac         - L2 multicast address
 *      macMsk      - L2 multicast address mask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_SVLAN_VID        - Invalid SVLAN VID parameter.
 *      RT_ERR_OUT_OF_RANGE     - input out of range.
 * Note:
 *      The API can delete Mutlicast to SVID configuration. There are 32 SVLAN multicast configurations for IP and L2 multicast.
 */
extern rtksw_api_ret_t rtksw_svlan_l2mc2s_del(rtksw_uint32 unit, rtksw_mac_t mac, rtksw_mac_t macMsk);

/* Function Name:
 *      rtksw_svlan_l2mc2s_get
 * Description:
 *      Get L2 multicast address to SVLAN
 * Input:
 *      unit        - Unit ID
 *      mac         - L2 multicast address
 *      macMsk      - L2 multicast address mask
 * Output:
 *      pSvid - SVLAN VID
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_INPUT            - Invalid input parameters.
 *      RT_ERR_OUT_OF_RANGE     - input out of range.
 * Note:
 *      The API can get L2 mutlicast to SVID configuration. There are 32 SVLAN multicast configurations for IP and L2 multicast.
 */
extern rtksw_api_ret_t rtksw_svlan_l2mc2s_get(rtksw_uint32 unit, rtksw_mac_t mac, rtksw_mac_t macMsk, rtksw_vlan_t *pSvid);

/* Function Name:
 *      rtksw_svlan_sp2c_add
 * Description:
 *      Add system SP2C configuration
 * Input:
 *      unit        - Unit ID
 *      cvid        - VLAN ID
 *      dst_port    - Destination port of SVLAN to CVLAN configuration
 *      svid        - SVLAN VID
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
extern rtksw_api_ret_t rtksw_svlan_sp2c_add(rtksw_uint32 unit, rtksw_vlan_t svid, rtksw_port_t dst_port, rtksw_vlan_t cvid);

/* Function Name:
 *      rtksw_svlan_sp2c_get
 * Description:
 *      Get configure system SP2C content
 * Input:
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
extern rtksw_api_ret_t rtksw_svlan_sp2c_get(rtksw_uint32 unit, rtksw_vlan_t svid, rtksw_port_t dst_port, rtksw_vlan_t *pCvid);

/* Function Name:
 *      rtksw_svlan_sp2c_del
 * Description:
 *      Delete system SP2C configuration
 * Input:
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
extern rtksw_api_ret_t rtksw_svlan_sp2c_del(rtksw_uint32 unit, rtksw_vlan_t svid, rtksw_port_t dst_port);

/* Function Name:
 *      rtksw_svlan_lookupType_set
 * Description:
 *      Set lookup type of SVLAN
 * Input:
 *      unit        - Unit ID
 *      type        - lookup type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      none
 */
extern rtksw_api_ret_t rtksw_svlan_lookupType_set(rtksw_uint32 unit, rtksw_svlan_lookupType_t type);

/* Function Name:
 *      rtksw_svlan_lookupType_get
 * Description:
 *      Get lookup type of SVLAN
 * Input:
 *      unit        - Unit ID
 *      pType       - lookup type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      none
 */
extern rtksw_api_ret_t rtksw_svlan_lookupType_get(rtksw_uint32 unit, rtksw_svlan_lookupType_t *pType);

/* Function Name:
 *      rtksw_svlan_trapPri_set
 * Description:
 *      Set svlan trap priority
 * Input:
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
extern rtksw_api_ret_t rtksw_svlan_trapPri_set(rtksw_uint32 unit, rtksw_pri_t priority);

/* Function Name:
 *      rtksw_svlan_trapPri_get
 * Description:
 *      Get svlan trap priority
 * Input:
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
extern rtksw_api_ret_t rtksw_svlan_trapPri_get(rtksw_uint32 unit, rtksw_pri_t *pPriority);

#if defined(CONFIG_COMPATIBLE_V1_4)

#define rtk_svlan_init(void)                                rtksw_svlan_init(0)
#define rtk_svlan_servicePort_add(port)                     rtksw_svlan_servicePort_add(0, port)
#define rtk_svlan_servicePort_get(pSvlan_portmask)          rtksw_svlan_servicePort_get(0, pSvlan_portmask)
#define rtk_svlan_servicePort_del(port)                     rtksw_svlan_servicePort_del(0, port)
#define rtk_svlan_tpidEntry_set(svlan_tag_id)               rtksw_svlan_tpidEntry_set(0, svlan_tag_id)
#define rtk_svlan_tpidEntry_get(pSvlan_tag_id)              rtksw_svlan_tpidEntry_get(0, pSvlan_tag_id)
#define rtk_svlan_priorityRef_set(ref)                      rtksw_svlan_priorityRef_set(0, ref)
#define rtk_svlan_priorityRef_get(pRef)                     rtksw_svlan_priorityRef_get(0, pRef)
#define rtk_svlan_memberPortEntry_set(svid, pSvlan_cfg)     rtksw_svlan_memberPortEntry_set(0, svid, pSvlan_cfg)
#define rtk_svlan_memberPortEntry_get(svid, pSvlan_cfg)     rtksw_svlan_memberPortEntry_get(0, svid, pSvlan_cfg)
#define rtk_svlan_memberPortEntry_adv_set(idx, pSvlan_cfg)  rtksw_svlan_memberPortEntry_adv_set(0, idx, pSvlan_cfg)
#define rtk_svlan_memberPortEntry_adv_get(idx, pSvlan_cfg)  rtksw_svlan_memberPortEntry_adv_get(0, idx, pSvlan_cfg)
#define rtk_svlan_defaultSvlan_set(port, svid)              rtksw_svlan_defaultSvlan_set(0, port, svid)
#define rtk_svlan_defaultSvlan_get(port, pSvid)             rtksw_svlan_defaultSvlan_get(0, port, pSvid)
#define rtk_svlan_c2s_add(vid, src_port, svid)              rtksw_svlan_c2s_add(0, vid, src_port, svid)
#define rtk_svlan_c2s_del(vid, src_port)                    rtksw_svlan_c2s_del(0, vid, src_port)
#define rtk_svlan_c2s_get(vid, src_port, pSvid)             rtksw_svlan_c2s_get(0, vid, src_port, pSvid)
#define rtk_svlan_untag_action_set(action, svid)            rtksw_svlan_untag_action_set(0, action, svid)
#define rtk_svlan_untag_action_get(pAction, pSvid)          rtksw_svlan_untag_action_get(0, pAction, pSvid)
#define rtk_svlan_unmatch_action_set(action, svid)          rtksw_svlan_unmatch_action_set(0, action, svid)
#define rtk_svlan_unmatch_action_get(pAction, pSvid)        rtksw_svlan_unmatch_action_get(0, pAction, pSvid)
#define rtk_svlan_unassign_action_set(action)               rtksw_svlan_unassign_action_set(0, action)
#define rtk_svlan_unassign_action_get(pAction)              rtksw_svlan_unassign_action_get(0, pAction)
#define rtk_svlan_dmac_vidsel_set(port, enable)             rtksw_svlan_dmac_vidsel_set(0, port, enable)
#define rtk_svlan_dmac_vidsel_get(port, pEnable)            rtksw_svlan_dmac_vidsel_get(0, port, pEnable)
#define rtk_svlan_ipmc2s_add(ipmc, ipmcMsk,svid)            rtksw_svlan_ipmc2s_add(0, ipmc, ipmcMsk,svid)
#define rtk_svlan_ipmc2s_del(ipmc, ipmcMsk)                 rtksw_svlan_ipmc2s_del(0, ipmc, ipmcMsk)
#define rtk_svlan_ipmc2s_get(ipmc, ipmcMsk, pSvid)          rtksw_svlan_ipmc2s_get(0, ipmc, ipmcMsk, pSvid)
#define rtk_svlan_l2mc2s_add(mac, macMsk, svid)             rtksw_svlan_l2mc2s_add(0, mac, macMsk, svid)
#define rtk_svlan_l2mc2s_del(mac, macMsk)                   rtksw_svlan_l2mc2s_del(0, mac, macMsk)
#define rtk_svlan_l2mc2s_get(mac, macMsk, pSvid)            rtksw_svlan_l2mc2s_get(0, mac, macMsk, pSvid)
#define rtk_svlan_sp2c_add(svid, dst_port, cvid)            rtksw_svlan_sp2c_add(0, svid, dst_port, cvid)
#define rtk_svlan_sp2c_get(svid, dst_port, pCvid)           rtksw_svlan_sp2c_get(0, svid, dst_port, pCvid)
#define rtk_svlan_sp2c_del(svid, dst_port)                  rtksw_svlan_sp2c_del(0, svid, dst_port)
#define rtk_svlan_lookupType_set(type)                      rtksw_svlan_lookupType_set(0, type)
#define rtk_svlan_lookupType_get(pType)                     rtksw_svlan_lookupType_get(0, pType)
#define rtk_svlan_trapPri_set(priority)                     rtksw_svlan_trapPri_set(0, priority)
#define rtk_svlan_trapPri_get(pPriority)                    rtksw_svlan_trapPri_get(0, pPriority)

#define rtk_svlan_index_t               rtksw_svlan_index_t
#define rtk_svlan_memberCfg_t           rtksw_svlan_memberCfg_t
#define rtk_svlan_pri_ref_t             rtksw_svlan_pri_ref_t
#define rtk_svlan_tpid_t                rtksw_svlan_tpid_t
#define rtk_svlan_untag_action_t        rtksw_svlan_untag_action_t
#define rtk_svlan_unmatch_action_t      rtksw_svlan_unmatch_action_t
#define rtk_svlan_unassign_action_t     rtksw_svlan_unassign_action_t
#define rtk_svlan_lookupType_t          rtksw_svlan_lookupType_t

#endif /* CONFIG_COMPATIBLE_V1_4 */

#endif /* __RTKSW_API_SVLAN_H__ */
