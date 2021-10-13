/*
 * Copyright (c) 2012, 2017, The Linux Foundation. All rights reserved.
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

#include "napt_acl.h"
#include "nat_helper.h"
#include "lib/nat_helper_hsl.h"
#include "hsl_shared_api.h"

static uint32_t aclrulemask = 0;
extern int nat_chip_ver;

uint32_t
get_aclrulemask(void)
{
    uint32_t ret;
    unsigned long  flags;

    local_irq_save(flags);
    ret = aclrulemask;
    local_irq_restore(flags);

    return ret;
}

void
set_aclrulemask(uint32_t acl_list)
{
    unsigned long  flags;

    local_irq_save(flags);
    aclrulemask |= 1<<acl_list;
    local_irq_restore(flags);

    return;
}

void
unset_aclrulemask(uint32_t acl_list)
{
    unsigned long  flags;

    local_irq_save(flags);
    aclrulemask &= ~(1<<acl_list);
    local_irq_restore(flags);

    return;
}

/*
 * set the IPv4 default route (to the next hop)
 */
void
droute_add_acl_rules(uint32_t local_ip, uint32_t local_ip_mask, uint32_t gw_entry_id)
{
    fal_acl_rule_t myacl;
    uint32_t rtnval;
    a_bool_t val;
    int i;

    if (get_aclrulemask() & (1 << S17_ACL_LIST_DROUTE))
        return;

    aos_printk("Adding ACL rules %d - Default Route \n", S17_ACL_LIST_DROUTE);

    printk("%s %d: 0x%08x\n", __FUNCTION__, __LINE__, ntohl(local_ip));

    memset(&myacl, 0, sizeof(fal_acl_rule_t));
    myacl.rule_type = FAL_ACL_RULE_IP4;
    myacl.dest_ip4_val = ntohl(local_ip);
    myacl.dest_ip4_mask = ntohl(local_ip_mask);
    /*
    IPv4 rule, with DIP field
    if DIP != Lan IP, force the ARP index redirect to the next hop
        enable packet forwarding & forward only. i.e. no FORCE_L3_MODE
        ARP_INDEX_EN, to the next hop ARP index, bind on LAN ports
    */
    FAL_FIELD_FLG_SET ( myacl.field_flg, FAL_ACL_FIELD_INVERSE_ALL );
    FAL_FIELD_FLG_SET ( myacl.field_flg, FAL_ACL_FIELD_IP4_DIP );
    FAL_ACTION_FLG_SET ( myacl.action_flg, FAL_ACL_ACTION_ARP_EN );

    myacl.arp_ptr = gw_entry_id;

    rtnval = ACL_LIST_CREATE(0, S17_ACL_LIST_DROUTE, S17_ACL_LIST_PRIO_HIGH);
    if ( rtnval != SW_OK )
    {
        aos_printk("ACL_LIST_CREATE ERROR...\n" );
        aos_printk("already created!? \n");
        return ;
    }

    rtnval = ACL_RULE_ADD (0, S17_ACL_LIST_DROUTE, 0, 1, &myacl );
    if ( rtnval != SW_OK )
    {
        aos_printk ( "ACL_RULE_ADD ERROR...\n" );
        return ;
    }

    for (i = S17_LAN_PORT0; i <= S17_LAN_PORT4; i++) {
        if (i != S17_WAN_PORT)
            ACL_LIST_BIND (0, S17_ACL_LIST_DROUTE, FAL_ACL_DIREC_IN, FAL_ACL_BIND_PORT, i);
    }

    /* check for the ACL enable bit */
    if (ACL_STATUS_GET(0, &val) == SW_OK)
    {
        if (val != A_TRUE)
        {
            aos_printk("ACL is not yet enabled. Enabling... \n");
            ACL_STATUS_SET(0, A_TRUE);
        }
    }

    set_aclrulemask(S17_ACL_LIST_DROUTE);
}
void
droute_del_acl_rules(void)
{
	int i;

	if (!(get_aclrulemask() & (1 << S17_ACL_LIST_DROUTE)))
		return;

	HNAT_PRINTK("IPv4 default route del rule #%d\n", S17_ACL_LIST_DROUTE);

	for (i = S17_LAN_PORT0; i <= S17_LAN_PORT4; i++) {
		if (i != S17_WAN_PORT)
			ACL_LIST_UNBIND(0, S17_ACL_LIST_DROUTE,
				FAL_ACL_DIREC_IN, FAL_ACL_BIND_PORT, i);
	}

	ACL_RULE_DEL(0, S17_ACL_LIST_DROUTE, 0, 1);


	ACL_LIST_DESTROY(0, S17_ACL_LIST_DROUTE);

	unset_aclrulemask(S17_ACL_LIST_DROUTE);
}

/*
 * set the IPv6 default route (to the next hop)
 */
void
ipv6_droute_add_acl_rules(struct in6_addr *local_ip, uint32_t gw_entry_id)
{
    fal_acl_rule_t myacl;
    uint32_t rtnval;
    a_bool_t val;
    int i;

    if (get_aclrulemask() & (1 << S17_ACL_LIST_IPV6DROUTE))
        return;

    printk("Adding ACL rules %d - IPv6 Default Route \n", S17_ACL_LIST_IPV6DROUTE);

    memset(&myacl, 0, sizeof(fal_acl_rule_t));
    myacl.rule_type = FAL_ACL_RULE_IP6;

    myacl.dest_ip6_val.ul[0] =  local_ip->s6_addr32[0]; /* FF02::1:FF00:0000/104 */
    myacl.dest_ip6_val.ul[1] =  local_ip->s6_addr32[1];
    myacl.dest_ip6_val.ul[2] =  local_ip->s6_addr32[2];
    myacl.dest_ip6_val.ul[3] =  local_ip->s6_addr32[3];
    myacl.dest_ip6_mask.ul[0] = 0xffffffff;
    myacl.dest_ip6_mask.ul[1] = 0xffffffff;
    myacl.dest_ip6_mask.ul[2] = 0xffffffff;
    myacl.dest_ip6_mask.ul[3] = 0xff000000;

    FAL_FIELD_FLG_SET ( myacl.field_flg, FAL_ACL_FIELD_INVERSE_ALL );
    FAL_FIELD_FLG_SET ( myacl.field_flg, FAL_ACL_FIELD_IP6_DIP );
    FAL_ACTION_FLG_SET ( myacl.action_flg, FAL_ACL_ACTION_ARP_EN );

    myacl.arp_ptr = gw_entry_id;

    rtnval = ACL_LIST_CREATE(0, S17_ACL_LIST_IPV6DROUTE, S17_ACL_LIST_PRIO_HIGH);
    if ( rtnval != SW_OK )
    {
        printk("qcaswitch_acl_list_creat ERROR...\n" );
        printk("already created!? \n");
        return ;
    }

    rtnval = ACL_RULE_ADD (0, S17_ACL_LIST_IPV6DROUTE, 0, 1, &myacl );
    if ( rtnval != SW_OK )
    {
        printk ( "qcaswitch_acl_rule_add ERROR...\n" );
        return ;
    }

    for (i = S17_LAN_PORT0; i <= S17_LAN_PORT4; i++) {
        if (i != S17_WAN_PORT)
            ACL_LIST_BIND (0, S17_ACL_LIST_IPV6DROUTE, FAL_ACL_DIREC_IN, FAL_ACL_BIND_PORT, i);
    }

    /* check for the ACL enable bit */
    if (ACL_STATUS_GET(0, &val) == SW_OK)
    {
        if (val != A_TRUE)
        {
            printk("ACL is not yet enabled. Enabling... \n");
            ACL_STATUS_SET(0, A_TRUE);
        }
    }

    set_aclrulemask(S17_ACL_LIST_IPV6DROUTE);
}

/*
 * Del Default Route  ACL rules
 */
void ipv6_droute_del_acl_rules(void)
{
    int i;

    HNAT_PRINTK("IPv6 default route del rule #%d\n", S17_ACL_LIST_IPV6DROUTE);

    if (!(get_aclrulemask() & (1 << S17_ACL_LIST_IPV6DROUTE)))
        return;

    for (i = S17_LAN_PORT0; i <= S17_LAN_PORT4; i++) {
        if (i != S17_WAN_PORT)
            ACL_LIST_UNBIND(0, S17_ACL_LIST_IPV6DROUTE, FAL_ACL_DIREC_IN, FAL_ACL_BIND_PORT, i);
    }

    ACL_RULE_DEL(0, S17_ACL_LIST_IPV6DROUTE, 0, 1);


    ACL_LIST_DESTROY(0, S17_ACL_LIST_IPV6DROUTE);

    unset_aclrulemask(S17_ACL_LIST_IPV6DROUTE);
}


static int isis_pppoe_del_rule0(void)
{
    int rtnval;

    rtnval = ACL_LIST_UNBIND(0, S17_ACL_LIST_PPPOE, FAL_ACL_DIREC_IN, FAL_ACL_BIND_PORT, S17_CPU_PORT);
    if (rtnval != SW_OK)
        aos_printk("unbind error... \n");

    rtnval = ACL_RULE_DEL(0, S17_ACL_LIST_PPPOE, 0, 1);
    if (rtnval != SW_OK)
        aos_printk("delete error... \n");

    rtnval = ACL_LIST_DESTROY(0, S17_ACL_LIST_PPPOE);
    if (rtnval != SW_OK)
        aos_printk("destroy error... \n");

    return rtnval;
}


/*
 * PPPoE ACL rules
 * Force ARP_INDEX_EN to the next hop for CPU port
 * Force SNAT and ARP_INDEX_EN to the next hop for LAN ports
 */
void pppoe_add_acl_rules(
		uint32_t wan_ip, uint32_t local_ip,
		uint32_t local_ip_mask, uint32_t gw_entry_id)
{
    fal_acl_rule_t myacl;
    uint32_t rtnval, cnt;
    a_bool_t val;
    int i;

    /* do the 1st, 2nd and 3rd rules */
    for (cnt = 0; cnt < 3; cnt++)
    {
        memset(&myacl, 0, sizeof(fal_acl_rule_t));

        switch (cnt)
        {
            case 0:
			if (((nat_chip_ver & 0xffff)>>8) != NAT_CHIP_VER_8327)
				break;

                aos_printk("PPPoE adding rule #%d\n", S17_ACL_LIST_PPPOE);
                myacl.rule_type = FAL_ACL_RULE_IP4;
                myacl.src_ip4_val = wan_ip;
                myacl.src_ip4_mask = 0xffffffff;
                aos_printk("WAN IP: %.8x\n", wan_ip);
                /*
                  IPv4 rule, with SIP field
                  IF SIP == WAN IP, FORCE the ARP_INDEX_EN to the PPPoE ARP INDEX
                  enable packet forwarding & forward only. i.e. no FORCE_L3_MODE
                  ARP_INDEX_EN, to the PPPoE peer ARP index,
                  Change the CVID to WAN_VID (2)
                  Bind to CPU port
                */
                FAL_FIELD_FLG_SET(myacl.field_flg, FAL_ACL_FIELD_IP4_SIP);
                FAL_FIELD_FLG_SET(myacl.field_flg, FAL_ACL_FIELD_MAC_CTAG_VID);

                FAL_ACTION_FLG_SET(myacl.action_flg, FAL_ACL_ACTION_ARP_EN);
                FAL_ACTION_FLG_SET(myacl.action_flg, FAL_ACL_ACTION_REMARK_CTAG_VID);

                myacl.arp_ptr = gw_entry_id;
                /* fixed with CVID = 2 */
                myacl.ctag_vid = 2;

                rtnval = ACL_LIST_CREATE(0, S17_ACL_LIST_PPPOE, S17_ACL_LIST_PRIO_HIGH);
                if (rtnval != SW_OK)
                {
                    aos_printk("ACL_LIST_CREATE ERROR...\n");
                    aos_printk("PPPoE Session ID changed !? \n");
                    /* delete the old ACL list */
                    rtnval = isis_pppoe_del_rule0();
                    if (rtnval != SW_OK)
                        aos_printk("pppoe_del_rule0: %d \n", rtnval);

                    /* create the ACL list again */
                    rtnval = ACL_LIST_CREATE(0, S17_ACL_LIST_PPPOE, S17_ACL_LIST_PRIO_HIGH);
                    if (rtnval != SW_OK)
                    {
                        aos_printk("ACL_LIST_CREATE ERROR...\n");
                        break;
                    }
                }

                rtnval = ACL_RULE_ADD(0, S17_ACL_LIST_PPPOE, 0, 1, &myacl);
                if (rtnval != SW_OK)
                {
                    aos_printk("ACL_RULE_ADD ERROR...\n");
                    break;
                }

                ACL_LIST_BIND(0, S17_ACL_LIST_PPPOE, FAL_ACL_DIREC_IN, FAL_ACL_BIND_PORT, S17_CPU_PORT_W);

                break;

            case 1:
                aos_printk("PPPoE adding rule #%d\n", S17_ACL_LIST_PPPOE+1);
                myacl.rule_type = FAL_ACL_RULE_IP4;
                myacl.dest_ip4_val = ntohl(local_ip);
                myacl.dest_ip4_mask = ntohl(local_ip_mask);

                /*
                  IPv4 rule, with DIP field
                  IF DIP != LAN IP, FORCE the ARP_INDEX_EN to the PPPoE ARP INDEX
                  AND FORCE L3 SNAT
                  ARP_INDEX_EN, to the PPPoE peer ARP index
                  Bind to LAN ports
                */

                FAL_FIELD_FLG_SET(myacl.field_flg, FAL_ACL_FIELD_INVERSE_ALL);
                FAL_FIELD_FLG_SET(myacl.field_flg, FAL_ACL_FIELD_IP4_DIP);

                FAL_ACTION_FLG_SET(myacl.action_flg, FAL_ACL_ACTION_POLICY_FORWARD_EN);
                FAL_ACTION_FLG_SET(myacl.action_flg, FAL_ACL_ACTION_ARP_EN);

                myacl.arp_ptr = gw_entry_id;
                myacl.policy_fwd = FAL_ACL_POLICY_SNAT;

                rtnval = ACL_LIST_CREATE(0, S17_ACL_LIST_PPPOE+1, S17_ACL_LIST_PRIO_HIGH);
                if (rtnval != SW_OK)
                {
                    aos_printk("ACL_LIST_CREATE ERROR...\n");
                    break;
                }

                rtnval = ACL_RULE_ADD(0, S17_ACL_LIST_PPPOE+1, 0, 1, &myacl);
                if (rtnval != SW_OK)
                {
                    aos_printk("ACL_RULE_ADD ERROR...\n");
                    break;
                }
                /* bind to LAN ports (1-4) */
                for (i = S17_LAN_PORT0; i <= S17_LAN_PORT4; i++) {
                    if (i != S17_WAN_PORT)
                        ACL_LIST_BIND(0, S17_ACL_LIST_PPPOE+1, FAL_ACL_DIREC_IN, FAL_ACL_BIND_PORT, i);
                }

                break;

            case 2:
                /*
                    User defined filter ACL
                    filter out 0x8100000288641100 packets and set the CVID to
                    a predefined VID (100 in this case)
                    ARP_INDEX_EN, to the PPPoE peer ARP index
                    Bind to CPU port
                 */
                aos_printk("PPPoE adding rule #%d\n", S17_ACL_LIST_PPPOE+2);
                myacl.rule_type = FAL_ACL_RULE_UDF;
                /* set the UDP ACL type as L2, with length 8, offset 12 */
                ACL_PORT_UDF_PROFILE_SET(0, S17_CPU_PORT, FAL_ACL_UDF_TYPE_L2, 12, 8);
                myacl.udf_len = 8;
                myacl.udf_offset = 12;
                myacl.udf_type = FAL_ACL_UDF_TYPE_L2;
                memset(&myacl.udf_val, 0, sizeof(myacl.udf_val));
                memset(&myacl.udf_mask, 0, sizeof(myacl.udf_mask));
                /* UDF filter to check for 0x8100000288641100 packets */
                myacl.udf_val[0] = 0x81;
                myacl.udf_val[1] = 0x00;
                myacl.udf_val[2] = 0x00;
                myacl.udf_val[3] = 0x02;
                myacl.udf_val[4] = 0x88;
                myacl.udf_val[5] = 0x64;
                myacl.udf_val[6] = 0x11;
                myacl.udf_val[7] = 0x00;

                myacl.udf_mask[0] = 0xff;
                myacl.udf_mask[1] = 0xff;
                myacl.udf_mask[2] = 0xff;
                myacl.udf_mask[3] = 0xff;
                myacl.udf_mask[4] = 0xff;
                myacl.udf_mask[5] = 0xff;
                myacl.udf_mask[6] = 0xff;
                myacl.udf_mask[7] = 0xff;

                FAL_FIELD_FLG_SET(myacl.field_flg, FAL_ACL_FIELD_UDF);
                FAL_FIELD_FLG_SET(myacl.field_flg, FAL_ACL_FIELD_MAC_CTAG_VID);

                FAL_ACTION_FLG_SET(myacl.action_flg, FAL_ACL_ACTION_ARP_EN);
                FAL_ACTION_FLG_SET(myacl.action_flg, FAL_ACL_ACTION_REMARK_CTAG_VID);

                myacl.arp_ptr = gw_entry_id;
                /* fixed with CVID = 100 */
                myacl.ctag_vid = 100;

                rtnval = ACL_LIST_CREATE(0, S17_ACL_LIST_PPPOE + 2, S17_ACL_LIST_PRIO_HIGH);
                if (rtnval != SW_OK)
                {
                    aos_printk("ACL_LIST_CREATE ERROR...\n");
                    break;
                }

                rtnval = ACL_RULE_ADD(0, S17_ACL_LIST_PPPOE + 2, 0, 1, &myacl);
                if (rtnval != SW_OK)
                {
                    aos_printk("ACL_RULE_ADD ERROR...\n");
                    break;
                }

                ACL_LIST_BIND(0, S17_ACL_LIST_PPPOE + 2, FAL_ACL_DIREC_IN, FAL_ACL_BIND_PORT, S17_CPU_PORT);
                break;
        }
    }

    if (ACL_STATUS_GET(0, &val) == SW_OK)
    {
        if (val != A_TRUE)
        {
            aos_printk("ACL is not yet enabled. Enabling... \n");
            ACL_STATUS_SET(0, A_TRUE);
        }
    }
}

static void pppoe_del_acl_rule1(void)
{
	int i;
	for (i = S17_LAN_PORT0; i <= S17_LAN_PORT4; i++) {
		if (i != S17_WAN_PORT)
			ACL_LIST_UNBIND(0, S17_ACL_LIST_PPPOE+1,
				FAL_ACL_DIREC_IN, FAL_ACL_BIND_PORT, i);
	}

	    ACL_RULE_DEL(0, S17_ACL_LIST_PPPOE+1, 0, 1);

	    ACL_LIST_DESTROY(0, S17_ACL_LIST_PPPOE+1);
}

static void pppoe_del_acl_rule2(void)
{
	    ACL_LIST_UNBIND(0, S17_ACL_LIST_PPPOE+2, FAL_ACL_DIREC_IN, FAL_ACL_BIND_PORT, S17_CPU_PORT);
	    ACL_LIST_UNBIND(0, S17_ACL_LIST_PPPOE+2, FAL_ACL_DIREC_IN, FAL_ACL_BIND_PORT, S17_LAN_PORT1);
	    ACL_LIST_UNBIND(0, S17_ACL_LIST_PPPOE+2, FAL_ACL_DIREC_IN, FAL_ACL_BIND_PORT, S17_LAN_PORT2);
	    ACL_LIST_UNBIND(0, S17_ACL_LIST_PPPOE+2, FAL_ACL_DIREC_IN, FAL_ACL_BIND_PORT, S17_LAN_PORT3);

	    ACL_RULE_DEL(0, S17_ACL_LIST_PPPOE+2, 0, 1);

	    ACL_LIST_DESTROY(0, S17_ACL_LIST_PPPOE+2);
}
void pppoe_del_acl_rules(void)
{
	if (((nat_chip_ver & 0xffff)>>8) == NAT_CHIP_VER_8327)
		isis_pppoe_del_rule0();
	pppoe_del_acl_rule1();
	pppoe_del_acl_rule2();
}
/*
 * When LAN & WAN IPs are too close, apply this ACL
 * ex: WAN 192.168.1.x, LAN 192.168.0.x
 */
void
ip_conflict_add_acl_rules(uint32_t wan_ip, uint32_t lan_ip, uint32_t gw_entry_id)
{
    fal_acl_rule_t myacl;
    uint32_t rtnval, cnt;
    a_bool_t val;
    int i;

    if (get_aclrulemask() & (1 << S17_ACL_LIST_IPCONF)) return;

    for (cnt = 0; cnt < 2; cnt++)
    {
        memset(&myacl, 0, sizeof(fal_acl_rule_t));

        aos_printk("IP conflict adding rule #%d\n", cnt);

        switch (cnt)
        {
            case 0:
                myacl.rule_type = FAL_ACL_RULE_IP4;
                myacl.dest_ip4_val = lan_ip;
                myacl.dest_ip4_mask = 0xffffff00;
                myacl.src_ip4_val = lan_ip;
                myacl.src_ip4_mask = 0xffffff00;

                /*
                  IPv4 rule, with DIP & SIP field
                  for DIP and SIP = LAN IP, do the next step
                */
                FAL_FIELD_FLG_SET(myacl.field_flg, FAL_ACL_FIELD_IP4_DIP);
                FAL_FIELD_FLG_SET(myacl.field_flg, FAL_ACL_FIELD_IP4_SIP);

                rtnval = ACL_LIST_CREATE(0, S17_ACL_LIST_IPCONF, S17_ACL_LIST_PRIO_HIGH);
                if (rtnval != SW_OK)
                {
                    aos_printk("ACL_LIST_CREATE ERROR...\n");
                    break;
                }

                rtnval = ACL_RULE_ADD(0, S17_ACL_LIST_IPCONF, 0, 1, &myacl);
                if (rtnval != SW_OK)
                {
                    aos_printk("ACL_RULE_ADD ERROR...\n");
                    break;
                }

                /* bind to LAN ports */
	   for (i = S17_LAN_PORT0; i <= S17_LAN_PORT4; i++) {
	       if (i != S17_WAN_PORT)
                        ACL_LIST_BIND(0, S17_ACL_LIST_IPCONF, FAL_ACL_DIREC_IN, FAL_ACL_BIND_PORT, i);
	   }

                aclrulemask |= (1 << S17_ACL_LIST_IPCONF);
                set_aclrulemask(S17_ACL_LIST_IPCONF);

                break;

            case 1:
                aos_printk("ARP index entry_id: %d\n", gw_entry_id);

                myacl.rule_type = FAL_ACL_RULE_IP4;
                myacl.src_ip4_val = lan_ip;
                myacl.src_ip4_mask = 0xffffff00;

                /*
                  IPv4 rule, with SIP field
                  enable packet forwarding & forward only. i.e. no FORCE_L3_MODE
                  ARP_INDEX_EN, to the PPPoE peer ARP index
                */

                FAL_FIELD_FLG_SET(myacl.field_flg, FAL_ACL_FIELD_IP4_SIP);

                FAL_ACTION_FLG_SET(myacl.action_flg, FAL_ACL_ACTION_POLICY_FORWARD_EN);
                FAL_ACTION_FLG_SET(myacl.action_flg, FAL_ACL_ACTION_ARP_EN);

                myacl.arp_ptr = gw_entry_id;
                myacl.policy_fwd = FAL_ACL_POLICY_SNAT;

                /* rule no. 4 */
                rtnval = ACL_LIST_CREATE(0, S17_ACL_LIST_IPCONF+1, S17_ACL_LIST_PRIO_HIGH);
                if (rtnval != SW_OK)
                {
                    aos_printk("ACL_LIST_CREATE ERROR...\n");
                    break;
                }

                rtnval = ACL_RULE_ADD(0, S17_ACL_LIST_IPCONF+1, 0, 1, &myacl);
                if (rtnval != SW_OK)
                {
                    aos_printk("ACL_RULE_ADD ERROR...\n");
                    break;
                }
                /* bind to LAN ports (1-4) */
	   for (i = S17_LAN_PORT0; i <= S17_LAN_PORT4; i++) {
	       if (i != S17_WAN_PORT)
                        ACL_LIST_BIND(0, S17_ACL_LIST_IPCONF+1, FAL_ACL_DIREC_IN, FAL_ACL_BIND_PORT, i);
	   }

                break;

            default:
                break;
        }
    }

    if (ACL_STATUS_GET(0, &val) == SW_OK)
    {
        if (val != A_TRUE)
        {
            aos_printk("ACL is not yet enabled. Enabling... \n");
            ACL_STATUS_SET(0, A_TRUE);
        }
    }
}

void ip_conflict_del_acl_rules(void)
{
	int i;

	if (!(get_aclrulemask() & (1 << S17_ACL_LIST_IPCONF)))
		return;

	for (i = S17_LAN_PORT0; i <= S17_LAN_PORT4; i++) {
	    if (i != S17_WAN_PORT) {
	        ACL_LIST_UNBIND(0, S17_ACL_LIST_IPCONF, FAL_ACL_DIREC_IN, FAL_ACL_BIND_PORT, i);
	        ACL_LIST_UNBIND(0, S17_ACL_LIST_IPCONF+1, FAL_ACL_DIREC_IN, FAL_ACL_BIND_PORT, i);
	    }
	}

	ACL_RULE_DEL(0, S17_ACL_LIST_IPCONF, 0, 1);

	ACL_LIST_DESTROY(0, S17_ACL_LIST_IPCONF);

	ACL_RULE_DEL(0, S17_ACL_LIST_IPCONF+1, 0, 1);

	ACL_LIST_DESTROY(0, S17_ACL_LIST_IPCONF+1);

	unset_aclrulemask(S17_ACL_LIST_IPCONF);
}


/*
solicted_node address
    FF02::1:FF00:0000/104 => Solicited-Node Address
*/
void
ipv6_snooping_solicted_node_add_acl_rules ( void )
{
    fal_acl_rule_t myacl;
    uint32_t rtnval;
    a_bool_t val;
    int i;

    memset ( &myacl, 0, sizeof ( fal_acl_rule_t ) );

    aos_printk("Adding ACL rules %d - %s\n", S17_ACL_LIST_IPV6_SOLICITED_NODE, __func__);
    myacl.rule_type = FAL_ACL_RULE_IP6;
    myacl.dest_ip6_val.ul[0] = 0xff020000; /* FF02::1:FF00:0000/104 */
    myacl.dest_ip6_val.ul[1] = 0x00000000;
    myacl.dest_ip6_val.ul[2] = 0x00000001;
    myacl.dest_ip6_val.ul[3] = 0xff000000;

    myacl.dest_ip6_mask.ul[0] = 0xffffffff;
    myacl.dest_ip6_mask.ul[1] = 0xffffffff;
    myacl.dest_ip6_mask.ul[2] = 0xffffffff;
    myacl.dest_ip6_mask.ul[3] = 0xff000000;

#ifdef CONFIG_ATH_8327_ACL_IPV6_PASSTHROUGH
    /* ACL action destination port, WAN, LAN and CPU ports */
    myacl.ports = (1 << S17_WAN_PORT) | (1 << S17_CPU_PORT) | (1 << S17_LAN_PORT0) |
                  (1 << S17_LAN_PORT1) | (1 << S17_LAN_PORT2) | (1 << S17_LAN_PORT3);

    FAL_FIELD_FLG_SET ( myacl.field_flg, FAL_ACL_FIELD_IP6_DIP );
#else
    /* ACL action destination port, LAN and CPU ports */
    myacl.ports = (1 << S17_CPU_PORT) | (1 << S17_LAN_PORT0) |
                  (1 << S17_LAN_PORT1) | (1 << S17_LAN_PORT2) | (1 << S17_LAN_PORT3);
    myacl.vid_val = 0x1;
    myacl.vid_mask = 0xfff;

    FAL_FIELD_FLG_SET ( myacl.field_flg, FAL_ACL_FIELD_IP6_DIP );
    FAL_FIELD_FLG_SET ( myacl.field_flg, FAL_ACL_FIELD_MAC_VID );
#endif

    FAL_ACTION_FLG_SET ( myacl.action_flg, FAL_ACL_ACTION_REDPT );

    rtnval = ACL_LIST_CREATE ( 0, S17_ACL_LIST_IPV6_SOLICITED_NODE, S17_ACL_LIST_PRIO_MID);
    if ( rtnval != SW_OK )
    {
        if(rtnval == SW_ALREADY_EXIST)
        {
            //aos_printk ( "ipv6_snooping_solicted rules acl list already exists.\n");

        } else {
            aos_printk ( "%s: ACL_LIST_CREATE ERROR (%d)...\n", __func__, rtnval );
        }
        return;
    }

    rtnval = ACL_RULE_ADD ( 0, S17_ACL_LIST_IPV6_SOLICITED_NODE, 0, 1, &myacl );
    if ( rtnval != SW_OK )
    {
        aos_printk ( "%s: ACL_RULE_ADD ERROR...(%d)\n", __func__, rtnval );
        return;
    }

    // ACL pattern source port
#ifdef CONFIG_ATH_8327_ACL_IPV6_PASSTHROUGH
    ACL_LIST_BIND ( 0, S17_ACL_LIST_IPV6_SOLICITED_NODE, FAL_ACL_DIREC_IN, FAL_ACL_BIND_PORT, S17_WAN_PORT );
#endif
    ACL_LIST_BIND ( 0, S17_ACL_LIST_IPV6_SOLICITED_NODE, FAL_ACL_DIREC_IN, FAL_ACL_BIND_PORT, S17_CPU_PORT );
    for (i = S17_LAN_PORT0; i <= S17_LAN_PORT4; i++) {
        if (i != S17_WAN_PORT)
            ACL_LIST_BIND ( 0, S17_ACL_LIST_IPV6_SOLICITED_NODE, FAL_ACL_DIREC_IN, FAL_ACL_BIND_PORT, i);
    }

    if ( ACL_STATUS_GET ( 0, &val ) == SW_OK )
    {
        if ( val != A_TRUE )
        {
            aos_printk ( "ACL is not yet enabled. Enabling... \n" );
            ACL_STATUS_SET(0, A_TRUE);
        }
    }
}


/*
Node Information Queries (RFC 4620 is experimental)
    FF02:0:0:0:0:2:FF00::/104 => Node Information Queries
*/
void
ipv6_snooping_nodeinfo_query_add_acl_rules ( void )
{
    fal_acl_rule_t myacl;
    uint32_t rtnval;
    a_bool_t val;
    int i;

    memset ( &myacl, 0, sizeof ( fal_acl_rule_t ) );

    aos_printk("Adding ACL rules %d - %s\n", S17_ACL_LIST_IPV6_NODEINFO_QUERY, __func__);
    myacl.rule_type = FAL_ACL_RULE_IP6;
    myacl.dest_ip6_val.ul[0] = 0xff020000; /* FF02:0:0:0:0:2:FF00::/104 */
    myacl.dest_ip6_val.ul[1] = 0x00000000;
    myacl.dest_ip6_val.ul[2] = 0x00000002;
    myacl.dest_ip6_val.ul[3] = 0xff000000;

    myacl.dest_ip6_mask.ul[0] = 0xffffffff;
    myacl.dest_ip6_mask.ul[1] = 0xffffffff;
    myacl.dest_ip6_mask.ul[2] = 0xffffffff;
    myacl.dest_ip6_mask.ul[3] = 0xff000000;

#ifdef CONFIG_ATH_8327_ACL_IPV6_PASSTHROUGH
    /* ACL action destination port, WAN, LAN and CPU ports */
    myacl.ports = (1 << S17_WAN_PORT) | (1 << S17_CPU_PORT) | (1 << S17_LAN_PORT0) |
                  (1 << S17_LAN_PORT1) | (1 << S17_LAN_PORT2) | (1 << S17_LAN_PORT3);

    FAL_FIELD_FLG_SET ( myacl.field_flg, FAL_ACL_FIELD_IP6_DIP );
#else
    /* ACL action destination port, LAN and CPU ports */
    myacl.ports = (1 << S17_CPU_PORT) | (1 << S17_LAN_PORT0) |
                  (1 << S17_LAN_PORT1) | (1 << S17_LAN_PORT2) | (1 << S17_LAN_PORT3);
    myacl.vid_val = 0x1;
    myacl.vid_mask = 0xfff;

    FAL_FIELD_FLG_SET ( myacl.field_flg, FAL_ACL_FIELD_IP6_DIP );
    FAL_FIELD_FLG_SET ( myacl.field_flg, FAL_ACL_FIELD_MAC_VID );
#endif

    FAL_ACTION_FLG_SET ( myacl.action_flg, FAL_ACL_ACTION_REDPT );

    rtnval = ACL_LIST_CREATE ( 0, S17_ACL_LIST_IPV6_NODEINFO_QUERY, S17_ACL_LIST_PRIO_MID);
    if ( rtnval != SW_OK )
    {
        aos_printk ( "%s: ACL_LIST_CREATE ERROR (%d)...\n", __func__, rtnval );
        return;
    }

    rtnval = ACL_RULE_ADD ( 0, S17_ACL_LIST_IPV6_NODEINFO_QUERY, 0, 1, &myacl );
    if ( rtnval != SW_OK )
    {
        aos_printk ( "%s: ACL_RULE_ADD ERROR...(%d)\n", __func__, rtnval );
        return;
    }

    // ACL pattern source port
#ifdef CONFIG_ATH_8327_ACL_IPV6_PASSTHROUGH
    ACL_LIST_BIND ( 0, S17_ACL_LIST_IPV6_NODEINFO_QUERY, FAL_ACL_DIREC_IN, FAL_ACL_BIND_PORT, S17_WAN_PORT );
#endif
    ACL_LIST_BIND ( 0, S17_ACL_LIST_IPV6_NODEINFO_QUERY, FAL_ACL_DIREC_IN, FAL_ACL_BIND_PORT, S17_CPU_PORT );
    for (i = S17_LAN_PORT0; i <= S17_LAN_PORT4; i++) {
        if (i != S17_WAN_PORT)
            ACL_LIST_BIND ( 0, S17_ACL_LIST_IPV6_NODEINFO_QUERY, FAL_ACL_DIREC_IN, FAL_ACL_BIND_PORT, i);
    }

    if ( ACL_STATUS_GET ( 0, &val ) == SW_OK )
    {
        if ( val != A_TRUE )
        {
            aos_printk ( "ACL is not yet enabled. Enabling... \n" );
            ACL_STATUS_SET(0, A_TRUE);
        }
    }
}


/*
sextuple0_group_acl_rules contains following addresses:
    FF02:0:0:0:0:0:0:1 => All Nodes Address
    FF02:0:0:0:0:0:0:2 => All Routers Address
    FF02:0:0:0:0:0:0:9 => RIP Routers
    FF02:0:0:0:0:0:0:C => SSDP
    FF02:0:0:0:0:0:0:16 => All MLDv2-capable routers
    FF02:0:0:0:0:0:0:FB => mDNSv6
*/
void
ipv6_snooping_sextuple0_group_add_acl_rules ( void )
{
    fal_acl_rule_t myacl;
    uint32_t rtnval;
    a_bool_t val;
    int i;

    memset ( &myacl, 0, sizeof ( fal_acl_rule_t ) );

    aos_printk("Adding ACL rules %d - %s\n", S17_ACL_LIST_IPV6_SEXTUPLE0_GROUP, __func__);
    myacl.rule_type = FAL_ACL_RULE_IP6;
    myacl.dest_ip6_val.ul[0] = 0xff020000; /* FF02::/120 */
    myacl.dest_ip6_val.ul[1] = 0x00000000;
    myacl.dest_ip6_val.ul[2] = 0x00000000;
    myacl.dest_ip6_val.ul[3] = 0x00000000;

    myacl.dest_ip6_mask.ul[0] = 0xffffffff;
    myacl.dest_ip6_mask.ul[1] = 0xffffffff;
    myacl.dest_ip6_mask.ul[2] = 0xffffffff;
    myacl.dest_ip6_mask.ul[3] = 0xffffff00;

#ifdef CONFIG_ATH_8327_ACL_IPV6_PASSTHROUGH
    /* ACL action destination port, WAN, LAN and CPU ports */
    myacl.ports = (1 << S17_WAN_PORT) | (1 << S17_CPU_PORT) | (1 << S17_LAN_PORT0) |
                  (1 << S17_LAN_PORT1) | (1 << S17_LAN_PORT2) | (1 << S17_LAN_PORT3);

    FAL_FIELD_FLG_SET ( myacl.field_flg, FAL_ACL_FIELD_IP6_DIP );
#else
    /* ACL action destination port, LAN and CPU ports */
    myacl.ports = (1 << S17_CPU_PORT) | (1 << S17_LAN_PORT0) |
                  (1 << S17_LAN_PORT1) | (1 << S17_LAN_PORT2) | (1 << S17_LAN_PORT3);
    myacl.vid_val = 0x1;
    myacl.vid_mask = 0xfff;

    FAL_FIELD_FLG_SET ( myacl.field_flg, FAL_ACL_FIELD_IP6_DIP );
    FAL_FIELD_FLG_SET ( myacl.field_flg, FAL_ACL_FIELD_MAC_VID );
#endif

    FAL_ACTION_FLG_SET ( myacl.action_flg, FAL_ACL_ACTION_REDPT );

    rtnval = ACL_LIST_CREATE ( 0, S17_ACL_LIST_IPV6_SEXTUPLE0_GROUP, S17_ACL_LIST_PRIO_MID);
    if ( rtnval != SW_OK )
    {
        if(rtnval == SW_ALREADY_EXIST)
        {
            //aos_printk ( "ipv6_snooping_sextuple0 rules acl list already exists.\n");
        } else {
            aos_printk ( "%s: ACL_LIST_CREATE ERROR (%d)...\n", __func__, rtnval );
        }
        return;
    }
    rtnval = ACL_RULE_ADD ( 0, S17_ACL_LIST_IPV6_SEXTUPLE0_GROUP, 0, 1, &myacl );
    if ( rtnval != SW_OK )
    {
        aos_printk ( "%s: ACL_RULE_ADD ERROR...(%d)\n", __func__, rtnval );
        return;
    }

    // ACL pattern source port
#ifdef CONFIG_ATH_8327_ACL_IPV6_PASSTHROUGH
    ACL_LIST_BIND ( 0, S17_ACL_LIST_IPV6_SEXTUPLE0_GROUP, FAL_ACL_DIREC_IN, FAL_ACL_BIND_PORT, S17_WAN_PORT );
#endif
    ACL_LIST_BIND ( 0, S17_ACL_LIST_IPV6_SEXTUPLE0_GROUP, FAL_ACL_DIREC_IN, FAL_ACL_BIND_PORT, S17_CPU_PORT );
    for (i = S17_LAN_PORT0; i <= S17_LAN_PORT4; i++) {
        if (i != S17_WAN_PORT)
            ACL_LIST_BIND ( 0, S17_ACL_LIST_IPV6_SEXTUPLE0_GROUP, FAL_ACL_DIREC_IN, FAL_ACL_BIND_PORT, i );
    }

    if ( ACL_STATUS_GET ( 0, &val ) == SW_OK )
    {
        if ( val != A_TRUE )
        {
            aos_printk ( "ACL is not yet enabled. Enabling... \n" );
            ACL_STATUS_SET(0, A_TRUE);
        }
    }
}


/*
quintruple0_1_group_acl_rules contains following addresses:
    FF02:0:0:0:0:0:1:2 => All-dhcp-agents
    FF02:0:0:0:0:0:1:3 => LLMNR
*/
void
ipv6_snooping_quintruple0_1_group_add_acl_rules ( void )
{
    fal_acl_rule_t myacl;
    uint32_t rtnval;
    a_bool_t val;
    int i;

    memset ( &myacl, 0, sizeof ( fal_acl_rule_t ) );

    aos_printk("Adding ACL rules %d - %s\n", S17_ACL_LIST_IPV6_QUINTRUPLE0_1_GROUP, __func__);
    myacl.rule_type = FAL_ACL_RULE_IP6;
    myacl.dest_ip6_val.ul[0] = 0xff020000; /* FF02:0:0:0:0:0:1::/125 */
    myacl.dest_ip6_val.ul[1] = 0x00000000;
    myacl.dest_ip6_val.ul[2] = 0x00000000;
    myacl.dest_ip6_val.ul[3] = 0x00010000;

    myacl.dest_ip6_mask.ul[0] = 0xffffffff;
    myacl.dest_ip6_mask.ul[1] = 0xffffffff;
    myacl.dest_ip6_mask.ul[2] = 0xffffffff;
    myacl.dest_ip6_mask.ul[3] = 0xfffffff8;

#ifdef CONFIG_ATH_8327_ACL_IPV6_PASSTHROUGH
    myacl.ports = (1 << S17_WAN_PORT) | (1 << S17_CPU_PORT) | (1 << S17_LAN_PORT0) |
                  (1 << S17_LAN_PORT1) | (1 << S17_LAN_PORT2) | (1 << S17_LAN_PORT3);

    FAL_FIELD_FLG_SET ( myacl.field_flg, FAL_ACL_FIELD_IP6_DIP );
#else
    /* ACL action destination port, LAN and CPU ports */
    myacl.ports = (1 << S17_CPU_PORT) | (1 << S17_LAN_PORT0) |
                  (1 << S17_LAN_PORT1) | (1 << S17_LAN_PORT2) | (1 << S17_LAN_PORT3);
    myacl.vid_val = 0x1;
    myacl.vid_mask = 0xfff;

    FAL_FIELD_FLG_SET ( myacl.field_flg, FAL_ACL_FIELD_IP6_DIP );
    FAL_FIELD_FLG_SET ( myacl.field_flg, FAL_ACL_FIELD_MAC_VID );
#endif

    FAL_ACTION_FLG_SET ( myacl.action_flg, FAL_ACL_ACTION_REDPT );

    rtnval = ACL_LIST_CREATE ( 0, S17_ACL_LIST_IPV6_QUINTRUPLE0_1_GROUP, S17_ACL_LIST_PRIO_MID);
    if ( rtnval != SW_OK )
    {
        if(rtnval == SW_ALREADY_EXIST)
        {
            //aos_printk ( "ipv6_snooping_quintruple0 rules acl list already exists.\n");
        } else {
            aos_printk ( "%s: ACL_LIST_CREATE ERROR (%d)...\n", __func__, rtnval );
        }
        return;
    }
    rtnval = ACL_RULE_ADD ( 0, S17_ACL_LIST_IPV6_QUINTRUPLE0_1_GROUP, 0, 1, &myacl );
    if ( rtnval != SW_OK )
    {
        aos_printk ( "%s: ACL_RULE_ADD ERROR...(%d)\n", __func__, rtnval );
        return;
    }

    /* ACL pattern soruce port */
#ifdef CONFIG_ATH_8327_ACL_IPV6_PASSTHROUGH
    ACL_LIST_BIND ( 0, S17_ACL_LIST_IPV6_QUINTRUPLE0_1_GROUP, FAL_ACL_DIREC_IN, FAL_ACL_BIND_PORT, S17_WAN_PORT );
#endif
    ACL_LIST_BIND ( 0, S17_ACL_LIST_IPV6_QUINTRUPLE0_1_GROUP, FAL_ACL_DIREC_IN, FAL_ACL_BIND_PORT, S17_CPU_PORT );
    for (i = S17_LAN_PORT0; i <= S17_LAN_PORT4; i++) {
        if (i != S17_WAN_PORT)
            ACL_LIST_BIND ( 0, S17_ACL_LIST_IPV6_QUINTRUPLE0_1_GROUP, FAL_ACL_DIREC_IN, FAL_ACL_BIND_PORT, i );
    }

    if ( ACL_STATUS_GET ( 0, &val ) == SW_OK )
    {
        if ( val != A_TRUE )
        {
            aos_printk ( "ACL is not yet enabled. Enabling... \n" );
            ACL_STATUS_SET(0, A_TRUE);
        }
    }
}

/*
When HW IGMPSNOOPING Enabled, we need to let UPnP SSDP Multicast packets send to lan
*/
void upnp_ssdp_add_acl_rules(void)
{
    fal_acl_rule_t myacl;
    uint32_t rtnval;
    a_bool_t val;
    int i;

    aos_printk("Adding ACL rules %d - %s\n", S17_ACL_LIST_UPNP_SSDP, __func__);
    memset ( &myacl, 0, sizeof ( fal_acl_rule_t ) );

    myacl.rule_type = FAL_ACL_RULE_IP4;
    myacl.dest_ip4_val = 0xeffffffa;  // 239.255.255.250
    myacl.dest_ip4_mask = 0xffffffff;
    /* ACL action destination port, LAN and CPU ports */
    myacl.ports = (1 << S17_CPU_PORT) | (1 << S17_LAN_PORT0) |
                  (1 << S17_LAN_PORT1) | (1 << S17_LAN_PORT2) | (1 << S17_LAN_PORT3);

    FAL_FIELD_FLG_SET ( myacl.field_flg, FAL_ACL_FIELD_IP4_DIP );
    FAL_ACTION_FLG_SET ( myacl.action_flg, FAL_ACL_ACTION_REDPT );

    rtnval = ACL_LIST_CREATE ( 0, S17_ACL_LIST_UPNP_SSDP, S17_ACL_LIST_PRIO_MID);
    if ( rtnval != SW_OK )
    {
        if(rtnval == SW_ALREADY_EXIST)
        {
            //aos_printk ( "upnp_ssdp rules acl list already exists.\n");
        } else {
            aos_printk ( "upnp_ssdp_acl_rules: ACL_LIST_CREATE ERROR (%d)...\n",rtnval );
        }
        return ;
    }

    rtnval = ACL_RULE_ADD ( 0, S17_ACL_LIST_UPNP_SSDP, 0, 1, &myacl );
    if ( rtnval != SW_OK )
    {
        aos_printk ( "upnp_ssdp_acl_rules: ACL_RULE_ADD ERROR...(%d)\n" ,rtnval );
        return ;
    }

    // Pattern source port
    ACL_LIST_BIND ( 0, S17_ACL_LIST_UPNP_SSDP, FAL_ACL_DIREC_IN, FAL_ACL_BIND_PORT, S17_CPU_PORT );
    for (i = S17_LAN_PORT0; i <= S17_LAN_PORT4; i++) {
        if (i != S17_WAN_PORT)
            ACL_LIST_BIND ( 0, S17_ACL_LIST_UPNP_SSDP, FAL_ACL_DIREC_IN, FAL_ACL_BIND_PORT, i );
    }

    if (ACL_STATUS_GET(0, &val) == SW_OK)
    {
        if (val != A_TRUE)
        {
            aos_printk("ACL is not yet enabled. Enabling... \n");
            ACL_STATUS_SET(0, A_TRUE);
        }
    }
}

void filter_power_cord_acl_rules(void)
{
    fal_acl_rule_t myacl;
    uint32_t rtnval;
    a_bool_t val;

    aos_printk("Adding ACL rules %d - %s\n", S17_ACL_LIST_PLC_FILTER, __func__);

    memset ( &myacl, 0, sizeof ( fal_acl_rule_t ) );
    myacl.rule_type = FAL_ACL_RULE_MAC;

    myacl.ethtype_val     = 0x88e1;
    myacl.ethtype_mask    = 0xffff;

    /* ACL action destination port PLC port */
    myacl.ports = (1 << S17_CPU_PORT) | (1 << 6);

    /* Set pattern type*/
    memset ( &myacl.field_flg, 0, sizeof ( myacl.field_flg ) );
    FAL_FIELD_FLG_SET ( myacl.field_flg, FAL_ACL_FIELD_MAC_ETHTYPE ); // set ethtype as pattern

    /* Set action type*/
    memset ( &myacl.action_flg, 0, sizeof ( myacl.action_flg ) );
    FAL_ACTION_FLG_SET ( myacl.action_flg, FAL_ACL_ACTION_REDPT); // set action as DENY, FAL_ACL_ACTION_DENY

    /*
        memcpy ( myacl.dest_mac_val.uc,  mac, 6 );
        memcpy ( myacl.dest_mac_mask.uc, mac_mask, 6 );
        FAL_FIELD_FLG_SET ( myacl.field_flg, FAL_ACL_FIELD_MAC_DA );
    */

    rtnval = ACL_LIST_CREATE (0, S17_ACL_LIST_PLC_FILTER, S17_ACL_LIST_PRIO_HIGH);
    if(rtnval != SW_OK)
    {
        aos_printk ( "filter_power_cord_acl_rules: ACL_LIST_CREATE ERROR (%d)\n",rtnval );
        return;
    }

    rtnval = ACL_RULE_ADD ( 0, S17_ACL_LIST_PLC_FILTER, 0, 1, &myacl );
    if ( rtnval != SW_OK )
    {
        aos_printk ( "filter_power_cord_acl_rules: ACL_RULE_ADD ERROR...(%d)\n", rtnval );
        return;
    }

    // ACL pattern source port
    ACL_LIST_BIND ( 0, S17_ACL_LIST_PLC_FILTER, FAL_ACL_DIREC_IN, FAL_ACL_BIND_PORT, S17_CPU_PORT);
    ACL_LIST_BIND ( 0, S17_ACL_LIST_PLC_FILTER, FAL_ACL_DIREC_IN, FAL_ACL_BIND_PORT, 6);

    if ( ACL_STATUS_GET ( 0, &val ) == SW_OK )
    {
        if ( val != A_TRUE )
        {
            aos_printk ( "ACL is not yet enabled. Enabling... \n" );
            ACL_STATUS_SET ( 0, A_TRUE );
        }
    }
}

/* enable pppoe_passthrough by default */
static int isis_pppoe_passthrough = 0;

unsigned int isis_pppoe_passthrough_process(struct sk_buff *skb, aos_header_t *athr_header)
{
    unsigned char *smac = &skb->data[6];

    if ( isis_pppoe_passthrough == 0 )
        return -1;

    if ( ( ( skb->data[20] == 0x88 && ( skb->data[21] == 0x63 ) ) || ( skb->data[16] == 0x88 && ( skb->data[17] == 0x63 ) ) )
            && ( athr_header->sport != S17_WAN_PORT ) )
    {
        pppoe_passthrough_acl_rules ( 0, smac );
    }

    return 0;
}

unsigned int isis_set_pppoe_passthrough ( int enable )
{
    if ( enable )
        isis_pppoe_passthrough = 1;
    else
        isis_pppoe_passthrough = 0;

    printk ( "## isis_set_pppoe_passthrough %s !\n", enable != 0 ? "enabled" : "disabled" );

    return 0;
}

unsigned int isis_enable_pppoe_discovery_acl(void)
{
#if 0
    printk ( "## isis_enable_pppoe_discovery_acl !\n");

    athrs17_reg_write ( MOD_ENABLE_OFFSET, athrs17_reg_read ( MOD_ENABLE_OFFSET ) | ( 1 << MOD_ENABLE_ACL_EN_BOFFSET ) );
    printk ( "athrs17_reg_write(MOD_ENABLE_OFFSET) = 0x%08x\n", athrs17_reg_read ( MOD_ENABLE_OFFSET ) );
#endif
    return 0;
}


int pppoe_passthrough_acl_rules(uint32_t gw_entry_id, unsigned char *mac)
{
    fal_acl_rule_t myacl;
    uint32_t rtnval;
    a_bool_t val;
    uint32_t rule_list_id;
    unsigned char mac_mask[6] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    int i;

    if (isis_pppoe_passthrough > MAX_PPPOE_PASSTHROUGH_NUM)
    {
        aos_printk("Support %d PPPoE passthrough hosts only... \n", MAX_PPPOE_PASSTHROUGH_NUM);
        return -1;
    }

    {
        /* lan -> wan, pppoe discovery */
        rule_list_id = S17_ACL_LIST_PPPOE_PASSTHROUGH_LAN_TO_WAN + (isis_pppoe_passthrough - 1) * 2;
        printk ( "creating ACL list_id: %d \n", rule_list_id );

        memset ( &myacl, 0, sizeof ( fal_acl_rule_t ) );
        myacl.rule_type = FAL_ACL_RULE_MAC;
        memcpy ( myacl.src_mac_val.uc, mac, 6 );
        memcpy ( myacl.src_mac_mask.uc, mac_mask, 6 );
        myacl.ethtype_val     = 0x8863;
        myacl.ethtype_mask    = 0xffff;

        myacl.ports = (1 << S17_WAN_PORT);

        memset ( &myacl.field_flg, 0, sizeof ( myacl.field_flg ) );

        FAL_FIELD_FLG_SET ( myacl.field_flg, FAL_ACL_FIELD_MAC_SA );
        FAL_FIELD_FLG_SET ( myacl.field_flg, FAL_ACL_FIELD_MAC_ETHTYPE );

        memset ( &myacl.action_flg, 0, sizeof ( myacl.action_flg ) );

        FAL_ACTION_FLG_SET ( myacl.action_flg, FAL_ACL_ACTION_REDPT );

        rtnval = ACL_LIST_CREATE (0, rule_list_id, S17_ACL_LIST_PRIO_MID);
        if ( rtnval != SW_OK )
        {
            aos_printk ( "pppoe_passthrough_acl_rules: ACL_LIST_CREATE ERROR (%d)\n",rtnval );
            return -1;
        }

        rtnval = ACL_RULE_ADD ( 0, rule_list_id, 0, 1, &myacl );
        if ( rtnval != SW_OK )
        {
            aos_printk ( "pppoe_passthrough_acl_rules: ACL_RULE_ADD ERROR...(%d)\n", rtnval );
            return -1;
        }

        for (i = S17_LAN_PORT0; i <= S17_LAN_PORT4; i++) {
            if (i != S17_WAN_PORT)
                ACL_LIST_BIND ( 0, rule_list_id, FAL_ACL_DIREC_IN, FAL_ACL_BIND_PORT, i);
        }

    }

    {
        /* lan -> wan, pppoe session */
        rule_list_id = S17_ACL_LIST_PPPOE_PASSTHROUGH_LAN_TO_WAN + (isis_pppoe_passthrough - 1) * 2 + 1;

        printk ( "creating ACL list_id: %d \n", rule_list_id );

        memset ( &myacl, 0, sizeof ( fal_acl_rule_t ) );
        myacl.rule_type = FAL_ACL_RULE_MAC;
        memcpy ( myacl.src_mac_val.uc, mac, 6 );
        memcpy ( myacl.src_mac_mask.uc, mac_mask, 6 );

        myacl.ethtype_val     = 0x8864;
        myacl.ethtype_mask    = 0xffff;

        myacl.ports = ( 1<<S17_WAN_PORT /* WAN */ );

        memset ( &myacl.field_flg, 0, sizeof ( myacl.field_flg ) );

        FAL_FIELD_FLG_SET ( myacl.field_flg, FAL_ACL_FIELD_MAC_SA );
        FAL_FIELD_FLG_SET ( myacl.field_flg, FAL_ACL_FIELD_MAC_ETHTYPE );

        memset ( &myacl.action_flg, 0, sizeof ( myacl.action_flg ) );

        FAL_ACTION_FLG_SET ( myacl.action_flg, FAL_ACL_ACTION_REDPT );

        rtnval = ACL_LIST_CREATE ( 0, rule_list_id, S17_ACL_LIST_PRIO_MID);
        if ( rtnval != SW_OK )
        {
            aos_printk ( "pppoe_passthrough_acl_rules: ACL_LIST_CREATE ERROR...(%d)\n",rtnval );
        }

        rtnval = ACL_RULE_ADD ( 0, rule_list_id, 0, 1, &myacl );

        if ( rtnval != SW_OK )
        {
            aos_printk ( "pppoe_passthrough_acl_rules: ACL_RULE_ADD ERROR...(%d)\n", rtnval );
        }
        for (i = S17_LAN_PORT0; i <= S17_LAN_PORT4; i++) {
            if (i != S17_WAN_PORT)
                ACL_LIST_BIND ( 0, rule_list_id, FAL_ACL_DIREC_IN, FAL_ACL_BIND_PORT, i);
        }

    }

    {
        /* lan <- wan, pppoe discovery */
        rule_list_id = S17_ACL_LIST_PPPOE_PASSTHROUGH_WAN_TO_LAN + (isis_pppoe_passthrough - 1) * 2;

        printk ( "creating ACL list_id: %d \n", rule_list_id );

        memset ( &myacl, 0, sizeof ( fal_acl_rule_t ) );
        myacl.rule_type = FAL_ACL_RULE_MAC;
        memcpy ( myacl.dest_mac_val.uc, mac, 6 );
        memcpy ( myacl.dest_mac_mask.uc, mac_mask, 6 );

        myacl.ethtype_val     = 0x8863;
        myacl.ethtype_mask    = 0xffff;

        myacl.ports = (1 << S17_LAN_PORT0) |
                      (1 << S17_LAN_PORT1) | (1 << S17_LAN_PORT2) | (1 << S17_LAN_PORT3);

        memset ( &myacl.field_flg, 0, sizeof ( myacl.field_flg ) );

        FAL_FIELD_FLG_SET ( myacl.field_flg, FAL_ACL_FIELD_MAC_DA );
        FAL_FIELD_FLG_SET ( myacl.field_flg, FAL_ACL_FIELD_MAC_ETHTYPE );

        memset ( &myacl.action_flg, 0, sizeof ( myacl.action_flg ) );

        FAL_ACTION_FLG_SET ( myacl.action_flg, FAL_ACL_ACTION_REDPT );

        rtnval = ACL_LIST_CREATE (0, rule_list_id, S17_ACL_LIST_PRIO_MID);
        if ( rtnval != SW_OK )
        {
            aos_printk ( "pppoe_passthrough_acl_rules: ACL_LIST_CREATE ERROR (%d)\n",rtnval );
            return -1;
        }

        rtnval = ACL_RULE_ADD ( 0, rule_list_id, 0, 1, &myacl );

        if ( rtnval != SW_OK )
        {
            aos_printk ( "pppoe_passthrough_acl_rules: ACL_RULE_ADD ERROR...(%d)\n", rtnval );
            return -1;
        }

        ACL_LIST_BIND ( 0, rule_list_id, FAL_ACL_DIREC_IN, FAL_ACL_BIND_PORT, S17_WAN_PORT);
    }

    {
        /* lan <- wan, pppoe session */
        rule_list_id = S17_ACL_LIST_PPPOE_PASSTHROUGH_WAN_TO_LAN + (isis_pppoe_passthrough - 1) * 2 + 1;

        printk ( "creating ACL list_id: %d \n", rule_list_id );

        memset ( &myacl, 0, sizeof ( fal_acl_rule_t ) );
        myacl.rule_type = FAL_ACL_RULE_MAC;
        memcpy ( myacl.dest_mac_val.uc, mac, 6 );
        memcpy ( myacl.dest_mac_mask.uc, mac_mask, 6 );

        myacl.ethtype_val     = 0x8864;
        myacl.ethtype_mask    = 0xffff;

        myacl.ports = (1 << S17_LAN_PORT0) |
                      (1 << S17_LAN_PORT1) | (1 << S17_LAN_PORT2) | (1 << S17_LAN_PORT3);

        memset ( &myacl.field_flg, 0, sizeof ( myacl.field_flg ) );

        FAL_FIELD_FLG_SET ( myacl.field_flg, FAL_ACL_FIELD_MAC_DA );
        FAL_FIELD_FLG_SET ( myacl.field_flg, FAL_ACL_FIELD_MAC_ETHTYPE );

        memset ( &myacl.action_flg, 0, sizeof ( myacl.action_flg ) );

        FAL_ACTION_FLG_SET ( myacl.action_flg, FAL_ACL_ACTION_REDPT );

        rtnval = ACL_LIST_CREATE ( 0, rule_list_id, S17_ACL_LIST_PRIO_MID);
        if ( rtnval != SW_OK )
        {
            aos_printk ( "pppoe_passthrough_acl_rules: ACL_LIST_CREATE ERROR...(%d)\n",rtnval );
            return -1;
        }

        rtnval = ACL_RULE_ADD ( 0, rule_list_id, 0, 1, &myacl );

        if ( rtnval != SW_OK )
        {
            aos_printk ( "pppoe_passthrough_acl_rules: ACL_RULE_ADD ERROR...(%d)\n", rtnval );
            return -1;
        }

        ACL_LIST_BIND ( 0, rule_list_id, FAL_ACL_DIREC_IN, FAL_ACL_BIND_PORT, S17_WAN_PORT);
    }

    if ( ACL_STATUS_GET ( 0, &val ) == SW_OK )
    {
        if ( val != A_TRUE )
        {
            aos_printk ( "ACL is not yet enabled. Enabling... \n" );
            ACL_STATUS_SET ( 0, A_TRUE );
        }
    }

    /* add the counter */
    isis_pppoe_passthrough++;

    return 0;
}

void icmp_from_wan_acl_rule(void)
{
    fal_acl_rule_t myacl;
    a_uint32_t rtnval;

    printk("WAN ICMP ACL Rules\n");
    memset(&myacl, 0, sizeof(fal_acl_rule_t));

    myacl.rule_type = FAL_ACL_RULE_IP4;
    myacl.ip_proto_val = 0x1;               /* ICMP */
    myacl.ip_proto_mask = 0xff;

    /* dest port : CPU Port */
    myacl.ports = 1 << S17_CPU_PORT;

    FAL_FIELD_FLG_SET (myacl.field_flg, FAL_ACL_FIELD_IP_PROTO);
    FAL_ACTION_FLG_SET (myacl.action_flg, FAL_ACL_ACTION_RDTCPU);

    rtnval = ACL_LIST_CREATE(0, S17_ACL_ICMP_FROM_WAN, S17_ACL_LIST_PRIO_HIGH);

    if (rtnval != SW_OK)
    {
        if(rtnval == SW_ALREADY_EXIST)
        {
            aos_printk ( "athrs17_icmp_from_wan_acl_rules: rules acl list already exists !\n");
        }
        else
        {
            aos_printk ( "athrs17_icmp_from_wan_acl_rules: ACL_LIST_CREATE ERROR (%d)...\n",rtnval );
        }
        return ;
    }

    rtnval = ACL_RULE_ADD(0, S17_ACL_ICMP_FROM_WAN, 0, 1, &myacl);

    if (rtnval != SW_OK)
    {
        ACL_LIST_DESTROY(0, S17_ACL_ICMP_FROM_WAN);
        aos_printk("athrs17_icmp_from_wan_acl_rules: ACL_RULE_ADD ERROR...(%d)\n" ,rtnval);
        return ;
    }

    /* source port : WAN Port */
    ACL_LIST_BIND(0, S17_ACL_ICMP_FROM_WAN, FAL_ACL_DIREC_IN, FAL_ACL_BIND_PORT, S17_WAN_PORT);
}

/*
Once
1. Both lan and wan interface entry have been created
2. Lan size PC's host entry has been created
3. Wan has receive the packet destination IP belong to PC on Lan
Force to redirect it to CPU
*/
void redirect_internal_ip_packets_to_cpu_on_wan_add_acl_rules(uint32_t lan_ip, uint32_t lan_netmask)
{
    fal_acl_rule_t myacl;
    a_uint32_t rtnval;
    a_bool_t val;

    if (get_aclrulemask() & (1 << S17_ACL_LIST_REDIRECT_INTERNAL_IP_TO_CPU_ON_WAN)) return;

    aos_printk("Adding ACL rules %d - %s\n", S17_ACL_LIST_REDIRECT_INTERNAL_IP_TO_CPU_ON_WAN, __func__);

    memset(&myacl, 0, sizeof(fal_acl_rule_t));
    myacl.rule_type = FAL_ACL_RULE_IP4;
    //myacl.ip_proto_val = 0x1;               /* ICMP */
    //myacl.ip_proto_mask = 0xff;
    printk("redirect_internal_ip_packets_to_cpu_on_wan_add_acl_rules, Lan: %u.%u.%u.%u / %u.%u.%u.%u\n"
           ,((unsigned char *)&lan_ip)[0] ,((unsigned char *)&lan_ip)[1]
           ,((unsigned char *)&lan_ip)[2] ,((unsigned char *)&lan_ip)[3]
           ,((unsigned char *)&lan_netmask)[0] ,((unsigned char *)&lan_netmask)[1]
           ,((unsigned char *)&lan_netmask)[2] ,((unsigned char *)&lan_netmask)[3]
          );

    myacl.dest_ip4_val = lan_ip;
    myacl.dest_ip4_mask = lan_netmask;

    /* dest port : CPU Port */
    myacl.ports = 1 << S17_CPU_PORT;

    //FAL_FIELD_FLG_SET (myacl.field_flg, FAL_ACL_FIELD_IP_PROTO);
    FAL_FIELD_FLG_SET(myacl.field_flg, FAL_ACL_FIELD_IP4_DIP);
    FAL_ACTION_FLG_SET (myacl.action_flg, FAL_ACL_ACTION_RDTCPU);

    rtnval = ACL_LIST_CREATE(0, S17_ACL_LIST_REDIRECT_INTERNAL_IP_TO_CPU_ON_WAN, S17_ACL_LIST_PRIO_HIGH);

    if (rtnval != SW_OK)
    {
        if(rtnval == SW_ALREADY_EXIST)
        {
            aos_printk ( "redirect_internal_ip_packets_to_cpu_on_wan_add_acl_rules: rules acl list already exists !\n");
        }
        else
        {
            aos_printk ( "redirect_internal_ip_packets_to_cpu_on_wan_add_acl_rules: ACL_LIST_CREATE ERROR (%d)...\n",rtnval );
        }
        return ;
    }

    rtnval = ACL_RULE_ADD(0, S17_ACL_LIST_REDIRECT_INTERNAL_IP_TO_CPU_ON_WAN, 0, 1, &myacl);

    if (rtnval != SW_OK)
    {
        ACL_LIST_DESTROY(0, S17_ACL_LIST_REDIRECT_INTERNAL_IP_TO_CPU_ON_WAN);
        aos_printk("redirect_internal_ip_packets_to_cpu_on_wan_add_acl_rules: ACL_RULE_ADD ERROR...(%d)\n" ,rtnval);
        return ;
    }

    /* source port : WAN Port */
    ACL_LIST_BIND(0, S17_ACL_LIST_REDIRECT_INTERNAL_IP_TO_CPU_ON_WAN, FAL_ACL_DIREC_IN, FAL_ACL_BIND_PORT, S17_WAN_PORT);

    /* check for the ACL enable bit */
    if (ACL_STATUS_GET(0, &val) == SW_OK)
    {
        if (val != A_TRUE)
        {
            aos_printk("ACL is not yet enabled. Enabling... \n");
            ACL_STATUS_SET(0, A_TRUE);
        }
    }
    set_aclrulemask(S17_ACL_LIST_REDIRECT_INTERNAL_IP_TO_CPU_ON_WAN);
}

void udp_checksum_zero_acl_rule(uint32_t gw_entry_id)
{
    fal_acl_rule_t myacl;
    a_uint32_t rtnval;
    int i;

    if (get_aclrulemask() & (1 << S17_ACL_LIST_UDP0)) return;

    memset(&myacl, 0, sizeof(fal_acl_rule_t));

    /*
        User defined filter ACL
        filter out UDP(0x11) checksum (0x0000) packets
        and set FORCE_L3_MODE ARP_INDEX_OVER_EN=1 ARP_INDEX=default route
        Bind to WAN/LAN port
    */
    aos_printk("UDP checksum adding rule #%d\n", S17_ACL_LIST_UDP0);
    myacl.rule_type = FAL_ACL_RULE_UDF;
    /* set the UDP ACL type as L3, with length 1, offset 9 */
    /* set the UDP ACL type as L4, with length 2, offset 6 */

    for (i = S17_LAN_PORT0; i <= S17_LAN_PORT4; i++) {
        ACL_PORT_UDF_PROFILE_SET(0, i, FAL_ACL_UDF_TYPE_L3, 9, 1);
        ACL_PORT_UDF_PROFILE_SET(0, i, FAL_ACL_UDF_TYPE_L4, 6, 2);
    }

    myacl.udf_len = 3;
    myacl.udf_offset = 0;
    myacl.udf_type = FAL_ACL_UDF_TYPE_L2;
    memset(&myacl.udf_val, 0, sizeof(myacl.udf_val));
    memset(&myacl.udf_mask, 0, sizeof(myacl.udf_mask));
    /* UDF filter to check for UDP checksum 0 packets */
    myacl.udf_val[0] = 0x11;
    myacl.udf_val[1] = 0x00;
    myacl.udf_val[2] = 0x00;
    myacl.udf_val[3] = 0x00;
    myacl.udf_val[4] = 0x00;
    myacl.udf_val[5] = 0x00;
    myacl.udf_val[6] = 0x00;
    myacl.udf_val[7] = 0x00;

    myacl.udf_mask[0] = 0xff;
    myacl.udf_mask[1] = 0xff;
    myacl.udf_mask[2] = 0xff;
    myacl.udf_mask[3] = 0x00;
    myacl.udf_mask[4] = 0x00;
    myacl.udf_mask[5] = 0x00;
    myacl.udf_mask[6] = 0x00;
    myacl.udf_mask[7] = 0x00;

    FAL_FIELD_FLG_SET(myacl.field_flg, FAL_ACL_FIELD_UDF);
    FAL_ACTION_FLG_SET(myacl.action_flg, FAL_ACL_ACTION_ARP_EN);

    myacl.arp_ptr = gw_entry_id;
    myacl.policy_fwd = FAL_ACL_POLICY_ROUTE;


    rtnval = ACL_LIST_CREATE(0, S17_ACL_LIST_UDP0, S17_ACL_LIST_PRIO_HIGH);
    if (rtnval != SW_OK)
    {
        aos_printk("#%d, ACL_LIST_CREATE ERROR...%d\n",S17_ACL_LIST_UDP0,rtnval);
        return;
    }

    rtnval = ACL_RULE_ADD(0, S17_ACL_LIST_UDP0, 0, 1, &myacl);
    if (rtnval != SW_OK)
    {
        ACL_LIST_DESTROY(0, S17_ACL_LIST_UDP0);
        aos_printk("#%d, ACL_RULE_ADD ERROR...%d\n",S17_ACL_LIST_UDP0,rtnval);
        return;
    }

    for (i = S17_LAN_PORT0; i <= S17_LAN_PORT4; i++) {
        ACL_LIST_BIND(0, S17_ACL_LIST_UDP0, FAL_ACL_DIREC_IN, FAL_ACL_BIND_PORT, i);
    }

    set_aclrulemask(S17_ACL_LIST_UDP0);
}


void dscp_remap_acl_rule(uint32_t ori_dscp, uint32_t dscp)
{
    fal_acl_rule_t myacl;
    a_uint32_t rtnval;
    uint32_t ori_tos, tos_val;
    a_bool_t val;
    int i;

    ori_tos = ori_dscp << 2;
    tos_val = dscp << 2;

    if (get_aclrulemask() & (1 << S17_ACL_LIST_DSCP_REMAP))
        return;
        
    aos_printk("Adding ACL rules %d - %s\n", S17_ACL_LIST_DSCP_REMAP, __func__);
    memset(&myacl, 0, sizeof(fal_acl_rule_t));
    myacl.rule_type = FAL_ACL_RULE_IP4;

    aos_printk("Remap DSCP value from %d to %d\n", ori_dscp, dscp);
    myacl.ip_dscp_val = ori_tos;
    myacl.ip_dscp_mask = 0xff;

    FAL_FIELD_FLG_SET(myacl.field_flg, FAL_ACL_FIELD_IP_DSCP);
    FAL_ACTION_FLG_SET (myacl.action_flg, FAL_ACL_ACTION_REMARK_DSCP);

    /* Action : new DSCP value */
    myacl.dscp = tos_val;

    rtnval = ACL_LIST_CREATE(0, S17_ACL_LIST_DSCP_REMAP, S17_ACL_LIST_PRIO_HIGH);

    if (rtnval != SW_OK)
    {
        if(rtnval == SW_ALREADY_EXIST)
        {
            //aos_printk ( "%s: rules acl list already exists !\n", __FUNCTION__);
        } else {
            aos_printk ( "%s: ACL_LIST_CREATE ERROR (%d)...\n", __FUNCTION__, rtnval );
        }
        return ;
    }

    rtnval = ACL_RULE_ADD(0, S17_ACL_LIST_DSCP_REMAP, 0, 1, &myacl);

    if (rtnval != SW_OK)
    {
        ACL_LIST_DESTROY(0, S17_ACL_LIST_DSCP_REMAP);
        aos_printk("%s: ACL_RULE_ADD ERROR...(%d)\n", __FUNCTION__, rtnval);
        return ;
    }

    /* source port : LAN Ports */
    for (i = S17_LAN_PORT0; i <= S17_LAN_PORT4; i++) {
        if (i != S17_WAN_PORT)
            ACL_LIST_BIND(0, S17_ACL_LIST_DSCP_REMAP, FAL_ACL_DIREC_IN, FAL_ACL_BIND_PORT, i);
    }

    /* check for the ACL enable bit */
    if (ACL_STATUS_GET(0, &val) == SW_OK)
    {
        if (val != A_TRUE)
        {
            aos_printk("ACL is not yet enabled. Enabling... \n");
            ACL_STATUS_SET(0, A_TRUE);
        }
    }
    set_aclrulemask(S17_ACL_LIST_DSCP_REMAP);
}

