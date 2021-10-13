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
 * @defgroup shiva_qos SHIVA_QOS
 * @{
 */
#include "sw.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_port_prop.h"
#include "shiva_qos.h"
#include "shiva_reg.h"

#define SHIVA_QOS_QUEUE_TX_BUFFER_MAX 60
#define SHIVA_QOS_PORT_TX_BUFFER_MAX  252
#define SHIVA_QOS_PORT_RX_BUFFER_MAX  60

//#define SHIVA_MIN_QOS_MODE_PRI 0
#define SHIVA_MAX_QOS_MODE_PRI 3

static sw_error_t
_shiva_qos_queue_tx_buf_status_set(a_uint32_t dev_id, fal_port_t port_id,
                                   a_bool_t enable)
{
    a_uint32_t val = 0;
    sw_error_t rv;

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

    HSL_REG_FIELD_SET(rv, dev_id, QUEUE_CTL, port_id, QUEUE_DESC_EN,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_shiva_qos_queue_tx_buf_status_get(a_uint32_t dev_id, fal_port_t port_id,
                                   a_bool_t * enable)
{
    a_uint32_t val = 0;
    sw_error_t rv;

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_GET(rv, dev_id, QUEUE_CTL, port_id, QUEUE_DESC_EN,
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
_shiva_qos_port_tx_buf_status_set(a_uint32_t dev_id, fal_port_t port_id,
                                  a_bool_t enable)
{
    a_uint32_t val = 0;
    sw_error_t rv;

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

    HSL_REG_FIELD_SET(rv, dev_id, QUEUE_CTL, port_id, PORT_DESC_EN,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_shiva_qos_port_tx_buf_status_get(a_uint32_t dev_id, fal_port_t port_id,
                                  a_bool_t * enable)
{
    a_uint32_t val = 0;
    sw_error_t rv;

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_GET(rv, dev_id, QUEUE_CTL, port_id, PORT_DESC_EN,
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
_shiva_qos_queue_tx_buf_nr_set(a_uint32_t dev_id, fal_port_t port_id,
                               fal_queue_t queue_id, a_uint32_t * number)
{
    a_uint32_t val = 0;
    sw_error_t rv;

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    if (SHIVA_QOS_QUEUE_TX_BUFFER_MAX < *number)
    {
        return SW_BAD_PARAM;
    }

    val = *number / 4;
    *number = val << 2;

    if (0 == queue_id)
    {
        HSL_REG_FIELD_SET(rv, dev_id, QUEUE_CTL, port_id, QUEUE0_DESC_NR,
                          (a_uint8_t *) (&val), sizeof (a_uint32_t));
    }
    else if (1 == queue_id)
    {
        HSL_REG_FIELD_SET(rv, dev_id, QUEUE_CTL, port_id, QUEUE1_DESC_NR,
                          (a_uint8_t *) (&val), sizeof (a_uint32_t));
    }
    else if (2 == queue_id)
    {
        HSL_REG_FIELD_SET(rv, dev_id, QUEUE_CTL, port_id, QUEUE2_DESC_NR,
                          (a_uint8_t *) (&val), sizeof (a_uint32_t));
    }
    else if (3 == queue_id)
    {
        HSL_REG_FIELD_SET(rv, dev_id, QUEUE_CTL, port_id, QUEUE3_DESC_NR,
                          (a_uint8_t *) (&val), sizeof (a_uint32_t));
    }
    else
    {
        return SW_BAD_PARAM;
    }

    return rv;
}

static sw_error_t
_shiva_qos_queue_tx_buf_nr_get(a_uint32_t dev_id, fal_port_t port_id,
                               fal_queue_t queue_id, a_uint32_t * number)
{
    a_uint32_t val = 0;
    sw_error_t rv;

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    if (0 == queue_id)
    {
        HSL_REG_FIELD_GET(rv, dev_id, QUEUE_CTL, port_id, QUEUE0_DESC_NR,
                          (a_uint8_t *) (&val), sizeof (a_uint32_t));
    }
    else if (1 == queue_id)
    {
        HSL_REG_FIELD_GET(rv, dev_id, QUEUE_CTL, port_id, QUEUE1_DESC_NR,
                          (a_uint8_t *) (&val), sizeof (a_uint32_t));
    }
    else if (2 == queue_id)
    {
        HSL_REG_FIELD_GET(rv, dev_id, QUEUE_CTL, port_id, QUEUE2_DESC_NR,
                          (a_uint8_t *) (&val), sizeof (a_uint32_t));
    }
    else if (3 == queue_id)
    {
        HSL_REG_FIELD_GET(rv, dev_id, QUEUE_CTL, port_id, QUEUE3_DESC_NR,
                          (a_uint8_t *) (&val), sizeof (a_uint32_t));
    }
    else
    {
        return SW_BAD_PARAM;
    }
    SW_RTN_ON_ERROR(rv);

    *number = val << 2;
    return SW_OK;
}

static sw_error_t
_shiva_qos_port_tx_buf_nr_set(a_uint32_t dev_id, fal_port_t port_id,
                              a_uint32_t * number)
{
    a_uint32_t val = 0;
    sw_error_t rv;

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    if (SHIVA_QOS_PORT_TX_BUFFER_MAX < *number)
    {
        return SW_BAD_PARAM;
    }

    val = *number / 4;
    *number = val << 2;
    HSL_REG_FIELD_SET(rv, dev_id, QUEUE_CTL, port_id, PORT_DESC_NR,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_shiva_qos_port_tx_buf_nr_get(a_uint32_t dev_id, fal_port_t port_id,
                              a_uint32_t * number)
{
    a_uint32_t val = 0;
    sw_error_t rv;

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_GET(rv, dev_id, QUEUE_CTL, port_id, PORT_DESC_NR,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    *number = val << 2;
    return SW_OK;
}

static sw_error_t
_shiva_qos_port_rx_buf_nr_set(a_uint32_t dev_id, fal_port_t port_id,
                              a_uint32_t * number)
{
    a_uint32_t val = 0;
    sw_error_t rv;

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    if (SHIVA_QOS_PORT_RX_BUFFER_MAX < *number)
    {
        return SW_BAD_PARAM;
    }

    val = *number / 4;
    *number = val << 2;
    HSL_REG_FIELD_SET(rv, dev_id, QUEUE_CTL, port_id, PORT_IN_DESC_EN,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_shiva_qos_port_rx_buf_nr_get(a_uint32_t dev_id, fal_port_t port_id,
                              a_uint32_t * number)
{
    a_uint32_t val = 0;
    sw_error_t rv;

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_GET(rv, dev_id, QUEUE_CTL, port_id, PORT_IN_DESC_EN,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    *number = val << 2;
    return SW_OK;
}

static sw_error_t
_shiva_cosmap_up_queue_set(a_uint32_t dev_id, a_uint32_t up,
                           fal_queue_t queue)
{
    sw_error_t rv;
    a_uint32_t val;
    hsl_dev_t *p_dev = NULL;

    HSL_DEV_ID_CHECK(dev_id);

    if (FAL_DOT1P_MAX < up)
    {
        return SW_BAD_PARAM;
    }

    SW_RTN_ON_NULL(p_dev = hsl_dev_ptr_get(dev_id));
    if (p_dev->nr_queue <= queue)
    {
        return SW_BAD_PARAM;
    }

    val = queue;
    HSL_REG_FIELD_GEN_SET(rv, dev_id, TAG_PRI_MAPPING_OFFSET, 2,
                          (a_uint16_t) (up << 1), (a_uint8_t *) (&val),
                          sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_shiva_cosmap_up_queue_get(a_uint32_t dev_id, a_uint32_t up,
                           fal_queue_t * queue)
{
    sw_error_t rv;
    a_uint32_t val = 0;

    HSL_DEV_ID_CHECK(dev_id);

    if (FAL_DOT1P_MAX < up)
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_GEN_GET(rv, dev_id, TAG_PRI_MAPPING_OFFSET, 2,
                          (a_uint16_t) (up << 1), (a_uint8_t *) (&val),
                          sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    *queue = val;
    return SW_OK;
}

static sw_error_t
_shiva_cosmap_dscp_queue_set(a_uint32_t dev_id, a_uint32_t dscp,
                             fal_queue_t queue)
{
    sw_error_t rv;
    a_uint32_t val;
    a_uint32_t offsetaddr;
    a_uint16_t fieldoffset;
    hsl_dev_t *p_dev = NULL;

    HSL_DEV_ID_CHECK(dev_id);

    if (FAL_DSCP_MAX < dscp)
    {
        return SW_BAD_PARAM;
    }

    SW_RTN_ON_NULL(p_dev = hsl_dev_ptr_get(dev_id));
    if (p_dev->nr_queue <= queue)
    {
        return SW_BAD_PARAM;
    }

    offsetaddr = (dscp >> 4) << 2;
    fieldoffset = (dscp & 0xf) << 1;

    val = queue;
    HSL_REG_FIELD_GEN_SET(rv, dev_id, (IP_PRI_MAPPING_OFFSET + offsetaddr),
                          2, fieldoffset, (a_uint8_t *) (&val), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_shiva_cosmap_dscp_queue_get(a_uint32_t dev_id, a_uint32_t dscp,
                             fal_queue_t * queue)
{
    sw_error_t rv;
    a_uint32_t val = 0;
    a_uint32_t offsetaddr;
    a_uint16_t fieldoffset;

    HSL_DEV_ID_CHECK(dev_id);

    if (FAL_DSCP_MAX < dscp)
    {
        return SW_BAD_PARAM;
    }

    offsetaddr = (dscp / 16) << 2;
    fieldoffset = (dscp & 0xf) << 1;

    HSL_REG_FIELD_GEN_GET(rv, dev_id, (IP_PRI_MAPPING_OFFSET + offsetaddr),
                          2, fieldoffset, (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    *queue = val;
    return SW_OK;
}

static sw_error_t
_shiva_qos_port_mode_set(a_uint32_t dev_id, fal_port_t port_id,
                         fal_qos_mode_t mode, a_bool_t enable)
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

    if (FAL_QOS_DA_MODE == mode)
    {
        HSL_REG_FIELD_SET(rv, dev_id, PRI_CTL, port_id, DA_PRI_EN,
                          (a_uint8_t *) (&val), sizeof (a_uint32_t));
    }
    else if (FAL_QOS_UP_MODE == mode)
    {
        HSL_REG_FIELD_SET(rv, dev_id, PRI_CTL, port_id, VLAN_PRI_EN,
                          (a_uint8_t *) (&val), sizeof (a_uint32_t));
    }
    else if (FAL_QOS_DSCP_MODE == mode)
    {
        HSL_REG_FIELD_SET(rv, dev_id, PRI_CTL, port_id, IP_PRI_EN,
                          (a_uint8_t *) (&val), sizeof (a_uint32_t));
    }
    else if (FAL_QOS_PORT_MODE == mode)
    {
        HSL_REG_FIELD_SET(rv, dev_id, PRI_CTL, port_id, PORT_PRI_EN,
                          (a_uint8_t *) (&val), sizeof (a_uint32_t));
    }
    else
    {
        return SW_BAD_PARAM;
    }

    return rv;
}

static sw_error_t
_shiva_qos_port_mode_get(a_uint32_t dev_id, fal_port_t port_id,
                         fal_qos_mode_t mode, a_bool_t * enable)
{
    sw_error_t rv;
    a_uint32_t val = 0;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    if (FAL_QOS_DA_MODE == mode)
    {
        HSL_REG_FIELD_GET(rv, dev_id, PRI_CTL, port_id, DA_PRI_EN,
                          (a_uint8_t *) (&val), sizeof (a_uint32_t));
    }
    else if (FAL_QOS_UP_MODE == mode)
    {
        HSL_REG_FIELD_GET(rv, dev_id, PRI_CTL, port_id, VLAN_PRI_EN,
                          (a_uint8_t *) (&val), sizeof (a_uint32_t));
    }
    else if (FAL_QOS_DSCP_MODE == mode)
    {
        HSL_REG_FIELD_GET(rv, dev_id, PRI_CTL, port_id, IP_PRI_EN,
                          (a_uint8_t *) (&val), sizeof (a_uint32_t));
    }
    else if (FAL_QOS_PORT_MODE == mode)
    {
        HSL_REG_FIELD_GET(rv, dev_id, PRI_CTL, port_id, PORT_PRI_EN,
                          (a_uint8_t *) (&val), sizeof (a_uint32_t));
    }
    else
    {
        return SW_BAD_PARAM;
    }
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
_shiva_qos_port_mode_pri_set(a_uint32_t dev_id, fal_port_t port_id,
                             fal_qos_mode_t mode, a_uint32_t pri)
{
    sw_error_t rv;
    a_uint32_t val = 0;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    if (SHIVA_MAX_QOS_MODE_PRI < pri)
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_ENTRY_GET(rv, dev_id, PRI_CTL, port_id, (a_uint8_t *) (&val),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (FAL_QOS_DA_MODE == mode)
    {
        SW_SET_REG_BY_FIELD(PRI_CTL, DA_PRI_SEL, pri, val);
    }
    else if (FAL_QOS_UP_MODE == mode)
    {
        SW_SET_REG_BY_FIELD(PRI_CTL, VLAN_PRI_SEL, pri, val);
    }
    else if (FAL_QOS_DSCP_MODE == mode)
    {
        SW_SET_REG_BY_FIELD(PRI_CTL, IP_PRI_SEL, pri, val);
    }
    else if (FAL_QOS_PORT_MODE == mode)
    {
        SW_SET_REG_BY_FIELD(PRI_CTL, PORT_PRI_SEL, pri, val);
    }
    else
    {
        return SW_NOT_SUPPORTED;
    }

    HSL_REG_ENTRY_SET(rv, dev_id, PRI_CTL, port_id, (a_uint8_t *) (&val),
                      sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_shiva_qos_port_mode_pri_get(a_uint32_t dev_id, fal_port_t port_id,
                             fal_qos_mode_t mode, a_uint32_t * pri)
{
    sw_error_t rv;
    a_uint32_t entry = 0, f_val;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_ENTRY_GET(rv, dev_id, PRI_CTL, port_id, (a_uint8_t *) (&entry),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (FAL_QOS_DA_MODE == mode)
    {
        SW_GET_FIELD_BY_REG(PRI_CTL, DA_PRI_SEL, f_val, entry);
    }
    else if (FAL_QOS_UP_MODE == mode)
    {
        SW_GET_FIELD_BY_REG(PRI_CTL, VLAN_PRI_SEL, f_val, entry);
    }
    else if (FAL_QOS_DSCP_MODE == mode)
    {
        SW_GET_FIELD_BY_REG(PRI_CTL, IP_PRI_SEL, f_val, entry);
    }
    else if (FAL_QOS_PORT_MODE == mode)
    {
        SW_GET_FIELD_BY_REG(PRI_CTL, PORT_PRI_SEL, f_val, entry);
    }
    else
    {
        return SW_NOT_SUPPORTED;
    }

    *pri = f_val;
    return SW_OK;
}

static sw_error_t
_shiva_qos_port_default_up_set(a_uint32_t dev_id, fal_port_t port_id,
                               a_uint32_t up)
{
    sw_error_t rv;
    a_uint32_t val;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    if (FAL_DOT1P_MAX < up)
    {
        return SW_BAD_PARAM;
    }

    val = up;
    HSL_REG_FIELD_SET(rv, dev_id, PORT_DOT1AD, port_id, ING_PRI,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_shiva_qos_port_default_up_get(a_uint32_t dev_id, fal_port_t port_id,
                               a_uint32_t * up)
{
    sw_error_t rv;
    a_uint32_t val = 0;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_GET(rv, dev_id, PORT_DOT1AD, port_id, ING_PRI,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    *up = val;
    return SW_OK;
}

static sw_error_t
_shiva_qos_port_sch_mode_set(a_uint32_t dev_id, a_uint32_t port_id,
                             fal_sch_mode_t mode, const a_uint32_t weight[])
{
    sw_error_t rv;
    a_uint32_t reg = 0, val, w[4] = {0};
    a_int32_t  i, _index;

    HSL_DEV_ID_CHECK(dev_id);

    if (FAL_SCH_SP_MODE == mode)
    {
        val   = 0;
        _index = -1;
    }
    else if (FAL_SCH_WRR_MODE == mode)
    {
        val   = 3;
        _index = 3;
    }
    else if (FAL_SCH_MIX_MODE == mode)
    {
        val   = 1;
        _index = 2;
    }
    else if (FAL_SCH_MIX_PLUS_MODE == mode)
    {
        val   = 2;
        _index = 1;
    }
    else
    {
        return SW_NOT_SUPPORTED;
    }

    for (i = _index; i >= 0; i--)
    {
        if (weight[i] > 0x1f)
        {
            return SW_BAD_PARAM;
        }
        w[i] = weight[i];
    }

    HSL_REG_ENTRY_GET(rv, dev_id, WRR_CTRL, port_id, (a_uint8_t *) (&reg),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    SW_SET_REG_BY_FIELD(WRR_CTRL, SCH_MODE, val, reg);
    SW_SET_REG_BY_FIELD(WRR_CTRL, Q3_W, w[3], reg);
    SW_SET_REG_BY_FIELD(WRR_CTRL, Q2_W, w[2], reg);
    SW_SET_REG_BY_FIELD(WRR_CTRL, Q1_W, w[1], reg);
    SW_SET_REG_BY_FIELD(WRR_CTRL, Q0_W, w[0], reg);

    HSL_REG_ENTRY_SET(rv, dev_id, WRR_CTRL, port_id,
                      (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_shiva_qos_port_sch_mode_get(a_uint32_t dev_id, a_uint32_t port_id,
                             fal_sch_mode_t * mode, a_uint32_t weight[])
{
    sw_error_t rv;
    a_uint32_t val = 0, sch, w[4], i;

    HSL_DEV_ID_CHECK(dev_id);

    HSL_REG_ENTRY_GET(rv, dev_id, WRR_CTRL, port_id, (a_uint8_t *) (&val),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    SW_GET_FIELD_BY_REG(WRR_CTRL, SCH_MODE, sch, val);
    SW_GET_FIELD_BY_REG(WRR_CTRL, Q3_W, w[3], val);
    SW_GET_FIELD_BY_REG(WRR_CTRL, Q2_W, w[2], val);
    SW_GET_FIELD_BY_REG(WRR_CTRL, Q1_W, w[1], val);
    SW_GET_FIELD_BY_REG(WRR_CTRL, Q0_W, w[0], val);

    if (0 == sch)
    {
        *mode = FAL_SCH_SP_MODE;
    }
    else if (1 == sch)
    {
        *mode = FAL_SCH_MIX_MODE;
    }
    else if (2 == sch)
    {
        *mode = FAL_SCH_MIX_PLUS_MODE;
    }
    else
    {
        *mode = FAL_SCH_WRR_MODE;
    }

    for (i = 0; i < 6; i++)
    {
        weight[i] = 0;
    }

    for (i = 0; i < 4; i++)
    {
        weight[i] = w[i];
    }

    return SW_OK;
}

/**
 * @brief Set buffer aggsinment status of transmitting queue on one particular port.
 *   @details  Comments:
 *    If enable queue tx buffer on one port that means each queue of this port
 *    will have fixed number buffers when transmitting packets. Otherwise they
 *    share the whole buffers with other queues in device.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_qos_queue_tx_buf_status_set(a_uint32_t dev_id, fal_port_t port_id,
                                  a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_qos_queue_tx_buf_status_set(dev_id, port_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get buffer aggsinment status of transmitting queue on particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_qos_queue_tx_buf_status_get(a_uint32_t dev_id, fal_port_t port_id,
                                  a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_qos_queue_tx_buf_status_get(dev_id, port_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set buffer aggsinment status of transmitting port on one particular port.
 *   @details  Comments:
    If enable tx buffer on one port that means this port will have fixed
    number buffers when transmitting packets. Otherwise they will
    share the whole buffers with other ports in device.
 *    function will return actual buffer numbers in hardware.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_qos_port_tx_buf_status_set(a_uint32_t dev_id, fal_port_t port_id,
                                 a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_qos_port_tx_buf_status_set(dev_id, port_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get buffer aggsinment status of transmitting port on one particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_qos_port_tx_buf_status_get(a_uint32_t dev_id, fal_port_t port_id,
                                 a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_qos_port_tx_buf_status_get(dev_id, port_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set max occupied buffer number of transmitting queue on one particular port.
 *   @details   Comments:
    The step of buffer number in SHIVA is 4, function will return actual
    buffer numbers in hardware.
    The buffer number range for queue is 4 to 60.
 *    share the whole buffers with other ports in device.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] queue_id queue id
 * @param number buffer number
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_qos_queue_tx_buf_nr_set(a_uint32_t dev_id, fal_port_t port_id,
                              fal_queue_t queue_id, a_uint32_t * number)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_qos_queue_tx_buf_nr_set(dev_id, port_id, queue_id, number);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get max occupied buffer number of transmitting queue on one particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] queue_id queue id
 * @param[out] number buffer number
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_qos_queue_tx_buf_nr_get(a_uint32_t dev_id, fal_port_t port_id,
                              fal_queue_t queue_id, a_uint32_t * number)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_qos_queue_tx_buf_nr_get(dev_id, port_id, queue_id, number);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set max occupied buffer number of transmitting port on one particular port.
 *   @details   Comments:
    The step of buffer number in SHIVA is four, function will return actual
    buffer numbers in hardware.
    The buffer number range for transmitting port is 4 to 124.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param number buffer number
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_qos_port_tx_buf_nr_set(a_uint32_t dev_id, fal_port_t port_id,
                             a_uint32_t * number)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_qos_port_tx_buf_nr_set(dev_id, port_id, number);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get max occupied buffer number of transmitting port on one particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] number buffer number
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_qos_port_tx_buf_nr_get(a_uint32_t dev_id, fal_port_t port_id,
                             a_uint32_t * number)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_qos_port_tx_buf_nr_get(dev_id, port_id, number);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set max occupied buffer number of receiving port on one particular port.
 *   @details   Comments:
    The step of buffer number in SHIVA is four, function will return actual
    buffer numbers in hardware.
    The buffer number range for receiving port is 4 to 60.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param number buffer number
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_qos_port_rx_buf_nr_set(a_uint32_t dev_id, fal_port_t port_id,
                             a_uint32_t * number)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_qos_port_rx_buf_nr_set(dev_id, port_id, number);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get max occupied buffer number of receiving port on one particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] number buffer number
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_qos_port_rx_buf_nr_get(a_uint32_t dev_id, fal_port_t port_id,
                             a_uint32_t * number)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_qos_port_rx_buf_nr_get(dev_id, port_id, number);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set user priority to mapping on one particular device.
 * @param[in] dev_id device id
 * @param[in] dot1p 802.1p
 * @param[in] queue queue id
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_cosmap_up_queue_set(a_uint32_t dev_id, a_uint32_t up,
                          fal_queue_t queue)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_cosmap_up_queue_set(dev_id, up, queue);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get user priority to mapping on one particular device.
 * @param[in] dev_id device id
 * @param[in] dot1p 802.1p
 * @param[out] queue queue id
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_cosmap_up_queue_get(a_uint32_t dev_id, a_uint32_t up,
                          fal_queue_t * queue)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_cosmap_up_queue_get(dev_id, up, queue);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set cos map dscp_2_queue item on one particular device.
 * @param[in] dev_id device id
 * @param[in] dscp dscp
 * @param[in] queue queue id
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_cosmap_dscp_queue_set(a_uint32_t dev_id, a_uint32_t dscp,
                            fal_queue_t queue)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_cosmap_dscp_queue_set(dev_id, dscp, queue);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get cos map dscp_2_queue item on one particular device.
 * @param[in] dev_id device id
 * @param[in] dscp dscp
 * @param[out] queue queue id
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_cosmap_dscp_queue_get(a_uint32_t dev_id, a_uint32_t dscp,
                            fal_queue_t * queue)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_cosmap_dscp_queue_get(dev_id, dscp, queue);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set port qos mode on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] mode qos mode
 * @param[in] enable A_TRUE of A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_qos_port_mode_set(a_uint32_t dev_id, fal_port_t port_id,
                        fal_qos_mode_t mode, a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_qos_port_mode_set(dev_id, port_id, mode, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get port qos mode on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] mode qos mode
 * @param[out] enable A_TRUE of A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_qos_port_mode_get(a_uint32_t dev_id, fal_port_t port_id,
                        fal_qos_mode_t mode, a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_qos_port_mode_get(dev_id, port_id, mode, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set priority of one particular qos mode on one particular port.
 *   @details   Comments:
    If the priority of a mode is more small then the priority is more high.
    Differnet mode should have differnet priority.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] mode qos mode
 * @param[in] pri priority of one particular qos mode
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_qos_port_mode_pri_set(a_uint32_t dev_id, fal_port_t port_id,
                            fal_qos_mode_t mode, a_uint32_t pri)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_qos_port_mode_pri_set(dev_id, port_id, mode, pri);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get priority of one particular qos mode on one particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] mode qos mode
 * @param[out] pri priority of one particular qos mode
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_qos_port_mode_pri_get(a_uint32_t dev_id, fal_port_t port_id,
                            fal_qos_mode_t mode, a_uint32_t * pri)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_qos_port_mode_pri_get(dev_id, port_id, mode, pri);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set default user priority on one particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] up 802.1p
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_qos_port_default_up_set(a_uint32_t dev_id, fal_port_t port_id,
                              a_uint32_t up)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_qos_port_default_up_set(dev_id, port_id, up);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get default user priority on one particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] up 802.1p
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_qos_port_default_up_get(a_uint32_t dev_id, fal_port_t port_id,
                              a_uint32_t * up)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_qos_port_default_up_get(dev_id, port_id, up);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set traffic scheduling mode on particular one port.
 *   @details   Comments:
 *   When scheduling mode is sp the weight is meaningless usually it's zero
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] fal_sch_mode_t traffic scheduling mode
 * @param[in] weight[] weight value for each queue when in wrr mode,
              the max value supported by SHIVA is 0x1f.
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_qos_port_sch_mode_set(a_uint32_t dev_id, a_uint32_t port_id,
                            fal_sch_mode_t mode, const a_uint32_t weight[])
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_qos_port_sch_mode_set(dev_id, port_id, mode, weight);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get traffic scheduling mode on particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] fal_sch_mode_t traffic scheduling mode
 * @param[out] weight weight value for wrr mode
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_qos_port_sch_mode_get(a_uint32_t dev_id, a_uint32_t port_id,
                            fal_sch_mode_t * mode, a_uint32_t weight[])
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_qos_port_sch_mode_get(dev_id, port_id, mode, weight);
    HSL_API_UNLOCK;
    return rv;
}

sw_error_t
shiva_qos_init(a_uint32_t dev_id)
{
    HSL_DEV_ID_CHECK(dev_id);

#ifndef HSL_STANDALONG
    {
        hsl_api_t *p_api;

        SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

        p_api->qos_queue_tx_buf_status_set = shiva_qos_queue_tx_buf_status_set;
        p_api->qos_queue_tx_buf_status_get = shiva_qos_queue_tx_buf_status_get;
        p_api->qos_port_tx_buf_status_set = shiva_qos_port_tx_buf_status_set;
        p_api->qos_port_tx_buf_status_get = shiva_qos_port_tx_buf_status_get;
        p_api->qos_queue_tx_buf_nr_set = shiva_qos_queue_tx_buf_nr_set;
        p_api->qos_queue_tx_buf_nr_get = shiva_qos_queue_tx_buf_nr_get;
        p_api->qos_port_tx_buf_nr_set = shiva_qos_port_tx_buf_nr_set;
        p_api->qos_port_tx_buf_nr_get = shiva_qos_port_tx_buf_nr_get;
        p_api->qos_port_rx_buf_nr_set = shiva_qos_port_rx_buf_nr_set;
        p_api->qos_port_rx_buf_nr_get = shiva_qos_port_rx_buf_nr_get;
        p_api->cosmap_up_queue_set = shiva_cosmap_up_queue_set;
        p_api->cosmap_up_queue_get = shiva_cosmap_up_queue_get;
        p_api->cosmap_dscp_queue_set = shiva_cosmap_dscp_queue_set;
        p_api->cosmap_dscp_queue_get = shiva_cosmap_dscp_queue_get;
        p_api->qos_port_mode_set = shiva_qos_port_mode_set;
        p_api->qos_port_mode_get = shiva_qos_port_mode_get;
        p_api->qos_port_mode_pri_set = shiva_qos_port_mode_pri_set;
        p_api->qos_port_mode_pri_get = shiva_qos_port_mode_pri_get;
        p_api->qos_port_default_up_set = shiva_qos_port_default_up_set;
        p_api->qos_port_default_up_get = shiva_qos_port_default_up_get;
        p_api->qos_port_sch_mode_set = shiva_qos_port_sch_mode_set;
        p_api->qos_port_sch_mode_get = shiva_qos_port_sch_mode_get;
    }
#endif

    return SW_OK;
}

/**
 * @}
 */

