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
 * @defgroup isisc_fdb ISISC_FDB
 * @{
 */
#include "sw.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_port_prop.h"
#include "isisc_fdb.h"
#include "isisc_reg.h"
#include "isisc_fdb_prv.h"

#ifndef IN_FDB_MINI
static sw_error_t
_isisc_wl_feature_check(a_uint32_t dev_id)
{
    sw_error_t rv;
    a_uint32_t entry = 0;

    HSL_REG_FIELD_GET(rv, dev_id, MASK_CTL, 0, DEVICE_ID,
                      (a_uint8_t *) (&entry), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (S17C_DEVICE_ID == entry)
    {
        return SW_OK;
    }
    else
    {
        return SW_NOT_SUPPORTED;
    }
}
#endif

static a_bool_t
_isisc_fdb_is_zeroaddr(fal_mac_addr_t addr)
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
_isisc_fdb_fill_addr(fal_mac_addr_t addr, a_uint32_t * reg0, a_uint32_t * reg1)
{
    SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC1, AT_ADDR_BYTE0, addr.uc[0], *reg1);
    SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC1, AT_ADDR_BYTE1, addr.uc[1], *reg1);

    SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC0, AT_ADDR_BYTE2, addr.uc[2], *reg0);
    SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC0, AT_ADDR_BYTE3, addr.uc[3], *reg0);
    SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC0, AT_ADDR_BYTE4, addr.uc[4], *reg0);
    SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC0, AT_ADDR_BYTE5, addr.uc[5], *reg0);

    return;
}

#ifndef IN_FDB_MINI
static sw_error_t
_isisc_atu_sw_to_hw(a_uint32_t dev_id, const fal_fdb_entry_t * entry,
                   a_uint32_t reg[])
{
    a_uint32_t port;
    sw_error_t rv;

    if (A_TRUE == entry->white_list_en)
    {
        rv = _isisc_wl_feature_check(dev_id);
        SW_RTN_ON_ERROR(rv);

        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC2, WL_EN, 1, reg[2]);
    }

    if (FAL_SVL_FID == entry->fid)
    {
        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC2, AT_VID, 0, reg[2]);
        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC1, AT_SVL_EN, 1, reg[1]);
    }
    else if (ISISC_MAX_FID >= entry->fid)
    {
        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC2, AT_VID, (entry->fid), reg[2]);
        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC1, AT_SVL_EN, 0, reg[1]);
    }
    else
    {
        return SW_BAD_PARAM;
    }

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
    SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC1, DES_PORT, port, reg[1]);

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

    if (FAL_MAC_DROP == entry->sacmd)
    {
        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC1, SA_DROP_EN, 1, reg[1]);
    }
    else if (FAL_MAC_FRWRD != entry->sacmd)
    {
        return SW_NOT_SUPPORTED;
    }

    if (A_TRUE == entry->mirror_en)
    {
        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC1, MIRROR_EN, 1, reg[1]);
    }

    if (A_TRUE == entry->cross_pt_state)
    {
        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC1, CROSS_PT, 1, reg[1]);
    }
    else
    {
        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC1, CROSS_PT, 0, reg[1]);
    }

    if (A_TRUE == entry->da_pri_en)
    {
        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC1, AT_PRI_EN, 1, reg[1]);
        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC1, AT_PRI, (entry->da_queue & 0x7),
                            reg[1]);
    }
    else
    {
        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC1, AT_PRI_EN, 0, reg[1]);
    }

    _isisc_fdb_fill_addr(entry->addr, &reg[0], &reg[1]);
    return SW_OK;
}
#endif

static void
_isisc_atu_hw_to_sw(const a_uint32_t reg[], fal_fdb_entry_t * entry)
{
    a_uint32_t i, data;

    aos_mem_zero(entry, sizeof (fal_fdb_entry_t));

    SW_GET_FIELD_BY_REG(ADDR_TABLE_FUNC1, AT_SVL_EN, data, reg[1]);
    if (data)
    {
        entry->fid = FAL_SVL_FID;
    }
    else
    {
        SW_GET_FIELD_BY_REG(ADDR_TABLE_FUNC2, AT_VID, data, reg[2]);
        entry->fid = data;
    }

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
    SW_GET_FIELD_BY_REG(ADDR_TABLE_FUNC1, SA_DROP_EN, data, reg[1]);
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
    SW_GET_FIELD_BY_REG(ADDR_TABLE_FUNC1, MIRROR_EN, data, reg[1]);
    if (1 == data)
    {
        entry->mirror_en = A_TRUE;
    }

    entry->da_pri_en = A_FALSE;
    SW_GET_FIELD_BY_REG(ADDR_TABLE_FUNC1, AT_PRI_EN, data, reg[1]);
    if (1 == data)
    {
        SW_GET_FIELD_BY_REG(ADDR_TABLE_FUNC1, AT_PRI, data, reg[1]);
        entry->da_pri_en = A_TRUE;
        entry->da_queue = data & 0x7;
    }

    entry->cross_pt_state = A_FALSE;
    SW_GET_FIELD_BY_REG(ADDR_TABLE_FUNC1, CROSS_PT, data, reg[1]);
    if (1 == data)
    {
        entry->cross_pt_state = A_TRUE;
    }

    SW_GET_FIELD_BY_REG(ADDR_TABLE_FUNC1, DES_PORT, data, reg[1]);
    entry->portmap_en = A_TRUE;
    entry->port.map = data;

    for (i = 2; i < 6; i++)
    {
        entry->addr.uc[i] = (reg[0] >> ((5 - i) << 3)) & 0xff;
    }

    for (i = 0; i < 2; i++)
    {
        entry->addr.uc[i] = (reg[1] >> ((1 - i) << 3)) & 0xff;
    }

    entry->white_list_en = A_FALSE;
    SW_GET_FIELD_BY_REG(ADDR_TABLE_FUNC2, WL_EN, data, reg[2]);
    if (1 == data)
    {
        entry->white_list_en = A_TRUE;
    }

    return;
}

static sw_error_t
_isisc_atu_down_to_hw(a_uint32_t dev_id, a_uint32_t reg[])
{
    sw_error_t rv;

    HSL_REG_ENTRY_SET(rv, dev_id, ADDR_TABLE_FUNC0, 0,
                      (a_uint8_t *) (&reg[0]), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    HSL_REG_ENTRY_SET(rv, dev_id, ADDR_TABLE_FUNC1, 0,
                      (a_uint8_t *) (&reg[1]), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    HSL_REG_ENTRY_SET(rv, dev_id, ADDR_TABLE_FUNC2, 0,
                      (a_uint8_t *) (&reg[2]), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    HSL_REG_ENTRY_SET(rv, dev_id, ADDR_TABLE_FUNC3, 0,
                      (a_uint8_t *) (&reg[3]), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_isisc_atu_up_to_sw(a_uint32_t dev_id, a_uint32_t reg[])
{
    sw_error_t rv;

    HSL_REG_ENTRY_GET(rv, dev_id, ADDR_TABLE_FUNC0, 0,
                      (a_uint8_t *) (&reg[0]), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    HSL_REG_ENTRY_GET(rv, dev_id, ADDR_TABLE_FUNC1, 0,
                      (a_uint8_t *) (&reg[1]), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    HSL_REG_ENTRY_GET(rv, dev_id, ADDR_TABLE_FUNC2, 0,
                      (a_uint8_t *) (&reg[2]), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    HSL_REG_ENTRY_GET(rv, dev_id, ADDR_TABLE_FUNC3, 0,
                      (a_uint8_t *) (&reg[3]), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_isisc_fdb_commit(a_uint32_t dev_id, a_uint32_t op)
{
    sw_error_t rv;
    a_uint32_t busy = 1;
    a_uint32_t full_vio;
    a_uint32_t i = 2000;
    a_uint32_t entry = 0;
    a_uint32_t hwop = op;

    while (busy && --i)
    {
        HSL_REG_ENTRY_GET(rv, dev_id, ADDR_TABLE_FUNC3, 0,
                          (a_uint8_t *) (&entry), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
        SW_GET_FIELD_BY_REG(ADDR_TABLE_FUNC3, AT_BUSY, busy, entry);
    }

    if (0 == i)
    {
        return SW_BUSY;
    }

    if (ARL_FIRST_ENTRY == op)
    {
        hwop = ARL_NEXT_ENTRY;
    }

    SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC3, AT_BUSY, 1, entry);

    if (ARL_FLUSH_PORT_AND_STATIC == hwop)
    {
        hwop = ARL_FLUSH_PORT_UNICAST;
        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC3, FLUSH_ST_EN, 1, entry);
    }

    if (ARL_FLUSH_PORT_NO_STATIC == hwop)
    {
        hwop = ARL_FLUSH_PORT_UNICAST;
        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC3, FLUSH_ST_EN, 0, entry);
    }

    SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC3, AT_FUNC, hwop, entry);

    HSL_REG_ENTRY_SET(rv, dev_id, ADDR_TABLE_FUNC3, 0,
                      (a_uint8_t *) (&entry), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    busy = 1;
    i = 2000;
    while (busy && --i)
    {
        HSL_REG_ENTRY_GET(rv, dev_id, ADDR_TABLE_FUNC3, 0,
                          (a_uint8_t *) (&entry), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
        SW_GET_FIELD_BY_REG(ADDR_TABLE_FUNC3, AT_BUSY, busy, entry);
    }

    if (0 == i)
    {
        return SW_FAIL;
    }

    SW_GET_FIELD_BY_REG(ADDR_TABLE_FUNC3, AT_FULL_VIO, full_vio, entry);

    if (full_vio)
    {
        /* must clear AT_FULL_VOI bit */
        entry = 0x1000;
        HSL_REG_ENTRY_SET(rv, dev_id, ADDR_TABLE_FUNC3, 0,
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
_isisc_fdb_get(a_uint32_t dev_id, fal_fdb_op_t * option, fal_fdb_entry_t * entry,
              a_uint32_t hwop)
{
    sw_error_t rv;
    a_uint32_t i, port = 0, status, reg[4] = { 0 };

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE == option->port_en)
    {
        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC3, AT_PORT_EN, 1, reg[3]);
        if (A_FALSE == entry->portmap_en)
        {
            if (A_TRUE !=
                    hsl_port_prop_check(dev_id, entry->port.id, HSL_PP_INCL_CPU))
            {
                return SW_BAD_PARAM;
            }
            port = entry->port.id;
        }
        else
        {
            if (A_FALSE ==
                    hsl_mports_prop_check(dev_id, entry->port.map,
                                          HSL_PP_INCL_CPU))
            {
                return SW_BAD_PARAM;
            }

            status = 0;
            for (i = 0; i < SW_MAX_NR_PORT; i++)
            {
                if ((entry->port.map) & (0x1UL << i))
                {
                    if (status)
                    {
                        return SW_BAD_PARAM;
                    }
                    port = i;
                    status = 1;
                }
            }
        }
        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC3, AT_PORT_NUM, port, reg[3]);
    }

    if (A_TRUE == option->fid_en)
    {
        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC3, AT_VID_EN, 1, reg[3]);
    }

    if (A_TRUE == option->multicast_en)
    {
        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC3, AT_MULTI_EN, 1, reg[3]);
    }

    if (FAL_SVL_FID == entry->fid)
    {
        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC2, AT_VID, 0, reg[2]);
        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC1, AT_SVL_EN, 1, reg[1]);
    }
    else if (ISISC_MAX_FID >= entry->fid)
    {
        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC2, AT_VID, entry->fid, reg[2]);
        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC1, AT_SVL_EN, 0, reg[1]);
    }
    else
    {
        return SW_BAD_PARAM;
    }

    if (ARL_FIRST_ENTRY != hwop)
    {
        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC2, AT_STATUS, 0xf, reg[2]);
    }

    _isisc_fdb_fill_addr(entry->addr, &reg[0], &reg[1]);

    rv = _isisc_atu_down_to_hw(dev_id, reg);
    SW_RTN_ON_ERROR(rv);

    rv = _isisc_fdb_commit(dev_id, hwop);
    SW_RTN_ON_ERROR(rv);

    rv = _isisc_atu_up_to_sw(dev_id, reg);
    SW_RTN_ON_ERROR(rv);

    _isisc_atu_hw_to_sw(reg, entry);

    SW_GET_FIELD_BY_REG(ADDR_TABLE_FUNC2, AT_STATUS, status, reg[2]);
    if ((A_TRUE == _isisc_fdb_is_zeroaddr(entry->addr))
            && (0 == status))
    {
        if (ARL_NEXT_ENTRY == hwop)
        {
            return SW_NO_MORE;
        }
        else
        {
            return SW_NOT_FOUND;
        }
    }
    else
    {
        return SW_OK;
    }

    return SW_OK;
}

#ifndef IN_FDB_MINI
static sw_error_t
_isisc_fdb_add(a_uint32_t dev_id, const fal_fdb_entry_t * entry)
{
    sw_error_t rv;
    a_uint32_t reg[4] = { 0 };

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isisc_atu_sw_to_hw(dev_id, entry, reg);
    SW_RTN_ON_ERROR(rv);

    rv = _isisc_atu_down_to_hw(dev_id, reg);
    SW_RTN_ON_ERROR(rv);

    rv = _isisc_fdb_commit(dev_id, ARL_LOAD_ENTRY);
    return rv;
}
#endif

static sw_error_t
_isisc_fdb_del_all(a_uint32_t dev_id, a_uint32_t flag)
{
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    if (FAL_FDB_DEL_STATIC & flag)
    {
        rv = _isisc_fdb_commit(dev_id, ARL_FLUSH_ALL);
    }
    else
    {
        rv = _isisc_fdb_commit(dev_id, ARL_FLUSH_ALL_UNLOCK);
    }

    return rv;
}
#ifndef IN_FDB_MINI
static sw_error_t
_isisc_fdb_del_by_port(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t flag)
{
    sw_error_t rv;
    a_uint32_t reg = 0;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_FALSE == hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC3, AT_PORT_NUM, port_id, reg);

    HSL_REG_ENTRY_SET(rv, dev_id, ADDR_TABLE_FUNC3, 0, (a_uint8_t *) (&reg),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (FAL_FDB_DEL_STATIC & flag)
    {
        rv = _isisc_fdb_commit(dev_id, ARL_FLUSH_PORT_AND_STATIC);
    }
    else
    {
        rv = _isisc_fdb_commit(dev_id, ARL_FLUSH_PORT_NO_STATIC);
    }

    return rv;
}

static sw_error_t
_isisc_fdb_del_by_mac(a_uint32_t dev_id, const fal_fdb_entry_t * entry)
{
    sw_error_t rv;
    a_uint32_t reg0 = 0, reg1 = 0, reg2 = 0;

    HSL_DEV_ID_CHECK(dev_id);

    _isisc_fdb_fill_addr(entry->addr, &reg0, &reg1);

    if (FAL_SVL_FID == entry->fid)
    {
        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC2, AT_VID, 0, reg2);
        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC1, AT_SVL_EN, 1, reg1);
    }
    else if (ISISC_MAX_FID >= entry->fid)
    {
        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC2, AT_VID, (entry->fid), reg2);
        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC1, AT_SVL_EN, 0, reg1);
    }
    else
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_ENTRY_SET(rv, dev_id, ADDR_TABLE_FUNC2, 0, (a_uint8_t *) (&reg2),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    HSL_REG_ENTRY_SET(rv, dev_id, ADDR_TABLE_FUNC1, 0, (a_uint8_t *) (&reg1),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    HSL_REG_ENTRY_SET(rv, dev_id, ADDR_TABLE_FUNC0, 0, (a_uint8_t *) (&reg0),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    rv = _isisc_fdb_commit(dev_id, ARL_PURGE_ENTRY);
    return rv;
}

static sw_error_t
_isisc_fdb_find(a_uint32_t dev_id, fal_fdb_entry_t * entry)
{
    sw_error_t rv;
    fal_fdb_op_t option;

    aos_mem_zero(&option, sizeof (fal_fdb_op_t));
    rv = _isisc_fdb_get(dev_id, &option, entry, ARL_FIND_ENTRY);
    return rv;
}
#endif
static sw_error_t
_isisc_fdb_extend_next(a_uint32_t dev_id, fal_fdb_op_t * option,
                      fal_fdb_entry_t * entry)
{
    sw_error_t rv;

    rv = _isisc_fdb_get(dev_id, option, entry, ARL_NEXT_ENTRY);
    return rv;
}

static sw_error_t
_isisc_fdb_extend_first(a_uint32_t dev_id, fal_fdb_op_t * option,
                       fal_fdb_entry_t * entry)
{
    sw_error_t rv;

    rv = _isisc_fdb_get(dev_id, option, entry, ARL_FIRST_ENTRY);
    return rv;
}
#ifndef IN_FDB_MINI
static sw_error_t
_isisc_fdb_transfer(a_uint32_t dev_id, fal_port_t old_port, fal_port_t new_port,
                   a_uint32_t fid, fal_fdb_op_t * option)
{
    sw_error_t rv;
    a_uint32_t reg[4] = { 0 };

    if (A_TRUE == option->port_en)
    {
        return SW_NOT_SUPPORTED;
    }

    if (A_TRUE == option->fid_en)
    {
        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC3, AT_VID_EN, 1, reg[3]);
    }

    if (A_TRUE == option->multicast_en)
    {
        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC3, AT_MULTI_EN, 1, reg[3]);
    }

    if (FAL_SVL_FID == fid)
    {
        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC2, AT_VID, 0, reg[2]);
        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC1, AT_SVL_EN, 1, reg[1]);
    }
    else if (ISISC_MAX_FID >= fid)
    {
        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC2, AT_VID, fid, reg[2]);
        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC1, AT_SVL_EN, 0, reg[1]);
    }
    else
    {
        return SW_BAD_PARAM;
    }

    SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC3, AT_PORT_NUM, old_port, reg[3]);
    SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC3, NEW_PORT_NUM, new_port, reg[3]);

    rv = _isisc_atu_down_to_hw(dev_id, reg);
    SW_RTN_ON_ERROR(rv);

    rv = _isisc_fdb_commit(dev_id, ARL_TRANSFER_ENTRY);
    return rv;
}
#endif
static sw_error_t
_isisc_fdb_port_learn_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
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

    HSL_REG_FIELD_SET(rv, dev_id, PORT_LOOKUP_CTL, port_id, LEARN_EN,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    return rv;
}
#ifndef IN_FDB_MINI
static sw_error_t
_isisc_fdb_port_learn_get(a_uint32_t dev_id, fal_port_t port_id,
                         a_bool_t * enable)
{
    a_uint32_t data = 0;
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_GET(rv, dev_id, PORT_LOOKUP_CTL, port_id, LEARN_EN,
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
_isisc_fdb_age_ctrl_set(a_uint32_t dev_id, a_bool_t enable)
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
_isisc_fdb_vlan_ivl_svl_set(a_uint32_t dev_id, fal_fdb_smode smode)
{
    a_uint32_t data;
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    data = smode;

    HSL_REG_FIELD_SET(rv, dev_id, ADDR_TABLE_CTL, 0, ARL_INI_EN,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_isisc_fdb_vlan_ivl_svl_get(a_uint32_t dev_id, fal_fdb_smode* smode)
{
    a_uint32_t data = 0;
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    HSL_REG_FIELD_GET(rv, dev_id, ADDR_TABLE_CTL, 0, ARL_INI_EN,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));

    *smode = data;

    return rv;
}



static sw_error_t
_isisc_fdb_age_ctrl_get(a_uint32_t dev_id, a_bool_t * enable)
{
    a_uint32_t data = 0;
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
_isisc_fdb_age_time_set(a_uint32_t dev_id, a_uint32_t * time)
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
_isisc_fdb_age_time_get(a_uint32_t dev_id, a_uint32_t * time)
{
    a_uint32_t data = 0;
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    HSL_REG_FIELD_GET(rv, dev_id, ADDR_TABLE_CTL, 0, AGE_TIME,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    *time = data * 7;
    return SW_OK;
}

static sw_error_t
_isisc_port_fdb_learn_limit_set(a_uint32_t dev_id, fal_port_t port_id,
                               a_bool_t enable, a_uint32_t cnt)
{
    sw_error_t rv;
    a_uint32_t reg = 0;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_ENTRY_GET(rv, dev_id, PORT_LEARN_LIMIT_CTL, port_id,
                      (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (A_TRUE == enable)
    {
        if (ISISC_MAX_PORT_LEARN_LIMIT_CNT < cnt)
        {
            return SW_BAD_PARAM;
        }
        SW_SET_REG_BY_FIELD(PORT_LEARN_LIMIT_CTL, SA_LEARN_LIMIT_EN, 1, reg);
        SW_SET_REG_BY_FIELD(PORT_LEARN_LIMIT_CTL, SA_LEARN_CNT, cnt, reg);
    }
    else if (A_FALSE == enable)
    {
        SW_SET_REG_BY_FIELD(PORT_LEARN_LIMIT_CTL, SA_LEARN_LIMIT_EN, 0, reg);
        SW_SET_REG_BY_FIELD(PORT_LEARN_LIMIT_CTL, SA_LEARN_CNT, 0, reg);
    }
    else
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_ENTRY_SET(rv, dev_id, PORT_LEARN_LIMIT_CTL, port_id,
                      (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_isisc_port_fdb_learn_limit_get(a_uint32_t dev_id, fal_port_t port_id,
                               a_bool_t * enable, a_uint32_t * cnt)
{
    sw_error_t rv;
    a_uint32_t data, reg = 0;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_ENTRY_GET(rv, dev_id, PORT_LEARN_LIMIT_CTL, port_id,
                      (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    SW_GET_FIELD_BY_REG(PORT_LEARN_LIMIT_CTL, SA_LEARN_LIMIT_EN, data, reg);
    if (data)
    {
        SW_GET_FIELD_BY_REG(PORT_LEARN_LIMIT_CTL, SA_LEARN_CNT, data, reg);
        *enable = A_TRUE;
        *cnt = data;
    }
    else
    {
        *enable = A_FALSE;
        *cnt = 0;
    }

    return SW_OK;
}

static sw_error_t
_isisc_port_fdb_learn_exceed_cmd_set(a_uint32_t dev_id, fal_port_t port_id,
                                    fal_fwd_cmd_t cmd)
{
    sw_error_t rv;
    a_uint32_t data = 0;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    if (FAL_MAC_DROP == cmd)
    {
        data = 1;
    }
    else if (FAL_MAC_RDT_TO_CPU == cmd)
    {
        data = 0;
    }
    else
    {
        return SW_NOT_SUPPORTED;
    }

    HSL_REG_FIELD_SET(rv, dev_id, PORT_LEARN_LIMIT_CTL, port_id,
                      SA_LEARN_LIMIT_DROP_EN, (a_uint8_t *) (&data),
                      sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_isisc_port_fdb_learn_exceed_cmd_get(a_uint32_t dev_id, fal_port_t port_id,
                                    fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;
    a_uint32_t data = 0;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_GET(rv, dev_id, PORT_LEARN_LIMIT_CTL, port_id,
                      SA_LEARN_LIMIT_DROP_EN, (a_uint8_t *) (&data),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (data)
    {
        *cmd = FAL_MAC_DROP;
    }
    else
    {
        *cmd = FAL_MAC_RDT_TO_CPU;
    }

    return SW_OK;
}

static sw_error_t
_isisc_fdb_learn_limit_set(a_uint32_t dev_id, a_bool_t enable, a_uint32_t cnt)
{
    sw_error_t rv;
    a_uint32_t reg = 0;

    HSL_REG_ENTRY_GET(rv, dev_id, GLOBAL_LEARN_LIMIT_CTL, 0,
                      (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (A_TRUE == enable)
    {
        if (ISISC_MAX_LEARN_LIMIT_CNT < cnt)
        {
            return SW_BAD_PARAM;
        }
        SW_SET_REG_BY_FIELD(GLOBAL_LEARN_LIMIT_CTL, GOL_SA_LEARN_LIMIT_EN, 1,
                            reg);
        SW_SET_REG_BY_FIELD(GLOBAL_LEARN_LIMIT_CTL, GOL_SA_LEARN_CNT, cnt, reg);
    }
    else if (A_FALSE == enable)
    {
        SW_SET_REG_BY_FIELD(GLOBAL_LEARN_LIMIT_CTL, GOL_SA_LEARN_LIMIT_EN, 0,
                            reg);
        SW_SET_REG_BY_FIELD(GLOBAL_LEARN_LIMIT_CTL, GOL_SA_LEARN_CNT, 0, reg);
    }
    else
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_ENTRY_SET(rv, dev_id, GLOBAL_LEARN_LIMIT_CTL, 0,
                      (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_isisc_fdb_learn_limit_get(a_uint32_t dev_id, a_bool_t * enable,
                          a_uint32_t * cnt)
{
    sw_error_t rv;
    a_uint32_t data, reg = 0;

    HSL_REG_ENTRY_GET(rv, dev_id, GLOBAL_LEARN_LIMIT_CTL, 0,
                      (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    SW_GET_FIELD_BY_REG(GLOBAL_LEARN_LIMIT_CTL, GOL_SA_LEARN_LIMIT_EN, data,
                        reg);
    if (data)
    {
        SW_GET_FIELD_BY_REG(GLOBAL_LEARN_LIMIT_CTL, GOL_SA_LEARN_CNT, data,
                            reg);
        *enable = A_TRUE;
        *cnt = data;
    }
    else
    {
        *enable = A_FALSE;
        *cnt = 0;
    }

    return SW_OK;
}

static sw_error_t
_isisc_fdb_learn_exceed_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd)
{
    sw_error_t rv;
    a_uint32_t data = 0;

    HSL_DEV_ID_CHECK(dev_id);

    if (FAL_MAC_DROP == cmd)
    {
        data = 1;
    }
    else if (FAL_MAC_RDT_TO_CPU == cmd)
    {
        data = 0;
    }
    else
    {
        return SW_NOT_SUPPORTED;
    }

    HSL_REG_FIELD_SET(rv, dev_id, GLOBAL_LEARN_LIMIT_CTL, 0,
                      GOL_SA_LEARN_LIMIT_DROP_EN, (a_uint8_t *) (&data),
                      sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_isisc_fdb_learn_exceed_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;
    a_uint32_t data = 0;

    HSL_DEV_ID_CHECK(dev_id);

    HSL_REG_FIELD_GET(rv, dev_id, GLOBAL_LEARN_LIMIT_CTL, 0,
                      GOL_SA_LEARN_LIMIT_DROP_EN, (a_uint8_t *) (&data),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (data)
    {
        *cmd = FAL_MAC_DROP;
    }
    else
    {
        *cmd = FAL_MAC_RDT_TO_CPU;
    }

    return SW_OK;
}

#define ISISC_RESV_ADDR_NUM  32
#define RESV_ADDR_TBL0_ADDR 0x3c000
#define RESV_ADDR_TBL1_ADDR 0x3c004
#define RESV_ADDR_TBL2_ADDR 0x3c008

static void
_isisc_resv_addr_parse(const a_uint32_t reg[], fal_mac_addr_t * addr)
{
    a_uint32_t i;

    for (i = 2; i < 6; i++)
    {
        addr->uc[i] = (reg[0] >> ((5 - i) << 3)) & 0xff;
    }

    for (i = 0; i < 2; i++)
    {
        addr->uc[i] = (reg[1] >> ((1 - i) << 3)) & 0xff;
    }
}

static sw_error_t
_isisc_resv_atu_sw_to_hw(a_uint32_t dev_id, fal_fdb_entry_t * entry,
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
    SW_SET_REG_BY_FIELD(RESV_ADDR_TBL1, RESV_DES_PORT, port, reg[1]);

    if (FAL_MAC_CPY_TO_CPU == entry->dacmd)
    {
        SW_SET_REG_BY_FIELD(RESV_ADDR_TBL1, RESV_COPY_TO_CPU, 1, reg[1]);
    }
    else if (FAL_MAC_RDT_TO_CPU == entry->dacmd)
    {
        SW_SET_REG_BY_FIELD(RESV_ADDR_TBL1, RESV_REDRCT_TO_CPU, 1, reg[1]);
    }
    else if (FAL_MAC_FRWRD != entry->dacmd)
    {
        return SW_NOT_SUPPORTED;
    }

    if (FAL_MAC_FRWRD != entry->sacmd)
    {
        return SW_NOT_SUPPORTED;
    }

    if (A_TRUE == entry->leaky_en)
    {
        SW_SET_REG_BY_FIELD(RESV_ADDR_TBL1, RESV_LEAKY_EN, 1, reg[1]);
    }
    else
    {
        SW_SET_REG_BY_FIELD(RESV_ADDR_TBL1, RESV_LEAKY_EN, 0, reg[1]);
    }

    if (A_TRUE != entry->static_en)
    {
        return SW_NOT_SUPPORTED;
    }
    SW_SET_REG_BY_FIELD(RESV_ADDR_TBL2, RESV_STATUS, 1, reg[2]);

    if (A_TRUE == entry->mirror_en)
    {
        SW_SET_REG_BY_FIELD(RESV_ADDR_TBL1, RESV_MIRROR_EN, 1, reg[1]);
    }

    if (A_TRUE == entry->cross_pt_state)
    {
        SW_SET_REG_BY_FIELD(RESV_ADDR_TBL1, RESV_CROSS_PT, 1, reg[1]);
    }

    if (A_TRUE == entry->da_pri_en)
    {
        SW_SET_REG_BY_FIELD(RESV_ADDR_TBL1, RESV_PRI_EN, 1, reg[1]);
        SW_SET_REG_BY_FIELD(RESV_ADDR_TBL1, RESV_PRI, (entry->da_queue & 0x7),
                            reg[1]);
    }

    _isisc_fdb_fill_addr(entry->addr, &reg[0], &reg[1]);
    return SW_OK;
}

static void
_isisc_resv_atu_hw_to_sw(const a_uint32_t reg[], fal_fdb_entry_t * entry)
{
    a_uint32_t data;

    aos_mem_zero(entry, sizeof (fal_fdb_entry_t));

    entry->fid = FAL_SVL_FID;

    entry->dacmd = FAL_MAC_FRWRD;
    SW_GET_FIELD_BY_REG(RESV_ADDR_TBL1, RESV_COPY_TO_CPU, data, reg[1]);
    if (1 == data)
    {
        entry->dacmd = FAL_MAC_CPY_TO_CPU;
    }

    SW_GET_FIELD_BY_REG(RESV_ADDR_TBL1, RESV_REDRCT_TO_CPU, data, reg[1]);
    if (1 == data)
    {
        entry->dacmd = FAL_MAC_RDT_TO_CPU;
    }

    entry->sacmd = FAL_MAC_FRWRD;

    entry->leaky_en = A_FALSE;
    SW_GET_FIELD_BY_REG(RESV_ADDR_TBL1, RESV_LEAKY_EN, data, reg[1]);
    if (1 == data)
    {
        entry->leaky_en = A_TRUE;
    }

    entry->static_en = A_TRUE;

    entry->mirror_en = A_FALSE;
    SW_GET_FIELD_BY_REG(RESV_ADDR_TBL1, RESV_MIRROR_EN, data, reg[1]);
    if (1 == data)
    {
        entry->mirror_en = A_TRUE;
    }

    entry->da_pri_en = A_FALSE;
    SW_GET_FIELD_BY_REG(RESV_ADDR_TBL1, RESV_PRI_EN, data, reg[1]);
    if (1 == data)
    {
        SW_GET_FIELD_BY_REG(RESV_ADDR_TBL1, RESV_PRI, data, reg[1]);
        entry->da_pri_en = A_TRUE;
        entry->da_queue = data & 0x7;
    }

    entry->cross_pt_state = A_FALSE;
    SW_GET_FIELD_BY_REG(RESV_ADDR_TBL1, RESV_CROSS_PT, data, reg[1]);
    if (1 == data)
    {
        entry->cross_pt_state = A_TRUE;
    }

    SW_GET_FIELD_BY_REG(RESV_ADDR_TBL1, RESV_DES_PORT, data, reg[1]);
    entry->portmap_en = A_TRUE;
    entry->port.map = data;

    _isisc_resv_addr_parse(reg, &(entry->addr));
    return;
}

static sw_error_t
_isisc_fdb_resv_commit(a_uint32_t dev_id, fal_fdb_entry_t * entry, a_uint32_t op,
                      a_uint32_t * empty)
{
    a_uint32_t index, addr, data, tbl[3] = { 0 };
    sw_error_t rv;
    fal_mac_addr_t mac_tmp;

    *empty = ISISC_RESV_ADDR_NUM;
    for (index = 0; index < ISISC_RESV_ADDR_NUM; index++)
    {
        addr = RESV_ADDR_TBL2_ADDR + (index << 4);
        HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr, sizeof (a_uint32_t),
                              (a_uint8_t *) (&(tbl[2])), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        SW_GET_FIELD_BY_REG(RESV_ADDR_TBL2, RESV_STATUS, data, tbl[2]);
        if (data)
        {
            addr = RESV_ADDR_TBL0_ADDR + (index << 4);
            HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr, sizeof (a_uint32_t),
                                  (a_uint8_t *) (&(tbl[0])),
                                  sizeof (a_uint32_t));
            SW_RTN_ON_ERROR(rv);

            addr = RESV_ADDR_TBL1_ADDR + (index << 4);
            HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr, sizeof (a_uint32_t),
                                  (a_uint8_t *) (&(tbl[1])),
                                  sizeof (a_uint32_t));
            SW_RTN_ON_ERROR(rv);

            _isisc_resv_addr_parse(tbl, &mac_tmp);
            if (!aos_mem_cmp
                    ((void *) &(entry->addr), (void *) &mac_tmp,
                     sizeof (fal_mac_addr_t)))
            {
                if (ARL_PURGE_ENTRY == op)
                {
                    addr = RESV_ADDR_TBL2_ADDR + (index << 4);
                    tbl[2] = 0;
                    HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr, sizeof (a_uint32_t),
                                          (a_uint8_t *) (&(tbl[2])),
                                          sizeof (a_uint32_t));
                    return rv;
                }
                else if (ARL_LOAD_ENTRY == op)
                {
                    return SW_ALREADY_EXIST;
                }
                else if (ARL_FIND_ENTRY == op)
                {
                    _isisc_resv_atu_hw_to_sw(tbl, entry);
                    return SW_OK;
                }
            }
        }
        else
        {
            *empty = index;
        }
    }

    return SW_NOT_FOUND;
}

static sw_error_t
_isisc_fdb_resv_add(a_uint32_t dev_id, fal_fdb_entry_t * entry)
{
    sw_error_t rv;
    a_uint32_t i, empty, addr, tbl[3] = { 0 };

    rv = _isisc_resv_atu_sw_to_hw(dev_id, entry, tbl);
    SW_RTN_ON_ERROR(rv);

    rv = _isisc_fdb_resv_commit(dev_id, entry, ARL_LOAD_ENTRY, &empty);
    if (SW_ALREADY_EXIST == rv)
    {
        return rv;
    }

    if (ISISC_RESV_ADDR_NUM == empty)
    {
        return SW_NO_RESOURCE;
    }

    for (i = 0; i < 3; i++)
    {
        addr = RESV_ADDR_TBL0_ADDR + (empty << 4) + (i << 2);
        HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr, sizeof (a_uint32_t),
                              (a_uint8_t *) (&(tbl[i])), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }

    return SW_OK;
}

static sw_error_t
_isisc_fdb_resv_del(a_uint32_t dev_id, fal_fdb_entry_t * entry)
{
    sw_error_t rv;
    a_uint32_t empty;

    rv = _isisc_fdb_resv_commit(dev_id, entry, ARL_PURGE_ENTRY, &empty);
    return rv;
}

static sw_error_t
_isisc_fdb_resv_find(a_uint32_t dev_id, fal_fdb_entry_t * entry)
{
    sw_error_t rv;
    a_uint32_t empty;

    rv = _isisc_fdb_resv_commit(dev_id, entry, ARL_FIND_ENTRY, &empty);
    return rv;
}

static sw_error_t
_isisc_fdb_resv_iterate(a_uint32_t dev_id, a_uint32_t * iterator,
                       fal_fdb_entry_t * entry)
{
    a_uint32_t index, addr, data, tbl[3] = { 0 };
    sw_error_t rv;

    if ((NULL == iterator) || (NULL == entry))
    {
        return SW_BAD_PTR;
    }

    if (ISISC_RESV_ADDR_NUM < *iterator)
    {
        return SW_BAD_PARAM;
    }

    for (index = *iterator; index < ISISC_RESV_ADDR_NUM; index++)
    {
        addr = RESV_ADDR_TBL2_ADDR + (index << 4);
        HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr, sizeof (a_uint32_t),
                              (a_uint8_t *) (&(tbl[2])), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        SW_GET_FIELD_BY_REG(RESV_ADDR_TBL2, RESV_STATUS, data, tbl[2]);
        if (data)
        {
            addr = RESV_ADDR_TBL0_ADDR + (index << 4);
            HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr, sizeof (a_uint32_t),
                                  (a_uint8_t *) (&(tbl[0])),
                                  sizeof (a_uint32_t));
            SW_RTN_ON_ERROR(rv);

            addr = RESV_ADDR_TBL1_ADDR + (index << 4);
            HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr, sizeof (a_uint32_t),
                                  (a_uint8_t *) (&(tbl[1])),
                                  sizeof (a_uint32_t));
            SW_RTN_ON_ERROR(rv);

            _isisc_resv_atu_hw_to_sw(tbl, entry);
            break;
        }
    }

    if (ISISC_RESV_ADDR_NUM == index)
    {
        return SW_NO_MORE;
    }

    *iterator = index + 1;
    return SW_OK;
}

static sw_error_t
_isisc_fdb_port_learn_static_set(a_uint32_t dev_id, fal_port_t port_id,
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
        data = 0xf;
    }
    else if (A_FALSE == enable)
    {
        data = 0x7;
    }
    else
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_SET(rv, dev_id, PORT_LEARN_LIMIT_CTL, port_id,
                      SA_LEARN_STATUS, (a_uint8_t *) (&data),
                      sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_isisc_fdb_port_learn_static_get(a_uint32_t dev_id, fal_port_t port_id,
                                a_bool_t * enable)
{
    a_uint32_t data = 0;
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_GET(rv, dev_id, PORT_LEARN_LIMIT_CTL, port_id,
                      SA_LEARN_STATUS, (a_uint8_t *) (&data),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (0xf == data)
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
_isisc_fdb_port_update(a_uint32_t dev_id, a_uint32_t fid, fal_mac_addr_t * addr, fal_port_t port_id, a_uint32_t op)
{
    sw_error_t      rv;
    fal_fdb_entry_t entry;
    fal_fdb_op_t    option;
    a_uint32_t      reg, port;

    HSL_DEV_ID_CHECK(dev_id);

    if (FAL_SVL_FID < fid)
    {
        return SW_BAD_PARAM;
    }

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    aos_mem_zero(&option, sizeof(fal_fdb_op_t));
    aos_mem_copy(&(entry.addr), addr, sizeof(fal_mac_addr_t));
    entry.fid = fid & 0xffff;
    rv = _isisc_fdb_get(dev_id, &option, &entry, ARL_FIND_ENTRY);
    SW_RTN_ON_ERROR(rv);

    HSL_REG_ENTRY_GET(rv, dev_id, ADDR_TABLE_FUNC1, 0,
                      (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    SW_GET_FIELD_BY_REG(ADDR_TABLE_FUNC1, DES_PORT, port, reg);
    if (op)
    {
        port |= (0x1 << port_id);
    }
    else
    {
        port &= (~(0x1 << port_id));
    }
    SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC1, DES_PORT, port, reg);
    HSL_REG_ENTRY_SET(rv, dev_id, ADDR_TABLE_FUNC1, 0,
                      (a_uint8_t *) (&reg), sizeof (a_uint32_t));

    rv = _isisc_fdb_commit(dev_id, ARL_LOAD_ENTRY);
    return rv;
}

sw_error_t
inter_isisc_fdb_flush(a_uint32_t dev_id, a_uint32_t flag)
{
    if (FAL_FDB_DEL_STATIC & flag)
    {
        return _isisc_fdb_commit(dev_id, ARL_FLUSH_ALL);
    }
    else
    {
        return _isisc_fdb_commit(dev_id, ARL_FLUSH_ALL_UNLOCK);
    }
}

/**
 * @brief Add a Fdb entry
 * @param[in] dev_id device id
 * @param[in] entry fdb entry
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_fdb_add(a_uint32_t dev_id, const fal_fdb_entry_t * entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_fdb_add(dev_id, entry);
    HSL_API_UNLOCK;
    return rv;
}
#endif
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
isisc_fdb_del_all(a_uint32_t dev_id, a_uint32_t flag)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_fdb_del_all(dev_id, flag);
    HSL_API_UNLOCK;
    return rv;
}
#ifndef IN_FDB_MINI
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
isisc_fdb_del_by_port(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t flag)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_fdb_del_by_port(dev_id, port_id, flag);
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
isisc_fdb_del_by_mac(a_uint32_t dev_id, const fal_fdb_entry_t * entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_fdb_del_by_mac(dev_id, entry);
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
isisc_fdb_find(a_uint32_t dev_id, fal_fdb_entry_t * entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_fdb_find(dev_id, entry);
    HSL_API_UNLOCK;
    return rv;
}
#endif
/**
 * @brief Get next Fdb entry from a particular device
 * @param[in] dev_id device id
 * @param[in] option next operation options
 * @param[out] entry fdb entry
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_fdb_extend_next(a_uint32_t dev_id, fal_fdb_op_t * option,
                     fal_fdb_entry_t * entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_fdb_extend_next(dev_id, option, entry);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get first Fdb entry from a particular device
 * @param[in] dev_id device id
 * @param[in] option first operation options
 * @param[out] entry fdb entry
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_fdb_extend_first(a_uint32_t dev_id, fal_fdb_op_t * option,
                      fal_fdb_entry_t * entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_fdb_extend_first(dev_id, option, entry);
    HSL_API_UNLOCK;
    return rv;
}
#ifndef IN_FDB_MINI
/**
 * @brief Transfer fdb entries port information on a particular device.
 * @param[in] dev_id device id
 * @param[in] old_port source port id
 * @param[in] new_port destination port id
 * @param[in] fid filter database id
 * @param[in] option transfer operation options
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_fdb_transfer(a_uint32_t dev_id, fal_port_t old_port, fal_port_t new_port,
                  a_uint32_t fid, fal_fdb_op_t * option)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_fdb_transfer(dev_id, old_port, new_port, fid, option);
    HSL_API_UNLOCK;
    return rv;
}
#endif
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
isisc_fdb_port_learn_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_fdb_port_learn_set(dev_id, port_id, enable);
    HSL_API_UNLOCK;
    return rv;
}
#ifndef IN_FDB_MINI
/**
 * @brief Get dynamic address learning status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_fdb_port_learn_get(a_uint32_t dev_id, fal_port_t port_id,
                        a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_fdb_port_learn_get(dev_id, port_id, enable);
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
isisc_fdb_age_ctrl_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_fdb_age_ctrl_set(dev_id, enable);
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
isisc_fdb_age_ctrl_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_fdb_age_ctrl_get(dev_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief set arl search mode as ivl or svl when vlan invalid.
 * @param[in] dev_id device id
 * @param[in] smode INVALID_VLAN_IVL or INVALID_VLAN_SVL
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_fdb_vlan_ivl_svl_set(a_uint32_t dev_id, fal_fdb_smode smode)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_fdb_vlan_ivl_svl_set(dev_id, smode);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief get arl search mode when vlan invalid.
 * @param[in] dev_id device id
 * @param[out] smode INVALID_VLAN_IVL or INVALID_VLAN_SVL
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_fdb_vlan_ivl_svl_get(a_uint32_t dev_id, fal_fdb_smode* smode)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_fdb_vlan_ivl_svl_get(dev_id, smode);
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
isisc_fdb_age_time_set(a_uint32_t dev_id, a_uint32_t * time)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_fdb_age_time_set(dev_id, time);
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
isisc_fdb_age_time_get(a_uint32_t dev_id, a_uint32_t * time)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_fdb_age_time_get(dev_id, time);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set dynamic address learning count limit on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @param[in] cnt limit count
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_port_fdb_learn_limit_set(a_uint32_t dev_id, fal_port_t port_id,
                              a_bool_t enable, a_uint32_t cnt)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_port_fdb_learn_limit_set(dev_id, port_id, enable, cnt);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get dynamic address learning count limit on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @param[out] cnt limit count
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_port_fdb_learn_limit_get(a_uint32_t dev_id, fal_port_t port_id,
                              a_bool_t * enable, a_uint32_t * cnt)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_port_fdb_learn_limit_get(dev_id, port_id, enable, cnt);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set dynamic address learning count exceed command on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] cmd forwarding command
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_port_fdb_learn_exceed_cmd_set(a_uint32_t dev_id, fal_port_t port_id,
                                   fal_fwd_cmd_t cmd)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_port_fdb_learn_exceed_cmd_set(dev_id, port_id, cmd);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get dynamic address learning count exceed command on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] cmd forwarding command
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_port_fdb_learn_exceed_cmd_get(a_uint32_t dev_id, fal_port_t port_id,
                                   fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_port_fdb_learn_exceed_cmd_get(dev_id, port_id, cmd);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set dynamic address learning count limit on a particular device.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @param[in] cnt limit count
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_fdb_learn_limit_set(a_uint32_t dev_id, a_bool_t enable, a_uint32_t cnt)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_fdb_learn_limit_set(dev_id, enable, cnt);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get dynamic address learning count limit on a particular device.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @param[in] cnt limit count
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_fdb_learn_limit_get(a_uint32_t dev_id, a_bool_t * enable, a_uint32_t * cnt)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_fdb_learn_limit_get(dev_id, enable, cnt);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set dynamic address learning count exceed command on a particular device.
 * @param[in] dev_id device id
 * @param[in] cmd forwarding command
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_fdb_learn_exceed_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_fdb_learn_exceed_cmd_set(dev_id, cmd);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get dynamic address learning count exceed command on a particular device.
 * @param[in] dev_id device id
 * @param[out] cmd forwarding command
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_fdb_learn_exceed_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_fdb_learn_exceed_cmd_get(dev_id, cmd);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Add a particular reserve Fdb entry
 * @param[in] dev_id device id
 * @param[in] entry reserve fdb entry
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_fdb_resv_add(a_uint32_t dev_id, fal_fdb_entry_t * entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_fdb_resv_add(dev_id, entry);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Delete a particular reserve Fdb entry through mac address
 * @param[in] dev_id device id
 * @param[in] entry reserve fdb entry
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_fdb_resv_del(a_uint32_t dev_id, fal_fdb_entry_t * entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_fdb_resv_del(dev_id, entry);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Find a particular reserve Fdb entry through mac address
 * @param[in] dev_id device id
 * @param[in] entry reserve fdb entry
 * @param[out] entry reserve fdb entry
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_fdb_resv_find(a_uint32_t dev_id, fal_fdb_entry_t * entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_fdb_resv_find(dev_id, entry);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Iterate all reserve fdb entries on a particular device.
 * @param[in] dev_id device id
 * @param[in] iterator reserve fdb entry index if it's zero means get the first entry
 * @param[out] iterator next valid fdb entry index
 * @param[out] entry reserve fdb entry
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_fdb_resv_iterate(a_uint32_t dev_id, a_uint32_t * iterator,
                      fal_fdb_entry_t * entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_fdb_resv_iterate(dev_id, iterator, entry);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set the static status of fdb entries which learned by hardware on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_fdb_port_learn_static_set(a_uint32_t dev_id, fal_port_t port_id,
                               a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_fdb_port_learn_static_set(dev_id, port_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get the static status of fdb entries which learned by hardware on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_fdb_port_learn_static_get(a_uint32_t dev_id, fal_port_t port_id,
                               a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_fdb_port_learn_static_get(dev_id, port_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Add a port to an exsiting entry
 * @param[in] dev_id device id
 * @param[in] fid filtering database id
 * @param[in] addr MAC address
 * @param[in] port_id port id
 * @return SW_OK or error code, If entry not exist will return error.
 */
HSL_LOCAL sw_error_t
isisc_fdb_port_add(a_uint32_t dev_id, a_uint32_t fid, fal_mac_addr_t * addr, fal_port_t port_id)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_fdb_port_update(dev_id, fid, addr, port_id, 1);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Delete a port from an exsiting entry
 * @param[in] dev_id device id
 * @param[in] fid filtering database id
 * @param[in] addr MAC address
 * @param[in] port_id port id
 * @return SW_OK or error code, If entry not exist will return error.
 */
HSL_LOCAL sw_error_t
isisc_fdb_port_del(a_uint32_t dev_id, a_uint32_t fid, fal_mac_addr_t * addr, fal_port_t port_id)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_fdb_port_update(dev_id, fid, addr, port_id, 0);
    HSL_API_UNLOCK;
    return rv;
}
#endif
sw_error_t
isisc_fdb_init(a_uint32_t dev_id)
{
    HSL_DEV_ID_CHECK(dev_id);

#ifndef HSL_STANDALONG
    {
        hsl_api_t *p_api;

        SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));
#ifndef IN_FDB_MINI
        p_api->fdb_add = isisc_fdb_add;
#endif
        p_api->fdb_del_all = isisc_fdb_del_all;
#ifndef IN_FDB_MINI
        p_api->fdb_del_by_port = isisc_fdb_del_by_port;
        p_api->fdb_del_by_mac = isisc_fdb_del_by_mac;
        p_api->fdb_find = isisc_fdb_find;
#endif
        p_api->port_learn_set = isisc_fdb_port_learn_set;
#ifndef IN_FDB_MINI
        p_api->port_learn_get = isisc_fdb_port_learn_get;
        p_api->age_ctrl_set = isisc_fdb_age_ctrl_set;
        p_api->age_ctrl_get = isisc_fdb_age_ctrl_get;
        p_api->vlan_ivl_svl_set = isisc_fdb_vlan_ivl_svl_set;
        p_api->vlan_ivl_svl_get = isisc_fdb_vlan_ivl_svl_get;
        p_api->age_time_set = isisc_fdb_age_time_set;
        p_api->age_time_get = isisc_fdb_age_time_get;
#endif
        p_api->fdb_extend_next  = isisc_fdb_extend_next;
        p_api->fdb_extend_first = isisc_fdb_extend_first;
#ifndef IN_FDB_MINI
        p_api->fdb_transfer     = isisc_fdb_transfer;
        p_api->port_fdb_learn_limit_set = isisc_port_fdb_learn_limit_set;
        p_api->port_fdb_learn_limit_get = isisc_port_fdb_learn_limit_get;
        p_api->port_fdb_learn_exceed_cmd_set = isisc_port_fdb_learn_exceed_cmd_set;
        p_api->port_fdb_learn_exceed_cmd_get = isisc_port_fdb_learn_exceed_cmd_get;
        p_api->fdb_learn_limit_set = isisc_fdb_learn_limit_set;
        p_api->fdb_learn_limit_get = isisc_fdb_learn_limit_get;
        p_api->fdb_learn_exceed_cmd_set = isisc_fdb_learn_exceed_cmd_set;
        p_api->fdb_learn_exceed_cmd_get = isisc_fdb_learn_exceed_cmd_get;
        p_api->fdb_resv_add     = isisc_fdb_resv_add;
        p_api->fdb_resv_del     = isisc_fdb_resv_del;
        p_api->fdb_resv_find    = isisc_fdb_resv_find;
        p_api->fdb_resv_iterate = isisc_fdb_resv_iterate;
        p_api->fdb_port_learn_static_set = isisc_fdb_port_learn_static_set;
        p_api->fdb_port_learn_static_get = isisc_fdb_port_learn_static_get;
        p_api->fdb_port_add = isisc_fdb_port_add;
        p_api->fdb_port_del = isisc_fdb_port_del;
#endif
    }
#endif

    return SW_OK;
}

/**
 * @}
 */

