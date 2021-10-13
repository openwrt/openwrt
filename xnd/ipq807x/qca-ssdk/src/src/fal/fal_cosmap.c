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
 * @defgroup fal_cosmap FAL_COSMAP
 * @{
 */
#include "sw.h"
#include "fal_cosmap.h"
#include "hsl_api.h"

#ifndef IN_COSMAP_MINI
static sw_error_t
_fal_cosmap_dscp_to_pri_set(a_uint32_t dev_id, a_uint32_t dscp, a_uint32_t pri)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->cosmap_dscp_to_pri_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->cosmap_dscp_to_pri_set(dev_id, dscp, pri);
    return rv;
}

static sw_error_t
_fal_cosmap_dscp_to_pri_get(a_uint32_t dev_id, a_uint32_t dscp,
                            a_uint32_t * pri)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->cosmap_dscp_to_pri_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->cosmap_dscp_to_pri_get(dev_id, dscp, pri);
    return rv;
}

static sw_error_t
_fal_cosmap_dscp_to_dp_set(a_uint32_t dev_id, a_uint32_t dscp, a_uint32_t dp)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->cosmap_dscp_to_dp_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->cosmap_dscp_to_dp_set(dev_id, dscp, dp);
    return rv;
}

static sw_error_t
_fal_cosmap_dscp_to_dp_get(a_uint32_t dev_id, a_uint32_t dscp, a_uint32_t * dp)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->cosmap_dscp_to_dp_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->cosmap_dscp_to_dp_get(dev_id, dscp, dp);
    return rv;
}

static sw_error_t
_fal_cosmap_up_to_pri_set(a_uint32_t dev_id, a_uint32_t up, a_uint32_t pri)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->cosmap_up_to_pri_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->cosmap_up_to_pri_set(dev_id, up, pri);
    return rv;
}

static sw_error_t
_fal_cosmap_up_to_pri_get(a_uint32_t dev_id, a_uint32_t up, a_uint32_t * pri)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->cosmap_up_to_pri_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->cosmap_up_to_pri_get(dev_id, up, pri);
    return rv;
}

static sw_error_t
_fal_cosmap_up_to_dp_set(a_uint32_t dev_id, a_uint32_t up, a_uint32_t dp)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->cosmap_up_to_dp_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->cosmap_up_to_dp_set(dev_id, up, dp);
    return rv;
}

static sw_error_t
_fal_cosmap_up_to_dp_get(a_uint32_t dev_id, a_uint32_t up, a_uint32_t * dp)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->cosmap_up_to_dp_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->cosmap_up_to_dp_get(dev_id, up, dp);
    return rv;
}

static sw_error_t
_fal_cosmap_dscp_to_ehpri_set(a_uint32_t dev_id, a_uint32_t dscp, a_uint32_t pri)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->cosmap_dscp_to_ehpri_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->cosmap_dscp_to_ehpri_set(dev_id, dscp, pri);
    return rv;
}

static sw_error_t
_fal_cosmap_dscp_to_ehpri_get(a_uint32_t dev_id, a_uint32_t dscp,
                            a_uint32_t * pri)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->cosmap_dscp_to_ehpri_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->cosmap_dscp_to_ehpri_get(dev_id, dscp, pri);
    return rv;
}

static sw_error_t
_fal_cosmap_dscp_to_ehdp_set(a_uint32_t dev_id, a_uint32_t dscp, a_uint32_t dp)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->cosmap_dscp_to_ehdp_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->cosmap_dscp_to_ehdp_set(dev_id, dscp, dp);
    return rv;
}

static sw_error_t
_fal_cosmap_dscp_to_ehdp_get(a_uint32_t dev_id, a_uint32_t dscp, a_uint32_t * dp)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->cosmap_dscp_to_ehdp_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->cosmap_dscp_to_ehdp_get(dev_id, dscp, dp);
    return rv;
}

static sw_error_t
_fal_cosmap_up_to_ehpri_set(a_uint32_t dev_id, a_uint32_t up, a_uint32_t pri)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->cosmap_up_to_ehpri_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->cosmap_up_to_ehpri_set(dev_id, up, pri);
    return rv;
}

static sw_error_t
_fal_cosmap_up_to_ehpri_get(a_uint32_t dev_id, a_uint32_t up, a_uint32_t * pri)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->cosmap_up_to_ehpri_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->cosmap_up_to_ehpri_get(dev_id, up, pri);
    return rv;
}

static sw_error_t
_fal_cosmap_up_to_ehdp_set(a_uint32_t dev_id, a_uint32_t up, a_uint32_t dp)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->cosmap_up_to_ehdp_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->cosmap_up_to_ehdp_set(dev_id, up, dp);
    return rv;
}

static sw_error_t
_fal_cosmap_up_to_ehdp_get(a_uint32_t dev_id, a_uint32_t up, a_uint32_t * dp)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->cosmap_up_to_ehdp_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->cosmap_up_to_ehdp_get(dev_id, up, dp);
    return rv;
}
#endif

static sw_error_t
_fal_cosmap_pri_to_queue_set(a_uint32_t dev_id, a_uint32_t pri,
                             a_uint32_t queue)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->cosmap_pri_to_queue_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->cosmap_pri_to_queue_set(dev_id, pri, queue);
    return rv;
}

static sw_error_t
_fal_cosmap_pri_to_ehqueue_set(a_uint32_t dev_id, a_uint32_t pri,
                               a_uint32_t queue)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->cosmap_pri_to_ehqueue_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->cosmap_pri_to_ehqueue_set(dev_id, pri, queue);
    return rv;
}

#ifndef IN_COSMAP_MINI
static sw_error_t
_fal_cosmap_pri_to_queue_get(a_uint32_t dev_id, a_uint32_t pri,
                             a_uint32_t * queue)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->cosmap_pri_to_queue_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->cosmap_pri_to_queue_get(dev_id, pri, queue);
    return rv;
}

static sw_error_t
_fal_cosmap_pri_to_ehqueue_get(a_uint32_t dev_id, a_uint32_t pri,
                               a_uint32_t * queue)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->cosmap_pri_to_ehqueue_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->cosmap_pri_to_ehqueue_get(dev_id, pri, queue);
    return rv;
}

static sw_error_t
_fal_cosmap_egress_remark_set(a_uint32_t dev_id, a_uint32_t tbl_id,
                              fal_egress_remark_table_t * tbl)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->cosmap_egress_remark_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->cosmap_egress_remark_set(dev_id, tbl_id, tbl);
    return rv;
}

static sw_error_t
_fal_cosmap_egress_remark_get(a_uint32_t dev_id, a_uint32_t tbl_id,
                              fal_egress_remark_table_t * tbl)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->cosmap_egress_remark_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->cosmap_egress_remark_get(dev_id, tbl_id, tbl);
    return rv;
}

/**
 * @brief Set dscp to internal priority mapping on one particular device.
 * @param[in] dev_id device id
 * @param[in] dscp dscp
 * @param[in] pri internal priority
 * @return SW_OK or error code
 */
sw_error_t
fal_cosmap_dscp_to_pri_set(a_uint32_t dev_id, a_uint32_t dscp, a_uint32_t pri)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_cosmap_dscp_to_pri_set(dev_id, dscp, pri);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get dscp to internal priority mapping on one particular device.
 * @param[in] dev_id device id
 * @param[in] dscp dscp
 * @param[out] pri internal priority
 * @return SW_OK or error code
 */
sw_error_t
fal_cosmap_dscp_to_pri_get(a_uint32_t dev_id, a_uint32_t dscp,
                           a_uint32_t * pri)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_cosmap_dscp_to_pri_get(dev_id, dscp, pri);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set dscp to internal drop precedence mapping on one particular device.
 * @param[in] dev_id device id
 * @param[in] dscp dscp
 * @param[in] dp internal drop precedence
 * @return SW_OK or error code
 */
sw_error_t
fal_cosmap_dscp_to_dp_set(a_uint32_t dev_id, a_uint32_t dscp, a_uint32_t dp)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_cosmap_dscp_to_dp_set(dev_id, dscp, dp);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get dscp to internal drop precedence mapping on one particular device.
 * @param[in] dev_id device id
 * @param[in] dscp dscp
 * @param[out] dp internal drop precedence
 * @return SW_OK or error code
 */
sw_error_t
fal_cosmap_dscp_to_dp_get(a_uint32_t dev_id, a_uint32_t dscp, a_uint32_t * dp)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_cosmap_dscp_to_dp_get(dev_id, dscp, dp);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set dot1p to internal priority mapping on one particular device.
 * @param[in] dev_id device id
 * @param[in] up dot1p
 * @param[in] pri internal priority
 * @return SW_OK or error code
 */
sw_error_t
fal_cosmap_up_to_pri_set(a_uint32_t dev_id, a_uint32_t up, a_uint32_t pri)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_cosmap_up_to_pri_set(dev_id, up, pri);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get dot1p to internal priority mapping on one particular device.
 * @param[in] dev_id device id
 * @param[in] up dot1p
 * @param[out] pri internal priority
 * @return SW_OK or error code
 */
sw_error_t
fal_cosmap_up_to_pri_get(a_uint32_t dev_id, a_uint32_t up, a_uint32_t * pri)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_cosmap_up_to_pri_get(dev_id, up, pri);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set dot1p to internal drop precedence mapping on one particular device.
 * @param[in] dev_id device id
 * @param[in] up dot1p
 * @param[in] dp internal drop precedence
 * @return SW_OK or error code
 */
sw_error_t
fal_cosmap_up_to_dp_set(a_uint32_t dev_id, a_uint32_t up, a_uint32_t dp)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_cosmap_up_to_dp_set(dev_id, up, dp);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get dot1p to internal drop precedence mapping on one particular device.
 * @param[in] dev_id device id
 * @param[in] up dot1p
 * @param[in] dp internal drop precedence
 * @return SW_OK or error code
 */
sw_error_t
fal_cosmap_up_to_dp_get(a_uint32_t dev_id, a_uint32_t up, a_uint32_t * dp)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_cosmap_up_to_dp_get(dev_id, up, dp);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set dscp to internal priority mapping on one particular device for WAN port.
 * @param[in] dev_id device id
 * @param[in] dscp dscp
 * @param[in] pri internal priority
 * @return SW_OK or error code
 */
sw_error_t
fal_cosmap_dscp_to_ehpri_set(a_uint32_t dev_id, a_uint32_t dscp, a_uint32_t pri)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_cosmap_dscp_to_ehpri_set(dev_id, dscp, pri);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get dscp to internal priority mapping on one particular device for WAN port.
 * @param[in] dev_id device id
 * @param[in] dscp dscp
 * @param[out] pri internal priority
 * @return SW_OK or error code
 */
sw_error_t
fal_cosmap_dscp_to_ehpri_get(a_uint32_t dev_id, a_uint32_t dscp,
                           a_uint32_t * pri)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_cosmap_dscp_to_ehpri_get(dev_id, dscp, pri);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set dscp to internal drop precedence mapping on one particular device for WAN port.
 * @param[in] dev_id device id
 * @param[in] dscp dscp
 * @param[in] dp internal drop precedence
 * @return SW_OK or error code
 */
sw_error_t
fal_cosmap_dscp_to_ehdp_set(a_uint32_t dev_id, a_uint32_t dscp, a_uint32_t dp)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_cosmap_dscp_to_ehdp_set(dev_id, dscp, dp);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get dscp to internal drop precedence mapping on one particular device for WAN port.
 * @param[in] dev_id device id
 * @param[in] dscp dscp
 * @param[out] dp internal drop precedence
 * @return SW_OK or error code
 */
sw_error_t
fal_cosmap_dscp_to_ehdp_get(a_uint32_t dev_id, a_uint32_t dscp, a_uint32_t * dp)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_cosmap_dscp_to_ehdp_get(dev_id, dscp, dp);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set dot1p to internal priority mapping on one particular device for WAN port.
 * @param[in] dev_id device id
 * @param[in] up dot1p
 * @param[in] pri internal priority
 * @return SW_OK or error code
 */
sw_error_t
fal_cosmap_up_to_ehpri_set(a_uint32_t dev_id, a_uint32_t up, a_uint32_t pri)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_cosmap_up_to_ehpri_set(dev_id, up, pri);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get dot1p to internal priority mapping on one particular device for WAN port.
 * @param[in] dev_id device id
 * @param[in] up dot1p
 * @param[out] pri internal priority
 * @return SW_OK or error code
 */
sw_error_t
fal_cosmap_up_to_ehpri_get(a_uint32_t dev_id, a_uint32_t up, a_uint32_t * pri)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_cosmap_up_to_ehpri_get(dev_id, up, pri);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set dot1p to internal drop precedence mapping on one particular device for WAN port.
 * @param[in] dev_id device id
 * @param[in] up dot1p
 * @param[in] dp internal drop precedence
 * @return SW_OK or error code
 */
sw_error_t
fal_cosmap_up_to_ehdp_set(a_uint32_t dev_id, a_uint32_t up, a_uint32_t dp)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_cosmap_up_to_ehdp_set(dev_id, up, dp);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get dot1p to internal drop precedence mapping on one particular device for WAN port.
 * @param[in] dev_id device id
 * @param[in] up dot1p
 * @param[in] dp internal drop precedence
 * @return SW_OK or error code
 */
sw_error_t
fal_cosmap_up_to_ehdp_get(a_uint32_t dev_id, a_uint32_t up, a_uint32_t * dp)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_cosmap_up_to_ehdp_get(dev_id, up, dp);
    FAL_API_UNLOCK;
    return rv;
}
#endif

/**
 * @brief Set internal priority to queue mapping on one particular device.
 * @details  Comments:
 * This function is for port 1/2/3/4 which have four egress queues
 * @param[in] dev_id device id
 * @param[in] pri internal priority
 * @param[in] queue queue id
 * @return SW_OK or error code
 */
sw_error_t
fal_cosmap_pri_to_queue_set(a_uint32_t dev_id, a_uint32_t pri,
                            a_uint32_t queue)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_cosmap_pri_to_queue_set(dev_id, pri, queue);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set internal priority to queue mapping on one particular device.
 * @details  Comments:
 * This function is for port 0/5/6 which have six egress queues
 * @param[in] dev_id device id
 * @param[in] pri internal priority
 * @param[in] queue queue id
 * @return SW_OK or error code
 */
sw_error_t
fal_cosmap_pri_to_ehqueue_set(a_uint32_t dev_id, a_uint32_t pri,
                              a_uint32_t queue)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_cosmap_pri_to_ehqueue_set(dev_id, pri, queue);
    FAL_API_UNLOCK;
    return rv;
}

#ifndef IN_COSMAP_MINI
/**
 * @brief Get internal priority to queue mapping on one particular device.
 * @details  Comments:
 * This function is for port 1/2/3/4 which have four egress queues
 * @param[in] dev_id device id
 * @param[in] pri internal priority
 * @param[out] queue queue id
 * @return SW_OK or error code
 */
sw_error_t
fal_cosmap_pri_to_queue_get(a_uint32_t dev_id, a_uint32_t pri,
                            a_uint32_t * queue)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_cosmap_pri_to_queue_get(dev_id, pri, queue);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get internal priority to queue mapping on one particular device.
 * @details  Comments:
 * This function is for port 0/5/6 which have six egress queues
 * @param[in] dev_id device id
 * @param[in] pri internal priority
 * @param[in] queue queue id
 * @return SW_OK or error code
 */
sw_error_t
fal_cosmap_pri_to_ehqueue_get(a_uint32_t dev_id, a_uint32_t pri,
                              a_uint32_t * queue)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_cosmap_pri_to_ehqueue_get(dev_id, pri, queue);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set egress queue based CoS remap table on one particular device.
 * @param[in] dev_id device id
 * @param[in] tbl_id CoS remap table id
 * @param[in] tbl CoS remap table
 * @return SW_OK or error code
 */
sw_error_t
fal_cosmap_egress_remark_set(a_uint32_t dev_id, a_uint32_t tbl_id,
                             fal_egress_remark_table_t * tbl)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_cosmap_egress_remark_set(dev_id, tbl_id, tbl);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get egress queue based CoS remap table on one particular device.
 * @param[in] dev_id device id
 * @param[in] tbl_id CoS remap table id
 * @param[out] tbl CoS remap table
 * @return SW_OK or error code
 */
sw_error_t
fal_cosmap_egress_remark_get(a_uint32_t dev_id, a_uint32_t tbl_id,
                             fal_egress_remark_table_t * tbl)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_cosmap_egress_remark_get(dev_id, tbl_id, tbl);
    FAL_API_UNLOCK;
    return rv;
}
#endif

/**
 * @}
 */

