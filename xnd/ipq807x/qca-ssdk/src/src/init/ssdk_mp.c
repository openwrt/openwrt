/*
 * Copyright (c) 2019-2020, The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "ssdk_init.h"
#include "ssdk_plat.h"
#include "ssdk_dts.h"
#include "ssdk_mp.h"
#include "adpt.h"
#include "ssdk_led.h"
#include "ssdk_clk.h"

#ifdef IN_PORTCONTROL
sw_error_t
qca_mp_portctrl_hw_init(a_uint32_t dev_id)
{
	sw_error_t rv = SW_OK;
	a_uint32_t i = 0;
	fal_port_eee_cfg_t port_eee_cfg;
	a_bool_t force_port;

	memset(&port_eee_cfg, 0, sizeof(fal_port_eee_cfg_t));

	for (i = SSDK_PHYSICAL_PORT1; i <= SSDK_PHYSICAL_PORT2; i++) {
		force_port = ssdk_port_feature_get(dev_id, i,
			PHY_F_FORCE);
		if (force_port == A_FALSE) {
			fal_port_txmac_status_set (dev_id, i, A_FALSE);
			fal_port_rxmac_status_set (dev_id, i, A_FALSE);
			/* init mac's lpi wake up timer to 70us */
			port_eee_cfg.lpi_wakeup_timer = MP_LPI_WAKEUP_TIMER;
			fal_port_interface_eee_cfg_set(dev_id, i, &port_eee_cfg);
		} else {
			fal_port_txmac_status_set (dev_id, i, A_TRUE);
			fal_port_rxmac_status_set (dev_id, i, A_TRUE);
		}
		fal_port_rxfc_status_set(dev_id, i, A_FALSE);
		fal_port_txfc_status_set(dev_id, i, A_FALSE);
		fal_port_max_frame_size_set(dev_id, i,
			FAL_DEFAULT_MAX_FRAME_SIZE);
		fal_port_promisc_mode_set(dev_id, i, A_TRUE);
		/* init software level port status */
		qca_mac_port_status_init(dev_id, i);
		/*enable ICC efuse loading*/
		ssdk_mp_gephy_icc_efuse_load_enable(A_TRUE);
	}
	return rv;
}
#endif
sw_error_t
qca_mp_interface_mode_init(a_uint32_t dev_id)
{
	sw_error_t rv = SW_OK;
	adpt_api_t *p_api;
	a_uint32_t i = 0, mode = 0;
	a_bool_t force_port;
	a_uint32_t force_speed = 0;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));
	SW_RTN_ON_NULL(p_api->adpt_uniphy_mode_set);
	SW_RTN_ON_NULL(p_api->adpt_port_mac_speed_set);
	SW_RTN_ON_NULL(p_api->adpt_port_mac_duplex_set);

	mode = ssdk_dt_global_get_mac_mode(dev_id, SSDK_UNIPHY_INSTANCE0);

	rv = p_api->adpt_uniphy_mode_set(dev_id, SSDK_UNIPHY_INSTANCE0,
				mode);
	SW_RTN_ON_ERROR(rv);

	for (i = SSDK_PHYSICAL_PORT1; i <= SSDK_PHYSICAL_PORT2; i++) {
		force_port = ssdk_port_feature_get(dev_id, i, PHY_F_FORCE);
		if (force_port == A_TRUE) {
			force_speed = ssdk_port_force_speed_get(dev_id, i);
			rv = p_api->adpt_port_mac_speed_set(dev_id,
				i, force_speed);
		} else {
			rv = p_api->adpt_port_mac_speed_set(dev_id,
				i, FAL_SPEED_1000);
		}
		SW_RTN_ON_ERROR(rv);
		rv = p_api->adpt_port_mac_duplex_set(dev_id, i, FAL_FULL_DUPLEX);
		SW_RTN_ON_ERROR(rv);
	}

	return rv;
}

sw_error_t
qca_mp_hw_init(a_uint32_t dev_id, ssdk_init_cfg *cfg)
{
	sw_error_t rv = SW_OK;

#ifdef IN_PORTCONTROL
	rv = qca_mp_portctrl_hw_init(dev_id);
	SW_RTN_ON_ERROR(rv);
#endif
#ifdef IN_MIB
	rv = fal_mib_cpukeep_set(dev_id, A_FALSE);
	SW_RTN_ON_ERROR(rv);
#endif
	rv = qca_mp_interface_mode_init(dev_id);
	SW_RTN_ON_ERROR(rv)
#ifdef IN_LED
	/*init MP led*/
	rv = ssdk_led_init(dev_id, cfg);
#endif
	return rv;
}
