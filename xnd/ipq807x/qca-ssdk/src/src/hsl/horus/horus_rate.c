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
 * @defgroup horus_rate HORUS_RATE
 * @{
 */
#include "sw.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_port_prop.h"
#include "horus_rate.h"
#include "horus_reg.h"

#define HORUS_STORM_MIN_RATE_PPS 1000
#define HORUS_STORM_MAX_RATE_PPS (1024 * 1000)

static sw_error_t
horus_stormrate_sw_to_hw(a_uint32_t swrate, a_uint32_t * hwrate)
{
    a_uint32_t shrnr = 0;
    a_uint32_t tmp = swrate / 1000;

    if ((HORUS_STORM_MIN_RATE_PPS > swrate)
            || (HORUS_STORM_MAX_RATE_PPS < swrate))
    {
        return SW_BAD_PARAM;
    }

    while ((tmp != 0) && (shrnr < 12))
    {
        tmp = tmp >> 1;
        shrnr++;
    }

    if (12 == shrnr)
    {
        return SW_BAD_PARAM;
    }

    *hwrate = shrnr;
    return SW_OK;
}

static sw_error_t
horus_stormrate_hw_to_sw(a_uint32_t hwrate, a_uint32_t * swrate)
{
    if (0 == hwrate)
    {
        hwrate = 1;
    }

    if ((1 > hwrate) || (11 < hwrate))
    {
        return SW_BAD_PARAM;
    }

    *swrate = (1 << (hwrate - 1)) * 1000;
    return SW_OK;
}

static sw_error_t
_horus_rate_port_egrl_set(a_uint32_t dev_id, fal_port_t port_id,
                          a_uint32_t * speed, a_bool_t enable)
{
    sw_error_t rv;
    a_uint32_t val;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    if (A_FALSE == enable)
    {
        *speed = 0;

        val = 0x1fff;
        HSL_REG_FIELD_SET(rv, dev_id, RATE_LIMIT1, port_id, EG_RATE,
                          (a_uint8_t *) (&val), sizeof (a_uint32_t));
    }
    else
    {
        if ((0x1ffe << 5) < *speed)
        {
            return SW_BAD_PARAM;
        }

        val = *speed >> 5;
        *speed = val << 5;
        HSL_REG_FIELD_SET(rv, dev_id, RATE_LIMIT1, port_id, EG_RATE,
                          (a_uint8_t *) (&val), sizeof (a_uint32_t));
    }

    return rv;
}

static sw_error_t
_horus_rate_port_egrl_get(a_uint32_t dev_id, fal_port_t port_id,
                          a_uint32_t * speed, a_bool_t * enable)
{
    sw_error_t rv;
    a_uint32_t val;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_GET(rv, dev_id, RATE_LIMIT1, port_id, EG_RATE,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (0x1fff == val)
    {
        *speed = 0;
        *enable = A_FALSE;
    }
    else
    {
        *enable = A_TRUE;
        *speed = val << 5;
    }

    return SW_OK;
}

static sw_error_t
_horus_rate_port_inrl_set(a_uint32_t dev_id, fal_port_t port_id,
                          a_uint32_t * speed, a_bool_t enable)
{
    sw_error_t rv;
    a_uint32_t val;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    if (A_TRUE == enable)
    {
        if ((0x7ffe << 5) < *speed)
        {
            return SW_BAD_PARAM;
        }
        val = *speed >> 5;
        *speed = val << 5;
    }
    else if (A_FALSE == enable)
    {
        val = 0x7fff;
        *speed = 0;
    }
    else
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_SET(rv, dev_id, RATE_LIMIT0, port_id, ING_RATE,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_horus_rate_port_inrl_get(a_uint32_t dev_id, fal_port_t port_id,
                          a_uint32_t * speed, a_bool_t * enable)
{
    sw_error_t rv;
    a_uint32_t val;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_GET(rv, dev_id, RATE_LIMIT0, port_id, ING_RATE,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (0x7fff == val)
    {
        *enable = A_FALSE;
        *speed = 0;
    }
    else
    {
        *enable = A_TRUE;
        *speed = val << 5;
    }

    return SW_OK;
}

static sw_error_t
_horus_storm_ctrl_frame_set(a_uint32_t dev_id, fal_port_t port_id,
                            fal_storm_type_t storm_type, a_bool_t enable)
{
    a_uint32_t data;
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    if (A_TRUE == enable)
    {
        data = 1;
    }
    else if (A_FALSE == enable)
    {
        data = 0;
    }
    else
    {
        return SW_BAD_PARAM;
    }

    if (FAL_UNICAST_STORM == storm_type)
    {
        HSL_REG_FIELD_SET(rv, dev_id, STORM_CTL, port_id, UNI_EN,
                          (a_uint8_t *) (&data), sizeof (a_uint32_t));
    }
    else if (FAL_MULTICAST_STORM == storm_type)
    {
        HSL_REG_FIELD_SET(rv, dev_id, STORM_CTL, port_id, MUL_EN,
                          (a_uint8_t *) (&data), sizeof (a_uint32_t));
    }
    else if (FAL_BROADCAST_STORM == storm_type)
    {
        HSL_REG_FIELD_SET(rv, dev_id, STORM_CTL, port_id, BRO_EN,
                          (a_uint8_t *) (&data), sizeof (a_uint32_t));
    }
    else
    {
        return SW_BAD_PARAM;
    }
    SW_RTN_ON_ERROR(rv);

    HSL_REG_FIELD_GET(rv, dev_id, STORM_CTL, port_id, RATE,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (0 == data)
    {
        data = 1;
        HSL_REG_FIELD_SET(rv, dev_id, STORM_CTL, port_id, RATE,
                          (a_uint8_t *) (&data), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }

    return SW_OK;
}

static sw_error_t
_horus_storm_ctrl_frame_get(a_uint32_t dev_id, fal_port_t port_id,
                            fal_storm_type_t storm_type, a_bool_t * enable)
{
    a_uint32_t data;
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    if (FAL_UNICAST_STORM == storm_type)
    {
        HSL_REG_FIELD_GET(rv, dev_id, STORM_CTL, port_id, UNI_EN,
                          (a_uint8_t *) (&data), sizeof (a_uint32_t));
    }
    else if (FAL_MULTICAST_STORM == storm_type)
    {
        HSL_REG_FIELD_GET(rv, dev_id, STORM_CTL, port_id, MUL_EN,
                          (a_uint8_t *) (&data), sizeof (a_uint32_t));
    }
    else if (FAL_BROADCAST_STORM == storm_type)
    {
        HSL_REG_FIELD_GET(rv, dev_id, STORM_CTL, port_id, BRO_EN,
                          (a_uint8_t *) (&data), sizeof (a_uint32_t));
    }
    else
    {
        return SW_BAD_PARAM;
    }

    SW_RTN_ON_ERROR(rv);

    if (1 == data)
    {
        data = 1;
        *enable = A_TRUE;
    }
    else
    {
        *enable = A_FALSE;
    }

    return SW_OK;
}

static sw_error_t
_horus_storm_ctrl_rate_set(a_uint32_t dev_id, fal_port_t port_id,
                           a_uint32_t * rate_in_pps)
{
    a_uint32_t data;
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    rv = horus_stormrate_sw_to_hw(*rate_in_pps, &data);
    SW_RTN_ON_ERROR(rv);

    HSL_REG_FIELD_SET(rv, dev_id, STORM_CTL, port_id, RATE,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    rv = horus_stormrate_hw_to_sw(data, rate_in_pps);
    SW_RTN_ON_ERROR(rv);

    return SW_OK;
}

static sw_error_t
_horus_storm_ctrl_rate_get(a_uint32_t dev_id, fal_port_t port_id,
                           a_uint32_t * rate_in_pps)
{
    a_uint32_t data;
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_GET(rv, dev_id, STORM_CTL, port_id, RATE,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    rv = horus_stormrate_hw_to_sw(data, rate_in_pps);
    return rv;
}

/**
 * @brief Set port egress rate limit status on one particular port.
 *   @details   Comments:
    The granularity of speed is bps.
    Because of hardware granularity function will return actual speed in hardware.
    When disable port egress rate limit input parameter speed is meaningless.
    The step of speed is 32kbps.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param speed rate limit speed
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
horus_rate_port_egrl_set(a_uint32_t dev_id, fal_port_t port_id,
                         a_uint32_t * speed, a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _horus_rate_port_egrl_set(dev_id, port_id, speed, enable);
    HSL_API_UNLOCK;
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
HSL_LOCAL sw_error_t
horus_rate_port_egrl_get(a_uint32_t dev_id, fal_port_t port_id,
                         a_uint32_t * speed, a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _horus_rate_port_egrl_get(dev_id, port_id, speed, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set port ingress rate limit status on one particular port.
 *   @details   Comments:
    The granularity of speed is bps.
    Because of hardware granularity function will return actual speed in hardware.
    When disable port ingress rate limit input parameter speed is meaningless.
    The step of speed is 32kbps.
 *    When disable port ingress rate limit input parameter speed is meaningless.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param speed rate limit speed
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
horus_rate_port_inrl_set(a_uint32_t dev_id, fal_port_t port_id,
                         a_uint32_t * speed, a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _horus_rate_port_inrl_set(dev_id, port_id, speed, enable);
    HSL_API_UNLOCK;
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
HSL_LOCAL sw_error_t
horus_rate_port_inrl_get(a_uint32_t dev_id, fal_port_t port_id,
                         a_uint32_t * speed, a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _horus_rate_port_inrl_get(dev_id, port_id, speed, enable);
    HSL_API_UNLOCK;
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
HSL_LOCAL sw_error_t
horus_storm_ctrl_frame_set(a_uint32_t dev_id, fal_port_t port_id,
                           fal_storm_type_t storm_type, a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _horus_storm_ctrl_frame_set(dev_id, port_id, storm_type, enable);
    HSL_API_UNLOCK;
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
HSL_LOCAL sw_error_t
horus_storm_ctrl_frame_get(a_uint32_t dev_id, fal_port_t port_id,
                           fal_storm_type_t storm_type, a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _horus_storm_ctrl_frame_get(dev_id, port_id, storm_type, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set storm control speed on one particular port.
 *   @details   Comments:
    Because of hardware granularity function will return actual speed in hardware.
    The step of speed is kpps.
    The speed range is from 1k to 1M
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param speed storm control speed
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
horus_storm_ctrl_rate_set(a_uint32_t dev_id, fal_port_t port_id,
                          a_uint32_t * rate_in_pps)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _horus_storm_ctrl_rate_set(dev_id, port_id, rate_in_pps);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get storm control speed on one particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] speed storm control speed
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
horus_storm_ctrl_rate_get(a_uint32_t dev_id, fal_port_t port_id,
                          a_uint32_t * rate_in_pps)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _horus_storm_ctrl_rate_get(dev_id, port_id, rate_in_pps);
    HSL_API_UNLOCK;
    return rv;
}

sw_error_t
horus_rate_init(a_uint32_t dev_id)
{
    HSL_DEV_ID_CHECK(dev_id);

#ifndef HSL_STANDALONG
    {
        hsl_api_t *p_api;

        SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

        p_api->rate_port_egrl_set = horus_rate_port_egrl_set;
        p_api->rate_port_egrl_get = horus_rate_port_egrl_get;
        p_api->rate_port_inrl_set = horus_rate_port_inrl_set;
        p_api->rate_port_inrl_get = horus_rate_port_inrl_get;
        p_api->storm_ctrl_frame_set = horus_storm_ctrl_frame_set;
        p_api->storm_ctrl_frame_get = horus_storm_ctrl_frame_get;
        p_api->storm_ctrl_rate_set = horus_storm_ctrl_rate_set;
        p_api->storm_ctrl_rate_get = horus_storm_ctrl_rate_get;
    }
#endif

    return SW_OK;
}

/**
 * @}
 */
