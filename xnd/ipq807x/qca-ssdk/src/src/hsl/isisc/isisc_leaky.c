/*
 * Copyright (c) 2012, 2016 The Linux Foundation. All rights reserved.
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
 * @defgroup isisc_leaky ISISC_LEAKY
 * @{
 */
#include "sw.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_port_prop.h"
#include "isisc_leaky.h"
#include "isisc_reg.h"

static sw_error_t
_isisc_uc_leaky_mode_set(a_uint32_t dev_id,
                        fal_leaky_ctrl_mode_t ctrl_mode)
{
    a_uint32_t data;
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    if (FAL_LEAKY_PORT_CTRL == ctrl_mode)
    {
        data = 0;
    }
    else if (FAL_LEAKY_FDB_CTRL == ctrl_mode)
    {
        data = 1;
    }
    else
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_SET(rv, dev_id, FORWARD_CTL0, 0, ARL_UNI_LEAKY,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_isisc_uc_leaky_mode_get(a_uint32_t dev_id,
                        fal_leaky_ctrl_mode_t *ctrl_mode)
{
    a_uint32_t data = 0;
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    HSL_REG_FIELD_GET(rv, dev_id, FORWARD_CTL0, 0, ARL_UNI_LEAKY,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (1 == data)
    {
        *ctrl_mode = FAL_LEAKY_FDB_CTRL;
    }
    else
    {
        *ctrl_mode = FAL_LEAKY_PORT_CTRL;
    }

    return SW_OK;
}

static sw_error_t
_isisc_mc_leaky_mode_set(a_uint32_t dev_id,
                        fal_leaky_ctrl_mode_t ctrl_mode)
{
    a_uint32_t data;
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    if (FAL_LEAKY_PORT_CTRL == ctrl_mode)
    {
        data = 0;
    }
    else if (FAL_LEAKY_FDB_CTRL == ctrl_mode)
    {
        data = 1;
    }
    else
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_SET(rv, dev_id, FORWARD_CTL0, 0, ARL_MUL_LEAKY,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_isisc_mc_leaky_mode_get(a_uint32_t dev_id,
                        fal_leaky_ctrl_mode_t *ctrl_mode)
{
    a_uint32_t data = 0;
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    HSL_REG_FIELD_GET(rv, dev_id, FORWARD_CTL0, 0, ARL_MUL_LEAKY,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (1 == data)
    {
        *ctrl_mode = FAL_LEAKY_FDB_CTRL;
    }
    else
    {
        *ctrl_mode = FAL_LEAKY_PORT_CTRL;
    }

    return SW_OK;
}

static sw_error_t
_isisc_port_arp_leaky_set(a_uint32_t dev_id, fal_port_t port_id,
                         a_bool_t enable)
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

    HSL_REG_FIELD_SET(rv, dev_id, PORT_LOOKUP_CTL, port_id, ARP_LEAKY_EN,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_isisc_port_arp_leaky_get(a_uint32_t dev_id, fal_port_t port_id,
                         a_bool_t *enable)
{
    a_uint32_t data = 0;
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_GET(rv, dev_id, PORT_LOOKUP_CTL, port_id, ARP_LEAKY_EN,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (1 == data)
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
_isisc_port_uc_leaky_set(a_uint32_t dev_id, fal_port_t port_id,
                        a_bool_t enable)
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

    HSL_REG_FIELD_SET(rv, dev_id, PORT_LOOKUP_CTL, port_id, UNI_LEAKY_EN,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_isisc_port_uc_leaky_get(a_uint32_t dev_id, fal_port_t port_id,
                        a_bool_t *enable)
{
    a_uint32_t data = 0;
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_GET(rv, dev_id, PORT_LOOKUP_CTL, port_id, UNI_LEAKY_EN,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (1 == data)
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
_isisc_port_mc_leaky_set(a_uint32_t dev_id, fal_port_t port_id,
                        a_bool_t enable)
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

    HSL_REG_FIELD_SET(rv, dev_id, PORT_LOOKUP_CTL, port_id, MUL_LEAKY_EN,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_isisc_port_mc_leaky_get(a_uint32_t dev_id, fal_port_t port_id,
                        a_bool_t *enable)
{
    a_uint32_t data = 0;
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_GET(rv, dev_id, PORT_LOOKUP_CTL, port_id, MUL_LEAKY_EN,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (1 == data)
    {
        *enable = A_TRUE;
    }
    else
    {
        *enable = A_FALSE;
    }

    return SW_OK;
}

/**
* @brief Set unicast packets leaky control mode on a particular device.
* @param[in] dev_id device id
* @param[in] ctrl_mode leaky control mode
* @return SW_OK or error code
*/
HSL_LOCAL sw_error_t
isisc_uc_leaky_mode_set(a_uint32_t dev_id,
                       fal_leaky_ctrl_mode_t ctrl_mode)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_uc_leaky_mode_set(dev_id, ctrl_mode);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get unicast packets leaky control mode on a particular device.
 * @param[in] dev_id device id
 * @param[out] ctrl_mode leaky control mode
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_uc_leaky_mode_get(a_uint32_t dev_id,
                       fal_leaky_ctrl_mode_t *ctrl_mode)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_uc_leaky_mode_get(dev_id, ctrl_mode);
    HSL_API_UNLOCK;
    return rv;
}

/**
* @brief Set multicast packets leaky control mode on a particular device.
* @param[in] dev_id device id
* @param[in] ctrl_mode leaky control mode
* @return SW_OK or error code
*/
HSL_LOCAL sw_error_t
isisc_mc_leaky_mode_set(a_uint32_t dev_id,
                       fal_leaky_ctrl_mode_t ctrl_mode)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_mc_leaky_mode_set(dev_id, ctrl_mode);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get multicast packets leaky control mode on a particular device.
 * @param[in] dev_id device id
 * @param[out] ctrl_mode leaky control mode
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_mc_leaky_mode_get(a_uint32_t dev_id,
                       fal_leaky_ctrl_mode_t *ctrl_mode)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_mc_leaky_mode_get(dev_id, ctrl_mode);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set arp packets leaky status on a particular port.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_port_arp_leaky_set(a_uint32_t dev_id, fal_port_t port_id,
                        a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_port_arp_leaky_set(dev_id, port_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get arp packets leaky status on a particular port.
 * @param[in] dev_id device id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_port_arp_leaky_get(a_uint32_t dev_id, fal_port_t port_id,
                        a_bool_t *enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_port_arp_leaky_get(dev_id, port_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set unicast packets leaky status on a particular port.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_port_uc_leaky_set(a_uint32_t dev_id, fal_port_t port_id,
                       a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_port_uc_leaky_set(dev_id, port_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get unicast packets leaky status on a particular port.
 * @param[in] dev_id device id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_port_uc_leaky_get(a_uint32_t dev_id, fal_port_t port_id,
                       a_bool_t *enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_port_uc_leaky_get(dev_id, port_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set multicast packets leaky status on a particular port.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_port_mc_leaky_set(a_uint32_t dev_id, fal_port_t port_id,
                       a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_port_mc_leaky_set(dev_id, port_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get multicast packets leaky status on a particular port.
 * @param[in] dev_id device id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_port_mc_leaky_get(a_uint32_t dev_id, fal_port_t port_id,
                       a_bool_t *enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_port_mc_leaky_get(dev_id, port_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

sw_error_t
isisc_leaky_init(a_uint32_t dev_id)
{
    HSL_DEV_ID_CHECK(dev_id);

#ifndef HSL_STANDALONG
    {
        hsl_api_t *p_api;

        SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));
        p_api->uc_leaky_mode_set = isisc_uc_leaky_mode_set;
        p_api->uc_leaky_mode_get = isisc_uc_leaky_mode_get;
        p_api->mc_leaky_mode_set = isisc_mc_leaky_mode_set;
        p_api->mc_leaky_mode_get = isisc_mc_leaky_mode_get;
        p_api->port_arp_leaky_set = isisc_port_arp_leaky_set;
        p_api->port_arp_leaky_get = isisc_port_arp_leaky_get;
        p_api->port_uc_leaky_set = isisc_port_uc_leaky_set;
        p_api->port_uc_leaky_get = isisc_port_uc_leaky_get;
        p_api->port_mc_leaky_set = isisc_port_mc_leaky_set;
        p_api->port_mc_leaky_get = isisc_port_mc_leaky_get;
    }
#endif

    return SW_OK;
}

/**
 * @}
 */

