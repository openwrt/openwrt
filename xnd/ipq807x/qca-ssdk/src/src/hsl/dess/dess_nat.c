/*
 * Copyright (c) 2014-2016, The Linux Foundation. All rights reserved.
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
 * @defgroup dess_ip DESS_NAT
 * @{
 */

#include "sw.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_port_prop.h"
#include "dess_nat.h"
#include "dess_reg.h"
#if defined(IN_NAT_HELPER)
#include "dess_nat_helper.h"
#endif

#define DESS_HOST_ENTRY_DATA0_ADDR              0x0e80
#define DESS_HOST_ENTRY_DATA1_ADDR              0x0e84
#define DESS_HOST_ENTRY_DATA2_ADDR              0x0e88
#define DESS_HOST_ENTRY_DATA3_ADDR              0x0e8c
#define DESS_HOST_ENTRY_DATA4_ADDR              0x0e90
#define DESS_HOST_ENTRY_DATA5_ADDR              0x0e94
#define DESS_HOST_ENTRY_DATA6_ADDR              0x0e98
#define DESS_HOST_ENTRY_DATA7_ADDR              0x0e58

#define DESS_HOST_ENTRY_REG_NUM                 8

#define DESS_NAT_ENTRY_FLUSH                    1
#define DESS_NAT_ENTRY_ADD                      2
#define DESS_NAT_ENTRY_DEL                      3
#define DESS_NAT_ENTRY_NEXT                     4
#define DESS_NAT_ENTRY_SEARCH                   5

#define DESS_ENTRY_NAPT                         0
#define DESS_ENTRY_FLOW                         1
#define DESS_ENTRY_NAT                          2
#define DESS_ENTRY_ARP                          3

#define DESS_PUB_ADDR_NUM                       16
#define DESS_PUB_ADDR_TBL0_ADDR                 0x5aa00
#define DESS_PUB_ADDR_TBL1_ADDR                 0x5aa04
#define DESS_PUB_ADDR_EDIT0_ADDR                0x02100
#define DESS_PUB_ADDR_EDIT1_ADDR                0x02104
#define DESS_PUB_ADDR_OFFLOAD_ADDR              0x2f000
#define DESS_PUB_ADDR_VALID_ADDR                0x2f040

#define DESS_NAT_ENTRY_NUM                      32
#define DESS_NAPT_ENTRY_NUM                     1024

#define DESS_NAT_COUTER_ADDR                    0x2b000

#define DESS_NAT_PORT_NUM                       255

aos_lock_t dess_nat_lock;
static a_uint32_t dess_nat_snap[SW_MAX_NR_DEV] = { 0 };
extern a_uint32_t dess_nat_global_status;

#if defined(IN_NAT_HELPER)
extern void nat_helper_cookie_del(a_uint32_t hw_index);
#endif

static sw_error_t
_dess_nat_feature_check(a_uint32_t dev_id)
{
    sw_error_t rv;
    a_uint32_t entry = 0;

    HSL_REG_FIELD_GET(rv, dev_id, MASK_CTL, 0, DEVICE_ID,
                      (a_uint8_t *) (&entry), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (DESS_DEVICE_ID == entry)
    {
        return SW_OK;
    }
    else
    {
        return SW_NOT_SUPPORTED;
    }
}

static sw_error_t
_dess_ip_prvaddr_sw_to_hw(a_uint32_t dev_id, fal_ip4_addr_t sw_addr,
                          a_uint32_t * hw_addr)
{
    /*
        sw_error_t rv;
        a_uint32_t data;

        HSL_REG_FIELD_GET(rv, dev_id, PRVIP_CTL, 0, BASEADDR_SEL,
                          (a_uint8_t *) (&data), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        if (data) {
            *hw_addr = (sw_addr & 0xff) | (((sw_addr >> 16) & 0xf) << 8);
        } else {
            *hw_addr = sw_addr & 0xfff;
        }
    */
    *hw_addr = sw_addr;
    return SW_OK;
}

static sw_error_t
_dess_ip_prvaddr_hw_to_sw(a_uint32_t dev_id, a_uint32_t hw_addr,
                          fal_ip4_addr_t * sw_addr)
{
    /*
        sw_error_t rv;
        a_uint32_t data, addr;

        HSL_REG_FIELD_GET(rv, dev_id, PRVIP_CTL, 0, BASEADDR_SEL,
                          (a_uint8_t *) (&data), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        HSL_REG_FIELD_GET(rv, dev_id, PRVIP_CTL, 0, IP4_BASEADDR,
                          (a_uint8_t *) (&addr), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        if (data) {
            *sw_addr = ((addr & 0xff) << 8) | (((addr >> 8) & 0xfff) << 8)
                | (hw_addr & 0xff) | (((hw_addr >> 8) & 0xf) << 16);
        } else {
            *sw_addr = (addr << 12) | (hw_addr & 0xfff);
        }
        */
    *sw_addr = hw_addr;

    return SW_OK;
}

static sw_error_t
_dess_nat_counter_get(a_uint32_t dev_id, a_uint32_t cnt_id,
                      a_uint32_t counter[4])
{
    sw_error_t rv;
    a_uint32_t i, addr;

    addr = DESS_NAT_COUTER_ADDR + (cnt_id << 4);
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
_dess_nat_entry_commit(a_uint32_t dev_id, a_uint32_t entry_type, a_uint32_t op)
{
    a_uint32_t busy = 1, i = 0x9000000, entry = 0;
    sw_error_t rv;


    while (busy && --i)
    {
        HSL_REG_ENTRY_GET(rv, dev_id, HOST_ENTRY7, 0, (a_uint8_t *) (&entry),
                          sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
        SW_GET_FIELD_BY_REG(HOST_ENTRY7, TBL_BUSY, busy, entry);
    }

    if (i == 0)
    {
	printk("busy 1\n");
        return SW_BUSY;
    }

    SW_SET_REG_BY_FIELD(HOST_ENTRY7, TBL_BUSY, 1, entry);
    SW_SET_REG_BY_FIELD(HOST_ENTRY7, TBL_SEL, entry_type, entry);
    SW_SET_REG_BY_FIELD(HOST_ENTRY7, ENTRY_FUNC, op, entry);

    HSL_REG_ENTRY_SET(rv, dev_id, HOST_ENTRY7, 0, (a_uint8_t *) (&entry),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    busy = 1;
    i = 0x90000000;
    while (busy && --i)
    {
        HSL_REG_ENTRY_GET(rv, dev_id, HOST_ENTRY7, 0, (a_uint8_t *) (&entry),
                          sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
        SW_GET_FIELD_BY_REG(HOST_ENTRY7, TBL_BUSY, busy, entry);
#if 1
        if(DESS_NAT_ENTRY_SEARCH == op &&  busy) break;
#endif
    }

    if (i == 0)
    {
	printk("busy 2\n");
        return SW_BUSY;
    }

    /* hardware requirement, we should delay... */
    if ((DESS_NAT_ENTRY_FLUSH == op) && ((DESS_ENTRY_NAPT == entry_type) ||
		(DESS_ENTRY_FLOW == entry_type)))
    {
        aos_mdelay(10);
    }

    /* hardware requirement, we should read again... */
    HSL_REG_ENTRY_GET(rv, dev_id, HOST_ENTRY7, 0, (a_uint8_t *) (&entry),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    SW_GET_FIELD_BY_REG(HOST_ENTRY7, TBL_STAUS, busy, entry);
    if (!busy)
    {
        if (DESS_NAT_ENTRY_NEXT == op)
        {
            return SW_NO_MORE;
        }
        else if (DESS_NAT_ENTRY_SEARCH == op)
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
_dess_nat_sw_to_hw(a_uint32_t dev_id, fal_nat_entry_t * entry, a_uint32_t reg[])
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
        SW_SET_REG_BY_FIELD(NAT_ENTRY3, PORT_EN, 1, reg[3]);
        SW_SET_REG_BY_FIELD(NAT_ENTRY1, PORT_RANGE, entry->port_range, reg[1]);
        if (DESS_NAT_PORT_NUM < entry->port_range)
        {
            return SW_BAD_PARAM;
        }
        SW_SET_REG_BY_FIELD(NAT_ENTRY1, PORT_NUM, entry->port_num, reg[1]);
    }
    else
    {
        SW_SET_REG_BY_FIELD(NAT_ENTRY3, PORT_EN, 0, reg[3]);
    }

    rv = _dess_ip_prvaddr_sw_to_hw(dev_id, entry->src_addr, &data);
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

    SW_SET_REG_BY_FIELD(NAT_ENTRY3, PRO_TYP, data, reg[3]);
	SW_SET_REG_BY_FIELD(NAT_ENTRY3, VRF_ID, entry->vrf_id, reg[3]);

    SW_SET_REG_BY_FIELD(NAT_ENTRY2, HASH_KEY, entry->slct_idx, reg[2]);

    SW_SET_REG_BY_FIELD(NAT_ENTRY3, ENTRY_VALID, 1, reg[3]);
    SW_SET_REG_BY_FIELD(HOST_ENTRY7, TBL_IDX, entry->entry_id, reg[7]);
    return SW_OK;
}

static sw_error_t
_dess_nat_hw_to_sw(a_uint32_t dev_id, a_uint32_t reg[], fal_nat_entry_t * entry)
{
    sw_error_t rv;
    a_uint32_t data, cnt[4] = {0};

    entry->trans_addr = reg[0];

    SW_GET_FIELD_BY_REG(NAT_ENTRY3, PORT_EN, data, reg[3]);
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

    rv = _dess_ip_prvaddr_hw_to_sw(dev_id, data, &(entry->src_addr));
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

        rv = _dess_nat_counter_get(dev_id, entry->counter_id, cnt);
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

    SW_GET_FIELD_BY_REG(NAT_ENTRY3, PRO_TYP, data, reg[3]);

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

	SW_GET_FIELD_BY_REG(NAT_ENTRY3, VRF_ID, data, reg[3]);
	entry->vrf_id = data;

    SW_GET_FIELD_BY_REG(NAT_ENTRY2, HASH_KEY, data, reg[2]);
    entry->slct_idx = data;

    return SW_OK;
}

static sw_error_t
_dess_napt_sw_to_hw(a_uint32_t dev_id, fal_napt_entry_t * entry,
                    a_uint32_t reg[])
{
    sw_error_t rv;
    a_uint32_t data;

    reg[0] = entry->dst_addr;

    SW_SET_REG_BY_FIELD(NAPT_ENTRY1, DST_PORT, entry->dst_port, reg[1]);
    SW_SET_REG_BY_FIELD(NAPT_ENTRY1, SRC_PORT, entry->src_port, reg[1]);
    SW_SET_REG_BY_FIELD(NAPT_ENTRY2, TRANS_PORT, entry->trans_port, reg[2]);

    rv = _dess_ip_prvaddr_sw_to_hw(dev_id, entry->src_addr, &data);
    SW_RTN_ON_ERROR(rv);

    SW_SET_REG_BY_FIELD(NAPT_ENTRY2, SRC_IPADDR0, (data & 0xfff), reg[2]);
    SW_SET_REG_BY_FIELD(NAPT_ENTRY3, SRC_IPADDR1, (data >> 12), reg[3]);

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

	if (A_TRUE == entry->priority_en)
    {
        SW_SET_REG_BY_FIELD(NAPT_ENTRY3, PRIORITY_EN, 1, reg[3]);
        SW_SET_REG_BY_FIELD(NAPT_ENTRY3, PRIORITY_VAL, entry->priority_val, reg[3]);
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

    SW_SET_REG_BY_FIELD(NAPT_ENTRY4, AGE_FLAG, entry->status, reg[4]);
	SW_SET_REG_BY_FIELD(NAPT_ENTRY4, AGE_SYNC, entry->aging_sync, reg[4]);
	SW_SET_REG_BY_FIELD(NAPT_ENTRY4, VRF_ID, entry->vrf_id, reg[4]);
	SW_SET_REG_BY_FIELD(NAPT_ENTRY4, FLOW_COOKIE, entry->flow_cookie, reg[4]);
	SW_SET_REG_BY_FIELD(NAPT_ENTRY4, LOAD_BALANCE, entry->load_balance, reg[4]);
    return SW_OK;
}

static sw_error_t
_dess_napt_hw_to_sw(a_uint32_t dev_id, a_uint32_t reg[],
                    fal_napt_entry_t * entry)
{
    sw_error_t rv;
    a_uint32_t data, cnt[4] = {0};

    entry->dst_addr = reg[0];

    SW_GET_FIELD_BY_REG(NAPT_ENTRY1, DST_PORT, entry->dst_port, reg[1]);
    SW_GET_FIELD_BY_REG(NAPT_ENTRY1, SRC_PORT, entry->src_port, reg[1]);
    SW_GET_FIELD_BY_REG(NAPT_ENTRY2, TRANS_PORT, entry->trans_port, reg[2]);

    SW_GET_FIELD_BY_REG(NAPT_ENTRY2, SRC_IPADDR0, data, reg[2]);
    entry->src_addr = data;
    SW_GET_FIELD_BY_REG(NAPT_ENTRY3, SRC_IPADDR1, data, reg[3]);
    data =  (entry->src_addr & 0xfff) | (data << 12);
    rv = _dess_ip_prvaddr_hw_to_sw(dev_id, data, &(entry->src_addr));
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

        rv = _dess_nat_counter_get(dev_id, entry->counter_id, cnt);
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

	SW_GET_FIELD_BY_REG(NAPT_ENTRY3, PRIORITY_EN, data, reg[3]);
	if (data)
	{
		entry->priority_en = A_TRUE;
		SW_GET_FIELD_BY_REG(NAPT_ENTRY3, PRIORITY_VAL, entry->priority_val, reg[3]);
	}
	else
	{
		entry->priority_en = A_FALSE;
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

    SW_GET_FIELD_BY_REG(NAPT_ENTRY4, AGE_FLAG, entry->status, reg[4]);
	SW_GET_FIELD_BY_REG(NAPT_ENTRY4, AGE_SYNC, entry->aging_sync, reg[4]);
	SW_GET_FIELD_BY_REG(NAPT_ENTRY4, VRF_ID, entry->vrf_id, reg[4]);
	SW_GET_FIELD_BY_REG(NAPT_ENTRY4, FLOW_COOKIE, entry->flow_cookie, reg[4]);
	SW_GET_FIELD_BY_REG(NAPT_ENTRY4, LOAD_BALANCE, entry->load_balance, reg[4]);
    return SW_OK;
}

static sw_error_t
_dess_nat_down_to_hw(a_uint32_t dev_id, a_uint32_t reg[])
{
    sw_error_t rv;
    a_uint32_t i, addr;

    for (i = 0; i < DESS_HOST_ENTRY_REG_NUM; i++)
    {
        if((DESS_HOST_ENTRY_REG_NUM - 1) == i)
        {
            addr = DESS_HOST_ENTRY_DATA7_ADDR;
        }
        else
        {
            addr = DESS_HOST_ENTRY_DATA0_ADDR + (i << 2);
        }

        HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr, sizeof (a_uint32_t),
                              (a_uint8_t *) (&reg[i]), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }

    return SW_OK;
}

static sw_error_t
_dess_nat_up_to_sw(a_uint32_t dev_id, a_uint32_t reg[])
{
    sw_error_t rv;
    a_uint32_t i, addr;

    for (i = 0; i < DESS_HOST_ENTRY_REG_NUM; i++)
    {
        if((DESS_HOST_ENTRY_REG_NUM -1) == i)
        {
            addr = DESS_HOST_ENTRY_DATA7_ADDR;
        }
        else
        {
            addr = DESS_HOST_ENTRY_DATA0_ADDR + (i << 2);
        }

        HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr, sizeof (a_uint32_t),
                              (a_uint8_t *) (&reg[i]), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }

    return SW_OK;
}

static sw_error_t
_dess_nat_add(a_uint32_t dev_id, fal_nat_entry_t * entry)
{
    sw_error_t rv;
    a_uint32_t i, reg[DESS_HOST_ENTRY_REG_NUM] = { 0 };

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_nat_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    for (i = 0; i < DESS_NAT_ENTRY_NUM; i++)
    {
        if (!(dess_nat_snap[dev_id] & (0x1 << i)))
        {
            break;
        }
    }

    if (DESS_NAT_ENTRY_NUM == i)
    {
        return SW_NO_RESOURCE;
    }

    entry->entry_id = i;

    rv = _dess_nat_sw_to_hw(dev_id, entry, reg);
    SW_RTN_ON_ERROR(rv);

    rv = _dess_nat_down_to_hw(dev_id, reg);
    SW_RTN_ON_ERROR(rv);

    rv = _dess_nat_entry_commit(dev_id, DESS_ENTRY_NAT, DESS_NAT_ENTRY_ADD);
    SW_RTN_ON_ERROR(rv);

    dess_nat_snap[dev_id] |= (0x1 << i);
    entry->entry_id = i;
    return SW_OK;
}

static sw_error_t
_dess_nat_del(a_uint32_t dev_id, a_uint32_t del_mode, fal_nat_entry_t * entry)
{
    sw_error_t rv;
    a_uint32_t reg[DESS_HOST_ENTRY_REG_NUM] = { 0 };

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_nat_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    if (FAL_NAT_ENTRY_ID_EN & del_mode)
    {
        SW_SET_REG_BY_FIELD(HOST_ENTRY7, ENTRY_FUNC, DESS_NAT_ENTRY_DEL, reg[7]);
        SW_SET_REG_BY_FIELD(HOST_ENTRY7, TBL_IDX, entry->entry_id, reg[7]);

        rv = _dess_nat_down_to_hw(dev_id, reg);
        SW_RTN_ON_ERROR(rv);

        rv = _dess_nat_entry_commit(dev_id, DESS_ENTRY_NAT, DESS_NAT_ENTRY_DEL);
        SW_RTN_ON_ERROR(rv);

        dess_nat_snap[dev_id] &= (~(0x1 << entry->entry_id));
    }
    else
    {
        rv = _dess_nat_down_to_hw(dev_id, reg);
        SW_RTN_ON_ERROR(rv);

        rv = _dess_nat_entry_commit(dev_id, DESS_ENTRY_NAT, DESS_NAT_ENTRY_FLUSH);
        SW_RTN_ON_ERROR(rv);

        dess_nat_snap[dev_id] = 0;
    }

    return SW_OK;
}

static sw_error_t
_dess_nat_get(a_uint32_t dev_id, a_uint32_t get_mode, fal_nat_entry_t * entry)
{
    sw_error_t rv;
    a_uint32_t reg[DESS_HOST_ENTRY_REG_NUM] = { 0 };

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_nat_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    if (FAL_NAT_ENTRY_ID_EN != get_mode)
    {
        return SW_NOT_SUPPORTED;
    }

    if (!(dess_nat_snap[dev_id] & (0x1 << entry->entry_id)))
    {
        return SW_NOT_FOUND;
    }

    SW_SET_REG_BY_FIELD(HOST_ENTRY7, TBL_IDX, entry->entry_id, reg[7]);

    rv = _dess_nat_down_to_hw(dev_id, reg);
    SW_RTN_ON_ERROR(rv);

    rv = _dess_nat_entry_commit(dev_id, DESS_ENTRY_NAT, DESS_NAT_ENTRY_SEARCH);
    SW_RTN_ON_ERROR(rv);

    rv = _dess_nat_up_to_sw(dev_id, reg);
    SW_RTN_ON_ERROR(rv);

    rv = _dess_nat_hw_to_sw(dev_id, reg, entry);
    return rv;
}

static sw_error_t
_dess_nat_next(a_uint32_t dev_id, a_uint32_t next_mode,
               fal_nat_entry_t * nat_entry)
{
    a_uint32_t i, idx, reg[DESS_HOST_ENTRY_REG_NUM] = { 0 };
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_nat_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    if (FAL_NEXT_ENTRY_FIRST_ID == nat_entry->entry_id)
    {
        idx = 0;
    }
    else
    {
        if ((DESS_NAT_ENTRY_NUM - 1) == nat_entry->entry_id)
        {
            return SW_NO_MORE;
        }
        else
        {
            idx = nat_entry->entry_id + 1;
        }
    }

    for (i = idx; i < DESS_NAT_ENTRY_NUM; i++)
    {
        if (dess_nat_snap[dev_id] & (0x1 << i))
        {
            break;
        }
    }

    if (DESS_NAT_ENTRY_NUM == i)
    {
        return SW_NO_MORE;
    }

    SW_SET_REG_BY_FIELD(HOST_ENTRY7, TBL_IDX, i, reg[7]);

    rv = _dess_nat_down_to_hw(dev_id, reg);
    SW_RTN_ON_ERROR(rv);

    rv = _dess_nat_entry_commit(dev_id, DESS_ENTRY_NAT, DESS_NAT_ENTRY_SEARCH);
    SW_RTN_ON_ERROR(rv);

    rv = _dess_nat_up_to_sw(dev_id, reg);
    SW_RTN_ON_ERROR(rv);

    aos_mem_zero(nat_entry, sizeof (fal_nat_entry_t));

    rv = _dess_nat_hw_to_sw(dev_id, reg, nat_entry);
    SW_RTN_ON_ERROR(rv);

    nat_entry->entry_id = i;
    return SW_OK;
}

static sw_error_t
_dess_nat_counter_bind(a_uint32_t dev_id, a_uint32_t entry_id,
                       a_uint32_t cnt_id, a_bool_t enable)
{
    sw_error_t rv;
    a_uint32_t reg[DESS_HOST_ENTRY_REG_NUM] = { 0 };

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_nat_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    if (!(dess_nat_snap[dev_id] & (0x1 << entry_id)))
    {
        return SW_NOT_FOUND;
    }

    SW_SET_REG_BY_FIELD(HOST_ENTRY7, TBL_IDX, entry_id, reg[7]);

    rv = _dess_nat_entry_commit(dev_id, DESS_ENTRY_NAT, DESS_NAT_ENTRY_SEARCH);
    SW_RTN_ON_ERROR(rv);

    rv = _dess_nat_up_to_sw(dev_id, reg);
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

    rv = _dess_nat_entry_commit(dev_id, DESS_ENTRY_NAT, DESS_NAT_ENTRY_DEL);
    SW_RTN_ON_ERROR(rv);

    rv = _dess_nat_down_to_hw(dev_id, reg);
    SW_RTN_ON_ERROR(rv);

    /* needn't set TBL_IDX, keep hardware register value */

    rv = _dess_nat_entry_commit(dev_id, DESS_ENTRY_NAT, DESS_NAT_ENTRY_ADD);
    return rv;
}

static sw_error_t
_dess_napt_add(a_uint32_t dev_id, fal_napt_entry_t * entry)
{
    sw_error_t rv;
    a_uint32_t reg[DESS_HOST_ENTRY_REG_NUM] = { 0 };

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_nat_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_napt_sw_to_hw(dev_id, entry, reg);
    SW_RTN_ON_ERROR(rv);

	aos_lock_bh(&dess_nat_lock);
    rv = _dess_nat_down_to_hw(dev_id, reg);
    	if (rv != SW_OK) {
		aos_unlock_bh(&dess_nat_lock);
		return rv;
    	}

    rv = _dess_nat_entry_commit(dev_id, DESS_ENTRY_NAPT, DESS_NAT_ENTRY_ADD);
    	if (rv != SW_OK) {
		aos_unlock_bh(&dess_nat_lock);
		return rv;
    	}

    rv = _dess_nat_up_to_sw(dev_id, reg);
    	if (rv != SW_OK) {
		aos_unlock_bh(&dess_nat_lock);
		return rv;
    	}

    SW_GET_FIELD_BY_REG(HOST_ENTRY7, TBL_IDX, entry->entry_id, reg[7]);
	aos_unlock_bh(&dess_nat_lock);
    return SW_OK;
}

static sw_error_t
_dess_napt_del(a_uint32_t dev_id, a_uint32_t del_mode, fal_napt_entry_t * entry)
{
    sw_error_t rv;
    a_uint32_t data, reg[DESS_HOST_ENTRY_REG_NUM] = { 0 };

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_nat_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    if (FAL_NAT_ENTRY_ID_EN & del_mode)
    {
        return SW_NOT_SUPPORTED;
    }

    if (FAL_NAT_ENTRY_KEY_EN & del_mode)
    {
        rv = _dess_napt_sw_to_hw(dev_id, entry, reg);
        SW_RTN_ON_ERROR(rv);

	aos_lock_bh(&dess_nat_lock);
        rv = _dess_nat_down_to_hw(dev_id, reg);
	if (rv != SW_OK) {
		aos_unlock_bh(&dess_nat_lock);
		return rv;
	}

        rv = _dess_nat_entry_commit(dev_id, DESS_ENTRY_NAPT, DESS_NAT_ENTRY_DEL);
	if (rv != SW_OK) {
		aos_unlock_bh(&dess_nat_lock);
		return rv;
	}

        rv = _dess_nat_up_to_sw(dev_id, reg);
	if (rv != SW_OK) {
		aos_unlock_bh(&dess_nat_lock);
		return rv;
	}

        SW_GET_FIELD_BY_REG(HOST_ENTRY7, TBL_IDX, entry->entry_id, reg[7]);
	aos_unlock_bh(&dess_nat_lock);
        return SW_OK;
    }
    else
    {
        if (FAL_NAT_ENTRY_PUBLIC_IP_EN & del_mode)
        {
            SW_SET_REG_BY_FIELD(HOST_ENTRY7, SPEC_PIP, 1, reg[7]);
            SW_SET_REG_BY_FIELD(NAPT_ENTRY2, TRANS_IPADDR, entry->trans_addr, reg[2]);
        }

        if (FAL_NAT_ENTRY_SOURCE_IP_EN & del_mode)
        {
            SW_SET_REG_BY_FIELD(HOST_ENTRY7, SPEC_SIP, 1, reg[7]);
            rv = _dess_ip_prvaddr_sw_to_hw(dev_id, entry->src_addr, &data);
            SW_RTN_ON_ERROR(rv);
            SW_SET_REG_BY_FIELD(NAPT_ENTRY2, SRC_IPADDR0, (data & 0xfff), reg[2]);
            SW_SET_REG_BY_FIELD(NAPT_ENTRY3, SRC_IPADDR1, (data >> 12), reg[3]);
        }

        if (FAL_NAT_ENTRY_AGE_EN & del_mode)
        {
            SW_SET_REG_BY_FIELD(HOST_ENTRY7, SPEC_STATUS, 1, reg[7]);
            SW_SET_REG_BY_FIELD(NAPT_ENTRY4, AGE_FLAG, entry->status, reg[4]);
        }

	aos_lock_bh(&dess_nat_lock);
        rv = _dess_nat_down_to_hw(dev_id, reg);
	if (rv != SW_OK) {
		aos_unlock_bh(&dess_nat_lock);
		return rv;
	}
        rv = _dess_nat_entry_commit(dev_id, DESS_ENTRY_NAPT, DESS_NAT_ENTRY_FLUSH);
	aos_unlock_bh(&dess_nat_lock);
        return rv;
    }
}

static sw_error_t
_dess_napt_get(a_uint32_t dev_id, a_uint32_t get_mode, fal_napt_entry_t * entry)
{
    sw_error_t rv;
    a_uint32_t found, age, reg[DESS_HOST_ENTRY_REG_NUM] = { 0 };

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_nat_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

#if 0
    if (FAL_NAT_ENTRY_ID_EN != get_mode)
    {
        return SW_NOT_SUPPORTED;
    }

    SW_SET_REG_BY_FIELD(HOST_ENTRY7, TBL_IDX, entry->entry_id, reg[7]);
#else
    rv = _dess_napt_sw_to_hw(dev_id, entry, reg);
    SW_RTN_ON_ERROR(rv);
#endif

	aos_lock_bh(&dess_nat_lock);
    rv = _dess_nat_down_to_hw(dev_id, reg);
	if (rv != SW_OK) {
		aos_unlock_bh(&dess_nat_lock);
		return rv;
	}

    rv = _dess_nat_entry_commit(dev_id, DESS_ENTRY_NAPT, DESS_NAT_ENTRY_SEARCH);
	if (rv != SW_OK) {
		aos_unlock_bh(&dess_nat_lock);
		return rv;
	}

    rv = _dess_nat_up_to_sw(dev_id, reg);
	if (rv != SW_OK) {
		aos_unlock_bh(&dess_nat_lock);
		return rv;
	}
	aos_unlock_bh(&dess_nat_lock);

    SW_GET_FIELD_BY_REG(HOST_ENTRY7, TBL_STAUS, found, reg[7]);
    SW_GET_FIELD_BY_REG(NAPT_ENTRY4, AGE_FLAG,  age, reg[4]);
    if (found && age)
    {
        found = 1;
    }
    else
    {
        found = 0;
    }

    rv = _dess_napt_hw_to_sw(dev_id, reg, entry);
    SW_RTN_ON_ERROR(rv);

    if (!found)
    {
        return SW_NOT_FOUND;
    }

    SW_GET_FIELD_BY_REG(HOST_ENTRY7, TBL_IDX,   entry->entry_id, reg[7]);
    return SW_OK;
}

static sw_error_t
_dess_napt_next(a_uint32_t dev_id, a_uint32_t next_mode,
                fal_napt_entry_t * napt_entry)
{
    a_uint32_t data, idx, reg[DESS_HOST_ENTRY_REG_NUM] = { 0 };
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_nat_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    if (FAL_NEXT_ENTRY_FIRST_ID == napt_entry->entry_id)
    {
        idx = DESS_NAPT_ENTRY_NUM - 1;
    }
    else
    {
        if ((DESS_NAPT_ENTRY_NUM - 1) == napt_entry->entry_id)
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
        SW_SET_REG_BY_FIELD(HOST_ENTRY7, SPEC_PIP, 1, reg[7]);
        SW_SET_REG_BY_FIELD(NAPT_ENTRY2, TRANS_IPADDR, napt_entry->trans_addr, reg[2]);
    }

    if (FAL_NAT_ENTRY_SOURCE_IP_EN & next_mode)
    {
        SW_SET_REG_BY_FIELD(HOST_ENTRY7, SPEC_SIP, 1, reg[7]);
        rv = _dess_ip_prvaddr_sw_to_hw(dev_id, napt_entry->src_addr, &data);
        SW_RTN_ON_ERROR(rv);
        SW_SET_REG_BY_FIELD(NAPT_ENTRY2, SRC_IPADDR0, (data & 0xfff), reg[2]);
        SW_SET_REG_BY_FIELD(NAPT_ENTRY3, SRC_IPADDR1, (data >> 12), reg[3]);
    }

    if (FAL_NAT_ENTRY_AGE_EN & next_mode)
    {
        SW_SET_REG_BY_FIELD(HOST_ENTRY7, SPEC_STATUS, 1, reg[7]);
        SW_SET_REG_BY_FIELD(NAPT_ENTRY4, AGE_FLAG, napt_entry->status, reg[4]);
    }

	if (FAL_NAT_ENTRY_SYNC_EN & next_mode)
    {
        SW_SET_REG_BY_FIELD(HOST_ENTRY7, SPEC_SYNC, 1, reg[7]);
        SW_SET_REG_BY_FIELD(NAPT_ENTRY4, AGE_SYNC, napt_entry->aging_sync, reg[4]);
    }

    SW_SET_REG_BY_FIELD(HOST_ENTRY7, TBL_IDX, idx, reg[7]);

	aos_lock_bh(&dess_nat_lock);
    rv = _dess_nat_down_to_hw(dev_id, reg);
	if (rv != SW_OK) {
		aos_unlock_bh(&dess_nat_lock);
		return rv;
	}

    rv = _dess_nat_entry_commit(dev_id, DESS_ENTRY_NAPT, DESS_NAT_ENTRY_NEXT);
	if (rv != SW_OK) {
		aos_unlock_bh(&dess_nat_lock);
		return rv;
	}

    rv = _dess_nat_up_to_sw(dev_id, reg);
	if (rv != SW_OK) {
		aos_unlock_bh(&dess_nat_lock);
		return rv;
	}
	aos_unlock_bh(&dess_nat_lock);

    aos_mem_zero(napt_entry, sizeof (fal_nat_entry_t));

    rv = _dess_napt_hw_to_sw(dev_id, reg, napt_entry);
    SW_RTN_ON_ERROR(rv);

#if 0
    a_uint32_t temp=0, complete=0;

    HSL_REG_ENTRY_GET(rv, dev_id, HOST_ENTRY7, 0, (a_uint8_t *) (&temp),
                      sizeof (a_uint32_t));

    SW_GET_FIELD_BY_REG(HOST_ENTRY7, TBL_STAUS, complete, temp);

    if (!complete)
    {
        return SW_NO_MORE;
    }
#endif

    SW_GET_FIELD_BY_REG(HOST_ENTRY7, TBL_IDX, napt_entry->entry_id, reg[7]);
    return SW_OK;
}

static sw_error_t
_dess_napt_counter_bind(a_uint32_t dev_id, a_uint32_t entry_id,
                        a_uint32_t cnt_id, a_bool_t enable)
{
    sw_error_t rv;
    a_uint32_t reg[DESS_HOST_ENTRY_REG_NUM] = { 0 }, tbl_idx;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_nat_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    tbl_idx = (entry_id - 1) & 0x3ff;
    SW_SET_REG_BY_FIELD(HOST_ENTRY7, TBL_IDX, tbl_idx, reg[7]);

    rv = _dess_nat_down_to_hw(dev_id, reg);
    SW_RTN_ON_ERROR(rv);

    rv = _dess_nat_entry_commit(dev_id, DESS_ENTRY_NAPT, DESS_NAT_ENTRY_NEXT);
    if (SW_OK != rv)
    {
        return SW_NOT_FOUND;
    }

    rv = _dess_nat_up_to_sw(dev_id, reg);
    SW_RTN_ON_ERROR(rv);

    SW_GET_FIELD_BY_REG(HOST_ENTRY7, TBL_IDX, tbl_idx, reg[7]);
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

    rv = _dess_nat_entry_commit(dev_id, DESS_ENTRY_NAPT, DESS_NAT_ENTRY_DEL);
    SW_RTN_ON_ERROR(rv);

    rv = _dess_nat_down_to_hw(dev_id, reg);
    SW_RTN_ON_ERROR(rv);

    rv = _dess_nat_entry_commit(dev_id, DESS_ENTRY_NAPT, DESS_NAT_ENTRY_ADD);
    return rv;
}

static sw_error_t
_dess_flow_add(a_uint32_t dev_id, fal_napt_entry_t * entry)
{
    sw_error_t rv;
    a_uint32_t reg[DESS_HOST_ENTRY_REG_NUM] = { 0 };

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_nat_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_napt_sw_to_hw(dev_id, entry, reg);
    SW_RTN_ON_ERROR(rv);

	aos_lock_bh(&dess_nat_lock);
    rv = _dess_nat_down_to_hw(dev_id, reg);
	if (rv != SW_OK) {
		aos_unlock_bh(&dess_nat_lock);
		return rv;
	}

    rv = _dess_nat_entry_commit(dev_id, DESS_ENTRY_FLOW, DESS_NAT_ENTRY_ADD);
	if (rv != SW_OK) {
		aos_unlock_bh(&dess_nat_lock);
		return rv;
	}

    rv = _dess_nat_up_to_sw(dev_id, reg);
	if (rv != SW_OK) {
		aos_unlock_bh(&dess_nat_lock);
		return rv;
	}

    SW_GET_FIELD_BY_REG(HOST_ENTRY7, TBL_IDX, entry->entry_id, reg[7]);
	aos_unlock_bh(&dess_nat_lock);
    return SW_OK;
}

static sw_error_t
_dess_flow_del(a_uint32_t dev_id, a_uint32_t del_mode, fal_napt_entry_t * entry)
{
    sw_error_t rv;
    a_uint32_t data, reg[DESS_HOST_ENTRY_REG_NUM] = { 0 };

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_nat_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    if (FAL_NAT_ENTRY_ID_EN & del_mode)
    {
        return SW_NOT_SUPPORTED;
    }

    if (FAL_NAT_ENTRY_KEY_EN & del_mode)
    {
        rv = _dess_napt_sw_to_hw(dev_id, entry, reg);
        SW_RTN_ON_ERROR(rv);

	aos_lock_bh(&dess_nat_lock);
        rv = _dess_nat_down_to_hw(dev_id, reg);
	if (rv != SW_OK) {
		aos_unlock_bh(&dess_nat_lock);
		return rv;
	}

        rv = _dess_nat_entry_commit(dev_id, DESS_ENTRY_FLOW, DESS_NAT_ENTRY_DEL);
	if (rv != SW_OK) {
		aos_unlock_bh(&dess_nat_lock);
		return rv;
	}

        rv = _dess_nat_up_to_sw(dev_id, reg);
	if (rv != SW_OK) {
		aos_unlock_bh(&dess_nat_lock);
		return rv;
	}

        SW_GET_FIELD_BY_REG(HOST_ENTRY7, TBL_IDX, entry->entry_id, reg[7]);
	aos_unlock_bh(&dess_nat_lock);
        return SW_OK;
    }
    else
    {
        if (FAL_NAT_ENTRY_PUBLIC_IP_EN & del_mode)
        {
            SW_SET_REG_BY_FIELD(HOST_ENTRY7, SPEC_PIP, 1, reg[7]);
            SW_SET_REG_BY_FIELD(NAPT_ENTRY2, TRANS_IPADDR, entry->trans_addr, reg[2]);
        }

        if (FAL_NAT_ENTRY_SOURCE_IP_EN & del_mode)
        {
            SW_SET_REG_BY_FIELD(HOST_ENTRY7, SPEC_SIP, 1, reg[7]);
            rv = _dess_ip_prvaddr_sw_to_hw(dev_id, entry->src_addr, &data);
            SW_RTN_ON_ERROR(rv);
            SW_SET_REG_BY_FIELD(NAPT_ENTRY2, SRC_IPADDR0, (data & 0xfff), reg[2]);
            SW_SET_REG_BY_FIELD(NAPT_ENTRY3, SRC_IPADDR1, (data >> 12), reg[3]);
        }

        if (FAL_NAT_ENTRY_AGE_EN & del_mode)
        {
            SW_SET_REG_BY_FIELD(HOST_ENTRY7, SPEC_STATUS, 1, reg[7]);
            SW_SET_REG_BY_FIELD(NAPT_ENTRY4, AGE_FLAG, entry->status, reg[4]);
        }

	aos_lock_bh(&dess_nat_lock);
        rv = _dess_nat_down_to_hw(dev_id, reg);
	if (rv != SW_OK) {
		aos_unlock_bh(&dess_nat_lock);
		return rv;
	}

        rv = _dess_nat_entry_commit(dev_id, DESS_ENTRY_FLOW, DESS_NAT_ENTRY_FLUSH);
	aos_unlock_bh(&dess_nat_lock);
        return rv;
    }
}

static sw_error_t
_dess_flow_get(a_uint32_t dev_id, a_uint32_t get_mode, fal_napt_entry_t * entry)
{
    sw_error_t rv;
    a_uint32_t found, age, reg[DESS_HOST_ENTRY_REG_NUM] = { 0 };

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_nat_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

#if 0
    if (FAL_NAT_ENTRY_ID_EN != get_mode)
    {
        return SW_NOT_SUPPORTED;
    }

    SW_SET_REG_BY_FIELD(HOST_ENTRY7, TBL_IDX, entry->entry_id, reg[7]);
#else
    rv = _dess_napt_sw_to_hw(dev_id, entry, reg);
    SW_RTN_ON_ERROR(rv);
#endif

	aos_lock_bh(&dess_nat_lock);
    rv = _dess_nat_down_to_hw(dev_id, reg);
	if (rv != SW_OK) {
		aos_unlock_bh(&dess_nat_lock);
		return rv;
	}

    rv = _dess_nat_entry_commit(dev_id, DESS_ENTRY_FLOW, DESS_NAT_ENTRY_SEARCH);
	if (rv != SW_OK) {
		aos_unlock_bh(&dess_nat_lock);
		return rv;
	}

    rv = _dess_nat_up_to_sw(dev_id, reg);
	if (rv != SW_OK) {
		aos_unlock_bh(&dess_nat_lock);
		return rv;
	}
	aos_unlock_bh(&dess_nat_lock);

    SW_GET_FIELD_BY_REG(HOST_ENTRY7, TBL_STAUS, found, reg[7]);
    SW_GET_FIELD_BY_REG(NAPT_ENTRY4, AGE_FLAG,  age, reg[4]);
    if (found && age)
    {
        found = 1;
    }
    else
    {
        found = 0;
    }

    rv = _dess_napt_hw_to_sw(dev_id, reg, entry);
    SW_RTN_ON_ERROR(rv);

    if (!found)
    {
        return SW_NOT_FOUND;
    }

    SW_GET_FIELD_BY_REG(HOST_ENTRY7, TBL_IDX,   entry->entry_id, reg[7]);
    return SW_OK;
}

static sw_error_t
_dess_flow_next(a_uint32_t dev_id, a_uint32_t next_mode,
                fal_napt_entry_t * napt_entry)
{
    a_uint32_t data, idx, reg[DESS_HOST_ENTRY_REG_NUM] = { 0 };
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_nat_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    if (FAL_NEXT_ENTRY_FIRST_ID == napt_entry->entry_id)
    {
        idx = DESS_NAPT_ENTRY_NUM - 1;
    }
    else
    {
        if ((DESS_NAPT_ENTRY_NUM - 1) == napt_entry->entry_id)
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
        SW_SET_REG_BY_FIELD(HOST_ENTRY7, SPEC_PIP, 1, reg[7]);
        SW_SET_REG_BY_FIELD(NAPT_ENTRY2, TRANS_IPADDR, napt_entry->trans_addr, reg[2]);
    }

    if (FAL_NAT_ENTRY_SOURCE_IP_EN & next_mode)
    {
        SW_SET_REG_BY_FIELD(HOST_ENTRY7, SPEC_SIP, 1, reg[7]);
        rv = _dess_ip_prvaddr_sw_to_hw(dev_id, napt_entry->src_addr, &data);
        SW_RTN_ON_ERROR(rv);
        SW_SET_REG_BY_FIELD(NAPT_ENTRY2, SRC_IPADDR0, (data & 0xfff), reg[2]);
        SW_SET_REG_BY_FIELD(NAPT_ENTRY3, SRC_IPADDR1, (data >> 12), reg[3]);
    }

    if (FAL_NAT_ENTRY_AGE_EN & next_mode)
    {
        SW_SET_REG_BY_FIELD(HOST_ENTRY7, SPEC_STATUS, 1, reg[7]);
        SW_SET_REG_BY_FIELD(NAPT_ENTRY4, AGE_FLAG, napt_entry->status, reg[4]);
    }

	if (FAL_NAT_ENTRY_SYNC_EN & next_mode)
    {
        SW_SET_REG_BY_FIELD(HOST_ENTRY7, SPEC_SYNC, 1, reg[7]);
        SW_SET_REG_BY_FIELD(NAPT_ENTRY4, AGE_SYNC, napt_entry->status, reg[4]);
    }

    SW_SET_REG_BY_FIELD(HOST_ENTRY7, TBL_IDX, idx, reg[7]);

	aos_lock_bh(&dess_nat_lock);
    rv = _dess_nat_down_to_hw(dev_id, reg);
	if (rv != SW_OK) {
		aos_unlock_bh(&dess_nat_lock);
		return rv;
	}

    rv = _dess_nat_entry_commit(dev_id, DESS_ENTRY_FLOW, DESS_NAT_ENTRY_NEXT);
	if (rv != SW_OK) {
		aos_unlock_bh(&dess_nat_lock);
		return rv;
	}

    rv = _dess_nat_up_to_sw(dev_id, reg);
	if (rv != SW_OK) {
		aos_unlock_bh(&dess_nat_lock);
		return rv;
	}
	aos_unlock_bh(&dess_nat_lock);

    aos_mem_zero(napt_entry, sizeof (fal_nat_entry_t));

    rv = _dess_napt_hw_to_sw(dev_id, reg, napt_entry);
    SW_RTN_ON_ERROR(rv);

#if 0
    a_uint32_t temp=0, complete=0;

    HSL_REG_ENTRY_GET(rv, dev_id, HOST_ENTRY7, 0, (a_uint8_t *) (&temp),
                      sizeof (a_uint32_t));

    SW_GET_FIELD_BY_REG(HOST_ENTRY7, TBL_STAUS, complete, temp);

    if (!complete)
    {
        return SW_NO_MORE;
    }
#endif

    SW_GET_FIELD_BY_REG(HOST_ENTRY7, TBL_IDX, napt_entry->entry_id, reg[7]);
    return SW_OK;
}

static sw_error_t
_dess_flow_counter_bind(a_uint32_t dev_id, a_uint32_t entry_id,
                        a_uint32_t cnt_id, a_bool_t enable)
{
    sw_error_t rv;
    a_uint32_t reg[DESS_HOST_ENTRY_REG_NUM] = { 0 }, tbl_idx;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_nat_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    tbl_idx = (entry_id - 1) & 0x3ff;
    SW_SET_REG_BY_FIELD(HOST_ENTRY7, TBL_IDX, tbl_idx, reg[7]);

    rv = _dess_nat_down_to_hw(dev_id, reg);
    SW_RTN_ON_ERROR(rv);

    rv = _dess_nat_entry_commit(dev_id, DESS_ENTRY_FLOW, DESS_NAT_ENTRY_NEXT);
    if (SW_OK != rv)
    {
        return SW_NOT_FOUND;
    }

    rv = _dess_nat_up_to_sw(dev_id, reg);
    SW_RTN_ON_ERROR(rv);

    SW_GET_FIELD_BY_REG(HOST_ENTRY7, TBL_IDX, tbl_idx, reg[7]);
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

    rv = _dess_nat_entry_commit(dev_id, DESS_ENTRY_FLOW, DESS_NAT_ENTRY_DEL);
    SW_RTN_ON_ERROR(rv);

    rv = _dess_nat_down_to_hw(dev_id, reg);
    SW_RTN_ON_ERROR(rv);

    rv = _dess_nat_entry_commit(dev_id, DESS_ENTRY_FLOW, DESS_NAT_ENTRY_ADD);
    return rv;
}

static sw_error_t
_dess_nat_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;
    a_uint32_t data;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_nat_feature_check(dev_id);
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
_dess_nat_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;
    a_uint32_t data = 0;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_nat_feature_check(dev_id);
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
_dess_napt_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;
    a_uint32_t data;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_nat_feature_check(dev_id);
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
_dess_napt_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;
    a_uint32_t data = 0;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_nat_feature_check(dev_id);
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
_dess_napt_mode_set(a_uint32_t dev_id, fal_napt_mode_t mode)
{
    sw_error_t rv;
    a_uint32_t data;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_nat_feature_check(dev_id);
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
_dess_napt_mode_get(a_uint32_t dev_id, fal_napt_mode_t * mode)
{
    sw_error_t rv;
    a_uint32_t data = 0;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_nat_feature_check(dev_id);
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
_dess_nat_hash_mode_set(a_uint32_t dev_id, a_uint32_t mode)
{
    sw_error_t rv;
    a_uint32_t data;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_nat_feature_check(dev_id);
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
_dess_nat_hash_mode_get(a_uint32_t dev_id, a_uint32_t * mode)
{
    sw_error_t rv;
    a_uint32_t data = 0;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_nat_feature_check(dev_id);
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
_dess_nat_prv_base_addr_set(a_uint32_t dev_id, fal_ip4_addr_t addr)
{
    sw_error_t rv;
    a_uint32_t data;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_nat_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    data = addr;
    HSL_REG_FIELD_SET(rv, dev_id, PRVIP_ADDR, 0, IP4_BASEADDR,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_dess_nat_prv_base_addr_get(a_uint32_t dev_id, fal_ip4_addr_t * addr)
{
    sw_error_t rv;
    a_uint32_t data = 0;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_nat_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    HSL_REG_FIELD_GET(rv, dev_id, PRVIP_ADDR, 0, IP4_BASEADDR,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    *addr = data;

    return SW_OK;
}

static sw_error_t
_dess_nat_prv_base_mask_set(a_uint32_t dev_id, fal_ip4_addr_t mask)
{
    sw_error_t rv;
    a_uint32_t data;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_nat_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    data = mask;
    HSL_REG_FIELD_SET(rv, dev_id, PRVIP_MASK, 0, IP4_BASEMASK,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_dess_nat_prv_base_mask_get(a_uint32_t dev_id, fal_ip4_addr_t * mask)
{
    sw_error_t rv;
    a_uint32_t data = 0;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_nat_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    HSL_REG_FIELD_GET(rv, dev_id, PRVIP_MASK, 0, IP4_BASEMASK,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    *mask = data;
    return SW_OK;
}

static sw_error_t
_dess_nat_pub_addr_commit(a_uint32_t dev_id, fal_nat_pub_addr_t * entry,
                          a_uint32_t op, a_uint32_t * empty)
{
    a_uint32_t index, addr, data, tbl[2] = { 0 };
    sw_error_t rv;

    *empty = DESS_PUB_ADDR_NUM;
    for (index = 0; index < DESS_PUB_ADDR_NUM; index++)
    {
        addr = DESS_PUB_ADDR_TBL1_ADDR + (index << 4);
        HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr, sizeof (a_uint32_t),
                              (a_uint8_t *) (&(tbl[1])), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        SW_GET_FIELD_BY_REG(PUB_ADDR1, ADDR_VALID, data, tbl[1]);
        if (data)
        {
            addr = DESS_PUB_ADDR_TBL0_ADDR + (index << 4);
            HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr, sizeof (a_uint32_t),
                                  (a_uint8_t *) (&(tbl[0])),
                                  sizeof (a_uint32_t));
            SW_RTN_ON_ERROR(rv);

            if (!aos_mem_cmp
                    ((void *) &(entry->pub_addr), (void *) &(tbl[0]),
                     sizeof (fal_ip4_addr_t)))
            {
                if (DESS_NAT_ENTRY_DEL == op)
                {
                    addr = DESS_PUB_ADDR_TBL1_ADDR + (index << 4);
                    tbl[1] = 0;
                    HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr, sizeof (a_uint32_t),
                                          (a_uint8_t *) (&(tbl[1])),
                                          sizeof (a_uint32_t));
                    *empty = index;
                    return rv;
                }
                else if (DESS_NAT_ENTRY_ADD == op)
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
_dess_nat_pub_addr_add(a_uint32_t dev_id, fal_nat_pub_addr_t * entry)
{
    sw_error_t rv;
    a_uint32_t i, empty, addr, data = 0, tbl[2] = { 0 };

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_nat_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    tbl[0] = entry->pub_addr;
    tbl[1] = 1;

    rv = _dess_nat_pub_addr_commit(dev_id, entry, DESS_NAT_ENTRY_ADD, &empty);
    if (SW_ALREADY_EXIST == rv)
    {
        return rv;
    }

    if (DESS_PUB_ADDR_NUM == empty)
    {
        return SW_NO_RESOURCE;
    }

    for (i = 0; i < 1; i++)
    {
        addr = DESS_PUB_ADDR_EDIT0_ADDR + (empty << 4) + (i << 2);
        HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr, sizeof (a_uint32_t),
                              (a_uint8_t *) (&(tbl[i])), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }

    addr = DESS_PUB_ADDR_OFFLOAD_ADDR + (empty << 2);
    HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr, sizeof (a_uint32_t),
                          (a_uint8_t *) (&(tbl[0])), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    addr = DESS_PUB_ADDR_VALID_ADDR;
    HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr, sizeof (a_uint32_t),
                          (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    data |= (0x1 << empty);
    HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr, sizeof (a_uint32_t),
                          (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    for (i = 0; i < 2; i++)
    {
        addr = DESS_PUB_ADDR_TBL0_ADDR + (empty << 4) + (i << 2);
        HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr, sizeof (a_uint32_t),
                              (a_uint8_t *) (&(tbl[i])), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }

    entry->entry_id = empty;
    return SW_OK;
}

static sw_error_t
_dess_nat_pub_addr_del(a_uint32_t dev_id, a_uint32_t del_mode,
                       fal_nat_pub_addr_t * entry)
{
    sw_error_t rv;
    a_uint32_t empty, addr, data = 0;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_nat_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    rv = _dess_nat_pub_addr_commit(dev_id, entry, DESS_NAT_ENTRY_DEL, &empty);
    SW_RTN_ON_ERROR(rv);

    addr = DESS_PUB_ADDR_VALID_ADDR;
    HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr, sizeof (a_uint32_t),
                          (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    data &= (~(0x1 << empty));
    HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr, sizeof (a_uint32_t),
                          (a_uint8_t *) (&data), sizeof (a_uint32_t));

    return rv;
}

static sw_error_t
_dess_nat_pub_addr_next(a_uint32_t dev_id, a_uint32_t next_mode,
                        fal_nat_pub_addr_t * entry)
{
    sw_error_t rv;
    a_uint32_t data, addr, idx, index, tbl[2] = {0};

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_nat_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    if (FAL_NEXT_ENTRY_FIRST_ID == entry->entry_id)
    {
        idx = 0;
    }
    else
    {
        if ((DESS_PUB_ADDR_NUM - 1) == entry->entry_id)
        {
            return SW_NO_MORE;
        }
        else
        {
            idx = entry->entry_id + 1;
        }
    }

    for (index = idx; index < DESS_PUB_ADDR_NUM; index++)
    {
        addr = DESS_PUB_ADDR_TBL1_ADDR + (index << 4);
        HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr, sizeof (a_uint32_t),
                              (a_uint8_t *) (&(tbl[1])), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        SW_GET_FIELD_BY_REG(PUB_ADDR1, ADDR_VALID, data, tbl[1]);
        if (data)
        {
            break;
        }
    }

    if (DESS_PUB_ADDR_NUM == index)
    {
        return SW_NO_MORE;
    }

    addr = DESS_PUB_ADDR_TBL0_ADDR + (index << 4);
    HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr, sizeof (a_uint32_t),
                          (a_uint8_t *) (&(tbl[0])), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    entry->entry_id = index;
    entry->pub_addr = tbl[0];

    return SW_OK;
}

static sw_error_t
_dess_nat_unk_session_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd)
{
    sw_error_t rv;
    a_uint32_t data;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_nat_feature_check(dev_id);
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
_dess_nat_unk_session_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;
    a_uint32_t data = 0;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_nat_feature_check(dev_id);
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

#define DESS_NAT_VRF_ENTRY_TBL_ADDR   0x0484
#define DESS_NAT_VRF_ENTRY_MASK_ADDR   0x0488

a_uint8_t _dess_snat_matched(a_uint32_t dev_id, fal_ip4_addr_t addr)
{
	a_bool_t nat_enable = 0, napt_enable = 0;
	fal_ip4_addr_t mask = 0, base = 0;
	a_uint32_t reg_addr;
	sw_error_t rv;

	_dess_nat_status_get(dev_id, &nat_enable);
	_dess_napt_status_get(dev_id, &napt_enable);
	if(!(nat_enable & napt_enable))
		return 0;

	/*check for private base ip*/
	reg_addr = DESS_NAT_VRF_ENTRY_MASK_ADDR;
	HSL_REG_ENTRY_GEN_GET(rv, dev_id, reg_addr, sizeof (a_uint32_t),
                          (a_uint8_t *) (&mask), sizeof (a_uint32_t));

	reg_addr = DESS_NAT_VRF_ENTRY_TBL_ADDR;
	HSL_REG_ENTRY_GEN_GET(rv, dev_id, reg_addr, sizeof (a_uint32_t),
                          (a_uint8_t *) (&base), sizeof (a_uint32_t));
	if (rv)
		return 0;
	if((mask&addr) == (mask&base)) {
		return 1;
	}

	return 0;
}

a_uint8_t _dess_dnat_matched(
		a_uint32_t dev_id,
		fal_ip4_addr_t addr,
		a_uint8_t *index)
{
	a_bool_t nat_enable = 0, napt_enable = 0;
	fal_nat_pub_addr_t entry;
	sw_error_t ret;

	_dess_nat_status_get(dev_id, &nat_enable);
	_dess_napt_status_get(dev_id, &napt_enable);
	if(!(nat_enable & napt_enable))
		return 0;

	/*check for public ip*/
	memset(&entry, 0, sizeof(entry));
	entry.entry_id = FAL_NEXT_ENTRY_FIRST_ID;
	while(1) {
		ret = _dess_nat_pub_addr_next(dev_id, 0, &entry);
		if(ret) {
			break;
		}
		if(entry.pub_addr == addr) {
			*index = entry.entry_id;
			return 1;
		}
	}
	return 0;
}



static sw_error_t
_dess_flow_cookie_snat_set(a_uint32_t dev_id, fal_flow_cookie_t * flow_cookie)
{
	fal_napt_entry_t entry;
	sw_error_t ret;

	memset(&entry, 0, sizeof(entry));
	entry.flags = FAL_NAT_ENTRY_TRANS_IPADDR_INDEX | flow_cookie->proto;
	entry.status = 0xf;
	entry.src_addr = flow_cookie->src_addr;
	entry.dst_addr = flow_cookie->dst_addr;
	entry.src_port = flow_cookie->src_port;
	entry.dst_port = flow_cookie->dst_port;
	entry.trans_port = flow_cookie->src_port;
	entry.action = FAL_MAC_RDT_TO_CPU;
	ret = _dess_napt_get(dev_id, 0, &entry);
	if(ret) {
		if(flow_cookie->flow_cookie == 0)
			return SW_OK;
	}
	if(flow_cookie->flow_cookie == 0) {
		if(entry.flow_cookie == 0) {
			ret = _dess_napt_del(dev_id, FAL_NAT_ENTRY_KEY_EN, &entry);
			#if defined(IN_NAT_HELPER)
			#if 0
			napt_cookie[entry.entry_id*2+1] = 0;
			#endif
			if (dess_nat_global_status)
				nat_helper_cookie_del(entry.entry_id);
			#endif
			return ret;
		}
		ret = _dess_napt_del(dev_id, FAL_NAT_ENTRY_KEY_EN, &entry);
	} else {
		entry.flow_cookie = flow_cookie->flow_cookie;
		ret = _dess_napt_add(dev_id, &entry);
	}

	return ret;
}

static sw_error_t
_dess_flow_cookie_dnat_set(
		a_uint32_t dev_id,
		fal_flow_cookie_t * flow_cookie,
		a_uint8_t index)
{
	fal_napt_entry_t entry;
	sw_error_t ret = 0;

	memset(&entry, 0, sizeof(entry));
	entry.flags = FAL_NAT_ENTRY_TRANS_IPADDR_INDEX | flow_cookie->proto;
	entry.status = 0xf;
	entry.trans_addr = index;
	entry.trans_port = flow_cookie->dst_port;
	entry.dst_addr = flow_cookie->src_addr;
	entry.dst_port = flow_cookie->src_port;
	entry.src_port = flow_cookie->dst_port;
	entry.action = FAL_MAC_RDT_TO_CPU;
	ret = _dess_napt_get(dev_id, 0, &entry);
	if(ret) {
		if(flow_cookie->flow_cookie == 0) {
			return SW_OK;
		} else {
			/*add a fresh flowcookie*/
			entry.flow_cookie = flow_cookie->flow_cookie;
			ret = _dess_napt_add(dev_id, &entry);
			return ret;
		}
	}
	if(flow_cookie->flow_cookie == 0) {
		/*del flow cookie*/
		if(entry.flow_cookie == 0) {
			ret = _dess_napt_del(dev_id, FAL_NAT_ENTRY_KEY_EN, &entry);
			#if defined(IN_NAT_HELPER)
			#if 0
			napt_cookie[entry.entry_id*2] = 0;
			#endif
			if (dess_nat_global_status)
				nat_helper_cookie_del(entry.entry_id);
			#endif
			return ret;
		}
		ret = _dess_napt_del(dev_id, FAL_NAT_ENTRY_KEY_EN, &entry);
		if(entry.load_balance & 4) {
			/*keep rfs*/
			entry.flow_cookie = 0;
			ret = _dess_napt_add(dev_id, &entry);
			return ret;
		}
	} else {
		/*add flow cookie*/
		ret = _dess_napt_del(dev_id, FAL_NAT_ENTRY_KEY_EN, &entry);
		entry.flow_cookie = flow_cookie->flow_cookie;
		ret = _dess_napt_add(dev_id, &entry);
		return ret;
	}
	return ret;

}

static sw_error_t
_dess_flow_rfs_dnat_set(
		a_uint32_t dev_id,
		a_uint8_t action,
		fal_flow_rfs_t * rfs,
		a_uint8_t index)
{
	fal_napt_entry_t entry;
	sw_error_t ret = 0;

	memset(&entry, 0, sizeof(entry));
	entry.flags = FAL_NAT_ENTRY_TRANS_IPADDR_INDEX | rfs->proto;
	entry.status = 0xf;
	entry.trans_addr = index;
	entry.trans_port = rfs->dst_port;
	entry.dst_addr = rfs->src_addr;
	entry.dst_port = rfs->src_port;
	entry.src_port = rfs->dst_port;
	entry.action = FAL_MAC_RDT_TO_CPU;
	ret = _dess_napt_get(dev_id, 0, &entry);
	if(ret) {
		if(action == 0) {
			return SW_FAIL;
		} else {
			/*add a fresh rfs*/
			entry.load_balance = rfs->load_balance | 4;
			ret = _dess_napt_add(dev_id, &entry);
			return ret;
		}
	}
	if(action == 0) {
		/*del flow rfs*/
		ret = _dess_napt_del(dev_id, FAL_NAT_ENTRY_KEY_EN, &entry);
		if(entry.flow_cookie != 0) {
			/*keep cookie*/
			entry.load_balance = 0;
			ret = _dess_napt_add(dev_id, &entry);
			return ret;
		}
	} else {
		/*add flow rfs*/
		ret = _dess_napt_del(dev_id, FAL_NAT_ENTRY_KEY_EN, &entry);
		entry.load_balance = rfs->load_balance | 4;
		ret = _dess_napt_add(dev_id, &entry);
		return ret;
	}
	return ret;

}


static sw_error_t
_dess_flow_cookie_set(a_uint32_t dev_id, fal_flow_cookie_t * flow_cookie)
{
	fal_napt_entry_t entry;
	sw_error_t ret;
	a_uint8_t index;

	if(_dess_dnat_matched(dev_id, flow_cookie->dst_addr, &index))
		return _dess_flow_cookie_dnat_set(dev_id, flow_cookie, index);
	if(_dess_snat_matched(dev_id, flow_cookie->src_addr))
		return _dess_flow_cookie_snat_set(dev_id, flow_cookie);

	/*normal flow*/
	memset(&entry, 0, sizeof(entry));
	entry.flags = flow_cookie->proto;
	entry.src_addr = flow_cookie->src_addr;
	entry.dst_addr = flow_cookie->dst_addr;
	entry.src_port = flow_cookie->src_port;
	entry.dst_port = flow_cookie->dst_port;
    ret = _dess_flow_get(0, 0, &entry);
	if(SW_OK != ret && flow_cookie->flow_cookie == 0)
		return ret;
	if(flow_cookie->flow_cookie == 0) {
		/*del*/
		_dess_flow_del(0, FAL_NAT_ENTRY_KEY_EN, &entry);
		if(entry.load_balance & 4) {
			entry.status = 0xf;
			entry.flow_cookie = 0;
			return _dess_flow_add(0, &entry);
		}
	} else {
		/*add*/
		if(ret == SW_OK)
			_dess_flow_del(0, FAL_NAT_ENTRY_KEY_EN, &entry);
		entry.status = 0xf;
		entry.flow_cookie = flow_cookie->flow_cookie;
		return _dess_flow_add(0, &entry);
	}
    return SW_OK;
}

static sw_error_t
_dess_flow_rfs_set(a_uint32_t dev_id, a_uint8_t action, fal_flow_rfs_t * rfs)
{
	fal_napt_entry_t entry;
	sw_error_t ret;
	a_uint8_t index;

	if(_dess_dnat_matched(dev_id, rfs->dst_addr, &index))
		return _dess_flow_rfs_dnat_set(dev_id, action, rfs, index);

	memset(&entry, 0, sizeof(entry));
	entry.flags = rfs->proto;
	entry.src_addr = rfs->src_addr;
	entry.dst_addr = rfs->dst_addr;
	entry.src_port = rfs->src_port;
	entry.dst_port = rfs->dst_port;
	ret = _dess_flow_get(0, 0, &entry);
	if(SW_OK != ret && action == 0)
		return ret;
	if(action == 0) {
		/*del*/
		_dess_flow_del(0, FAL_NAT_ENTRY_KEY_EN, &entry);
		if(entry.flow_cookie != 0) {
			entry.load_balance = 0;
			return _dess_flow_add(0, &entry);
		}
	} else {
		/*add*/
		if(ret == SW_OK)
			_dess_flow_del(0, FAL_NAT_ENTRY_KEY_EN, &entry);
		entry.status = 0xf;
		entry.load_balance = rfs->load_balance | 0x4;
		return _dess_flow_add(0, &entry);
	}
	return SW_OK;
}



sw_error_t
dess_nat_reset(a_uint32_t dev_id)
{
    sw_error_t rv;
    a_uint32_t index, addr, data = 0;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_nat_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    dess_nat_snap[dev_id] = 0;

    HSL_REG_ENTRY_SET(rv, dev_id, HOST_ENTRY7, 0, (a_uint8_t *) (&data),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    rv = _dess_nat_entry_commit(dev_id, DESS_ENTRY_NAT, DESS_NAT_ENTRY_FLUSH);
    SW_RTN_ON_ERROR(rv);

    HSL_REG_ENTRY_SET(rv, dev_id, HOST_ENTRY7, 0, (a_uint8_t *) (&data),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    rv = _dess_nat_entry_commit(dev_id, DESS_ENTRY_NAPT, DESS_NAT_ENTRY_FLUSH);
    SW_RTN_ON_ERROR(rv);

	rv = _dess_nat_entry_commit(dev_id, DESS_ENTRY_FLOW, DESS_NAT_ENTRY_FLUSH);
    SW_RTN_ON_ERROR(rv);

    for (index = 0; index < DESS_PUB_ADDR_NUM; index++)
    {
        addr = DESS_PUB_ADDR_TBL1_ADDR + (index << 4);
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
dess_nat_add(a_uint32_t dev_id, fal_nat_entry_t * nat_entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_nat_add(dev_id, nat_entry);
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
dess_nat_del(a_uint32_t dev_id, a_uint32_t del_mode,
             fal_nat_entry_t * nat_entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_nat_del(dev_id, del_mode, nat_entry);
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
dess_nat_get(a_uint32_t dev_id, a_uint32_t get_mode,
             fal_nat_entry_t * nat_entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_nat_get(dev_id, get_mode, nat_entry);
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
dess_nat_next(a_uint32_t dev_id, a_uint32_t next_mode,
              fal_nat_entry_t * nat_entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_nat_next(dev_id, next_mode, nat_entry);
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
dess_nat_counter_bind(a_uint32_t dev_id, a_uint32_t entry_id, a_uint32_t cnt_id,
                      a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_nat_counter_bind(dev_id, entry_id, cnt_id, enable);
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
dess_napt_add(a_uint32_t dev_id, fal_napt_entry_t * napt_entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_napt_add(dev_id, napt_entry);
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
dess_napt_del(a_uint32_t dev_id, a_uint32_t del_mode,
              fal_napt_entry_t * napt_entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_napt_del(dev_id, del_mode, napt_entry);
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
dess_napt_get(a_uint32_t dev_id, a_uint32_t get_mode,
              fal_napt_entry_t * napt_entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_napt_get(dev_id, get_mode, napt_entry);
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
dess_napt_next(a_uint32_t dev_id, a_uint32_t next_mode,
               fal_napt_entry_t * napt_entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_napt_next(dev_id, next_mode, napt_entry);
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
dess_napt_counter_bind(a_uint32_t dev_id, a_uint32_t entry_id,
                       a_uint32_t cnt_id, a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_napt_counter_bind(dev_id, entry_id, cnt_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Add one FLOW entry to one particular device.
 *   @details Comments:
       Before FLOW entry added related ip4 private base address must be set
       at first.
       In parameter napt_entry related entry flags must be set
       Hardware entry id will be returned.
 * @param[in] dev_id device id
 * @param[in] napt_entry FLOW entry parameter
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_flow_add(a_uint32_t dev_id, fal_napt_entry_t * napt_entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_flow_add(dev_id, napt_entry);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Del FLOW entries from one particular device.
 * @param[in] dev_id device id
 * @param[in] del_mode FLOW entry delete operation mode
 * @param[in] napt_entry FLOW entry parameter
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_flow_del(a_uint32_t dev_id, a_uint32_t del_mode,
              fal_napt_entry_t * napt_entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_flow_del(dev_id, del_mode, napt_entry);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get one FLOW entry from one particular device.
 * @param[in] dev_id device id
 * @param[in] get_mode FLOW entry get operation mode
 * @param[in] nat_entry FLOW entry parameter
 * @param[out] nat_entry FLOW entry parameter
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_flow_get(a_uint32_t dev_id, a_uint32_t get_mode,
              fal_napt_entry_t * napt_entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_flow_get(dev_id, get_mode, napt_entry);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Next FLOW entries from one particular device.
 * @param[in] dev_id device id
 * @param[in] next_mode FLOW entry next operation mode
 * @param[in] napt_entry FLOW entry parameter
 * @param[out] napt_entry FLOW entry parameter
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_flow_next(a_uint32_t dev_id, a_uint32_t next_mode,
               fal_napt_entry_t * napt_entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_flow_next(dev_id, next_mode, napt_entry);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Bind one counter entry to one FLOW entry to one particular device.
 * @param[in] dev_id device id
 * @param[in] entry_id FLOW entry id
 * @param[in] cnt_id counter entry id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_flow_counter_bind(a_uint32_t dev_id, a_uint32_t entry_id,
                       a_uint32_t cnt_id, a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_flow_counter_bind(dev_id, entry_id, cnt_id, enable);
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
dess_nat_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_nat_status_set(dev_id, enable);
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
dess_nat_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_nat_status_get(dev_id, enable);
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
dess_nat_hash_mode_set(a_uint32_t dev_id, a_uint32_t mode)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_nat_hash_mode_set(dev_id, mode);
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
dess_nat_hash_mode_get(a_uint32_t dev_id, a_uint32_t * mode)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_nat_hash_mode_get(dev_id, mode);
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
dess_napt_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_napt_status_set(dev_id, enable);
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
dess_napt_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_napt_status_get(dev_id, enable);
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
dess_napt_mode_set(a_uint32_t dev_id, fal_napt_mode_t mode)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_napt_mode_set(dev_id, mode);
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
dess_napt_mode_get(a_uint32_t dev_id, fal_napt_mode_t * mode)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_napt_mode_get(dev_id, mode);
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
dess_nat_prv_base_addr_set(a_uint32_t dev_id, fal_ip4_addr_t addr)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_nat_prv_base_addr_set(dev_id, addr);
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
dess_nat_prv_base_addr_get(a_uint32_t dev_id, fal_ip4_addr_t * addr)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_nat_prv_base_addr_get(dev_id, addr);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set IP4 private base address on a particular device
 * @param[in] dev_id device id
 * @param[in] mask private base mask
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_nat_prv_base_mask_set(a_uint32_t dev_id, fal_ip4_addr_t mask)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_nat_prv_base_mask_set(dev_id, mask);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get IP4 private base address on a particular device
 * @param[in] dev_id device id
 * @param[out] mask private base mask
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_nat_prv_base_mask_get(a_uint32_t dev_id, fal_ip4_addr_t * mask)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_nat_prv_base_mask_get(dev_id, mask);
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
dess_nat_pub_addr_add(a_uint32_t dev_id, fal_nat_pub_addr_t * entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_nat_pub_addr_add(dev_id, entry);
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
dess_nat_pub_addr_del(a_uint32_t dev_id, a_uint32_t del_mode,
                      fal_nat_pub_addr_t * entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_nat_pub_addr_del(dev_id, del_mode, entry);
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
dess_nat_pub_addr_next(a_uint32_t dev_id, a_uint32_t next_mode,
                       fal_nat_pub_addr_t * entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_nat_pub_addr_next(dev_id, next_mode, entry);
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
dess_nat_unk_session_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_nat_unk_session_cmd_set(dev_id, cmd);
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
dess_nat_unk_session_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_nat_unk_session_cmd_get(dev_id, cmd);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set working status of NAT engine on a particular device
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @param[in] portbmp port bitmap
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_nat_global_set(a_uint32_t dev_id, a_bool_t enable, a_uint32_t portbmp)
{
    sw_error_t rv = SW_OK;

    HSL_API_LOCK;
    printk("enable:%d\n", enable);
    if(enable) {
        if(dess_nat_global_status == 0) {
            dess_nat_global_status = 1;
#if defined(IN_NAT_HELPER)
            DESS_NAT_HELPER_INIT(rv, dev_id, portbmp);
#endif
        }
    } else {
        if(dess_nat_global_status == 1) {
            dess_nat_global_status = 0;
#if defined(IN_NAT_HELPER)
            DESS_NAT_HELPER_CLEANUP(rv, dev_id);
#endif
        }
    }
    //rv = SW_OK;
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Add/del one FLOW cookie entry to one particular device.
 *   @details Comments:
       Before FLOW entry added related ip4 private base address must be set
       at first.
       In parameter napt_entry related entry flags must be set
       Hardware entry id will be returned.
 * @param[in] dev_id device id
 * @param[in]  FLOW cookie entry parameter
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_flow_cookie_set(a_uint32_t dev_id, fal_flow_cookie_t * flow_cookie)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_flow_cookie_set(dev_id, flow_cookie);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Add/del one FLOW rfs entry to one particular device.
 *   @details Comments:
       Before FLOW entry added related ip4 private base address must be set
       at first.
       In parameter napt_entry related entry flags must be set
       Hardware entry id will be returned.
 * @param[in] dev_id device id
 * @param[in]  FLOW cookie entry parameter
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_flow_rfs_set(a_uint32_t dev_id, a_uint8_t action, fal_flow_rfs_t * rfs)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_flow_rfs_set(dev_id, action, rfs);
    HSL_API_UNLOCK;
    return rv;
}



sw_error_t
dess_nat_init(a_uint32_t dev_id)
{
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);
	aos_lock_init(&dess_nat_lock);

    rv = dess_nat_reset(dev_id);
    SW_RTN_ON_ERROR(rv);

#ifndef HSL_STANDALONG
    {
        hsl_api_t *p_api;

        SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

        p_api->nat_add = dess_nat_add;
        p_api->nat_del = dess_nat_del;
        p_api->nat_get = dess_nat_get;
        p_api->nat_next = dess_nat_next;
        p_api->nat_counter_bind = dess_nat_counter_bind;
        p_api->napt_add = dess_napt_add;
        p_api->napt_del = dess_napt_del;
        p_api->napt_get = dess_napt_get;
        p_api->napt_next = dess_napt_next;
        p_api->napt_counter_bind = dess_napt_counter_bind;
		p_api->flow_add = dess_flow_add;
        p_api->flow_del = dess_flow_del;
        p_api->flow_get = dess_flow_get;
        p_api->flow_next = dess_flow_next;
        p_api->flow_counter_bind = dess_flow_counter_bind;
        p_api->nat_status_set = dess_nat_status_set;
        p_api->nat_status_get = dess_nat_status_get;
        p_api->nat_hash_mode_set = dess_nat_hash_mode_set;
        p_api->nat_hash_mode_get = dess_nat_hash_mode_get;
        p_api->napt_status_set = dess_napt_status_set;
        p_api->napt_status_get = dess_napt_status_get;
        p_api->napt_mode_set = dess_napt_mode_set;
        p_api->napt_mode_get = dess_napt_mode_get;
        p_api->nat_pub_addr_add = dess_nat_pub_addr_add;
        p_api->nat_pub_addr_del = dess_nat_pub_addr_del;
        p_api->nat_pub_addr_next = dess_nat_pub_addr_next;
        p_api->nat_unk_session_cmd_set = dess_nat_unk_session_cmd_set;
        p_api->nat_unk_session_cmd_get = dess_nat_unk_session_cmd_get;
        p_api->nat_prv_base_addr_set = dess_nat_prv_base_addr_set;
        p_api->nat_prv_base_addr_get = dess_nat_prv_base_addr_get;
        p_api->nat_prv_base_mask_set = dess_nat_prv_base_mask_set;
        p_api->nat_prv_base_mask_get = dess_nat_prv_base_mask_get;
        p_api->nat_global_set = dess_nat_global_set;
		p_api->flow_cookie_set = dess_flow_cookie_set;
		p_api->flow_rfs_set = dess_flow_rfs_set;
    }
#endif

    return SW_OK;
}

/**
 * @}
 */
