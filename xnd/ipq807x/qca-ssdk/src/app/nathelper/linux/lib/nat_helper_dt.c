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
#include <linux/kthread.h>
#include <linux/udp.h>
#include <linux/rculist_nulls.h>
#include <net/netfilter/nf_conntrack_acct.h>

#include "../nat_helper.h"
#include "../napt_helper.h"
#include "../napt_acl.h"

#include "fal_type.h"
#include "fal_nat.h"

#include "nat_helper_dt.h"
#include "nat_helper_hsl.h"

#define NAPT_CT_POLLING_SEC         5
#define NPAT_CT_POLLING_QUOTA	256

extern int nat_sockopts_init;
extern uint32_t napt_set_default_route(fal_ip4_addr_t dst_addr, fal_ip4_addr_t src_addr);
#ifdef CONFIG_IPV6_HWACCEL
extern uint32_t napt_set_ipv6_default_route(void);
#endif
extern void nat_ipt_sockopts_replace(void);
extern void qcaswitch_hostentry_flush(void);

#define NAPT_BUFFER_HASH_SIZE        (NAPT_TABLE_SIZE)
#define NAPT_BUFFER_SIZE             ((NAPT_BUFFER_HASH_SIZE)*8)

static a_uint32_t napt_buffer_hash_size = NAPT_TABLE_SIZE;
static a_uint32_t napt_buffer_size = (NAPT_BUFFER_HASH_SIZE)*8;
static a_uint32_t napt_ct_hw_cnt = 0;
static a_uint8_t napt_thread_pending = 0;

a_uint32_t polling_quota = NPAT_CT_POLLING_QUOTA;

extern int nat_chip_ver;

struct napt_ct
{
    struct napt_ct *next;
    a_uint32_t ct_addr;
    a_uint64_t ct_packets;
    a_uint8_t in_hw;
    a_uint16_t hw_index;
    a_uint8_t deny;
	a_uint64_t last_jiffies;
};

struct napt_debug_counter
{
	a_uint32_t interate_cnt;
	a_uint32_t care_cnt;
	a_uint32_t thresh_low_cnt;
};

struct nhlist_head
{
    struct napt_ct *next;
};

static struct nhlist_head *ct_buf_hash_head = NULL;
static struct napt_ct *ct_buf = NULL;
static a_uint32_t ct_buf_ct_cnt = 0;
static struct napt_debug_counter napt_ct_debug_info;

int scan_period = NAPT_CT_POLLING_SEC;
int scan_enable = 1;
int napt_need_clean = 0;
/*pppoe dhcp siwtch pre-handle*/
int wan_switch = 0;


static a_int32_t
napt_hash_buf_init(struct napt_ct **hash, struct nhlist_head **hash_head)
{
    a_uint32_t hash_size = NAPT_BUFFER_HASH_SIZE;
    a_uint32_t buffer_size = NAPT_BUFFER_SIZE;

    *hash = (struct napt_ct *)kmalloc(sizeof(struct napt_ct)*buffer_size, GFP_ATOMIC);
    if(!(*hash))
    {
        HNAT_PRINTK("NAPT INIT ERROR! No Sufficient Memory!");
        return -1;
    }

    *hash_head = (struct nhlist_head *)kmalloc(sizeof(struct nhlist_head)*hash_size, GFP_ATOMIC);
    if(!(*hash_head))
    {
        HNAT_PRINTK("NAPT INIT ERROR! No Sufficient Memory!");
        kfree(*hash);
        return -1;
    }

    memset(*hash,0,sizeof(struct napt_ct)*buffer_size);
    memset(*hash_head,0,sizeof(struct nhlist_head)*hash_size);

    return 0;
}

static a_uint32_t
napt_ct_hash(a_uint32_t ct_addr)
{
	if(nf_athrs17_hnat_sync_counter_en == 0)
		return (((ct_addr) * (0x9e370001UL)) >> 22);
	else
		return (((ct_addr) * (0x9e370001UL)) >> 29);
}

static struct napt_ct *
napt_hash_add(a_uint32_t ct_addr, a_uint32_t *hash_cnt,
              struct napt_ct *hash, struct nhlist_head *hash_head)
{
    struct napt_ct *entry = 0,*last = 0,*node = 0;
    struct nhlist_head *head = 0;
    a_uint32_t hash_index = napt_ct_hash(ct_addr);
		a_uint32_t i = 0, j = 0, index = 0xffffffff;

    if(*hash_cnt >= napt_buffer_size)
    {
        return NULL;
    }
    head = &(hash_head[hash_index]);
    entry = head->next;

    while(entry)
    {
        if(ct_addr == entry->ct_addr)
        {
            return entry;
        }
        else
        {
            last = entry;
            entry = entry->next;
        }
    }
	/*find a valid position*/
	for (i = *hash_cnt; i < napt_buffer_size; i++) {
		if (hash[i].ct_addr == 0)
			break;
	}
	if (i >= napt_buffer_size) {
		for (j = 0; j < *hash_cnt; j++) {
			if (hash[j].ct_addr == 0)
				break;
		}
		if (j >= *hash_cnt)
			return NULL;
		index = j;
	} else {
		index = i;
	}

    node = &(hash[index]);
    node->ct_addr = ct_addr;
    node->ct_packets = 0;
    node->in_hw = 0;
    node->hw_index = 0;
    node->deny = 0;

    if(head->next == 0)
    {
        head->next = node;
    }
    else
    {
        last->next = node;
    }
    (*hash_cnt)++;

    return node;
}

static struct napt_ct *
napt_hash_find(a_uint32_t ct_addr, a_uint32_t *hash_cnt,
               struct napt_ct *hash, struct nhlist_head *hash_head)
{
    struct napt_ct *entry = 0;
    struct nhlist_head *head = 0;
    a_uint32_t hash_index = napt_ct_hash(ct_addr);

    if(*hash_cnt == 0)
    {
        return NULL;
    }
#if 0 /* prevent empty entries. */
    if(*hash_cnt >= NAPT_BUFFER_SIZE)
    {
        return NULL;
    }
#endif

    head = &(hash_head[hash_index]);

    if(head->next == 0)
    {
        return NULL;
    }
    entry = head->next;
    do
    {
        if(ct_addr == entry->ct_addr)
        {
            return entry;
        }

        entry = entry->next;
    }
    while(entry);

    return NULL;
}

static a_int32_t
napt_ct_buf_init(void)
{
    return napt_hash_buf_init(&ct_buf, &ct_buf_hash_head);
}

static void
napt_ct_buf_exit(void)
{
    if(ct_buf_hash_head)
        kfree(ct_buf_hash_head);

    if(ct_buf)
        kfree(ct_buf);
}

static void
napt_ct_buf_flush(void)
{
    ct_buf_ct_cnt = 0;
    memset(ct_buf,0,sizeof(struct napt_ct)*NAPT_BUFFER_SIZE);
    memset(ct_buf_hash_head,0,sizeof(struct nhlist_head)*NAPT_BUFFER_HASH_SIZE);
}

static a_uint32_t
napt_ct_cnt_get(void)
{
    return ct_buf_ct_cnt;
}

static struct napt_ct *
napt_ct_buf_ct_find(a_uint32_t ct_addr)
{
    return napt_hash_find(ct_addr, &ct_buf_ct_cnt,
                          ct_buf, ct_buf_hash_head);
}

static a_uint64_t
napt_ct_buf_pkts_get(struct napt_ct *napt_ct)
{
    return napt_ct->ct_packets;
}

static void
napt_ct_buf_pkts_update(struct napt_ct *napt_ct, a_uint64_t packets)
{
    napt_ct->ct_packets = packets;
}

static a_uint8_t
napt_ct_buf_deny_get(struct napt_ct *napt_ct)
{
    return napt_ct->deny;
}

static void
napt_ct_buf_deny_set(struct napt_ct *napt_ct, a_uint8_t deny)
{
    napt_ct->deny = deny;
}

static void
napt_ct_buf_deny_clear(struct napt_ct *napt_ct)
{
    napt_ct->deny = 0;
}

static a_uint8_t
napt_ct_buf_in_hw_get(struct napt_ct *napt_ct, a_uint16_t *hw_index)
{
    *hw_index = napt_ct->hw_index;
    return napt_ct->in_hw;
}

static void
napt_ct_buf_in_hw_set(struct napt_ct *napt_ct, a_uint16_t hw_index)
{
    napt_ct->in_hw = 1;
    napt_ct->hw_index = hw_index;
}

static void
napt_ct_buf_in_hw_clear(struct napt_ct *napt_ct)
{
    napt_ct->in_hw = 0;
    napt_ct->hw_index = 0;
}

static void
napt_ct_buf_ct_info_clear(struct napt_ct *napt_ct)
{
	a_uint32_t hash_index = napt_ct_hash(napt_ct->ct_addr);
	struct nhlist_head *head = 0;
	struct napt_ct *entry = 0, *last = 0;

	head = &ct_buf_hash_head[hash_index];
	entry = head->next;
	while (entry && entry != napt_ct) {
		last = entry;
		entry = entry->next;
	}
	if (last == 0)
		head->next = napt_ct->next;
	else
		last->next = napt_ct->next;
    napt_ct->ct_addr = 0;
    napt_ct->ct_packets = 0;
	napt_ct->deny = 0;
	napt_ct->next = NULL;
}

static struct napt_ct *
napt_ct_buf_ct_add(a_uint32_t ct_addr)
{
    struct napt_ct *napt_ct;
    napt_ct = napt_hash_add(ct_addr, &ct_buf_ct_cnt,
                            ct_buf, ct_buf_hash_head);

    if(napt_ct)
    {
        /*ct pkts initial*/
        napt_ct_buf_pkts_update(napt_ct, NAPT_CT_PKTS_GET(ct_addr));
    }

    return napt_ct;
}

#define NAPT_CT_PERMANENT_DENY 5
static a_uint32_t napt_ct_addr[NAPT_TABLE_SIZE] = {0};
a_uint32_t napt_cookie[NAPT_TABLE_SIZE*2] = {0};

a_uint32_t packet_thres_base = 50;
static a_uint64_t packets_bdir_total = 0;
static a_uint64_t packets_bdir_thres = 0;

static inline a_int32_t
before(a_uint64_t seq1, a_uint64_t seq2)
{
    return ((int64_t)(seq1-seq2) < 0);
}

static a_uint8_t
napt_ct_pkts_reach_thres(a_uint32_t ct_addr, struct napt_ct *napt_ct,
                         a_uint8_t pkts_sum)
{
    a_uint64_t packets_bdir_old = napt_ct_buf_pkts_get(napt_ct);
    a_uint64_t packets_bdir_new = NAPT_CT_PKTS_GET(ct_addr);

    if(pkts_sum)
    {
        if(packets_bdir_new > packets_bdir_old)
        {
            packets_bdir_total += (packets_bdir_new - packets_bdir_old);
        }
    }

    napt_ct_buf_pkts_update(napt_ct, packets_bdir_new);

    HNAT_PRINTK("<%s> ct:%x packets_bdir_old:%lld ==> packets_bdir_new:%lld, thresh:%lld\n",
                __func__, ct_addr, packets_bdir_old, packets_bdir_new, packets_bdir_thres);

    if(before((packets_bdir_old+packets_bdir_thres), packets_bdir_new))
    {
        return 1;
    }

    return 0;
}

a_uint8_t napt_dnat_flow_find_del(
		napt_entry_t *napt,
		fal_napt_entry_t *entry)
{
	fal_napt_entry_t tmp_entry;
	napt_entry_t tmp_napt;
	a_int32_t ret;

	memset(&tmp_entry, 0, sizeof(tmp_entry));
	memset(&tmp_napt, 0, sizeof(tmp_napt));
	tmp_napt = *napt;
	tmp_napt.src_addr = 0;
	ret = napt_hw_get(&tmp_napt, &tmp_entry);
	if(!ret) {
		napt_hw_del(&tmp_napt);
		*entry = tmp_entry;
		return 0;
	}
	return 1;
}

a_uint8_t napt_snat_flow_find_del(
		napt_entry_t *napt,
		fal_napt_entry_t *entry)
{
	fal_napt_entry_t tmp_entry;
	napt_entry_t tmp_napt;
	a_int32_t ret;

	memset(&tmp_entry, 0, sizeof(tmp_entry));
	memset(&tmp_napt, 0, sizeof(tmp_napt));
	tmp_napt = *napt;
	tmp_napt.trans_addr = 0;
	ret = napt_hw_get(&tmp_napt, &tmp_entry);
	if(!ret) {
		napt_hw_del(&tmp_napt);
		*entry = tmp_entry;
		return 0;
	}
	return 1;
}




static a_int32_t
napt_ct_hw_add(a_uint32_t ct_addr, a_uint16_t *hw_index)
{
    napt_entry_t napt = {0};
    a_uint32_t index, i, dcookie = 0, scookie = 0;
	fal_napt_entry_t entry;
	a_uint8_t ret = 0;
	sw_error_t rv = SW_OK;

    if (!ct_addr)
        return -1;

    NAPT_CT_TO_HW_ENTRY(ct_addr, &napt);

    if(nat_hw_pub_ip_add(napt.trans_addr, &index) == 0)
    {
        napt.trans_addr = index;

    }
    else
    {
        HNAT_ERR_PRINTK("####%s##nat_hw_pub_ip_add fail!\n", __func__);
        return -1;
    }
	if (((nat_chip_ver & 0xffff)>>8) == NAT_CHIP_VER_DESS) {
		ret = napt_dnat_flow_find_del(&napt, &entry);
		if(!ret) {
			dcookie = entry.flow_cookie;
		}
		ret = napt_snat_flow_find_del(&napt, &entry);
		if(!ret) {
			scookie = entry.flow_cookie;
		}
	}
	rv = napt_hw_add(&napt);

    if(rv == 0)
    {
        HNAT_PRINTK("%s: success entry_id:0x%x ct 0x%x\n", __func__, napt.entry_id, ct_addr);

        if(napt_ct_addr[napt.entry_id])
        {
            HNAT_ERR_PRINTK("fault: napt HW:%x can not be overwrited!\n",
                            napt.entry_id);

        }
        else
        {
            napt_ct_addr[napt.entry_id] = ct_addr;
            *hw_index = napt.entry_id;
            // Added from 1.0.7 for default route.
            napt_set_default_route(napt.dst_addr, napt.src_addr);
			if (((nat_chip_ver & 0xffff)>>8) == NAT_CHIP_VER_DESS) {
				i = napt.entry_id;
				napt_cookie[i*2] = dcookie;
				napt_cookie[i*2+1] = scookie;
			}
		napt_ct_hw_cnt++;

            return 0;
        }
    }
    else
    {
        HNAT_PRINTK("%s:fail rv:%d entry_id:%x(maybe full)\n",
                    __func__,rv, napt.entry_id);
        nat_hw_pub_ip_del(napt.trans_addr);
    }

    return -1;
}

static a_int32_t
napt_ct_hw_del (napt_entry_t *napt)
{
    if(napt_hw_del(napt)!= 0)
    {
        HNAT_ERR_PRINTK("%s: isis_napt_del fail\n", __func__);
        return -1;
    }
	napt_ct_hw_cnt--;
    if(nat_hw_pub_ip_del(napt->trans_addr) != 0)
    {
        HNAT_ERR_PRINTK("%s: public_ip_del fail\n", __func__);
        //return -1;
    }
    return 0;
}

void nat_helper_cookie_del(a_uint32_t hw_index)
{
	struct napt_ct *napt_ct = NULL;
	a_uint32_t ct_addr;
	ct_addr = napt_ct_addr[hw_index];
	if(ct_addr) {
		napt_ct = napt_ct_buf_ct_find(ct_addr);
	}
	if(napt_ct)
    {
        napt_ct_buf_in_hw_clear(napt_ct);

        if(napt_ct_buf_deny_get(napt_ct) != NAPT_CT_PERMANENT_DENY)
        {
            napt_ct_buf_ct_info_clear(napt_ct);
        }
		NAPT_CT_AGING_ENABLE(napt_ct_addr[hw_index]);
    }
	napt_ct_addr[hw_index] = 0;
	napt_cookie[hw_index*2] = 0;
	napt_cookie[hw_index*2+1] = 0;
	HNAT_INFO_PRINTK("nat_helper_cookie_del for index 0x%x\n", hw_index);
}


static a_int32_t
napt_ct_del(struct napt_ct *napt_ct, napt_entry_t *napt)
{
    a_uint16_t hw_index = napt->entry_id;

    HNAT_PRINTK("%s: 0x%x ct 0x%x\n", __FUNCTION__, hw_index, napt_ct_addr[hw_index]);

    if(napt_ct_hw_del(napt) != 0)
    {
        return -1;
    }

    NAPT_CT_AGING_ENABLE(napt_ct_addr[hw_index]);
    napt_ct_addr[hw_index] = 0;
	if(napt_cookie[hw_index*2]) {
		napt_hw_dnat_cookie_add(napt, napt_cookie[hw_index*2]);
		napt_cookie[hw_index*2] = 0;
	}
	if(napt_cookie[hw_index*2+1]) {
		napt_hw_snat_cookie_add(napt, napt_cookie[hw_index*2+1]);
		napt_cookie[hw_index*2+1] = 0;
	}

    if(napt_ct)
    {
        napt_ct_buf_in_hw_clear(napt_ct);

        if(napt_ct_buf_deny_get(napt_ct) != NAPT_CT_PERMANENT_DENY)
        {
            napt_ct_buf_ct_info_clear(napt_ct);
        }
    }

    return 0;
}

static a_int32_t
napt_ct_del_by_index (struct napt_ct *napt_ct, a_uint16_t hw_index)
{
    napt_entry_t napt = {0};

    if(napt_hw_get_by_index(&napt, hw_index) != 0)
    {
        return -1;
    }

    return napt_ct_del(napt_ct, &napt);
}

static a_int32_t
napt_ct_in_hw_sanity_check(struct napt_ct *napt_ct, a_uint16_t hw_index)
{
	a_uint16_t ct_hw_index = 0;
    if(!napt_ct)
    {
        HNAT_ERR_PRINTK("<%s>hw_index:%d error napt_ct can't find\n",
                        __func__, hw_index);
        return -1;
    }

    if(napt_ct_buf_in_hw_get(napt_ct, &ct_hw_index) == 0)
    {
        HNAT_ERR_PRINTK("<%s>hw_index:%d in_hw:0 error\n",
                        __func__, hw_index);
        return -1;
    }

    if(hw_index != ct_hw_index)
    {
        HNAT_ERR_PRINTK("<%s>hw_index:%d buf_hw_index:%d\n",
                        __func__, hw_index, ct_hw_index);
        return -1;
    }

    return 0;
}

void
napt_ct_hw_aging(void)
{
#define NAPT_AGEOUT_STATUS 1

    a_uint32_t ct_addr, cnt= 0;
    napt_entry_t napt = {0};


    if(napt_hw_first_by_age(&napt, NAPT_AGEOUT_STATUS) != 0)
    {
        return;
    }

    do
    {
        a_uint16_t hw_index = napt.entry_id;
		struct napt_ct *napt_ct = NULL;
        ct_addr = napt_ct_addr[hw_index];

        if(ct_addr)
        {
            napt_ct = napt_ct_buf_ct_find(ct_addr);
            if(napt_ct_in_hw_sanity_check(napt_ct, hw_index) != 0)
            {
                HNAT_ERR_PRINTK("<%s> napt_ct_in_hw_sanity_check fail\n", __func__);
                continue;
            }

            if((nf_athrs17_hnat_sync_counter_en == 0) && (napt_ct_pkts_reach_thres(ct_addr, napt_ct, 0)))
            {
                printk("<aging>set PERMANENT deny ct:%x\n", ct_addr);
                napt_ct_buf_deny_set(napt_ct, NAPT_CT_PERMANENT_DENY);
            }
        }
        else
        {
            HNAT_ERR_PRINTK("<aging> error: in_hw but ct = NULL hw_index:%x\n", hw_index);
        }

        napt_ct_del(napt_ct, &napt);
        HNAT_INFO_PRINTK("ct:%x aged!\n", ct_addr);
        cnt++;
        if ((cnt % polling_quota) == 0) {
            NAPT_CT_TASK_SLEEP(1);
        }

    }
    while(napt_hw_next_by_age(&napt, NAPT_AGEOUT_STATUS) != -1);

#if 0
    if(napt_hw_used_count_get() == 0)
    {
        nat_hw_prv_base_update_enable();
    }
#endif 

    return;
}

static a_uint32_t
napt_ct_counter_sync(a_uint32_t hw_index)
{
	napt_entry_t napt = {0};
	struct nf_conn *ct = NULL;
	struct nf_conn_counter *cct = NULL;
	a_uint64_t delta_jiffies = 0, now_jiffies;
	a_uint32_t ct_addr = napt_ct_addr[hw_index];
	struct napt_ct *napt_ct;

	if((nf_athrs17_hnat_sync_counter_en == 0) || (napt_ct_addr[hw_index] == 0) || (hw_index >= NAPT_TABLE_SIZE))
		return -1;

	ct = (struct nf_conn *)napt_ct_addr[hw_index];
	cct = (struct nf_conn_counter *)nf_conn_acct_find(ct);
	napt_ct = napt_ct_buf_ct_find(ct_addr);
	if (napt_ct) {
		now_jiffies = (a_uint64_t)get_jiffies_64();
		delta_jiffies = now_jiffies - napt_ct->last_jiffies;
		napt_ct->last_jiffies = now_jiffies;
	}
	
	if (!test_bit(IPS_FIXED_TIMEOUT_BIT, &ct->status)) {
		ct->timeout.expires += delta_jiffies;
	}

	if((cct != NULL) && (napt_hw_get_by_index(&napt, hw_index) == 0))
	{
		spin_lock_bh(&ct->lock);
		if ((ct->status & IPS_NAT_MASK) == IPS_SRC_NAT) {
			atomic64_add(napt.egress_packet, &cct[IP_CT_DIR_ORIGINAL].packets);
			atomic64_add(napt.egress_byte, &cct[IP_CT_DIR_ORIGINAL].bytes);
			atomic64_add(napt.ingress_packet, &cct[IP_CT_DIR_REPLY].packets);
			atomic64_add(napt.ingress_byte, &cct[IP_CT_DIR_REPLY].bytes);
		} else {
			atomic64_add(napt.ingress_packet, &cct[IP_CT_DIR_ORIGINAL].packets);
			atomic64_add(napt.ingress_byte, &cct[IP_CT_DIR_ORIGINAL].bytes);
			atomic64_add(napt.egress_packet, &cct[IP_CT_DIR_REPLY].packets);
			atomic64_add(napt.egress_byte, &cct[IP_CT_DIR_REPLY].bytes);
		}
		spin_unlock_bh(&ct->lock);
		HNAT_PRINTK("original packets:0x%llx  bytes:0x%llx\n",
				cct[IP_CT_DIR_ORIGINAL].packets, cct[IP_CT_DIR_ORIGINAL].bytes);
		HNAT_PRINTK("reply packets:0x%llx  bytes:0x%llx\n",
				cct[IP_CT_DIR_REPLY].packets, cct[IP_CT_DIR_REPLY].bytes);
	}

	return 0;
}

static a_int32_t
napt_ct_timer_update(a_uint32_t hw_index)
{
	struct nf_conn *ct = NULL;
	struct nf_conn_counter *cct = NULL;
	a_uint64_t delta_jiffies = 0, now_jiffies;
	a_uint32_t ct_addr = napt_ct_addr[hw_index];
	struct napt_ct *napt_ct;

	if((napt_ct_addr[hw_index] == 0) || (hw_index >= NAPT_TABLE_SIZE))
		return -1;

	ct = (struct nf_conn *)napt_ct_addr[hw_index];
	cct = (struct nf_conn_counter *)nf_conn_acct_find(ct);
	napt_ct = napt_ct_buf_ct_find(ct_addr);
	if (napt_ct) {
		now_jiffies = (a_uint64_t)get_jiffies_64();
		delta_jiffies = now_jiffies - napt_ct->last_jiffies;
		napt_ct->last_jiffies = now_jiffies;
	}

	if (!test_bit(IPS_FIXED_TIMEOUT_BIT, &ct->status)) {
		ct->timeout.expires += delta_jiffies;
	}

	return 0;
}

void napt_ct_counter_decrease(void)
{
	ct_buf_ct_cnt--;
}

#define NAPT_INVALID_CT_NEED_HW_CLEAR(hw_index)  \
                                ((napt_ct_valid[hw_index] == 0) && \
                                 (napt_ct_addr[hw_index] != 0))
static a_uint32_t
napt_ct_hw_sync(a_uint8_t napt_ct_valid[])
{
    a_uint16_t hw_index;
    a_uint32_t napt_ct_offload_cnt = 0;

    for(hw_index = 0; hw_index < NAPT_TABLE_SIZE; hw_index++)
    {
		#if 0
		napt_ct_counter_sync(hw_index);
		#endif

        if(NAPT_INVALID_CT_NEED_HW_CLEAR(hw_index))
        {

            a_uint32_t ct_addr = napt_ct_addr[hw_index];
            struct napt_ct *napt_ct = napt_ct_buf_ct_find(ct_addr);

			HNAT_INFO_PRINTK("should hw clear for  %d\n", hw_index);

            if(napt_ct_in_hw_sanity_check(napt_ct, hw_index) != 0)
            {
                HNAT_ERR_PRINTK("<%s> napt_ct_in_hw_sanity_check fail\n", __func__);
                continue;
            }

            if(napt_ct_del_by_index(napt_ct, hw_index) == 0)
            {
                napt_ct_buf_deny_clear(napt_ct);
            }
            else
            {
                HNAT_ERR_PRINTK("<napt_ct_hw_sync>hw_index:%d napt_hw_del_by_index fail\n",
                                hw_index);
            }
        }

        if(napt_ct_valid[hw_index])
        {
            napt_ct_offload_cnt++;
        }
    }

    return napt_ct_offload_cnt;
}

static void
napt_ct_frag_hw_yield(struct napt_ct *napt_ct, a_uint16_t hw_index)
{
    napt_entry_t napt = {0};

    /*os and hw are both traffic; hw offload giveup*/
    if(napt_hw_get_by_index(&napt, hw_index) == 0)
    {
        if(napt.status == 0xe)
        {
            a_uint8_t deny = napt_ct_buf_deny_get(napt_ct);
            napt_ct_buf_deny_set(napt_ct, (++deny));

            if(deny >= NAPT_CT_PERMANENT_DENY)
            {
                /*os service only*/
                HNAT_ERR_PRINTK("<napt_ct_frag_hw_yield> hw service deny\n");
                napt_ct_del(napt_ct, &napt);
            }

            //printk("<napt_ct_frag_hw_yield> deny:%d\n", deny);
        }
    }
}

#define NAPT_CT_IS_REUSED_BY_OS(in_hw, ct_addr)   ((in_hw) && \
                                    NAPT_CT_AGING_IS_ENABLE(ct_addr))
static a_int32_t
napt_ct_check_add_one(a_uint32_t ct_addr, a_uint8_t *napt_ct_valid)
{
    struct napt_ct *napt_ct = NULL;
    a_uint16_t hw_index;
    a_uint8_t in_hw;

    if((napt_ct = napt_ct_buf_ct_find(ct_addr)) == NULL)
    {
        if((napt_ct = napt_ct_buf_ct_add(ct_addr)) == NULL)
        {
            HNAT_ERR_PRINTK("<napt_ct_scan> error hash full\n");
            return -1;
        }
    }

    if(napt_ct_buf_deny_get(napt_ct) >= NAPT_CT_PERMANENT_DENY)
    {
        HNAT_INFO_PRINTK("<napt_ct_scan> ct:%x is PERMANENT deny\n",
               ct_addr);
        return -1;

    }
    else
    {
        if (napt_ct_pkts_reach_thres(ct_addr, napt_ct, 1))
        {
            if(napt_ct_buf_in_hw_get(napt_ct, &hw_index))
            {
                //printk("<napt_ct_scan> ct:%x* is exist\n", ct_addr);
				if(nf_athrs17_hnat_sync_counter_en == 0)
				{
                	HNAT_ERR_PRINTK("<napt_ct_scan> ct:%x* is exist\n", ct_addr);
                	napt_ct_frag_hw_yield(napt_ct, hw_index);
				}
            }
            else
            {
                if(napt_ct_hw_add(ct_addr, &hw_index) == 0)
                {
                    NAPT_CT_AGING_DISABLE(ct_addr);
	       napt_ct->last_jiffies = get_jiffies_64();

                    napt_ct_buf_in_hw_set(napt_ct, hw_index);
#ifdef NAT_TODO
                    ct->in_hnat = 1; /* contrack in HNAT now. */
#endif
                }
            }
        } else {
        	HNAT_INFO_PRINTK("can not reach thres for napt_ct=%pK\n", napt_ct);
        }

        in_hw = napt_ct_buf_in_hw_get(napt_ct, &hw_index);
        if(in_hw)
        {
            if(!NAPT_CT_IS_REUSED_BY_OS(in_hw, ct_addr))
            {
                napt_ct_valid[hw_index] = 1;
            }
        }
    }

    return 0;
}

static void
napt_ct_pkts_thres_calc_init(void)
{
    packets_bdir_total = 0;
    packets_bdir_thres = packet_thres_base;

}

a_uint64_t
uint64_div_uint32(a_uint64_t div, a_uint32_t base)
{
    register a_uint32_t i;
    a_uint64_t result;

    union
    {
        a_uint64_t n64[2];
        struct
        {
            a_uint32_t l0;// 0
            a_uint32_t h0;// 1
            a_uint32_t l1;// 2
            a_uint32_t h1;// 3
        } n32;
    } n;

    if(base == 0)
    {
        return  0;
    }

    if(div < base)
    {
        return 0;
    }

    n.n64[0] = div;
    n.n64[1] = 0;
    result = 0;
    i = 0;

    //if div is 32bits, set start from 32
    if(n.n32.h0 == 0)
    {
        n.n32.h0 = n.n32.l0;
        n.n32.l0 = 0;
        i = 32;
    }

    //left shift until highest bit
    for(; i<64; ++i)
    {
        if((n.n32.h0 & 0x80000000) == 0x80000000)
        {
            break;
        }
        else
        {
            n.n64[0] = n.n64[0] << 1;
        }
    }

    for (; i<64; ++i)
    {
        n.n64[1] = (n.n64[1] << 1) + (n.n64[0] >> 63);
        n.n64[0] = (n.n64[0] << 1);
        result = result << 1 ;

        if(n.n64[1] >= base)
        {
            n.n64[1] = n.n64[1]- base;
            ++result;
        }
    }

    return result;
}

static void
napt_ct_pkts_thres_calc(a_uint32_t cnt, a_uint32_t napt_ct_offload_cnt)
{
    a_uint64_t packets_bdir_avg = 0;
    a_uint64_t packets_bdir_thres_temp = 0;

    /*ct_avg_pkts* (1+ (ct_offload_cnt/ct_hw_max) )*/
    packets_bdir_avg = uint64_div_uint32(packets_bdir_total, cnt);
    packets_bdir_thres_temp = packets_bdir_avg +
                              uint64_div_uint32((packets_bdir_avg *(a_uint64_t)napt_ct_offload_cnt),
                                      NAPT_TABLE_SIZE);

    if(packets_bdir_thres_temp > packet_thres_base)
    {
        packets_bdir_thres = packets_bdir_thres_temp;
    }

    //HNAT_ERR_PRINTK("###<%s> total:%lld cnt:%d avg:%lld  threshold:%lld###\n", __func__,
    //    packets_bdir_total, cnt, packets_bdir_avg, packets_bdir_thres);
    HNAT_INFO_PRINTK("calc pkts avg:%lld offload_cnt:%d threshold:%lld\n",
                    packets_bdir_avg, napt_ct_offload_cnt, packets_bdir_thres);
}

#define NAPT_CT_SHOULD_CARE(ct) ((ct) && \
                                  NAPT_CT_TYPE_IS_NAT(ct) && \
                                  !NAPT_CT_TYPE_IS_NAT_ALG(ct) && \
                                  NAPT_CT_INTF_EXPECTED(ct) && \
                                  NAPT_CT_STATUS_IS_ESTAB(ct) &&\
                                  nat_hw_prv_base_is_match( \
                                            NAPT_CT_PRIV_IP_GET(ct)))
a_uint8_t napt_ct_valid_tbl[NAPT_TABLE_SIZE] = {0};
static a_int32_t
napt_ct_check_add(void)
{
	a_uint32_t ct_addr = 0;
	a_uint32_t ct_buf_valid_cnt = 0, care_cnt = 0, ct_cnt = 0;
	static a_uint32_t hash = 0;
	a_uint32_t iterate = 0;
	a_uint32_t napt_ct_offload_cnt = 0;
	a_uint16_t hw_index;

	napt_ct_pkts_thres_calc_init();

	NAPT_CT_LIST_LOCK();
	while((ct_addr = NAPT_CT_LIST_ITERATE(&hash, &iterate)))
	{
		ct_cnt++;
		if (NAPT_CT_SHOULD_CARE(ct_addr))
		{
			if(napt_ct_check_add_one(ct_addr, napt_ct_valid_tbl) != -1)
			{
				ct_buf_valid_cnt++;
			}
			care_cnt++;
			if (care_cnt >= polling_quota) {
				break;
			}
		}
	}

	NAPT_CT_LIST_UNLOCK();
	HNAT_INFO_PRINTK("ct_cnt=0x%x, care_cnt=0x%x\n", ct_cnt, care_cnt);


	if (!ct_addr) {
		napt_ct_offload_cnt = napt_ct_hw_sync(napt_ct_valid_tbl);
		napt_ct_pkts_thres_calc(ct_buf_valid_cnt, napt_ct_offload_cnt);
		memset(napt_ct_valid_tbl, 0, NAPT_TABLE_SIZE);
	}

	for(hw_index = 0; hw_index < NAPT_TABLE_SIZE; hw_index++) {
		if (nf_athrs17_hnat_sync_counter_en)
			napt_ct_counter_sync(hw_index);
		else
			napt_ct_timer_update(hw_index);
	}

	return ct_buf_valid_cnt;
}

static a_int32_t
napt_ct_add(a_uint32_t ct_addr, a_uint8_t *napt_ct_valid)
{
    struct napt_ct *napt_ct;

    if((napt_ct = napt_ct_buf_ct_add(ct_addr)) == NULL)
    {
        HNAT_ERR_PRINTK("<napt_ct_buffer_update> error hash full\n");
        return -1;
    }

    return 0;
}

static a_int32_t
napt_ct_buffer_ct_status_update(void)
{
    a_uint32_t ct_addr = 0;
    a_uint32_t hash = 0;
    a_uint32_t iterate = 0;

    NAPT_CT_LIST_LOCK();

    while((ct_addr = NAPT_CT_LIST_ITERATE(&hash, &iterate)))
    {
        if (NAPT_CT_SHOULD_CARE(ct_addr))
        {
            napt_ct_add(ct_addr, NULL);
        }
    }

    NAPT_CT_LIST_UNLOCK();

    return 0;
}

static void
napt_ct_buffer_hw_status_update(void)
{
    a_uint16_t hw_index;

    for(hw_index = 0; hw_index < NAPT_TABLE_SIZE; hw_index++)
    {
        a_uint32_t ct_addr = napt_ct_addr[hw_index];
        if(ct_addr)
        {
            struct napt_ct *napt_ct = napt_ct_buf_ct_find(ct_addr);
            if(napt_ct)
            {
                napt_ct_buf_in_hw_set(napt_ct, hw_index);

            }
            else
            {
                if(napt_ct_del_by_index(napt_ct, hw_index) != 0)
                {
                    HNAT_ERR_PRINTK("<%s>hw_index:%d napt_ct_del_by_index fail\n",
                                    __func__, hw_index);
                }
            }
        }
    }

    return;
}

static void
napt_ct_buffer_refresh(void)
{
    HNAT_INFO_PRINTK("napt_ct_buffer_refresh\n");

    napt_ct_buf_flush();

    napt_ct_buffer_ct_status_update();
    napt_ct_buffer_hw_status_update();
}

static void
napt_ct_buffer_refresh_check(a_uint32_t ct_buf_valid_cnt)
{
#define NAPT_CT_BUF_REFRESH_THRES             1000
    HNAT_INFO_PRINTK("ct_buffer_hash_cnt:%d cnt:%d max:%d\n",
                    napt_ct_cnt_get(), ct_buf_valid_cnt/2, NAPT_CT_BUF_REFRESH_THRES);

    if((napt_ct_cnt_get() - ct_buf_valid_cnt/2) > NAPT_CT_BUF_REFRESH_THRES)
    {
        napt_ct_buffer_refresh();
    }
}

static void
napt_ct_hw_exit(void)
{
    a_uint8_t napt_ct_valid[NAPT_TABLE_SIZE];

    /*set all ct invalid to cleanup*/
    memset(napt_ct_valid, 0, sizeof(napt_ct_valid));

    napt_ct_hw_sync(napt_ct_valid);
}

void
napt_ct_scan(void)
{
    a_uint32_t ct_buf_valid_cnt = 0;

    ct_buf_valid_cnt = napt_ct_check_add();

    napt_ct_buffer_refresh_check(ct_buf_valid_cnt);
}

void napt_wan_switch_prehandle(void)
{
	if (wan_switch) {
		napt_thread_pending = 1;
		napt_l3_status_set(0, A_FALSE);
		napt_ct_hw_exit();
		napt_hw_flush();
		qcaswitch_hostentry_flush();
		droute_del_acl_rules();
		ipv6_droute_del_acl_rules();
		if (nf_athrs17_hnat_wan_type == NF_S17_WAN_TYPE_PPPOE)
			pppoe_del_acl_rules();
		#ifdef MULTIROUTE_WR
		ip_conflict_del_acl_rules();
		#endif
		if_mac_cleanup();
		napt_l3_status_set(0, A_TRUE);
		napt_thread_pending = 0;
	}
}

static a_int32_t
napt_ct_init(void)
{
	if(nf_athrs17_hnat_sync_counter_en == 0)
	{
		napt_buffer_hash_size = NAPT_TABLE_SIZE;
		napt_buffer_size = NAPT_TABLE_SIZE*8;
	}
	else
	{
		napt_buffer_hash_size = 8;
		napt_buffer_size = napt_buffer_hash_size;
	}

    napt_hw_mode_init();

    if(napt_ct_buf_init() != 0)
    {
        HNAT_ERR_PRINTK("*****napt_ct_buf_init fail*******\n");
        return -1;
    }

    return 0;
}

static a_int32_t
napt_ct_exit(void)
{
    napt_hw_mode_cleanup();

    napt_ct_hw_exit();
    napt_ct_buf_exit();

    return 0;
}

static a_int32_t
napt_ct_scan_thread(void *param)
{
#define NAPT_CT_AGING_SEC           20
#define ARP_CHECK_AGING_SEC       40

    a_uint32_t times = (NAPT_CT_AGING_SEC/scan_period);
    a_uint32_t arp_check_time = (ARP_CHECK_AGING_SEC/scan_period);
    // a_bool_t l3_enable;

    if(napt_ct_init() != 0)
    {
        return 0;
    }

    while(1)
    {
		if(!nat_sockopts_init) {
			nat_ipt_sockopts_replace();
		}
		if (napt_thread_pending) {
			NAPT_CT_TASK_SLEEP(scan_period);
			continue;
		}

		if (scan_enable) {
			if((--times) == 0)
			{
				HNAT_PRINTK("[ct hw aging start]\n");
				napt_ct_hw_aging();
				HNAT_PRINTK("[ct hw aging end]\n");
				times = (NAPT_CT_AGING_SEC/scan_period);
			}

			if((--arp_check_time) == 0)
			{
				host_check_aging();
				arp_check_time = (ARP_CHECK_AGING_SEC/scan_period);
			}

			if (times != NAPT_CT_AGING_SEC/scan_period) {
				HNAT_PRINTK("[ct scan start]\n");
				napt_ct_scan();
				HNAT_PRINTK("[ct scan end]\n");
			}
		} else {
			if (napt_need_clean) {
				napt_need_clean = 0;
				napt_ct_hw_exit();
			}
		}

#ifdef ISISC /* only for S17c */
#ifdef CONFIG_IPV6_HWACCEL
		napt_set_ipv6_default_route();
#endif
#endif
        
		if (NAPT_CT_TASK_SHOULD_STOP()) {
			printk("should stop!\n");
			break;
		}

		NAPT_CT_TASK_SLEEP(scan_period);
	}

    napt_ct_exit();

    return 0;
}

void napt_helper_show(void)
{
	a_uint16_t hw_index;
	for(hw_index = 0; hw_index < NAPT_TABLE_SIZE; hw_index++) {
		if (napt_ct_addr[hw_index]) {
			printk("index[%d] exist: 0x%x\n", hw_index, napt_ct_addr[hw_index]);
		}
	}
	printk("current hardware offload count: 0x%x\n", napt_ct_hw_cnt);
	printk("interate:0x%x, care:0x%x, low_thresh=0x%x\n",
			napt_ct_debug_info.interate_cnt,
			napt_ct_debug_info.care_cnt,
			napt_ct_debug_info.thresh_low_cnt);
}

void
napt_helper_init(void)
{
    const char napt_thread_name[] = "napt_ct_scan";

    NAPT_CT_TASK_START(napt_ct_scan_thread, napt_thread_name);
}


void
napt_helper_exit(void)
{
    NAPT_CT_TASK_STOP();
}

