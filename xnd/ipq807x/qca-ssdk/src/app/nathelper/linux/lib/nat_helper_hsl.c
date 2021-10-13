/*
 * Copyright (c) 2012, 2015, 2018, The Linux Foundation. All rights reserved.
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


#ifdef KVER32
#include <linux/kconfig.h> 
#include <generated/autoconf.h>
#else
#include <linux/autoconf.h>
#endif
#include <linux/if_arp.h>
#include <linux/netdevice.h>
#include <linux/netfilter_arp.h>
#include <linux/netfilter_ipv4/ip_tables.h>
#include <net/netfilter/nf_conntrack.h>

#include "fal_nat.h"
#include "fal_ip.h"

#include "hsl_api.h"
#include "hsl.h"
#include "hsl_shared_api.h"
#include "../nat_helper.h"
#include "nat_helper_dt.h"
#include "nat_helper_hsl.h"
#include "../napt_acl.h"

int nat_chip_ver = 0;
a_bool_t napt_add_bypass_check = A_TRUE;

/* support 4 different interfaces (or 4 VLANs) */
static fal_intf_mac_entry_t global_if_mac_entry[MAX_INTF_NUM] = {{0}};
static a_uint8_t if_mac_count = 0;

extern int setup_wan_if;
extern int setup_lan_if;

#define DESS_CHIP(ver) ((((ver)&0xffff)>>8) == NAT_CHIP_VER_DESS)

#define ARP_HW_COUNTER_OFFSET  8

static a_uint8_t
nat_hw_debug_counter_get(void)
{
    static a_uint32_t nat_debug_counter = 0;

    return ((nat_debug_counter++) & 0x7);
}

static a_uint8_t
arp_hw_debug_counter_get(void)
{
    static a_uint32_t ip_debug_counter = 0;

    return ((ip_debug_counter++) & 0x7) + ARP_HW_COUNTER_OFFSET;
}


a_int32_t
nat_hw_add(nat_entry_t *nat)
{
    fal_nat_entry_t hw_nat = {0};

    hw_nat.flags = nat->flags;
    hw_nat.src_addr = nat->src_addr;
    hw_nat.trans_addr = nat->trans_addr;
    hw_nat.port_num = nat->port_num;
    hw_nat.port_range = nat->port_range;
    hw_nat.counter_en = 1;
    hw_nat.counter_id = nat_hw_debug_counter_get();

    if(NAT_ADD(0, &hw_nat) != 0)
    {
        return -1;
    }
    nat->entry_id = hw_nat.entry_id;

    return 0;
}

a_int32_t
nat_hw_del_by_index(a_uint32_t index)
{
    fal_nat_entry_t nat_entry = {0};

    HNAT_PRINTK("NAT_DEL(1) index=%d########\n", index);

    nat_entry.entry_id = index;
    if(NAT_DEL(0, FAL_NAT_ENTRY_ID_EN, &nat_entry)!= 0)
    {
        return -1;
    }

    return 0;
}

a_int32_t
nat_hw_flush(void)
{
    if(NAT_DEL(0, 0, 0)!= 0)
    {
        return -1;
    }

    return 0;
}

a_int32_t
napt_hw_flush(void)
{
    if(NAPT_DEL(0, 0, 0)!= 0)
    {
        return -1;
    }

    return 0;
}

static a_uint32_t private_ip_can_update = 1;
a_int32_t
nat_hw_prv_base_can_update(void)
{
    return private_ip_can_update;
}

void
nat_hw_prv_base_update_enable(void)
{
    private_ip_can_update = 1;
}

void
nat_hw_prv_base_update_disable(void)
{
    private_ip_can_update = 0;
}

static a_uint32_t private_ip_base = 0xc0a80000;
static a_uint32_t private_net_mask = 0xffffff00;
a_int32_t
nat_hw_prv_base_set(a_uint32_t ip)
{
#define PRIVATE_IP_MASK 0xffffff00

    ip = ntohl(ip);

	if (((nat_chip_ver & 0xffff) >> 8) == NAT_CHIP_VER_8327)
    		private_ip_base = ip & PRIVATE_IP_MASK;
	else
		private_ip_base = ip & nat_hw_prv_mask_get();


	if(DESS_CHIP(nat_chip_ver)) {
		if (IP_PRV_BASE_ADDR_SET(0, 0, (fal_ip4_addr_t)ip) != 0)
	    {
	        return -1;
	    }
	} else {
	    if (NAT_PRV_BASE_ADDR_SET(0, (fal_ip4_addr_t)ip) != 0)
	    {
	        return -1;
	    }
	}

    HNAT_PRINTK("%s: private_ip_base:%x private_ip_can_update:%d\n",
                __func__, private_ip_base, private_ip_can_update);

    return 0;
}

a_uint32_t
nat_hw_prv_base_get(void)
{
    return private_ip_base;
}


a_int32_t
nat_hw_prv_mask_set(a_uint32_t ipmask)
{
    ipmask = ntohl(ipmask);

	if(DESS_CHIP(nat_chip_ver)) {
		if (IP_PRV_BASE_MASK_SET(0, 0, (fal_ip4_addr_t)ipmask) != 0)
	    {
	        return -1;
	    }
	} else if (((nat_chip_ver & 0xffff)>>8) == NAT_CHIP_VER_8337) {
	    if (NAT_PRV_BASE_MASK_SET(0, (fal_ip4_addr_t)ipmask) != 0)
	    {
	        return -1;
	    }
	}
    private_net_mask = ipmask;

    HNAT_PRINTK("%s: 0x%08x\n", __FUNCTION__, private_net_mask);

    return 0;
}

a_uint32_t
nat_hw_prv_mask_get(void)
{
    return private_net_mask;
}


a_int32_t
nat_hw_prv_base_is_match(a_uint32_t ip)
{
#define PRIVATE_IP_MASK 0xffffff00

    a_uint32_t prv_base = private_ip_base;
    a_uint32_t prv_mask;

	if (((nat_chip_ver & 0xffff)>>8) == NAT_CHIP_VER_8327) {
    		if((prv_base & PRIVATE_IP_MASK) == (ip & PRIVATE_IP_MASK))
				return 1;
	} else {
		prv_mask = nat_hw_prv_mask_get();
		if((prv_base & prv_mask) == (ip & prv_mask))
        		return 1;
	}

    HNAT_PRINTK("%s: private_ip_base:%x usaddr:%x mismatch\n",
                __func__, prv_base, ip);

    return 0;
}

static a_int32_t
_arp_hw_if_mac_add(fal_intf_mac_entry_t *if_mac_entry)
{
    return IP_INTF_ENTRY_ADD(0, if_mac_entry);
}

static a_int32_t
_arp_hw_if_mac_del(fal_intf_mac_entry_t *if_mac_entry)
{
    return IP_INTF_ENTRY_DEL(0, FAL_IP_ENTRY_ID_EN, if_mac_entry);
}

a_int32_t
if_mac_cleanup(void)
{
	a_uint8_t i = 0;

	if_mac_count = 0;
	for(i = 0; i < MAX_INTF_NUM; i++)
	{
		if(_arp_hw_if_mac_del(&global_if_mac_entry[i]) != 0) {
			printk("mac del fail!\n");
			return -1;
		}
		memset(&global_if_mac_entry[i], 0, sizeof(fal_intf_mac_entry_t));
	}

	setup_wan_if = 0;
	setup_lan_if = 0;

	return 0;
}

#define MACADDR_LEN	6
a_int32_t
if_mac_add(a_uint8_t *mac, a_uint32_t vid, uint32_t ipv6)
{
    a_uint8_t i = 0;
    a_uint8_t zero_mac[MACADDR_LEN] = {0};

    if (!memcmp(mac, zero_mac, MACADDR_LEN))
        return 0;

    if(if_mac_count > MAX_INTF_NUM)
        return -1;

    for(i = 0; i < if_mac_count; i++)
    {
        if((!memcmp(global_if_mac_entry[i].mac_addr.uc, mac, 6)) &&
                (global_if_mac_entry[i].vid_low == vid))
        {
            HNAT_PRINTK("%s: mac exist id:%d\n", __func__,
                        global_if_mac_entry[i].entry_id);
            return 0;
        }
    }

    if(if_mac_count == MAX_INTF_NUM)
    {
        HNAT_ERR_PRINTK("%s: reach mac count max\n", __func__);
        return -1;
    }

    memset(&global_if_mac_entry[if_mac_count], 0, sizeof(fal_intf_mac_entry_t));
    memcpy(global_if_mac_entry[if_mac_count].mac_addr.uc, mac, 6);

    global_if_mac_entry[if_mac_count].entry_id = if_mac_count;
    if (1 == ipv6)
    {
        global_if_mac_entry[if_mac_count].ip6_route = 1;
    }
    else
    {
        global_if_mac_entry[if_mac_count].ip6_route = 0;
    }
    global_if_mac_entry[if_mac_count].ip4_route = 1;

    if (vid == 0)
    {
        global_if_mac_entry[if_mac_count].vid_low = 0;
        global_if_mac_entry[if_mac_count].vid_high = 511;
    }
    else
    {
        global_if_mac_entry[if_mac_count].vid_low = vid;
        global_if_mac_entry[if_mac_count].vid_high = vid;
    }

    if(_arp_hw_if_mac_add(&global_if_mac_entry[if_mac_count])!= 0)
    {
        return -1;
    }

    HNAT_PRINTK("%s: count:%d index:%d vid:%d  mac:%02x-%02x-%02x-%02x-%02x-%02x\n",
                __func__, if_mac_count, global_if_mac_entry[if_mac_count].entry_id, vid,
                mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
    if_mac_count ++;

    return 0;
}

static a_int32_t
_arp_hw_add(fal_host_entry_t *arp_entry)
{
    return IP_HOST_ADD(0, arp_entry);
}

a_int32_t
arp_hw_add(a_uint32_t port, a_uint32_t intf_id, a_uint8_t *ip, a_uint8_t *mac, int is_ipv6_entry)
{
    fal_host_entry_t arp_entry;


#ifdef ISIS /* Only for AR8337(S17) */
    if (NF_S17_WAN_TYPE_PPPOEV6 == nf_athrs17_hnat_wan_type)
    {
        memset(&arp_entry,0,sizeof(arp_entry));
        memcpy(&arp_entry.ip4_addr, ip, 4);
        memcpy(arp_entry.mac_addr.uc, mac, 6);
        arp_entry.status = ARP_AGE_NEVER;
        if (port == S17_WAN_PORT)
        {
            arp_entry.port_id = port;
        }
	 else
        {
            arp_entry.port_id = 6; /* always assigned to MAC 6 */
        }
        arp_entry.flags = FAL_IP_IP4_ADDR;
    }
    else
#endif /* not ISIS */
    {
        memset(&arp_entry,0,sizeof(arp_entry));
        if (0 == is_ipv6_entry)
        {
            memcpy(&arp_entry.ip4_addr, ip, 4);
            arp_entry.ip4_addr = ntohl(arp_entry.ip4_addr);
            arp_entry.flags = FAL_IP_IP4_ADDR;
        }
        else
        {
            memcpy(&arp_entry.ip6_addr, ip, 16);
            arp_entry.flags = FAL_IP_IP6_ADDR;
        }
        memcpy(arp_entry.mac_addr.uc, mac, 6);
        if ((NF_S17_WAN_TYPE_PPPOE == nf_athrs17_hnat_wan_type) && \
            (S17_WAN_PORT == port))
        {
            arp_entry.status = ARP_AGE_NEVER;
        }
        else
        {
            arp_entry.status = ARP_AGE;
        }
        arp_entry.port_id = port;
    }

    arp_entry.intf_id = intf_id;

    arp_entry.counter_en = 1;
    if (S17_WAN_PORT == port)
    {
        arp_entry.counter_id = 0xf;
    }
    else
    {
        arp_entry.counter_id = arp_hw_debug_counter_get();
    }

	if (IP_HOST_GET(0, 0x10, &arp_entry)) {
		HNAT_PRINTK("new arp for 0x%x\n", arp_entry.ip4_addr);
		if(_arp_hw_add(&arp_entry) != 0)
		{
			HNAT_ERR_PRINTK("%s: fail\n", __func__);
			return -1;
		}
	}

    if (0 == is_ipv6_entry)
    {
        HNAT_PRINTK("%s: index:%x  port:%d ip:%d.%d.%d.%d\n",
                    __func__, arp_entry.entry_id, port,
                    *(ip), *(ip+1), *(ip+2), *(ip+3));
    }
    
    if (0 != (arp_entry.entry_id & 0xFFFFFC00))
    {
        printk("Warning: arp_entry id should be only 10 bits!\n");
    }

    return arp_entry.entry_id;
}

#define AOS_HEADER_MAGIC 0xC0DE

a_int32_t
arp_if_info_get(void *data, a_uint32_t *sport, a_uint32_t *vid)
{
	aos_header_t *athr_header = NULL;
    if((data==0) || (sport==0) || (vid==0))
    {
        return -1;
    }

	athr_header = (aos_header_t *)data;

#if 0
    /*atheros header magic check*/
    if(athr_header->magic != AOS_HEADER_MAGIC)
    {
        return -1;
    }
#endif

    *sport = athr_header->sport;
    *vid = athr_header->vid;

    return 0;
}

#define MAX_PUBLIC_IP_CNT 16
struct public_ip_shadow
{
    a_uint32_t   ip;
    a_uint32_t   use_cnt;
};

static struct public_ip_shadow public_ip_shadow[MAX_PUBLIC_IP_CNT]= {{0}};
static a_uint32_t public_ip_cnt = 0;

a_int32_t
nat_hw_pub_ip_add(a_uint32_t ip, a_uint32_t *index)
{
    sw_error_t rv;
    a_uint32_t hw_index;
    a_uint32_t i;
	fal_nat_pub_addr_t ip_entry;

    for(i=0; i<MAX_PUBLIC_IP_CNT; i++)
    {
        if((ip == public_ip_shadow[i].ip) && (public_ip_shadow[i].use_cnt))
        {
            public_ip_shadow[i].use_cnt++;
            *index = i;
            return 0;
        }
    }

    if(public_ip_cnt >= MAX_PUBLIC_IP_CNT)
    {
        return -1;
    }

	memset(&ip_entry, 0, sizeof(ip_entry));
    ip_entry.pub_addr = ip;
    rv = NAT_PUB_ADDR_ADD(0,&ip_entry);
    if(rv != 0)
    {
        return -1;
    }

    public_ip_cnt++;
    hw_index = ip_entry.entry_id;
    public_ip_shadow[hw_index].ip = ip;
    public_ip_shadow[hw_index].use_cnt++;
    *index = hw_index;

    HNAT_PRINTK("%s: public_ip_cnt:%d index:%d ip:0x%x\n",
                __func__, public_ip_cnt, hw_index, public_ip_shadow[hw_index].ip);
    return 0;
}


void
napt_hw_mode_init(void)
{
    sw_error_t rv;
    /* age_speedup+age_thres_1/4+age_step_4+age_timer_28s*1+
       stop_age_when1+overwrite_disable */
    /* Also set NAT mode Port strict mode/symmetric mode */
    a_uint32_t entry = 0x15F01CB;

    HSL_REG_ENTRY_SET(rv, 0, NAT_CTRL, 0, (a_uint8_t *) (&entry),
                      sizeof (a_uint32_t));

    HSL_REG_ENTRY_GET(rv, 0, ROUTER_CTRL, 0,
                      (a_uint8_t *) (&entry), sizeof (a_uint32_t));

    /*set locktime 100us*/
    SW_SET_REG_BY_FIELD(ROUTER_CTRL, GLB_LOCKTIME, 1, entry);
    SW_SET_REG_BY_FIELD(ROUTER_CTRL, ARP_AGE_MODE, 1, entry);

    HSL_REG_ENTRY_SET(rv, 0, ROUTER_CTRL, 0,
                      (a_uint8_t *) (&entry), sizeof (a_uint32_t));
    HSL_REG_ENTRY_GET(rv, 0, MOD_ENABLE, 0,
                      (a_uint8_t *) (&entry), sizeof (a_uint32_t));
    SW_SET_REG_BY_FIELD(MOD_ENABLE, L3_EN, 1, entry);
    HSL_REG_ENTRY_SET(rv, 0, MOD_ENABLE, 0,
                      (a_uint8_t *) (&entry), sizeof (a_uint32_t));

    ACL_STATUS_SET(0, A_TRUE);
}

void
napt_hw_mode_cleanup(void)
{
	a_uint32_t entry;
	sw_error_t rv;
	if (!DESS_CHIP(nat_chip_ver)) {
		IP_ROUTE_STATUS_SET(0, A_FALSE);
		entry = 0;
	} else {
		HSL_REG_ENTRY_GET(rv, 0, NAT_CTRL, 0,
                      		(a_uint8_t *) (&entry), sizeof (a_uint32_t));
		SW_SET_REG_BY_FIELD(NAT_CTRL, NAT_EN, 0, entry);
	}
	HSL_REG_ENTRY_SET(rv, 0, NAT_CTRL, 0,
                      (a_uint8_t *) (&entry), sizeof (a_uint32_t));
	ACL_STATUS_SET(0, A_FALSE);
}

a_int32_t
nat_hw_pub_ip_del(a_uint32_t index)
{
    sw_error_t rv;

    if(public_ip_shadow[index].use_cnt>0)
    {
        public_ip_shadow[index].use_cnt--;
        if(public_ip_shadow[index].use_cnt == 0)
        {
			fal_nat_pub_addr_t ip_entry;
            HNAT_PRINTK("%s: public_ip_cnt:%d index:%d ip:0x%x\n",
                        __func__, public_ip_cnt, index, public_ip_shadow[index].ip);
            memset(&ip_entry,0,sizeof(ip_entry));
            ip_entry.pub_addr = public_ip_shadow[index].ip;
            rv = NAT_PUB_ADDR_DEL(0, 1, &ip_entry);
            if(rv != 0)
            {
                return -1;
            }

            public_ip_cnt--;
        }
        return 0;
    }

    return -1;
}

#define napt_entry_cp(to, from) \
{ \
    (to)->entry_id = (from)->entry_id; \
    (to)->status = (from)->status; \
    (to)->flags = (from)->flags; \
    (to)->src_addr = (from)->src_addr; \
    (to)->src_port = (from)->src_port; \
    (to)->dst_addr = (from)->dst_addr; \
    (to)->dst_port = (from)->dst_port; \
    (to)->trans_addr = (from)->trans_addr; \
    (to)->trans_port = (from)->trans_port; \
    (to)->ingress_packet = (from)->ingress_packet; \
    (to)->ingress_byte = (from)->ingress_byte; \
    (to)->egress_packet = (from)->egress_packet; \
    (to)->egress_byte = (from)->egress_byte; \
}

a_int32_t
napt_hw_add(napt_entry_t *napt)
{
	a_int32_t ret = 0;
	fal_napt_entry_t fal_napt = {0};
	fal_host_entry_t host_entry = {0};
	a_uint32_t next_hop = 0;

	napt_entry_cp(&fal_napt, napt);

	fal_napt.flags |= FAL_NAT_ENTRY_TRANS_IPADDR_INDEX;
	fal_napt.counter_en = 1;
	fal_napt.counter_id = nat_hw_debug_counter_get();
	fal_napt.action = FAL_MAC_FRWRD;

	if (!napt_add_bypass_check) {
		/*check arp entry*/
		host_entry.flags = FAL_IP_IP4_ADDR;
		host_entry.ip4_addr = fal_napt.src_addr;
		ret = IP_HOST_GET(0, FAL_IP_ENTRY_IPADDR_EN, &host_entry);
		if (ret) {
			HNAT_ERR_PRINTK("can not find src host entry!\n");
			return ret;
		}
		if (nf_athrs17_hnat_wan_type != NF_S17_WAN_TYPE_PPPOE) {
			next_hop = get_next_hop(fal_napt.dst_addr, fal_napt.src_addr);
			host_entry.ip4_addr =  next_hop ? next_hop : fal_napt.dst_addr;
			ret = IP_HOST_GET(0, FAL_IP_ENTRY_IPADDR_EN, &host_entry);
			if (ret) {
				HNAT_ERR_PRINTK("can not find dst host entry!\n");
				return ret;
			}
		}
	}

	ret = NAPT_ADD(0, &fal_napt);

	napt->entry_id = fal_napt.entry_id;
	return ret;
}

a_int32_t
napt_hw_get(napt_entry_t *napt, fal_napt_entry_t *entry)
{
	a_int32_t ret = 0;
	fal_napt_entry_t fal_napt = {0};

	napt_entry_cp(&fal_napt, napt);

	ret = NAPT_GET(0, 0, &fal_napt);

	if(!ret)
		*entry = fal_napt;
	return ret;
}

a_int32_t
napt_hw_dnat_cookie_add(napt_entry_t *napt, a_uint32_t cookie)
{
	a_int32_t ret = 0;
	fal_napt_entry_t fal_napt = {0};
	fal_napt.flags = napt->flags | 0x10;
	fal_napt.status = 0xf;
	fal_napt.dst_addr = napt->dst_addr;
	fal_napt.dst_port = napt->dst_port;
	fal_napt.trans_addr = napt->trans_addr;
	fal_napt.trans_port = napt->trans_port;
	fal_napt.src_port = napt->src_port;
	fal_napt.action = FAL_MAC_RDT_TO_CPU;
	fal_napt.flow_cookie = cookie;
	ret = NAPT_ADD(0, &fal_napt);
	return ret;
}

a_int32_t
napt_hw_snat_cookie_add(napt_entry_t *napt, a_uint32_t cookie)
{
	a_int32_t ret = 0;
	fal_napt_entry_t fal_napt = {0};
	fal_napt.flags = napt->flags | 0x10;
	fal_napt.status = 0xf;
	fal_napt.dst_addr = napt->dst_addr;
	fal_napt.dst_port = napt->dst_port;
	fal_napt.src_addr = napt->src_addr;
	fal_napt.src_port = napt->src_port;
	fal_napt.trans_port = napt->trans_port;
	fal_napt.action = FAL_MAC_RDT_TO_CPU;
	fal_napt.flow_cookie = cookie;
	ret = NAPT_ADD(0, &fal_napt);
	return ret;
}




a_int32_t
napt_hw_del(napt_entry_t *napt)
{
    a_int32_t ret = 0;

    fal_napt_entry_t fal_napt = {0};

    napt_entry_cp(&fal_napt, napt);
	napt_ct_counter_decrease();

    ret = NAPT_DEL(0, FAL_NAT_ENTRY_KEY_EN, &fal_napt);

    if(ret != 0)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

a_int32_t
napt_hw_first_by_age(napt_entry_t *napt, a_uint32_t age)
{
    a_int32_t ret = 0;
    fal_napt_entry_t fal_napt = {0};

    fal_napt.entry_id = FAL_NEXT_ENTRY_FIRST_ID;
    fal_napt.status = age;

    if(NAPT_NEXT(0, FAL_NAT_ENTRY_AGE_EN ,&fal_napt) !=0)
    {
        ret = -1;
    }

    napt_entry_cp(napt, &fal_napt);

    return ret;
}

a_int32_t
napt_hw_next_by_age(napt_entry_t *napt, a_uint32_t age)
{
    a_int32_t ret = 0;
    fal_napt_entry_t fal_napt = {0};

    fal_napt.entry_id = napt->entry_id;
    fal_napt.status = age;

    if(NAPT_NEXT(0, FAL_NAT_ENTRY_AGE_EN ,&fal_napt) !=0)
    {
        ret = -1;
    }

    napt_entry_cp(napt, &fal_napt);

    return ret;
}

a_int32_t
napt_hw_get_by_index(napt_entry_t *napt, a_uint16_t hw_index)
{
    fal_napt_entry_t fal_napt = {0};
	sw_error_t rv;

    if(hw_index == 0)
    {
        fal_napt.entry_id = FAL_NEXT_ENTRY_FIRST_ID;
    }
    else
    {
        fal_napt.entry_id = hw_index - 1;
    }

    if((rv = NAPT_NEXT(0, 0, &fal_napt)) != 0)
    {
        HNAT_ERR_PRINTK("<napt_hw_get_by_index>[rv:%d] error hw:%x sw:%x\n",
                        rv, napt->entry_id, hw_index);
        return -1;
    }

    napt_entry_cp(napt, &fal_napt);

    if(napt->entry_id != hw_index)
    {
        HNAT_ERR_PRINTK("<napt_hw_get_by_index>hw_index error hw:%x sw:%x\n",
                        napt->entry_id, hw_index);
        return -1;
    }

    return 0;
}

a_int32_t napt_hw_get_by_sip(a_uint32_t sip)
{
	fal_napt_entry_t napt;

	memset(&napt, 0, sizeof(fal_napt_entry_t));
	napt.entry_id = FAL_NEXT_ENTRY_FIRST_ID;
	napt.src_addr = sip;
	if (fal_napt_next(0, FAL_NAT_ENTRY_SOURCE_IP_EN, &napt) == SW_OK) {
		return 0;
	}

	return -1;
}

a_uint32_t
napt_hw_used_count_get(void)
{
#define NAPT_USED_COUNT
#define NAPT_USED_COUNT_OFFSET                       0x0e44 /*was:0x0e38*/
#define NAPT_USED_COUNT_E_LENGTH                     11
#define NAPT_USED_COUNT_E_OFFSET                     0x0
#define NAPT_USED_COUNT_NR_E                         1
    sw_error_t rv;

    a_uint32_t count = 0;
    HSL_REG_ENTRY_GET(rv, 0, NAPT_USED_COUNT, 0, (a_uint8_t *) (&count),
                      sizeof (a_uint32_t));

    return count;
}

sw_error_t napt_l3_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;
    a_uint32_t val;

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

    HSL_REG_FIELD_SET(rv, dev_id, MOD_ENABLE, 0, L3_EN, (a_uint8_t *) (&val),
                      sizeof (a_uint32_t));
    return rv;
}

sw_error_t napt_l3_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;
    a_uint32_t val;

    HSL_REG_FIELD_GET(rv, dev_id, MOD_ENABLE, 0, L3_EN, (a_uint8_t *) (&val),
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

sw_error_t napt_helper_hsl_init()
{
	return SW_OK;
}


