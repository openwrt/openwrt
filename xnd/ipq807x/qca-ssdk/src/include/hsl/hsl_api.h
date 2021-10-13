/*
 * Copyright (c) 2012, 2015, 2017-2018, The Linux Foundation. All rights reserved.
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


/*qca808x_start*/
#ifndef _HSL_API_H
#define _HSL_API_H

#ifdef __cplusplus
extern "C"
{
#endif				/* __cplusplus */

#include "fal.h"
/*qca808x_end*/

  /* Misc */
#define MISC_FUNC_PROTOTYPE_DEF
  typedef sw_error_t
    (*hsl_arp_status_set) (a_uint32_t dev_id, a_bool_t enable);

  typedef sw_error_t
    (*hsl_arp_status_get) (a_uint32_t dev_id, a_bool_t * enable);

  typedef sw_error_t
    (*hsl_frame_max_size_set) (a_uint32_t dev_id, a_uint32_t size);

  typedef sw_error_t
    (*hsl_frame_max_size_get) (a_uint32_t dev_id, a_uint32_t * size);

  typedef sw_error_t
    (*hsl_port_unk_sa_cmd_set) (a_uint32_t dev_id, fal_port_t port_id,
				fal_fwd_cmd_t cmd);

  typedef sw_error_t
    (*hsl_port_unk_sa_cmd_get) (a_uint32_t dev_id, fal_port_t port_id,
				fal_fwd_cmd_t * cmd);

  typedef sw_error_t
    (*hsl_port_unk_uc_filter_set) (a_uint32_t dev_id, fal_port_t port_id,
				   a_bool_t enable);

  typedef sw_error_t
    (*hsl_port_unk_uc_filter_get) (a_uint32_t dev_id, fal_port_t port_id,
				   a_bool_t * enable);

  typedef sw_error_t
    (*hsl_port_unk_mc_filter_set) (a_uint32_t dev_id, fal_port_t port_id,
				   a_bool_t enable);

  typedef sw_error_t
    (*hsl_port_unk_mc_filter_get) (a_uint32_t dev_id, fal_port_t port_id,
				   a_bool_t * enable);

  typedef sw_error_t
    (*hsl_port_bc_filter_set) (a_uint32_t dev_id, fal_port_t port_id,
			       a_bool_t enable);

  typedef sw_error_t
    (*hsl_port_bc_filter_get) (a_uint32_t dev_id, fal_port_t port_id,
			       a_bool_t * enable);

  typedef sw_error_t
    (*hsl_cpu_port_status_set) (a_uint32_t dev_id, a_bool_t enable);

  typedef sw_error_t
    (*hsl_cpu_port_status_get) (a_uint32_t dev_id, a_bool_t * enable);

  typedef sw_error_t
    (*hsl_bc_to_cpu_port_set) (a_uint32_t dev_id, a_bool_t enable);

  typedef sw_error_t
    (*hsl_bc_to_cpu_port_get) (a_uint32_t dev_id, a_bool_t * enable);

  typedef sw_error_t
    (*hsl_port_dhcp_set) (a_uint32_t dev_id, fal_port_t port_id,
			  a_bool_t enable);

  typedef sw_error_t
    (*hsl_pppoe_cmd_set) (a_uint32_t dev_id, fal_fwd_cmd_t cmd);

  typedef sw_error_t
    (*hsl_pppoe_cmd_get) (a_uint32_t dev_id, fal_fwd_cmd_t * cmd);

  typedef sw_error_t
    (*hsl_pppoe_status_set) (a_uint32_t dev_id, a_bool_t enable);

  typedef sw_error_t
    (*hsl_pppoe_status_get) (a_uint32_t dev_id, a_bool_t * enable);

  typedef sw_error_t
    (*hsl_port_dhcp_get) (a_uint32_t dev_id, fal_port_t port_id,
			  a_bool_t * enable);

  typedef sw_error_t
    (*hsl_arp_cmd_set) (a_uint32_t dev_id, fal_fwd_cmd_t cmd);

  typedef sw_error_t
    (*hsl_arp_cmd_get) (a_uint32_t dev_id, fal_fwd_cmd_t * cmd);

  typedef sw_error_t
    (*hsl_eapol_cmd_set) (a_uint32_t dev_id, fal_fwd_cmd_t cmd);

  typedef sw_error_t
    (*hsl_eapol_cmd_get) (a_uint32_t dev_id, fal_fwd_cmd_t * cmd);

  typedef sw_error_t
    (*hsl_pppoe_session_add) (a_uint32_t dev_id, a_uint32_t session_id,
			      a_bool_t strip_hdr);

  typedef sw_error_t
    (*hsl_pppoe_session_del) (a_uint32_t dev_id, a_uint32_t session_id);

  typedef sw_error_t
    (*hsl_pppoe_session_get) (a_uint32_t dev_id, a_uint32_t session_id,
			      a_bool_t * strip_hdr);

  typedef sw_error_t
    (*hsl_eapol_status_set) (a_uint32_t dev_id, a_uint32_t port_id,
			     a_bool_t enable);

  typedef sw_error_t
    (*hsl_eapol_status_get) (a_uint32_t dev_id, a_uint32_t port_id,
			     a_bool_t * enable);

  typedef sw_error_t
    (*hsl_ripv1_status_set) (a_uint32_t dev_id, a_bool_t enable);

  typedef sw_error_t
    (*hsl_ripv1_status_get) (a_uint32_t dev_id, a_bool_t * enable);

  typedef sw_error_t
    (*hsl_port_arp_req_status_set) (a_uint32_t dev_id, fal_port_t port_id,
				    a_bool_t enable);

  typedef sw_error_t
    (*hsl_port_arp_req_status_get) (a_uint32_t dev_id, fal_port_t port_id,
				    a_bool_t * enable);

  typedef sw_error_t
    (*hsl_port_arp_ack_status_set) (a_uint32_t dev_id, fal_port_t port_id,
				    a_bool_t enable);

  typedef sw_error_t
    (*hsl_port_arp_ack_status_get) (a_uint32_t dev_id, fal_port_t port_id,
				    a_bool_t * enable);

  typedef sw_error_t
    (*hsl_pppoe_session_table_add) (a_uint32_t dev_id,
				    fal_pppoe_session_t * session_tbl);

  typedef sw_error_t
    (*hsl_pppoe_session_table_del) (a_uint32_t dev_id,
				    fal_pppoe_session_t * session_tbl);

  typedef sw_error_t
    (*hsl_pppoe_session_table_get) (a_uint32_t dev_id,
				    fal_pppoe_session_t * session_tbl);

  typedef sw_error_t
    (*hsl_pppoe_session_id_set) (a_uint32_t dev_id, a_uint32_t index,
				 a_uint32_t id);

  typedef sw_error_t
    (*hsl_pppoe_session_id_get) (a_uint32_t dev_id, a_uint32_t index,
				 a_uint32_t * id);

  typedef sw_error_t
    (*hsl_intr_mask_set) (a_uint32_t dev_id, a_uint32_t intr_mask);

  typedef sw_error_t
    (*hsl_intr_mask_get) (a_uint32_t dev_id, a_uint32_t * intr_mask);

  typedef sw_error_t
    (*hsl_intr_status_get) (a_uint32_t dev_id, a_uint32_t * intr_status);

  typedef sw_error_t
    (*hsl_intr_status_clear) (a_uint32_t dev_id, a_uint32_t intr_status);

  typedef sw_error_t
    (*hsl_intr_port_link_mask_set) (a_uint32_t dev_id, fal_port_t port_id,
				    a_uint32_t intr_mask);

  typedef sw_error_t
    (*hsl_intr_port_link_mask_get) (a_uint32_t dev_id, fal_port_t port_id,
				    a_uint32_t * intr_mask);

  typedef sw_error_t
    (*hsl_intr_port_link_status_get) (a_uint32_t dev_id, fal_port_t port_id,
				      a_uint32_t * intr_mask);

  typedef sw_error_t
    (*hsl_intr_mask_mac_linkchg_set) (a_uint32_t dev_id, fal_port_t port_id,
				      a_bool_t enable);

  typedef sw_error_t
    (*hsl_intr_mask_mac_linkchg_get) (a_uint32_t dev_id, fal_port_t port_id,
				      a_bool_t * enable);

  typedef sw_error_t
    (*hsl_intr_status_mac_linkchg_get) (a_uint32_t dev_id,
					fal_pbmp_t * port_bitmap);

  typedef sw_error_t (*hsl_intr_status_mac_linkchg_clear) (a_uint32_t dev_id);

  typedef sw_error_t
    (*hsl_cpu_vid_en_set) (a_uint32_t dev_id, a_bool_t enable);

  typedef sw_error_t
    (*hsl_cpu_vid_en_get) (a_uint32_t dev_id, a_bool_t * enable);

  typedef sw_error_t
    (*hsl_rtd_pppoe_en_set) (a_uint32_t dev_id, a_bool_t enable);

  typedef sw_error_t
    (*hsl_rtd_pppoe_en_get) (a_uint32_t dev_id, a_bool_t * enable);

  typedef sw_error_t
    (*hsl_global_macaddr_set) (a_uint32_t dev_id, fal_mac_addr_t * addr);

  typedef sw_error_t
    (*hsl_global_macaddr_get) (a_uint32_t dev_id, fal_mac_addr_t * addr);

  typedef sw_error_t
    (*hsl_lldp_status_set) (a_uint32_t dev_id, a_bool_t enable);

  typedef sw_error_t
    (*hsl_lldp_status_get) (a_uint32_t dev_id, a_bool_t * enable);

  typedef sw_error_t
    (*hsl_frame_crc_reserve_set) (a_uint32_t dev_id, a_bool_t enable);

  typedef sw_error_t
    (*hsl_frame_crc_reserve_get) (a_uint32_t dev_id, a_bool_t * enable);


  typedef sw_error_t
    (*hsl_register_dump) (a_uint32_t dev_id,a_uint32_t register_idx, fal_reg_dump_t * reg_dump);

  typedef sw_error_t
    (*hsl_debug_register_dump) (a_uint32_t dev_id, fal_debug_reg_dump_t * reg_dump);
 /*qca808x_start*/

  /* Port Control */
#define PORT_CONTROL_FUNC_PROTOTYPE_DEF
  typedef sw_error_t
    (*hsl_port_duplex_get) (a_uint32_t dev_id, fal_port_t port_id,
			    fal_port_duplex_t * pduplex);

  typedef sw_error_t
    (*hsl_port_duplex_set) (a_uint32_t dev_id, fal_port_t port_id,
			    fal_port_duplex_t duplex);

  typedef sw_error_t
    (*hsl_port_speed_get) (a_uint32_t dev_id, fal_port_t port_id,
			   fal_port_speed_t * pspeed);

  typedef sw_error_t
    (*hsl_port_autoneg_status_get) (a_uint32_t dev_id, fal_port_t port_id,
				    a_bool_t * status);

  typedef sw_error_t
    (*hsl_port_speed_set) (a_uint32_t dev_id, fal_port_t port_id,
			   fal_port_speed_t speed);

  typedef sw_error_t
    (*hsl_port_autoneg_enable) (a_uint32_t dev_id, fal_port_t port_id);

  typedef sw_error_t
    (*hsl_port_autoneg_restart) (a_uint32_t dev_id, fal_port_t port_id);

  typedef sw_error_t
    (*hsl_port_autoneg_adv_get) (a_uint32_t dev_id, fal_port_t port_id,
				 a_uint32_t * autoadv);

  typedef sw_error_t
    (*hsl_port_autoneg_adv_set) (a_uint32_t dev_id, fal_port_t port_id,
				 a_uint32_t autoadv);

  typedef sw_error_t
    (*hsl_port_hdr_status_set) (a_uint32_t dev_id, fal_port_t port_id,
				a_bool_t enable);

  typedef sw_error_t
    (*hsl_port_hdr_status_get) (a_uint32_t dev_id, fal_port_t port_id,
				a_bool_t * enable);

  typedef sw_error_t
    (*hsl_port_flowctrl_set) (a_uint32_t dev_id, fal_port_t port_id,
			      a_bool_t enable);

  typedef sw_error_t
    (*hsl_port_flowctrl_get) (a_uint32_t dev_id, fal_port_t port_id,
			      a_bool_t * enable);
typedef sw_error_t
    (*hsl_port_flowctrl_thresh_set) (a_uint32_t dev_id, fal_port_t port_id,
			      a_uint8_t  on, a_uint8_t  off);

  typedef sw_error_t
    (*hsl_port_flowctrl_forcemode_set) (a_uint32_t dev_id, fal_port_t port_id,
					a_bool_t enable);

  typedef sw_error_t
    (*hsl_port_flowctrl_forcemode_get) (a_uint32_t dev_id, fal_port_t port_id,
					a_bool_t * enable);

  typedef sw_error_t
    (*hsl_port_powersave_set) (a_uint32_t dev_id, fal_port_t port_id,
			       a_bool_t enable);


  typedef sw_error_t
    (*hsl_port_powersave_get) (a_uint32_t dev_id, fal_port_t port_id,
			       a_bool_t * enable);


  typedef sw_error_t
    (*hsl_port_hibernate_set) (a_uint32_t dev_id, fal_port_t port_id,
			       a_bool_t enable);


  typedef sw_error_t
    (*hsl_port_hibernate_get) (a_uint32_t dev_id, fal_port_t port_id,
			       a_bool_t * enable);


  typedef sw_error_t
    (*hsl_port_cdt) (a_uint32_t dev_id, fal_port_t port_id,
		     a_uint32_t mdi_pair, fal_cable_status_t * cable_status,
		     a_uint32_t * cable_len);

  typedef sw_error_t
    (*hsl_port_rxhdr_mode_set) (a_uint32_t dev_id, fal_port_t port_id,
				fal_port_header_mode_t mode);

  typedef sw_error_t
    (*hsl_port_rxhdr_mode_get) (a_uint32_t dev_id, fal_port_t port_id,
				fal_port_header_mode_t * mode);

  typedef sw_error_t
    (*hsl_port_txhdr_mode_set) (a_uint32_t dev_id, fal_port_t port_id,
				fal_port_header_mode_t mode);

  typedef sw_error_t
    (*hsl_port_txhdr_mode_get) (a_uint32_t dev_id, fal_port_t port_id,
				fal_port_header_mode_t * mode);

  typedef sw_error_t
    (*hsl_header_type_set) (a_uint32_t dev_id, a_bool_t enable,
			    a_uint32_t type);

  typedef sw_error_t
    (*hsl_header_type_get) (a_uint32_t dev_id, a_bool_t * enable,
			    a_uint32_t * type);

  typedef sw_error_t
    (*hsl_port_txmac_status_set) (a_uint32_t dev_id, fal_port_t port_id,
				  a_bool_t enable);

  typedef sw_error_t
    (*hsl_port_txmac_status_get) (a_uint32_t dev_id, fal_port_t port_id,
				  a_bool_t * enable);

  typedef sw_error_t
    (*hsl_port_rxmac_status_set) (a_uint32_t dev_id, fal_port_t port_id,
				  a_bool_t enable);

  typedef sw_error_t
    (*hsl_port_rxmac_status_get) (a_uint32_t dev_id, fal_port_t port_id,
				  a_bool_t * enable);

  typedef sw_error_t
    (*hsl_port_txfc_status_set) (a_uint32_t dev_id, fal_port_t port_id,
				 a_bool_t enable);

  typedef sw_error_t
    (*hsl_port_txfc_status_get) (a_uint32_t dev_id, fal_port_t port_id,
				 a_bool_t * enable);

  typedef sw_error_t
    (*hsl_port_rxfc_status_set) (a_uint32_t dev_id, fal_port_t port_id,
				 a_bool_t enable);

  typedef sw_error_t
    (*hsl_port_rxfc_status_get) (a_uint32_t dev_id, fal_port_t port_id,
				 a_bool_t * enable);

  typedef sw_error_t
    (*hsl_port_bp_status_set) (a_uint32_t dev_id, fal_port_t port_id,
			       a_bool_t enable);

  typedef sw_error_t
    (*hsl_port_bp_status_get) (a_uint32_t dev_id, fal_port_t port_id,
			       a_bool_t * enable);

  typedef sw_error_t
    (*hsl_port_link_forcemode_set) (a_uint32_t dev_id, fal_port_t port_id,
				    a_bool_t enable);

  typedef sw_error_t
    (*hsl_port_link_forcemode_get) (a_uint32_t dev_id, fal_port_t port_id,
				    a_bool_t * enable);

  typedef sw_error_t
    (*hsl_port_link_status_get) (a_uint32_t dev_id, fal_port_t port_id,
				 a_bool_t * status);

  typedef sw_error_t
    (*hsl_ports_link_status_get) (a_uint32_t dev_id, a_uint32_t * status);

  typedef sw_error_t
    (*hsl_port_mac_loopback_set) (a_uint32_t dev_id, fal_port_t port_id,
				  a_bool_t enable);

  typedef sw_error_t
    (*hsl_port_mac_loopback_get) (a_uint32_t dev_id, fal_port_t port_id,
				  a_bool_t * enable);

  typedef sw_error_t
    (*hsl_port_congestion_drop_set) (a_uint32_t dev_id, fal_port_t port_id,
				     a_uint32_t queue_id, a_bool_t enable);

  typedef sw_error_t
    (*hsl_port_congestion_drop_get) (a_uint32_t dev_id, fal_port_t port_id,
				     a_uint32_t queue_id, a_bool_t * enable);

  typedef sw_error_t
    (*hsl_ring_flow_ctrl_thres_set) (a_uint32_t dev_id, a_uint32_t ring_id,
				     a_uint8_t on_thres, a_uint8_t off_thres);

  typedef sw_error_t
    (*hsl_ring_flow_ctrl_thres_get) (a_uint32_t dev_id, a_uint32_t ring_id,
				     a_uint8_t * on_thres,
				     a_uint8_t * off_thres);

  typedef sw_error_t
    (*hsl_port_8023az_set) (a_uint32_t dev_id, fal_port_t port_id,
			    a_bool_t enable);


  typedef sw_error_t
    (*hsl_port_8023az_get) (a_uint32_t dev_id, fal_port_t port_id,
			    a_bool_t * enable);

  typedef sw_error_t
    (*hsl_port_mdix_set) (a_uint32_t dev_id, fal_port_t port_id,
			  fal_port_mdix_mode_t mode);

  typedef sw_error_t
    (*hsl_port_mdix_get) (a_uint32_t dev_id, fal_port_t port_id,
			  fal_port_mdix_mode_t * mode);

  typedef sw_error_t
    (*hsl_port_mdix_status_get) (a_uint32_t dev_id, fal_port_t port_id,
				 fal_port_mdix_status_t * mode);

  typedef sw_error_t
    (*hsl_port_combo_prefer_medium_set) (a_uint32_t dev_id,
					 fal_port_t port_id,
					 fal_port_medium_t medium);

  typedef sw_error_t
    (*hsl_port_combo_prefer_medium_get) (a_uint32_t dev_id,
					 fal_port_t port_id,
					 fal_port_medium_t * medium);

  typedef sw_error_t
    (*hsl_port_combo_medium_status_get) (a_uint32_t dev_id,
					 fal_port_t port_id,
					 fal_port_medium_t * medium);

  typedef sw_error_t
    (*hsl_port_combo_fiber_mode_set) (a_uint32_t dev_id, fal_port_t port_id,
				      fal_port_fiber_mode_t mode);

  typedef sw_error_t
    (*hsl_port_combo_fiber_mode_get) (a_uint32_t dev_id, fal_port_t port_id,
				      fal_port_fiber_mode_t * mode);

  typedef sw_error_t
    (*hsl_port_local_loopback_set) (a_uint32_t dev_id, fal_port_t port_id,
				    a_bool_t enable);

  typedef sw_error_t
    (*hsl_port_local_loopback_get) (a_uint32_t dev_id, fal_port_t port_id,
				    a_bool_t * enable);

  typedef sw_error_t
    (*hsl_port_remote_loopback_set) (a_uint32_t dev_id, fal_port_t port_id,
				     a_bool_t enable);

  typedef sw_error_t
    (*hsl_port_remote_loopback_get) (a_uint32_t dev_id, fal_port_t port_id,
				     a_bool_t * enable);

  typedef sw_error_t
    (*hsl_port_reset) (a_uint32_t dev_id, fal_port_t port_id);

  typedef sw_error_t
    (*hsl_port_power_off) (a_uint32_t dev_id, fal_port_t port_id);

  typedef sw_error_t
    (*hsl_port_power_on) (a_uint32_t dev_id, fal_port_t port_id);

  typedef sw_error_t
    (*hsl_port_phy_id_get) (a_uint32_t dev_id, fal_port_t port_id,a_uint16_t * org_id, a_uint16_t * rev_id);

  typedef sw_error_t
    (*hsl_port_wol_status_set) (a_uint32_t dev_id, fal_port_t port_id,a_bool_t enable);

  typedef sw_error_t
    (*hsl_port_wol_status_get) (a_uint32_t dev_id, fal_port_t port_id,a_bool_t *enable);

  typedef sw_error_t
    (*hsl_port_magic_frame_mac_set) (a_uint32_t dev_id, fal_port_t port_id,fal_mac_addr_t * mac);

  typedef sw_error_t
    (*hsl_port_magic_frame_mac_get) (a_uint32_t dev_id, fal_port_t port_id,fal_mac_addr_t * mac);

  typedef sw_error_t
    (*hsl_port_interface_mode_set) (a_uint32_t dev_id, fal_port_t port_id,
			      fal_port_interface_mode_t mode);

  typedef sw_error_t
    (*hsl_port_interface_mode_get) (a_uint32_t dev_id, fal_port_t port_id,
			      fal_port_interface_mode_t * mode);
    typedef sw_error_t
    (*hsl_port_interface_mode_apply) (a_uint32_t dev_id);

  typedef sw_error_t
    (*hsl_port_interface_mode_status_get) (a_uint32_t dev_id, fal_port_t port_id,
			      fal_port_interface_mode_t * mode);
   typedef sw_error_t
    (*hsl_port_counter_set) (a_uint32_t dev_id, fal_port_t port_id,
			    a_bool_t enable);
  typedef sw_error_t
    (*hsl_port_counter_get) (a_uint32_t dev_id, fal_port_t port_id,
			    a_bool_t * enable);
    typedef sw_error_t
    (*hsl_port_counter_show) (a_uint32_t dev_id, fal_port_t port_id,
			      fal_port_counter_info_t * counter_info);
/*qca808x_end*/

  /* VLAN */
#define VLAN_FUNC_PROTOTYPE_DEF
  typedef sw_error_t
    (*hsl_vlan_entry_append) (a_uint32_t dev_id,
			      const fal_vlan_t * vlan_entry);

  typedef sw_error_t
    (*hsl_vlan_create) (a_uint32_t dev_id, a_uint32_t vlan_id);

  typedef sw_error_t
    (*hsl_vlan_next) (a_uint32_t dev_id, a_uint32_t vlan_id,
		      fal_vlan_t * p_vlan);

  typedef sw_error_t
    (*hsl_vlan_find) (a_uint32_t dev_id, a_uint32_t vlan_id,
		      fal_vlan_t * p_vlan);

  typedef sw_error_t
    (*hsl_vlan_member_update) (a_uint32_t dev_id, a_uint32_t vlan_id,
			       fal_pbmp_t member, fal_pbmp_t u_member);

  typedef sw_error_t
    (*hsl_vlan_delete) (a_uint32_t dev_id, a_uint32_t vlan_id);

  typedef sw_error_t (*hsl_vlan_flush) (a_uint32_t dev_id);

  typedef sw_error_t
    (*hsl_vlan_fid_set) (a_uint32_t dev_id, a_uint32_t vlan_id,
			 a_uint32_t fid);

  typedef sw_error_t
    (*hsl_vlan_fid_get) (a_uint32_t dev_id, a_uint32_t vlan_id,
			 a_uint32_t * fid);

  typedef sw_error_t
    (*hsl_vlan_member_add) (a_uint32_t dev_id, a_uint32_t vlan_id,
			    fal_port_t port_id, fal_pt_1q_egmode_t port_info);

  typedef sw_error_t
    (*hsl_vlan_member_del) (a_uint32_t dev_id, a_uint32_t vlan_id,
			    fal_port_t port_id);

  typedef sw_error_t
    (*hsl_vlan_learning_state_set) (a_uint32_t dev_id, a_uint32_t vlan_id,
				    a_bool_t enable);

  typedef sw_error_t
    (*hsl_vlan_learning_state_get) (a_uint32_t dev_id, a_uint32_t vlan_id,
				    a_bool_t * enable);

  /* Port Vlan */
#define PORT_VLAN_FUNC_PROTOTYPE_DEF
  typedef sw_error_t
    (*hsl_port_1qmode_get) (a_uint32_t dev_id, fal_port_t port_id,
			    fal_pt_1qmode_t * pport_1qmode);

  typedef sw_error_t
    (*hsl_port_1qmode_set) (a_uint32_t dev_id, fal_port_t port_id,
			    fal_pt_1qmode_t port_1qmode);

  typedef sw_error_t
    (*hsl_port_egvlanmode_get) (a_uint32_t dev_id, fal_port_t port_id,
				fal_pt_1q_egmode_t * pport_egvlanmode);

  typedef sw_error_t
    (*hsl_port_egvlanmode_set) (a_uint32_t dev_id, fal_port_t port_id,
				fal_pt_1q_egmode_t port_egvlanmode);

  typedef sw_error_t
    (*hsl_portvlan_member_add) (a_uint32_t dev_id, fal_port_t port_id,
				a_uint32_t mem_port_id);

  typedef sw_error_t
    (*hsl_portvlan_member_del) (a_uint32_t dev_id, fal_port_t port_id,
				a_uint32_t mem_port_id);

  typedef sw_error_t
    (*hsl_portvlan_member_update) (a_uint32_t dev_id, fal_port_t port_id,
				   fal_pbmp_t mem_port_map);

  typedef sw_error_t
    (*hsl_portvlan_member_get) (a_uint32_t dev_id, fal_port_t port_id,
				fal_pbmp_t * mem_port_map);

  typedef sw_error_t
    (*hsl_port_nestvlan_set) (a_uint32_t dev_id, fal_port_t port_id,
			      a_bool_t enable);

  typedef sw_error_t
    (*hsl_port_nestvlan_get) (a_uint32_t dev_id, fal_port_t port_id,
			      a_bool_t * enable);

  typedef sw_error_t
    (*hsl_nestvlan_tpid_set) (a_uint32_t dev_id, a_uint32_t tpid);

  typedef sw_error_t
    (*hsl_nestvlan_tpid_get) (a_uint32_t dev_id, a_uint32_t * tpid);

  typedef sw_error_t
    (*hsl_port_default_vid_set) (a_uint32_t dev_id, fal_port_t port_id,
				 a_uint32_t vid);

  typedef sw_error_t
    (*hsl_port_default_vid_get) (a_uint32_t dev_id, fal_port_t port_id,
				 a_uint32_t * vid);

  typedef sw_error_t
    (*hsl_port_force_default_vid_set) (a_uint32_t dev_id,
				       fal_port_t port_id, a_bool_t enable);

  typedef sw_error_t
    (*hsl_port_force_default_vid_get) (a_uint32_t dev_id,
				       fal_port_t port_id, a_bool_t * enable);

  typedef sw_error_t
    (*hsl_port_force_portvlan_set) (a_uint32_t dev_id, fal_port_t port_id,
				    a_bool_t enable);

  typedef sw_error_t
    (*hsl_port_force_portvlan_get) (a_uint32_t dev_id, fal_port_t port_id,
				    a_bool_t * enable);

  typedef sw_error_t
    (*hsl_port_invlan_mode_set) (a_uint32_t dev_id, fal_port_t port_id,
				 fal_pt_invlan_mode_t mode);

  typedef sw_error_t
    (*hsl_port_invlan_mode_get) (a_uint32_t dev_id, fal_port_t port_id,
				 fal_pt_invlan_mode_t * mode);

  typedef sw_error_t
    (*hsl_port_tls_set) (a_uint32_t dev_id, fal_port_t port_id,
			 a_bool_t enable);

  typedef sw_error_t
    (*hsl_port_tls_get) (a_uint32_t dev_id, fal_port_t port_id,
			 a_bool_t * enable);

  typedef sw_error_t
    (*hsl_port_pri_propagation_set) (a_uint32_t dev_id, fal_port_t port_id,
				     a_bool_t enable);

  typedef sw_error_t
    (*hsl_port_pri_propagation_get) (a_uint32_t dev_id, fal_port_t port_id,
				     a_bool_t * enable);

  typedef sw_error_t
    (*hsl_port_default_svid_set) (a_uint32_t dev_id, fal_port_t port_id,
				  a_uint32_t vid);

  typedef sw_error_t
    (*hsl_port_default_svid_get) (a_uint32_t dev_id, fal_port_t port_id,
				  a_uint32_t * vid);

  typedef sw_error_t
    (*hsl_port_default_cvid_set) (a_uint32_t dev_id, fal_port_t port_id,
				  a_uint32_t vid);

  typedef sw_error_t
    (*hsl_port_default_cvid_get) (a_uint32_t dev_id, fal_port_t port_id,
				  a_uint32_t * vid);

  typedef sw_error_t
    (*hsl_port_vlan_propagation_set) (a_uint32_t dev_id, fal_port_t port_id,
				      fal_vlan_propagation_mode_t mode);

  typedef sw_error_t
    (*hsl_port_vlan_propagation_get) (a_uint32_t dev_id, fal_port_t port_id,
				      fal_vlan_propagation_mode_t * mode);

  typedef sw_error_t
    (*hsl_port_vlan_trans_add) (a_uint32_t dev_id, fal_port_t port_id,
				fal_vlan_trans_entry_t * entry);

  typedef sw_error_t
    (*hsl_port_vlan_trans_del) (a_uint32_t dev_id, fal_port_t port_id,
				fal_vlan_trans_entry_t * entry);

  typedef sw_error_t
    (*hsl_port_vlan_trans_get) (a_uint32_t dev_id, fal_port_t port_id,
				fal_vlan_trans_entry_t * entry);

  typedef sw_error_t
    (*hsl_qinq_mode_set) (a_uint32_t dev_id, fal_qinq_mode_t mode);

  typedef sw_error_t
    (*hsl_qinq_mode_get) (a_uint32_t dev_id, fal_qinq_mode_t * mode);

  typedef sw_error_t
    (*hsl_port_qinq_role_set) (a_uint32_t dev_id, fal_port_t port_id,
			       fal_qinq_port_role_t role);

  typedef sw_error_t
    (*hsl_port_qinq_role_get) (a_uint32_t dev_id, fal_port_t port_id,
			       fal_qinq_port_role_t * role);

  typedef sw_error_t
    (*hsl_port_vlan_trans_iterate) (a_uint32_t dev_id, fal_port_t port_id,
				    a_uint32_t * iterator,
				    fal_vlan_trans_entry_t * entry);

  typedef sw_error_t
    (*hsl_port_mac_vlan_xlt_set) (a_uint32_t dev_id, fal_port_t port_id,
				  a_bool_t enable);

  typedef sw_error_t
    (*hsl_port_mac_vlan_xlt_get) (a_uint32_t dev_id, fal_port_t port_id,
				  a_bool_t * enable);

  typedef sw_error_t
    (*hsl_netisolate_set) (a_uint32_t dev_id, a_uint32_t enable);

  typedef sw_error_t
    (*hsl_netisolate_get) (a_uint32_t dev_id, a_uint32_t * enable);

  typedef sw_error_t
    (*hsl_eg_trans_filter_bypass_en_set) (a_uint32_t dev_id,
					  a_uint32_t enable);

  typedef sw_error_t
    (*hsl_eg_trans_filter_bypass_en_get) (a_uint32_t dev_id,
					  a_uint32_t * enable);

  typedef sw_error_t
    (*hsl_port_vrf_id_set) (a_uint32_t dev_id, fal_port_t port_id,
			    a_uint32_t vrf_id);

  typedef sw_error_t
    (*hsl_port_vrf_id_get) (a_uint32_t dev_id, fal_port_t port_id,
			    a_uint32_t * vrf_id);

  /* FDB */
#define FDB_FUNC_PROTOTYPE_DEF
  typedef sw_error_t
    (*hsl_fdb_add) (a_uint32_t dev_id, const fal_fdb_entry_t * entry);

  typedef sw_error_t
    (*hsl_fdb_rfs_set) (a_uint32_t dev_id, const fal_fdb_rfs_t * entry);

  typedef sw_error_t
    (*hsl_fdb_rfs_del) (a_uint32_t dev_id, const fal_fdb_rfs_t * entry);

  typedef sw_error_t (*hsl_fdb_del_all) (a_uint32_t dev_id, a_uint32_t flag);

  typedef sw_error_t
    (*hsl_fdb_del_by_port) (a_uint32_t dev_id, a_uint32_t port_id,
			    a_uint32_t flag);

  typedef sw_error_t
    (*hsl_fdb_del_by_mac) (a_uint32_t dev_id, const fal_fdb_entry_t * addr);

  typedef sw_error_t
    (*hsl_fdb_first) (a_uint32_t dev_id, fal_fdb_entry_t * entry);

  typedef sw_error_t
    (*hsl_fdb_next) (a_uint32_t dev_id, fal_fdb_entry_t * entry);

  typedef sw_error_t
    (*hsl_fdb_find) (a_uint32_t dev_id, fal_fdb_entry_t * entry);

  typedef sw_error_t
    (*hsl_fdb_port_learn_set) (a_uint32_t dev_id, fal_port_t port_id,
			       a_bool_t enable);

  typedef sw_error_t
    (*hsl_fdb_port_learn_get) (a_uint32_t dev_id, fal_port_t port_id,
			       a_bool_t * enable);

  typedef sw_error_t
    (*hsl_fdb_age_ctrl_set) (a_uint32_t dev_id, a_bool_t enable);

  typedef sw_error_t
    (*hsl_fdb_age_ctrl_get) (a_uint32_t dev_id, a_bool_t * enable);

  typedef sw_error_t
    (*hsl_fdb_vlan_ivl_svl_set) (a_uint32_t dev_id, fal_fdb_smode smode);

  typedef sw_error_t
    (*hsl_fdb_vlan_ivl_svl_get) (a_uint32_t dev_id, fal_fdb_smode * smode);

  typedef sw_error_t
    (*hsl_fdb_age_time_set) (a_uint32_t dev_id, a_uint32_t * time);

  typedef sw_error_t
    (*hsl_fdb_age_time_get) (a_uint32_t dev_id, a_uint32_t * time);

  typedef sw_error_t
    (*hsl_fdb_iterate) (a_uint32_t dev_id, a_uint32_t * iterator,
			fal_fdb_entry_t * entry);

  typedef sw_error_t
    (*hsl_fdb_extend_next) (a_uint32_t dev_id, fal_fdb_op_t * op,
			    fal_fdb_entry_t * entry);

  typedef sw_error_t
    (*hsl_fdb_extend_first) (a_uint32_t dev_id, fal_fdb_op_t * option,
			     fal_fdb_entry_t * entry);

  typedef sw_error_t
    (*hsl_fdb_transfer) (a_uint32_t dev_id, fal_port_t old_port,
			 fal_port_t new_port, a_uint32_t fid,
			 fal_fdb_op_t * option);

  typedef sw_error_t
    (*hsl_port_fdb_learn_limit_set) (a_uint32_t dev_id, fal_port_t port_id,
				     a_bool_t enable, a_uint32_t cnt);

  typedef sw_error_t
    (*hsl_port_fdb_learn_limit_get) (a_uint32_t dev_id, fal_port_t port_id,
				     a_bool_t * enable, a_uint32_t * cnt);

  typedef sw_error_t
    (*hsl_port_fdb_learn_exceed_cmd_set) (a_uint32_t dev_id,
					  fal_port_t port_id,
					  fal_fwd_cmd_t cmd);

  typedef sw_error_t
    (*hsl_port_fdb_learn_exceed_cmd_get) (a_uint32_t dev_id,
					  fal_port_t port_id,
					  fal_fwd_cmd_t * cmd);

  typedef sw_error_t
    (*hsl_fdb_learn_limit_set) (a_uint32_t dev_id, a_bool_t enable,
				a_uint32_t cnt);

  typedef sw_error_t
    (*hsl_fdb_learn_limit_get) (a_uint32_t dev_id, a_bool_t * enable,
				a_uint32_t * cnt);

  typedef sw_error_t
    (*hsl_fdb_learn_exceed_cmd_set) (a_uint32_t dev_id, fal_fwd_cmd_t cmd);

  typedef sw_error_t
    (*hsl_fdb_learn_exceed_cmd_get) (a_uint32_t dev_id, fal_fwd_cmd_t * cmd);

  typedef sw_error_t
    (*hsl_fdb_resv_add) (a_uint32_t dev_id, fal_fdb_entry_t * entry);

  typedef sw_error_t
    (*hsl_fdb_resv_del) (a_uint32_t dev_id, fal_fdb_entry_t * entry);

  typedef sw_error_t
    (*hsl_fdb_resv_find) (a_uint32_t dev_id, fal_fdb_entry_t * entry);

  typedef sw_error_t
    (*hsl_fdb_resv_iterate) (a_uint32_t dev_id, a_uint32_t * iterator,
			     fal_fdb_entry_t * entry);

  typedef sw_error_t
    (*hsl_fdb_port_learn_static_set) (a_uint32_t dev_id, fal_port_t port_id,
				      a_bool_t enable);

  typedef sw_error_t
    (*hsl_fdb_port_learn_static_get) (a_uint32_t dev_id, fal_port_t port_id,
				      a_bool_t * enable);

  typedef sw_error_t
    (*hsl_fdb_port_add) (a_uint32_t dev_id, a_uint32_t fid,
			 fal_mac_addr_t * addr, fal_port_t port_id);

  typedef sw_error_t
    (*hsl_fdb_port_del) (a_uint32_t dev_id, a_uint32_t fid,
			 fal_mac_addr_t * addr, fal_port_t port_id);

  /* QOS */
#define QOS_FUNC_PROTOTYPE_DEF
  typedef sw_error_t
    (*hsl_cosmap_up_queue_set) (a_uint32_t dev_id, a_uint32_t up,
				fal_queue_t queue);

  typedef sw_error_t
    (*hsl_cosmap_up_queue_get) (a_uint32_t dev_id, a_uint32_t up,
				fal_queue_t * queue);

  typedef sw_error_t
    (*hsl_cosmap_dscp_queue_set) (a_uint32_t dev_id, a_uint32_t dscp,
				  fal_queue_t queue);

  typedef sw_error_t
    (*hsl_cosmap_dscp_queue_get) (a_uint32_t dev_id, a_uint32_t dscp,
				  fal_queue_t * queue);

  typedef sw_error_t
    (*hsl_qos_port_mode_set) (a_uint32_t dev_id, fal_port_t port_id,
			      fal_qos_mode_t mode, a_bool_t enable);

  typedef sw_error_t
    (*hsl_qos_port_mode_get) (a_uint32_t dev_id, fal_port_t port_id,
			      fal_qos_mode_t mode, a_bool_t * enable);

  typedef sw_error_t
    (*hsl_qos_port_mode_pri_set) (a_uint32_t dev_id, fal_port_t port_id,
				  fal_qos_mode_t mode, a_uint32_t pri);

  typedef sw_error_t
    (*hsl_qos_port_mode_pri_get) (a_uint32_t dev_id, fal_port_t port_id,
				  fal_qos_mode_t mode, a_uint32_t * pri);

  typedef sw_error_t
    (*hsl_qos_port_default_up_set) (a_uint32_t dev_id, fal_port_t port_id,
				    a_uint32_t up);

  typedef sw_error_t
    (*hsl_qos_port_default_up_get) (a_uint32_t dev_id, fal_port_t port_id,
				    a_uint32_t * up);

  typedef sw_error_t
    (*hsl_qos_sch_mode_set) (a_uint32_t dev_id,
			     fal_sch_mode_t mode, const a_uint32_t weight[]);

  typedef sw_error_t
    (*hsl_qos_sch_mode_get) (a_uint32_t dev_id,
			     fal_sch_mode_t * mode, a_uint32_t weight[]);

  typedef sw_error_t
    (*hsl_qos_queue_tx_buf_status_set) (a_uint32_t dev_id, fal_port_t port_id,
					a_bool_t enable);

  typedef sw_error_t
    (*hsl_qos_queue_tx_buf_status_get) (a_uint32_t dev_id, fal_port_t port_id,
					a_bool_t * enable);

  typedef sw_error_t
    (*hsl_qos_queue_tx_buf_nr_set) (a_uint32_t dev_id, fal_port_t port_id,
				    a_uint32_t queue_id, a_uint32_t * number);

  typedef sw_error_t
    (*hsl_qos_queue_tx_buf_nr_get) (a_uint32_t dev_id, fal_port_t port_id,
				    a_uint32_t queue_id, a_uint32_t * number);

  typedef sw_error_t
    (*hsl_qos_port_tx_buf_status_set) (a_uint32_t dev_id, fal_port_t port_id,
				       a_bool_t enable);

  typedef sw_error_t
    (*hsl_qos_port_tx_buf_status_get) (a_uint32_t dev_id, fal_port_t port_id,
				       a_bool_t * enable);

  typedef sw_error_t
    (*hsl_qos_port_red_en_set) (a_uint32_t dev_id, fal_port_t port_id,
				a_bool_t enable);

  typedef sw_error_t
    (*hsl_qos_port_red_en_get) (a_uint32_t dev_id, fal_port_t port_id,
				a_bool_t * enable);

  typedef sw_error_t
    (*hsl_qos_port_tx_buf_nr_set) (a_uint32_t dev_id, fal_port_t port_id,
				   a_uint32_t * number);

  typedef sw_error_t
    (*hsl_qos_port_tx_buf_nr_get) (a_uint32_t dev_id, fal_port_t port_id,
				   a_uint32_t * number);

  typedef sw_error_t
    (*hsl_qos_port_rx_buf_nr_set) (a_uint32_t dev_id, fal_port_t port_id,
				   a_uint32_t * number);

  typedef sw_error_t
    (*hsl_qos_port_rx_buf_nr_get) (a_uint32_t dev_id, fal_port_t port_id,
				   a_uint32_t * number);

  typedef sw_error_t
    (*hsl_qos_port_sch_mode_set) (a_uint32_t dev_id, a_uint32_t port_id,
				  fal_sch_mode_t mode,
				  const a_uint32_t weight[]);

  typedef sw_error_t
    (*hsl_qos_port_sch_mode_get) (a_uint32_t dev_id, a_uint32_t port_id,
				  fal_sch_mode_t * mode, a_uint32_t weight[]);

  typedef sw_error_t
    (*hsl_qos_port_default_spri_set) (a_uint32_t dev_id, fal_port_t port_id,
				      a_uint32_t spri);

  typedef sw_error_t
    (*hsl_qos_port_default_spri_get) (a_uint32_t dev_id, fal_port_t port_id,
				      a_uint32_t * spri);

  typedef sw_error_t
    (*hsl_qos_port_default_cpri_set) (a_uint32_t dev_id, fal_port_t port_id,
				      a_uint32_t cpri);

  typedef sw_error_t
    (*hsl_qos_port_default_cpri_get) (a_uint32_t dev_id, fal_port_t port_id,
				      a_uint32_t * cpri);

  typedef sw_error_t
    (*hsl_qos_port_force_spri_status_set) (a_uint32_t dev_id,
					   fal_port_t port_id,
					   a_bool_t enable);

  typedef sw_error_t
    (*hsl_qos_port_force_spri_status_get) (a_uint32_t dev_id,
					   fal_port_t port_id,
					   a_bool_t * enable);

  typedef sw_error_t
    (*hsl_qos_port_force_cpri_status_set) (a_uint32_t dev_id,
					   fal_port_t port_id,
					   a_bool_t enable);

  typedef sw_error_t
    (*hsl_qos_port_force_cpri_status_get) (a_uint32_t dev_id,
					   fal_port_t port_id,
					   a_bool_t * enable);

  typedef sw_error_t
    (*hsl_qos_queue_remark_table_set) (a_uint32_t dev_id, fal_port_t port_id,
				       fal_queue_t queue_id,
				       a_uint32_t tbl_id, a_bool_t enable);

  typedef sw_error_t
    (*hsl_qos_queue_remark_table_get) (a_uint32_t dev_id, fal_port_t port_id,
				       fal_queue_t queue_id,
				       a_uint32_t * tbl_id,
				       a_bool_t * enable);

  typedef sw_error_t
    (*hsl_port_static_thresh_get)(a_uint32_t dev_id, fal_port_t port,
                                       fal_bm_static_cfg_t *cfg);

  typedef sw_error_t
    (*hsl_port_static_thresh_set)(a_uint32_t dev_id, fal_port_t port,
                                       fal_bm_static_cfg_t *cfg);

  /* Rate */
#define RATE_FUNC_PROTOTYPE_DEF
  typedef sw_error_t
    (*hsl_rate_queue_egrl_set) (a_uint32_t dev_id, fal_port_t port_id,
				a_uint32_t queue_id, a_uint32_t * speed,
				a_bool_t enable);

  typedef sw_error_t
    (*hsl_rate_queue_egrl_get) (a_uint32_t dev_id, fal_port_t port_id,
				a_uint32_t queue_id, a_uint32_t * speed,
				a_bool_t * enable);

  typedef sw_error_t
    (*hsl_rate_port_egrl_set) (a_uint32_t dev_id, fal_port_t port_id,
			       a_uint32_t * speed, a_bool_t enable);

  typedef sw_error_t
    (*hsl_rate_port_egrl_get) (a_uint32_t dev_id, fal_port_t port_id,
			       a_uint32_t * speed, a_bool_t * enable);

  typedef sw_error_t
    (*hsl_rate_port_inrl_set) (a_uint32_t dev_id, fal_port_t port_id,
			       a_uint32_t * speed, a_bool_t enable);

  typedef sw_error_t
    (*hsl_rate_port_inrl_get) (a_uint32_t dev_id, fal_port_t port_id,
			       a_uint32_t * speed, a_bool_t * enable);

  typedef sw_error_t
    (*hsl_storm_ctrl_frame_set) (a_uint32_t dev_id, fal_port_t port_id,
				 fal_storm_type_t storm_type,
				 a_bool_t enable);

  typedef sw_error_t
    (*hsl_storm_ctrl_frame_get) (a_uint32_t dev_id, fal_port_t port_id,
				 fal_storm_type_t storm_type,
				 a_bool_t * enable);

  typedef sw_error_t
    (*hsl_storm_ctrl_rate_set) (a_uint32_t dev_id, fal_port_t port_id,
				a_uint32_t * rate);

  typedef sw_error_t
    (*hsl_storm_ctrl_rate_get) (a_uint32_t dev_id, fal_port_t port_id,
				a_uint32_t * rate);

  typedef sw_error_t
    (*hsl_rate_port_policer_set) (a_uint32_t dev_id, fal_port_t port_id,
				  fal_port_policer_t * policer);

  typedef sw_error_t
    (*hsl_rate_port_policer_get) (a_uint32_t dev_id, fal_port_t port_id,
				  fal_port_policer_t * policer);

  typedef sw_error_t
    (*hsl_rate_port_shaper_set) (a_uint32_t dev_id, fal_port_t port_id,
				 a_bool_t enable,
				 fal_egress_shaper_t * shaper);

  typedef sw_error_t
    (*hsl_rate_port_shaper_get) (a_uint32_t dev_id, fal_port_t port_id,
				 a_bool_t * enable,
				 fal_egress_shaper_t * shaper);

  typedef sw_error_t
    (*hsl_rate_queue_shaper_set) (a_uint32_t dev_id, fal_port_t port_id,
				  fal_queue_t queue_id, a_bool_t enable,
				  fal_egress_shaper_t * shaper);

  typedef sw_error_t
    (*hsl_rate_queue_shaper_get) (a_uint32_t dev_id, fal_port_t port_id,
				  fal_queue_t queue_id, a_bool_t * enable,
				  fal_egress_shaper_t * shaper);

  typedef sw_error_t
    (*hsl_rate_acl_policer_set) (a_uint32_t dev_id, a_uint32_t policer_id,
				 fal_acl_policer_t * policer);

  typedef sw_error_t
    (*hsl_rate_acl_policer_get) (a_uint32_t dev_id, a_uint32_t policer_id,
				 fal_acl_policer_t * policer);

  typedef sw_error_t
    (*hsl_rate_port_add_rate_byte_set) (a_uint32_t dev_id, fal_port_t port_id,
					a_uint32_t number);

  typedef sw_error_t
    (*hsl_rate_port_add_rate_byte_get) (a_uint32_t dev_id, fal_port_t port_id,
					a_uint32_t * number);
  typedef sw_error_t
    (*hsl_rate_port_gol_flow_en_set) (a_uint32_t dev_id, fal_port_t port_id,
				      a_bool_t enable);

  typedef sw_error_t
    (*hsl_rate_port_gol_flow_en_get) (a_uint32_t dev_id, fal_port_t port_id,
				      a_bool_t * enable);

  /* Mirror */
#define MIRROR_FUNC_PROTOTYPE_DEF
  typedef sw_error_t
    (*hsl_mirr_analysis_port_set) (a_uint32_t dev_id, fal_port_t port_id);

  typedef sw_error_t
    (*hsl_mirr_analysis_port_get) (a_uint32_t dev_id, fal_port_t * port_id);

  typedef sw_error_t
    (*hsl_mirr_port_in_set) (a_uint32_t dev_id, fal_port_t port_id,
			     a_bool_t enable);

  typedef sw_error_t
    (*hsl_mirr_port_in_get) (a_uint32_t dev_id, fal_port_t port_id,
			     a_bool_t * enable);

  typedef sw_error_t
    (*hsl_mirr_port_eg_set) (a_uint32_t dev_id, fal_port_t port_id,
			     a_bool_t enable);

  typedef sw_error_t
    (*hsl_mirr_port_eg_get) (a_uint32_t dev_id, fal_port_t port_id,
			     a_bool_t * enable);

  /* STP */
#define STP_FUNC_PROTOTYPE_DEF
  typedef sw_error_t
    (*hsl_stp_port_state_set) (a_uint32_t dev_id, a_uint32_t st_id,
			       fal_port_t port_id, fal_stp_state_t state);

  typedef sw_error_t
    (*hsl_stp_port_state_get) (a_uint32_t dev_id, a_uint32_t st_id,
			       fal_port_t port_id, fal_stp_state_t * state);
  /* IGMP */
#define IGMP_FUNC_PROTOTYPE_DEF
  typedef sw_error_t
    (*hsl_port_igmps_status_set) (a_uint32_t dev_id, fal_port_t port_id,
				  a_bool_t enable);

  typedef sw_error_t
    (*hsl_port_igmps_status_get) (a_uint32_t dev_id, fal_port_t port_id,
				  a_bool_t * enable);

  typedef sw_error_t
    (*hsl_igmp_mld_cmd_set) (a_uint32_t dev_id, fal_fwd_cmd_t cmd);

  typedef sw_error_t
    (*hsl_igmp_mld_cmd_get) (a_uint32_t dev_id, fal_fwd_cmd_t * cmd);

  typedef sw_error_t
    (*hsl_port_igmp_join_set) (a_uint32_t dev_id, fal_port_t port_id,
			       a_bool_t enable);

  typedef sw_error_t
    (*hsl_port_igmp_join_get) (a_uint32_t dev_id, fal_port_t port_id,
			       a_bool_t * enable);

  typedef sw_error_t
    (*hsl_port_igmp_leave_set) (a_uint32_t dev_id, fal_port_t port_id,
				a_bool_t enable);

  typedef sw_error_t
    (*hsl_port_igmp_leave_get) (a_uint32_t dev_id, fal_port_t port_id,
				a_bool_t * enable);

  typedef sw_error_t (*hsl_igmp_rp_set) (a_uint32_t dev_id, fal_pbmp_t pts);

  typedef sw_error_t (*hsl_igmp_rp_get) (a_uint32_t dev_id, fal_pbmp_t * pts);

  typedef sw_error_t
    (*hsl_igmp_entry_creat_set) (a_uint32_t dev_id, a_bool_t enable);

  typedef sw_error_t
    (*hsl_igmp_entry_creat_get) (a_uint32_t dev_id, a_bool_t * enable);

  typedef sw_error_t
    (*hsl_igmp_entry_static_set) (a_uint32_t dev_id, a_bool_t enable);

  typedef sw_error_t
    (*hsl_igmp_entry_static_get) (a_uint32_t dev_id, a_bool_t * enable);

  typedef sw_error_t
    (*hsl_igmp_entry_leaky_set) (a_uint32_t dev_id, a_bool_t enable);

  typedef sw_error_t
    (*hsl_igmp_entry_leaky_get) (a_uint32_t dev_id, a_bool_t * enable);

  typedef sw_error_t
    (*hsl_igmp_entry_v3_set) (a_uint32_t dev_id, a_bool_t enable);

  typedef sw_error_t
    (*hsl_igmp_entry_v3_get) (a_uint32_t dev_id, a_bool_t * enable);

  typedef sw_error_t
    (*hsl_igmp_entry_queue_set) (a_uint32_t dev_id, a_bool_t enable,
				 a_uint32_t queue);

  typedef sw_error_t
    (*hsl_igmp_entry_queue_get) (a_uint32_t dev_id, a_bool_t * enable,
				 a_uint32_t * queue);

  typedef sw_error_t
    (*hsl_port_igmp_mld_learn_limit_set) (a_uint32_t dev_id,
					  fal_port_t port_id, a_bool_t enable,
					  a_uint32_t cnt);

  typedef sw_error_t
    (*hsl_port_igmp_mld_learn_limit_get) (a_uint32_t dev_id,
					  fal_port_t port_id,
					  a_bool_t * enable,
					  a_uint32_t * cnt);

  typedef sw_error_t
    (*hsl_port_igmp_mld_learn_exceed_cmd_set) (a_uint32_t dev_id,
					       fal_port_t port_id,
					       fal_fwd_cmd_t cmd);

  typedef sw_error_t
    (*hsl_port_igmp_mld_learn_exceed_cmd_get) (a_uint32_t dev_id,
					       fal_port_t port_id,
					       fal_fwd_cmd_t * cmd);
  typedef sw_error_t (*hsl_igmp_sg_entry_set) (a_uint32_t dev_id,
					       fal_igmp_sg_entry_t * entry);

  typedef sw_error_t
    (*hsl_igmp_sg_entry_clear) (a_uint32_t dev_id,
				fal_igmp_sg_entry_t * entry);

  typedef sw_error_t (*hsl_igmp_sg_entry_show) (a_uint32_t dev_id);

  typedef sw_error_t
    (*hsl_igmp_sg_entry_query) (a_uint32_t dev_id, fal_igmp_sg_info_t * info);

  /* Leaky */
#define LEAKY_FUNC_PROTOTYPE_DEF
  typedef sw_error_t
    (*hsl_uc_leaky_mode_set) (a_uint32_t dev_id,
			      fal_leaky_ctrl_mode_t ctrl_mode);

  typedef sw_error_t
    (*hsl_uc_leaky_mode_get) (a_uint32_t dev_id,
			      fal_leaky_ctrl_mode_t * ctrl_mode);

  typedef sw_error_t
    (*hsl_mc_leaky_mode_set) (a_uint32_t dev_id,
			      fal_leaky_ctrl_mode_t ctrl_mode);

  typedef sw_error_t
    (*hsl_mc_leaky_mode_get) (a_uint32_t dev_id,
			      fal_leaky_ctrl_mode_t * ctrl_mode);

  typedef sw_error_t
    (*hsl_port_arp_leaky_set) (a_uint32_t dev_id, fal_port_t port_id,
			       a_bool_t enable);

  typedef sw_error_t
    (*hsl_port_arp_leaky_get) (a_uint32_t dev_id, fal_port_t port_id,
			       a_bool_t * enable);

  typedef sw_error_t
    (*hsl_port_uc_leaky_set) (a_uint32_t dev_id, fal_port_t port_id,
			      a_bool_t enable);

  typedef sw_error_t
    (*hsl_port_uc_leaky_get) (a_uint32_t dev_id, fal_port_t port_id,
			      a_bool_t * enable);

  typedef sw_error_t
    (*hsl_port_mc_leaky_set) (a_uint32_t dev_id, fal_port_t port_id,
			      a_bool_t enable);

  typedef sw_error_t
    (*hsl_port_mc_leaky_get) (a_uint32_t dev_id, fal_port_t port_id,
			      a_bool_t * enable);

  /* Mib */
#define MIB_FUNC_PROTOTYPE_DEF
  typedef sw_error_t
    (*hsl_get_mib_info) (a_uint32_t dev_id, fal_port_t port_id,
			 fal_mib_info_t * mib_info);

  typedef sw_error_t
    (*hsl_get_rx_mib_info) (a_uint32_t dev_id, fal_port_t port_id,
			    fal_mib_info_t * mib_info);

  typedef sw_error_t
    (*hsl_get_tx_mib_info) (a_uint32_t dev_id, fal_port_t port_id,
			    fal_mib_info_t * mib_info);

  typedef sw_error_t
    (*hsl_mib_status_set) (a_uint32_t dev_id, a_bool_t enable);

  typedef sw_error_t
    (*hsl_mib_status_get) (a_uint32_t dev_id, a_bool_t * enable);

  typedef sw_error_t
    (*hsl_mib_port_flush_counters) (a_uint32_t dev_id, fal_port_t port_id);

  typedef sw_error_t
    (*hsl_mib_cpukeep_set) (a_uint32_t dev_id, a_bool_t enable);

  typedef sw_error_t
    (*hsl_mib_cpukeep_get) (a_uint32_t dev_id, a_bool_t * enable);

  /* Acl */
#define ACL_FUNC_PROTOTYPE_DEF
  typedef sw_error_t
    (*hsl_acl_list_creat) (a_uint32_t dev_id, a_uint32_t list_id,
			   a_uint32_t list_pri);

  typedef sw_error_t
    (*hsl_acl_list_destroy) (a_uint32_t dev_id, a_uint32_t list_id);

  typedef sw_error_t
    (*hsl_acl_rule_add) (a_uint32_t dev_id, a_uint32_t list_id,
			 a_uint32_t rule_id, a_uint32_t rule_nr,
			 fal_acl_rule_t * rule);

  typedef sw_error_t
    (*hsl_acl_rule_delete) (a_uint32_t dev_id, a_uint32_t list_id,
			    a_uint32_t rule_id, a_uint32_t rule_nr);

  typedef sw_error_t
    (*hsl_acl_rule_query) (a_uint32_t dev_id, a_uint32_t list_id,
			   a_uint32_t rule_id, fal_acl_rule_t * rule);

  typedef sw_error_t
    (*hsl_acl_list_bind) (a_uint32_t dev_id, a_uint32_t list_id,
			  fal_acl_direc_t direc, fal_acl_bind_obj_t obj_t,
			  a_uint32_t obj_idx);

  typedef sw_error_t
    (*hsl_acl_list_unbind) (a_uint32_t dev_id, a_uint32_t list_id,
			    fal_acl_direc_t direc, fal_acl_bind_obj_t obj_t,
			    a_uint32_t obj_idx);

  typedef sw_error_t
    (*hsl_acl_status_set) (a_uint32_t dev_id, a_bool_t enable);

  typedef sw_error_t
    (*hsl_acl_status_get) (a_uint32_t dev_id, a_bool_t * enable);

  typedef sw_error_t (*hsl_acl_list_dump) (a_uint32_t dev_id);

  typedef sw_error_t (*hsl_acl_rule_dump) (a_uint32_t dev_id);

  typedef sw_error_t
    (*hsl_acl_port_udf_profile_set) (a_uint32_t dev_id, fal_port_t port_id,
				     fal_acl_udf_type_t udf_type,
				     a_uint32_t offset, a_uint32_t length);

  typedef sw_error_t
    (*hsl_acl_port_udf_profile_get) (a_uint32_t dev_id, fal_port_t port_id,
				     fal_acl_udf_type_t udf_type,
				     a_uint32_t * offset,
				     a_uint32_t * length);

  typedef sw_error_t
    (*hsl_acl_rule_active) (a_uint32_t dev_id, a_uint32_t list_id,
			    a_uint32_t rule_id, a_uint32_t rule_nr);

  typedef sw_error_t
    (*hsl_acl_rule_deactive) (a_uint32_t dev_id, a_uint32_t list_id,
			      a_uint32_t rule_id, a_uint32_t rule_nr);

  typedef sw_error_t
    (*hsl_acl_rule_src_filter_sts_set) (a_uint32_t dev_id,
					a_uint32_t rule_id, a_bool_t enable);

  typedef sw_error_t
    (*hsl_acl_rule_src_filter_sts_get) (a_uint32_t dev_id,
					a_uint32_t rule_id,
					a_bool_t * enable);

  typedef a_uint32_t
    (*hsl_acl_rule_get_offset) (a_uint32_t dev_id, a_uint32_t list_id,
				a_uint32_t rule_id);

  typedef sw_error_t
    (*hsl_acl_rule_sync_multi_portmap) (a_uint32_t dev_id, a_uint32_t pos,
					a_uint32_t * act);
/*qca808x_start*/

  typedef sw_error_t (*hsl_dev_reset) (a_uint32_t dev_id);

  typedef sw_error_t (*hsl_dev_clean) (a_uint32_t dev_id);

  typedef sw_error_t
    (*hsl_dev_access_set) (a_uint32_t dev_id, hsl_access_mode mode);
/*qca808x_end*/

  /* LED */
#define LED_FUNC_PROTOTYPE_DEF
  typedef sw_error_t
    (*hsl_led_ctrl_pattern_set) (a_uint32_t dev_id, led_pattern_group_t group,
				 led_pattern_id_t id,
				 led_ctrl_pattern_t * pattern);

  typedef sw_error_t
    (*hsl_led_ctrl_pattern_get) (a_uint32_t dev_id, led_pattern_group_t group,
				 led_pattern_id_t id,
				 led_ctrl_pattern_t * pattern);

  typedef sw_error_t
    (*hsl_led_ctrl_source_set) (a_uint32_t dev_id, a_uint32_t source_id,
				 led_ctrl_pattern_t * pattern);

  /* CoSMAP */
#define COSMAP_FUNC_PROTOTYPE_DEF
  typedef sw_error_t
    (*hsl_cosmap_dscp_to_pri_set) (a_uint32_t dev_id, a_uint32_t dscp,
				   a_uint32_t pri);

  typedef sw_error_t
    (*hsl_cosmap_dscp_to_pri_get) (a_uint32_t dev_id, a_uint32_t dscp,
				   a_uint32_t * pri);

  typedef sw_error_t
    (*hsl_cosmap_dscp_to_dp_set) (a_uint32_t dev_id, a_uint32_t dscp,
				  a_uint32_t dp);

  typedef sw_error_t
    (*hsl_cosmap_dscp_to_dp_get) (a_uint32_t dev_id, a_uint32_t dscp,
				  a_uint32_t * dp);

  typedef sw_error_t
    (*hsl_cosmap_up_to_pri_set) (a_uint32_t dev_id, a_uint32_t up,
				 a_uint32_t pri);

  typedef sw_error_t
    (*hsl_cosmap_up_to_pri_get) (a_uint32_t dev_id, a_uint32_t up,
				 a_uint32_t * pri);

  typedef sw_error_t
    (*hsl_cosmap_up_to_dp_set) (a_uint32_t dev_id, a_uint32_t up,
				a_uint32_t dp);

  typedef sw_error_t
    (*hsl_cosmap_up_to_dp_get) (a_uint32_t dev_id, a_uint32_t up,
				a_uint32_t * dp);

  typedef sw_error_t
    (*hsl_cosmap_dscp_to_ehpri_set) (a_uint32_t dev_id, a_uint32_t dscp,
				     a_uint32_t pri);

  typedef sw_error_t
    (*hsl_cosmap_dscp_to_ehpri_get) (a_uint32_t dev_id, a_uint32_t dscp,
				     a_uint32_t * pri);

  typedef sw_error_t
    (*hsl_cosmap_dscp_to_ehdp_set) (a_uint32_t dev_id, a_uint32_t dscp,
				    a_uint32_t dp);

  typedef sw_error_t
    (*hsl_cosmap_dscp_to_ehdp_get) (a_uint32_t dev_id, a_uint32_t dscp,
				    a_uint32_t * dp);

  typedef sw_error_t
    (*hsl_cosmap_up_to_ehpri_set) (a_uint32_t dev_id, a_uint32_t up,
				   a_uint32_t pri);

  typedef sw_error_t
    (*hsl_cosmap_up_to_ehpri_get) (a_uint32_t dev_id, a_uint32_t up,
				   a_uint32_t * pri);

  typedef sw_error_t
    (*hsl_cosmap_up_to_ehdp_set) (a_uint32_t dev_id, a_uint32_t up,
				  a_uint32_t dp);

  typedef sw_error_t
    (*hsl_cosmap_up_to_ehdp_get) (a_uint32_t dev_id, a_uint32_t up,
				  a_uint32_t * dp);

  typedef sw_error_t
    (*hsl_cosmap_pri_to_queue_set) (a_uint32_t dev_id, a_uint32_t pri,
				    a_uint32_t queue);

  typedef sw_error_t
    (*hsl_cosmap_pri_to_queue_get) (a_uint32_t dev_id, a_uint32_t pri,
				    a_uint32_t * queue);

  typedef sw_error_t
    (*hsl_cosmap_pri_to_ehqueue_set) (a_uint32_t dev_id, a_uint32_t pri,
				      a_uint32_t queue);

  typedef sw_error_t
    (*hsl_cosmap_pri_to_ehqueue_get) (a_uint32_t dev_id, a_uint32_t pri,
				      a_uint32_t * queue);

  typedef sw_error_t
    (*hsl_cosmap_egress_remark_set) (a_uint32_t dev_id, a_uint32_t tbl_id,
				     fal_egress_remark_table_t * tbl);

  typedef sw_error_t
    (*hsl_cosmap_egress_remark_get) (a_uint32_t dev_id, a_uint32_t tbl_id,
				     fal_egress_remark_table_t * tbl);


  /* IP */
#define IP_FUNC_PROTOTYPE_DEF
  typedef sw_error_t
    (*hsl_ip_host_add) (a_uint32_t dev_id, fal_host_entry_t * host_entry);

  typedef sw_error_t
    (*hsl_ip_host_del) (a_uint32_t dev_id, a_uint32_t del_mode,
			fal_host_entry_t * host_entry);

  typedef sw_error_t
    (*hsl_ip_host_get) (a_uint32_t dev_id, a_uint32_t get_mode,
			fal_host_entry_t * host_entry);

  typedef sw_error_t
    (*hsl_ip_host_next) (a_uint32_t dev_id, a_uint32_t next_mode,
			 fal_host_entry_t * host_entry);

  typedef sw_error_t
    (*hsl_ip_host_counter_bind) (a_uint32_t dev_id, a_uint32_t entry_id,
				 a_uint32_t cnt_id, a_bool_t enable);

  typedef sw_error_t
    (*hsl_ip_host_pppoe_bind) (a_uint32_t dev_id, a_uint32_t entry_id,
			       a_uint32_t pppoe_id, a_bool_t enable);

  typedef sw_error_t
    (*hsl_ip_pt_arp_learn_set) (a_uint32_t dev_id, fal_port_t port_id,
				a_uint32_t flags);

  typedef sw_error_t
    (*hsl_ip_pt_arp_learn_get) (a_uint32_t dev_id, fal_port_t port_id,
				a_uint32_t * flags);

  typedef sw_error_t
    (*hsl_ip_arp_learn_set) (a_uint32_t dev_id, fal_arp_learn_mode_t mode);

  typedef sw_error_t
    (*hsl_ip_arp_learn_get) (a_uint32_t dev_id, fal_arp_learn_mode_t * mode);

  typedef sw_error_t
    (*hsl_ip_source_guard_set) (a_uint32_t dev_id, fal_port_t port_id,
				fal_source_guard_mode_t mode);

  typedef sw_error_t
    (*hsl_ip_source_guard_get) (a_uint32_t dev_id, fal_port_t port_id,
				fal_source_guard_mode_t * mode);

  typedef sw_error_t
    (*hsl_ip_unk_source_cmd_set) (a_uint32_t dev_id, fal_fwd_cmd_t cmd);

  typedef sw_error_t
    (*hsl_ip_unk_source_cmd_get) (a_uint32_t dev_id, fal_fwd_cmd_t * cmd);

  typedef sw_error_t
    (*hsl_ip_arp_guard_set) (a_uint32_t dev_id, fal_port_t port_id,
			     fal_source_guard_mode_t mode);

  typedef sw_error_t
    (*hsl_ip_arp_guard_get) (a_uint32_t dev_id, fal_port_t port_id,
			     fal_source_guard_mode_t * mode);

  typedef sw_error_t
    (*hsl_arp_unk_source_cmd_set) (a_uint32_t dev_id, fal_fwd_cmd_t cmd);

  typedef sw_error_t
    (*hsl_arp_unk_source_cmd_get) (a_uint32_t dev_id, fal_fwd_cmd_t * cmd);

  typedef sw_error_t
    (*hsl_ip_route_status_set) (a_uint32_t dev_id, a_bool_t enable);

  typedef sw_error_t
    (*hsl_ip_route_status_get) (a_uint32_t dev_id, a_bool_t * enable);

  typedef sw_error_t
    (*hsl_ip_intf_entry_add) (a_uint32_t dev_id,
			      fal_intf_mac_entry_t * entry);

  typedef sw_error_t
    (*hsl_ip_intf_entry_del) (a_uint32_t dev_id, a_uint32_t del_mode,
			      fal_intf_mac_entry_t * entry);

  typedef sw_error_t
    (*hsl_ip_intf_entry_next) (a_uint32_t dev_id, a_uint32_t next_mode,
			       fal_intf_mac_entry_t * entry);

  typedef sw_error_t
    (*hsl_ip_age_time_set) (a_uint32_t dev_id, a_uint32_t * time);

  typedef sw_error_t
    (*hsl_ip_age_time_get) (a_uint32_t dev_id, a_uint32_t * time);

  typedef sw_error_t
    (*hsl_ip_wcmp_hash_mode_set) (a_uint32_t dev_id, a_uint32_t hash_mode);

  typedef sw_error_t
    (*hsl_ip_wcmp_hash_mode_get) (a_uint32_t dev_id, a_uint32_t * hash_mode);

  typedef sw_error_t
    (*hsl_ip_vrf_base_addr_set) (a_uint32_t dev_id,
				 a_uint32_t vrf_id, fal_ip4_addr_t addr);

  typedef sw_error_t
    (*hsl_ip_vrf_base_addr_get) (a_uint32_t dev_id,
				 a_uint32_t vrf_id, fal_ip4_addr_t * addr);

  typedef sw_error_t
    (*hsl_ip_vrf_base_mask_set) (a_uint32_t dev_id,
				 a_uint32_t vrf_id, fal_ip4_addr_t addr);

  typedef sw_error_t
    (*hsl_ip_vrf_base_mask_get) (a_uint32_t dev_id,
				 a_uint32_t vrf_id, fal_ip4_addr_t * addr);

  typedef sw_error_t
    (*hsl_ip_default_route_set) (a_uint32_t dev_id,
				 a_uint32_t droute_id,
				 fal_default_route_t * entry);

  typedef sw_error_t
    (*hsl_ip_default_route_get) (a_uint32_t dev_id,
				 a_uint32_t droute_id,
				 fal_default_route_t * entry);

  typedef sw_error_t
    (*hsl_ip_host_route_set) (a_uint32_t dev_id,
			      a_uint32_t hroute_id, fal_host_route_t * entry);

  typedef sw_error_t
    (*hsl_ip_host_route_get) (a_uint32_t dev_id,
			      a_uint32_t hroute_id, fal_host_route_t * entry);

  typedef sw_error_t
    (*hsl_ip_wcmp_entry_set) (a_uint32_t dev_id,
			      a_uint32_t wcmp_id, fal_ip_wcmp_t * wcmp);

  typedef sw_error_t
    (*hsl_ip_wcmp_entry_get) (a_uint32_t dev_id,
			      a_uint32_t wcmp_id, fal_ip_wcmp_t * wcmp);
  typedef sw_error_t
    (*hsl_ip_rfs_ip4_set) (a_uint32_t dev_id, fal_ip4_rfs_t * rfs);

  typedef sw_error_t
    (*hsl_ip_rfs_ip6_set) (a_uint32_t dev_id, fal_ip6_rfs_t * rfs);
  typedef sw_error_t
    (*hsl_ip_rfs_ip4_del) (a_uint32_t dev_id, fal_ip4_rfs_t * rfs);

  typedef sw_error_t
    (*hsl_ip_rfs_ip6_del) (a_uint32_t dev_id, fal_ip6_rfs_t * rfs);

  typedef sw_error_t
    (*hsl_default_flow_cmd_set) (a_uint32_t dev_id,
				 a_uint32_t vrf_id, fal_flow_type_t type,
				 fal_default_flow_cmd_t cmd);

  typedef sw_error_t
    (*hsl_default_flow_cmd_get) (a_uint32_t dev_id,
				 a_uint32_t vrf_id, fal_flow_type_t type,
				 fal_default_flow_cmd_t * cmd);

  typedef sw_error_t
    (*hsl_default_rt_flow_cmd_set) (a_uint32_t dev_id,
				    a_uint32_t vrf_id, fal_flow_type_t type,
				    fal_default_flow_cmd_t cmd);

  typedef sw_error_t
    (*hsl_default_rt_flow_cmd_get) (a_uint32_t dev_id,
				    a_uint32_t vrf_id, fal_flow_type_t type,
				    fal_default_flow_cmd_t * cmd);

typedef sw_error_t
	(*hsl_ip_glb_lock_time_set) (a_uint32_t dev_id, fal_glb_lock_time_t lock_time);

  /* NAT */
#define NAT_FUNC_PROTOTYPE_DEF
  typedef sw_error_t
    (*hsl_nat_add) (a_uint32_t dev_id, fal_nat_entry_t * nat_entry);

  typedef sw_error_t
    (*hsl_nat_del) (a_uint32_t dev_id, a_uint32_t del_mode,
		    fal_nat_entry_t * nat_entry);

  typedef sw_error_t
    (*hsl_nat_get) (a_uint32_t dev_id, a_uint32_t get_mode,
		    fal_nat_entry_t * nat_entry);

  typedef sw_error_t
    (*hsl_nat_next) (a_uint32_t dev_id, a_uint32_t next_mode,
		     fal_nat_entry_t * nat_entry);

  typedef sw_error_t
    (*hsl_nat_counter_bind) (a_uint32_t dev_id, a_uint32_t entry_id,
			     a_uint32_t cnt_id, a_bool_t enable);

  typedef sw_error_t
    (*hsl_napt_add) (a_uint32_t dev_id, fal_napt_entry_t * napt_entry);

  typedef sw_error_t
    (*hsl_napt_del) (a_uint32_t dev_id, a_uint32_t del_mode,
		     fal_napt_entry_t * napt_entry);

  typedef sw_error_t
    (*hsl_napt_get) (a_uint32_t dev_id, a_uint32_t get_mode,
		     fal_napt_entry_t * napt_entry);

  typedef sw_error_t
    (*hsl_napt_next) (a_uint32_t dev_id, a_uint32_t next_mode,
		      fal_napt_entry_t * napt_entry);

  typedef sw_error_t
    (*hsl_napt_counter_bind) (a_uint32_t dev_id, a_uint32_t entry_id,
			      a_uint32_t cnt_id, a_bool_t enable);

  typedef sw_error_t
    (*hsl_nat_status_set) (a_uint32_t dev_id, a_bool_t enable);

  typedef sw_error_t
    (*hsl_nat_status_get) (a_uint32_t dev_id, a_bool_t * enable);

  typedef sw_error_t
    (*hsl_nat_hash_mode_set) (a_uint32_t dev_id, a_uint32_t mode);

  typedef sw_error_t
    (*hsl_nat_hash_mode_get) (a_uint32_t dev_id, a_uint32_t * mode);

  typedef sw_error_t
    (*hsl_napt_status_set) (a_uint32_t dev_id, a_bool_t enable);

  typedef sw_error_t
    (*hsl_napt_status_get) (a_uint32_t dev_id, a_bool_t * enable);

  typedef sw_error_t
    (*hsl_napt_mode_set) (a_uint32_t dev_id, fal_napt_mode_t mode);

  typedef sw_error_t
    (*hsl_napt_mode_get) (a_uint32_t dev_id, fal_napt_mode_t * mode);

  typedef sw_error_t
    (*hsl_nat_prv_base_addr_set) (a_uint32_t dev_id, fal_ip4_addr_t addr);

  typedef sw_error_t
    (*hsl_nat_prv_base_addr_get) (a_uint32_t dev_id, fal_ip4_addr_t * addr);

  typedef sw_error_t
    (*hsl_nat_prv_base_mask_set) (a_uint32_t dev_id, fal_ip4_addr_t mask);

  typedef sw_error_t
    (*hsl_nat_prv_base_mask_get) (a_uint32_t dev_id, fal_ip4_addr_t * mask);

  typedef sw_error_t
    (*hsl_nat_prv_addr_mode_set) (a_uint32_t dev_id, a_bool_t map_en);

  typedef sw_error_t
    (*hsl_nat_prv_addr_mode_get) (a_uint32_t dev_id, a_bool_t * map_en);

  typedef sw_error_t
    (*hsl_nat_pub_addr_add) (a_uint32_t dev_id, fal_nat_pub_addr_t * entry);

  typedef sw_error_t
    (*hsl_nat_pub_addr_del) (a_uint32_t dev_id, a_uint32_t del_mode,
			     fal_nat_pub_addr_t * entry);

  typedef sw_error_t
    (*hsl_nat_pub_addr_next) (a_uint32_t dev_id, a_uint32_t next_mode,
			      fal_nat_pub_addr_t * entry);

  typedef sw_error_t
    (*hsl_nat_unk_session_cmd_set) (a_uint32_t dev_id, fal_fwd_cmd_t cmd);

  typedef sw_error_t
    (*hsl_nat_unk_session_cmd_get) (a_uint32_t dev_id, fal_fwd_cmd_t * cmd);

  typedef sw_error_t
    (*hsl_nat_global_set) (a_uint32_t dev_id, a_bool_t enable, a_uint32_t portbmp);

  typedef sw_error_t
    (*hsl_flow_cookie_set) (a_uint32_t dev_id,
			    fal_flow_cookie_t * flow_cookie);

  typedef sw_error_t
    (*hsl_flow_rfs_set) (a_uint32_t dev_id, a_uint8_t action,
			 fal_flow_rfs_t * rfs);

  /* SEC */
#define SEC_FUNC_PROTOTYPE_DEF
  typedef sw_error_t
    (*hsl_sec_norm_item_set) (a_uint32_t dev_id, fal_norm_item_t item,
			      void *value);

  typedef sw_error_t
    (*hsl_sec_norm_item_get) (a_uint32_t dev_id, fal_norm_item_t item,
			      void *value);


  /* Trunk */
#define TRUNK_FUNC_PROTOTYPE_DEF
  typedef sw_error_t
    (*hsl_trunk_group_set) (a_uint32_t dev_id, a_uint32_t trunk_id,
			    a_bool_t enable, fal_pbmp_t member);

  typedef sw_error_t
    (*hsl_trunk_group_get) (a_uint32_t dev_id, a_uint32_t trunk_id,
			    a_bool_t * enable, fal_pbmp_t * member);

  typedef sw_error_t
    (*hsl_trunk_hash_mode_set) (a_uint32_t dev_id, a_uint32_t hash_mode);

  typedef sw_error_t
    (*hsl_trunk_hash_mode_get) (a_uint32_t dev_id, a_uint32_t * hash_mode);

  typedef sw_error_t
    (*hsl_trunk_manipulate_sa_set) (a_uint32_t dev_id, fal_mac_addr_t * addr);

  typedef sw_error_t
    (*hsl_trunk_manipulate_sa_get) (a_uint32_t dev_id, fal_mac_addr_t * addr);

  /* Interface Control */
#define INTERFACE_CONTROL_FUNC_PROTOTYPE_DEF
  typedef sw_error_t
    (*hsl_interface_mac_mode_set) (a_uint32_t dev_id, fal_port_t port_id,
				   fal_mac_config_t * config);

  typedef sw_error_t
    (*hsl_interface_mac_mode_get) (a_uint32_t dev_id, fal_port_t port_id,
				   fal_mac_config_t * config);

  typedef sw_error_t
    (*hsl_port_3az_status_set) (a_uint32_t dev_id, fal_port_t port_id,
				a_bool_t enable);

  typedef sw_error_t
    (*hsl_port_3az_status_get) (a_uint32_t dev_id, fal_port_t port_id,
				a_bool_t * enable);

  typedef sw_error_t
    (*hsl_interface_phy_mode_set) (a_uint32_t dev_id, a_uint32_t phy_id,
				   fal_phy_config_t * config);

  typedef sw_error_t
    (*hsl_interface_phy_mode_get) (a_uint32_t dev_id, a_uint32_t phy_id,
				   fal_phy_config_t * config);

  typedef sw_error_t
    (*hsl_interface_fx100_ctrl_set) (a_uint32_t dev_id,
				     fal_fx100_ctrl_config_t * config);

  typedef sw_error_t
    (*hsl_interface_fx100_ctrl_get) (a_uint32_t dev_id,
				     fal_fx100_ctrl_config_t * config);

  typedef sw_error_t
    (*hsl_interface_fx100_status_get) (a_uint32_t dev_id,
				       a_uint32_t * status);

  typedef sw_error_t
    (*hsl_interface_mac06_exch_set) (a_uint32_t dev_id, a_bool_t enable);

  typedef sw_error_t
    (*hsl_interface_mac06_exch_get) (a_uint32_t dev_id, a_bool_t * enable);

  typedef sw_error_t
    (*hsl_interface_pad_get) (a_uint32_t dev_id,a_uint32_t port_num, a_uint32_t *value);

  typedef sw_error_t
    (*hsl_interface_pad_set) (a_uint32_t dev_id,a_uint32_t port_num, a_uint32_t value);

  typedef sw_error_t
    (*hsl_interface_sgmii_get) (a_uint32_t dev_id, a_uint32_t * value);

  typedef sw_error_t
    (*hsl_interface_sgmii_set) (a_uint32_t dev_id, a_uint32_t value);


  /* REG */
/*qca808x_start*/
#define REG_FUNC_PROTOTYPE_DEF
  typedef sw_error_t
    (*hsl_phy_get) (a_uint32_t dev_id, a_uint32_t phy_addr, a_uint32_t reg,
		    a_uint16_t * value);

  typedef sw_error_t
    (*hsl_phy_set) (a_uint32_t dev_id, a_uint32_t phy_addr, a_uint32_t reg,
		    a_uint16_t value);
/*qca808x_end*/
  typedef sw_error_t
    (*hsl_reg_get) (a_uint32_t dev_id, a_uint32_t reg_addr,
		    a_uint8_t value[], a_uint32_t value_len);

  typedef sw_error_t
    (*hsl_reg_set) (a_uint32_t dev_id, a_uint32_t reg_addr,
		    a_uint8_t value[], a_uint32_t value_len);

  typedef sw_error_t
    (*hsl_psgmii_reg_get) (a_uint32_t dev_id, a_uint32_t reg_addr,
		    a_uint8_t *value, a_uint32_t value_len);

  typedef sw_error_t
    (*hsl_psgmii_reg_set) (a_uint32_t dev_id, a_uint32_t reg_addr,
		    a_uint8_t *value, a_uint32_t value_len);

  typedef sw_error_t
    (*hsl_reg_field_get) (a_uint32_t dev_id, a_uint32_t reg_addr,
			  a_uint32_t bit_offset, a_uint32_t field_len,
			  a_uint8_t value[], a_uint32_t value_len);

  typedef sw_error_t
    (*hsl_reg_field_set) (a_uint32_t dev_id, a_uint32_t reg_addr,
			  a_uint32_t bit_offset, a_uint32_t field_len,
			  const a_uint8_t value[], a_uint32_t value_len);

  typedef sw_error_t
    (*hsl_reg_entries_get) (a_uint32_t dev_id, a_uint32_t reg_addr,
			    a_uint32_t entry_len, a_uint8_t value[],
			    a_uint32_t value_len);

  typedef sw_error_t
    (*hsl_reg_entries_set) (a_uint32_t dev_id, a_uint32_t reg_addr,
			    a_uint32_t entry_len, const a_uint8_t value[],
			    a_uint32_t value_len);

  typedef sw_error_t
    (*hsl_debug_psgmii_self_test) (a_uint32_t dev_id, a_bool_t enable,
			    a_uint32_t times, a_uint32_t *result);
  typedef sw_error_t
    (*hsl_phy_dump)(a_uint32_t dev_id, a_uint32_t phy_addr,
			a_uint32_t idx,fal_phy_dump_t *phy_dump);

  typedef sw_error_t
    (*hsl_uniphy_reg_get) (a_uint32_t dev_id, a_uint32_t index,
		    a_uint32_t reg_addr, a_uint8_t value[], a_uint32_t value_len);

  typedef sw_error_t
    (*hsl_uniphy_reg_set) (a_uint32_t dev_id, a_uint32_t index,
    a_uint32_t reg_addr, a_uint8_t value[], a_uint32_t value_len);
/*qca808x_start*/

  typedef struct
  {
/*qca808x_end*/
#if (!(defined(USER_MODE) && defined(KERNEL_MODULE)))
#ifndef HSL_STANDALONG

    /* Misc */
    hsl_arp_status_set arp_status_set;
    hsl_arp_status_get arp_status_get;
    hsl_frame_max_size_set frame_max_size_set;
    hsl_frame_max_size_get frame_max_size_get;
    hsl_port_unk_sa_cmd_set port_unk_sa_cmd_set;
    hsl_port_unk_sa_cmd_get port_unk_sa_cmd_get;
    hsl_port_unk_uc_filter_set port_unk_uc_filter_set;
    hsl_port_unk_uc_filter_get port_unk_uc_filter_get;
    hsl_port_unk_mc_filter_set port_unk_mc_filter_set;
    hsl_port_unk_mc_filter_get port_unk_mc_filter_get;
    hsl_port_bc_filter_set port_bc_filter_set;
    hsl_port_bc_filter_get port_bc_filter_get;
    hsl_nestvlan_tpid_set nestvlan_tpid_set;
    hsl_nestvlan_tpid_get nestvlan_tpid_get;
    hsl_cpu_port_status_set cpu_port_status_set;
    hsl_cpu_port_status_get cpu_port_status_get;
    hsl_bc_to_cpu_port_set bc_to_cpu_port_set;
    hsl_bc_to_cpu_port_get bc_to_cpu_port_get;
    hsl_pppoe_cmd_set pppoe_cmd_set;
    hsl_pppoe_cmd_get pppoe_cmd_get;
    hsl_pppoe_status_set pppoe_status_set;
    hsl_pppoe_status_get pppoe_status_get;
    hsl_port_dhcp_set port_dhcp_set;
    hsl_port_dhcp_get port_dhcp_get;
    hsl_arp_cmd_set arp_cmd_set;
    hsl_arp_cmd_get arp_cmd_get;
    hsl_eapol_cmd_set eapol_cmd_set;
    hsl_eapol_cmd_get eapol_cmd_get;
    hsl_pppoe_session_add pppoe_session_add;
    hsl_pppoe_session_del pppoe_session_del;
    hsl_pppoe_session_get pppoe_session_get;
    hsl_eapol_status_set eapol_status_set;
    hsl_eapol_status_get eapol_status_get;
    hsl_ripv1_status_set ripv1_status_set;
    hsl_ripv1_status_get ripv1_status_get;
    hsl_port_arp_req_status_set port_arp_req_status_set;
    hsl_port_arp_req_status_get port_arp_req_status_get;
    hsl_port_arp_ack_status_set port_arp_ack_status_set;
    hsl_port_arp_ack_status_get port_arp_ack_status_get;
    hsl_pppoe_session_table_add pppoe_session_table_add;
    hsl_pppoe_session_table_del pppoe_session_table_del;
    hsl_pppoe_session_table_get pppoe_session_table_get;
    hsl_pppoe_session_id_set pppoe_session_id_set;
    hsl_pppoe_session_id_get pppoe_session_id_get;
    hsl_intr_mask_set intr_mask_set;
    hsl_intr_mask_get intr_mask_get;
    hsl_intr_status_get intr_status_get;
    hsl_intr_status_clear intr_status_clear;
    hsl_intr_port_link_mask_set intr_port_link_mask_set;
    hsl_intr_port_link_mask_get intr_port_link_mask_get;
    hsl_intr_port_link_status_get intr_port_link_status_get;
    hsl_intr_mask_mac_linkchg_set intr_mask_mac_linkchg_set;
    hsl_intr_mask_mac_linkchg_get intr_mask_mac_linkchg_get;
    hsl_intr_status_mac_linkchg_get intr_status_mac_linkchg_get;
    hsl_cpu_vid_en_set cpu_vid_en_set;
    hsl_cpu_vid_en_get cpu_vid_en_get;
    hsl_rtd_pppoe_en_set rtd_pppoe_en_set;
    hsl_rtd_pppoe_en_get rtd_pppoe_en_get;
    hsl_intr_status_mac_linkchg_clear intr_status_mac_linkchg_clear;
    hsl_global_macaddr_set global_macaddr_set;
    hsl_global_macaddr_get global_macaddr_get;
    hsl_lldp_status_set lldp_status_set;
    hsl_lldp_status_get lldp_status_get;
    hsl_frame_crc_reserve_set frame_crc_reserve_set;
    hsl_frame_crc_reserve_get frame_crc_reserve_get;

/*qca808x_start*/
    /* Port control */
    hsl_port_duplex_set port_duplex_set;
    hsl_port_duplex_get port_duplex_get;
    hsl_port_speed_set port_speed_set;
    hsl_port_speed_get port_speed_get;
    hsl_port_autoneg_status_get port_autoneg_status_get;
    hsl_port_autoneg_enable port_autoneg_enable;
    hsl_port_autoneg_restart port_autoneg_restart;
    hsl_port_autoneg_adv_get port_autoneg_adv_get;
    hsl_port_autoneg_adv_set port_autoneg_adv_set;
/*qca808x_end*/
    hsl_port_hdr_status_set port_hdr_status_set;
    hsl_port_hdr_status_get port_hdr_status_get;
/*qca808x_start*/
    hsl_port_flowctrl_set port_flowctrl_set;
    hsl_port_flowctrl_get port_flowctrl_get;
/*qca808x_end*/
	hsl_port_flowctrl_thresh_set port_flowctrl_thresh_set;
    hsl_port_flowctrl_forcemode_set port_flowctrl_forcemode_set;
    hsl_port_flowctrl_forcemode_get port_flowctrl_forcemode_get;
/*qca808x_start*/
    hsl_port_powersave_set port_powersave_set;
    hsl_port_powersave_get port_powersave_get;
    hsl_port_hibernate_set port_hibernate_set;
    hsl_port_hibernate_get port_hibernate_get;
    hsl_port_cdt port_cdt;
/*qca808x_end*/
    hsl_port_rxhdr_mode_set port_rxhdr_mode_set;
    hsl_port_rxhdr_mode_get port_rxhdr_mode_get;
    hsl_port_txhdr_mode_set port_txhdr_mode_set;
    hsl_port_txhdr_mode_get port_txhdr_mode_get;
    hsl_header_type_set header_type_set;
    hsl_header_type_get header_type_get;
    hsl_port_txmac_status_set port_txmac_status_set;
    hsl_port_txmac_status_get port_txmac_status_get;
    hsl_port_rxmac_status_set port_rxmac_status_set;
    hsl_port_rxmac_status_get port_rxmac_status_get;
    hsl_port_txfc_status_set port_txfc_status_set;
    hsl_port_txfc_status_get port_txfc_status_get;
    hsl_port_rxfc_status_set port_rxfc_status_set;
    hsl_port_rxfc_status_get port_rxfc_status_get;
    hsl_port_bp_status_set port_bp_status_set;
    hsl_port_bp_status_get port_bp_status_get;
    hsl_port_link_forcemode_set port_link_forcemode_set;
    hsl_port_link_forcemode_get port_link_forcemode_get;
/*qca808x_start*/
    hsl_port_link_status_get port_link_status_get;
    hsl_ports_link_status_get ports_link_status_get;
/*qca808x_end*/
    hsl_port_mac_loopback_set port_mac_loopback_set;
    hsl_port_mac_loopback_get port_mac_loopback_get;
    hsl_port_congestion_drop_set port_congestion_drop_set;
    hsl_port_congestion_drop_get port_congestion_drop_get;
    hsl_ring_flow_ctrl_thres_set ring_flow_ctrl_thres_set;
    hsl_ring_flow_ctrl_thres_get ring_flow_ctrl_thres_get;
/*qca808x_start*/
    hsl_port_8023az_set port_8023az_set;
    hsl_port_8023az_get port_8023az_get;
    hsl_port_mdix_set port_mdix_set;
    hsl_port_mdix_get port_mdix_get;
    hsl_port_mdix_status_get port_mdix_status_get;
/*qca808x_end*/
    hsl_port_combo_prefer_medium_set port_combo_prefer_medium_set;
    hsl_port_combo_prefer_medium_get port_combo_prefer_medium_get;
    hsl_port_combo_medium_status_get port_combo_medium_status_get;
    hsl_port_combo_fiber_mode_set port_combo_fiber_mode_set;
    hsl_port_combo_fiber_mode_get port_combo_fiber_mode_get;
/*qca808x_start*/
    hsl_port_local_loopback_set port_local_loopback_set;
    hsl_port_local_loopback_get port_local_loopback_get;
    hsl_port_remote_loopback_set port_remote_loopback_set;
    hsl_port_remote_loopback_get port_remote_loopback_get;
    hsl_port_reset port_reset;
    hsl_port_power_off port_power_off;
    hsl_port_power_on port_power_on;
    hsl_port_phy_id_get port_phy_id_get;
    hsl_port_wol_status_set port_wol_status_set;
    hsl_port_wol_status_get port_wol_status_get;
    hsl_port_magic_frame_mac_set port_magic_frame_mac_set;
    hsl_port_magic_frame_mac_get port_magic_frame_mac_get;
/*qca808x_end*/
    hsl_port_interface_mode_set port_interface_mode_set;
    hsl_port_interface_mode_get port_interface_mode_get;
    hsl_port_interface_mode_apply port_interface_mode_apply;
/*qca808x_start*/
    hsl_port_interface_mode_status_get port_interface_mode_status_get;
    hsl_port_counter_set port_counter_set;
    hsl_port_counter_get port_counter_get;
    hsl_port_counter_show port_counter_show;
/*qca808x_end*/

    /* VLAN */
    hsl_vlan_entry_append vlan_entry_append;
    hsl_vlan_create vlan_creat;
    hsl_vlan_member_update vlan_member_update;
    hsl_vlan_delete vlan_delete;
    hsl_vlan_find vlan_find;
    hsl_vlan_next vlan_next;
    hsl_vlan_flush vlan_flush;
    hsl_vlan_fid_set vlan_fid_set;
    hsl_vlan_fid_get vlan_fid_get;
    hsl_vlan_member_add vlan_member_add;
    hsl_vlan_member_del vlan_member_del;
    hsl_vlan_learning_state_set vlan_learning_state_set;
    hsl_vlan_learning_state_get vlan_learning_state_get;

    /* Port VLAN */
    hsl_port_1qmode_set port_1qmode_set;
    hsl_port_1qmode_get port_1qmode_get;
    hsl_port_egvlanmode_get port_egvlanmode_get;
    hsl_port_egvlanmode_set port_egvlanmode_set;
    hsl_portvlan_member_add portvlan_member_add;
    hsl_portvlan_member_del portvlan_member_del;
    hsl_portvlan_member_update portvlan_member_update;
    hsl_portvlan_member_get portvlan_member_get;
    hsl_port_default_vid_set port_default_vid_set;
    hsl_port_default_vid_get port_default_vid_get;
    hsl_port_force_default_vid_set port_force_default_vid_set;
    hsl_port_force_default_vid_get port_force_default_vid_get;
    hsl_port_force_portvlan_set port_force_portvlan_set;
    hsl_port_force_portvlan_get port_force_portvlan_get;
    hsl_port_nestvlan_set port_nestvlan_set;
    hsl_port_nestvlan_get port_nestvlan_get;
    hsl_port_invlan_mode_set port_invlan_mode_set;
    hsl_port_invlan_mode_get port_invlan_mode_get;
    hsl_port_tls_set port_tls_set;
    hsl_port_tls_get port_tls_get;
    hsl_port_pri_propagation_set port_pri_propagation_set;
    hsl_port_pri_propagation_get port_pri_propagation_get;
    hsl_port_default_svid_set port_default_svid_set;
    hsl_port_default_svid_get port_default_svid_get;
    hsl_port_default_cvid_set port_default_cvid_set;
    hsl_port_default_cvid_get port_default_cvid_get;
    hsl_port_vlan_propagation_set port_vlan_propagation_set;
    hsl_port_vlan_propagation_get port_vlan_propagation_get;
    hsl_port_vlan_trans_add port_vlan_trans_add;
    hsl_port_vlan_trans_del port_vlan_trans_del;
    hsl_port_vlan_trans_get port_vlan_trans_get;
    hsl_qinq_mode_set qinq_mode_set;
    hsl_qinq_mode_get qinq_mode_get;
    hsl_port_qinq_role_set port_qinq_role_set;
    hsl_port_qinq_role_get port_qinq_role_get;
    hsl_port_vlan_trans_iterate port_vlan_trans_iterate;
    hsl_port_mac_vlan_xlt_set port_mac_vlan_xlt_set;
    hsl_port_mac_vlan_xlt_get port_mac_vlan_xlt_get;
    hsl_netisolate_set netisolate_set;
    hsl_netisolate_get netisolate_get;
    hsl_eg_trans_filter_bypass_en_set eg_trans_filter_bypass_en_set;
    hsl_eg_trans_filter_bypass_en_get eg_trans_filter_bypass_en_get;
    hsl_port_vrf_id_set port_vrf_id_set;
    hsl_port_vrf_id_get port_vrf_id_get;

    /* FDB */
    hsl_fdb_add fdb_add;
    hsl_fdb_del_all fdb_del_all;
    hsl_fdb_del_by_port fdb_del_by_port;
    hsl_fdb_del_by_mac fdb_del_by_mac;
    hsl_fdb_first fdb_first;
    hsl_fdb_next fdb_next;
    hsl_fdb_find fdb_find;
    hsl_fdb_port_learn_set port_learn_set;
    hsl_fdb_port_learn_get port_learn_get;
    hsl_fdb_age_ctrl_set age_ctrl_set;
    hsl_fdb_age_ctrl_get age_ctrl_get;
    hsl_fdb_vlan_ivl_svl_set vlan_ivl_svl_set;
    hsl_fdb_vlan_ivl_svl_get vlan_ivl_svl_get;
    hsl_fdb_age_time_set age_time_set;
    hsl_fdb_age_time_get age_time_get;
    hsl_fdb_iterate fdb_iterate;
    hsl_fdb_extend_next fdb_extend_next;
    hsl_fdb_extend_first fdb_extend_first;
    hsl_fdb_transfer fdb_transfer;
    hsl_port_fdb_learn_limit_set port_fdb_learn_limit_set;
    hsl_port_fdb_learn_limit_get port_fdb_learn_limit_get;
    hsl_port_fdb_learn_exceed_cmd_set port_fdb_learn_exceed_cmd_set;
    hsl_port_fdb_learn_exceed_cmd_get port_fdb_learn_exceed_cmd_get;
    hsl_fdb_learn_limit_set fdb_learn_limit_set;
    hsl_fdb_learn_limit_get fdb_learn_limit_get;
    hsl_fdb_learn_exceed_cmd_set fdb_learn_exceed_cmd_set;
    hsl_fdb_learn_exceed_cmd_get fdb_learn_exceed_cmd_get;
    hsl_fdb_resv_add fdb_resv_add;
    hsl_fdb_resv_del fdb_resv_del;
    hsl_fdb_resv_find fdb_resv_find;
    hsl_fdb_resv_iterate fdb_resv_iterate;
    hsl_fdb_port_learn_static_set fdb_port_learn_static_set;
    hsl_fdb_port_learn_static_get fdb_port_learn_static_get;
    hsl_fdb_port_add fdb_port_add;
    hsl_fdb_port_del fdb_port_del;
    hsl_fdb_rfs_set fdb_rfs_set;
    hsl_fdb_rfs_del fdb_rfs_del;

    /* QOS */
    hsl_qos_sch_mode_set qos_sch_mode_set;
    hsl_qos_sch_mode_get qos_sch_mode_get;
    hsl_qos_queue_tx_buf_status_set qos_queue_tx_buf_status_set;
    hsl_qos_queue_tx_buf_status_get qos_queue_tx_buf_status_get;
    hsl_qos_port_tx_buf_status_set qos_port_tx_buf_status_set;
    hsl_qos_port_tx_buf_status_get qos_port_tx_buf_status_get;
    hsl_qos_port_red_en_set qos_port_red_en_set;
    hsl_qos_port_red_en_get qos_port_red_en_get;
    hsl_qos_queue_tx_buf_nr_set qos_queue_tx_buf_nr_set;
    hsl_qos_queue_tx_buf_nr_get qos_queue_tx_buf_nr_get;
    hsl_qos_port_tx_buf_nr_set qos_port_tx_buf_nr_set;
    hsl_qos_port_tx_buf_nr_get qos_port_tx_buf_nr_get;
    hsl_qos_port_rx_buf_nr_set qos_port_rx_buf_nr_set;
    hsl_qos_port_rx_buf_nr_get qos_port_rx_buf_nr_get;
    hsl_cosmap_up_queue_set cosmap_up_queue_set;
    hsl_cosmap_up_queue_get cosmap_up_queue_get;
    hsl_cosmap_dscp_queue_set cosmap_dscp_queue_set;
    hsl_cosmap_dscp_queue_get cosmap_dscp_queue_get;
    hsl_qos_port_mode_set qos_port_mode_set;
    hsl_qos_port_mode_get qos_port_mode_get;
    hsl_qos_port_mode_pri_set qos_port_mode_pri_set;
    hsl_qos_port_mode_pri_get qos_port_mode_pri_get;
    hsl_qos_port_default_up_set qos_port_default_up_set;
    hsl_qos_port_default_up_get qos_port_default_up_get;
    hsl_qos_port_sch_mode_set qos_port_sch_mode_set;
    hsl_qos_port_sch_mode_get qos_port_sch_mode_get;
    hsl_qos_port_default_spri_set qos_port_default_spri_set;
    hsl_qos_port_default_spri_get qos_port_default_spri_get;
    hsl_qos_port_default_cpri_set qos_port_default_cpri_set;
    hsl_qos_port_default_cpri_get qos_port_default_cpri_get;
    hsl_qos_port_force_spri_status_set qos_port_force_spri_status_set;
    hsl_qos_port_force_spri_status_get qos_port_force_spri_status_get;
    hsl_qos_port_force_cpri_status_set qos_port_force_cpri_status_set;
    hsl_qos_port_force_cpri_status_get qos_port_force_cpri_status_get;

    hsl_qos_queue_remark_table_set qos_queue_remark_table_set;
    hsl_qos_queue_remark_table_get qos_queue_remark_table_get;
    hsl_port_static_thresh_get port_static_thresh_get;
    hsl_port_static_thresh_set port_static_thresh_set;

    /* Rate */
    hsl_storm_ctrl_frame_set storm_ctrl_frame_set;
    hsl_storm_ctrl_frame_get storm_ctrl_frame_get;
    hsl_storm_ctrl_rate_set storm_ctrl_rate_set;
    hsl_storm_ctrl_rate_get storm_ctrl_rate_get;
    hsl_rate_queue_egrl_set rate_queue_egrl_set;
    hsl_rate_queue_egrl_get rate_queue_egrl_get;
    hsl_rate_port_egrl_set rate_port_egrl_set;
    hsl_rate_port_egrl_get rate_port_egrl_get;
    hsl_rate_port_inrl_set rate_port_inrl_set;
    hsl_rate_port_inrl_get rate_port_inrl_get;
    hsl_rate_port_policer_set rate_port_policer_set;
    hsl_rate_port_policer_get rate_port_policer_get;
    hsl_rate_port_shaper_set rate_port_shaper_set;
    hsl_rate_port_shaper_get rate_port_shaper_get;
    hsl_rate_queue_shaper_set rate_queue_shaper_set;
    hsl_rate_queue_shaper_get rate_queue_shaper_get;
    hsl_rate_acl_policer_set rate_acl_policer_set;
    hsl_rate_acl_policer_get rate_acl_policer_get;
    hsl_rate_port_add_rate_byte_set rate_port_add_rate_byte_set;
    hsl_rate_port_add_rate_byte_get rate_port_add_rate_byte_get;
    hsl_rate_port_gol_flow_en_set rate_port_gol_flow_en_set;
    hsl_rate_port_gol_flow_en_get rate_port_gol_flow_en_get;

    /* Mirror */
    hsl_mirr_analysis_port_set mirr_analysis_port_set;
    hsl_mirr_analysis_port_get mirr_analysis_port_get;
    hsl_mirr_port_in_set mirr_port_in_set;
    hsl_mirr_port_in_get mirr_port_in_get;
    hsl_mirr_port_eg_set mirr_port_eg_set;
    hsl_mirr_port_eg_get mirr_port_eg_get;

    /* Stp */
    hsl_stp_port_state_set stp_port_state_set;
    hsl_stp_port_state_get stp_port_state_get;

    /* IGMP */
    hsl_port_igmps_status_set port_igmps_status_set;
    hsl_port_igmps_status_get port_igmps_status_get;
    hsl_igmp_mld_cmd_set igmp_mld_cmd_set;
    hsl_igmp_mld_cmd_get igmp_mld_cmd_get;
    hsl_port_igmp_join_set port_igmp_join_set;
    hsl_port_igmp_join_get port_igmp_join_get;
    hsl_port_igmp_leave_set port_igmp_leave_set;
    hsl_port_igmp_leave_get port_igmp_leave_get;
    hsl_igmp_rp_set igmp_rp_set;
    hsl_igmp_rp_get igmp_rp_get;
    hsl_igmp_entry_creat_set igmp_entry_creat_set;
    hsl_igmp_entry_creat_get igmp_entry_creat_get;
    hsl_igmp_entry_static_set igmp_entry_static_set;
    hsl_igmp_entry_static_get igmp_entry_static_get;
    hsl_igmp_entry_leaky_set igmp_entry_leaky_set;
    hsl_igmp_entry_leaky_get igmp_entry_leaky_get;
    hsl_igmp_entry_v3_set igmp_entry_v3_set;
    hsl_igmp_entry_v3_get igmp_entry_v3_get;
    hsl_igmp_entry_queue_set igmp_entry_queue_set;
    hsl_igmp_entry_queue_get igmp_entry_queue_get;
    hsl_port_igmp_mld_learn_limit_set port_igmp_mld_learn_limit_set;
    hsl_port_igmp_mld_learn_limit_get port_igmp_mld_learn_limit_get;
    hsl_port_igmp_mld_learn_exceed_cmd_set port_igmp_mld_learn_exceed_cmd_set;
    hsl_port_igmp_mld_learn_exceed_cmd_get port_igmp_mld_learn_exceed_cmd_get;
    hsl_igmp_sg_entry_set igmp_sg_entry_set;
    hsl_igmp_sg_entry_clear igmp_sg_entry_clear;
    hsl_igmp_sg_entry_show igmp_sg_entry_show;
    hsl_igmp_sg_entry_query igmp_sg_entry_query;

    /* Leaky */
    hsl_uc_leaky_mode_set uc_leaky_mode_set;
    hsl_uc_leaky_mode_get uc_leaky_mode_get;
    hsl_mc_leaky_mode_set mc_leaky_mode_set;
    hsl_mc_leaky_mode_get mc_leaky_mode_get;
    hsl_port_arp_leaky_set port_arp_leaky_set;
    hsl_port_arp_leaky_get port_arp_leaky_get;
    hsl_port_uc_leaky_set port_uc_leaky_set;
    hsl_port_uc_leaky_get port_uc_leaky_get;
    hsl_port_mc_leaky_set port_mc_leaky_set;
    hsl_port_mc_leaky_get port_mc_leaky_get;

    /* MIB API */
    hsl_get_mib_info get_mib_info;
    hsl_get_rx_mib_info get_rx_mib_info;
    hsl_get_tx_mib_info get_tx_mib_info;
    hsl_mib_status_set mib_status_set;
    hsl_mib_status_get mib_status_get;
    hsl_mib_port_flush_counters mib_port_flush_counters;
    hsl_mib_cpukeep_set mib_cpukeep_set;
    hsl_mib_cpukeep_get mib_cpukeep_get;


    /* Acl */
    hsl_acl_list_creat acl_list_creat;
    hsl_acl_list_destroy acl_list_destroy;
    hsl_acl_rule_add acl_rule_add;
    hsl_acl_rule_delete acl_rule_delete;
    hsl_acl_rule_query acl_rule_query;
    hsl_acl_list_bind acl_list_bind;
    hsl_acl_list_unbind acl_list_unbind;
    hsl_acl_status_set acl_status_set;
    hsl_acl_status_get acl_status_get;
    hsl_acl_list_dump acl_list_dump;
    hsl_acl_rule_dump acl_rule_dump;
    hsl_acl_port_udf_profile_set acl_port_udf_profile_set;
    hsl_acl_port_udf_profile_get acl_port_udf_profile_get;
    hsl_acl_rule_active acl_rule_active;
    hsl_acl_rule_deactive acl_rule_deactive;
    hsl_acl_rule_src_filter_sts_set acl_rule_src_filter_sts_set;
    hsl_acl_rule_src_filter_sts_get acl_rule_src_filter_sts_get;
    hsl_acl_rule_get_offset acl_rule_get_offset;
    hsl_acl_rule_sync_multi_portmap acl_rule_sync_multi_portmap;

    /* LED */
    hsl_led_ctrl_pattern_set led_ctrl_pattern_set;
    hsl_led_ctrl_pattern_get led_ctrl_pattern_get;
    hsl_led_ctrl_source_set led_ctrl_source_set;

    /* CoSMap */
    hsl_cosmap_dscp_to_pri_set cosmap_dscp_to_pri_set;
    hsl_cosmap_dscp_to_pri_get cosmap_dscp_to_pri_get;
    hsl_cosmap_dscp_to_dp_set cosmap_dscp_to_dp_set;
    hsl_cosmap_dscp_to_dp_get cosmap_dscp_to_dp_get;
    hsl_cosmap_up_to_pri_set cosmap_up_to_pri_set;
    hsl_cosmap_up_to_pri_get cosmap_up_to_pri_get;
    hsl_cosmap_up_to_dp_set cosmap_up_to_dp_set;
    hsl_cosmap_up_to_dp_get cosmap_up_to_dp_get;
    hsl_cosmap_dscp_to_ehpri_set cosmap_dscp_to_ehpri_set;
    hsl_cosmap_dscp_to_ehpri_get cosmap_dscp_to_ehpri_get;
    hsl_cosmap_dscp_to_ehdp_set cosmap_dscp_to_ehdp_set;
    hsl_cosmap_dscp_to_ehdp_get cosmap_dscp_to_ehdp_get;
    hsl_cosmap_up_to_ehpri_set cosmap_up_to_ehpri_set;
    hsl_cosmap_up_to_ehpri_get cosmap_up_to_ehpri_get;
    hsl_cosmap_up_to_ehdp_set cosmap_up_to_ehdp_set;
    hsl_cosmap_up_to_ehdp_get cosmap_up_to_ehdp_get;
    hsl_cosmap_pri_to_queue_set cosmap_pri_to_queue_set;
    hsl_cosmap_pri_to_queue_get cosmap_pri_to_queue_get;
    hsl_cosmap_pri_to_ehqueue_set cosmap_pri_to_ehqueue_set;
    hsl_cosmap_pri_to_ehqueue_get cosmap_pri_to_ehqueue_get;
    hsl_cosmap_egress_remark_set cosmap_egress_remark_set;
    hsl_cosmap_egress_remark_get cosmap_egress_remark_get;

    /* IP */
    hsl_ip_host_add ip_host_add;
    hsl_ip_host_del ip_host_del;
    hsl_ip_host_get ip_host_get;
    hsl_ip_host_next ip_host_next;
    hsl_ip_host_counter_bind ip_host_counter_bind;
    hsl_ip_host_pppoe_bind ip_host_pppoe_bind;
    hsl_ip_pt_arp_learn_set ip_pt_arp_learn_set;
    hsl_ip_pt_arp_learn_get ip_pt_arp_learn_get;
    hsl_ip_arp_learn_set ip_arp_learn_set;
    hsl_ip_arp_learn_get ip_arp_learn_get;
    hsl_ip_source_guard_set ip_source_guard_set;
    hsl_ip_source_guard_get ip_source_guard_get;
    hsl_ip_unk_source_cmd_set ip_unk_source_cmd_set;
    hsl_ip_unk_source_cmd_get ip_unk_source_cmd_get;
    hsl_ip_arp_guard_set ip_arp_guard_set;
    hsl_ip_arp_guard_get ip_arp_guard_get;
    hsl_arp_unk_source_cmd_set arp_unk_source_cmd_set;
    hsl_arp_unk_source_cmd_get arp_unk_source_cmd_get;
    hsl_ip_route_status_set ip_route_status_set;
    hsl_ip_route_status_get ip_route_status_get;
    hsl_ip_intf_entry_add ip_intf_entry_add;
    hsl_ip_intf_entry_del ip_intf_entry_del;
    hsl_ip_intf_entry_next ip_intf_entry_next;
    hsl_ip_age_time_set ip_age_time_set;
    hsl_ip_age_time_get ip_age_time_get;
    hsl_ip_wcmp_hash_mode_set ip_wcmp_hash_mode_set;
    hsl_ip_wcmp_hash_mode_get ip_wcmp_hash_mode_get;
    hsl_ip_vrf_base_addr_set ip_vrf_base_addr_set;
    hsl_ip_vrf_base_addr_get ip_vrf_base_addr_get;
    hsl_ip_vrf_base_mask_set ip_vrf_base_mask_set;
    hsl_ip_vrf_base_mask_get ip_vrf_base_mask_get;
    hsl_ip_default_route_set ip_default_route_set;
    hsl_ip_default_route_get ip_default_route_get;
    hsl_ip_host_route_set ip_host_route_set;
    hsl_ip_host_route_get ip_host_route_get;
    hsl_ip_wcmp_entry_set ip_wcmp_entry_set;
    hsl_ip_wcmp_entry_get ip_wcmp_entry_get;
    hsl_ip_rfs_ip4_set ip_rfs_ip4_set;
    hsl_ip_rfs_ip6_set ip_rfs_ip6_set;
    hsl_ip_rfs_ip4_del ip_rfs_ip4_del;
    hsl_ip_rfs_ip6_del ip_rfs_ip6_del;
    hsl_default_flow_cmd_set ip_default_flow_cmd_set;
    hsl_default_flow_cmd_get ip_default_flow_cmd_get;
    hsl_default_rt_flow_cmd_set ip_default_rt_flow_cmd_set;
    hsl_default_rt_flow_cmd_get ip_default_rt_flow_cmd_get;
	hsl_ip_glb_lock_time_set ip_glb_lock_time_set;

    /* NAT */
    hsl_nat_add nat_add;
    hsl_nat_del nat_del;
    hsl_nat_get nat_get;
    hsl_nat_next nat_next;
    hsl_nat_counter_bind nat_counter_bind;
    hsl_napt_add napt_add;
    hsl_napt_del napt_del;
    hsl_napt_get napt_get;
    hsl_napt_next napt_next;
    hsl_napt_counter_bind napt_counter_bind;
    hsl_napt_add flow_add;
    hsl_napt_del flow_del;
    hsl_napt_get flow_get;
    hsl_napt_next flow_next;
    hsl_napt_counter_bind flow_counter_bind;
    hsl_nat_status_set nat_status_set;
    hsl_nat_status_get nat_status_get;
    hsl_nat_hash_mode_set nat_hash_mode_set;
    hsl_nat_hash_mode_get nat_hash_mode_get;
    hsl_napt_status_set napt_status_set;
    hsl_napt_status_get napt_status_get;
    hsl_napt_mode_set napt_mode_set;
    hsl_napt_mode_get napt_mode_get;
    hsl_nat_prv_base_addr_set nat_prv_base_addr_set;
    hsl_nat_prv_base_addr_get nat_prv_base_addr_get;
    hsl_nat_prv_base_mask_set nat_prv_base_mask_set;
    hsl_nat_prv_base_mask_get nat_prv_base_mask_get;
    hsl_nat_prv_addr_mode_set nat_prv_addr_mode_set;
    hsl_nat_prv_addr_mode_get nat_prv_addr_mode_get;
    hsl_nat_pub_addr_add nat_pub_addr_add;
    hsl_nat_pub_addr_del nat_pub_addr_del;
    hsl_nat_pub_addr_next nat_pub_addr_next;
    hsl_nat_unk_session_cmd_set nat_unk_session_cmd_set;
    hsl_nat_unk_session_cmd_get nat_unk_session_cmd_get;
    hsl_nat_global_set nat_global_set;
    hsl_flow_cookie_set flow_cookie_set;
    hsl_flow_rfs_set flow_rfs_set;

    /* SEC */
    hsl_sec_norm_item_set sec_norm_item_set;
    hsl_sec_norm_item_get sec_norm_item_get;

    /* Trunk */
    hsl_trunk_group_set trunk_group_set;
    hsl_trunk_group_get trunk_group_get;
    hsl_trunk_hash_mode_set trunk_hash_mode_set;
    hsl_trunk_hash_mode_get trunk_hash_mode_get;
    hsl_trunk_manipulate_sa_set trunk_manipulate_sa_set;
    hsl_trunk_manipulate_sa_get trunk_manipulate_sa_get;

    /* Interface Control */
    hsl_interface_mac_mode_set interface_mac_mode_set;
    hsl_interface_mac_mode_get interface_mac_mode_get;
    hsl_port_3az_status_set port_3az_status_set;
    hsl_port_3az_status_get port_3az_status_get;
    hsl_interface_phy_mode_set interface_phy_mode_set;
    hsl_interface_phy_mode_get interface_phy_mode_get;
    hsl_interface_fx100_ctrl_set interface_fx100_ctrl_set;
    hsl_interface_fx100_ctrl_get interface_fx100_ctrl_get;
    hsl_interface_fx100_status_get interface_fx100_status_get;
    hsl_interface_mac06_exch_set interface_mac06_exch_set;
    hsl_interface_mac06_exch_get interface_mac06_exch_get;
    hsl_interface_pad_get	interface_mac_pad_get;
    hsl_interface_pad_set	interface_mac_pad_set;
    hsl_interface_sgmii_get	interface_mac_sgmii_get;
    hsl_interface_sgmii_set	interface_mac_sgmii_set;
#endif
#endif
/*qca808x_start*/
    /* REG Access */
    hsl_phy_get phy_get;
    hsl_phy_set phy_set;
/*qca808x_end*/
    hsl_reg_get reg_get;
    hsl_reg_set reg_set;
    hsl_reg_field_get reg_field_get;
    hsl_reg_field_set reg_field_set;
    hsl_reg_entries_get reg_entries_get;
    hsl_reg_entries_set reg_entries_set;
    hsl_psgmii_reg_get psgmii_reg_get;
    hsl_psgmii_reg_set psgmii_reg_set;
	hsl_register_dump	register_dump;
	hsl_debug_register_dump	debug_register_dump;
	hsl_debug_psgmii_self_test	debug_psgmii_self_test;
	hsl_phy_dump	 phy_dump;
    hsl_uniphy_reg_get uniphy_reg_get;
    hsl_uniphy_reg_set uniphy_reg_set;
/*qca808x_start*/
      /*INIT*/ hsl_dev_reset dev_reset;
    hsl_dev_clean dev_clean;
/*qca808x_end*/
    hsl_dev_access_set dev_access_set;
/*qca808x_start*/
    hsl_phy_get phy_i2c_get;
    hsl_phy_set phy_i2c_set;
  } hsl_api_t;

  hsl_api_t *hsl_api_ptr_get (a_uint32_t dev_id);

    sw_error_t hsl_api_init (a_uint32_t dev_id);

#if defined(SW_API_LOCK) && (!defined(HSL_STANDALONG))
  extern aos_lock_t sw_hsl_api_lock;
#define FAL_API_LOCK    aos_lock(&sw_hsl_api_lock)
#define FAL_API_UNLOCK  aos_unlock(&sw_hsl_api_lock)
#else
#define FAL_API_LOCK
#define FAL_API_UNLOCK
#endif

#ifdef __cplusplus
}
#endif				/* __cplusplus */
#endif				/* _SW_API_H */
/*qca808x_end*/
