/*
 * Copyright (c) 2020, The Linux Foundation. All rights reserved.
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

#include "sw.h"
#include "adpt.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_phy.h"
#include "hsl_port_prop.h"
#include "adpt_mp.h"

static sw_error_t
adpt_mp_intr_port_link_mask_set(a_uint32_t dev_id, a_uint32_t port_id,
	a_uint32_t intr_mask_flag)
{
	sw_error_t rv;
	a_uint32_t phy_id;
	hsl_phy_ops_t *phy_drv;

	HSL_DEV_ID_CHECK(dev_id);

	if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	SW_RTN_ON_NULL (phy_drv->phy_intr_mask_set);

	rv = hsl_port_prop_get_phyid(dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR(rv);

	rv = phy_drv->phy_intr_mask_set(dev_id, phy_id, intr_mask_flag);

	return rv;
}

static sw_error_t
adpt_mp_intr_port_link_mask_get(a_uint32_t dev_id, a_uint32_t port_id,
	a_uint32_t * intr_mask_flag)
{
	sw_error_t rv;
	a_uint32_t phy_id;
	hsl_phy_ops_t *phy_drv;

	HSL_DEV_ID_CHECK(dev_id);

	if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	SW_RTN_ON_NULL (phy_drv->phy_intr_mask_get);

	rv = hsl_port_prop_get_phyid(dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR(rv);

	rv = phy_drv->phy_intr_mask_get(dev_id, phy_id, intr_mask_flag);

	return rv;
}

static sw_error_t
adpt_mp_intr_port_link_status_get(a_uint32_t dev_id, a_uint32_t port_id,
	a_uint32_t * intr_status)
{
	sw_error_t rv;
	a_uint32_t phy_id;
	hsl_phy_ops_t *phy_drv;

	HSL_DEV_ID_CHECK(dev_id);

	if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	SW_RTN_ON_NULL (phy_drv->phy_intr_status_get);

	rv = hsl_port_prop_get_phyid(dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR(rv);

	rv = phy_drv->phy_intr_status_get(dev_id, phy_id, intr_status);

	return rv;
}

sw_error_t adpt_mp_intr_init(a_uint32_t dev_id)
{
	adpt_api_t *p_adpt_api = NULL;

	p_adpt_api = adpt_api_ptr_get(dev_id);

	SW_RTN_ON_NULL (p_adpt_api);

	p_adpt_api->adpt_intr_port_link_mask_set = adpt_mp_intr_port_link_mask_set;
	p_adpt_api->adpt_intr_port_link_mask_get = adpt_mp_intr_port_link_mask_get;
	p_adpt_api->adpt_intr_port_link_status_get = adpt_mp_intr_port_link_status_get;

	return SW_OK;
}
/**
 * @}
 */
