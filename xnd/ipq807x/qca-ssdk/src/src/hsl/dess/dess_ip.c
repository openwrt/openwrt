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
 * @defgroup dess_ip DESS_IP
 * @{
 */

#include "sw.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_port_prop.h"
#include "dess_ip.h"
#include "dess_reg.h"

#define DESS_HOST_ENTRY_DATA0_ADDR              0x0e80
#define DESS_HOST_ENTRY_DATA1_ADDR              0x0e84
#define DESS_HOST_ENTRY_DATA2_ADDR              0x0e88
#define DESS_HOST_ENTRY_DATA3_ADDR              0x0e8c
#define DESS_HOST_ENTRY_DATA4_ADDR              0x0e90
#define DESS_HOST_ENTRY_DATA5_ADDR              0x0e94
#define DESS_HOST_ENTRY_DATA6_ADDR              0x0e98
#define DESS_HOST_ENTRY_DATA7_ADDR              0x0e58

#define DESS_HOST_ENTRY_REG_NUM                 8

#define DESS_HOST_ENTRY_FLUSH                   1
#define DESS_HOST_ENTRY_ADD                     2
#define DESS_HOST_ENTRY_DEL                     3
#define DESS_HOST_ENTRY_NEXT                    4
#define DESS_HOST_ENTRY_SEARCH                  5

#define DESS_ENTRY_ARP                          3

#define DESS_INTF_MAC_ADDR_NUM                  8
#define DESS_INTF_MAC_TBL0_ADDR                 0x5a900
#define DESS_INTF_MAC_TBL1_ADDR                 0x5a904
#define DESS_INTF_MAC_TBL2_ADDR                 0x5a908
#define DESS_INTF_MAC_EDIT0_ADDR                0x02000
#define DESS_INTF_MAC_EDIT1_ADDR                0x02004
#define DESS_INTF_MAC_EDIT2_ADDR                0x02008

#define DESS_IP6_BASE_ADDR                      0x0470

#define DESS_HOST_ENTRY_NUM                     128

#define DESS_IP_COUTER_ADDR                     0x2b000

#define DESS_DEFAULT_ROUTE_NUM                  8
#define DESS_IP4_DEFAULT_ROUTE_TBL_ADDR                 0x004c4
#define DESS_IP6_DEFAULT_ROUTE_TBL_ADDR                 0x004e4
#define DESS_HOST_ROUTE_NUM                  16
#define DESS_IP4_HOST_ROUTE_TBL0_ADDR                 0x5b000
#define DESS_IP4_HOST_ROUTE_TBL1_ADDR                 0x5b004
#define DESS_IP6_HOST_ROUTE_TBL0_ADDR                 0x5b100
#define DESS_IP6_HOST_ROUTE_TBL1_ADDR                 0x5b104
#define DESS_IP6_HOST_ROUTE_TBL2_ADDR                 0x5b108
#define DESS_IP6_HOST_ROUTE_TBL3_ADDR                 0x5b10c
#define DESS_IP6_HOST_ROUTE_TBL4_ADDR                 0x5b110


static a_uint32_t dess_mac_snap[SW_MAX_NR_DEV] = { 0 };
static fal_intf_mac_entry_t dess_intf_snap[SW_MAX_NR_DEV][DESS_INTF_MAC_ADDR_NUM];

extern aos_lock_t dess_nat_lock;


static void
_dess_ip_pt_learn_save(a_uint32_t dev_id, a_uint32_t * status)
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
_dess_ip_pt_learn_restore(a_uint32_t dev_id, a_uint32_t status)
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
_dess_ip_feature_check(a_uint32_t dev_id)
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
_dess_ip_counter_get(a_uint32_t dev_id, a_uint32_t cnt_id,
                     a_uint32_t counter[2])
{
    sw_error_t rv;
    a_uint32_t i, addr;

    addr = DESS_IP_COUTER_ADDR + (cnt_id << 3);
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
_dess_host_entry_commit(a_uint32_t dev_id, a_uint32_t entry_type, a_uint32_t op)
{
    a_uint32_t busy = 1, i = 0x9000000, entry = 0, j, try_num;
    a_uint32_t learn_status = 0, data = 0;
    sw_error_t rv;

    while (busy && --i)
    {
        HSL_REG_ENTRY_GET(rv, dev_id, HOST_ENTRY7, 0, (a_uint8_t *) (&entry),
                          sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
        //printk("IP first entry is 0x%x\r\n", entry);
        SW_GET_FIELD_BY_REG(HOST_ENTRY7, TBL_BUSY, busy, entry);
    }

    if (i == 0)
    {
	printk("host entry busy!\n");
        return SW_BUSY;
    }


    /* hardware requirements, we should disable ARP learn at first */
    /* and maybe we should try several times... */
    _dess_ip_pt_learn_save(dev_id, &learn_status);

    //printk("data0=0x%x\n", data);
    if(learn_status) {
        aos_mdelay(800);
        HSL_REG_ENTRY_GET(rv, dev_id, ROUTER_PTCTRL2, 0,
                          (a_uint8_t *) (&data), sizeof (a_uint32_t));
    }
        //printk("data1=0x%x\n", data);


    SW_SET_REG_BY_FIELD(HOST_ENTRY7, TBL_BUSY, 1, entry);
    SW_SET_REG_BY_FIELD(HOST_ENTRY7, TBL_SEL, entry_type, entry);
    SW_SET_REG_BY_FIELD(HOST_ENTRY7, ENTRY_FUNC, op, entry);

    HSL_REG_ENTRY_SET(rv, dev_id, HOST_ENTRY7, 0, (a_uint8_t *) (&entry),
			sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (learn_status)
    {
        try_num = 300;
    }
    else
    {
        try_num = 1;
    }

    for (j = 0; j < try_num; j++)
    {
        busy = 1;
	i = 0x9000000;
        while (busy && --i)
        {
            HSL_REG_ENTRY_GET(rv, dev_id, HOST_ENTRY7, 0, (a_uint8_t *) (&entry),
                              sizeof (a_uint32_t));
	    //printk("IP second entry is 0x%x\r\n", entry);
            if (SW_OK != rv)
            {
                _dess_ip_pt_learn_restore(dev_id, learn_status);
                return rv;
            }
            SW_GET_FIELD_BY_REG(HOST_ENTRY7, TBL_BUSY, busy, entry);
        }

        if (i == 0)
        {
            _dess_ip_pt_learn_restore(dev_id, learn_status);
            return SW_BUSY;
        }

        /* hardware requirement, we should read again... */
        HSL_REG_ENTRY_GET(rv, dev_id, HOST_ENTRY7, 0, (a_uint8_t *) (&entry),
                          sizeof (a_uint32_t));
        if (SW_OK != rv)
        {
            _dess_ip_pt_learn_restore(dev_id, learn_status);
            return rv;
        }

        /* operation success...... */
        SW_GET_FIELD_BY_REG(HOST_ENTRY7, TBL_STAUS, busy, entry);
        if (busy)
        {
            _dess_ip_pt_learn_restore(dev_id, learn_status);
            return SW_OK;
        }
    }

    _dess_ip_pt_learn_restore(dev_id, learn_status);
    if (DESS_HOST_ENTRY_NEXT == op)
    {
        return SW_NO_MORE;
    }
    else if (DESS_HOST_ENTRY_SEARCH == op)
    {
        return SW_NOT_FOUND;
    }
    else if (DESS_HOST_ENTRY_DEL == op)
    {
        return SW_NOT_FOUND;
    }
    else
    {
        return SW_FAIL;
    }
}

static sw_error_t
_dess_ip_intf_sw_to_hw(a_uint32_t dev_id, fal_host_entry_t * entry,
                       a_uint32_t * hw_intf)
{
    sw_error_t rv;
    a_uint32_t addr, lvid, hvid, tbl[3] = {0}, i;
    a_uint32_t sw_intf = entry->intf_id;
    a_uint32_t vid_offset;

    for (i = 0; i < DESS_INTF_MAC_ADDR_NUM; i++)
    {
        if (dess_mac_snap[dev_id] & (0x1 << i))
        {
            addr = DESS_INTF_MAC_TBL0_ADDR + (i << 4) + 4;
            HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr, sizeof (a_uint32_t),
                                  (a_uint8_t *) (&(tbl[1])),
                                  sizeof (a_uint32_t));
            SW_RTN_ON_ERROR(rv);

            addr = DESS_INTF_MAC_TBL0_ADDR + (i << 4) + 8;
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
_dess_ip_intf_hw_to_sw(a_uint32_t dev_id, a_uint32_t hw_intf,
                       a_uint32_t * sw_intf)
{
    sw_error_t rv;
    a_uint32_t addr, lvid, tbl = 0, i;

    i = hw_intf & 0x7;

    addr = DESS_INTF_MAC_TBL0_ADDR + (i << 4) + 4;
    HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr, sizeof (a_uint32_t),
                          (a_uint8_t *) (&tbl), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    SW_GET_FIELD_BY_REG(INTF_ADDR_ENTRY1, VID_LOW, lvid, tbl);
    *sw_intf = lvid + (hw_intf >> 3);

    return SW_OK;
}

static sw_error_t
_dess_ip_age_time_set(a_uint32_t dev_id, a_uint32_t * time)
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
_dess_ip_age_time_get(a_uint32_t dev_id, a_uint32_t * time)
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
_dess_host_sw_to_hw(a_uint32_t dev_id, fal_host_entry_t * entry,
                    a_uint32_t reg[])
{
    sw_error_t rv;
    a_uint32_t data;

    if (FAL_IP_IP4_ADDR & entry->flags)
    {
        reg[0] = entry->ip4_addr;
        SW_SET_REG_BY_FIELD(HOST_ENTRY6, IP_VER, 0, reg[6]);
    }

    if (FAL_IP_IP6_ADDR & entry->flags)
    {
        reg[0] = entry->ip6_addr.ul[3];
        reg[1] = entry->ip6_addr.ul[2];
        reg[2] = entry->ip6_addr.ul[1];
        reg[3] = entry->ip6_addr.ul[0];
        SW_SET_REG_BY_FIELD(HOST_ENTRY6, IP_VER, 1, reg[6]);
    }

    SW_SET_REG_BY_FIELD(HOST_ENTRY4, MAC_ADDR2, entry->mac_addr.uc[2], reg[4]);
    SW_SET_REG_BY_FIELD(HOST_ENTRY4, MAC_ADDR3, entry->mac_addr.uc[3], reg[4]);
    SW_SET_REG_BY_FIELD(HOST_ENTRY4, MAC_ADDR4, entry->mac_addr.uc[4], reg[4]);
    SW_SET_REG_BY_FIELD(HOST_ENTRY4, MAC_ADDR5, entry->mac_addr.uc[5], reg[4]);
    SW_SET_REG_BY_FIELD(HOST_ENTRY5, MAC_ADDR0, entry->mac_addr.uc[0], reg[5]);
    SW_SET_REG_BY_FIELD(HOST_ENTRY5, MAC_ADDR1, entry->mac_addr.uc[1], reg[5]);

    rv = _dess_ip_intf_sw_to_hw(dev_id, entry/*was:->intf_id*/, &data);

    SW_RTN_ON_ERROR(rv);
    SW_SET_REG_BY_FIELD(HOST_ENTRY5, INTF_ID, data, reg[5]);

#if 0
    if (A_TRUE != hsl_port_prop_check(dev_id, entry->port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }
#endif

    SW_SET_REG_BY_FIELD(HOST_ENTRY5, SRC_PORT, entry->port_id, reg[5]);

    if (FAL_IP_CPU_ADDR & entry->flags)
    {
        SW_SET_REG_BY_FIELD(HOST_ENTRY5, CPU_ADDR, 1, reg[5]);
    }

    SW_SET_REG_BY_FIELD(HOST_ENTRY6, AGE_FLAG, entry->status, reg[6]);
    SW_SET_REG_BY_FIELD(HOST_ENTRY6, VRF_ID, entry->vrf_id, reg[6]);
    SW_SET_REG_BY_FIELD(HOST_ENTRY6, LB_BIT, entry->lb_num, reg[6]);

    if ((A_TRUE == entry->mirror_en) && (FAL_MAC_FRWRD != entry->action))
    {
        return SW_NOT_SUPPORTED;
    }

    if (A_TRUE == entry->counter_en)
    {
        SW_SET_REG_BY_FIELD(HOST_ENTRY6, CNT_EN, 1, reg[6]);
        SW_SET_REG_BY_FIELD(HOST_ENTRY6, CNT_IDX, entry->counter_id, reg[6]);
    }

    if (FAL_MAC_DROP == entry->action)
    {
        SW_SET_REG_BY_FIELD(HOST_ENTRY5, SRC_PORT, 7, reg[5]);
        SW_SET_REG_BY_FIELD(HOST_ENTRY6, ACTION, 3, reg[6]);
    }
    else if (FAL_MAC_RDT_TO_CPU == entry->action)
    {
        SW_SET_REG_BY_FIELD(HOST_ENTRY6, ACTION, 1, reg[6]);
    }
    else if (FAL_MAC_CPY_TO_CPU == entry->action)
    {
        SW_SET_REG_BY_FIELD(HOST_ENTRY6, ACTION, 2, reg[6]);
    }
    else
    {
        if (A_TRUE == entry->mirror_en)
        {
            SW_SET_REG_BY_FIELD(HOST_ENTRY6, ACTION, 0, reg[6]);
        }
        else
        {
            SW_SET_REG_BY_FIELD(HOST_ENTRY6, ACTION, 3, reg[6]);
        }
    }

    return SW_OK;
}

static sw_error_t
_dess_host_hw_to_sw(a_uint32_t dev_id, a_uint32_t reg[],
                    fal_host_entry_t * entry)
{
    sw_error_t rv;
    a_uint32_t data = 0, cnt[2] = {0};

    SW_GET_FIELD_BY_REG(HOST_ENTRY6, IP_VER, data, reg[6]);
    if (data)
    {
        entry->ip6_addr.ul[0] = reg[3];
        entry->ip6_addr.ul[1] = reg[2];
        entry->ip6_addr.ul[2] = reg[1];
        entry->ip6_addr.ul[3] = reg[0];
        entry->flags |= FAL_IP_IP6_ADDR;
    }
    else
    {
        entry->ip4_addr = reg[0];
        entry->flags |= FAL_IP_IP4_ADDR;
    }

    SW_GET_FIELD_BY_REG(HOST_ENTRY4, MAC_ADDR2, entry->mac_addr.uc[2], reg[4]);
    SW_GET_FIELD_BY_REG(HOST_ENTRY4, MAC_ADDR3, entry->mac_addr.uc[3], reg[4]);
    SW_GET_FIELD_BY_REG(HOST_ENTRY4, MAC_ADDR4, entry->mac_addr.uc[4], reg[4]);
    SW_GET_FIELD_BY_REG(HOST_ENTRY4, MAC_ADDR5, entry->mac_addr.uc[5], reg[4]);
    SW_GET_FIELD_BY_REG(HOST_ENTRY5, MAC_ADDR0, entry->mac_addr.uc[0], reg[5]);
    SW_GET_FIELD_BY_REG(HOST_ENTRY5, MAC_ADDR1, entry->mac_addr.uc[1], reg[5]);

    SW_GET_FIELD_BY_REG(HOST_ENTRY5, INTF_ID, data, reg[5]);
    rv = _dess_ip_intf_hw_to_sw(dev_id, data, &(entry->intf_id));
    SW_RTN_ON_ERROR(rv);

    SW_GET_FIELD_BY_REG(HOST_ENTRY5, SRC_PORT, entry->port_id, reg[5]);

    SW_GET_FIELD_BY_REG(HOST_ENTRY5, CPU_ADDR, data, reg[5]);
    if (data)
    {
        entry->flags |= FAL_IP_CPU_ADDR;
    }

    SW_GET_FIELD_BY_REG(HOST_ENTRY6, AGE_FLAG, entry->status, reg[6]);
    SW_GET_FIELD_BY_REG(HOST_ENTRY6, VRF_ID, entry->vrf_id, reg[6]);
    SW_GET_FIELD_BY_REG(HOST_ENTRY6, LB_BIT, entry->lb_num, reg[6]);

    SW_GET_FIELD_BY_REG(HOST_ENTRY6, CNT_EN, data, reg[6]);
    if (data)
    {
        entry->counter_en = A_TRUE;
        SW_GET_FIELD_BY_REG(HOST_ENTRY6, CNT_IDX, entry->counter_id, reg[6]);

        rv = _dess_ip_counter_get(dev_id, entry->counter_id, cnt);
        SW_RTN_ON_ERROR(rv);

        entry->packet = cnt[0];
        entry->byte = cnt[1];
    }
    else
    {
        entry->counter_en = A_FALSE;
    }

    SW_GET_FIELD_BY_REG(HOST_ENTRY6, PPPOE_EN, data, reg[6]);
    if (data)
    {
        entry->pppoe_en = A_TRUE;
        SW_GET_FIELD_BY_REG(HOST_ENTRY6, PPPOE_IDX, data, reg[6]);
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
        SW_GET_FIELD_BY_REG(HOST_ENTRY6, ACTION, data, reg[6]);
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
_dess_host_down_to_hw(a_uint32_t dev_id, a_uint32_t reg[])
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
_dess_host_up_to_sw(a_uint32_t dev_id, a_uint32_t reg[])
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
        HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr, sizeof (a_uint32_t),
                              (a_uint8_t *) (&reg[i]), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }

    return SW_OK;
}

static sw_error_t
_dess_ip_host_add(a_uint32_t dev_id, fal_host_entry_t * entry)
{
    sw_error_t rv;
    a_uint32_t reg[DESS_HOST_ENTRY_REG_NUM] = { 0 };

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_ip_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    rv = _dess_host_sw_to_hw(dev_id, entry, reg);
    SW_RTN_ON_ERROR(rv);

	aos_lock_bh(&dess_nat_lock);
    rv = _dess_host_down_to_hw(dev_id, reg);
	if (rv != SW_OK) {
		aos_unlock_bh(&dess_nat_lock);
		return rv;
	}

    rv = _dess_host_entry_commit(dev_id, DESS_ENTRY_ARP, DESS_HOST_ENTRY_ADD);
	if (rv != SW_OK) {
		aos_unlock_bh(&dess_nat_lock);
		return rv;
	}

    HSL_REG_ENTRY_GET(rv, dev_id, HOST_ENTRY7, 0, (a_uint8_t *) (&reg[7]),
                      sizeof (a_uint32_t));
	if (rv != SW_OK) {
		aos_unlock_bh(&dess_nat_lock);
		return rv;
	}
	aos_unlock_bh(&dess_nat_lock);
    SW_GET_FIELD_BY_REG(HOST_ENTRY7, TBL_IDX, entry->entry_id, reg[7]);

    return SW_OK;
}

static sw_error_t
_dess_ip_host_del(a_uint32_t dev_id, a_uint32_t del_mode,
                  fal_host_entry_t * entry)
{
    sw_error_t rv;
    a_uint32_t data, reg[DESS_HOST_ENTRY_REG_NUM] = { 0 }, op = DESS_HOST_ENTRY_FLUSH;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_ip_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    if (FAL_IP_ENTRY_ID_EN & del_mode)
    {
        return SW_NOT_SUPPORTED;
    }

    if (FAL_IP_ENTRY_IPADDR_EN & del_mode)
    {
        op = DESS_HOST_ENTRY_DEL;
        if (FAL_IP_IP4_ADDR & entry->flags)
        {
            reg[0] = entry->ip4_addr;
        }

        if (FAL_IP_IP6_ADDR & entry->flags)
        {
            reg[0] = entry->ip6_addr.ul[3];
            reg[1] = entry->ip6_addr.ul[2];
            reg[2] = entry->ip6_addr.ul[1];
            reg[3] = entry->ip6_addr.ul[0];
            SW_SET_REG_BY_FIELD(HOST_ENTRY6, IP_VER, 1, reg[6]);
        }
    }

    if (FAL_IP_ENTRY_INTF_EN & del_mode)
    {
        rv = _dess_ip_intf_sw_to_hw(dev_id, entry/*was:->intf_id*/, &data);
        SW_RTN_ON_ERROR(rv);

        SW_SET_REG_BY_FIELD(HOST_ENTRY7, SPEC_VID, 1, reg[7]);
        SW_SET_REG_BY_FIELD(HOST_ENTRY5, INTF_ID, data, reg[5]);
    }

    if (FAL_IP_ENTRY_PORT_EN & del_mode)
    {
        SW_SET_REG_BY_FIELD(HOST_ENTRY7, SPEC_SP, 1, reg[7]);
        SW_SET_REG_BY_FIELD(HOST_ENTRY5, SRC_PORT, entry->port_id, reg[5]);
    }

    if (FAL_IP_ENTRY_STATUS_EN & del_mode)
    {
        SW_SET_REG_BY_FIELD(HOST_ENTRY7, SPEC_STATUS, 1, reg[7]);
        SW_SET_REG_BY_FIELD(HOST_ENTRY6, AGE_FLAG, entry->status, reg[6]);
    }
	aos_lock_bh(&dess_nat_lock);
    rv = _dess_host_down_to_hw(dev_id, reg);
	if (rv != SW_OK) {
		aos_unlock_bh(&dess_nat_lock);
		return rv;
	}

    rv = _dess_host_entry_commit(dev_id, DESS_ENTRY_ARP, op);
	aos_unlock_bh(&dess_nat_lock);

    return rv;
}

static sw_error_t
_dess_ip_host_get(a_uint32_t dev_id, a_uint32_t get_mode,
                  fal_host_entry_t * entry)
{
    sw_error_t rv;
    a_uint32_t reg[DESS_HOST_ENTRY_REG_NUM] = { 0 };

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_ip_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    if (FAL_IP_ENTRY_IPADDR_EN != get_mode)
    {
        return SW_NOT_SUPPORTED;
    }

    if (FAL_IP_IP4_ADDR & entry->flags)
    {
        reg[0] = entry->ip4_addr;
    }
    else
    {
        reg[0] = entry->ip6_addr.ul[3];
        reg[1] = entry->ip6_addr.ul[2];
        reg[2] = entry->ip6_addr.ul[1];
        reg[3] = entry->ip6_addr.ul[0];
        SW_SET_REG_BY_FIELD(HOST_ENTRY6, IP_VER, 1, reg[6]);
    }
	aos_lock_bh(&dess_nat_lock);
    rv = _dess_host_down_to_hw(dev_id, reg);
	if (rv != SW_OK) {
		aos_unlock_bh(&dess_nat_lock);
		return rv;
	}

    rv = _dess_host_entry_commit(dev_id, DESS_ENTRY_ARP,
                                 DESS_HOST_ENTRY_SEARCH);
	if (rv != SW_OK) {
		aos_unlock_bh(&dess_nat_lock);
		return rv;
	}

    rv = _dess_host_up_to_sw(dev_id, reg);
	if (rv != SW_OK) {
		aos_unlock_bh(&dess_nat_lock);
		return rv;
	}

    aos_mem_zero(entry, sizeof (fal_host_entry_t));

    rv = _dess_host_hw_to_sw(dev_id, reg, entry);
	if (rv != SW_OK) {
		aos_unlock_bh(&dess_nat_lock);
		return rv;
	}

    if (!(entry->status))
    {
		aos_unlock_bh(&dess_nat_lock);
        return SW_NOT_FOUND;
    }

    HSL_REG_ENTRY_GET(rv, dev_id, HOST_ENTRY7, 0, (a_uint8_t *) (&reg[7]),
                      sizeof (a_uint32_t));
	aos_unlock_bh(&dess_nat_lock);
    SW_RTN_ON_ERROR(rv);

    SW_GET_FIELD_BY_REG(HOST_ENTRY7, TBL_IDX, entry->entry_id, reg[7]);
    return SW_OK;
}

static sw_error_t
_dess_ip_host_next(a_uint32_t dev_id, a_uint32_t next_mode,
                   fal_host_entry_t * entry)
{
    sw_error_t rv;
    a_uint32_t idx, data, reg[DESS_HOST_ENTRY_REG_NUM] = { 0 };

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_ip_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    if (FAL_NEXT_ENTRY_FIRST_ID == entry->entry_id)
    {
        idx = DESS_HOST_ENTRY_NUM - 1;
    }
    else
    {
        if ((DESS_HOST_ENTRY_NUM - 1) == entry->entry_id)
        {
            return SW_NO_MORE;
        }
        else
        {
            idx = entry->entry_id;
        }
    }

    SW_SET_REG_BY_FIELD(HOST_ENTRY7, TBL_IDX, idx, reg[7]);

    if (FAL_IP_ENTRY_INTF_EN & next_mode)
    {
        rv = _dess_ip_intf_sw_to_hw(dev_id, entry/*was:->intf_id*/, &data);
        SW_RTN_ON_ERROR(rv);

        SW_SET_REG_BY_FIELD(HOST_ENTRY7, SPEC_VID, 1, reg[7]);
        SW_SET_REG_BY_FIELD(HOST_ENTRY5, INTF_ID, data, reg[5]);
    }

    if (FAL_IP_ENTRY_PORT_EN & next_mode)
    {
        SW_SET_REG_BY_FIELD(HOST_ENTRY7, SPEC_SP, 1, reg[7]);
        SW_SET_REG_BY_FIELD(HOST_ENTRY5, SRC_PORT, entry->port_id, reg[5]);
    }

    if (FAL_IP_ENTRY_STATUS_EN & next_mode)
    {
        SW_SET_REG_BY_FIELD(HOST_ENTRY7, SPEC_STATUS, 1, reg[7]);
        SW_SET_REG_BY_FIELD(HOST_ENTRY6, AGE_FLAG, entry->status, reg[6]);
    }

    SW_SET_REG_BY_FIELD(HOST_ENTRY6, VRF_ID, entry->vrf_id, reg[6]);
	aos_lock_bh(&dess_nat_lock);
    rv = _dess_host_down_to_hw(dev_id, reg);
	if (rv != SW_OK) {
		aos_unlock_bh(&dess_nat_lock);
		return rv;
	}

    rv = _dess_host_entry_commit(dev_id, DESS_ENTRY_ARP, DESS_HOST_ENTRY_NEXT);
	if (rv != SW_OK) {
		aos_unlock_bh(&dess_nat_lock);
		return rv;
	}

    rv = _dess_host_up_to_sw(dev_id, reg);
	aos_unlock_bh(&dess_nat_lock);
    SW_RTN_ON_ERROR(rv);

    aos_mem_zero(entry, sizeof (fal_host_entry_t));

    rv = _dess_host_hw_to_sw(dev_id, reg, entry);
    SW_RTN_ON_ERROR(rv);

    if (!(entry->status))
    {
        return SW_NO_MORE;
    }

    SW_GET_FIELD_BY_REG(HOST_ENTRY7, TBL_IDX, entry->entry_id, reg[7]);
    return SW_OK;
}

static sw_error_t
_dess_ip_host_counter_bind(a_uint32_t dev_id, a_uint32_t entry_id,
                           a_uint32_t cnt_id, a_bool_t enable)
{
    sw_error_t rv;
    a_uint32_t reg[DESS_HOST_ENTRY_REG_NUM] = { 0 }, tbl[DESS_HOST_ENTRY_REG_NUM] = { 0 }, tbl_idx;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_ip_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    tbl_idx = (entry_id - 1) & 0x7f;
    SW_SET_REG_BY_FIELD(HOST_ENTRY7, TBL_IDX, tbl_idx, reg[7]);

    rv = _dess_host_down_to_hw(dev_id, reg);
    SW_RTN_ON_ERROR(rv);

    rv = _dess_host_entry_commit(dev_id, DESS_ENTRY_ARP, DESS_HOST_ENTRY_NEXT);
    if (SW_OK != rv)
    {
        return SW_NOT_FOUND;
    }

    rv = _dess_host_up_to_sw(dev_id, reg);
    SW_RTN_ON_ERROR(rv);

    SW_GET_FIELD_BY_REG(HOST_ENTRY7, TBL_IDX, tbl_idx, reg[7]);
    if (entry_id != tbl_idx)
    {
        return SW_NOT_FOUND;
    }

    tbl[0] = reg[0];
    tbl[1] = reg[1];
    tbl[2] = reg[2];
    tbl[3] = reg[3];
    tbl[6] = (reg[6] >> 15) << 15;
    rv = _dess_host_down_to_hw(dev_id, tbl);
    SW_RTN_ON_ERROR(rv);

    rv = _dess_host_entry_commit(dev_id, DESS_ENTRY_ARP, DESS_HOST_ENTRY_DEL);
    SW_RTN_ON_ERROR(rv);

    if (A_FALSE == enable)
    {
        SW_SET_REG_BY_FIELD(HOST_ENTRY6, CNT_EN, 0, reg[6]);
    }
    else if (A_TRUE == enable)
    {
        SW_SET_REG_BY_FIELD(HOST_ENTRY6, CNT_EN, 1, reg[6]);
        SW_SET_REG_BY_FIELD(HOST_ENTRY6, CNT_IDX, cnt_id, reg[6]);
    }
    else
    {
        return SW_BAD_PARAM;
    }

    reg[7] = 0x0;
    rv = _dess_host_down_to_hw(dev_id, reg);
    SW_RTN_ON_ERROR(rv);

    rv = _dess_host_entry_commit(dev_id, DESS_ENTRY_ARP, DESS_HOST_ENTRY_ADD);
    return rv;
}

static sw_error_t
_dess_ip_host_pppoe_bind(a_uint32_t dev_id, a_uint32_t entry_id,
                         a_uint32_t pppoe_id, a_bool_t enable)
{
    sw_error_t rv;
    a_uint32_t reg[DESS_HOST_ENTRY_REG_NUM] = { 0 }, tbl[DESS_HOST_ENTRY_REG_NUM] = { 0 }, tbl_idx;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_ip_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    tbl_idx = (entry_id - 1) & 0x7f;
    SW_SET_REG_BY_FIELD(HOST_ENTRY7, TBL_IDX, tbl_idx, reg[7]);
	aos_lock_bh(&dess_nat_lock);
    rv = _dess_host_down_to_hw(dev_id, reg);
	if (rv != SW_OK) {
		aos_unlock_bh(&dess_nat_lock);
		return rv;
	}

    rv = _dess_host_entry_commit(dev_id, DESS_ENTRY_ARP, DESS_HOST_ENTRY_NEXT);
	if (rv != SW_OK) {
		aos_unlock_bh(&dess_nat_lock);
		return rv;
	}

    rv = _dess_host_up_to_sw(dev_id, reg);
	aos_unlock_bh(&dess_nat_lock);
    if (SW_OK != rv)
    {
        return SW_NOT_FOUND;
    }

    SW_GET_FIELD_BY_REG(HOST_ENTRY7, TBL_IDX, tbl_idx, reg[7]);
    if (entry_id != tbl_idx)
    {
        return SW_NOT_FOUND;
    }

    if (A_FALSE == enable)
    {
        SW_SET_REG_BY_FIELD(HOST_ENTRY6, PPPOE_EN, 0, reg[6]);
    }
    else if (A_TRUE == enable)
    {
        SW_SET_REG_BY_FIELD(HOST_ENTRY6, PPPOE_EN, 1, reg[6]);
        SW_SET_REG_BY_FIELD(HOST_ENTRY6, PPPOE_IDX, pppoe_id, reg[6]);
    }
    else
    {
        return SW_BAD_PARAM;
    }

    tbl[0] = reg[0];
    tbl[1] = reg[1];
    tbl[2] = reg[2];
    tbl[3] = reg[3];
    tbl[6] = (reg[6] >> 15) << 15;
	aos_lock_bh(&dess_nat_lock);
    rv = _dess_host_down_to_hw(dev_id, tbl);
	if (rv != SW_OK) {
		aos_unlock_bh(&dess_nat_lock);
		return rv;
	}

    rv = _dess_host_entry_commit(dev_id, DESS_ENTRY_ARP, DESS_HOST_ENTRY_DEL);
	if (rv != SW_OK) {
		aos_unlock_bh(&dess_nat_lock);
		return rv;
	}

    reg[7] = 0x0;
    rv = _dess_host_down_to_hw(dev_id, reg);
	if (rv != SW_OK) {
		aos_unlock_bh(&dess_nat_lock);
		return rv;
	}

    rv = _dess_host_entry_commit(dev_id, DESS_ENTRY_ARP, DESS_HOST_ENTRY_ADD);
	if (rv != SW_OK) {
		aos_unlock_bh(&dess_nat_lock);
		return rv;
	}

    rv = _dess_host_up_to_sw(dev_id, reg);
	aos_unlock_bh(&dess_nat_lock);
    SW_RTN_ON_ERROR(rv);

    return SW_OK;
}

static sw_error_t
_dess_ip_pt_arp_learn_set(a_uint32_t dev_id, fal_port_t port_id,
                          a_uint32_t flags)
{
    sw_error_t rv;
    a_uint32_t data = 0;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_ip_feature_check(dev_id);
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
_dess_ip_pt_arp_learn_get(a_uint32_t dev_id, fal_port_t port_id,
                          a_uint32_t * flags)
{
    sw_error_t rv;
    a_uint32_t data = 0;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_ip_feature_check(dev_id);
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
_dess_ip_arp_learn_set(a_uint32_t dev_id, fal_arp_learn_mode_t mode)
{
    sw_error_t rv;
    a_uint32_t data;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_ip_feature_check(dev_id);
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
_dess_ip_arp_learn_get(a_uint32_t dev_id, fal_arp_learn_mode_t * mode)
{
    sw_error_t rv;
    a_uint32_t data = 0;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_ip_feature_check(dev_id);
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
_dess_ip_source_guard_set(a_uint32_t dev_id, fal_port_t port_id,
                          fal_source_guard_mode_t mode)
{
    sw_error_t rv;
    a_uint32_t reg = 0, data;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_ip_feature_check(dev_id);
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
_dess_ip_source_guard_get(a_uint32_t dev_id, fal_port_t port_id,
                          fal_source_guard_mode_t * mode)
{
    sw_error_t rv;
    a_uint32_t reg = 0, data;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_ip_feature_check(dev_id);
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
_dess_ip_unk_source_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd)
{
    sw_error_t rv;
    a_uint32_t data;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_ip_feature_check(dev_id);
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
_dess_ip_unk_source_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;
    a_uint32_t data = 0;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_ip_feature_check(dev_id);
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
_dess_ip_arp_guard_set(a_uint32_t dev_id, fal_port_t port_id,
                       fal_source_guard_mode_t mode)
{
    sw_error_t rv;
    a_uint32_t reg = 0, data;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_ip_feature_check(dev_id);
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
_dess_ip_arp_guard_get(a_uint32_t dev_id, fal_port_t port_id,
                       fal_source_guard_mode_t * mode)
{
    sw_error_t rv;
    a_uint32_t reg = 0, data;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_ip_feature_check(dev_id);
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
_dess_arp_unk_source_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd)
{
    sw_error_t rv;
    a_uint32_t data;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_ip_feature_check(dev_id);
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
_dess_arp_unk_source_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;
    a_uint32_t data = 0;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_ip_feature_check(dev_id);
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
_dess_ip_route_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;
    a_uint32_t data;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_ip_feature_check(dev_id);
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
_dess_ip_route_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;
    a_uint32_t route_en = 0, l3_en = 0;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_ip_feature_check(dev_id);
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
_dess_ip_intf_entry_add(a_uint32_t dev_id, fal_intf_mac_entry_t * entry)
{
    sw_error_t rv;
    a_uint32_t i, j, found = 0, addr, tbl[3] = { 0 };
    fal_intf_mac_entry_t * intf_entry = NULL;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_ip_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    for (i = 0; i < DESS_INTF_MAC_ADDR_NUM; i++)
    {
        if (dess_mac_snap[dev_id] & (0x1 << i))
        {
            intf_entry = &(dess_intf_snap[dev_id][i]);
            if ((entry->vid_low == intf_entry->vid_low)
                    && (entry->vid_high == intf_entry->vid_high))
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
            }
        }
    }

    if (!found)
    {
        for (i = 0; i < DESS_INTF_MAC_ADDR_NUM; i++)
        {
            if (!(dess_mac_snap[dev_id] & (0x1 << i)))
            {
                intf_entry = &(dess_intf_snap[dev_id][i]);
                break;
            }
        }
    }

    if (DESS_INTF_MAC_ADDR_NUM == i)
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

    SW_SET_REG_BY_FIELD(INTF_ADDR_ENTRY2, VRF_ID, entry->vrf_id, tbl[2]);

    for (j = 0; j < 2; j++)
    {
        addr = DESS_INTF_MAC_EDIT0_ADDR + (i << 4) + (j << 2);
        HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr, sizeof (a_uint32_t),
                              (a_uint8_t *) (&(tbl[j])), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }

    for (j = 0; j < 3; j++)
    {
        addr = DESS_INTF_MAC_TBL0_ADDR + (i << 4) + (j << 2);
        HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr, sizeof (a_uint32_t),
                              (a_uint8_t *) (&(tbl[j])), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }

    dess_mac_snap[dev_id] |= (0x1 << i);
    *intf_entry = *entry;
    entry->entry_id = i;
    return SW_OK;
}

static sw_error_t
_dess_ip_intf_entry_del(a_uint32_t dev_id, a_uint32_t del_mode,
                        fal_intf_mac_entry_t * entry)
{
    sw_error_t rv;
    a_uint32_t addr, tbl[3] = { 0 };

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_ip_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    if (!(FAL_IP_ENTRY_ID_EN & del_mode))
    {
        return SW_NOT_SUPPORTED;
    }

    if (DESS_INTF_MAC_ADDR_NUM <= entry->entry_id)
    {
        return SW_BAD_PARAM;
    }

    /* clear valid bits */
    addr = DESS_INTF_MAC_TBL2_ADDR + (entry->entry_id << 4);
    HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr, sizeof (a_uint32_t),
                          (a_uint8_t *) (&(tbl[2])), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    dess_mac_snap[dev_id] &= (~(0x1 << entry->entry_id));
    return SW_OK;
}

static sw_error_t
_dess_ip_intf_entry_next(a_uint32_t dev_id, a_uint32_t next_mode,
                         fal_intf_mac_entry_t * entry)
{
    sw_error_t rv;
    a_uint32_t i, j, idx, addr, tbl[3] = { 0 };

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_ip_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    if (FAL_NEXT_ENTRY_FIRST_ID == entry->entry_id)
    {
        idx = 0;
    }
    else
    {
        if ((DESS_INTF_MAC_ADDR_NUM - 1) == entry->entry_id)
        {
            return SW_NO_MORE;
        }
        else
        {
            idx = entry->entry_id + 1;
        }
    }

    for (i = idx; i < DESS_INTF_MAC_ADDR_NUM; i++)
    {
        if (dess_mac_snap[dev_id] & (0x1 << i))
        {
            break;
        }
    }

    if (DESS_INTF_MAC_ADDR_NUM == i)
    {
        return SW_NO_MORE;
    }

    for (j = 0; j < 3; j++)
    {
        addr = DESS_INTF_MAC_TBL0_ADDR + (i << 4) + (j << 2);
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
    SW_GET_FIELD_BY_REG(INTF_ADDR_ENTRY2, VRF_ID, entry->vrf_id, tbl[2]);

    entry->entry_id = i;
    return SW_OK;
}

#define DESS_WCMP_ENTRY_MAX_ID    3
#define DESS_WCMP_HASH_MAX_NUM    16
#define DESS_IP_ENTRY_MAX_ID      127

#define DESS_WCMP_HASH_TBL_ADDR   0x0e10
#define DESS_WCMP_NHOP_TBL_ADDR   0x0e20

static sw_error_t
_dess_ip_wcmp_entry_set(a_uint32_t dev_id, a_uint32_t wcmp_id, fal_ip_wcmp_t * wcmp)
{
    sw_error_t rv;
    a_uint32_t i, j, addr, data;
    a_uint8_t  idx, ptr[4] = { 0 }, pos[16] = { 0 };

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_ip_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    if (DESS_WCMP_ENTRY_MAX_ID < wcmp_id)
    {
        return SW_BAD_PARAM;
    }

    if (DESS_WCMP_HASH_MAX_NUM < wcmp->nh_nr)
    {
        return SW_BAD_PARAM;
    }

    for (i = 0; i < wcmp->nh_nr; i++)
    {
        if (DESS_IP_ENTRY_MAX_ID < wcmp->nh_id[i])
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

    addr = DESS_WCMP_NHOP_TBL_ADDR + (wcmp_id << 2);
    HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr, sizeof (a_uint32_t),
                          (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    data = 0;
    for (j = 0; j < 16; j++)
    {
        data |= (pos[j] << (j << 1));
    }

    addr = DESS_WCMP_HASH_TBL_ADDR + (wcmp_id << 2);
    HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr, sizeof (a_uint32_t),
                          (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    return SW_OK;
}

static sw_error_t
_dess_ip_wcmp_entry_get(a_uint32_t dev_id, a_uint32_t wcmp_id, fal_ip_wcmp_t * wcmp)
{
    sw_error_t rv;
    a_uint32_t i, addr, data = 0;
    a_uint8_t  ptr[4] = { 0 }, pos[16] = { 0 };

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_ip_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    if (DESS_WCMP_ENTRY_MAX_ID < wcmp_id)
    {
        return SW_BAD_PARAM;
    }

    wcmp->nh_nr = DESS_WCMP_HASH_MAX_NUM;

    addr = DESS_WCMP_NHOP_TBL_ADDR + (wcmp_id << 2);
    HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr, sizeof (a_uint32_t),
                          (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    for (i = 0; i < 4; i++)
    {
        ptr[i] = (data >> (i << 3)) & 0x7f;
    }

    addr = DESS_WCMP_HASH_TBL_ADDR + (wcmp_id << 2);
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

static sw_error_t
_dess_ip_wcmp_hash_mode_set(a_uint32_t dev_id, a_uint32_t hash_mode)
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
_dess_ip_wcmp_hash_mode_get(a_uint32_t dev_id, a_uint32_t * hash_mode)
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

#define DESS_VRF_ENTRY_MAX_ID    7

#define DESS_VRF_ENTRY_TBL_ADDR   0x0484
#define DESS_VRF_ENTRY_MASK_ADDR   0x0488

static sw_error_t
_dess_ip_vrf_base_addr_set(a_uint32_t dev_id, a_uint32_t vrf_id, fal_ip4_addr_t addr)
{
    sw_error_t rv;
    a_uint32_t reg_addr;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_ip_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    if (DESS_VRF_ENTRY_MAX_ID < vrf_id)
    {
        return SW_BAD_PARAM;
    }

    reg_addr = DESS_VRF_ENTRY_TBL_ADDR + (vrf_id << 3);
    HSL_REG_ENTRY_GEN_SET(rv, dev_id, reg_addr, sizeof (a_uint32_t),
                          (a_uint8_t *) (&addr), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    return rv;
}

static sw_error_t
_dess_ip_vrf_base_addr_get(a_uint32_t dev_id, a_uint32_t vrf_id, fal_ip4_addr_t * addr)
{
    sw_error_t rv;
    a_uint32_t reg_addr;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_ip_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    if (DESS_VRF_ENTRY_MAX_ID < vrf_id)
    {
        return SW_BAD_PARAM;
    }

    reg_addr = DESS_VRF_ENTRY_TBL_ADDR + (vrf_id << 3);
    HSL_REG_ENTRY_GEN_GET(rv, dev_id, reg_addr, sizeof (a_uint32_t),
                          (a_uint8_t *) (addr), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    return SW_OK;
}

static sw_error_t
_dess_ip_vrf_base_mask_set(a_uint32_t dev_id, a_uint32_t vrf_id, fal_ip4_addr_t addr)
{
    sw_error_t rv;
    a_uint32_t reg_addr;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_ip_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    if (DESS_VRF_ENTRY_MAX_ID < vrf_id)
    {
        return SW_BAD_PARAM;
    }

    reg_addr = DESS_VRF_ENTRY_MASK_ADDR + (vrf_id << 3);
    HSL_REG_ENTRY_GEN_SET(rv, dev_id, reg_addr, sizeof (a_uint32_t),
                          (a_uint8_t *) (&addr), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    return rv;
}

static sw_error_t
_dess_ip_vrf_base_mask_get(a_uint32_t dev_id, a_uint32_t vrf_id, fal_ip4_addr_t * addr)
{
    sw_error_t rv;
    a_uint32_t reg_addr;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_ip_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    if (DESS_VRF_ENTRY_MAX_ID < vrf_id)
    {
        return SW_BAD_PARAM;
    }

    reg_addr = DESS_VRF_ENTRY_MASK_ADDR + (vrf_id << 3);
    HSL_REG_ENTRY_GEN_GET(rv, dev_id, reg_addr, sizeof (a_uint32_t),
                          (a_uint8_t *) (addr), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    return SW_OK;
}

static sw_error_t
_dess_ip_default_route_set(a_uint32_t dev_id, a_uint32_t droute_id, fal_default_route_t * entry)
{
    sw_error_t rv;
    a_uint32_t data = 0;
    a_uint32_t addr;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_ip_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    if (entry->ip_version == FAL_ADDR_IPV4)
    {
    	SW_SET_REG_BY_FIELD(IP4_DEFAULT_ROUTE_ENTRY, VALID, entry->valid, data);
    	SW_SET_REG_BY_FIELD(IP4_DEFAULT_ROUTE_ENTRY, VRF, entry->vrf_id, data);
    	SW_SET_REG_BY_FIELD(IP4_DEFAULT_ROUTE_ENTRY, ARP_WCMP_TYPE, entry->droute_type, data);
    	SW_SET_REG_BY_FIELD(IP4_DEFAULT_ROUTE_ENTRY, ARP_WCMP_INDEX, entry->index, data);

    	addr = DESS_IP4_DEFAULT_ROUTE_TBL_ADDR + (droute_id << 2);
    	HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr, sizeof (a_uint32_t),
                              (a_uint8_t *) (&data), sizeof (a_uint32_t));
    }
    else
    {
    	SW_SET_REG_BY_FIELD(IP6_DEFAULT_ROUTE_ENTRY, VALID, entry->valid, data);
    	SW_SET_REG_BY_FIELD(IP6_DEFAULT_ROUTE_ENTRY, VRF, entry->vrf_id, data);
    	SW_SET_REG_BY_FIELD(IP6_DEFAULT_ROUTE_ENTRY, ARP_WCMP_TYPE, entry->droute_type, data);
    	SW_SET_REG_BY_FIELD(IP6_DEFAULT_ROUTE_ENTRY, ARP_WCMP_INDEX, entry->index, data);

    	addr = DESS_IP6_DEFAULT_ROUTE_TBL_ADDR + (droute_id << 2);
    	HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr, sizeof (a_uint32_t),
                              (a_uint8_t *) (&data), sizeof (a_uint32_t));
    }

    return rv;
}

static sw_error_t
_dess_ip_default_route_get(a_uint32_t dev_id, a_uint32_t droute_id, fal_default_route_t * entry)
{
    sw_error_t rv;
    a_uint32_t data = 0;
    a_uint32_t addr;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_ip_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    if (entry->ip_version == FAL_ADDR_IPV4)
    {
    	addr = DESS_IP4_DEFAULT_ROUTE_TBL_ADDR + (droute_id << 2);
    	HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr, sizeof (a_uint32_t),
                              (a_uint8_t *) (&data), sizeof (a_uint32_t));

    	SW_GET_FIELD_BY_REG(IP4_DEFAULT_ROUTE_ENTRY, VALID, entry->valid, data);
    	SW_GET_FIELD_BY_REG(IP4_DEFAULT_ROUTE_ENTRY, VRF, entry->vrf_id, data);
    	SW_GET_FIELD_BY_REG(IP4_DEFAULT_ROUTE_ENTRY, ARP_WCMP_TYPE, entry->droute_type, data);
    	SW_GET_FIELD_BY_REG(IP4_DEFAULT_ROUTE_ENTRY, ARP_WCMP_INDEX, entry->index, data);
    }
    else
    {
    	addr = DESS_IP6_DEFAULT_ROUTE_TBL_ADDR + (droute_id << 2);
    	HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr, sizeof (a_uint32_t),
                              (a_uint8_t *) (&data), sizeof (a_uint32_t));

    	SW_GET_FIELD_BY_REG(IP6_DEFAULT_ROUTE_ENTRY, VALID, entry->valid, data);
    	SW_GET_FIELD_BY_REG(IP6_DEFAULT_ROUTE_ENTRY, VRF, entry->vrf_id, data);
    	SW_GET_FIELD_BY_REG(IP6_DEFAULT_ROUTE_ENTRY, ARP_WCMP_TYPE, entry->droute_type, data);
    	SW_GET_FIELD_BY_REG(IP6_DEFAULT_ROUTE_ENTRY, ARP_WCMP_INDEX, entry->index, data);
    }

    return SW_OK;
}

static sw_error_t
_dess_ip_host_route_set(a_uint32_t dev_id, a_uint32_t hroute_id, fal_host_route_t * entry)
{
    sw_error_t rv;
    a_uint32_t j, addr, tbl[5] = { 0 };
    a_uint32_t addr0_low, addr0_high, addr1_low, addr1_high,
		     addr2_low, addr2_high, addr3_low, addr3_high;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_ip_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    if (entry->ip_version == FAL_ADDR_IPV4)
    {
	addr0_low = entry->route_addr.ip4_addr & 0x7ffffff;
	addr0_high = entry->route_addr.ip4_addr >> 27;
	SW_SET_REG_BY_FIELD(IP4_HOST_ROUTE_ENTRY0, PREFIX_LENGTH, entry->prefix_length,
				tbl[0]);
	SW_SET_REG_BY_FIELD(IP4_HOST_ROUTE_ENTRY0, IP4_ADDRL, addr0_low, tbl[0]);
	SW_SET_REG_BY_FIELD(IP4_HOST_ROUTE_ENTRY1, IP4_ADDRH, addr0_high, tbl[1]);
	SW_SET_REG_BY_FIELD(IP4_HOST_ROUTE_ENTRY1, VALID, entry->valid,
				tbl[1]);
	SW_SET_REG_BY_FIELD(IP4_HOST_ROUTE_ENTRY1, VRF, entry->vrf_id,
				tbl[1]);
    	for (j = 0; j < 2; j++)
    	{
        	    addr = DESS_IP4_HOST_ROUTE_TBL0_ADDR + (hroute_id << 4) + (j << 2);
        	    HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr, sizeof (a_uint32_t),
                              (a_uint8_t *) (&(tbl[j])), sizeof (a_uint32_t));
        	    SW_RTN_ON_ERROR(rv);
    	}
    }
    else
    {
        addr0_low = entry->route_addr.ip6_addr.ul[3] & 0x1ffffff;
        addr0_high = (entry->route_addr.ip6_addr.ul[3] >> 25) & 0x7f;
        addr1_low = entry->route_addr.ip6_addr.ul[2] & 0x1ffffff;
        addr1_high = (entry->route_addr.ip6_addr.ul[2] >> 25) & 0x7f;
        addr2_low = entry->route_addr.ip6_addr.ul[1] & 0x1ffffff;
        addr2_high = (entry->route_addr.ip6_addr.ul[1] >> 25) & 0x7f;
        addr3_low = entry->route_addr.ip6_addr.ul[0] & 0x1ffffff;
        addr3_high = (entry->route_addr.ip6_addr.ul[0] >> 25) & 0x7f;

        SW_SET_REG_BY_FIELD(IP6_HOST_ROUTE_ENTRY0, IP6_ADDR0L, addr0_low, tbl[0]);
        SW_SET_REG_BY_FIELD(IP6_HOST_ROUTE_ENTRY0, PREFIX_LENGTH, entry->prefix_length, tbl[0]);

        SW_SET_REG_BY_FIELD(IP6_HOST_ROUTE_ENTRY1, IP6_ADDR1L, addr1_low, tbl[1]);
        SW_SET_REG_BY_FIELD(IP6_HOST_ROUTE_ENTRY1, IP6_ADDR0H, addr0_high, tbl[1]);

        SW_SET_REG_BY_FIELD(IP6_HOST_ROUTE_ENTRY2, IP6_ADDR2L, addr2_low, tbl[2]);
        SW_SET_REG_BY_FIELD(IP6_HOST_ROUTE_ENTRY2, IP6_ADDR1H, addr1_high, tbl[2]);

        SW_SET_REG_BY_FIELD(IP6_HOST_ROUTE_ENTRY3, IP6_ADDR3L, addr3_low, tbl[3]);
        SW_SET_REG_BY_FIELD(IP6_HOST_ROUTE_ENTRY3, IP6_ADDR2H, addr2_high, tbl[3]);

        SW_SET_REG_BY_FIELD(IP6_HOST_ROUTE_ENTRY4, VALID, entry->valid, tbl[4]);
        SW_SET_REG_BY_FIELD(IP6_HOST_ROUTE_ENTRY4, VRF, entry->vrf_id, tbl[4]);
        SW_SET_REG_BY_FIELD(IP6_HOST_ROUTE_ENTRY4, IP6_ADDR3H, addr3_high, tbl[4]);

    	for (j = 0; j < 5; j++)
    	{
        	    addr = DESS_IP6_HOST_ROUTE_TBL0_ADDR + (hroute_id << 5) + (j << 2);
        	    HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr, sizeof (a_uint32_t),
                              (a_uint8_t *) (&(tbl[j])), sizeof (a_uint32_t));
        	    SW_RTN_ON_ERROR(rv);
    	}    
    }

    return SW_OK;
}

static sw_error_t
_dess_ip_host_route_get(a_uint32_t dev_id, a_uint32_t hroute_id, fal_host_route_t * entry)
{
    sw_error_t rv;
    a_uint32_t j, addr, tbl[5] = { 0 };
    a_uint32_t addr0_low, addr0_high, addr1_low, addr1_high,
		     addr2_low, addr2_high, addr3_low, addr3_high;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_ip_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    if (entry->ip_version == FAL_ADDR_IPV4)
    {
    	for (j = 0; j < 2; j++)
    	{
        	    addr = DESS_IP4_HOST_ROUTE_TBL0_ADDR + (hroute_id << 4) + (j << 2);
        	    HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr, sizeof (a_uint32_t),
                              (a_uint8_t *) (&(tbl[j])), sizeof (a_uint32_t));
        	    SW_RTN_ON_ERROR(rv);
    	}
	SW_GET_FIELD_BY_REG(IP4_HOST_ROUTE_ENTRY0, PREFIX_LENGTH, entry->prefix_length,
				tbl[0]);
	SW_GET_FIELD_BY_REG(IP4_HOST_ROUTE_ENTRY0, IP4_ADDRL, addr0_low,
				tbl[0]);
	SW_GET_FIELD_BY_REG(IP4_HOST_ROUTE_ENTRY1, IP4_ADDRH, addr0_high,
				tbl[1]);
	SW_GET_FIELD_BY_REG(IP4_HOST_ROUTE_ENTRY1, VALID, entry->valid,
				tbl[1]);
	SW_GET_FIELD_BY_REG(IP4_HOST_ROUTE_ENTRY1, VRF, entry->vrf_id,
				tbl[1]);
	entry->route_addr.ip4_addr = addr0_low | (addr0_high << 27);
    }
    else
    {
    	for (j = 0; j < 5; j++)
    	{
        	    addr = DESS_IP6_HOST_ROUTE_TBL0_ADDR + (hroute_id << 5) + (j << 2);
        	    HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr, sizeof (a_uint32_t),
                              (a_uint8_t *) (&(tbl[j])), sizeof (a_uint32_t));
        	    SW_RTN_ON_ERROR(rv);
    	}

        SW_GET_FIELD_BY_REG(IP6_HOST_ROUTE_ENTRY0, IP6_ADDR0L, addr0_low, tbl[0]);
        SW_GET_FIELD_BY_REG(IP6_HOST_ROUTE_ENTRY0, PREFIX_LENGTH, entry->prefix_length, tbl[0]);
        SW_GET_FIELD_BY_REG(IP6_HOST_ROUTE_ENTRY1, IP6_ADDR1L, addr1_low, tbl[1]);
        SW_GET_FIELD_BY_REG(IP6_HOST_ROUTE_ENTRY1, IP6_ADDR0H, addr0_high, tbl[1]);
        SW_GET_FIELD_BY_REG(IP6_HOST_ROUTE_ENTRY2, IP6_ADDR2L, addr2_low, tbl[2]);
        SW_GET_FIELD_BY_REG(IP6_HOST_ROUTE_ENTRY2, IP6_ADDR1H, addr1_high, tbl[2]);
        SW_GET_FIELD_BY_REG(IP6_HOST_ROUTE_ENTRY3, IP6_ADDR3L, addr3_low, tbl[3]);
        SW_GET_FIELD_BY_REG(IP6_HOST_ROUTE_ENTRY3, IP6_ADDR2H, addr2_high, tbl[3]);
        SW_GET_FIELD_BY_REG(IP6_HOST_ROUTE_ENTRY4, VRF, entry->vrf_id, tbl[4]);
        SW_GET_FIELD_BY_REG(IP6_HOST_ROUTE_ENTRY4, IP6_ADDR3H, addr3_high, tbl[4]);
        SW_GET_FIELD_BY_REG(IP6_HOST_ROUTE_ENTRY4, VALID, entry->valid, tbl[4]);
        entry->route_addr.ip6_addr.ul[3] = (addr0_high << 25) | addr0_low;
        entry->route_addr.ip6_addr.ul[2] = (addr1_high << 25) | addr1_low;
        entry->route_addr.ip6_addr.ul[1] = (addr2_high << 25) | addr2_low;
        entry->route_addr.ip6_addr.ul[0] = (addr3_high << 25) | addr3_low;
    }

    return SW_OK;
}

#define RFS_ADD_OP  1
#define RFS_DEL_OP  2
static sw_error_t
_dess_ip_rfs_ip4_update(
		a_uint32_t dev_id,
		fal_host_entry_t *entry,
		fal_ip4_rfs_t * rfs,
		char op)
{
	fal_host_entry_t tmp = *entry;

	_dess_ip_host_del(dev_id, FAL_IP_ENTRY_IPADDR_EN, entry);
	if(RFS_ADD_OP == op) {
		tmp.lb_num = rfs->load_balance | 0x4;
		tmp.status = 0x7;
	}
	else {
		tmp.lb_num = 0;
		tmp.status = 0x6;
	}
	return _dess_ip_host_add(dev_id, &tmp);
}

static sw_error_t
_dess_ip_rfs_ip6_update(
		a_uint32_t dev_id,
		fal_host_entry_t *entry,
		fal_ip6_rfs_t * rfs,
		char op)
{
	fal_host_entry_t tmp = *entry;

	_dess_ip_host_del(dev_id, FAL_IP_ENTRY_IPADDR_EN, entry);
	if(RFS_ADD_OP == op) {
		tmp.lb_num = rfs->load_balance | 0x4;
		tmp.status = 0x7;
	}
	else {
		tmp.lb_num = 0;
		tmp.status = 0x6;
	}
	return _dess_ip_host_add(dev_id, &tmp);
}

static sw_error_t
_dess_ip_rfs_ip4_set(a_uint32_t dev_id, fal_ip4_rfs_t * rfs)
{
	fal_host_entry_t entry;

	memset(&entry, 0, sizeof(entry));
	entry.flags = FAL_IP_IP4_ADDR;
	entry.ip4_addr = rfs->ip4_addr;
	if(SW_OK == _dess_ip_host_get(dev_id, 0x10, &entry)) {
		return _dess_ip_rfs_ip4_update(dev_id, &entry, rfs, RFS_ADD_OP);
	}
	/*add a new one*/
	entry.status = 0x7;
	entry.flags = FAL_IP_IP4_ADDR;
	entry.ip4_addr = rfs->ip4_addr;
	memcpy(&entry.mac_addr, &rfs->mac_addr, 6);
	entry.intf_id = rfs->vid;
	entry.port_id = 0;
	entry.lb_num = rfs->load_balance | 0x4;
	entry.action = FAL_MAC_RDT_TO_CPU;
	return _dess_ip_host_add(dev_id, &entry);
}

static sw_error_t
_dess_ip_rfs_ip6_set(a_uint32_t dev_id, fal_ip6_rfs_t * rfs)
{
	fal_host_entry_t entry;

	memset(&entry, 0, sizeof(entry));
	entry.flags = FAL_IP_IP6_ADDR;
	entry.ip6_addr = rfs->ip6_addr;
	if(SW_OK == _dess_ip_host_get(dev_id, 0x10, &entry)) {
		return _dess_ip_rfs_ip6_update(dev_id, &entry, rfs, RFS_ADD_OP);
	}
	/*add a new one*/
	entry.status = 0x7;
	entry.flags = FAL_IP_IP6_ADDR;
	entry.ip6_addr = rfs->ip6_addr;
	memcpy(&entry.mac_addr, &rfs->mac_addr, 6);
	entry.intf_id = rfs->vid;
	entry.port_id = 0;
	entry.lb_num = rfs->load_balance | 0x4;
	entry.action = FAL_MAC_RDT_TO_CPU;
	return _dess_ip_host_add(dev_id, &entry);
}

static sw_error_t
_dess_ip_rfs_ip4_del(a_uint32_t dev_id, fal_ip4_rfs_t * rfs)
{
	fal_host_entry_t entry;
	sw_error_t ret;

	memset(&entry, 0, sizeof(entry));
	entry.flags = FAL_IP_IP4_ADDR;
	entry.ip4_addr = rfs->ip4_addr;
	if(SW_OK == (ret = _dess_ip_host_get(dev_id, 0x10, &entry))) {
		return _dess_ip_rfs_ip4_update(dev_id, &entry, rfs, RFS_DEL_OP);
	}
	return ret;
}

static sw_error_t
_dess_ip_rfs_ip6_del(a_uint32_t dev_id, fal_ip6_rfs_t * rfs)
{
	fal_host_entry_t entry;
	sw_error_t ret;

	memset(&entry, 0, sizeof(entry));
	entry.flags = FAL_IP_IP6_ADDR;
	entry.ip6_addr = rfs->ip6_addr;
	if(SW_OK == (ret = _dess_ip_host_get(dev_id, 0x10, &entry))) {
		return _dess_ip_rfs_ip6_update(dev_id, &entry, rfs, RFS_DEL_OP);
	}
	return ret;
}

static sw_error_t
_dess_default_flow_cmd_set(a_uint32_t dev_id, a_uint32_t vrf_id, fal_flow_type_t type, fal_default_flow_cmd_t cmd)
{
    sw_error_t rv;
    a_uint32_t data;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_ip_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    if (FAL_DEFAULT_FLOW_FORWARD == cmd)
    {
        data = 0;
    }
    else if (FAL_DEFAULT_FLOW_DROP == cmd)
    {
        data = 1;
    }
    else if (FAL_DEFAULT_FLOW_RDT_TO_CPU == cmd)
    {
        data = 2;
    }
    else if (FAL_DEFAULT_FLOW_ADMIT_ALL == cmd)
    {
        data = 3;
    }
    else
    {
        return SW_NOT_SUPPORTED;
    }

    if (FAL_FLOW_LAN_TO_LAN == type)
    {
        HSL_REG_FIELD_SET(rv, dev_id, FlOW_CMD_CTL, vrf_id, LAN_2_LAN_DEFAULT,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    }
    else if (FAL_FLOW_WAN_TO_LAN == type)
    {
        HSL_REG_FIELD_SET(rv, dev_id, FlOW_CMD_CTL, vrf_id, WAN_2_LAN_DEFAULT,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    }
    else if (FAL_FLOW_LAN_TO_WAN == type)
    {
        HSL_REG_FIELD_SET(rv, dev_id, FlOW_CMD_CTL, vrf_id, LAN_2_WAN_DEFAULT,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    }
    else if (FAL_FLOW_WAN_TO_WAN == type)
    {
        HSL_REG_FIELD_SET(rv, dev_id, FlOW_CMD_CTL, vrf_id, WAN_2_WAN_DEFAULT,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    }
    else
    {
        return SW_NOT_SUPPORTED;
    }

    return rv;
}

static sw_error_t
_dess_default_flow_cmd_get(a_uint32_t dev_id, a_uint32_t vrf_id, fal_flow_type_t type, fal_default_flow_cmd_t * cmd)
{
    sw_error_t rv;
    a_uint32_t data = 0;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_ip_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    if (FAL_FLOW_LAN_TO_LAN == type)
    {
        HSL_REG_FIELD_GET(rv, dev_id, FlOW_CMD_CTL, vrf_id, LAN_2_LAN_DEFAULT,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    }
    else if (FAL_FLOW_WAN_TO_LAN == type)
    {
        HSL_REG_FIELD_GET(rv, dev_id, FlOW_CMD_CTL, vrf_id, WAN_2_LAN_DEFAULT,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    }
    else if (FAL_FLOW_LAN_TO_WAN == type)
    {
        HSL_REG_FIELD_GET(rv, dev_id, FlOW_CMD_CTL, vrf_id, LAN_2_WAN_DEFAULT,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    }
    else if (FAL_FLOW_WAN_TO_WAN == type)
    {
        HSL_REG_FIELD_GET(rv, dev_id, FlOW_CMD_CTL, vrf_id, WAN_2_WAN_DEFAULT,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    }
    else
    {
        return SW_NOT_SUPPORTED;
    }
    SW_RTN_ON_ERROR(rv);

    if (0 == data)
    {
        *cmd = FAL_DEFAULT_FLOW_FORWARD;
    }
    else if (1 == data)
    {
        *cmd = FAL_DEFAULT_FLOW_DROP;
    }
    else if (2 == data)
    {
        *cmd = FAL_DEFAULT_FLOW_RDT_TO_CPU;
    }
    else if (3 == data)
    {
        *cmd = FAL_DEFAULT_FLOW_ADMIT_ALL;
    }

    return SW_OK;
}

static sw_error_t
_dess_default_rt_flow_cmd_set(a_uint32_t dev_id, a_uint32_t vrf_id, fal_flow_type_t type, fal_default_flow_cmd_t cmd)
{
    sw_error_t rv;
    a_uint32_t data;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_ip_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    if (FAL_DEFAULT_FLOW_FORWARD == cmd)
    {
        data = 0;
    }
    else if (FAL_DEFAULT_FLOW_DROP == cmd)
    {
        data = 1;
    }
    else if (FAL_DEFAULT_FLOW_RDT_TO_CPU == cmd)
    {
        data = 2;
    }
    else if (FAL_DEFAULT_FLOW_ADMIT_ALL == cmd)
    {
        data = 3;
    }
    else
    {
        return SW_NOT_SUPPORTED;
    }

    if (FAL_FLOW_LAN_TO_LAN == type)
    {
        HSL_REG_FIELD_SET(rv, dev_id, FlOW_RT_CMD_CTL, vrf_id, LAN_2_LAN_DEFAULT,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    }
    else if (FAL_FLOW_WAN_TO_LAN == type)
    {
        HSL_REG_FIELD_SET(rv, dev_id, FlOW_RT_CMD_CTL, vrf_id, WAN_2_LAN_DEFAULT,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    }
    else if (FAL_FLOW_LAN_TO_WAN == type)
    {
        HSL_REG_FIELD_SET(rv, dev_id, FlOW_RT_CMD_CTL, vrf_id, LAN_2_WAN_DEFAULT,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    }
    else if (FAL_FLOW_WAN_TO_WAN == type)
    {
        HSL_REG_FIELD_SET(rv, dev_id, FlOW_RT_CMD_CTL, vrf_id, WAN_2_WAN_DEFAULT,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    }
    else
    {
        return SW_NOT_SUPPORTED;
    }

    return rv;
}

static sw_error_t
_dess_default_rt_flow_cmd_get(a_uint32_t dev_id, a_uint32_t vrf_id, fal_flow_type_t type, fal_default_flow_cmd_t * cmd)
{
    sw_error_t rv;
    a_uint32_t data = 0;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_ip_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    if (FAL_FLOW_LAN_TO_LAN == type)
    {
        HSL_REG_FIELD_GET(rv, dev_id, FlOW_RT_CMD_CTL, vrf_id, LAN_2_LAN_DEFAULT,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    }
    else if (FAL_FLOW_WAN_TO_LAN == type)
    {
        HSL_REG_FIELD_GET(rv, dev_id, FlOW_RT_CMD_CTL, vrf_id, WAN_2_LAN_DEFAULT,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    }
    else if (FAL_FLOW_LAN_TO_WAN == type)
    {
        HSL_REG_FIELD_GET(rv, dev_id, FlOW_RT_CMD_CTL, vrf_id, LAN_2_WAN_DEFAULT,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    }
    else if (FAL_FLOW_WAN_TO_WAN == type)
    {
        HSL_REG_FIELD_GET(rv, dev_id, FlOW_RT_CMD_CTL, vrf_id, WAN_2_WAN_DEFAULT,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    }
    else
    {
        return SW_NOT_SUPPORTED;
    }
    SW_RTN_ON_ERROR(rv);

    if (0 == data)
    {
        *cmd = FAL_DEFAULT_FLOW_FORWARD;
    }
    else if (1 == data)
    {
        *cmd = FAL_DEFAULT_FLOW_DROP;
    }
    else if (2 == data)
    {
        *cmd = FAL_DEFAULT_FLOW_RDT_TO_CPU;
    }
    else if (3 == data)
    {
        *cmd = FAL_DEFAULT_FLOW_ADMIT_ALL;
    }

    return SW_OK;
}

static sw_error_t
_dess_ip_glb_lock_time_set(a_uint32_t dev_id, fal_glb_lock_time_t lock_time)
{
	sw_error_t rv;
	a_uint32_t data = lock_time;

	HSL_REG_FIELD_SET(rv, dev_id, ROUTER_CTRL, 0, GLB_LOCKTIME,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
	return rv;
}

sw_error_t
dess_ip_reset(a_uint32_t dev_id)
{
    sw_error_t rv;
    a_uint32_t i, addr, data = 0;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _dess_ip_feature_check(dev_id);
    SW_RTN_ON_ERROR(rv);

    HSL_REG_ENTRY_SET(rv, dev_id, HOST_ENTRY7, 0, (a_uint8_t *) (&data),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    rv = _dess_host_entry_commit(dev_id, DESS_ENTRY_ARP, DESS_HOST_ENTRY_FLUSH);
    SW_RTN_ON_ERROR(rv);

    dess_mac_snap[dev_id] = 0;
    for (i = 0; i < DESS_INTF_MAC_ADDR_NUM; i++)
    {
        addr = DESS_INTF_MAC_TBL2_ADDR + (i << 4);
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
dess_ip_host_add(a_uint32_t dev_id, fal_host_entry_t * host_entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_ip_host_add(dev_id, host_entry);
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
dess_ip_host_del(a_uint32_t dev_id, a_uint32_t del_mode,
                 fal_host_entry_t * host_entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_ip_host_del(dev_id, del_mode, host_entry);
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
dess_ip_host_get(a_uint32_t dev_id, a_uint32_t get_mode,
                 fal_host_entry_t * host_entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_ip_host_get(dev_id, get_mode, host_entry);
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
dess_ip_host_next(a_uint32_t dev_id, a_uint32_t next_mode,
                  fal_host_entry_t * host_entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_ip_host_next(dev_id, next_mode, host_entry);
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
dess_ip_host_counter_bind(a_uint32_t dev_id, a_uint32_t entry_id,
                          a_uint32_t cnt_id, a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_ip_host_counter_bind(dev_id, entry_id, cnt_id, enable);
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
dess_ip_host_pppoe_bind(a_uint32_t dev_id, a_uint32_t entry_id,
                        a_uint32_t pppoe_id, a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_ip_host_pppoe_bind(dev_id, entry_id, pppoe_id, enable);
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
dess_ip_pt_arp_learn_set(a_uint32_t dev_id, fal_port_t port_id,
                         a_uint32_t flags)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_ip_pt_arp_learn_set(dev_id, port_id, flags);
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
dess_ip_pt_arp_learn_get(a_uint32_t dev_id, fal_port_t port_id,
                         a_uint32_t * flags)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_ip_pt_arp_learn_get(dev_id, port_id, flags);
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
dess_ip_arp_learn_set(a_uint32_t dev_id, fal_arp_learn_mode_t mode)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_ip_arp_learn_set(dev_id, mode);
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
dess_ip_arp_learn_get(a_uint32_t dev_id, fal_arp_learn_mode_t * mode)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_ip_arp_learn_get(dev_id, mode);
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
dess_ip_source_guard_set(a_uint32_t dev_id, fal_port_t port_id,
                         fal_source_guard_mode_t mode)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_ip_source_guard_set(dev_id, port_id, mode);
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
dess_ip_source_guard_get(a_uint32_t dev_id, fal_port_t port_id,
                         fal_source_guard_mode_t * mode)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_ip_source_guard_get(dev_id, port_id, mode);
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
dess_ip_unk_source_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_ip_unk_source_cmd_set(dev_id, cmd);
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
dess_ip_unk_source_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_ip_unk_source_cmd_get(dev_id, cmd);
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
dess_ip_arp_guard_set(a_uint32_t dev_id, fal_port_t port_id,
                      fal_source_guard_mode_t mode)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_ip_arp_guard_set(dev_id, port_id, mode);
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
dess_ip_arp_guard_get(a_uint32_t dev_id, fal_port_t port_id,
                      fal_source_guard_mode_t * mode)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_ip_arp_guard_get(dev_id, port_id, mode);
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
dess_arp_unk_source_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_arp_unk_source_cmd_set(dev_id, cmd);
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
dess_arp_unk_source_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_arp_unk_source_cmd_get(dev_id, cmd);
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
dess_ip_route_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_ip_route_status_set(dev_id, enable);
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
dess_ip_route_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_ip_route_status_get(dev_id, enable);
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
dess_ip_intf_entry_add(a_uint32_t dev_id, fal_intf_mac_entry_t * entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_ip_intf_entry_add(dev_id, entry);
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
dess_ip_intf_entry_del(a_uint32_t dev_id, a_uint32_t del_mode,
                       fal_intf_mac_entry_t * entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_ip_intf_entry_del(dev_id, del_mode, entry);
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
dess_ip_intf_entry_next(a_uint32_t dev_id, a_uint32_t next_mode,
                        fal_intf_mac_entry_t * entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_ip_intf_entry_next(dev_id, next_mode, entry);
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
dess_ip_age_time_set(a_uint32_t dev_id, a_uint32_t * time)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_ip_age_time_set(dev_id, time);
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
dess_ip_age_time_get(a_uint32_t dev_id, a_uint32_t * time)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_ip_age_time_get(dev_id, time);
    HSL_API_UNLOCK;
    return rv;
}

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
dess_ip_wcmp_entry_set(a_uint32_t dev_id, a_uint32_t wcmp_id, fal_ip_wcmp_t * wcmp)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_ip_wcmp_entry_set(dev_id, wcmp_id, wcmp);
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
dess_ip_wcmp_entry_get(a_uint32_t dev_id, a_uint32_t wcmp_id, fal_ip_wcmp_t * wcmp)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_ip_wcmp_entry_get(dev_id, wcmp_id, wcmp);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set IP WCMP hash key mode.
 * @param[in] dev_id device id
 * @param[in] hash_mode hash mode
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_ip_wcmp_hash_mode_set(a_uint32_t dev_id, a_uint32_t hash_mode)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_ip_wcmp_hash_mode_set(dev_id, hash_mode);
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
dess_ip_wcmp_hash_mode_get(a_uint32_t dev_id, a_uint32_t * hash_mode)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_ip_wcmp_hash_mode_get(dev_id, hash_mode);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set IP VRF base IP address.
 * @param[in] dev_id device id
 * @param[in] vrf_id VRF route index, from 0~7
 * @param[in] fal_ip4_addr_t IPv4 address for this VRF route
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_ip_vrf_base_addr_set(a_uint32_t dev_id, a_uint32_t vrf_id, fal_ip4_addr_t addr)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_ip_vrf_base_addr_set(dev_id, vrf_id, addr);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get IP VRF base IP address.
 * @param[in] dev_id device id
 * @param[in] vrf_id VRF route index, from 0~7
 * @param[out] fal_ip4_addr_t IPv4 address for this VRF route
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_ip_vrf_base_addr_get(a_uint32_t dev_id, a_uint32_t vrf_id, fal_ip4_addr_t * addr)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_ip_vrf_base_addr_get(dev_id, vrf_id, addr);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set IP VRF base IP address mask.
 * @param[in] dev_id device id
 * @param[in] vrf_id VRF route index, from 0~7
 * @param[in] fal_ip4_addr_t IPv4 address mask for this VRF route
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_ip_vrf_base_mask_set(a_uint32_t dev_id, a_uint32_t vrf_id, fal_ip4_addr_t addr)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_ip_vrf_base_mask_set(dev_id, vrf_id, addr);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get IP VRF base IP address mask.
 * @param[in] dev_id device id
 * @param[in] vrf_id VRF route index, from 0~7
 * @param[out] fal_ip4_addr_t IPv4 address mask for this VRF route
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_ip_vrf_base_mask_get(a_uint32_t dev_id, a_uint32_t vrf_id, fal_ip4_addr_t * addr)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_ip_vrf_base_mask_get(dev_id, vrf_id, addr);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set IP default route entry with special default route id.
 * @param[in] dev_id device id
 * @param[in] droute_id default route index, from 0~7
 * @param[in] fal_default_route_t default route entry
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_ip_default_route_set(a_uint32_t dev_id, a_uint32_t droute_id, fal_default_route_t * entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_ip_default_route_set(dev_id, droute_id, entry);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get IP default route entry with special default route id.
 * @param[in] dev_id device id
 * @param[in] droute_id default route index, from 0~7
 * @param[in] fal_default_route_t default route entry
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_ip_default_route_get(a_uint32_t dev_id, a_uint32_t droute_id, fal_default_route_t * entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_ip_default_route_get(dev_id, droute_id, entry);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set IP host route entry with special default route id.
 * @param[in] dev_id device id
 * @param[in] hroute_id default route index, from 0~15
 * @param[in] fal_host_route_t host route entry
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_ip_host_route_set(a_uint32_t dev_id, a_uint32_t hroute_id, fal_host_route_t * entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_ip_host_route_set(dev_id, hroute_id, entry);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get IP host route entry with special default route id.
 * @param[in] dev_id device id
 * @param[in] hroute_id default route index, from 0~15
 * @param[in] fal_host_route_t host route entry
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_ip_host_route_get(a_uint32_t dev_id, a_uint32_t hroute_id, fal_host_route_t * entry)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_ip_host_route_get(dev_id, hroute_id, entry);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set IP host route load balance.
 * @param[in] dev_id device id
 * @param[in] fal_ip4_rfs_t host route entry
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_ip_rfs_ip4_set(a_uint32_t dev_id, fal_ip4_rfs_t * rfs)
{
	sw_error_t rv;
	fal_intf_mac_entry_t mac_entry;

	HSL_API_LOCK;
	memset(&mac_entry, 0, sizeof(mac_entry));
	mac_entry.ip4_route = A_TRUE;
	mac_entry.ip6_route = A_TRUE;
	mac_entry.vid_low = rfs->vid;
	mac_entry.vid_high = rfs->vid;
	mac_entry.mac_addr = rfs->mac_addr;
	rv = _dess_ip_intf_entry_add(dev_id, &mac_entry);
	if(!rv)
    		rv = _dess_ip_rfs_ip4_set(dev_id, rfs);
	HSL_API_UNLOCK;
	return rv;
}

/**
 * @brief Set IP6 host route load balance.
 * @param[in] dev_id device id
 * @param[in] fal_ip6_rfs_t host route entry
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_ip_rfs_ip6_set(a_uint32_t dev_id, fal_ip6_rfs_t * rfs)
{
	sw_error_t rv;
	fal_intf_mac_entry_t mac_entry;

	HSL_API_LOCK;
	memset(&mac_entry, 0, sizeof(mac_entry));
	mac_entry.ip4_route = A_TRUE;
	mac_entry.ip6_route = A_TRUE;
	mac_entry.vid_low = rfs->vid;
	mac_entry.vid_high = rfs->vid;
	mac_entry.mac_addr = rfs->mac_addr;
	rv = _dess_ip_intf_entry_add(dev_id, &mac_entry);
	if(!rv)
		rv = _dess_ip_rfs_ip6_set(dev_id, rfs);
	HSL_API_UNLOCK;
	return rv;
}

/**
 * @brief del IP host route load balance.
 * @param[in] dev_id device id
 * @param[in] fal_ip4_rfs_t host route entry
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_ip_rfs_ip4_del(a_uint32_t dev_id, fal_ip4_rfs_t * rfs)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_ip_rfs_ip4_del(dev_id, rfs);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief del IP6 host route load balance.
 * @param[in] dev_id device id
 * @param[in] fal_ip6_rfs_t host route entry
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_ip_rfs_ip6_del(a_uint32_t dev_id, fal_ip6_rfs_t * rfs)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_ip_rfs_ip6_del(dev_id, rfs);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set flow type traffic default forward command.
 * @param[in] dev_id device id
 * @param[in] vrf_id VRF route index, from 0~7
 * @param[in] type traffic flow type pass through switch core
 * @param[in] fal_default_flow_cmd_mode_t default flow forward command when flow table mismatch
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_default_flow_cmd_set(a_uint32_t dev_id, a_uint32_t vrf_id, fal_flow_type_t type, fal_default_flow_cmd_t cmd)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_default_flow_cmd_set(dev_id, vrf_id, type, cmd);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get flow type traffic default forward command.
 * @param[in] dev_id device id
 * @param[in] vrf_id VRF route index, from 0~7
 * @param[in] type traffic flow type pass through switch core
 * @param[out] fal_default_flow_cmd_mode_t default flow forward command when flow table mismatch
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_default_flow_cmd_get(a_uint32_t dev_id, a_uint32_t vrf_id, fal_flow_type_t type, fal_default_flow_cmd_t * cmd)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_default_flow_cmd_get(dev_id, vrf_id, type, cmd);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set flow&route type traffic default forward command.
 * @param[in] dev_id device id
 * @param[in] vrf_id VRF route index, from 0~7
 * @param[in] type traffic flow type pass through switch core
 * @param[in] fal_default_flow_cmd_mode_t default flow&route forward command when route mac match but flow table mismatch
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_default_rt_flow_cmd_set(a_uint32_t dev_id, a_uint32_t vrf_id, fal_flow_type_t type, fal_default_flow_cmd_t cmd)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_default_rt_flow_cmd_set(dev_id, vrf_id, type, cmd);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get flow&route type traffic default forward command.
 * @param[in] dev_id device id
 * @param[in] vrf_id VRF route index, from 0~7
 * @param[in] type traffic flow type pass through switch core
 * @param[in] fal_default_flow_cmd_mode_t default flow&route forward command when route mac match but flow table mismatch
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_default_rt_flow_cmd_get(a_uint32_t dev_id, a_uint32_t vrf_id, fal_flow_type_t type, fal_default_flow_cmd_t * cmd)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_default_rt_flow_cmd_get(dev_id, vrf_id, type, cmd);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set blobal lock time.
 * @param[in] dev_id device id
 * @param[in] fal_glb_lock_time_t lock time
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_ip_glb_lock_time_set(a_uint32_t dev_id, fal_glb_lock_time_t lock_time)
{
	sw_error_t rv;

	HSL_API_LOCK;
	rv = _dess_ip_glb_lock_time_set(dev_id, lock_time);
	HSL_API_UNLOCK;
	return rv;
}

sw_error_t
dess_ip_init(a_uint32_t dev_id)
{
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    rv = dess_ip_reset(dev_id);
    SW_RTN_ON_ERROR(rv);

#ifndef HSL_STANDALONG
    {
        hsl_api_t *p_api;

        SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

        p_api->ip_host_add = dess_ip_host_add;
        p_api->ip_host_del = dess_ip_host_del;
        p_api->ip_host_get = dess_ip_host_get;
        p_api->ip_host_next = dess_ip_host_next;
        p_api->ip_host_counter_bind = dess_ip_host_counter_bind;
        p_api->ip_host_pppoe_bind = dess_ip_host_pppoe_bind;
        p_api->ip_pt_arp_learn_set = dess_ip_pt_arp_learn_set;
        p_api->ip_pt_arp_learn_get = dess_ip_pt_arp_learn_get;
        p_api->ip_arp_learn_set = dess_ip_arp_learn_set;
        p_api->ip_arp_learn_get = dess_ip_arp_learn_get;
        p_api->ip_source_guard_set = dess_ip_source_guard_set;
        p_api->ip_source_guard_get = dess_ip_source_guard_get;
        p_api->ip_unk_source_cmd_set = dess_ip_unk_source_cmd_set;
        p_api->ip_unk_source_cmd_get = dess_ip_unk_source_cmd_get;
        p_api->ip_arp_guard_set = dess_ip_arp_guard_set;
        p_api->ip_arp_guard_get = dess_ip_arp_guard_get;
        p_api->arp_unk_source_cmd_set = dess_arp_unk_source_cmd_set;
        p_api->arp_unk_source_cmd_get = dess_arp_unk_source_cmd_get;
        p_api->ip_route_status_set = dess_ip_route_status_set;
        p_api->ip_route_status_get = dess_ip_route_status_get;
        p_api->ip_intf_entry_add = dess_ip_intf_entry_add;
        p_api->ip_intf_entry_del = dess_ip_intf_entry_del;
        p_api->ip_intf_entry_next = dess_ip_intf_entry_next;
        p_api->ip_age_time_set = dess_ip_age_time_set;
        p_api->ip_age_time_get = dess_ip_age_time_get;
        p_api->ip_wcmp_hash_mode_set = dess_ip_wcmp_hash_mode_set;
        p_api->ip_wcmp_hash_mode_get = dess_ip_wcmp_hash_mode_get;
        p_api->ip_vrf_base_addr_set = dess_ip_vrf_base_addr_set;
        p_api->ip_vrf_base_addr_get = dess_ip_vrf_base_addr_get;
        p_api->ip_vrf_base_mask_set = dess_ip_vrf_base_mask_set;
        p_api->ip_vrf_base_mask_get = dess_ip_vrf_base_mask_get;
        p_api->ip_default_route_set = dess_ip_default_route_set;
        p_api->ip_default_route_get = dess_ip_default_route_get;
        p_api->ip_host_route_set = dess_ip_host_route_set;
        p_api->ip_host_route_get = dess_ip_host_route_get;
		p_api->ip_wcmp_entry_set = dess_ip_wcmp_entry_set;
        p_api->ip_wcmp_entry_get = dess_ip_wcmp_entry_get;
		p_api->ip_rfs_ip4_set = dess_ip_rfs_ip4_set;
		p_api->ip_rfs_ip6_set = dess_ip_rfs_ip6_set;
		p_api->ip_rfs_ip4_del = dess_ip_rfs_ip4_del;
		p_api->ip_rfs_ip6_del = dess_ip_rfs_ip6_del;
        p_api->ip_default_flow_cmd_set = dess_default_flow_cmd_set;
        p_api->ip_default_flow_cmd_get = dess_default_flow_cmd_get;
        p_api->ip_default_rt_flow_cmd_set = dess_default_rt_flow_cmd_set;
        p_api->ip_default_rt_flow_cmd_get = dess_default_rt_flow_cmd_get;
	p_api->ip_glb_lock_time_set = dess_ip_glb_lock_time_set;
    }
#endif

    return SW_OK;
}

/**
 * @}
 */

