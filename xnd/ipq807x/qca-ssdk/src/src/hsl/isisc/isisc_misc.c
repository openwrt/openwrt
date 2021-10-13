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
 * @defgroup isisc_misc ISISC_MISC
 * @{
 */
#include "sw.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_port_prop.h"
#include "isisc_misc.h"
#include "isisc_reg.h"
#include "hsl_phy.h"


#define ISISC_MAX_FRMAE_SIZE      9216

#define ARP_REQ_EN_OFFSET    6
#define ARP_ACK_EN_OFFSET    5
#define DHCP_EN_OFFSET       4
#define EAPOL_EN_OFFSET      3

#define ISISC_SWITCH_INT_PHY_INT   0x8000


static sw_error_t
_isisc_port_misc_property_set(a_uint32_t dev_id, fal_port_t port_id,
                             a_bool_t enable, a_uint32_t item)
{
    sw_error_t rv;
    a_uint32_t reg = 0, val;

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

    if (3 >= port_id)
    {
        HSL_REG_ENTRY_GET(rv, dev_id, FRAME_ACK_CTL0, 0,
                          (a_uint8_t *) (&reg), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        reg &= ~(0x1UL << ((port_id << 3) + item));
        reg |= (val << ((port_id << 3) + item));

        HSL_REG_ENTRY_SET(rv, dev_id, FRAME_ACK_CTL0, 0,
                          (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    }
    else
    {
        HSL_REG_ENTRY_GET(rv, dev_id, FRAME_ACK_CTL1, 0,
                          (a_uint8_t *) (&reg), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        reg &= ~(0x1UL << (((port_id - 4) << 3) + item));
        reg |= (val << (((port_id - 4) << 3) + item));

        HSL_REG_ENTRY_SET(rv, dev_id, FRAME_ACK_CTL1, 0,
                          (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    }
    return rv;
}

#ifndef IN_MISC_MINI
static sw_error_t
_isisc_port_misc_property_get(a_uint32_t dev_id, fal_port_t port_id,
                             a_bool_t * enable, a_uint32_t item)
{
    sw_error_t rv;
    a_uint32_t reg = 0, val;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    if (3 >= port_id)
    {
        HSL_REG_ENTRY_GET(rv, dev_id, FRAME_ACK_CTL0, 0,
                          (a_uint8_t *) (&reg), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        val = (reg >> ((port_id << 3) + item)) & 0x1UL;
    }
    else
    {
        HSL_REG_ENTRY_GET(rv, dev_id, FRAME_ACK_CTL1, 0,
                          (a_uint8_t *) (&reg), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        val = (reg >> (((port_id - 4) << 3) + item)) & 0x1UL;
    }

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
#endif

static sw_error_t
_isisc_frame_max_size_set(a_uint32_t dev_id, a_uint32_t size)
{
    a_uint32_t data;
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    if (ISISC_MAX_FRMAE_SIZE < size)
    {
        return SW_BAD_PARAM;
    }

    data = size;
    HSL_REG_FIELD_SET(rv, dev_id, MAX_SIZE, 0, MAX_FRAME_SIZE,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_isisc_frame_max_size_get(a_uint32_t dev_id, a_uint32_t * size)
{
    a_uint32_t data = 0;
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    HSL_REG_FIELD_GET(rv, dev_id, MAX_SIZE, 0, MAX_FRAME_SIZE,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    *size = data;
    return SW_OK;
}

static sw_error_t
_isisc_port_unk_uc_filter_set(a_uint32_t dev_id, fal_port_t port_id,
                             a_bool_t enable)
{
    sw_error_t rv;
    a_uint32_t data = 0;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_GET(rv, dev_id, FORWARD_CTL1, 0, UNI_FLOOD_DP,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (A_TRUE == enable)
    {
        data &= (~((a_uint32_t) 0x1 << port_id));
    }
    else if (A_FALSE == enable)
    {
        data |= (0x1 << port_id);
    }
    else
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_SET(rv, dev_id, FORWARD_CTL1, 0, UNI_FLOOD_DP,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    return rv;
}


static sw_error_t
_isisc_port_unk_mc_filter_set(a_uint32_t dev_id, fal_port_t port_id,
                             a_bool_t enable)
{
    sw_error_t rv;
    a_uint32_t data = 0;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_GET(rv, dev_id, FORWARD_CTL1, 0, MUL_FLOOD_DP,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (A_TRUE == enable)
    {
        data &= (~((a_uint32_t) 0x1 << port_id));
    }
    else if (A_FALSE == enable)
    {
        data |= (0x1 << port_id);
    }
    else
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_SET(rv, dev_id, FORWARD_CTL1, 0, MUL_FLOOD_DP,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    return rv;
}


static sw_error_t
_isisc_port_bc_filter_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
    sw_error_t rv;
    a_uint32_t data = 0;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_GET(rv, dev_id, FORWARD_CTL1, 0, BC_FLOOD_DP,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (A_TRUE == enable)
    {
        data &= (~((a_uint32_t) 0x1 << port_id));
    }
    else if (A_FALSE == enable)
    {
        data |= (0x1 << port_id);
    }
    else
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_SET(rv, dev_id, FORWARD_CTL1, 0, BC_FLOOD_DP,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_isisc_cpu_port_status_set(a_uint32_t dev_id, a_bool_t enable)
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

    HSL_REG_FIELD_SET(rv, dev_id, FORWARD_CTL0, 0, CPU_PORT_EN,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    return rv;
}

#ifndef IN_MISC_MINI
static sw_error_t
_isisc_port_bc_filter_get(a_uint32_t dev_id, fal_port_t port_id,
                         a_bool_t * enable)
{
    sw_error_t rv;
    a_uint32_t reg = 0, field;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_GET(rv, dev_id, FORWARD_CTL1, 0, BC_FLOOD_DP,
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
_isisc_port_unk_uc_filter_get(a_uint32_t dev_id, fal_port_t port_id,
                             a_bool_t * enable)
{
    sw_error_t rv;
    a_uint32_t reg = 0, field;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_GET(rv, dev_id, FORWARD_CTL1, 0, UNI_FLOOD_DP,
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
_isisc_port_unk_mc_filter_get(a_uint32_t dev_id, fal_port_t port_id,
                             a_bool_t * enable)
{
    sw_error_t rv;
    a_uint32_t reg = 0, field;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_GET(rv, dev_id, FORWARD_CTL1, 0, MUL_FLOOD_DP,
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
_isisc_cpu_port_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;
    a_uint32_t val = 0;

    HSL_DEV_ID_CHECK(dev_id);

    HSL_REG_FIELD_GET(rv, dev_id, FORWARD_CTL0, 0, CPU_PORT_EN,
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
_isisc_cpu_vid_en_set(a_uint32_t dev_id, a_bool_t enable)
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

    HSL_REG_FIELD_SET(rv, dev_id, PKT_CTRL, 0, CPU_VID_EN,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_isisc_cpu_vid_en_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;
    a_uint32_t val = 0;

    HSL_DEV_ID_CHECK(dev_id);

    HSL_REG_FIELD_GET(rv, dev_id, PKT_CTRL, 0, CPU_VID_EN,
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
_isisc_rtd_pppoe_en_set(a_uint32_t dev_id, a_bool_t enable)
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

    HSL_REG_FIELD_SET(rv, dev_id, PKT_CTRL, 0, RTD_PPPOE_EN,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_isisc_rtd_pppoe_en_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;
    a_uint32_t val = 0;

    HSL_DEV_ID_CHECK(dev_id);

    HSL_REG_FIELD_GET(rv, dev_id, PKT_CTRL, 0, RTD_PPPOE_EN,
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
_isisc_pppoe_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd)
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

    HSL_REG_FIELD_SET(rv, dev_id, FORWARD_CTL0, 0, PPPOE_RDT_EN,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_isisc_pppoe_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;
    a_uint32_t val = 0;

    HSL_DEV_ID_CHECK(dev_id);

    HSL_REG_FIELD_GET(rv, dev_id, FORWARD_CTL0, 0, PPPOE_RDT_EN,
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
_isisc_pppoe_status_set(a_uint32_t dev_id, a_bool_t enable)
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

    HSL_REG_FIELD_SET(rv, dev_id, FRAME_ACK_CTL1, 0, PPPOE_EN,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_isisc_pppoe_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;
    a_uint32_t val = 0;

    HSL_DEV_ID_CHECK(dev_id);

    HSL_REG_FIELD_GET(rv, dev_id, FRAME_ACK_CTL1, 0, PPPOE_EN,
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
_isisc_arp_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd)
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
    else if (FAL_MAC_FRWRD == cmd)
    {
        val = 2;
    }
    else
    {
        return SW_NOT_SUPPORTED;
    }

    HSL_REG_FIELD_SET(rv, dev_id, FORWARD_CTL0, 0, ARP_CMD,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_isisc_arp_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;
    a_uint32_t val = 0;

    HSL_DEV_ID_CHECK(dev_id);

    HSL_REG_FIELD_GET(rv, dev_id, FORWARD_CTL0, 0, ARP_CMD,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (1 == val)
    {
        *cmd = FAL_MAC_CPY_TO_CPU;
    }
    else if (0 == val)
    {
        *cmd = FAL_MAC_RDT_TO_CPU;
    }
    else
    {
        *cmd = FAL_MAC_FRWRD;
    }

    return SW_OK;
}
#endif

static sw_error_t
_isisc_eapol_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd)
{
    sw_error_t rv;
    a_uint32_t val;

    HSL_DEV_ID_CHECK(dev_id);

    if (FAL_MAC_CPY_TO_CPU == cmd)
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

    HSL_REG_FIELD_SET(rv, dev_id, FORWARD_CTL0, 0, EAPOL_CMD,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    return rv;
}

#ifndef IN_MISC_MINI
static sw_error_t
_isisc_eapol_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;
    a_uint32_t val = 0;

    HSL_DEV_ID_CHECK(dev_id);

    HSL_REG_FIELD_GET(rv, dev_id, FORWARD_CTL0, 0, EAPOL_CMD,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (0 == val)
    {
        *cmd = FAL_MAC_CPY_TO_CPU;
    }
    else
    {
        *cmd = FAL_MAC_RDT_TO_CPU;
    }

    return SW_OK;
}

#define ISISC_MAX_PPPOE_SESSION  16
#define ISISC_MAX_SESSION_ID     0xffff

static sw_error_t
_isisc_pppoe_session_add(a_uint32_t dev_id, fal_pppoe_session_t * session_tbl)
{
    sw_error_t rv;
    a_uint32_t reg = 0, i, valid, id, entry_idx = ISISC_MAX_PPPOE_SESSION;

    HSL_DEV_ID_CHECK(dev_id);

    if (session_tbl->session_id > ISISC_MAX_SESSION_ID)
    {
        return SW_BAD_PARAM;
    }

    if ((A_FALSE == session_tbl->multi_session)
            && (A_TRUE == session_tbl->uni_session))
    {
        return SW_BAD_PARAM;
    }

    if ((A_FALSE == session_tbl->multi_session)
            && (A_FALSE == session_tbl->uni_session))
    {
        return SW_BAD_PARAM;
    }

    for (i = 0; i < ISISC_MAX_PPPOE_SESSION; i++)
    {
        HSL_REG_ENTRY_GET(rv, dev_id, PPPOE_SESSION, i,
                          (a_uint8_t *) (&reg), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        SW_GET_FIELD_BY_REG(PPPOE_SESSION, ENTRY_VALID, valid, reg);
        SW_GET_FIELD_BY_REG(PPPOE_SESSION, SEESION_ID, id, reg);

        if (!valid)
        {
            entry_idx = i;
        }
        else if (id == session_tbl->session_id)
        {
            return SW_ALREADY_EXIST;
        }
    }

    if (ISISC_MAX_PPPOE_SESSION == entry_idx)
    {
        return SW_NO_RESOURCE;
    }

#if 0
    if (A_TRUE == session_tbl->uni_session)
    {
        SW_SET_REG_BY_FIELD(PPPOE_SESSION, ENTRY_VALID, 2, reg);
    }
    else
#endif
    {
        SW_SET_REG_BY_FIELD(PPPOE_SESSION, ENTRY_VALID, 1, reg);
    }
    SW_SET_REG_BY_FIELD(PPPOE_SESSION, SEESION_ID, session_tbl->session_id,
                        reg);

    HSL_REG_ENTRY_SET(rv, dev_id, PPPOE_SESSION, entry_idx,
                      (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    session_tbl->entry_id = entry_idx;
    return SW_OK;
}

static sw_error_t
_isisc_pppoe_session_del(a_uint32_t dev_id, fal_pppoe_session_t * session_tbl)
{
    sw_error_t rv;
    a_uint32_t reg = 0, i, valid, id;

    HSL_DEV_ID_CHECK(dev_id);

    if (session_tbl->session_id > ISISC_MAX_SESSION_ID)
    {
        return SW_BAD_PARAM;
    }

    for (i = 0; i < ISISC_MAX_PPPOE_SESSION; i++)
    {
        HSL_REG_ENTRY_GET(rv, dev_id, PPPOE_SESSION, i,
                          (a_uint8_t *) (&reg), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        SW_GET_FIELD_BY_REG(PPPOE_SESSION, ENTRY_VALID, valid, reg);
        SW_GET_FIELD_BY_REG(PPPOE_SESSION, SEESION_ID, id, reg);

        if (((1 == valid) || (2 == valid)) && (id == session_tbl->session_id))
        {
            SW_SET_REG_BY_FIELD(PPPOE_SESSION, ENTRY_VALID, 0, reg);
            SW_SET_REG_BY_FIELD(PPPOE_SESSION, SEESION_ID, 0, reg);
            HSL_REG_ENTRY_SET(rv, dev_id, PPPOE_SESSION, i,
                              (a_uint8_t *) (&reg), sizeof (a_uint32_t));
            return rv;
        }
    }

    return SW_NOT_FOUND;
}

static sw_error_t
_isisc_pppoe_session_get(a_uint32_t dev_id, fal_pppoe_session_t * session_tbl)
{
    sw_error_t rv;
    a_uint32_t reg = 0, i, valid, id;

    HSL_DEV_ID_CHECK(dev_id);

    if (session_tbl->session_id > ISISC_MAX_SESSION_ID)
    {
        return SW_BAD_PARAM;
    }

    for (i = 0; i < ISISC_MAX_PPPOE_SESSION; i++)
    {
        HSL_REG_ENTRY_GET(rv, dev_id, PPPOE_SESSION, i,
                          (a_uint8_t *) (&reg), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        SW_GET_FIELD_BY_REG(PPPOE_SESSION, ENTRY_VALID, valid, reg);
        SW_GET_FIELD_BY_REG(PPPOE_SESSION, SEESION_ID, id, reg);

        if (((1 == valid) || (2 == valid)) && (id == session_tbl->session_id))
        {
            if (1 == valid)
            {
                session_tbl->multi_session = A_TRUE;
                session_tbl->uni_session = A_FALSE;
            }
            else
            {
                session_tbl->multi_session = A_TRUE;
                session_tbl->uni_session = A_TRUE;
            }

            session_tbl->entry_id = i;
            return SW_OK;
        }
    }

    return SW_NOT_FOUND;
}

static sw_error_t
_isisc_pppoe_session_id_set(a_uint32_t dev_id, a_uint32_t index,
                           a_uint32_t id)
{
    sw_error_t rv;
    a_uint32_t reg;

    if (ISISC_MAX_PPPOE_SESSION <= index)
    {
        return SW_BAD_PARAM;
    }

    if (ISISC_MAX_SESSION_ID < id)
    {
        return SW_BAD_PARAM;
    }

    reg = 0;
    SW_SET_REG_BY_FIELD(PPPOE_EDIT, EDIT_ID, id, reg);
    HSL_REG_ENTRY_SET(rv, dev_id, PPPOE_EDIT, index, (a_uint8_t *) (&reg),
                      sizeof (a_uint32_t));

    return rv;
}

static sw_error_t
_isisc_pppoe_session_id_get(a_uint32_t dev_id, a_uint32_t index,
                           a_uint32_t * id)
{
    sw_error_t rv;
    a_uint32_t reg = 0, tmp;

    if (ISISC_MAX_PPPOE_SESSION <= index)
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_ENTRY_GET(rv, dev_id, PPPOE_EDIT, index, (a_uint8_t *) (&reg),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    tmp = 0;
    SW_GET_FIELD_BY_REG(PPPOE_EDIT, EDIT_ID, tmp, reg);
    *id = tmp;
    return SW_OK;
}

static sw_error_t
_isisc_ripv1_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    a_uint32_t data;
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, 0, HSL_PP_INCL_CPU))
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

    HSL_REG_FIELD_SET(rv, dev_id, FORWARD_CTL0, 0, RIP_CPY_EN,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_isisc_ripv1_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    a_uint32_t data = 0;
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, 0, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_GET(rv, dev_id, FORWARD_CTL0, 0, RIP_CPY_EN,
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
_isisc_intr_mask_set(a_uint32_t dev_id, a_uint32_t intr_mask)
{
    sw_error_t rv;
    a_uint32_t reg = 0;

    HSL_REG_ENTRY_GET(rv, dev_id, GBL_INT_MASK1, 0, (a_uint8_t *) (&reg),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (intr_mask & FAL_SWITCH_INTR_LINK_STATUS)
    {
        reg |= ISISC_SWITCH_INT_PHY_INT;
    }
    else
    {
        reg &= (~ISISC_SWITCH_INT_PHY_INT);
    }

    HSL_REG_ENTRY_SET(rv, dev_id, GBL_INT_MASK1, 0, (a_uint8_t *) (&reg),
                      sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_isisc_intr_mask_get(a_uint32_t dev_id, a_uint32_t * intr_mask)
{
    sw_error_t rv;
    a_uint32_t reg = 0;

    *intr_mask = 0;
    HSL_REG_ENTRY_GET(rv, dev_id, GBL_INT_MASK1, 0, (a_uint8_t *) (&reg),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (reg & ISISC_SWITCH_INT_PHY_INT)
    {
        *intr_mask |= FAL_SWITCH_INTR_LINK_STATUS;
    }

    return SW_OK;
}

static sw_error_t
_isisc_intr_status_get(a_uint32_t dev_id, a_uint32_t * intr_status)
{
    sw_error_t rv;
    a_uint32_t reg = 0;

    *intr_status = 0;
    HSL_REG_ENTRY_GET(rv, dev_id, GBL_INT_STATUS1, 0, (a_uint8_t *) (&reg),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (reg & ISISC_SWITCH_INT_PHY_INT)
    {
        *intr_status |= FAL_SWITCH_INTR_LINK_STATUS;
    }

    return SW_OK;
}

static sw_error_t
_isisc_intr_status_clear(a_uint32_t dev_id, a_uint32_t intr_status)
{
    sw_error_t rv;
    a_uint32_t reg;

    reg = 0;
    if (intr_status & FAL_SWITCH_INTR_LINK_STATUS)
    {
        reg |= ISISC_SWITCH_INT_PHY_INT;
    }

    HSL_REG_ENTRY_SET(rv, dev_id, GBL_INT_STATUS1, 0, (a_uint8_t *) (&reg),
                      sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_isisc_port_link_intr_mask_set(a_uint32_t dev_id, a_uint32_t port_id, a_uint32_t intr_mask_flag)
{
    sw_error_t rv;
    a_uint32_t phy_id;
    hsl_phy_ops_t *phy_drv;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_PHY))
    {
        return SW_BAD_PARAM;
    }

    SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
    if (NULL == phy_drv->phy_intr_mask_set)
	  return SW_NOT_SUPPORTED;

    rv = hsl_port_prop_get_phyid(dev_id, port_id, &phy_id);
    SW_RTN_ON_ERROR(rv);

    rv = phy_drv->phy_intr_mask_set(dev_id, phy_id, intr_mask_flag);
    return rv;
}

static sw_error_t
_isisc_port_link_intr_mask_get(a_uint32_t dev_id, a_uint32_t port_id, a_uint32_t * intr_mask_flag)
{
    sw_error_t rv;
    a_uint32_t phy_id;
    hsl_phy_ops_t *phy_drv;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_PHY))
    {
        return SW_BAD_PARAM;
    }

    SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
    if (NULL == phy_drv->phy_intr_mask_get)
	  return SW_NOT_SUPPORTED;

    rv = hsl_port_prop_get_phyid(dev_id, port_id, &phy_id);
    SW_RTN_ON_ERROR(rv);

    rv = phy_drv->phy_intr_mask_get(dev_id, phy_id, intr_mask_flag);
    return rv;
}

static sw_error_t
_isisc_port_link_intr_status_get(a_uint32_t dev_id, a_uint32_t port_id, a_uint32_t * intr_mask_flag)
{
    sw_error_t rv;
    a_uint32_t phy_id;
    hsl_phy_ops_t *phy_drv;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_PHY))
    {
        return SW_BAD_PARAM;
    }

    SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
    if (NULL == phy_drv->phy_intr_status_get)
	  return SW_NOT_SUPPORTED;

    rv = hsl_port_prop_get_phyid(dev_id, port_id, &phy_id);
    SW_RTN_ON_ERROR(rv);

    rv = phy_drv->phy_intr_status_get(dev_id, phy_id, intr_mask_flag);
    return rv;
}

static sw_error_t
_isisc_intr_mask_mac_linkchg_set(a_uint32_t dev_id, a_uint32_t port_id, a_bool_t enable)
{
    sw_error_t rv;
    a_uint32_t data = 0;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_GET(rv, dev_id, GBL_INT_MASK1, 0, LINK_CHG_INT_M,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (A_FALSE == enable)
    {
        data &= (~((a_uint32_t) 0x1 << port_id));
    }
    else if (A_TRUE == enable)
    {
        data |= (0x1 << port_id);
    }
    else
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_SET(rv, dev_id, GBL_INT_MASK1, 0, LINK_CHG_INT_M,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));

    return rv;
}


static sw_error_t
_isisc_intr_mask_mac_linkchg_get(a_uint32_t dev_id, a_uint32_t port_id, a_bool_t * enable)
{
    sw_error_t rv;
    a_uint32_t reg = 0, field;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_GET(rv, dev_id, GBL_INT_MASK1, 0, LINK_CHG_INT_M,
                      (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    field = reg & (0x1 << port_id);
    if (field)
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
_isisc_intr_status_mac_linkchg_get(a_uint32_t dev_id, fal_pbmp_t* port_bitmap)
{
    sw_error_t rv;
    a_uint32_t reg = 0;

    HSL_DEV_ID_CHECK(dev_id);

    HSL_REG_FIELD_GET(rv, dev_id, GBL_INT_STATUS1, 0, LINK_CHG_INT_S,
                      (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    *port_bitmap = reg;

    return rv;

}

static sw_error_t
_isisc_intr_status_mac_linkchg_clear(a_uint32_t dev_id)
{
    sw_error_t rv;
    a_uint32_t reg;

    HSL_DEV_ID_CHECK(dev_id);

    HSL_REG_FIELD_GET(rv, dev_id, GBL_INT_STATUS1, 0, LINK_CHG_INT_S,
                      (a_uint8_t *) (&reg), sizeof (a_uint32_t));

    HSL_REG_FIELD_SET(rv, dev_id, GBL_INT_STATUS1, 0, LINK_CHG_INT_S,
                      (a_uint8_t *) (&reg), sizeof (a_uint32_t));

    return rv;

}
#endif

/**
 * @brief Set max frame size which device can received on a particular device.
 * @details    Comments:
 * The granularity of packets size is byte.
 * @param[in] dev_id device id
 * @param[in] size packet size
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_frame_max_size_set(a_uint32_t dev_id, a_uint32_t size)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_frame_max_size_set(dev_id, size);
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
isisc_frame_max_size_get(a_uint32_t dev_id, a_uint32_t * size)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_frame_max_size_get(dev_id, size);
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
isisc_port_unk_uc_filter_set(a_uint32_t dev_id, fal_port_t port_id,
                            a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_port_unk_uc_filter_set(dev_id, port_id, enable);
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
isisc_port_unk_mc_filter_set(a_uint32_t dev_id, fal_port_t port_id,
                            a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_port_unk_mc_filter_set(dev_id, port_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set flooding status of broadcast packets on a particular port.
 * @details  Comments:
 *   If enable unknown multicast packets filter on one port then unknown
 *   multicast packets can't flood out from this port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_port_bc_filter_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_port_bc_filter_set(dev_id, port_id, enable);
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
isisc_cpu_port_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_cpu_port_status_set(dev_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

#ifndef IN_MISC_MINI
/**
 * @brief Get flooding status of unknown unicast packets on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_port_unk_uc_filter_get(a_uint32_t dev_id, fal_port_t port_id,
                            a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_port_unk_uc_filter_get(dev_id, port_id, enable);
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
isisc_port_unk_mc_filter_get(a_uint32_t dev_id, fal_port_t port_id,
                            a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_port_unk_mc_filter_get(dev_id, port_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/** @brief Get flooding status of broadcast packets on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_port_bc_filter_get(a_uint32_t dev_id, fal_port_t port_id,
                        a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_port_bc_filter_get(dev_id, port_id, enable);
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
isisc_cpu_port_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_cpu_port_status_get(dev_id, enable);
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
isisc_pppoe_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_pppoe_cmd_set(dev_id, cmd);
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
isisc_pppoe_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_pppoe_cmd_get(dev_id, cmd);
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
isisc_pppoe_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_pppoe_status_set(dev_id, enable);
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
isisc_pppoe_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_pppoe_status_get(dev_id, enable);
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
isisc_port_dhcp_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_port_misc_property_set(dev_id, port_id, enable, DHCP_EN_OFFSET);
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
isisc_port_dhcp_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_port_misc_property_get(dev_id, port_id, enable, DHCP_EN_OFFSET);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set arp packets forwarding command on a particular device.
 * @details     comments:
 *   Particular device may only support parts of forwarding commands.
 *   Ihis operation will take effect only after enabling arp packets
 *   hardware acknowledgement
 * @param[in] dev_id device id
 * @param[in] cmd forwarding command
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_arp_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_arp_cmd_set(dev_id, cmd);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get arp packets forwarding command on a particular device.
 * @param[in] dev_id device id
 * @param[out] cmd forwarding command
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_arp_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_arp_cmd_get(dev_id, cmd);
    HSL_API_UNLOCK;
    return rv;
}
#endif

/**
 * @brief Set eapol packets forwarding command on a particular device.
 * @details     comments:
 *   Particular device may only support parts of forwarding commands.
 *   Ihis operation will take effect only after enabling eapol packets
 *   hardware acknowledgement
 * @param[in] dev_id device id
 * @param[in] cmd forwarding command
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_eapol_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_eapol_cmd_set(dev_id, cmd);
    HSL_API_UNLOCK;
    return rv;
}

#ifndef IN_MISC_MINI
/**
 * @brief Get eapol packets forwarding command on a particular device.
 * @param[in] dev_id device id
 * @param[out] cmd forwarding command
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_eapol_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_eapol_cmd_get(dev_id, cmd);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Add a pppoe session entry to a particular device.
 *        The entry only for pppoe/ppp header remove.
 * @param[in] dev_id device id
 * @param[in] session_tbl pppoe session table
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_pppoe_session_table_add(a_uint32_t dev_id,
                             fal_pppoe_session_t * session_tbl)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_pppoe_session_add(dev_id, session_tbl);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Delete a pppoe session entry from a particular device.
 *        The entry only for pppoe/ppp header remove.
 * @param[in] dev_id device id
 * @param[in] session_tbl pppoe session table
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_pppoe_session_table_del(a_uint32_t dev_id,
                             fal_pppoe_session_t * session_tbl)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_pppoe_session_del(dev_id, session_tbl);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get a pppoe session entry from a particular device.
 *        The entry only for pppoe/ppp header remove.
 * @param[in] dev_id device id
 * @param[out] session_tbl pppoe session table
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_pppoe_session_table_get(a_uint32_t dev_id,
                             fal_pppoe_session_t * session_tbl)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_pppoe_session_get(dev_id, session_tbl);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set a pppoe session id entry to a particular device.
 *        The entry only for pppoe/ppp header add.
 * @param[in] dev_id device id
 * @param[in] session_tbl pppoe session table
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_pppoe_session_id_set(a_uint32_t dev_id, a_uint32_t index,
                          a_uint32_t id)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_pppoe_session_id_set(dev_id, index, id);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get a pppoe session id entry from a particular device.
 *        The entry only for pppoe/ppp header add.
 * @param[in] dev_id device id
 * @param[out] session_tbl pppoe session table
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_pppoe_session_id_get(a_uint32_t dev_id, a_uint32_t index,
                          a_uint32_t * id)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_pppoe_session_id_get(dev_id, index, id);
    HSL_API_UNLOCK;
    return rv;
}
#endif

/**
 * @brief Set eapol packets hardware acknowledgement status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_eapol_status_set(a_uint32_t dev_id, a_uint32_t port_id, a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_port_misc_property_set(dev_id, port_id, enable, EAPOL_EN_OFFSET);
    HSL_API_UNLOCK;
    return rv;
}

#ifndef IN_MISC_MINI
/**
 * @brief Get eapol packets hardware acknowledgement status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_eapol_status_get(a_uint32_t dev_id, a_uint32_t port_id, a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_port_misc_property_get(dev_id, port_id, enable, EAPOL_EN_OFFSET);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set rip v1 packets hardware acknowledgement status on a particular port.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_ripv1_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_ripv1_status_set(dev_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get rip v1 packets hardware acknowledgement status on a particular port.
 * @param[in] dev_id device id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_ripv1_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_ripv1_status_get(dev_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set arp req packets hardware acknowledgement status on a particular device.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_port_arp_req_status_set(a_uint32_t dev_id, fal_port_t port_id,
                             a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_port_misc_property_set(dev_id, port_id, enable,
                                      ARP_REQ_EN_OFFSET);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get arp req packets hardware acknowledgement status on a particular device.
 * @param[in] dev_id device id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_port_arp_req_status_get(a_uint32_t dev_id, fal_port_t port_id,
                             a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_port_misc_property_get(dev_id, port_id, enable,
                                      ARP_REQ_EN_OFFSET);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set arp ack packets hardware acknowledgement status on a particular device.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_port_arp_ack_status_set(a_uint32_t dev_id, fal_port_t port_id,
                             a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_port_misc_property_set(dev_id, port_id, enable,
                                      ARP_ACK_EN_OFFSET);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get arp ack packets hardware acknowledgement status on a particular device.
 * @param[in] dev_id device id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_port_arp_ack_status_get(a_uint32_t dev_id, fal_port_t port_id,
                             a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_port_misc_property_get(dev_id, port_id, enable,
                                      ARP_ACK_EN_OFFSET);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set switch interrupt mask on one particular device.
 * @param[in] dev_id device id
 * @param[in] intr_mask mask
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_intr_mask_set(a_uint32_t dev_id, a_uint32_t intr_mask)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_intr_mask_set(dev_id, intr_mask);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get switch interrupt mask on one particular device.
 * @param[in] dev_id device id
 * @param[in] intr_mask mask
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_intr_mask_get(a_uint32_t dev_id, a_uint32_t * intr_mask)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_intr_mask_get(dev_id, intr_mask);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get switch interrupt status on one particular device.
 * @param[in] dev_id device id
 * @param[in] intr_status status
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_intr_status_get(a_uint32_t dev_id, a_uint32_t * intr_status)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_intr_status_get(dev_id, intr_status);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Clear switch interrupt status on one particular device.
 * @param[in] dev_id device id
 * @param[in] intr_status status
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_intr_status_clear(a_uint32_t dev_id, a_uint32_t intr_status)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_intr_status_clear(dev_id, intr_status);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set link interrupt mask on particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] intr_mask_flag interrupt mask
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_intr_port_link_mask_set(a_uint32_t dev_id, a_uint32_t port_id, a_uint32_t intr_mask_flag)
{
    sw_error_t rv;
    HSL_API_LOCK;
    rv = _isisc_port_link_intr_mask_set(dev_id, port_id, intr_mask_flag);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get link interrupt mask on particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] intr_mask_flag interrupt mask
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_intr_port_link_mask_get(a_uint32_t dev_id, a_uint32_t port_id, a_uint32_t * intr_mask_flag)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_port_link_intr_mask_get(dev_id, port_id, intr_mask_flag);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get link interrupt status on particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] intr_mask_flag interrupt mask
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_intr_port_link_status_get(a_uint32_t dev_id, a_uint32_t port_id, a_uint32_t * intr_mask_flag)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_port_link_intr_status_get(dev_id, port_id, intr_mask_flag);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set mac link change interrupt mask on particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable ports intr mask enabled
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_intr_mask_mac_linkchg_set(a_uint32_t dev_id, a_uint32_t port_id, a_bool_t enable)
{
    sw_error_t rv;
    FAL_API_LOCK;
    rv = _isisc_intr_mask_mac_linkchg_set(dev_id, port_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get mac link change interrupt mask on particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] port interrupt mask or not
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_intr_mask_mac_linkchg_get(a_uint32_t dev_id, a_uint32_t port_id, a_bool_t * enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _isisc_intr_mask_mac_linkchg_get(dev_id, port_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get link change interrupt status for all ports.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] ports bitmap which generates interrupt
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_intr_status_mac_linkchg_get(a_uint32_t dev_id, fal_pbmp_t* port_bitmap)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _isisc_intr_status_mac_linkchg_get(dev_id, port_bitmap);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set cpu vid enable status on a particular device.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_cpu_vid_en_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_cpu_vid_en_set(dev_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get cpu vid enable status on a particular device.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_cpu_vid_en_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_cpu_vid_en_get(dev_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set RM_RTD_PPPOE_EN status on a particular device.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_rtd_pppoe_en_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_rtd_pppoe_en_set(dev_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get RM_RTD_PPPOE_EN status on a particular device.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_rtd_pppoe_en_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_rtd_pppoe_en_get(dev_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Clear link change interrupt status for all ports.
 * @param[in] dev_id device id
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_intr_status_mac_linkchg_clear(a_uint32_t dev_id)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _isisc_intr_status_mac_linkchg_clear(dev_id);
    FAL_API_UNLOCK;
    return rv;
}
#endif

sw_error_t
isisc_misc_init(a_uint32_t dev_id)
{
    HSL_DEV_ID_CHECK(dev_id);

#ifndef HSL_STANDALONG
    {
        hsl_api_t *p_api;

        SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

        p_api->frame_max_size_set = isisc_frame_max_size_set;
        p_api->frame_max_size_get = isisc_frame_max_size_get;
        p_api->port_unk_uc_filter_set = isisc_port_unk_uc_filter_set;
        p_api->port_unk_mc_filter_set = isisc_port_unk_mc_filter_set;
        p_api->port_bc_filter_set = isisc_port_bc_filter_set;
	p_api->cpu_port_status_set = isisc_cpu_port_status_set;
#ifndef IN_MISC_MINI
	p_api->port_unk_uc_filter_get = isisc_port_unk_uc_filter_get;
	p_api->port_unk_mc_filter_get = isisc_port_unk_mc_filter_get;
        p_api->port_bc_filter_get = isisc_port_bc_filter_get;
        p_api->cpu_port_status_get = isisc_cpu_port_status_get;
        p_api->pppoe_cmd_set = isisc_pppoe_cmd_set;
        p_api->pppoe_cmd_get = isisc_pppoe_cmd_get;
        p_api->pppoe_status_set = isisc_pppoe_status_set;
        p_api->pppoe_status_get = isisc_pppoe_status_get;
        p_api->port_dhcp_set = isisc_port_dhcp_set;
        p_api->port_dhcp_get = isisc_port_dhcp_get;
        p_api->arp_cmd_set = isisc_arp_cmd_set;
        p_api->arp_cmd_get = isisc_arp_cmd_get;
#endif
        p_api->eapol_cmd_set = isisc_eapol_cmd_set;
#ifndef IN_MISC_MINI
        p_api->eapol_cmd_get = isisc_eapol_cmd_get;
        p_api->pppoe_session_table_add = isisc_pppoe_session_table_add;
        p_api->pppoe_session_table_del = isisc_pppoe_session_table_del;
        p_api->pppoe_session_table_get = isisc_pppoe_session_table_get;
        p_api->pppoe_session_id_set = isisc_pppoe_session_id_set;
        p_api->pppoe_session_id_get = isisc_pppoe_session_id_get;
#endif
        p_api->eapol_status_set = isisc_eapol_status_set;
#ifndef IN_MISC_MINI
        p_api->eapol_status_get = isisc_eapol_status_get;
        p_api->ripv1_status_set = isisc_ripv1_status_set;
        p_api->ripv1_status_get = isisc_ripv1_status_get;
        p_api->port_arp_req_status_set = isisc_port_arp_req_status_set;
        p_api->port_arp_req_status_get = isisc_port_arp_req_status_get;
        p_api->port_arp_ack_status_set = isisc_port_arp_ack_status_set;
        p_api->port_arp_ack_status_get = isisc_port_arp_ack_status_get;
        p_api->intr_mask_set = isisc_intr_mask_set;
        p_api->intr_mask_get = isisc_intr_mask_get;
        p_api->intr_status_get = isisc_intr_status_get;
        p_api->intr_status_clear = isisc_intr_status_clear;
        p_api->intr_port_link_mask_set = isisc_intr_port_link_mask_set;
        p_api->intr_port_link_mask_get = isisc_intr_port_link_mask_get;
        p_api->intr_port_link_status_get = isisc_intr_port_link_status_get;
        p_api->intr_mask_mac_linkchg_set = isisc_intr_mask_mac_linkchg_set;
        p_api->intr_mask_mac_linkchg_get = isisc_intr_mask_mac_linkchg_get;
        p_api->intr_status_mac_linkchg_get = isisc_intr_status_mac_linkchg_get;
        p_api->cpu_vid_en_set = isisc_cpu_vid_en_set;
        p_api->cpu_vid_en_get = isisc_cpu_vid_en_get;
        p_api->rtd_pppoe_en_set = isisc_rtd_pppoe_en_set;
        p_api->rtd_pppoe_en_get = isisc_rtd_pppoe_en_get;
        p_api->intr_status_mac_linkchg_clear = isisc_intr_status_mac_linkchg_clear;
#endif

    }
#endif

    return SW_OK;
}

/**
 * @}
 */
