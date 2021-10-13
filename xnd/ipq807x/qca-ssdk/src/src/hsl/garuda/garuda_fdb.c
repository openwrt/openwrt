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
 * @defgroup garuda_fdb GARUDA_FDB
 * @{
 */
#include "sw.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_port_prop.h"
#include "garuda_fdb.h"
#include "garuda_reg.h"

#define ARL_FLUSH_ALL             1
#define ARL_LOAD_ENTRY            2
#define ARL_PURGE_ENTRY           3
#define ARL_FLUSH_ALL_UNLOCK      4
#define ARL_FLUSH_PORT_UNICAST    5
#define ARL_NEXT_ENTRY            6
#define ARL_FIND_ENTRY            7

#define ARL_FIRST_ENTRY           1001
#define ARL_FLUSH_PORT_NO_STATIC  1002
#define ARL_FLUSH_PORT_AND_STATIC 1003

static a_bool_t
garuda_fdb_is_zeroaddr(fal_mac_addr_t addr)
{
    a_uint32_t i;

    for (i = 0; i < 6; i++)
    {
        if (addr.uc[i])
        {
            return A_FALSE;
        }
    }

    return A_TRUE;
}

static void
garuda_fdb_fill_addr(fal_mac_addr_t addr, a_uint32_t * reg0, a_uint32_t * reg1)
{
    SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC1, AT_ADDR_BYTE0, addr.uc[0], *reg1);
    SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC1, AT_ADDR_BYTE1, addr.uc[1], *reg1);
    SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC1, AT_ADDR_BYTE2, addr.uc[2], *reg1);
    SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC1, AT_ADDR_BYTE3, addr.uc[3], *reg1);

    SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC0, AT_ADDR_BYTE4, addr.uc[4], *reg0);
    SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC0, AT_ADDR_BYTE5, addr.uc[5], *reg0);

    return;
}

static sw_error_t
garuda_atu_sw_to_hw(a_uint32_t dev_id, const fal_fdb_entry_t * entry,
                    a_uint32_t reg[])
{
    a_uint32_t port;

    if (A_FALSE == entry->portmap_en)
    {
        if (A_TRUE !=
                hsl_port_prop_check(dev_id, entry->port.id, HSL_PP_INCL_CPU))
        {
            return SW_BAD_PARAM;
        }

        port = 0x1UL << entry->port.id;
    }
    else
    {
        if (A_FALSE ==
                hsl_mports_prop_check(dev_id, entry->port.map, HSL_PP_INCL_CPU))
        {
            return SW_BAD_PARAM;
        }

        port = entry->port.map;
    }

    if (FAL_MAC_CPY_TO_CPU == entry->dacmd)
    {
        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC2, COPY_TO_CPU, 1, reg[2]);
    }
    else if (FAL_MAC_RDT_TO_CPU == entry->dacmd)
    {
        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC2, REDRCT_TO_CPU, 1, reg[2]);
    }
    else if (FAL_MAC_FRWRD != entry->dacmd)
    {
        return SW_NOT_SUPPORTED;
    }

    if (FAL_MAC_DROP == entry->sacmd)
    {
        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC2, SA_DROP_EN, 1, reg[2]);
    }
    else if (FAL_MAC_FRWRD != entry->sacmd)
    {
        return SW_NOT_SUPPORTED;
    }

    if (A_TRUE == entry->leaky_en)
    {
        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC2, LEAKY_EN, 1, reg[2]);
    }
    else
    {
        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC2, LEAKY_EN, 0, reg[2]);
    }

    if (A_TRUE == entry->static_en)
    {
        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC2, AT_STATUS, 15, reg[2]);
    }
    else
    {
        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC2, AT_STATUS, 7, reg[2]);
    }

    if (A_TRUE == entry->mirror_en)
    {
        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC2, MIRROR_EN, 1, reg[2]);
    }

    if (A_TRUE == entry->clone_en)
    {
        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC2, CLONE_EN, 1, reg[2]);
    }

    if (A_TRUE == entry->da_pri_en)
    {
        hsl_dev_t *p_dev;

        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC2, AT_PRI_EN, 1, reg[2]);

        SW_RTN_ON_NULL(p_dev = hsl_dev_ptr_get(dev_id));

        if (entry->da_queue > (p_dev->nr_queue - 1))
            return SW_BAD_PARAM;

        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC2, AT_PRI, entry->da_queue, reg[2]);
    }

    if (A_TRUE == entry->cross_pt_state)
    {
        return SW_NOT_SUPPORTED;
    }

    SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC2, DES_PORT, port, reg[2]);
    garuda_fdb_fill_addr(entry->addr, &reg[0], &reg[1]);

    return SW_OK;
}

static void
garuda_atu_hw_to_sw(const a_uint32_t reg[], fal_fdb_entry_t * entry)
{
    a_uint32_t i, data;

    aos_mem_zero(entry, sizeof (fal_fdb_entry_t));

    entry->dacmd = FAL_MAC_FRWRD;
    SW_GET_FIELD_BY_REG(ADDR_TABLE_FUNC2, COPY_TO_CPU, data, reg[2]);
    if (1 == data)
    {
        entry->dacmd = FAL_MAC_CPY_TO_CPU;
    }

    SW_GET_FIELD_BY_REG(ADDR_TABLE_FUNC2, REDRCT_TO_CPU, data, reg[2]);
    if (1 == data)
    {
        entry->dacmd = FAL_MAC_RDT_TO_CPU;
    }

    entry->sacmd = FAL_MAC_FRWRD;
    SW_GET_FIELD_BY_REG(ADDR_TABLE_FUNC2, SA_DROP_EN, data, reg[2]);
    if (1 == data)
    {
        entry->sacmd = FAL_MAC_DROP;
    }

    entry->leaky_en = A_FALSE;
    SW_GET_FIELD_BY_REG(ADDR_TABLE_FUNC2, LEAKY_EN, data, reg[2]);
    if (1 == data)
    {
        entry->leaky_en = A_TRUE;
    }

    entry->static_en = A_FALSE;
    SW_GET_FIELD_BY_REG(ADDR_TABLE_FUNC2, AT_STATUS, data, reg[2]);
    if (0xf == data)
    {
        entry->static_en = A_TRUE;
    }

    entry->mirror_en = A_FALSE;
    SW_GET_FIELD_BY_REG(ADDR_TABLE_FUNC2, MIRROR_EN, data, reg[2]);
    if (1 == data)
    {
        entry->mirror_en = A_TRUE;
    }

    entry->clone_en = A_FALSE;
    SW_GET_FIELD_BY_REG(ADDR_TABLE_FUNC2, CLONE_EN, data, reg[2]);
    if (1 == data)
    {
        entry->clone_en = A_TRUE;
    }

    entry->da_pri_en = A_FALSE;
    SW_GET_FIELD_BY_REG(ADDR_TABLE_FUNC2, AT_PRI_EN, data, reg[2]);
    if (1 == data)
    {
        SW_GET_FIELD_BY_REG(ADDR_TABLE_FUNC2, AT_PRI, data, reg[2]);
        entry->da_pri_en = A_TRUE;
        entry->da_queue = data & 0x3;
    }

    entry->cross_pt_state = A_FALSE;

    SW_GET_FIELD_BY_REG(ADDR_TABLE_FUNC2, DES_PORT, data, reg[2]);

    entry->portmap_en = A_TRUE;
    entry->port.map = data;

    for (i = 0; i < 4; i++)
    {
        entry->addr.uc[i] = (reg[1] >> ((3 - i) << 3)) & 0xff;
    }

    for (i = 4; i < 6; i++)
    {
        entry->addr.uc[i] = (reg[0] >> ((7 - i) << 3)) & 0xff;
    }

    return;
}

static sw_error_t
garuda_fdb_commit(a_uint32_t dev_id, a_uint32_t op)
{
    sw_error_t rv;
    a_uint32_t busy = 1;
    a_uint32_t full_vio;
    a_uint32_t i = 1000;
    a_uint32_t entry;
    a_uint32_t hwop = op;

    while (busy && --i)
    {
        HSL_REG_FIELD_GET(rv, dev_id, ADDR_TABLE_FUNC0, 0, AT_BUSY,
                          (a_uint8_t *) (&busy), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
        aos_udelay(5);
    }

    if (0 == i)
    {
        return SW_BUSY;
    }

    HSL_REG_ENTRY_GET(rv, dev_id, ADDR_TABLE_FUNC0, 0,
                      (a_uint8_t *) (&entry), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC0, AT_BUSY, 1, entry);

    if (ARL_FLUSH_PORT_AND_STATIC == hwop)
    {
        hwop = ARL_FLUSH_PORT_UNICAST;
        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC0, FLUSH_ST_EN, 1, entry);
    }

    if (ARL_FLUSH_PORT_NO_STATIC == hwop)
    {
        hwop = ARL_FLUSH_PORT_UNICAST;
        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC0, FLUSH_ST_EN, 0, entry);
    }

    SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC0, AT_FUNC, hwop, entry);

    HSL_REG_ENTRY_SET(rv, dev_id, ADDR_TABLE_FUNC0, 0,
                      (a_uint8_t *) (&entry), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    busy = 1;
    i = 1000;
    while (busy && --i)
    {
        HSL_REG_FIELD_GET(rv, dev_id, ADDR_TABLE_FUNC0, 0, AT_BUSY,
                          (a_uint8_t *) (&busy), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
        aos_udelay(5);
    }

    if (0 == i)
    {
        return SW_FAIL;
    }

    HSL_REG_FIELD_GET(rv, dev_id, ADDR_TABLE_FUNC0, 0, AT_FULL_VIO,
                      (a_uint8_t *) (&full_vio), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (full_vio)
    {
        /* must clear AT_FULL_VOI bit */
        entry = 0x1000;
        HSL_REG_ENTRY_SET(rv, dev_id, ADDR_TABLE_FUNC0, 0,
                          (a_uint8_t *) (&entry), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        if (ARL_LOAD_ENTRY == hwop)
        {
            return SW_FULL;
        }
        else if ((ARL_PURGE_ENTRY == hwop)
                 || (ARL_FLUSH_PORT_UNICAST == hwop))
        {
            return SW_NOT_FOUND;
        }
        else
        {
            return SW_FAIL;
        }
    }

    return SW_OK;
}

static sw_error_t
garuda_atu_get(a_uint32_t dev_id, fal_fdb_entry_t * entry, a_uint32_t op)
{
    sw_error_t rv;
    a_uint32_t reg[3] = { 0 };
    a_uint32_t status = 0;
    a_uint32_t hwop = op;

    if ((ARL_NEXT_ENTRY == op)
            || (ARL_FIND_ENTRY == op))
    {
        garuda_fdb_fill_addr(entry->addr, &reg[0], &reg[1]);
    }

    HSL_REG_ENTRY_SET(rv, dev_id, ADDR_TABLE_FUNC0, 0,
                      (a_uint8_t *) (&reg[0]), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    HSL_REG_ENTRY_SET(rv, dev_id, ADDR_TABLE_FUNC1, 0,
                      (a_uint8_t *) (&reg[1]), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    /* set status not zero */
    if (ARL_NEXT_ENTRY == op)
    {
        reg[2] = 0xf0000;
    }

    if (ARL_FIRST_ENTRY == op)
    {
        hwop = ARL_NEXT_ENTRY;
    }

    HSL_REG_ENTRY_SET(rv, dev_id, ADDR_TABLE_FUNC2, 0,
                      (a_uint8_t *) (&reg[2]), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    rv = garuda_fdb_commit(dev_id, hwop);
    SW_RTN_ON_ERROR(rv);

    /* get hardware enrety */
    HSL_REG_ENTRY_GET(rv, dev_id, ADDR_TABLE_FUNC0, 0,
                      (a_uint8_t *) (&reg[0]), sizeof (a_uint32_t));

    SW_RTN_ON_ERROR(rv);

    HSL_REG_ENTRY_GET(rv, dev_id, ADDR_TABLE_FUNC1, 0,
                      (a_uint8_t *) (&reg[1]), sizeof (a_uint32_t));

    SW_RTN_ON_ERROR(rv);

    HSL_REG_ENTRY_GET(rv, dev_id, ADDR_TABLE_FUNC2, 0,
                      (a_uint8_t *) (&reg[2]), sizeof (a_uint32_t));

    SW_RTN_ON_ERROR(rv);

    SW_GET_FIELD_BY_REG(ADDR_TABLE_FUNC2, AT_STATUS, status, reg[2]);

    garuda_atu_hw_to_sw(reg, entry);

    /* If hardware return back with address and status all zero,
       that means no other next valid entry in fdb table */
    if ((A_TRUE == garuda_fdb_is_zeroaddr(entry->addr))
            && (0 == status))
    {
        if (ARL_NEXT_ENTRY == op)
        {
            return SW_NO_MORE;
        }
        else if ((ARL_FIND_ENTRY == op)
                 || (ARL_FIRST_ENTRY == op))
        {
            return SW_NOT_FOUND;
        }
        else
        {
            return SW_FAIL;
        }
    }
    else
    {
        return SW_OK;
    }
}

static sw_error_t
_garuda_fdb_add(a_uint32_t dev_id, const fal_fdb_entry_t * entry)
{
    sw_error_t rv;
    a_uint32_t reg[3] = { 0, 0, 0 };

    HSL_DEV_ID_CHECK(dev_id);

    rv = garuda_atu_sw_to_hw(dev_id, entry, reg);
    SW_RTN_ON_ERROR(rv);

    HSL_REG_ENTRY_SET(rv, dev_id, ADDR_TABLE_FUNC2, 0,
                      (a_uint8_t *) (&reg[2]), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    HSL_REG_ENTRY_SET(rv, dev_id, ADDR_TABLE_FUNC1, 0, (a_uint8_t *) (&reg[1]),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    HSL_REG_ENTRY_SET(rv, dev_id, ADDR_TABLE_FUNC0, 0, (a_uint8_t *) (&reg[0]),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    rv = garuda_fdb_commit(dev_id, ARL_LOAD_ENTRY);

    return rv;
}


static sw_error_t
_garuda_fdb_del_all(a_uint32_t dev_id, a_uint32_t flag)
{
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    if (FAL_FDB_DEL_STATIC & flag)
    {
        rv = garuda_fdb_commit(dev_id, ARL_FLUSH_ALL);
    }
    else
    {
        rv = garuda_fdb_commit(dev_id, ARL_FLUSH_ALL_UNLOCK);
    }

    return rv;
}


static sw_error_t
_garuda_fdb_del_by_port(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t flag)
{
    sw_error_t rv;
    a_uint32_t reg = 0;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_FALSE == hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC0, AT_PORT_NUM, port_id, reg);

    HSL_REG_ENTRY_SET(rv, dev_id, ADDR_TABLE_FUNC0, 0, (a_uint8_t *) (&reg),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (FAL_FDB_DEL_STATIC & flag)
    {
        rv = garuda_fdb_commit(dev_id, ARL_FLUSH_PORT_AND_STATIC);
    }
    else
    {
        rv = garuda_fdb_commit(dev_id, ARL_FLUSH_PORT_NO_STATIC);
    }

    return rv;
}


static sw_error_t
_garuda_fdb_del_by_mac(a_uint32_t dev_id, const fal_fdb_entry_t * entry)
{
    sw_error_t rv;
    a_uint32_t reg0 = 0, reg1 = 0;

    HSL_DEV_ID_CHECK(dev_id);

    garuda_fdb_fill_addr(entry->addr, &reg0, &reg1);

    HSL_REG_ENTRY_SET(rv, dev_id, ADDR_TABLE_FUNC1, 0, (a_uint8_t *) (&reg1),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    HSL_REG_ENTRY_SET(rv, dev_id, ADDR_TABLE_FUNC0, 0, (a_uint8_t *) (&reg0),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    rv = garuda_fdb_commit(dev_id, ARL_PURGE_ENTRY);
    return rv;
}


static sw_error_t
_garuda_fdb_next(a_uint32_t dev_id, fal_fdb_entry_t * entry)
{
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    rv = garuda_atu_get(dev_id, entry, ARL_NEXT_ENTRY);
    return rv;
}


static sw_error_t
_garuda_fdb_first(a_uint32_t dev_id, fal_fdb_entry_t * entry)
{
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    rv = garuda_atu_get(dev_id, entry, ARL_FIRST_ENTRY);
    return rv;
}


static sw_error_t
_garuda_fdb_find(a_uint32_t dev_id, fal_fdb_entry_t * entry)
{
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    rv = garuda_atu_get(dev_id, entry, ARL_FIND_ENTRY);
    return rv;
}


static sw_error_t
_garuda_fdb_port_learn_set(a_uint32_t dev_id, fal_port_t port_id,
                           a_bool_t enable)
{
    a_uint32_t data;
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
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

    HSL_REG_FIELD_SET(rv, dev_id, PORT_CTL, port_id, LEARN_EN,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    return rv;
}


static sw_error_t
_garuda_fdb_port_learn_get(a_uint32_t dev_id, fal_port_t port_id,
                           a_bool_t *enable)
{
    a_uint32_t data;
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_GET(rv, dev_id, PORT_CTL, port_id, LEARN_EN,
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
_garuda_fdb_age_ctrl_set(a_uint32_t dev_id, a_bool_t enable)
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

    HSL_REG_FIELD_SET(rv, dev_id, ADDR_TABLE_CTL, 0, AGE_EN,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    return rv;
}


static sw_error_t
_garuda_fdb_age_ctrl_get(a_uint32_t dev_id, a_bool_t *enable)
{
    a_uint32_t data;
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    HSL_REG_FIELD_GET(rv, dev_id, ADDR_TABLE_CTL, 0, AGE_EN,
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
_garuda_fdb_age_time_set(a_uint32_t dev_id, a_uint32_t * time)
{
    a_uint32_t data;
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    if ((65535 * 7 < *time) || (7 > *time))
    {
        return SW_BAD_PARAM;
    }
    data = *time / 7;
    *time = data * 7;
    HSL_REG_FIELD_SET(rv, dev_id, ADDR_TABLE_CTL, 0, AGE_TIME,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    return rv;
}


static sw_error_t
_garuda_fdb_age_time_get(a_uint32_t dev_id, a_uint32_t *time)
{
    a_uint32_t data;
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    HSL_REG_FIELD_GET(rv, dev_id, ADDR_TABLE_CTL, 0, AGE_TIME,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    *time = data * 7;
    return SW_OK;
}

/**
 * @brief Add a Fdb entry
 * @param[in] dev_id device id
 * @param[in] entry fdb entry
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
garuda_fdb_add(a_uint32_t dev_id, const fal_fdb_entry_t * entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _garuda_fdb_add(dev_id, entry);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Delete all Fdb entries
 *   @details   Comments:
 *         If set FAL_FDB_DEL_STATIC bit in flag which means delete all fdb
 *       entries otherwise only delete dynamic entries.
 * @param[in] dev_id device id
 * @param[in] flag delete operation option
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
garuda_fdb_del_all(a_uint32_t dev_id, a_uint32_t flag)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _garuda_fdb_del_all(dev_id, flag);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Delete Fdb entries on a particular port
 *   @details   Comments:
 *       If set FAL_FDB_DEL_STATIC bit in flag which means delete all fdb
 *       entries otherwise only delete dynamic entries.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] flag delete operation option
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
garuda_fdb_del_by_port(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t flag)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _garuda_fdb_del_by_port(dev_id, port_id, flag);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Delete a particular Fdb entry through mac address
 *   @details   Comments:
 *       Only addr field in entry is meaning. For IVL learning vid or fid field
 *       also is meaning.
 * @param[in] dev_id device id
 * @param[in] entry fdb entry
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
garuda_fdb_del_by_mac(a_uint32_t dev_id, const fal_fdb_entry_t * entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _garuda_fdb_del_by_mac(dev_id, entry);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get next Fdb entry from particular device
 *   @details   Comments:
 *    For input parameter only addr field in entry is meaning.
 * @param[in] dev_id device id
 * @param entry fdb entry
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
garuda_fdb_next(a_uint32_t dev_id, fal_fdb_entry_t * entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _garuda_fdb_next(dev_id, entry);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get first Fdb entry from particular device
 * @param[in] dev_id device id
 * @param[out] entry fdb entry
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
garuda_fdb_first(a_uint32_t dev_id, fal_fdb_entry_t * entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _garuda_fdb_first(dev_id, entry);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Find a particular Fdb entry from device through mac address.
 *    @details  Comments:
    For input parameter only addr field in entry is meaning.
 * @param[in] dev_id device id
 * @param[in] entry fdb entry
 * @param[out] entry fdb entry
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
garuda_fdb_find(a_uint32_t dev_id, fal_fdb_entry_t * entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _garuda_fdb_find(dev_id, entry);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set dynamic address learning status on a particular port.
 *    @details  Comments:
 *       This operation will enable or disable dynamic address learning
 *       feature on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable enable or disable
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
garuda_fdb_port_learn_set(a_uint32_t dev_id, fal_port_t port_id,
                          a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _garuda_fdb_port_learn_set(dev_id, port_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get dynamic address learning status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
garuda_fdb_port_learn_get(a_uint32_t dev_id, fal_port_t port_id,
                          a_bool_t *enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _garuda_fdb_port_learn_get(dev_id, port_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set dynamic address aging status on particular device.
 *   @details  Comments:
 *       This operation will enable or disable dynamic address aging
 *       feature on particular device.
 * @param[in] dev_id device id
 * @param[in] enable enable or disable
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
garuda_fdb_age_ctrl_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _garuda_fdb_age_ctrl_set(dev_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get dynamic address aging status on particular device.
 * @param[in] dev_id device id
 * @param[in] enable enable or disable
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
garuda_fdb_age_ctrl_get(a_uint32_t dev_id, a_bool_t *enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _garuda_fdb_age_ctrl_get(dev_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set dynamic address aging time on a particular device.
 * @details   Comments:
 *       This operation will set dynamic address aging time on a particular device.
 *       The unit of time is second. Because different device has differnet
 *       hardware granularity function will return actual time in hardware.
 * @param[in] dev_id device id
 * @param time aging time
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
garuda_fdb_age_time_set(a_uint32_t dev_id, a_uint32_t * time)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _garuda_fdb_age_time_set(dev_id, time);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get dynamic address aging time on a particular device.
 * @param[in] dev_id device id
 * @param[out] time aging time
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
garuda_fdb_age_time_get(a_uint32_t dev_id, a_uint32_t *time)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _garuda_fdb_age_time_get(dev_id, time);
    HSL_API_UNLOCK;
    return rv;
}

sw_error_t
garuda_fdb_init(a_uint32_t dev_id)
{
    HSL_DEV_ID_CHECK(dev_id);

#ifndef HSL_STANDALONG
    {
        hsl_api_t *p_api;

        SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

        p_api->fdb_add = garuda_fdb_add;
        p_api->fdb_del_all = garuda_fdb_del_all;
        p_api->fdb_del_by_port = garuda_fdb_del_by_port;
        p_api->fdb_del_by_mac = garuda_fdb_del_by_mac;
        p_api->fdb_first = garuda_fdb_first;
        p_api->fdb_next = garuda_fdb_next;
        p_api->fdb_find = garuda_fdb_find;
        p_api->port_learn_set = garuda_fdb_port_learn_set;
        p_api->port_learn_get = garuda_fdb_port_learn_get;
        p_api->age_ctrl_set = garuda_fdb_age_ctrl_set;
        p_api->age_ctrl_get = garuda_fdb_age_ctrl_get;
        p_api->age_time_set = garuda_fdb_age_time_set;
        p_api->age_time_get = garuda_fdb_age_time_get;
    }
#endif

    return SW_OK;
}

/**
 * @}
 */

