/*
 * Copyright (c) 2014, 2016, The Linux Foundation. All rights reserved.
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



#include "fal_nat.h"
#include "fal_ip.h"
#include "hsl_api.h"
#include "sw.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_port_prop.h"
#include "dess_igmp.h"
#include "dess_reg.h"
#include "dess_acl.h"
#include "fal_multi.h"
#include "sal/os/aos_lock.h"

#if 0
/**
 * I/F prototype for complete igmpv3 & mldv2 support
 */

/*supports 32 entries*/
#define FAL_IGMP_SG_ENTRY_MAX 32

typedef enum
{
    FAL_ADDR_IPV4 = 0,
    FAL_ADDR_IPV6
} fal_addr_type_t;

typedef struct
{
    fal_addr_type_t type;
    union
    {
        fal_ip4_addr_t ip4_addr;
        fal_ip6_addr_t ip6_addr;
    } u;
} fal_igmp_sg_addr_t;

typedef struct
{
    fal_igmp_sg_addr_t source;
    fal_igmp_sg_addr_t group;
    fal_pbmp_t port_map;
} fal_igmp_sg_entry_t;

/**
 * @brief set PortMap of IGMP sg entry.
 *        search entry according to source/group address,
 *        update PortMap if SG entry is found, otherwise create a new sg entry.
 * @param[in] dev_id device id
 * @param[in-out] entry SG entry
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_igmp_sg_entry_set(a_uint32_t dev_id, fal_igmp_sg_entry_t * entry);

/**
 * @brief clear PortMap of IGMP sg entry.
 *        search entry according to source/group address,
 *        update PortMap if SG entry is found, delete the entry in case PortMap was 0.
 *        SW_NOT_FOUND will be returned in case search failed.
 * @param[in] dev_id device id
 * @param[in-out] entry SG entry
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_igmp_sg_entry_clear(a_uint32_t dev_id, fal_igmp_sg_entry_t * entry);

#define MULTI_DEBUG_
#ifdef MULTI_DEBUG_
#define MULTI_DEBUG(x...) aos_printk(x)
#else
#define MULTI_DEBUG(x...)
#endif

#define FAL_ACL_LIST_MULTICAST 55
#define FAL_MULTICAST_PRI   5

#define MULT_ACTION_SET      1
#define MULT_ACTION_CLEAR    1

static a_uint32_t rule_nr=1;

typedef struct
{
    a_uint8_t index; //MAX is 32
    fal_igmp_sg_entry_t entry; //Stores the specific ACL rule info
} multi_acl_info_t;
#endif

static a_uint32_t mul_rule_nr=1;

void
dess_multicast_init(a_uint32_t dev_id);

HSL_LOCAL sw_error_t multi_portmap_aclreg_set(a_uint32_t pos, fal_igmp_sg_entry_t * entry);

static multi_acl_info_t multi_acl_info[FAL_IGMP_SG_ENTRY_MAX];
static multi_acl_info_t multi_acl_group[FAL_IGMP_SG_ENTRY_MAX];

static int ip6_addr_is_null(fal_ip6_addr_t *ip6)
{
    if (NULL == ip6)
    {
        aos_printk("Invalid ip6 address\n");
        return -1;
    }
    if(0 == ip6->ul[0] && 0 == ip6->ul[1] && 0 == ip6->ul[2] && 0 == ip6->ul[3])
        return 1;
    else
        return 0;
}
static int multi_source_is_null(fal_igmp_sg_addr_t *s)
{
    if (NULL == s)
    {
        aos_printk("Invalid source address\n");
        return -1;
    }
    if(0 == s->type && 0==s->u.ip4_addr)
        return 1;
    if(1 == s->type && 1 == ip6_addr_is_null(&(s->u.ip6_addr)))
        return 1;

    return 0;
}

HSL_LOCAL int iterate_multicast_acl_rule(int list_id, int start_n)
{
    a_uint32_t dev_id=0;
    a_uint32_t rule_id;
    sw_error_t ret;
    fal_acl_rule_t  rule= {0};

    if(start_n>=FAL_IGMP_SG_ENTRY_MAX || start_n < 0)
    {
        return -1;
    }

    for(rule_id=0; rule_id<FAL_IGMP_SG_ENTRY_MAX; rule_id++)
    {
        ret = dess_acl_rule_query(dev_id, list_id, rule_id, &rule);

        if (ret==SW_NOT_FOUND )
            break;//NOT found in ACL rule
        if((rule_id+start_n)>=FAL_IGMP_SG_ENTRY_MAX)
        {
            return -1;
        }

        multi_acl_info[rule_id+start_n].index = rule_id; // consider here... index is NOT related start_n
        //MULTI_DEBUG("normal query1: rule dest_ip4_val=%x, src ip4=%x, dst_ip6=%x, ports=%x\n",
        //rule.dest_ip4_val, rule.src_ip4_val, rule.dest_ip6_val.ul[0], rule.ports);

        if(rule.dest_ip4_val !=0 && ip6_addr_is_null(&rule.dest_ip6_val))  //only ip4
        {
            multi_acl_info[rule_id+start_n].entry.group.type = FAL_ADDR_IPV4;
            multi_acl_info[rule_id+start_n].entry.source.type = FAL_ADDR_IPV4;
            multi_acl_info[rule_id+start_n].entry.group.u.ip4_addr = rule.dest_ip4_val;
            multi_acl_info[rule_id+start_n].entry.source.u.ip4_addr = rule.src_ip4_val;
            multi_acl_info[rule_id+start_n].entry.port_map= rule.ports;
        }
        else if(rule.dest_ip4_val ==0 && !ip6_addr_is_null(&rule.dest_ip6_val))  //only ip6
        {
            multi_acl_info[rule_id+start_n].entry.group.type = FAL_ADDR_IPV6;
            multi_acl_info[rule_id+start_n].entry.source.type = FAL_ADDR_IPV6;
            memcpy(&(multi_acl_info[rule_id+start_n].entry.group.u.ip6_addr), &(rule.dest_ip6_val), sizeof(rule.dest_ip6_val));
            memcpy(&(multi_acl_info[rule_id+start_n].entry.source.u.ip6_addr), &(rule.src_ip6_val), sizeof(rule.src_ip6_val));
            multi_acl_info[rule_id+start_n].entry.port_map= rule.ports;
        }
        if (FAL_FIELD_FLG_TST(rule.field_flg, FAL_ACL_FIELD_MAC_VID))
        {
            multi_acl_info[rule_id+start_n].entry.vlan_id = rule.vid_val;
        }
        else
        {
            multi_acl_info[rule_id+start_n].entry.vlan_id = 0xffff;
        }
    }

    return rule_id+start_n;
}
/*
** Iterate the total 32 multicast ACL entries.
    After the function completes:
         1. Stores all multicast related ACL rules in multi_acl_info[32]
         2. return the number of multicast related ACL rules
*/
HSL_LOCAL a_uint32_t dess_multicast_acl_query(void)
{
    int start_n;
    int total_n;
    //a_uint32_t i;

    start_n = iterate_multicast_acl_rule(FAL_ACL_LIST_MULTICAST, 0);
    if(-1 == start_n)
        aos_printk("ACL rule1 is FULL\n");
    total_n = iterate_multicast_acl_rule(FAL_ACL_LIST_MULTICAST+1, start_n);
    if(-1 == total_n)
        aos_printk("ACL rule2 is FULL\n");

    MULTI_DEBUG("KKK, the total ACL rule number is %d, (G,S) number=%d\n", total_n, start_n);
    /*
    for(i=0;i<total_n;i++)
    MULTI_DEBUG("KKK, indx=%d, multi_acl_info[%d].entry=[%d][%x]\n", multi_acl_info[i].index,i,
        multi_acl_info[i].entry.group.type, multi_acl_info[i].entry.group.u.ip4_addr );
        */

    return total_n;
}

HSL_LOCAL a_uint32_t dess_multicast_acl_total_n(a_uint32_t list_id)
{
    a_uint32_t dev_id=0;
    a_uint32_t ret;
    a_uint32_t rule_id;
    fal_acl_rule_t  rule= {0};

    for(rule_id=0; rule_id<FAL_IGMP_SG_ENTRY_MAX; rule_id++)
    {
        ret = dess_acl_rule_query(dev_id, list_id,
                                   rule_id, &rule);
        if(ret==SW_NOT_FOUND)
            return rule_id;

    }
    return 0;
}

#define DESS_FILTER_ACT_ADDR    0x5a000
#define DESS_FILTER_MSK_ADDR    0x59000
HSL_LOCAL sw_error_t multi_portmap_aclreg_set_all(a_uint32_t pos, fal_igmp_sg_entry_t * entry)
{
    a_uint32_t i, base, addr;
    a_uint32_t dev_id=0;
    a_uint32_t msk_valid=0;
    sw_error_t rv = SW_OK;

    /*  2'b00:start; 2'b01:continue; 2'b10:end; 2'b11:start&end*/
    for(i=pos; i<pos+4; i++)
    {
        base = DESS_FILTER_MSK_ADDR +(i<<5);
        addr = base+(4<<2); //fifth byte
        HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr, sizeof (a_uint32_t),
                              (a_uint8_t *) (&msk_valid),
                              sizeof (a_uint32_t));

        SW_RTN_ON_ERROR(rv);
        if ((((msk_valid>>6)&0x3) == 0x3) || (((msk_valid>>6)&0x3) == 0x2))
        {
            rv = multi_portmap_aclreg_set(i, entry);
            break;
        }
        else if ((((msk_valid>>6)&0x3)) == 0x0 || (((msk_valid>>6)&0x3) == 0x1))
        {
            rv = multi_portmap_aclreg_set(i, entry);
            continue;
        }
        else
        {
            aos_printk("The rule valid bit:6 7 is wrong!!!");
            break;
        }
    }
    return rv;
}
HSL_LOCAL sw_error_t multi_portmap_aclreg_set(a_uint32_t pos, fal_igmp_sg_entry_t * entry)
{
    a_uint32_t i, base, addr;
    a_uint32_t dev_id=0;
    sw_error_t rv;
    a_uint32_t act[3]= {0};
    fal_pbmp_t pm;

    pm = entry->port_map;

    base = DESS_FILTER_ACT_ADDR + (pos << 4);
    for (i = 0; i < 3; i++)
    {
        addr = base + (i << 2);
        HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr, sizeof (a_uint32_t),
                              (a_uint8_t *) (&act[i]),
                              sizeof (a_uint32_t));
        //MULTI_DEBUG("2:Get register value  0x%x =%x\n", addr, act[i]);
        SW_RTN_ON_ERROR(rv);
    }

    act[1] &= ~(0x7<<29); // clear the high 3 bits
    act[1] |= (pm&0x7)<<29;  //the low 3 bits of pm means redirect port 0,1,2

    /* New modification: update acl ACTION register from DENY to redirect */
    if (((act[2]>>6)&0x7) == 0x7) //DENY mode
    {
        if(pm)
        {
            act[2] &= ~(0x7<<6);//clear DENY bits
            act[2] |= (0x1<<4); //DES_PORT_EN set 1, enable
        }
    }
    else if (((act[2]>>4)&0x1) == 0x1) //redirect mode
    {
        if(pm==0)
        {
            act[2] &= ~(0x1<<4);//clear redirect bits
            act[2] |= (0x7<<6); //set to DENY
        }
    }

    act[2] &= ~0xf; //clear the low 4 bits of port 3,4,5,6
    act[2] |= (pm>>3)&0xf;

    addr = base + (1<<2);
    HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr, sizeof (a_uint32_t),
                          (a_uint8_t *) (&act[1]), sizeof (a_uint32_t));
    addr = base + (2<<2);
    HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr, sizeof (a_uint32_t),
                          (a_uint8_t *) (&act[2]), sizeof (a_uint32_t));
    MULTI_DEBUG("pos=%d, before sync portmap, the new act=%x %x\n", pos, act[1],act[2]);
    if((rv = dess_acl_rule_sync_multi_portmap(dev_id, pos, act)) < 0)
        aos_printk("Sync multicast portmap error\n");
    return rv;
}

HSL_LOCAL int multi_get_dp(void)
{
    a_uint32_t addr;
    a_uint32_t dev_id=0;
    sw_error_t rv;
    int val=0;

    addr = 0x624;//GLOBAL_FW_CTRL1
    HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr, sizeof (a_uint32_t),
                          (a_uint8_t *) (&val),
                          sizeof (a_uint32_t));
    if (rv != SW_OK)
        aos_printk("Get entry value error\n");

    val = (val>>24)&0x7f; //30:24, IGMP_JOIN_LEAVE_DP

    return val;
}
static int old_bind_p=-1;
HSL_LOCAL int multi_acl_bind(void)
{
    int bind_p;
    int i;

    bind_p = multi_get_dp();
    if(bind_p == old_bind_p)
        return 0;
    old_bind_p = bind_p;

    for(i=0; i<7; i++)
    {
        dess_acl_list_unbind(0, FAL_ACL_LIST_MULTICAST, FAL_ACL_DIREC_IN, FAL_ACL_BIND_PORT, i);
        dess_acl_list_unbind(0, FAL_ACL_LIST_MULTICAST+1, FAL_ACL_DIREC_IN, FAL_ACL_BIND_PORT, i);
    }

    if(bind_p==0)
    {
        for(i=0; i<7; i++)
        {
            dess_acl_list_bind(0, FAL_ACL_LIST_MULTICAST, FAL_ACL_DIREC_IN, FAL_ACL_BIND_PORT, i);
            dess_acl_list_bind(0, FAL_ACL_LIST_MULTICAST+1, FAL_ACL_DIREC_IN, FAL_ACL_BIND_PORT, i);
        }
    }
    else
    {
        for(i=0; i<7; i++)
            if((bind_p>>i) &0x1)
            {
                dess_acl_list_bind(0, FAL_ACL_LIST_MULTICAST, FAL_ACL_DIREC_IN, FAL_ACL_BIND_PORT, i);
                dess_acl_list_bind(0, FAL_ACL_LIST_MULTICAST+1, FAL_ACL_DIREC_IN, FAL_ACL_BIND_PORT, i);
            }
            else
                continue;
    }
	return 0;
}
/*
** Only update the related portmap from the privious input.
*/
HSL_LOCAL sw_error_t dess_multicast_acl_update( int list_id, int acl_index, fal_igmp_sg_entry_t * entry, int action)
{
    a_uint32_t dev_id=0;
    a_uint32_t rule_pos;
    sw_error_t rv = SW_OK;

    if(acl_index<0)
    {
        aos_printk("Something is wrong...\n");
        return SW_FAIL;
    }

    rule_pos = dess_acl_rule_get_offset(dev_id, list_id, multi_acl_group[acl_index].index);
    if(MULT_ACTION_SET == action)
    {
        multi_acl_group[acl_index].entry.port_map |= entry->port_map;
        if(entry->port_map == 0)
        {
            multi_acl_group[acl_index].entry.port_map = 0;
        }
    }
    else if(MULT_ACTION_CLEAR == action)
        multi_acl_group[acl_index].entry.port_map &= ~(entry->port_map);

    rv = multi_portmap_aclreg_set_all(rule_pos, &multi_acl_group[acl_index].entry);

    multi_acl_bind(); //Here need extra bind since IGMP join/leave would happen
    return rv;
}

HSL_LOCAL sw_error_t dess_multicast_acl_del(int list_id, int index)
{
    sw_error_t rv;
    int rule_id;

    rule_id = multi_acl_group[index].index;

    rv = dess_acl_rule_delete(0, list_id, rule_id, 1);
    multi_acl_bind(); //Here need extra bind since IGMP join/leave would happen
	return rv;
}

/*
** Add new acl rule with parameters: DIP, SIP, redirect port.
*/
HSL_LOCAL sw_error_t dess_multicast_acl_add(int list_id, fal_igmp_sg_entry_t * entry)
{
    sw_error_t val;
    a_uint32_t pos;
    fal_acl_rule_t acl= {0};

    /* IPv4 multicast */
    if( entry->group.type == FAL_ADDR_IPV4 )
    {
        MULTI_DEBUG("KKK1, group[%d][%x], source[%d][%x]\n",entry->group.type,
                    entry->group.u.ip4_addr, entry->source.type, entry->source.u.ip4_addr);

        acl.rule_type = FAL_ACL_RULE_IP4;

        if(entry->group.u.ip4_addr!= 0)
        {
            acl.dest_ip4_val = entry->group.u.ip4_addr;
            acl.dest_ip4_mask = 0xffffffff;//e->ip.dmsk.s_addr;
            FAL_FIELD_FLG_SET(acl.field_flg, FAL_ACL_FIELD_IP4_DIP);
        }
        if(entry->source.u.ip4_addr!= 0)
        {
            acl.src_ip4_val = entry->source.u.ip4_addr;
            acl.src_ip4_mask = 0xffffffff;//e->ip.smsk.s_addr;
            FAL_FIELD_FLG_SET(acl.field_flg, FAL_ACL_FIELD_IP4_SIP);
        }
        if( entry->port_map==0 )
            FAL_ACTION_FLG_SET ( acl.action_flg, FAL_ACL_ACTION_DENY);
        else
            //FAL_FIELD_FLG_SET(acl.field_flg, FAL_ACL_FIELD_INVERSE_ALL);
            FAL_ACTION_FLG_SET ( acl.action_flg, FAL_ACL_ACTION_PERMIT );

        /* Be careful, _dess_acl_action_parse() will block FAL_ACL_ACTION_DENY action, So we change it. */
        if( entry->port_map )
        {
            FAL_ACTION_FLG_SET(acl.action_flg, FAL_ACL_ACTION_REDPT);
            acl.ports = entry->port_map;
        }
    }
    else if( entry->group.type == FAL_ADDR_IPV6 )
    {
        MULTI_DEBUG("KKK2, group[%d][%x], source[%d][%x], pm=%x\n",entry->group.type,
                    entry->group.u.ip6_addr.ul[0], entry->source.type, entry->source.u.ip6_addr.ul[0], entry->port_map);

        acl.rule_type = FAL_ACL_RULE_IP6;

        if(!ip6_addr_is_null(&(entry->group.u.ip6_addr)))
        {
            memcpy(&acl.dest_ip6_val, &(entry->group.u.ip6_addr), sizeof(entry->group.u.ip6_addr));
            acl.dest_ip6_mask.ul[0] = 0xffffffff;
            acl.dest_ip6_mask.ul[1] = 0xffffffff;
            acl.dest_ip6_mask.ul[2] = 0xffffffff;
            acl.dest_ip6_mask.ul[3] = 0xffffffff;
            FAL_FIELD_FLG_SET(acl.field_flg, FAL_ACL_FIELD_IP6_DIP);
        }
        if(!ip6_addr_is_null(&(entry->source.u.ip6_addr)))
        {
            memcpy(&acl.src_ip6_val, &(entry->source.u.ip6_addr), sizeof(entry->source.u.ip6_addr));
            acl.src_ip6_mask.ul[0] = 0xffffffff;
            acl.src_ip6_mask.ul[1] = 0xffffffff;
            acl.src_ip6_mask.ul[2] = 0xffffffff;
            acl.src_ip6_mask.ul[3] = 0xffffffff;
            FAL_FIELD_FLG_SET(acl.field_flg, FAL_ACL_FIELD_IP6_SIP);
        }

        if( entry->port_map==0 )
            FAL_ACTION_FLG_SET ( acl.action_flg, FAL_ACL_ACTION_DENY);
        else
            //FAL_FIELD_FLG_SET(acl.field_flg, FAL_ACL_FIELD_INVERSE_ALL);
            FAL_ACTION_FLG_SET ( acl.action_flg, FAL_ACL_ACTION_PERMIT );

        /* Be careful, _dess_acl_action_parse() will block FAL_ACL_ACTION_DENY action, So we change it. */
        if( entry->port_map )
        {
            FAL_ACTION_FLG_SET(acl.action_flg, FAL_ACL_ACTION_REDPT);
            acl.ports = entry->port_map;
        }
    }

    if (entry->vlan_id < 4096)
    {
        FAL_FIELD_FLG_SET(acl.field_flg, FAL_ACL_FIELD_MAC_VID);
        acl.vid_val = entry->vlan_id;
        acl.vid_op = FAL_ACL_FIELD_MASK;
        acl.vid_mask = 0xfff;
    }

    pos = dess_multicast_acl_total_n(list_id);

    MULTI_DEBUG("In dess_multicast_acl_add, list_id=%d, rule_id=%d\n", list_id, pos);
    val = dess_acl_rule_add(0, list_id, pos, mul_rule_nr, &acl);

    multi_acl_bind();

    return val;
}


HSL_LOCAL int iterate_multicast_acl_group(a_uint32_t number, fal_igmp_sg_entry_t * entry)
{
    int count=0;
    int i;

    if (number == 0)
        return 0; //no any ACL rules based the query

    for(i=0; i<number; i++)
    {

        /*MULTI_DEBUG("2:iterate_multicast_acl_group, index=%d, multi_acl_info[%d].entry=type[%d]-addr[%x], pm=%x, new entry=type[%d]-addr[%x], pm=%x\n",
            multi_acl_info[i].index,i, multi_acl_info[i].entry.group.type, multi_acl_info[i].entry.group.u.ip6_addr.ul[0], multi_acl_info[i].entry.port_map,
            entry->group.type, entry->group.u.ip6_addr.ul[0], entry->port_map);*/

        if(0 == memcmp(&(multi_acl_info[i].entry.group), &(entry->group), sizeof(entry->group)))
        {
            memcpy(&multi_acl_group[count], &multi_acl_info[i], sizeof(multi_acl_info[i]));
            count++;//return the real number of multi_acl_group[]
            MULTI_DEBUG("in iterate_multicast_acl_group, count=%d, i=%d\n", count, i);
        }
    }

    return count;
}

HSL_LOCAL int mult_acl_has_entry(fal_igmp_sg_addr_t * group, fal_igmp_sg_addr_t *source)
{
    int rule_id;
    int ret = 0;
#if 0
    if(source != NULL)
    {
        MULTI_DEBUG("new group[%d]= %x %x %x %x, new source[%d]=%x %x %x %x\n",
                    group->type, group->u.ip6_addr.ul[0], group->u.ip6_addr.ul[1], group->u.ip6_addr.ul[2], group->u.ip6_addr.ul[3],
                    source->type, source->u.ip6_addr.ul[0], source->u.ip6_addr.ul[1], source->u.ip6_addr.ul[2], source->u.ip6_addr.ul[3]);

        MULTI_DEBUG("old group[%d]= %x %x %x %x, old source[%d]=%x %x %x %x\n",
                    multi_acl_group[0].entry.group.type, multi_acl_group[0].entry.group.u.ip6_addr.ul[0],
                    multi_acl_group[0].entry.group.u.ip6_addr.ul[1], multi_acl_group[0].entry.group.u.ip6_addr.ul[2], multi_acl_group[0].entry.group.u.ip6_addr.ul[3],
                    multi_acl_group[0].entry.source.type, multi_acl_group[0].entry.source.u.ip6_addr.ul[0],
                    multi_acl_group[0].entry.source.u.ip6_addr.ul[1], multi_acl_group[0].entry.source.u.ip6_addr.ul[2], multi_acl_group[0].entry.source.u.ip6_addr.ul[3]);
    }
#endif
    if(source == NULL)
    {
        for(rule_id=0; rule_id<FAL_IGMP_SG_ENTRY_MAX; rule_id++)
        {
            if( (0==memcmp(&multi_acl_group[rule_id].entry.group, group, sizeof(fal_igmp_sg_addr_t))) &&
                    (multi_source_is_null(&multi_acl_group[rule_id].entry.source)))
            {
                MULTI_DEBUG("Source=0:Orignal ACL rule have this entry! rule id= %d\n", rule_id);
                ret = rule_id+1; // ensure the return value is the actually number of entry
                break;
            }
        }
    }
    else
    {
        for(rule_id=0; rule_id<FAL_IGMP_SG_ENTRY_MAX; rule_id++)
        {
            if( (0==memcmp(&multi_acl_group[rule_id].entry.group, group, sizeof(fal_igmp_sg_addr_t))) &&
                    (0==memcmp(&multi_acl_group[rule_id].entry.source, source, sizeof(fal_igmp_sg_addr_t))))
            {
                MULTI_DEBUG("Orignal ACL rule have this entry! rule id= %d\n", rule_id);
                ret = rule_id+1; // ensure the return value is the actually number of entry
                break;
            }
        }
    }

    return ret;
}
HSL_LOCAL int portmap_null(int index, fal_pbmp_t portmap)
{
    int val;
    if (index<0)
    {
        aos_printk("portmap_null, index error\n");
        return SW_FAIL;
    }
    val = multi_acl_group[index].entry.port_map&(~portmap);

    if( 0 == (val&0xff) )
        return 1;
    else
        return 0;
}

HSL_LOCAL int portmap_valid(fal_igmp_sg_entry_t *g_source, fal_igmp_sg_entry_t *g_star)
{
    /* return 0 means the portmap is Not valid
        return 1 means the protmap is valid
    */
    /* MULTI_DEBUG("portmap_valid:g_source portmap=%x,  source=%x,group=%x, g_star portmap=%x, source=%x, group=%x\n",
          g_source->port_map, g_source->source.u.ip4_addr, g_source->group.u.ip4_addr,
          g_star->port_map, g_star->source.u.ip4_addr,g_star->group.u.ip4_addr);*/

    if(multi_source_is_null(&(g_star->source)))
    {
        if((g_source->port_map|g_star->port_map) == g_star->port_map)
        {
            return 0;
        }
    }

    return 1;
}


HSL_LOCAL int portmap_clear_type(int count, int index, fal_pbmp_t portmap)
{
    if(count>=0 && index<count) //new_index must >0; this means there're (G,*) and (G,S)
    {
        //if the new clear portmap will cause (G,S)=(G,*), Delete the (G,S)
        if((multi_acl_group[index].entry.port_map & (~portmap)) == multi_acl_group[count].entry.port_map)
            return 1; //delete


        //The following means there must be at least one bit clear wrong. Clear the (G,*) portmap.
        if( ((multi_acl_group[index].entry.port_map & (~portmap)) & (multi_acl_group[count].entry.port_map))
                != (multi_acl_group[count].entry.port_map))
            return 0;

        return 2; //Normal update
    }
    return 0;
}
sw_error_t dess_igmp_sg_entry_set(a_uint32_t dev_id, fal_igmp_sg_entry_t * entry)
{
    int number, count;
    int new_index=0;
    sw_error_t rv;
    int action = MULT_ACTION_SET;
    int i=0;

    HSL_API_LOCK;
    (void)dess_multicast_init(0);
    aos_mem_zero(multi_acl_info, FAL_IGMP_SG_ENTRY_MAX * sizeof (multi_acl_info_t));
    aos_mem_zero(multi_acl_group, FAL_IGMP_SG_ENTRY_MAX * sizeof (multi_acl_info_t));
    MULTI_DEBUG("Before query: group=%x, source=%x, portmap=%x\n", entry->group.u.ip4_addr, entry->source.u.ip4_addr, entry->port_map);
    //number is the total multicast ACL rules amount, stores in multi_acl_info[];
    number = dess_multicast_acl_query();

    if(number > FAL_IGMP_SG_ENTRY_MAX)
		return SW_FAIL;
    //count the total specific multicast group ACL rules, stores in multi_acl_group[]; count <=number
    count = iterate_multicast_acl_group(number, entry);
    //new_index-1 is the found entry index in multi_acl_group[], the real index is [new_index-1], 0 means no entry
    new_index = mult_acl_has_entry(&entry->group, &entry->source);

    MULTI_DEBUG("Start entry set: number=%d, count=%d, new_index=%d, pm=%x\n", number, count, new_index, entry->port_map);
    if( 0==multi_source_is_null(&entry->source) )  // new entry is (G, S)
    {
        MULTI_DEBUG("the new entry is (G,S)\n");
        if(count>0 && 0 == portmap_valid(entry, &(multi_acl_group[count-1].entry))) //specfic group entry exist,(G,S) or (G,*)
        {
            //return SW_NO_CHANGE; // The new portmap is Not valid
            MULTI_DEBUG("KKK, modified 1 !!!\n");
        }

        if(0 == new_index) //new entry, need add
        {
#if 0
            /*The method:
                1. predict if the portmap should be modified.
                2. add new acl rule with new portmap value.
            */
            if((tmp_index = mult_acl_has_entry(&entry->group, NULL))>0) // (G, *) entry exist
            {
                /*Here the update should new (G, S) OR orignal (G,*) portmap,
                be careful, entry's portmap value will be modified, so I use tmp_entry.
                */
                memcpy(tmp_entry, entry, sizeof(fal_igmp_sg_entry_t));
                MULTI_DEBUG("Here, (G,*) exist! tmp_index=%d\n", tmp_index);
                sw_multicast_acl_update(FAL_ACL_LIST_MULTICAST+1, tmp_index-1, tmp_entry, action);

                dess_multicast_acl_add(FAL_ACL_LIST_MULTICAST, tmp_entry);
                return SW_OK;
            }
#endif
            dess_multicast_acl_add(FAL_ACL_LIST_MULTICAST, entry);
            MULTI_DEBUG("Here, need add (G, S), portmap=%x\n", entry->port_map);
            return SW_OK;
        }
        else
        {
            //Here update Just: the old exist entry portmap OR the new entry portmap
            dess_multicast_acl_update(FAL_ACL_LIST_MULTICAST, new_index-1, entry, action);
            return SW_OK;
        }
    } //end of memcmp
    else  // new entry is (G, *)
    {
        if(0 == new_index) //new entry, need add
        {
            dess_multicast_acl_add(FAL_ACL_LIST_MULTICAST+1, entry);
            rv = SW_OK;
        }
        else if(new_index > 0) // (G, *) entry exist?
        {
            //Update exist (G, *) portmap with new portmap
            MULTI_DEBUG("(G,*) exist, before update, new_index=%d\n", new_index );
            dess_multicast_acl_update(FAL_ACL_LIST_MULTICAST+1, new_index-1, entry, action);
            rv = SW_OK;
        }

        if(new_index>0&&count>1) //(G,S*) and (G,*) exist, new entry is (G,*)
        {
            for(i=count-2; i>=0&&i<FAL_IGMP_SG_ENTRY_MAX; i--)
            {
                if(multi_acl_group[i].entry.port_map==0) //This ACL rule should be done nothing, DENY rule
                    continue;

                if(0 == portmap_valid(&(multi_acl_group[i].entry), &(multi_acl_group[count-1].entry)))
                {
                    MULTI_DEBUG("1:portmap is not valid, should delete, i=%d, source portmap=%x, gstar pm=%x\n",
                                i, multi_acl_group[i].entry.port_map, multi_acl_group[count-1].entry.port_map);
                    dess_multicast_acl_del(FAL_ACL_LIST_MULTICAST, i);
                    rv = SW_NO_MORE;
                }
                else
                {
                    MULTI_DEBUG("1:Start update all (G,S),i=%d, gstar portmap=%x\n", i, multi_acl_group[count-1].entry.port_map);
                    //Update all (G,S) entry portmap with new(G, *) portmap
                    dess_multicast_acl_update(FAL_ACL_LIST_MULTICAST, i, entry, action);
                    rv = SW_OK;
                }
            }
        }
        else if(new_index==0&&count>0) //only exist (G,S*) orignally
        {
            for(i=count-1; i>=0&&i<FAL_IGMP_SG_ENTRY_MAX; i--)
            {
                if(multi_acl_group[i].entry.port_map==0) //This ACL rule should be done nothing, DENY rule
                    continue;

                if(0 == portmap_valid(&(multi_acl_group[i].entry), entry))
                {
                    MULTI_DEBUG("2:portmap is not valid, should delete, i=%d, source portmap=%x, gstar pm=%x\n",
                                i, multi_acl_group[i].entry.port_map, entry->port_map);
                    dess_multicast_acl_del(FAL_ACL_LIST_MULTICAST, i);
                    rv = SW_NO_MORE;
                }
                else
                {
                    MULTI_DEBUG("2:Start update all (G,S),i=%d, portmap=%x\n", i, entry->port_map);
                    //Update all (G,S) entry portmap with new(G, *) portmap
                    dess_multicast_acl_update(FAL_ACL_LIST_MULTICAST, i, entry, action);
                    rv = SW_OK;
                }
            }
        }
    }
    HSL_API_UNLOCK;
    return rv;
}

sw_error_t dess_igmp_sg_entry_clear(a_uint32_t dev_id, fal_igmp_sg_entry_t * entry)
{
    a_uint32_t number, count;
    int new_index=0;
    sw_error_t rv = SW_OK;
    int action= MULT_ACTION_CLEAR;
    int i=0;
    int pm_type;

    HSL_API_LOCK;
    (void)dess_multicast_init(0);
    aos_mem_zero(multi_acl_info, FAL_IGMP_SG_ENTRY_MAX * sizeof (multi_acl_info_t));
    aos_mem_zero(multi_acl_group, FAL_IGMP_SG_ENTRY_MAX * sizeof (multi_acl_info_t));
    //number is the total multicast ACL rules amount, stores in multi_acl_info[];
    number = dess_multicast_acl_query();
    if(number > FAL_IGMP_SG_ENTRY_MAX)
		return SW_FAIL;
    //count the total specific multicast group ACL rules, stores in multi_acl_group[]; count <=number
    count = iterate_multicast_acl_group(number, entry);
	if(count == 0)
		return SW_OK;
    //new_index-1 is the found entry index in multi_acl_group[]
    new_index = mult_acl_has_entry(&entry->group, &entry->source);

    MULTI_DEBUG("Start entry clear: number=%d, count=%d, new_index=%d\n", number, count, new_index);
    if(0 == new_index || new_index > FAL_IGMP_SG_ENTRY_MAX || count > FAL_IGMP_SG_ENTRY_MAX) //new entry, the user command is wrong
    {
        return SW_NO_SUCH;
    }

    if( 0==multi_source_is_null(&entry->source) )  // new entry is (G, S)
    {
        if (portmap_null(new_index-1, entry->port_map))
        {
            MULTI_DEBUG("KKK entry clear, new(G,S), with null portmap. \n");
            dess_multicast_acl_del(FAL_ACL_LIST_MULTICAST, new_index-1);
            return SW_OK;
        }
        else
        {
            MULTI_DEBUG("KKK entry clear, new(G,S), with NOT null portmap. \n");
            /* If (G,*) doesn't exist, [count-1] is the last specfic group, maybe(G,*) */
            if(0 == multi_source_is_null(&(multi_acl_group[count-1].entry.source)))
            {
                dess_multicast_acl_update(FAL_ACL_LIST_MULTICAST, new_index-1, entry, action);
            }
            else //(G,*) exist
            {
                pm_type = portmap_clear_type(count-1, new_index-1, entry->port_map);
                if(pm_type == 0)
                    return SW_NO_CHANGE;
                else if(pm_type == 1)
                {
                    dess_multicast_acl_del(FAL_ACL_LIST_MULTICAST, new_index-1);
                    return SW_OK;
                }
                else
                {
                    //normal update; consider here...wangson
                    dess_multicast_acl_update(FAL_ACL_LIST_MULTICAST, new_index-1, entry, action);
                }
            }
        }
        return SW_OK;
    }
    else  //clear entry is (G,*)
    {
        MULTI_DEBUG("Here, new_index[%d]>=0, new portmap to clear is %x\n", new_index, entry->port_map);
        if (portmap_null(new_index-1, entry->port_map))
        {
            dess_multicast_acl_del(FAL_ACL_LIST_MULTICAST+1, new_index-1);
            rv = SW_OK;
        }
        else
        {
            MULTI_DEBUG("Update (G,*)!, new_index=%d, pm=%x\n", new_index, entry->port_map);
            dess_multicast_acl_update(FAL_ACL_LIST_MULTICAST+1, new_index-1, entry, action);
        }
        MULTI_DEBUG("KKK, ready clear (G, S*), count=%d\n", count);
#if 0
        if(count>1) // (G, S*) entry exist, if count=1 here, only exist(G,*)entry
        {
            //count must >=2
            for(i=count-2; i>=0; i--)
            {
                if(portmap_null(i, entry->port_map))
                {
                    MULTI_DEBUG("portmap_null, i=%d\n", i);
                    dess_multicast_acl_del(FAL_ACL_LIST_MULTICAST, i);
                    rv = SW_NO_MORE;
                }
                else
                {
                    //Update all (G,S) entry portmap with new(G, *) portmap
                    dess_multicast_acl_update(FAL_ACL_LIST_MULTICAST, i, entry, action);
                    rv = SW_OK;
                }
            }
        }
#else
        if(count>1) // (G, S*) entry exist, if count=1 here, only exist(G,*)entry
        {
            //count must >=2
            for(i=count-2; i>=0&&i<FAL_IGMP_SG_ENTRY_MAX; i--)
            {
                //PortMap of entry (S,G) == (*,G) portmap after clear?
                if((multi_acl_group[new_index-1].entry.port_map&(~(entry->port_map))) ==
                        multi_acl_group[i].entry.port_map)
                    dess_multicast_acl_del(FAL_ACL_LIST_MULTICAST, i);
                else
                    //Update all (G,S) entry portmap with new(G, *) portmap
                    dess_multicast_acl_update(FAL_ACL_LIST_MULTICAST, i, entry, action);
                rv = SW_OK;
            }
        }
#endif
    }
    HSL_API_UNLOCK;
    return rv;
}

static void
print_ip4addr(char * param_name, a_uint32_t * buf,
              a_uint32_t size)
{
    a_uint32_t i;
    fal_ip4_addr_t ip4;

    ip4 = *((fal_ip4_addr_t *) buf);
    aos_printk("%s", param_name);
    for (i = 0; i < 3; i++)
    {
        aos_printk("%d.", (ip4 >> (24 - i * 8)) & 0xff);
    }
    aos_printk("%d", (ip4 & 0xff));
}
static void
print_ip6addr(char * param_name, a_uint32_t * buf,
              a_uint32_t size)
{
    a_uint32_t i;
    fal_ip6_addr_t ip6;

    ip6 = *(fal_ip6_addr_t *) buf;
    aos_printk("%s", param_name);
    for (i = 0; i < 3; i++)
    {
        aos_printk("%x:%x:", (ip6.ul[i] >> 16) & 0xffff, ip6.ul[i] & 0xffff);
    }
    aos_printk("%x:%x", (ip6.ul[3] >> 16) & 0xffff, ip6.ul[3] & 0xffff);
}
sw_error_t dess_igmp_sg_entry_show(a_uint32_t dev_id)
{
    a_uint32_t number;
    int i;

    HSL_API_LOCK;
    (void)dess_multicast_init(0);
    aos_mem_zero(multi_acl_info, FAL_IGMP_SG_ENTRY_MAX * sizeof (multi_acl_info_t));
    aos_mem_zero(multi_acl_group, FAL_IGMP_SG_ENTRY_MAX * sizeof (multi_acl_info_t));
    //number is the total multicast ACL rules amount, stores in multi_acl_info[];
    number = dess_multicast_acl_query();

    for(i=0; i<number; i++)
    {
        if(0 == multi_acl_info[i].entry.group.type)  //ipv4
        {
            aos_printk("\n[%d]:", i);
            print_ip4addr(" [Group IPv4 addr]:", (a_uint32_t *)&(multi_acl_info[i].entry.group.u.ip4_addr), sizeof (fal_ip4_addr_t));
            print_ip4addr(" [Source IPv4 addr]:", (a_uint32_t *)&(multi_acl_info[i].entry.source.u.ip4_addr), sizeof (fal_ip4_addr_t));
            aos_printk("\n     [Portmap]: 0x%x ", multi_acl_info[i].entry.port_map);
            aos_printk(" [Vlanid]: %d ", multi_acl_info[i].entry.vlan_id);
        }
        else if(1 == multi_acl_info[i].entry.group.type)  //ipv6
        {
            aos_printk("\n[%d]:", i);
            print_ip6addr(" [Group IPv6 addr]: ", (a_uint32_t *)&(multi_acl_info[i].entry.group.u.ip6_addr), sizeof (fal_ip6_addr_t));
            print_ip6addr(" [Source IPv6 addr]: ", (a_uint32_t *)&(multi_acl_info[i].entry.source.u.ip6_addr), sizeof (fal_ip6_addr_t));
            aos_printk("\n     [Portmap]: 0x%x ", multi_acl_info[i].entry.port_map);
            aos_printk(" [Vlanid]: %d ", multi_acl_info[i].entry.vlan_id);
        }

    }
    aos_printk("\n\nTotal %d multicast ACL rules.\n", number);
    HSL_API_UNLOCK;

    return SW_OK;
}

void
dess_multicast_init(a_uint32_t dev_id)
{
    sw_error_t val;

    dess_acl_status_set(0, 1);

    val = dess_acl_list_creat(0, FAL_ACL_LIST_MULTICAST, FAL_MULTICAST_PRI);
    if(val !=SW_OK && val != SW_ALREADY_EXIST)
        aos_printk("Multicast 1 acl list create error, val=%d\n", val);

    val = dess_acl_list_creat(0, FAL_ACL_LIST_MULTICAST+1, FAL_MULTICAST_PRI+1);
    if(val !=SW_OK && val != SW_ALREADY_EXIST)
        aos_printk("Multicast 2 acl list create error, val=%d\n", val);

}

sw_error_t dess_igmp_sg_entry_query(a_uint32_t dev_id, fal_igmp_sg_info_t *info)
{
    a_uint32_t number;
    int i;

    HSL_API_LOCK;
    dess_multicast_init(0);
    aos_mem_zero(multi_acl_info, FAL_IGMP_SG_ENTRY_MAX * sizeof (multi_acl_info_t));
    /*number is the total multicast ACL rules amount, stores in multi_acl_info[];*/
    number = dess_multicast_acl_query();
    if(number > FAL_IGMP_SG_ENTRY_MAX)
    {
	    HSL_API_UNLOCK;
	    return SW_FAIL;
    }
    info->cnt = number;

    for(i=0; i<number; i++)
    {
        aos_mem_copy(&(info->acl_info[i]), &(multi_acl_info[i]), sizeof(multi_acl_info_t));
    }
    HSL_API_UNLOCK;

    return SW_OK;
}

