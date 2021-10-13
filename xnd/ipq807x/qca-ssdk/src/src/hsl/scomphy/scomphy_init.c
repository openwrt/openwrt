/*
 * Copyright (c) 2017-2019, The Linux Foundation. All rights reserved.
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
 * @defgroup scomphy_init SCOMPHY_INIT
 * @{
 */
#include "sw.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_phy.h"
#include "hsl_port_prop.h"
#include "scomphy_port_ctrl.h"
#include "scomphy_reg_access.h"
#include "scomphy_init.h"

static ssdk_init_cfg * scomphy_cfg[SW_MAX_NR_DEV] = { 0 };

static sw_error_t
scomphy_portproperty_init(a_uint32_t dev_id, hsl_init_mode mode)
{
	hsl_port_prop_t p_type;
	hsl_dev_t *pdev = NULL;
	fal_port_t port_id;
	a_uint32_t port_bmp = qca_ssdk_port_bmp_get(dev_id);

	pdev = hsl_dev_ptr_get(dev_id);
	if (pdev == NULL)
		return SW_NOT_INITIALIZED;

	/* for port property set, SSDK should not generate some limitations */
	for (port_id = 1; port_id < SW_MAX_NR_PORT; port_id++)
	{
		if((1 << port_id) & port_bmp)
		{
			hsl_port_prop_portmap_set(dev_id, port_id);

			for (p_type = HSL_PP_PHY; p_type < HSL_PP_BUTT; p_type++)
			{
				SW_RTN_ON_ERROR(hsl_port_prop_set(dev_id, port_id, p_type));
			}
		}
	}

	return SW_OK;
}

static sw_error_t
scomphy_dev_init(a_uint32_t dev_id, hsl_init_mode cpu_mode)
{
	sw_error_t rv = SW_OK;
	hsl_dev_t *pdev = NULL;
	a_uint8_t nr_ports = 0;
	a_uint32_t port_bmp = qca_ssdk_port_bmp_get(dev_id);

	pdev = hsl_dev_ptr_get(dev_id);
	if (pdev == NULL)
		return SW_NOT_INITIALIZED;

	while (port_bmp) {
		if (port_bmp & 1)
			nr_ports++;
		port_bmp >>= 1;
	}

	pdev->nr_ports = nr_ports;
	pdev->nr_phy = nr_ports;
	pdev->cpu_port_nr = 0;
	pdev->hw_vlan_query = A_TRUE;
	pdev->cpu_mode = cpu_mode;

	return rv;
}


static sw_error_t
_scomphy_reset(a_uint32_t dev_id)
{
	sw_error_t rv = SW_OK;
	a_uint32_t port_bmp = 0;
	a_uint8_t port_id = 0;

	HSL_DEV_ID_CHECK(dev_id);

	port_bmp = qca_ssdk_port_bmp_get(dev_id);
	while (port_bmp) {
		if (port_bmp & 1)
			SW_RTN_ON_ERROR(scomphy_port_reset(dev_id, port_id));

		port_bmp >>= 1;
		port_id++;
	}

	return rv;
}

sw_error_t
scomphy_cleanup(a_uint32_t dev_id)
{
	sw_error_t rv = SW_OK;

	if (scomphy_cfg[dev_id])
	{
		SW_RTN_ON_ERROR(hsl_port_prop_cleanup_by_dev(dev_id));

		aos_mem_free(scomphy_cfg[dev_id]);
		scomphy_cfg[dev_id] = NULL;
	}

	return rv;
}

/**
 * @brief reset hsl layer.
 * @details Comments:
 *   This operation will reset hsl layer
 * @param[in] dev_id device id
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
scomphy_reset(a_uint32_t dev_id)
{
	sw_error_t rv;

	HSL_API_LOCK;
	rv = _scomphy_reset(dev_id);
	HSL_API_UNLOCK;
	return rv;
}

/**
 * @brief Init hsl layer.
 * @details Comments:
 *   This operation will init hsl layer and hsl layer
 * @param[in] dev_id device id
 * @param[in] cfg configuration for initialization
 * @return SW_OK or error code
 */
sw_error_t
scomphy_init(a_uint32_t dev_id, ssdk_init_cfg *cfg)
{
	HSL_DEV_ID_CHECK(dev_id);

	if (NULL == scomphy_cfg[dev_id])
	{
		scomphy_cfg[dev_id] = aos_mem_alloc(sizeof (ssdk_init_cfg));
	}

	if (NULL == scomphy_cfg[dev_id])
	{
		return SW_OUT_OF_MEM;
	}

	aos_mem_copy(scomphy_cfg[dev_id], cfg, sizeof (ssdk_init_cfg));

	SW_RTN_ON_ERROR(scomphy_reg_access_init(dev_id, cfg));

	SW_RTN_ON_ERROR(scomphy_dev_init(dev_id, cfg->cpu_mode));

	{
		sw_error_t rv;

		SW_RTN_ON_ERROR(hsl_port_prop_init(dev_id));
		SW_RTN_ON_ERROR(hsl_port_prop_init_by_dev(dev_id));
		SW_RTN_ON_ERROR(scomphy_portproperty_init(dev_id, cfg->cpu_mode));

		SCOMPHY_PORT_CTRL_INIT(rv, dev_id);

		{
			hsl_api_t *p_api;

			SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));
			p_api->dev_reset   = scomphy_reset;
			p_api->dev_clean   = scomphy_cleanup;
		}
	}

	return SW_OK;
}

/**
 * @}
 */

