/*
 * Copyright (c) 2012, 2016, The Linux Foundation. All rights reserved.
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
 * @defgroup shiva_stp SHIVA_STP
 * @{
 */
#include "sw.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_port_prop.h"
#include "shiva_stp.h"
#include "shiva_reg.h"

#define SHIVA_PORT_DISABLED      0
#define SHIVA_STP_BLOCKING       1
#define SHIVA_STP_LISTENING      2
#define SHIVA_STP_LEARNING       3
#define SHIVA_STP_FARWARDING     4

static sw_error_t
_shiva_stp_port_state_set(a_uint32_t dev_id, a_uint32_t st_id,
                          fal_port_t port_id, fal_stp_state_t state)
{
    sw_error_t rv;
    a_uint32_t val;

    HSL_DEV_ID_CHECK(dev_id);

    if (FAL_SINGLE_STP_ID != st_id)
    {
        return SW_BAD_PARAM;
    }

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_EXCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    switch (state)
    {
        case FAL_STP_BLOKING:
            val = SHIVA_STP_BLOCKING;
            break;
        case FAL_STP_LISTENING:
            val = SHIVA_STP_LISTENING;
            break;
        case FAL_STP_LEARNING:
            val = SHIVA_STP_LEARNING;
            break;
        case FAL_STP_FARWARDING:
            val = SHIVA_STP_FARWARDING;
            break;
        case FAL_STP_DISABLED:
            val =  SHIVA_PORT_DISABLED;
            break;
        default:
            return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_SET(rv, dev_id, PORT_CTL, port_id, PORT_STATE,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_shiva_stp_port_state_get(a_uint32_t dev_id, a_uint32_t st_id,
                          fal_port_t port_id, fal_stp_state_t * state)
{
    sw_error_t rv;
    a_uint32_t val = 0;

    HSL_DEV_ID_CHECK(dev_id);

    if (FAL_SINGLE_STP_ID != st_id)
    {
        return SW_BAD_PARAM;
    }

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_EXCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_GET(rv, dev_id, PORT_CTL, port_id, PORT_STATE,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    switch (val)
    {
        case SHIVA_STP_BLOCKING:
            *state = FAL_STP_BLOKING;
            break;
        case SHIVA_STP_LISTENING:
            *state = FAL_STP_LISTENING;
            break;
        case SHIVA_STP_LEARNING:
            *state = FAL_STP_LEARNING;
            break;
        case SHIVA_STP_FARWARDING:
            *state = FAL_STP_FARWARDING;
            break;
        case SHIVA_PORT_DISABLED:
            *state = FAL_STP_DISABLED;
            break;
        default:
            return SW_FAIL;
    }

    return SW_OK;
}

/**
 * @brief Set port stp state on a particular spanning tree and port.
 *   @details   Comments:
    SHIVA only support single spanning tree so st_id should be
    FAL_SINGLE_STP_ID that is zero.
 * @param[in] dev_id device id
 * @param[in] st_id spanning tree id
 * @param[in] port_id port id
 * @param[in] state port state for spanning tree
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_stp_port_state_set(a_uint32_t dev_id, a_uint32_t st_id,
                         fal_port_t port_id, fal_stp_state_t state)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_stp_port_state_set(dev_id, st_id, port_id, state);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get port stp state on a particular spanning tree and port.
 *   @details  Comments:
    SHIVA only support single spanning tree so st_id should be
    FAL_SINGLE_STP_ID that is zero.
 * @param[in] dev_id device id
 * @param[in] st_id spanning tree id
 * @param[in] port_id port id
 * @param[out] state port state for spanning tree
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_stp_port_state_get(a_uint32_t dev_id, a_uint32_t st_id,
                         fal_port_t port_id, fal_stp_state_t * state)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_stp_port_state_get(dev_id, st_id, port_id, state);
    HSL_API_UNLOCK;
    return rv;
}

sw_error_t
shiva_stp_init(a_uint32_t dev_id)
{
    HSL_DEV_ID_CHECK(dev_id);

#ifndef HSL_STANDALONG
    {
        hsl_api_t *p_api;

        SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

        p_api->stp_port_state_set = shiva_stp_port_state_set;
        p_api->stp_port_state_get = shiva_stp_port_state_get;
    }
#endif

    return SW_OK;
}

/**
 * @}
 */

