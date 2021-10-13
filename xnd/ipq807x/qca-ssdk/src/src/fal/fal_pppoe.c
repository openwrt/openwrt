/*
 * Copyright (c) 2012, 2015-2017, The Linux Foundation. All rights reserved.
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
 * @defgroup fal_pppoe FAL_PPPOE
 * @{
 */
#include "sw.h"
#include "fal_pppoe.h"
#include "hsl_api.h"
#include "adpt.h"

#include <linux/kernel.h>
#include <linux/module.h>


static sw_error_t
_fal_pppoe_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->pppoe_cmd_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->pppoe_cmd_set(dev_id, cmd);
    return rv;
}


static sw_error_t
_fal_pppoe_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->pppoe_cmd_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->pppoe_cmd_get(dev_id, cmd);
    return rv;
}


static sw_error_t
_fal_pppoe_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->pppoe_status_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->pppoe_status_set(dev_id, enable);
    return rv;
}


static sw_error_t
_fal_pppoe_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->pppoe_status_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->pppoe_status_get(dev_id, enable);
    return rv;
}

static sw_error_t
_fal_pppoe_session_add(a_uint32_t dev_id, a_uint32_t session_id, a_bool_t strip_hdr)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->pppoe_session_add)
        return SW_NOT_SUPPORTED;

    rv = p_api->pppoe_session_add(dev_id, session_id, strip_hdr);
    return rv;
}

static sw_error_t
_fal_pppoe_session_del(a_uint32_t dev_id, a_uint32_t session_id)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->pppoe_session_del)
        return SW_NOT_SUPPORTED;

    rv = p_api->pppoe_session_del(dev_id, session_id);
    return rv;
}

static sw_error_t
_fal_pppoe_session_get(a_uint32_t dev_id, a_uint32_t session_id, a_bool_t * strip_hdr)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->pppoe_session_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->pppoe_session_get(dev_id, session_id, strip_hdr);
    return rv;
}

static sw_error_t
_fal_pppoe_session_table_add(a_uint32_t dev_id,
                             fal_pppoe_session_t * session_tbl)
{
    sw_error_t rv;
    hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
        if (NULL == p_adpt_api->adpt_pppoe_session_table_add)
            return SW_NOT_SUPPORTED;

        rv = p_adpt_api->adpt_pppoe_session_table_add(dev_id, session_tbl);
        return rv;
    }

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->pppoe_session_table_add)
        return SW_NOT_SUPPORTED;

    rv = p_api->pppoe_session_table_add(dev_id, session_tbl);
    return rv;
}


static sw_error_t
_fal_pppoe_session_table_del(a_uint32_t dev_id,
                             fal_pppoe_session_t * session_tbl)
{
    sw_error_t rv;
    hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
        if (NULL == p_adpt_api->adpt_pppoe_session_table_del)
            return SW_NOT_SUPPORTED;

        rv = p_adpt_api->adpt_pppoe_session_table_del(dev_id, session_tbl);
        return rv;
    }

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->pppoe_session_table_del)
        return SW_NOT_SUPPORTED;

    rv = p_api->pppoe_session_table_del(dev_id, session_tbl);
    return rv;
}

static sw_error_t
_fal_pppoe_session_table_get(a_uint32_t dev_id,
                             fal_pppoe_session_t * session_tbl)
{
    sw_error_t rv;
    hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
        if (NULL == p_adpt_api->adpt_pppoe_session_table_get)
            return SW_NOT_SUPPORTED;

        rv = p_adpt_api->adpt_pppoe_session_table_get(dev_id, session_tbl);
        return rv;
    }

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->pppoe_session_table_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->pppoe_session_table_get(dev_id, session_tbl);
    return rv;
}

static sw_error_t
_fal_pppoe_session_id_set(a_uint32_t dev_id, a_uint32_t index,
                          a_uint32_t id)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->pppoe_session_id_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->pppoe_session_id_set(dev_id, index, id);
    return rv;
}

static sw_error_t
_fal_pppoe_session_id_get(a_uint32_t dev_id, a_uint32_t index,
                          a_uint32_t * id)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->pppoe_session_id_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->pppoe_session_id_get(dev_id, index, id);
    return rv;
}

static sw_error_t
_fal_rtd_pppoe_en_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->rtd_pppoe_en_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->rtd_pppoe_en_set(dev_id, enable);
    return rv;
}


static sw_error_t
_fal_rtd_pppoe_en_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->rtd_pppoe_en_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->rtd_pppoe_en_get(dev_id, enable);
    return rv;
}

static sw_error_t
_fal_pppoe_l3intf_status_get(a_uint32_t dev_id, a_uint32_t l3_if, a_uint32_t *enable)
{
    adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_pppoe_en_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_pppoe_en_get(dev_id, l3_if, enable);
    return rv;
}

static sw_error_t
_fal_pppoe_l3intf_enable(a_uint32_t dev_id, a_uint32_t l3_if, a_uint32_t enable)
{
    adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_pppoe_en_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_pppoe_en_set(dev_id, l3_if, enable);
    return rv;
}
/*insert flag for inner fal, don't remove it*/

/**
 * @brief Set pppoe packets forwarding command on a particular device.
 * @details     comments:
 *   Particular device may only support parts of forwarding commands.
 *   Ihis operation will take effect only after enabling pppoe packets
 *   hardware acknowledgement
 * @param[in] dev_id device id
 * @param[in] cmd forwarding command
 * @return SW_OK or error code
 */
sw_error_t
fal_pppoe_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_pppoe_cmd_set(dev_id, cmd);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get pppoe packets forwarding command on a particular device.
 * @param[in] dev_id device id
 * @param[out] cmd forwarding command
 * @return SW_OK or error code
 */
sw_error_t
fal_pppoe_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_pppoe_cmd_get(dev_id, cmd);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set pppoe packets hardware acknowledgement status on particular device.
 * @details     comments:
 *   Particular device may only support parts of pppoe packets.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_pppoe_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_pppoe_status_set(dev_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get pppoe packets hardware acknowledgement status on a particular device.
 * @param[in] dev_id device id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_pppoe_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_pppoe_status_get(dev_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Add a pppoe session entry to a particular device.
 * @param[in] dev_id device id
 * @param[in] session_id pppoe session id
 * @param[in] strip_hdr strip or not strip pppoe header
 * @return SW_OK or error code
 */
sw_error_t
fal_pppoe_session_add(a_uint32_t dev_id, a_uint32_t session_id, a_bool_t strip_hdr)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_pppoe_session_add(dev_id, session_id, strip_hdr);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Delete a pppoe session entry from a particular device.
 * @param[in] dev_id device id
 * @param[in] session_id pppoe session id
 * @return SW_OK or error code
 */
sw_error_t
fal_pppoe_session_del(a_uint32_t dev_id, a_uint32_t session_id)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_pppoe_session_del(dev_id, session_id);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get a pppoe session entry from a particular device.
 * @param[in] dev_id device id
 * @param[in] session_id pppoe session id
 * @param[out] strip_hdr strip or not strip pppoe header
 * @return SW_OK or error code
 */
sw_error_t
fal_pppoe_session_get(a_uint32_t dev_id, a_uint32_t session_id, a_bool_t * strip_hdr)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_pppoe_session_get(dev_id, session_id, strip_hdr);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Add a pppoe session entry to a particular device.
 *        The entry only for pppoe/ppp header remove.
 * @param[in] dev_id device id
 * @param[in] session_tbl pppoe session table
 * @return SW_OK or error code
 */
sw_error_t
fal_pppoe_session_table_add(a_uint32_t dev_id,
                            fal_pppoe_session_t * session_tbl)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_pppoe_session_table_add(dev_id, session_tbl);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Delete a pppoe session entry from a particular device.
 *        The entry only for pppoe/ppp header remove.
 * @param[in] dev_id device id
 * @param[in] session_tbl pppoe session table
 * @return SW_OK or error code
 */
sw_error_t
fal_pppoe_session_table_del(a_uint32_t dev_id,
                            fal_pppoe_session_t * session_tbl)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_pppoe_session_table_del(dev_id, session_tbl);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get a pppoe session entry from a particular device.
 *        The entry only for pppoe/ppp header remove.
 * @param[in] dev_id device id
 * @param[out] session_tbl pppoe session table
 * @return SW_OK or error code
 */
sw_error_t
fal_pppoe_session_table_get(a_uint32_t dev_id,
                            fal_pppoe_session_t * session_tbl)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_pppoe_session_table_get(dev_id, session_tbl);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set a pppoe session id entry to a particular device.
 *        The entry only for pppoe/ppp header add.
 * @param[in] dev_id device id
 * @param[in] session_tbl pppoe session table
 * @return SW_OK or error code
 */
sw_error_t
fal_pppoe_session_id_set(a_uint32_t dev_id, a_uint32_t index,
                         a_uint32_t id)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_pppoe_session_id_set(dev_id, index, id);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get a pppoe session id entry to a particular device.
 *        The entry only for pppoe/ppp header add.
 * @param[in] dev_id device id
 * @param[out] session_tbl pppoe session table
 * @return SW_OK or error code
 */
sw_error_t
fal_pppoe_session_id_get(a_uint32_t dev_id, a_uint32_t index,
                         a_uint32_t * id)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_pppoe_session_id_get(dev_id, index, id);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set RM_RTD_PPPOE_EN status on a particular device.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_rtd_pppoe_en_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_rtd_pppoe_en_set(dev_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get RM_RTD_PPPOE_EN status on a particular device.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_rtd_pppoe_en_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_rtd_pppoe_en_get(dev_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set a l3 interface pppoe status
 * @param[in] dev_id device id
 * @param[in] l3 interface name
 * @param[in] pppoe status enable or disable
 * @return SW_OK or error code
 */
sw_error_t
fal_pppoe_l3intf_status_get(a_uint32_t dev_id, a_uint32_t l3_if, a_uint32_t *enable)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_pppoe_l3intf_status_get(dev_id, l3_if, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get a l3 interface pppoe status
 * @param[in] dev_id device id
 * @param[in] l3 interface name
 * @param[out] pppoe status enable or disable
 * @return SW_OK or error code
 */
sw_error_t
fal_pppoe_l3intf_enable(a_uint32_t dev_id, a_uint32_t l3_if, a_uint32_t enable)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_pppoe_l3intf_enable(dev_id, l3_if, enable);
    FAL_API_UNLOCK;
    return rv;
}
/*insert flag for outter fal, don't remove it*/

EXPORT_SYMBOL(fal_pppoe_session_table_add);
EXPORT_SYMBOL(fal_pppoe_session_table_del);
EXPORT_SYMBOL(fal_pppoe_session_table_get);
EXPORT_SYMBOL(fal_pppoe_l3intf_status_get);
EXPORT_SYMBOL(fal_pppoe_l3intf_enable);

/**
 * @}
 */

