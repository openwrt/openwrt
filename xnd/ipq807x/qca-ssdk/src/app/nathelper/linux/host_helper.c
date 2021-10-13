/*
 * Copyright (c) 2012, 2015, 2017-2018, The Linux Foundation. All rights reserved.
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
#include <linux/version.h>
#include <generated/autoconf.h>
#else
#include <linux/autoconf.h>
#endif
#include <linux/if_arp.h>
#include <linux/netdevice.h>
#include <linux/netfilter_arp.h>
#include <linux/inetdevice.h>
#include <linux/netfilter_ipv4/ip_tables.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <linux/tcp.h>
#include <linux/icmp.h>
#ifdef KVER32
#include <linux/export.h>
#endif
#include <net/netfilter/nf_conntrack.h>
#if defined(CONFIG_VLAN_8021Q) || defined(CONFIG_VLAN_8021Q_MODULE)
#include <linux/if_vlan.h>
#endif
#if defined (CONFIG_BRIDGE)
#include <../net/bridge/br_private.h>
#endif
#include <linux/ppp_defs.h>
#include <linux/filter.h>
#include <linux/if_pppox.h>
#include <linux/if_ppp.h>
#include <linux/ppp_channel.h>
#include <linux/ppp-comp.h>
#include <net/sock.h>
#include <net/route.h>
#include <net/netevent.h>
#include <net/ipv6.h>
#include <net/ip_fib.h>
#include "hsl_api.h"
#include "fal_nat.h"
#include "fal_ip.h"
#include "fal_fdb.h"
#include "hsl.h"
#include "nat_helper.h"
#include "napt_acl.h"
#include "lib/nat_helper_hsl.h"
#include "lib/nat_helper_dt.h"
#include "hsl_shared_api.h"
#include <net/addrconf.h>


#undef CONFIG_IPV6_HWACCEL


#ifdef CONFIG_IPV6_HWACCEL
#include <net/ndisc.h>
#include <net/neighbour.h>
#include <net/netevent.h>
#include <net/ipv6.h>
#include <net/ip6_route.h>
#include <linux/ipv6.h>
#include <linux/netfilter_ipv6.h>
#endif

//#define AP136_QCA_HEADER_EN 1
#define MAC_LEN 6
#define IP_LEN 4
#define ARP_HEADER_LEN 8

#define ARP_ENTRY_MAX 128

#define DESS_CHIP_VER	0x14
#define HOST_PREFIX_MAX	31

/* P6 is used by loop dev. */
#define S17_P6PAD_MODE_REG_VALUE 0x01000000

#define MULTIROUTE_WR

extern struct net init_net;

//char *nat_lan_dev_list = "eth0.1";
char nat_lan_dev_list[IFNAMSIZ*4] = "br-lan eth0.1";
char nat_wan_dev_list[IFNAMSIZ*5] = "eth0.2";

char nat_wan_port = 0x20;
int setup_wan_if = 0;
int setup_lan_if=0;

#define NAT_LAN_DEV_VID 1
#define NAT_WAN_DEV_VID 2

uint32_t nat_lan_vid  = NAT_LAN_DEV_VID;
uint32_t nat_wan_vid = NAT_WAN_DEV_VID;


static int wan_fid = 0xffff;
static fal_pppoe_session_t pppoetbl = {0};
static uint32_t pppoe_gwid = 0;
static char nat_bridge_dev[IFNAMSIZ*4] = "br-lan";
static uint8_t lanip[4] = {0}, lanipmask[4] = {0}, wanip[4] = {0};
#ifdef CONFIG_IPV6_HWACCEL
static struct in6_addr wan6ip = IN6ADDR_ANY_INIT;
static struct in6_addr lan6ip = IN6ADDR_ANY_INIT;
#endif

extern int nat_chip_ver;


#ifdef ISISC
struct ipv6_default_route_binding
{
    struct in6_addr next_hop;
    uint32_t nh_entry_id;
};
#endif

#ifdef MULTIROUTE_WR
#define MAX_HOST 8
struct wan_next_hop
{
    u_int32_t host_ip;
    u_int32_t entry_id;
    u_int32_t in_acl;
    u_int32_t in_use;
    u_int8_t  host_mac[6];
};
static struct net_device *multi_route_indev = NULL;
static struct wan_next_hop wan_nh_ent[MAX_HOST] = {{0}};

#define NAT_BACKGROUND_TASK

#ifdef NAT_BACKGROUND_TASK
#define NAT_HELPER_MSG_MAX 512

struct bg_ring_buf_cb {
	unsigned int read_idx;
	unsigned int write_idx;
	unsigned int num;
	unsigned int full_flag;
	struct nat_helper_bg_msg *buf;
};

struct bg_task_cb {
	/*struct semaphore bg_sem;  */   /*trigger thread work*/
	spinlock_t bg_lock;           /*ring buf access protect*/
	/*struct task_struct *bg_task;*/
	struct workqueue_struct *nat_wq;
	struct bg_ring_buf_cb ring;
};

enum{
	NAT_HELPER_ARP_ADD_MSG = 0,
	NAT_HELPER_ARP_DEL_MSG,
	NAT_HELPER_IPV6_MSG
};

struct arp_in_msg {
	uint8_t mac[ETH_ALEN];
	char name[IFNAMSIZ]; /* device name */
	uint32_t ip;
	struct net_device *in;
};

struct ipv6_msg {
	struct sk_buff *skb;
	struct net_device *in;
};


struct nat_helper_bg_msg {
	struct work_struct work;
	uint32_t msg_type;
	uint16_t sub_type;
	uint16_t reservd;
	union {
		struct arp_in_msg arp_in;
		struct ipv6_msg   ipv6;
	};
};


struct bg_task_cb task_cb;

int bg_ring_buf_write(struct nat_helper_bg_msg msg)
{
	unsigned int idx = 0;

	spin_lock_bh(&task_cb.bg_lock);
	
	if((task_cb.ring).full_flag &&
		((task_cb.ring).read_idx == (task_cb.ring).write_idx)) {
		HNAT_PRINTK("ring buf is full!\n");
		spin_unlock_bh(&task_cb.bg_lock);
		return -1;
	}
	msg.work = (task_cb.ring).buf[(task_cb.ring).write_idx].work;
	(task_cb.ring).buf[(task_cb.ring).write_idx] = msg;
	idx = (task_cb.ring).write_idx;

	(task_cb.ring).write_idx = ((task_cb.ring).write_idx+1)%NAT_HELPER_MSG_MAX;
	if(task_cb.ring.read_idx == task_cb.ring.write_idx)
		task_cb.ring.full_flag = 1;

	spin_unlock_bh(&task_cb.bg_lock);
	queue_work(task_cb.nat_wq, &(task_cb.ring).buf[idx].work);

	return 0;
}

int bg_ring_buf_read(struct nat_helper_bg_msg *msg)
{
	spin_lock_bh(&task_cb.bg_lock);
	task_cb.ring.read_idx = (task_cb.ring.read_idx+1)%NAT_HELPER_MSG_MAX;
	task_cb.ring.full_flag = 0;
	spin_unlock_bh(&task_cb.bg_lock);

	return 0;
}



#endif

a_uint32_t nat_helper_wan_port_get(void)
{
	a_uint32_t i = 0;

	for (i = 0; i < 6; i ++) {
		if ((nat_wan_port >> i) & 1)
			break;
	}

	return i;
}

static int wan_nh_get(u_int32_t host_ip)
{
    int i;

    host_ip = htonl(host_ip);

    for (i=0; i<MAX_HOST; i++)
    {
        if ((wan_nh_ent[i].host_ip != 0) && !memcmp(&wan_nh_ent[i].host_ip, &host_ip, 4))
        {
            // printk("%s %d\n", __FUNCTION__, __LINE__);
            // if ((wan_nh_ent[i].entry_id != 0) && (wan_nh_ent[i].in_acl != 1))
            if (wan_nh_ent[i].in_acl != 1)
            {
                wan_nh_ent[i].in_acl = 1;

                return i;
            }
            // printk("%s %d\n", __FUNCTION__, __LINE__);
        }
        printk("%s %d wan_nh_ent 0x%08x host_ip 0x%08x\n", __FUNCTION__, __LINE__, wan_nh_ent[i].host_ip, host_ip);
    }

    return -1;
}

static void wan_nh_add(u_int8_t *host_ip , u_int8_t *host_mac, u_int32_t id)
{
    int i;

    for( i = 0 ; i < MAX_HOST ; i++ )
    {
        if((wan_nh_ent[i].host_ip != 0) && !memcmp(&wan_nh_ent[i].host_ip, host_ip, 4))
        {
            if (host_mac == NULL) break;

            if(!memcmp(&wan_nh_ent[i].host_mac, host_mac,6))
                return;
            else
                break ;
        }

        if(wan_nh_ent[i].host_ip == 0)
            break;
    }

    if (i < MAX_HOST)
    {
        if ((wan_nh_ent[i].in_use) && (wan_nh_ent[i].in_acl)) return;

        memcpy(&wan_nh_ent[i].host_ip, host_ip, 4);
        if (host_mac != NULL)
        {
            memcpy(wan_nh_ent[i].host_mac, host_mac, 6);
            wan_nh_ent[i].entry_id = id;
            if ((wan_nh_ent[i].in_use) && !(wan_nh_ent[i].in_acl))
            {
                droute_add_acl_rules(*(uint32_t *)&lanip, *(uint32_t *)&lanipmask, id);
                /* set the in_acl flag */
                wan_nh_ent[i].in_acl = 1;
            }
        }
        else
        {
            /* set the in_use flag */
            wan_nh_ent[i].in_use = 1;
        }
        HNAT_PRINTK("%s: ip %08x (%d)\n" ,__func__, wan_nh_ent[i].host_ip, i);
    }
}

uint32_t get_next_hop(uint32_t daddr, uint32_t saddr)
{
    struct fib_result res;
#ifdef KVER32
    struct flowi4 fl =
    {
        .flowi4_iif =  multi_route_indev->ifindex,
        .flowi4_mark = 0,
        .flowi4_tos = 0,
        .flowi4_scope = RT_SCOPE_UNIVERSE,
        .daddr = htonl(daddr),
        .saddr = htonl(saddr),
    };
#else
    struct flowi fl = { .nl_u = { .ip4_u =
            {
                .daddr = daddr,
                .saddr = saddr,
                .tos = 0,
                .scope = RT_SCOPE_UNIVERSE,
            }
        },
        .mark = 0,
        .iif = multi_route_indev->ifindex
    };
#endif
    struct net    * net = dev_net(multi_route_indev);
    struct fib_nh *mrnh = NULL;
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4,4,0))
    if (fib_lookup(net, &fl, &res) != 0)
#else
   if (fib_lookup(net, &fl, &res,0) != 0)
#endif
    {
        return 0;
    }
    else
    {
        mrnh = res.fi->fib_nh;
        if (NULL == mrnh)
        {
            return 0;
        }
    }

    return ntohl(mrnh->nh_gw);
}

uint32_t napt_set_default_route(fal_ip4_addr_t dst_addr, fal_ip4_addr_t src_addr)
{
    sw_error_t rv;

    /* search for the next hop (s) */
    if (!(get_aclrulemask() & (1 << S17_ACL_LIST_DROUTE)))
    {
        if (multi_route_indev && \
                (nf_athrs17_hnat_wan_type != NF_S17_WAN_TYPE_PPPOE) &&
                (nf_athrs17_hnat_wan_type != NF_S17_WAN_TYPE_PPPOES0))
        {
            uint32_t next_hop = get_next_hop(dst_addr, src_addr);

            HNAT_PRINTK("Next hop: %08x\n", next_hop);
            if (next_hop != 0)
            {
                fal_host_entry_t arp_entry;

                memset(&arp_entry, 0, sizeof(arp_entry));
                arp_entry.ip4_addr = next_hop;
                arp_entry.flags = FAL_IP_IP4_ADDR;
                rv = IP_HOST_GET(0, FAL_IP_ENTRY_IPADDR_EN, &arp_entry);
                if (rv != SW_OK)
                {
                    printk("%s: IP_HOST_GET error... (non-existed host: %08x?) \n", __func__, next_hop);
                    /* add into the nh_ent */
                    wan_nh_add((u_int8_t *)&next_hop, (u_int8_t *)NULL, 0);
                }
                else
                {
                    if (wan_nh_get(next_hop) != -1)
                        droute_add_acl_rules(*(uint32_t *)&lanip, *(uint32_t *)&lanipmask, arp_entry.entry_id);
                    else
                        printk("%s %d\n", __FUNCTION__, __LINE__);
                }
            }
            else
            {
                HNAT_PRINTK("no need to set the default route... \n");
                // set_aclrulemask (S17_ACL_LIST_DROUTE);
            }
        }
        else
        {
            printk("multi_route_indev %pK nf_athrs17_hnat_wan_type %d\n", multi_route_indev, nf_athrs17_hnat_wan_type);
        }
    }
    /* end next hop (s) */

    return SW_OK;
}
#endif /* MULTIROUTE_WR */

void qcaswitch_hostentry_flush(void)
{
    fal_host_entry_t hostentry;
    sw_error_t ret;

    do
    {
        memset(&hostentry, 0, sizeof(fal_host_entry_t));
        hostentry.entry_id = FAL_NEXT_ENTRY_FIRST_ID;
        ret = IP_HOST_NEXT (0, FAL_IP_ENTRY_ID_EN, &hostentry);
        if (SW_OK == ret)
        {
            IP_HOST_DEL(0, FAL_IP_ENTRY_IPADDR_EN, &hostentry);
        }
    }while (SW_OK == ret);

    return;
}

#ifdef CONFIG_IPV6_HWACCEL /* only for S17c */
static struct in6_addr* get_ipv6_default_gateway(void)
{
    /* ip_route_output_key can't return correct default nexhop
     * routes are less than 4 and it only searches in route
     * hash, not in fib, so use fib_lookup.
     */
    struct in6_addr *ip6addr = NULL;
    struct in6_addr des_addr = IN6ADDR_ANY_INIT;
    struct rt6_info *rt = rt6_lookup(&init_net, &des_addr, NULL, 0, 0);

    if (rt)
    {
        ip6addr = &rt->rt6i_gateway;
    }

    return ip6addr;
}

static int add_pppoev6_host_entry(void)
{
    struct in6_addr local_lan6ip = IN6ADDR_ANY_INIT;
    unsigned long  flags;
    int ppp_sid, ppp_sid2;
    unsigned char ppp_peer_mac[ETH_ALEN];
    unsigned char ppp_peer_mac2[ETH_ALEN];
    a_uint32_t ppp_peer_ip = 0;
    int wvid;
    fal_host_entry_t nh_arp_entry;
    sw_error_t rv;
    a_uint32_t droute_entry_id = 0;
    a_bool_t ena;
    static fal_pppoe_session_t pppoev6_sid_table = {0};
    struct  in6_addr *next_hop;

    local_irq_save(flags);
    memcpy(&local_lan6ip, &lan6ip, sizeof(struct in6_addr));
    ppp_sid2 = nf_athrs17_hnat_ppp_id2;
    ppp_sid = nf_athrs17_hnat_ppp_id;
    ppp_peer_ip = nf_athrs17_hnat_ppp_peer_ip;
    memcpy(ppp_peer_mac, nf_athrs17_hnat_ppp_peer_mac, ETH_ALEN);
    memcpy(ppp_peer_mac2, nf_athrs17_hnat_ppp_peer_mac2, ETH_ALEN);
    wvid = wan_fid;
    local_irq_restore(flags);

    if (NF_S17_WAN_TYPE_PPPOE != nf_athrs17_hnat_wan_type)
    {
        return SW_BAD_STATE;
    }

    if (__ipv6_addr_type(&local_lan6ip) == IPV6_ADDR_ANY)
    {
        /* Cannot get lanip6 successfully. */
        return SW_BAD_STATE;
    }
    if (0xffff == wvid)
    {
        printk("%s: Cannot get WAN vid!\n", __FUNCTION__);
        return SW_FAIL;
    }

    if (0 == nf_athrs17_hnat_ppp_peer_ip)
    {
        return SW_FAIL;
    }

    next_hop = get_ipv6_default_gateway();
    if (NULL == next_hop)
    {
        printk("No IPv6 Gateway!\n");
        return SW_BAD_STATE;
    }

    if (0 != ppp_sid)
    {
        if ((ppp_sid == ppp_sid2)||(0 == ppp_sid2)) /* v4 and v6 have the same session id */
        {
            memset(&nh_arp_entry, 0, sizeof(nh_arp_entry));
            nh_arp_entry.ip4_addr = ppp_peer_ip;
            nh_arp_entry.flags = FAL_IP_IP4_ADDR;
            rv = IP_HOST_GET(0, FAL_IP_ENTRY_IPADDR_EN, &nh_arp_entry);
            if (rv != SW_OK)
            {
                printk("%s: IP_HOST_GET error (0x%08x)\n", __func__, ppp_peer_ip);
                if (PPPOE_STATUS_GET(0, &ena) != SW_OK)
                {
                    if (!ena)
                    {
                        if (PPPOE_STATUS_SET(0, A_TRUE) != SW_OK)
                        {
                            aos_printk("Cannot enable the PPPoE mode\n");
                            return SW_FAIL;
                        }
                    }
                }
                pppoev6_sid_table.session_id = ppp_sid;
                pppoev6_sid_table.multi_session = 1;
                pppoev6_sid_table.uni_session = 1;
                pppoev6_sid_table.entry_id = 0;
                /* set the PPPoE edit reg (0x2200), and PPPoE session reg (0x5f000) */
                rv = PPPOE_SESSION_TABLE_ADD(0, &pppoev6_sid_table);
                if (rv == SW_OK)
                {
                    a_int32_t a_entry_id = -1;

                    PPPOE_SESSION_ID_SET(0, pppoev6_sid_table.entry_id, pppoev6_sid_table.session_id);
                    aos_printk("pppoe session: %d, entry_id: %d\n",
                               pppoev6_sid_table.session_id, pppoev6_sid_table.entry_id);
                    /* create the peer host ARP entry */
                    a_entry_id = arp_hw_add(S17_WAN_PORT, wan_fid, (void *)&ppp_peer_ip, (void *)ppp_peer_mac, 0);
                    if (a_entry_id >= 0) /* hostentry creation okay */
                    {
                        rv = IP_HOST_PPPOE_BIND(0, a_entry_id, pppoev6_sid_table.entry_id, A_TRUE);
                        if ( rv != SW_OK)
                        {
                            aos_printk("IP_HOST_PPPOE_BIND failed (entry: %d, rv: %d)... \n",
                                       a_entry_id, rv);
                            PPPOE_SESSION_TABLE_DEL(0, &pppoev6_sid_table);
                            return SW_FAIL;
                        }
                        droute_entry_id = a_entry_id;
                    }
                    else
                    {
                        PPPOE_SESSION_TABLE_DEL(0, &pppoev6_sid_table);
                        return SW_FAIL;
                    }
                }
                else
                {
                    aos_printk("PPPoE session add failed.. (id: %d)\n",
                               pppoev6_sid_table.session_id);
                    aos_printk("rv: %d\n", rv);
                    return SW_FAIL;
                }
            }
            else
            {
                droute_entry_id = nh_arp_entry.entry_id;
            }
            ipv6_droute_add_acl_rules(&local_lan6ip, droute_entry_id);
        }
        else /* Not the same session id */
        {
            if (PPPOE_STATUS_GET(0, &ena) != SW_OK)
            {
                if (!ena)
                {
                    if (PPPOE_STATUS_SET(0, A_TRUE) != SW_OK)
                    {
                        aos_printk("Cannot enable the PPPoE mode\n");
                        return SW_FAIL;
                    }
                }
            }
            memset(&nh_arp_entry, 0, sizeof(nh_arp_entry));
            memcpy((void *)&nh_arp_entry.ip6_addr, (void *)next_hop, sizeof(nh_arp_entry.ip6_addr));
            nh_arp_entry.flags = FAL_IP_IP6_ADDR;
            rv = IP_HOST_GET(0, FAL_IP_ENTRY_IPADDR_EN, &nh_arp_entry);
            if (rv != SW_OK)
            {
                /* ARP alread setup. */
                return SW_OK;
            }
            pppoev6_sid_table.session_id = ppp_sid2;
            pppoev6_sid_table.multi_session = 1;
            pppoev6_sid_table.uni_session = 1;
            pppoev6_sid_table.entry_id = 0;
            /* set the PPPoE edit reg (0x2200), and PPPoE session reg (0x5f000) */
            rv = PPPOE_SESSION_TABLE_ADD(0, &pppoev6_sid_table);
            if (rv == SW_OK)
            {
                a_int32_t a_entry_id = -1;

                PPPOE_SESSION_ID_SET(0, pppoev6_sid_table.entry_id, pppoev6_sid_table.session_id);
                aos_printk("pppoe session: %d, entry_id: %d\n",
                           pppoev6_sid_table.session_id, pppoev6_sid_table.entry_id);
                /* create the peer host ARP entry */
                a_entry_id = arp_hw_add(S17_WAN_PORT, wan_fid, (void *)next_hop, ppp_peer_mac2, 1);
                if (a_entry_id >= 0) /* hostentry creation okay */
                {
                    rv = IP_HOST_PPPOE_BIND(0, a_entry_id, pppoev6_sid_table.entry_id, A_TRUE);
                    if ( rv != SW_OK)
                    {
                        aos_printk("IP_HOST_PPPOE_BIND failed (entry: %d, rv: %d)... \n",
                                   a_entry_id, rv);
                        PPPOE_SESSION_TABLE_DEL(0, &pppoev6_sid_table);
                        return SW_FAIL;
                    }
                    droute_entry_id = a_entry_id;
                }
                else
                {
                    PPPOE_SESSION_TABLE_DEL(0, &pppoev6_sid_table);
                    return SW_FAIL;
                }
            }
            else
            {
                aos_printk("PPPoE session add failed.. (id: %d)\n",
                           pppoev6_sid_table.session_id);
                aos_printk("rv: %d\n", rv);
                return SW_FAIL;
            }
            ipv6_droute_add_acl_rules(&local_lan6ip, droute_entry_id);
        }
    }

    return SW_OK;
}

uint32_t napt_set_ipv6_default_route(void)
{
    sw_error_t rv;
    static a_bool_t ipv6_droute_setup = A_FALSE;
    static struct ipv6_default_route_binding ipv6_droute_bind = {IN6ADDR_ANY_INIT,0};
    struct in6_addr local_lan6ip = IN6ADDR_ANY_INIT;
    unsigned long  flags;

	if (((nat_chip_ver&0xffff)>>8) == DESS_CHIP_VER)
		return SW_OK;

    /* search for the next hop (s)*/
    if (NF_S17_WAN_TYPE_IP == nf_athrs17_hnat_wan_type)
    {
        struct  in6_addr *next_hop = get_ipv6_default_gateway();

        // printk("IPv6 next hop: %pI6\n", next_hop);

        if (next_hop != NULL)
        {
            fal_host_entry_t ipv6_neigh_entry;

            if (__ipv6_addr_type(next_hop) == IPV6_ADDR_LINKLOCAL)
                return SW_OK;

            local_irq_save(flags);
            memcpy(&local_lan6ip, &lan6ip, sizeof(struct in6_addr));
            local_irq_restore(flags);

            memset(&ipv6_neigh_entry, 0, sizeof(ipv6_neigh_entry));
            memcpy(&ipv6_neigh_entry.ip6_addr, next_hop, 16);
            ipv6_neigh_entry.flags = FAL_IP_IP6_ADDR;
            rv = IP_HOST_GET(0, FAL_IP_ENTRY_IPADDR_EN, &ipv6_neigh_entry);
            if ((rv != SW_OK)||(__ipv6_addr_type(&local_lan6ip) == IPV6_ADDR_ANY))
            {
                if (ipv6_droute_setup)
                {
                    ipv6_droute_del_acl_rules();
                    memset(&ipv6_droute_bind, 0, sizeof(ipv6_droute_bind));
                    ipv6_droute_setup = A_FALSE;
                }
            }
            else
            {
                if (ipv6_droute_setup)
                {
                    if (!ipv6_addr_equal(&ipv6_droute_bind.next_hop, next_hop) ||
                            ipv6_droute_bind.nh_entry_id != ipv6_neigh_entry.entry_id)
                    {
                        ipv6_droute_del_acl_rules();
                    }
                }
                ipv6_droute_bind.next_hop = *next_hop;
                ipv6_droute_bind.nh_entry_id = ipv6_neigh_entry.entry_id;

                ipv6_droute_add_acl_rules(&local_lan6ip, ipv6_neigh_entry.entry_id);
                ipv6_droute_setup = A_TRUE;
            }
        }
        else
        {
            if (ipv6_droute_setup)
            {
                ipv6_droute_del_acl_rules();
                memset(&ipv6_droute_bind, 0, sizeof(ipv6_droute_bind));
                ipv6_droute_setup = A_FALSE;
            }
        }
    }
    else if (NF_S17_WAN_TYPE_IP == nf_athrs17_hnat_wan_type)
    {
        add_pppoev6_host_entry();
    }

    return SW_OK;
}
#endif /* ifdef CONFIG_IPV6_HWACCEL */

static sw_error_t hnat_add_host_route(
		fal_ip4_addr_t ip_addr,
		uint32_t prefix_len)
{
	fal_host_route_t host_route;

	memset(&host_route, 0, sizeof(fal_host_route_t));

	fal_ip_host_route_get(0, 0, &host_route);
	if ((host_route.route_addr.ip4_addr == ip_addr) &&
	    (host_route.prefix_length == prefix_len) &&
	    host_route.valid)
	    return SW_OK;

	host_route.valid = A_TRUE;
	host_route.vrf_id = 0;
	host_route.ip_version = 0;
	host_route.route_addr.ip4_addr = ip_addr;
	host_route.prefix_length = prefix_len;

	return fal_ip_host_route_set(0, 0, &host_route);
}
static sw_error_t setup_interface_entry(char *list_if, int is_wan)
{
    char temp[IFNAMSIZ*4]; /* Max 4 interface entries right now. */
    char *dev_name, *list_all;
    struct net_device *nat_dev;
    struct in_device *in_device_lan = NULL, *in_device_wan = NULL;
    uint8_t *devmac, if_mac_addr[MAC_LEN];
    char *br_name;
    uint32_t vid = 0;
    sw_error_t setup_error;
    uint32_t ipv6 = 0;

    memcpy(temp, list_if, strlen(list_if)+1);
    list_all = temp;

    setup_error = SW_FAIL;
    rcu_read_lock();
    while ((dev_name = strsep(&list_all, " ")) != NULL)
    {
        nat_dev = dev_get_by_name(&init_net, dev_name);
        if (NULL == nat_dev)
        {
            // printk("%s: Cannot get device %s by name!\n", __FUNCTION__, dev_name);
            setup_error = SW_FAIL;
            continue;
        }
#if defined (CONFIG_BRIDGE)
#ifdef KVER32
        if (NULL != br_port_get_rcu(nat_dev)) /* under bridge interface. */
        {
            /* Get bridge interface name */
            br_name = (char *)(br_port_get_rcu(nat_dev)->br->dev->name);
            if (!is_wan) {
                strlcat (nat_lan_dev_list, " ", sizeof(nat_lan_dev_list));
                strlcat (nat_lan_dev_list, br_name, sizeof(nat_lan_dev_list));
            }
            /* Get dmac */
            devmac = (uint8_t *)(br_port_get_rcu(nat_dev)->br->dev->dev_addr);
        }
#else
        if (NULL != nat_dev->br_port) /* under bridge interface. */
        {
            /* Get bridge interface name */
            br_name = (char *)nat_dev->br_port->br->dev->name;
            //memcpy (nat_bridge_dev, br_name, sizeof(br_name));
            strcat (nat_lan_dev_list, " ");
            strcat (nat_lan_dev_list, br_name);
            /* Get dmac */
            devmac = (uint8_t *)nat_dev->br_port->br->dev->dev_addr;
        }
#endif
        else
#endif /* CONFIG_BRIDGE */
        {
            devmac = (uint8_t *)nat_dev->dev_addr;
        }
        /* get vid */
#if 0
#if defined(CONFIG_VLAN_8021Q) || defined(CONFIG_VLAN_8021Q_MODULE)
        vid = vlan_dev_vlan_id(nat_dev);
#else
        vid = 0;
#endif
#endif
		if(is_wan)
			vid = nat_wan_vid;
		else
			vid = nat_lan_vid;
#ifdef CONFIG_IPV6_HWACCEL
        ipv6 = 1;
        if (is_wan)
        {
            wan_fid = vid;
        }
#else
        ipv6 = 0;
        if (is_wan)
        {
            if (NF_S17_WAN_TYPE_PPPOEV6 == nf_athrs17_hnat_wan_type)
                ipv6 = 1;
            wan_fid = vid;
        }
#endif
#ifdef ISISC
        if (0 == is_wan) /* Not WAN -> LAN */
        {
            /* Setup private and netmask as soon as possible */
            if (br_port_get_rcu(nat_dev)) /* under bridge interface. */
            {
                in_device_lan = (struct in_device *) (br_port_get_rcu(nat_dev)->br->dev->ip_ptr);
            }
            else
            {
                in_device_lan = (struct in_device *) nat_dev->ip_ptr;
            }

            if ((in_device_lan) && (in_device_lan->ifa_list))
            {
                nat_hw_prv_mask_set((a_uint32_t)(in_device_lan->ifa_list->ifa_mask));
                nat_hw_prv_base_set((a_uint32_t)(in_device_lan->ifa_list->ifa_address));
#ifndef KVER32
                printk("Set private base 0x%08x for %s\n", (a_uint32_t)(in_device_lan->ifa_list->ifa_address), nat_dev->br_port->br->dev->name);
#endif
                memcpy(&lanip, (void *)&(in_device_lan->ifa_list->ifa_address), 4); /* copy Lan port IP. */
		memcpy(&lanipmask, (void *)&(in_device_lan->ifa_list->ifa_mask), 4);
#ifndef ISISC
                redirect_internal_ip_packets_to_cpu_on_wan_add_acl_rules((a_uint32_t)(in_device_lan->ifa_list->ifa_address),
                                                                            (a_uint32_t)(in_device_lan->ifa_list->ifa_mask));
#endif
            }

            if(setup_lan_if) {
                dev_put(nat_dev);
                rcu_read_unlock();
                return SW_OK;
            } else {
                setup_lan_if = 1;
            }
        }
#endif
        if (1 == is_wan) {
            if (br_port_get_rcu(nat_dev)) {
                in_device_wan = (struct in_device *) (br_port_get_rcu(nat_dev)->br->dev->ip_ptr);
            } else {
                in_device_wan = (struct in_device *) nat_dev->ip_ptr;
            }
            if((nf_athrs17_hnat_wan_type == NF_S17_WAN_TYPE_IP) &&
               (in_device_wan) && (in_device_wan->ifa_list))
            {
                a_uint32_t index;

                if (((nat_chip_ver&0xffff) >> 8) == DESS_CHIP_VER) {
                    a_uint32_t ip, len;

                    ip = in_device_wan->ifa_list->ifa_address & in_device_wan->ifa_list->ifa_mask;
                    ip = ntohl(ip);
                    len = 32 - ffs(ntohl(in_device_wan->ifa_list->ifa_mask));
                    hnat_add_host_route(ip, len);
                }
                nat_hw_pub_ip_add(ntohl((a_uint32_t)(in_device_wan->ifa_list->ifa_address)),
                                                &index);
                HNAT_PRINTK("pubip add 0x%x\n", (a_uint32_t)(in_device_wan->ifa_list->ifa_address));
        	}
        }
        memcpy(if_mac_addr, devmac, MAC_LEN);
        devmac = if_mac_addr;
        dev_put(nat_dev);

        if(if_mac_add(devmac, vid, ipv6) != 0)
        {
            setup_error = SW_FAIL;
            continue;
        }
        else
        {
            setup_error = SW_OK;
            break;
        }
    }

    rcu_read_unlock();
    return setup_error;
}

static void setup_dev_list(void)
{
	fal_vlan_t entry;
	uint32_t tmp_vid = 0xffffffff;

	/*get the vlan entry*/
	while(1) {
		if(SW_OK != VLAN_NEXT(0, tmp_vid, &entry))
			break;
		tmp_vid = entry.vid;
		if(tmp_vid != 0) {
			if(entry.mem_ports & nat_wan_port) {
				/*wan port*/
				HNAT_PRINTK("wan port vid:%d\n", tmp_vid);
				nat_wan_vid = tmp_vid;
				snprintf(nat_wan_dev_list, IFNAMSIZ*5,
					 "eth0.%d eth0 pppoe-wan erouter0 br-wan",
					 tmp_vid);
			} else {
				/*lan port*/
				HNAT_PRINTK("lan port vid:%d\n", tmp_vid);
				nat_lan_vid = tmp_vid;
				snprintf(nat_lan_dev_list, IFNAMSIZ*4, "eth0.%d eth1 eth1.%d br-lan",
					tmp_vid, tmp_vid);
			}
		}
	}
}

static int setup_all_interface_entry(void)
{
    //static int setup_lan_if=0;
    static int setup_default_vid = 0;
    int i = 0;

	setup_dev_list();

    if (0 == setup_default_vid)
    {
        for (i=0; i<7; i++) /* For AR8327/AR8337, only 7 port */
        {
#ifdef NAT_TODO /* need to implement here */
            PORTVLAN_ROUTE_DEFV_SET(0, i);
#endif
        }
        setup_default_vid = 1;
    }

    //if (0 == setup_lan_if)
    {
#ifdef ISISC
        //MISC_ARP_CMD_SET(0, FAL_MAC_FRWRD); /* Should be put in init function. */
#if 0
        MISC_ARP_SP_NOT_FOUND_SET(0, FAL_MAC_RDT_TO_CPU);
#endif
#endif
        if (SW_OK == setup_interface_entry(nat_lan_dev_list, 0))
        {
            //setup_lan_if = 1; /* setup LAN interface entry success */
            //printk("Setup LAN interface entry done!\n");
        }
    }

    if (0 == setup_wan_if)
    {
        if (SW_OK == setup_interface_entry(nat_wan_dev_list, 1))
        {
            setup_wan_if = 1; /* setup WAN interface entry success */
        }
    }
	if (((nat_chip_ver&0xffff)>>8) == NAT_CHIP_VER_8327) {
		if ((nf_athrs17_hnat_wan_type == NF_S17_WAN_TYPE_PPPOE) ||
			(nf_athrs17_hnat_wan_type == NF_S17_WAN_TYPE_PPPOEV6))
		{
			uint8_t buf[6];

			memcpy(buf, nf_athrs17_hnat_ppp_peer_mac, ETH_ALEN);
			HNAT_PRINTK("Peer MAC: %s ", buf);
			/* add the peer interface with VID */
			if_mac_add(buf, wan_fid, 0);
			HNAT_PRINTK(" --> (%.2x-%.2x-%.2x-%.2x-%.2x-%.2x)\n", \
				buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);
			memcpy(&wanip, (void *)&nf_athrs17_hnat_wan_ip, 4);
		}
	}

    return 1;
}

/* check for pppoe session change */
static void isis_pppoe_check_for_redial(void)
{
    if (nf_athrs17_hnat_wan_type == NF_S17_WAN_TYPE_IP)
        return;

    if(((nf_athrs17_hnat_wan_type == NF_S17_WAN_TYPE_PPPOE) \
            || (nf_athrs17_hnat_wan_type == NF_S17_WAN_TYPE_PPPOEV6)) \
            && (pppoetbl.session_id != 0))
    {
        if(pppoetbl.session_id != nf_athrs17_hnat_ppp_id)
        {
            aos_printk("%s: PPPoE session ID changed... \n", __func__);
            if (nf_athrs17_hnat_wan_type != NF_S17_WAN_TYPE_PPPOEV6)
            {
                if (PPPOE_SESSION_TABLE_DEL(0, &pppoetbl) != SW_OK)
                {
                    aos_printk("delete old pppoe session %d entry_id %d failed.. \n", pppoetbl.session_id, pppoetbl.entry_id);
                    return;
                }

                /* force PPPoE parser for multi- and uni-cast packets; for v1.0.7+ */
                pppoetbl.session_id = nf_athrs17_hnat_ppp_id;
                pppoetbl.multi_session = 1;
			if (((nat_chip_ver & 0xffff)>>8) == NAT_CHIP_VER_8327)
				pppoetbl.uni_session = 1;
			else
                		pppoetbl.uni_session = 0;
                pppoetbl.entry_id = 0;
                /* set the PPPoE edit reg (0x2200), and PPPoE session reg (0x5f000) */
                if (PPPOE_SESSION_TABLE_ADD(0, &pppoetbl) == SW_OK)
                {
                    PPPOE_SESSION_ID_SET(0, pppoetbl.entry_id, pppoetbl.session_id);
                    printk("%s: new pppoe session id: %x, entry_id: %x\n", __func__, pppoetbl.session_id, pppoetbl.entry_id);
                }
            }
            else  /* nf_athrs17_hnat_wan_type == NF_S17_WAN_TYPE_PPPOEV6 */
            {
                /* reset the session Id only */
                aos_printk("IPV6 PPPOE mode... \n");
                pppoetbl.session_id = nf_athrs17_hnat_ppp_id;
                PPPOE_SESSION_ID_SET(0, pppoetbl.entry_id, pppoetbl.session_id);
                printk("%s: new pppoe session id: %x, entry_id: %x\n", __func__, pppoetbl.session_id, pppoetbl.entry_id);
            }
            /* read back the WAN IP */
            memcpy(&wanip, (void *)&nf_athrs17_hnat_wan_ip, 4);
            aos_printk("Read the WAN IP back... %.8x\n", *(uint32_t *)&wanip);
            /* change the PPPoE ACL to ensure the packet is correctly forwarded by the HNAT engine */
            pppoe_add_acl_rules(*(uint32_t *)&wanip, *(uint32_t *)&lanip,
            						*(uint32_t *)&lanipmask, pppoe_gwid);
        }
    }
}

#ifdef ISIS /* only for S17 */
static void pppoev6_mac6_loop_dev(void)
{
#define PPPOEV6_SESSION_ID  0xfffe
    fal_pppoe_session_t ptbl;
	sw_error_t rv;
	a_uint32_t entry;

    memset(&ptbl, 0, sizeof(fal_pppoe_session_t));

    aos_printk("%s: set MAC6 as loopback device\n", __func__);

    ptbl.session_id = PPPOEV6_SESSION_ID;
    ptbl.multi_session = 1;
    ptbl.uni_session = 1;
    ptbl.entry_id = 0xe;

    /* set the PPPoE edit reg (0x2200), and PPPoE session reg (0x5f000) */
    if (PPPOE_SESSION_TABLE_ADD(0, &ptbl) == SW_OK)
    {
        PPPOE_SESSION_ID_SET(0, ptbl.entry_id, ptbl.session_id);
        aos_printk("%s: pppoe session id: %d added into entry: %d \n", __func__, ptbl.session_id, ptbl.entry_id);
    }
    else
    {
        aos_printk("%s: failed on adding pppoe session id: %d\n", __func__, ptbl.session_id);
    }

    /* PPPoE entry 0 */
	entry = PPPOEV6_SESSION_ID;
	HSL_REG_ENTRY_SET(rv, 0, PPPOE_EDIT, 0, (a_uint8_t *) (&entry), sizeof (a_uint32_t));

    aos_printk("%s: end of function... \n", __func__);
}

static void pppoev6_remove_parser(uint32_t entry_id)
{
	sw_error_t rv;
	a_uint32_t entry;
	
    aos_printk("%s: clear entry id: %d\n", __func__, entry_id);
    /* clear the session id in the PPPoE parser engine */
	entry = 0;
	HSL_REG_ENTRY_SET(rv, 0, PPPOE_SESSION,
					entry_id, (a_uint8_t *) (&entry), sizeof (a_uint32_t));
}

#if 0
static void pppoev6_mac6_stop_learning(void)
{
    /* do not disable this port if some other registers are already filled in
       to prevent setting conflict */
    int val = S17_P6PAD_MODE_REG_VALUE;
	sw_error_t rv;
	a_uint32_t entry;

    if ( val != (1<<24))
    {
        aos_printk("%s: MAC 6 already being used!\n", __FUNCTION__);
        return;
    }


    /* clear the MAC6 learning bit */
	HSL_REG_ENTRY_GET(rv, 0, PORT_LOOKUP_CTL, 6, (a_uint8_t *) (&entry), sizeof (a_uint32_t));
	entry = entry & ~(1<<20);
	HSL_REG_ENTRY_SET(rv, 0, PORT_LOOKUP_CTL, 6, (a_uint8_t *) (&entry), sizeof (a_uint32_t));

    /* force loopback mode */
	entry = 0x7e;
	HSL_REG_ENTRY_SET(rv, 0, PORT_STATUS, 6, (a_uint8_t *) (&entry), sizeof (a_uint32_t));
	entry = 0x10;
	HSL_REG_ENTRY_SET(rv, 0, PORT_HDR_CTL, 6, (a_uint8_t *) (&entry), sizeof (a_uint32_t));
}
#endif
#endif // ifdef ISIS

static int add_pppoe_host_entry(uint32_t sport, a_int32_t arp_entry_id)
{
    a_bool_t ena;
    int rv = SW_OK;
    fal_host_entry_t nh_arp_entry;

    if (0xffff == wan_fid)
    {
        printk("%s: Cannot get WAN vid!\n", __FUNCTION__);
        return SW_FAIL;
    }

    if (PPPOE_STATUS_GET(0, &ena) != SW_OK)
    {
        aos_printk("Cannot get the PPPoE mode\n");
        ena = 0;
    }

    memset(&nh_arp_entry, 0, sizeof(nh_arp_entry));
    nh_arp_entry.ip4_addr = ntohl(nf_athrs17_hnat_ppp_peer_ip);
    nh_arp_entry.flags = FAL_IP_IP4_ADDR;
    rv = IP_HOST_GET(0, FAL_IP_ENTRY_IPADDR_EN, &nh_arp_entry);
    if (SW_OK != rv || pppoetbl.session_id != nf_athrs17_hnat_ppp_id)
    {
        if ((!ena) && (PPPOE_STATUS_SET(0, A_TRUE) != SW_OK))
            aos_printk("Cannot enable the PPPoE mode\n");

        aos_printk("PPPoE enable mode: %d\n", ena);

        pppoetbl.session_id = nf_athrs17_hnat_ppp_id;
        pppoetbl.multi_session = 1;
		if (((nat_chip_ver & 0xffff)>>8) == NAT_CHIP_VER_8327)
			pppoetbl.uni_session = 1;
		else
        		pppoetbl.uni_session = 0;
        pppoetbl.entry_id = 0;

        /* set the PPPoE edit reg (0x2200), and PPPoE session reg (0x5f000) */
        rv = PPPOE_SESSION_TABLE_ADD(0, &pppoetbl);
        if (rv == SW_OK)
        {
            uint8_t mbuf[6], ibuf[4];
            a_int32_t a_entry_id = -1;
			a_uint32_t index;

            PPPOE_SESSION_ID_SET(0, pppoetbl.entry_id, pppoetbl.session_id);
            aos_printk("pppoe session: %d, entry_id: %d\n", pppoetbl.session_id, pppoetbl.entry_id);

            if (((nat_chip_ver&0xffff)>>8) == DESS_CHIP_VER) {
                hnat_add_host_route(ntohl(nf_athrs17_hnat_wan_ip),
				HOST_PREFIX_MAX);
            }

            /* create the peer host ARP entry */
            memcpy(ibuf, (void *)&nf_athrs17_hnat_ppp_peer_ip, 4);
            memcpy(mbuf, nf_athrs17_hnat_ppp_peer_mac, ETH_ALEN);

            a_entry_id = arp_hw_add(S17_WAN_PORT, wan_fid, ibuf, mbuf, 0);
            if (a_entry_id >= 0) /* hostentry creation okay */
            {
                aos_printk("(1)Bind PPPoE session ID: %d, entry_id: %d to host entry: %d\n", \
                           pppoetbl.session_id, pppoetbl.entry_id, a_entry_id);

                rv = IP_HOST_PPPOE_BIND(0, a_entry_id, pppoetbl.entry_id, A_TRUE);
                if ( rv != SW_OK)
                {
                    aos_printk("IP_HOST_PPPOE_BIND failed (entry: %d, rv: %d)... \n", a_entry_id, rv);
                }

                aos_printk("adding ACLs \n");
                pppoe_gwid = a_entry_id;
			pppoe_add_acl_rules(nf_athrs17_hnat_wan_ip, *(uint32_t *)&lanip,
								*(uint32_t *)&lanipmask, a_entry_id);
			nat_hw_pub_ip_add(ntohl(nf_athrs17_hnat_wan_ip), &index);
                aos_printk("ACL creation okay... \n");
            } else {
			HNAT_PRINTK("pppoe arp add fail!\n");
            }
        }
        else
        {
            aos_printk("PPPoE session add failed.. (id: %d)\n", pppoetbl.session_id);
            aos_printk("rv: %d\n", rv);
        }

#ifdef ISIS
        if (nf_athrs17_hnat_wan_type == NF_S17_WAN_TYPE_PPPOEV6)
        {
            aos_printk("IPV6 PPPOE mode... (share the same ID with IPV4's)\n");
            pppoev6_mac6_loop_dev();
            pppoev6_remove_parser(pppoetbl.entry_id);

            /* bind the first LAN host to the pseudo PPPoE ID */
            rv = IP_HOST_PPPOE_BIND(0, arp_entry_id, 0, A_TRUE);
            if ( rv != SW_OK)
            {
                aos_printk("IP_HOST_PPPOE_BIND failed (entry: %d, rv: %d)... \n", arp_entry_id, rv);
            }
        }
#endif // ifdef ISIS
    }
#ifdef ISIS
    else  /* ena */
    {
        if ((nf_athrs17_hnat_wan_type == NF_S17_WAN_TYPE_PPPOEV6) &&
                (sport != S17_WAN_PORT)&& (arp_entry_id != 0))
        {
            aos_printk("IPV6 PPPoE mode\n");
            /* bind LAN hosts to the pseudo PPPoE ID */
            rv = IP_HOST_PPPOE_BIND(0, arp_entry_id, 0, A_TRUE);
            if ( rv != SW_OK)
            {
                aos_printk("IP_HOST_PPPOE_BIND failed (entry: %d, rv: %d)... \n", arp_entry_id, rv);
            }
        }
    }
#endif // ifdef ISIS

    return SW_OK;
}

static int
dev_check(char *in_dev, char *dev_list)
{
    char *list_dev;
    char temp[100] = {0};
    char *list;

    if(!in_dev || !dev_list)
    {
        return 0;
    }

    strlcpy(temp, dev_list, sizeof(temp));
    list = temp;

    while ((list_dev = strsep(&list, " ")) != NULL)
    {
        HNAT_PRINTK("%s: strlen:%d list_dev:%s in_dev:%s\n",
                    __func__, strlen(list_dev), list_dev, in_dev);

        if (!strncmp(list_dev, in_dev, strlen(list_dev)))
        {
            HNAT_PRINTK("%s: %s\n", __FUNCTION__, list_dev);
            return 1;
        }
    }

    return 0;
}

#ifndef ISISC
static uint32_t get_netmask_from_netdevice(const struct net_device *in_net_dev)
{
    struct in_device *my_in_device = NULL;
    uint32_t result = 0xffffff00;

    if((in_net_dev) && (in_net_dev->ip_ptr != NULL))
    {
        my_in_device = (struct in_device *)(in_net_dev->ip_ptr);
        if(my_in_device->ifa_list != NULL)
        {
            result = my_in_device->ifa_list->ifa_mask;
        }
    }

    return result;
}
#endif


static void hnat_add_neigh(struct neighbour *neigh) 
{
	struct nat_helper_bg_msg msg;
	struct net_device *dev = NULL;

	memset(&msg, 0, sizeof(msg));
	msg.arp_in.ip = *((uint32_t *)neigh->primary_key);
	memcpy(msg.arp_in.mac, neigh->ha, ETH_ALEN);
	strlcpy(msg.arp_in.name, neigh->dev->name, IFNAMSIZ);
	msg.arp_in.in = neigh->dev;

	if (neigh->dev->priv_flags & IFF_EBRIDGE) {
		if (!(dev = br_port_dev_get(neigh->dev, neigh->ha, NULL, 0))) {
			HNAT_ERR_PRINTK("Failed to find bridge port by [%pM]\n",
				neigh->ha);
			return ;
		}
	} else {
		dev = neigh->dev;
		dev_hold(dev);
	}

	if (strncmp(dev->name, "eth", strlen("eth")) &&
	     strncmp(dev->name, "erouter", strlen("erouter"))) {
		dev_put(dev);
		return ;
	}

	dev_put(dev);

	msg.msg_type = NAT_HELPER_ARP_ADD_MSG;
	bg_ring_buf_write(msg);
}

static void hnat_del_neigh(struct neighbour *neigh)
{
	struct nat_helper_bg_msg msg;

	memset(&msg, 0, sizeof(msg));
	msg.arp_in.ip  = ntohl(*((uint32_t *)neigh->primary_key));

	msg.msg_type = NAT_HELPER_ARP_DEL_MSG;
	bg_ring_buf_write(msg);
}

static int hnat_netevent_event(struct notifier_block *unused, unsigned long event, void *ptr)
{
	struct neigh_table *tbl;
	struct neighbour *neigh;

	if (event != NETEVENT_NEIGH_UPDATE)
		return NOTIFY_DONE;

	neigh = ptr;
	tbl = neigh->tbl;

	if (tbl->family != AF_INET)
		return NOTIFY_DONE;

	HNAT_PRINTK("netevent state %d for ip[%pI4]\n",
		neigh->nud_state, (uint32_t *)neigh->primary_key);

	if (neigh->nud_state & NUD_VALID) {
		if (neigh->nud_state & NUD_NOARP) {
			return NOTIFY_DONE;
		}

		HNAT_PRINTK("New ARP entry ip[%pI4] mac[%pM]\n",
			(uint32_t *)neigh->primary_key, neigh->ha);
		hnat_add_neigh(neigh);
	} else {
		HNAT_PRINTK("Del ARP entry ip[%pI4] mac[%pM] with status[%d]\n",
			(uint32_t *)neigh->primary_key, neigh->ha, neigh->nud_state);
		hnat_del_neigh(neigh);
	}

	return NOTIFY_DONE;	
}

#ifdef NAT_BACKGROUND_TASK
static unsigned int
arp_del(struct nat_helper_bg_msg *msg)
{
	fal_host_entry_t host;

	memset(&host, 0, sizeof(host));

	if (!nat_hw_prv_base_is_match(msg->arp_in.ip))
		return 0;

	if (napt_hw_get_by_sip(msg->arp_in.ip)) {
		host.flags |= FAL_IP_IP4_ADDR;
		host.ip4_addr = msg->arp_in.ip;
		IP_HOST_DEL(0, FAL_IP_ENTRY_IPADDR_EN, &host);
	}

	return 0;
}

static unsigned int
arp_add(struct nat_helper_bg_msg *msg)
{
    uint8_t *smac;
    uint8_t dev_is_lan = 0;
    uint32_t sport = 0, vid = 0;
    a_bool_t prvbasemode = 1;
    sw_error_t rv = SW_OK;
    struct arp_in_msg *arp_info = &msg->arp_in;
    a_int32_t arp_entry_id = -1;
    fal_fdb_entry_t entry;


    /* check for PPPoE redial here, to reduce overheads */
    isis_pppoe_check_for_redial();

    /* do not write out host table if HNAT is disabled */
    if (!nf_athrs17_hnat)
        return 0;

    setup_all_interface_entry();

    if(dev_check((char *)arp_info->name, (char *)nat_wan_dev_list))
    {

    }
    else if (dev_check((char *)arp_info->name, (char *)nat_lan_dev_list))
    {
        dev_is_lan = 1;
    }
    else
    {
        HNAT_INFO_PRINTK("Not Support device: %s\n",  (char *)arp_info->name);
        return 0;
    }

    if(dev_is_lan) {
         vid = nat_lan_vid;
    } else {
         vid = nat_wan_vid;
    }

    memset(&entry, 0, sizeof(entry));

    entry.fid = vid;

    smac = arp_info->mac;
    aos_mem_copy(&(entry.addr), smac, sizeof(fal_mac_addr_t));
    if(fal_fdb_entry_search(0, &entry) == SW_OK) {
        vid  = entry.fid;
        sport = 0;
        while (sport < 32) {
            if(entry.port.map & (1 << sport)) {
                break;
            }
            sport++;
        }
    } else {
        HNAT_PRINTK("not find the FDB entry\n");
    }


    if (sport == 0) {
        HNAT_PRINTK("Not the expected arp, ignore it!\n");
        return 0;
    }

    arp_entry_id = arp_hw_add(sport, vid, (a_uint8_t *)&arp_info->ip, smac, 0);
    if(arp_entry_id < 0)
    {
        HNAT_ERR_PRINTK("ARP entry error!!\n");
        return 0;
    }

    if (0 == dev_is_lan)
    {
        struct in_device *in_dev;

        in_dev = __in_dev_get_rtnl(arp_info->in);
        if (in_dev) {
            if (in_dev->ifa_list) {
                *(uint32_t *)&wanip = ntohl(in_dev->ifa_list->ifa_local);
            }
        }
#ifdef MULTIROUTE_WR
        wan_nh_add((u_int8_t *)&arp_info->ip, smac, arp_entry_id);
#endif
    }

    if(dev_is_lan && nat_hw_prv_base_can_update())
    {
        nat_hw_flush();
        nat_hw_prv_base_update_disable();
#ifdef MULTIROUTE_WR
        //multi_route_indev = in;
#endif
    }
    multi_route_indev = arp_info->in;

    if ((nf_athrs17_hnat_wan_type == NF_S17_WAN_TYPE_PPPOE) ||
            (nf_athrs17_hnat_wan_type == NF_S17_WAN_TYPE_PPPOEV6))
    {
        add_pppoe_host_entry(sport, arp_entry_id);
    }

	if (((nat_chip_ver & 0xffff)>>8) != NAT_CHIP_VER_8327)
		return 1;

    /* check for SIP and DIP range */
    if ((lanip[0] != 0) && (wanip[0] != 0))
    {
	 rv = NAT_PRV_ADDR_MODE_GET(0, &prvbasemode);
	 if (rv == SW_NOT_SUPPORTED || rv == SW_NOT_INITIALIZED) {
		 return 1;
	 }
	 else if (rv != SW_OK) {
		 aos_printk("Private IP base mode check failed: %d\n", prvbasemode);
	 }

        if (!prvbasemode) /* mode 0 */
        {
            if ((lanip[0] == wanip[0]) && (lanip[1] == wanip[1]))
            {
                if ((lanip[2] & 0xf0) == (wanip[2] & 0xf0))
                {
                    if (get_aclrulemask()& (1 << S17_ACL_LIST_IPCONF))
                        return 0;

                    aos_printk("LAN IP and WAN IP conflict... \n");
                    /* set h/w acl to filter out this case */
#ifdef MULTIROUTE_WR
                    // if ( (wan_nh_ent[0].host_ip != 0) && (wan_nh_ent[0].entry_id != 0))
                    if ( (wan_nh_ent[0].host_ip != 0))
                        ip_conflict_add_acl_rules(*(uint32_t *)&wanip, *(uint32_t *)&lanip, wan_nh_ent[0].entry_id);
#endif
                    return 0;
                }
            }
        }
        else  /* mode 1*/
        {
            ;; /* do nothing */
        }
    }

    return 1;
}
#endif

static struct notifier_block hnat_netevent_notifier = {
	.notifier_call = hnat_netevent_event,
};

#ifdef AUTO_UPDATE_PPPOE_INFO
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,14,0))
struct prv_ppp_file {
	int		k;
	struct sk_buff_head xmitq;
	struct sk_buff_head recvq;
	wait_queue_head_t wait;
	atomic_t	cnt;
	int		hlen;
	int		idx;
	int		d;
};
struct prv_ppp {
	struct prv_ppp_file	file;
	char	*o;
	struct list_head channels;
	int		n_channels;
	spinlock_t	rlock;
	spinlock_t	wlock;
	int		m;
	unsigned int	flags;
	unsigned int	xmitstate;
	unsigned int	recvstate;
	int		d;
	char *vj;
	int	mode[6];
	struct sk_buff	*xpending;
	char *xcmp;
	void		*xstate;
	char *rcmp;
	void		*rstate;
	unsigned long	l_xmit;
	unsigned long	l_recv;
	struct net_device *dev;
	int		resv1;
#ifdef CONFIG_PPP_MULTILINK
	int		resv2;
	u32		resv3;
	int		resv4;
	u32		resv5;
	u32		resv6;
	struct sk_buff_head rq;
#endif /* CONFIG_PPP_MULTILINK */
#ifdef CONFIG_PPP_FILTER
	char *p_filter;
	char *a_filter;
	unsigned p_len, a_len;
#endif /* CONFIG_PPP_FILTER */
	char	*pnet;
};

struct prv_channel {
	struct prv_ppp_file	file;
	struct list_head list;
	struct ppp_channel *chan;
	struct rw_semaphore sem;
	spinlock_t	downlock;
	struct prv_ppp	*ppp;
	char	*cnet;
	struct list_head clist;
	rwlock_t	uplock;
#ifdef CONFIG_PPP_MULTILINK
	u8		resv1;
	u8		resv2;
	u32		resv3;
	int		resv4;
#endif
};
#endif

static int qcaswitch_pppoe_ip_event(struct notifier_block *this,
                                    unsigned long event, void *ptr)
{
    struct in_ifaddr *ifa = (struct in_ifaddr *)ptr;
    struct net_device *dev = (struct net_device *)ifa->ifa_dev->dev;
	#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,14,0))
	struct list_head *list;
	struct prv_channel *pch;
	struct prv_ppp *ppp = netdev_priv(dev);
	#endif
    struct sock *sk;
    struct pppox_sock *po;
    static int connection_count = 0;
    fal_pppoe_session_t del_pppoetbl;
	int channel_count;
	struct ppp_channel *ppp_chan[1];
	int channel_protocol;

    if (!((dev->type == ARPHRD_PPP) && (dev->flags & IFF_POINTOPOINT)))
        return NOTIFY_DONE;

    if (dev_net(dev) != &init_net)
        return NOTIFY_DONE;

    setup_all_interface_entry();

    switch (event)
    {
        case NETDEV_UP:
	#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,14,0))
            if (ppp_is_multilink(dev) == 0) {
                /* not doing multilink: send it down the first channel */
			channel_count = ppp_hold_channels(dev, ppp_chan, 1);
			if (channel_count != 1)
				return NOTIFY_DONE;

			channel_protocol = ppp_channel_get_protocol(ppp_chan[0]);
			if (channel_protocol == PX_PROTO_OE)
			{
				if (ppp_chan[0]->private)
				{
					/* the NETDEV_UP event will be sent many times
					* because of ifa operation ifa->ifa_local != ifa->ifa_address
					* means that remote ip is really added.
					*/
					if (ifa->ifa_local == ifa->ifa_address)
					{
						ppp_release_channels(ppp_chan, 1);
						return NOTIFY_DONE;
					}
					sk = (struct sock *)(ppp_chan[0]->private);
					po = (struct pppox_sock*)sk;
					connection_count++;
					if (((NF_S17_WAN_TYPE_PPPOE == nf_athrs17_hnat_wan_type) &&
					(0 != nf_athrs17_hnat_ppp_id))) /* another session for IPv6 */
					{
						nf_athrs17_hnat_ppp_id2 = ntohs(po->proto.pppoe.pa.sid);
						memcpy(nf_athrs17_hnat_ppp_peer_mac2,
							po->proto.pppoe.pa.remote, ETH_ALEN);
					} else {
						nf_athrs17_hnat_wan_type = NF_S17_WAN_TYPE_PPPOE;
						nf_athrs17_hnat_wan_ip = ifa->ifa_local;
						nf_athrs17_hnat_ppp_peer_ip = ifa->ifa_address;
						memcpy(nf_athrs17_hnat_ppp_peer_mac,
							po->pppoe_pa.remote, ETH_ALEN);
						nf_athrs17_hnat_ppp_id = ntohs(po->pppoe_pa.sid);
					}
					ppp_release_channels(ppp_chan, 1);
				} else {
					ppp_release_channels(ppp_chan, 1);
					/* channel got unregistered */
					return NOTIFY_DONE;
				}
			} else {
				ppp_release_channels(ppp_chan, 1);
				/* channel got unregistered */
				return NOTIFY_DONE;
			}
		}
	#else
		//struct prv_ppp *ppp = netdev_priv(dev);
		//struct prv_channel *pch;
		list = &ppp->channels;
		if (list_empty(list))
                return NOTIFY_DONE;
		if ((ppp->flags & SC_MULTILINK) == 0) {
			list = list->next;
			pch = list_entry(list, struct prv_channel, clist);
			if (pch->chan)
			{
				if (pch->chan->private) {
					if (ifa->ifa_local == ifa->ifa_address)
						return NOTIFY_DONE;
					sk = (struct sock *)pch->chan->private;
					po = (struct pppox_sock*)sk;
					connection_count++;
					if (((NF_S17_WAN_TYPE_PPPOE == nf_athrs17_hnat_wan_type) &&
						(0 != nf_athrs17_hnat_ppp_id)))
					{
						nf_athrs17_hnat_ppp_id2 = po->num;
						memcpy(nf_athrs17_hnat_ppp_peer_mac2, po->pppoe_pa.remote, ETH_ALEN);
					}
					else
					{
						nf_athrs17_hnat_wan_type = NF_S17_WAN_TYPE_PPPOE;
						nf_athrs17_hnat_wan_ip = ifa->ifa_local;
		                            nf_athrs17_hnat_ppp_peer_ip = ifa->ifa_address;
		                            memcpy(nf_athrs17_hnat_ppp_peer_mac, po->pppoe_pa.remote, ETH_ALEN);
		                            nf_athrs17_hnat_ppp_id = ntohs(po->pppoe_pa.sid);
					}
				}
			}
			else
			{
				return NOTIFY_DONE;
			}
		}
	#endif
		break;

        case NETDEV_DOWN:
            if (NF_S17_WAN_TYPE_PPPOE != nf_athrs17_hnat_wan_type)
            {
                return NOTIFY_DONE;
            }
            connection_count--;
            if (ifa->ifa_local == nf_athrs17_hnat_wan_ip)
            {
                /* PPPoE Interface really down */
                ipv6_droute_del_acl_rules();
                del_pppoetbl.session_id = nf_athrs17_hnat_ppp_id;
                del_pppoetbl.multi_session = 1;
			if (((nat_chip_ver & 0xffff)>>8) == NAT_CHIP_VER_8327)
				del_pppoetbl.uni_session = 1;
			else
                		del_pppoetbl.uni_session = 0;
                del_pppoetbl.entry_id = 0;
                PPPOE_SESSION_TABLE_DEL(0, &del_pppoetbl);
		memset(&pppoetbl, 0, sizeof(pppoetbl));
                nf_athrs17_hnat_wan_type = NF_S17_WAN_TYPE_IP;
                nf_athrs17_hnat_wan_ip = 0;
                nf_athrs17_hnat_ppp_peer_ip = 0;
                nf_athrs17_hnat_ppp_id = 0;
                memset(&nf_athrs17_hnat_ppp_peer_mac, 0, ETH_ALEN);
            }
            else
            {
                if (0 != nf_athrs17_hnat_ppp_id2)
                {
                    del_pppoetbl.session_id = nf_athrs17_hnat_ppp_id2;
                    del_pppoetbl.multi_session = 1;
				if (((nat_chip_ver & 0xffff)>>8) == NAT_CHIP_VER_8327)
					del_pppoetbl.uni_session = 1;
				else
                    		del_pppoetbl.uni_session = 0;
                    del_pppoetbl.entry_id = 0;
                    PPPOE_SESSION_TABLE_DEL(0, &del_pppoetbl);
			memset(&pppoetbl, 0, sizeof(pppoetbl));
                }
                nf_athrs17_hnat_ppp_id2 = 0;
                memset(&nf_athrs17_hnat_ppp_peer_mac2, 0, ETH_ALEN);
            }
            qcaswitch_hostentry_flush();
            break;

        default:
            break;
    }
    return NOTIFY_DONE;
}

/* a linux interface is configured with ipaddr, then
 * it becomes a L3 routing interface
 * add the router mac of this interface to the table
 */
/* FIXME: only hande pppoe event right now. */
static int qcaswitch_ip_event(struct notifier_block *this,
                              unsigned long event, void *ptr)
{
    struct in_ifaddr *ifa = (struct in_ifaddr *)ptr;
    struct net_device *dev = (struct net_device *)ifa->ifa_dev->dev;

    if ((dev->type == ARPHRD_PPP) && (dev->flags & IFF_POINTOPOINT))
    {
        return qcaswitch_pppoe_ip_event(this, event, ptr);
    }

    return NOTIFY_DONE;
}


static struct notifier_block qcaswitch_ip_notifier =
{
    .notifier_call = qcaswitch_ip_event,
    .priority = 100,
};
#endif // ifdef AUTO_UPDATE_PPPOE_INFO

#define HOST_AGEOUT_STATUS 1
void host_check_aging(void)
{
    fal_host_entry_t *host_entry_p, host_entry= {0};
    sw_error_t rv;
    int cnt = 0;
    unsigned long flags;
    fal_napt_entry_t src_napt = {0}, pub_napt = {0};

    host_entry_p = &host_entry;
    host_entry_p->entry_id = FAL_NEXT_ENTRY_FIRST_ID;

	/*check host is not neccessary, check napt is enough*/
	return;

    local_irq_save(flags);
    while (1)
    {
        host_entry_p->status = HOST_AGEOUT_STATUS;
        /* FIXME: now device id is set to 0. */
        rv = IP_HOST_NEXT (0, FAL_IP_ENTRY_STATUS_EN, host_entry_p);
        // rv = IP_HOST_NEXT (0, 0, host_entry_p);
        if (SW_OK != rv)
            break;
        if (cnt >= ARP_ENTRY_MAX) // arp entry number
            break;

        if (ARP_AGE_NEVER == host_entry_p->status)
            continue;

        if ((S17_WAN_PORT == host_entry_p->port_id) &&
                (host_entry_p->counter_en))
        {
            if (0 != host_entry_p->packet)
            {
                // arp entry is using, update it.
                host_entry.status = ARP_AGE;
                printk("Update WAN port hostentry!\n");
                IP_HOST_ADD(0, host_entry_p);
            }
            else
            {
                printk("Del WAN port hostentry!\n");
                IP_HOST_DEL(0, FAL_IP_ENTRY_IPADDR_EN, host_entry_p);
            }
            continue;
        }

        src_napt.entry_id = FAL_NEXT_ENTRY_FIRST_ID;
        memcpy(&src_napt.src_addr, &host_entry_p->ip4_addr, sizeof(fal_ip4_addr_t));
        pub_napt.entry_id = FAL_NEXT_ENTRY_FIRST_ID;
        memcpy(&pub_napt.trans_addr, &host_entry_p->ip4_addr, sizeof(fal_ip4_addr_t));
        if((NAPT_NEXT(0, FAL_NAT_ENTRY_SOURCE_IP_EN ,&src_napt) !=0) && \
                (NAPT_NEXT(0, FAL_NAT_ENTRY_PUBLIC_IP_EN ,&pub_napt) != 0))
        {
            /* Cannot find naptentry */
            printk("ARP id 0x%x: Cannot find NAPT entry!\n", host_entry_p->entry_id);
            IP_HOST_DEL(0, FAL_IP_ENTRY_IPADDR_EN, host_entry_p);
            continue;
        }
        // arp entry is using, update it.
        host_entry_p->status = ARP_AGE;
        IP_HOST_ADD(0, host_entry_p);
        printk("update entry 0x%x port %d\n", host_entry_p->entry_id, host_entry_p->port_id);
        cnt++;
    }
    local_irq_restore(flags);
}

#ifdef CONFIG_IPV6_HWACCEL
#define IPV6_LEN 16
#define MAC_LEN 6
#define PROTO_ICMPV6 0x3a
#define NEIGHBOUR_SOL 135
#define NEIGHBOUR_AD 136

struct icmpv6_option
{
    __u8 type;
    __u8 len;
    __u8 mac[MAC_LEN];
};
#if 0
static unsigned int ipv6_handle(unsigned   int   hooknum,
                                struct   sk_buff   *skb,
                                const   struct   net_device   *in,
                                const   struct   net_device   *out,
                                int   (*okfn)(struct   sk_buff   *))
{
	struct sk_buff *new_skb = NULL;
	struct nat_helper_bg_msg msg;
	/*unsigned long flags = 0;*/

	if (!nf_athrs17_hnat)
        return NF_ACCEPT;

	new_skb = skb_clone(skb, GFP_ATOMIC);
	if(new_skb) {
		memset(&msg, 0, sizeof(msg));
		msg.msg_type = NAT_HELPER_IPV6_MSG;
		msg.ipv6.skb = new_skb;
		msg.ipv6.in = (struct net_device *)in;

		/*send msgto background task*/
		/*spin_lock_irqsave(&task_cb.bg_lock, flags);*/
		if(bg_ring_buf_write(msg))
			kfree_skb(new_skb);
		/*spin_unlock_irqrestore(&task_cb.bg_lock, flags);*/
	}
	
	return NF_ACCEPT;
	
}
#endif

#ifdef NAT_BACKGROUND_TASK

static unsigned int ipv6_bg_handle(struct nat_helper_bg_msg *msg)
{
	struct net_device *in = msg->arp_in.in;
	struct sk_buff *skb = msg->arp_in.skb;
	
    struct ipv6hdr *iph6 = ipv6_hdr(skb);
    struct icmp6hdr *icmp6 = icmp6_hdr(skb);
    __u8 *sip = ((__u8 *)icmp6)+sizeof(struct icmp6hdr);
    struct icmpv6_option *icmpv6_opt = (struct icmpv6_option *)(sip+IPV6_LEN);
    __u8 *sa = icmpv6_opt->mac;

    uint32_t sport = 0, vid = 0;
    struct inet6_ifaddr *in_device_addr = NULL;
    uint8_t dev_is_lan = 0;
    uint8_t *smac;
	

    /* do not write out host table if HNAT is disabled */
    if (!nf_athrs17_hnat)
        return 0;

    setup_all_interface_entry();

    if(dev_check((char *)in->name, (char *)nat_wan_dev_list))
    {
        dev_is_lan = 0;
    }
    else if (dev_check((char *)in->name, (char *)nat_lan_dev_list))
    {
        dev_is_lan = 1;
    }
    else
    {
        HNAT_PRINTK("Not Support device: %s\n",  (char *)in->name);
        return 0;
    }

    if(PROTO_ICMPV6 == iph6->nexthdr)
    {
        if(NEIGHBOUR_AD == icmp6->icmp6_type)
        {
			fal_fdb_entry_t entry;
            if (__ipv6_addr_type((struct in6_addr*)sip) & IPV6_ADDR_LINKLOCAL)
                return 0;

#ifdef AP136_QCA_HEADER_EN
            if(arp_if_info_get((void *)(skb->head), &sport, &vid) != 0)
            {
                return 0;
            }

            if ((0 == vid)||(0 == sport))
            {
                printk("Error: Null sport or vid!!\n");
                return 0;
            }
#else
            if(dev_is_lan) {
                 vid = NAT_LAN_DEV_VID;
            } else {
                 vid = NAT_WAN_DEV_VID;
            }

            memset(&entry, 0, sizeof(entry));

            entry.fid = vid;
            smac = skb_mac_header(skb) + MAC_LEN;
            aos_mem_copy(&(entry.addr), smac, sizeof(fal_mac_addr_t));

            if(fal_fdb_entry_search(0, &entry) == SW_OK) {
                vid  = entry.fid;
                sport = 0;
                while (sport < 32) {
                    if(entry.port.map & (1 << sport)) {
                        break;
                    }
                    sport++;
                }
            } else {
                printk("not find the FDB entry\n");
            }
#endif
            if ((0 == dev_is_lan) && (S17_WAN_PORT != sport))
            {
                printk("Error: WAN port %d\n", sport);
                return 0;
            }

            HNAT_PRINTK("ND Reply %x %x\n",icmpv6_opt->type,icmpv6_opt->len);
            HNAT_PRINTK("isis_v6: incoming packet, sip = %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x\n"
                        ,sip[0],sip[1],sip[2],sip[3],sip[4],sip[5],sip[6],sip[7]
                        ,sip[8],sip[9],sip[10],sip[11],sip[12],sip[13],sip[14],sip[15]
                       );
            HNAT_PRINTK("isis_v6: incoming packet, sa  = %.2x-%.2x-%.2x-%.2x-%.2x-%.2x\n", sa[0], sa[1], sa[2], sa[3], sa[4], sa[5]);
            HNAT_PRINTK("isis_v6: vid = %d sport = %d\n", vid, sport);

            //add nd entry
            if((2 == icmpv6_opt->type) && (1 == icmpv6_opt->len))
            {
                arp_hw_add(sport, vid, sip, sa, 1);
            }
            else /* ND AD packets without option filed? Fix Me!! */
            {
		sa = skb_mac_header(skb) + MAC_LEN;
                HNAT_PRINTK("isis_v6 Changed sa  = %.2x-%.2x-%.2x-%.2x-%.2x-%.2x\n", sa[0], sa[1], sa[2], sa[3], sa[4], sa[5]);
                arp_hw_add(sport, vid, sip, sa, 1);
            }

#ifdef NAT_TODO /* should be ok */
            if ((NULL != in->ip6_ptr) && (NULL != ((struct inet6_dev *)in->ip6_ptr)->addr_list))
#else
            if (NULL != in->ip6_ptr)
#endif
            {
                //list_for_each_entry(in_device_addr, &(in->ip6_ptr)->addr_list, if_list);
                struct inet6_dev *idev = __in6_dev_get(in);
                list_for_each_entry(in_device_addr, &idev->addr_list, if_list) {
            		if (in_device_addr->scope == 0 &&
            		    !(in_device_addr->flags & IFA_F_TENTATIVE)) {
            			break;
            		}
        	    }

                if (0 == dev_is_lan)
                {
                    /* WAN ipv6 address*/
                    memcpy(&wan6ip, (__u8 *)&in_device_addr->addr, sizeof(struct in6_addr));
                    HNAT_PRINTK("%s: ipv6 wanip %pI6\n", in->name, &wan6ip);
                }
                else
                {
                    /* LAN ipv6 address*/
                    memcpy(&lan6ip, (__u8 *)&in_device_addr->addr, sizeof(struct in6_addr));
                    HNAT_PRINTK("%s: ipv6 lanip %pI6\n", in->name, &lan6ip);
                }
            }
        }
    }

    return 1;
}
#endif

#if 0
static struct nf_hook_ops ipv6_inhook =
{
    .hook = ipv6_handle,
    #if (LINUX_VERSION_CODE < KERNEL_VERSION(4,4,0))
    .owner = THIS_MODULE,
    #endif
    .pf = PF_INET6,
    .hooknum = NF_INET_PRE_ROUTING,
    .priority = NF_IP6_PRI_CONNTRACK,
};
#endif
#endif /* CONFIG_IPV6_HWACCEL */

#ifdef NAT_BACKGROUND_TASK

static void nat_task_entry(struct work_struct *wq)
{
	struct nat_helper_bg_msg msg;
	/*unsigned long flags = 0;*/
	unsigned int result = 0;

	msg = *(struct nat_helper_bg_msg *)wq;

	/*spin_lock_irqsave(&task_cb.bg_lock, flags);*/
	bg_ring_buf_read(NULL);
	/*spin_unlock_irqrestore(&task_cb.bg_lock, flags);*/

	if(msg.msg_type == NAT_HELPER_ARP_ADD_MSG) {
		result =  arp_add(&msg);
	} else if (msg.msg_type == NAT_HELPER_ARP_DEL_MSG) {
		result =  arp_del(&msg);
	}
	#ifdef CONFIG_IPV6_HWACCEL
	else if(msg.msg_type == NAT_HELPER_IPV6_MSG) {
		result = ipv6_bg_handle(&msg);
	}
	#endif

	HNAT_PRINTK("handle msg: %d, result: %d\n", msg.msg_type, result);
	
}

void nat_helper_bg_task_init()
{
	unsigned int i = 0;
	struct nat_helper_bg_msg *msg;
	/*create the thread and alloc ring buffer*/

	memset(&task_cb, 0, sizeof(task_cb));

	task_cb.nat_wq = create_singlethread_workqueue("nat_wq");

    if(!task_cb.nat_wq)
    {
        aos_printk("create nat workqueuefail\n");
        return;
    }

	/*init lock and alloc the ring buffer*/
	
	/*sema_init(&task_cb.bg_sem, 0);*/
	spin_lock_init(&task_cb.bg_lock);
	
	task_cb.ring.num = NAT_HELPER_MSG_MAX;
	task_cb.ring.buf = kzalloc(NAT_HELPER_MSG_MAX * sizeof(struct nat_helper_bg_msg), GFP_ATOMIC);
	if(!task_cb.ring.buf) {
		aos_printk("ring buf alloc fail!\n");
		return;
	}
	msg = (struct nat_helper_bg_msg*)task_cb.ring.buf;
	for(i = 0; i < task_cb.ring.num; i++) 
	{
		INIT_WORK(&msg[i].work, nat_task_entry);
	}

	aos_printk("bg task init successfull!\n");
	
}

void nat_helper_bg_task_exit()
{
	/*stop the workqueue and release the ring buffer*/
	if(task_cb.nat_wq)
		destroy_workqueue(task_cb.nat_wq);
	if(task_cb.ring.buf)
		kfree(task_cb.ring.buf);
}
#endif


extern int napt_procfs_init(void);
extern void napt_procfs_exit(void);

void host_helper_init(a_uint32_t portbmp)
{
	REG_GET(0, 0, (a_uint8_t *)&nat_chip_ver, 4);

	/* header len 4 with type 0xaaaa */
	HEADER_TYPE_SET(0, A_TRUE, 0xaaaa);
	if (((nat_chip_ver & 0xffff)>>8) == NAT_CHIP_VER_8337 ||
		((nat_chip_ver & 0xffff)>>8) == NAT_CHIP_VER_DESS) {
		/* For S17c (ISISC), it is not necessary to make all frame with header */
		printk("host_helper_init start\n");
		//PORT_TXHDR_MODE_SET(0, 0, FAL_ONLY_MANAGE_FRAME_EN);
		/* Fix tag disappear problem, set TO_CPU_VID_CHG_EN, 0xc00 bit1 */
		CPU_VID_EN_SET(0, A_TRUE);
		/* set RM_RTD_PPPOE_EN, 0xc00 bit0 */
		RTD_PPPOE_EN_SET(0, A_TRUE);
		/* Avoid ARP response storm for HUB, now this fix only apply on PORT5 */
		#if 0
		MISC_ARP_SP_NOT_FOUND_SET(0, FAL_MAC_RDT_TO_CPU);
		MISC_ARP_GUARD_SET(0, S17_WAN_PORT, FAL_MAC_IP_PORT_GUARD);
		#endif
		/* set VLAN_TRANS_TEST register bit, to block packets from WAN port has private dip */
		NETISOLATE_SET(0, A_TRUE);
	} else {
		PORT_TXHDR_MODE_SET(0, 0, FAL_ALL_TYPE_FRAME_EN);
	}
    CPU_PORT_STATUS_SET(0, A_TRUE);
    IP_ROUTE_STATUS_SET(0, A_TRUE);

    napt_procfs_init();
    memcpy(nat_bridge_dev, nat_lan_dev_list, strlen(nat_lan_dev_list)+1);

    register_netevent_notifier(&hnat_netevent_notifier);
/*hnat not upport ipv6*/
#if 0
#ifdef CONFIG_IPV6_HWACCEL
    aos_printk("Registering IPv6 hooks... \n");
    nf_register_hook(&ipv6_inhook);
#endif
#endif

#ifdef AUTO_UPDATE_PPPOE_INFO
    register_inetaddr_notifier(&qcaswitch_ip_notifier);
#endif // ifdef AUTO_UPDATE_PPPOE_INFO

    nat_wan_port = portbmp;

    /* Enable ACLs to handle MLD packets */
    upnp_ssdp_add_acl_rules();
    ipv6_snooping_solicted_node_add_acl_rules();
    ipv6_snooping_sextuple0_group_add_acl_rules();
    ipv6_snooping_quintruple0_1_group_add_acl_rules();

    napt_helper_hsl_init();
}

void host_helper_exit(void)
{
    napt_procfs_exit();

    unregister_netevent_notifier(&hnat_netevent_notifier);
#if 0
#ifdef CONFIG_IPV6_HWACCEL
    nf_unregister_hook(&ipv6_inhook);
#endif
#endif

	#ifdef AUTO_UPDATE_PPPOE_INFO
	unregister_inetaddr_notifier(&qcaswitch_ip_notifier);
	#endif
}

