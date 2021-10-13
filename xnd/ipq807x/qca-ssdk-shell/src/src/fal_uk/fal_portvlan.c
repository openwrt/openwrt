/*
 * Copyright (c) 2014, 2016-2017, The Linux Foundation. All rights reserved.
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */



#include "sw.h"
#include "sw_ioctl.h"
#include "fal_portvlan.h"
#include "fal_uk_if.h"

sw_error_t
fal_port_1qmode_set(a_uint32_t dev_id, fal_port_t port_id,
                    fal_pt_1qmode_t port_1qmode)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_ING_MODE_SET, dev_id, port_id,
                    (a_uint32_t) port_1qmode);
    return rv;
}

sw_error_t
fal_port_1qmode_get(a_uint32_t dev_id, fal_port_t port_id,
                    fal_pt_1qmode_t * pport_1qmode)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_ING_MODE_GET, dev_id, port_id,
                    pport_1qmode);
    return rv;
}

sw_error_t
fal_port_egvlanmode_set(a_uint32_t dev_id, fal_port_t port_id,
                        fal_pt_1q_egmode_t port_egvlanmode)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_EG_MODE_SET, dev_id, port_id,
                    (a_uint32_t) port_egvlanmode);
    return rv;
}

sw_error_t
fal_port_egvlanmode_get(a_uint32_t dev_id, fal_port_t port_id,
                        fal_pt_1q_egmode_t * pport_egvlanmode)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_EG_MODE_GET, dev_id, port_id,
                    pport_egvlanmode);
    return rv;
}

sw_error_t
fal_portvlan_member_add(a_uint32_t dev_id, fal_port_t port_id,
                        a_uint32_t mem_port_id)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_VLAN_MEM_ADD, dev_id, port_id,
                    (a_uint32_t) mem_port_id);
    return rv;
}

sw_error_t
fal_portvlan_member_del(a_uint32_t dev_id, fal_port_t port_id,
                        a_uint32_t mem_port_id)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_VLAN_MEM_DEL, dev_id, port_id,
                    (a_uint32_t) mem_port_id);
    return rv;
}

sw_error_t
fal_portvlan_member_update(a_uint32_t dev_id, fal_port_t port_id,
                           fal_pbmp_t mem_port_map)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_VLAN_MEM_UPDATE, dev_id, port_id,
                    (a_uint32_t) mem_port_map);
    return rv;
}

sw_error_t
fal_portvlan_member_get(a_uint32_t dev_id, fal_port_t port_id,
                        fal_pbmp_t * mem_port_map)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_VLAN_MEM_GET, dev_id, port_id,
                    mem_port_map);
    return rv;
}

sw_error_t
fal_port_default_vid_set(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t vid)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_DEF_VID_SET, dev_id, port_id,
                    vid);
    return rv;
}

sw_error_t
fal_port_default_vid_get(a_uint32_t dev_id, fal_port_t port_id,
                         a_uint32_t * vid)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_DEF_VID_GET, dev_id, port_id,
                    vid);
    return rv;
}

sw_error_t
fal_port_force_default_vid_set(a_uint32_t dev_id, fal_port_t port_id,
                               a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_FORCE_DEF_VID_SET, dev_id, port_id,
                    (a_uint32_t) enable);
    return rv;
}

sw_error_t
fal_port_force_default_vid_get(a_uint32_t dev_id, fal_port_t port_id,
                               a_bool_t * enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_FORCE_DEF_VID_GET, dev_id, port_id,
                    enable);
    return rv;
}

sw_error_t
fal_port_force_portvlan_set(a_uint32_t dev_id, fal_port_t port_id,
                            a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_FORCE_PORTVLAN_SET, dev_id, port_id,
                    (a_uint32_t) enable);
    return rv;
}

sw_error_t
fal_port_force_portvlan_get(a_uint32_t dev_id, fal_port_t port_id,
                            a_bool_t * enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_FORCE_PORTVLAN_GET, dev_id, port_id,
                    enable);
    return rv;
}

sw_error_t
fal_port_nestvlan_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_NESTVLAN_SET, dev_id, port_id,
                    (a_uint32_t) enable);
    return rv;
}

sw_error_t
fal_port_nestvlan_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_NESTVLAN_GET, dev_id, port_id,
                    enable);
    return rv;
}

sw_error_t
fal_nestvlan_tpid_set(a_uint32_t dev_id, a_uint32_t tpid)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_NESTVLAN_TPID_SET, dev_id, tpid);
    return rv;
}

sw_error_t
fal_nestvlan_tpid_get(a_uint32_t dev_id, a_uint32_t * tpid)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_NESTVLAN_TPID_GET, dev_id, tpid);
    return rv;
}

sw_error_t
fal_port_invlan_mode_set(a_uint32_t dev_id, fal_port_t port_id,
                         fal_pt_invlan_mode_t mode)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_IN_VLAN_MODE_SET, dev_id, port_id, (a_uint32_t) mode);
    return rv;
}

sw_error_t
fal_port_invlan_mode_get(a_uint32_t dev_id, fal_port_t port_id,
                         fal_pt_invlan_mode_t * mode)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_IN_VLAN_MODE_GET, dev_id, port_id, mode);
    return rv;
}

sw_error_t
fal_port_tls_set(a_uint32_t dev_id, fal_port_t port_id,
                 a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_TLS_SET, dev_id, port_id, (a_uint32_t) enable);
    return rv;
}

sw_error_t
fal_port_tls_get(a_uint32_t dev_id, fal_port_t port_id,
                 a_bool_t * enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_TLS_GET, dev_id, port_id, enable);
    return rv;
}

sw_error_t
fal_port_pri_propagation_set(a_uint32_t dev_id, fal_port_t port_id,
                             a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_PRI_PROPAGATION_SET, dev_id, port_id, (a_uint32_t) enable);
    return rv;
}

sw_error_t
fal_port_pri_propagation_get(a_uint32_t dev_id, fal_port_t port_id,
                             a_bool_t * enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_PRI_PROPAGATION_GET, dev_id, port_id, enable);
    return rv;
}

sw_error_t
fal_port_default_svid_set(a_uint32_t dev_id, fal_port_t port_id,
                          a_uint32_t vid)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_DEF_SVID_SET, dev_id, port_id, vid);
    return rv;
}

sw_error_t
fal_port_default_svid_get(a_uint32_t dev_id, fal_port_t port_id,
                          a_uint32_t * vid)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_DEF_SVID_GET, dev_id, port_id, vid);
    return rv;
}

sw_error_t
fal_port_default_cvid_set(a_uint32_t dev_id, fal_port_t port_id,
                          a_uint32_t vid)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_DEF_CVID_SET, dev_id, port_id, vid);
    return rv;
}

sw_error_t
fal_port_default_cvid_get(a_uint32_t dev_id, fal_port_t port_id,
                          a_uint32_t * vid)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_DEF_CVID_GET, dev_id, port_id, vid);
    return rv;
}

sw_error_t
fal_port_vlan_propagation_set(a_uint32_t dev_id, fal_port_t port_id,
                              fal_vlan_propagation_mode_t mode)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_VLAN_PROPAGATION_SET, dev_id, port_id, (a_uint32_t)mode);
    return rv;
}

sw_error_t
fal_port_vlan_propagation_get(a_uint32_t dev_id, fal_port_t port_id,
                              fal_vlan_propagation_mode_t * mode)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_VLAN_PROPAGATION_GET, dev_id, port_id, mode);
    return rv;
}

sw_error_t
fal_port_vlan_trans_add(a_uint32_t dev_id, fal_port_t port_id, fal_vlan_trans_entry_t *entry)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_VLAN_TRANS_ADD, dev_id, port_id, entry);
    return rv;
}

sw_error_t
fal_port_vlan_trans_del(a_uint32_t dev_id, fal_port_t port_id, fal_vlan_trans_entry_t *entry)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_VLAN_TRANS_DEL, dev_id, port_id, entry);
    return rv;
}

sw_error_t
fal_port_vlan_trans_get(a_uint32_t dev_id, fal_port_t port_id, fal_vlan_trans_entry_t *entry)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_VLAN_TRANS_GET, dev_id, port_id, entry);
    return rv;
}

sw_error_t
fal_qinq_mode_set(a_uint32_t dev_id, fal_qinq_mode_t mode)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QINQ_MODE_SET, dev_id, (a_uint32_t)mode);
    return rv;
}

sw_error_t
fal_qinq_mode_get(a_uint32_t dev_id, fal_qinq_mode_t * mode)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QINQ_MODE_GET, dev_id, mode);
    return rv;
}

sw_error_t
fal_port_qinq_role_set(a_uint32_t dev_id, fal_port_t port_id, fal_qinq_port_role_t role)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_QINQ_ROLE_SET, dev_id, port_id, (a_uint32_t)role);
    return rv;
}

sw_error_t
fal_port_qinq_role_get(a_uint32_t dev_id, fal_port_t port_id, fal_qinq_port_role_t * role)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_QINQ_ROLE_GET, dev_id, port_id, role);
    return rv;
}

sw_error_t
fal_port_vlan_trans_iterate(a_uint32_t dev_id, fal_port_t port_id,
                            a_uint32_t * iterator, fal_vlan_trans_entry_t * entry)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_VLAN_TRANS_ITERATE, dev_id, port_id,
                    iterator,entry);
    return rv;
}

sw_error_t
fal_port_mac_vlan_xlt_set(a_uint32_t dev_id, fal_port_t port_id,
                          a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_MAC_VLAN_XLT_SET, dev_id, port_id, (a_uint32_t)enable);
    return rv;
}

sw_error_t
fal_port_mac_vlan_xlt_get(a_uint32_t dev_id, fal_port_t port_id,
                          a_bool_t * enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_MAC_VLAN_XLT_GET, dev_id, port_id, enable);
    return rv;
}

sw_error_t
fal_netisolate_set(a_uint32_t dev_id, a_uint32_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_NETISOLATE_SET, dev_id, enable);
    return rv;
}

sw_error_t
fal_netisolate_get(a_uint32_t dev_id, a_uint32_t * enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_NETISOLATE_GET, dev_id, enable);
    return rv;
}

sw_error_t
fal_eg_trans_filter_bypass_en_set(a_uint32_t dev_id, a_uint32_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_EG_FLTR_BYPASS_EN_SET, dev_id, enable);
    return rv;
}

sw_error_t
fal_eg_trans_filter_bypass_en_get(a_uint32_t dev_id, a_uint32_t * enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_EG_FLTR_BYPASS_EN_GET, dev_id, enable);
    return rv;
}

sw_error_t
fal_port_vrf_id_set(a_uint32_t dev_id, fal_port_t port_id,
                              a_uint32_t vrf_id)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_VRF_ID_SET, dev_id, port_id, vrf_id);
    return rv;
}

sw_error_t
fal_port_vrf_id_get(a_uint32_t dev_id, fal_port_t port_id,
                              a_uint32_t * vrf_id)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_VRF_ID_GET, dev_id, port_id, vrf_id);
    return rv;
}

sw_error_t
fal_global_qinq_mode_set(a_uint32_t dev_id, fal_global_qinq_mode_t *mode)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_GLOBAL_QINQ_MODE_SET, dev_id, mode);
    return rv;
}

sw_error_t
fal_global_qinq_mode_get(a_uint32_t dev_id, fal_global_qinq_mode_t *mode)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_GLOBAL_QINQ_MODE_GET, dev_id, mode);
    return rv;
}

sw_error_t
fal_port_qinq_mode_set(a_uint32_t dev_id, fal_port_t port_id, fal_port_qinq_role_t *mode)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PORT_QINQ_MODE_SET, dev_id, port_id, mode);
    return rv;
}

sw_error_t
fal_port_qinq_mode_get(a_uint32_t dev_id, fal_port_t port_id, fal_port_qinq_role_t *mode)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PORT_QINQ_MODE_GET, dev_id, port_id, mode);
    return rv;
}

sw_error_t
fal_ingress_tpid_set(a_uint32_t dev_id, fal_tpid_t *tpid)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_TPID_SET, dev_id, tpid);
    return rv;
}

sw_error_t
fal_ingress_tpid_get(a_uint32_t dev_id, fal_tpid_t *tpid)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_TPID_GET, dev_id, tpid);
    return rv;
}

sw_error_t
fal_egress_tpid_set(a_uint32_t dev_id, fal_tpid_t *tpid)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_EGRESS_TPID_SET, dev_id, tpid);
    return rv;
}

sw_error_t
fal_egress_tpid_get(a_uint32_t dev_id, fal_tpid_t *tpid)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_EGRESS_TPID_GET, dev_id, tpid);
    return rv;
}

sw_error_t
fal_port_ingress_vlan_filter_set(a_uint32_t dev_id, fal_port_t port_id, fal_ingress_vlan_filter_t *filter)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_INGRESS_VLAN_FILTER_SET, dev_id, port_id, filter);
    return rv;
}

sw_error_t
fal_port_ingress_vlan_filter_get(a_uint32_t dev_id, fal_port_t port_id, fal_ingress_vlan_filter_t *filter)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_INGRESS_VLAN_FILTER_GET, dev_id, port_id, filter);
    return rv;
}

sw_error_t
fal_port_default_vlantag_set(a_uint32_t dev_id, fal_port_t port_id,
                             fal_port_vlan_direction_t direction, fal_port_default_vid_enable_t *default_vid_en,
                             fal_port_vlan_tag_t *default_tag)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_DEFAULT_VLANTAG_SET, dev_id, port_id, (a_uint32_t) direction,
			default_vid_en, default_tag);
    return rv;
}

sw_error_t
fal_port_default_vlantag_get(a_uint32_t dev_id, fal_port_t port_id,
                             fal_port_vlan_direction_t direction, fal_port_default_vid_enable_t *default_vid_en,
                             fal_port_vlan_tag_t *default_tag)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_DEFAULT_VLANTAG_GET, dev_id, port_id, (a_uint32_t) direction, 
			default_vid_en, default_tag);
    return rv;
}

sw_error_t
fal_port_tag_propagation_set(a_uint32_t dev_id, fal_port_t port_id, fal_port_vlan_direction_t direction,
                             fal_vlantag_propagation_t *prop)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_TAG_PROPAGATION_SET, dev_id, port_id, (a_uint32_t) direction, prop);
    return rv;
}

sw_error_t
fal_port_tag_propagation_get(a_uint32_t dev_id, fal_port_t port_id, fal_port_vlan_direction_t direction,
                             fal_vlantag_propagation_t *prop)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_TAG_PROPAGATION_GET, dev_id, port_id, (a_uint32_t) direction, prop);
    return rv;
}

sw_error_t
fal_port_vlan_xlt_miss_cmd_set(a_uint32_t dev_id, fal_port_t port_id,
                             fal_fwd_cmd_t cmd)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_VLAN_XLT_MISS_CMD_SET, dev_id, port_id, (a_uint32_t) cmd);
    return rv;
}

sw_error_t
fal_port_vlan_xlt_miss_cmd_get(a_uint32_t dev_id, fal_port_t port_id,
                             fal_fwd_cmd_t *cmd)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_VLAN_XLT_MISS_CMD_GET, dev_id, port_id, cmd);
    return rv;
}

sw_error_t
fal_port_vlantag_egmode_set(a_uint32_t dev_id, fal_port_t port_id,
                        fal_vlantag_egress_mode_t *port_egvlanmode)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_VLANTAG_EGMODE_SET, dev_id, port_id, port_egvlanmode);
    return rv;
}

sw_error_t
fal_port_vlantag_egmode_get(a_uint32_t dev_id, fal_port_t port_id,
                        fal_vlantag_egress_mode_t *port_egvlanmode)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_VLANTAG_EGMODE_GET, dev_id, port_id, port_egvlanmode);
    return rv;
}

sw_error_t
fal_port_vsi_egmode_set(a_uint32_t dev_id, a_uint32_t vsi, a_uint32_t port_id, fal_pt_1q_egmode_t egmode)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_VSI_EGMODE_SET, dev_id, vsi, port_id, (a_uint32_t) egmode);
    return rv;
}

sw_error_t
fal_port_vsi_egmode_get(a_uint32_t dev_id, a_uint32_t vsi, a_uint32_t port_id, fal_pt_1q_egmode_t * egmode)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_VSI_EGMODE_GET, dev_id, vsi, port_id, egmode);
    return rv;
}

sw_error_t
fal_port_vlantag_vsi_egmode_enable(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_VLANTAG_VSI_EGMODE_EN_SET, dev_id, port_id, (a_uint32_t) enable);
    return rv;
}

sw_error_t
fal_port_vlantag_vsi_egmode_status_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_VLANTAG_VSI_EGMODE_EN_GET, dev_id, port_id, enable);
    return rv;
}

sw_error_t
fal_port_vlan_trans_adv_add(a_uint32_t dev_id, fal_port_t port_id, fal_port_vlan_direction_t direction,
			fal_vlan_trans_adv_rule_t * rule, fal_vlan_trans_adv_action_t * action)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_VLAN_TRANS_ADV_ADD, dev_id, port_id, (a_uint32_t) direction,
		rule, action);
    return rv;
}

sw_error_t
fal_port_vlan_trans_adv_del(a_uint32_t dev_id, fal_port_t port_id, fal_port_vlan_direction_t direction,
			fal_vlan_trans_adv_rule_t * rule, fal_vlan_trans_adv_action_t * action)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_VLAN_TRANS_ADV_DEL, dev_id, port_id, (a_uint32_t) direction,
		rule, action);
    return rv;
}

sw_error_t
fal_port_vlan_trans_adv_getfirst(a_uint32_t dev_id, fal_port_t port_id, fal_port_vlan_direction_t direction,
			fal_vlan_trans_adv_rule_t * rule, fal_vlan_trans_adv_action_t * action)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_VLAN_TRANS_ADV_GETFIRST, dev_id, port_id, (a_uint32_t) direction,
		rule, action);
    return rv;
}

sw_error_t
fal_port_vlan_trans_adv_getnext(a_uint32_t dev_id, fal_port_t port_id, fal_port_vlan_direction_t direction,
			fal_vlan_trans_adv_rule_t * rule, fal_vlan_trans_adv_action_t * action)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_VLAN_TRANS_ADV_GETNEXT, dev_id, port_id, (a_uint32_t) direction,
		rule, action);
    return rv;
}

sw_error_t
fal_port_vlan_counter_get(a_uint32_t dev_id, a_uint32_t cnt_index, fal_port_vlan_counter_t * counter)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_VLAN_COUNTER_GET, dev_id, cnt_index, counter);
    return rv;
}

sw_error_t
fal_port_vlan_counter_cleanup(a_uint32_t dev_id, a_uint32_t cnt_index)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_VLAN_COUNTER_CLEANUP, dev_id, cnt_index);
    return rv;
}

