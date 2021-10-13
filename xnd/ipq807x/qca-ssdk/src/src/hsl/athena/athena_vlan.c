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
 * @defgroup athena_vlan ATHENA_VLAN
 * @{
 */
#include "sw.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_port_prop.h"
#include "athena_vlan.h"
#include "athena_reg.h"

#define MAX_VLAN_ENTRY      16
#define MAX_VLAN_ID         4094

#define VLAN_FLUSH          1
#define VLAN_LOAD_ENTRY     2
#define VLAN_PURGE_ENTRY    3
#define VLAN_REMOVE_PORT    4

typedef struct
{
    fal_vlan_t vlan_entry;
    a_bool_t active;
} v_array_t;

static v_array_t *p_vlan_table[SW_MAX_NR_DEV] = { 0 };

static sw_error_t
athena_vlan_commit(a_uint32_t dev_id, a_uint32_t op)
{
    a_uint32_t vt_busy = 1, i = 0x1000, vt_full, val;
    sw_error_t rv;

    while (vt_busy && --i)
    {
        HSL_REG_FIELD_GET(rv, dev_id, VLAN_TABLE_FUNC0, 0, VT_BUSY,
                          (a_uint8_t *) (&vt_busy), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
        aos_udelay(5);
    }

    if (i == 0)
        return SW_BUSY;

    HSL_REG_ENTRY_GET(rv, dev_id, VLAN_TABLE_FUNC0, 0,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_SET_REG_BY_FIELD(VLAN_TABLE_FUNC0, VT_FUNC, op, val);
    SW_SET_REG_BY_FIELD(VLAN_TABLE_FUNC0, VT_BUSY, 1, val);

    HSL_REG_ENTRY_SET(rv, dev_id, VLAN_TABLE_FUNC0, 0,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));

    SW_RTN_ON_ERROR(rv);

    HSL_REG_FIELD_GET(rv, dev_id, VLAN_TABLE_FUNC0, 0, VT_FULL_VIO,
                      (a_uint8_t *) (&vt_full), sizeof (a_uint32_t));

    SW_RTN_ON_ERROR(rv);

    if (vt_full)
    {
        val = 0x10;
        HSL_REG_ENTRY_SET(rv, dev_id, VLAN_TABLE_FUNC0, 0,
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

    return SW_OK;
}

static sw_error_t
athena_vlan_table_location(a_uint32_t dev_id, a_uint16_t vlan_id,
                           a_int16_t * loc)
{
    a_int16_t i = 0;
    v_array_t *p_v_array;

    if (p_vlan_table[dev_id] == NULL)
        return SW_NOT_INITIALIZED;

    p_v_array = p_vlan_table[dev_id];

    for (i = 0; i < MAX_VLAN_ENTRY; i++)
    {
        if ((p_v_array[i].active == A_TRUE)
                && (p_v_array[i].vlan_entry.vid == vlan_id))
            break;
    }

    if (i == MAX_VLAN_ENTRY)
        return SW_NOT_FOUND;

    *loc = i;

    return SW_OK;
}

static sw_error_t
athena_vlan_sw_to_hw(const fal_vlan_t * vlan_entry, a_uint32_t reg[])
{
    if (A_TRUE == vlan_entry->vid_pri_en)
    {
        SW_SET_REG_BY_FIELD(VLAN_TABLE_FUNC0, VT_PRI_EN, 1, reg[0]);
        SW_SET_REG_BY_FIELD(VLAN_TABLE_FUNC0, VT_PRI, vlan_entry->vid_pri, reg[0]);
    }
    else
    {
        SW_SET_REG_BY_FIELD(VLAN_TABLE_FUNC0, VT_PRI_EN, 0, reg[0]);
    }

    SW_SET_REG_BY_FIELD(VLAN_TABLE_FUNC0, VLAN_ID, vlan_entry->vid, reg[0]);

    SW_SET_REG_BY_FIELD(VLAN_TABLE_FUNC1, VT_VALID, 1, reg[1]);
    SW_SET_REG_BY_FIELD(VLAN_TABLE_FUNC1, VID_MEM, vlan_entry->mem_ports, reg[1]);

    if (0 != vlan_entry->u_ports)
    {
        return SW_BAD_VALUE;
    }

    return SW_OK;
}

static sw_error_t
_athena_vlan_entry_append(a_uint32_t dev_id, const fal_vlan_t * vlan_entry)
{
    sw_error_t rv;
    a_uint32_t reg[2] = { 0 };
#ifdef HSL_STANDALONG
    a_int16_t i, loc = MAX_VLAN_ENTRY;
    v_array_t *p_v_array;
#endif

    HSL_DEV_ID_CHECK(dev_id);

    if ((vlan_entry->vid == 0) || (vlan_entry->vid > MAX_VLAN_ID))
        return SW_OUT_OF_RANGE;

    if (A_FALSE == hsl_mports_prop_check(dev_id, vlan_entry->mem_ports, HSL_PP_INCL_CPU))
        return SW_BAD_PARAM;

#ifdef HSL_STANDALONG
    if ((p_v_array = p_vlan_table[dev_id]) == NULL)
        return SW_NOT_INITIALIZED;

    for (i = 0; i < MAX_VLAN_ENTRY; i++)
    {
        if (p_v_array[i].active == A_FALSE)
        {
            loc = i;
        }
        else if (p_v_array[i].vlan_entry.vid == vlan_entry->vid)
        {
            return SW_ALREADY_EXIST;
        }
    }

    if (loc == MAX_VLAN_ENTRY)
        return SW_FULL;
#endif

    rv = athena_vlan_sw_to_hw(vlan_entry, reg);
    SW_RTN_ON_ERROR(rv);

    HSL_REG_ENTRY_SET(rv, dev_id, VLAN_TABLE_FUNC0, 0,
                      (a_uint8_t *) (&reg[0]), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    HSL_REG_ENTRY_SET(rv, dev_id, VLAN_TABLE_FUNC1, 0,
                      (a_uint8_t *) (&reg[1]), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    rv = athena_vlan_commit(dev_id, VLAN_LOAD_ENTRY);
    SW_RTN_ON_ERROR(rv);

#ifdef HSL_STANDALONG
    p_v_array[loc].vlan_entry = *vlan_entry;
    p_v_array[loc].active     = A_TRUE;
#endif

    return SW_OK;
}


static sw_error_t
_athena_vlan_create(a_uint32_t dev_id, a_uint32_t vlan_id)
{
    sw_error_t rv;
    a_uint32_t vtable_entry = 0;
#ifdef HSL_STANDALONG
    a_int16_t i, loc = MAX_VLAN_ENTRY;
    v_array_t *p_v_array;
#endif

    HSL_DEV_ID_CHECK(dev_id);

    if ((vlan_id == 0) || (vlan_id > MAX_VLAN_ID))
        return SW_OUT_OF_RANGE;

#ifdef HSL_STANDALONG
    if ((p_v_array = p_vlan_table[dev_id]) == NULL)
        return SW_NOT_INITIALIZED;

    for (i = 0; i < MAX_VLAN_ENTRY; i++)
    {
        if (p_v_array[i].active == A_FALSE)
        {
            loc = i;
        }
        else if (p_v_array[i].vlan_entry.vid == vlan_id)
        {
            return SW_ALREADY_EXIST;
        }
    }

    if (loc == MAX_VLAN_ENTRY)
        return SW_FULL;
#endif

    /* set default value for VLAN_TABLE_FUNC0, all 0 except vid */
    vtable_entry = 0;
    SW_SET_REG_BY_FIELD(VLAN_TABLE_FUNC0, VLAN_ID, vlan_id, vtable_entry);
    HSL_REG_ENTRY_SET(rv, dev_id, VLAN_TABLE_FUNC0, 0,
                      (a_uint8_t *) (&vtable_entry), sizeof (a_uint32_t));

    SW_RTN_ON_ERROR(rv);

    /* set default value for VLAN_TABLE_FUNC1, all 0 */
    vtable_entry = 0;
    SW_SET_REG_BY_FIELD(VLAN_TABLE_FUNC1, VT_VALID, 1, vtable_entry);
    HSL_REG_ENTRY_SET(rv, dev_id, VLAN_TABLE_FUNC1, 0,
                      (a_uint8_t *) (&vtable_entry), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    rv = athena_vlan_commit(dev_id, VLAN_LOAD_ENTRY);
    SW_RTN_ON_ERROR(rv);

#ifdef HSL_STANDALONG
    p_v_array[loc].vlan_entry.vid = vlan_id;
    p_v_array[loc].vlan_entry.mem_ports = 0;
    p_v_array[loc].vlan_entry.u_ports = 0;
    p_v_array[loc].vlan_entry.vid_pri_en = A_FALSE;
    p_v_array[loc].vlan_entry.vid_pri = 0;
    p_v_array[loc].active = A_TRUE;
#endif

    return SW_OK;
}


static sw_error_t
_athena_vlan_next(a_uint32_t dev_id, a_uint32_t vlan_id, fal_vlan_t * p_vlan)
{
#ifdef HSL_STANDALONG
    a_uint16_t i = 0, loc = MAX_VLAN_ENTRY;
    a_uint16_t tmp_vid = MAX_VLAN_ID + 1;
    v_array_t *p_v_array;

    HSL_DEV_ID_CHECK(dev_id);

    if (vlan_id > MAX_VLAN_ID)
        return SW_OUT_OF_RANGE;

    if ((p_v_array = p_vlan_table[dev_id]) == NULL)
        return SW_NOT_INITIALIZED;

    for (i = 0; i < MAX_VLAN_ENTRY; i++)
    {
        if ((p_v_array[i].active == A_TRUE)
                && (p_v_array[i].vlan_entry.vid > vlan_id))
        {
            if (tmp_vid > p_v_array[i].vlan_entry.vid)
            {
                loc = i;
                tmp_vid = p_v_array[i].vlan_entry.vid;
            }
        }
    }

    if (loc == MAX_VLAN_ENTRY)
        return SW_NO_MORE;

    *p_vlan = p_v_array[loc].vlan_entry;

    return SW_OK;
#else
    return SW_NOT_SUPPORTED;
#endif
}


static sw_error_t
_athena_vlan_find(a_uint32_t dev_id, a_uint32_t vlan_id, fal_vlan_t * p_vlan)
{
#ifdef HSL_STANDALONG
    a_int16_t loc;
    v_array_t *p_v_array;
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    if ((vlan_id == 0) || (vlan_id > MAX_VLAN_ID))
        return SW_OUT_OF_RANGE;

    if ((p_v_array = p_vlan_table[dev_id]) == NULL)
        return SW_NOT_INITIALIZED;

    rv = athena_vlan_table_location(dev_id, vlan_id, &loc);
    SW_RTN_ON_ERROR(rv);
    *p_vlan = p_v_array[loc].vlan_entry;

    return SW_OK;
#else
    return SW_NOT_SUPPORTED;
#endif
}


static sw_error_t
_athena_vlan_member_update(a_uint32_t dev_id, a_uint32_t vlan_id,
                           fal_pbmp_t member, fal_pbmp_t u_member)
{
#ifdef HSL_STANDALONG
    sw_error_t rv;
    a_int16_t loc;
    a_uint32_t reg_tmp;
    v_array_t *p_v_array;
    fal_vlan_t *p_sw_vlan;

    HSL_DEV_ID_CHECK(dev_id);

    if ((vlan_id == 0) || (vlan_id > MAX_VLAN_ID))
        return SW_OUT_OF_RANGE;

    if (A_FALSE == hsl_mports_prop_check(dev_id, member, HSL_PP_INCL_CPU))
        return SW_BAD_PARAM;

    if (u_member != 0)
        return SW_BAD_PARAM;

    if ((p_v_array = p_vlan_table[dev_id]) == NULL)
        return SW_NOT_INITIALIZED;

    rv = athena_vlan_table_location(dev_id, vlan_id, &loc);
    SW_RTN_ON_ERROR(rv);
    p_sw_vlan = &p_v_array[loc].vlan_entry;

    /* set  value for VLAN_TABLE_FUNC0, all 0 except vid */
    reg_tmp = 0;
    SW_SET_REG_BY_FIELD(VLAN_TABLE_FUNC0, VLAN_ID, vlan_id, reg_tmp);
    SW_SET_REG_BY_FIELD(VLAN_TABLE_FUNC0, VT_PRI_EN,
                        (a_int32_t)p_sw_vlan->vid_pri_en, reg_tmp);
    SW_SET_REG_BY_FIELD(VLAN_TABLE_FUNC0, VT_PRI, p_sw_vlan->vid_pri, reg_tmp);
    HSL_REG_ENTRY_SET(rv, dev_id, VLAN_TABLE_FUNC0, 0,
                      (a_uint8_t *) (&reg_tmp), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    /* set  vlan member for VLAN_TABLE_FUNC1 */
    HSL_REG_FIELD_SET(rv, dev_id, VLAN_TABLE_FUNC1, 0, VID_MEM,
                      (a_uint8_t *) (&member), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    rv = athena_vlan_commit(dev_id, VLAN_LOAD_ENTRY);
    SW_RTN_ON_ERROR(rv);

    p_v_array[loc].vlan_entry.mem_ports = member;

    return SW_OK;
#else
    return SW_NOT_SUPPORTED;
#endif
}


static sw_error_t
_athena_vlan_delete(a_uint32_t dev_id, a_uint32_t vlan_id)
{
    sw_error_t rv;
    a_int16_t loc;
    a_uint32_t reg_tmp;
#ifdef HSL_STANDALONG
    v_array_t *p_v_array;
#endif

    HSL_DEV_ID_CHECK(dev_id);

    if ((vlan_id == 0) || (vlan_id > MAX_VLAN_ID))
        return SW_OUT_OF_RANGE;

#ifdef HSL_STANDALONG
    if ((p_v_array = p_vlan_table[dev_id]) == NULL)
        return SW_NOT_INITIALIZED;

    rv = athena_vlan_table_location(dev_id, vlan_id, &loc);
    SW_RTN_ON_ERROR(rv);
#endif

    reg_tmp = (a_int32_t) vlan_id;
    HSL_REG_FIELD_SET(rv, dev_id, VLAN_TABLE_FUNC0, 0, VLAN_ID,
                      (a_uint8_t *) (&reg_tmp), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    rv = athena_vlan_commit(dev_id, VLAN_PURGE_ENTRY);
    SW_RTN_ON_ERROR(rv);

#ifdef HSL_STANDALONG
    p_v_array[loc].active = A_FALSE;
#endif

    return SW_OK;
}

sw_error_t
athena_vlan_reset(a_uint32_t dev_id)
{
#ifdef HSL_STANDALONG
    a_int16_t i;
    v_array_t *p_v_array;

    HSL_DEV_ID_CHECK(dev_id);

    aos_mem_zero(p_vlan_table[dev_id], MAX_VLAN_ENTRY * (sizeof (v_array_t)));

    p_v_array = p_vlan_table[dev_id];
    for (i = 0; i < MAX_VLAN_ENTRY; i++)
    {
        p_v_array[i].active = A_FALSE;
    }
#endif

    return SW_OK;
}

sw_error_t
athena_vlan_cleanup(a_uint32_t dev_id)
{
    if (p_vlan_table[dev_id])
    {
        aos_mem_free(p_vlan_table[dev_id]);
        p_vlan_table[dev_id] = NULL;
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
athena_vlan_entry_append(a_uint32_t dev_id, const fal_vlan_t * vlan_entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _athena_vlan_entry_append(dev_id, vlan_entry);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Creat a vlan entry through vlan id on a paticular device.
 * @details Comments:
 *    After this operation the member ports of the created vlan entry are null.
 * @param[in] dev_id device id
 * @param[in] vlan_id vlan id
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
athena_vlan_create(a_uint32_t dev_id, a_uint32_t vlan_id)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _athena_vlan_create(dev_id, vlan_id);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Next a vlan entry through vlan id on a paticular device.
 * @details Comments:
 *    If the value of vid is zero this operation will get the first entry.
 * @param[in] dev_id device id
 * @param[in] vlan_id vlan id
 * @param[out] p_vlan vlan entry
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
athena_vlan_next(a_uint32_t dev_id, a_uint32_t vlan_id, fal_vlan_t * p_vlan)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _athena_vlan_next(dev_id, vlan_id, p_vlan);
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
athena_vlan_find(a_uint32_t dev_id, a_uint32_t vlan_id, fal_vlan_t * p_vlan)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _athena_vlan_find(dev_id, vlan_id, p_vlan);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Update a vlan entry member port through vlan id on a paticular device.
 * @param[in] dev_id device id
 * @param[in] vlan_id vlan id
 * @param[in] member member ports
 * @param[in] u_member tagged or untagged infomation for member ports
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
athena_vlan_member_update(a_uint32_t dev_id, a_uint32_t vlan_id,
                          fal_pbmp_t member, fal_pbmp_t u_member)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _athena_vlan_member_update(dev_id, vlan_id, member, u_member);
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
athena_vlan_delete(a_uint32_t dev_id, a_uint32_t vlan_id)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _athena_vlan_delete(dev_id, vlan_id);
    HSL_API_UNLOCK;
    return rv;
}

sw_error_t
athena_vlan_init(a_uint32_t dev_id)
{
#ifdef HSL_STANDALONG
    a_int16_t i;
    v_array_t *p_v_array;
    v_array_t *p_mem;

    HSL_DEV_ID_CHECK(dev_id);

    /* allocate memory for vlan info */
    p_mem = aos_mem_alloc(MAX_VLAN_ENTRY * (sizeof (v_array_t)));
    if (p_mem == NULL)
        return SW_OUT_OF_MEM;

    aos_mem_zero(p_mem, MAX_VLAN_ENTRY * (sizeof (v_array_t)));

    /* start address for vlan info */
    p_vlan_table[dev_id] = p_v_array = p_mem;

    for (i = 0; i < MAX_VLAN_ENTRY; i++)
    {
        p_v_array[i].active = A_FALSE;
    }
#endif

#ifndef HSL_STANDALONG
    hsl_api_t *p_api;

    SW_RTN_ON_NULL (p_api = hsl_api_ptr_get(dev_id));

    p_api->vlan_entry_append = athena_vlan_entry_append;
    p_api->vlan_creat = athena_vlan_create;
    p_api->vlan_delete = athena_vlan_delete;
#endif

    return SW_OK;
}

/**
 * @}
 */

