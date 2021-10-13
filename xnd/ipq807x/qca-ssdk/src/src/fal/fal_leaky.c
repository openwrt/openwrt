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
 * @defgroup fal_leaky FAL_LEAKY
 * @{
 */
#include "sw.h"
#include "fal_leaky.h"
#include "hsl_api.h"

static sw_error_t
_fal_uc_leaky_mode_set(a_uint32_t dev_id, fal_leaky_ctrl_mode_t ctrl_mode)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->uc_leaky_mode_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->uc_leaky_mode_set(dev_id, ctrl_mode);
    return rv;
}

static sw_error_t
_fal_uc_leaky_mode_get(a_uint32_t dev_id, fal_leaky_ctrl_mode_t * ctrl_mode)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->uc_leaky_mode_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->uc_leaky_mode_get(dev_id, ctrl_mode);
    return rv;
}


static sw_error_t
_fal_mc_leaky_mode_set(a_uint32_t dev_id, fal_leaky_ctrl_mode_t ctrl_mode)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->mc_leaky_mode_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->mc_leaky_mode_set(dev_id, ctrl_mode);
    return rv;
}


static sw_error_t
_fal_mc_leaky_mode_get(a_uint32_t dev_id, fal_leaky_ctrl_mode_t * ctrl_mode)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->mc_leaky_mode_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->mc_leaky_mode_get(dev_id, ctrl_mode);
    return rv;
}


static sw_error_t
_fal_port_arp_leaky_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->port_arp_leaky_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->port_arp_leaky_set(dev_id, port_id, enable);
    return rv;
}


static sw_error_t
_fal_port_arp_leaky_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->port_arp_leaky_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->port_arp_leaky_get(dev_id, port_id, enable);
    return rv;
}

static sw_error_t
_fal_port_uc_leaky_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->port_uc_leaky_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->port_uc_leaky_set(dev_id, port_id, enable);
    return rv;
}


static sw_error_t
_fal_port_uc_leaky_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->port_uc_leaky_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->port_uc_leaky_get(dev_id, port_id, enable);
    return rv;
}



static sw_error_t
_fal_port_mc_leaky_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->port_mc_leaky_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->port_mc_leaky_set(dev_id, port_id, enable);
    return rv;
}


static sw_error_t
_fal_port_mc_leaky_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->port_mc_leaky_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->port_mc_leaky_get(dev_id, port_id, enable);
    return rv;
}
/**
* @brief Set unicast packets leaky control mode on a particular device.
* @param[in] dev_id device id
* @param[in] ctrl_mode leaky control mode
* @return SW_OK or error code
*/
sw_error_t
fal_uc_leaky_mode_set(a_uint32_t dev_id, fal_leaky_ctrl_mode_t ctrl_mode)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_uc_leaky_mode_set(dev_id, ctrl_mode);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get unicast packets leaky control mode on a particular device.
 * @param[in] dev_id device id
 * @param[out] ctrl_mode leaky control mode
 * @return SW_OK or error code
 */
sw_error_t
fal_uc_leaky_mode_get(a_uint32_t dev_id, fal_leaky_ctrl_mode_t * ctrl_mode)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_uc_leaky_mode_get(dev_id, ctrl_mode);
    FAL_API_UNLOCK;
    return rv;
}

/**
* @brief Set multicast packets leaky control mode on a particular device.
* @param[in] dev_id device id
* @param[in] ctrl_mode leaky control mode
* @return SW_OK or error code
*/
sw_error_t
fal_mc_leaky_mode_set(a_uint32_t dev_id, fal_leaky_ctrl_mode_t ctrl_mode)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_mc_leaky_mode_set(dev_id, ctrl_mode);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get multicast packets leaky control mode on a particular device.
 * @param[in] dev_id device id
 * @param[out] ctrl_mode leaky control mode
 * @return SW_OK or error code
 */
sw_error_t
fal_mc_leaky_mode_get(a_uint32_t dev_id, fal_leaky_ctrl_mode_t * ctrl_mode)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_mc_leaky_mode_get(dev_id, ctrl_mode);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set arp packets leaky status on a particular port.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_arp_leaky_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_port_arp_leaky_set(dev_id, port_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get arp packets leaky status on a particular port.
 * @param[in] dev_id device id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_arp_leaky_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_port_arp_leaky_get(dev_id, port_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set unicast packets leaky status on a particular port.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_uc_leaky_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_port_uc_leaky_set(dev_id, port_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get unicast packets leaky status on a particular port.
 * @param[in] dev_id device id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_uc_leaky_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_port_uc_leaky_get(dev_id, port_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set multicast packets leaky status on a particular port.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_mc_leaky_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_port_mc_leaky_set(dev_id, port_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get multicast packets leaky status on a particular port.
 * @param[in] dev_id device id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_mc_leaky_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_port_mc_leaky_get(dev_id, port_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @}
 */
