/*
 * Copyright (c) 2017-2018, The Linux Foundation. All rights reserved.
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
#ifndef _SCOMPHY_PORT_CTRL_H_
#define _SCOMPHY_PORT_CTRL_H_

#ifdef __cplusplus
extern "C"
{
#endif				/* __cplusplus */

#include "fal/fal_port_ctrl.h"

sw_error_t scomphy_port_ctrl_init (a_uint32_t dev_id);

sw_error_t
scomphy_port_reset (a_uint32_t dev_id, fal_port_t port_id);

/*qca808x_end*/
#ifdef IN_PORTCONTROL
/*qca808x_start*/
#define SCOMPHY_PORT_CTRL_INIT(rv, dev_id) \
    { \
        rv = scomphy_port_ctrl_init(dev_id); \
        SW_RTN_ON_ERROR(rv); \
    }
/*qca808x_end*/
#else
#define SCOMPHY_PORT_CTRL_INIT(rv, dev_id)
#endif

#ifdef HSL_STANDALONG


HSL_LOCAL sw_error_t
scomphy_port_duplex_set (a_uint32_t dev_id, fal_port_t port_id,
		fal_port_duplex_t duplex);


HSL_LOCAL sw_error_t
scomphy_port_duplex_get (a_uint32_t dev_id, fal_port_t port_id,
		fal_port_duplex_t * pduplex);


HSL_LOCAL sw_error_t
scomphy_port_speed_set (a_uint32_t dev_id, fal_port_t port_id,
		fal_port_speed_t speed);


HSL_LOCAL sw_error_t
scomphy_port_speed_get (a_uint32_t dev_id, fal_port_t port_id,
		fal_port_speed_t * pspeed);


HSL_LOCAL sw_error_t
scomphy_port_autoneg_status_get (a_uint32_t dev_id, fal_port_t port_id,
		a_bool_t * status);


HSL_LOCAL sw_error_t
scomphy_port_autoneg_enable (a_uint32_t dev_id, fal_port_t port_id);


HSL_LOCAL sw_error_t
scomphy_port_autoneg_restart (a_uint32_t dev_id, fal_port_t port_id);


HSL_LOCAL sw_error_t
scomphy_port_autoneg_adv_set (a_uint32_t dev_id, fal_port_t port_id,
		a_uint32_t autoadv);


HSL_LOCAL sw_error_t
scomphy_port_autoneg_adv_get (a_uint32_t dev_id, fal_port_t port_id,
		a_uint32_t * autoadv);

#ifndef IN_PORTCONTROL_MINI
HSL_LOCAL sw_error_t
scomphy_port_powersave_set (a_uint32_t dev_id, fal_port_t port_id,
		a_bool_t enable);


HSL_LOCAL sw_error_t
scomphy_port_powersave_get (a_uint32_t dev_id, fal_port_t port_id,
		a_bool_t * enable);


HSL_LOCAL sw_error_t
scomphy_port_hibernate_set (a_uint32_t dev_id, fal_port_t port_id,
		a_bool_t enable);


HSL_LOCAL sw_error_t
scomphy_port_hibernate_get (a_uint32_t dev_id, fal_port_t port_id,
		a_bool_t * enable);


HSL_LOCAL sw_error_t
scomphy_port_cdt (a_uint32_t dev_id, fal_port_t port_id, a_uint32_t mdi_pair,
		fal_cable_status_t * cable_status, a_uint32_t * cable_len);
#endif

HSL_LOCAL sw_error_t
scomphy_port_link_status_get (a_uint32_t dev_id, fal_port_t port_id,
		a_bool_t * status);
#ifndef IN_PORTCONTROL_MINI

HSL_LOCAL sw_error_t
scomphy_port_8023az_set (a_uint32_t dev_id, fal_port_t port_id,
		a_bool_t enable);

HSL_LOCAL sw_error_t
scomphy_port_8023az_get (a_uint32_t dev_id, fal_port_t port_id,
		a_bool_t * enable);

HSL_LOCAL sw_error_t
scomphy_port_mdix_set (a_uint32_t dev_id, a_uint32_t phy_id,
		fal_port_mdix_mode_t mode);

HSL_LOCAL sw_error_t
scomphy_port_mdix_get (a_uint32_t dev_id, fal_port_t port_id,
		fal_port_mdix_mode_t * mode);

HSL_LOCAL sw_error_t
scomphy_port_mdix_status_get (a_uint32_t dev_id, fal_port_t port_id,
		fal_port_mdix_status_t * mode);

HSL_LOCAL sw_error_t
scomphy_port_combo_prefer_medium_set (a_uint32_t dev_id, a_uint32_t phy_id,
		fal_port_medium_t phy_medium);

HSL_LOCAL sw_error_t
scomphy_port_combo_prefer_medium_get (a_uint32_t dev_id, a_uint32_t phy_id,
		fal_port_medium_t * phy_medium);

HSL_LOCAL sw_error_t
scomphy_port_combo_medium_status_get (a_uint32_t dev_id, a_uint32_t phy_id,
		fal_port_medium_t * phy_medium);

HSL_LOCAL sw_error_t
scomphy_port_combo_fiber_mode_set (a_uint32_t dev_id, a_uint32_t phy_id,
		fal_port_fiber_mode_t fiber_mode);

HSL_LOCAL sw_error_t
scomphy_port_combo_fiber_mode_get (a_uint32_t dev_id, a_uint32_t phy_id,
		fal_port_fiber_mode_t * fiber_mode);

HSL_LOCAL sw_error_t
scomphy_port_local_loopback_set (a_uint32_t dev_id, fal_port_t port_id,
		a_bool_t enable);

HSL_LOCAL sw_error_t
scomphy_port_local_loopback_get (a_uint32_t dev_id, fal_port_t port_id,
		a_bool_t * enable);

HSL_LOCAL sw_error_t
scomphy_port_remote_loopback_set (a_uint32_t dev_id, fal_port_t port_id,
		a_bool_t enable);

HSL_LOCAL sw_error_t
scomphy_port_remote_loopback_get (a_uint32_t dev_id, fal_port_t port_id,
		a_bool_t * enable);

HSL_LOCAL sw_error_t
scomphy_port_magic_frame_mac_set (a_uint32_t dev_id, fal_port_t port_id,
		fal_mac_addr_t * mac);

HSL_LOCAL sw_error_t
scomphy_port_magic_frame_mac_get (a_uint32_t dev_id, fal_port_t port_id,
		fal_mac_addr_t * mac);

HSL_LOCAL sw_error_t
scomphy_port_phy_id_get (a_uint32_t dev_id, fal_port_t port_id,
		a_uint16_t * org_id, a_uint16_t * rev_id);

HSL_LOCAL sw_error_t
scomphy_port_wol_status_set (a_uint32_t dev_id, fal_port_t port_id,
		a_bool_t enable);

HSL_LOCAL sw_error_t
scomphy_port_wol_status_get (a_uint32_t dev_id, fal_port_t port_id,
		a_bool_t * enable);
#endif
HSL_LOCAL sw_error_t
scomphy_port_power_off (a_uint32_t dev_id, fal_port_t port_id);

HSL_LOCAL sw_error_t
scomphy_port_power_on (a_uint32_t dev_id, fal_port_t port_id);

#ifndef IN_PORTCONTROL_MINI
HSL_LOCAL sw_error_t
scomphy_port_interface_mode_set (a_uint32_t dev_id, fal_port_t port_id,
			fal_port_interface_mode_t mode);

HSL_LOCAL sw_error_t
scomphy_port_interface_mode_get (a_uint32_t dev_id, fal_port_t port_id,
		fal_port_interface_mode_t * mode);

HSL_LOCAL sw_error_t
scomphy_port_interface_mode_status_get (a_uint32_t dev_id, fal_port_t port_id,
		fal_port_interface_mode_t * mode);

HSL_LOCAL sw_error_t
scomphy_port_counter_set (a_uint32_t dev_id, fal_port_t port_id,
		a_bool_t enable);

HSL_LOCAL sw_error_t
scomphy_port_counter_get (a_uint32_t dev_id, fal_port_t port_id,
		a_bool_t * enable);

HSL_LOCAL sw_error_t
scomphy_port_counter_show (a_uint32_t dev_id, fal_port_t port_id,
		fal_port_counter_info_t * counter_info);
#endif
#endif
/*qca808x_start*/
#ifdef __cplusplus
}
#endif				/* __cplusplus */
#endif				/* _SCOMPHY_PORT_CTRL_H_ */
/*qca808x_end*/
