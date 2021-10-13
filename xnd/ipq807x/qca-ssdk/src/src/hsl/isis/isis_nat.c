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
 * @defgroup isis_ip ISIS_NAT
 * @{
 */

#include "sw.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_port_prop.h"
#include "isis_nat.h"
#include "isis_reg.h"
#if defined(IN_NAT_HELPER)
#include "isis_nat_helper.h"
#endif


#define ISIS_HOST_ENTRY_DATA0_ADDR              0x0e48
#define ISIS_HOST_ENTRY_DATA1_ADDR              0x0e4c
#define ISIS_HOST_ENTRY_DATA2_ADDR              0x0e50
#define ISIS_HOST_ENTRY_DATA3_ADDR              0x0e54
#define ISIS_HOST_ENTRY_DATA4_ADDR              0x0e58

#define ISIS_NAT_ENTRY_FLUSH                    1
#define ISIS_NAT_ENTRY_ADD                      2
#define ISIS_NAT_ENTRY_DEL                      3
#define ISIS_NAT_ENTRY_NEXT                     4
#define ISIS_NAT_ENTRY_SEARCH                   5

#define ISIS_ENTRY_NAPT                         0
#define ISIS_ENTRY_NAT                          2
#define ISIS_ENTRY_ARP                          3

#define ISIS_PUB_ADDR_NUM                       16
#define ISIS_PUB_ADDR_TBL0_ADDR                 0x5aa00
#define ISIS_PUB_ADDR_TBL1_ADDR                 0x5aa04
#define ISIS_PUB_ADDR_EDIT0_ADDR                0x02100
#define ISIS_PUB_ADDR_EDIT1_ADDR                0x02104
#define ISIS_PUB_ADDR_OFFLOAD_ADDR              0x2a000
#define ISIS_PUB_ADDR_VALID_ADDR                0x2a040

#define ISIS_NAT_ENTRY_NUM                      32
#define ISIS_NAPT_ENTRY_NUM                     1024

#define ISIS_NAT_COUTER_ADDR                    0x2b000

#define ISIS_NAT_PORT_NUM                       255

static a_uint32_t isis_nat_snap[SW_MAX_NR_DEV] = { 0 };
extern a_uint32_t isis_nat_global_status;

static sw_error_t
_isis_nat_feature_check(a_uint32_t dev_id)
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
_isis_ip_prvaddr_sw_to_hw(a_uint32_t dev_id, fal_ip4_addr_t sw_addr,
                          a_uint32_t * hw_addr)
{
    sw_error_t rv;
    a_uint32_t data = 0;

    HSL_REG_FIELD_GET(rv, dev_id, PRVIP_CTL, 0, BASEADDR_SEL,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (data)
    {
        *hw_addr = (sw_addr & 0xff) | (((sw_addr >> 16) & 0xf) << 8);
    }
    else
    {
        *hw_addr = sw_addr & 0xfff;
    }

    return SW_OK;
}

static sw_error_t
_isis_ip_prvaddr_hw_to_sw(a_uint32_t dev_id, a_uint32_t hw_addr,
                          fal_ip4_addr_t * sw_addr)
{
    sw_error_t rv;
    a_uint32_t data = 0, addr = 0;

    HSL_REG_FIELD_GET(rv, dev_id, PRVIP_CTL, 0, BASEADDR_SEL,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    HSL_REG_FIELD_GET(rv, dev_id, PRVIP_CTL, 0, IP4_BASEADDR,
                      (a_uint8_t *) (&addr), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (data)
    {
        *sw_addr = ((addr & 0xff) << 8) | (((addr >> 8) & 0xfff) << 8)
                   | (hw_addr & 0xff) | (((hw_addr >> 8) & 0xf) << 16);
    }
    else
    {
        *sw_addr = (addr << 12) | (hw_addr & 0xfff);
    }

    return SW_OK;
}

static sw_error_t
_isis_nat_counter_get(a_uint32_t dev_id, a_uint32_t cnt_id,
                      a_uint32_t counter[4])
{
    sw_error_t rv;
    a_uint32_t i, addr;

    addr = ISIS_NAT_COUTER_ADDR + (cnt_id << 4);
    for (i = 0; i < 4; i++)
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
_isis_nat_entry_commit(a_uint32_t dev_id, a_uint32_t entry_type, a_uint32_t op)
{
    a_uint32_t busy = 1, i = 0x100, entry = 0;
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

    busy = 1;
    i = 0x100;
    while (busy && --i)
    {
        HSL_REG_ENTRY_GET(rv, dev_id, HOST_ENTRY4, 0, (a_uint8_t *) (&entry),
                          sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
        SW_GET_FIELD_BY_REG(HOST_ENTRY4, TBL_BUSY, busy, entry);
        aos_udelay(500);
#if 1
        if(ISIS_NAT_ENTRY_SEARCH == op &&  busy) break;
#endif
    }

    if (i == 0)
    {
        printk("%s BUSY\n", __FUNCTION__);
        return SW_BUSY;
    }

    /* hardware requirement, we should delay... */
    if ((ISIS_NAT_ENTRY_FLUSH == op) && (ISIS_ENTRY_NAPT == entry_type))
    {
        aos_mdelay(10);
    }

    /* hardware requirement, we should read again... */
    HSL_REG_ENTRY_GET(rv, dev_id, HOST_ENTRY4, 0, (a_uint8_t *) (&entry),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    SW_GET_FIELD_BY_REG(HOST_ENTRY4, TBL_STAUS, busy, entry);
    if (!busy)
    {
        if (ISIS_NAT_ENTRY_NEXT == op)
        {
            return SW_NO_MORE;
        }
        else if (ISIS_NAT_ENTRY_SEARCH == op)
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
_isis_nat_sw_to_hw(a_uint32_t dev_id, fal_nat_entry_t * entry, a_uint32_t reg[])
{
    sw_error_t rv;
    a_uint32_t data;

    if (FAL_NAT_ENTRY_TRANS_IPADDR_INDEX & entry->flags)
    {
        return SW_BAD_PARAM;
    }

    reg[0] = entry->trans_addr;

    if (FAL_NAT_ENTRY_PORT_CHECK & entry->flags)
    {
        SW_SET_REG_BY_FIELD(NAT_ENTRY2, PORT_EN, 1, reg[2]);
        SW_SET_REG_BY_FIELD(NAT_ENTRY1, PORT_RANGE, entry->port_range, reg[1]);
        if (ISIS_NAT_PORT_NUM < entry->port_range)
        {
            return SW_BAD_PARAM;
        }
        SW_SET_REG_BY_FIELD(NAT_ENTRY1, PORT_NUM, entry->port_num, reg[1]);
    }
    else
    {
        SW_SET_REG_BY_FIELD(NAT_ENTRY2, PORT_EN, 0, reg[2]);
    }

    rv = _isis_ip_prvaddr_sw_to_hw(dev_id, entry->src_addr, &data);
    SW_RTN_ON_ERROR(rv);

    SW_SET_REG_BY_FIELD(NAT_ENTRY1, PRV_IPADDR0, data, reg[1]);
    SW_SET_REG_BY_FIELD(NAT_ENTRY2, PRV_IPADDR1, (data >> 8), reg[2]);

    if (FAL_MAC_FRWRD == entry->action)
    {
        if (A_TRUE == entry->mirror_en)
        {
            SW_SET_REG_BY_FIELD(NAT_ENTRY2, ACTION, 0, reg[2]);
        }
        else
        {
            SW_SET_REG_BY_FIELD(NAT_ENTRY2, ACTION, 3, reg[2]);
        }
    }
    else if (FAL_MAC_CPY_TO_CPU == entry->action)
    {
        SW_SET_REG_BY_FIELD(NAT_ENTRY2, ACTION, 2, reg[2]);
    }
    else if (FAL_MAC_RDT_TO_CPU == entry->action)
    {
        SW_SET_REG_BY_FIELD(NAT_ENTRY2, ACTION, 1, reg[2]);
    }
    else
    {
        return SW_NOT_SUPPORTED;
    }

    if (A_TRUE == entry->counter_en)
    {
        SW_SET_REG_BY_FIELD(NAT_ENTRY2, CNT_EN, 1, reg[2]);
        SW_SET_REG_BY_FIELD(NAT_ENTRY2, CNT_IDX, entry->counter_id, reg[2]);
    }

    if (FAL_NAT_ENTRY_PROTOCOL_ANY & entry->flags)
    {
        data = 3;
    }
    else if ((FAL_NAT_ENTRY_PROTOCOL_TCP & entry->flags)
             && (FAL_NAT_ENTRY_PROTOCOL_UDP & entry->flags))
    {
        data = 2;
    }
    else if (FAL_NAT_ENTRY_PROTOCOL_TCP & entry->flags)
    {
        data = 0;
    }
    else if (FAL_NAT_ENTRY_PROTOCOL_UDP & entry->flags)
    {
        data = 1;
    }
    else
    {
        return SW_BAD_PARAM;
    }

    SW_SET_REG_BY_FIELD(NAT_ENTRY2, PRO_TYP, data, reg[2]);

    SW_SET_REG_BY_FIELD(NAT_ENTRY2, HASH_KEY, entry->slct_idx, reg[2]);

    SW_SET_REG_BY_FIELD(NAT_ENTRY2, ENTRY_VALID, 1, reg[2]);
    SW_SET_REG_BY_FIELD(HOST_ENTRY4, TBL_IDX, entry->entry_id, reg[4]);
    return SW_OK;
}

static sw_error_t
_isis_nat_hw_to_sw(a_uint32_t dev_id, a_uint32_t reg[], fal_nat_entry_t * entry)
{
    sw_error_t rv;
    a_uint32_t data, cnt[4] = {0};

    entry->trans_addr = reg[0];

    SW_GET_FIELD_BY_REG(NAT_ENTRY2, PORT_EN, data, reg[2]);
    if (data)
    {
        entry->flags |= FAL_NAT_ENTRY_PORT_CHECK;
        SW_GET_FIELD_BY_REG(NAT_ENTRY1, PORT_RANGE, data, reg[1]);
        entry->port_range = data;
        SW_GET_FIELD_BY_REG(NAT_ENTRY1, PORT_NUM, data, reg[1]);
        entry->port_num = data;
    }

    SW_GET_FIELD_BY_REG(NAT_ENTRY1, PRV_IPADDR0, data, reg[1]);
    entry->src_addr = data;
    SW_GET_FIELD_BY_REG(NAT_ENTRY2, PRV_IPADDR1, data, reg[2]);
    data = (entry->src_addr & 0xff) | (data << 8);

    rv = _isis_ip_prvaddr_hw_to_sw(dev_id, data, &(entry->src_addr));
    SW_RTN_ON_ERROR(rv);

    SW_GET_FIELD_BY_REG(NAT_ENTRY2, ACTION, data, reg[2]);
    entry->action = FAL_MAC_FRWRD;
    if (0 == data)
    {
        entry->mirror_en = A_TRUE;
    }
    else if (2 == data)
    {
        entry->action = FAL_MAC_CPY_TO_CPU;
    }
    else if (1 == data)
    {
        entry->action = FAL_MAC_RDT_TO_CPU;
    }

    SW_GET_FIELD_BY_REG(NAT_ENTRY2, CNT_EN, data, reg[2]);
    if (data)
    {
        entry->counter_en = A_TRUE;
        SW_GET_FIELD_BY_REG(NAT_ENTRY2, CNT_IDX, entry->counter_id, reg[2]);

        rv = _isis_nat_counter_get(dev_id, entry->counter_id, cnt);
        SW_RTN_ON_ERROR(rv);

        entry->ingress_packet = cnt[0];
        entry->ingress_byte = cnt[1];
        entry->egress_packet = cnt[2];
        entry->egress_byte = cnt[3];
    }
    else
    {
        entry->counter_en = A_FALSE;
    }

    SW_GET_FIELD_BY_REG(NAT_ENTRY2, PRO_TYP, data, reg[2]);
    if (3 == data)
    {
        entry->flags |= FAL_NAT_ENTRY_PROTOCOL_ANY;
    }
    else if (2 == data)
    {
        entry->flags |= FAL_NAT_ENTRY_PROTOCOL_TCP;
        entry->flags |= FAL_NAT_ENTRY_PROTOCOL_UDP;
    }
    else if (1 == data)
    {
        entry->flags |= FAL_NAT_ENTRY_PROTOCOL_UDP;
    }
    else if (0 == data)
    {
        entry->flags |= FAL_NAT_ENTRY_PROTOCOL_TCP;
    }

    SW_GET_FIELD_BY_REG(NAT_ENTRY2, HASH_KEY, data, reg[2]);
    entry->slct_idx = data;

    return SW_OK;
}

static sw_error_t
_isis_napt_sw_to_hw(a_uint32_t dev_id, fal_napt_entry_t * entry,
                    a_uint32_t reg[])
{
    sw_error_t rv;
    a_uint32_t data;

    reg[0] = entry->dst_addr;

    SW_SET_REG_BY_FIELD(NAPT_ENTRY1, DST_PORT, entry->dst_port, reg[1]);
    SW_SET_REG_BY_FIELD(NAPT_ENTRY1, SRC_PORT, entry->src_port, reg[1]);
    SW_SET_REG_BY_FIELD(NAPT_ENTRY2, TRANS_PORT, entry->trans_port, reg[2]);

    rv = _isis_ip_prvaddr_sw_to_hw(dev_id, entry->src_addr, &data);
    SW_RTN_ON_ERROR(rv);
    SW_SET_REG_BY_FIELD(NAPT_ENTRY2, SRC_IPADDR, data, reg[2]);

    if (!(FAL_NAT_ENTRY_TRANS_IPADDR_INDEX & entry->flags))
    {
        return SW_BAD_PARAM;
    }
    SW_SET_REG_BY_FIELD(NAPT_ENTRY2, TRANS_IPADDR, entry->trans_addr, reg[2]);

    if (FAL_MAC_FRWRD == entry->action)
    {
        if (A_TRUE == entry->mirror_en)
        {
            SW_SET_REG_BY_FIELD(NAPT_ENTRY3, ACTION, 0, reg[3]);
        }
        else
        {
            SW_SET_REG_BY_FIELD(NAPT_ENTRY3, ACTION, 3, reg[3]);
        }
    }
    else if (FAL_MAC_CPY_TO_CPU == entry->action)
    {
        SW_SET_REG_BY_FIELD(NAPT_ENTRY3, ACTION, 2, reg[3]);
    }
    else if (FAL_MAC_RDT_TO_CPU == entry->action)
    {
        SW_SET_REG_BY_FIELD(NAPT_ENTRY3, ACTION, 1, reg[3]);
    }
    else
    {
        return SW_NOT_SUPPORTED;
    }

    if (A_TRUE == entry->counter_en)
    {
        SW_SET_REG_BY_FIELD(NAPT_ENTRY3, CNT_EN, 1, reg[3]);
        SW_SET_REG_BY_FIELD(NAPT_ENTRY3, CNT_IDX, entry->counter_id, reg[3]);
    }

    data = 2;
    if (FAL_NAT_ENTRY_PROTOCOL_TCP & entry->flags)
    {
        data = 0;
    }
    else if (FAL_NAT_ENTRY_PROTOCOL_UDP & entry->flags)
    {
        data = 1;
    }
    else if (FAL_NAT_ENTRY_PROTOCOL_PPTP & entry->flags)
    {
        data = 3;
    }
    SW_SET_REG_BY_FIELD(NAPT_ENTRY3, PROT_TYP, data, reg[3]);

    SW_SET_REG_BY_FIELD(NAPT_ENTRY3, AGE_FLAG, entry->status, reg[3]);
    return SW_OK;
}

static sw_error_t
_isis_napt_hw_to_sw(a_uint32_t dev_id, a_uint32_t reg[],
                    fal_napt_entry_t * entry)
{
    sw_error_t rv;
    a_uint32_t data, cnt[4] = {0};

    entry->dst_addr = reg[0];

    SW_GET_FIELD_BY_REG(NAPT_ENTRY1, DST_PORT, entry->dst_port, reg[1]);
    SW_GET_FIELD_BY_REG(NAPT_ENTRY1, SRC_PORT, entry->src_port, reg[1]);
    SW_GET_FIELD_BY_REG(NAPT_ENTRY2, TRANS_PORT, entry->trans_port, reg[2]);

    SW_GET_FIELD_BY_REG(NAPT_ENTRY2, SRC_IPADDR, data, reg[2]);
    rv = _isis_ip_prvaddr_hw_to_sw(dev_id, data, &(entry->src_addr));
    SW_RTN_ON_ERROR(rv);

    entry->flags |= FAL_NAT_ENTRY_TRANS_IPADDR_INDEX;
    SW_GET_FIELD_BY_REG(NAPT_ENTRY2, TRANS_IPADDR, entry->trans_addr, reg[2]);

    SW_GET_FIELD_BY_REG(NAPT_ENTRY3, ACTION, data, reg[3]);
    entry->action = FAL_MAC_FRWRD;
    if (0 == data)
    {
        entry->mirror_en = A_TRUE;
    }
    else if (2 == data)
    {
        entry->action = FAL_MAC_CPY_TO_CPU;
    }
    else if (1 == data)
    {
        entry->action = FAL_MAC_RDT_TO_CPU;
    }

    SW_GET_FIELD_BY_REG(NAPT_ENTRY3, CNT_EN, data, reg[3]);
    if (data)
    {
        entry->counter_en = A_TRUE;
        SW_GET_FIELD_BY_REG(NAPT_ENTRY3, CNT_IDX, entry->counter_id, reg[3]);

        rv = _isis_nat_counter_get(dev_id, entry->counter_id, cnt);
        SW_RTN_ON_ERROR(rv);

        entry->ingress_packet = cnt[0];
        entry->ingress_byte = cnt[1];
        entry->egress_packet = cnt[2];
        entry->egress_byte = cnt[3];
    }
    else
    {
        entry->counter_en = A_FALSE;
    }

    SW_GET_FIELD_BY_REG(NAPT_ENTRY3, PROT_TYP, data, reg[3]);
    if (0 == data)
    {
        entry->flags |= FAL_NAT_ENTRY_PROTOCOL_TCP;
    }
    else if (1 == data)
    {
        entry->flags |= FAL_NAT_ENTRY_PROTOCOL_UDP;
    }
    else if (3 == data)
    {
        entry->flags |= FAL_NAT_ENTRY_PROTOCOL_PPTP;
    }

    SW_GET_FIELD_BY_REG(NAPT_ENTRY3, AGE_FLAG, entry->status, reg[3]);
    return SW_OK;
}

static sw_error_t
_isis_nat_down_to_hw(a_uint32_t dev_id, a_uint32_t reg[])
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
_isis_nat_up_to_sw(a_uint32_t dev_id, a_uint32_t reg[])
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
_isis_nat_add(a_uint32_t dev_id, fal_nat_entry_t * entry)
{
    sw_error_t rv;
    a_uint32_t i, reg[5] = { 0 };

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isis_nat_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    for (i = 0; i < ISIS_NAT_ENTRY_NUM; i++)
    {
        if (!(isis_nat_snap[dev_id] & (0x1 << i)))
        {
            break;
        }
    }

    if (ISIS_NAT_ENTRY_NUM == i)
    {
        return SW_NO_RESOURCE;
    }

    entry->entry_id = i;

    rv = _isis_nat_sw_to_hw(dev_id, entry, reg);
    SW_RTN_ON_ERROR(rv);

    rv = _isis_nat_down_to_hw(dev_id, reg);
    SW_RTN_ON_ERROR(rv);

    rv = _isis_nat_entry_commit(dev_id, ISIS_ENTRY_NAT, ISIS_NAT_ENTRY_ADD);
    SW_RTN_ON_ERROR(rv);

    isis_nat_snap[dev_id] |= (0x1 << i);
    entry->entry_id = i;
    return SW_OK;
}

static sw_error_t
_isis_nat_del(a_uint32_t dev_id, a_uint32_t del_mode, fal_nat_entry_t * entry)
{
    sw_error_t rv;
    a_uint32_t reg[5] = { 0 };

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isis_nat_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    if (FAL_NAT_ENTRY_ID_EN & del_mode)
    {
        SW_SET_REG_BY_FIELD(HOST_ENTRY4, ENTRY_FUNC, ISIS_NAT_ENTRY_DEL, reg[4]);
        SW_SET_REG_BY_FIELD(HOST_ENTRY4, TBL_IDX, entry->entry_id, reg[4]);

        rv = _isis_nat_down_to_hw(dev_id, reg);
        SW_RTN_ON_ERROR(rv);

        rv = _isis_nat_entry_commit(dev_id, ISIS_ENTRY_NAT, ISIS_NAT_ENTRY_DEL);
        SW_RTN_ON_ERROR(rv);

        isis_nat_snap[dev_id] &= (~(0x1 << entry->entry_id));
    }
    else
    {
        rv = _isis_nat_down_to_hw(dev_id, reg);
        SW_RTN_ON_ERROR(rv);

        rv = _isis_nat_entry_commit(dev_id, ISIS_ENTRY_NAT, ISIS_NAT_ENTRY_FLUSH);
        SW_RTN_ON_ERROR(rv);

        isis_nat_snap[dev_id] = 0;
    }

    return SW_OK;
}

static sw_error_t
_isis_nat_get(a_uint32_t dev_id, a_uint32_t get_mode, fal_nat_entry_t * entry)
{
    sw_error_t rv;
    a_uint32_t reg[5] = { 0 };

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isis_nat_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    if (FAL_NAT_ENTRY_ID_EN != get_mode)
    {
        return SW_NOT_SUPPORTED;
    }

    if (!(isis_nat_snap[dev_id] & (0x1 << entry->entry_id)))
    {
        return SW_NOT_FOUND;
    }

    SW_SET_REG_BY_FIELD(HOST_ENTRY4, TBL_IDX, entry->entry_id, reg[4]);

    rv = _isis_nat_down_to_hw(dev_id, reg);
    SW_RTN_ON_ERROR(rv);

    rv = _isis_nat_entry_commit(dev_id, ISIS_ENTRY_NAT, ISIS_NAT_ENTRY_SEARCH);
    SW_RTN_ON_ERROR(rv);

    rv = _isis_nat_up_to_sw(dev_id, reg);
    SW_RTN_ON_ERROR(rv);

    rv = _isis_nat_hw_to_sw(dev_id, reg, entry);
    return rv;
}

static sw_error_t
_isis_nat_next(a_uint32_t dev_id, a_uint32_t next_mode,
               fal_nat_entry_t * nat_entry)
{
    a_uint32_t i, idx, reg[5] = { 0 };
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isis_nat_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    if (FAL_NEXT_ENTRY_FIRST_ID == nat_entry->entry_id)
    {
        idx = 0;
    }
    else
    {
        if ((ISIS_NAT_ENTRY_NUM - 1) == nat_entry->entry_id)
        {
            return SW_NO_MORE;
        }
        else
        {
            idx = nat_entry->entry_id + 1;
        }
    }

    for (i = idx; i < ISIS_NAT_ENTRY_NUM; i++)
    {
        if (isis_nat_snap[dev_id] & (0x1 << i))
        {
            break;
        }
    }

    if (ISIS_NAT_ENTRY_NUM == i)
    {
        return SW_NO_MORE;
    }

    SW_SET_REG_BY_FIELD(HOST_ENTRY4, TBL_IDX, i, reg[4]);

    rv = _isis_nat_down_to_hw(dev_id, reg);
    SW_RTN_ON_ERROR(rv);

    rv = _isis_nat_entry_commit(dev_id, ISIS_ENTRY_NAT, ISIS_NAT_ENTRY_SEARCH);
    SW_RTN_ON_ERROR(rv);

    rv = _isis_nat_up_to_sw(dev_id, reg);
    SW_RTN_ON_ERROR(rv);

    aos_mem_zero(nat_entry, sizeof (fal_nat_entry_t));

    rv = _isis_nat_hw_to_sw(dev_id, reg, nat_entry);
    SW_RTN_ON_ERROR(rv);

    nat_entry->entry_id = i;
    return SW_OK;
}

static sw_error_t
_isis_nat_counter_bind(a_uint32_t dev_id, a_uint32_t entry_id,
                       a_uint32_t cnt_id, a_bool_t enable)
{
    sw_error_t rv;
    a_uint32_t reg[5] = { 0 };

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isis_nat_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    if (!(isis_nat_snap[dev_id] & (0x1 << entry_id)))
    {
        return SW_NOT_FOUND;
    }

    SW_SET_REG_BY_FIELD(HOST_ENTRY4, TBL_IDX, entry_id, reg[4]);

    rv = _isis_nat_entry_commit(dev_id, ISIS_ENTRY_NAT, ISIS_NAT_ENTRY_SEARCH);
    SW_RTN_ON_ERROR(rv);

    rv = _isis_nat_up_to_sw(dev_id, reg);
    SW_RTN_ON_ERROR(rv);

    if (A_FALSE == enable)
    {
        SW_SET_REG_BY_FIELD(NAT_ENTRY2, CNT_EN, 0, reg[2]);
    }
    else if (A_TRUE == enable)
    {
        SW_SET_REG_BY_FIELD(NAT_ENTRY2, CNT_EN, 1, reg[2]);
        SW_SET_REG_BY_FIELD(NAT_ENTRY2, CNT_IDX, cnt_id, reg[2]);
    }
    else
    {
        return SW_BAD_PARAM;
    }

    /* needn't set TBL_IDX, keep hardware register value */

    rv = _isis_nat_entry_commit(dev_id, ISIS_ENTRY_NAT, ISIS_NAT_ENTRY_DEL);
    SW_RTN_ON_ERROR(rv);

    rv = _isis_nat_down_to_hw(dev_id, reg);
    SW_RTN_ON_ERROR(rv);

    /* needn't set TBL_IDX, keep hardware register value */

    rv = _isis_nat_entry_commit(dev_id, ISIS_ENTRY_NAT, ISIS_NAT_ENTRY_ADD);
    return rv;
}

static sw_error_t
_isis_napt_add(a_uint32_t dev_id, fal_napt_entry_t * entry)
{
    sw_error_t rv;
    a_uint32_t reg[5] = { 0 };

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isis_nat_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isis_napt_sw_to_hw(dev_id, entry, reg);
    SW_RTN_ON_ERROR(rv);

    rv = _isis_nat_down_to_hw(dev_id, reg);
    SW_RTN_ON_ERROR(rv);

    rv = _isis_nat_entry_commit(dev_id, ISIS_ENTRY_NAPT, ISIS_NAT_ENTRY_ADD);
    SW_RTN_ON_ERROR(rv);

    rv = _isis_nat_up_to_sw(dev_id, reg);
    SW_RTN_ON_ERROR(rv);

    SW_GET_FIELD_BY_REG(HOST_ENTRY4, TBL_IDX, entry->entry_id, reg[4]);
    return SW_OK;
}

static sw_error_t
_isis_napt_del(a_uint32_t dev_id, a_uint32_t del_mode, fal_napt_entry_t * entry)
{
    sw_error_t rv;
    a_uint32_t data, reg[5] = { 0 };

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isis_nat_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    if (FAL_NAT_ENTRY_ID_EN & del_mode)
    {
        return SW_NOT_SUPPORTED;
    }

    if (FAL_NAT_ENTRY_KEY_EN & del_mode)
    {
        rv = _isis_napt_sw_to_hw(dev_id, entry, reg);
        SW_RTN_ON_ERROR(rv);

        rv = _isis_nat_down_to_hw(dev_id, reg);
        SW_RTN_ON_ERROR(rv);

        rv = _isis_nat_entry_commit(dev_id, ISIS_ENTRY_NAPT, ISIS_NAT_ENTRY_DEL);
        SW_RTN_ON_ERROR(rv);

        rv = _isis_nat_up_to_sw(dev_id, reg);
        SW_RTN_ON_ERROR(rv);

        SW_GET_FIELD_BY_REG(HOST_ENTRY4, TBL_IDX, entry->entry_id, reg[4]);
        return SW_OK;
    }
    else
    {
        if (FAL_NAT_ENTRY_PUBLIC_IP_EN & del_mode)
        {
            SW_SET_REG_BY_FIELD(HOST_ENTRY4, SPEC_PIP, 1, reg[4]);
            SW_SET_REG_BY_FIELD(NAPT_ENTRY2, TRANS_IPADDR, entry->trans_addr, reg[2]);
        }

        if (FAL_NAT_ENTRY_SOURCE_IP_EN & del_mode)
        {
            SW_SET_REG_BY_FIELD(HOST_ENTRY4, SPEC_SIP, 1, reg[4]);
            rv = _isis_ip_prvaddr_sw_to_hw(dev_id, entry->src_addr, &data);
            SW_RTN_ON_ERROR(rv);
            SW_SET_REG_BY_FIELD(NAPT_ENTRY2, SRC_IPADDR, data, reg[2]);
        }

        if (FAL_NAT_ENTRY_AGE_EN & del_mode)
        {
            SW_SET_REG_BY_FIELD(HOST_ENTRY4, SPEC_STATUS, 1, reg[4]);
            SW_SET_REG_BY_FIELD(NAPT_ENTRY3, AGE_FLAG, entry->status, reg[3]);
        }

        rv = _isis_nat_down_to_hw(dev_id, reg);
        SW_RTN_ON_ERROR(rv);

        rv = _isis_nat_entry_commit(dev_id, ISIS_ENTRY_NAPT, ISIS_NAT_ENTRY_FLUSH);
        return rv;
    }
}

static sw_error_t
_isis_napt_get(a_uint32_t dev_id, a_uint32_t get_mode, fal_napt_entry_t * entry)
{
    sw_error_t rv;
    a_uint32_t found, age, reg[5] = { 0 };

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isis_nat_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

#if 0
    if (FAL_NAT_ENTRY_ID_EN != get_mode)
    {
        return SW_NOT_SUPPORTED;
    }

    SW_SET_REG_BY_FIELD(HOST_ENTRY4, TBL_IDX, entry->entry_id, reg[4]);
#else
    rv = _isis_napt_sw_to_hw(dev_id, entry, reg);
    SW_RTN_ON_ERROR(rv);
#endif

    rv = _isis_nat_down_to_hw(dev_id, reg);
    SW_RTN_ON_ERROR(rv);

    rv = _isis_nat_entry_commit(dev_id, ISIS_ENTRY_NAPT, ISIS_NAT_ENTRY_SEARCH);
    SW_RTN_ON_ERROR(rv);

    rv = _isis_nat_up_to_sw(dev_id, reg);
    SW_RTN_ON_ERROR(rv);

    SW_GET_FIELD_BY_REG(HOST_ENTRY4, TBL_STAUS, found, reg[4]);
    SW_GET_FIELD_BY_REG(NAPT_ENTRY3, AGE_FLAG,  age, reg[3]);
    if (found && age)
    {
        found = 1;
    }
    else
    {
        found = 0;
    }

    rv = _isis_napt_hw_to_sw(dev_id, reg, entry);
    SW_RTN_ON_ERROR(rv);

    if (!found)
    {
        return SW_NOT_FOUND;
    }

    SW_GET_FIELD_BY_REG(HOST_ENTRY4, TBL_IDX,   entry->entry_id, reg[4]);
    return SW_OK;
}

static sw_error_t
_isis_napt_next(a_uint32_t dev_id, a_uint32_t next_mode,
                fal_napt_entry_t * napt_entry)
{
    a_uint32_t data, idx, reg[5] = { 0 };
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isis_nat_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    if (FAL_NEXT_ENTRY_FIRST_ID == napt_entry->entry_id)
    {
        idx = ISIS_NAPT_ENTRY_NUM - 1;
    }
    else
    {
        if ((ISIS_NAPT_ENTRY_NUM - 1) == napt_entry->entry_id)
        {
            return SW_NO_MORE;
        }
        else
        {
            idx = napt_entry->entry_id;
        }
    }

    if (FAL_NAT_ENTRY_PUBLIC_IP_EN & next_mode)
    {
        SW_SET_REG_BY_FIELD(HOST_ENTRY4, SPEC_PIP, 1, reg[4]);
        SW_SET_REG_BY_FIELD(NAPT_ENTRY2, TRANS_IPADDR, napt_entry->trans_addr, reg[2]);
    }

    if (FAL_NAT_ENTRY_SOURCE_IP_EN & next_mode)
    {
        SW_SET_REG_BY_FIELD(HOST_ENTRY4, SPEC_SIP, 1, reg[4]);
        rv = _isis_ip_prvaddr_sw_to_hw(dev_id, napt_entry->src_addr, &data);
        SW_RTN_ON_ERROR(rv);
        SW_SET_REG_BY_FIELD(NAPT_ENTRY2, SRC_IPADDR, data, reg[2]);
    }

    if (FAL_NAT_ENTRY_AGE_EN & next_mode)
    {
        SW_SET_REG_BY_FIELD(HOST_ENTRY4, SPEC_STATUS, 1, reg[4]);
        SW_SET_REG_BY_FIELD(NAPT_ENTRY3, AGE_FLAG, napt_entry->status, reg[3]);
    }

    SW_SET_REG_BY_FIELD(HOST_ENTRY4, TBL_IDX, idx, reg[4]);

    rv = _isis_nat_down_to_hw(dev_id, reg);
    SW_RTN_ON_ERROR(rv);

    rv = _isis_nat_entry_commit(dev_id, ISIS_ENTRY_NAPT, ISIS_NAT_ENTRY_NEXT);
    SW_RTN_ON_ERROR(rv);

    rv = _isis_nat_up_to_sw(dev_id, reg);
    SW_RTN_ON_ERROR(rv);

    aos_mem_zero(napt_entry, sizeof (fal_nat_entry_t));

    rv = _isis_napt_hw_to_sw(dev_id, reg, napt_entry);
    SW_RTN_ON_ERROR(rv);

#if 0
    a_uint32_t temp=0, complete=0;

    HSL_REG_ENTRY_GET(rv, dev_id, HOST_ENTRY4, 0, (a_uint8_t *) (&temp),
                      sizeof (a_uint32_t));

    SW_GET_FIELD_BY_REG(HOST_ENTRY4, TBL_STAUS, complete, temp);

    if (!complete)
    {
        return SW_NO_MORE;
    }
#endif

    SW_GET_FIELD_BY_REG(HOST_ENTRY4, TBL_IDX, napt_entry->entry_id, reg[4]);
    return SW_OK;
}

static sw_error_t
_isis_napt_counter_bind(a_uint32_t dev_id, a_uint32_t entry_id,
                        a_uint32_t cnt_id, a_bool_t enable)
{
    sw_error_t rv;
    a_uint32_t reg[5] = { 0 }, tbl_idx;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isis_nat_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    tbl_idx = (entry_id - 1) & 0x3ff;
    SW_SET_REG_BY_FIELD(HOST_ENTRY4, TBL_IDX, tbl_idx, reg[4]);

    rv = _isis_nat_down_to_hw(dev_id, reg);
    SW_RTN_ON_ERROR(rv);

    rv = _isis_nat_entry_commit(dev_id, ISIS_ENTRY_NAPT, ISIS_NAT_ENTRY_NEXT);
    if (SW_OK != rv)
    {
        return SW_NOT_FOUND;
    }

    rv = _isis_nat_up_to_sw(dev_id, reg);
    SW_RTN_ON_ERROR(rv);

    SW_GET_FIELD_BY_REG(HOST_ENTRY4, TBL_IDX, tbl_idx, reg[4]);
    if (entry_id != tbl_idx)
    {
        return SW_NOT_FOUND;
    }

    if (A_FALSE == enable)
    {
        SW_SET_REG_BY_FIELD(NAPT_ENTRY3, CNT_EN, 0, reg[3]);
    }
    else if (A_TRUE == enable)
    {
        SW_SET_REG_BY_FIELD(NAPT_ENTRY3, CNT_EN, 1, reg[3]);
        SW_SET_REG_BY_FIELD(NAPT_ENTRY3, CNT_IDX, cnt_id, reg[3]);
    }
    else
    {
        return SW_BAD_PARAM;
    }

    rv = _isis_nat_entry_commit(dev_id, ISIS_ENTRY_NAPT, ISIS_NAT_ENTRY_DEL);
    SW_RTN_ON_ERROR(rv);

    reg[4] = 0x0;
    rv = _isis_nat_down_to_hw(dev_id, reg);
    SW_RTN_ON_ERROR(rv);

    rv = _isis_nat_entry_commit(dev_id, ISIS_ENTRY_NAPT, ISIS_NAT_ENTRY_ADD);
    return rv;
}

static sw_error_t
_isis_nat_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;
    a_uint32_t data;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isis_nat_feature_check(dev_id);
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

    HSL_REG_FIELD_SET(rv, dev_id, NAT_CTRL, 0, NAT_EN,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_isis_nat_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;
    a_uint32_t data = 0;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isis_nat_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    HSL_REG_FIELD_GET(rv, dev_id, NAT_CTRL, 0, NAT_EN,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (data)
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
_isis_napt_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;
    a_uint32_t data;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isis_nat_feature_check(dev_id);
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

    HSL_REG_FIELD_SET(rv, dev_id, NAT_CTRL, 0, NAPT_EN,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_isis_napt_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;
    a_uint32_t data = 0;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isis_nat_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    HSL_REG_FIELD_GET(rv, dev_id, NAT_CTRL, 0, NAPT_EN,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (data)
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
_isis_napt_mode_set(a_uint32_t dev_id, fal_napt_mode_t mode)
{
    sw_error_t rv;
    a_uint32_t data;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isis_nat_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    if (FAL_NAPT_FULL_CONE == mode)
    {
        data = 0;
    }
    else if (FAL_NAPT_STRICT_CONE == mode)
    {
        data = 1;
    }
    else if ((FAL_NAPT_PORT_STRICT == mode)
             || (FAL_NAPT_SYNMETRIC == mode))
    {
        data = 2;
    }
    else
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_SET(rv, dev_id, NAT_CTRL, 0, NAPT_MODE,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_isis_napt_mode_get(a_uint32_t dev_id, fal_napt_mode_t * mode)
{
    sw_error_t rv;
    a_uint32_t data = 0;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isis_nat_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    HSL_REG_FIELD_GET(rv, dev_id, NAT_CTRL, 0, NAPT_MODE,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (0 == data)
    {
        *mode = FAL_NAPT_FULL_CONE;
    }
    else if (1 == data)
    {
        *mode = FAL_NAPT_STRICT_CONE;
    }
    else
    {
        *mode = FAL_NAPT_PORT_STRICT;
    }

    return SW_OK;
}

static sw_error_t
_isis_nat_hash_mode_set(a_uint32_t dev_id, a_uint32_t mode)
{
    sw_error_t rv;
    a_uint32_t data;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isis_nat_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    if ((FAL_NAT_HASH_KEY_PORT & mode)
            && (FAL_NAT_HASH_KEY_IPADDR & mode))
    {
        data = 2;
    }
    else if (FAL_NAT_HASH_KEY_PORT & mode)
    {
        data = 0;
    }
    else if (FAL_NAT_HASH_KEY_IPADDR & mode)
    {
        data = 1;
    }
    else
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_SET(rv, dev_id, NAT_CTRL, 0, NAT_HASH_MODE,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_isis_nat_hash_mode_get(a_uint32_t dev_id, a_uint32_t * mode)
{
    sw_error_t rv;
    a_uint32_t data = 0;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isis_nat_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    HSL_REG_FIELD_GET(rv, dev_id, NAT_CTRL, 0, NAT_HASH_MODE,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    *mode = 0;
    if (0 == data)
    {
        *mode = FAL_NAT_HASH_KEY_PORT;
    }
    else if (1 == data)
    {
        *mode = FAL_NAT_HASH_KEY_IPADDR;
    }
    else if (2 == data)
    {
        *mode = FAL_NAT_HASH_KEY_PORT;
        *mode |= FAL_NAT_HASH_KEY_IPADDR;
    }

    return SW_OK;
}

static sw_error_t
_isis_nat_prv_base_addr_set(a_uint32_t dev_id, fal_ip4_addr_t addr)
{
    sw_error_t rv;
    a_uint32_t data = 0;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isis_nat_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    HSL_REG_FIELD_GET(rv, dev_id, PRVIP_CTL, 0, BASEADDR_SEL,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (data)
    {
        data = (((addr >> 20) & 0xfff) << 8) | ((addr >> 8) & 0xff);
    }
    else
    {
        data = (addr >> 12) & 0xfffff;
    }

    HSL_REG_FIELD_SET(rv, dev_id, PRVIP_CTL, 0, IP4_BASEADDR,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    HSL_REG_FIELD_SET(rv, dev_id, OFFLOAD_PRVIP_CTL, 0, IP4_BASEADDR,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_isis_nat_prv_base_addr_get(a_uint32_t dev_id, fal_ip4_addr_t * addr)
{
    sw_error_t rv;
    a_uint32_t data = 0, tmp = 0;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isis_nat_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    HSL_REG_FIELD_GET(rv, dev_id, PRVIP_CTL, 0, BASEADDR_SEL,
                      (a_uint8_t *) (&tmp), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    HSL_REG_FIELD_GET(rv, dev_id, PRVIP_CTL, 0, IP4_BASEADDR,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (tmp)
    {
        *addr = ((data & 0xff) << 8) | (((data >> 8) & 0xfff) << 20);
    }
    else
    {
        *addr = (data & 0xfffff) << 12;
    }

    return SW_OK;
}

#if 0
static sw_error_t
_isis_nat_psr_prv_base_addr_set(a_uint32_t dev_id, fal_ip4_addr_t addr)
{
    sw_error_t rv;
    a_uint32_t data = 0;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isis_nat_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    HSL_REG_FIELD_GET(rv, dev_id, PRVIP_CTL, 0, BASEADDR_SEL,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (data)
    {
        data = (((addr >> 20) & 0xfff) << 8) | ((addr >> 8) & 0xff);
    }
    else
    {
        data = (addr >> 12) & 0xfffff;
    }

    HSL_REG_FIELD_SET(rv, dev_id, PRVIP_CTL, 0, IP4_BASEADDR,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_isis_nat_psr_prv_base_addr_get(a_uint32_t dev_id, fal_ip4_addr_t * addr)
{
    sw_error_t rv;
    a_uint32_t data, tmp = 0;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isis_nat_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    HSL_REG_FIELD_GET(rv, dev_id, PRVIP_CTL, 0, BASEADDR_SEL,
                      (a_uint8_t *) (&tmp), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    HSL_REG_FIELD_GET(rv, dev_id, PRVIP_CTL, 0, IP4_BASEADDR,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (tmp)
    {
        *addr = ((data & 0xff) << 8) | (((data >> 8) & 0xfff) << 20);
    }
    else
    {
        *addr = (data & 0xfffff) << 12;
    }

    return SW_OK;
}
#endif

static sw_error_t
_isis_nat_prv_addr_mode_set(a_uint32_t dev_id, a_bool_t map_en)
{
    sw_error_t rv;
    a_uint32_t data;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isis_nat_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    if (A_TRUE == map_en)
    {
        data = 1;
    }
    else if (A_FALSE == map_en)
    {
        data = 0;
    }
    else
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_SET(rv, dev_id, PRVIP_CTL, 0, BASEADDR_SEL,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_isis_nat_prv_addr_mode_get(a_uint32_t dev_id, a_bool_t * map_en)
{
    sw_error_t rv;
    a_uint32_t data = 0;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isis_nat_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    HSL_REG_FIELD_GET(rv, dev_id, PRVIP_CTL, 0, BASEADDR_SEL,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (data)
    {
        *map_en = A_TRUE;
    }
    else
    {
        *map_en = A_FALSE;
    }

    return SW_OK;
}

static sw_error_t
_isis_nat_pub_addr_commit(a_uint32_t dev_id, fal_nat_pub_addr_t * entry,
                          a_uint32_t op, a_uint32_t * empty)
{
    a_uint32_t index, addr, data, tbl[2] = { 0 };
    sw_error_t rv;

    *empty = ISIS_PUB_ADDR_NUM;
    for (index = 0; index < ISIS_PUB_ADDR_NUM; index++)
    {
        addr = ISIS_PUB_ADDR_TBL1_ADDR + (index << 4);
        HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr, sizeof (a_uint32_t),
                              (a_uint8_t *) (&(tbl[1])), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        SW_GET_FIELD_BY_REG(PUB_ADDR1, ADDR_VALID, data, tbl[1]);
        if (data)
        {
            addr = ISIS_PUB_ADDR_TBL0_ADDR + (index << 4);
            HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr, sizeof (a_uint32_t),
                                  (a_uint8_t *) (&(tbl[0])),
                                  sizeof (a_uint32_t));
            SW_RTN_ON_ERROR(rv);

            if (!aos_mem_cmp
                    ((void *) &(entry->pub_addr), (void *) &(tbl[0]),
                     sizeof (fal_ip4_addr_t)))
            {
                if (ISIS_NAT_ENTRY_DEL == op)
                {
                    addr = ISIS_PUB_ADDR_TBL1_ADDR + (index << 4);
                    tbl[1] = 0;
                    HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr, sizeof (a_uint32_t),
                                          (a_uint8_t *) (&(tbl[1])),
                                          sizeof (a_uint32_t));
                    *empty = index;
                    return rv;
                }
                else if (ISIS_NAT_ENTRY_ADD == op)
                {
                    entry->entry_id = index;
                    return SW_ALREADY_EXIST;
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
_isis_nat_pub_addr_add(a_uint32_t dev_id, fal_nat_pub_addr_t * entry)
{
    sw_error_t rv;
    a_uint32_t i, empty, addr, data = 0, tbl[2] = { 0 };

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isis_nat_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    tbl[0] = entry->pub_addr;
    tbl[1] = 1;

    rv = _isis_nat_pub_addr_commit(dev_id, entry, ISIS_NAT_ENTRY_ADD, &empty);
    if (SW_ALREADY_EXIST == rv)
    {
        return rv;
    }

    if (ISIS_PUB_ADDR_NUM == empty)
    {
        return SW_NO_RESOURCE;
    }

    for (i = 0; i < 1; i++)
    {
        addr = ISIS_PUB_ADDR_EDIT0_ADDR + (empty << 4) + (i << 2);
        HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr, sizeof (a_uint32_t),
                              (a_uint8_t *) (&(tbl[i])), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }

    addr = ISIS_PUB_ADDR_OFFLOAD_ADDR + (empty << 2);
    HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr, sizeof (a_uint32_t),
                          (a_uint8_t *) (&(tbl[0])), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    addr = ISIS_PUB_ADDR_VALID_ADDR;
    HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr, sizeof (a_uint32_t),
                          (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    data |= (0x1 << empty);
    HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr, sizeof (a_uint32_t),
                          (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    for (i = 0; i < 2; i++)
    {
        addr = ISIS_PUB_ADDR_TBL0_ADDR + (empty << 4) + (i << 2);
        HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr, sizeof (a_uint32_t),
                              (a_uint8_t *) (&(tbl[i])), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }

    entry->entry_id = empty;
    return SW_OK;
}

static sw_error_t
_isis_nat_pub_addr_del(a_uint32_t dev_id, a_uint32_t del_mode,
                       fal_nat_pub_addr_t * entry)
{
    sw_error_t rv;
    a_uint32_t empty, addr, data = 0;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isis_nat_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    rv = _isis_nat_pub_addr_commit(dev_id, entry, ISIS_NAT_ENTRY_DEL, &empty);
    SW_RTN_ON_ERROR(rv);

    addr = ISIS_PUB_ADDR_VALID_ADDR;
    HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr, sizeof (a_uint32_t),
                          (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    data &= (~(0x1 << empty));
    HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr, sizeof (a_uint32_t),
                          (a_uint8_t *) (&data), sizeof (a_uint32_t));

    return rv;
}

static sw_error_t
_isis_nat_pub_addr_next(a_uint32_t dev_id, a_uint32_t next_mode,
                        fal_nat_pub_addr_t * entry)
{
    sw_error_t rv;
    a_uint32_t data, addr, idx, index, tbl[2] = {0};

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isis_nat_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    if (FAL_NEXT_ENTRY_FIRST_ID == entry->entry_id)
    {
        idx = 0;
    }
    else
    {
        if ((ISIS_PUB_ADDR_NUM - 1) == entry->entry_id)
        {
            return SW_NO_MORE;
        }
        else
        {
            idx = entry->entry_id + 1;
        }
    }

    for (index = idx; index < ISIS_PUB_ADDR_NUM; index++)
    {
        addr = ISIS_PUB_ADDR_TBL1_ADDR + (index << 4);
        HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr, sizeof (a_uint32_t),
                              (a_uint8_t *) (&(tbl[1])), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        SW_GET_FIELD_BY_REG(PUB_ADDR1, ADDR_VALID, data, tbl[1]);
        if (data)
        {
            break;
        }
    }

    if (ISIS_PUB_ADDR_NUM == index)
    {
        return SW_NO_MORE;
    }

    addr = ISIS_PUB_ADDR_TBL0_ADDR + (index << 4);
    HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr, sizeof (a_uint32_t),
                          (a_uint8_t *) (&(tbl[0])), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    entry->entry_id = index;
    entry->pub_addr = tbl[0];

    return SW_OK;
}

static sw_error_t
_isis_nat_unk_session_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd)
{
    sw_error_t rv;
    a_uint32_t data;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isis_nat_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

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

    HSL_REG_FIELD_SET(rv, dev_id, FORWARD_CTL0, 0, NAT_NOT_FOUND_DROP,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_isis_nat_unk_session_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;
    a_uint32_t data = 0;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isis_nat_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    HSL_REG_FIELD_GET(rv, dev_id, FORWARD_CTL0, 0, NAT_NOT_FOUND_DROP,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (0 == data)
    {
        *cmd = FAL_MAC_RDT_TO_CPU;
    }
    else
    {
        *cmd = FAL_MAC_DROP;
    }

    return SW_OK;
}

sw_error_t
isis_nat_reset(a_uint32_t dev_id)
{
    sw_error_t rv;
    a_uint32_t index, addr, data = 0;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _isis_nat_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    isis_nat_snap[dev_id] = 0;

    HSL_REG_ENTRY_SET(rv, dev_id, HOST_ENTRY4, 0, (a_uint8_t *) (&data),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    rv = _isis_nat_entry_commit(dev_id, ISIS_ENTRY_NAT, ISIS_NAT_ENTRY_FLUSH);
    SW_RTN_ON_ERROR(rv);

    HSL_REG_ENTRY_SET(rv, dev_id, HOST_ENTRY4, 0, (a_uint8_t *) (&data),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    rv = _isis_nat_entry_commit(dev_id, ISIS_ENTRY_NAPT, ISIS_NAT_ENTRY_FLUSH);
    SW_RTN_ON_ERROR(rv);

    for (index = 0; index < ISIS_PUB_ADDR_NUM; index++)
    {
        addr = ISIS_PUB_ADDR_TBL1_ADDR + (index << 4);
        HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr, sizeof (a_uint32_t),
                              (a_uint8_t *) (&data), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }

    return SW_OK;
}

/**
 * @brief Add one NAT entry to one particular device.
 *   @details Comments:
       Before NAT entry added ip4 private base address must be set
       at first.
       In parameter nat_entry entry flags must be set
       Hardware entry id will be returned.
 * @param[in] dev_id device id
 * @param[in] nat_entry NAT entry parameter
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_nat_add(a_uint32_t dev_id, fal_nat_entry_t * nat_entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_nat_add(dev_id, nat_entry);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Del NAT entries from one particular device.
 * @param[in] dev_id device id
 * @param[in] del_mode NAT entry delete operation mode
 * @param[in] nat_entry NAT entry parameter
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_nat_del(a_uint32_t dev_id, a_uint32_t del_mode,
             fal_nat_entry_t * nat_entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_nat_del(dev_id, del_mode, nat_entry);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get one NAT entry from one particular device.
 * @param[in] dev_id device id
 * @param[in] get_mode NAT entry get operation mode
 * @param[in] nat_entry NAT entry parameter
 * @param[out] nat_entry NAT entry parameter
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_nat_get(a_uint32_t dev_id, a_uint32_t get_mode,
             fal_nat_entry_t * nat_entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_nat_get(dev_id, get_mode, nat_entry);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Next NAT entries from one particular device.
 * @param[in] dev_id device id
 * @param[in] next_mode NAT entry next operation mode
 * @param[in] nat_entry NAT entry parameter
 * @param[out] nat_entry NAT entry parameter
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_nat_next(a_uint32_t dev_id, a_uint32_t next_mode,
              fal_nat_entry_t * nat_entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_nat_next(dev_id, next_mode, nat_entry);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Bind one counter entry to one NAT entry to one particular device.
 * @param[in] dev_id device id
 * @param[in] entry_id NAT entry id
 * @param[in] cnt_id counter entry id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_nat_counter_bind(a_uint32_t dev_id, a_uint32_t entry_id, a_uint32_t cnt_id,
                      a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_nat_counter_bind(dev_id, entry_id, cnt_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Add one NAPT entry to one particular device.
 *   @details Comments:
       Before NAPT entry added related ip4 private base address must be set
       at first.
       In parameter napt_entry related entry flags must be set
       Hardware entry id will be returned.
 * @param[in] dev_id device id
 * @param[in] napt_entry NAPT entry parameter
 * @return SW_OK or error code
 */

HSL_LOCAL sw_error_t
isis_napt_add(a_uint32_t dev_id, fal_napt_entry_t * napt_entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_napt_add(dev_id, napt_entry);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Del NAPT entries from one particular device.
 * @param[in] dev_id device id
 * @param[in] del_mode NAPT entry delete operation mode
 * @param[in] napt_entry NAPT entry parameter
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_napt_del(a_uint32_t dev_id, a_uint32_t del_mode,
              fal_napt_entry_t * napt_entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_napt_del(dev_id, del_mode, napt_entry);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get one NAPT entry from one particular device.
 * @param[in] dev_id device id
 * @param[in] get_mode NAPT entry get operation mode
 * @param[in] nat_entry NAPT entry parameter
 * @param[out] nat_entry NAPT entry parameter
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_napt_get(a_uint32_t dev_id, a_uint32_t get_mode,
              fal_napt_entry_t * napt_entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_napt_get(dev_id, get_mode, napt_entry);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Next NAPT entries from one particular device.
 * @param[in] dev_id device id
 * @param[in] next_mode NAPT entry next operation mode
 * @param[in] napt_entry NAPT entry parameter
 * @param[out] napt_entry NAPT entry parameter
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_napt_next(a_uint32_t dev_id, a_uint32_t next_mode,
               fal_napt_entry_t * napt_entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_napt_next(dev_id, next_mode, napt_entry);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Bind one counter entry to one NAPT entry to one particular device.
 * @param[in] dev_id device id
 * @param[in] entry_id NAPT entry id
 * @param[in] cnt_id counter entry id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_napt_counter_bind(a_uint32_t dev_id, a_uint32_t entry_id,
                       a_uint32_t cnt_id, a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_napt_counter_bind(dev_id, entry_id, cnt_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set working status of NAT engine on a particular device
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_nat_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_nat_status_set(dev_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get working status of NAT engine on a particular device
 * @param[in] dev_id device id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_nat_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_nat_status_get(dev_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set NAT hash mode on a particular device
 * @param[in] dev_id device id
 * @param[in] mode NAT hash mode
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_nat_hash_mode_set(a_uint32_t dev_id, a_uint32_t mode)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_nat_hash_mode_set(dev_id, mode);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get NAT hash mode on a particular device
 * @param[in] dev_id device id
 * @param[out] mode NAT hash mode
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_nat_hash_mode_get(a_uint32_t dev_id, a_uint32_t * mode)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_nat_hash_mode_get(dev_id, mode);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set working status of NAPT engine on a particular device
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_napt_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_napt_status_set(dev_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get working status of NAPT engine on a particular device
 * @param[in] dev_id device id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_napt_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_napt_status_get(dev_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set working mode of NAPT engine on a particular device
 * @param[in] dev_id device id
 * @param[in] mode NAPT mode
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_napt_mode_set(a_uint32_t dev_id, fal_napt_mode_t mode)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_napt_mode_set(dev_id, mode);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get working mode of NAPT engine on a particular device
 * @param[in] dev_id device id
 * @param[out] mode NAPT mode
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_napt_mode_get(a_uint32_t dev_id, fal_napt_mode_t * mode)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_napt_mode_get(dev_id, mode);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set IP4 private base address on a particular device
 *   @details Comments:
        Only 20bits is meaning which 20bits is determined by private address mode.
 * @param[in] dev_id device id
 * @param[in] addr private base address
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_nat_prv_base_addr_set(a_uint32_t dev_id, fal_ip4_addr_t addr)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_nat_prv_base_addr_set(dev_id, addr);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get IP4 private base address on a particular device
 * @param[in] dev_id device id
 * @param[out] addr private base address
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_nat_prv_base_addr_get(a_uint32_t dev_id, fal_ip4_addr_t * addr)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_nat_prv_base_addr_get(dev_id, addr);
    HSL_API_UNLOCK;
    return rv;
}

#if 0
/**
 * @brief Set IP4 private base address on a particular device
 *   @details Comments:
        Only 20bits is meaning which 20bits is determined by private address mode.
 * @param[in] dev_id device id
 * @param[in] addr private base address
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_nat_psr_prv_base_addr_set(a_uint32_t dev_id, fal_ip4_addr_t addr)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_nat_psr_prv_base_addr_set(dev_id, addr);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get IP4 private base address on a particular device
 * @param[in] dev_id device id
 * @param[out] addr private base address
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_nat_psr_prv_base_addr_get(a_uint32_t dev_id, fal_ip4_addr_t * addr)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_nat_psr_prv_base_addr_get(dev_id, addr);
    HSL_API_UNLOCK;
    return rv;
}
#endif

/**
 * @brief Set IP4 private base address mode on a particular device
 *   @details Comments:
        If map_en equal true means bits31-20 bits15-8 are base address
        else bits31-12 are base address.
 * @param[in] dev_id device id
 * @param[in] map_en private base mapping mode
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_nat_prv_addr_mode_set(a_uint32_t dev_id, a_bool_t map_en)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_nat_prv_addr_mode_set(dev_id, map_en);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get IP4 private base address mode on a particular device
 * @param[in] dev_id device id
 * @param[out] map_en private base mapping mode
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_nat_prv_addr_mode_get(a_uint32_t dev_id, a_bool_t * map_en)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_nat_prv_addr_mode_get(dev_id, map_en);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Add one public address entry to one particular device.
 *   @details Comments:
       Hardware entry id will be returned.
 * @param[in] dev_id device id
 * @param[in] entry public address entry parameter
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_nat_pub_addr_add(a_uint32_t dev_id, fal_nat_pub_addr_t * entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_nat_pub_addr_add(dev_id, entry);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Delete one public address entry from one particular device.
 * @param[in] dev_id device id
 * @param[in] del_mode delete operaton mode
 * @param[in] entry public address entry parameter
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_nat_pub_addr_del(a_uint32_t dev_id, a_uint32_t del_mode,
                      fal_nat_pub_addr_t * entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_nat_pub_addr_del(dev_id, del_mode, entry);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Next public address entries from one particular device.
 * @param[in] dev_id device id
 * @param[in] next_mode next operaton mode
 * @param[out] entry public address entry parameter
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_nat_pub_addr_next(a_uint32_t dev_id, a_uint32_t next_mode,
                       fal_nat_pub_addr_t * entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_nat_pub_addr_next(dev_id, next_mode, entry);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set forwarding command for those packets miss NAT entries on a particular device.
 * @param[in] dev_id device id
 * @param[in] cmd forwarding command
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_nat_unk_session_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_nat_unk_session_cmd_set(dev_id, cmd);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get forwarding command for those packets miss NAT entries on a particular device.
 * @param[in] dev_id device id
 * @param[out] cmd forwarding command
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_nat_unk_session_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_nat_unk_session_cmd_get(dev_id, cmd);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set working status of NAT engine on a particular device
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_nat_global_set(a_uint32_t dev_id, a_bool_t enable, a_uint32_t portbmp)
{
    sw_error_t rv = SW_OK;

    HSL_API_LOCK;
    printk("enable:%d\n", enable);
    if(enable) {
        if(isis_nat_global_status == 0) {
            isis_nat_global_status = 1;
#if defined(IN_NAT_HELPER)
            ISIS_NAT_HELPER_INIT(rv, dev_id, portbmp);
#endif
        }
    } else {
        if(isis_nat_global_status == 1) {
            isis_nat_global_status = 0;
#if defined(IN_NAT_HELPER)
            ISIS_NAT_HELPER_CLEANUP(rv, dev_id);
#endif
        }
    }
    //rv = SW_OK;
    HSL_API_UNLOCK;
    return rv;
}


sw_error_t
isis_nat_init(a_uint32_t dev_id)
{
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    rv = isis_nat_reset(dev_id);
    SW_RTN_ON_ERROR(rv);

#ifndef HSL_STANDALONG
    {
        hsl_api_t *p_api;

        SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

        p_api->nat_add = isis_nat_add;
        p_api->nat_del = isis_nat_del;
        p_api->nat_get = isis_nat_get;
        p_api->nat_next = isis_nat_next;
        p_api->nat_counter_bind = isis_nat_counter_bind;
        p_api->napt_add = isis_napt_add;
        p_api->napt_del = isis_napt_del;
        p_api->napt_get = isis_napt_get;
        p_api->napt_next = isis_napt_next;
        p_api->napt_counter_bind = isis_napt_counter_bind;
        p_api->nat_status_set = isis_nat_status_set;
        p_api->nat_status_get = isis_nat_status_get;
        p_api->nat_hash_mode_set = isis_nat_hash_mode_set;
        p_api->nat_hash_mode_get = isis_nat_hash_mode_get;
        p_api->napt_status_set = isis_napt_status_set;
        p_api->napt_status_get = isis_napt_status_get;
        p_api->napt_mode_set = isis_napt_mode_set;
        p_api->napt_mode_get = isis_napt_mode_get;
        p_api->nat_prv_base_addr_set = isis_nat_prv_base_addr_set;
        p_api->nat_prv_base_addr_get = isis_nat_prv_base_addr_get;
        p_api->nat_prv_addr_mode_set = isis_nat_prv_addr_mode_set;
        p_api->nat_prv_addr_mode_get = isis_nat_prv_addr_mode_get;
        p_api->nat_pub_addr_add = isis_nat_pub_addr_add;
        p_api->nat_pub_addr_del = isis_nat_pub_addr_del;
        p_api->nat_pub_addr_next = isis_nat_pub_addr_next;
        p_api->nat_unk_session_cmd_set = isis_nat_unk_session_cmd_set;
        p_api->nat_unk_session_cmd_get = isis_nat_unk_session_cmd_get;
		p_api->nat_global_set = isis_nat_global_set;
    }
#endif

    return SW_OK;
}

/**
 * @}
 */
