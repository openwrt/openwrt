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
 * @defgroup shiva_port_vlan SHIVA_PORT_VLAN
 * @{
 */
#include "sw.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_port_prop.h"
#include "shiva_portvlan.h"
#include "shiva_reg.h"

#define MAX_VLAN_ID           4095
#define SHIVA_MAX_VLAN_TRANS  16
#define SHIVA_VLAN_TRANS_ADDR 0x59000

static sw_error_t
_shiva_port_1qmode_set(a_uint32_t dev_id, fal_port_t port_id,
                       fal_pt_1qmode_t port_1qmode)
{
    sw_error_t rv;
    a_uint32_t regval[FAL_1Q_MODE_BUTT] = { 0, 3, 2, 1 };

    HSL_DEV_ID_CHECK(dev_id);

    if (A_FALSE == hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    if (FAL_1Q_MODE_BUTT <= port_1qmode)
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_SET(rv, dev_id, PORT_BASE_VLAN, port_id, DOT1Q_MODE,
                      (a_uint8_t *) (&regval[port_1qmode]),
                      sizeof (a_uint32_t));

    return rv;

}

static sw_error_t
_shiva_port_1qmode_get(a_uint32_t dev_id, fal_port_t port_id,
                       fal_pt_1qmode_t * pport_1qmode)
{
    sw_error_t rv;
    a_uint32_t regval = 0;
    fal_pt_1qmode_t retval[4] = { FAL_1Q_DISABLE, FAL_1Q_FALLBACK,
                                  FAL_1Q_CHECK, FAL_1Q_SECURE
                                };

    HSL_DEV_ID_CHECK(dev_id);

    if (A_FALSE == hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    SW_RTN_ON_NULL(pport_1qmode);

    HSL_REG_FIELD_GET(rv, dev_id, PORT_BASE_VLAN, port_id, DOT1Q_MODE,
                      (a_uint8_t *) (&regval), sizeof (a_uint32_t));

    SW_RTN_ON_ERROR(rv);

    *pport_1qmode = retval[regval & 0x3];

    return SW_OK;

}

static sw_error_t
_shiva_port_egvlanmode_set(a_uint32_t dev_id, fal_port_t port_id,
                           fal_pt_1q_egmode_t port_egvlanmode)
{
    sw_error_t rv;
    a_uint32_t regval[FAL_EG_MODE_BUTT] = { 0, 1, 2, 3};

    HSL_DEV_ID_CHECK(dev_id);

    if (A_FALSE == hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    if (FAL_EG_MODE_BUTT <= port_egvlanmode)
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_SET(rv, dev_id, PORT_CTL, port_id, EG_VLAN_MODE,
                      (a_uint8_t *) (&regval[port_egvlanmode]),
                      sizeof (a_uint32_t));

    return rv;

}

static sw_error_t
_shiva_port_egvlanmode_get(a_uint32_t dev_id, fal_port_t port_id,
                           fal_pt_1q_egmode_t * pport_egvlanmode)
{
    sw_error_t rv;
    a_uint32_t regval = 0;
    fal_pt_1q_egmode_t retval[4] = { FAL_EG_UNMODIFIED, FAL_EG_UNTAGGED,
                                     FAL_EG_TAGGED, FAL_EG_HYBRID
                                   };

    HSL_DEV_ID_CHECK(dev_id);

    if (A_FALSE == hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    SW_RTN_ON_NULL(pport_egvlanmode);

    HSL_REG_FIELD_GET(rv, dev_id, PORT_CTL, port_id, EG_VLAN_MODE,
                      (a_uint8_t *) (&regval), sizeof (a_uint32_t));

    SW_RTN_ON_ERROR(rv);

    *pport_egvlanmode = retval[regval & 0x3];

    return SW_OK;

}

static sw_error_t
_shiva_portvlan_member_add(a_uint32_t dev_id, fal_port_t port_id,
                           a_uint32_t mem_port_id)
{
    sw_error_t rv;
    a_uint32_t regval = 0;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_FALSE == hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    if (A_FALSE == hsl_port_prop_check(dev_id, mem_port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_GET(rv, dev_id, PORT_BASE_VLAN, port_id,
                      PORT_VID_MEM, (a_uint8_t *) (&regval),
                      sizeof (a_uint32_t));

    SW_RTN_ON_ERROR(rv);

    regval |= (0x1UL << mem_port_id);

    HSL_REG_FIELD_SET(rv, dev_id, PORT_BASE_VLAN, port_id,
                      PORT_VID_MEM, (a_uint8_t *) (&regval),
                      sizeof (a_uint32_t));

    return rv;

}

static sw_error_t
_shiva_portvlan_member_del(a_uint32_t dev_id, fal_port_t port_id,
                           a_uint32_t mem_port_id)
{
    sw_error_t rv;
    a_uint32_t regval = 0;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_FALSE == hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    if (A_FALSE == hsl_port_prop_check(dev_id, mem_port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_GET(rv, dev_id, PORT_BASE_VLAN, port_id,
                      PORT_VID_MEM, (a_uint8_t *) (&regval),
                      sizeof (a_uint32_t));

    SW_RTN_ON_ERROR(rv);

    regval &= (~(0x1UL << mem_port_id));

    HSL_REG_FIELD_SET(rv, dev_id, PORT_BASE_VLAN, port_id,
                      PORT_VID_MEM, (a_uint8_t *) (&regval),
                      sizeof (a_uint32_t));

    return rv;

}

static sw_error_t
_shiva_portvlan_member_update(a_uint32_t dev_id, fal_port_t port_id,
                              fal_pbmp_t mem_port_map)
{
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_FALSE == hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    if (A_FALSE ==
            hsl_mports_prop_check(dev_id, mem_port_map, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_SET(rv, dev_id, PORT_BASE_VLAN, port_id,
                      PORT_VID_MEM, (a_uint8_t *) (&mem_port_map),
                      sizeof (a_uint32_t));

    return rv;
}

static sw_error_t
_shiva_portvlan_member_get(a_uint32_t dev_id, fal_port_t port_id,
                           fal_pbmp_t * mem_port_map)
{
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_FALSE == hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    *mem_port_map = 0;
    HSL_REG_FIELD_GET(rv, dev_id, PORT_BASE_VLAN, port_id,
                      PORT_VID_MEM, (a_uint8_t *) mem_port_map,
                      sizeof (a_uint32_t));

    SW_RTN_ON_ERROR(rv);

    return SW_OK;
}

static sw_error_t
_shiva_port_force_default_vid_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
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

    HSL_REG_FIELD_SET(rv, dev_id, PORT_DOT1AD, port_id,
                      FORCE_DEF_VID, (a_uint8_t *) (&val),
                      sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_shiva_port_force_default_vid_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable)
{
    sw_error_t rv;
    a_uint32_t val = 0;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_GET(rv, dev_id, PORT_DOT1AD, port_id,
                      FORCE_DEF_VID, (a_uint8_t *) (&val),
                      sizeof (a_uint32_t));
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
_shiva_port_force_portvlan_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
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

    HSL_REG_FIELD_SET(rv, dev_id, PORT_DOT1AD, port_id,
                      FORCE_PVLAN, (a_uint8_t *) (&val),
                      sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_shiva_port_force_portvlan_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable)
{
    sw_error_t rv;
    a_uint32_t val = 0;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_GET(rv, dev_id, PORT_DOT1AD, port_id,
                      FORCE_PVLAN, (a_uint8_t *) (&val),
                      sizeof (a_uint32_t));
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
_shiva_nestvlan_tpid_set(a_uint32_t dev_id, a_uint32_t tpid)
{
    sw_error_t rv;
    a_uint32_t val;

    HSL_DEV_ID_CHECK(dev_id);

    val = tpid;
    HSL_REG_FIELD_SET(rv, dev_id, SERVICE_TAG, 0,
                      TAG_VALUE, (a_uint8_t *) (&val),
                      sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_shiva_nestvlan_tpid_get(a_uint32_t dev_id, a_uint32_t *tpid)
{
    sw_error_t rv;
    a_uint32_t val = 0;

    HSL_DEV_ID_CHECK(dev_id);

    HSL_REG_FIELD_GET(rv, dev_id, SERVICE_TAG, 0,
                      TAG_VALUE, (a_uint8_t *) (&val),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    *tpid = val;
    return SW_OK;
}

static sw_error_t
_shiva_port_invlan_mode_set(a_uint32_t dev_id, fal_port_t port_id,
                            fal_pt_invlan_mode_t mode)
{
    sw_error_t rv;
    a_uint32_t regval[FAL_INVLAN_MODE_BUTT] = { 0, 1, 2};

    HSL_DEV_ID_CHECK(dev_id);

    if (A_FALSE == hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    if (FAL_INVLAN_MODE_BUTT <= mode)
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_SET(rv, dev_id, PORT_BASE_VLAN, port_id, IN_VLAN_MODE,
                      (a_uint8_t *) (&regval[mode]),
                      sizeof (a_uint32_t));

    return rv;
}

static sw_error_t
_shiva_port_invlan_mode_get(a_uint32_t dev_id, fal_port_t port_id,
                            fal_pt_invlan_mode_t * mode)
{
    sw_error_t rv;
    a_uint32_t regval = 0;
    fal_pt_invlan_mode_t retval[FAL_INVLAN_MODE_BUTT] = { FAL_INVLAN_ADMIT_ALL,
            FAL_INVLAN_ADMIT_TAGGED, FAL_INVLAN_ADMIT_UNTAGGED
                                                        };

    HSL_DEV_ID_CHECK(dev_id);

    if (A_FALSE == hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    SW_RTN_ON_NULL(mode);

    HSL_REG_FIELD_GET(rv, dev_id, PORT_BASE_VLAN, port_id, IN_VLAN_MODE,
                      (a_uint8_t *) (&regval),
                      sizeof (a_uint32_t));

    SW_RTN_ON_ERROR(rv);

    if (regval >= 3)
    {
        return SW_FAIL;
    }
    *mode = retval[regval & 0x3];

    return rv;
}

static sw_error_t
_shiva_port_tls_set(a_uint32_t dev_id, fal_port_t port_id,
                    a_bool_t enable)
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

    HSL_REG_FIELD_SET(rv, dev_id, PORT_DOT1AD, port_id,
                      TLS_EN, (a_uint8_t *) (&val),
                      sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_shiva_port_tls_get(a_uint32_t dev_id, fal_port_t port_id,
                    a_bool_t * enable)
{
    sw_error_t rv;
    a_uint32_t val = 0;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_GET(rv, dev_id, PORT_DOT1AD, port_id,
                      TLS_EN, (a_uint8_t *) (&val),
                      sizeof (a_uint32_t));
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
_shiva_port_pri_propagation_set(a_uint32_t dev_id, fal_port_t port_id,
                                a_bool_t enable)
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

    HSL_REG_FIELD_SET(rv, dev_id, PORT_BASE_VLAN, port_id,
                      PRI_PROPAGATION, (a_uint8_t *) (&val),
                      sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_shiva_port_pri_propagation_get(a_uint32_t dev_id, fal_port_t port_id,
                                a_bool_t * enable)
{
    sw_error_t rv;
    a_uint32_t val = 0;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_GET(rv, dev_id, PORT_BASE_VLAN, port_id,
                      PRI_PROPAGATION, (a_uint8_t *) (&val),
                      sizeof (a_uint32_t));
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
_shiva_port_default_svid_set(a_uint32_t dev_id, fal_port_t port_id,
                             a_uint32_t vid)
{
    sw_error_t rv;
    a_uint32_t val;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    if ((0 == vid) || (vid > MAX_VLAN_ID))
    {
        return SW_BAD_PARAM;
    }

    val = vid;
    HSL_REG_FIELD_SET(rv, dev_id, PORT_DOT1AD, port_id,
                      DEF_SVID, (a_uint8_t *) (&val),
                      sizeof (a_uint32_t));

    return rv;
}

static sw_error_t
_shiva_port_default_svid_get(a_uint32_t dev_id, fal_port_t port_id,
                             a_uint32_t * vid)
{
    sw_error_t rv;
    a_uint32_t val = 0;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_GET(rv, dev_id, PORT_DOT1AD, port_id,
                      DEF_SVID, (a_uint8_t *) (&val),
                      sizeof (a_uint32_t));

    *vid = val & 0xfff;
    return rv;
}

static sw_error_t
_shiva_port_default_cvid_set(a_uint32_t dev_id, fal_port_t port_id,
                             a_uint32_t vid)
{
    sw_error_t rv;
    a_uint32_t val;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    if ((0 == vid) || (vid > MAX_VLAN_ID))
    {
        return SW_BAD_PARAM;
    }

    val = vid;
    HSL_REG_FIELD_SET(rv, dev_id, PORT_DOT1AD, port_id,
                      DEF_CVID, (a_uint8_t *) (&val),
                      sizeof (a_uint32_t));

    return rv;
}

static sw_error_t
_shiva_port_default_cvid_get(a_uint32_t dev_id, fal_port_t port_id,
                             a_uint32_t * vid)
{
    sw_error_t rv;
    a_uint32_t val = 0;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_GET(rv, dev_id, PORT_DOT1AD, port_id,
                      DEF_CVID, (a_uint8_t *) (&val),
                      sizeof (a_uint32_t));

    *vid = val & 0xfff;
    return rv;
}

static sw_error_t
_shiva_port_vlan_propagation_set(a_uint32_t dev_id, fal_port_t port_id,
                                 fal_vlan_propagation_mode_t mode)
{
    sw_error_t rv;
    a_uint32_t reg = 0, p, c;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    if (FAL_VLAN_PROPAGATION_DISABLE == mode)
    {
        p = 0;
        c = 0;
    }
    else if (FAL_VLAN_PROPAGATION_CLONE == mode)
    {
        p = 1;
        c = 1;
    }
    else if (FAL_VLAN_PROPAGATION_REPLACE == mode)
    {
        p = 1;
        c = 0;
    }
    else
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_ENTRY_GET(rv, dev_id, PORT_DOT1AD, port_id,
                      (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    SW_SET_REG_BY_FIELD(PORT_DOT1AD, PROPAGATION_EN, p, reg);
    SW_SET_REG_BY_FIELD(PORT_DOT1AD, CLONE, c, reg);

    HSL_REG_ENTRY_SET(rv, dev_id, PORT_DOT1AD, port_id,
                      (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_shiva_port_vlan_propagation_get(a_uint32_t dev_id, fal_port_t port_id,
                                 fal_vlan_propagation_mode_t * mode)
{
    sw_error_t rv;
    a_uint32_t reg = 0, p, c;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_ENTRY_GET(rv, dev_id, PORT_DOT1AD, port_id,
                      (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    SW_GET_FIELD_BY_REG(PORT_DOT1AD, PROPAGATION_EN, p, reg);
    SW_GET_FIELD_BY_REG(PORT_DOT1AD, CLONE, c, reg);

    if (p)
    {
        if (c)
        {
            *mode = FAL_VLAN_PROPAGATION_CLONE;
        }
        else
        {
            *mode = FAL_VLAN_PROPAGATION_REPLACE;
        }
    }
    else
    {
        *mode = FAL_VLAN_PROPAGATION_DISABLE;
    }

    return SW_OK;
}

static sw_error_t
_shiva_vlan_trans_read(a_uint32_t dev_id, a_uint32_t entry_idx, fal_pbmp_t * pbmp, fal_vlan_trans_entry_t *entry)
{
    sw_error_t rv;
    a_uint32_t i, addr, table[2] = {0};

    addr = SHIVA_VLAN_TRANS_ADDR + (entry_idx << 3);

    /* get vlan trans table */
    for (i = 0; i < 2; i++)
    {
        HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr + (i << 2), sizeof (a_uint32_t),
                              (a_uint8_t *) (&(table[i])), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }

    if (0x1 & (table[1] >> 4))
    {
        entry->o_vid = table[0] & 0xfff;
        entry->s_vid = (table[0] >> 12) & 0xfff;
        entry->c_vid = ((table[0] >> 24) & 0xff) | ((table[1] & 0xf) << 8);
        entry->bi_dir = (~(table[1] >> 5)) & 0x1;
        *pbmp     =  (table[1] >> 6) & 0x7f;
        return SW_OK;
    }
    else
    {
        return SW_EMPTY;
    }
}

static sw_error_t
_shiva_vlan_trans_write(a_uint32_t dev_id, a_uint32_t entry_idx, fal_pbmp_t pbmp, fal_vlan_trans_entry_t entry)
{
    sw_error_t rv;
    a_uint32_t i, addr, table[2] = {0};

    addr = SHIVA_VLAN_TRANS_ADDR + (entry_idx << 3);

    if (0 != pbmp)
    {
        table[0]  = entry.o_vid & 0xfff;
        table[0] |= ((entry.s_vid & 0xfff) << 12);
        table[0] |= ((entry.c_vid & 0xff) << 24);
        table[1]  = (entry.c_vid >> 8) & 0xf;
        table[1] |= (0x1 << 4);
        table[1] |= (((~(entry.bi_dir))& 0x1) << 5);
        table[1] |= (pbmp << 6);
    }

    /* set vlan trans table */
    for (i = 0; i < 2; i++)
    {
        HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr + (i << 2), sizeof (a_uint32_t),
                              (a_uint8_t *) (&(table[i])), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }

    return SW_OK;
}

static sw_error_t
_shiva_port_vlan_trans_add(a_uint32_t dev_id, fal_port_t port_id, fal_vlan_trans_entry_t *entry)
{
    sw_error_t rv;
    fal_pbmp_t t_pbmp;
    a_uint32_t idx, entry_idx = 0xffff, old_idx = 0xffff;
    fal_vlan_trans_entry_t entry_temp;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    for (idx = 0; idx < SHIVA_MAX_VLAN_TRANS; idx++)
    {
        aos_mem_set(&entry_temp, 0, sizeof(fal_vlan_trans_entry_t));
        rv = _shiva_vlan_trans_read(dev_id, idx, &t_pbmp, &entry_temp);
        if (SW_EMPTY == rv)
        {
            entry_idx = idx;
            continue;
        }
        SW_RTN_ON_ERROR(rv);

        if (!aos_mem_cmp(&entry_temp, entry, sizeof(fal_vlan_trans_entry_t)))
        {
            if (SW_IS_PBMP_MEMBER(t_pbmp, port_id))
            {
                return SW_ALREADY_EXIST;
            }
            old_idx = idx;
            break;
        }
    }

    if (0xffff != old_idx)
    {
        t_pbmp |= (0x1 << port_id);
        entry_idx = old_idx;
    }
    else if (0xffff != entry_idx)
    {
        t_pbmp  = (0x1 << port_id);
        aos_mem_copy(&entry_temp, entry, sizeof(fal_vlan_trans_entry_t));
    }
    else
    {
        return SW_NO_RESOURCE;
    }

    return _shiva_vlan_trans_write(dev_id, entry_idx, t_pbmp, entry_temp);
}

static sw_error_t
_shiva_port_vlan_trans_del(a_uint32_t dev_id, fal_port_t port_id, fal_vlan_trans_entry_t *entry)
{
    sw_error_t rv;
    fal_pbmp_t t_pbmp;
    a_uint32_t idx, entry_idx = 0xffff;
    fal_vlan_trans_entry_t entry_temp;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    for (idx = 0; idx < SHIVA_MAX_VLAN_TRANS; idx++)
    {
        aos_mem_set(&entry_temp, 0, sizeof(fal_vlan_trans_entry_t));
        rv = _shiva_vlan_trans_read(dev_id, idx, &t_pbmp, &entry_temp);
        if (SW_EMPTY == rv)
        {
            continue;
        }
        SW_RTN_ON_ERROR(rv);

        if ((entry->o_vid == entry_temp.o_vid)
                && (entry->bi_dir == entry_temp.bi_dir))
        {
            if (SW_IS_PBMP_MEMBER(t_pbmp, port_id))
            {
                entry_idx = idx;
                break;
            }
        }
    }

    if (0xffff != entry_idx)
    {
        t_pbmp &= (~(0x1 << port_id));
    }
    else
    {
        return SW_NOT_FOUND;
    }

    return _shiva_vlan_trans_write(dev_id, entry_idx, t_pbmp, entry_temp);
}

static sw_error_t
_shiva_port_vlan_trans_get(a_uint32_t dev_id, fal_port_t port_id, fal_vlan_trans_entry_t *entry)
{
    sw_error_t rv;
    fal_pbmp_t t_pbmp;
    a_uint32_t idx;
    fal_vlan_trans_entry_t entry_temp;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    for (idx = 0; idx < SHIVA_MAX_VLAN_TRANS; idx++)
    {
        aos_mem_set(&entry_temp, 0, sizeof(fal_vlan_trans_entry_t));
        rv = _shiva_vlan_trans_read(dev_id, idx, &t_pbmp, &entry_temp);
        if (SW_EMPTY == rv)
        {
            continue;
        }

        SW_RTN_ON_ERROR(rv);
        if ((entry->o_vid == entry_temp.o_vid)
                && (entry->bi_dir == entry_temp.bi_dir))
        {
            if (SW_IS_PBMP_MEMBER(t_pbmp, port_id))
            {
                aos_mem_copy(entry, &entry_temp, sizeof(fal_vlan_trans_entry_t));
                return SW_OK;
            }
        }
    }

    return SW_NOT_FOUND;
}

static sw_error_t
_shiva_port_vlan_trans_iterate(a_uint32_t dev_id, fal_port_t port_id,
                               a_uint32_t * iterator, fal_vlan_trans_entry_t * entry)
{
    a_uint32_t index;
    sw_error_t rv;
    fal_vlan_trans_entry_t entry_t;
    fal_pbmp_t pbmp_t = 0;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    if ((NULL == iterator) || (NULL == entry))
    {
        return SW_BAD_PTR;
    }

    if (SHIVA_MAX_VLAN_TRANS == *iterator)
    {
        return SW_NO_MORE;
    }

    if (SHIVA_MAX_VLAN_TRANS < *iterator)
    {
        return SW_BAD_PARAM;
    }

    aos_mem_set(&entry_t, 0, sizeof(fal_vlan_trans_entry_t));
    for (index = *iterator; index < SHIVA_MAX_VLAN_TRANS; index++)
    {
        rv = _shiva_vlan_trans_read(dev_id, index, &pbmp_t, &entry_t);
        if (SW_EMPTY == rv)
        {
            continue;
        }

        if (SW_IS_PBMP_MEMBER(pbmp_t, port_id))
        {
            aos_mem_copy(entry, &entry_t, sizeof(fal_vlan_trans_entry_t));
            break;
        }
    }

    if (SHIVA_MAX_VLAN_TRANS == index)
    {
        return SW_NO_MORE;
    }

    *iterator = index + 1;
    return SW_OK;
}

static sw_error_t
_shiva_qinq_mode_set(a_uint32_t dev_id, fal_qinq_mode_t mode)
{
    sw_error_t rv;
    a_uint32_t stag = 0;

    HSL_DEV_ID_CHECK(dev_id);

    if (FAL_QINQ_MODE_BUTT <= mode)
    {
        return SW_BAD_PARAM;
    }

    if (FAL_QINQ_STAG_MODE == mode)
    {
        stag = 1;
    }

    HSL_REG_FIELD_SET(rv, dev_id, ADDR_TABLE_CTL, 0,
                      STAG_MODE, (a_uint8_t *) (&stag),
                      sizeof (a_uint32_t));

    return rv;
}

static sw_error_t
_shiva_qinq_mode_get(a_uint32_t dev_id, fal_qinq_mode_t * mode)
{
    sw_error_t rv;
    a_uint32_t stag = 0;

    HSL_DEV_ID_CHECK(dev_id);

    HSL_REG_FIELD_GET(rv, dev_id, ADDR_TABLE_CTL, 0,
                      STAG_MODE, (a_uint8_t *) (&stag),
                      sizeof (a_uint32_t));

    SW_RTN_ON_ERROR(rv);

    if (stag)
    {
        *mode = FAL_QINQ_STAG_MODE;
    }
    else
    {
        *mode = FAL_QINQ_CTAG_MODE;
    }

    return SW_OK;
}

static sw_error_t
_shiva_port_qinq_role_set(a_uint32_t dev_id, fal_port_t port_id, fal_qinq_port_role_t role)
{
    sw_error_t rv;
    a_uint32_t core = 0;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    if (FAL_QINQ_PORT_ROLE_BUTT <= role)
    {
        return SW_BAD_PARAM;
    }

    if (FAL_QINQ_CORE_PORT == role)
    {
        core = 1;
    }

    HSL_REG_FIELD_SET(rv, dev_id, PORT_BASE_VLAN, port_id,
                      COREP_EN, (a_uint8_t *) (&core),
                      sizeof (a_uint32_t));

    return rv;
}

static sw_error_t
_shiva_port_qinq_role_get(a_uint32_t dev_id, fal_port_t port_id, fal_qinq_port_role_t * role)
{
    sw_error_t rv;
    a_uint32_t core = 0;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_GET(rv, dev_id, PORT_BASE_VLAN, port_id,
                      COREP_EN, (a_uint8_t *) (&core),
                      sizeof (a_uint32_t));

    SW_RTN_ON_ERROR(rv);

    if (core)
    {
        *role = FAL_QINQ_CORE_PORT;
    }
    else
    {
        *role = FAL_QINQ_EDGE_PORT;
    }

    return SW_OK;
}

/**
 * @brief Set 802.1q work mode on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] port_1qmode 802.1q work mode
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_port_1qmode_set(a_uint32_t dev_id, fal_port_t port_id,
                      fal_pt_1qmode_t port_1qmode)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_port_1qmode_set(dev_id, port_id, port_1qmode);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get 802.1q work mode on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] port_1qmode 802.1q work mode
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_port_1qmode_get(a_uint32_t dev_id, fal_port_t port_id,
                      fal_pt_1qmode_t * pport_1qmode)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_port_1qmode_get(dev_id, port_id, pport_1qmode);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set packets transmitted out vlan tagged mode on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] port_egvlanmode packets transmitted out vlan tagged mode
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_port_egvlanmode_set(a_uint32_t dev_id, fal_port_t port_id,
                          fal_pt_1q_egmode_t port_egvlanmode)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_port_egvlanmode_set(dev_id, port_id, port_egvlanmode);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get packets transmitted out vlan tagged mode on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] port_egvlanmode packets transmitted out vlan tagged mode
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_port_egvlanmode_get(a_uint32_t dev_id, fal_port_t port_id,
                          fal_pt_1q_egmode_t * pport_egvlanmode)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_port_egvlanmode_get(dev_id, port_id, pport_egvlanmode);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Add member of port based vlan on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] mem_port_id port member
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_portvlan_member_add(a_uint32_t dev_id, fal_port_t port_id,
                          a_uint32_t mem_port_id)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_portvlan_member_add(dev_id, port_id, mem_port_id);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Delete member of port based vlan on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] mem_port_id port member
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_portvlan_member_del(a_uint32_t dev_id, fal_port_t port_id,
                          a_uint32_t mem_port_id)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_portvlan_member_del(dev_id, port_id, mem_port_id);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Update member of port based vlan on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] mem_port_map port members
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_portvlan_member_update(a_uint32_t dev_id, fal_port_t port_id,
                             fal_pbmp_t mem_port_map)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_portvlan_member_update(dev_id, port_id, mem_port_map);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get member of port based vlan on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] mem_port_map port members
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_portvlan_member_get(a_uint32_t dev_id, fal_port_t port_id,
                          fal_pbmp_t * mem_port_map)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_portvlan_member_get(dev_id, port_id, mem_port_map);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set force default vlan id status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_port_force_default_vid_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_port_force_default_vid_set(dev_id, port_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get force default vlan id status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_port_force_default_vid_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_port_force_default_vid_get(dev_id, port_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set force port based vlan status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_port_force_portvlan_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_port_force_portvlan_set(dev_id, port_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get force port based vlan status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_port_force_portvlan_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_port_force_portvlan_get(dev_id, port_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set nest vlan tpid on a particular device.
 * @param[in] dev_id device id
 * @param[in] tpid tag protocol identification
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_nestvlan_tpid_set(a_uint32_t dev_id, a_uint32_t tpid)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_nestvlan_tpid_set(dev_id, tpid);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get nest vlan tpid on a particular device.
 * @param[in] dev_id device id
 * @param[out] tpid tag protocol identification
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_nestvlan_tpid_get(a_uint32_t dev_id, a_uint32_t *tpid)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_nestvlan_tpid_get(dev_id, tpid);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set ingress vlan mode mode on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] mode ingress vlan mode
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_port_invlan_mode_set(a_uint32_t dev_id, fal_port_t port_id,
                           fal_pt_invlan_mode_t mode)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_port_invlan_mode_set(dev_id, port_id, mode);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get ingress vlan mode mode on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] mode ingress vlan mode
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_port_invlan_mode_get(a_uint32_t dev_id, fal_port_t port_id,
                           fal_pt_invlan_mode_t * mode)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_port_invlan_mode_get(dev_id, port_id, mode);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set tls status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_port_tls_set(a_uint32_t dev_id, fal_port_t port_id,
                   a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_port_tls_set(dev_id, port_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get tls status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_port_tls_get(a_uint32_t dev_id, fal_port_t port_id,
                   a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_port_tls_get(dev_id, port_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set priority propagation status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_port_pri_propagation_set(a_uint32_t dev_id, fal_port_t port_id,
                               a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_port_pri_propagation_set(dev_id, port_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get priority propagation status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_port_pri_propagation_get(a_uint32_t dev_id, fal_port_t port_id,
                               a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_port_pri_propagation_get(dev_id, port_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set default s-vid on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] vid s-vid
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_port_default_svid_set(a_uint32_t dev_id, fal_port_t port_id,
                            a_uint32_t vid)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_port_default_svid_set(dev_id, port_id, vid);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get default s-vid on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] vid s-vid
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_port_default_svid_get(a_uint32_t dev_id, fal_port_t port_id,
                            a_uint32_t * vid)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_port_default_svid_get(dev_id, port_id, vid);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set default c-vid on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] vid c-vid
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_port_default_cvid_set(a_uint32_t dev_id, fal_port_t port_id,
                            a_uint32_t vid)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_port_default_cvid_set(dev_id, port_id, vid);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get default c-vid on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] vid c-vid
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_port_default_cvid_get(a_uint32_t dev_id, fal_port_t port_id,
                            a_uint32_t * vid)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_port_default_cvid_get(dev_id, port_id, vid);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set vlan propagation status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] mode vlan propagation mode
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_port_vlan_propagation_set(a_uint32_t dev_id, fal_port_t port_id,
                                fal_vlan_propagation_mode_t mode)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_port_vlan_propagation_set(dev_id, port_id, mode);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get vlan propagation status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] mode vlan propagation mode
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_port_vlan_propagation_get(a_uint32_t dev_id, fal_port_t port_id,
                                fal_vlan_propagation_mode_t * mode)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_port_vlan_propagation_get(dev_id, port_id, mode);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Add a vlan translation entry to a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param entry vlan translation entry
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_port_vlan_trans_add(a_uint32_t dev_id, fal_port_t port_id, fal_vlan_trans_entry_t *entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_port_vlan_trans_add(dev_id, port_id, entry);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Delete a vlan translation entry from a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param entry vlan translation entry
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_port_vlan_trans_del(a_uint32_t dev_id, fal_port_t port_id, fal_vlan_trans_entry_t *entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_port_vlan_trans_del(dev_id, port_id, entry);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get a vlan translation entry from a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param entry vlan translation entry
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_port_vlan_trans_get(a_uint32_t dev_id, fal_port_t port_id, fal_vlan_trans_entry_t *entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_port_vlan_trans_get(dev_id, port_id, entry);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Iterate all vlan translation entries from a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] iterator translation entry index if it's zero means get the first entry
 * @param[out] iterator next valid translation entry index
 * @param[out] entry vlan translation entry
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_port_vlan_trans_iterate(a_uint32_t dev_id, fal_port_t port_id,
                              a_uint32_t * iterator, fal_vlan_trans_entry_t * entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_port_vlan_trans_iterate(dev_id, port_id, iterator, entry);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set switch qinq work mode on a particular device.
 * @param[in] dev_id device id
 * @param[in] mode qinq work mode
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_qinq_mode_set(a_uint32_t dev_id, fal_qinq_mode_t mode)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_qinq_mode_set(dev_id, mode);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get switch qinq work mode on a particular device.
 * @param[in] dev_id device id
 * @param[out] mode qinq work mode
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_qinq_mode_get(a_uint32_t dev_id, fal_qinq_mode_t * mode)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_qinq_mode_get(dev_id, mode);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set qinq role on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] role port role
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_port_qinq_role_set(a_uint32_t dev_id, fal_port_t port_id, fal_qinq_port_role_t role)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_port_qinq_role_set(dev_id, port_id, role);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get qinq role on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] role port role
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_port_qinq_role_get(a_uint32_t dev_id, fal_port_t port_id, fal_qinq_port_role_t * role)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_port_qinq_role_get(dev_id, port_id, role);
    HSL_API_UNLOCK;
    return rv;
}

sw_error_t
shiva_portvlan_init(a_uint32_t dev_id)
{
    a_uint32_t i;
    sw_error_t rv;
    fal_vlan_trans_entry_t entry_init;
    hsl_api_t *p_api;

    HSL_DEV_ID_CHECK(dev_id);

    aos_mem_set(&entry_init, 0, sizeof(fal_vlan_trans_entry_t));
    entry_init.bi_dir = A_TRUE;

    for (i = 0; i < SHIVA_MAX_VLAN_TRANS; i++)
    {
        rv = _shiva_vlan_trans_write(dev_id, i, 0, entry_init);
        SW_RTN_ON_ERROR(rv);
    }

#ifndef HSL_STANDALONG

    SW_RTN_ON_NULL (p_api = hsl_api_ptr_get(dev_id));

    p_api->port_1qmode_get = shiva_port_1qmode_get;
    p_api->port_1qmode_set = shiva_port_1qmode_set;
    p_api->port_egvlanmode_get = shiva_port_egvlanmode_get;
    p_api->port_egvlanmode_set = shiva_port_egvlanmode_set;
    p_api->portvlan_member_add = shiva_portvlan_member_add;
    p_api->portvlan_member_del = shiva_portvlan_member_del;
    p_api->portvlan_member_update = shiva_portvlan_member_update;
    p_api->portvlan_member_get = shiva_portvlan_member_get;
    p_api->port_force_default_vid_set = shiva_port_force_default_vid_set;
    p_api->port_force_default_vid_get = shiva_port_force_default_vid_get;
    p_api->port_force_portvlan_set = shiva_port_force_portvlan_set;
    p_api->port_force_portvlan_get = shiva_port_force_portvlan_get;
    p_api->nestvlan_tpid_set = shiva_nestvlan_tpid_set;
    p_api->nestvlan_tpid_get = shiva_nestvlan_tpid_get;
    p_api->port_invlan_mode_set = shiva_port_invlan_mode_set;
    p_api->port_invlan_mode_get = shiva_port_invlan_mode_get;
    p_api->port_tls_set = shiva_port_tls_set;
    p_api->port_tls_get = shiva_port_tls_get;
    p_api->port_pri_propagation_set = shiva_port_pri_propagation_set;
    p_api->port_pri_propagation_get = shiva_port_pri_propagation_get;
    p_api->port_default_svid_set = shiva_port_default_svid_set;
    p_api->port_default_svid_get = shiva_port_default_svid_get;
    p_api->port_default_cvid_set = shiva_port_default_cvid_set;
    p_api->port_default_cvid_get = shiva_port_default_cvid_get;
    p_api->port_vlan_propagation_set = shiva_port_vlan_propagation_set;
    p_api->port_vlan_propagation_get = shiva_port_vlan_propagation_get;
    p_api->port_vlan_trans_add = shiva_port_vlan_trans_add;
    p_api->port_vlan_trans_del = shiva_port_vlan_trans_del;
    p_api->port_vlan_trans_get = shiva_port_vlan_trans_get;
    p_api->qinq_mode_set = shiva_qinq_mode_set;
    p_api->qinq_mode_get = shiva_qinq_mode_get;
    p_api->port_qinq_role_set = shiva_port_qinq_role_set;
    p_api->port_qinq_role_get = shiva_port_qinq_role_get;
    p_api->port_vlan_trans_iterate = shiva_port_vlan_trans_iterate;
#endif

    return SW_OK;
}

/**
 * @}
 */

