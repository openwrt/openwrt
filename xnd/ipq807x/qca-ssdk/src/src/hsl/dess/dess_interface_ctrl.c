/*
 * Copyright (c) 2014, 2016, The Linux Foundation. All rights reserved.
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
 * @defgroup dess_interface_ctrl DESS_INTERFACE_CONTROL
 * @{
 */
#include "sw.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_port_prop.h"
#include "dess_interface_ctrl.h"
#include "dess_reg.h"


#define DESS_PHY_MODE_PHY_ID  4
#define DESS_LPI_PORT1_OFFSET 4
#define DESS_LPI_BIT_STEP     2
#define DESS_LPI_ENABLE     3

#define DESS_MAC4  4
#define DESS_MAC5  5

static sw_error_t
_dess_port_3az_status_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
    sw_error_t rv = SW_OK;
    a_uint32_t reg = 0, field, offset, device_id;

    HSL_REG_ENTRY_GET(rv, dev_id, MASK_CTL, 0,
                      (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    SW_GET_FIELD_BY_REG(MASK_CTL, DEVICE_ID, device_id, reg);
    if (DESS_DEVICE_ID != device_id)
    {
        return SW_NOT_SUPPORTED;
    }

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_PHY))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_ENTRY_GET(rv, dev_id, EEE_CTL, 0,
                      (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (A_TRUE == enable)
    {
        field  = DESS_LPI_ENABLE;
    }
    else if (A_FALSE == enable)
    {
        field  = 0;
    }
    else
    {
        return SW_BAD_PARAM;
    }

    offset = (port_id - 1) * DESS_LPI_BIT_STEP + DESS_LPI_PORT1_OFFSET;
    reg &= (~(DESS_LPI_ENABLE << offset));
    reg |= (field << offset);

    HSL_REG_ENTRY_SET(rv, dev_id, EEE_CTL, 0,
                      (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_dess_port_3az_status_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable)
{
    sw_error_t rv = SW_OK;
    a_uint32_t reg = 0, field, offset, device_id;

    HSL_REG_ENTRY_GET(rv, dev_id, MASK_CTL, 0,
                      (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    SW_GET_FIELD_BY_REG(MASK_CTL, DEVICE_ID, device_id, reg);
    if (DESS_DEVICE_ID != device_id)
    {
        return SW_NOT_SUPPORTED;
    }

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_PHY))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_ENTRY_GET(rv, dev_id, EEE_CTL, 0,
                      (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    offset = (port_id - 1) * DESS_LPI_BIT_STEP + DESS_LPI_PORT1_OFFSET;
    field = (reg >> offset) & 0x3;

    if (field == DESS_LPI_ENABLE)
    {
        *enable = A_TRUE;
    }
    else
    {
        *enable = A_FALSE;
    }

    return SW_OK;
}

static sw_error_t
_dess_interface_mac_mode_set(a_uint32_t dev_id, fal_port_t port_id, fal_mac_config_t * config)
{
	sw_error_t rv = SW_OK;
	a_uint32_t reg = 0, field;

	HSL_DEV_ID_CHECK(dev_id);


	if (FAL_MAC_MODE_RMII == config->mac_mode)
	{
		HSL_REG_ENTRY_GET(rv, dev_id, RGMII_CTRL, 0,
						  (a_uint8_t *) (&reg), sizeof (a_uint32_t));
		if (port_id == DESS_MAC4) {
			SW_GET_FIELD_BY_REG(RGMII_CTRL, RMII1_MASTER_EN, field, reg);
			if(field == config->config.rmii.master_mode)
				return rv;
			SW_SET_REG_BY_FIELD(RGMII_CTRL, RMII1_MASTER_EN, config->config.rmii.master_mode, reg);
		}
		else if (port_id == DESS_MAC5) {
			SW_GET_FIELD_BY_REG(RGMII_CTRL, RMII0_MASTER_EN, field, reg);
			if(field == config->config.rmii.master_mode)
				return rv;
			SW_SET_REG_BY_FIELD(RGMII_CTRL, RMII1_MASTER_EN, config->config.rmii.master_mode, reg);
		}
		else
		{
			return SW_BAD_PARAM;
		}
		HSL_REG_ENTRY_SET(rv, dev_id, RGMII_CTRL, 0,
                          (a_uint8_t *) (&reg), sizeof (a_uint32_t));
	}
	else
	{
		return SW_BAD_PARAM;
	}

	return rv;
}

static sw_error_t
_dess_interface_mac_mode_get(a_uint32_t dev_id, fal_port_t port_id, fal_mac_config_t * config)
{
	sw_error_t rv = SW_OK;
	a_uint32_t reg = 0, field;

	HSL_DEV_ID_CHECK(dev_id);


	if (FAL_MAC_MODE_RMII == config->mac_mode)
	{
		HSL_REG_ENTRY_GET(rv, dev_id, RGMII_CTRL, 0,
						  (a_uint8_t *) (&reg), sizeof (a_uint32_t));
		if (port_id == DESS_MAC4) {
			SW_GET_FIELD_BY_REG(RGMII_CTRL, RMII1_MASTER_EN, field, reg);
			config->config.rmii.master_mode = field;
		}
		else if (port_id == DESS_MAC5) {
			SW_GET_FIELD_BY_REG(RGMII_CTRL, RMII0_MASTER_EN, field, reg);
			config->config.rmii.master_mode = field;
		}
		else
		{
			return SW_BAD_PARAM;
		}
	}
	else
	{
		return SW_BAD_PARAM;
	}

	return rv;
}


/**
  * @brief Set 802.3az status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_3az_status_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_port_3az_status_set(dev_id, port_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
  * @brief Get 802.3az status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_port_3az_status_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_port_3az_status_get(dev_id, port_id, enable);
    HSL_API_UNLOCK;
    return rv;
}


/**
  * @brief Set interface mode on RGMII/RMII.
 * @param[in] dev_id device id
 * @param[in] delay
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_interface_mac_mode_set(a_uint32_t dev_id, fal_port_t port_id, fal_mac_config_t * config)
{
	sw_error_t rv;

	HSL_API_LOCK;
	rv = _dess_interface_mac_mode_set(dev_id, port_id, config);
	HSL_API_UNLOCK;
	return rv;
}


/**
  * @brief Get interface mode on RGMII/RMII.
 * @param[in] dev_id device id
 * @param[in] mca_id MAC device ID
 * @param[out] config interface configuration
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_interface_mac_mode_get(a_uint32_t dev_id, fal_port_t port_id, fal_mac_config_t * config)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_interface_mac_mode_get(dev_id, port_id, config);
    HSL_API_UNLOCK;
    return rv;
}




sw_error_t
dess_interface_ctrl_init(a_uint32_t dev_id)
{
    HSL_DEV_ID_CHECK(dev_id);

#ifndef HSL_STANDALONG
    {
        hsl_api_t *p_api;

        SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

        p_api->port_3az_status_set = dess_port_3az_status_set;
        p_api->port_3az_status_get = dess_port_3az_status_get;
		p_api->interface_mac_mode_set = dess_interface_mac_mode_set;
        p_api->interface_mac_mode_get = dess_interface_mac_mode_get;

    }
#endif

    return SW_OK;
}

/**
 * @}
 */

