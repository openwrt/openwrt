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

#ifndef __RTKSW_API_VLAN_H__
#define __RTKSW_API_VLAN_H__


/*
 * Data Type Declaration
 */
#define RTKSW_MAX_NUM_OF_PROTO_TYPE                   0xFFFF
#define RTKSW_MAX_NUM_OF_MSTI                         0xF
#define RTKSW_FID_MAX                                 0xF

typedef struct  rtksw_vlan_cfg_s
{
    rtksw_portmask_t  mbr;
    rtksw_portmask_t  untag;
    rtksw_uint16      ivl_en;
    rtksw_uint16      fid_msti;
    rtksw_uint16      envlanpol;
    rtksw_uint16      meteridx;
    rtksw_uint16      vbpen;
    rtksw_uint16      vbpri;
}rtksw_vlan_cfg_t;

typedef struct  rtksw_vlan_mbrcfg_s
{
    rtksw_uint16      evid;
    rtksw_portmask_t  mbr;
    rtksw_uint16      fid_msti;
    rtksw_uint16      envlanpol;
    rtksw_uint16      meteridx;
    rtksw_uint16      vbpen;
    rtksw_uint16      vbpri;
}rtksw_vlan_mbrcfg_t;

typedef rtksw_uint32  rtksw_stp_msti_id_t;     /* MSTI ID  */

typedef enum rtksw_stp_state_e
{
    RTKSW_STP_STATE_DISABLED = 0,
    RTKSW_STP_STATE_BLOCKING,
    RTKSW_STP_STATE_LEARNING,
    RTKSW_STP_STATE_FORWARDING,
    RTKSW_STP_STATE_END
} rtksw_stp_state_t;

typedef rtksw_uint32  rtksw_vlan_proto_type_t;     /* protocol and port based VLAN protocol type  */


typedef enum rtksw_vlan_acceptFrameType_e
{
    RTKSW_ACCEPT_FRAME_TYPE_ALL = 0,             /* untagged, priority-tagged and tagged */
    RTKSW_ACCEPT_FRAME_TYPE_TAG_ONLY,         /* tagged */
    RTKSW_ACCEPT_FRAME_TYPE_UNTAG_ONLY,     /* untagged and priority-tagged */
    RTKSW_ACCEPT_FRAME_TYPE_END
} rtksw_vlan_acceptFrameType_t;


/* frame type of protocol vlan - reference 802.1v standard */
typedef enum rtksw_vlan_protoVlan_frameType_e
{
    RTKSW_FRAME_TYPE_ETHERNET = 0,
    RTKSW_FRAME_TYPE_LLCOTHER,
    RTKSW_FRAME_TYPE_RFC1042,
    RTKSW_FRAME_TYPE_END
} rtksw_vlan_protoVlan_frameType_t;

/* Protocol-and-port-based Vlan structure */
typedef struct rtksw_vlan_protoAndPortInfo_s
{
    rtksw_uint32                         proto_type;
    rtksw_vlan_protoVlan_frameType_t frame_type;
    rtksw_vlan_t                     cvid;
    rtksw_pri_t                     cpri;
}rtksw_vlan_protoAndPortInfo_t;

/* tagged mode of VLAN - reference realtek private specification */
typedef enum rtksw_vlan_tagMode_e
{
    RTKSW_VLAN_TAG_MODE_ORIGINAL = 0,
    RTKSW_VLAN_TAG_MODE_KEEP_FORMAT,
    RTKSW_VLAN_TAG_MODE_PRI,
    RTKSW_VLAN_TAG_MODE_REAL_KEEP_FORMAT,
    RTKSW_VLAN_TAG_MODE_END
} rtksw_vlan_tagMode_t;

typedef enum rtksw_vlan_resVidAction_e
{
    RTKSW_RESVID_ACTION_UNTAG = 0,
    RTKSW_RESVID_ACTION_TAG,
    RTKSW_RESVID_ACTION_END
}
rtksw_vlan_resVidAction_t;

/* Function Name:
 *      rtksw_vlan_init
 * Description:
 *      Initialize VLAN.
 * Input:
 *      unit        - Unit ID
 *      None
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
extern rtksw_api_ret_t rtksw_vlan_init(rtksw_uint32 unit);

/* Function Name:
 *      rtksw_vlan_set
 * Description:
 *      Set a VLAN entry.
 * Input:
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
extern rtksw_api_ret_t rtksw_vlan_set(rtksw_uint32 unit, rtksw_vlan_t vid, rtksw_vlan_cfg_t *pVlanCfg);

/* Function Name:
 *      rtksw_vlan_get
 * Description:
 *      Get a VLAN entry.
 * Input:
 *      unit        - Unit ID
 *      vid         - VLAN ID to configure.
 * Output:
 *      pVlanCfg    - VLAN Configuration
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_VLAN_VID     - Invalid VID parameter.
 * Note:
 *
 */
extern rtksw_api_ret_t rtksw_vlan_get(rtksw_uint32 unit, rtksw_vlan_t vid, rtksw_vlan_cfg_t *pVlanCfg);

/* Function Name:
 *      rtksw_vlan_egrFilterEnable_set
 * Description:
 *      Set VLAN egress filter.
 * Input:
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
extern rtksw_api_ret_t rtksw_vlan_egrFilterEnable_set(rtksw_uint32 unit, rtksw_enable_t egrFilter);

/* Function Name:
 *      rtksw_vlan_egrFilterEnable_get
 * Description:
 *      Get VLAN egress filter.
 * Input:
 *      unit        - Unit ID
 *      pEgrFilter - Egress filtering
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
extern rtksw_api_ret_t rtksw_vlan_egrFilterEnable_get(rtksw_uint32 unit, rtksw_enable_t *pEgrFilter);

/* Function Name:
 *      rtksw_vlan_mbrCfg_set
 * Description:
 *      Set a VLAN Member Configuration entry by index.
 * Input:
 *      unit        - Unit ID
 *      idx         - Index of VLAN Member Configuration.
 *      pMbrcfg     - VLAN member Configuration.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_VLAN_VID     - Invalid VID parameter.
 * Note:
 *     Set a VLAN Member Configuration entry by index.
 */
extern rtksw_api_ret_t rtksw_vlan_mbrCfg_set(rtksw_uint32 unit, rtksw_uint32 idx, rtksw_vlan_mbrcfg_t *pMbrcfg);

/* Function Name:
 *      rtksw_vlan_mbrCfg_get
 * Description:
 *      Get a VLAN Member Configuration entry by index.
 * Input:
 *      unit        - Unit ID
 *      idx         - Index of VLAN Member Configuration.
 * Output:
 *      pMbrcfg     - VLAN member Configuration.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_VLAN_VID     - Invalid VID parameter.
 * Note:
 *     Get a VLAN Member Configuration entry by index.
 */
extern rtksw_api_ret_t rtksw_vlan_mbrCfg_get(rtksw_uint32 unit, rtksw_uint32 idx, rtksw_vlan_mbrcfg_t *pMbrcfg);

/* Function Name:
 *     rtksw_vlan_portPvid_set
 * Description:
 *      Set port to specified VLAN ID(PVID).
 * Input:
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
extern rtksw_api_ret_t rtksw_vlan_portPvid_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_vlan_t pvid, rtksw_pri_t priority);

/* Function Name:
 *      rtksw_vlan_portPvid_get
 * Description:
 *      Get VLAN ID(PVID) on specified port.
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
 * Output:
 *      pPvid       - Specified VLAN ID.
 *      pPriority   - 802.1p priority for the PVID.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *     The API can get the PVID and 802.1p priority for the PVID of Port-based VLAN.
 */
extern rtksw_api_ret_t rtksw_vlan_portPvid_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_vlan_t *pPvid, rtksw_pri_t *pPriority);

/* Function Name:
 *      rtksw_vlan_portIgrFilterEnable_set
 * Description:
 *      Set VLAN ingress for each port.
 * Input:
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
extern rtksw_api_ret_t rtksw_vlan_portIgrFilterEnable_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t igr_filter);

/* Function Name:
 *      rtksw_vlan_portIgrFilterEnable_get
 * Description:
 *      Get VLAN Ingress Filter
 * Input:
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
extern rtksw_api_ret_t rtksw_vlan_portIgrFilterEnable_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t *pIgr_filter);

/* Function Name:
 *      rtksw_vlan_portAcceptFrameType_set
 * Description:
 *      Set VLAN accept_frame_type
 * Input:
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
extern rtksw_api_ret_t rtksw_vlan_portAcceptFrameType_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_vlan_acceptFrameType_t accept_frame_type);

/* Function Name:
 *      rtksw_vlan_portAcceptFrameType_get
 * Description:
 *      Get VLAN accept_frame_type
 * Input:
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
extern rtksw_api_ret_t rtksw_vlan_portAcceptFrameType_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_vlan_acceptFrameType_t *pAccept_frame_type);

/* Function Name:
 *      rtksw_vlan_protoAndPortBasedVlan_add
 * Description:
 *      Add the protocol-and-port-based vlan to the specified port of device.
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
 *      pInfo       - Protocol and port based VLAN configuration information.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_ID          - Invalid port number.
 *      RT_ERR_VLAN_VID         - Invalid VID parameter.
 *      RT_ERR_VLAN_PRIORITY    - Invalid priority.
 *      RT_ERR_TBL_FULL         - Table is full.
 *      RT_ERR_OUT_OF_RANGE     - input out of range.
 * Note:
 *      The incoming packet which match the protocol-and-port-based vlan will use the configure vid for ingress pipeline
 *      The frame type is shown in the following:
 *      - RTKSW_FRAME_TYPE_ETHERNET
 *      - RTKSW_FRAME_TYPE_RFC1042
 *      - RTKSW_FRAME_TYPE_LLCOTHER
 */
extern rtksw_api_ret_t rtksw_vlan_protoAndPortBasedVlan_add(rtksw_uint32 unit, rtksw_port_t port, rtksw_vlan_protoAndPortInfo_t *pInfo);

/* Function Name:
 *      rtksw_vlan_protoAndPortBasedVlan_get
 * Description:
 *      Get the protocol-and-port-based vlan to the specified port of device.
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
 *      proto_type  - protocol-and-port-based vlan protocol type.
 *      frame_type  - protocol-and-port-based vlan frame type.
 * Output:
 *      pInfo - Protocol and port based VLAN configuration information.
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_ID          - Invalid port number.
 *      RT_ERR_OUT_OF_RANGE     - input out of range.
 *      RT_ERR_TBL_FULL         - Table is full.
 * Note:
 *     The incoming packet which match the protocol-and-port-based vlan will use the configure vid for ingress pipeline
 *     The frame type is shown in the following:
 *      - RTKSW_FRAME_TYPE_ETHERNET
 *      - RTKSW_FRAME_TYPE_RFC1042
 *      - RTKSW_FRAME_TYPE_LLCOTHER
 */
extern rtksw_api_ret_t rtksw_vlan_protoAndPortBasedVlan_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_vlan_proto_type_t proto_type, rtksw_vlan_protoVlan_frameType_t frame_type, rtksw_vlan_protoAndPortInfo_t *pInfo);

/* Function Name:
 *      rtksw_vlan_protoAndPortBasedVlan_del
 * Description:
 *      Delete the protocol-and-port-based vlan from the specified port of device.
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
 *      proto_type  - protocol-and-port-based vlan protocol type.
 *      frame_type  - protocol-and-port-based vlan frame type.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_ID          - Invalid port number.
 *      RT_ERR_OUT_OF_RANGE     - input out of range.
 *      RT_ERR_TBL_FULL         - Table is full.
 * Note:
 *     The incoming packet which match the protocol-and-port-based vlan will use the configure vid for ingress pipeline
 *     The frame type is shown in the following:
 *      - RTKSW_FRAME_TYPE_ETHERNET
 *      - RTKSW_FRAME_TYPE_RFC1042
 *      - RTKSW_FRAME_TYPE_LLCOTHER
 */
extern rtksw_api_ret_t rtksw_vlan_protoAndPortBasedVlan_del(rtksw_uint32 unit, rtksw_port_t port, rtksw_vlan_proto_type_t proto_type, rtksw_vlan_protoVlan_frameType_t frame_type);

/* Function Name:
 *      rtksw_vlan_protoAndPortBasedVlan_delAll
 * Description:
 *     Delete all protocol-and-port-based vlans from the specified port of device.
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_ID          - Invalid port number.
 *      RT_ERR_OUT_OF_RANGE     - input out of range.
 * Note:
 *     The incoming packet which match the protocol-and-port-based vlan will use the configure vid for ingress pipeline
 *     Delete all flow table protocol-and-port-based vlan entries.
 */
extern rtksw_api_ret_t rtksw_vlan_protoAndPortBasedVlan_delAll(rtksw_uint32 unit, rtksw_port_t port);

/* Function Name:
 *      rtksw_vlan_tagMode_set
 * Description:
 *      Set CVLAN egress tag mode
 * Input:
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
extern rtksw_api_ret_t rtksw_vlan_tagMode_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_vlan_tagMode_t tag_mode);

/* Function Name:
 *      rtksw_vlan_tagMode_get
 * Description:
 *      Get CVLAN egress tag mode
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
 * Output:
 *      pTag_mode   - The egress tag mode.
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
extern rtksw_api_ret_t rtksw_vlan_tagMode_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_vlan_tagMode_t *pTag_mode);

/* Function Name:
 *      rtksw_vlan_transparent_set
 * Description:
 *      Set VLAN transparent mode
 * Input:
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
extern rtksw_api_ret_t rtksw_vlan_transparent_set(rtksw_uint32 unit, rtksw_port_t egr_port, rtksw_portmask_t *pIgr_pmask);

/* Function Name:
 *      rtksw_vlan_transparent_get
 * Description:
 *      Get VLAN transparent mode
 * Input:
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
extern rtksw_api_ret_t rtksw_vlan_transparent_get(rtksw_uint32 unit, rtksw_port_t egr_port, rtksw_portmask_t *pIgr_pmask);

/* Function Name:
 *      rtksw_vlan_keep_set
 * Description:
 *      Set VLAN egress keep mode
 * Input:
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
extern rtksw_api_ret_t rtksw_vlan_keep_set(rtksw_uint32 unit, rtksw_port_t egr_port, rtksw_portmask_t *pIgr_pmask);

/* Function Name:
 *      rtksw_vlan_keep_get
 * Description:
 *      Get VLAN egress keep mode
 * Input:
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
extern rtksw_api_ret_t rtksw_vlan_keep_get(rtksw_uint32 unit, rtksw_port_t egr_port, rtksw_portmask_t *pIgr_pmask);

/* Function Name:
 *      rtksw_vlan_stg_set
 * Description:
 *      Set spanning tree group instance of the vlan to the specified device
 * Input:
 *      unit        - Unit ID
 *      vid         - Specified VLAN ID.
 *      stg         - spanning tree group instance.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_MSTI         - Invalid msti parameter
 *      RT_ERR_INPUT        - Invalid input parameter.
 *      RT_ERR_VLAN_VID     - Invalid VID parameter.
 * Note:
 *      The API can set spanning tree group instance of the vlan to the specified device.
 */
extern rtksw_api_ret_t rtksw_vlan_stg_set(rtksw_uint32 unit, rtksw_vlan_t vid, rtksw_stp_msti_id_t stg);

/* Function Name:
 *      rtksw_vlan_stg_get
 * Description:
 *      Get spanning tree group instance of the vlan to the specified device
 * Input:
 *      unit        - Unit ID
 *      vid         - Specified VLAN ID.
 * Output:
 *      pStg        - spanning tree group instance.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_VLAN_VID     - Invalid VID parameter.
 * Note:
 *      The API can get spanning tree group instance of the vlan to the specified device.
 */
extern rtksw_api_ret_t rtksw_vlan_stg_get(rtksw_uint32 unit, rtksw_vlan_t vid, rtksw_stp_msti_id_t *pStg);

/* Function Name:
 *      rtksw_vlan_portFid_set
 * Description:
 *      Set port-based filtering database
 * Input:
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
extern rtksw_api_ret_t rtksw_vlan_portFid_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t enable, rtksw_fid_t fid);

/* Function Name:
 *      rtksw_vlan_portFid_get
 * Description:
 *      Get port-based filtering database
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
 * Output:
 *      pEnable     - ebable port-based FID
 *      pFid        - Specified filtering database.
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
extern rtksw_api_ret_t rtksw_vlan_portFid_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t *pEnable, rtksw_fid_t *pFid);

/* Function Name:
 *      rtksw_vlan_UntagDscpPriorityEnable_set
 * Description:
 *      Set Untag DSCP priority assign
 * Input:
 *      unit        - Unit ID
 *      enable      - state of Untag DSCP priority assign
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_ENABLE          - Invalid input parameters.
 * Note:
 *
 */
extern rtksw_api_ret_t rtksw_vlan_UntagDscpPriorityEnable_set(rtksw_uint32 unit, rtksw_enable_t enable);

/* Function Name:
 *      rtksw_vlan_UntagDscpPriorityEnable_get
 * Description:
 *      Get Untag DSCP priority assign
 * Input:
 *      unit        - Unit ID
 * Output:
 *      pEnable - state of Untag DSCP priority assign
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_NULL_POINTER    - Null pointer
 * Note:
 *
 */
extern rtksw_api_ret_t rtksw_vlan_UntagDscpPriorityEnable_get(rtksw_uint32 unit, rtksw_enable_t *pEnable);

/* Function Name:
 *      rtksw_stp_mstpState_set
 * Description:
 *      Configure spanning tree state per each port.
 * Input:
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
extern rtksw_api_ret_t rtksw_stp_mstpState_set(rtksw_uint32 unit, rtksw_stp_msti_id_t msti, rtksw_port_t port, rtksw_stp_state_t stp_state);

/* Function Name:
 *      rtksw_stp_mstpState_get
 * Description:
 *      Get spanning tree state per each port.
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
 *      msti        - Multiple spanning tree instance.
 * Output:
 *      pStp_state  - Spanning tree state for msti
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
extern rtksw_api_ret_t rtksw_stp_mstpState_get(rtksw_uint32 unit, rtksw_stp_msti_id_t msti, rtksw_port_t port, rtksw_stp_state_t *pStp_state);

/* Function Name:
 *      rtksw_vlan_reservedVidAction_set
 * Description:
 *      Set Action of VLAN ID = 0 & 4095 tagged packet
 * Input:
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
extern rtksw_api_ret_t rtksw_vlan_reservedVidAction_set(rtksw_uint32 unit, rtksw_vlan_resVidAction_t action_vid0, rtksw_vlan_resVidAction_t action_vid4095);

/* Function Name:
 *      rtksw_vlan_reservedVidAction_get
 * Description:
 *      Get Action of VLAN ID = 0 & 4095 tagged packet
 * Input:
 *      unit                - Unit ID
 *      pAction_vid0        - Action for VID 0.
 *      pAction_vid4095     - Action for VID 4095.
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
extern rtksw_api_ret_t rtksw_vlan_reservedVidAction_get(rtksw_uint32 unit, rtksw_vlan_resVidAction_t *pAction_vid0, rtksw_vlan_resVidAction_t *pAction_vid4095);

/* Function Name:
 *      rtksw_vlan_realKeepRemarkEnable_set
 * Description:
 *      Set Real keep 1p remarking feature
 * Input:
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
extern rtksw_api_ret_t rtksw_vlan_realKeepRemarkEnable_set(rtksw_uint32 unit, rtksw_enable_t enabled);

/* Function Name:
 *      rtksw_vlan_realKeepRemarkEnable_get
 * Description:
 *      Get Real keep 1p remarking feature
 * Input:
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
extern rtksw_api_ret_t rtksw_vlan_realKeepRemarkEnable_get(rtksw_uint32 unit, rtksw_enable_t *pEnabled);

/* Function Name:
 *      rtksw_vlan_reset
 * Description:
 *      Reset VLAN
 * Input:
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
extern rtksw_api_ret_t rtksw_vlan_reset(rtksw_uint32 unit);

#if defined(CONFIG_COMPATIBLE_V1_4)

#define rtk_vlan_init(void)                                                     rtksw_vlan_init(0)
#define rtk_vlan_set(vid, pVlanCfg)                                             rtksw_vlan_set(0, vid, pVlanCfg)
#define rtk_vlan_get(vid, pVlanCfg)                                             rtksw_vlan_get(0, vid, pVlanCfg)
#define rtk_vlan_egrFilterEnable_set(egrFilter)                                 rtksw_vlan_egrFilterEnable_set(0, egrFilter)
#define rtk_vlan_egrFilterEnable_get(pEgrFilter)                                rtksw_vlan_egrFilterEnable_get(0, pEgrFilter)
#define rtk_vlan_mbrCfg_set(idx, pMbrcfg)                                       rtksw_vlan_mbrCfg_set(0, idx, pMbrcfg)
#define rtk_vlan_mbrCfg_get(idx, pMbrcfg)                                       rtksw_vlan_mbrCfg_get(0, idx, pMbrcfg)
#define rtk_vlan_portPvid_set(port, pvid, priority)                             rtksw_vlan_portPvid_set(0, port, pvid, priority)
#define rtk_vlan_portPvid_get(port, pPvid, pPriority)                           rtksw_vlan_portPvid_get(0, port, pPvid, pPriority)
#define rtk_vlan_portIgrFilterEnable_set(port, igr_filter)                      rtksw_vlan_portIgrFilterEnable_set(0, port, igr_filter)
#define rtk_vlan_portIgrFilterEnable_get(port, pIgr_filter)                     rtksw_vlan_portIgrFilterEnable_get(0, port, pIgr_filter)
#define rtk_vlan_portAcceptFrameType_set(port, accept_frame_type)               rtksw_vlan_portAcceptFrameType_set(0, port, accept_frame_type)
#define rtk_vlan_portAcceptFrameType_get(port, pAccept_frame_type)              rtksw_vlan_portAcceptFrameType_get(0, port, pAccept_frame_type)
#define rtk_vlan_protoAndPortBasedVlan_add(port, pInfo)                         rtksw_vlan_protoAndPortBasedVlan_add(0, port, pInfo)
#define rtk_vlan_protoAndPortBasedVlan_get(port, proto_type, frame_type, pInfo) rtksw_vlan_protoAndPortBasedVlan_get(0, port, proto_type, frame_type, pInfo)
#define rtk_vlan_protoAndPortBasedVlan_del(port, proto_type, frame_type)        rtksw_vlan_protoAndPortBasedVlan_del(0, port, proto_type, frame_type)
#define rtk_vlan_protoAndPortBasedVlan_delAll(port)                             rtksw_vlan_protoAndPortBasedVlan_delAll(0, port)
#define rtk_vlan_tagMode_set(port, tag_mode)                                    rtksw_vlan_tagMode_set(0, port, tag_mode)
#define rtk_vlan_tagMode_get(port, pTag_mode)                                   rtksw_vlan_tagMode_get(0, port, pTag_mode)
#define rtk_vlan_transparent_set(egr_port, pIgr_pmask)                          rtksw_vlan_transparent_set(0, egr_port, pIgr_pmask)
#define rtk_vlan_transparent_get(egr_port, pIgr_pmask)                          rtksw_vlan_transparent_get(0, egr_port, pIgr_pmask)
#define rtk_vlan_keep_set(egr_port, pIgr_pmask)                                 rtksw_vlan_keep_set(0, egr_port, pIgr_pmask)
#define rtk_vlan_keep_get(egr_port, pIgr_pmask)                                 rtksw_vlan_keep_get(0, egr_port, pIgr_pmask)
#define rtk_vlan_stg_set(vid, stg)                                              rtksw_vlan_stg_set(0, vid, stg)
#define rtk_vlan_stg_get(vid, pStg)                                             rtksw_vlan_stg_get(0, vid, pStg)
#define rtk_vlan_portFid_set(port, enable, fid)                                 rtksw_vlan_portFid_set(0, port, enable, fid)
#define rtk_vlan_portFid_get(port, pEnable, pFid)                               rtksw_vlan_portFid_get(0, port, pEnable, pFid)
#define rtk_vlan_UntagDscpPriorityEnable_set(enable)                            rtksw_vlan_UntagDscpPriorityEnable_set(0, enable)
#define rtk_vlan_UntagDscpPriorityEnable_get(pEnable)                           rtksw_vlan_UntagDscpPriorityEnable_get(0, pEnable)
#define rtk_stp_mstpState_set(msti, port, stp_state)                            rtksw_stp_mstpState_set(0, msti, port, stp_state)
#define rtk_stp_mstpState_get(msti, port, pStp_state)                           rtksw_stp_mstpState_get(0, msti, port, pStp_state)
#define rtk_vlan_reservedVidAction_set(action_vid0, action_vid4095)             rtksw_vlan_reservedVidAction_set(0, action_vid0, action_vid4095)
#define rtk_vlan_reservedVidAction_get(pAction_vid0, pAction_vid4095)           rtksw_vlan_reservedVidAction_get(0, pAction_vid0, pAction_vid4095)
#define rtk_vlan_realKeepRemarkEnable_set(enabled)                              rtksw_vlan_realKeepRemarkEnable_set(0, enabled)
#define rtk_vlan_realKeepRemarkEnable_get(pEnabled)                             rtksw_vlan_realKeepRemarkEnable_get(0, pEnabled)
#define rtk_vlan_reset(void)                                                    rtksw_vlan_reset(0)

#define ACCEPT_FRAME_TYPE_ALL           RTKSW_ACCEPT_FRAME_TYPE_ALL
#define ACCEPT_FRAME_TYPE_TAG_ONLY      RTKSW_ACCEPT_FRAME_TYPE_TAG_ONLY
#define ACCEPT_FRAME_TYPE_UNTAG_ONLY    RTKSW_ACCEPT_FRAME_TYPE_UNTAG_ONLY

#define VLAN_TAG_MODE_ORIGINAL          RTKSW_VLAN_TAG_MODE_ORIGINAL
#define VLAN_TAG_MODE_KEEP_FORMAT       RTKSW_VLAN_TAG_MODE_KEEP_FORMAT
#define VLAN_TAG_MODE_PRI               RTKSW_VLAN_TAG_MODE_PRI
#define VLAN_TAG_MODE_REAL_KEEP_FORMAT  RTKSW_VLAN_TAG_MODE_REAL_KEEP_FORMAT

#define FRAME_TYPE_ETHERNET             RTKSW_FRAME_TYPE_ETHERNET
#define FRAME_TYPE_LLCOTHER             RTKSW_FRAME_TYPE_LLCOTHER
#define FRAME_TYPE_RFC1042              RTKSW_FRAME_TYPE_RFC1042

#define RESVID_ACTION_UNTAG             RTKSW_RESVID_ACTION_UNTAG   
#define RESVID_ACTION_TAG               RTKSW_RESVID_ACTION_TAG 

#define STP_STATE_DISABLED              RTKSW_STP_STATE_DISABLED
#define STP_STATE_BLOCKING              RTKSW_STP_STATE_BLOCKING
#define STP_STATE_LEARNING              RTKSW_STP_STATE_LEARNING
#define STP_STATE_FORWARDING            RTKSW_STP_STATE_FORWARDING

#define rtk_vlan_cfg_t                  rtksw_vlan_cfg_t
#define rtk_vlan_mbrcfg_t               rtksw_vlan_mbrcfg_t
#define rtk_stp_msti_id_t               rtksw_stp_msti_id_t
#define rtk_stp_state_t                 rtksw_stp_state_t
#define rtk_vlan_proto_type_t           rtksw_vlan_proto_type_t
#define rtk_vlan_acceptFrameType_t      rtksw_vlan_acceptFrameType_t
#define rtk_vlan_protoVlan_frameType_t  rtksw_vlan_protoVlan_frameType_t
#define rtk_vlan_protoAndPortInfo_t     rtksw_vlan_protoAndPortInfo_t
#define rtk_vlan_tagMode_t              rtksw_vlan_tagMode_t
#define rtk_vlan_resVidAction_t         rtksw_vlan_resVidAction_t

#endif /* CONFIG_COMPATIBLE_V1_4 */

#endif /* __RTKSW_API_VLAN_H__ */
