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
 * @defgroup garuda_misc GARUDA_MISC
 * @{
 */

#include "sw.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_port_prop.h"
#include "garuda_misc.h"
#include "garuda_reg.h"

#define GARUDA_MAX_FRMAE_SIZE      9216


static sw_error_t
_garuda_arp_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    a_uint32_t data;
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

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

    HSL_REG_FIELD_SET(rv, dev_id, ADDR_TABLE_CTL, 0, ARP_EN,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    return rv;
}


static sw_error_t
_garuda_arp_status_get(a_uint32_t dev_id, a_bool_t *enable)
{
    a_uint32_t data;
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    HSL_REG_FIELD_GET(rv, dev_id, ADDR_TABLE_CTL, 0, ARP_EN,
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
_garuda_frame_max_size_set(a_uint32_t dev_id, a_uint32_t size)
{
    a_uint32_t data;
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    if (GARUDA_MAX_FRMAE_SIZE < size)
    {
        return SW_BAD_PARAM;
    }

    data = size;
    HSL_REG_FIELD_SET(rv, dev_id, GLOBAL_CTL, 0, MAX_FRAME_SIZE,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    return rv;
}


static sw_error_t
_garuda_frame_max_size_get(a_uint32_t dev_id, a_uint32_t *size)
{
    a_uint32_t data;
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    HSL_REG_FIELD_GET(rv, dev_id, GLOBAL_CTL, 0, MAX_FRAME_SIZE,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    *size = data;
    return SW_OK;
}

static sw_error_t
_garuda_port_unk_sa_cmd_set(a_uint32_t dev_id, fal_port_t port_id,
                            fal_fwd_cmd_t cmd)
{
    sw_error_t rv;
    a_uint32_t data;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_ENTRY_GET(rv, dev_id, PORT_CTL, port_id, (a_uint8_t *) (&data),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (FAL_MAC_FRWRD == cmd)
    {
        SW_SET_REG_BY_FIELD(PORT_CTL, PORT_LOCK_EN, 0, data);
    }
    else if (FAL_MAC_DROP == cmd)
    {
        SW_SET_REG_BY_FIELD(PORT_CTL, PORT_LOCK_EN, 1, data);
        SW_SET_REG_BY_FIELD(PORT_CTL, LOCK_DROP_EN, 1, data);
    }
    else if (FAL_MAC_RDT_TO_CPU == cmd)
    {
        SW_SET_REG_BY_FIELD(PORT_CTL, PORT_LOCK_EN, 1, data);
        SW_SET_REG_BY_FIELD(PORT_CTL, LOCK_DROP_EN, 0, data);
    }
    else
    {
        return SW_NOT_SUPPORTED;
    }

    HSL_REG_ENTRY_SET(rv, dev_id, PORT_CTL, port_id, (a_uint8_t *) (&data),
                      sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_garuda_port_unk_sa_cmd_get(a_uint32_t dev_id, fal_port_t port_id,
                            fal_fwd_cmd_t * action)
{
    sw_error_t rv;
    a_uint32_t data;
    a_uint32_t port_lock_en, port_drop_en;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_ENTRY_GET(rv, dev_id, PORT_CTL, port_id, (a_uint8_t *) (&data),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    SW_GET_FIELD_BY_REG(PORT_CTL, PORT_LOCK_EN, port_lock_en, data);
    SW_GET_FIELD_BY_REG(PORT_CTL, LOCK_DROP_EN, port_drop_en, data);

    if (1 == port_lock_en)
    {
        if (1 == port_drop_en)
        {
            *action = FAL_MAC_DROP;
        }
        else
        {
            *action = FAL_MAC_RDT_TO_CPU;
        }
    }
    else
    {
        *action = FAL_MAC_FRWRD;
    }

    return SW_OK;
}

static sw_error_t
_garuda_port_unk_uc_filter_set(a_uint32_t dev_id, fal_port_t port_id,
                               a_bool_t enable)
{
    sw_error_t rv;
    a_uint32_t data;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_GET(rv, dev_id, FLOOD_MASK, 0, UNI_FLOOD_DP,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (A_TRUE == enable)
    {
        data &= (~((a_uint32_t)0x1 << port_id));
    }
    else if (A_FALSE == enable)
    {
        data |= (0x1 << port_id);
    }
    else
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_SET(rv, dev_id, FLOOD_MASK, 0, UNI_FLOOD_DP,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_garuda_port_unk_uc_filter_get(a_uint32_t dev_id, fal_port_t port_id,
                               a_bool_t * enable)
{
    sw_error_t rv;
    a_uint32_t reg, field;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_GET(rv, dev_id, FLOOD_MASK, 0, UNI_FLOOD_DP,
                      (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    field = reg & (0x1 << port_id);
    if (field)
    {
        *enable = A_FALSE;
    }
    else
    {
        *enable = A_TRUE;
    }

    return SW_OK;
}

static sw_error_t
_garuda_port_unk_mc_filter_set(a_uint32_t dev_id, fal_port_t port_id,
                               a_bool_t enable)
{
    sw_error_t rv;
    a_uint32_t data;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_GET(rv, dev_id, FLOOD_MASK, 0, MUL_FLOOD_DP,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (A_TRUE == enable)
    {
        data &= (~((a_uint32_t)0x1 << port_id));
    }
    else if (A_FALSE == enable)
    {
        data |= (0x1 << port_id);
    }
    else
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_SET(rv, dev_id, FLOOD_MASK, 0, MUL_FLOOD_DP,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_garuda_port_unk_mc_filter_get(a_uint32_t dev_id, fal_port_t port_id,
                               a_bool_t * enable)
{
    sw_error_t rv;
    a_uint32_t reg, field;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_GET(rv, dev_id, FLOOD_MASK, 0, MUL_FLOOD_DP,
                      (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    field = reg & (0x1 << port_id);
    if (field)
    {
        *enable = A_FALSE;
    }
    else
    {
        *enable = A_TRUE;
    }

    return SW_OK;
}

static sw_error_t
_garuda_cpu_port_status_set(a_uint32_t dev_id, a_bool_t enable)
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

    HSL_REG_FIELD_SET(rv, dev_id, CPU_PORT, 0, CPU_PORT_EN,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_garuda_cpu_port_status_get(a_uint32_t dev_id, a_bool_t *enable)
{
    sw_error_t rv;
    a_uint32_t val;

    HSL_DEV_ID_CHECK(dev_id);

    HSL_REG_FIELD_GET(rv, dev_id, CPU_PORT, 0, CPU_PORT_EN,
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
_garuda_bc_to_cpu_port_set(a_uint32_t dev_id, a_bool_t enable)
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

    HSL_REG_FIELD_SET(rv, dev_id, FLOOD_MASK, 0, BROAD_TO_CPU,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    return rv;
}


static sw_error_t
_garuda_bc_to_cpu_port_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;
    a_uint32_t val;

    HSL_REG_FIELD_GET(rv, dev_id, FLOOD_MASK, 0, BROAD_TO_CPU,
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
_garuda_pppoe_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd)
{
    sw_error_t rv;
    a_uint32_t val;

    HSL_DEV_ID_CHECK(dev_id);

    if (FAL_MAC_FRWRD == cmd)
    {
        val = 0;
    }
    else if (FAL_MAC_RDT_TO_CPU == cmd)
    {
        val = 1;
    }
    else
    {
        return SW_NOT_SUPPORTED;
    }

    HSL_REG_FIELD_SET(rv, dev_id, QM_CTL, 0, PPPOE_RDT_EN,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    return rv;
}


static sw_error_t
_garuda_pppoe_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;
    a_uint32_t val;

    HSL_DEV_ID_CHECK(dev_id);

    HSL_REG_FIELD_GET(rv, dev_id, QM_CTL, 0, PPPOE_RDT_EN,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (1 == val)
    {
        *cmd = FAL_MAC_RDT_TO_CPU;
    }
    else
    {
        *cmd = FAL_MAC_FRWRD;
    }

    return SW_OK;
}


static sw_error_t
_garuda_pppoe_status_set(a_uint32_t dev_id, a_bool_t enable)
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

    HSL_REG_FIELD_SET(rv, dev_id, QM_CTL, 0, PPPOE_EN,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_garuda_pppoe_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;
    a_uint32_t val;

    HSL_DEV_ID_CHECK(dev_id);

    HSL_REG_FIELD_GET(rv, dev_id, QM_CTL, 0, PPPOE_EN,
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
_garuda_port_dhcp_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
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

    HSL_REG_FIELD_SET(rv, dev_id, PORT_CTL, port_id, DHCP_EN,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_garuda_port_dhcp_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable)
{
    sw_error_t rv;
    a_uint32_t val;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_GET(rv, dev_id, PORT_CTL, port_id, DHCP_EN,
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

/**
 * @brief Set arp packets hardware acknowledgement status on a particular device.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
garuda_arp_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _garuda_arp_status_set(dev_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get arp packets hardware acknowledgement status on a particular device.
 * @param[in] dev_id device id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
garuda_arp_status_get(a_uint32_t dev_id, a_bool_t *enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _garuda_arp_status_get(dev_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set max frame size which device can received on a particular device.
 * @details    Comments:
 * The granularity of packets size is byte.
 * @param[in] dev_id device id
 * @param[in] size packet size
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
garuda_frame_max_size_set(a_uint32_t dev_id, a_uint32_t size)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _garuda_frame_max_size_set(dev_id, size);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get max frame size which device can received on a particular device.
 * @details   Comments:
 *    The unit of packets size is byte.
 * @param[in] dev_id device id
 * @param[out] size packet size
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
garuda_frame_max_size_get(a_uint32_t dev_id, a_uint32_t *size)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _garuda_frame_max_size_get(dev_id, size);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set forwarding command for packets which source address is unknown on a particular port.
 * @details Comments:
 *    Particular device may only support parts of forwarding commands.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] cmd forwarding command
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
garuda_port_unk_sa_cmd_set(a_uint32_t dev_id, fal_port_t port_id,
                           fal_fwd_cmd_t cmd)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _garuda_port_unk_sa_cmd_set(dev_id, port_id, cmd);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get forwarding command for packets which source address is unknown on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] cmd forwarding command
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
garuda_port_unk_sa_cmd_get(a_uint32_t dev_id, fal_port_t port_id,
                           fal_fwd_cmd_t * action)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _garuda_port_unk_sa_cmd_get(dev_id, port_id, action);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set flooding status of unknown unicast packets on a particular port.
 * @details  Comments:
 *   If enable unknown unicast packets filter on one port then unknown
 *   unicast packets can't flood out from this port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
garuda_port_unk_uc_filter_set(a_uint32_t dev_id, fal_port_t port_id,
                              a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _garuda_port_unk_uc_filter_set(dev_id, port_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get flooding status of unknown unicast packets on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
garuda_port_unk_uc_filter_get(a_uint32_t dev_id, fal_port_t port_id,
                              a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _garuda_port_unk_uc_filter_get(dev_id, port_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set flooding status of unknown multicast packets on a particular port.
 * @details  Comments:
 *   If enable unknown multicast packets filter on one port then unknown
 *   multicast packets can't flood out from this port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
garuda_port_unk_mc_filter_set(a_uint32_t dev_id, fal_port_t port_id,
                              a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _garuda_port_unk_mc_filter_set(dev_id, port_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/** @brief Get flooding status of unknown multicast packets on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
garuda_port_unk_mc_filter_get(a_uint32_t dev_id, fal_port_t port_id,
                              a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _garuda_port_unk_mc_filter_get(dev_id, port_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set cpu port status on a particular device.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
garuda_cpu_port_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _garuda_cpu_port_status_set(dev_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get cpu port status on a particular device.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
garuda_cpu_port_status_get(a_uint32_t dev_id, a_bool_t *enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _garuda_cpu_port_status_get(dev_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set status of braodcast packets broadcasting to cpu on a particular device.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
garuda_bc_to_cpu_port_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _garuda_bc_to_cpu_port_set(dev_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get status of braodcast packets broadcasting to cpu on a particular device.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
garuda_bc_to_cpu_port_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _garuda_bc_to_cpu_port_get(dev_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

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
HSL_LOCAL sw_error_t
garuda_pppoe_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _garuda_pppoe_cmd_set(dev_id, cmd);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get pppoe packets forwarding command on a particular device.
 * @param[in] dev_id device id
 * @param[out] cmd forwarding command
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
garuda_pppoe_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _garuda_pppoe_cmd_get(dev_id, cmd);
    HSL_API_UNLOCK;
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
HSL_LOCAL sw_error_t
garuda_pppoe_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _garuda_pppoe_status_set(dev_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get pppoe packets hardware acknowledgement status on a particular device.
 * @param[in] dev_id device id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
garuda_pppoe_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _garuda_pppoe_status_get(dev_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set dhcp packets hardware acknowledgement status on a particular device.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
garuda_port_dhcp_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _garuda_port_dhcp_set(dev_id, port_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get dhcp packets hardware acknowledgement status on particular device.
 * @param[in] dev_id device id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
garuda_port_dhcp_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _garuda_port_dhcp_get(dev_id, port_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

sw_error_t
garuda_misc_init(a_uint32_t dev_id)
{
    HSL_DEV_ID_CHECK(dev_id);

#ifndef HSL_STANDALONG
    {
        hsl_api_t *p_api;

        SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

        p_api->arp_status_set = garuda_arp_status_set;
        p_api->arp_status_get = garuda_arp_status_get;
        p_api->frame_max_size_set = garuda_frame_max_size_set;
        p_api->frame_max_size_get = garuda_frame_max_size_get;
        p_api->port_unk_sa_cmd_set = garuda_port_unk_sa_cmd_set;
        p_api->port_unk_sa_cmd_get = garuda_port_unk_sa_cmd_get;
        p_api->port_unk_uc_filter_set = garuda_port_unk_uc_filter_set;
        p_api->port_unk_uc_filter_get = garuda_port_unk_uc_filter_get;
        p_api->port_unk_mc_filter_set = garuda_port_unk_mc_filter_set;
        p_api->port_unk_mc_filter_get = garuda_port_unk_mc_filter_get;
        p_api->cpu_port_status_set = garuda_cpu_port_status_set;
        p_api->cpu_port_status_get = garuda_cpu_port_status_get;
        p_api->bc_to_cpu_port_set = garuda_bc_to_cpu_port_set;
        p_api->bc_to_cpu_port_get = garuda_bc_to_cpu_port_get;
        p_api->pppoe_cmd_set = garuda_pppoe_cmd_set;
        p_api->pppoe_cmd_get = garuda_pppoe_cmd_get;
        p_api->pppoe_status_set = garuda_pppoe_status_set;
        p_api->pppoe_status_get = garuda_pppoe_status_get;
        p_api->port_dhcp_set = garuda_port_dhcp_set;
        p_api->port_dhcp_get = garuda_port_dhcp_get;
    }
#endif

    return SW_OK;
}

/**
 * @}
 */

