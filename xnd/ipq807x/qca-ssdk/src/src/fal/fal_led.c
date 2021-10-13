/*
 * Copyright (c) 2012, 2020, The Linux Foundation. All rights reserved.
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
 * @defgroup fal_led FAL_LED
 * @{
 */

#include "sw.h"
#include "fal_led.h"
#include "hsl_api.h"
#include "adpt.h"

static sw_error_t
_fal_led_ctrl_pattern_set(a_uint32_t dev_id, led_pattern_group_t group,
                          led_pattern_id_t id, led_ctrl_pattern_t * pattern)
{
    sw_error_t rv;
    hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL &&
        p_adpt_api->adpt_led_ctrl_pattern_set != NULL) {
        rv = p_adpt_api->adpt_led_ctrl_pattern_set(dev_id, group, id, pattern);
        return rv;
    }
    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->led_ctrl_pattern_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->led_ctrl_pattern_set(dev_id, group, id, pattern);
    return rv;
}



static sw_error_t
_fal_led_ctrl_pattern_get(a_uint32_t dev_id, led_pattern_group_t group,
                          led_pattern_id_t id, led_ctrl_pattern_t * pattern)
{
    sw_error_t rv;
    hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL &&
        p_adpt_api->adpt_led_ctrl_pattern_get != NULL) {
        rv = p_adpt_api->adpt_led_ctrl_pattern_get(dev_id, group, id, pattern);
        return rv;
    }
    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->led_ctrl_pattern_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->led_ctrl_pattern_get(dev_id, group, id, pattern);
    return rv;
}

static sw_error_t
_fal_led_source_pattern_set(a_uint32_t dev_id, a_uint32_t source_id,
                          led_ctrl_pattern_t * pattern)
{
    sw_error_t rv;
    hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL &&
        p_adpt_api->adpt_led_ctrl_source_set != NULL) {
        rv = p_adpt_api->adpt_led_ctrl_source_set(dev_id, source_id, pattern);
        return rv;
    }
    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->led_ctrl_source_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->led_ctrl_source_set(dev_id, source_id, pattern);
    return rv;
}


/**
* @brief Set led control pattern on a particular device.
* @param[in] dev_id device id
* @param[in] group  pattern group, lan or wan
* @param[in] id pattern id
* @param[in] pattern led control pattern
* @return SW_OK or error code
*/
sw_error_t
fal_led_ctrl_pattern_set(a_uint32_t dev_id, led_pattern_group_t group,
                         led_pattern_id_t id, led_ctrl_pattern_t * pattern)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_led_ctrl_pattern_set(dev_id, group, id, pattern);
    FAL_API_UNLOCK;
    return rv;
}

/**
* @brief Get led control pattern on a particular device.
* @param[in] dev_id device id
* @param[in] group  pattern group, lan or wan
* @param[in] id pattern id
* @param[out] pattern led control pattern
* @return SW_OK or error code
*/
sw_error_t
fal_led_ctrl_pattern_get(a_uint32_t dev_id, led_pattern_group_t group,
                         led_pattern_id_t id, led_ctrl_pattern_t * pattern)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_led_ctrl_pattern_get(dev_id, group, id, pattern);
    FAL_API_UNLOCK;
    return rv;
}

/**
* @brief Set led control source on a particular device.
* @param[in] dev_id device id
* @param[in] source id
* @param[in] id pattern id
* @param[in] pattern led control pattern
* @return SW_OK or error code
*/
sw_error_t
fal_led_source_pattern_set(a_uint32_t dev_id, a_uint32_t source_id,
                        led_ctrl_pattern_t * pattern)
{
	sw_error_t rv;

	FAL_API_LOCK;
	rv = _fal_led_source_pattern_set(dev_id, source_id, pattern);
	FAL_API_UNLOCK;
	return rv;
}


/**
 * @}
 */
