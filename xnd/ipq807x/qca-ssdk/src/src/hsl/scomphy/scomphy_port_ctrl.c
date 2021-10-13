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
/**
 * @defgroup scomphy_port_ctrl SCOMPHY_PORT_CONTROL
 * @{
 */
#include "sw.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_port_prop.h"
#include "scomphy_port_ctrl.h"
#include "hsl_phy.h"

static sw_error_t
_scomphy_port_duplex_set (a_uint32_t dev_id, fal_port_t port_id,
		       fal_port_duplex_t duplex)
{
	sw_error_t rv;
	a_uint32_t phy_id;
	hsl_phy_ops_t *phy_drv;

	HSL_DEV_ID_CHECK (dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_duplex_set)
		return SW_NOT_SUPPORTED;

	if (FAL_DUPLEX_BUTT <= duplex)
	{
		return SW_BAD_PARAM;
	}
	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_duplex_set (dev_id, phy_id, duplex);
	SW_RTN_ON_ERROR (rv);
	return rv;
}

static sw_error_t
_scomphy_port_duplex_get (a_uint32_t dev_id, fal_port_t port_id,
		       fal_port_duplex_t * pduplex)
{
	sw_error_t rv = SW_OK;
	a_uint32_t phy_id;
	hsl_phy_ops_t *phy_drv;

	HSL_DEV_ID_CHECK (dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_duplex_get)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_duplex_get (dev_id, phy_id, pduplex);
	SW_RTN_ON_ERROR (rv);

	return rv;
}

static sw_error_t
_scomphy_port_speed_set (a_uint32_t dev_id, fal_port_t port_id,
		      fal_port_speed_t speed)
{
	sw_error_t rv;
	a_uint32_t phy_id;
	hsl_phy_ops_t *phy_drv;

	HSL_DEV_ID_CHECK (dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_speed_set)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_speed_set (dev_id, phy_id, speed);
	SW_RTN_ON_ERROR (rv);
	return rv;
}

static sw_error_t
_scomphy_port_speed_get (a_uint32_t dev_id, fal_port_t port_id,
		      fal_port_speed_t * pspeed)
{
	sw_error_t rv = SW_OK;
	a_uint32_t phy_id;
	hsl_phy_ops_t *phy_drv;

	HSL_DEV_ID_CHECK (dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_speed_get)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);
	rv = phy_drv->phy_speed_get (dev_id, phy_id, pspeed);
	SW_RTN_ON_ERROR (rv);

	return rv;
}

static sw_error_t
_scomphy_port_autoneg_status_get (a_uint32_t dev_id, fal_port_t port_id,
			       a_bool_t * status)
{
	a_uint32_t phy_id;
	sw_error_t rv;
	hsl_phy_ops_t *phy_drv;

	HSL_DEV_ID_CHECK (dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_autoneg_status_get)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	*status = phy_drv->phy_autoneg_status_get (dev_id, phy_id);

	return SW_OK;
}

static sw_error_t
_scomphy_port_autoneg_enable (a_uint32_t dev_id, fal_port_t port_id)
{
	sw_error_t rv;
	a_uint32_t phy_id;
	hsl_phy_ops_t *phy_drv;

	HSL_DEV_ID_CHECK (dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_autoneg_enable_set)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_autoneg_enable_set (dev_id, phy_id);
	return rv;
}

static sw_error_t
_scomphy_port_autoneg_restart (a_uint32_t dev_id, fal_port_t port_id)
{
	sw_error_t rv;
	a_uint32_t phy_id;
	hsl_phy_ops_t *phy_drv;

	HSL_DEV_ID_CHECK (dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_restart_autoneg)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_restart_autoneg (dev_id, phy_id);
	return rv;
}

static sw_error_t
_scomphy_port_autoneg_adv_set (a_uint32_t dev_id, fal_port_t port_id,
			    a_uint32_t autoadv)
{
	sw_error_t rv;
	a_uint32_t phy_id;
	hsl_phy_ops_t *phy_drv;

	HSL_DEV_ID_CHECK (dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_autoneg_adv_set)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_autoneg_adv_set (dev_id, phy_id, autoadv);
	SW_RTN_ON_ERROR (rv);

	return SW_OK;
}

static sw_error_t
_scomphy_port_autoneg_adv_get (a_uint32_t dev_id, fal_port_t port_id,
			    a_uint32_t * autoadv)
{
	sw_error_t rv;
	a_uint32_t phy_id;
	hsl_phy_ops_t *phy_drv;

	HSL_DEV_ID_CHECK (dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_autoneg_adv_get)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	*autoadv = 0;
	rv = phy_drv->phy_autoneg_adv_get (dev_id, phy_id, autoadv);
	SW_RTN_ON_ERROR (rv);

	return SW_OK;
}

#ifndef IN_PORTCONTROL_MINI
static sw_error_t
_scomphy_port_powersave_set (a_uint32_t dev_id, fal_port_t port_id,
			  a_bool_t enable)
{
	sw_error_t rv;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	HSL_DEV_ID_CHECK (dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_powersave_set)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_powersave_set (dev_id, phy_id, enable);

	return rv;
}

static sw_error_t
_scomphy_port_powersave_get (a_uint32_t dev_id, fal_port_t port_id,
			  a_bool_t * enable)
{
	sw_error_t rv;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	HSL_DEV_ID_CHECK (dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_powersave_get)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_powersave_get (dev_id, phy_id, enable);

	return rv;
}

static sw_error_t
_scomphy_port_hibernate_set (a_uint32_t dev_id, fal_port_t port_id,
			  a_bool_t enable)
{
	sw_error_t rv;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	HSL_DEV_ID_CHECK (dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_hibernation_set)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_hibernation_set (dev_id, phy_id, enable);

	return rv;
}

static sw_error_t
_scomphy_port_hibernate_get (a_uint32_t dev_id, fal_port_t port_id,
			  a_bool_t * enable)
{
	sw_error_t rv;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	HSL_DEV_ID_CHECK (dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_hibernation_get)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_hibernation_get (dev_id, phy_id, enable);

	return rv;
}

static sw_error_t
_scomphy_port_cdt (a_uint32_t dev_id, fal_port_t port_id, a_uint32_t mdi_pair,
		fal_cable_status_t * cable_status, a_uint32_t * cable_len)
{
	sw_error_t rv;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	HSL_DEV_ID_CHECK (dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_cdt)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_cdt (dev_id, phy_id, mdi_pair, cable_status, cable_len);

	return rv;
}

static sw_error_t
_scomphy_port_8023az_set (a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
	sw_error_t rv;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	HSL_DEV_ID_CHECK (dev_id);
	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_8023az_set)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_8023az_set (dev_id, phy_id, enable);

	return rv;
}

static sw_error_t
_scomphy_port_8023az_get (a_uint32_t dev_id, fal_port_t port_id,
		       a_bool_t * enable)
{
	sw_error_t rv;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	HSL_DEV_ID_CHECK (dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_8023az_get)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_8023az_get (dev_id, phy_id, enable);

	return rv;
}

static sw_error_t
_scomphy_port_mdix_set (a_uint32_t dev_id, fal_port_t port_id,
		     fal_port_mdix_mode_t mode)
{
	sw_error_t rv;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	HSL_DEV_ID_CHECK (dev_id);
	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_mdix_set)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_mdix_set (dev_id, phy_id, mode);

	return rv;
}

static sw_error_t
_scomphy_port_mdix_get (a_uint32_t dev_id, fal_port_t port_id,
		     fal_port_mdix_mode_t * mode)
{
	sw_error_t rv;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	HSL_DEV_ID_CHECK (dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_mdix_get)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_mdix_get (dev_id, phy_id, mode);

	return rv;
}

static sw_error_t
_scomphy_port_mdix_status_get (a_uint32_t dev_id, fal_port_t port_id,
			    fal_port_mdix_status_t * mode)
{
	sw_error_t rv;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	HSL_DEV_ID_CHECK (dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_mdix_status_get)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_mdix_status_get (dev_id, phy_id, mode);

	return rv;
}
/*qca808x_end*/
static sw_error_t
_scomphy_port_combo_prefer_medium_set (a_uint32_t dev_id, fal_port_t port_id,
				    fal_port_medium_t phy_medium)
{
	sw_error_t rv;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	HSL_DEV_ID_CHECK (dev_id);
	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_combo_prefer_medium_set)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_combo_prefer_medium_set (dev_id, phy_id, phy_medium);

	return rv;
}

static sw_error_t
_scomphy_port_combo_prefer_medium_get (a_uint32_t dev_id, fal_port_t port_id,
				    fal_port_medium_t * phy_medium)
{
	sw_error_t rv;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	HSL_DEV_ID_CHECK (dev_id);
	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_combo_prefer_medium_get)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_combo_prefer_medium_get (dev_id, phy_id, phy_medium);

	return rv;
}

static sw_error_t
_scomphy_port_combo_medium_status_get (a_uint32_t dev_id, fal_port_t port_id,
				    fal_port_medium_t * phy_medium)
{
	sw_error_t rv;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	HSL_DEV_ID_CHECK (dev_id);
	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_combo_medium_status_get)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_combo_medium_status_get (dev_id, phy_id, phy_medium);

	return rv;
}

static sw_error_t
_scomphy_port_combo_fiber_mode_set (a_uint32_t dev_id, fal_port_t port_id,
				 fal_port_fiber_mode_t fiber_mode)
{
	sw_error_t rv;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	HSL_DEV_ID_CHECK (dev_id);
	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_combo_fiber_mode_set)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_combo_fiber_mode_set (dev_id, phy_id, fiber_mode);

	return rv;
}

static sw_error_t
_scomphy_port_combo_fiber_mode_get (a_uint32_t dev_id, fal_port_t port_id,
				 fal_port_fiber_mode_t * fiber_mode)
{
	sw_error_t rv;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	HSL_DEV_ID_CHECK (dev_id);
	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_combo_fiber_mode_get)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_combo_fiber_mode_get (dev_id, phy_id, fiber_mode);

	return rv;
}
/*qca808x_start*/
static sw_error_t
_scomphy_port_local_loopback_set (a_uint32_t dev_id, fal_port_t port_id,
			       a_bool_t enable)
{
	sw_error_t rv;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	HSL_DEV_ID_CHECK (dev_id);
	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_local_loopback_set)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_local_loopback_set (dev_id, phy_id, enable);

	return rv;
}

static sw_error_t
_scomphy_port_local_loopback_get (a_uint32_t dev_id, fal_port_t port_id,
			       a_bool_t * enable)
{
	sw_error_t rv;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	HSL_DEV_ID_CHECK (dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_local_loopback_get)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_local_loopback_get (dev_id, phy_id, enable);

	return rv;
}

static sw_error_t
_scomphy_port_remote_loopback_set (a_uint32_t dev_id, fal_port_t port_id,
				a_bool_t enable)
{
	sw_error_t rv;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	HSL_DEV_ID_CHECK (dev_id);
	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_remote_loopback_set)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_remote_loopback_set (dev_id, phy_id, enable);

	return rv;
}

static sw_error_t
_scomphy_port_remote_loopback_get (a_uint32_t dev_id, fal_port_t port_id,
				a_bool_t * enable)
{
	sw_error_t rv;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	HSL_DEV_ID_CHECK (dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_remote_loopback_get)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_remote_loopback_get (dev_id, phy_id, enable);

	return rv;
}
#endif

static sw_error_t
_scomphy_port_power_off (a_uint32_t dev_id, fal_port_t port_id)
{
	sw_error_t rv;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	HSL_DEV_ID_CHECK (dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_power_off)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_power_off(dev_id, phy_id);

	return rv;
}

static sw_error_t
_scomphy_port_power_on (a_uint32_t dev_id, fal_port_t port_id)
{
	sw_error_t rv;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	HSL_DEV_ID_CHECK (dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_power_on)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_power_on(dev_id, phy_id);

	return rv;
}

#ifndef IN_PORTCONTROL_MINI
static sw_error_t
_scomphy_port_wol_status_set (a_uint32_t dev_id, fal_port_t port_id,
				a_bool_t enable)
{
	sw_error_t rv;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	HSL_DEV_ID_CHECK (dev_id);
	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_wol_status_set)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_wol_status_set (dev_id, phy_id, enable);

	return rv;
}

static sw_error_t
_scomphy_port_wol_status_get (a_uint32_t dev_id, fal_port_t port_id,
				a_bool_t * enable)
{
	sw_error_t rv;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	HSL_DEV_ID_CHECK (dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_wol_status_get)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_wol_status_get (dev_id, phy_id, enable);

	return rv;
}
/*qca808x_end*/
static sw_error_t
_scomphy_port_interface_mode_set (a_uint32_t dev_id, fal_port_t port_id,
				fal_port_interface_mode_t  mode)
{
	sw_error_t rv;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	HSL_DEV_ID_CHECK (dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_interface_mode_set)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_interface_mode_set (dev_id, phy_id,mode);

	return rv;
}

static sw_error_t
_scomphy_port_interface_mode_get (a_uint32_t dev_id, fal_port_t port_id,
				fal_port_interface_mode_t * mode)
{
	sw_error_t rv;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	HSL_DEV_ID_CHECK (dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_interface_mode_get)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_interface_mode_get (dev_id, phy_id,mode);

	return rv;
}
/*qca808x_start*/
static sw_error_t
_scomphy_port_interface_mode_status_get (a_uint32_t dev_id, fal_port_t port_id,
				fal_port_interface_mode_t * mode)
{
	sw_error_t rv;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	HSL_DEV_ID_CHECK (dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_interface_mode_status_get)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_interface_mode_status_get (dev_id, phy_id,mode);

	return rv;
}

static sw_error_t
_scomphy_port_magic_frame_mac_set (a_uint32_t dev_id, fal_port_t port_id,
				fal_mac_addr_t * mac)
{
	sw_error_t rv;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	HSL_DEV_ID_CHECK (dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_magic_frame_mac_set)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_magic_frame_mac_set (dev_id, phy_id,mac);

	return rv;
}

static sw_error_t
_scomphy_port_magic_frame_mac_get (a_uint32_t dev_id, fal_port_t port_id,
				fal_mac_addr_t * mac)
{
	sw_error_t rv;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	HSL_DEV_ID_CHECK (dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_magic_frame_mac_get)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_magic_frame_mac_get (dev_id, phy_id,mac);

	return rv;
}

static sw_error_t
_scomphy_port_counter_set (a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
	sw_error_t rv;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	HSL_DEV_ID_CHECK (dev_id);
	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_counter_set)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_counter_set (dev_id, phy_id, enable);

	return rv;
}

static sw_error_t
_scomphy_port_counter_get (a_uint32_t dev_id, fal_port_t port_id,
		       a_bool_t * enable)
{
	sw_error_t rv;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	HSL_DEV_ID_CHECK (dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_counter_get)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_counter_get (dev_id, phy_id, enable);

	return rv;
}

static sw_error_t
_scomphy_port_counter_show (a_uint32_t dev_id, fal_port_t port_id,
				fal_port_counter_info_t * counter_info)
{
	sw_error_t rv;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	HSL_DEV_ID_CHECK (dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_counter_show)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_counter_show (dev_id, phy_id,counter_info);

	return rv;
}
#endif

static sw_error_t
_scomphy_port_link_status_get (a_uint32_t dev_id, fal_port_t port_id,
			    a_bool_t * status)
{
	sw_error_t rv;
	a_uint32_t phy_id;
	hsl_phy_ops_t *phy_drv;

	HSL_DEV_ID_CHECK (dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_link_status_get)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	if (A_TRUE == phy_drv->phy_link_status_get (dev_id, phy_id))
	{
		*status = A_TRUE;
	}
	else
	{
		*status = A_FALSE;
	}

	return SW_OK;
}

static sw_error_t
_scomphy_port_reset (a_uint32_t dev_id, fal_port_t port_id)
{
	sw_error_t rv;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	HSL_DEV_ID_CHECK (dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_reset)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_reset(dev_id, phy_id);

	return rv;
}

#ifndef IN_PORTCONTROL_MINI
static sw_error_t
_scomphy_port_phy_id_get (a_uint32_t dev_id, fal_port_t port_id,
				a_uint16_t * org_id, a_uint16_t * rev_id)
{
	sw_error_t rv;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;
	a_uint32_t phy_data;

	HSL_DEV_ID_CHECK (dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_id_get)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_id_get (dev_id, phy_id, &phy_data);
	SW_RTN_ON_ERROR (rv);

	*org_id = (phy_data >> 16) & 0xffff;
	*rev_id = phy_data & 0xffff;

	return rv;
}
#endif

/**
 * @brief Set duplex mode on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] duplex duplex mode
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
scomphy_port_duplex_set (a_uint32_t dev_id, fal_port_t port_id,
		      fal_port_duplex_t duplex)
{
	sw_error_t rv;

	HSL_API_LOCK;
	rv = _scomphy_port_duplex_set (dev_id, port_id, duplex);
	HSL_API_UNLOCK;
	return rv;
}

/**
 * @brief Get duplex mode on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] duplex duplex mode
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
scomphy_port_duplex_get (a_uint32_t dev_id, fal_port_t port_id,
		      fal_port_duplex_t * pduplex)
{
	sw_error_t rv;

	HSL_API_LOCK;
	rv = _scomphy_port_duplex_get (dev_id, port_id, pduplex);
	HSL_API_UNLOCK;
	return rv;
}

/**
 * @brief Set speed on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] speed port speed
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
scomphy_port_speed_set (a_uint32_t dev_id, fal_port_t port_id,
		     fal_port_speed_t speed)
{
	sw_error_t rv;

	HSL_API_LOCK;
	rv = _scomphy_port_speed_set (dev_id, port_id, speed);
	HSL_API_UNLOCK;
	return rv;
}

/**
 * @brief Get speed on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] speed port speed
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
scomphy_port_speed_get (a_uint32_t dev_id, fal_port_t port_id,
		     fal_port_speed_t * pspeed)
{
	sw_error_t rv;

	HSL_API_LOCK;
	rv = _scomphy_port_speed_get (dev_id, port_id, pspeed);
	HSL_API_UNLOCK;
	return rv;
}

/**
 * @brief Get auto negotiation status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] status A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
scomphy_port_autoneg_status_get (a_uint32_t dev_id, fal_port_t port_id,
			      a_bool_t * status)
{
	sw_error_t rv;

	HSL_API_LOCK;
	rv = _scomphy_port_autoneg_status_get (dev_id, port_id, status);
	HSL_API_UNLOCK;
	return rv;
}

/**
 * @brief Enable auto negotiation status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
scomphy_port_autoneg_enable (a_uint32_t dev_id, fal_port_t port_id)
{
	sw_error_t rv;

	HSL_API_LOCK;
	rv = _scomphy_port_autoneg_enable (dev_id, port_id);
	HSL_API_UNLOCK;
	return rv;
}

/**
 * @brief Restart auto negotiation procedule on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
scomphy_port_autoneg_restart (a_uint32_t dev_id, fal_port_t port_id)
{
	sw_error_t rv;

	HSL_API_LOCK;
	rv = _scomphy_port_autoneg_restart (dev_id, port_id);
	HSL_API_UNLOCK;
	return rv;
}

/**
 * @brief Set auto negotiation advtisement ability on a particular port.
 *   @details  Comments:
 *   auto negotiation advtisement ability is defined by macro such as
 *   FAL_PHY_ADV_10T_HD, FAL_PHY_ADV_PAUSE...
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] autoadv auto negotiation advtisement ability bit map
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
scomphy_port_autoneg_adv_set (a_uint32_t dev_id, fal_port_t port_id,
			   a_uint32_t autoadv)
{
	sw_error_t rv;

	HSL_API_LOCK;
	rv = _scomphy_port_autoneg_adv_set (dev_id, port_id, autoadv);
	HSL_API_UNLOCK;
	return rv;
}

/**
 * @brief Get auto negotiation advtisement ability on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] autoadv auto negotiation advtisement ability bit map
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
scomphy_port_autoneg_adv_get (a_uint32_t dev_id, fal_port_t port_id,
			   a_uint32_t * autoadv)
{
	sw_error_t rv;

	HSL_API_LOCK;
	rv = _scomphy_port_autoneg_adv_get (dev_id, port_id, autoadv);
	HSL_API_UNLOCK;
	return rv;
}

#ifndef IN_PORTCONTROL_MINI
/**
 * @brief Set powersaving status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
scomphy_port_powersave_set (a_uint32_t dev_id, fal_port_t port_id,
			 a_bool_t enable)
{
	sw_error_t rv;

	HSL_API_LOCK;
	rv = _scomphy_port_powersave_set (dev_id, port_id, enable);
	HSL_API_UNLOCK;
	return rv;
}

/**
 * @brief Get powersaving status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
scomphy_port_powersave_get (a_uint32_t dev_id, fal_port_t port_id,
			 a_bool_t * enable)
{
	sw_error_t rv;

	HSL_API_LOCK;
	rv = _scomphy_port_powersave_get (dev_id, port_id, enable);
	HSL_API_UNLOCK;
	return rv;
}

/**
 * @brief Set hibernate status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
scomphy_port_hibernate_set (a_uint32_t dev_id, fal_port_t port_id,
			 a_bool_t enable)
{
	sw_error_t rv;

	HSL_API_LOCK;
	rv = _scomphy_port_hibernate_set (dev_id, port_id, enable);
	HSL_API_UNLOCK;
	return rv;
}

/**
 * @brief Get hibernate status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
scomphy_port_hibernate_get (a_uint32_t dev_id, fal_port_t port_id,
			 a_bool_t * enable)
{
	sw_error_t rv;

	HSL_API_LOCK;
	rv = _scomphy_port_hibernate_get (dev_id, port_id, enable);
	HSL_API_UNLOCK;
	return rv;
}

/**
 * @brief Run cable diagnostic test on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] mdi_pair mdi pair id
 * @param[out] cable_status cable status
 * @param[out] cable_len cable len
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
scomphy_port_cdt (a_uint32_t dev_id, fal_port_t port_id, a_uint32_t mdi_pair,
	       fal_cable_status_t * cable_status, a_uint32_t * cable_len)
{
	sw_error_t rv;

	HSL_API_LOCK;
	rv = _scomphy_port_cdt (dev_id, port_id, mdi_pair, cable_status, cable_len);
	HSL_API_UNLOCK;
	return rv;
}
/**
 * @brief Set 802.3az status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
scomphy_port_8023az_set (a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
	sw_error_t rv;

	HSL_API_LOCK;
	rv = _scomphy_port_8023az_set (dev_id, port_id, enable);
	HSL_API_UNLOCK;
	return rv;
}

/**
 * @brief Get 8023az status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
scomphy_port_8023az_get (a_uint32_t dev_id, fal_port_t port_id,
		      a_bool_t * enable)
{
	sw_error_t rv;

	HSL_API_LOCK;
	rv = _scomphy_port_8023az_get (dev_id, port_id, enable);
	HSL_API_UNLOCK;
	return rv;
}

/**
 * @brief Set mdix on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] mode
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
scomphy_port_mdix_set (a_uint32_t dev_id, a_uint32_t phy_id,
		    fal_port_mdix_mode_t mode)
{
	sw_error_t rv;

	HSL_API_LOCK;
	rv = _scomphy_port_mdix_set (dev_id, phy_id, mode);
	HSL_API_UNLOCK;
	return rv;
}

/**
 * @brief Get mdix configuration on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] mode
 * @return SW_OK or error code
 */

HSL_LOCAL sw_error_t
scomphy_port_mdix_get (a_uint32_t dev_id, fal_port_t port_id,
		    fal_port_mdix_mode_t * mode)
{
	sw_error_t rv;

	HSL_API_LOCK;
	rv = _scomphy_port_mdix_get (dev_id, port_id, mode);
	HSL_API_UNLOCK;
	return rv;
}

/**
 * @brief Get mdix status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] mode
 * @return SW_OK or error code
 */

HSL_LOCAL sw_error_t
scomphy_port_mdix_status_get (a_uint32_t dev_id, fal_port_t port_id,
			   fal_port_mdix_status_t * mode)
{
	sw_error_t rv;

	HSL_API_LOCK;
	rv = _scomphy_port_mdix_status_get (dev_id, port_id, mode);
	HSL_API_UNLOCK;
	return rv;
}
/*qca808x_end*/
/**
 * @brief Set combo prefer medium on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] phy_medium [fiber or copper]
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
scomphy_port_combo_prefer_medium_set (a_uint32_t dev_id, a_uint32_t phy_id,
				   fal_port_medium_t phy_medium)
{
	sw_error_t rv;

	HSL_API_LOCK;
	rv = _scomphy_port_combo_prefer_medium_set (dev_id, phy_id, phy_medium);
	HSL_API_UNLOCK;
	return rv;
}

/**
 * @brief Get combo prefer medium on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] phy_medium [fiber or copper]
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
scomphy_port_combo_prefer_medium_get (a_uint32_t dev_id, a_uint32_t phy_id,
				   fal_port_medium_t * phy_medium)
{
	sw_error_t rv;

	HSL_API_LOCK;
	rv = _scomphy_port_combo_prefer_medium_get (dev_id, phy_id, phy_medium);
	HSL_API_UNLOCK;
	return rv;
}

/**
 * @brief Get current combo medium status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] phy_medium [fiber or copper]
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
scomphy_port_combo_medium_status_get (a_uint32_t dev_id, a_uint32_t phy_id,
				   fal_port_medium_t * phy_medium)
{
	sw_error_t rv;

	HSL_API_LOCK;
	rv = _scomphy_port_combo_medium_status_get (dev_id, phy_id, phy_medium);
	HSL_API_UNLOCK;
	return rv;
}

/**
 * @brief Set fiber mode on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] fiber mode [1000bx or 100fx]
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
scomphy_port_combo_fiber_mode_set (a_uint32_t dev_id, a_uint32_t phy_id,
				fal_port_fiber_mode_t fiber_mode)
{
	sw_error_t rv;

	HSL_API_LOCK;
	rv = _scomphy_port_combo_fiber_mode_set (dev_id, phy_id, fiber_mode);
	HSL_API_UNLOCK;
	return rv;
}

/**
 * @brief Get fiber mode on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] fiber mode [1000bx or 100fx]
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
scomphy_port_combo_fiber_mode_get (a_uint32_t dev_id, a_uint32_t phy_id,
				fal_port_fiber_mode_t * fiber_mode)
{
	sw_error_t rv;

	HSL_API_LOCK;
	rv = _scomphy_port_combo_fiber_mode_get (dev_id, phy_id, fiber_mode);
	HSL_API_UNLOCK;
	return rv;
}
/*qca808x_start*/
/**
 * @brief Set phy local loop back on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
scomphy_port_local_loopback_set (a_uint32_t dev_id, fal_port_t port_id,
			      a_bool_t enable)
{
	sw_error_t rv;
	HSL_API_LOCK;
	rv = _scomphy_port_local_loopback_set (dev_id, port_id, enable);
	HSL_API_UNLOCK;
	return rv;
}

/**
 * @brief Get phy local loop back on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
scomphy_port_local_loopback_get (a_uint32_t dev_id, fal_port_t port_id,
			      a_bool_t * enable)
{
	sw_error_t rv;

	HSL_API_LOCK;
	rv = _scomphy_port_local_loopback_get (dev_id, port_id, enable);
	HSL_API_UNLOCK;
	return rv;
}

/**
 * @brief Set phy remote loop back on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
scomphy_port_remote_loopback_set (a_uint32_t dev_id, fal_port_t port_id,
			       a_bool_t enable)
{
	sw_error_t rv;
	HSL_API_LOCK;
	rv = _scomphy_port_remote_loopback_set (dev_id, port_id, enable);
	HSL_API_UNLOCK;
	return rv;
}

/**
 * @brief Get phy remote loop back on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
scomphy_port_remote_loopback_get (a_uint32_t dev_id, fal_port_t port_id,
			       a_bool_t * enable)
{
	sw_error_t rv;

	HSL_API_LOCK;
	rv = _scomphy_port_remote_loopback_get (dev_id, port_id, enable);
	HSL_API_UNLOCK;
	return rv;
}
#endif

/**
 * @brief phy power off on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in]
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
scomphy_port_power_off (a_uint32_t dev_id, fal_port_t port_id)
{
	sw_error_t rv;
	HSL_API_LOCK;
	rv = _scomphy_port_power_off (dev_id, port_id);
	HSL_API_UNLOCK;
	return rv;
}

/**
 * @brief phy power on on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in]
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
scomphy_port_power_on (a_uint32_t dev_id, fal_port_t port_id)
{
	sw_error_t rv;
	HSL_API_LOCK;
	rv = _scomphy_port_power_on (dev_id, port_id);
	HSL_API_UNLOCK;
	return rv;
}

#ifndef IN_PORTCONTROL_MINI
/**
 * @brief Set phy wol enable on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
scomphy_port_wol_status_set (a_uint32_t dev_id, fal_port_t port_id,
			       a_bool_t enable)
{
	sw_error_t rv;
	HSL_API_LOCK;
	rv = _scomphy_port_wol_status_set (dev_id, port_id, enable);
	HSL_API_UNLOCK;
	return rv;
}

/**
 * @brief Get phy wol status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
scomphy_port_wol_status_get (a_uint32_t dev_id, fal_port_t port_id,
			       a_bool_t * enable)
{
	sw_error_t rv;

	HSL_API_LOCK;
	rv = _scomphy_port_wol_status_get (dev_id, port_id, enable);
	HSL_API_UNLOCK;
	return rv;
}

/**
 * @brief Set phy magic frame mac on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] mac address
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
scomphy_port_magic_frame_mac_set (a_uint32_t dev_id, fal_port_t port_id,
			      fal_mac_addr_t * mac)
{
	sw_error_t rv;
	HSL_API_LOCK;
	rv = _scomphy_port_magic_frame_mac_set (dev_id, port_id, mac);
	HSL_API_UNLOCK;
	return rv;
}

/**
 * @brief Get phy magic frame mac on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] mac address
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
scomphy_port_magic_frame_mac_get (a_uint32_t dev_id, fal_port_t port_id,
			      fal_mac_addr_t * mac)
{
	sw_error_t rv;
	HSL_API_LOCK;
	rv = _scomphy_port_magic_frame_mac_get (dev_id, port_id, mac);
	HSL_API_UNLOCK;
	return rv;
}
/*qca808x_end*/
/**
 * @brief Set phy interface mode.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] interface mode..
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
scomphy_port_interface_mode_set (a_uint32_t dev_id, fal_port_t port_id,
			      fal_port_interface_mode_t  mode)
{
	sw_error_t rv;
	HSL_API_LOCK;
	rv = _scomphy_port_interface_mode_set (dev_id, port_id, mode);
	HSL_API_UNLOCK;
	return rv;
}

/**
 * @brief Set phy interface mode.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] interface mode..
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
scomphy_port_interface_mode_get (a_uint32_t dev_id, fal_port_t port_id,
			      fal_port_interface_mode_t  * mode)
{
	sw_error_t rv;
	HSL_API_LOCK;
	rv = _scomphy_port_interface_mode_get (dev_id, port_id, mode);
	HSL_API_UNLOCK;
	return rv;
}
/*qca808x_start*/
/**
 * @brief Set phy interface mode status.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] interface mode..
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
scomphy_port_interface_mode_status_get (a_uint32_t dev_id, fal_port_t port_id,
			      fal_port_interface_mode_t  * mode)
{
	sw_error_t rv;
	HSL_API_LOCK;
	rv = _scomphy_port_interface_mode_status_get (dev_id, port_id, mode);
	HSL_API_UNLOCK;
	return rv;
}
#endif

/**
 * @brief Get link status on particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] status link status up (A_TRUE) or down (A_FALSE)
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
scomphy_port_link_status_get (a_uint32_t dev_id, fal_port_t port_id,
			   a_bool_t * status)
{
  sw_error_t rv;

  HSL_API_LOCK;
  rv = _scomphy_port_link_status_get (dev_id, port_id, status);
  HSL_API_UNLOCK;
  return rv;
}

#ifndef IN_PORTCONTROL_MINI
/**
 * @brief Set counter status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
scomphy_port_counter_set (a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
	sw_error_t rv;

	HSL_API_LOCK;
	rv = _scomphy_port_counter_set (dev_id, port_id, enable);
	HSL_API_UNLOCK;
	return rv;
}

/**
 * @brief Get counter status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
scomphy_port_counter_get (a_uint32_t dev_id, fal_port_t port_id,
		      a_bool_t * enable)
{
	sw_error_t rv;

	HSL_API_LOCK;
	rv = _scomphy_port_counter_get (dev_id, port_id, enable);
	HSL_API_UNLOCK;
	return rv;
}

/**
 * @brief Get counter statistics  on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] frame counter statistics
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
scomphy_port_counter_show(a_uint32_t dev_id, fal_port_t port_id,
		      fal_port_counter_info_t * counter_info)
{
	sw_error_t rv;

	HSL_API_LOCK;
	rv = _scomphy_port_counter_show (dev_id, port_id, counter_info);
	HSL_API_UNLOCK;
	return rv;
}
#endif

/**
 * @brief software reset on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in]
 * @return SW_OK or error code
 */
sw_error_t
scomphy_port_reset (a_uint32_t dev_id, fal_port_t port_id)
{
	sw_error_t rv;
	HSL_API_LOCK;
	rv = _scomphy_port_reset (dev_id, port_id);
	HSL_API_UNLOCK;
	return rv;
}

#ifndef IN_PORTCONTROL_MINI
/**
 * @brief Get phy id on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] org_id and rev_id
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
scomphy_port_phy_id_get (a_uint32_t dev_id, fal_port_t port_id,
			       a_uint16_t * org_id, a_uint16_t * rev_id)
{
	sw_error_t rv;

	HSL_API_LOCK;
	rv = _scomphy_port_phy_id_get (dev_id, port_id, org_id,rev_id);
	HSL_API_UNLOCK;
	return rv;
}
#endif

sw_error_t
scomphy_port_ctrl_init(a_uint32_t dev_id)
{
	HSL_DEV_ID_CHECK(dev_id);

#ifndef HSL_STANDALONG
    {
        hsl_api_t *p_api;

        SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

	p_api->port_duplex_get = scomphy_port_duplex_get;
	p_api->port_duplex_set = scomphy_port_duplex_set;
	p_api->port_speed_get = scomphy_port_speed_get;
	p_api->port_speed_set = scomphy_port_speed_set;
	p_api->port_autoneg_status_get = scomphy_port_autoneg_status_get;
	p_api->port_autoneg_enable = scomphy_port_autoneg_enable;
	p_api->port_autoneg_restart = scomphy_port_autoneg_restart;
	p_api->port_autoneg_adv_get = scomphy_port_autoneg_adv_get;
	p_api->port_autoneg_adv_set = scomphy_port_autoneg_adv_set;
#ifndef IN_PORTCONTROL_MINI
	p_api->port_powersave_set = scomphy_port_powersave_set;
	p_api->port_powersave_get = scomphy_port_powersave_get;
	p_api->port_hibernate_set = scomphy_port_hibernate_set;
	p_api->port_hibernate_get = scomphy_port_hibernate_get;
	p_api->port_cdt = scomphy_port_cdt;
	p_api->port_8023az_set = scomphy_port_8023az_set;
	p_api->port_8023az_get = scomphy_port_8023az_get;
	p_api->port_mdix_set = scomphy_port_mdix_set;
	p_api->port_mdix_get = scomphy_port_mdix_get;
	p_api->port_mdix_status_get = scomphy_port_mdix_status_get;
/*qca808x_end*/
	p_api->port_combo_prefer_medium_set = scomphy_port_combo_prefer_medium_set;
	p_api->port_combo_prefer_medium_get = scomphy_port_combo_prefer_medium_get;
	p_api->port_combo_medium_status_get = scomphy_port_combo_medium_status_get;
	p_api->port_combo_fiber_mode_set = scomphy_port_combo_fiber_mode_set;
	p_api->port_combo_fiber_mode_get = scomphy_port_combo_fiber_mode_get;
/*qca808x_start*/
	p_api->port_local_loopback_set = scomphy_port_local_loopback_set;
	p_api->port_local_loopback_get = scomphy_port_local_loopback_get;
	p_api->port_remote_loopback_set = scomphy_port_remote_loopback_set;
	p_api->port_remote_loopback_get = scomphy_port_remote_loopback_get;
#endif
	p_api->port_power_off = scomphy_port_power_off;
	p_api->port_power_on = scomphy_port_power_on;
#ifndef IN_PORTCONTROL_MINI
	p_api->port_wol_status_set = scomphy_port_wol_status_set;
	p_api->port_wol_status_get = scomphy_port_wol_status_get;
	p_api->port_magic_frame_mac_set = scomphy_port_magic_frame_mac_set;
	p_api->port_magic_frame_mac_get = scomphy_port_magic_frame_mac_get;
/*qca808x_end*/
	p_api->port_interface_mode_set = scomphy_port_interface_mode_set;
	p_api->port_interface_mode_get = scomphy_port_interface_mode_get;
/*qca808x_start*/
	p_api->port_interface_mode_status_get = scomphy_port_interface_mode_status_get;
	p_api->port_counter_set = scomphy_port_counter_set;
	p_api->port_counter_get = scomphy_port_counter_get;
	p_api->port_counter_show = scomphy_port_counter_show;
#endif
	p_api->port_link_status_get = scomphy_port_link_status_get;
	p_api->port_reset = scomphy_port_reset;
#ifndef IN_PORTCONTROL_MINI
	p_api->port_phy_id_get = scomphy_port_phy_id_get;
#endif
    }
#endif

    return SW_OK;
}

/**
 * @}
 */
/*qca808x_end*/
