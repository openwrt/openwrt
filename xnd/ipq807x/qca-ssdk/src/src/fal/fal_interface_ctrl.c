/*
 * Copyright (c) 2012, The Linux Foundation. All rights reserved.
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
 * @defgroup fal_interface_ctrl FAL_INTERFACE_CONTROL
 * @{
 */
#include "sw.h"
#include "fal_interface_ctrl.h"
#include "hsl_api.h"

static sw_error_t
_fal_port_3az_status_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->port_3az_status_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->port_3az_status_set(dev_id, port_id, enable);
    return rv;
}

static sw_error_t
_fal_port_3az_status_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->port_3az_status_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->port_3az_status_get(dev_id, port_id, enable);
    return rv;
}

static sw_error_t
_fal_interface_mac_mode_set(a_uint32_t dev_id, fal_port_t port_id, fal_mac_config_t * config)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->interface_mac_mode_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->interface_mac_mode_set(dev_id, port_id, config);
    return rv;
}

static sw_error_t
_fal_interface_mac_mode_get(a_uint32_t dev_id, fal_port_t port_id, fal_mac_config_t * config)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->interface_mac_mode_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->interface_mac_mode_get(dev_id, port_id, config);
    return rv;
}

static sw_error_t
_fal_interface_phy_mode_set(a_uint32_t dev_id, a_uint32_t phy_id, fal_phy_config_t * config)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->interface_phy_mode_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->interface_phy_mode_set(dev_id, phy_id, config);
    return rv;
}

static sw_error_t
_fal_interface_phy_mode_get(a_uint32_t dev_id, a_uint32_t phy_id, fal_phy_config_t * config)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->interface_phy_mode_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->interface_phy_mode_get(dev_id, phy_id, config);
    return rv;
}

static sw_error_t
_fal_interface_fx100_ctrl_set(a_uint32_t dev_id, fal_fx100_ctrl_config_t * config)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->interface_fx100_ctrl_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->interface_fx100_ctrl_set(dev_id, config);
    return rv;
}

static sw_error_t
_fal_interface_fx100_ctrl_get(a_uint32_t dev_id, fal_fx100_ctrl_config_t * config)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->interface_fx100_ctrl_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->interface_fx100_ctrl_get(dev_id, config);
    return rv;
}

static sw_error_t
_fal_interface_fx100_status_get(a_uint32_t dev_id, a_uint32_t* status)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->interface_fx100_status_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->interface_fx100_status_get(dev_id, status);
    return rv;
}

static sw_error_t
_fal_interface_mac06_exch_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->interface_mac06_exch_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->interface_mac06_exch_set(dev_id, enable);
    return rv;
}

static sw_error_t
_fal_interface_mac06_exch_get(a_uint32_t dev_id, a_bool_t* enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->interface_mac06_exch_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->interface_mac06_exch_get(dev_id, enable);
    return rv;
}

/**
  * @brief Set 802.3az status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_3az_status_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_port_3az_status_set(dev_id, port_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
  * @brief Get 802.3az status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_3az_status_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_port_3az_status_get(dev_id, port_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
  * @brief Set interface mode on a particular MAC device.
 * @param[in] dev_id device id
 * @param[in] mca_id MAC device ID
 * @param[in] config interface configuration
 * @return SW_OK or error code
 */
sw_error_t
fal_interface_mac_mode_set(a_uint32_t dev_id, fal_port_t port_id, fal_mac_config_t * config)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_interface_mac_mode_set(dev_id, port_id, config);
    FAL_API_UNLOCK;
    return rv;
}

/**
  * @brief Get interface mode on a particular MAC device.
 * @param[in] dev_id device id
 * @param[in] mca_id MAC device ID
 * @param[out] config interface configuration
 * @return SW_OK or error code
 */
sw_error_t
fal_interface_mac_mode_get(a_uint32_t dev_id, fal_port_t port_id, fal_mac_config_t * config)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_interface_mac_mode_get(dev_id, port_id, config);
    FAL_API_UNLOCK;
    return rv;
}

/**
  * @brief Set interface phy mode on a particular PHY device.
 * @param[in] dev_id device id
 * @param[in] phy_id PHY device ID
 * @param[in] config interface configuration
 * @return SW_OK or error code
 */
sw_error_t
fal_interface_phy_mode_set(a_uint32_t dev_id, a_uint32_t phy_id, fal_phy_config_t * config)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_interface_phy_mode_set(dev_id, phy_id, config);
    FAL_API_UNLOCK;
    return rv;
}

/**
  * @brief Get interface phy mode on a particular PHY device.
 * @param[in] dev_id device id
 * @param[in] phy_id PHY device ID
 * @param[out] config interface configuration
 * @return SW_OK or error code
 */
sw_error_t
fal_interface_phy_mode_get(a_uint32_t dev_id, a_uint32_t phy_id, fal_phy_config_t * config)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_interface_phy_mode_get(dev_id, phy_id, config);
    FAL_API_UNLOCK;
    return rv;
}

/**
  * @brief Set fx100 control configuration.
 * @param[in] dev_id device id
 * @param[in] config fx100 control configuration
 * @return SW_OK or error code
 */
sw_error_t
fal_interface_fx100_ctrl_set(a_uint32_t dev_id,  fal_fx100_ctrl_config_t * config)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_interface_fx100_ctrl_set(dev_id, config);
    FAL_API_UNLOCK;
    return rv;
}


/**
  * @brief Get fx100 control configuration.
 * @param[in] dev_id device id
 * @param[out] config fx100 control configuration
 * @return SW_OK or error code
 */
sw_error_t
fal_interface_fx100_ctrl_get(a_uint32_t dev_id,  fal_fx100_ctrl_config_t * config)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_interface_fx100_ctrl_get(dev_id, config);
    FAL_API_UNLOCK;
    return rv;
}

/**
  * @brief Get fx100 control configuration.
 * @param[in] dev_id device id
 * @param[out] fx100 status
 * @return SW_OK or error code
 */
sw_error_t
fal_interface_fx100_status_get(a_uint32_t dev_id,  a_uint32_t* status)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_interface_fx100_status_get(dev_id, status);
    FAL_API_UNLOCK;
    return rv;
}


/**
  * @brief Set mac0 and mac6 exchange status.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_interface_mac06_exch_set(a_uint32_t dev_id,  a_bool_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_interface_mac06_exch_set(dev_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
  * @brief Get mac0 and mac6 exchange status.
 * @param[in] dev_id device id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_interface_mac06_exch_get(a_uint32_t dev_id,  a_bool_t* enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_interface_mac06_exch_get(dev_id, enable);
    FAL_API_UNLOCK;
    return rv;
}
/**
 * @}
 */

