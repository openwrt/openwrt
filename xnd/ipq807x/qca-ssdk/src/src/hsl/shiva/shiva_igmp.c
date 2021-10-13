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
 * @defgroup shiva_igmp SHIVA_IGMP
 * @{
 */
#include "sw.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_port_prop.h"
#include "shiva_igmp.h"
#include "shiva_reg.h"

static sw_error_t
_shiva_port_igmps_status_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
    sw_error_t rv;
    a_uint32_t val;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_EXCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    if (A_TRUE == enable)
    {
        val = 1;
    }
    else if (A_FALSE == enable)
    {
        val = 0;
    }
    else
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_SET(rv, dev_id, PORT_CTL, port_id, IGMP_MLD_EN,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_shiva_port_igmps_status_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t *enable)
{
    sw_error_t rv;
    a_uint32_t val = 0;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_EXCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_GET(rv, dev_id, PORT_CTL, port_id, IGMP_MLD_EN,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (1 == val)
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
_shiva_igmp_mld_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd)
{
    sw_error_t rv;
    a_uint32_t val;

    HSL_DEV_ID_CHECK(dev_id);

    if (FAL_MAC_CPY_TO_CPU == cmd)
    {
        val = 1;
    }
    else if (FAL_MAC_RDT_TO_CPU == cmd)
    {
        val = 0;
    }
    else
    {
        return SW_NOT_SUPPORTED;
    }

    HSL_REG_FIELD_SET(rv, dev_id, QM_CTL, 0, IGMP_COPY_EN,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_shiva_igmp_mld_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;
    a_uint32_t val = 0;

    HSL_DEV_ID_CHECK(dev_id);

    HSL_REG_FIELD_GET(rv, dev_id, QM_CTL, 0, IGMP_COPY_EN,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (1 == val)
    {
        *cmd = FAL_MAC_CPY_TO_CPU;
    }
    else
    {
        *cmd = FAL_MAC_RDT_TO_CPU;
    }

    return SW_OK;
}

static sw_error_t
_shiva_port_igmp_mld_join_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
    sw_error_t rv;
    a_uint32_t val;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_EXCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    if (A_TRUE == enable)
    {
        val = 1;
    }
    else if (A_FALSE == enable)
    {
        val = 0;
    }
    else
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_SET(rv, dev_id, PORT_CTL, port_id, JOIN_EN,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_shiva_port_igmp_mld_join_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable)
{
    sw_error_t rv;
    a_uint32_t val = 0;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_EXCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_GET(rv, dev_id, PORT_CTL, port_id, JOIN_EN,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (val)
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
_shiva_port_igmp_mld_leave_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
    sw_error_t rv;
    a_uint32_t val;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_EXCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    if (A_TRUE == enable)
    {
        val = 1;
    }
    else if (A_FALSE == enable)
    {
        val = 0;
    }
    else
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_SET(rv, dev_id, PORT_CTL, port_id, LEAVE_EN,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_shiva_port_igmp_mld_leave_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable)
{
    sw_error_t rv;
    a_uint32_t val = 0;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_EXCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_GET(rv, dev_id, PORT_CTL, port_id, LEAVE_EN,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (val)
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
_shiva_igmp_mld_rp_set(a_uint32_t dev_id, fal_pbmp_t pts)
{
    sw_error_t rv;
    a_uint32_t val;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_FALSE == hsl_mports_validity_check(dev_id, pts))
    {
        return SW_BAD_PARAM;
    }
    val = pts;
    HSL_REG_FIELD_SET(rv, dev_id, FLOOD_MASK, 0, IGMP_DP,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_shiva_igmp_mld_rp_get(a_uint32_t dev_id, fal_pbmp_t * pts)
{
    sw_error_t rv;
    a_uint32_t val = 0;

    HSL_DEV_ID_CHECK(dev_id);

    HSL_REG_FIELD_GET(rv, dev_id, FLOOD_MASK, 0, IGMP_DP,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    *pts = val;
    return SW_OK;
}

static sw_error_t
_shiva_igmp_mld_entry_creat_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;
    a_uint32_t val;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE == enable)
    {
        val = 1;
    }
    else if (A_FALSE == enable)
    {
        val = 0;
    }
    else
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_SET(rv, dev_id, QM_CTL, 0, IGMP_CREAT_EN,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_shiva_igmp_mld_entry_creat_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;
    a_uint32_t val = 0;

    HSL_DEV_ID_CHECK(dev_id);

    HSL_REG_FIELD_GET(rv, dev_id, QM_CTL, 0, IGMP_CREAT_EN,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (val)
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
_shiva_igmp_mld_entry_static_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;
    a_uint32_t val;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE == enable)
    {
        val = 0xf;
    }
    else if (A_FALSE == enable)
    {
        val = 0xe;
    }
    else
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_SET(rv, dev_id, QM_CTL, 0, IGMP_JOIN_STATIC,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_shiva_igmp_mld_entry_static_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;
    a_uint32_t val = 0;

    HSL_DEV_ID_CHECK(dev_id);

    HSL_REG_FIELD_GET(rv, dev_id, QM_CTL, 0, IGMP_JOIN_STATIC,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (0xf == val)
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
_shiva_igmp_mld_entry_leaky_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;
    a_uint32_t val;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE == enable)
    {
        val = 1;
    }
    else if (A_FALSE == enable)
    {
        val = 0;
    }
    else
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_SET(rv, dev_id, QM_CTL, 0, IGMP_JOIN_LEAKY,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_shiva_igmp_mld_entry_leaky_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;
    a_uint32_t val = 0;

    HSL_DEV_ID_CHECK(dev_id);

    HSL_REG_FIELD_GET(rv, dev_id, QM_CTL, 0, IGMP_JOIN_LEAKY,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (val)
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
_shiva_igmp_mld_entry_v3_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;
    a_uint32_t val;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE == enable)
    {
        val = 1;
    }
    else if (A_FALSE == enable)
    {
        val = 0;
    }
    else
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_SET(rv, dev_id, QM_CTL, 0, IGMP_V3_EN,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_shiva_igmp_mld_entry_v3_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;
    a_uint32_t val = 0;

    HSL_DEV_ID_CHECK(dev_id);

    HSL_REG_FIELD_GET(rv, dev_id, QM_CTL, 0, IGMP_V3_EN,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (val)
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
_shiva_igmp_mld_entry_queue_set(a_uint32_t dev_id, a_bool_t enable, a_uint32_t queue)
{
    sw_error_t rv;
    a_uint32_t entry = 0;
    hsl_dev_t *p_dev;

    HSL_DEV_ID_CHECK(dev_id);

    HSL_REG_ENTRY_GET(rv, dev_id, QM_CTL, 0,
                      (a_uint8_t *) (&entry), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (A_TRUE == enable)
    {
        SW_SET_REG_BY_FIELD(QM_CTL, IGMP_PRI_EN, 1, entry);
        SW_RTN_ON_NULL(p_dev = hsl_dev_ptr_get(dev_id));
        if (queue >= p_dev->nr_queue)
        {
            return SW_BAD_PARAM;
        }
        SW_SET_REG_BY_FIELD(QM_CTL, IGMP_PRI, queue, entry);
    }
    else if (A_FALSE == enable)
    {
        SW_SET_REG_BY_FIELD(QM_CTL, IGMP_PRI_EN, 0, entry);
        SW_SET_REG_BY_FIELD(QM_CTL, IGMP_PRI, 0, entry);
    }
    else
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_ENTRY_SET(rv, dev_id, QM_CTL, 0,
                      (a_uint8_t *) (&entry), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_shiva_igmp_mld_entry_queue_get(a_uint32_t dev_id, a_bool_t * enable, a_uint32_t * queue)
{
    sw_error_t rv;
    a_uint32_t entry = 0, data;

    HSL_DEV_ID_CHECK(dev_id);

    HSL_REG_ENTRY_GET(rv, dev_id, QM_CTL, 0,
                      (a_uint8_t *) (&entry), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    SW_GET_FIELD_BY_REG(QM_CTL, IGMP_PRI_EN, data, entry);
    if (data)
    {
        *enable = A_TRUE;
        SW_GET_FIELD_BY_REG(QM_CTL, IGMP_PRI, data, entry);
        *queue  = data;
    }
    else
    {
        *enable = A_FALSE;
        *queue  = 0;
    }

    return SW_OK;
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
HSL_LOCAL sw_error_t
shiva_port_igmps_status_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_port_igmps_status_set(dev_id, port_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get igmp/mld packets snooping status on particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_port_igmps_status_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t *enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_port_igmps_status_get(dev_id, port_id, enable);
    HSL_API_UNLOCK;
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
HSL_LOCAL sw_error_t
shiva_igmp_mld_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_igmp_mld_cmd_set(dev_id, cmd);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get igmp/mld packets forwarding command on a particular device.
 * @param[in] dev_id device id
 * @param[out] cmd forwarding command
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_igmp_mld_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_igmp_mld_cmd_get(dev_id, cmd);
    HSL_API_UNLOCK;
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
HSL_LOCAL sw_error_t
shiva_port_igmp_mld_join_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_port_igmp_mld_join_set(dev_id, port_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get igmp/mld join packets hardware acknowledgement status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_port_igmp_mld_join_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_port_igmp_mld_join_get(dev_id, port_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set igmp/mld leave packets hardware acknowledgement status on a particular port.
 * @details    Comments:
 *    After enabling igmp leave feature on a particular port hardware will dynamic
 *    deleting or changing multicast entry.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_port_igmp_mld_leave_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_port_igmp_mld_leave_set(dev_id, port_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get igmp/mld leave packets hardware acknowledgement status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_port_igmp_mld_leave_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_port_igmp_mld_leave_get(dev_id, port_id, enable);
    HSL_API_UNLOCK;
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
HSL_LOCAL sw_error_t
shiva_igmp_mld_rp_set(a_uint32_t dev_id, fal_pbmp_t pts)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_igmp_mld_rp_set(dev_id, pts);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get igmp/mld router ports on a particular device.
 * @param[in] dev_id device id
 * @param[out] pts dedicates ports
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_igmp_mld_rp_get(a_uint32_t dev_id, fal_pbmp_t * pts)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_igmp_mld_rp_get(dev_id, pts);
    HSL_API_UNLOCK;
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
HSL_LOCAL sw_error_t
shiva_igmp_mld_entry_creat_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_igmp_mld_entry_creat_set(dev_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get the status of creating multicast entry during igmp/mld join/leave procedure.
 * @param[in] dev_id device id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_igmp_mld_entry_creat_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_igmp_mld_entry_creat_get(dev_id, enable);
    HSL_API_UNLOCK;
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
HSL_LOCAL sw_error_t
shiva_igmp_mld_entry_static_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_igmp_mld_entry_static_set(dev_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get the static status of multicast entry which learned by hardware.
 * @param[in] dev_id device id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_igmp_mld_entry_static_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_igmp_mld_entry_static_get(dev_id, enable);
    HSL_API_UNLOCK;
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
HSL_LOCAL sw_error_t
shiva_igmp_mld_entry_leaky_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_igmp_mld_entry_leaky_set(dev_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get the leaky status of multicast entry which learned by hardware.
 * @param[in] dev_id device id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_igmp_mld_entry_leaky_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_igmp_mld_entry_leaky_get(dev_id, enable);
    HSL_API_UNLOCK;
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
HSL_LOCAL sw_error_t
shiva_igmp_mld_entry_v3_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_igmp_mld_entry_v3_set(dev_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get igmpv3/mldv2 packets hardware acknowledgement status on a particular device.
 * @param[in] dev_id device id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_igmp_mld_entry_v3_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_igmp_mld_entry_v3_get(dev_id, enable);
    HSL_API_UNLOCK;
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
HSL_LOCAL sw_error_t
shiva_igmp_mld_entry_queue_set(a_uint32_t dev_id, a_bool_t enable, a_uint32_t queue)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_igmp_mld_entry_queue_set(dev_id, enable, queue);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get the queue status of multicast entry which learned by hardware.
 * @param[in] dev_id device id
 * @param[out] enable A_TRUE or A_FALSE
 * @param[out] queue queue id
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_igmp_mld_entry_queue_get(a_uint32_t dev_id, a_bool_t * enable, a_uint32_t * queue)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_igmp_mld_entry_queue_get(dev_id, enable, queue);
    HSL_API_UNLOCK;
    return rv;
}

sw_error_t
shiva_igmp_init(a_uint32_t dev_id)
{
    HSL_DEV_ID_CHECK(dev_id);

#ifndef HSL_STANDALONG
    {
        hsl_api_t *p_api;

        SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

        p_api->port_igmps_status_set = shiva_port_igmps_status_set;
        p_api->port_igmps_status_get = shiva_port_igmps_status_get;
        p_api->igmp_mld_cmd_set = shiva_igmp_mld_cmd_set;
        p_api->igmp_mld_cmd_get = shiva_igmp_mld_cmd_get;
        p_api->port_igmp_join_set = shiva_port_igmp_mld_join_set;
        p_api->port_igmp_join_get = shiva_port_igmp_mld_join_get;
        p_api->port_igmp_leave_set = shiva_port_igmp_mld_leave_set;
        p_api->port_igmp_leave_get = shiva_port_igmp_mld_leave_get;
        p_api->igmp_rp_set = shiva_igmp_mld_rp_set;
        p_api->igmp_rp_get = shiva_igmp_mld_rp_get;
        p_api->igmp_entry_creat_set = shiva_igmp_mld_entry_creat_set;
        p_api->igmp_entry_creat_get = shiva_igmp_mld_entry_creat_get;
        p_api->igmp_entry_static_set = shiva_igmp_mld_entry_static_set;
        p_api->igmp_entry_static_get = shiva_igmp_mld_entry_static_get;
        p_api->igmp_entry_leaky_set = shiva_igmp_mld_entry_leaky_set;
        p_api->igmp_entry_leaky_get = shiva_igmp_mld_entry_leaky_get;
        p_api->igmp_entry_v3_set = shiva_igmp_mld_entry_v3_set;
        p_api->igmp_entry_v3_get = shiva_igmp_mld_entry_v3_get;
        p_api->igmp_entry_queue_set = shiva_igmp_mld_entry_queue_set;
        p_api->igmp_entry_queue_get = shiva_igmp_mld_entry_queue_get;
    }
#endif

    return SW_OK;
}

/**
 * @}
 */

