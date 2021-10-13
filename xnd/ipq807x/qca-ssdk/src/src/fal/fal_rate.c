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
 * @defgroup fal_rate FAL_RATE
 * @{
 */
#include "sw.h"
#include "fal_rate.h"
#include "hsl_api.h"


static sw_error_t
_fal_rate_queue_egrl_set(a_uint32_t dev_id, fal_port_t port_id,
                         fal_queue_t queue_id, a_uint32_t * speed,
                         a_bool_t enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->rate_queue_egrl_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->rate_queue_egrl_set(dev_id, port_id, queue_id, speed, enable);
    return rv;
}


static sw_error_t
_fal_rate_queue_egrl_get(a_uint32_t dev_id, fal_port_t port_id,
                         fal_queue_t queue_id, a_uint32_t * speed,
                         a_bool_t * enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->rate_queue_egrl_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->rate_queue_egrl_get(dev_id, port_id, queue_id, speed, enable);
    return rv;
}


static sw_error_t
_fal_rate_port_egrl_set(a_uint32_t dev_id, fal_port_t port_id,
                        a_uint32_t * speed, a_bool_t enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->rate_port_egrl_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->rate_port_egrl_set(dev_id, port_id, speed, enable);
    return rv;
}


static sw_error_t
_fal_rate_port_egrl_get(a_uint32_t dev_id, fal_port_t port_id,
                        a_uint32_t * speed, a_bool_t * enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->rate_port_egrl_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->rate_port_egrl_get(dev_id, port_id, speed, enable);
    return rv;
}


static sw_error_t
_fal_rate_port_inrl_set(a_uint32_t dev_id, fal_port_t port_id,
                        a_uint32_t * speed, a_bool_t enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->rate_port_inrl_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->rate_port_inrl_set(dev_id, port_id, speed, enable);
    return rv;
}


static sw_error_t
_fal_rate_port_inrl_get(a_uint32_t dev_id, fal_port_t port_id,
                        a_uint32_t * speed, a_bool_t * enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->rate_port_inrl_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->rate_port_inrl_get(dev_id, port_id, speed, enable);
    return rv;
}


static sw_error_t
_fal_storm_ctrl_frame_set(a_uint32_t dev_id, fal_port_t port_id,
                          fal_storm_type_t frame_type, a_bool_t enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->storm_ctrl_frame_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->storm_ctrl_frame_set(dev_id, port_id, frame_type, enable);
    return rv;
}


static sw_error_t
_fal_storm_ctrl_frame_get(a_uint32_t dev_id, fal_port_t port_id,
                          fal_storm_type_t frame_type, a_bool_t * enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->storm_ctrl_frame_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->storm_ctrl_frame_get(dev_id, port_id, frame_type, enable);
    return rv;
}


static sw_error_t
_fal_storm_ctrl_rate_set(a_uint32_t dev_id, fal_port_t port_id,
                         a_uint32_t * rate)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->storm_ctrl_rate_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->storm_ctrl_rate_set(dev_id, port_id, rate);
    return rv;
}


static sw_error_t
_fal_storm_ctrl_rate_get(a_uint32_t dev_id, fal_port_t port_id,
                         a_uint32_t * rate)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->storm_ctrl_rate_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->storm_ctrl_rate_get(dev_id, port_id, rate);
    return rv;
}

static sw_error_t
_fal_rate_port_policer_set(a_uint32_t dev_id, fal_port_t port_id,
                           fal_port_policer_t * policer)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->rate_port_policer_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->rate_port_policer_set(dev_id, port_id, policer);
    return rv;
}

static sw_error_t
_fal_rate_port_policer_get(a_uint32_t dev_id, fal_port_t port_id,
                           fal_port_policer_t * policer)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->rate_port_policer_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->rate_port_policer_get(dev_id, port_id, policer);
    return rv;
}

static sw_error_t
_fal_rate_port_shaper_set(a_uint32_t dev_id, fal_port_t port_id,
                          a_bool_t enable, fal_egress_shaper_t * shaper)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->rate_port_shaper_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->rate_port_shaper_set(dev_id, port_id, enable, shaper);
    return rv;
}

static sw_error_t
_fal_rate_port_shaper_get(a_uint32_t dev_id, fal_port_t port_id,
                          a_bool_t * enable, fal_egress_shaper_t * shaper)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->rate_port_shaper_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->rate_port_shaper_get(dev_id, port_id, enable, shaper);
    return rv;
}

static sw_error_t
_fal_rate_queue_shaper_set(a_uint32_t dev_id, fal_port_t port_id,
                           fal_queue_t queue_id, a_bool_t enable,
                           fal_egress_shaper_t * shaper)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->rate_queue_shaper_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->rate_queue_shaper_set(dev_id, port_id, queue_id, enable, shaper);
    return rv;
}

static sw_error_t
_fal_rate_queue_shaper_get(a_uint32_t dev_id, fal_port_t port_id,
                           fal_queue_t queue_id, a_bool_t * enable,
                           fal_egress_shaper_t * shaper)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->rate_queue_shaper_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->rate_queue_shaper_get(dev_id, port_id, queue_id, enable, shaper);
    return rv;
}

static sw_error_t
_fal_rate_acl_policer_set(a_uint32_t dev_id, a_uint32_t policer_id,
                          fal_acl_policer_t * policer)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->rate_acl_policer_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->rate_acl_policer_set(dev_id, policer_id, policer);
    return rv;
}

static sw_error_t
_fal_rate_acl_policer_get(a_uint32_t dev_id, a_uint32_t policer_id,
                          fal_acl_policer_t * policer)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->rate_acl_policer_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->rate_acl_policer_get(dev_id, policer_id, policer);
    return rv;
}

sw_error_t
_fal_rate_port_add_rate_byte_set(a_uint32_t dev_id, fal_port_t port_id,
                                 a_uint32_t  number)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->rate_port_add_rate_byte_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->rate_port_add_rate_byte_set(dev_id, port_id, number);
    return rv;
}

sw_error_t
_fal_rate_port_add_rate_byte_get(a_uint32_t dev_id, fal_port_t port_id,
                                 a_uint32_t  *number)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->rate_port_add_rate_byte_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->rate_port_add_rate_byte_get(dev_id, port_id, number);
    return rv;
}

sw_error_t
_fal_rate_port_gol_flow_en_set(a_uint32_t dev_id, fal_port_t port_id,
                               a_bool_t  enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->rate_port_gol_flow_en_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->rate_port_gol_flow_en_set(dev_id, port_id, enable);
    return rv;
}

sw_error_t
_fal_rate_port_gol_flow_en_get(a_uint32_t dev_id, fal_port_t port_id,
                               a_bool_t  *enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->rate_port_gol_flow_en_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->rate_port_gol_flow_en_get(dev_id, port_id, enable);
    return rv;
}



/**
 * @brief Set queue egress rate limit status on one particular port and queue.
 *   @details   Comments:
 *    The granularity of speed is bps.
 *    Because different device has differnet hardware granularity function
 *    will return actual speed in hardware.
 *    When disable queue egress rate limit input parameter speed is meaningless.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] queue_id queue id
 * @param speed rate limit speed
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_rate_queue_egrl_set(a_uint32_t dev_id, fal_port_t port_id,
                        fal_queue_t queue_id, a_uint32_t * speed,
                        a_bool_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_rate_queue_egrl_set(dev_id, port_id, queue_id, speed, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get queue egress rate limit status on one particular port and queue.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] queue_id queue id
 * @param[out] speed rate limit speed
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_rate_queue_egrl_get(a_uint32_t dev_id, fal_port_t port_id,
                        fal_queue_t queue_id, a_uint32_t * speed,
                        a_bool_t * enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_rate_queue_egrl_get(dev_id, port_id, queue_id, speed, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set port egress rate limit status on one particular port.
 *   @details   Comments:
 *    The granularity of speed is bps.
 *    Because different device has differnet hardware granularity function
 *    will return actual speed in hardware.
 *    When disable port egress rate limit input parameter speed is meaningless.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param speed rate limit speed
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_rate_port_egrl_set(a_uint32_t dev_id, fal_port_t port_id,
                       a_uint32_t * speed, a_bool_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_rate_port_egrl_set(dev_id, port_id, speed, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get port egress rate limit status on one particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] speed rate limit speed
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_rate_port_egrl_get(a_uint32_t dev_id, fal_port_t port_id,
                       a_uint32_t * speed, a_bool_t * enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_rate_port_egrl_get(dev_id, port_id, speed, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set port ingress rate limit status on one particular port.
 *   @details   Comments:
 *   The granularity of speed is bps.
 *    Because different device has differnet hardware granularity function
 *    will return actual speed in hardware.
 *    When disable port ingress rate limit input parameter speed is meaningless.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param speed rate limit speed
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_rate_port_inrl_set(a_uint32_t dev_id, fal_port_t port_id,
                       a_uint32_t * speed, a_bool_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_rate_port_inrl_set(dev_id, port_id, speed, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get port ingress rate limit status on one particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] speed rate limit speed
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_rate_port_inrl_get(a_uint32_t dev_id, fal_port_t port_id,
                       a_uint32_t * speed, a_bool_t * enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_rate_port_inrl_get(dev_id, port_id, speed, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set particular type storm control status on one particular port.
 *   @details   Comments:
 *    When enable one particular packets type storm control this type packets
 *    speed will be calculated in storm control.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] frame_type packets type which causes storm
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_storm_ctrl_frame_set(a_uint32_t dev_id, fal_port_t port_id,
                         fal_storm_type_t frame_type, a_bool_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_storm_ctrl_frame_set(dev_id, port_id, frame_type, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get particular type storm control status on one particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] frame_type packets type which causes storm
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_storm_ctrl_frame_get(a_uint32_t dev_id, fal_port_t port_id,
                         fal_storm_type_t frame_type, a_bool_t * enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_storm_ctrl_frame_get(dev_id, port_id, frame_type, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set storm control speed on one particular port.
 *   @details   Comments:
 *   The granularity of speed is packets per second.
 *    Because different device has differnet hardware granularity function
 *    will return actual speed in hardware.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param speed storm control speed
 * @return SW_OK or error code
 */
sw_error_t
fal_storm_ctrl_rate_set(a_uint32_t dev_id, fal_port_t port_id,
                        a_uint32_t * rate)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_storm_ctrl_rate_set(dev_id, port_id, rate);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get storm control speed on one particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] speed storm control speed
 * @return SW_OK or error code
 */
sw_error_t
fal_storm_ctrl_rate_get(a_uint32_t dev_id, fal_port_t port_id,
                        a_uint32_t * rate)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_storm_ctrl_rate_get(dev_id, port_id, rate);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set port ingress policer parameters on one particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] policer port ingress policer parameter
 * @return SW_OK or error code
 */
sw_error_t
fal_rate_port_policer_set(a_uint32_t dev_id, fal_port_t port_id,
                          fal_port_policer_t * policer)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_rate_port_policer_set(dev_id, port_id, policer);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get port ingress policer parameters on one particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] policer port ingress policer parameter
 * @return SW_OK or error code
 */
sw_error_t
fal_rate_port_policer_get(a_uint32_t dev_id, fal_port_t port_id,
                          fal_port_policer_t * policer)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_rate_port_policer_get(dev_id, port_id, policer);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set port egress shaper parameters on one particular port.
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @param[in] shaper port egress shaper parameter
 * @return SW_OK or error code
 */
sw_error_t
fal_rate_port_shaper_set(a_uint32_t dev_id, fal_port_t port_id,
                         a_bool_t enable, fal_egress_shaper_t * shaper)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_rate_port_shaper_set(dev_id, port_id, enable, shaper);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get port egress shaper parameters on one particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @param[out] shaper port egress shaper parameter
 * @return SW_OK or error code
 */
sw_error_t
fal_rate_port_shaper_get(a_uint32_t dev_id, fal_port_t port_id,
                         a_bool_t * enable, fal_egress_shaper_t * shaper)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_rate_port_shaper_get(dev_id, port_id, enable, shaper);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set queue egress shaper parameters on one particular port.
 * @param[in] port_id port id
 * @param[in] queue_id queue id
 * @param[in] enable A_TRUE or A_FALSE
 * @param[in] shaper port egress shaper parameter
 * @return SW_OK or error code
 */
sw_error_t
fal_rate_queue_shaper_set(a_uint32_t dev_id, fal_port_t port_id,
                          fal_queue_t queue_id, a_bool_t enable,
                          fal_egress_shaper_t * shaper)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_rate_queue_shaper_set(dev_id, port_id, queue_id, enable, shaper);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get queue egress shaper parameters on one particular port.
 * @param[in] port_id port id
 * @param[in] queue_id queue id
 * @param[out] enable A_TRUE or A_FALSE
 * @param[out] shaper port egress shaper parameter
 * @return SW_OK or error code
 */
sw_error_t
fal_rate_queue_shaper_get(a_uint32_t dev_id, fal_port_t port_id,
                          fal_queue_t queue_id, a_bool_t * enable,
                          fal_egress_shaper_t * shaper)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_rate_queue_shaper_get(dev_id, port_id, queue_id, enable, shaper);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set ACL ingress policer parameters.
 * @param[in] dev_id device id
 * @param[in] policer_id ACL policer id
 * @param[in] policer ACL ingress policer parameters
 * @return SW_OK or error code
 */
sw_error_t
fal_rate_acl_policer_set(a_uint32_t dev_id, a_uint32_t policer_id,
                         fal_acl_policer_t * policer)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_rate_acl_policer_set(dev_id, policer_id, policer);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get ACL ingress policer parameters.
 * @param[in] dev_id device id
 * @param[in] policer_id ACL policer id
 * @param[in] policer ACL ingress policer parameters
 * @return SW_OK or error code
 */
sw_error_t
fal_rate_acl_policer_get(a_uint32_t dev_id, a_uint32_t policer_id,
                         fal_acl_policer_t * policer)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_rate_acl_policer_get(dev_id, policer_id, policer);
    FAL_API_UNLOCK;
    return rv;
}


sw_error_t
fal_rate_port_add_rate_byte_set(a_uint32_t dev_id, fal_port_t port_id,
                                a_uint32_t  number)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_rate_port_add_rate_byte_set(dev_id, port_id, number);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t
fal_rate_port_add_rate_byte_get(a_uint32_t dev_id, fal_port_t port_id,
                                a_uint32_t  *number)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_rate_port_add_rate_byte_get(dev_id, port_id, number);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set status of port global flow control when global threshold is reached.
  * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_rate_port_gol_flow_en_set(a_uint32_t dev_id, fal_port_t port_id,
                              a_bool_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_rate_port_gol_flow_en_set(dev_id, port_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief get status of port global flow control when global threshold is reached.
  * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_rate_port_gol_flow_en_get(a_uint32_t dev_id, fal_port_t port_id,
                              a_bool_t* enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_rate_port_gol_flow_en_get(dev_id, port_id, enable);
    FAL_API_UNLOCK;
    return rv;
}



/**
 * @}
 */
