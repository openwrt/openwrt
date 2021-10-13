/*
 * Copyright (c) 2012, 2015, The Linux Foundation. All rights reserved.
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
#include <linux/netfilter_ipv4/ip_tables.h>
#include <net/netfilter/nf_nat.h>
#include <linux/netfilter/nf_nat.h>
#include <linux/netfilter/xt_multiport.h>
#include <linux/netfilter/xt_tcpudp.h>
#include <linux/netfilter/x_tables.h>
#include "nat_helper.h"

#include "lib/nat_helper_hsl.h"
#include "lib/nat_helper_dt.h"


#define nf_nat_ipv4_multi_range_compat  \
                nf_nat_ipv4_multi_range_compat
#define nf_nat_range        nf_nat_ipv4_range
#define ipt_entry_target    xt_entry_target
#define ipt_entry_match     xt_entry_match

#define IPT_MATCH_ITERATE(e, fun, args...) \
({								    \
	unsigned int i;					\
	int ret = 0;					\
	struct xt_entry_match *m;	    \
								    \
	for (i = sizeof(struct ipt_entry);	\
	     i < (e)->target_offset;		\
	     i += m->u.match_size) {	\
		m = (void *)e + i;			\
		ret = fun(m , ##args);		\
		if (ret != 0)					\
			break;					    \
	}							        \
	ret;							\
})

#define IPT_ENTRY_ITERATE(entries, size, fun, args...) \
({								        \
	unsigned int k, j;					\
	int ret = 0;						\
	struct ipt_entry *e;			    \
								        \
	for (k = 0, j = 0; k < (size);		\
	    k += (e)->next_offset, j++) { \
		e = (void *)(entries) + k;	\
		if (j < 0)					\
			continue;				\
								    \
		ret = fun(e , ##args);	    \
		if (ret != 0)				\
			break;					\
	}							    \
	ret;						\
})


#define IPT_BUFFER_INIT_LEN 1000
#define NF_NAT_INIT_ENTRIES_NUM 5

static int
nat_ipt_set_ctl(struct sock *sk, int cmd, void __user * user, unsigned int len);
static int
nat_ipt_get_ctl(struct sock *sk, int cmd, void __user * user, int *len);

int nat_sockopts_init = 0;

/*those initial value will be overwrited by orignal iptables sockopts*/
static struct nf_sockopt_ops orgi_ipt_sockopts;
static struct nf_sockopt_ops tmp_ipt_sockopts =
{
    /*pls check linux/in.h*/
#define IPT_TEMP_BASE_CTL       60
#define IPT_TEMP_SET_MAX        (IPT_TEMP_BASE_CTL+1)
#define IPT_TEMP_GET_MAX        (IPT_TEMP_BASE_CTL+2)
    .pf = PF_INET,
    .set_optmin = IPT_TEMP_BASE_CTL,
    .set_optmax = IPT_TEMP_SET_MAX,
    .set = nat_ipt_set_ctl,
    .get_optmin = IPT_TEMP_BASE_CTL,
    .get_optmax = IPT_TEMP_GET_MAX,
    .get = nat_ipt_get_ctl
};

static struct nf_sockopt_ops *ipt_sockopts = NULL;
static uint32_t snat_seq = 0;
static uint32_t hw_nat_ipt_seq[NAT_HW_NUM] = {0};
static uint32_t hw_nat_pip_idx[NAT_HW_NUM] = {0};
static uint8_t *gbuffer, *sbuffer;
static unsigned int glen, slen;
static struct ipt_replace old_replace;

static void
nat_ipt_del(struct ipt_replace ireplace)
{
    int i, j;
    struct ipt_entry *gentry = NULL;
    struct ipt_entry *sentry = NULL;
    struct xt_entry_target *gtarget = NULL;
    struct xt_entry_target *starget = NULL;
    struct nf_nat_ipv4_multi_range_compat *grange = NULL;
    struct nf_nat_ipv4_multi_range_compat *srange = NULL;
    uint8_t *gptr, *sptr;
	unsigned int oldnum = ireplace.num_counters;
	unsigned int seq = 1;
    gptr = gbuffer;
    sptr = sbuffer;

    HNAT_PRINTK("into nat_ipt_del\n");
    for (i = oldnum; i >= 0; i--)//NF_NAT_INIT_ENTRIES_NUM; i--)
    {
        gentry = (struct ipt_entry *)gptr;
        sentry = (struct ipt_entry *)sptr;
        gtarget = (struct xt_entry_target *)((uint8_t *) gentry + gentry->target_offset);
        starget = (struct xt_entry_target *)((uint8_t *) sentry + sentry->target_offset);
        grange = (struct nf_nat_ipv4_multi_range_compat *)((uint8_t *) gtarget + sizeof (*gtarget));
        srange = (struct nf_nat_ipv4_multi_range_compat *)((uint8_t *) starget + sizeof (*starget));

        HNAT_PRINTK("(%d)isis_nat_del name %s:%s#####(%x:%x %x)###\n",
                    i, gtarget->u.user.name, starget->u.user.name,
                    gentry->ip.src.s_addr, gentry->ip.dst.s_addr,
                    grange->range[0].min.all);

        if (strcmp(gtarget->u.user.name, starget->u.user.name))
        {
            /*if (!strcmp(gtarget->u.user.name, "DNAT")) {
                if (gentry->ip.src.s_addr || !gentry->ip.dst.s_addr
                    || grange->range[0].min.all)
                    return;
                goto delete;
            } else */
            if (!strcmp(gtarget->u.user.name, "SNAT"))
            {
                if (!gentry->ip.src.s_addr || gentry->ip.dst.s_addr
                        || grange->range[0].min.all)
                    return;
                goto delete;
            }
            return;
        } /*else if (!strcmp(gtarget->u.user.name, "DNAT")) {
            if (memcmp(gentry, sentry, gentry->next_offset)) {
                if (gentry->ip.src.s_addr || !gentry->ip.dst.s_addr
                    || grange->range[0].min.all)
                    return;
                goto delete;
            }
        } */else if (!strcmp(gtarget->u.user.name, "SNAT"))
        {
            if (memcmp(gentry, sentry, gentry->next_offset))
            {
                if (!gentry->ip.src.s_addr || gentry->ip.dst.s_addr
                        || grange->range[0].min.all)
                    return;
                goto delete;
            }
        }
        gptr += gentry->next_offset;
        sptr += gentry->next_offset;
        if(!strcmp(gtarget->u.user.name, "SNAT"))
        {
            seq++;
        }
    }
    HNAT_PRINTK("NONE to delete\n");
    return;

delete:
    HNAT_PRINTK("READY to delete one\n");
    for (j = 0; j < NAT_HW_NUM; j++)
    {
        HNAT_PRINTK("ready [%d] (hw)%x:(sw)%x######\n",
                    j, hw_nat_ipt_seq[j], seq);
        if (hw_nat_ipt_seq[j] == seq)
        {
            if(nat_hw_del_by_index(j) != 0)
            {
                return;
            }
            //public_ip_del(hw_nat_pip_idx[j]);
        }
    }

    for(i = 0; i < NAT_HW_NUM; i++)
    {
        if(hw_nat_ipt_seq[i] > seq)
        {
            hw_nat_ipt_seq[i]--;
        }
        else if(hw_nat_ipt_seq[i] == seq)
        {
            hw_nat_ipt_seq[i]=0;
        }
    }

    return;
}

static void
nat_ipt_to_hw_entry(struct ipt_entry *e,
                    nat_entry_t *nat)
{
#define P_ANY  0
#define P_TCP  6
#define P_UDP  17

    struct ipt_entry_target *t = (struct ipt_entry_target *)ipt_get_target(e);

    const struct nf_nat_ipv4_multi_range_compat *mr =
        (struct nf_nat_ipv4_multi_range_compat *)(t->data);
    const struct nf_nat_range *range = &mr->range[0];

    uint32_t sip = ntohl(e->ip.src.s_addr);
    uint32_t pip = ntohl(range->min_ip);
    uint16_t proto = e->ip.proto;

    memset((void *) nat, 0, sizeof (nat_entry_t));

    nat->src_addr = sip;
    nat->trans_addr = pip;

    if (proto == P_TCP)
    {
        nat->flags = FAL_NAT_ENTRY_PROTOCOL_TCP;
    }
    else if (proto == P_UDP)
    {
        nat->flags = FAL_NAT_ENTRY_PROTOCOL_UDP;
    }
    else if (proto == P_ANY)
    {
        nat->flags = FAL_NAT_ENTRY_PROTOCOL_ANY;
    }
}

static int
nat_ipt_hw_add(nat_entry_t *nat)
{
	uint32_t index = 0;
    if(nat_hw_add(nat) != 0)
    {
        return -1;
    }

    hw_nat_ipt_seq[nat->entry_id] = snat_seq;
    HNAT_PRINTK("###nat_ipt_hw_add hw_nat_ipt_seq[%d]:%d###\n",
                nat->entry_id, snat_seq);

    hw_nat_pip_idx[nat->entry_id] = nat->trans_addr;

    if(nat_hw_prv_base_can_update())
    {
        nat_hw_prv_base_set(nat->src_addr);
        nat_hw_prv_base_update_disable();
    }

    if(nat_hw_pub_ip_add(nat->trans_addr, &index)!= 0)
    {
        return -1;
    }

    return 0;
}

static int
nat_ipt_hw_port_range_add(nat_entry_t *nat,
                          uint16_t port_start, uint16_t port_end,
                          struct xt_multiport *xport)
{
    unsigned int i;

    nat->flags |= FAL_NAT_ENTRY_PORT_CHECK;

    if(xport)
    {
        //some discontinuous ports
        for (i = 0; i < xport->count; i++)
        {

            nat->port_num = xport->ports[i];
            nat->port_range = 1;

            if(nat_ipt_hw_add(nat))
            {
                HNAT_PRINTK("isis_nat_add(xport:%d) fail!\n", nat->port_num);
                return -1;
            }

            HNAT_PRINTK("(1)isis_nat_add(xport:%d) success\n", nat->port_num);
        }
    }
    else
    {
        //one port or port range
        uint16_t port_min, port_max;

        for (i = port_start; i <= port_end; i+= NAT_HW_PORT_RANGE_MAX)
        {
            port_min = i;
            if((port_end-port_min)>(NAT_HW_PORT_RANGE_MAX-1))
            {
                port_max = port_min+(NAT_HW_PORT_RANGE_MAX-1);
            }
            else
            {
                port_max = port_end;
            }

            nat->port_num = port_min;
            nat->port_range = (port_max - port_min + 1);

            if(nat_ipt_hw_add(nat))
            {
                HNAT_PRINTK("isis_nat_add(range port:%d~%d) fail!\n",
                            port_min, port_max);
                return -1;
            }

            HNAT_PRINTK("(2)isis_nat_add(range port:%d~%d) success\n", port_min, port_max);
        }
    }

    return 0;
}

static int
nat_ipt_check_none_matches(struct ipt_entry *e)
{
    nat_entry_t nat = {0};

    nat_ipt_to_hw_entry(e, &nat);

    if(nat_ipt_hw_add(&nat) != 0)
    {
        HNAT_PRINTK("(1)isis_nat_add(none port)fail!\n");
        return -1;
    }

    HNAT_PRINTK("(1)isis_nat_add(none port) success\n");

    return 0;
}
static int
nat_ipt_check_matches(struct ipt_entry_match *m,
                      struct ipt_entry *e,
                      unsigned int *j)
{
    int ret = 0;

    nat_entry_t nat = {0};
    uint16_t port_start = 0, port_end = 0;
    struct xt_multiport *xport = NULL;

    if(strcmp(m->u.user.name, "udp") == 0)
    {
        struct xt_udp *udpinfo = (struct xt_udp *)m->data;
        port_start = udpinfo->spts[0];
        port_end = udpinfo->spts[1];

    }
    else if(strcmp(m->u.user.name, "tcp") == 0)
    {
        struct xt_tcp *tcpinfo = (struct xt_tcp *)m->data;
        port_start = tcpinfo->spts[0];
        port_end = tcpinfo->spts[1];

    }
    else if(strcmp(m->u.user.name, "multiport") == 0)
    {
        struct xt_multiport xport_data = {0};
		struct ipt_entry_target *t = ipt_get_target(e);
        xport = &xport_data;

        if(t->u.user.revision == 0)
        {
            xport = (struct xt_multiport *)m->data;

        }
        else if(t->u.user.revision == 1)
        {
            const struct xt_multiport_v1 *xportv1 =
                (struct xt_multiport_v1 *)m->data;
            memcpy(xport->ports, xportv1->ports, sizeof(xportv1->ports));
            xport->count = xportv1->count;
        }

        if(xport->flags != XT_MULTIPORT_SOURCE)
        {
            memset(xport->ports, 0, sizeof(xport->ports));
        }

    }
    else
    {
        (*j)++ ;
        HNAT_PRINTK("###no support matches m->u.user.name:%s\n",
                    m->u.user.name);
        return -1;
    }

    nat_ipt_to_hw_entry(e, &nat);
    ret = nat_ipt_hw_port_range_add(&nat, port_start, port_end, xport);

    (*j)++ ;

    return ret;
}

//check netmask !=32
#define NAT_IPT_RULE_IS_FOR_NAPT(e)  (((e)->ip.smsk.s_addr) != 0xffffffff)
#define NAT_IPT_RULE_IS_NONE_MATCHES(e)  (((e)->target_offset) == \
                                            (sizeof(struct ipt_entry)))

static int
nat_ipt_find_check_entry(struct ipt_entry *e, unsigned int underflow,
                         unsigned int *i)
{
    int ret = 0;
    static uint16_t next_offset = 0;
	struct ipt_entry_target *t = ipt_get_target(e);

    if(*i == 0)
    {
        snat_seq = 0;
        next_offset = e->next_offset;
    }
    else
    {
        next_offset += e->next_offset;
    }

    if (!strcmp(t->u.user.name, "SNAT"))
    {
        ++snat_seq;

        if(NAT_IPT_RULE_IS_FOR_NAPT(e))
        {
            HNAT_PRINTK("this ipt rule only for HW napt offload\n");

        }
        else
        {
            /*for basic nat offload*/
            HNAT_PRINTK("[%d]next_offset:%d underflow:%d\n",
                        *i, next_offset, underflow);

            if(next_offset == underflow)  //new one
            {

                if(NAT_IPT_RULE_IS_NONE_MATCHES(e))
                {
                    /*none matches*/
                    ret = nat_ipt_check_none_matches(e);

                }
                else
                {
                    unsigned int j = 0;
                    /*iterate matches*/
                    ret = IPT_MATCH_ITERATE(e, nat_ipt_check_matches, e, &j);
                }
            }
        }
    }

    (*i)++ ;

    return ret;
}

static void
nat_ipt_data_cleanup(void)
{
    if (gbuffer)
        kfree(gbuffer);

    gbuffer = NULL;

    if (sbuffer)
        kfree(sbuffer);

    sbuffer = NULL;
}

static void
nat_ipt_data_init(void)
{
    /*alloc initial set buffer*/
    sbuffer = kmalloc(IPT_BUFFER_INIT_LEN, GFP_ATOMIC);

    if(sbuffer)
    {
        memset(sbuffer, 0, IPT_BUFFER_INIT_LEN);
        slen = IPT_BUFFER_INIT_LEN;
    }
    else
    {
        HNAT_PRINTK("%s sbuffer memory allocate fail\n", __func__);
    }

    /*alloc initial get buffer*/
    gbuffer = kmalloc(IPT_BUFFER_INIT_LEN, GFP_ATOMIC);

    if(gbuffer)
    {
        memset(gbuffer, 0, IPT_BUFFER_INIT_LEN);
        glen = IPT_BUFFER_INIT_LEN;
    }
    else
    {
        HNAT_PRINTK("%s gbuffer memory allocate fail\n", __func__);
    }


    /*set initial underflow: nf_nat_rule.c*/
    memset(&old_replace, 0, sizeof (old_replace));

    /*record ipt rule(SNAT) sequence for hw nat*/
    memset(hw_nat_ipt_seq, 0, NAT_HW_NUM);

    /*record ipt rule(SNAT) pubip index for hw nat*/
    memset(hw_nat_pip_idx, 0, NAT_HW_NUM);
}

static void
nat_ipt_flush(void)
{
    napt_hw_flush();

    nat_hw_flush();

    nat_ipt_data_cleanup();
    nat_ipt_data_init();

    HNAT_PRINTK("------(nat flush done)------\n");
}

static void
nat_ipt_add(struct ipt_replace ireplace)
{
    unsigned int i = 0;

    IPT_ENTRY_ITERATE(sbuffer,
                      ireplace.size,
                      nat_ipt_find_check_entry,
                      ireplace.underflow[NF_INET_POST_ROUTING],
                      &i);
}

static int
nat_ipt_hook_type_check(struct ipt_replace ireplace)
{
    int ret = -1;

    HNAT_PRINTK("------we only support SNAT-----\n");

    if((old_replace.underflow[NF_INET_POST_ROUTING]-
        old_replace.hook_entry[NF_INET_POST_ROUTING]) !=
        (ireplace.underflow[NF_INET_POST_ROUTING]-
        ireplace.hook_entry[NF_INET_POST_ROUTING]))
    {
        HNAT_PRINTK("------this is POSTROUTING(SNAT):yes!------\n");
        ret = 0;

    }
    else if ((old_replace.underflow[NF_INET_PRE_ROUTING]-
        old_replace.hook_entry[NF_INET_PRE_ROUTING]) !=
        (ireplace.underflow[NF_INET_PRE_ROUTING]-
        ireplace.hook_entry[NF_INET_PRE_ROUTING]))
    {
        HNAT_PRINTK("------this is PREROUTING(DNAT):no!------\n");

    }
    else if((old_replace.underflow[NF_INET_LOCAL_OUT]-
        old_replace.hook_entry[NF_INET_LOCAL_OUT]) !=
        (ireplace.underflow[NF_INET_LOCAL_OUT]-
        ireplace.hook_entry[NF_INET_LOCAL_OUT]))
    {
        HNAT_PRINTK("------this is OUTPUT:no!------\n");

    }
    else
    {
        HNAT_PRINTK("------this is UNKNOW:no!------\n");

    }

    return ret;
}

static void
nat_ipt_rules_cp_from_user(void **buf, unsigned int *buf_len,
                           void __user *user, unsigned int user_len)
{
    if((*buf == 0) || (user == 0))
    {
        return;
    }

    if (*buf_len < user_len)
    {
        if(*buf)
        {
            kfree(*buf);
            *buf = kmalloc(user_len, GFP_ATOMIC);
			if(*buf == NULL)
			{
				HNAT_PRINTK("%s memory allocate fail\n", __func__);
				return;
			}
            *buf_len = user_len;
        }
    }
    HNAT_PRINTK("(2)nat_ipt_rules_cp_from_user *buf:%x user:%x user_len:%d\n",
                (unsigned int)*buf, (unsigned int)user, user_len);
    copy_from_user(*buf, user, user_len);

    return;
}

static int
nat_ipt_set_ctl(struct sock *sk, int cmd, void __user * user, unsigned int len)
{
    struct ipt_replace ireplace;

	memset(&ireplace, 0, sizeof(ireplace));

    HNAT_PRINTK("NAT set hook\n");

    if (cmd != IPT_SO_SET_REPLACE)
        goto normal;

    copy_from_user(&ireplace, user, sizeof (ireplace));

    if (strcmp(ireplace.name, "nat")
            || (ireplace.num_entries == ireplace.num_counters))
    {
        HNAT_PRINTK("none NAT or no new entry %d", ireplace.num_entries);
        goto normal;
    }


    if (ireplace.num_entries == NF_NAT_INIT_ENTRIES_NUM)
    {
        nat_ipt_flush();
        goto normal;
    }

    if (nat_ipt_hook_type_check(ireplace) != 0)
    {
        goto normal;
    }

    nat_ipt_rules_cp_from_user((void **)&sbuffer, &slen,
                               (user + sizeof (ireplace)),
                               ireplace.size);

    if (ireplace.num_entries > ireplace.num_counters)
    {
        nat_ipt_add(ireplace);
    }
    else
    {
        nat_ipt_del(ireplace);
    }

normal:
    /*save old_replace for next hook type check*/
    old_replace = ireplace;

    return orgi_ipt_sockopts.set(sk, cmd, user, len);
}

static int
nat_ipt_get_ctl(struct sock *sk, int cmd, void __user * user, int *len)
{
    int k = orgi_ipt_sockopts.get(sk, cmd, user, len);

    if (cmd == IPT_SO_GET_ENTRIES)
    {

        struct ipt_get_entries entries;

        copy_from_user(&entries, user, sizeof (entries));

        nat_ipt_rules_cp_from_user((void **)&gbuffer, &glen,
                                   (user + sizeof (struct ipt_get_entries)),
                                   (*len - sizeof (entries)));
    }
    return k;
}


void
nat_ipt_sockopts_replace(void)
{
	int ret = 0;
    /*register an temp sockopts to find ipt_sockopts*/
    if((ret = nf_register_sockopt(&tmp_ipt_sockopts)) < 0) {
		return;
    }
    list_for_each_entry(ipt_sockopts, tmp_ipt_sockopts.list.next, list)
    {
        if (ipt_sockopts->set_optmin == IPT_BASE_CTL)
        {
			nat_sockopts_init = 1;
            break;
        }
    }
    nf_unregister_sockopt(&tmp_ipt_sockopts);
	if(!nat_sockopts_init)
		return;

    /*save orginal ipt_sockopts*/
    orgi_ipt_sockopts = *ipt_sockopts;

    /*replace ipt_sockopts with our opts*/
    ipt_sockopts->set = nat_ipt_set_ctl;
    ipt_sockopts->get = nat_ipt_get_ctl;
}

static void
nat_ipt_sockopts_restore(void)
{
    ipt_sockopts->set = orgi_ipt_sockopts.set;
    ipt_sockopts->get = orgi_ipt_sockopts.get;
}

void
nat_ipt_helper_init(void)
{
    nat_ipt_sockopts_replace();
    nat_ipt_data_init();
}

void
nat_ipt_helper_exit(void)
{
    nat_ipt_sockopts_restore();
    nat_ipt_data_cleanup();
    nat_hw_flush();
}

