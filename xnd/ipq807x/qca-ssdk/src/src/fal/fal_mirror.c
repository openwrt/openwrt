/*
 * Copyright (c) 2012, 2016-2017, The Linux Foundation. All rights reserved.
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
 * @defgroup fal_mirror FAL_MIRROR
 * @{
 */
#include "sw.h"
#include "fal_mirror.h"
#include "hsl_api.h"
#include "adpt.h"

#include <linux/kernel.h>
#include <linux/module.h>



static sw_error_t
_fal_mirr_analysis_port_set(a_uint32_t dev_id, fal_port_t port_id)
{
    sw_error_t rv;
    hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
        if (NULL == p_adpt_api->adpt_mirr_analysis_port_set)
            return SW_NOT_SUPPORTED;

        rv = p_adpt_api->adpt_mirr_analysis_port_set(dev_id, port_id);
        return rv;
    }

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->mirr_analysis_port_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->mirr_analysis_port_set(dev_id, port_id);
    return rv;
}

static sw_error_t
_fal_mirr_analysis_port_get(a_uint32_t dev_id, fal_port_t * port_id)
{
    sw_error_t rv;
    hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
        if (NULL == p_adpt_api->adpt_mirr_analysis_port_get)
            return SW_NOT_SUPPORTED;

        rv = p_adpt_api->adpt_mirr_analysis_port_get(dev_id, port_id);
        return rv;
    }

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->mirr_analysis_port_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->mirr_analysis_port_get(dev_id, port_id);
    return rv;
}

static sw_error_t
_fal_mirr_port_in_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
        if (NULL == p_adpt_api->adpt_mirr_port_in_set)
            return SW_NOT_SUPPORTED;

        rv = p_adpt_api->adpt_mirr_port_in_set(dev_id, port_id, enable);
        return rv;
    }

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->mirr_port_in_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->mirr_port_in_set(dev_id, port_id, enable);
    return rv;
}


static sw_error_t
_fal_mirr_port_in_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
        if (NULL == p_adpt_api->adpt_mirr_port_in_get)
            return SW_NOT_SUPPORTED;

        rv = p_adpt_api->adpt_mirr_port_in_get(dev_id, port_id, enable);
        return rv;
    }

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->mirr_port_in_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->mirr_port_in_get(dev_id, port_id, enable);
    return rv;
}

static sw_error_t
_fal_mirr_port_eg_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
        if (NULL == p_adpt_api->adpt_mirr_port_eg_set)
            return SW_NOT_SUPPORTED;

        rv = p_adpt_api->adpt_mirr_port_eg_set(dev_id, port_id, enable);
        return rv;
    }

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->mirr_port_eg_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->mirr_port_eg_set(dev_id, port_id, enable);
    return rv;
}


static sw_error_t
_fal_mirr_port_eg_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
        if (NULL == p_adpt_api->adpt_mirr_port_eg_get)
            return SW_NOT_SUPPORTED;

        rv = p_adpt_api->adpt_mirr_port_eg_get(dev_id, port_id, enable);
        return rv;
    }

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->mirr_port_eg_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->mirr_port_eg_get(dev_id, port_id, enable);
    return rv;
}
sw_error_t
_fal_mirr_analysis_config_set(a_uint32_t dev_id, fal_mirr_direction_t direction, fal_mirr_analysis_config_t * config)
{
    adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_mirr_analysis_config_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_mirr_analysis_config_set(dev_id, direction, config);
    return rv;
}
sw_error_t
_fal_mirr_analysis_config_get(a_uint32_t dev_id, fal_mirr_direction_t direction, fal_mirr_analysis_config_t * config)
{
    adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_mirr_analysis_config_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_mirr_analysis_config_get(dev_id, direction, config);
    return rv;
}
/*insert flag for inner fal, don't remove it*/
/**
 *   @details  Comments:
 *   The analysis port works for both ingress and egress mirror.
 * @brief Set mirror analyzer port on particular a device.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @return SW_OK or error code
 */
sw_error_t
fal_mirr_analysis_port_set(a_uint32_t dev_id, fal_port_t port_id)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_mirr_analysis_port_set(dev_id, port_id);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get mirror analysis port on particular a device.
 * @param[in] dev_id device id
 * @param[out] port_id port id
 * @return SW_OK or error code
 */
sw_error_t
fal_mirr_analysis_port_get(a_uint32_t dev_id, fal_port_t * port_id)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_mirr_analysis_port_get(dev_id, port_id);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set ingress mirror status on particular a port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_mirr_port_in_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_mirr_port_in_set(dev_id, port_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get ingress mirror status on particular a port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_mirr_port_in_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_mirr_port_in_get(dev_id, port_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set egress mirror status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_mirr_port_eg_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_mirr_port_eg_set(dev_id, port_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get egress mirror status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_mirr_port_eg_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_mirr_port_eg_get(dev_id, port_id, enable);
    FAL_API_UNLOCK;
    return rv;
}
sw_error_t
fal_mirr_analysis_config_set(a_uint32_t dev_id, fal_mirr_direction_t direction, fal_mirr_analysis_config_t * config)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_mirr_analysis_config_set(dev_id, direction, config);
    FAL_API_UNLOCK;
    return rv;
}
sw_error_t
fal_mirr_analysis_config_get(a_uint32_t dev_id, fal_mirr_direction_t direction, fal_mirr_analysis_config_t * config)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_mirr_analysis_config_get(dev_id, direction, config);
    FAL_API_UNLOCK;
    return rv;
}
/*insert flag for outter fal, don't remove it*/

EXPORT_SYMBOL(fal_mirr_analysis_port_set);
EXPORT_SYMBOL(fal_mirr_analysis_port_get);
EXPORT_SYMBOL(fal_mirr_port_in_set);
EXPORT_SYMBOL(fal_mirr_port_in_get);
EXPORT_SYMBOL(fal_mirr_port_eg_set);
EXPORT_SYMBOL(fal_mirr_port_eg_get);
EXPORT_SYMBOL(fal_mirr_analysis_config_set);
EXPORT_SYMBOL(fal_mirr_analysis_config_get);

/**
 * @}
 */
