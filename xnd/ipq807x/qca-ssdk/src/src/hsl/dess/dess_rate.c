/*
 * Copyright (c) 2014,2016 The Linux Foundation. All rights reserved.
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
 * @defgroup dess_rate DESS_RATE
 * @{
 */
#include "sw.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_port_prop.h"
#include "dess_rate.h"
#include "dess_reg.h"

#define DESS_MAX_POLICER_ID          31
#define DESS_MAX_QUEUE               3
#define DESS_MAX_EH_QUEUE            5

#define ACL_POLICER_CNT_SEL_ADDR          0x09f0
#define ACL_POLICER_CNT_MODE_ADDR         0x09f4
#define ACL_POLICER_CNT_RST_ADDR          0x09f8

static sw_error_t
_dess_rate_port_queue_check(fal_port_t port_id, fal_queue_t queue_id)
{
    if ((0 == port_id) || (5 == port_id) || (6 == port_id))
    {
        if (DESS_MAX_EH_QUEUE < queue_id)
        {
            return SW_BAD_PARAM;
        }
    }
    else
    {
        if (DESS_MAX_QUEUE < queue_id)
        {
            return SW_BAD_PARAM;
        }
    }

    return SW_OK;
}

static void
_dess_egress_bs_byte_sw_to_hw(a_uint32_t sw_bs, a_uint32_t * hw_bs)
{
    a_int32_t i;
    a_uint32_t data[8] =
    {
        0, 2 * 1024, 4 * 1024, 8 * 1024, 16 * 1024, 32 * 1024, 128 * 1024,
        512 * 1024
    };

    for (i = 7; i >= 0; i--)
    {
        if (sw_bs >= data[i])
        {
            *hw_bs = i;
            break;
        }
    }
}

static void
_dess_egress_bs_byte_hw_to_sw(a_uint32_t hw_bs, a_uint32_t * sw_bs)
{
    a_uint32_t data[8] =
    {
        0, 2 * 1024, 4 * 1024, 8 * 1024, 16 * 1024, 32 * 1024, 128 * 1024,
        512 * 1024
    };

    *sw_bs = data[hw_bs & 0x7];
}

static void
_dess_egress_bs_frame_sw_to_hw(a_uint32_t sw_bs, a_uint32_t * hw_bs)
{
    a_uint32_t data[8] = { 0, 2, 4, 16, 64, 256, 512, 1024 };
    a_int32_t i;

    for (i = 7; i >= 0; i--)
    {
        if (sw_bs >= data[i])
        {
            *hw_bs = i;
            break;
        }
    }
}

static void
_dess_egress_bs_frame_hw_to_sw(a_uint32_t hw_bs, a_uint32_t * sw_bs)
{
    a_uint32_t data[8] = { 0, 2, 4, 16, 64, 256, 512, 1024 };

    *sw_bs = data[hw_bs & 0x7];
}

static void
_dess_ingress_bs_byte_sw_to_hw(a_uint32_t sw_bs, a_uint32_t * hw_bs)
{
    a_int32_t i;
    a_uint32_t data[8] =
    {
        0, 4 * 1024, 32 * 1024, 128 * 1024, 512 * 1024, 2 * 1024 * 1024,
        8 * 1024 * 1024, 32 * 1024 * 1024
    };

    for (i = 7; i >= 0; i--)
    {
        if (sw_bs >= data[i])
        {
            *hw_bs = i;
            break;
        }
    }
}

static void
_dess_ingress_bs_byte_hw_to_sw(a_uint32_t hw_bs, a_uint32_t * sw_bs)
{
    a_uint32_t data[8] =
    {
        0, 4 * 1024, 32 * 1024, 128 * 1024, 512 * 1024, 2 * 1024 * 1024,
        8 * 1024 * 1024, 32 * 1024 * 1024
    };

    *sw_bs = data[hw_bs & 0x7];
}

static void
_dess_ingress_bs_frame_sw_to_hw(a_uint32_t sw_bs, a_uint32_t * hw_bs)
{
    a_uint32_t data[8] = { 0, 4, 16, 64, 256, 1024, 4096, 16384 };
    a_int32_t i;

    for (i = 7; i >= 0; i--)
    {
        if (sw_bs >= data[i])
        {
            *hw_bs = i;
            break;
        }
    }
}

static void
_dess_ingress_bs_frame_hw_to_sw(a_uint32_t hw_bs, a_uint32_t * sw_bs)
{
    a_uint32_t data[8] = { 0, 4, 16, 64, 256, 1024, 4096, 16384 };

    *sw_bs = data[hw_bs & 0x7];
}

static void
_dess_rate_flag_parse(a_uint32_t sw_flag, a_uint32_t * hw_flag)
{
    *hw_flag = 0;

    if (FAL_INGRESS_POLICING_TCP_CTRL & sw_flag)
    {
        *hw_flag |= (0x1 << 1);
    }

    if (FAL_INGRESS_POLICING_MANAGEMENT & sw_flag)
    {
        *hw_flag |= (0x1 << 2);
    }

    if (FAL_INGRESS_POLICING_BROAD & sw_flag)
    {
        *hw_flag |= (0x1 << 3);
    }

    if (FAL_INGRESS_POLICING_UNK_UNI & sw_flag)
    {
        *hw_flag |= (0x1 << 4);
    }

    if (FAL_INGRESS_POLICING_UNK_MUL & sw_flag)
    {
        *hw_flag |= (0x1 << 5);
    }

    if (FAL_INGRESS_POLICING_UNI & sw_flag)
    {
        *hw_flag |= (0x1 << 6);
    }

    if (FAL_INGRESS_POLICING_MUL & sw_flag)
    {
        *hw_flag |= (0x1 << 7);
    }
}

static void
_dess_rate_flag_reparse(a_uint32_t hw_flag, a_uint32_t * sw_flag)
{
    *sw_flag = 0;

    if (hw_flag & 0x2)
    {
        *sw_flag |= FAL_INGRESS_POLICING_TCP_CTRL;
    }

    if (hw_flag & 0x4)
    {
        *sw_flag |= FAL_INGRESS_POLICING_MANAGEMENT;
    }

    if (hw_flag & 0x8)
    {
        *sw_flag |= FAL_INGRESS_POLICING_BROAD;
    }

    if (hw_flag & 0x10)
    {
        *sw_flag |= FAL_INGRESS_POLICING_UNK_UNI;
    }

    if (hw_flag & 0x20)
    {
        *sw_flag |= FAL_INGRESS_POLICING_UNK_MUL;
    }

    if (hw_flag & 0x40)
    {
        *sw_flag |= FAL_INGRESS_POLICING_UNI;
    }

    if (hw_flag & 0x80)
    {
        *sw_flag |= FAL_INGRESS_POLICING_MUL;
    }
}

static void
_dess_rate_ts_parse(fal_rate_mt_t sw, a_uint32_t * hw)
{
    if (FAL_RATE_MI_100US == sw)
    {
        *hw = 0;
    }
    else if (FAL_RATE_MI_1MS == sw)
    {
        *hw = 1;
    }
    else if (FAL_RATE_MI_10MS == sw)
    {
        *hw = 2;
    }
    else if (FAL_RATE_MI_100MS)
    {
        *hw = 3;
    }
    else
    {
        *hw = 0;
    }
}

static void
_dess_rate_ts_reparse(a_uint32_t hw, fal_rate_mt_t * sw)
{
    if (0 == hw)
    {
        *sw = FAL_RATE_MI_100US;
    }
    else if (1 == hw)
    {
        *sw = FAL_RATE_MI_1MS;
    }
    else if (2 == hw)
    {
        *sw = FAL_RATE_MI_10MS;
    }
    else
    {
        *sw = FAL_RATE_MI_100MS;
    }
}

static sw_error_t
_dess_rate_port_policer_set(a_uint32_t dev_id, fal_port_t port_id,
                            fal_port_policer_t * policer)
{
    sw_error_t rv;
    a_uint32_t cir = 0x7fff, eir = 0x7fff, cbs = 0, ebs = 0, tmp, data[3] = { 0 };

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    data[0] = 0x18000000;
    if (FAL_BYTE_BASED == policer->meter_unit)
    {
        if (A_TRUE == policer->c_enable)
        {
            cir = policer->cir >> 5;
            policer->cir = cir << 5;
            _dess_ingress_bs_byte_sw_to_hw(policer->cbs, &cbs);
            _dess_ingress_bs_byte_hw_to_sw(cbs, &(policer->cbs));
        }

        if (A_TRUE == policer->e_enable)
        {
            eir = policer->eir >> 5;
            policer->eir = eir << 5;
            _dess_ingress_bs_byte_sw_to_hw(policer->ebs, &ebs);
            _dess_ingress_bs_byte_hw_to_sw(ebs, &(policer->ebs));
        }

        SW_SET_REG_BY_FIELD(INGRESS_POLICER1, INGRESS_UNIT, 0, data[1]);
    }
    else if (FAL_FRAME_BASED == policer->meter_unit)
    {
        if (A_TRUE == policer->c_enable)
        {
            cir = (policer->cir * 2) / 125;
            policer->cir = cir / 2 * 125 + cir % 2 * 63;
            _dess_ingress_bs_frame_sw_to_hw(policer->cbs, &cbs);
            _dess_ingress_bs_frame_hw_to_sw(cbs, &(policer->cbs));
        }

        if (A_TRUE == policer->e_enable)
        {
            eir = (policer->eir * 2) / 125;
            policer->eir = eir / 2 * 125 + eir % 2 * 63;
            _dess_ingress_bs_frame_sw_to_hw(policer->ebs, &ebs);
            _dess_ingress_bs_frame_hw_to_sw(ebs, &(policer->ebs));
        }

        SW_SET_REG_BY_FIELD(INGRESS_POLICER1, INGRESS_UNIT, 1, data[1]);
    }
    else
    {
        return SW_BAD_PARAM;
    }

    SW_SET_REG_BY_FIELD(INGRESS_POLICER0, INGRESS_CIR, cir, data[0]);
    SW_SET_REG_BY_FIELD(INGRESS_POLICER0, INGRESS_CBS, cbs, data[0]);
    SW_SET_REG_BY_FIELD(INGRESS_POLICER1, INGRESS_EIR, eir, data[1]);
    SW_SET_REG_BY_FIELD(INGRESS_POLICER1, INGRESS_EBS, ebs, data[1]);

    if (A_TRUE == policer->combine_mode)
    {
        SW_SET_REG_BY_FIELD(INGRESS_POLICER0, RATE_MODE, 1, data[0]);
    }

    if (A_TRUE == policer->deficit_en)
    {
        SW_SET_REG_BY_FIELD(INGRESS_POLICER1, INGRESS_BORROW, 1, data[1]);
    }

    if (A_TRUE == policer->color_mode)
    {
        SW_SET_REG_BY_FIELD(INGRESS_POLICER1, INGRESS_CM, 1, data[1]);
    }

    if (A_TRUE == policer->couple_flag)
    {
        SW_SET_REG_BY_FIELD(INGRESS_POLICER1, INGRESS_CF, 1, data[1]);
    }

    _dess_rate_ts_parse(policer->c_meter_interval, &tmp);
    SW_SET_REG_BY_FIELD(INGRESS_POLICER0, C_ING_TS, tmp, data[0]);

    _dess_rate_ts_parse(policer->e_meter_interval, &tmp);
    SW_SET_REG_BY_FIELD(INGRESS_POLICER1, E_ING_TS, tmp, data[1]);

    _dess_rate_flag_parse(policer->c_rate_flag, &tmp);
    data[2] = (tmp << 8) & 0xff00;

    _dess_rate_flag_parse(policer->e_rate_flag, &tmp);
    data[2] |= (tmp & 0xff);

    HSL_REG_ENTRY_SET(rv, dev_id, INGRESS_POLICER0, port_id,
                      (a_uint8_t *) (&data[0]), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    HSL_REG_ENTRY_SET(rv, dev_id, INGRESS_POLICER1, port_id,
                      (a_uint8_t *) (&data[1]), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    HSL_REG_ENTRY_SET(rv, dev_id, INGRESS_POLICER2, port_id,
                      (a_uint8_t *) (&data[2]), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_dess_rate_port_policer_get(a_uint32_t dev_id, fal_port_t port_id,
                            fal_port_policer_t * policer)
{
    sw_error_t rv;
    a_uint32_t unit, ts, cir, eir, cbs, ebs, data[3] = {0};

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_ENTRY_GET(rv, dev_id, INGRESS_POLICER0, port_id,
                      (a_uint8_t *) (&data[0]), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    HSL_REG_ENTRY_GET(rv, dev_id, INGRESS_POLICER1, port_id,
                      (a_uint8_t *) (&data[1]), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    HSL_REG_ENTRY_GET(rv, dev_id, INGRESS_POLICER2, port_id,
                      (a_uint8_t *) (&data[2]), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    SW_GET_FIELD_BY_REG(INGRESS_POLICER0, INGRESS_CIR, cir, data[0]);
    SW_GET_FIELD_BY_REG(INGRESS_POLICER0, INGRESS_CBS, cbs, data[0]);
    SW_GET_FIELD_BY_REG(INGRESS_POLICER1, INGRESS_EIR, eir, data[1]);
    SW_GET_FIELD_BY_REG(INGRESS_POLICER1, INGRESS_EBS, ebs, data[1]);
    SW_GET_FIELD_BY_REG(INGRESS_POLICER1, INGRESS_UNIT, unit, data[1]);

    policer->c_enable = A_TRUE;
    if (0x7fff == cir)
    {
        policer->c_enable = A_FALSE;
        cir = 0;
    }

    policer->e_enable = A_TRUE;
    if (0x7fff == eir)
    {
        policer->e_enable = A_FALSE;
        eir = 0;
    }

    if (unit)
    {
        policer->meter_unit = FAL_FRAME_BASED;
        policer->cir = cir / 2 * 125 + cir % 2 * 63;
        policer->eir = eir / 2 * 125 + eir % 2 * 63;
        _dess_ingress_bs_frame_hw_to_sw(cbs, &(policer->cbs));
        _dess_ingress_bs_frame_hw_to_sw(ebs, &(policer->ebs));
    }
    else
    {
        policer->meter_unit = FAL_BYTE_BASED;
        policer->cir = cir << 5;
        policer->eir = eir << 5;
        _dess_ingress_bs_byte_hw_to_sw(cbs, &(policer->cbs));
        _dess_ingress_bs_byte_hw_to_sw(ebs, &(policer->ebs));
    }

    SW_GET_FIELD_BY_REG(INGRESS_POLICER0, RATE_MODE, policer->combine_mode,
                        data[0]);
    SW_GET_FIELD_BY_REG(INGRESS_POLICER1, INGRESS_BORROW, policer->deficit_en,
                        data[1]);
    SW_GET_FIELD_BY_REG(INGRESS_POLICER1, INGRESS_CF, policer->couple_flag,
                        data[1]);
    SW_GET_FIELD_BY_REG(INGRESS_POLICER1, INGRESS_CM, policer->color_mode,
                        data[1]);

    ts = (data[2] >> 8) & 0xff;
    _dess_rate_flag_reparse(ts, &(policer->c_rate_flag));

    ts = data[2] & 0xff;
    _dess_rate_flag_reparse(ts, &(policer->e_rate_flag));

    SW_GET_FIELD_BY_REG(INGRESS_POLICER0, C_ING_TS, ts, data[0]);
    _dess_rate_ts_reparse(ts, &(policer->c_meter_interval));

    SW_GET_FIELD_BY_REG(INGRESS_POLICER1, E_ING_TS, ts, data[1]);
    _dess_rate_ts_reparse(ts, &(policer->e_meter_interval));

    return SW_OK;
}

static sw_error_t
_dess_rate_port_shaper_set(a_uint32_t dev_id, fal_port_t port_id,
                           a_bool_t enable, fal_egress_shaper_t * shaper)
{
    sw_error_t rv;
    a_uint32_t data, cir, eir, cbs = 0, ebs = 0;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    if (A_FALSE == enable)
    {
        aos_mem_zero(shaper, sizeof (fal_egress_shaper_t));

        cir = 0x7fff;
        eir = 0x7fff;
    }
    else
    {
        if (FAL_BYTE_BASED == shaper->meter_unit)
        {
            cir = shaper->cir >> 5;
            shaper->cir = cir << 5;

            eir = shaper->eir >> 5;
            shaper->eir = eir << 5;

            _dess_egress_bs_byte_sw_to_hw(shaper->cbs, &cbs);
            _dess_egress_bs_byte_hw_to_sw(cbs, &(shaper->cbs));

            _dess_egress_bs_byte_sw_to_hw(shaper->ebs, &ebs);
            _dess_egress_bs_byte_hw_to_sw(ebs, &(shaper->ebs));

            data = 0;
        }
        else if (FAL_FRAME_BASED == shaper->meter_unit)
        {
            cir = (shaper->cir * 2) / 125;
            shaper->cir = cir / 2 * 125 + cir % 2 * 63;

            eir = (shaper->eir * 2) / 125;
            shaper->eir = eir / 2 * 125 + eir % 2 * 63;

            _dess_egress_bs_frame_sw_to_hw(shaper->cbs, &cbs);
            _dess_egress_bs_frame_hw_to_sw(cbs, &(shaper->cbs));

            _dess_egress_bs_frame_sw_to_hw(shaper->ebs, &ebs);
            _dess_egress_bs_frame_hw_to_sw(ebs, &(shaper->ebs));

            data = 1;
        }
        else
        {
            return SW_BAD_PARAM;
        }

        HSL_REG_FIELD_SET(rv, dev_id, EG_SHAPER7, port_id, EG_Q0_UNIT,
                          (a_uint8_t *) (&data), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        data = 1;
        HSL_REG_FIELD_SET(rv, dev_id, EG_SHAPER7, port_id, EG_PT,
                          (a_uint8_t *) (&data), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        data = 0;
        HSL_REG_FIELD_SET(rv, dev_id, EG_SHAPER7, port_id, EG_TS,
                          (a_uint8_t *) (&data), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }

    HSL_REG_FIELD_SET(rv, dev_id, EG_SHAPER0, port_id, EG_Q0_CIR,
                      (a_uint8_t *) (&cir), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    HSL_REG_FIELD_SET(rv, dev_id, EG_SHAPER3, port_id, EG_Q0_EIR,
                      (a_uint8_t *) (&eir), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    HSL_REG_FIELD_SET(rv, dev_id, EG_SHAPER6, port_id, EG_Q0_CBS,
                      (a_uint8_t *) (&cbs), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    HSL_REG_FIELD_SET(rv, dev_id, EG_SHAPER6, port_id, EG_Q0_EBS,
                      (a_uint8_t *) (&ebs), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_dess_rate_port_shaper_get(a_uint32_t dev_id, fal_port_t port_id,
                           a_bool_t * enable, fal_egress_shaper_t * shaper)
{
    sw_error_t rv;
    a_uint32_t data = 0, cir = 0, eir = 0, cbs = 0, ebs = 0;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    aos_mem_zero(shaper, sizeof (fal_egress_shaper_t));

    HSL_REG_FIELD_GET(rv, dev_id, EG_SHAPER7, port_id, EG_PT,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (!data)
    {
        *enable = A_FALSE;
        return SW_OK;
    }

    HSL_REG_FIELD_GET(rv, dev_id, EG_SHAPER0, port_id, EG_Q0_CIR,
                      (a_uint8_t *) (&cir), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    HSL_REG_FIELD_GET(rv, dev_id, EG_SHAPER3, port_id, EG_Q0_EIR,
                      (a_uint8_t *) (&eir), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    HSL_REG_FIELD_GET(rv, dev_id, EG_SHAPER6, port_id, EG_Q0_CBS,
                      (a_uint8_t *) (&cbs), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    HSL_REG_FIELD_GET(rv, dev_id, EG_SHAPER6, port_id, EG_Q0_EBS,
                      (a_uint8_t *) (&ebs), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if ((0x7fff == cir) && (0x7fff == eir))
    {
        *enable = A_FALSE;
        return SW_OK;
    }

    *enable = A_TRUE;
    HSL_REG_FIELD_GET(rv, dev_id, EG_SHAPER7, port_id, EG_Q0_UNIT,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (data)
    {
        shaper->meter_unit = FAL_FRAME_BASED;
        shaper->cir = cir / 2 * 125 + cir % 2 * 63;
        shaper->eir = eir / 2 * 125 + eir % 2 * 63;
        _dess_egress_bs_frame_hw_to_sw(cbs, &(shaper->cbs));
        _dess_egress_bs_frame_hw_to_sw(ebs, &(shaper->ebs));
    }
    else
    {
        shaper->meter_unit = FAL_BYTE_BASED;
        shaper->cir = cir << 5;
        shaper->eir = eir << 5;
        _dess_egress_bs_byte_hw_to_sw(cbs, &(shaper->cbs));
        _dess_egress_bs_byte_hw_to_sw(ebs, &(shaper->ebs));
    }

    return SW_OK;
}

static sw_error_t
_dess_rate_queue_shaper_set(a_uint32_t dev_id, fal_port_t port_id,
                            fal_queue_t queue_id, a_bool_t enable,
                            fal_egress_shaper_t * shaper)
{
    sw_error_t rv;
    a_uint32_t unit = 0, data, cir, eir, cbs = 0, ebs = 0;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    rv = _dess_rate_port_queue_check(port_id, queue_id);
    SW_RTN_ON_ERROR(rv);

    if (A_FALSE == enable)
    {
        aos_mem_zero(shaper, sizeof (fal_egress_shaper_t));

        cir = 0x7fff;
        eir = 0x7fff;
    }
    else
    {
        if (FAL_BYTE_BASED == shaper->meter_unit)
        {
            cir = shaper->cir >> 5;
            shaper->cir = cir << 5;

            eir = shaper->eir >> 5;
            shaper->eir = eir << 5;

            _dess_egress_bs_byte_sw_to_hw(shaper->cbs, &cbs);
            _dess_egress_bs_byte_hw_to_sw(cbs, &(shaper->cbs));

            _dess_egress_bs_byte_sw_to_hw(shaper->ebs, &ebs);
            _dess_egress_bs_byte_hw_to_sw(ebs, &(shaper->ebs));

            unit = 0;
        }
        else if (FAL_FRAME_BASED == shaper->meter_unit)
        {
            cir = (shaper->cir * 2) / 125;
            shaper->cir = cir / 2 * 125 + cir % 2 * 63;

            eir = (shaper->eir * 2) / 125;
            shaper->eir = eir / 2 * 125 + eir % 2 * 63;

            _dess_egress_bs_frame_sw_to_hw(shaper->cbs, &cbs);
            _dess_egress_bs_frame_hw_to_sw(cbs, &(shaper->cbs));

            _dess_egress_bs_frame_sw_to_hw(shaper->ebs, &ebs);
            _dess_egress_bs_frame_hw_to_sw(ebs, &(shaper->ebs));

            unit = 1;
        }

        data = 0;
        HSL_REG_FIELD_SET(rv, dev_id, EG_SHAPER7, port_id, EG_PT,
                          (a_uint8_t *) (&data), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        data = 0;
        HSL_REG_FIELD_SET(rv, dev_id, EG_SHAPER7, port_id, EG_TS,
                          (a_uint8_t *) (&data), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }

    if (0 == queue_id)
    {
        HSL_REG_FIELD_SET(rv, dev_id, EG_SHAPER0, port_id, EG_Q0_CIR,
                          (a_uint8_t *) (&cir), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        HSL_REG_FIELD_SET(rv, dev_id, EG_SHAPER3, port_id, EG_Q0_EIR,
                          (a_uint8_t *) (&eir), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        HSL_REG_FIELD_SET(rv, dev_id, EG_SHAPER6, port_id, EG_Q0_CBS,
                          (a_uint8_t *) (&cbs), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        HSL_REG_FIELD_SET(rv, dev_id, EG_SHAPER6, port_id, EG_Q0_EBS,
                          (a_uint8_t *) (&ebs), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        HSL_REG_FIELD_SET(rv, dev_id, EG_SHAPER7, port_id, EG_Q0_UNIT,
                          (a_uint8_t *) (&unit), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }
    else if (1 == queue_id)
    {
        HSL_REG_FIELD_SET(rv, dev_id, EG_SHAPER0, port_id, EG_Q1_CIR,
                          (a_uint8_t *) (&cir), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        HSL_REG_FIELD_SET(rv, dev_id, EG_SHAPER3, port_id, EG_Q1_EIR,
                          (a_uint8_t *) (&eir), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        HSL_REG_FIELD_SET(rv, dev_id, EG_SHAPER6, port_id, EG_Q1_CBS,
                          (a_uint8_t *) (&cbs), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        HSL_REG_FIELD_SET(rv, dev_id, EG_SHAPER6, port_id, EG_Q1_EBS,
                          (a_uint8_t *) (&ebs), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        HSL_REG_FIELD_SET(rv, dev_id, EG_SHAPER7, port_id, EG_Q1_UNIT,
                          (a_uint8_t *) (&unit), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }
    else if (2 == queue_id)
    {
        HSL_REG_FIELD_SET(rv, dev_id, EG_SHAPER1, port_id, EG_Q2_CIR,
                          (a_uint8_t *) (&cir), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        HSL_REG_FIELD_SET(rv, dev_id, EG_SHAPER4, port_id, EG_Q2_EIR,
                          (a_uint8_t *) (&eir), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        HSL_REG_FIELD_SET(rv, dev_id, EG_SHAPER6, port_id, EG_Q2_CBS,
                          (a_uint8_t *) (&cbs), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        HSL_REG_FIELD_SET(rv, dev_id, EG_SHAPER6, port_id, EG_Q2_EBS,
                          (a_uint8_t *) (&ebs), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        HSL_REG_FIELD_SET(rv, dev_id, EG_SHAPER7, port_id, EG_Q2_UNIT,
                          (a_uint8_t *) (&unit), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }
    else if (3 == queue_id)
    {
        HSL_REG_FIELD_SET(rv, dev_id, EG_SHAPER1, port_id, EG_Q3_CIR,
                          (a_uint8_t *) (&cir), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        HSL_REG_FIELD_SET(rv, dev_id, EG_SHAPER4, port_id, EG_Q3_EIR,
                          (a_uint8_t *) (&eir), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        HSL_REG_FIELD_SET(rv, dev_id, EG_SHAPER6, port_id, EG_Q3_CBS,
                          (a_uint8_t *) (&cbs), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        HSL_REG_FIELD_SET(rv, dev_id, EG_SHAPER6, port_id, EG_Q3_EBS,
                          (a_uint8_t *) (&ebs), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        HSL_REG_FIELD_SET(rv, dev_id, EG_SHAPER7, port_id, EG_Q3_UNIT,
                          (a_uint8_t *) (&unit), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }
    else if (4 == queue_id)
    {
        HSL_REG_FIELD_SET(rv, dev_id, EG_SHAPER2, port_id, EG_Q4_CIR,
                          (a_uint8_t *) (&cir), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        HSL_REG_FIELD_SET(rv, dev_id, EG_SHAPER5, port_id, EG_Q4_EIR,
                          (a_uint8_t *) (&eir), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        HSL_REG_FIELD_SET(rv, dev_id, EG_SHAPER7, port_id, EG_Q4_CBS,
                          (a_uint8_t *) (&cbs), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        HSL_REG_FIELD_SET(rv, dev_id, EG_SHAPER7, port_id, EG_Q4_EBS,
                          (a_uint8_t *) (&ebs), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        HSL_REG_FIELD_SET(rv, dev_id, EG_SHAPER7, port_id, EG_Q4_UNIT,
                          (a_uint8_t *) (&unit), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }
    else
    {
        HSL_REG_FIELD_SET(rv, dev_id, EG_SHAPER2, port_id, EG_Q5_CIR,
                          (a_uint8_t *) (&cir), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        HSL_REG_FIELD_SET(rv, dev_id, EG_SHAPER5, port_id, EG_Q5_EIR,
                          (a_uint8_t *) (&eir), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        HSL_REG_FIELD_SET(rv, dev_id, EG_SHAPER7, port_id, EG_Q5_CBS,
                          (a_uint8_t *) (&cbs), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        HSL_REG_FIELD_SET(rv, dev_id, EG_SHAPER7, port_id, EG_Q5_EBS,
                          (a_uint8_t *) (&ebs), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        HSL_REG_FIELD_SET(rv, dev_id, EG_SHAPER7, port_id, EG_Q5_UNIT,
                          (a_uint8_t *) (&unit), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }

    return SW_OK;
}

static sw_error_t
_dess_rate_queue_shaper_get(a_uint32_t dev_id, fal_port_t port_id,
                            fal_queue_t queue_id, a_bool_t * enable,
                            fal_egress_shaper_t * shaper)
{
    sw_error_t rv;
    a_uint32_t data = 0, cir = 0, eir = 0, cbs = 0, ebs = 0;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    rv = _dess_rate_port_queue_check(port_id, queue_id);
    SW_RTN_ON_ERROR(rv);

    aos_mem_zero(shaper, sizeof (fal_egress_shaper_t));

    HSL_REG_FIELD_GET(rv, dev_id, EG_SHAPER7, port_id, EG_PT,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (data)
    {
        *enable = A_FALSE;
        return SW_OK;
    }

    if (0 == queue_id)
    {
        HSL_REG_FIELD_GET(rv, dev_id, EG_SHAPER0, port_id, EG_Q0_CIR,
                          (a_uint8_t *) (&cir), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        HSL_REG_FIELD_GET(rv, dev_id, EG_SHAPER3, port_id, EG_Q0_EIR,
                          (a_uint8_t *) (&eir), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        HSL_REG_FIELD_GET(rv, dev_id, EG_SHAPER6, port_id, EG_Q0_CBS,
                          (a_uint8_t *) (&cbs), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        HSL_REG_FIELD_GET(rv, dev_id, EG_SHAPER6, port_id, EG_Q0_EBS,
                          (a_uint8_t *) (&ebs), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        HSL_REG_FIELD_GET(rv, dev_id, EG_SHAPER7, port_id, EG_Q0_UNIT,
                          (a_uint8_t *) (&data), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }
    else if (1 == queue_id)
    {
        HSL_REG_FIELD_GET(rv, dev_id, EG_SHAPER0, port_id, EG_Q1_CIR,
                          (a_uint8_t *) (&cir), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        HSL_REG_FIELD_GET(rv, dev_id, EG_SHAPER3, port_id, EG_Q1_EIR,
                          (a_uint8_t *) (&eir), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        HSL_REG_FIELD_GET(rv, dev_id, EG_SHAPER6, port_id, EG_Q1_CBS,
                          (a_uint8_t *) (&cbs), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        HSL_REG_FIELD_GET(rv, dev_id, EG_SHAPER6, port_id, EG_Q1_EBS,
                          (a_uint8_t *) (&ebs), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        HSL_REG_FIELD_GET(rv, dev_id, EG_SHAPER7, port_id, EG_Q1_UNIT,
                          (a_uint8_t *) (&data), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }
    else if (2 == queue_id)
    {
        HSL_REG_FIELD_GET(rv, dev_id, EG_SHAPER1, port_id, EG_Q2_CIR,
                          (a_uint8_t *) (&cir), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        HSL_REG_FIELD_GET(rv, dev_id, EG_SHAPER4, port_id, EG_Q2_EIR,
                          (a_uint8_t *) (&eir), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        HSL_REG_FIELD_GET(rv, dev_id, EG_SHAPER6, port_id, EG_Q2_CBS,
                          (a_uint8_t *) (&cbs), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        HSL_REG_FIELD_GET(rv, dev_id, EG_SHAPER6, port_id, EG_Q2_EBS,
                          (a_uint8_t *) (&ebs), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        HSL_REG_FIELD_GET(rv, dev_id, EG_SHAPER7, port_id, EG_Q2_UNIT,
                          (a_uint8_t *) (&data), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }
    else if (3 == queue_id)
    {
        HSL_REG_FIELD_GET(rv, dev_id, EG_SHAPER1, port_id, EG_Q3_CIR,
                          (a_uint8_t *) (&cir), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        HSL_REG_FIELD_GET(rv, dev_id, EG_SHAPER4, port_id, EG_Q3_EIR,
                          (a_uint8_t *) (&eir), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        HSL_REG_FIELD_GET(rv, dev_id, EG_SHAPER6, port_id, EG_Q3_CBS,
                          (a_uint8_t *) (&cbs), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        HSL_REG_FIELD_GET(rv, dev_id, EG_SHAPER6, port_id, EG_Q3_EBS,
                          (a_uint8_t *) (&ebs), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        HSL_REG_FIELD_GET(rv, dev_id, EG_SHAPER7, port_id, EG_Q3_UNIT,
                          (a_uint8_t *) (&data), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }
    else if (4 == queue_id)
    {
        HSL_REG_FIELD_GET(rv, dev_id, EG_SHAPER2, port_id, EG_Q4_CIR,
                          (a_uint8_t *) (&cir), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        HSL_REG_FIELD_GET(rv, dev_id, EG_SHAPER5, port_id, EG_Q4_EIR,
                          (a_uint8_t *) (&eir), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        HSL_REG_FIELD_GET(rv, dev_id, EG_SHAPER7, port_id, EG_Q4_CBS,
                          (a_uint8_t *) (&cbs), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        HSL_REG_FIELD_GET(rv, dev_id, EG_SHAPER7, port_id, EG_Q4_EBS,
                          (a_uint8_t *) (&ebs), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        HSL_REG_FIELD_GET(rv, dev_id, EG_SHAPER7, port_id, EG_Q4_UNIT,
                          (a_uint8_t *) (&data), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }
    else
    {
        HSL_REG_FIELD_GET(rv, dev_id, EG_SHAPER2, port_id, EG_Q5_CIR,
                          (a_uint8_t *) (&cir), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        HSL_REG_FIELD_GET(rv, dev_id, EG_SHAPER5, port_id, EG_Q5_EIR,
                          (a_uint8_t *) (&eir), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        HSL_REG_FIELD_GET(rv, dev_id, EG_SHAPER7, port_id, EG_Q5_CBS,
                          (a_uint8_t *) (&cbs), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        HSL_REG_FIELD_GET(rv, dev_id, EG_SHAPER7, port_id, EG_Q5_EBS,
                          (a_uint8_t *) (&ebs), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        HSL_REG_FIELD_GET(rv, dev_id, EG_SHAPER7, port_id, EG_Q5_UNIT,
                          (a_uint8_t *) (&data), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }

    if ((0x7fff == cir) && (0x7fff == eir))
    {
        *enable = A_FALSE;
        return SW_OK;
    }

    *enable = A_TRUE;
    if (data)
    {
        shaper->meter_unit = FAL_FRAME_BASED;
        shaper->cir = cir / 2 * 125 + cir % 2 * 63;
        shaper->eir = eir / 2 * 125 + eir % 2 * 63;
        _dess_egress_bs_frame_hw_to_sw(cbs, &(shaper->cbs));
        _dess_egress_bs_frame_hw_to_sw(ebs, &(shaper->ebs));
    }
    else
    {
        shaper->meter_unit = FAL_BYTE_BASED;
        shaper->cir = cir << 5;
        shaper->eir = eir << 5;
        _dess_egress_bs_byte_hw_to_sw(cbs, &(shaper->cbs));
        _dess_egress_bs_byte_hw_to_sw(ebs, &(shaper->ebs));
    }

    return SW_OK;
}

static sw_error_t
_dess_rate_acl_policer_set(a_uint32_t dev_id, a_uint32_t policer_id,
                           fal_acl_policer_t * policer)
{
    sw_error_t rv;
    a_uint32_t ts, cir, eir, cbs = 0, ebs = 0, addr, data[2] = { 0 };

    HSL_DEV_ID_CHECK(dev_id);

    if (DESS_MAX_POLICER_ID < policer_id)
    {
        return SW_BAD_PARAM;
    }

    if (A_TRUE == policer->counter_mode)
    {
        addr = ACL_POLICER_CNT_SEL_ADDR;
        data[0] = 0x1;
        HSL_REG_FIELD_GEN_SET(rv, dev_id, addr, 1, policer_id,
                              (a_uint8_t *) (&data[0]), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        addr = ACL_POLICER_CNT_MODE_ADDR;
        if (FAL_FRAME_BASED == policer->meter_unit)
        {
            data[0] = 0x0;
        }
        else
        {
            data[0] = 0x1;
        }
        HSL_REG_FIELD_GEN_SET(rv, dev_id, addr, 1, policer_id,
                              (a_uint8_t *) (&data[0]), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        addr = ACL_POLICER_CNT_RST_ADDR;
        data[0] = 0x1;
        HSL_REG_FIELD_GEN_SET(rv, dev_id, addr, 1, policer_id,
                              (a_uint8_t *) (&data[0]), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        data[0] = 0x0;
        HSL_REG_FIELD_GEN_SET(rv, dev_id, addr, 1, policer_id,
                              (a_uint8_t *) (&data[0]), sizeof (a_uint32_t));
        return rv;
    }

    addr = ACL_POLICER_CNT_SEL_ADDR;
    data[0] = 0x0;
    HSL_REG_FIELD_GEN_SET(rv, dev_id, addr, 1, policer_id,
                          (a_uint8_t *) (&data[0]), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (FAL_BYTE_BASED == policer->meter_unit)
    {
        cir = policer->cir >> 5;
        policer->cir = cir << 5;

        eir = policer->eir >> 5;
        policer->eir = eir << 5;

        _dess_ingress_bs_byte_sw_to_hw(policer->cbs, &cbs);
        _dess_ingress_bs_byte_hw_to_sw(cbs, &(policer->cbs));

        _dess_ingress_bs_byte_sw_to_hw(policer->ebs, &ebs);
        _dess_ingress_bs_byte_hw_to_sw(ebs, &(policer->ebs));

        SW_SET_REG_BY_FIELD(ACL_POLICER1, ACL_UNIT, 0, data[1]);
    }
    else if (FAL_FRAME_BASED == policer->meter_unit)
    {
        cir = (policer->cir * 2) / 125;
        policer->cir = cir / 2 * 125 + cir % 2 * 63;

        eir = (policer->eir * 2) / 125;
        policer->eir = eir / 2 * 125 + eir % 2 * 63;

        _dess_ingress_bs_frame_sw_to_hw(policer->cbs, &cbs);
        _dess_ingress_bs_frame_hw_to_sw(cbs, &(policer->cbs));

        _dess_ingress_bs_frame_sw_to_hw(policer->ebs, &ebs);
        _dess_ingress_bs_frame_hw_to_sw(ebs, &(policer->ebs));

        SW_SET_REG_BY_FIELD(ACL_POLICER1, ACL_UNIT, 1, data[1]);
    }
    else
    {
        return SW_BAD_PARAM;
    }

    SW_SET_REG_BY_FIELD(ACL_POLICER0, ACL_CIR, cir, data[0]);
    SW_SET_REG_BY_FIELD(ACL_POLICER0, ACL_CBS, cbs, data[0]);
    SW_SET_REG_BY_FIELD(ACL_POLICER1, ACL_EIR, eir, data[1]);
    SW_SET_REG_BY_FIELD(ACL_POLICER1, ACL_EBS, ebs, data[1]);

    if (A_TRUE == policer->deficit_en)
    {
        SW_SET_REG_BY_FIELD(ACL_POLICER1, ACL_BORROW, 1, data[1]);
    }

    if (A_TRUE == policer->color_mode)
    {
        SW_SET_REG_BY_FIELD(ACL_POLICER1, ACL_CM, 1, data[1]);
    }

    if (A_TRUE == policer->couple_flag)
    {
        SW_SET_REG_BY_FIELD(ACL_POLICER1, ACL_CF, 1, data[1]);
    }

    _dess_rate_ts_parse(policer->meter_interval, &ts);
    SW_SET_REG_BY_FIELD(ACL_POLICER1, ACL_TS, ts, data[1]);

    HSL_REG_ENTRY_SET(rv, dev_id, ACL_POLICER0, policer_id,
                      (a_uint8_t *) (&data[0]), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    HSL_REG_ENTRY_SET(rv, dev_id, ACL_POLICER1, policer_id,
                      (a_uint8_t *) (&data[1]), sizeof (a_uint32_t));

    return rv;
}

static sw_error_t
_dess_rate_acl_policer_get(a_uint32_t dev_id, a_uint32_t policer_id,
                           fal_acl_policer_t * policer)
{
    sw_error_t rv;
    a_uint32_t unit, ts, cir, eir, cbs, ebs, addr, data[2] = {0};

    HSL_DEV_ID_CHECK(dev_id);

    if (DESS_MAX_POLICER_ID < policer_id)
    {
        return SW_BAD_PARAM;
    }

    aos_mem_zero(policer, sizeof (policer));

    addr = ACL_POLICER_CNT_SEL_ADDR;
    HSL_REG_FIELD_GEN_GET(rv, dev_id, addr, 1, policer_id,
                          (a_uint8_t *) (&data[0]), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (data[0])
    {
        policer->counter_mode = A_TRUE;

        addr = ACL_POLICER_CNT_MODE_ADDR;
        HSL_REG_FIELD_GEN_GET(rv, dev_id, addr, 1, policer_id,
                              (a_uint8_t *) (&data[0]), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        if (data[0])
        {
            policer->meter_unit = FAL_BYTE_BASED;
        }
        else
        {
            policer->meter_unit = FAL_FRAME_BASED;
        }

        HSL_REG_ENTRY_GET(rv, dev_id, ACL_COUNTER0, policer_id,
                          (a_uint8_t *) (&data[0]), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        HSL_REG_ENTRY_GET(rv, dev_id, ACL_COUNTER1, policer_id,
                          (a_uint8_t *) (&data[1]), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        policer->counter_low = data[0];
        policer->counter_high = data[1];

        return SW_OK;
    }

    HSL_REG_ENTRY_GET(rv, dev_id, ACL_POLICER0, policer_id,
                      (a_uint8_t *) (&data[0]), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    HSL_REG_ENTRY_GET(rv, dev_id, ACL_POLICER1, policer_id,
                      (a_uint8_t *) (&data[1]), sizeof (a_uint32_t));

    SW_GET_FIELD_BY_REG(ACL_POLICER0, ACL_CIR, cir, data[0]);
    SW_GET_FIELD_BY_REG(ACL_POLICER0, ACL_CBS, cbs, data[0]);
    SW_GET_FIELD_BY_REG(ACL_POLICER1, ACL_EIR, eir, data[1]);
    SW_GET_FIELD_BY_REG(ACL_POLICER1, ACL_EBS, ebs, data[1]);
    SW_GET_FIELD_BY_REG(ACL_POLICER1, ACL_UNIT, unit, data[1]);
    if (unit)
    {
        policer->meter_unit = FAL_FRAME_BASED;
        policer->cir = cir / 2 * 125 + cir % 2 * 63;
        policer->eir = eir / 2 * 125 + eir % 2 * 63;
        _dess_ingress_bs_frame_hw_to_sw(cbs, &(policer->cbs));
        _dess_ingress_bs_frame_hw_to_sw(ebs, &(policer->ebs));

    }
    else
    {
        policer->meter_unit = FAL_BYTE_BASED;
        policer->cir = cir << 5;
        policer->eir = eir << 5;
        _dess_ingress_bs_byte_hw_to_sw(cbs, &(policer->cbs));
        _dess_ingress_bs_byte_hw_to_sw(ebs, &(policer->ebs));
    }

    SW_GET_FIELD_BY_REG(ACL_POLICER1, ACL_BORROW, policer->deficit_en,
                        data[1]);
    SW_GET_FIELD_BY_REG(ACL_POLICER1, ACL_CF, policer->couple_flag, data[1]);
    SW_GET_FIELD_BY_REG(ACL_POLICER1, ACL_CM, policer->color_mode, data[1]);

    SW_GET_FIELD_BY_REG(ACL_POLICER1, ACL_TS, ts, data[1]);
    _dess_rate_ts_reparse(ts, &(policer->meter_interval));

    return SW_OK;
}

sw_error_t
_dess_rate_port_add_rate_byte_set(a_uint32_t dev_id, fal_port_t port_id,
                                  a_uint32_t  number)
{
    a_uint32_t val = number;
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    if (val>255)
        return SW_BAD_PARAM;

    HSL_REG_FIELD_SET(rv, dev_id, INGRESS_POLICER0, port_id, ADD_RATE_BYTE,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));

    return rv;
}

sw_error_t
_dess_rate_port_add_rate_byte_get(a_uint32_t dev_id, fal_port_t port_id,
                                  a_uint32_t  *number)
{
    a_uint32_t val = 0;
    sw_error_t rv = SW_OK;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }


    HSL_REG_FIELD_GET(rv, dev_id, INGRESS_POLICER0, port_id, ADD_RATE_BYTE,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    *number = val;

    return rv;
}

sw_error_t
_dess_rate_port_gol_flow_en_set(a_uint32_t dev_id, fal_port_t port_id,
                                a_bool_t  enable)
{
    a_uint32_t val = 0;
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_ENTRY_GET(rv, dev_id, QM_CTRL_REG, 0,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));

    SW_RTN_ON_ERROR(rv);


    if (A_TRUE == enable)
    {
        val |= (0x1<<(16+port_id));
    }
    else if (A_FALSE == enable)
    {
        val &= ~(0x1<<(16+port_id));
    }
    else
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_ENTRY_SET(rv, dev_id, QM_CTRL_REG, 0,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));

    return rv;
}

sw_error_t
_dess_rate_port_gol_flow_en_get(a_uint32_t dev_id, fal_port_t port_id,
                                a_bool_t*  enable)
{
    a_uint32_t val = 0;
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_ENTRY_GET(rv, dev_id, QM_CTRL_REG, 0,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));

    SW_RTN_ON_ERROR(rv);

    if (val&(0x1<<(16+port_id)))
    {
        *enable = A_TRUE;
    }
    else
    {
        *enable = A_FALSE;
    }

    return rv;
}



/**
 * @brief Set port ingress policer parameters on one particular port.
 * @details   Comments:
    The granularity of speed is 32kbps or 62.5fps.
    Because of hardware granularity function will return actual speed in hardware.
    When disable port ingress policer input parameter speed is meaningless.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] policer port ingress policer parameter
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_rate_port_policer_set(a_uint32_t dev_id, fal_port_t port_id,
                           fal_port_policer_t * policer)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_rate_port_policer_set(dev_id, port_id, policer);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get port ingress policer parameters on one particular port.
 * @details   Comments:
    The granularity of speed is 32kbps or 62.5fps.
    Because of hardware granularity function will return actual speed in hardware.
    When disable port ingress policer input parameter speed is meaningless.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] policer port ingress policer parameter
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_rate_port_policer_get(a_uint32_t dev_id, fal_port_t port_id,
                           fal_port_policer_t * policer)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_rate_port_policer_get(dev_id, port_id, policer);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set port egress shaper parameters on one particular port.
 * @details   Comments:
    The granularity of speed is 32kbps or 62.5fps.
    Because of hardware granularity function will return actual speed in hardware.
    When disable port egress shaper parameters is meaningless.
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @param[in] shaper port egress shaper parameter
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_rate_port_shaper_set(a_uint32_t dev_id, fal_port_t port_id,
                          a_bool_t enable, fal_egress_shaper_t * shaper)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_rate_port_shaper_set(dev_id, port_id, enable, shaper);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get port egress shaper parameters on one particular port.
 * @details   Comments:
    The granularity of speed is 32kbps or 62.5fps.
    Because of hardware granularity function will return actual speed in hardware.
    When disable port egress shaper parameters is meaningless.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @param[out] shaper port egress shaper parameter
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_rate_port_shaper_get(a_uint32_t dev_id, fal_port_t port_id,
                          a_bool_t * enable, fal_egress_shaper_t * shaper)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_rate_port_shaper_get(dev_id, port_id, enable, shaper);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set queue egress shaper parameters on one particular port.
 * @details   Comments:
    The granularity of speed is 32kbps or 62.5fps.
    Because of hardware granularity function will return actual speed in hardware.
    When disable queue egress shaper parameters is meaningless.
 * @param[in] port_id port id
 * @param[in] queue_id queue id
 * @param[in] enable A_TRUE or A_FALSE
 * @param[in] shaper port egress shaper parameter
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_rate_queue_shaper_set(a_uint32_t dev_id, fal_port_t port_id,
                           fal_queue_t queue_id, a_bool_t enable,
                           fal_egress_shaper_t * shaper)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_rate_queue_shaper_set(dev_id, port_id, queue_id, enable, shaper);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get queue egress shaper parameters on one particular port.
 * @details   Comments:
    The granularity of speed is 32kbps or 62.5fps.
    Because of hardware granularity function will return actual speed in hardware.
    When disable queue egress shaper parameters is meaningless.
 * @param[in] port_id port id
 * @param[in] queue_id queue id
 * @param[out] enable A_TRUE or A_FALSE
 * @param[out] shaper port egress shaper parameter
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_rate_queue_shaper_get(a_uint32_t dev_id, fal_port_t port_id,
                           fal_queue_t queue_id, a_bool_t * enable,
                           fal_egress_shaper_t * shaper)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_rate_queue_shaper_get(dev_id, port_id, queue_id, enable, shaper);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set ACL ingress policer parameters.
 * @details   Comments:
    The granularity of speed is 32kbps or 62.5fps.
    Because of hardware granularity function will return actual speed in hardware.
 * @param[in] dev_id device id
 * @param[in] policer_id ACL policer id
 * @param[in] policer ACL ingress policer parameters
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_rate_acl_policer_set(a_uint32_t dev_id, a_uint32_t policer_id,
                          fal_acl_policer_t * policer)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_rate_acl_policer_set(dev_id, policer_id, policer);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get ACL ingress policer parameters.
 * @details   Comments:
    The granularity of speed is 32kbps or 62.5fps.
    Because of hardware granularity function will return actual speed in hardware.
 * @param[in] dev_id device id
 * @param[in] policer_id ACL policer id
 * @param[in] policer ACL ingress policer parameters
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_rate_acl_policer_get(a_uint32_t dev_id, a_uint32_t policer_id,
                          fal_acl_policer_t * policer)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_rate_acl_policer_get(dev_id, policer_id, policer);
    HSL_API_UNLOCK;
    return rv;
}

HSL_LOCAL sw_error_t
dess_rate_port_add_rate_byte_set(a_uint32_t dev_id, fal_port_t port_id,
                                 a_uint32_t  number)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_rate_port_add_rate_byte_set(dev_id, port_id, number);
    HSL_API_UNLOCK;
    return rv;
}

HSL_LOCAL sw_error_t
dess_rate_port_add_rate_byte_get(a_uint32_t dev_id, fal_port_t port_id,
                                 a_uint32_t  *number)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_rate_port_add_rate_byte_get(dev_id, port_id, number);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set status of port global flow control when global threshold is reached.
  * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_rate_port_gol_flow_en_set(a_uint32_t dev_id, fal_port_t port_id,
                               a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_rate_port_gol_flow_en_set(dev_id, port_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief get status of port global flow control when global threshold is reached.
  * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_rate_port_gol_flow_en_get(a_uint32_t dev_id, fal_port_t port_id,
                               a_bool_t* enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_rate_port_gol_flow_en_get(dev_id, port_id, enable);
    HSL_API_UNLOCK;
    return rv;
}



sw_error_t
dess_rate_init(a_uint32_t dev_id)
{
    HSL_DEV_ID_CHECK(dev_id);

#ifndef HSL_STANDALONG
    {
        hsl_api_t *p_api;

        SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

        p_api->rate_port_policer_set = dess_rate_port_policer_set;
        p_api->rate_port_policer_get = dess_rate_port_policer_get;
        p_api->rate_port_shaper_set = dess_rate_port_shaper_set;
        p_api->rate_port_shaper_get = dess_rate_port_shaper_get;
        p_api->rate_queue_shaper_set = dess_rate_queue_shaper_set;
        p_api->rate_queue_shaper_get = dess_rate_queue_shaper_get;
        p_api->rate_acl_policer_set = dess_rate_acl_policer_set;
        p_api->rate_acl_policer_get = dess_rate_acl_policer_get;
        p_api->rate_port_gol_flow_en_set = dess_rate_port_gol_flow_en_set;
        p_api->rate_port_gol_flow_en_get = dess_rate_port_gol_flow_en_get;
        p_api->rate_port_add_rate_byte_set=dess_rate_port_add_rate_byte_set;
        p_api->rate_port_add_rate_byte_get=dess_rate_port_add_rate_byte_get;
    }
#endif

    return SW_OK;
}

/**
 * @}
 */

