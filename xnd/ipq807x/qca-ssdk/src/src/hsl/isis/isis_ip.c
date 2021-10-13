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
 * @defgroup isis_ip ISIS_IP
 * @{
 */

#include "sw.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_port_prop.h"
#include "isis_ip.h"
#include "isis_reg.h"

#define ISIS_HOST_ENTRY_DATA0_ADDR              0x0e48
#define ISIS_HOST_ENTRY_DATA1_ADDR              0x0e4c
#define ISIS_HOST_ENTRY_DATA2_ADDR              0x0e50
#define ISIS_HOST_ENTRY_DATA3_ADDR              0x0e54
#define ISIS_HOST_ENTRY_DATA4_ADDR              0x0e58

#define ISIS_HOST_ENTRY_FLUSH                   1
#define ISIS_HOST_ENTRY_ADD                     2
#define ISIS_HOST_ENTRY_DEL                     3
#define ISIS_HOST_ENTRY_NEXT                    4
#define ISIS_HOST_ENTRY_SEARCH                  5

#define ISIS_ENTRY_ARP                          3

#define ISIS_INTF_MAC_ADDR_NUM                  8
#define ISIS_INTF_MAC_TBL0_ADDR                 0x5a900
#define ISIS_INTF_MAC_TBL1_ADDR                 0x5a904
#define ISIS_INTF_MAC_TBL2_ADDR                 0x5a908
#define ISIS_INTF_MAC_EDIT0_ADDR                0x02000
#define ISIS_INTF_MAC_EDIT1_ADDR                0x02004
#define ISIS_INTF_MAC_EDIT2_ADDR                0x02008

#define ISIS_IP6_BASE_ADDR                      0x0470

#define ISIS_HOST_ENTRY_NUM                     128

#define ISIS_IP_COUTER_ADDR                     0x2b000

static a_uint32_t isis_mac_snap[SW_MAX_NR_DEV] = { 0 };
static fal_intf_mac_entry_t isis_intf_snap[SW_MAX_NR_DEV][ISIS_INTF_MAC_ADDR_NUM];

static void
_isis_ip_pt_learn_save(a_uint32_t dev_id, a_uint32_t * status)
{
    sw_error_t rv;
    a_uint32_t data = 0;

    HSL_REG_ENTRY_GET(rv, dev_id, ROUTER_PTCTRL2, 0,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    if (SW_OK != rv)
    {
        return;
    }

    *status = (data & 0x7f7f);

    data &= 0xffff8080;
    HSL_REG_ENTRY_SET(rv, dev_id, ROUTER_PTCTRL2, 0,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    return;
}

static void
_isis_ip_pt_learn_restore(a_uint32_t dev_id, a_uint32_t status)
{
    sw_error_t rv;
    a_uint32_t data = 0;

    HSL_REG_ENTRY_GET(rv, dev_id, ROUTER_PTCTRL2, 0,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    if (SW_OK != rv)
    {
        return;
    }

    data &= 0xffff8080;
    data |= (status & 0x7f7f);

    HSL_REG_ENTRY_SET(rv, dev_id, ROUTER_PTCTRL2, 0,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    return;
}

static sw_error_t
_isis_ip_feature_check(a_uint32_t dev_id)
{
    sw_error_t rv;
    a_uint32_t entry = 0;

    HSL_REG_FIELD_GET(rv, dev_id, MASK_CTL, 0, DEVICE_ID,
                      (a_uint8_t *) (&entry), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (S17_DEVICE_ID == entry)
    {
        return SW_OK;
    }
    else
    {
        return SW_NOT_SUPPORTED;
    }
}

static sw_error_t
_isis_ip_counter_get(a_uint32_t dev_id, a_uint32_t cnt_id,
                     a_uint32_t counter[2])
{
    sw_error_t rv;
    a_uint32_t i, addr;

    addr = ISIS_IP_COUTER_ADDR + (cnt_id << 3);
    for (i = 0; i < 2; i++)
    {
        HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr, sizeof (a_uint32_t),
                              (a_uint8_t *) (&(counter[i])),
                              sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        addr += 4;
    }

    return SW_OK;
}

static sw_error_t
_isis_host_entry_commit(a_uint32_t dev_id, a_uint32_t entry_type, a_uint32_t op)
{
    a_uint32_t busy = 1, i = 0x100, entry = 0, j, try_num;
    a_uint32_t learn_status = 0;
    sw_error_t rv;

    while (busy && --i)
    {
        HSL_REG_ENTRY_GET(rv, dev_id, HOST_ENTRY4, 0, (a_uint8_t *) (&entry),
                          sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
        SW_GET_FIELD_BY_REG(HOST_ENTRY4, TBL_BUSY, busy, entry);
        aos_udelay(500);
    }

    if (i == 0)
    {
        printk("%s BUSY\n", __FUNCTION__);
        return SW_BUSY;
    }

    SW_SET_REG_BY_FIELD(HOST_ENTRY4, TBL_BUSY, 1, entry);
    SW_SET_REG_BY_FIELD(HOST_ENTRY4, TBL_SEL, entry_type, entry);
    SW_SET_REG_BY_FIELD(HOST_ENTRY4, ENTRY_FUNC, op, entry);

    HSL_REG_ENTRY_SET(rv, dev_id, HOST_ENTRY4, 0, (a_uint8_t *) (&entry),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    /* hardware requirements, we should disable ARP learn at first */
    /* and maybe we should try several times... */
    _isis_ip_pt_learn_save(dev_id, &learn_status);
    if (learn_status)
    {
        try_num = 10;
    }
    else
    {
        try_num = 1;
    }

    for (j = 0; j < try_num; j++)
    {
        busy = 1;
        i = 0x100;
        while (busy && --i)
        {
            HSL_REG_ENTRY_GET(rv, dev_id, HOST_ENTRY4, 0, (a_uint8_t *) (&entry),
                              sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                _isis_ip_pt_learn_restore(dev_id, learn_status);
                return rv;
            }
            SW_GET_FIELD_BY_REG(HOST_ENTRY4, TBL_BUSY, busy, entry);
            aos_udelay(500);
        }

        if (i == 0)
        {
            _isis_ip_pt_learn_restore(dev_id, learn_status);
            printk("%s BUSY\n", __FUNCTION__);
            return SW_BUSY;
        }

        /* hardware requirement, we should read again... */
        HSL_REG_ENTRY_GET(rv, dev_id, HOST_ENTRY4, 0, (a_uint8_t *) (&entry),
                          sizeof (a_uint32_t));
        if (SW_OK != rv)
        {
            _isis_ip_pt_learn_restore(dev_id, learn_status);
            return rv;
        }

        /* operation success...... */
        SW_GET_FIELD_BY_REG(HOST_ENTRY4, TBL_STAUS, busy, entry);
        if (busy)
        {
            _isis_ip_pt_learn_restore(dev_id, learn_status);
            return SW_OK;
        }
    }

    _isis_ip_pt_learn_restore(dev_id, learn_status);
    if (ISIS_HOST_ENTRY_NEXT == op)
    {
        return SW_NO_MORE;
    }
    else if (ISIS_HOST_ENTRY_SEARCH == op)
    {
        return SW_NOT_FOUND;
    }
    else if (ISIS_HOST_ENTRY_DEL == op)
    {
        return SW_NOT_FOUND;
    }
    else
    {
        return SW_FAIL;
    }
}

static sw_error_t
_isis_ip_intf_sw_to_hw(a_uint32_t dev_id, fal_host_entry_t * entry,
                       a_uint32_t * hw_intf)
{
    sw_error_t rv;
    a_uint32_t addr, lvid, hvid, tbl[3] = {0}, i;
    a_uint32_t sw_intf = entry->intf_id;
    a_uint32_t vid_offset;

    for (i = 0; i < ISIS_INTF_MAC_ADDR_NUM; i++)
    {
        if (isis_mac_snap[dev_id] & (0x1 << i))
        {
            addr = ISIS_INTF_MAC_TBL0_ADDR + (i << 4) + 4;
            HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr, sizeof (a_uint32_t),
                                  (a_uint8_t *) (&(tbl[1])),
                                  sizeof (a_uint32_t));
            SW_RTN_ON_ERROR(rv);

            addr = ISIS_INTF_MAC_TBL0_ADDR + (i << 4) + 8;
            HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr, sizeof (a_uint32_t),
                                  (a_uint8_t *) (&(tbl[2])),
                                  sizeof (a_uint32_t));
            SW_RTN_ON_ERROR(rv);

            SW_GET_FIELD_BY_REG(INTF_ADDR_ENTRY1, VID_HIGH0, hvid, tbl[1]);
            SW_GET_FIELD_BY_REG(INTF_ADDR_ENTRY2, VID_HIGH1, lvid, tbl[2]);
            hvid |= ((lvid & 0xff) << 4);

            SW_GET_FIELD_BY_REG(INTF_ADDR_ENTRY1, VID_LOW, lvid, tbl[1]);

            if ((lvid <= sw_intf) && (hvid >= sw_intf))
            {
                vid_offset = entry->expect_vid ? (entry->expect_vid - lvid) : (sw_intf - lvid);
                *hw_intf = (vid_offset << 3) | i;
                return SW_OK;
            }
        }
    }

    return SW_BAD_PARAM;
}

static sw_error_t
_isis_ip_intf_hw_to_sw(a_uint32_t dev_id, a_uint32_t hw_intf,
                       a_uint32_t * sw_intf)
{
    sw_error_t rv;
    a_uint32_t addr, lvid, tbl = 0, i;

    i = hw_intf & 0x7;

    addr = ISIS_INTF_MAC_TBL0_ADDR + (i << 4) + 4;
    HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr, sizeof (a_uint32_t),
                          (a_uint8_t *) (&tbl), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    SW_GET_FIELD_BY_REG(INTF_ADDR_ENTRY1, VID_LOW, lvid, tbl);
    *sw_intf = lvid + (hw_intf >> 3);

    return SW_OK;
}

static sw_error_t
_isis_ip_age_time_set(a_uint32_t dev_id, a_uint32_t * time)
{
    sw_error_t rv;
    a_uint32_t data;

    if (255 < ((*time + 5) / 6))
    {
        return SW_BAD_PARAM;
    }

    data = ((*time + 5) / 6);
    *time = data * 6;

    HSL_REG_FIELD_SET(rv, dev_id, ROUTER_CTRL, 0, ARP_AGE_TIME,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_isis_ip_age_time_get(a_uint32_t dev_id, a_uint32_t * time)
{
    sw_error_t rv;
    a_uint32_t data = 0;

    HSL_REG_FIELD_GET(rv, dev_id, ROUTER_CTRL, 0, ARP_AGE_TIME,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    *time = data * 6;
    return SW_OK;
}

static sw_error_t
_isis_host_sw_to_hw(a_uint32_t dev_id, fal_host_entry_t * entry,
                    a_uint32_t reg[])
{
    sw_error_t rv;
    a_uint32_t data;

    if (FAL_IP_IP4_ADDR & entry->flags)
    {
        reg[0] = entry->ip4_addr;
    }

    if (FAL_IP_IP6_ADDR & entry->flags)
    {
        return SW_NOT_SUPPORTED;
    }

    SW_SET_REG_BY_FIELD(HOST_ENTRY1, MAC_ADDR2, entry->mac_addr.uc[2], reg[1]);
    SW_SET_REG_BY_FIELD(HOST_ENTRY1, MAC_ADDR3, entry->mac_addr.uc[3], reg[1]);
    SW_SET_REG_BY_FIELD(HOST_ENTRY1, MAC_ADDR4, entry->mac_addr.uc[4], reg[1]);
    SW_SET_REG_BY_FIELD(HOST_ENTRY1, MAC_ADDR5, entry->mac_addr.uc[5], reg[1]);
    SW_SET_REG_BY_FIELD(HOST_ENTRY2, MAC_ADDR0, entry->mac_addr.uc[0], reg[2]);
    SW_SET_REG_BY_FIELD(HOST_ENTRY2, MAC_ADDR1, entry->mac_addr.uc[1], reg[2]);

    rv = _isis_ip_intf_sw_to_hw(dev_id, entry/*was:->intf_id*/, &data);
    SW_RTN_ON_ERROR(rv);
    SW_SET_REG_BY_FIELD(HOST_ENTRY2, INTF_ID, data, reg[2]);

    if (A_TRUE != hsl_port_prop_check(dev_id, entry->port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }
    SW_SET_REG_BY_FIELD(HOST_ENTRY2, SRC_PORT, entry->port_id, reg[2]);

    if (FAL_IP_CPU_ADDR & entry->flags)
    {
        SW_SET_REG_BY_FIELD(HOST_ENTRY2, CPU_ADDR, 1, reg[2]);
    }

    SW_SET_REG_BY_FIELD(HOST_ENTRY3, AGE_FLAG, entry->status, reg[3]);

    if ((A_TRUE == entry->mirror_en) && (FAL_MAC_FRWRD != entry->action))
    {
        return SW_NOT_SUPPORTED;
    }

    if (A_TRUE == entry->counter_en)
    {
        SW_SET_REG_BY_FIELD(HOST_ENTRY3, CNT_EN, 1, reg[3]);
        SW_SET_REG_BY_FIELD(HOST_ENTRY3, CNT_IDX, entry->counter_id, reg[3]);
    }

    if (FAL_MAC_DROP == entry->action)
    {
        SW_SET_REG_BY_FIELD(HOST_ENTRY2, SRC_PORT, 7, reg[2]);
        SW_SET_REG_BY_FIELD(HOST_ENTRY3, ACTION, 3, reg[3]);
    }
    else if (FAL_MAC_RDT_TO_CPU == entry->action)
    {
        SW_SET_REG_BY_FIELD(HOST_ENTRY3, ACTION, 1, reg[3]);
    }
    else if (FAL_MAC_CPY_TO_CPU == entry->action)
    {
        SW_SET_REG_BY_FIELD(HOST_ENTRY3, ACTION, 2, reg[3]);
    }
    else
    {
        if (A_TRUE == entry->mirror_en)
        {
            SW_SET_REG_BY_FIELD(HOST_ENTRY3, ACTION, 0, reg[3]);
        }
        else
        {
            SW_SET_REG_BY_FIELD(HOST_ENTRY3, ACTION, 3, reg[3]);
        }
    }

    return SW_OK;
}

static sw_error_t
_isis_host_hw_to_sw(a_uint32_t dev_id, a_uint32_t reg[],
                    fal_host_entry_t * entry)
{
    sw_error_t rv;
    a_uint32_t data, cnt[2] = {0};

    SW_GET_FIELD_BY_REG(HOST_ENTRY3, IP_VER, data, reg[3]);
    if (data)
    {
        entry->ip6_addr.ul[0] =  reg[0];
        entry->flags |= FAL_IP_IP6_ADDR;
    }
    else
    {
        entry->ip4_addr = reg[0];
        entry->flags |= FAL_IP_IP4_ADDR;
    }

    SW_GET_FIELD_BY_REG(HOST_ENTRY1, MAC_ADDR2, entry->mac_addr.uc[2], reg[1]);
    SW_GET_FIELD_BY_REG(HOST_ENTRY1, MAC_ADDR3, entry->mac_addr.uc[3], reg[1]);
    SW_GET_FIELD_BY_REG(HOST_ENTRY1, MAC_ADDR4, entry->mac_addr.uc[4], reg[1]);
    SW_GET_FIELD_BY_REG(HOST_ENTRY1, MAC_ADDR5, entry->mac_addr.uc[5], reg[1]);
    SW_GET_FIELD_BY_REG(HOST_ENTRY2, MAC_ADDR0, entry->mac_addr.uc[0], reg[2]);
    SW_GET_FIELD_BY_REG(HOST_ENTRY2, MAC_ADDR1, entry->mac_addr.uc[1], reg[2]);

    SW_GET_FIELD_BY_REG(HOST_ENTRY2, INTF_ID, data, reg[2]);
    rv = _isis_ip_intf_hw_to_sw(dev_id, data, &(entry->intf_id));
    SW_RTN_ON_ERROR(rv);

    SW_GET_FIELD_BY_REG(HOST_ENTRY2, SRC_PORT, entry->port_id, reg[2]);

    SW_GET_FIELD_BY_REG(HOST_ENTRY2, CPU_ADDR, data, reg[2]);
    if (data)
    {
        entry->flags |= FAL_IP_CPU_ADDR;
    }

    SW_GET_FIELD_BY_REG(HOST_ENTRY3, AGE_FLAG, entry->status, reg[3]);

    SW_GET_FIELD_BY_REG(HOST_ENTRY3, CNT_EN, data, reg[3]);
    if (data)
    {
        entry->counter_en = A_TRUE;
        SW_GET_FIELD_BY_REG(HOST_ENTRY3, CNT_IDX, entry->counter_id, reg[3]);

        rv = _isis_ip_counter_get(dev_id, entry->counter_id, cnt);
        SW_RTN_ON_ERROR(rv);

        entry->packet = cnt[0];
        entry->byte = cnt[1];
    }
    else
    {
        entry->counter_en = A_FALSE;
    }

    SW_GET_FIELD_BY_REG(HOST_ENTRY3, PPPOE_EN, data, reg[3]);
    if (data)
    {
        entry->pppoe_en = A_TRUE;
        SW_GET_FIELD_BY_REG(HOST_ENTRY3, PPPOE_IDX, data, reg[3]);
        entry->pppoe_id = data;
    }
    else
    {
        entry->pppoe_en = A_FALSE;
    }

    if (7 == entry->port_id)
    {
        entry->port_id = 0;
        entry->action = FAL_MAC_DROP;
    }
    else
    {
        SW_GET_FIELD_BY_REG(HOST_ENTRY3, ACTION, data, reg[3]);
        entry->action = FAL_MAC_FRWRD;
        if (0 == data)
        {
            entry->mirror_en = A_TRUE;
        }
        else if (1 == data)
        {
            entry->action = FAL_MAC_RDT_TO_CPU;
        }
        else if (2 == data)
        {
            entry->action = FAL_MAC_CPY_TO_CPU;
        }
    }

    return SW_OK;
}

static sw_error_t
_isis_host_down_to_hw(a_uint32_t dev_id, a_uint32_t reg[])
{
    sw_error_t rv;
    a_uint32_t i, addr;

    for (i = 0; i < 5; i++)
    {
        addr = ISIS_HOST_ENTRY_DATA0_ADDR + (i << 2);
        HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr, sizeof (a_uint32_t),
                              (a_uint8_t *) (&reg[i]), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }

    return SW_OK;
}

static sw_error_t
_isis_host_up_to_sw(a_uint32_t dev_id, a_uint32_t reg[])
{
    sw_error_t rv;
    a_uint32_t i, addr;

    for (i = 0; i < 5; i++)
    {
        addr = ISIS_HOST_ENTRY_DATA0_ADDR + (i << 2);
        HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr, sizeof (a_uint32_t),
                              (a_uint8_t *) (&reg[i]), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }

    return SW_OK;
}

static sw_error_t
_isis_ip_host_add(a_uint32_t dev_id, fal_host_entry_t * entry)
{
    sw_error_t rv;
    a_uint32_t reg[5] = { 0 };

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isis_ip_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    rv = _isis_host_sw_to_hw(dev_id, entry, reg);
    SW_RTN_ON_ERROR(rv);

    rv = _isis_host_down_to_hw(dev_id, reg);
    SW_RTN_ON_ERROR(rv);

    rv = _isis_host_entry_commit(dev_id, ISIS_ENTRY_ARP, ISIS_HOST_ENTRY_ADD);
    SW_RTN_ON_ERROR(rv);

    HSL_REG_ENTRY_GET(rv, dev_id, HOST_ENTRY4, 0, (a_uint8_t *) (&reg[4]),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    SW_GET_FIELD_BY_REG(HOST_ENTRY4, TBL_IDX, entry->entry_id, reg[4]);
    return SW_OK;
}

static sw_error_t
_isis_ip_host_del(a_uint32_t dev_id, a_uint32_t del_mode,
                  fal_host_entry_t * entry)
{
    sw_error_t rv;
    a_uint32_t data, reg[5] = { 0 }, op = ISIS_HOST_ENTRY_FLUSH;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isis_ip_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    if (FAL_IP_ENTRY_ID_EN & del_mode)
    {
        return SW_NOT_SUPPORTED;
    }

    if (FAL_IP_ENTRY_IPADDR_EN & del_mode)
    {
        op = ISIS_HOST_ENTRY_DEL;
        if (FAL_IP_IP4_ADDR & entry->flags)
        {
            reg[0] = entry->ip4_addr;
        }

        if (FAL_IP_IP6_ADDR & entry->flags)
        {
            return SW_NOT_SUPPORTED;
        }
    }

    if (FAL_IP_ENTRY_INTF_EN & del_mode)
    {
        rv = _isis_ip_intf_sw_to_hw(dev_id, entry/*was:->intf_id*/, &data);
        SW_RTN_ON_ERROR(rv);

        SW_SET_REG_BY_FIELD(HOST_ENTRY4, SPEC_VID, 1, reg[4]);
        SW_SET_REG_BY_FIELD(HOST_ENTRY2, INTF_ID, data, reg[2]);
    }

    if (FAL_IP_ENTRY_PORT_EN & del_mode)
    {
        SW_SET_REG_BY_FIELD(HOST_ENTRY4, SPEC_SP, 1, reg[4]);
        SW_SET_REG_BY_FIELD(HOST_ENTRY2, SRC_PORT, entry->port_id, reg[2]);
    }

    if (FAL_IP_ENTRY_STATUS_EN & del_mode)
    {
        SW_SET_REG_BY_FIELD(HOST_ENTRY4, SPEC_STATUS, 1, reg[4]);
        SW_SET_REG_BY_FIELD(HOST_ENTRY3, AGE_FLAG, entry->status, reg[3]);
    }

    rv = _isis_host_down_to_hw(dev_id, reg);
    SW_RTN_ON_ERROR(rv);

    rv = _isis_host_entry_commit(dev_id, ISIS_ENTRY_ARP, op);
    return rv;
}

static sw_error_t
_isis_ip_host_get(a_uint32_t dev_id, a_uint32_t get_mode,
                  fal_host_entry_t * entry)
{
    sw_error_t rv;
    a_uint32_t reg[5] = { 0 };

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isis_ip_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    if (FAL_IP_ENTRY_IPADDR_EN != get_mode)
    {
        return SW_NOT_SUPPORTED;
    }

    if (FAL_IP_IP4_ADDR & entry->flags)
    {
        reg[0] = entry->ip4_addr;
    }
    else if (FAL_IP_IP6_ADDR & entry->flags)
    {
        return SW_NOT_SUPPORTED;
    }

    rv = _isis_host_down_to_hw(dev_id, reg);
    SW_RTN_ON_ERROR(rv);

    rv = _isis_host_entry_commit(dev_id, ISIS_ENTRY_ARP,
                                 ISIS_HOST_ENTRY_SEARCH);
    SW_RTN_ON_ERROR(rv);

    rv = _isis_host_up_to_sw(dev_id, reg);
    SW_RTN_ON_ERROR(rv);

    aos_mem_zero(entry, sizeof (fal_host_entry_t));

    rv = _isis_host_hw_to_sw(dev_id, reg, entry);
    SW_RTN_ON_ERROR(rv);

    if (!(entry->status))
    {
        return SW_NOT_FOUND;
    }

    HSL_REG_ENTRY_GET(rv, dev_id, HOST_ENTRY4, 0, (a_uint8_t *) (&reg[4]),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    SW_GET_FIELD_BY_REG(HOST_ENTRY4, TBL_IDX, entry->entry_id, reg[4]);
    return SW_OK;
}

static sw_error_t
_isis_ip_host_next(a_uint32_t dev_id, a_uint32_t next_mode,
                   fal_host_entry_t * entry)
{
    sw_error_t rv;
    a_uint32_t idx, data, reg[5] = { 0 };

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isis_ip_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    if (FAL_NEXT_ENTRY_FIRST_ID == entry->entry_id)
    {
        idx = ISIS_HOST_ENTRY_NUM - 1;
    }
    else
    {
        if ((ISIS_HOST_ENTRY_NUM - 1) == entry->entry_id)
        {
            return SW_NO_MORE;
        }
        else
        {
            idx = entry->entry_id;
        }
    }

    SW_SET_REG_BY_FIELD(HOST_ENTRY4, TBL_IDX, idx, reg[4]);

    if (FAL_IP_ENTRY_INTF_EN & next_mode)
    {
        rv = _isis_ip_intf_sw_to_hw(dev_id, entry/*was:->intf_id*/, &data);
        SW_RTN_ON_ERROR(rv);

        SW_SET_REG_BY_FIELD(HOST_ENTRY4, SPEC_VID, 1, reg[4]);
        SW_SET_REG_BY_FIELD(HOST_ENTRY2, INTF_ID, data, reg[2]);
    }

    if (FAL_IP_ENTRY_PORT_EN & next_mode)
    {
        SW_SET_REG_BY_FIELD(HOST_ENTRY4, SPEC_SP, 1, reg[4]);
        SW_SET_REG_BY_FIELD(HOST_ENTRY2, SRC_PORT, entry->port_id, reg[2]);
    }

    if (FAL_IP_ENTRY_STATUS_EN & next_mode)
    {
        SW_SET_REG_BY_FIELD(HOST_ENTRY4, SPEC_STATUS, 1, reg[4]);
        SW_SET_REG_BY_FIELD(HOST_ENTRY3, AGE_FLAG, entry->status, reg[3]);
    }

    rv = _isis_host_down_to_hw(dev_id, reg);
    SW_RTN_ON_ERROR(rv);

    rv = _isis_host_entry_commit(dev_id, ISIS_ENTRY_ARP, ISIS_HOST_ENTRY_NEXT);
    SW_RTN_ON_ERROR(rv);

    rv = _isis_host_up_to_sw(dev_id, reg);
    SW_RTN_ON_ERROR(rv);

    aos_mem_zero(entry, sizeof (fal_host_entry_t));

    rv = _isis_host_hw_to_sw(dev_id, reg, entry);
    SW_RTN_ON_ERROR(rv);

    if (!(entry->status))
    {
        return SW_NO_MORE;
    }

    SW_GET_FIELD_BY_REG(HOST_ENTRY4, TBL_IDX, entry->entry_id, reg[4]);
    return SW_OK;
}

static sw_error_t
_isis_ip_host_counter_bind(a_uint32_t dev_id, a_uint32_t entry_id,
                           a_uint32_t cnt_id, a_bool_t enable)
{
    sw_error_t rv;
    a_uint32_t reg[5] = { 0 }, tbl[5] = { 0 }, tbl_idx;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isis_ip_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    tbl_idx = (entry_id - 1) & 0x7f;
    SW_SET_REG_BY_FIELD(HOST_ENTRY4, TBL_IDX, tbl_idx, reg[4]);

    rv = _isis_host_down_to_hw(dev_id, reg);
    SW_RTN_ON_ERROR(rv);

    rv = _isis_host_entry_commit(dev_id, ISIS_ENTRY_ARP, ISIS_HOST_ENTRY_NEXT);
    if (SW_OK != rv)
    {
        return SW_NOT_FOUND;
    }

    rv = _isis_host_up_to_sw(dev_id, reg);
    SW_RTN_ON_ERROR(rv);

    SW_GET_FIELD_BY_REG(HOST_ENTRY4, TBL_IDX, tbl_idx, reg[4]);
    if (entry_id != tbl_idx)
    {
        return SW_NOT_FOUND;
    }

    tbl[0] = reg[0];
    tbl[3] = (reg[3] >> 15) << 15;
    rv = _isis_host_down_to_hw(dev_id, tbl);
    SW_RTN_ON_ERROR(rv);

    rv = _isis_host_entry_commit(dev_id, ISIS_ENTRY_ARP, ISIS_HOST_ENTRY_DEL);
    SW_RTN_ON_ERROR(rv);

    if (A_FALSE == enable)
    {
        SW_SET_REG_BY_FIELD(HOST_ENTRY3, CNT_EN, 0, reg[3]);
    }
    else if (A_TRUE == enable)
    {
        SW_SET_REG_BY_FIELD(HOST_ENTRY3, CNT_EN, 1, reg[3]);
        SW_SET_REG_BY_FIELD(HOST_ENTRY3, CNT_IDX, cnt_id, reg[3]);
    }
    else
    {
        return SW_BAD_PARAM;
    }

    reg[4] = 0x0;
    rv = _isis_host_down_to_hw(dev_id, reg);
    SW_RTN_ON_ERROR(rv);

    rv = _isis_host_entry_commit(dev_id, ISIS_ENTRY_ARP, ISIS_HOST_ENTRY_ADD);
    return rv;
}

static sw_error_t
_isis_ip_host_pppoe_bind(a_uint32_t dev_id, a_uint32_t entry_id,
                         a_uint32_t pppoe_id, a_bool_t enable)
{
    sw_error_t rv;
    a_uint32_t reg[5] = { 0 }, tbl[5] = { 0 }, tbl_idx;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isis_ip_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    tbl_idx = (entry_id - 1) & 0x7f;
    SW_SET_REG_BY_FIELD(HOST_ENTRY4, TBL_IDX, tbl_idx, reg[4]);

    rv = _isis_host_down_to_hw(dev_id, reg);
    SW_RTN_ON_ERROR(rv);

    rv = _isis_host_entry_commit(dev_id, ISIS_ENTRY_ARP, ISIS_HOST_ENTRY_NEXT);
    SW_RTN_ON_ERROR(rv);

    rv = _isis_host_up_to_sw(dev_id, reg);
    if (SW_OK != rv)
    {
        return SW_NOT_FOUND;
    }

    SW_GET_FIELD_BY_REG(HOST_ENTRY4, TBL_IDX, tbl_idx, reg[4]);
    if (entry_id != tbl_idx)
    {
        return SW_NOT_FOUND;
    }

    if (A_FALSE == enable)
    {
        SW_SET_REG_BY_FIELD(HOST_ENTRY3, PPPOE_EN, 0, reg[3]);
    }
    else if (A_TRUE == enable)
    {
        SW_SET_REG_BY_FIELD(HOST_ENTRY3, PPPOE_EN, 1, reg[3]);
        SW_SET_REG_BY_FIELD(HOST_ENTRY3, PPPOE_IDX, pppoe_id, reg[3]);
    }
    else
    {
        return SW_BAD_PARAM;
    }

    tbl[0] = reg[0];
    tbl[3] = (reg[3] >> 15) << 15;
    rv = _isis_host_down_to_hw(dev_id, tbl);
    SW_RTN_ON_ERROR(rv);

    rv = _isis_host_entry_commit(dev_id, ISIS_ENTRY_ARP, ISIS_HOST_ENTRY_DEL);
    SW_RTN_ON_ERROR(rv);

    reg[4] = 0x0;
    rv = _isis_host_down_to_hw(dev_id, reg);
    SW_RTN_ON_ERROR(rv);

    rv = _isis_host_entry_commit(dev_id, ISIS_ENTRY_ARP, ISIS_HOST_ENTRY_ADD);
    SW_RTN_ON_ERROR(rv);

    rv = _isis_host_up_to_sw(dev_id, reg);
    SW_RTN_ON_ERROR(rv);

    return SW_OK;
}

static sw_error_t
_isis_ip_pt_arp_learn_set(a_uint32_t dev_id, fal_port_t port_id,
                          a_uint32_t flags)
{
    sw_error_t rv;
    a_uint32_t data = 0;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isis_ip_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    HSL_REG_ENTRY_GET(rv, dev_id, ROUTER_PTCTRL2, 0,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (FAL_ARP_LEARN_REQ & flags)
    {
        data |= (0x1 << port_id);
    }
    else
    {
        data &= (~(0x1 << port_id));
    }

    if (FAL_ARP_LEARN_ACK & flags)
    {
        data |= (0x1 << (ROUTER_PTCTRL2_ARP_LEARN_ACK_BOFFSET + port_id));
    }
    else
    {
        data &= (~(0x1 << (ROUTER_PTCTRL2_ARP_LEARN_ACK_BOFFSET + port_id)));
    }

    HSL_REG_ENTRY_SET(rv, dev_id, ROUTER_PTCTRL2, 0,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_isis_ip_pt_arp_learn_get(a_uint32_t dev_id, fal_port_t port_id,
                          a_uint32_t * flags)
{
    sw_error_t rv;
    a_uint32_t data = 0;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isis_ip_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    *flags = 0;

    HSL_REG_ENTRY_GET(rv, dev_id, ROUTER_PTCTRL2, 0,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (data & (0x1 << port_id))
    {
        *flags |= FAL_ARP_LEARN_REQ;
    }

    if (data & (0x1 << (ROUTER_PTCTRL2_ARP_LEARN_ACK_BOFFSET + port_id)))
    {
        *flags |= FAL_ARP_LEARN_ACK;
    }

    return SW_OK;
}

static sw_error_t
_isis_ip_arp_learn_set(a_uint32_t dev_id, fal_arp_learn_mode_t mode)
{
    sw_error_t rv;
    a_uint32_t data;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isis_ip_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    if (FAL_ARP_LEARN_ALL == mode)
    {
        data = 1;
    }
    else if (FAL_ARP_LEARN_LOCAL == mode)
    {
        data = 0;
    }
    else
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_SET(rv, dev_id, ROUTER_CTRL, 0, ARP_LEARN_MODE,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_isis_ip_arp_learn_get(a_uint32_t dev_id, fal_arp_learn_mode_t * mode)
{
    sw_error_t rv;
    a_uint32_t data = 0;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isis_ip_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    HSL_REG_FIELD_GET(rv, dev_id, ROUTER_CTRL, 0, ARP_LEARN_MODE,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (data)
    {
        *mode = FAL_ARP_LEARN_ALL;
    }
    else
    {
        *mode = FAL_ARP_LEARN_LOCAL;
    }

    return SW_OK;
}

static sw_error_t
_isis_ip_source_guard_set(a_uint32_t dev_id, fal_port_t port_id,
                          fal_source_guard_mode_t mode)
{
    sw_error_t rv;
    a_uint32_t reg = 0, data;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isis_ip_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    HSL_REG_ENTRY_GET(rv, dev_id, ROUTER_PTCTRL1, 0,
                      (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (FAL_NO_SOURCE_GUARD < mode)
    {
        return SW_BAD_PARAM;
    }

    data = 0;
    if (FAL_MAC_IP_GUARD == mode)
    {
        data = 1;
    }
    else if (FAL_MAC_IP_PORT_GUARD == mode)
    {
        data = 2;
    }
    else if (FAL_MAC_IP_VLAN_GUARD == mode)
    {
        data = 3;
    }
    else if (FAL_MAC_IP_PORT_VLAN_GUARD == mode)
    {
        data = 4;
    }
    reg &= (~(0x7 << (port_id * 3)));
    reg |= ((data & 0x7) << (port_id * 3));

    HSL_REG_ENTRY_SET(rv, dev_id, ROUTER_PTCTRL1, 0,
                      (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    data = 1;
    if (FAL_NO_SOURCE_GUARD == mode)
    {
        data = 0;
    }

    HSL_REG_FIELD_SET(rv, dev_id, PORT_VLAN1, port_id, SP_CHECK_EN,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_isis_ip_source_guard_get(a_uint32_t dev_id, fal_port_t port_id,
                          fal_source_guard_mode_t * mode)
{
    sw_error_t rv;
    a_uint32_t reg = 0, data;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isis_ip_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    HSL_REG_ENTRY_GET(rv, dev_id, ROUTER_PTCTRL1, 0,
                      (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    data = (reg >> (port_id * 3)) & 0x7;

    *mode = FAL_NO_SOURCE_GUARD;
    if (1 == data)
    {
        *mode = FAL_MAC_IP_GUARD;
    }
    else if (2 == data)
    {
        *mode = FAL_MAC_IP_PORT_GUARD;
    }
    else if (3 == data)
    {
        *mode = FAL_MAC_IP_VLAN_GUARD;
    }
    else if (4 == data)
    {
        *mode = FAL_MAC_IP_PORT_VLAN_GUARD;
    }

    return SW_OK;
}

static sw_error_t
_isis_ip_unk_source_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd)
{
    sw_error_t rv;
    a_uint32_t data;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isis_ip_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    if (FAL_MAC_FRWRD == cmd)
    {
        data = 0;
    }
    else if (FAL_MAC_DROP == cmd)
    {
        data = 1;
    }
    else if (FAL_MAC_RDT_TO_CPU == cmd)
    {
        data = 2;
    }
    else
    {
        return SW_NOT_SUPPORTED;
    }

    HSL_REG_FIELD_SET(rv, dev_id, FORWARD_CTL0, 0, IP_NOT_FOUND,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_isis_ip_unk_source_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;
    a_uint32_t data = 0;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isis_ip_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    HSL_REG_FIELD_GET(rv, dev_id, FORWARD_CTL0, 0, IP_NOT_FOUND,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (0 == data)
    {
        *cmd = FAL_MAC_FRWRD;
    }
    else if (1 == data)
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
_isis_ip_arp_guard_set(a_uint32_t dev_id, fal_port_t port_id,
                       fal_source_guard_mode_t mode)
{
    sw_error_t rv;
    a_uint32_t reg = 0, data;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isis_ip_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    HSL_DEV_ID_CHECK(dev_id);

    HSL_REG_ENTRY_GET(rv, dev_id, ROUTER_PTCTRL0, 0,
                      (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (FAL_NO_SOURCE_GUARD < mode)
    {
        return SW_BAD_PARAM;
    }

    data = 0;
    if (FAL_MAC_IP_GUARD == mode)
    {
        data = 1;
    }
    else if (FAL_MAC_IP_PORT_GUARD == mode)
    {
        data = 2;
    }
    else if (FAL_MAC_IP_VLAN_GUARD == mode)
    {
        data = 3;
    }
    else if (FAL_MAC_IP_PORT_VLAN_GUARD == mode)
    {
        data = 4;
    }
    reg &= (~(0x7 << (port_id * 3)));
    reg |= ((data & 0x7) << (port_id * 3));

    HSL_REG_ENTRY_SET(rv, dev_id, ROUTER_PTCTRL0, 0,
                      (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_isis_ip_arp_guard_get(a_uint32_t dev_id, fal_port_t port_id,
                       fal_source_guard_mode_t * mode)
{
    sw_error_t rv;
    a_uint32_t reg = 0, data;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isis_ip_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    HSL_DEV_ID_CHECK(dev_id);

    HSL_REG_ENTRY_GET(rv, dev_id, ROUTER_PTCTRL0, 0,
                      (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    data = (reg >> (port_id * 3)) & 0x7;

    *mode = FAL_NO_SOURCE_GUARD;
    if (1 == data)
    {
        *mode = FAL_MAC_IP_GUARD;
    }
    else if (2 == data)
    {
        *mode = FAL_MAC_IP_PORT_GUARD;
    }
    else if (3 == data)
    {
        *mode = FAL_MAC_IP_VLAN_GUARD;
    }
    else if (4 == data)
    {
        *mode = FAL_MAC_IP_PORT_VLAN_GUARD;
    }

    return SW_OK;
}

static sw_error_t
_isis_arp_unk_source_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd)
{
    sw_error_t rv;
    a_uint32_t data;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isis_ip_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    if (FAL_MAC_FRWRD == cmd)
    {
        data = 0;
    }
    else if (FAL_MAC_DROP == cmd)
    {
        data = 1;
    }
    else if (FAL_MAC_RDT_TO_CPU == cmd)
    {
        data = 2;
    }
    else
    {
        return SW_NOT_SUPPORTED;
    }

    HSL_REG_FIELD_SET(rv, dev_id, FORWARD_CTL0, 0, ARP_NOT_FOUND,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_isis_arp_unk_source_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;
    a_uint32_t data = 0;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isis_ip_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    HSL_REG_FIELD_GET(rv, dev_id, FORWARD_CTL0, 0, ARP_NOT_FOUND,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (0 == data)
    {
        *cmd = FAL_MAC_FRWRD;
    }
    else if (1 == data)
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
_isis_ip_route_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;
    a_uint32_t data;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isis_ip_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

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

    HSL_REG_FIELD_SET(rv, dev_id, ROUTER_CTRL, 0, ROUTER_EN,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    HSL_REG_FIELD_SET(rv, dev_id, MOD_ENABLE, 0, L3_EN,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_isis_ip_route_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;
    a_uint32_t route_en = 0, l3_en = 0;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isis_ip_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    HSL_REG_FIELD_GET(rv, dev_id, ROUTER_CTRL, 0, ROUTER_EN,
                      (a_uint8_t *) (&route_en), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    HSL_REG_FIELD_GET(rv, dev_id, MOD_ENABLE, 0, L3_EN,
                      (a_uint8_t *) (&l3_en), sizeof (a_uint32_t))
    SW_RTN_ON_ERROR(rv);

    if (route_en && l3_en)
    {
        *enable = A_TRUE;
    }
    else
    {
        *enable = A_FALSE;
    }

    return rv;
}

static sw_error_t
_isis_ip_intf_entry_add(a_uint32_t dev_id, fal_intf_mac_entry_t * entry)
{
    sw_error_t rv;
    a_uint32_t i, j, found = 0, addr, tbl[3] = { 0 };
    fal_intf_mac_entry_t * intf_entry;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isis_ip_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    for (i = 0; i < ISIS_INTF_MAC_ADDR_NUM; i++)
    {
        if (isis_mac_snap[dev_id] & (0x1 << i))
        {
            intf_entry = &(isis_intf_snap[dev_id][i]);
            if ((entry->vid_low == intf_entry->vid_low)
                    && (entry->vid_high == intf_entry->vid_high)
                    &&(!memcmp(&entry->mac_addr, &intf_entry->mac_addr, 6)))
            {
                /* all same, return OK directly */
                if (!aos_mem_cmp(intf_entry, entry, sizeof(fal_intf_mac_entry_t)))
                {
                    return SW_OK;
                }
                else
                {
                    /* update entry */
                    found = 1;
                    break;
                }
            }
            else
            {
#if 0 /* Different mac should be ok for VID range? */
                /* entry VID cross border, not support */
                if ((entry->vid_low >= intf_entry->vid_low) && (entry->vid_low <= intf_entry->vid_high))
                {
                    return SW_BAD_PARAM;
                }

                /* entry VID cross border, not support */
                if ((entry->vid_high >= intf_entry->vid_low) && (entry->vid_low <= intf_entry->vid_high))
                {
                    return SW_BAD_PARAM;
                }
#endif
            }
        }
    }

    if (!found)
    {
        for (i = 0; i < ISIS_INTF_MAC_ADDR_NUM; i++)
        {
            if (!(isis_mac_snap[dev_id] & (0x1 << i)))
            {
                intf_entry = &(isis_intf_snap[dev_id][i]);
                break;
            }
        }
    }

    if (ISIS_INTF_MAC_ADDR_NUM == i)
    {
        return SW_NO_RESOURCE;
    }

    if ((A_FALSE == entry->ip4_route) && (A_FALSE == entry->ip6_route))
    {
        return SW_NOT_SUPPORTED;
    }

    if (512 <= (entry->vid_high - entry->vid_low))
    {
        return SW_BAD_PARAM;
    }

    SW_SET_REG_BY_FIELD(INTF_ADDR_ENTRY0, MAC_ADDR2, entry->mac_addr.uc[2],
                        tbl[0]);
    SW_SET_REG_BY_FIELD(INTF_ADDR_ENTRY0, MAC_ADDR3, entry->mac_addr.uc[3],
                        tbl[0]);
    SW_SET_REG_BY_FIELD(INTF_ADDR_ENTRY0, MAC_ADDR4, entry->mac_addr.uc[4],
                        tbl[0]);
    SW_SET_REG_BY_FIELD(INTF_ADDR_ENTRY0, MAC_ADDR5, entry->mac_addr.uc[5],
                        tbl[0]);
    SW_SET_REG_BY_FIELD(INTF_ADDR_ENTRY1, MAC_ADDR0, entry->mac_addr.uc[0],
                        tbl[1]);
    SW_SET_REG_BY_FIELD(INTF_ADDR_ENTRY1, MAC_ADDR1, entry->mac_addr.uc[1],
                        tbl[1]);

    SW_SET_REG_BY_FIELD(INTF_ADDR_ENTRY1, VID_LOW, entry->vid_low, tbl[1]);
    SW_SET_REG_BY_FIELD(INTF_ADDR_ENTRY1, VID_HIGH0, (entry->vid_high & 0xf),
                        tbl[1]);
    SW_SET_REG_BY_FIELD(INTF_ADDR_ENTRY2, VID_HIGH1, (entry->vid_high >> 4),
                        tbl[2]);

    if (A_TRUE == entry->ip4_route)
    {
        SW_SET_REG_BY_FIELD(INTF_ADDR_ENTRY2, IP4_ROUTE, 1, tbl[2]);
    }

    if (A_TRUE == entry->ip6_route)
    {
        SW_SET_REG_BY_FIELD(INTF_ADDR_ENTRY2, IP6_ROUTE, 1, tbl[2]);
    }

    for (j = 0; j < 2; j++)
    {
        addr = ISIS_INTF_MAC_EDIT0_ADDR + (i << 4) + (j << 2);
        HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr, sizeof (a_uint32_t),
                              (a_uint8_t *) (&(tbl[j])), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }

    for (j = 0; j < 3; j++)
    {
        addr = ISIS_INTF_MAC_TBL0_ADDR + (i << 4) + (j << 2);
        HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr, sizeof (a_uint32_t),
                              (a_uint8_t *) (&(tbl[j])), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }

    isis_mac_snap[dev_id] |= (0x1 << i);
    *intf_entry = *entry;
    entry->entry_id = i;
    return SW_OK;
}

static sw_error_t
_isis_ip_intf_entry_del(a_uint32_t dev_id, a_uint32_t del_mode,
                        fal_intf_mac_entry_t * entry)
{
    sw_error_t rv;
    a_uint32_t addr, tbl[3] = { 0 };

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isis_ip_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    if (!(FAL_IP_ENTRY_ID_EN & del_mode))
    {
        return SW_NOT_SUPPORTED;
    }

    if (ISIS_INTF_MAC_ADDR_NUM <= entry->entry_id)
    {
        return SW_BAD_PARAM;
    }

    /* clear valid bits */
    addr = ISIS_INTF_MAC_TBL2_ADDR + (entry->entry_id << 4);
    HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr, sizeof (a_uint32_t),
                          (a_uint8_t *) (&(tbl[2])), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    isis_mac_snap[dev_id] &= (~(0x1 << entry->entry_id));
    return SW_OK;
}

static sw_error_t
_isis_ip_intf_entry_next(a_uint32_t dev_id, a_uint32_t next_mode,
                         fal_intf_mac_entry_t * entry)
{
    sw_error_t rv;
    a_uint32_t i, j, idx, addr, tbl[3] = { 0 };

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isis_ip_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    if (FAL_NEXT_ENTRY_FIRST_ID == entry->entry_id)
    {
        idx = 0;
    }
    else
    {
        if ((ISIS_INTF_MAC_ADDR_NUM - 1) == entry->entry_id)
        {
            return SW_NO_MORE;
        }
        else
        {
            idx = entry->entry_id + 1;
        }
    }

    for (i = idx; i < ISIS_INTF_MAC_ADDR_NUM; i++)
    {
        if (isis_mac_snap[dev_id] & (0x1 << i))
        {
            break;
        }
    }

    if (ISIS_INTF_MAC_ADDR_NUM == i)
    {
        return SW_NO_MORE;
    }

    for (j = 0; j < 3; j++)
    {
        addr = ISIS_INTF_MAC_TBL0_ADDR + (i << 4) + (j << 2);
        HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr, sizeof (a_uint32_t),
                              (a_uint8_t *) (&(tbl[j])), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }

    aos_mem_zero(entry, sizeof (fal_intf_mac_entry_t));

    SW_GET_FIELD_BY_REG(INTF_ADDR_ENTRY0, MAC_ADDR2, entry->mac_addr.uc[2],
                        tbl[0]);
    SW_GET_FIELD_BY_REG(INTF_ADDR_ENTRY0, MAC_ADDR3, entry->mac_addr.uc[3],
                        tbl[0]);
    SW_GET_FIELD_BY_REG(INTF_ADDR_ENTRY0, MAC_ADDR4, entry->mac_addr.uc[4],
                        tbl[0]);
    SW_GET_FIELD_BY_REG(INTF_ADDR_ENTRY0, MAC_ADDR5, entry->mac_addr.uc[5],
                        tbl[0]);
    SW_GET_FIELD_BY_REG(INTF_ADDR_ENTRY1, MAC_ADDR0, entry->mac_addr.uc[0],
                        tbl[1]);
    SW_GET_FIELD_BY_REG(INTF_ADDR_ENTRY1, MAC_ADDR1, entry->mac_addr.uc[1],
                        tbl[1]);

    SW_GET_FIELD_BY_REG(INTF_ADDR_ENTRY1, VID_LOW, entry->vid_low, tbl[1]);
    SW_GET_FIELD_BY_REG(INTF_ADDR_ENTRY1, VID_HIGH0, j, tbl[1]);
    entry->vid_high = j & 0xf;
    SW_GET_FIELD_BY_REG(INTF_ADDR_ENTRY2, VID_HIGH1, j, tbl[2]);
    entry->vid_high |= ((j & 0xff) << 4);

    SW_GET_FIELD_BY_REG(INTF_ADDR_ENTRY2, IP4_ROUTE, j, tbl[2]);
    if (j)
    {
        entry->ip4_route = A_TRUE;
    }

    SW_GET_FIELD_BY_REG(INTF_ADDR_ENTRY2, IP6_ROUTE, j, tbl[2]);
    if (j)
    {
        entry->ip6_route = A_TRUE;
    }

    entry->entry_id = i;
    return SW_OK;
}

#define ISIS_WCMP_ENTRY_MAX_ID    3
#define ISIS_WCMP_HASH_MAX_NUM    16
#define ISIS_IP_ENTRY_MAX_ID      127

#define ISIS_WCMP_HASH_TBL_ADDR   0x0e10
#define ISIS_WCMP_NHOP_TBL_ADDR   0x0e20

#if 0
static sw_error_t
_isis_ip_wcmp_entry_set(a_uint32_t dev_id, a_uint32_t wcmp_id, fal_ip_wcmp_t * wcmp)
{
    sw_error_t rv;
    a_uint32_t i, j, addr, data;
    a_uint8_t  idx, ptr[4] = { 0 }, pos[16] = { 0 };

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isis_ip_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    if (ISIS_WCMP_ENTRY_MAX_ID < wcmp_id)
    {
        return SW_BAD_PARAM;
    }

    if (ISIS_WCMP_HASH_MAX_NUM < wcmp->nh_nr)
    {
        return SW_BAD_PARAM;
    }

    for (i = 0; i < wcmp->nh_nr; i++)
    {
        if (ISIS_IP_ENTRY_MAX_ID < wcmp->nh_id[i])
        {
            return SW_BAD_PARAM;
        }

        idx = 4;
        for (j = 0; j < 4; j++)
        {
            if (ptr[j] & 0x80)
            {
                if ((ptr[j] & 0x7f) == wcmp->nh_id[i])
                {
                    idx = j;
                    break;
                }
            }
            else
            {
                idx = j;
            }
        }

        if (4 == idx)
        {
            return SW_BAD_PARAM;
        }
        else
        {
            ptr[idx] = (wcmp->nh_id[i] & 0x7f) | 0x80;
            pos[i]   = idx;
        }
    }

    data = 0;
    for (j = 0; j < 4; j++)
    {
        data |= (ptr[j] << (j << 3));
    }

    addr = ISIS_WCMP_NHOP_TBL_ADDR + (wcmp_id << 2);
    HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr, sizeof (a_uint32_t),
                          (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    data = 0;
    for (j = 0; j < 16; j++)
    {
        data |= (pos[j] << (j << 1));
    }

    addr = ISIS_WCMP_HASH_TBL_ADDR + (wcmp_id << 2);
    HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr, sizeof (a_uint32_t),
                          (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    return SW_OK;
}

static sw_error_t
_isis_ip_wcmp_entry_get(a_uint32_t dev_id, a_uint32_t wcmp_id, fal_ip_wcmp_t * wcmp)
{
    sw_error_t rv;
    a_uint32_t i, addr, data= 0;
    a_uint8_t  ptr[4] = { 0 }, pos[16] = { 0 };

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isis_ip_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    if (ISIS_WCMP_ENTRY_MAX_ID < wcmp_id)
    {
        return SW_BAD_PARAM;
    }

    wcmp->nh_nr = ISIS_WCMP_HASH_MAX_NUM;

    addr = ISIS_WCMP_NHOP_TBL_ADDR + (wcmp_id << 2);
    HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr, sizeof (a_uint32_t),
                          (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    for (i = 0; i < 4; i++)
    {
        ptr[i] = (data >> (i << 3)) & 0x7f;
    }

    addr = ISIS_WCMP_HASH_TBL_ADDR + (wcmp_id << 2);
    HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr, sizeof (a_uint32_t),
                          (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    for (i = 0; i < 16; i++)
    {
        pos[i] = (data >> (i << 1)) & 0x3;
    }

    for (i = 0; i < 16; i++)
    {
        wcmp->nh_id[i] = ptr[pos[i]];
    }

    return SW_OK;
}
#endif

static sw_error_t
_isis_ip_wcmp_hash_mode_set(a_uint32_t dev_id, a_uint32_t hash_mode)
{
    sw_error_t rv;
    a_uint32_t data = 0;

    HSL_REG_ENTRY_GET(rv, dev_id, ROUTER_CTRL, 0,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (FAL_WCMP_HASH_KEY_SIP & hash_mode)
    {
        SW_SET_REG_BY_FIELD(ROUTER_CTRL, WCMP_HAHS_SIP, 1, data);
    }
    else
    {
        SW_SET_REG_BY_FIELD(ROUTER_CTRL, WCMP_HAHS_SIP, 0, data);
    }

    if (FAL_WCMP_HASH_KEY_DIP & hash_mode)
    {
        SW_SET_REG_BY_FIELD(ROUTER_CTRL, WCMP_HAHS_DIP, 1, data);
    }
    else
    {
        SW_SET_REG_BY_FIELD(ROUTER_CTRL, WCMP_HAHS_DIP, 0, data);
    }

    if (FAL_WCMP_HASH_KEY_SPORT & hash_mode)
    {
        SW_SET_REG_BY_FIELD(ROUTER_CTRL, WCMP_HAHS_SP, 1, data);
    }
    else
    {
        SW_SET_REG_BY_FIELD(ROUTER_CTRL, WCMP_HAHS_SP, 0, data);
    }

    if (FAL_WCMP_HASH_KEY_DPORT & hash_mode)
    {
        SW_SET_REG_BY_FIELD(ROUTER_CTRL, WCMP_HAHS_DP, 1, data);
    }
    else
    {
        SW_SET_REG_BY_FIELD(ROUTER_CTRL, WCMP_HAHS_DP, 0, data);
    }


    HSL_REG_ENTRY_SET(rv, dev_id, ROUTER_CTRL, 0,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_isis_ip_wcmp_hash_mode_get(a_uint32_t dev_id, a_uint32_t * hash_mode)
{
    sw_error_t rv;
    a_uint32_t data = 0, field;

    *hash_mode = 0;

    HSL_REG_ENTRY_GET(rv, dev_id, ROUTER_CTRL, 0,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    SW_GET_FIELD_BY_REG(ROUTER_CTRL, WCMP_HAHS_SIP, field, data);
    if (field)
    {
        *hash_mode |= FAL_WCMP_HASH_KEY_SIP;
    }

    SW_GET_FIELD_BY_REG(ROUTER_CTRL, WCMP_HAHS_DIP, field, data);
    if (field)
    {
        *hash_mode |= FAL_WCMP_HASH_KEY_DIP;
    }

    SW_GET_FIELD_BY_REG(ROUTER_CTRL, WCMP_HAHS_SP, field, data);
    if (field)
    {
        *hash_mode |= FAL_WCMP_HASH_KEY_SPORT;
    }

    SW_GET_FIELD_BY_REG(ROUTER_CTRL, WCMP_HAHS_DP, field, data);
    if (field)
    {
        *hash_mode |= FAL_WCMP_HASH_KEY_DPORT;
    }

    return SW_OK;
}

sw_error_t
isis_ip_reset(a_uint32_t dev_id)
{
    sw_error_t rv;
    a_uint32_t i, addr, data = 0;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isis_ip_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    HSL_REG_ENTRY_SET(rv, dev_id, HOST_ENTRY4, 0, (a_uint8_t *) (&data),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    rv = _isis_host_entry_commit(dev_id, ISIS_ENTRY_ARP, ISIS_HOST_ENTRY_FLUSH);
    SW_RTN_ON_ERROR(rv);

    isis_mac_snap[dev_id] = 0;
    for (i = 0; i < ISIS_INTF_MAC_ADDR_NUM; i++)
    {
        addr = ISIS_INTF_MAC_TBL2_ADDR + (i << 4);
        HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr, sizeof (a_uint32_t),
                              (a_uint8_t *) (&data), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }

    return SW_OK;
}

/**
 * @brief Add one host entry to one particular device.
 *   @details Comments:
 *     For ISIS the intf_id parameter in host_entry means vlan id.
       Before host entry added related interface entry and ip6 base address
       must be set at first.
       Hardware entry id will be returned.
 * @param[in] dev_id device id
 * @param[in] host_entry host entry parameter
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_ip_host_add(a_uint32_t dev_id, fal_host_entry_t * host_entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_ip_host_add(dev_id, host_entry);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Delete one host entry from one particular device.
 *   @details Comments:
 *     For ISIS the intf_id parameter in host_entry means vlan id.
       Before host entry deleted related interface entry and ip6 base address
       must be set atfirst.
       For del_mode please refer IP entry operation flags.
 * @param[in] dev_id device id
 * @param[in] del_mode delete operation mode
 * @param[in] host_entry host entry parameter
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_ip_host_del(a_uint32_t dev_id, a_uint32_t del_mode,
                 fal_host_entry_t * host_entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_ip_host_del(dev_id, del_mode, host_entry);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get one host entry from one particular device.
 *   @details Comments:
 *     For ISIS the intf_id parameter in host_entry means vlan id.
       Before host entry deleted related interface entry and ip6 base address
       must be set atfirst.
       For get_mode please refer IP entry operation flags.
 * @param[in] dev_id device id
 * @param[in] get_mode get operation mode
 * @param[out] host_entry host entry parameter
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_ip_host_get(a_uint32_t dev_id, a_uint32_t get_mode,
                 fal_host_entry_t * host_entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_ip_host_get(dev_id, get_mode, host_entry);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Next one host entry from one particular device.
 *   @details Comments:
 *     For ISIS the intf_id parameter in host_entry means vlan id.
       Before host entry deleted related interface entry and ip6 base address
       must be set atfirst.
       For next_mode please refer IP entry operation flags.
       For get the first entry please set entry id as FAL_NEXT_ENTRY_FIRST_ID
 * @param[in] dev_id device id
 * @param[in] next_mode next operation mode
 * @param[out] host_entry host entry parameter
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_ip_host_next(a_uint32_t dev_id, a_uint32_t next_mode,
                  fal_host_entry_t * host_entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_ip_host_next(dev_id, next_mode, host_entry);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Bind one counter entry to one host entry on one particular device.
 * @param[in] dev_id device id
 * @param[in] entry_id host entry id
 * @param[in] cnt_id counter entry id
 * @param[in] enable A_TRUE means bind, A_FALSE means unbind
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_ip_host_counter_bind(a_uint32_t dev_id, a_uint32_t entry_id,
                          a_uint32_t cnt_id, a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_ip_host_counter_bind(dev_id, entry_id, cnt_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Bind one pppoe session entry to one host entry on one particular device.
 * @param[in] dev_id device id
 * @param[in] entry_id host entry id
 * @param[in] pppoe_id pppoe session entry id
 * @param[in] enable A_TRUE means bind, A_FALSE means unbind
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_ip_host_pppoe_bind(a_uint32_t dev_id, a_uint32_t entry_id,
                        a_uint32_t pppoe_id, a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_ip_host_pppoe_bind(dev_id, entry_id, pppoe_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set arp packets type to learn on one particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] flags arp type FAL_ARP_LEARN_REQ and/or FAL_ARP_LEARN_ACK
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_ip_pt_arp_learn_set(a_uint32_t dev_id, fal_port_t port_id,
                         a_uint32_t flags)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_ip_pt_arp_learn_set(dev_id, port_id, flags);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get arp packets type to learn on one particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] flags arp type FAL_ARP_LEARN_REQ and/or FAL_ARP_LEARN_ACK
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_ip_pt_arp_learn_get(a_uint32_t dev_id, fal_port_t port_id,
                         a_uint32_t * flags)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_ip_pt_arp_learn_get(dev_id, port_id, flags);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set arp packets type to learn on one particular device.
 * @param[in] dev_id device id
 * @param[in] mode learning mode
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_ip_arp_learn_set(a_uint32_t dev_id, fal_arp_learn_mode_t mode)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_ip_arp_learn_set(dev_id, mode);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get arp packets type to learn on one particular device.
 * @param[in] dev_id device id
 * @param[out] mode learning mode
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_ip_arp_learn_get(a_uint32_t dev_id, fal_arp_learn_mode_t * mode)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_ip_arp_learn_get(dev_id, mode);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set ip packets source guarding mode on one particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] mode source guarding mode
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_ip_source_guard_set(a_uint32_t dev_id, fal_port_t port_id,
                         fal_source_guard_mode_t mode)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_ip_source_guard_set(dev_id, port_id, mode);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get ip packets source guarding mode on one particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] mode source guarding mode
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_ip_source_guard_get(a_uint32_t dev_id, fal_port_t port_id,
                         fal_source_guard_mode_t * mode)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_ip_source_guard_get(dev_id, port_id, mode);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set unkonw source ip packets forwarding command on one particular device.
 *   @details Comments:
 *     This settin is no meaning when ip source guard not enable
 * @param[in] dev_id device id
 * @param[in] cmd forwarding command
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_ip_unk_source_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_ip_unk_source_cmd_set(dev_id, cmd);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get unkonw source ip packets forwarding command on one particular device.
 * @param[in] dev_id device id
 * @param[out] cmd forwarding command
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_ip_unk_source_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_ip_unk_source_cmd_get(dev_id, cmd);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set arp packets source guarding mode on one particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] mode source guarding mode
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_ip_arp_guard_set(a_uint32_t dev_id, fal_port_t port_id,
                      fal_source_guard_mode_t mode)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_ip_arp_guard_set(dev_id, port_id, mode);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get arp packets source guarding mode on one particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] mode source guarding mode
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_ip_arp_guard_get(a_uint32_t dev_id, fal_port_t port_id,
                      fal_source_guard_mode_t * mode)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_ip_arp_guard_get(dev_id, port_id, mode);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set unkonw source arp packets forwarding command on one particular device.
 *   @details Comments:
 *     This settin is no meaning when arp source guard not enable
 * @param[in] dev_id device id
 * @param[in] cmd forwarding command
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_arp_unk_source_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_arp_unk_source_cmd_set(dev_id, cmd);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get unkonw source arp packets forwarding command on one particular device.
 * @param[in] dev_id device id
 * @param[out] cmd forwarding command
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_arp_unk_source_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_arp_unk_source_cmd_get(dev_id, cmd);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set IP unicast routing status on one particular device.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_ip_route_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_ip_route_status_set(dev_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get IP unicast routing status on one particular device.
 * @param[in] dev_id device id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_ip_route_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_ip_route_status_get(dev_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Add one interface entry to one particular device.
 * @param[in] dev_id device id
 * @param[in] entry interface entry
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_ip_intf_entry_add(a_uint32_t dev_id, fal_intf_mac_entry_t * entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_ip_intf_entry_add(dev_id, entry);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Delete one interface entry from one particular device.
 * @param[in] dev_id device id
 * @param[in] del_mode delete operation mode
 * @param[in] entry interface entry
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_ip_intf_entry_del(a_uint32_t dev_id, a_uint32_t del_mode,
                       fal_intf_mac_entry_t * entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_ip_intf_entry_del(dev_id, del_mode, entry);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Next one interface entry from one particular device.
 * @param[in] dev_id device id
 * @param[in] next_mode next operation mode
 * @param[out] entry interface entry
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_ip_intf_entry_next(a_uint32_t dev_id, a_uint32_t next_mode,
                        fal_intf_mac_entry_t * entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_ip_intf_entry_next(dev_id, next_mode, entry);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set IP host entry aging time on one particular device.
 * @details   Comments:
 *       This operation will set dynamic entry aging time on a particular device.
 *       The unit of time is second. Because different device has differnet
 *       hardware granularity function will return actual time in hardware.
 * @param[in] dev_id device id
 * @param[in] time aging time
 * @param[out] time actual aging time
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_ip_age_time_set(a_uint32_t dev_id, a_uint32_t * time)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_ip_age_time_set(dev_id, time);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get IP host entry aging time on one particular device.
 * @param[in] dev_id device id
 * @param[out] time aging time
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_ip_age_time_get(a_uint32_t dev_id, a_uint32_t * time)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_ip_age_time_get(dev_id, time);
    HSL_API_UNLOCK;
    return rv;
}

#if 0
/**
 * @brief Set IP WCMP table one particular device.
 *   @details Comments:
 *     Hardware only support 0 - 15 hash values and 4 different host tables.
 * @param[in] dev_id device id
 * @param[in] wcmp_id wcmp entry id
 * @param[in] wcmp wcmp entry
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_ip_wcmp_entry_set(a_uint32_t dev_id, a_uint32_t wcmp_id, fal_ip_wcmp_t * wcmp)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_ip_wcmp_entry_set(dev_id, wcmp_id, wcmp);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get IP WCMP table one particular device.
 *   @details Comments:
 *     Hardware only support 0 - 15 hash values and 4 different host tables.
 * @param[in] dev_id device id
 * @param[in] wcmp_id wcmp entry id
 * @param[out] wcmp wcmp entry
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_ip_wcmp_entry_get(a_uint32_t dev_id, a_uint32_t wcmp_id, fal_ip_wcmp_t * wcmp)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_ip_wcmp_entry_get(dev_id, wcmp_id, wcmp);
    HSL_API_UNLOCK;
    return rv;
}
#endif

/**
 * @brief Set IP WCMP hash key mode.
 * @param[in] dev_id device id
 * @param[in] hash_mode hash mode
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_ip_wcmp_hash_mode_set(a_uint32_t dev_id, a_uint32_t hash_mode)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_ip_wcmp_hash_mode_set(dev_id, hash_mode);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get IP WCMP hash key mode.
 * @param[in] dev_id device id
 * @param[out] hash_mode hash mode
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_ip_wcmp_hash_mode_get(a_uint32_t dev_id, a_uint32_t * hash_mode)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_ip_wcmp_hash_mode_get(dev_id, hash_mode);
    HSL_API_UNLOCK;
    return rv;
}

sw_error_t
isis_ip_init(a_uint32_t dev_id)
{
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    rv = isis_ip_reset(dev_id);
    SW_RTN_ON_ERROR(rv);

#ifndef HSL_STANDALONG
    {
        hsl_api_t *p_api;

        SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

        p_api->ip_host_add = isis_ip_host_add;
        p_api->ip_host_del = isis_ip_host_del;
        p_api->ip_host_get = isis_ip_host_get;
        p_api->ip_host_next = isis_ip_host_next;
        p_api->ip_host_counter_bind = isis_ip_host_counter_bind;
        p_api->ip_host_pppoe_bind = isis_ip_host_pppoe_bind;
        p_api->ip_pt_arp_learn_set = isis_ip_pt_arp_learn_set;
        p_api->ip_pt_arp_learn_get = isis_ip_pt_arp_learn_get;
        p_api->ip_arp_learn_set = isis_ip_arp_learn_set;
        p_api->ip_arp_learn_get = isis_ip_arp_learn_get;
        p_api->ip_source_guard_set = isis_ip_source_guard_set;
        p_api->ip_source_guard_get = isis_ip_source_guard_get;
        p_api->ip_unk_source_cmd_set = isis_ip_unk_source_cmd_set;
        p_api->ip_unk_source_cmd_get = isis_ip_unk_source_cmd_get;
        p_api->ip_arp_guard_set = isis_ip_arp_guard_set;
        p_api->ip_arp_guard_get = isis_ip_arp_guard_get;
        p_api->arp_unk_source_cmd_set = isis_arp_unk_source_cmd_set;
        p_api->arp_unk_source_cmd_get = isis_arp_unk_source_cmd_get;
        p_api->ip_route_status_set = isis_ip_route_status_set;
        p_api->ip_route_status_get = isis_ip_route_status_get;
        p_api->ip_intf_entry_add = isis_ip_intf_entry_add;
        p_api->ip_intf_entry_del = isis_ip_intf_entry_del;
        p_api->ip_intf_entry_next = isis_ip_intf_entry_next;
        p_api->ip_age_time_set = isis_ip_age_time_set;
        p_api->ip_age_time_get = isis_ip_age_time_get;
        p_api->ip_wcmp_hash_mode_set = isis_ip_wcmp_hash_mode_set;
        p_api->ip_wcmp_hash_mode_get = isis_ip_wcmp_hash_mode_get;
    }
#endif

    return SW_OK;
}

/**
 * @}
 */

