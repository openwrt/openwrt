/*
 * Copyright (c) 2016-2021, The Linux Foundation. All rights reserved.
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


/**
 * @defgroup
 * @{
 */
#ifndef _ADPT_H_
#define _ADPT_H_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

#include "sw.h"
#include "fal_fdb.h"
#include "fal_portvlan.h"
#include "fal_ctrlpkt.h"
#include "fal_servcode.h"
#include "fal_rss_hash.h"
#include "fal_mib.h"
#include "fal_port_ctrl.h"
#include "fal_mirror.h"
#include "fal_trunk.h"
#include "fal_ip.h"
#include "fal_qm.h"
#include "fal_flow.h"
#include "ssdk_init.h"
#include "fal_type.h"
#include "fal_stp.h"
#include "fal_vsi.h"
#include "fal_pppoe.h"
#include "fal_sec.h"
#include "fal_acl.h"
#include "fal_qos.h"
#include "fal_shaper.h"
#include "fal_bm.h"
#include "fal_init.h"
#include "fal_policer.h"
#include "fal_misc.h"
#include "fal_ptp.h"
#include "fal_sfp.h"
#include "ssdk_plat.h"

#define ADPT_DEV_ID_CHECK(dev_id) \
do { \
    if (dev_id >= SW_MAX_NR_DEV) \
        return SW_OUT_OF_RANGE; \
} while (0)

#define ADPT_PORT_ID_CHECK(port_id) \
do { \
    if (port_id >= SW_MAX_NR_PORT) \
        return SW_OUT_OF_RANGE; \
} while (0)

#define ADPT_NULL_POINT_CHECK(point) \
do { \
    if (point == NULL) \
        return SW_BAD_PTR; \
} while (0)

typedef sw_error_t (*adpt_fdb_first_func)(a_uint32_t dev_id, fal_fdb_entry_t * entry);
typedef sw_error_t (*adpt_fdb_next_func)(a_uint32_t dev_id, fal_fdb_entry_t * entry);
typedef sw_error_t (*adpt_fdb_add_func)(a_uint32_t dev_id, const fal_fdb_entry_t * entry);
typedef sw_error_t (*adpt_fdb_age_time_set_func)(a_uint32_t dev_id, a_uint32_t * time);
typedef sw_error_t (*adpt_fdb_extend_next_func)(a_uint32_t dev_id, fal_fdb_op_t * option,
                        fal_fdb_entry_t * entry);
typedef sw_error_t (*adpt_fdb_learn_ctrl_get_func)(a_uint32_t dev_id, a_bool_t * enable);
typedef sw_error_t (*adpt_fdb_age_time_get_func)(a_uint32_t dev_id, a_uint32_t * time);
typedef sw_error_t (*adpt_port_fdb_learn_limit_set_func)(a_uint32_t dev_id, fal_port_t port_id,
                                 a_bool_t enable, a_uint32_t cnt);
typedef sw_error_t (*adpt_fdb_port_add_func)(a_uint32_t dev_id, a_uint32_t fid, fal_mac_addr_t * addr, fal_port_t port_id);
typedef sw_error_t (*adpt_fdb_port_learn_set_func)(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable);
typedef sw_error_t (*adpt_fdb_port_learn_get_func)(a_uint32_t dev_id, fal_port_t port_id, a_bool_t *enable);
typedef sw_error_t (*adpt_fdb_port_newaddr_lrn_set_func)(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable, fal_fwd_cmd_t cmd);
typedef sw_error_t (*adpt_fdb_port_newaddr_lrn_get_func)(a_uint32_t dev_id, fal_port_t port_id, a_bool_t *enable, fal_fwd_cmd_t *cmd);
typedef sw_error_t (*adpt_fdb_port_stamove_set_func)(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable, fal_fwd_cmd_t cmd);
typedef sw_error_t (*adpt_fdb_port_stamove_get_func)(a_uint32_t dev_id, fal_port_t port_id, a_bool_t *enable, fal_fwd_cmd_t *cmd);
typedef sw_error_t (*adpt_port_fdb_learn_counter_get_func)(a_uint32_t dev_id, fal_port_t port_id,
                                  a_uint32_t * cnt);
typedef sw_error_t (*adpt_fdb_extend_first_func)(a_uint32_t dev_id, fal_fdb_op_t * option,
                         fal_fdb_entry_t * entry);
typedef sw_error_t (*adpt_fdb_transfer_func)(a_uint32_t dev_id, fal_port_t old_port, fal_port_t new_port,
                     a_uint32_t fid, fal_fdb_op_t * option);
typedef sw_error_t (*adpt_fdb_port_del_func)(a_uint32_t dev_id, a_uint32_t fid, fal_mac_addr_t * addr, fal_port_t port_id);
typedef sw_error_t (*adpt_fdb_find_func)(a_uint32_t dev_id, fal_fdb_entry_t * entry);
typedef sw_error_t (*adpt_fdb_learn_ctrl_set_func)(a_uint32_t dev_id, a_bool_t enable);
typedef sw_error_t (*adpt_port_fdb_learn_exceed_cmd_get_func)(a_uint32_t dev_id, fal_port_t port_id,
                                      fal_fwd_cmd_t * cmd);
typedef sw_error_t (*adpt_fdb_del_by_port_func)(a_uint32_t dev_id, a_uint32_t port_id, a_uint32_t flag);
typedef sw_error_t (*adpt_port_fdb_learn_limit_get_func)(a_uint32_t dev_id, fal_port_t port_id,
                                 a_bool_t * enable, a_uint32_t * cnt);
typedef sw_error_t (*adpt_fdb_age_ctrl_set_func)(a_uint32_t dev_id, a_bool_t enable);
typedef sw_error_t (*adpt_fdb_del_by_mac_func)(a_uint32_t dev_id, const fal_fdb_entry_t *entry);
typedef sw_error_t (*adpt_fdb_iterate_func)(a_uint32_t dev_id, a_uint32_t * iterator, fal_fdb_entry_t * entry);
typedef sw_error_t (*adpt_port_fdb_learn_exceed_cmd_set_func)(a_uint32_t dev_id, fal_port_t port_id,
                                      fal_fwd_cmd_t cmd);
typedef sw_error_t (*adpt_fdb_del_all_func)(a_uint32_t dev_id, a_uint32_t flag);
typedef sw_error_t (*adpt_fdb_age_ctrl_get_func)(a_uint32_t dev_id, a_bool_t * enable);
typedef sw_error_t (*adpt_fdb_port_maclimit_ctrl_set_func)(a_uint32_t dev_id, fal_port_t port_id, fal_maclimit_ctrl_t * maclimit_ctrl);
typedef sw_error_t (*adpt_fdb_port_maclimit_ctrl_get_func)(a_uint32_t dev_id, fal_port_t port_id, fal_maclimit_ctrl_t * maclimit_ctrl);
typedef sw_error_t (*adpt_fdb_del_by_fid_func)(a_uint32_t dev_id, a_uint16_t fid, a_uint32_t flag);

typedef sw_error_t (*adpt_mib_cpukeep_get_func)(a_uint32_t dev_id, a_bool_t * enable);
typedef sw_error_t (*adpt_mib_cpukeep_set_func)(a_uint32_t dev_id, a_bool_t  enable);
typedef sw_error_t (*adpt_get_mib_info_func)(a_uint32_t dev_id, fal_port_t port_id,
                     fal_mib_info_t * mib_info );
typedef sw_error_t (*adpt_get_tx_mib_info_func)(a_uint32_t dev_id, fal_port_t port_id,
                     fal_mib_info_t * mib_info );
typedef sw_error_t (*adpt_mib_status_set_func)(a_uint32_t dev_id, a_bool_t enable);
typedef sw_error_t (*adpt_mib_port_flush_counters_func)(a_uint32_t dev_id, fal_port_t port_id);
typedef sw_error_t (*adpt_mib_status_get_func)(a_uint32_t dev_id, a_bool_t * enable);
typedef sw_error_t (*adpt_get_rx_mib_info_func)(a_uint32_t dev_id, fal_port_t port_id,
                     fal_mib_info_t * mib_info );
typedef sw_error_t (*adpt_get_xgmib_info_func)(a_uint32_t dev_id, fal_port_t port_id,
                     fal_xgmib_info_t * mib_info );
typedef sw_error_t (*adpt_get_tx_xgmib_info_func)(a_uint32_t dev_id, fal_port_t port_id,
                     fal_xgmib_info_t * mib_info );
typedef sw_error_t (*adpt_get_rx_xgmib_info_func)(a_uint32_t dev_id, fal_port_t port_id,
                     fal_xgmib_info_t * mib_info );

typedef sw_error_t (*adpt_stp_port_state_get_func)(a_uint32_t dev_id, a_uint32_t st_id,
                     fal_port_t port_id, fal_stp_state_t * state);
typedef sw_error_t (*adpt_stp_port_state_set_func)(a_uint32_t dev_id, a_uint32_t st_id,
                     fal_port_t port_id, fal_stp_state_t state);

typedef sw_error_t (*adpt_port_vlan_vsi_set_func)(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t stag_vid, a_uint32_t ctag_vid, a_uint32_t vsi_id);
typedef sw_error_t (*adpt_port_vlan_vsi_get_func)(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t stag_vid, a_uint32_t ctag_vid, a_uint32_t *vsi_id);
typedef sw_error_t (*adpt_port_vsi_set_func)(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t vsi_id);
typedef sw_error_t (*adpt_port_vsi_get_func)(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t *vsi_id);
typedef sw_error_t (*adpt_vsi_stamove_set_func)(a_uint32_t dev_id, a_uint32_t vsi_id, fal_vsi_stamove_t *stamove);
typedef sw_error_t (*adpt_vsi_stamove_get_func)(a_uint32_t dev_id, a_uint32_t vsi_id, fal_vsi_stamove_t *stamove);
typedef sw_error_t (*adpt_vsi_newaddr_lrn_set_func)(a_uint32_t dev_id, a_uint32_t vsi_id, fal_vsi_newaddr_lrn_t *newaddr_lrn);
typedef sw_error_t (*adpt_vsi_newaddr_lrn_get_func)(a_uint32_t dev_id, a_uint32_t vsi_id, fal_vsi_newaddr_lrn_t *newaddr_lrn);
typedef sw_error_t (*adpt_vsi_member_set_func)(a_uint32_t dev_id, a_uint32_t vsi_id, fal_vsi_member_t *vsi_member);
typedef sw_error_t (*adpt_vsi_member_get_func)(a_uint32_t dev_id, a_uint32_t vsi_id, fal_vsi_member_t *vsi_member);
typedef sw_error_t (*adpt_vsi_counter_get_func)(a_uint32_t dev_id, a_uint32_t vsi_id, fal_vsi_counter_t *counter);
typedef sw_error_t (*adpt_vsi_counter_cleanup_func)(a_uint32_t dev_id, a_uint32_t vsi_id);

// portctrl function.

typedef sw_error_t (*adpt_port_local_loopback_get_func)(a_uint32_t dev_id, fal_port_t port_id,
					a_bool_t * enable);
typedef sw_error_t (*adpt_port_autoneg_restart_func)(a_uint32_t dev_id, fal_port_t port_id);
typedef sw_error_t (*adpt_port_duplex_set_func)(a_uint32_t dev_id, fal_port_t port_id,
				fal_port_duplex_t duplex);
typedef sw_error_t (*adpt_port_rxmac_status_get_func)(a_uint32_t dev_id, fal_port_t port_id,
					  a_bool_t * enable);
typedef sw_error_t (*adpt_port_cdt_func)(a_uint32_t dev_id, fal_port_t port_id,
		a_uint32_t mdi_pair, fal_cable_status_t * cable_status,
		a_uint32_t * cable_len);
typedef sw_error_t (*adpt_port_txmac_status_set_func)(a_uint32_t dev_id, fal_port_t port_id,
					  a_bool_t enable);
typedef sw_error_t (*adpt_port_combo_fiber_mode_set_func)(a_uint32_t dev_id,
						  a_uint32_t port_id,
						  fal_port_fiber_mode_t mode);
typedef sw_error_t (*adpt_port_combo_medium_status_get_func)(a_uint32_t dev_id,
							 a_uint32_t port_id,
							 fal_port_medium_t *
							 medium);
typedef sw_error_t (*adpt_port_magic_frame_mac_set_func)(a_uint32_t dev_id, fal_port_t port_id,
				   fal_mac_addr_t * mac);
typedef sw_error_t (*adpt_port_powersave_set_func)(a_uint32_t dev_id, fal_port_t port_id,
			  a_bool_t enable);
typedef sw_error_t (*adpt_port_hibernate_set_func)(a_uint32_t dev_id, fal_port_t port_id,
			  a_bool_t enable);
typedef sw_error_t (*adpt_port_max_frame_size_set_func)(a_uint32_t dev_id, fal_port_t port_id,
		a_uint32_t max_frame);
typedef sw_error_t (*adpt_port_8023az_get_func)(a_uint32_t dev_id, fal_port_t port_id,
				a_bool_t * enable);
typedef sw_error_t (*adpt_port_rxfc_status_get_func)(a_uint32_t dev_id, fal_port_t port_id,
					 a_bool_t * enable);
typedef sw_error_t (*adpt_port_txfc_status_get_func)(a_uint32_t dev_id, fal_port_t port_id,
					 a_bool_t * enable);
typedef sw_error_t (*adpt_port_remote_loopback_set_func)(a_uint32_t dev_id, fal_port_t port_id,
					 a_bool_t enable);
typedef sw_error_t (*adpt_port_flowctrl_set_func)(a_uint32_t dev_id, fal_port_t port_id,
				  a_bool_t enable);
typedef sw_error_t (*adpt_port_mru_set_func)(a_uint32_t dev_id, fal_port_t port_id,
		fal_mru_ctrl_t *ctrl);
typedef sw_error_t (*adpt_port_autoneg_status_get_func)(a_uint32_t dev_id, fal_port_t port_id,
					a_bool_t * status);
typedef sw_error_t (*adpt_port_txmac_status_get_func)(a_uint32_t dev_id, fal_port_t port_id,
					  a_bool_t * enable);
typedef sw_error_t (*adpt_port_mdix_get_func)(a_uint32_t dev_id, fal_port_t port_id,
				  fal_port_mdix_mode_t * mode);
typedef sw_error_t (*adpt_ports_link_status_get_func)(a_uint32_t dev_id, a_uint32_t * status);
typedef sw_error_t (*adpt_port_mac_loopback_set_func)(a_uint32_t dev_id, fal_port_t port_id,
				 a_bool_t enable);
typedef sw_error_t (*adpt_port_phy_id_get_func)(a_uint32_t dev_id, fal_port_t port_id,
			  a_uint16_t * org_id, a_uint16_t * rev_id);
typedef sw_error_t (*adpt_port_mru_get_func)(a_uint32_t dev_id, fal_port_t port_id,
		fal_mru_ctrl_t *ctrl);
typedef sw_error_t (*adpt_port_power_on_func)(a_uint32_t dev_id, fal_port_t port_id);
typedef sw_error_t (*adpt_port_speed_set_func)(a_uint32_t dev_id, fal_port_t port_id,
				   fal_port_speed_t speed);
typedef sw_error_t (*adpt_port_interface_mode_get_func)(a_uint32_t dev_id, fal_port_t port_id,
				  fal_port_interface_mode_t * mode);
typedef sw_error_t (*adpt_port_duplex_get_func)(a_uint32_t dev_id, fal_port_t port_id,
				fal_port_duplex_t * pduplex);
typedef sw_error_t (*adpt_port_autoneg_adv_get_func)(a_uint32_t dev_id, fal_port_t port_id,
					 a_uint32_t * autoadv);
typedef sw_error_t (*adpt_port_mdix_status_get_func)(a_uint32_t dev_id, fal_port_t port_id,
					 fal_port_mdix_status_t * mode);
typedef sw_error_t (*adpt_port_mtu_set_func)(a_uint32_t dev_id, fal_port_t port_id,
		fal_mtu_ctrl_t *ctrl);
typedef sw_error_t (*adpt_port_link_status_get_func)(a_uint32_t dev_id, fal_port_t port_id,
					 a_bool_t * status);
typedef sw_error_t (*adpt_port_8023az_set_func)(a_uint32_t dev_id, fal_port_t port_id,
				a_bool_t enable);
typedef sw_error_t (*adpt_port_powersave_get_func)(a_uint32_t dev_id, fal_port_t port_id,
			  a_bool_t * enable);
typedef sw_error_t (*adpt_port_combo_prefer_medium_get_func)(a_uint32_t dev_id,
							 a_uint32_t port_id,
							 fal_port_medium_t *
							 medium);
typedef sw_error_t (*adpt_port_max_frame_size_get_func)(a_uint32_t dev_id, fal_port_t port_id,
		a_uint32_t *max_frame);
typedef sw_error_t (*adpt_port_combo_prefer_medium_set_func)(a_uint32_t dev_id,
						 a_uint32_t port_id,
						 fal_port_medium_t medium);
typedef sw_error_t (*adpt_port_power_off_func)(a_uint32_t dev_id, fal_port_t port_id);
typedef sw_error_t (*adpt_port_txfc_status_set_func)(a_uint32_t dev_id, fal_port_t port_id,
					 a_bool_t enable);
typedef sw_error_t (*adpt_port_counter_set_func)(a_uint32_t dev_id, fal_port_t port_id,
		   a_bool_t enable);
typedef sw_error_t (*adpt_port_combo_fiber_mode_get_func)(a_uint32_t dev_id,
						  a_uint32_t port_id,
						  fal_port_fiber_mode_t * mode);
typedef sw_error_t (*adpt_port_local_loopback_set_func)(a_uint32_t dev_id,
						fal_port_t port_id,
						a_bool_t enable);
typedef sw_error_t (*adpt_port_wol_status_set_func)(a_uint32_t dev_id, fal_port_t port_id,
				  a_bool_t enable);
typedef sw_error_t (*adpt_port_magic_frame_mac_get_func)(a_uint32_t dev_id, fal_port_t port_id,
				   fal_mac_addr_t * mac);
typedef sw_error_t (*adpt_port_flowctrl_get_func)(a_uint32_t dev_id, fal_port_t port_id,
				  a_bool_t * enable);
typedef sw_error_t (*adpt_port_rxmac_status_set_func)(a_uint32_t dev_id, fal_port_t port_id,
					  a_bool_t enable);
typedef sw_error_t (*adpt_port_counter_get_func)(a_uint32_t dev_id, fal_port_t port_id,
		   a_bool_t * enable);
typedef sw_error_t (*adpt_port_interface_mode_set_func)(a_uint32_t dev_id, fal_port_t port_id,
				  fal_port_interface_mode_t mode);
typedef sw_error_t (*adpt_port_mac_loopback_get_func)(a_uint32_t dev_id, fal_port_t port_id,
				 a_bool_t * enable);
typedef sw_error_t (*adpt_port_hibernate_get_func)(a_uint32_t dev_id, fal_port_t port_id,
			  a_bool_t * enable);
typedef sw_error_t (*adpt_port_autoneg_adv_set_func)(a_uint32_t dev_id, fal_port_t port_id,
					 a_uint32_t autoadv);
typedef sw_error_t (*adpt_port_remote_loopback_get_func)(a_uint32_t dev_id, fal_port_t port_id,
					 a_bool_t * enable);
typedef sw_error_t (*adpt_port_counter_show_func)(a_uint32_t dev_id, fal_port_t port_id,
				 fal_port_counter_info_t * counter_info);
typedef sw_error_t (*adpt_port_autoneg_enable_func)(a_uint32_t dev_id, fal_port_t port_id);
typedef sw_error_t (*adpt_port_mtu_get_func)(a_uint32_t dev_id, fal_port_t port_id,
		fal_mtu_ctrl_t *ctrl);
typedef sw_error_t (*adpt_port_interface_mode_status_get_func)(a_uint32_t dev_id, fal_port_t port_id,
				  fal_port_interface_mode_t * mode);
typedef sw_error_t (*adpt_port_reset_func)(a_uint32_t dev_id, fal_port_t port_id);
typedef sw_error_t (*adpt_port_rxfc_status_set_func)(a_uint32_t dev_id, fal_port_t port_id,
					 a_bool_t enable);
typedef sw_error_t (*adpt_port_speed_get_func)(a_uint32_t dev_id, fal_port_t port_id,
				   fal_port_speed_t * pspeed);
typedef sw_error_t (*adpt_port_mdix_set_func)(a_uint32_t dev_id, fal_port_t port_id,
				  fal_port_mdix_mode_t mode);
typedef sw_error_t (*adpt_port_wol_status_get_func)(a_uint32_t dev_id, fal_port_t port_id,
				  a_bool_t * enable);
typedef sw_error_t (*adpt_port_source_filter_get_func)(a_uint32_t dev_id, fal_port_t port_id,
				a_bool_t * enable);
typedef sw_error_t (*adpt_port_source_filter_set_func)(a_uint32_t dev_id, fal_port_t port_id,
				a_bool_t enable);
typedef sw_error_t (*adpt_port_mux_mac_type_set_func)(a_uint32_t dev_id, fal_port_t port_id,
	a_uint32_t mode0,	a_uint32_t mode1, a_uint32_t mode2);
typedef sw_error_t (*adpt_port_mac_speed_set_func)(a_uint32_t dev_id, fal_port_t port_id,
				fal_port_speed_t speed);
typedef sw_error_t (*adpt_port_mac_duplex_set_func)(a_uint32_t dev_id, fal_port_t port_id,
				fal_port_duplex_t duplex);
typedef sw_error_t (*adpt_port_polling_sw_sync_func)(struct qca_phy_priv *priv);

typedef sw_error_t (*adpt_port_bridge_txmac_set_func)(a_uint32_t dev_id,
		fal_port_t port_id, a_bool_t enable);

typedef sw_error_t (*adpt_port_interface_mode_apply_func)(a_uint32_t dev_id);

typedef sw_error_t (*adpt_port_interface_3az_status_set_func)(a_uint32_t dev_id,
		a_uint32_t port_id, a_bool_t enable);
typedef sw_error_t (*adpt_port_interface_3az_status_get_func)(a_uint32_t dev_id,
		a_uint32_t port_id, a_bool_t * enable);
typedef sw_error_t (*adpt_port_flowctrl_forcemode_set_func) (a_uint32_t dev_id,
		fal_port_t port_id, a_bool_t enable);
typedef sw_error_t (*adpt_port_flowctrl_forcemode_get_func) (a_uint32_t dev_id,
		fal_port_t port_id, a_bool_t * enable);
typedef sw_error_t (*adpt_port_promisc_mode_set_func)(a_uint32_t dev_id,
			fal_port_t port_id, a_bool_t enable);
typedef sw_error_t (*adpt_port_promisc_mode_get_func)(a_uint32_t dev_id,
			fal_port_t port_id, a_bool_t *enable);
typedef sw_error_t (*adpt_port_interface_eee_cfg_set_func)(a_uint32_t dev_id,
			fal_port_t port_id, fal_port_eee_cfg_t *port_eee_cfg);
typedef sw_error_t (*adpt_port_interface_eee_cfg_get_func)(a_uint32_t dev_id,
			fal_port_t port_id, fal_port_eee_cfg_t *port_eee_cfg);
typedef sw_error_t (*adpt_port_source_filter_config_set_func)(a_uint32_t dev_id,
			fal_port_t port_id, fal_src_filter_config_t *src_filter_config);
typedef sw_error_t (*adpt_port_source_filter_config_get_func)(a_uint32_t dev_id,
			fal_port_t port_id, fal_src_filter_config_t *src_filter_config);
typedef sw_error_t (*adpt_switch_port_loopback_set_func)(a_uint32_t dev_id,
			fal_port_t port_id, fal_loopback_config_t *loopback_cfg);
typedef sw_error_t (*adpt_switch_port_loopback_get_func)(a_uint32_t dev_id,
			fal_port_t port_id, fal_loopback_config_t *loopback_cfg);
typedef sw_error_t (*adpt_port_netdev_notify_func)(struct qca_phy_priv *priv,
			a_uint32_t port_id);
typedef sw_error_t (*adpt_port_phy_status_get_func)(a_uint32_t dev_id,
			fal_port_t port_id, struct port_phy_status *phy_status);
// mirror
typedef sw_error_t (*adpt_mirr_port_in_set_func)(a_uint32_t dev_id, fal_port_t port_id,
                         a_bool_t enable);
typedef sw_error_t (*adpt_mirr_port_in_get_func)(a_uint32_t dev_id, fal_port_t port_id,
                         a_bool_t * enable);
typedef sw_error_t (*adpt_mirr_port_eg_set_func)(a_uint32_t dev_id, fal_port_t port_id,
                         a_bool_t enable);
typedef sw_error_t (*adpt_mirr_port_eg_get_func)(a_uint32_t dev_id, fal_port_t port_id,
                         a_bool_t * enable);
typedef sw_error_t (*adpt_mirr_analysis_port_set_func)(a_uint32_t dev_id, fal_port_t port_id);
typedef sw_error_t (*adpt_mirr_analysis_port_get_func)(a_uint32_t dev_id, fal_port_t * port_id);
typedef sw_error_t (*adpt_mirr_analysis_config_set_func)(a_uint32_t dev_id, fal_mirr_direction_t direction,
				fal_mirr_analysis_config_t * config);
typedef sw_error_t (*adpt_mirr_analysis_config_get_func)(a_uint32_t dev_id, fal_mirr_direction_t direction,
				fal_mirr_analysis_config_t * config);

//rss hash
typedef sw_error_t (*adpt_rss_hash_config_set_func)(a_uint32_t dev_id, fal_rss_hash_mode_t mode, fal_rss_hash_config_t * config);
typedef sw_error_t (*adpt_rss_hash_config_get_func)(a_uint32_t dev_id, fal_rss_hash_mode_t mode, fal_rss_hash_config_t * config);

//trunk
typedef sw_error_t (*adpt_trunk_fail_over_en_get_func)(a_uint32_t dev_id, a_bool_t * fail_over);
typedef sw_error_t (*adpt_trunk_hash_mode_get_func)(a_uint32_t dev_id, a_uint32_t * hash_mode);
typedef sw_error_t (*adpt_trunk_group_get_func)(a_uint32_t dev_id, a_uint32_t trunk_id,
                        a_bool_t * enable, fal_pbmp_t * member);
typedef sw_error_t (*adpt_trunk_group_set_func)(a_uint32_t dev_id, a_uint32_t trunk_id,
                        a_bool_t enable, fal_pbmp_t member);
typedef sw_error_t (*adpt_trunk_fail_over_en_set_func)(a_uint32_t dev_id, a_bool_t fail_over);
typedef sw_error_t (*adpt_trunk_hash_mode_set_func)(a_uint32_t dev_id, a_uint32_t hash_mode);

typedef sw_error_t (*adpt_ip_network_route_get_func)(a_uint32_t dev_id,
			a_uint32_t index, a_uint8_t type,
			fal_network_route_entry_t *entry);
typedef sw_error_t (*adpt_ip_network_route_add_func)(a_uint32_t dev_id,
			a_uint32_t index,
			fal_network_route_entry_t *entry);
typedef sw_error_t (*adpt_ip_network_route_del_func)(a_uint32_t dev_id,
			a_uint32_t index,
			a_uint8_t type);
typedef sw_error_t (*adpt_ip_host_add_func)(
			a_uint32_t dev_id, fal_host_entry_t * host_entry);
typedef sw_error_t (*adpt_ip_vsi_sg_cfg_get_func)(
			a_uint32_t dev_id, a_uint32_t vsi,
    			fal_sg_cfg_t *sg_cfg);
typedef sw_error_t (*adpt_ip_pub_addr_set_func)(
			a_uint32_t dev_id, a_uint32_t index,
			fal_ip_pub_addr_t *entry);
typedef sw_error_t (*adpt_ip_pub_addr_get_func)(
			a_uint32_t dev_id, a_uint32_t index,
			fal_ip_pub_addr_t *entry);
typedef sw_error_t (*adpt_ip_port_sg_cfg_set_func)(
			a_uint32_t dev_id, fal_port_t port_id,
    			fal_sg_cfg_t *sg_cfg);
typedef sw_error_t (*adpt_ip_port_intf_get_func)(
			a_uint32_t dev_id, fal_port_t port_id, fal_intf_id_t *id);
typedef sw_error_t (*adpt_ip_vsi_arp_sg_cfg_set_func)(
			a_uint32_t dev_id, a_uint32_t vsi,
    			fal_arp_sg_cfg_t *arp_sg_cfg);
typedef sw_error_t (*adpt_ip_port_intf_set_func)(
			a_uint32_t dev_id, fal_port_t port_id, fal_intf_id_t *id);
typedef sw_error_t (*adpt_ip_vsi_sg_cfg_set_func)(
			a_uint32_t dev_id, a_uint32_t vsi,
    			fal_sg_cfg_t *sg_cfg);
typedef sw_error_t (*adpt_ip_host_next_func)(
			a_uint32_t dev_id, a_uint32_t next_mode,
			fal_host_entry_t * host_entry);
typedef sw_error_t (*adpt_ip_port_macaddr_set_func)(a_uint32_t dev_id, fal_port_t port_id,
    			fal_macaddr_entry_t *macaddr);
typedef sw_error_t (*adpt_ip_vsi_intf_get_func)(
			a_uint32_t dev_id, a_uint32_t vsi, fal_intf_id_t *id);
typedef sw_error_t (*adpt_ip_port_sg_cfg_get_func)(
			a_uint32_t dev_id, fal_port_t port_id,
    			fal_sg_cfg_t *sg_cfg);
typedef sw_error_t (*adpt_ip_intf_get_func)(
    			a_uint32_t dev_id,
    			a_uint32_t index,
    			fal_intf_entry_t *entry);
typedef sw_error_t (*adpt_ip_host_del_func)(
			a_uint32_t dev_id, a_uint32_t del_mode,
			fal_host_entry_t * host_entry);
typedef sw_error_t (*adpt_ip_route_mismatch_get_func)(
			a_uint32_t dev_id, fal_fwd_cmd_t *cmd);
typedef sw_error_t (*adpt_ip_vsi_arp_sg_cfg_get_func)(
			a_uint32_t dev_id, a_uint32_t vsi,
    			fal_arp_sg_cfg_t *arp_sg_cfg);
typedef sw_error_t (*adpt_ip_port_arp_sg_cfg_set_func)(
			a_uint32_t dev_id, fal_port_t port_id,
    			fal_arp_sg_cfg_t *arp_sg_cfg);
typedef sw_error_t (*adpt_ip_vsi_mc_mode_set_func)(
			a_uint32_t dev_id, a_uint32_t vsi,
    			fal_mc_mode_cfg_t *cfg);
typedef sw_error_t (*adpt_ip_vsi_intf_set_func)(
			a_uint32_t dev_id, a_uint32_t vsi, fal_intf_id_t *id);
typedef sw_error_t (*adpt_ip_nexthop_get_func)(a_uint32_t dev_id,
			a_uint32_t index, fal_ip_nexthop_t *entry);
typedef sw_error_t (*adpt_ip_route_mismatch_set_func)(
			a_uint32_t dev_id, fal_fwd_cmd_t cmd);
typedef sw_error_t (*adpt_ip_host_get_func)(
			a_uint32_t dev_id, a_uint32_t get_mode,
                    fal_host_entry_t * host_entry);
typedef sw_error_t (*adpt_ip_intf_set_func)(
    			a_uint32_t dev_id,
    			a_uint32_t index,
    			fal_intf_entry_t *entry);
typedef sw_error_t (*adpt_ip_vsi_mc_mode_get_func)(
			a_uint32_t dev_id,
			a_uint32_t vsi,
    			fal_mc_mode_cfg_t *cfg);
typedef sw_error_t (*adpt_ip_port_macaddr_get_func)(
			a_uint32_t dev_id, fal_port_t port_id,
    			fal_macaddr_entry_t *macaddr);
typedef sw_error_t (*adpt_ip_port_arp_sg_cfg_get_func)(
			a_uint32_t dev_id, fal_port_t port_id,
    			fal_arp_sg_cfg_t *arp_sg_cfg);
typedef sw_error_t (*adpt_ip_nexthop_set_func)(a_uint32_t dev_id,
			a_uint32_t index, fal_ip_nexthop_t *entry);
typedef sw_error_t (*adpt_ip_global_ctrl_get_func)(a_uint32_t dev_id,
			fal_ip_global_cfg_t *cfg);
typedef sw_error_t (*adpt_ip_global_ctrl_set_func)(a_uint32_t dev_id,
			fal_ip_global_cfg_t *cfg);

typedef sw_error_t (*adpt_flow_global_cfg_get_func)(
		a_uint32_t dev_id,
		fal_flow_global_cfg_t *cfg);
typedef sw_error_t (*adpt_flow_global_cfg_set_func)(
		a_uint32_t dev_id,
		fal_flow_global_cfg_t *cfg);
typedef sw_error_t (*adpt_flow_host_add_func)(
		a_uint32_t dev_id,
		a_uint32_t add_mode,
		fal_flow_host_entry_t *flow_host_entry);
typedef sw_error_t (*adpt_flow_entry_get_func)(
		a_uint32_t dev_id,
		a_uint32_t get_mode,
		fal_flow_entry_t *flow_entry);
typedef sw_error_t (*adpt_flow_entry_del_func)(
		a_uint32_t dev_id,
		a_uint32_t del_mode,
		fal_flow_entry_t *flow_entry);
typedef sw_error_t (*adpt_flow_entry_next_func)(
		a_uint32_t dev_id,
		a_uint32_t next_mode,
		fal_flow_entry_t *flow_entry);
typedef sw_error_t (*adpt_flow_status_get_func)(
		a_uint32_t dev_id, a_bool_t *enable);
typedef sw_error_t (*adpt_flow_ctrl_set_func)(
		a_uint32_t dev_id,
		fal_flow_pkt_type_t type,
		fal_flow_direction_t dir,
		fal_flow_mgmt_t *ctrl);
typedef sw_error_t (*adpt_flow_age_timer_get_func)(
		a_uint32_t dev_id, fal_flow_age_timer_t *age_timer);
typedef sw_error_t (*adpt_flow_status_set_func)(
		a_uint32_t dev_id, a_bool_t enable);
typedef sw_error_t (*adpt_flow_host_get_func)(
		a_uint32_t dev_id,
		a_uint32_t get_mode,
		fal_flow_host_entry_t *flow_host_entry);
typedef sw_error_t (*adpt_flow_host_del_func)(
		a_uint32_t dev_id,
		a_uint32_t del_mode,
		fal_flow_host_entry_t *flow_host_entry);
typedef sw_error_t (*adpt_flow_ctrl_get_func)(
		a_uint32_t dev_id,
		fal_flow_pkt_type_t type,
		fal_flow_direction_t dir,
		fal_flow_mgmt_t *ctrl);
typedef sw_error_t (*adpt_flow_age_timer_set_func)(
		a_uint32_t dev_id, fal_flow_age_timer_t *age_timer);
typedef sw_error_t (*adpt_flow_entry_add_func)(
		a_uint32_t dev_id,
		a_uint32_t add_mode, /*index or hash*/
		fal_flow_entry_t *flow_entry);
typedef sw_error_t (*adpt_ucast_hash_map_set_func)(
		a_uint32_t dev_id,
		a_uint8_t profile,
		a_uint8_t rss_hash,
		a_int8_t queue_hash);
typedef sw_error_t (*adpt_ac_dynamic_threshold_get_func)(
		a_uint32_t dev_id,
		a_uint32_t queue_id,
		fal_ac_dynamic_threshold_t *cfg);
typedef sw_error_t (*adpt_ucast_queue_base_profile_get_func)(
		a_uint32_t dev_id,
		fal_ucast_queue_dest_t *queue_dest,
		a_uint32_t *queue_base, a_uint8_t *profile);
typedef sw_error_t (*adpt_port_mcast_priority_class_get_func)(
		a_uint32_t dev_id,
		fal_port_t port,
		a_uint8_t priority,
		a_uint8_t *queue_class);
typedef sw_error_t (*adpt_ac_dynamic_threshold_set_func)(
		a_uint32_t dev_id,
		a_uint32_t queue_id,
		fal_ac_dynamic_threshold_t *cfg);
typedef sw_error_t (*adpt_ac_prealloc_buffer_set_func)(
		a_uint32_t dev_id,
		fal_ac_obj_t *obj,
		a_uint16_t num);
typedef sw_error_t (*adpt_ucast_default_hash_get_func)(
		a_uint32_t dev_id,
		a_uint8_t *hash_value);
typedef sw_error_t (*adpt_ucast_default_hash_set_func)(
		a_uint32_t dev_id,
		a_uint8_t hash_value);
typedef sw_error_t (*adpt_ac_queue_group_get_func)(
		a_uint32_t dev_id,
		a_uint32_t queue_id,
		a_uint8_t *group_id);
typedef sw_error_t (*adpt_ac_ctrl_get_func)(
		a_uint32_t dev_id,
		fal_ac_obj_t *obj,
		fal_ac_ctrl_t *cfg);
typedef sw_error_t (*adpt_ac_prealloc_buffer_get_func)(
		a_uint32_t dev_id,
		fal_ac_obj_t *obj,
		a_uint16_t *num);
typedef sw_error_t (*adpt_port_mcast_priority_class_set_func)(
		a_uint32_t dev_id,
		fal_port_t port,
		a_uint8_t priority,
		a_uint8_t queue_class);
typedef sw_error_t (*adpt_ucast_hash_map_get_func)(
		a_uint32_t dev_id,
		a_uint8_t profile,
		a_uint8_t rss_hash,
		a_int8_t *queue_hash);
typedef sw_error_t (*adpt_ac_static_threshold_set_func)(
		a_uint32_t dev_id,
		fal_ac_obj_t *obj,
		fal_ac_static_threshold_t *cfg);
typedef sw_error_t (*adpt_ac_queue_group_set_func)(
		a_uint32_t dev_id,
		a_uint32_t queue_id,
		a_uint8_t group_id);
typedef sw_error_t (*adpt_ac_group_buffer_get_func)(
		a_uint32_t dev_id,
		a_uint8_t group_id,
		fal_ac_group_buffer_t *cfg);
typedef sw_error_t (*adpt_mcast_cpu_code_class_get_func)(
		a_uint32_t dev_id,
		a_uint8_t cpu_code,
		a_uint8_t *queue_class);
typedef sw_error_t (*adpt_ac_ctrl_set_func)(
		a_uint32_t dev_id,
		fal_ac_obj_t *obj,
		fal_ac_ctrl_t *cfg);
typedef sw_error_t (*adpt_ucast_priority_class_get_func)(
		a_uint32_t dev_id,
		a_uint8_t profile,
		a_uint8_t priority,
		a_uint8_t *class);
typedef sw_error_t (*adpt_queue_flush_func)(
		a_uint32_t dev_id,
		fal_port_t port,
		a_uint16_t queue_id);
typedef sw_error_t (*adpt_mcast_cpu_code_class_set_func)(
		a_uint32_t dev_id,
		a_uint8_t cpu_code,
		a_uint8_t queue_class);
typedef sw_error_t (*adpt_ucast_priority_class_set_func)(
		a_uint32_t dev_id,
		a_uint8_t profile,
		a_uint8_t priority,
		a_uint8_t class);
typedef sw_error_t (*adpt_ac_static_threshold_get_func)(
		a_uint32_t dev_id,
		fal_ac_obj_t *obj,
		fal_ac_static_threshold_t *cfg);
typedef sw_error_t (*adpt_ucast_queue_base_profile_set_func)(
		a_uint32_t dev_id,
		fal_ucast_queue_dest_t *queue_dest,
		a_uint32_t queue_base, a_uint8_t profile);
typedef sw_error_t (*adpt_ac_group_buffer_set_func)(
		a_uint32_t dev_id,
		a_uint8_t group_id,
		fal_ac_group_buffer_t *cfg);
typedef sw_error_t (*adpt_queue_counter_cleanup_func)(
		a_uint32_t dev_id, a_uint32_t queue_id);
typedef sw_error_t (*adpt_queue_counter_get_func)(
		a_uint32_t dev_id, a_uint32_t queue_id,
		fal_queue_stats_t *info);
typedef sw_error_t (*adpt_queue_counter_ctrl_get_func)(
		a_uint32_t dev_id, a_bool_t *cnt_en);
typedef sw_error_t (*adpt_queue_counter_ctrl_set_func)(
		a_uint32_t dev_id, a_bool_t cnt_en);
typedef sw_error_t (*adpt_qm_enqueue_ctrl_set_func)(
		a_uint32_t dev_id, a_uint32_t queue_id, a_bool_t enable);
typedef sw_error_t (*adpt_qm_enqueue_ctrl_get_func)(
		a_uint32_t dev_id, a_uint32_t queue_id, a_bool_t *enable);
typedef sw_error_t (*adpt_qm_port_source_profile_set_func)(
		a_uint32_t dev_id, fal_port_t port, a_uint32_t src_profile);
typedef sw_error_t (*adpt_qm_port_source_profile_get_func)(
		a_uint32_t dev_id, fal_port_t port, a_uint32_t *src_profile);


/*portvlan module begin*/
typedef sw_error_t (*adpt_global_qinq_mode_set_func)(a_uint32_t dev_id, fal_global_qinq_mode_t *mode);
typedef sw_error_t (*adpt_global_qinq_mode_get_func)(a_uint32_t dev_id, fal_global_qinq_mode_t *mode);
typedef sw_error_t (*adpt_tpid_set_func)(a_uint32_t dev_id, fal_tpid_t *tpid);
typedef sw_error_t (*adpt_tpid_get_func)(a_uint32_t dev_id, fal_tpid_t *tpid);
typedef sw_error_t (*adpt_egress_tpid_set_func)(a_uint32_t dev_id, fal_tpid_t *tpid);
typedef sw_error_t (*adpt_egress_tpid_get_func)(a_uint32_t dev_id, fal_tpid_t *tpid);
typedef sw_error_t (*adpt_port_qinq_mode_set_func)(a_uint32_t dev_id, fal_port_t port_id, fal_port_qinq_role_t *mode);
typedef sw_error_t (*adpt_port_qinq_mode_get_func)(a_uint32_t dev_id, fal_port_t port_id, fal_port_qinq_role_t *mode);
typedef sw_error_t (*adpt_port_ingress_vlan_filter_set_func)(a_uint32_t dev_id, fal_port_t port_id, fal_ingress_vlan_filter_t *filter);
typedef sw_error_t (*adpt_port_ingress_vlan_filter_get_func)(a_uint32_t dev_id, fal_port_t port_id, fal_ingress_vlan_filter_t *filter);
typedef sw_error_t (*adpt_port_default_vlantag_set_func)(a_uint32_t dev_id, fal_port_t port_id, fal_port_vlan_direction_t direction,
                                 fal_port_default_vid_enable_t *default_vid_en, fal_port_vlan_tag_t *default_tag);
typedef sw_error_t (*adpt_port_default_vlantag_get_func)(a_uint32_t dev_id, fal_port_t port_id, fal_port_vlan_direction_t direction,
                                 fal_port_default_vid_enable_t *default_vid_en, fal_port_vlan_tag_t *default_tag);
typedef sw_error_t (*adpt_port_tag_propagation_set_func)(a_uint32_t dev_id, fal_port_t port_id, fal_port_vlan_direction_t direction,
                                 fal_vlantag_propagation_t *prop);
typedef sw_error_t (*adpt_port_tag_propagation_get_func)(a_uint32_t dev_id, fal_port_t port_id, fal_port_vlan_direction_t direction,
                                 fal_vlantag_propagation_t *prop);
typedef sw_error_t (*adpt_port_vlantag_egmode_set_func)(a_uint32_t dev_id, fal_port_t port_id,
                            fal_vlantag_egress_mode_t *port_egvlanmode);
typedef sw_error_t (*adpt_port_vlantag_egmode_get_func)(a_uint32_t dev_id, fal_port_t port_id,
                            fal_vlantag_egress_mode_t *port_egvlanmode);
typedef sw_error_t (*adpt_port_vlan_xlt_miss_cmd_get_func)(a_uint32_t dev_id, fal_port_t port_id,
                                 fal_fwd_cmd_t *cmd);
typedef sw_error_t (*adpt_port_vlan_xlt_miss_cmd_set_func)(a_uint32_t dev_id, fal_port_t port_id,
                                 fal_fwd_cmd_t cmd);
typedef sw_error_t (*adpt_port_vlan_trans_add_func)(a_uint32_t dev_id, fal_port_t port_id, fal_vlan_trans_entry_t *entry);
typedef sw_error_t (*adpt_port_vlan_trans_get_func)(a_uint32_t dev_id, fal_port_t port_id, fal_vlan_trans_entry_t *entry);
typedef sw_error_t (*adpt_port_vlan_trans_del_func)(a_uint32_t dev_id, fal_port_t port_id, fal_vlan_trans_entry_t *entry);
typedef sw_error_t (*adpt_port_vlan_trans_iterate_func)(a_uint32_t dev_id, fal_port_t port_id,
                                a_uint32_t * iterator, fal_vlan_trans_entry_t *entry);
typedef sw_error_t (*adpt_port_vsi_egmode_set_func)(a_uint32_t dev_id, a_uint32_t vsi, a_uint32_t port_id, fal_pt_1q_egmode_t egmode);
typedef sw_error_t (*adpt_port_vsi_egmode_get_func)(a_uint32_t dev_id, a_uint32_t vsi, a_uint32_t port_id, fal_pt_1q_egmode_t * egmode);
typedef sw_error_t (*adpt_port_vlantag_vsi_egmode_enable_set_func)(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable);
typedef sw_error_t (*adpt_port_vlantag_vsi_egmode_enable_get_func)(a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable);
typedef sw_error_t (*adpt_qinq_mode_set_func)(a_uint32_t dev_id, fal_qinq_mode_t mode);
typedef sw_error_t (*adpt_qinq_mode_get_func)(a_uint32_t dev_id, fal_qinq_mode_t * mode);
typedef sw_error_t (*adpt_port_qinq_role_set_func)(a_uint32_t dev_id, fal_port_t port_id, fal_qinq_port_role_t role);
typedef sw_error_t (*adpt_port_qinq_role_get_func)(a_uint32_t dev_id, fal_port_t port_id, fal_qinq_port_role_t * role);
typedef sw_error_t (*adpt_port_invlan_mode_set_func)(a_uint32_t dev_id, fal_port_t port_id, fal_pt_invlan_mode_t mode);
typedef sw_error_t (*adpt_port_invlan_mode_get_func)(a_uint32_t dev_id, fal_port_t port_id, fal_pt_invlan_mode_t * mode);
typedef sw_error_t (*adpt_port_vlan_trans_adv_add_func)(a_uint32_t dev_id, fal_port_t port_id, fal_port_vlan_direction_t direction,
                                fal_vlan_trans_adv_rule_t * rule, fal_vlan_trans_adv_action_t * action);
typedef sw_error_t (*adpt_port_vlan_trans_adv_del_func)(a_uint32_t dev_id, fal_port_t port_id, fal_port_vlan_direction_t direction,
                                fal_vlan_trans_adv_rule_t * rule, fal_vlan_trans_adv_action_t * action);
typedef sw_error_t (*adpt_port_vlan_trans_adv_getfirst_func)(a_uint32_t dev_id, fal_port_t port_id, fal_port_vlan_direction_t direction,
                                fal_vlan_trans_adv_rule_t * rule, fal_vlan_trans_adv_action_t * action);
typedef sw_error_t (*adpt_port_vlan_trans_adv_getnext_func)(a_uint32_t dev_id, fal_port_t port_id, fal_port_vlan_direction_t direction,
                                fal_vlan_trans_adv_rule_t * rule, fal_vlan_trans_adv_action_t * action);
typedef sw_error_t (*adpt_port_vlan_counter_get_func)(a_uint32_t dev_id, a_uint32_t cnt_index, fal_port_vlan_counter_t * counter);
typedef sw_error_t (*adpt_port_vlan_counter_cleanup_func)(a_uint32_t dev_id, a_uint32_t cnt_index);
typedef sw_error_t (*adpt_portvlan_member_add_func)(a_uint32_t dev_id, fal_port_t port_id, fal_port_t mem_port_id);
typedef sw_error_t (*adpt_portvlan_member_del_func)(a_uint32_t dev_id, fal_port_t port_id, fal_port_t mem_port_id);
typedef sw_error_t (*adpt_portvlan_member_update_func)(a_uint32_t dev_id, fal_port_t port_id, fal_pbmp_t mem_port_map);
typedef sw_error_t (*adpt_portvlan_member_get_func)(a_uint32_t dev_id, fal_port_t port_id, fal_pbmp_t * mem_port_map);
/*portvlan module end*/

/*ctrlpkt module end*/
typedef sw_error_t (*adpt_mgmtctrl_ethtype_profile_set_func)(a_uint32_t dev_id, a_uint32_t profile_id, a_uint32_t ethtype);
typedef sw_error_t (*adpt_mgmtctrl_ethtype_profile_get_func)(a_uint32_t dev_id, a_uint32_t profile_id, a_uint32_t * ethtype);
typedef sw_error_t (*adpt_mgmtctrl_rfdb_profile_set_func)(a_uint32_t dev_id, a_uint32_t profile_id, fal_mac_addr_t *addr);
typedef sw_error_t (*adpt_mgmtctrl_rfdb_profile_get_func)(a_uint32_t dev_id, a_uint32_t profile_id, fal_mac_addr_t *addr);
typedef sw_error_t (*adpt_mgmtctrl_ctrlpkt_profile_add_func)(a_uint32_t dev_id, fal_ctrlpkt_profile_t *ctrlpkt);
typedef sw_error_t (*adpt_mgmtctrl_ctrlpkt_profile_del_func)(a_uint32_t dev_id, fal_ctrlpkt_profile_t *ctrlpkt);
typedef sw_error_t (*adpt_mgmtctrl_ctrlpkt_profile_getfirst_func)(a_uint32_t dev_id, fal_ctrlpkt_profile_t *ctrlpkt);
typedef sw_error_t (*adpt_mgmtctrl_ctrlpkt_profile_getnext_func)(a_uint32_t dev_id, fal_ctrlpkt_profile_t *ctrlpkt);
/*ctrlpkt module end*/

/*service module end*/
typedef sw_error_t (*adpt_servcode_config_set_func)(a_uint32_t dev_id,
		a_uint32_t servcode_index, fal_servcode_config_t *entry);
typedef sw_error_t (*adpt_servcode_config_get_func)(a_uint32_t dev_id,
		a_uint32_t servcode_index, fal_servcode_config_t *entry);
typedef sw_error_t (*adpt_servcode_loopcheck_en_func)(a_uint32_t dev_id, a_bool_t enable);
typedef sw_error_t (*adpt_servcode_loopcheck_status_get_func)(a_uint32_t dev_id, a_bool_t *enable);
/*service module end*/

//pppoe
typedef sw_error_t (*adpt_pppoe_session_table_add_func)(
		a_uint32_t dev_id,
		fal_pppoe_session_t * session_tbl);
typedef sw_error_t (*adpt_pppoe_session_table_del_func)(
		a_uint32_t dev_id,
		fal_pppoe_session_t * session_tbl);
typedef sw_error_t (*adpt_pppoe_session_table_get_func)(
		a_uint32_t dev_id,
		fal_pppoe_session_t * session_tbl);
typedef sw_error_t (*adpt_pppoe_en_set_func)(
		a_uint32_t dev_id,
		a_uint32_t l3_if,
		a_uint32_t enable);
typedef sw_error_t (*adpt_pppoe_en_get_func)(
		a_uint32_t dev_id,
		a_uint32_t l3_if,
		a_uint32_t *enable);

typedef sw_error_t (*adpt_sec_l3_excep_parser_ctrl_set_func)(
		a_uint32_t dev_id,
		fal_l3_excep_parser_ctrl *ctrl);
typedef sw_error_t (*adpt_sec_l3_excep_ctrl_get_func)(
		a_uint32_t dev_id, a_uint32_t excep_type, fal_l3_excep_ctrl_t *ctrl);
typedef sw_error_t (*adpt_sec_l3_excep_parser_ctrl_get_func)(
		a_uint32_t dev_id, fal_l3_excep_parser_ctrl *ctrl);
typedef sw_error_t (*adpt_sec_l4_excep_parser_ctrl_set_func)(
		a_uint32_t dev_id, fal_l4_excep_parser_ctrl *ctrl);
typedef sw_error_t (*adpt_sec_l3_excep_ctrl_set_func)(
		a_uint32_t dev_id, a_uint32_t excep_type, fal_l3_excep_ctrl_t *ctrl);
typedef sw_error_t (*adpt_sec_l4_excep_parser_ctrl_get_func)(
		a_uint32_t dev_id, fal_l4_excep_parser_ctrl *ctrl);

typedef sw_error_t (*adpt_acl_list_bind_func)(a_uint32_t dev_id, a_uint32_t list_id, fal_acl_direc_t direc, fal_acl_bind_obj_t obj_t, a_uint32_t obj_idx);
typedef sw_error_t (*adpt_acl_list_dump_func)(a_uint32_t dev_id);
typedef sw_error_t (*adpt_acl_udf_profile_set_func)(a_uint32_t dev_id, fal_acl_udf_pkt_type_t pkt_type,a_uint32_t udf_idx, fal_acl_udf_type_t udf_type, a_uint32_t offset);
typedef sw_error_t (*adpt_acl_rule_query_func)(a_uint32_t dev_id, a_uint32_t list_id, a_uint32_t rule_id, fal_acl_rule_t * rule);
typedef sw_error_t (*adpt_acl_list_unbind_func)(a_uint32_t dev_id, a_uint32_t list_id, fal_acl_direc_t direc, fal_acl_bind_obj_t obj_t, a_uint32_t obj_idx);
typedef sw_error_t (*adpt_acl_rule_add_func)(a_uint32_t dev_id, a_uint32_t list_id, a_uint32_t rule_id, a_uint32_t rule_nr, fal_acl_rule_t * rule);
typedef sw_error_t (*adpt_acl_rule_delete_func)(a_uint32_t dev_id, a_uint32_t list_id, a_uint32_t rule_id, a_uint32_t rule_nr);
typedef sw_error_t (*adpt_acl_rule_dump_func)(a_uint32_t dev_id);
typedef sw_error_t (*adpt_acl_udf_profile_get_func)(a_uint32_t dev_id, fal_acl_udf_pkt_type_t pkt_type,a_uint32_t udf_idx, fal_acl_udf_type_t *udf_type, a_uint32_t *offset);
typedef sw_error_t (*adpt_acl_list_creat_func)(a_uint32_t dev_id, a_uint32_t list_id, a_uint32_t list_pri);
typedef sw_error_t (*adpt_acl_list_destroy_func)(a_uint32_t dev_id, a_uint32_t list_id);

typedef sw_error_t (*adpt_qos_port_pri_set_func)(a_uint32_t dev_id, fal_port_t port_id,
					fal_qos_pri_precedence_t *pri);
typedef sw_error_t (*adpt_qos_port_pri_get_func)(a_uint32_t dev_id, fal_port_t port_id,
					fal_qos_pri_precedence_t *pri);
typedef sw_error_t (*adpt_qos_cosmap_pcp_get_func)(a_uint32_t dev_id, a_uint8_t group_id,
					a_uint8_t pcp, fal_qos_cosmap_t *cosmap);
typedef sw_error_t (*adpt_queue_scheduler_set_func)(a_uint32_t dev_id,
					a_uint32_t node_id,
					fal_queue_scheduler_level_t level,
					fal_port_t port_id,
					fal_qos_scheduler_cfg_t *scheduler_cfg);
typedef sw_error_t (*adpt_queue_scheduler_get_func)(a_uint32_t dev_id,
					a_uint32_t node_id,
					fal_queue_scheduler_level_t level,
					fal_port_t *port_id,
					fal_qos_scheduler_cfg_t *scheduler_cfg);
typedef sw_error_t (*adpt_port_queues_get_func)(a_uint32_t dev_id,
					fal_port_t port_id,
					fal_queue_bmp_t *queue_bmp);
typedef sw_error_t (*adpt_qos_cosmap_pcp_set_func)(a_uint32_t dev_id, a_uint8_t group_id,
					a_uint8_t pcp, fal_qos_cosmap_t *cosmap);
typedef sw_error_t (*adpt_qos_port_remark_get_func)(a_uint32_t dev_id, fal_port_t port_id,
					fal_qos_remark_enable_t *remark);
typedef sw_error_t (*adpt_qos_cosmap_dscp_get_func)(a_uint32_t dev_id, a_uint8_t group_id,
					a_uint8_t dscp, fal_qos_cosmap_t *cosmap);
typedef sw_error_t (*adpt_qos_cosmap_flow_set_func)(a_uint32_t dev_id, a_uint8_t group_id,
					a_uint16_t flow, fal_qos_cosmap_t *cosmap);
typedef sw_error_t (*adpt_qos_port_group_set_func)(a_uint32_t dev_id, fal_port_t port_id,
					fal_qos_group_t *group);
typedef sw_error_t (*adpt_ring_queue_map_set_func)(a_uint32_t dev_id,
					a_uint32_t ring_id, fal_queue_bmp_t *queue_bmp);
typedef sw_error_t (*adpt_qos_cosmap_dscp_set_func)(a_uint32_t dev_id, a_uint8_t group_id,
					a_uint8_t dscp, fal_qos_cosmap_t *cosmap);
typedef sw_error_t (*adpt_qos_port_remark_set_func)(a_uint32_t dev_id, fal_port_t port_id,
					fal_qos_remark_enable_t *remark);
typedef sw_error_t (*adpt_qos_cosmap_flow_get_func)(a_uint32_t dev_id, a_uint8_t group_id,
					a_uint16_t flow, fal_qos_cosmap_t *cosmap);
typedef sw_error_t (*adpt_qos_port_group_get_func)(a_uint32_t dev_id, fal_port_t port_id,
					fal_qos_group_t *group);
typedef sw_error_t (*adpt_ring_queue_map_get_func)(a_uint32_t dev_id,
					a_uint32_t ring_id, fal_queue_bmp_t *queue_bmp);

//shaper

typedef sw_error_t (*adpt_flow_shaper_set_func)(a_uint32_t dev_id, a_uint32_t flow_id,
					fal_shaper_config_t * shaper);
typedef sw_error_t (*adpt_queue_shaper_get_func)(a_uint32_t dev_id, a_uint32_t queue_id,
					fal_shaper_config_t * shaper);
typedef sw_error_t (*adpt_queue_shaper_token_number_set_func)(a_uint32_t dev_id,a_uint32_t queue_id,
					fal_shaper_token_number_t *token_number);
typedef sw_error_t (*adpt_port_shaper_get_func)(a_uint32_t dev_id, fal_port_t port_id,
					fal_shaper_config_t * shaper);
typedef sw_error_t (*adpt_flow_shaper_time_slot_get_func)(a_uint32_t dev_id, a_uint32_t *time_slot);
typedef sw_error_t (*adpt_port_shaper_time_slot_get_func)(a_uint32_t dev_id, a_uint32_t *time_slot);
typedef sw_error_t (*adpt_flow_shaper_time_slot_set_func)(a_uint32_t dev_id, a_uint32_t time_slot);
typedef sw_error_t (*adpt_port_shaper_token_number_set_func)(a_uint32_t dev_id, fal_port_t port_id,
					fal_shaper_token_number_t *token_number);
typedef sw_error_t (*adpt_queue_shaper_token_number_get_func)(a_uint32_t dev_id, a_uint32_t queue_id,
					fal_shaper_token_number_t *token_number);
typedef sw_error_t (*adpt_queue_shaper_time_slot_get_func)(a_uint32_t dev_id, a_uint32_t *time_slot);
typedef sw_error_t (*adpt_port_shaper_token_number_get_func)(a_uint32_t dev_id, fal_port_t port_id,
					fal_shaper_token_number_t *token_number);
typedef sw_error_t (*adpt_flow_shaper_token_number_set_func)(a_uint32_t dev_id, a_uint32_t flow_id,
					fal_shaper_token_number_t *token_number);
typedef sw_error_t (*adpt_flow_shaper_token_number_get_func)(a_uint32_t dev_id, a_uint32_t flow_id,
					fal_shaper_token_number_t *token_number);
typedef sw_error_t (*adpt_port_shaper_set_func)(a_uint32_t dev_id, fal_port_t port_id,
					fal_shaper_config_t * shaper);
typedef sw_error_t (*adpt_port_shaper_time_slot_set_func)(a_uint32_t dev_id, a_uint32_t time_slot);
typedef sw_error_t (*adpt_flow_shaper_get_func)(a_uint32_t dev_id, a_uint32_t flow_id,
					fal_shaper_config_t * shaper);
typedef sw_error_t (*adpt_queue_shaper_set_func)(a_uint32_t dev_id,a_uint32_t queue_id,
					fal_shaper_config_t * shaper);
typedef sw_error_t (*adpt_queue_shaper_time_slot_set_func)(a_uint32_t dev_id, a_uint32_t time_slot);
typedef sw_error_t (*adpt_shaper_ipg_preamble_length_get_func)(a_uint32_t dev_id, a_uint32_t *ipg_pre_length);
typedef sw_error_t (*adpt_shaper_ipg_preamble_length_set_func)(a_uint32_t dev_id, a_uint32_t ipg_pre_length);

typedef sw_error_t (*adpt_tdm_tick_num_set_func)(a_uint32_t dev_id, a_uint32_t tick_num);
typedef sw_error_t (*adpt_tdm_tick_num_get_func)(a_uint32_t dev_id, a_uint32_t *tick_num);
typedef sw_error_t (*adpt_port_scheduler_cfg_set_func)(a_uint32_t dev_id, a_uint32_t tick_index,
					fal_port_scheduler_cfg_t *cfg);
typedef sw_error_t (*adpt_port_scheduler_cfg_reset_func)(a_uint32_t dev_id, fal_port_t port_id);
typedef sw_error_t (*adpt_port_scheduler_cfg_get_func)(a_uint32_t dev_id, a_uint32_t tick_index,
					fal_port_scheduler_cfg_t *cfg);
typedef sw_error_t (*adpt_scheduler_dequeue_ctrl_get_func)(a_uint32_t dev_id, a_uint32_t queue_id,
					a_bool_t *enable);
typedef sw_error_t (*adpt_scheduler_dequeue_ctrl_set_func)(a_uint32_t dev_id, a_uint32_t queue_id,
					a_bool_t enable);
typedef sw_error_t (*adpt_qos_port_mode_pri_get_func)(a_uint32_t dev_id, fal_port_t port_id,
					fal_qos_mode_t mode, a_uint32_t *pri);
typedef sw_error_t (*adpt_qos_port_mode_pri_set_func)(a_uint32_t dev_id, fal_port_t port_id,
					fal_qos_mode_t mode, a_uint32_t pri);
typedef sw_error_t (*adpt_port_scheduler_resource_get_func)(a_uint32_t dev_id, fal_port_t port_id,
					fal_portscheduler_resource_t *cfg);
typedef sw_error_t (*adpt_port_bufgroup_map_get_func)(a_uint32_t dev_id, fal_port_t port,
			a_uint8_t *group);
typedef sw_error_t (*adpt_bm_port_reserved_buffer_get_func)(a_uint32_t dev_id, fal_port_t port,
			a_uint16_t *prealloc_buff, a_uint16_t *react_buff);
typedef sw_error_t (*adpt_bm_bufgroup_buffer_get_func)(a_uint32_t dev_id, a_uint8_t group,
			a_uint16_t *buff_num);
typedef sw_error_t (*adpt_bm_port_dynamic_thresh_get_func)(a_uint32_t dev_id, fal_port_t port,
			fal_bm_dynamic_cfg_t *cfg);
typedef sw_error_t (*adpt_port_bm_ctrl_get_func)(a_uint32_t dev_id, fal_port_t port, a_bool_t *enable);
typedef sw_error_t (*adpt_bm_bufgroup_buffer_set_func)(a_uint32_t dev_id, a_uint8_t group,
			a_uint16_t buff_num);
typedef sw_error_t (*adpt_port_bufgroup_map_set_func)(a_uint32_t dev_id, fal_port_t port,
			a_uint8_t group);
typedef sw_error_t (*adpt_bm_port_static_thresh_get_func)(a_uint32_t dev_id, fal_port_t port,
			fal_bm_static_cfg_t *cfg);
typedef sw_error_t (*adpt_bm_port_reserved_buffer_set_func)(a_uint32_t dev_id, fal_port_t port,
			a_uint16_t prealloc_buff, a_uint16_t react_buff);
typedef sw_error_t (*adpt_bm_port_static_thresh_set_func)(a_uint32_t dev_id, fal_port_t port,
			fal_bm_static_cfg_t *cfg);
typedef sw_error_t (*adpt_bm_port_dynamic_thresh_set_func)(a_uint32_t dev_id, fal_port_t port,
			fal_bm_dynamic_cfg_t *cfg);
typedef sw_error_t (*adpt_port_bm_ctrl_set_func)(a_uint32_t dev_id, fal_port_t port, a_bool_t enable);
typedef sw_error_t (*adpt_port_tdm_ctrl_set_func)(a_uint32_t dev_id, fal_port_tdm_ctrl_t *ctrl);
typedef sw_error_t (*adpt_port_tdm_tick_cfg_set_func)(a_uint32_t dev_id, a_uint32_t tick_index,
			fal_port_tdm_tick_cfg_t *cfg);
typedef sw_error_t (*adpt_bm_port_counter_get_func)(a_uint32_t dev_id, fal_port_t port,
			fal_bm_port_counter_t *counter);

//policer
typedef sw_error_t (*adpt_acl_policer_counter_get_func)(a_uint32_t dev_id, a_uint32_t index,
		fal_policer_counter_t *counter);
typedef sw_error_t (*adpt_port_policer_counter_get_func)(a_uint32_t dev_id, fal_port_t port_id,
		fal_policer_counter_t *counter);
typedef sw_error_t (*adpt_port_compensation_byte_get_func)(a_uint32_t dev_id, fal_port_t port_id,
		a_uint32_t  *length);
typedef sw_error_t (*adpt_port_policer_entry_get_func)(a_uint32_t dev_id, fal_port_t port_id,
		fal_policer_config_t *policer, fal_policer_action_t *ation);
typedef sw_error_t (*adpt_port_policer_entry_set_func)(a_uint32_t dev_id, fal_port_t port_id,
		fal_policer_config_t *policer, fal_policer_action_t *ation);
typedef sw_error_t (*adpt_acl_policer_entry_get_func)(a_uint32_t dev_id, a_uint32_t index,
		fal_policer_config_t *policer, fal_policer_action_t *ation);
typedef sw_error_t (*adpt_acl_policer_entry_set_func)(a_uint32_t dev_id, a_uint32_t index,
		fal_policer_config_t *policer, fal_policer_action_t *ation);
typedef sw_error_t (*adpt_policer_time_slot_get_func)(a_uint32_t dev_id, a_uint32_t *time_slot);
typedef sw_error_t (*adpt_port_compensation_byte_set_func)(a_uint32_t dev_id, fal_port_t port_id,
		a_uint32_t  length);
typedef sw_error_t (*adpt_policer_time_slot_set_func)(a_uint32_t dev_id, a_uint32_t time_slot);

typedef sw_error_t (*adpt_policer_global_counter_get_func)(a_uint32_t dev_id,
		fal_policer_global_counter_t *counter);
typedef sw_error_t (*adpt_policer_bypass_en_set_func)(a_uint32_t dev_id,
	fal_policer_frame_type_t frame_type, a_bool_t enable);
typedef sw_error_t (*adpt_policer_bypass_en_get_func)(a_uint32_t dev_id,
	fal_policer_frame_type_t frame_type, a_bool_t *enable);

/* misc */
typedef sw_error_t (*adpt_debug_port_counter_enable_func)(a_uint32_t dev_id,
			fal_port_t port_id, fal_counter_en_t * cnt_en);
typedef sw_error_t (*adpt_debug_port_counter_status_get_func)(a_uint32_t dev_id,
			fal_port_t port_id, fal_counter_en_t * cnt_en);
typedef sw_error_t (*adpt_debug_counter_get_func)(a_bool_t show_type);
typedef sw_error_t (*adpt_debug_counter_set_func)(void);
typedef sw_error_t (*adpt_intr_port_link_mask_set_func) (a_uint32_t dev_id,
			fal_port_t port_id, a_uint32_t intr_mask);
typedef sw_error_t (*adpt_intr_port_link_mask_get_func) (a_uint32_t dev_id,
			fal_port_t port_id, a_uint32_t * intr_mask);
typedef sw_error_t (*adpt_intr_port_link_status_get_func)(a_uint32_t dev_id,
			fal_port_t port_id, a_uint32_t * intr_status);

/* uniphy */
typedef sw_error_t (*adpt_uniphy_mode_set_func)(a_uint32_t dev_id, a_uint32_t index, a_uint32_t mode);

/* ptp */
typedef sw_error_t (*adpt_ptp_config_set_func)(a_uint32_t dev_id,
		a_uint32_t port_id, fal_ptp_config_t *config);
typedef sw_error_t (*adpt_ptp_config_get_func)(a_uint32_t dev_id,
		a_uint32_t port_id, fal_ptp_config_t *config);
typedef sw_error_t (*adpt_ptp_reference_clock_set_func)(a_uint32_t dev_id,
		a_uint32_t port_id, fal_ptp_reference_clock_t ref_clock);
typedef sw_error_t (*adpt_ptp_reference_clock_get_func)(a_uint32_t dev_id,
		a_uint32_t port_id, fal_ptp_reference_clock_t *ref_clock);
typedef sw_error_t (*adpt_ptp_rx_timestamp_mode_set_func)(a_uint32_t dev_id,
		a_uint32_t port_id, fal_ptp_rx_timestamp_mode_t ts_mode);
typedef sw_error_t (*adpt_ptp_rx_timestamp_mode_get_func)(a_uint32_t dev_id,
		a_uint32_t port_id, fal_ptp_rx_timestamp_mode_t *ts_mode);
typedef sw_error_t (*adpt_ptp_timestamp_get_func)(a_uint32_t dev_id,
		a_uint32_t port_id, fal_ptp_direction_t direction,
		fal_ptp_pkt_info_t *pkt_info, fal_ptp_time_t *time);
typedef sw_error_t (*adpt_ptp_pkt_timestamp_set_func)(a_uint32_t dev_id,
		a_uint32_t port_id, fal_ptp_time_t *time);
typedef sw_error_t (*adpt_ptp_pkt_timestamp_get_func)(a_uint32_t dev_id,
		a_uint32_t port_id, fal_ptp_time_t *time);
typedef sw_error_t (*adpt_ptp_grandmaster_mode_set_func)(a_uint32_t dev_id,
		a_uint32_t port_id, fal_ptp_grandmaster_mode_t *gm_mode);
typedef sw_error_t (*adpt_ptp_grandmaster_mode_get_func)(a_uint32_t dev_id,
		a_uint32_t port_id, fal_ptp_grandmaster_mode_t *gm_mode);
typedef sw_error_t (*adpt_ptp_rtc_time_get_func)(a_uint32_t dev_id,
		a_uint32_t port_id, fal_ptp_time_t *time);
typedef sw_error_t (*adpt_ptp_rtc_time_set_func)(a_uint32_t dev_id,
		a_uint32_t port_id, fal_ptp_time_t *time);
typedef sw_error_t (*adpt_ptp_rtc_time_clear_func)(a_uint32_t dev_id,
		a_uint32_t port_id);
typedef sw_error_t (*adpt_ptp_rtc_adjtime_set_func)(a_uint32_t dev_id,
		a_uint32_t port_id, fal_ptp_time_t *time);
typedef sw_error_t (*adpt_ptp_rtc_adjfreq_set_func)(a_uint32_t dev_id,
		a_uint32_t port_id, fal_ptp_time_t *time);
typedef sw_error_t (*adpt_ptp_rtc_adjfreq_get_func)(a_uint32_t dev_id,
		a_uint32_t port_id, fal_ptp_time_t *time);
typedef sw_error_t (*adpt_ptp_link_delay_set_func)(a_uint32_t dev_id,
		a_uint32_t port_id, fal_ptp_time_t *time);
typedef sw_error_t (*adpt_ptp_link_delay_get_func)(a_uint32_t dev_id,
		a_uint32_t port_id, fal_ptp_time_t *time);
typedef sw_error_t (*adpt_ptp_security_set_func)(a_uint32_t dev_id,
		a_uint32_t port_id, fal_ptp_security_t *sec);
typedef sw_error_t (*adpt_ptp_security_get_func)(a_uint32_t dev_id,
		a_uint32_t port_id, fal_ptp_security_t *sec);
typedef sw_error_t (*adpt_ptp_pps_signal_control_set_func)(a_uint32_t dev_id,
		a_uint32_t port_id, fal_ptp_pps_signal_control_t *sig_control);
typedef sw_error_t (*adpt_ptp_pps_signal_control_get_func)(a_uint32_t dev_id,
		a_uint32_t port_id, fal_ptp_pps_signal_control_t *sig_control);
typedef sw_error_t (*adpt_ptp_rx_crc_recalc_enable_func)(a_uint32_t dev_id,
		a_uint32_t port_id, a_bool_t status);
typedef sw_error_t (*adpt_ptp_rx_crc_recalc_status_get_func)(a_uint32_t dev_id,
		a_uint32_t port_id, a_bool_t *status);
typedef sw_error_t (*adpt_ptp_asym_correction_set_func)(a_uint32_t dev_id,
		a_uint32_t port_id, fal_ptp_asym_correction_t *asym_cf);
typedef sw_error_t (*adpt_ptp_asym_correction_get_func)(a_uint32_t dev_id,
		a_uint32_t port_id, fal_ptp_asym_correction_t *asym_cf);
typedef sw_error_t (*adpt_ptp_output_waveform_set_func)(a_uint32_t dev_id,
		a_uint32_t port_id, fal_ptp_output_waveform_t *waveform);
typedef sw_error_t (*adpt_ptp_output_waveform_get_func)(a_uint32_t dev_id,
		a_uint32_t port_id, fal_ptp_output_waveform_t *waveform);
typedef sw_error_t (*adpt_ptp_rtc_time_snapshot_enable_func)(a_uint32_t dev_id,
		a_uint32_t port_id, a_bool_t status);
typedef sw_error_t (*adpt_ptp_rtc_time_snapshot_status_get_func)(a_uint32_t dev_id,
		a_uint32_t port_id, a_bool_t *status);
typedef sw_error_t (*adpt_ptp_increment_sync_from_clock_enable_func)(a_uint32_t dev_id,
		a_uint32_t port_id, a_bool_t status);
typedef sw_error_t (*adpt_ptp_increment_sync_from_clock_status_get_func)(a_uint32_t dev_id,
		a_uint32_t port_id, a_bool_t *status);
typedef sw_error_t (*adpt_ptp_tod_uart_set_func)(a_uint32_t dev_id,
		a_uint32_t port_id, fal_ptp_tod_uart_t *tod_uart);
typedef sw_error_t (*adpt_ptp_tod_uart_get_func)(a_uint32_t dev_id,
		a_uint32_t port_id, fal_ptp_tod_uart_t *tod_uart);
typedef sw_error_t (*adpt_ptp_enhanced_timestamp_engine_set_func)(a_uint32_t dev_id,
		a_uint32_t port_id, fal_ptp_direction_t direction,
		fal_ptp_enhanced_ts_engine_t *ts_engine);
typedef sw_error_t (*adpt_ptp_enhanced_timestamp_engine_get_func)(a_uint32_t dev_id,
		a_uint32_t port_id, fal_ptp_direction_t direction,
		fal_ptp_enhanced_ts_engine_t *ts_engine);
typedef sw_error_t (*adpt_ptp_trigger_set_func)(a_uint32_t dev_id, a_uint32_t port_id,
		a_uint32_t trigger_id, fal_ptp_trigger_t *triger);
typedef sw_error_t (*adpt_ptp_trigger_get_func)(a_uint32_t dev_id, a_uint32_t port_id,
		a_uint32_t trigger_id, fal_ptp_trigger_t *triger);
typedef sw_error_t (*adpt_ptp_capture_set_func)(a_uint32_t dev_id, a_uint32_t port_id,
		a_uint32_t capture_id, fal_ptp_capture_t *capture);
typedef sw_error_t (*adpt_ptp_capture_get_func)(a_uint32_t dev_id, a_uint32_t port_id,
		a_uint32_t capture_id, fal_ptp_capture_t *capture);
typedef sw_error_t (*adpt_ptp_interrupt_set_func)(a_uint32_t dev_id,
		a_uint32_t port_id, fal_ptp_interrupt_t *interrupt);
typedef sw_error_t (*adpt_ptp_interrupt_get_func)(a_uint32_t dev_id,
		a_uint32_t port_id, fal_ptp_interrupt_t *interrupt);

/* sfp */
typedef sw_error_t (*adpt_sfp_eeprom_data_get_func)(a_uint32_t dev_id,
		a_uint32_t port_id, fal_sfp_data_t *entry);
typedef sw_error_t (*adpt_sfp_eeprom_data_set_func)(a_uint32_t dev_id,
		a_uint32_t port_id, fal_sfp_data_t *entry);
typedef sw_error_t (*adpt_sfp_diag_ctrl_status_get_func)(a_uint32_t dev_id,
		a_uint32_t port_id, fal_sfp_ctrl_status_t *ctrl_status);
typedef sw_error_t (*adpt_sfp_diag_extenal_calibration_const_get_func)(a_uint32_t dev_id,
		a_uint32_t port_id, fal_sfp_cal_const_t *cal_const);
typedef sw_error_t (*adpt_sfp_link_length_get_func)(a_uint32_t dev_id,
		a_uint32_t port_id, fal_sfp_link_length_t *link_len);
typedef sw_error_t (*adpt_sfp_diag_internal_threshold_get_func)(a_uint32_t dev_id,
		a_uint32_t port_id, fal_sfp_internal_threshold_t *threshold);
typedef sw_error_t (*adpt_sfp_diag_realtime_get_func)(a_uint32_t dev_id,
		a_uint32_t port_id, fal_sfp_realtime_diag_t *real_diag);
typedef sw_error_t (*adpt_sfp_laser_wavelength_get_func)(a_uint32_t dev_id,
		a_uint32_t port_id, fal_sfp_laser_wavelength_t *laser_wavelen);
typedef sw_error_t (*adpt_sfp_option_get_func)(a_uint32_t dev_id,
		a_uint32_t port_id, fal_sfp_option_t *option);
typedef sw_error_t (*adpt_sfp_checkcode_get_func)(a_uint32_t dev_id,
		a_uint32_t port_id, fal_sfp_cc_type_t cc_type, a_uint8_t *ccode);
typedef sw_error_t (*adpt_sfp_diag_alarm_warning_flag_get_func)(a_uint32_t dev_id,
		a_uint32_t port_id, fal_sfp_alarm_warn_flag_t *alarm_warn_flag);
typedef sw_error_t (*adpt_sfp_device_type_get_func)(a_uint32_t dev_id,
		a_uint32_t port_id, fal_sfp_dev_type_t *sfp_id);
typedef sw_error_t (*adpt_sfp_vendor_info_get_func)(a_uint32_t dev_id,
		a_uint32_t port_id, fal_sfp_vendor_info_t *vender_info);
typedef sw_error_t (*adpt_sfp_transceiver_code_get_func)(a_uint32_t dev_id,
		a_uint32_t port_id, fal_sfp_transc_code_t *transc_code);
typedef sw_error_t (*adpt_sfp_ctrl_rate_get_func)(a_uint32_t dev_id,
		a_uint32_t port_id, fal_sfp_rate_t *rate_limit);
typedef sw_error_t (*adpt_sfp_enhanced_cfg_get_func)(a_uint32_t dev_id,
		a_uint32_t port_id, fal_sfp_enhanced_cfg_t *enhanced_feature);
typedef sw_error_t (*adpt_sfp_rate_encode_get_func)(a_uint32_t dev_id,
		a_uint32_t port_id, fal_sfp_rate_encode_t *encode);

/*led*/
typedef sw_error_t (*adpt_led_ctrl_pattern_set_func)(a_uint32_t dev_id,
	led_pattern_group_t group, led_pattern_id_t led_pattern_id,
	led_ctrl_pattern_t * pattern);
typedef sw_error_t (*adpt_led_ctrl_pattern_get_func)(a_uint32_t dev_id,
	led_pattern_group_t group, led_pattern_id_t led_pattern_id,
	led_ctrl_pattern_t * pattern);
typedef sw_error_t (*adpt_led_ctrl_source_set_func)(a_uint32_t dev_id,
	a_uint32_t source_id, led_ctrl_pattern_t *pattern);

typedef struct
{
	ssdk_chip_type chip_type;
	a_uint32_t chip_revision;
}adpt_chip_ver_t;
typedef struct
{
	a_uint32_t adpt_fdb_func_bitmap[2];
	adpt_fdb_first_func adpt_fdb_first;
	adpt_fdb_next_func adpt_fdb_next;
	adpt_fdb_add_func adpt_fdb_add;
	adpt_fdb_age_time_set_func adpt_fdb_age_time_set;
	adpt_fdb_extend_next_func adpt_fdb_extend_next;
	adpt_fdb_learn_ctrl_get_func adpt_fdb_learn_ctrl_get;
	adpt_fdb_age_time_get_func adpt_fdb_age_time_get;
	adpt_port_fdb_learn_limit_set_func adpt_port_fdb_learn_limit_set;
	adpt_fdb_port_add_func adpt_fdb_port_add;
	adpt_fdb_port_learn_set_func adpt_fdb_port_learn_set;
	adpt_fdb_port_learn_get_func adpt_fdb_port_learn_get;
	adpt_fdb_port_newaddr_lrn_set_func adpt_fdb_port_newaddr_lrn_set;
	adpt_fdb_port_newaddr_lrn_get_func adpt_fdb_port_newaddr_lrn_get;
	adpt_fdb_port_stamove_set_func adpt_fdb_port_stamove_set;
	adpt_fdb_port_stamove_get_func adpt_fdb_port_stamove_get;
	adpt_port_fdb_learn_counter_get_func adpt_port_fdb_learn_counter_get;
	adpt_fdb_extend_first_func adpt_fdb_extend_first;
	adpt_fdb_transfer_func adpt_fdb_transfer;
	adpt_fdb_port_del_func adpt_fdb_port_del;
	adpt_fdb_find_func adpt_fdb_find;
	adpt_fdb_learn_ctrl_set_func adpt_fdb_learn_ctrl_set;
	adpt_port_fdb_learn_exceed_cmd_get_func adpt_port_fdb_learn_exceed_cmd_get;
	adpt_fdb_del_by_port_func adpt_fdb_del_by_port;
	adpt_port_fdb_learn_limit_get_func adpt_port_fdb_learn_limit_get;
	adpt_fdb_age_ctrl_set_func adpt_fdb_age_ctrl_set;
	adpt_fdb_del_by_mac_func adpt_fdb_del_by_mac;
	adpt_fdb_iterate_func adpt_fdb_iterate;
	adpt_port_fdb_learn_exceed_cmd_set_func adpt_port_fdb_learn_exceed_cmd_set;
	adpt_fdb_del_all_func adpt_fdb_del_all;
	adpt_fdb_age_ctrl_get_func adpt_fdb_age_ctrl_get;
	adpt_fdb_port_maclimit_ctrl_set_func adpt_fdb_port_maclimit_ctrl_set;
	adpt_fdb_port_maclimit_ctrl_get_func adpt_fdb_port_maclimit_ctrl_get;
	adpt_fdb_del_by_fid_func adpt_fdb_del_by_fid;
	/*mib*/
	a_uint32_t adpt_mib_func_bitmap;
	adpt_mib_cpukeep_get_func adpt_mib_cpukeep_get;
	adpt_mib_cpukeep_set_func adpt_mib_cpukeep_set;
	adpt_get_mib_info_func adpt_get_mib_info;
	adpt_get_tx_mib_info_func adpt_get_tx_mib_info;
	adpt_mib_status_set_func adpt_mib_status_set;
	adpt_mib_port_flush_counters_func adpt_mib_port_flush_counters;
	adpt_mib_status_get_func adpt_mib_status_get;
	adpt_get_rx_mib_info_func adpt_get_rx_mib_info;
	adpt_get_xgmib_info_func adpt_get_xgmib_info;
	adpt_get_tx_xgmib_info_func adpt_get_tx_xgmib_info;
	adpt_get_rx_xgmib_info_func adpt_get_rx_xgmib_info;

	a_uint32_t adpt_stp_func_bitmap;
	adpt_stp_port_state_get_func adpt_stp_port_state_get;
	adpt_stp_port_state_set_func adpt_stp_port_state_set;

    /*vsi*/
	a_uint32_t adpt_vsi_func_bitmap;
	adpt_port_vlan_vsi_set_func adpt_port_vlan_vsi_set;
	adpt_port_vlan_vsi_get_func adpt_port_vlan_vsi_get;
	adpt_port_vsi_set_func adpt_port_vsi_set;
	adpt_port_vsi_get_func adpt_port_vsi_get;
	adpt_vsi_stamove_set_func adpt_vsi_stamove_set;
	adpt_vsi_stamove_get_func adpt_vsi_stamove_get;
	adpt_vsi_newaddr_lrn_set_func adpt_vsi_newaddr_lrn_set;
	adpt_vsi_newaddr_lrn_get_func adpt_vsi_newaddr_lrn_get;
	adpt_vsi_member_set_func adpt_vsi_member_set;
	adpt_vsi_member_get_func adpt_vsi_member_get;
	adpt_vsi_counter_get_func adpt_vsi_counter_get;
	adpt_vsi_counter_cleanup_func adpt_vsi_counter_cleanup;

	// port_ctrl
	a_uint32_t adpt_port_ctrl_func_bitmap[3];
	adpt_port_local_loopback_get_func adpt_port_local_loopback_get;
	adpt_port_autoneg_restart_func adpt_port_autoneg_restart;
	adpt_port_duplex_set_func adpt_port_duplex_set;
	adpt_port_rxmac_status_get_func adpt_port_rxmac_status_get;
	adpt_port_cdt_func adpt_port_cdt;
	adpt_port_txmac_status_set_func adpt_port_txmac_status_set;
	adpt_port_combo_fiber_mode_set_func adpt_port_combo_fiber_mode_set;
	adpt_port_combo_medium_status_get_func adpt_port_combo_medium_status_get;
	adpt_port_magic_frame_mac_set_func adpt_port_magic_frame_mac_set;
	adpt_port_powersave_set_func adpt_port_powersave_set;
	adpt_port_hibernate_set_func adpt_port_hibernate_set;
	adpt_port_max_frame_size_get_func adpt_port_max_frame_size_get;
	adpt_port_8023az_get_func adpt_port_8023az_get;
	adpt_port_rxfc_status_get_func adpt_port_rxfc_status_get;
	adpt_port_txfc_status_get_func adpt_port_txfc_status_get;
	adpt_port_remote_loopback_set_func adpt_port_remote_loopback_set;
	adpt_port_flowctrl_set_func adpt_port_flowctrl_set;
	adpt_port_mru_set_func adpt_port_mru_set;
	adpt_port_autoneg_status_get_func adpt_port_autoneg_status_get;
	adpt_port_txmac_status_get_func adpt_port_txmac_status_get;
	adpt_port_mdix_get_func adpt_port_mdix_get;
	adpt_ports_link_status_get_func adpt_ports_link_status_get;
	adpt_port_mac_loopback_set_func adpt_port_mac_loopback_set;
	adpt_port_phy_id_get_func adpt_port_phy_id_get;
	adpt_port_mru_get_func adpt_port_mru_get;
	adpt_port_power_on_func adpt_port_power_on;
	adpt_port_speed_set_func adpt_port_speed_set;
	adpt_port_interface_mode_get_func adpt_port_interface_mode_get;
	adpt_port_duplex_get_func adpt_port_duplex_get;
	adpt_port_autoneg_adv_get_func adpt_port_autoneg_adv_get;
	adpt_port_mdix_status_get_func adpt_port_mdix_status_get;
	adpt_port_mtu_set_func adpt_port_mtu_set;
	adpt_port_link_status_get_func adpt_port_link_status_get;
	adpt_port_8023az_set_func adpt_port_8023az_set;
	adpt_port_powersave_get_func adpt_port_powersave_get;
	adpt_port_combo_prefer_medium_get_func adpt_port_combo_prefer_medium_get;
	adpt_port_max_frame_size_set_func adpt_port_max_frame_size_set;
	adpt_port_combo_prefer_medium_set_func adpt_port_combo_prefer_medium_set;
	adpt_port_power_off_func adpt_port_power_off;
	adpt_port_txfc_status_set_func adpt_port_txfc_status_set;
	adpt_port_counter_set_func adpt_port_counter_set;
	adpt_port_combo_fiber_mode_get_func adpt_port_combo_fiber_mode_get;
	adpt_port_local_loopback_set_func adpt_port_local_loopback_set;
	adpt_port_wol_status_set_func adpt_port_wol_status_set;
	adpt_port_magic_frame_mac_get_func adpt_port_magic_frame_mac_get;
	adpt_port_flowctrl_get_func adpt_port_flowctrl_get;
	adpt_port_rxmac_status_set_func adpt_port_rxmac_status_set;
	adpt_port_counter_get_func adpt_port_counter_get;
	adpt_port_interface_mode_set_func adpt_port_interface_mode_set;
	adpt_port_mac_loopback_get_func adpt_port_mac_loopback_get;
	adpt_port_hibernate_get_func adpt_port_hibernate_get;
	adpt_port_autoneg_adv_set_func adpt_port_autoneg_adv_set;
	adpt_port_remote_loopback_get_func adpt_port_remote_loopback_get;
	adpt_port_counter_show_func adpt_port_counter_show;
	adpt_port_autoneg_enable_func adpt_port_autoneg_enable;
	adpt_port_mtu_get_func adpt_port_mtu_get;
	adpt_port_interface_mode_status_get_func adpt_port_interface_mode_status_get;
	adpt_port_reset_func adpt_port_reset;
	adpt_port_rxfc_status_set_func adpt_port_rxfc_status_set;
	adpt_port_speed_get_func adpt_port_speed_get;
	adpt_port_mdix_set_func adpt_port_mdix_set;
	adpt_port_wol_status_get_func adpt_port_wol_status_get;
	adpt_port_source_filter_set_func adpt_port_source_filter_set;
	adpt_port_source_filter_get_func adpt_port_source_filter_get;
	adpt_port_mux_mac_type_set_func adpt_port_mux_mac_type_set;
	adpt_port_mac_speed_set_func adpt_port_mac_speed_set;
	adpt_port_mac_duplex_set_func adpt_port_mac_duplex_set;
	adpt_port_netdev_notify_func adpt_port_netdev_notify_set;
	adpt_port_polling_sw_sync_func adpt_port_polling_sw_sync_set;

	adpt_port_bridge_txmac_set_func adpt_port_bridge_txmac_set;

	adpt_port_interface_mode_apply_func adpt_port_interface_mode_apply;
	adpt_port_interface_3az_status_set_func adpt_port_interface_3az_status_set;
	adpt_port_interface_3az_status_get_func adpt_port_interface_3az_status_get;
	adpt_port_flowctrl_forcemode_set_func adpt_port_flowctrl_forcemode_set;
	adpt_port_flowctrl_forcemode_get_func adpt_port_flowctrl_forcemode_get;
	adpt_port_promisc_mode_set_func adpt_port_promisc_mode_set;
	adpt_port_promisc_mode_get_func adpt_port_promisc_mode_get;
	adpt_port_interface_eee_cfg_set_func adpt_port_interface_eee_cfg_set;
	adpt_port_interface_eee_cfg_get_func adpt_port_interface_eee_cfg_get;
	adpt_port_source_filter_config_set_func adpt_port_source_filter_config_set;
	adpt_port_source_filter_config_get_func adpt_port_source_filter_config_get;
	adpt_switch_port_loopback_set_func adpt_switch_port_loopback_set;
	adpt_switch_port_loopback_get_func adpt_switch_port_loopback_get;
	adpt_port_phy_status_get_func adpt_port_phy_status_get;
// mirror
	a_uint32_t adpt_mirror_func_bitmap;
	adpt_mirr_port_in_set_func adpt_mirr_port_in_set;
	adpt_mirr_port_in_get_func adpt_mirr_port_in_get;
	adpt_mirr_port_eg_set_func adpt_mirr_port_eg_set;
	adpt_mirr_port_eg_get_func adpt_mirr_port_eg_get;
	adpt_mirr_analysis_port_set_func adpt_mirr_analysis_port_set;
	adpt_mirr_analysis_port_get_func adpt_mirr_analysis_port_get;
	adpt_mirr_analysis_config_set_func adpt_mirr_analysis_config_set;
	adpt_mirr_analysis_config_get_func adpt_mirr_analysis_config_get;
//rss hash
	a_uint32_t adpt_rss_hash_func_bitmap;
	adpt_rss_hash_config_set_func adpt_rss_hash_config_set;
	adpt_rss_hash_config_get_func adpt_rss_hash_config_get;
//trunk
	a_uint32_t adpt_trunk_func_bitmap;
	adpt_trunk_fail_over_en_get_func adpt_trunk_fail_over_en_get;
	adpt_trunk_hash_mode_get_func adpt_trunk_hash_mode_get;
	adpt_trunk_group_get_func adpt_trunk_group_get;
	adpt_trunk_group_set_func adpt_trunk_group_set;
	adpt_trunk_fail_over_en_set_func adpt_trunk_fail_over_en_set;
	adpt_trunk_hash_mode_set_func adpt_trunk_hash_mode_set;

	/* ip */
	a_uint32_t adpt_ip_func_bitmap[2];
	adpt_ip_network_route_get_func adpt_ip_network_route_get;
	adpt_ip_network_route_add_func adpt_ip_network_route_add;
	adpt_ip_network_route_del_func adpt_ip_network_route_del;
	adpt_ip_host_add_func adpt_ip_host_add;
	adpt_ip_vsi_sg_cfg_get_func adpt_ip_vsi_sg_cfg_get;
	adpt_ip_pub_addr_set_func adpt_ip_pub_addr_set;
	adpt_ip_pub_addr_get_func adpt_ip_pub_addr_get;
	adpt_ip_port_sg_cfg_set_func adpt_ip_port_sg_cfg_set;
	adpt_ip_port_intf_get_func adpt_ip_port_intf_get;
	adpt_ip_vsi_arp_sg_cfg_set_func adpt_ip_vsi_arp_sg_cfg_set;
	adpt_ip_port_intf_set_func adpt_ip_port_intf_set;
	adpt_ip_vsi_sg_cfg_set_func adpt_ip_vsi_sg_cfg_set;
	adpt_ip_host_next_func adpt_ip_host_next;
	adpt_ip_port_macaddr_set_func adpt_ip_port_macaddr_set;
	adpt_ip_vsi_intf_get_func adpt_ip_vsi_intf_get;
	adpt_ip_port_sg_cfg_get_func adpt_ip_port_sg_cfg_get;
	adpt_ip_intf_get_func adpt_ip_intf_get;
	adpt_ip_host_del_func adpt_ip_host_del;
	adpt_ip_route_mismatch_get_func adpt_ip_route_mismatch_get;
	adpt_ip_vsi_arp_sg_cfg_get_func adpt_ip_vsi_arp_sg_cfg_get;
	adpt_ip_port_arp_sg_cfg_set_func adpt_ip_port_arp_sg_cfg_set;
	adpt_ip_vsi_mc_mode_set_func adpt_ip_vsi_mc_mode_set;
	adpt_ip_vsi_intf_set_func adpt_ip_vsi_intf_set;
	adpt_ip_nexthop_get_func adpt_ip_nexthop_get;
	adpt_ip_route_mismatch_set_func adpt_ip_route_mismatch_set;
	adpt_ip_host_get_func adpt_ip_host_get;
	adpt_ip_intf_set_func adpt_ip_intf_set;
	adpt_ip_vsi_mc_mode_get_func adpt_ip_vsi_mc_mode_get;
	adpt_ip_port_macaddr_get_func adpt_ip_port_macaddr_get;
	adpt_ip_port_arp_sg_cfg_get_func adpt_ip_port_arp_sg_cfg_get;
	adpt_ip_nexthop_set_func adpt_ip_nexthop_set;
	adpt_ip_global_ctrl_get_func adpt_ip_global_ctrl_get;
	adpt_ip_global_ctrl_set_func adpt_ip_global_ctrl_set;
	/* flow */
	a_uint32_t adpt_flow_func_bitmap;
	adpt_flow_host_add_func adpt_flow_host_add;
	adpt_flow_entry_get_func adpt_flow_entry_get;
	adpt_flow_entry_del_func adpt_flow_entry_del;
	adpt_flow_entry_next_func adpt_flow_entry_next;
	adpt_flow_status_get_func adpt_flow_status_get;
	adpt_flow_ctrl_set_func adpt_flow_ctrl_set;
	adpt_flow_age_timer_get_func adpt_flow_age_timer_get;
	adpt_flow_status_set_func adpt_flow_status_set;
	adpt_flow_host_get_func adpt_flow_host_get;
	adpt_flow_host_del_func adpt_flow_host_del;
	adpt_flow_ctrl_get_func adpt_flow_ctrl_get;
	adpt_flow_age_timer_set_func adpt_flow_age_timer_set;
	adpt_flow_entry_add_func adpt_flow_entry_add;
	adpt_flow_global_cfg_get_func adpt_flow_global_cfg_get;
	adpt_flow_global_cfg_set_func adpt_flow_global_cfg_set;

	/* qm */
	a_uint32_t adpt_qm_func_bitmap[2];
	adpt_ucast_hash_map_set_func adpt_ucast_hash_map_set;
	adpt_ac_dynamic_threshold_get_func adpt_ac_dynamic_threshold_get;
	adpt_ucast_queue_base_profile_get_func adpt_ucast_queue_base_profile_get;
	adpt_port_mcast_priority_class_get_func adpt_port_mcast_priority_class_get;
	adpt_ac_dynamic_threshold_set_func adpt_ac_dynamic_threshold_set;
	adpt_ac_prealloc_buffer_set_func adpt_ac_prealloc_buffer_set;
	adpt_ucast_default_hash_get_func adpt_ucast_default_hash_get;
	adpt_ucast_default_hash_set_func adpt_ucast_default_hash_set;
	adpt_ac_queue_group_get_func adpt_ac_queue_group_get;
	adpt_ac_ctrl_get_func adpt_ac_ctrl_get;
	adpt_ac_prealloc_buffer_get_func adpt_ac_prealloc_buffer_get;
	adpt_port_mcast_priority_class_set_func adpt_port_mcast_priority_class_set;
	adpt_ucast_hash_map_get_func adpt_ucast_hash_map_get;
	adpt_ac_static_threshold_set_func adpt_ac_static_threshold_set;
	adpt_ac_queue_group_set_func adpt_ac_queue_group_set;
	adpt_ac_group_buffer_get_func adpt_ac_group_buffer_get;
	adpt_mcast_cpu_code_class_get_func adpt_mcast_cpu_code_class_get;
	adpt_ac_ctrl_set_func adpt_ac_ctrl_set;
	adpt_ucast_priority_class_get_func adpt_ucast_priority_class_get;
	adpt_queue_flush_func adpt_queue_flush;
	adpt_mcast_cpu_code_class_set_func adpt_mcast_cpu_code_class_set;
	adpt_ucast_priority_class_set_func adpt_ucast_priority_class_set;
	adpt_ac_static_threshold_get_func adpt_ac_static_threshold_get;
	adpt_ucast_queue_base_profile_set_func adpt_ucast_queue_base_profile_set;
	adpt_ac_group_buffer_set_func adpt_ac_group_buffer_set;
	adpt_queue_counter_cleanup_func adpt_queue_counter_cleanup;
	adpt_queue_counter_get_func adpt_queue_counter_get;
	adpt_queue_counter_ctrl_get_func adpt_queue_counter_ctrl_get;
	adpt_queue_counter_ctrl_set_func adpt_queue_counter_ctrl_set;
	adpt_qm_enqueue_ctrl_set_func adpt_qm_enqueue_ctrl_set;
	adpt_qm_enqueue_ctrl_get_func adpt_qm_enqueue_ctrl_get;
	adpt_qm_port_source_profile_set_func adpt_qm_port_source_profile_set;
	adpt_qm_port_source_profile_get_func adpt_qm_port_source_profile_get;

	/*portvlan module begin*/
	a_uint32_t adpt_portvlan_func_bitmap[2];
	adpt_global_qinq_mode_set_func adpt_global_qinq_mode_set;
	adpt_global_qinq_mode_get_func adpt_global_qinq_mode_get;
	adpt_tpid_set_func adpt_tpid_set;
	adpt_tpid_get_func adpt_tpid_get;
	adpt_egress_tpid_set_func adpt_egress_tpid_set;
	adpt_egress_tpid_get_func adpt_egress_tpid_get;
	adpt_port_qinq_mode_set_func adpt_port_qinq_mode_set;
	adpt_port_qinq_mode_get_func adpt_port_qinq_mode_get;
	adpt_port_ingress_vlan_filter_set_func adpt_port_ingress_vlan_filter_set;
	adpt_port_ingress_vlan_filter_get_func adpt_port_ingress_vlan_filter_get;
	adpt_port_default_vlantag_set_func adpt_port_default_vlantag_set;
	adpt_port_default_vlantag_get_func adpt_port_default_vlantag_get;
	adpt_port_tag_propagation_set_func adpt_port_tag_propagation_set;
	adpt_port_tag_propagation_get_func adpt_port_tag_propagation_get;
	adpt_port_vlantag_egmode_set_func adpt_port_vlantag_egmode_set;
	adpt_port_vlantag_egmode_get_func adpt_port_vlantag_egmode_get;
	adpt_port_vlan_xlt_miss_cmd_set_func adpt_port_vlan_xlt_miss_cmd_set;
	adpt_port_vlan_xlt_miss_cmd_get_func adpt_port_vlan_xlt_miss_cmd_get;
	adpt_port_vlan_trans_add_func adpt_port_vlan_trans_add;
	adpt_port_vlan_trans_get_func adpt_port_vlan_trans_get;
	adpt_port_vlan_trans_del_func adpt_port_vlan_trans_del;
	adpt_port_vlan_trans_iterate_func adpt_port_vlan_trans_iterate;
	adpt_port_vsi_egmode_set_func adpt_port_vsi_egmode_set;
	adpt_port_vsi_egmode_get_func adpt_port_vsi_egmode_get;
	adpt_port_vlantag_vsi_egmode_enable_set_func adpt_port_vlantag_vsi_egmode_enable_set;
	adpt_port_vlantag_vsi_egmode_enable_get_func adpt_port_vlantag_vsi_egmode_enable_get;
	adpt_qinq_mode_set_func adpt_qinq_mode_set;
	adpt_qinq_mode_get_func adpt_qinq_mode_get;
	adpt_port_qinq_role_set_func adpt_port_qinq_role_set;
	adpt_port_qinq_role_get_func adpt_port_qinq_role_get;
	adpt_port_invlan_mode_set_func adpt_port_invlan_mode_set;
	adpt_port_invlan_mode_get_func adpt_port_invlan_mode_get;
	adpt_port_vlan_trans_adv_add_func adpt_port_vlan_trans_adv_add;
	adpt_port_vlan_trans_adv_del_func adpt_port_vlan_trans_adv_del;
	adpt_port_vlan_trans_adv_getfirst_func adpt_port_vlan_trans_adv_getfirst;
	adpt_port_vlan_trans_adv_getnext_func adpt_port_vlan_trans_adv_getnext;
	adpt_port_vlan_counter_get_func adpt_port_vlan_counter_get;
	adpt_port_vlan_counter_cleanup_func adpt_port_vlan_counter_cleanup;
	adpt_portvlan_member_add_func adpt_portvlan_member_add;
	adpt_portvlan_member_del_func adpt_portvlan_member_del;
	adpt_portvlan_member_update_func adpt_portvlan_member_update;
	adpt_portvlan_member_get_func adpt_portvlan_member_get;
	/*portvlan module end*/

	/*ctrlpkt module begin*/
	a_uint32_t adpt_ctrlpkt_func_bitmap;
	adpt_mgmtctrl_ethtype_profile_set_func adpt_mgmtctrl_ethtype_profile_set;
	adpt_mgmtctrl_ethtype_profile_get_func adpt_mgmtctrl_ethtype_profile_get;
	adpt_mgmtctrl_rfdb_profile_set_func adpt_mgmtctrl_rfdb_profile_set;
	adpt_mgmtctrl_rfdb_profile_get_func adpt_mgmtctrl_rfdb_profile_get;
	adpt_mgmtctrl_ctrlpkt_profile_add_func adpt_mgmtctrl_ctrlpkt_profile_add;
	adpt_mgmtctrl_ctrlpkt_profile_del_func adpt_mgmtctrl_ctrlpkt_profile_del;
	adpt_mgmtctrl_ctrlpkt_profile_getfirst_func adpt_mgmtctrl_ctrlpkt_profile_getfirst;
	adpt_mgmtctrl_ctrlpkt_profile_getnext_func adpt_mgmtctrl_ctrlpkt_profile_getnext;
	/*ctrlpkt module end*/

	/*servcode module begin*/
	a_uint32_t adpt_servcode_func_bitmap;
	adpt_servcode_config_set_func adpt_servcode_config_set;
	adpt_servcode_config_get_func adpt_servcode_config_get;
	adpt_servcode_loopcheck_en_func adpt_servcode_loopcheck_en;
	adpt_servcode_loopcheck_status_get_func adpt_servcode_loopcheck_status_get;
	/*servcode module end*/

	/* pppoe */
	a_uint32_t adpt_pppoe_func_bitmap;
	adpt_pppoe_session_table_add_func adpt_pppoe_session_table_add;
	adpt_pppoe_session_table_del_func adpt_pppoe_session_table_del;
	adpt_pppoe_session_table_get_func adpt_pppoe_session_table_get;
	adpt_pppoe_en_set_func adpt_pppoe_en_set;
	adpt_pppoe_en_get_func adpt_pppoe_en_get;

	/*sec */
	a_uint32_t adpt_sec_func_bitmap;
	adpt_sec_l3_excep_parser_ctrl_set_func adpt_sec_l3_excep_parser_ctrl_set;
	adpt_sec_l3_excep_ctrl_get_func adpt_sec_l3_excep_ctrl_get;
	adpt_sec_l3_excep_parser_ctrl_get_func adpt_sec_l3_excep_parser_ctrl_get;
	adpt_sec_l4_excep_parser_ctrl_set_func adpt_sec_l4_excep_parser_ctrl_set;
	adpt_sec_l3_excep_ctrl_set_func adpt_sec_l3_excep_ctrl_set;
	adpt_sec_l4_excep_parser_ctrl_get_func adpt_sec_l4_excep_parser_ctrl_get;

	/*acl*/
	a_uint32_t adpt_acl_func_bitmap;
	adpt_acl_list_bind_func adpt_acl_list_bind;
	adpt_acl_list_dump_func adpt_acl_list_dump;
	adpt_acl_udf_profile_set_func adpt_acl_udf_profile_set;
	adpt_acl_rule_query_func adpt_acl_rule_query;
	adpt_acl_list_unbind_func adpt_acl_list_unbind;
	adpt_acl_rule_add_func adpt_acl_rule_add;
	adpt_acl_rule_delete_func adpt_acl_rule_delete;
	adpt_acl_rule_dump_func adpt_acl_rule_dump;
	adpt_acl_udf_profile_get_func adpt_acl_udf_profile_get;
	adpt_acl_list_creat_func adpt_acl_list_creat;
	adpt_acl_list_destroy_func adpt_acl_list_destroy;

	/* qos */
	a_uint32_t adpt_qos_func_bitmap;
	adpt_qos_port_pri_set_func adpt_qos_port_pri_set;
	adpt_qos_port_pri_get_func adpt_qos_port_pri_get;
	adpt_qos_cosmap_pcp_get_func adpt_qos_cosmap_pcp_get;
	adpt_queue_scheduler_set_func adpt_queue_scheduler_set;
	adpt_queue_scheduler_get_func adpt_queue_scheduler_get;
	adpt_port_queues_get_func adpt_port_queues_get;
	adpt_qos_cosmap_pcp_set_func adpt_qos_cosmap_pcp_set;
	adpt_qos_port_remark_get_func adpt_qos_port_remark_get;
	adpt_qos_cosmap_dscp_get_func adpt_qos_cosmap_dscp_get;
	adpt_qos_cosmap_flow_set_func adpt_qos_cosmap_flow_set;
	adpt_qos_port_group_set_func adpt_qos_port_group_set;
	adpt_ring_queue_map_set_func adpt_ring_queue_map_set;
	adpt_qos_cosmap_dscp_set_func adpt_qos_cosmap_dscp_set;
	adpt_qos_port_remark_set_func adpt_qos_port_remark_set;
	adpt_qos_cosmap_flow_get_func adpt_qos_cosmap_flow_get;
	adpt_qos_port_group_get_func adpt_qos_port_group_get;
	adpt_ring_queue_map_get_func adpt_ring_queue_map_get;
	adpt_tdm_tick_num_set_func adpt_tdm_tick_num_set;
	adpt_tdm_tick_num_get_func adpt_tdm_tick_num_get;
	adpt_port_scheduler_cfg_set_func adpt_port_scheduler_cfg_set;
	adpt_port_scheduler_cfg_get_func adpt_port_scheduler_cfg_get;
	adpt_scheduler_dequeue_ctrl_get_func adpt_scheduler_dequeue_ctrl_get;
	adpt_scheduler_dequeue_ctrl_set_func adpt_scheduler_dequeue_ctrl_set;
	adpt_qos_port_mode_pri_get_func adpt_qos_port_mode_pri_get;
	adpt_qos_port_mode_pri_set_func adpt_qos_port_mode_pri_set;
	adpt_port_scheduler_cfg_reset_func adpt_port_scheduler_cfg_reset;
	adpt_port_scheduler_resource_get_func adpt_port_scheduler_resource_get;

	/* bm */
	a_uint32_t adpt_bm_func_bitmap;
	adpt_port_bufgroup_map_get_func adpt_port_bufgroup_map_get;
	adpt_bm_port_reserved_buffer_get_func adpt_bm_port_reserved_buffer_get;
	adpt_bm_bufgroup_buffer_get_func adpt_bm_bufgroup_buffer_get;
	adpt_bm_port_dynamic_thresh_get_func adpt_bm_port_dynamic_thresh_get;
	adpt_port_bm_ctrl_get_func adpt_port_bm_ctrl_get;
	adpt_bm_bufgroup_buffer_set_func adpt_bm_bufgroup_buffer_set;
	adpt_port_bufgroup_map_set_func adpt_port_bufgroup_map_set;
	adpt_bm_port_static_thresh_get_func adpt_bm_port_static_thresh_get;
	adpt_bm_port_reserved_buffer_set_func adpt_bm_port_reserved_buffer_set;
	adpt_bm_port_static_thresh_set_func adpt_bm_port_static_thresh_set;
	adpt_bm_port_dynamic_thresh_set_func adpt_bm_port_dynamic_thresh_set;
	adpt_port_bm_ctrl_set_func adpt_port_bm_ctrl_set;
	adpt_port_tdm_ctrl_set_func adpt_port_tdm_ctrl_set;
	adpt_port_tdm_tick_cfg_set_func adpt_port_tdm_tick_cfg_set;
	adpt_bm_port_counter_get_func adpt_bm_port_counter_get;

	//shaper
	a_uint32_t adpt_shaper_func_bitmap;
	adpt_flow_shaper_set_func adpt_flow_shaper_set;
	adpt_queue_shaper_get_func adpt_queue_shaper_get;
	adpt_queue_shaper_token_number_set_func adpt_queue_shaper_token_number_set;
	adpt_port_shaper_get_func adpt_port_shaper_get;
	adpt_flow_shaper_time_slot_get_func adpt_flow_shaper_time_slot_get;
	adpt_port_shaper_time_slot_get_func adpt_port_shaper_time_slot_get;
	adpt_flow_shaper_time_slot_set_func adpt_flow_shaper_time_slot_set;
	adpt_shaper_ipg_preamble_length_set_func adpt_shaper_ipg_preamble_length_set;
	adpt_port_shaper_token_number_set_func adpt_port_shaper_token_number_set;
	adpt_queue_shaper_token_number_get_func adpt_queue_shaper_token_number_get;
	adpt_queue_shaper_time_slot_get_func adpt_queue_shaper_time_slot_get;
	adpt_port_shaper_token_number_get_func adpt_port_shaper_token_number_get;
	adpt_flow_shaper_token_number_set_func adpt_flow_shaper_token_number_set;
	adpt_flow_shaper_token_number_get_func adpt_flow_shaper_token_number_get;
	adpt_shaper_ipg_preamble_length_get_func adpt_shaper_ipg_preamble_length_get;
	adpt_port_shaper_set_func adpt_port_shaper_set;
	adpt_port_shaper_time_slot_set_func adpt_port_shaper_time_slot_set;
	adpt_flow_shaper_get_func adpt_flow_shaper_get;
	adpt_queue_shaper_set_func adpt_queue_shaper_set;
	adpt_queue_shaper_time_slot_set_func adpt_queue_shaper_time_slot_set;

//policer
	a_uint32_t adpt_policer_func_bitmap;
	adpt_acl_policer_counter_get_func adpt_acl_policer_counter_get;
	adpt_port_policer_counter_get_func adpt_port_policer_counter_get;
	adpt_port_compensation_byte_get_func adpt_port_compensation_byte_get;
	adpt_port_policer_entry_get_func adpt_port_policer_entry_get;
	adpt_port_policer_entry_set_func adpt_port_policer_entry_set;
	adpt_acl_policer_entry_get_func adpt_acl_policer_entry_get;
	adpt_acl_policer_entry_set_func adpt_acl_policer_entry_set;
	adpt_policer_time_slot_get_func adpt_policer_time_slot_get;
	adpt_port_compensation_byte_set_func adpt_port_compensation_byte_set;
	adpt_policer_time_slot_set_func adpt_policer_time_slot_set;
	adpt_policer_global_counter_get_func adpt_policer_global_counter_get;
	adpt_policer_bypass_en_set_func adpt_policer_bypass_en_set;
	adpt_policer_bypass_en_get_func adpt_policer_bypass_en_get;

	/* misc */
	adpt_debug_port_counter_enable_func adpt_debug_port_counter_enable;
	adpt_debug_port_counter_status_get_func adpt_debug_port_counter_status_get;
	adpt_debug_counter_set_func adpt_debug_counter_set;
	adpt_debug_counter_get_func adpt_debug_counter_get;
	adpt_intr_port_link_mask_set_func adpt_intr_port_link_mask_set;
	adpt_intr_port_link_mask_get_func adpt_intr_port_link_mask_get;
	adpt_intr_port_link_status_get_func adpt_intr_port_link_status_get;

	/* uniphy */
	adpt_uniphy_mode_set_func adpt_uniphy_mode_set;

	/* ptp */
	adpt_ptp_config_set_func adpt_ptp_config_set;
	adpt_ptp_config_get_func adpt_ptp_config_get;
	adpt_ptp_reference_clock_set_func adpt_ptp_reference_clock_set;
	adpt_ptp_reference_clock_get_func adpt_ptp_reference_clock_get;
	adpt_ptp_rx_timestamp_mode_set_func adpt_ptp_rx_timestamp_mode_set;
	adpt_ptp_rx_timestamp_mode_get_func adpt_ptp_rx_timestamp_mode_get;
	adpt_ptp_timestamp_get_func adpt_ptp_timestamp_get;
	adpt_ptp_pkt_timestamp_set_func adpt_ptp_pkt_timestamp_set;
	adpt_ptp_pkt_timestamp_get_func adpt_ptp_pkt_timestamp_get;
	adpt_ptp_grandmaster_mode_set_func adpt_ptp_grandmaster_mode_set;
	adpt_ptp_grandmaster_mode_get_func adpt_ptp_grandmaster_mode_get;
	adpt_ptp_rtc_time_get_func adpt_ptp_rtc_time_get;
	adpt_ptp_rtc_time_set_func adpt_ptp_rtc_time_set;
	adpt_ptp_rtc_time_clear_func adpt_ptp_rtc_time_clear;
	adpt_ptp_rtc_adjtime_set_func adpt_ptp_rtc_adjtime_set;
	adpt_ptp_rtc_adjfreq_set_func adpt_ptp_rtc_adjfreq_set;
	adpt_ptp_rtc_adjfreq_get_func adpt_ptp_rtc_adjfreq_get;
	adpt_ptp_link_delay_set_func adpt_ptp_link_delay_set;
	adpt_ptp_link_delay_get_func adpt_ptp_link_delay_get;
	adpt_ptp_security_set_func adpt_ptp_security_set;
	adpt_ptp_security_get_func adpt_ptp_security_get;
	adpt_ptp_pps_signal_control_set_func adpt_ptp_pps_signal_control_set;
	adpt_ptp_pps_signal_control_get_func adpt_ptp_pps_signal_control_get;
	adpt_ptp_rx_crc_recalc_enable_func adpt_ptp_rx_crc_recalc_enable;
	adpt_ptp_rx_crc_recalc_status_get_func adpt_ptp_rx_crc_recalc_status_get;
	adpt_ptp_asym_correction_set_func adpt_ptp_asym_correction_set;
	adpt_ptp_asym_correction_get_func adpt_ptp_asym_correction_get;
	adpt_ptp_output_waveform_set_func adpt_ptp_output_waveform_set;
	adpt_ptp_output_waveform_get_func adpt_ptp_output_waveform_get;
	adpt_ptp_rtc_time_snapshot_enable_func adpt_ptp_rtc_time_snapshot_enable;
	adpt_ptp_rtc_time_snapshot_status_get_func adpt_ptp_rtc_time_snapshot_status_get;
	adpt_ptp_increment_sync_from_clock_enable_func adpt_ptp_increment_sync_from_clock_enable;
	adpt_ptp_increment_sync_from_clock_status_get_func \
		adpt_ptp_increment_sync_from_clock_status_get;
	adpt_ptp_tod_uart_set_func adpt_ptp_tod_uart_set;
	adpt_ptp_tod_uart_get_func adpt_ptp_tod_uart_get;
	adpt_ptp_enhanced_timestamp_engine_set_func adpt_ptp_enhanced_timestamp_engine_set;
	adpt_ptp_enhanced_timestamp_engine_get_func adpt_ptp_enhanced_timestamp_engine_get;
	adpt_ptp_trigger_set_func adpt_ptp_trigger_set;
	adpt_ptp_trigger_get_func adpt_ptp_trigger_get;
	adpt_ptp_capture_set_func adpt_ptp_capture_set;
	adpt_ptp_capture_get_func adpt_ptp_capture_get;
	adpt_ptp_interrupt_set_func adpt_ptp_interrupt_set;
	adpt_ptp_interrupt_get_func adpt_ptp_interrupt_get;

	/* sfp */
	adpt_sfp_eeprom_data_get_func adpt_sfp_eeprom_data_get;
	adpt_sfp_eeprom_data_set_func adpt_sfp_eeprom_data_set;
	adpt_sfp_diag_ctrl_status_get_func adpt_sfp_diag_ctrl_status_get;
	adpt_sfp_diag_extenal_calibration_const_get_func
		adpt_sfp_diag_extenal_calibration_const_get;
	adpt_sfp_link_length_get_func adpt_sfp_link_length_get;
	adpt_sfp_diag_internal_threshold_get_func adpt_sfp_diag_internal_threshold_get;
	adpt_sfp_diag_realtime_get_func adpt_sfp_diag_realtime_get;
	adpt_sfp_laser_wavelength_get_func adpt_sfp_laser_wavelength_get;
	adpt_sfp_option_get_func adpt_sfp_option_get;
	adpt_sfp_checkcode_get_func adpt_sfp_checkcode_get;
	adpt_sfp_diag_alarm_warning_flag_get_func adpt_sfp_diag_alarm_warning_flag_get;
	adpt_sfp_device_type_get_func adpt_sfp_device_type_get;
	adpt_sfp_vendor_info_get_func adpt_sfp_vendor_info_get;
	adpt_sfp_transceiver_code_get_func adpt_sfp_transceiver_code_get;
	adpt_sfp_ctrl_rate_get_func adpt_sfp_ctrl_rate_get;
	adpt_sfp_enhanced_cfg_get_func adpt_sfp_enhanced_cfg_get;
	adpt_sfp_rate_encode_get_func adpt_sfp_rate_encode_get;
	/*led*/
	adpt_led_ctrl_pattern_set_func adpt_led_ctrl_pattern_set;
	adpt_led_ctrl_pattern_get_func adpt_led_ctrl_pattern_get;
	adpt_led_ctrl_source_set_func adpt_led_ctrl_source_set;
}adpt_api_t;


adpt_api_t *adpt_api_ptr_get(a_uint32_t dev_id);
sw_error_t adpt_init(a_uint32_t dev_id, ssdk_init_cfg *cfg);
sw_error_t adpt_module_func_ctrl_set(a_uint32_t dev_id,
		a_uint32_t module, fal_func_ctrl_t *func_ctrl);
sw_error_t adpt_module_func_ctrl_get(a_uint32_t dev_id,
		a_uint32_t module, fal_func_ctrl_t *func_ctrl);
sw_error_t adpt_module_func_init(a_uint32_t dev_id, ssdk_init_cfg *cfg);
#ifdef SCOMPHY
a_uint32_t adapt_scomphy_revision_get(a_uint32_t dev_id);
#endif
#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif
