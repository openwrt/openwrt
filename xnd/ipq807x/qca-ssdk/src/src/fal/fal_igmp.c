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
* @defgroup fal_igmp FAL_IGMP
* @{
*/
#include "sw.h"
#include "fal_igmp.h"
#include "hsl_api.h"

static sw_error_t
_fal_port_igmps_status_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->port_igmps_status_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->port_igmps_status_set(dev_id, port_id, enable);
    return rv;
}


static sw_error_t
_fal_port_igmps_status_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t *enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->port_igmps_status_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->port_igmps_status_get(dev_id, port_id, enable);
    return rv;
}


static sw_error_t
_fal_igmp_mld_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->igmp_mld_cmd_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->igmp_mld_cmd_set(dev_id, cmd);
    return rv;
}


static sw_error_t
_fal_igmp_mld_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->igmp_mld_cmd_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->igmp_mld_cmd_get(dev_id, cmd);
    return rv;
}


static sw_error_t
_fal_port_igmp_mld_join_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->port_igmp_join_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->port_igmp_join_set(dev_id, port_id, enable);
    return rv;
}


static sw_error_t
_fal_port_igmp_mld_join_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->port_igmp_join_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->port_igmp_join_get(dev_id, port_id, enable);
    return rv;
}


static sw_error_t
_fal_port_igmp_mld_leave_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->port_igmp_leave_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->port_igmp_leave_set(dev_id, port_id, enable);
    return rv;
}


static sw_error_t
_fal_port_igmp_mld_leave_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->port_igmp_leave_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->port_igmp_leave_get(dev_id, port_id, enable);
    return rv;
}


static sw_error_t
_fal_igmp_mld_rp_set(a_uint32_t dev_id, fal_pbmp_t pts)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->igmp_rp_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->igmp_rp_set(dev_id, pts);
    return rv;
}


static sw_error_t
_fal_igmp_mld_rp_get(a_uint32_t dev_id, fal_pbmp_t * pts)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->igmp_rp_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->igmp_rp_get(dev_id, pts);
    return rv;
}


static sw_error_t
_fal_igmp_mld_entry_creat_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->igmp_entry_creat_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->igmp_entry_creat_set(dev_id, enable);
    return rv;
}


static sw_error_t
_fal_igmp_mld_entry_creat_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->igmp_entry_creat_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->igmp_entry_creat_get(dev_id, enable);
    return rv;
}

static sw_error_t
_fal_igmp_mld_entry_static_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->igmp_entry_static_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->igmp_entry_static_set(dev_id, enable);
    return rv;
}

static sw_error_t
_fal_igmp_mld_entry_static_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->igmp_entry_static_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->igmp_entry_static_get(dev_id, enable);
    return rv;
}

static sw_error_t
_fal_igmp_mld_entry_leaky_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->igmp_entry_leaky_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->igmp_entry_leaky_set(dev_id, enable);
    return rv;
}

static sw_error_t
_fal_igmp_mld_entry_leaky_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->igmp_entry_leaky_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->igmp_entry_leaky_get(dev_id, enable);
    return rv;
}

static sw_error_t
_fal_igmp_mld_entry_v3_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->igmp_entry_v3_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->igmp_entry_v3_set(dev_id, enable);
    return rv;
}

static sw_error_t
_fal_igmp_mld_entry_v3_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->igmp_entry_v3_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->igmp_entry_v3_get(dev_id, enable);
    return rv;
}

static sw_error_t
_fal_igmp_mld_entry_queue_set(a_uint32_t dev_id, a_bool_t enable, a_uint32_t queue)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->igmp_entry_queue_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->igmp_entry_queue_set(dev_id, enable, queue);
    return rv;
}

static sw_error_t
_fal_igmp_mld_entry_queue_get(a_uint32_t dev_id, a_bool_t * enable, a_uint32_t * queue)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->igmp_entry_queue_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->igmp_entry_queue_get(dev_id, enable, queue);
    return rv;
}

static sw_error_t
_fal_port_igmp_mld_learn_limit_set(a_uint32_t dev_id, fal_port_t port_id,
                                   a_bool_t enable, a_uint32_t cnt)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->port_igmp_mld_learn_limit_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->port_igmp_mld_learn_limit_set(dev_id, port_id, enable, cnt);
    return rv;
}

static sw_error_t
_fal_port_igmp_mld_learn_limit_get(a_uint32_t dev_id, fal_port_t port_id,
                                   a_bool_t * enable, a_uint32_t * cnt)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->port_igmp_mld_learn_limit_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->port_igmp_mld_learn_limit_get(dev_id, port_id, enable, cnt);
    return rv;
}

static sw_error_t
_fal_port_igmp_mld_learn_exceed_cmd_set(a_uint32_t dev_id, fal_port_t port_id,
                                        fal_fwd_cmd_t cmd)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->port_igmp_mld_learn_exceed_cmd_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->port_igmp_mld_learn_exceed_cmd_set(dev_id, port_id, cmd);
    return rv;
}

static sw_error_t
_fal_port_igmp_mld_learn_exceed_cmd_get(a_uint32_t dev_id, fal_port_t port_id,
                                        fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->port_igmp_mld_learn_exceed_cmd_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->port_igmp_mld_learn_exceed_cmd_get(dev_id, port_id, cmd);
    return rv;
}

static sw_error_t
_fal_igmp_sg_entry_set(a_uint32_t dev_id, fal_igmp_sg_entry_t * entry)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->igmp_sg_entry_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->igmp_sg_entry_set(dev_id, entry);
    return rv;
}
static sw_error_t
_fal_igmp_sg_entry_clear(a_uint32_t dev_id, fal_igmp_sg_entry_t * entry)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->igmp_sg_entry_clear)
        return SW_NOT_SUPPORTED;

    rv = p_api->igmp_sg_entry_clear(dev_id, entry);
    return rv;
}
static sw_error_t
_fal_igmp_sg_entry_show(a_uint32_t dev_id)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->igmp_sg_entry_show)
        return SW_NOT_SUPPORTED;

    rv = p_api->igmp_sg_entry_show(dev_id);
    return rv;
}
static sw_error_t
_fal_igmp_sg_entry_query(a_uint32_t dev_id, fal_igmp_sg_info_t * info)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->igmp_sg_entry_query)
        return SW_NOT_SUPPORTED;

    rv = p_api->igmp_sg_entry_query(dev_id, info);
    return rv;
}
/**
 * @brief Set igmp/mld packets snooping status on a particular port.
 * @details    Comments:
 *   After enabling igmp/mld snooping feature on a particular port all kinds
 *   igmp/mld packets received on this port would be acknowledged by hardware.
 *   Particular forwarding decision could be setted by fal_igmp_mld_cmd_set.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_igmps_status_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_port_igmps_status_set(dev_id, port_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get igmp/mld packets snooping status on particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_igmps_status_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t *enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_port_igmps_status_get(dev_id, port_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set igmp/mld packets forwarding command on a particular device.
 * @details    Comments:
 *    Particular device may only support parts of forwarding commands.
 *    This operation will take effect only after enabling igmp/mld snooping
 * @param[in] dev_id device id
 * @param[in] cmd forwarding command
 * @return SW_OK or error code
 */
sw_error_t
fal_igmp_mld_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_igmp_mld_cmd_set(dev_id, cmd);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get igmp/mld packets forwarding command on a particular device.
 * @param[in] dev_id device id
 * @param[out] cmd forwarding command
 * @return SW_OK or error code
 */
sw_error_t
fal_igmp_mld_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_igmp_mld_cmd_get(dev_id, cmd);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set igmp/mld join packets hardware acknowledgement status on particular port.
 * @details    Comments:
 *    After enabling igmp/mld join feature on a particular port hardware will
 *    dynamic learning or changing multicast entry.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_igmp_mld_join_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_port_igmp_mld_join_set(dev_id, port_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get igmp/mld join packets hardware acknowledgement status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_igmp_mld_join_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_port_igmp_mld_join_get(dev_id, port_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set igmp/mld leave packets hardware acknowledgement status on a particular port.
 * @details    Comments:
 *    After enabling igmp join feature on a particular port hardware will dynamic
 *    deleting or changing multicast entry.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_igmp_mld_leave_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_port_igmp_mld_leave_set(dev_id, port_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get igmp/mld leave packets hardware acknowledgement status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_igmp_mld_leave_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_port_igmp_mld_leave_get(dev_id, port_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set igmp/mld router ports on a particular device.
 * @details     Comments:
 *    After enabling igmp/mld join/leave feature on a particular port igmp/mld
 *    join/leave packets received on this port will be forwarded to router ports.
 * @param[in] dev_id device id
 * @param[in] pts dedicates ports
 * @return SW_OK or error code
 */
sw_error_t
fal_igmp_mld_rp_set(a_uint32_t dev_id, fal_pbmp_t pts)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_igmp_mld_rp_set(dev_id, pts);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get igmp/mld router ports on a particular device.
 * @param[in] dev_id device id
 * @param[out] pts dedicates ports
 * @return SW_OK or error code
 */
sw_error_t
fal_igmp_mld_rp_get(a_uint32_t dev_id, fal_pbmp_t * pts)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_igmp_mld_rp_get(dev_id, pts);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set the status of creating multicast entry during igmp/mld join/leave procedure.
 * @details    Comments:
 *  After enabling igmp/mld join/leave feature on a particular port if enable
 *   entry creat hardware will dynamic creat and delete multicast entry,
 *   otherwise hardware only can change destination ports of existing muticast entry.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_igmp_mld_entry_creat_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_igmp_mld_entry_creat_set(dev_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get the status of creating multicast entry during igmp/mld join/leave procedure.
 * @param[in] dev_id device id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_igmp_mld_entry_creat_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_igmp_mld_entry_creat_get(dev_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set the static status of multicast entry which learned by hardware.
 * @details    Comments:
 *  After enabling igmp/mld join/leave feature on a particular port if enable
 *   static status hardware will not age out multicast entry which leardned by hardware,
 *   otherwise hardware will age out multicast entry which leardned by hardware.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_igmp_mld_entry_static_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_igmp_mld_entry_static_set(dev_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get the static status of multicast entry which learned by hardware.
 * @param[in] dev_id device id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_igmp_mld_entry_static_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_igmp_mld_entry_static_get(dev_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set the leaky status of multicast entry which learned by hardware.
 * @details    Comments:
 *  After enabling igmp/mld join/leave feature on a particular port if enable
 *   leaky status hardware will set leaky flag of multicast entry which leardned by hardware,
 *   otherwise hardware will not set leaky flag of multicast entry which leardned by hardware.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_igmp_mld_entry_leaky_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_igmp_mld_entry_leaky_set(dev_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get the leaky status of multicast entry which learned by hardware.
 * @param[in] dev_id device id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_igmp_mld_entry_leaky_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_igmp_mld_entry_leaky_get(dev_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set igmpv3/mldv2 packets hardware acknowledgement status on a particular device.
 * @details    Comments:
 *    After enabling igmp join/leave feature on a particular port hardware will dynamic
 *    creating or changing multicast entry after receiving igmpv3/mldv2 packets.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_igmp_mld_entry_v3_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_igmp_mld_entry_v3_set(dev_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get igmpv3/mldv2 packets hardware acknowledgement status on a particular device.
 * @param[in] dev_id device id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_igmp_mld_entry_v3_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_igmp_mld_entry_v3_get(dev_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set the queue status of multicast entry which learned by hardware.
 * @details    Comments:
 *  After enabling igmp/mld join/leave feature on a particular port if enable
 *   leaky status hardware will set queue flag of multicast entry which leardned by hardware,
 *   otherwise hardware will not set queue flag of multicast entry which leardned by hardware.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @param[in] queue queue id
 * @return SW_OK or error code
 */
sw_error_t
fal_igmp_mld_entry_queue_set(a_uint32_t dev_id, a_bool_t enable, a_uint32_t queue)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_igmp_mld_entry_queue_set(dev_id, enable, queue);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get the queue status of multicast entry which learned by hardware.
 * @param[in] dev_id device id
 * @param[out] enable A_TRUE or A_FALSE
 * @param[out] queue queue id
 * @return SW_OK or error code
 */
sw_error_t
fal_igmp_mld_entry_queue_get(a_uint32_t dev_id, a_bool_t * enable, a_uint32_t * queue)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_igmp_mld_entry_queue_get(dev_id, enable, queue);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set IGMP hardware learning count limit on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @param[in] cnt limit count
 * @return SW_OK or error code
 */
sw_error_t
fal_port_igmp_mld_learn_limit_set(a_uint32_t dev_id, fal_port_t port_id,
                                  a_bool_t enable, a_uint32_t cnt)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_port_igmp_mld_learn_limit_set(dev_id, port_id, enable, cnt);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get IGMP hardware learning count limit on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @param[out] cnt limit count
 * @return SW_OK or error code
 */
sw_error_t
fal_port_igmp_mld_learn_limit_get(a_uint32_t dev_id, fal_port_t port_id,
                                  a_bool_t * enable, a_uint32_t * cnt)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_port_igmp_mld_learn_limit_get(dev_id, port_id, enable, cnt);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set IGMP hardware learning count exceed command on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] cmd forwarding command
 * @return SW_OK or error code
 */
sw_error_t
fal_port_igmp_mld_learn_exceed_cmd_set(a_uint32_t dev_id, fal_port_t port_id,
                                       fal_fwd_cmd_t cmd)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_port_igmp_mld_learn_exceed_cmd_set(dev_id, port_id, cmd);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get IGMP hardware learning count exceed command on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] cmd forwarding command
 * @return SW_OK or error code
 */
sw_error_t
fal_port_igmp_mld_learn_exceed_cmd_get(a_uint32_t dev_id, fal_port_t port_id,
                                       fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_port_igmp_mld_learn_exceed_cmd_get(dev_id, port_id, cmd);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t
fal_igmp_sg_entry_set(a_uint32_t dev_id, fal_igmp_sg_entry_t * entry)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_igmp_sg_entry_set(dev_id, entry);
    FAL_API_UNLOCK;
    return rv;
}
sw_error_t
fal_igmp_sg_entry_clear(a_uint32_t dev_id, fal_igmp_sg_entry_t * entry)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_igmp_sg_entry_clear(dev_id, entry);
    FAL_API_UNLOCK;
    return rv;
}
sw_error_t
fal_igmp_sg_entry_show(a_uint32_t dev_id)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_igmp_sg_entry_show(dev_id);
    FAL_API_UNLOCK;
    return rv;
}
sw_error_t
fal_igmp_sg_entry_query(a_uint32_t dev_id, fal_igmp_sg_info_t * info)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_igmp_sg_entry_query(dev_id, info);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @}
 */

