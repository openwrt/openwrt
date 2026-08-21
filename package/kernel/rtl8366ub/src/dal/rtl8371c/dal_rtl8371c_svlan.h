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

#ifndef __DAL_RTL8371C_SVLAN_H__
#define __DAL_RTL8371C_SVLAN_H__

#include <svlan.h>

#define RTL8371C_C2SIDXMAX                  31
#define RTL8371C_SP2CMAX                    63

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
extern rtksw_api_ret_t dal_rtl8371c_svlaninit(rtksw_uint32 unit);

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
extern rtksw_api_ret_t dal_rtl8371c_svlanservicePort_add(rtksw_uint32 unit, rtksw_port_t port);

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
extern rtksw_api_ret_t dal_rtl8371c_svlanservicePort_get(rtksw_uint32 unit, rtksw_portmask_t *pSvlan_portmask);

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
extern rtksw_api_ret_t dal_rtl8371c_svlanservicePort_del(rtksw_uint32 unit, rtksw_port_t port);

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
extern rtksw_api_ret_t dal_rtl8371c_svlantpidEntry_set(rtksw_uint32 unit, rtksw_uint32 svlan_tag_id);

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
extern rtksw_api_ret_t dal_rtl8371c_svlantpidEntry_get(rtksw_uint32 unit, rtksw_uint32 *pSvlan_tag_id);

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
extern rtksw_api_ret_t dal_rtl8371c_svlanpriorityRef_set(rtksw_uint32 unit, rtksw_svlan_pri_ref_t ref);

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
extern rtksw_api_ret_t dal_rtl8371c_svlanpriorityRef_get(rtksw_uint32 unit, rtksw_svlan_pri_ref_t *pRef);

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
extern rtksw_api_ret_t dal_rtl8371c_svlanmemberPortEntry_set(rtksw_uint32 unit, rtksw_uint32 svid_idx, rtksw_svlan_memberCfg_t *psvlan_cfg);

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
extern rtksw_api_ret_t dal_rtl8371c_svlanmemberPortEntry_get(rtksw_uint32 unit, rtksw_uint32 svid_idx, rtksw_svlan_memberCfg_t *pSvlan_cfg);

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
extern rtksw_api_ret_t dal_rtl8371c_svlandefaultSvlan_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_vlan_t svid);

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
extern rtksw_api_ret_t dal_rtl8371c_svlandefaultSvlan_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_vlan_t *pSvid);

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
extern rtksw_api_ret_t dal_rtl8371c_svlanc2s_add(rtksw_uint32 unit, rtksw_vlan_t vid, rtksw_port_t src_port, rtksw_vlan_t svid);

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
extern rtksw_api_ret_t dal_rtl8371c_svlanc2s_del(rtksw_uint32 unit, rtksw_vlan_t vid, rtksw_port_t src_port);

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
extern rtksw_api_ret_t dal_rtl8371c_svlanc2s_get(rtksw_uint32 unit, rtksw_vlan_t vid, rtksw_port_t src_port, rtksw_vlan_t *pSvid);


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
extern rtksw_api_ret_t dal_rtl8371c_svlan_sp2c_add(rtksw_uint32 unit, rtksw_vlan_t svid, rtksw_port_t dst_port, rtksw_vlan_t cvid);

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
extern rtksw_api_ret_t dal_rtl8371c_svlan_sp2c_get(rtksw_uint32 unit, rtksw_vlan_t svid, rtksw_port_t dst_port, rtksw_vlan_t *pCvid);

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
extern rtksw_api_ret_t dal_rtl8371c_svlan_sp2c_del(rtksw_uint32 unit, rtksw_vlan_t svid, rtksw_port_t dst_port);

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
extern rtksw_api_ret_t dal_rtl8371c_svlanuntag_action_set(rtksw_uint32 unit, rtksw_svlan_untag_action_t action, rtksw_vlan_t svid);

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
extern rtksw_api_ret_t dal_rtl8371c_svlanuntag_action_get(rtksw_uint32 unit, rtksw_svlan_untag_action_t *pAction, rtksw_vlan_t *pSvid);

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
extern rtksw_api_ret_t dal_rtl8371c_svlantrapPri_set(rtksw_uint32 unit, rtksw_pri_t priority);

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
extern rtksw_api_ret_t dal_rtl8371c_svlantrapPri_get(rtksw_uint32 unit, rtksw_pri_t *pPriority);

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
extern rtksw_api_ret_t dal_rtl8371c_svlanunassign_action_set(rtksw_uint32 unit, rtksw_svlan_unassign_action_t action);

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
extern rtksw_api_ret_t dal_rtl8371c_svlanunassign_action_get(rtksw_uint32 unit, rtksw_svlan_unassign_action_t *pAction);

#endif /* __DAL_RTL8371C_SVLAN_H__ */
