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
 * @defgroup isisc_vlan ISISC_VLAN
 * @{
 */
#include "sw.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_port_prop.h"
#include "isisc_vlan.h"
#include "isisc_reg.h"

#define MAX_VLAN_ID         4095

#define VLAN_FLUSH          1
#define VLAN_LOAD_ENTRY     2
#define VLAN_PURGE_ENTRY    3
#define VLAN_REMOVE_PORT    4
#define VLAN_NEXT_ENTRY     5
#define VLAN_FIND_ENTRY     6

static void
_isisc_vlan_hw_to_sw(a_uint32_t reg[], fal_vlan_t * vlan_entry)
{
    a_uint32_t i, data, tmp;

    aos_mem_zero(vlan_entry, sizeof (fal_vlan_t));

    SW_GET_FIELD_BY_REG(VLAN_TABLE_FUNC1, VLAN_ID, data, reg[1]);
    vlan_entry->vid = data & 0xfff;

    SW_GET_FIELD_BY_REG(VLAN_TABLE_FUNC0, IVL_EN, data, reg[0]);
    if (1 == data)
    {
        vlan_entry->fid = vlan_entry->vid;
    }
    else
    {
        vlan_entry->fid = FAL_SVL_FID;
    }

    SW_GET_FIELD_BY_REG(VLAN_TABLE_FUNC0, LEARN_DIS, data, reg[0]);
    if (1 == data)
    {
        vlan_entry->learn_dis = A_TRUE;
    }
    else
    {
        vlan_entry->learn_dis = A_FALSE;
    }

    SW_GET_FIELD_BY_REG(VLAN_TABLE_FUNC0, VT_PRI_EN, data, reg[0]);
    if (1 == data)
    {
        vlan_entry->vid_pri_en = A_TRUE;

        SW_GET_FIELD_BY_REG(VLAN_TABLE_FUNC0, VT_PRI, data, reg[0]);
        vlan_entry->vid_pri = data & 0xff;
    }
    else
    {
        vlan_entry->vid_pri_en = A_FALSE;
    }

    SW_GET_FIELD_BY_REG(VLAN_TABLE_FUNC0, VID_MEM, data, reg[0]);
    for (i = 0; i < 7; i++)
    {
        tmp = (data >> (i << 1)) & 0x3UL;
        if (0 == tmp)
        {
            vlan_entry->mem_ports |= (0x1UL << i);
            vlan_entry->unmodify_ports |= (0x1UL << i);
        }
        else if (1 == tmp)
        {
            vlan_entry->mem_ports |= (0x1UL << i);
            vlan_entry->untagged_ports |= (0x1UL << i);
        }
        else if (2 == tmp)
        {
            vlan_entry->mem_ports |= (0x1UL << i);
            vlan_entry->tagged_ports |= (0x1UL << i);
        }
    }

    return;
}

static sw_error_t
_isisc_vlan_sw_to_hw(a_uint32_t dev_id, const fal_vlan_t * vlan_entry,
                    a_uint32_t reg[])
{
    a_uint32_t i, tag, untag, unmodify, member = 0;

    if (vlan_entry->vid > MAX_VLAN_ID)
    {
        return SW_OUT_OF_RANGE;
    }

    if (A_FALSE ==
            hsl_mports_prop_check(dev_id, vlan_entry->mem_ports, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    SW_SET_REG_BY_FIELD(VLAN_TABLE_FUNC0, VT_VALID, 1, reg[0]);

    if (FAL_SVL_FID == vlan_entry->fid)
    {
        SW_SET_REG_BY_FIELD(VLAN_TABLE_FUNC0, IVL_EN, 0, reg[0]);
    }
    else if (vlan_entry->vid == vlan_entry->fid)
    {
        SW_SET_REG_BY_FIELD(VLAN_TABLE_FUNC0, IVL_EN, 1, reg[0]);
    }
    else
    {
        return SW_BAD_VALUE;
    }

    if (A_TRUE == vlan_entry->learn_dis)
    {
        SW_SET_REG_BY_FIELD(VLAN_TABLE_FUNC0, LEARN_DIS, 1, reg[0]);
    }
    else
    {
        SW_SET_REG_BY_FIELD(VLAN_TABLE_FUNC0, LEARN_DIS, 0, reg[0]);
    }

    for (i = 0; i < 7; i++)
    {
        if ((vlan_entry->mem_ports >> i) & 0x1UL)
        {
            tag = (vlan_entry->tagged_ports >> i) & 0x1UL;
            untag = (vlan_entry->untagged_ports >> i) & 0x1UL;
            unmodify = (vlan_entry->unmodify_ports >> i) & 0x1UL;

            if ((0 == (tag + untag + unmodify))
                    || (1 < (tag + untag + unmodify)))
            {
                return SW_BAD_VALUE;
            }

            if (tag)
            {
                member |= (2 << (i << 1));
            }
            else if (untag)
            {
                member |= (1 << (i << 1));
            }
        }
        else
        {
            member |= (3 << (i << 1));
        }
    }
    SW_SET_REG_BY_FIELD(VLAN_TABLE_FUNC0, VID_MEM, member, reg[0]);

    if (A_TRUE == vlan_entry->vid_pri_en)
    {
        SW_SET_REG_BY_FIELD(VLAN_TABLE_FUNC0, VT_PRI_EN, 1, reg[0]);
        SW_SET_REG_BY_FIELD(VLAN_TABLE_FUNC0, VT_PRI, vlan_entry->vid_pri,
                            reg[0]);
    }
    else
    {
        SW_SET_REG_BY_FIELD(VLAN_TABLE_FUNC0, VT_PRI_EN, 0, reg[0]);
    }

    SW_SET_REG_BY_FIELD(VLAN_TABLE_FUNC1, VLAN_ID, vlan_entry->vid, reg[1]);

    return SW_OK;
}

static sw_error_t
_isisc_vlan_down_to_hw(a_uint32_t dev_id, a_uint32_t reg[])
{
    sw_error_t rv;

    HSL_REG_ENTRY_SET(rv, dev_id, VLAN_TABLE_FUNC0, 0,
                      (a_uint8_t *) (&reg[0]), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    HSL_REG_ENTRY_SET(rv, dev_id, VLAN_TABLE_FUNC1, 0,
                      (a_uint8_t *) (&reg[1]), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_isisc_vlan_up_to_sw(a_uint32_t dev_id, a_uint32_t reg[])
{
    sw_error_t rv;

    HSL_REG_ENTRY_GET(rv, dev_id, VLAN_TABLE_FUNC0, 0,
                      (a_uint8_t *) (&reg[0]), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    HSL_REG_ENTRY_GET(rv, dev_id, VLAN_TABLE_FUNC1, 0,
                      (a_uint8_t *) (&reg[1]), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_isisc_vlan_commit(a_uint32_t dev_id, a_uint32_t op)
{
    a_uint32_t vt_busy = 1, i = 0x1000, vt_full, val;
    sw_error_t rv;

    while (vt_busy && --i)
    {
        HSL_REG_FIELD_GET(rv, dev_id, VLAN_TABLE_FUNC1, 0, VT_BUSY,
                          (a_uint8_t *) (&vt_busy), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
        aos_udelay(5);
    }

    if (i == 0)
        return SW_BUSY;

    HSL_REG_ENTRY_GET(rv, dev_id, VLAN_TABLE_FUNC1, 0,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_SET_REG_BY_FIELD(VLAN_TABLE_FUNC1, VT_FUNC, op, val);
    SW_SET_REG_BY_FIELD(VLAN_TABLE_FUNC1, VT_BUSY, 1, val);

    HSL_REG_ENTRY_SET(rv, dev_id, VLAN_TABLE_FUNC1, 0,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));

    SW_RTN_ON_ERROR(rv);

    vt_busy = 1;
    i = 0x1000;
    while (vt_busy && --i)
    {
        HSL_REG_FIELD_GET(rv, dev_id, VLAN_TABLE_FUNC1, 0, VT_BUSY,
                          (a_uint8_t *) (&vt_busy), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
        aos_udelay(5);
    }

    if (i == 0)
        return SW_FAIL;

    HSL_REG_FIELD_GET(rv, dev_id, VLAN_TABLE_FUNC1, 0, VT_FULL_VIO,
                      (a_uint8_t *) (&vt_full), sizeof (a_uint32_t));

    SW_RTN_ON_ERROR(rv);

    if (vt_full)
    {
        val = 0x10;
        HSL_REG_ENTRY_SET(rv, dev_id, VLAN_TABLE_FUNC1, 0,
                          (a_uint8_t *) (&val), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
        if (VLAN_LOAD_ENTRY == op)
        {
            return SW_FULL;
        }
        else if (VLAN_PURGE_ENTRY == op)
        {
            return SW_NOT_FOUND;
        }
    }

    HSL_REG_FIELD_GET(rv, dev_id, VLAN_TABLE_FUNC0, 0, VT_VALID,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));

    SW_RTN_ON_ERROR(rv);

    if (!val)
    {
        if (VLAN_FIND_ENTRY == op)
            return SW_NOT_FOUND;

        if (VLAN_NEXT_ENTRY == op)
            return SW_NO_MORE;
    }

    return SW_OK;
}

static sw_error_t
_isisc_vlan_hwentry_get(a_uint32_t dev_id, a_uint32_t vlan_id, a_uint32_t reg[])
{
    sw_error_t rv;

    if (vlan_id > MAX_VLAN_ID)
    {
        return SW_OUT_OF_RANGE;
    }

    SW_SET_REG_BY_FIELD(VLAN_TABLE_FUNC1, VLAN_ID, vlan_id, reg[1]);
    HSL_REG_ENTRY_SET(rv, dev_id, VLAN_TABLE_FUNC1, 0,
                      (a_uint8_t *) (&reg[1]), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    rv = _isisc_vlan_commit(dev_id, VLAN_FIND_ENTRY);
    SW_RTN_ON_ERROR(rv);

    rv = _isisc_vlan_up_to_sw(dev_id, reg);
    return rv;
}

static sw_error_t
_isisc_vlan_entry_append(a_uint32_t dev_id, const fal_vlan_t * vlan_entry)
{
    sw_error_t rv;
    a_uint32_t reg[2] = { 0 };

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isisc_vlan_sw_to_hw(dev_id, vlan_entry, reg);
    SW_RTN_ON_ERROR(rv);

    rv = _isisc_vlan_down_to_hw(dev_id, reg);
    SW_RTN_ON_ERROR(rv);

    rv = _isisc_vlan_commit(dev_id, VLAN_LOAD_ENTRY);
    return rv;
}

static sw_error_t
_isisc_vlan_create(a_uint32_t dev_id, a_uint32_t vlan_id)
{
    sw_error_t rv;
    a_uint32_t reg[2] = { 0 };

    HSL_DEV_ID_CHECK(dev_id);

    if (vlan_id > MAX_VLAN_ID)
    {
        return SW_OUT_OF_RANGE;
    }

    SW_SET_REG_BY_FIELD(VLAN_TABLE_FUNC0, VT_VALID, 1, reg[0]);
    SW_SET_REG_BY_FIELD(VLAN_TABLE_FUNC0, IVL_EN, 1, reg[0]);
    SW_SET_REG_BY_FIELD(VLAN_TABLE_FUNC0, LEARN_DIS, 0, reg[0]);
    SW_SET_REG_BY_FIELD(VLAN_TABLE_FUNC0, VID_MEM, 0x3fff, reg[0]);
    SW_SET_REG_BY_FIELD(VLAN_TABLE_FUNC1, VLAN_ID, vlan_id, reg[1]);

    rv = _isisc_vlan_down_to_hw(dev_id, reg);
    SW_RTN_ON_ERROR(rv);

    rv = _isisc_vlan_commit(dev_id, VLAN_LOAD_ENTRY);
    return rv;
}

static sw_error_t
_isisc_vlan_next(a_uint32_t dev_id, a_uint32_t vlan_id, fal_vlan_t * p_vlan)
{
    sw_error_t rv;
    a_uint32_t reg[2] = { 0 };

    HSL_DEV_ID_CHECK(dev_id);

    if (FAL_NEXT_ENTRY_FIRST_ID == vlan_id)
    {
        rv = _isisc_vlan_hwentry_get(dev_id, 0, reg);

        if (SW_OK == rv)
        {
            _isisc_vlan_hw_to_sw(reg, p_vlan);
            return SW_OK;
        }
        else
        {
            vlan_id = 0;
        }
    }

    if (vlan_id > MAX_VLAN_ID)
        return SW_OUT_OF_RANGE;

    SW_SET_REG_BY_FIELD(VLAN_TABLE_FUNC1, VLAN_ID, vlan_id, reg[1]);
    HSL_REG_ENTRY_SET(rv, dev_id, VLAN_TABLE_FUNC1, 0,
                      (a_uint8_t *) (&reg[1]), sizeof (a_uint32_t));

    SW_RTN_ON_ERROR(rv);

    rv = _isisc_vlan_commit(dev_id, VLAN_NEXT_ENTRY);
    SW_RTN_ON_ERROR(rv);

    rv = _isisc_vlan_up_to_sw(dev_id, reg);
    SW_RTN_ON_ERROR(rv);

    _isisc_vlan_hw_to_sw(reg, p_vlan);

    if (0 == p_vlan->vid)
    {
        return SW_NO_MORE;
    }
    else
    {
        return SW_OK;
    }
}

static sw_error_t
_isisc_vlan_find(a_uint32_t dev_id, a_uint32_t vlan_id, fal_vlan_t * p_vlan)
{
    sw_error_t rv;
    a_uint32_t reg[2] = { 0 };

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isisc_vlan_hwentry_get(dev_id, vlan_id, reg);
    SW_RTN_ON_ERROR(rv);

    _isisc_vlan_hw_to_sw(reg, p_vlan);
    return SW_OK;
}

static sw_error_t
_isisc_vlan_delete(a_uint32_t dev_id, a_uint32_t vlan_id)
{
    sw_error_t rv;
    a_uint32_t reg;

    HSL_DEV_ID_CHECK(dev_id);

    if (vlan_id > MAX_VLAN_ID)
    {
        return SW_OUT_OF_RANGE;
    }

    reg = (a_int32_t) vlan_id;
    HSL_REG_FIELD_SET(rv, dev_id, VLAN_TABLE_FUNC1, 0, VLAN_ID,
                      (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    rv = _isisc_vlan_commit(dev_id, VLAN_PURGE_ENTRY);
    return rv;
}

static sw_error_t
_isisc_vlan_flush(a_uint32_t dev_id)
{
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isisc_vlan_commit(dev_id, VLAN_FLUSH);
    return rv;
}

static sw_error_t
_isisc_vlan_fid_set(a_uint32_t dev_id, a_uint32_t vlan_id, a_uint32_t fid)
{
    sw_error_t rv;
    a_uint32_t reg[2] = { 0 };

    HSL_DEV_ID_CHECK(dev_id);

    if ((MAX_VLAN_ID < fid) && (FAL_SVL_FID != fid))
    {
        return SW_BAD_PARAM;
    }

    if ((MAX_VLAN_ID >= fid) && (vlan_id != fid))
    {
        return SW_BAD_PARAM;
    }

    rv = _isisc_vlan_hwentry_get(dev_id, vlan_id, reg);
    SW_RTN_ON_ERROR(rv);

    if (FAL_SVL_FID == fid)
    {
        SW_SET_REG_BY_FIELD(VLAN_TABLE_FUNC0, IVL_EN, 0, reg[0]);
    }
    else
    {
        SW_SET_REG_BY_FIELD(VLAN_TABLE_FUNC0, IVL_EN, 1, reg[0]);
    }

    rv = _isisc_vlan_down_to_hw(dev_id, reg);
    SW_RTN_ON_ERROR(rv);

    rv = _isisc_vlan_commit(dev_id, VLAN_LOAD_ENTRY);
    if (SW_FULL == rv)
    {
        rv = SW_OK;
    }
    return rv;
}

static sw_error_t
_isisc_vlan_fid_get(a_uint32_t dev_id, a_uint32_t vlan_id, a_uint32_t * fid)
{
    sw_error_t rv;
    a_uint32_t data, reg[2] = { 0 };

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isisc_vlan_hwentry_get(dev_id, vlan_id, reg);
    SW_RTN_ON_ERROR(rv);

    SW_GET_FIELD_BY_REG(VLAN_TABLE_FUNC0, IVL_EN, data, reg[0]);
    if (data)
    {
        *fid = vlan_id;
    }
    else
    {
        *fid = FAL_SVL_FID;
    }
    return SW_OK;
}

static sw_error_t
_isisc_vlan_member_update(a_uint32_t dev_id, a_uint32_t vlan_id,
                         fal_port_t port_id, a_uint32_t port_info)
{
    sw_error_t rv;
    a_uint32_t data, reg[2] = { 0 };

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    rv = _isisc_vlan_hwentry_get(dev_id, vlan_id, reg);
    SW_RTN_ON_ERROR(rv);

    SW_GET_FIELD_BY_REG(VLAN_TABLE_FUNC0, VID_MEM, data, reg[0]);
    data &= (~(0x3 << (port_id << 1)));
    data |= ((port_info & 0x3) << (port_id << 1));
    SW_SET_REG_BY_FIELD(VLAN_TABLE_FUNC0, VID_MEM, data, reg[0]);

    rv = _isisc_vlan_down_to_hw(dev_id, reg);
    SW_RTN_ON_ERROR(rv);

    rv = _isisc_vlan_commit(dev_id, VLAN_LOAD_ENTRY);
    if (SW_FULL == rv)
    {
        rv = SW_OK;
    }
    return rv;
}

static sw_error_t
_isisc_vlan_member_add(a_uint32_t dev_id, a_uint32_t vlan_id,
                      fal_port_t port_id, fal_pt_1q_egmode_t port_info)
{
    sw_error_t rv;
    a_uint32_t info = 0;

    if (FAL_EG_UNMODIFIED == port_info)
    {
        info = 0;
    }
    else if (FAL_EG_TAGGED == port_info)
    {
        info = 0x2;
    }
    else if (FAL_EG_UNTAGGED == port_info)
    {
        info = 0x1;
    }
    else
    {
        return SW_BAD_PARAM;
    }

    rv = _isisc_vlan_member_update(dev_id, vlan_id, port_id, info);
    return rv;
}

static sw_error_t
_isisc_vlan_member_del(a_uint32_t dev_id, a_uint32_t vlan_id, fal_port_t port_id)
{
    sw_error_t rv;
    a_uint32_t info = 0x3;

    rv = _isisc_vlan_member_update(dev_id, vlan_id, port_id, info);
    return rv;
}

static sw_error_t
_isisc_vlan_learning_state_set(a_uint32_t dev_id, a_uint32_t vlan_id,
                              a_bool_t enable)
{
    sw_error_t rv;
    a_uint32_t reg[2] = { 0 };

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isisc_vlan_hwentry_get(dev_id, vlan_id, reg);
    SW_RTN_ON_ERROR(rv);

    if (A_TRUE == enable)
    {
        SW_SET_REG_BY_FIELD(VLAN_TABLE_FUNC0, LEARN_DIS, 0, reg[0]);
    }
    else if (A_FALSE == enable)
    {
        SW_SET_REG_BY_FIELD(VLAN_TABLE_FUNC0, LEARN_DIS, 1, reg[0]);
    }
    else
    {
        return SW_BAD_PARAM;
    }

    rv = _isisc_vlan_down_to_hw(dev_id, reg);
    SW_RTN_ON_ERROR(rv);

    rv = _isisc_vlan_commit(dev_id, VLAN_LOAD_ENTRY);
    if (SW_FULL == rv)
    {
        rv = SW_OK;
    }
    return rv;
}

static sw_error_t
_isisc_vlan_learning_state_get(a_uint32_t dev_id, a_uint32_t vlan_id,
                              a_bool_t * enable)
{
    sw_error_t rv;
    a_uint32_t data, reg[2] = { 0 };

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isisc_vlan_hwentry_get(dev_id, vlan_id, reg);
    SW_RTN_ON_ERROR(rv);

    SW_GET_FIELD_BY_REG(VLAN_TABLE_FUNC0, LEARN_DIS, data, reg[0]);
    if (data)
    {
        *enable = A_FALSE;
    }
    else
    {
        *enable = A_TRUE;
    }
    return SW_OK;
}

/**
 * @brief Append a vlan entry on paticular device.
 * @param[in] dev_id device id
 * @param[in] vlan_entry vlan entry
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_vlan_entry_append(a_uint32_t dev_id, const fal_vlan_t * vlan_entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_vlan_entry_append(dev_id, vlan_entry);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Creat a vlan entry through vlan id on a paticular device.
 *   @details   Comments:
 *    After this operation the member ports of the created vlan entry are null.
 * @param[in] dev_id device id
 * @param[in] vlan_id vlan id
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_vlan_create(a_uint32_t dev_id, a_uint32_t vlan_id)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_vlan_create(dev_id, vlan_id);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Next a vlan entry through vlan id on a paticular device.
 *   @details   Comments:
 *    If the value of vid is zero this operation will get the first entry.
 * @param[in] dev_id device id
 * @param[in] vlan_id vlan id
 * @param[out] p_vlan vlan entry
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_vlan_next(a_uint32_t dev_id, a_uint32_t vlan_id, fal_vlan_t * p_vlan)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_vlan_next(dev_id, vlan_id, p_vlan);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Find a vlan entry through vlan id on paticular device.
 * @param[in] dev_id device id
 * @param[in] vlan_id vlan id
 * @param[out] p_vlan vlan entry
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_vlan_find(a_uint32_t dev_id, a_uint32_t vlan_id, fal_vlan_t * p_vlan)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_vlan_find(dev_id, vlan_id, p_vlan);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Delete a vlan entry through vlan id on a paticular device.
 * @param[in] dev_id device id
 * @param[in] vlan_id vlan id
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_vlan_delete(a_uint32_t dev_id, a_uint32_t vlan_id)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_vlan_delete(dev_id, vlan_id);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Flush all vlan entries on a paticular device.
 * @param[in] dev_id device id
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_vlan_flush(a_uint32_t dev_id)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_vlan_flush(dev_id);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set FID of a paticular vlan entry on a paticular device.
 * @param[in] dev_id device id
 * @param[in] vlan_id vlan id
 * @param[in] fid FDB id
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_vlan_fid_set(a_uint32_t dev_id, a_uint32_t vlan_id, a_uint32_t fid)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_vlan_fid_set(dev_id, vlan_id, fid);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get FID of a paticular vlan entry on a paticular device.
 * @param[in] dev_id device id
 * @param[in] vlan_id vlan id
 * @param[out] fid FDB id
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_vlan_fid_get(a_uint32_t dev_id, a_uint32_t vlan_id, a_uint32_t * fid)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_vlan_fid_get(dev_id, vlan_id, fid);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Add a port member to a paticular vlan entry on a paticular device.
 * @param[in] dev_id device id
 * @param[in] vlan_id vlan id
 * @param[in] port_id port id
 * @param[in] port_info port tag information
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_vlan_member_add(a_uint32_t dev_id, a_uint32_t vlan_id,
                     fal_port_t port_id, fal_pt_1q_egmode_t port_info)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_vlan_member_add(dev_id, vlan_id, port_id, port_info);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Del a port member from a paticular vlan entry on a paticular device.
 * @param[in] dev_id device id
 * @param[in] vlan_id vlan id
 * @param[in] port_id port id
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_vlan_member_del(a_uint32_t dev_id, a_uint32_t vlan_id, fal_port_t port_id)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_vlan_member_del(dev_id, vlan_id, port_id);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set FDB learning status of a paticular vlan entry on a paticular device.
 * @param[in] dev_id device id
 * @param[in] vlan_id vlan id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_vlan_learning_state_set(a_uint32_t dev_id, a_uint32_t vlan_id,
                             a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_vlan_learning_state_set(dev_id, vlan_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get FDB learning status of a paticular vlan entry on a paticular device.
 * @param[in] dev_id device id
 * @param[in] vlan_id vlan id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_vlan_learning_state_get(a_uint32_t dev_id, a_uint32_t vlan_id,
                             a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_vlan_learning_state_get(dev_id, vlan_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

sw_error_t
isisc_vlan_init(a_uint32_t dev_id)
{
    hsl_api_t *p_api;
    HSL_DEV_ID_CHECK(dev_id);

#ifndef HSL_STANDALONG

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    p_api->vlan_entry_append = isisc_vlan_entry_append;
    p_api->vlan_creat = isisc_vlan_create;
    p_api->vlan_delete = isisc_vlan_delete;
    p_api->vlan_next = isisc_vlan_next;
    p_api->vlan_find = isisc_vlan_find;
    p_api->vlan_flush = isisc_vlan_flush;
    p_api->vlan_fid_set = isisc_vlan_fid_set;
    p_api->vlan_fid_get = isisc_vlan_fid_get;
    p_api->vlan_member_add = isisc_vlan_member_add;
    p_api->vlan_member_del = isisc_vlan_member_del;
    p_api->vlan_learning_state_set = isisc_vlan_learning_state_set;
    p_api->vlan_learning_state_get = isisc_vlan_learning_state_get;


#endif

    return SW_OK;
}

/**
 * @}
 */

