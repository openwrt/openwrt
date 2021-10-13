/*
 * Copyright (c) 2014, 2015, The Linux Foundation. All rights reserved.
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
 * @defgroup fal_nat FAL_NAT
 * @{
 */
#ifndef _FAL_NAT_H_
#define _FAL_NAT_H_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

#include "common/sw.h"
#include "fal/fal_type.h"


#define FAL_NAT_ENTRY_PROTOCOL_TCP         0x1
#define FAL_NAT_ENTRY_PROTOCOL_UDP         0x2
#define FAL_NAT_ENTRY_PROTOCOL_PPTP        0x4
#define FAL_NAT_ENTRY_PROTOCOL_ANY         0x8
#define FAL_NAT_ENTRY_TRANS_IPADDR_INDEX   0x10
#define FAL_NAT_ENTRY_PORT_CHECK           0x20
#define FAL_NAT_HASH_KEY_PORT              0x40
#define FAL_NAT_HASH_KEY_IPADDR            0x80


    /* NAT entry operation flags */
#define FAL_NAT_ENTRY_ID_EN                 0x1
#define FAL_NAT_ENTRY_SRC_IPADDR_EN         0x2
#define FAL_NAT_ENTRY_TRANS_IPADDR_EN       0x4
#define FAL_NAT_ENTRY_KEY_EN                0x8
#define FAL_NAT_ENTRY_PUBLIC_IP_EN          0x10
#define FAL_NAT_ENTRY_SOURCE_IP_EN          0x20
#define FAL_NAT_ENTRY_AGE_EN                0x40
#define FAL_NAT_ENTRY_SYNC_EN               0x80


    typedef struct
    {
        a_uint32_t     entry_id;
        a_uint32_t     flags;
        a_uint32_t     status;
        fal_ip4_addr_t src_addr;
        fal_ip4_addr_t dst_addr;
        a_uint16_t     src_port;
        a_uint16_t     dst_port;
        fal_ip4_addr_t trans_addr;
        a_uint16_t     trans_port;
        a_uint16_t     rsv;
        a_bool_t       mirror_en;
        a_bool_t       counter_en;
        a_uint32_t     counter_id;
        a_uint32_t     ingress_packet;
        a_uint32_t     ingress_byte;
        a_uint32_t     egress_packet;
        a_uint32_t     egress_byte;
        fal_fwd_cmd_t  action;
		a_uint32_t     load_balance;
		a_uint32_t     flow_cookie;
		a_uint32_t     vrf_id;
		a_uint32_t     aging_sync;
		a_bool_t       priority_en;
		a_uint32_t     priority_val;
    } fal_napt_entry_t;

	typedef struct
    {
        a_uint32_t     proto;  /*1 tcp; 2 udp*/
        fal_ip4_addr_t src_addr;
        fal_ip4_addr_t dst_addr;
        a_uint16_t     src_port;
        a_uint16_t     dst_port;
		a_uint32_t     flow_cookie;
    } fal_flow_cookie_t;

	typedef struct
    {
        a_uint32_t     proto;  /*1 tcp; 2 udp*/
        fal_ip4_addr_t src_addr;
        fal_ip4_addr_t dst_addr;
        a_uint16_t     src_port;
        a_uint16_t     dst_port;
		a_uint8_t     load_balance;
    } fal_flow_rfs_t;

    typedef struct
    {
        a_uint32_t     entry_id;
        a_uint32_t     flags;
        a_uint32_t     status;
        fal_ip4_addr_t src_addr;
        fal_ip4_addr_t trans_addr;
        a_uint16_t     port_num;
        a_uint16_t     port_range;
        a_uint32_t     slct_idx;
        a_bool_t       mirror_en;
        a_bool_t       counter_en;
        a_uint32_t     counter_id;
        a_uint32_t     ingress_packet;
        a_uint32_t     ingress_byte;
        a_uint32_t     egress_packet;
        a_uint32_t     egress_byte;
        fal_fwd_cmd_t  action;
		a_uint32_t     vrf_id;
    } fal_nat_entry_t;


    typedef enum
    {
        FAL_NAPT_FULL_CONE = 0,
        FAL_NAPT_STRICT_CONE,
        FAL_NAPT_PORT_STRICT,
        FAL_NAPT_SYNMETRIC,
    } fal_napt_mode_t;


    typedef struct
    {
        a_uint32_t     entry_id;
        fal_ip4_addr_t pub_addr;
    } fal_nat_pub_addr_t;


    sw_error_t
    fal_nat_add(a_uint32_t dev_id, fal_nat_entry_t * nat_entry);


    sw_error_t
    fal_nat_del(a_uint32_t dev_id, a_uint32_t del_mode, fal_nat_entry_t * nat_entry);


    sw_error_t
    fal_nat_get(a_uint32_t dev_id, a_uint32_t get_mode, fal_nat_entry_t * nat_entry);


    sw_error_t
    fal_nat_next(a_uint32_t dev_id, a_uint32_t get_mode, fal_nat_entry_t * nat_entry);


    sw_error_t
    fal_nat_counter_bind(a_uint32_t dev_id, a_uint32_t entry_id, a_uint32_t cnt_id, a_bool_t enable);


    sw_error_t
    fal_napt_add(a_uint32_t dev_id, fal_napt_entry_t * napt_entry);


    sw_error_t
    fal_napt_del(a_uint32_t dev_id, a_uint32_t del_mode, fal_napt_entry_t * napt_entry);


    sw_error_t
    fal_napt_get(a_uint32_t dev_id, a_uint32_t get_mode, fal_napt_entry_t * napt_entry);


    sw_error_t
    fal_napt_next(a_uint32_t dev_id, a_uint32_t next_mode, fal_napt_entry_t * napt_entry);


    sw_error_t
    fal_napt_counter_bind(a_uint32_t dev_id, a_uint32_t entry_id, a_uint32_t cnt_id, a_bool_t enable);


    sw_error_t
    fal_flow_add(a_uint32_t dev_id, fal_napt_entry_t * napt_entry);


    sw_error_t
    fal_flow_del(a_uint32_t dev_id, a_uint32_t del_mode, fal_napt_entry_t * napt_entry);


    sw_error_t
    fal_flow_get(a_uint32_t dev_id, a_uint32_t get_mode, fal_napt_entry_t * napt_entry);


    sw_error_t
    fal_flow_next(a_uint32_t dev_id, a_uint32_t next_mode, fal_napt_entry_t * napt_entry);


    sw_error_t
    fal_flow_counter_bind(a_uint32_t dev_id, a_uint32_t entry_id, a_uint32_t cnt_id, a_bool_t enable);


    sw_error_t
    fal_nat_status_set(a_uint32_t dev_id, a_bool_t enable);


    sw_error_t
    fal_nat_status_get(a_uint32_t dev_id, a_bool_t * enable);


    sw_error_t
    fal_nat_hash_mode_set(a_uint32_t dev_id, a_uint32_t mode);


    sw_error_t
    fal_nat_hash_mode_get(a_uint32_t dev_id, a_uint32_t * mode);


    sw_error_t
    fal_napt_status_set(a_uint32_t dev_id, a_bool_t enable);


    sw_error_t
    fal_napt_status_get(a_uint32_t dev_id, a_bool_t * enable);


    sw_error_t
    fal_napt_mode_set(a_uint32_t dev_id, fal_napt_mode_t mode);


    sw_error_t
    fal_napt_mode_get(a_uint32_t dev_id, fal_napt_mode_t * mode);


    sw_error_t
    fal_napt_mode_get(a_uint32_t dev_id, fal_napt_mode_t * mode);


    sw_error_t
    fal_nat_prv_base_addr_set(a_uint32_t dev_id, fal_ip4_addr_t addr);


    sw_error_t
    fal_nat_prv_base_addr_get(a_uint32_t dev_id, fal_ip4_addr_t * addr);

    sw_error_t
    fal_nat_prv_base_mask_set(a_uint32_t dev_id, fal_ip4_addr_t addr);

    sw_error_t
    fal_nat_prv_base_mask_get(a_uint32_t dev_id, fal_ip4_addr_t * addr);


    sw_error_t
    fal_nat_prv_addr_mode_set(a_uint32_t dev_id, a_bool_t map_en);


    sw_error_t
    fal_nat_prv_addr_mode_get(a_uint32_t dev_id, a_bool_t * map_en);


    sw_error_t
    fal_nat_pub_addr_add(a_uint32_t dev_id, fal_nat_pub_addr_t * entry);


    sw_error_t
    fal_nat_pub_addr_del(a_uint32_t dev_id, a_uint32_t del_mode, fal_nat_pub_addr_t * entry);


    sw_error_t
    fal_nat_pub_addr_next(a_uint32_t dev_id, a_uint32_t next_mode, fal_nat_pub_addr_t * entry);


    sw_error_t
    fal_nat_unk_session_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd);


    sw_error_t
    fal_nat_unk_session_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd);

    sw_error_t
    fal_nat_global_set(a_uint32_t dev_id, a_bool_t enable,
    	a_bool_t sync_cnt_enable, a_uint32_t portbmp);

	sw_error_t
    fal_flow_cookie_set(a_uint32_t dev_id, fal_flow_cookie_t * flow_cookie);

	sw_error_t
    fal_flow_rfs_set(a_uint32_t dev_id, a_uint8_t action, fal_flow_rfs_t * rfs);

#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif                          /* _FAL_NAT_H_ */

/**
 * @}
 */

