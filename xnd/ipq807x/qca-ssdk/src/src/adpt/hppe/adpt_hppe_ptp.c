/*
 * Copyright (c) 2018, The Linux Foundation. All rights reserved.
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
#include "sw.h"
#include "adpt.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_port_prop.h"
#include "hsl_phy.h"

sw_error_t
adpt_hppe_ptp_config_set(a_uint32_t dev_id, a_uint32_t port_id, fal_ptp_config_t *config)
{
	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(config);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_ptp_ops.phy_ptp_config_set)
	{
		return SW_NOT_SUPPORTED;
	}

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_ptp_ops.phy_ptp_config_set(dev_id, phy_id, config);

	return rv;
}

sw_error_t
adpt_hppe_ptp_config_get(a_uint32_t dev_id, a_uint32_t port_id, fal_ptp_config_t *config)
{
	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(config);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_ptp_ops.phy_ptp_config_get)
	{
		return SW_NOT_SUPPORTED;
	}

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_ptp_ops.phy_ptp_config_get(dev_id, phy_id, config);

	return rv;
}

sw_error_t
adpt_hppe_ptp_reference_clock_set(a_uint32_t dev_id, a_uint32_t port_id, fal_ptp_reference_clock_t ref_clock)
{
	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_ptp_ops.phy_ptp_reference_clock_set)
	{
		return SW_NOT_SUPPORTED;
	}

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_ptp_ops.phy_ptp_reference_clock_set(dev_id, phy_id, ref_clock);

	return rv;
}

sw_error_t
adpt_hppe_ptp_reference_clock_get(a_uint32_t dev_id, a_uint32_t port_id, fal_ptp_reference_clock_t *ref_clock)
{
	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(ref_clock);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_ptp_ops.phy_ptp_reference_clock_get)
	{
		return SW_NOT_SUPPORTED;
	}

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_ptp_ops.phy_ptp_reference_clock_get(dev_id, phy_id, ref_clock);

	return rv;
}

sw_error_t
adpt_hppe_ptp_rx_timestamp_mode_set(a_uint32_t dev_id,
			a_uint32_t port_id,
			fal_ptp_rx_timestamp_mode_t ts_mode)
{
	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_ptp_ops.phy_ptp_rx_timestamp_mode_set)
	{
		return SW_NOT_SUPPORTED;
	}

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_ptp_ops.phy_ptp_rx_timestamp_mode_set(dev_id, phy_id, ts_mode);

	return rv;
}

sw_error_t
adpt_hppe_ptp_rx_timestamp_mode_get(a_uint32_t dev_id,
			a_uint32_t port_id,
			fal_ptp_rx_timestamp_mode_t *ts_mode)
{
	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(ts_mode);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_ptp_ops.phy_ptp_rx_timestamp_mode_get)
	{
		return SW_NOT_SUPPORTED;
	}

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_ptp_ops.phy_ptp_rx_timestamp_mode_get(dev_id, phy_id, ts_mode);

	return rv;
}

sw_error_t
adpt_hppe_ptp_timestamp_get(a_uint32_t dev_id, a_uint32_t port_id,
			fal_ptp_direction_t direction,
			fal_ptp_pkt_info_t *pkt_info,
			fal_ptp_time_t *time)
{
	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(time);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_ptp_ops.phy_ptp_timestamp_get)
	{
		return SW_NOT_SUPPORTED;
	}

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_ptp_ops.phy_ptp_timestamp_get(dev_id, phy_id, direction, pkt_info, time);

	return rv;
}

sw_error_t
adpt_hppe_ptp_pkt_timestamp_set(a_uint32_t dev_id, a_uint32_t port_id,
			fal_ptp_time_t *time)
{
	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(time);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_ptp_ops.phy_ptp_pkt_timestamp_set)
	{
		return SW_NOT_SUPPORTED;
	}

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_ptp_ops.phy_ptp_pkt_timestamp_set(dev_id, phy_id, time);

	return rv;
}

sw_error_t
adpt_hppe_ptp_pkt_timestamp_get(a_uint32_t dev_id, a_uint32_t port_id,
			fal_ptp_time_t *time)
{
	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(time);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_ptp_ops.phy_ptp_pkt_timestamp_get)
	{
		return SW_NOT_SUPPORTED;
	}

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_ptp_ops.phy_ptp_pkt_timestamp_get(dev_id, phy_id, time);

	return rv;
}

sw_error_t
adpt_hppe_ptp_grandmaster_mode_set(a_uint32_t dev_id, a_uint32_t port_id,
			fal_ptp_grandmaster_mode_t *gm_mode)
{
	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(gm_mode);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_ptp_ops.phy_ptp_grandmaster_mode_set)
	{
		return SW_NOT_SUPPORTED;
	}

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_ptp_ops.phy_ptp_grandmaster_mode_set(dev_id, phy_id, gm_mode);

	return rv;
}

sw_error_t
adpt_hppe_ptp_grandmaster_mode_get(a_uint32_t dev_id, a_uint32_t port_id,
			fal_ptp_grandmaster_mode_t *gm_mode)
{
	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(gm_mode);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_ptp_ops.phy_ptp_grandmaster_mode_get)
	{
		return SW_NOT_SUPPORTED;
	}

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_ptp_ops.phy_ptp_grandmaster_mode_get(dev_id, phy_id, gm_mode);

	return rv;
}

sw_error_t
adpt_hppe_ptp_rtc_time_get(a_uint32_t dev_id, a_uint32_t port_id,
			fal_ptp_time_t *time)
{
	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(time);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_ptp_ops.phy_ptp_rtc_time_get)
	{
		return SW_NOT_SUPPORTED;
	}

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_ptp_ops.phy_ptp_rtc_time_get(dev_id, phy_id, time);

	return rv;
}

sw_error_t
adpt_hppe_ptp_rtc_time_set(a_uint32_t dev_id, a_uint32_t port_id,
			fal_ptp_time_t *time)
{
	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(time);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_ptp_ops.phy_ptp_rtc_time_set)
	{
		return SW_NOT_SUPPORTED;
	}

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_ptp_ops.phy_ptp_rtc_time_set(dev_id, phy_id, time);

	return rv;
}

sw_error_t
adpt_hppe_ptp_rtc_time_clear(a_uint32_t dev_id, a_uint32_t port_id)
{
	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_ptp_ops.phy_ptp_rtc_time_clear)
	{
		return SW_NOT_SUPPORTED;
	}

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_ptp_ops.phy_ptp_rtc_time_clear(dev_id, phy_id);

	return rv;
}

sw_error_t
adpt_hppe_ptp_rtc_adjtime_set(a_uint32_t dev_id, a_uint32_t port_id,
			fal_ptp_time_t *time)
{
	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(time);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_ptp_ops.phy_ptp_rtc_adjtime_set)
	{
		return SW_NOT_SUPPORTED;
	}

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_ptp_ops.phy_ptp_rtc_adjtime_set(dev_id, phy_id, time);

	return rv;
}

sw_error_t
adpt_hppe_ptp_rtc_adjfreq_set(a_uint32_t dev_id, a_uint32_t port_id,
			fal_ptp_time_t *time)
{
	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(time);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_ptp_ops.phy_ptp_rtc_adjfreq_set)
	{
		return SW_NOT_SUPPORTED;
	}

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_ptp_ops.phy_ptp_rtc_adjfreq_set(dev_id, phy_id, time);

	return rv;
}

sw_error_t
adpt_hppe_ptp_rtc_adjfreq_get(a_uint32_t dev_id, a_uint32_t port_id,
			fal_ptp_time_t *time)
{
	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(time);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_ptp_ops.phy_ptp_rtc_adjfreq_get)
	{
		return SW_NOT_SUPPORTED;
	}

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_ptp_ops.phy_ptp_rtc_adjfreq_get(dev_id, phy_id, time);

	return rv;
}

sw_error_t
adpt_hppe_ptp_link_delay_set(a_uint32_t dev_id, a_uint32_t port_id,
			fal_ptp_time_t *time)
{
	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(time);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_ptp_ops.phy_ptp_link_delay_set)
	{
		return SW_NOT_SUPPORTED;
	}

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_ptp_ops.phy_ptp_link_delay_set(dev_id, phy_id, time);

	return rv;
}

sw_error_t
adpt_hppe_ptp_link_delay_get(a_uint32_t dev_id, a_uint32_t port_id,
			fal_ptp_time_t *time)
{
	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(time);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_ptp_ops.phy_ptp_link_delay_get)
	{
		return SW_NOT_SUPPORTED;
	}

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_ptp_ops.phy_ptp_link_delay_get(dev_id, phy_id, time);

	return rv;
}

sw_error_t
adpt_hppe_ptp_security_set(a_uint32_t dev_id, a_uint32_t port_id,
			fal_ptp_security_t *sec)
{
	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(sec);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_ptp_ops.phy_ptp_security_set)
	{
		return SW_NOT_SUPPORTED;
	}

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_ptp_ops.phy_ptp_security_set(dev_id, phy_id, sec);

	return rv;
}

sw_error_t
adpt_hppe_ptp_security_get(a_uint32_t dev_id, a_uint32_t port_id,
			fal_ptp_security_t *sec)
{
	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(sec);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_ptp_ops.phy_ptp_security_get)
	{
		return SW_NOT_SUPPORTED;
	}

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_ptp_ops.phy_ptp_security_get(dev_id, phy_id, sec);

	return rv;
}

sw_error_t
adpt_hppe_ptp_pps_signal_control_set(a_uint32_t dev_id,
			a_uint32_t port_id,
			fal_ptp_pps_signal_control_t *sig_control)
{
	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(sig_control);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_ptp_ops.phy_ptp_pps_signal_control_set)
	{
		return SW_NOT_SUPPORTED;
	}

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_ptp_ops.phy_ptp_pps_signal_control_set(dev_id, phy_id, sig_control);

	return rv;
}

sw_error_t
adpt_hppe_ptp_pps_signal_control_get(a_uint32_t dev_id,
			a_uint32_t port_id,
			fal_ptp_pps_signal_control_t *sig_control)
{
	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(sig_control);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_ptp_ops.phy_ptp_pps_signal_control_get)
	{
		return SW_NOT_SUPPORTED;
	}

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_ptp_ops.phy_ptp_pps_signal_control_get(dev_id, phy_id, sig_control);

	return rv;
}

sw_error_t
adpt_hppe_ptp_rx_crc_recalc_enable(a_uint32_t dev_id, a_uint32_t port_id,
			a_bool_t status)
{
	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_ptp_ops.phy_ptp_rx_crc_recalc_enable)
	{
		return SW_NOT_SUPPORTED;
	}

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_ptp_ops.phy_ptp_rx_crc_recalc_enable(dev_id, phy_id, status);

	return rv;
}

sw_error_t
adpt_hppe_ptp_rx_crc_recalc_status_get(a_uint32_t dev_id,
			a_uint32_t port_id, a_bool_t *status)
{
	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(status);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_ptp_ops.phy_ptp_rx_crc_recalc_status_get)
	{
		return SW_NOT_SUPPORTED;
	}

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_ptp_ops.phy_ptp_rx_crc_recalc_status_get(dev_id, phy_id, status);

	return rv;
}

sw_error_t
adpt_hppe_ptp_asym_correction_set(a_uint32_t dev_id, a_uint32_t port_id,
			fal_ptp_asym_correction_t *asym_cf)
{
	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(asym_cf);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_ptp_ops.phy_ptp_asym_correction_set)
	{
		return SW_NOT_SUPPORTED;
	}

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_ptp_ops.phy_ptp_asym_correction_set(dev_id, phy_id, asym_cf);

	return rv;
}

sw_error_t
adpt_hppe_ptp_asym_correction_get(a_uint32_t dev_id, a_uint32_t port_id,
			fal_ptp_asym_correction_t* asym_cf)
{
	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(asym_cf);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_ptp_ops.phy_ptp_asym_correction_get)
	{
		return SW_NOT_SUPPORTED;
	}

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_ptp_ops.phy_ptp_asym_correction_get(dev_id, phy_id, asym_cf);

	return rv;
}

sw_error_t
adpt_hppe_ptp_output_waveform_set(a_uint32_t dev_id, a_uint32_t port_id,
			fal_ptp_output_waveform_t *waveform)
{
	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(waveform);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_ptp_ops.phy_ptp_output_waveform_set)
	{
		return SW_NOT_SUPPORTED;
	}

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_ptp_ops.phy_ptp_output_waveform_set(dev_id, phy_id, waveform);

	return rv;
}

sw_error_t
adpt_hppe_ptp_output_waveform_get(a_uint32_t dev_id, a_uint32_t port_id,
			fal_ptp_output_waveform_t *waveform)
{
	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(waveform);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_ptp_ops.phy_ptp_output_waveform_get)
	{
		return SW_NOT_SUPPORTED;
	}

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_ptp_ops.phy_ptp_output_waveform_get(dev_id, phy_id, waveform);

	return rv;
}

sw_error_t
adpt_hppe_ptp_rtc_time_snapshot_enable(a_uint32_t dev_id,
			a_uint32_t port_id, a_bool_t status)
{
	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_ptp_ops.phy_ptp_rtc_time_snapshot_enable)
	{
		return SW_NOT_SUPPORTED;
	}

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_ptp_ops.phy_ptp_rtc_time_snapshot_enable(dev_id, phy_id, status);

	return rv;
}

sw_error_t
adpt_hppe_ptp_rtc_time_snapshot_status_get(a_uint32_t dev_id,
			a_uint32_t port_id, a_bool_t *status)
{
	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(status);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_ptp_ops.phy_ptp_rtc_time_snapshot_status_get)
	{
		return SW_NOT_SUPPORTED;
	}

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_ptp_ops.phy_ptp_rtc_time_snapshot_status_get(dev_id, phy_id, status);

	return rv;
}

sw_error_t
adpt_hppe_ptp_increment_sync_from_clock_enable(a_uint32_t dev_id,
			a_uint32_t port_id, a_bool_t status)
{
	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_ptp_ops.phy_ptp_increment_sync_from_clock_enable)
	{
		return SW_NOT_SUPPORTED;
	}

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_ptp_ops.phy_ptp_increment_sync_from_clock_enable(dev_id, phy_id, status);

	return rv;
}

sw_error_t
adpt_hppe_ptp_increment_sync_from_clock_status_get(a_uint32_t dev_id,
			a_uint32_t port_id, a_bool_t *status)
{
	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(status);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_ptp_ops.phy_ptp_increment_sync_from_clock_status_get)
	{
		return SW_NOT_SUPPORTED;
	}

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_ptp_ops.phy_ptp_increment_sync_from_clock_status_get(dev_id,
			phy_id, status);

	return rv;
}

sw_error_t
adpt_hppe_ptp_tod_uart_set(a_uint32_t dev_id, a_uint32_t port_id,
			fal_ptp_tod_uart_t *tod_uart)
{
	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(tod_uart);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_ptp_ops.phy_ptp_tod_uart_set)
	{
		return SW_NOT_SUPPORTED;
	}

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_ptp_ops.phy_ptp_tod_uart_set(dev_id, phy_id, tod_uart);

	return rv;
}

sw_error_t
adpt_hppe_ptp_tod_uart_get(a_uint32_t dev_id, a_uint32_t port_id,
			fal_ptp_tod_uart_t *tod_uart)
{
	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(tod_uart);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_ptp_ops.phy_ptp_tod_uart_get)
	{
		return SW_NOT_SUPPORTED;
	}

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_ptp_ops.phy_ptp_tod_uart_get(dev_id, phy_id, tod_uart);

	return rv;
}

sw_error_t
adpt_hppe_ptp_enhanced_timestamp_engine_set(a_uint32_t dev_id,
			a_uint32_t port_id, fal_ptp_direction_t direction,
			fal_ptp_enhanced_ts_engine_t *ts_engine)
{
	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(ts_engine);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_ptp_ops.phy_ptp_enhanced_timestamp_engine_set)
	{
		return SW_NOT_SUPPORTED;
	}

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_ptp_ops.phy_ptp_enhanced_timestamp_engine_set(dev_id,
			phy_id, direction, ts_engine);

	return rv;
}

sw_error_t
adpt_hppe_ptp_enhanced_timestamp_engine_get(a_uint32_t dev_id,
			a_uint32_t port_id, fal_ptp_direction_t direction,
			fal_ptp_enhanced_ts_engine_t *ts_engine)
{
	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(ts_engine);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_ptp_ops.phy_ptp_enhanced_timestamp_engine_get)
	{
		return SW_NOT_SUPPORTED;
	}

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_ptp_ops.phy_ptp_enhanced_timestamp_engine_get(dev_id,
			phy_id, direction, ts_engine);

	return rv;
}

sw_error_t
adpt_hppe_ptp_trigger_set(a_uint32_t dev_id, a_uint32_t port_id,
			a_uint32_t trigger_id, fal_ptp_trigger_t *triger)
{
	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(triger);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_ptp_ops.phy_ptp_trigger_set)
	{
		return SW_NOT_SUPPORTED;
	}

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_ptp_ops.phy_ptp_trigger_set(dev_id, phy_id, trigger_id, triger);

	return rv;
}

sw_error_t
adpt_hppe_ptp_trigger_get(a_uint32_t dev_id, a_uint32_t port_id,
			a_uint32_t trigger_id, fal_ptp_trigger_t *triger)
{
	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(triger);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_ptp_ops.phy_ptp_trigger_get)
	{
		return SW_NOT_SUPPORTED;
	}

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_ptp_ops.phy_ptp_trigger_get(dev_id, phy_id, trigger_id, triger);

	return rv;
}

sw_error_t
adpt_hppe_ptp_capture_set(a_uint32_t dev_id, a_uint32_t port_id,
			a_uint32_t capture_id, fal_ptp_capture_t *capture)
{
	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(capture);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_ptp_ops.phy_ptp_capture_set)
	{
		return SW_NOT_SUPPORTED;
	}

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_ptp_ops.phy_ptp_capture_set(dev_id, phy_id, capture_id, capture);

	return rv;
}

sw_error_t
adpt_hppe_ptp_capture_get(a_uint32_t dev_id, a_uint32_t port_id,
			a_uint32_t capture_id, fal_ptp_capture_t *capture)
{
	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(capture);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_ptp_ops.phy_ptp_capture_get)
	{
		return SW_NOT_SUPPORTED;
	}

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_ptp_ops.phy_ptp_capture_get(dev_id, phy_id, capture_id, capture);

	return rv;
}

sw_error_t
adpt_hppe_ptp_interrupt_set(a_uint32_t dev_id, a_uint32_t port_id,
			fal_ptp_interrupt_t *interrupt)
{
	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(interrupt);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_ptp_ops.phy_ptp_interrupt_set)
	{
		return SW_NOT_SUPPORTED;
	}

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_ptp_ops.phy_ptp_interrupt_set(dev_id, phy_id, interrupt);

	return rv;
}

sw_error_t
adpt_hppe_ptp_interrupt_get(a_uint32_t dev_id, a_uint32_t port_id,
			fal_ptp_interrupt_t *interrupt)
{
	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(interrupt);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_ptp_ops.phy_ptp_interrupt_get)
	{
		return SW_NOT_SUPPORTED;
	}

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_ptp_ops.phy_ptp_interrupt_get(dev_id, phy_id, interrupt);

	return rv;
}

sw_error_t adpt_hppe_ptp_init(a_uint32_t dev_id)
{
	adpt_api_t *p_adpt_api = NULL;

	p_adpt_api = adpt_api_ptr_get(dev_id);

	if(p_adpt_api == NULL)
	{
		return SW_FAIL;
	}

	p_adpt_api->adpt_ptp_config_set = adpt_hppe_ptp_config_set;
	p_adpt_api->adpt_ptp_config_get = adpt_hppe_ptp_config_get;
	p_adpt_api->adpt_ptp_reference_clock_set = adpt_hppe_ptp_reference_clock_set;
	p_adpt_api->adpt_ptp_reference_clock_get = adpt_hppe_ptp_reference_clock_get;
	p_adpt_api->adpt_ptp_rx_timestamp_mode_set = adpt_hppe_ptp_rx_timestamp_mode_set;
	p_adpt_api->adpt_ptp_rx_timestamp_mode_get = adpt_hppe_ptp_rx_timestamp_mode_get;
	p_adpt_api->adpt_ptp_timestamp_get = adpt_hppe_ptp_timestamp_get;
	p_adpt_api->adpt_ptp_pkt_timestamp_set = adpt_hppe_ptp_pkt_timestamp_set;
	p_adpt_api->adpt_ptp_pkt_timestamp_get = adpt_hppe_ptp_pkt_timestamp_get;
	p_adpt_api->adpt_ptp_grandmaster_mode_set = adpt_hppe_ptp_grandmaster_mode_set;
	p_adpt_api->adpt_ptp_grandmaster_mode_get = adpt_hppe_ptp_grandmaster_mode_get;
	p_adpt_api->adpt_ptp_rtc_time_set = adpt_hppe_ptp_rtc_time_set;
	p_adpt_api->adpt_ptp_rtc_time_get = adpt_hppe_ptp_rtc_time_get;
	p_adpt_api->adpt_ptp_rtc_time_clear = adpt_hppe_ptp_rtc_time_clear;
	p_adpt_api->adpt_ptp_rtc_adjtime_set = adpt_hppe_ptp_rtc_adjtime_set;
	p_adpt_api->adpt_ptp_rtc_adjfreq_set = adpt_hppe_ptp_rtc_adjfreq_set;
	p_adpt_api->adpt_ptp_rtc_adjfreq_get = adpt_hppe_ptp_rtc_adjfreq_get;
	p_adpt_api->adpt_ptp_link_delay_set = adpt_hppe_ptp_link_delay_set;
	p_adpt_api->adpt_ptp_link_delay_get = adpt_hppe_ptp_link_delay_get;
	p_adpt_api->adpt_ptp_security_set = adpt_hppe_ptp_security_set;
	p_adpt_api->adpt_ptp_security_get = adpt_hppe_ptp_security_get;
	p_adpt_api->adpt_ptp_pps_signal_control_set = adpt_hppe_ptp_pps_signal_control_set;
	p_adpt_api->adpt_ptp_pps_signal_control_get = adpt_hppe_ptp_pps_signal_control_get;
	p_adpt_api->adpt_ptp_rx_crc_recalc_enable = adpt_hppe_ptp_rx_crc_recalc_enable;
	p_adpt_api->adpt_ptp_rx_crc_recalc_status_get = adpt_hppe_ptp_rx_crc_recalc_status_get;
	p_adpt_api->adpt_ptp_asym_correction_set = adpt_hppe_ptp_asym_correction_set;
	p_adpt_api->adpt_ptp_asym_correction_get = adpt_hppe_ptp_asym_correction_get;
	p_adpt_api->adpt_ptp_output_waveform_set = adpt_hppe_ptp_output_waveform_set;
	p_adpt_api->adpt_ptp_output_waveform_get = adpt_hppe_ptp_output_waveform_get;
	p_adpt_api->adpt_ptp_rtc_time_snapshot_enable = adpt_hppe_ptp_rtc_time_snapshot_enable;
	p_adpt_api->adpt_ptp_tod_uart_set = adpt_hppe_ptp_tod_uart_set;
	p_adpt_api->adpt_ptp_tod_uart_get = adpt_hppe_ptp_tod_uart_get;
	p_adpt_api->adpt_ptp_trigger_set = adpt_hppe_ptp_trigger_set;
	p_adpt_api->adpt_ptp_trigger_get = adpt_hppe_ptp_trigger_get;
	p_adpt_api->adpt_ptp_capture_set = adpt_hppe_ptp_capture_set;
	p_adpt_api->adpt_ptp_capture_get = adpt_hppe_ptp_capture_get;
	p_adpt_api->adpt_ptp_interrupt_set = adpt_hppe_ptp_interrupt_set;
	p_adpt_api->adpt_ptp_interrupt_get = adpt_hppe_ptp_interrupt_get;
	p_adpt_api->adpt_ptp_rtc_time_snapshot_status_get =
		adpt_hppe_ptp_rtc_time_snapshot_status_get;
	p_adpt_api->adpt_ptp_increment_sync_from_clock_enable =
		adpt_hppe_ptp_increment_sync_from_clock_enable;
	p_adpt_api->adpt_ptp_increment_sync_from_clock_status_get =
		adpt_hppe_ptp_increment_sync_from_clock_status_get;
	p_adpt_api->adpt_ptp_enhanced_timestamp_engine_set =
		adpt_hppe_ptp_enhanced_timestamp_engine_set;
	p_adpt_api->adpt_ptp_enhanced_timestamp_engine_get =
		adpt_hppe_ptp_enhanced_timestamp_engine_get;

	return SW_OK;
}

/**
 * @}
 */
