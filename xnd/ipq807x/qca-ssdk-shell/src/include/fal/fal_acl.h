/*
 * Copyright (c) 2014, 2016-2017, The Linux Foundation. All rights reserved.
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
 * @defgroup fal_acl FAL_ACL
 * @{
 */
#ifndef _FAL_ACL_H_
#define _FAL_ACL_H_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

#include "common/sw.h"
#include "fal/fal_type.h"


    /**
    @brief This enum defines the ACL rule type.
    */
    typedef enum {
        FAL_ACL_RULE_MAC = 0,   /**< include MAC, udf fields*/
        FAL_ACL_RULE_IP4,       /**< include MAC, IP4 and Tcp/Udp udf fields*/
        FAL_ACL_RULE_IP6,       /**< include MAC, IP6 and Tcp/Udp udf fields*/
        FAL_ACL_RULE_UDF,       /**< only include user defined fields*/
        FAL_ACL_RULE_BUTT,
    }
    fal_acl_rule_type_t;


    /**
    @brief This enum defines the ACL field operation type.
    */
    typedef enum
    {
        FAL_ACL_FIELD_MASK = 0, /**< match operation is mask*/
        FAL_ACL_FIELD_RANGE,    /**< match operation is range*/
        FAL_ACL_FIELD_LE,       /**< match operation is less and equal*/
        FAL_ACL_FIELD_GE,       /**< match operation is great and equal*/
        FAL_ACL_FIELD_NE,       /**<- match operation is not equal*/
        FAL_ACL_FIELD_OP_BUTT,
    } fal_acl_field_op_t;


    typedef enum
    {
        FAL_ACL_POLICY_ROUTE = 0,
        FAL_ACL_POLICY_SNAT,
        FAL_ACL_POLICY_DNAT,
        FAL_ACL_POLICY_RESERVE,
    } fal_policy_forward_t;

    typedef enum
    {
        FAL_ACL_COMBINED_NONE = 0,
        FAL_ACL_COMBINED_START,
        FAL_ACL_COMBINED_CONTINUE,
        FAL_ACL_COMBINED_END,
    } fal_combined_t;

    /**
    @brief This enum defines the ACL field operation type.
    */
    typedef enum
    {
        FAL_ACL_UDF_TYPE_L2 = 0, /**< */
        FAL_ACL_UDF_TYPE_L3,     /**< */
        FAL_ACL_UDF_TYPE_L4,     /**< */
        FAL_ACL_UDF_TYPE_L2_SNAP, /**< */
        FAL_ACL_UDF_TYPE_L3_PLUS, /**< */
        FAL_ACL_UDF_TYPE_BUTT,
    } fal_acl_udf_type_t;

    /**
    @brief This enum defines the ACL rule type.
    */
    typedef enum {
        FAL_ACL_UDF_NON_IP = 0,
        FAL_ACL_UDF_IP4,
        FAL_ACL_UDF_IP6,
        FAL_ACL_UDF_BUTT,
    }fal_acl_udf_pkt_type_t;

    typedef enum {
        FAL_ACL_DEST_PORT_BMP = 0, /*dest info is bitmap*/
        FAL_ACL_DEST_NEXTHOP, /*dest info is nexthop*/
        FAL_ACL_DEST_PORT_ID, /*dest info is port id*/
    }fal_acl_dest_type_t;

#define FAL_ACL_DEST_OFFSET(type,value) (((type)<<24)|(value))
#define FAL_ACL_DEST_TYPE(dest) (((dest)>>24)&0xff)
#define FAL_ACL_DEST_VALUE(dest) ((dest)&0xffffff)

#define    FAL_ACL_FIELD_MAC_DA         0
#define    FAL_ACL_FIELD_MAC_SA         1
#define    FAL_ACL_FIELD_MAC_ETHTYPE    2
#define    FAL_ACL_FIELD_MAC_TAGGED     3
#define    FAL_ACL_FIELD_MAC_UP         4
#define    FAL_ACL_FIELD_MAC_VID        5
#define    FAL_ACL_FIELD_IP4_SIP        6
#define    FAL_ACL_FIELD_IP4_DIP        7
#define    FAL_ACL_FIELD_IP6_LABEL      8
#define    FAL_ACL_FIELD_IP6_SIP        9
#define    FAL_ACL_FIELD_IP6_DIP        10
#define    FAL_ACL_FIELD_IP_PROTO       11
#define    FAL_ACL_FIELD_IP_DSCP        12
#define    FAL_ACL_FIELD_L4_SPORT       13
#define    FAL_ACL_FIELD_L4_DPORT       14
#define    FAL_ACL_FIELD_UDF            15
#define    FAL_ACL_FIELD_MAC_CFI        16
#define    FAL_ACL_FIELD_ICMP_TYPE      17
#define    FAL_ACL_FIELD_ICMP_CODE      18
#define    FAL_ACL_FIELD_TCP_FLAG       19
#define    FAL_ACL_FIELD_RIPV1          20
#define    FAL_ACL_FIELD_DHCPV4         21
#define    FAL_ACL_FIELD_DHCPV6         22
#define    FAL_ACL_FIELD_MAC_STAG_VID   23
#define    FAL_ACL_FIELD_MAC_STAG_PRI   24
#define    FAL_ACL_FIELD_MAC_STAG_DEI   25
#define    FAL_ACL_FIELD_MAC_STAGGED    26
#define    FAL_ACL_FIELD_MAC_CTAG_VID   27
#define    FAL_ACL_FIELD_MAC_CTAG_PRI   28
#define    FAL_ACL_FIELD_MAC_CTAG_CFI   29
#define    FAL_ACL_FIELD_MAC_CTAGGED    30
#define    FAL_ACL_FIELD_INVERSE_ALL    31
/*new add for hawkeye*/
#define    FAL_ACL_FIELD_POST_ROURING_EN    32
#define    FAL_ACL_FIELD_RES_CHAIN    	    33
#define    FAL_ACL_FIELD_FAKE_MAC_HEADER    34
#define    FAL_ACL_FIELD_SNAP    35
#define    FAL_ACL_FIELD_ETHERNET    36
#define    FAL_ACL_FIELD_IPV6    37
#define    FAL_ACL_FIELD_IP    38
#define    FAL_ACL_FIELD_VSI    39
#define    FAL_ACL_FIELD_PPPOE_SESSIONID    40
#define    FAL_ACL_FIELD_L3_FRAGMENT    41
#define    FAL_ACL_FIELD_AH_HEADER    42
#define    FAL_ACL_FIELD_ESP_HEADER    43
#define    FAL_ACL_FIELD_MOBILITY_HEADER    44
#define    FAL_ACL_FIELD_FRAGMENT_HEADER    45
#define    FAL_ACL_FIELD_OTHER_EXT_HEADER    46
#define    FAL_ACL_FIELD_L3_TTL    47
#define    FAL_ACL_FIELD_IPV4_OPTION    48
#define    FAL_ACL_FIELD_FIRST_FRAGMENT    49
#define    FAL_ACL_FIELD_L3_LENGTH    50
#define    FAL_ACL_FIELD_VSI_VALID    51
#define    FAL_ACL_FIELD_IP_PKT_TYPE    52

#define    FAL_ACL_FIELD_UDF0            53
#define    FAL_ACL_FIELD_UDF1            54
#define    FAL_ACL_FIELD_UDF2            55
#define    FAL_ACL_FIELD_UDF3            56

#define    FAL_ACL_FIELD_NUM    57


#define    FAL_ACL_ACTION_PERMIT        0
#define    FAL_ACL_ACTION_DENY          1
#define    FAL_ACL_ACTION_REDPT         2
#define    FAL_ACL_ACTION_RDTCPU        3
#define    FAL_ACL_ACTION_CPYCPU        4
#define    FAL_ACL_ACTION_MIRROR        5
#define    FAL_ACL_ACTION_MODIFY_VLAN   6
#define    FAL_ACL_ACTION_NEST_VLAN     7
#define    FAL_ACL_ACTION_REMARK_UP     8
#define    FAL_ACL_ACTION_REMARK_QUEUE  9
#define    FAL_ACL_ACTION_REMARK_STAG_VID     10
#define    FAL_ACL_ACTION_REMARK_STAG_PRI     11
#define    FAL_ACL_ACTION_REMARK_STAG_DEI     12
#define    FAL_ACL_ACTION_REMARK_CTAG_VID     13
#define    FAL_ACL_ACTION_REMARK_CTAG_PRI     14
#define    FAL_ACL_ACTION_REMARK_CTAG_CFI     15
#define    FAL_ACL_ACTION_REMARK_LOOKUP_VID   16
#define    FAL_ACL_ACTION_REMARK_DSCP         17
#define    FAL_ACL_ACTION_POLICER_EN          18
#define    FAL_ACL_ACTION_WCMP_EN             19
#define    FAL_ACL_ACTION_ARP_EN              20
#define    FAL_ACL_ACTION_POLICY_FORWARD_EN   21
#define    FAL_ACL_ACTION_BYPASS_EGRESS_TRANS 22
#define    FAL_ACL_ACTION_MATCH_TRIGGER_INTR  23
/*new add for hawkeye*/
#define    FAL_ACL_ACTION_ENQUEUE_PRI 25
#define    FAL_ACL_ACTION_INT_DP 26
#define    FAL_ACL_ACTION_SERVICE_CODE 27
#define    FAL_ACL_ACTION_CPU_CODE 28
#define    FAL_ACL_ACTION_SYN_TOGGLE 29
#define    FAL_ACL_ACTION_METADATA_EN 30


enum{
	FAL_ACL_BYPASS_IN_VLAN_MISS = 0,
	FAL_ACL_BYPASS_SOUCE_GUARD,
	FAL_ACL_BYPASS_MRU_MTU_CHECK,
	FAL_ACL_BYPASS_EG_VSI_MEMBER_CHECK = 8,
	FAL_ACL_BYPASS_EG_VLAN_TRANSLATION,
	FAL_ACL_BYPASS_EG_VLAN_TAG_CTRL = 10,
	FAL_ACL_BYPASS_FDB_LEARNING,
	FAL_ACL_BYPASS_FDB_REFRESH,
	FAL_ACL_BYPASS_L2_SECURITY,/*new address, station move, learn limit, hash full*/
	FAL_ACL_BYPASS_MANAGEMENT_FWD,
	FAL_ACL_BYPASS_L2_FWD = 15,
	FAL_ACL_BYPASS_IN_STP_CHECK,
	FAL_ACL_BYPASS_EG_STP_CHECK,
	FAL_ACL_BYPASS_SOURCE_FILTER,
	FAL_ACL_BYPASS_POLICYER,
	FAL_ACL_BYPASS_L2_EDIT = 20,/*VLAN tag edit*/
	FAL_ACL_BYPASS_L3_EDIT,/*Edit MAC address, PPPoE, IP address, TTL, DSCP, L4 port*/
	FAL_ACL_BYPASS_POST_ACL_CHECK_ROUTING,
	FAL_ACL_BYPASS_PORT_ISOLATION,
};


    /**
      * @brief This type defines the action in Acl rule.
      *   @details  Comments:
      *  It's a bit map type, we can access it through macro FAL_ACTION_FLG_SET,
      * FAL_ACTION_FLG_CLR and FAL_ACTION_FLG_TST.
    */
    typedef a_uint32_t fal_acl_action_map_t;

#define FAL_ACTION_FLG_SET(flag, action) \
    (flag) |= (0x1UL << (action))

#define FAL_ACTION_FLG_CLR(flag, action) \
    (flag) &= (~(0x1UL << (action)))

#define FAL_ACTION_FLG_TST(flag, action) \
    ((flag) & (0x1UL << (action))) ? 1 : 0


    /**
      * @brief This type defines the field in Acl rule.
      *   @details   Comments:
      *   It's a bit map type, we can access it through macro FAL_FIELD_FLG_SET,
      *   FAL_FIELD_FLG_CLR and FAL_FIELD_FLG_TST.
    */
    typedef a_uint32_t fal_acl_field_map_t[2];

#define FAL_FIELD_FLG_SET(flag, field) \
    ((flag[(field) / 32]) |= (0x1UL << ((field) % 32)))

#define FAL_FIELD_FLG_CLR(flag, field) \
    ((flag[(field) / 32]) &= (~(0x1UL << ((field) % 32))))

#define FAL_FIELD_FLG_TST(flag, field) \
    (((flag[(field) / 32]) & (0x1UL << ((field) % 32))) ? 1 : 0)

#define FAL_ACL_UDF_MAX_LENGTH 16

    /**
     * @brief This structure defines the Acl rule.
     *   @details  Fields description:
     *
     *
     *    vid_val - If vid_op equals FAL_ACL_FIELD_MASK it's vlan id field value.
     *     If vid_op equals FAL_ACL_FIELD_RANGE it's vlan id field low value. If
     *     vid_op equals other value it's the compared value.
     *
     *     vid_mask - If vid_op equals FAL_ACL_FIELD_MASK it's vlan id field mask.
     *     If vid_op equals FAL_ACL_FIELD_RANGE it's vlan id field high value. If vid_op
     *     equals other value it's meaningless.
     *
     *
     *     ip_dscp_val - It's eight bits field we can set any value between 0 - 255.
     *     ip_dscp_mask - It's eight bits field we can set any value between 0 - 255.
     *
     *
     *     src_l4port_val - If src_l4port_op equals FAL_ACL_FIELD_MASK it's layer four
     *     source port field value. If src_l4port_op equals FAL_ACL_FIELD_RANGE it's
     *     layer four source port field low value. If src_l4port_op equals other value
     *     it's the compared value.
     *
     *
     *     src_l4port_mask - If src_l4port_op equals FAL_ACL_FIELD_MASK it's layer four
     *     source port field mask. If src_l4port_op equals FAL_ACL_FIELD_RANGE it's
     *     layer four source port field high value. If src_l4port_op equals other value
     *     it's meaningless.
     *
     *
     *     dest_l4port_val - If dest_l4port_op equals FAL_ACL_FIELD_MASK it's layer four
     *     destination port field value. If dest_l4port_op equals FAL_ACL_FIELD_RANGE it's
     *     layer four source port field low value. If dest_l4port_op equals other value
     *     it's the compared value.
     *
     *
     *     dest_l4port_mask - If dest_l4port_op equals FAL_ACL_FIELD_MASK it's layer four
     *     source port field mask. If dest_l4port_op equals FAL_ACL_FIELD_RANGE it's
     *     layer four source port field high value. If dest_l4port_op equals other value
     *     it's meaningless.
     *
     *
     *     ports - If FAL_ACL_ACTION_REDPT bit is setted in action_flg it's redirect
     *     destination ports.
     *
     *
     *     dot1p - If FAL_ACL_ACTION_REMARK_DOT1P bit is setted in action_flg it's
     *     the expected dot1p value.
     *
     *
     *     queue - If FAL_ACL_ACTION_REMARK_QUEUE bit is setted in action_flg it's
     *     the expected queue value.
     *
     *
     *     vid - If FAL_ACL_ACTION_MODIFY_VLAN or FAL_ACL_ACTION_NEST_VLAN bit is
     *     setted in action_flg it's the expected vlan id value.
     */
    typedef struct
    {
        fal_acl_rule_type_t rule_type;
        fal_acl_field_map_t field_flg;

        /* fields of mac rule */
        fal_mac_addr_t     src_mac_val;
        fal_mac_addr_t     src_mac_mask;
        fal_mac_addr_t     dest_mac_val;
        fal_mac_addr_t     dest_mac_mask;
        a_uint16_t         ethtype_val;
        a_uint16_t         ethtype_mask;
        a_uint16_t         vid_val;
        a_uint16_t         vid_mask;
        fal_acl_field_op_t vid_op;
        a_uint8_t          tagged_val;
        a_uint8_t          tagged_mask;
        a_uint8_t          up_val;
        a_uint8_t          up_mask;
        a_uint8_t          cfi_val;
        a_uint8_t          cfi_mask;
        a_uint16_t         resv0;

        /* fields of enhanced mac rule*/
        a_uint8_t          stagged_val; /*for s17c : 0-untag, 1-tag, for hawkeye: 2-pritag, 3-utag+pritag, 4- untag+tag, 5-tag+pritag, 6-all*/
        a_uint8_t          stagged_mask;
        a_uint8_t          ctagged_val;
        a_uint8_t          ctagged_mask;
        a_uint16_t         stag_vid_val;
        a_uint16_t         stag_vid_mask;
        fal_acl_field_op_t stag_vid_op;
        a_uint16_t         ctag_vid_val;
        a_uint16_t         ctag_vid_mask;
        fal_acl_field_op_t ctag_vid_op;
        a_uint8_t          stag_pri_val;
        a_uint8_t          stag_pri_mask;
        a_uint8_t          ctag_pri_val;
        a_uint8_t          ctag_pri_mask;
        a_uint8_t          stag_dei_val;
        a_uint8_t          stag_dei_mask;
        a_uint8_t          ctag_cfi_val;
        a_uint8_t          ctag_cfi_mask;


        /* fields of ip4 rule */
        fal_ip4_addr_t      src_ip4_val;
        fal_ip4_addr_t      src_ip4_mask;
        fal_ip4_addr_t      dest_ip4_val;
        fal_ip4_addr_t      dest_ip4_mask;

        /* fields of ip6 rule */
        a_uint32_t         ip6_lable_val;
        a_uint32_t         ip6_lable_mask;
        fal_ip6_addr_t      src_ip6_val;
        fal_ip6_addr_t      src_ip6_mask;
        fal_ip6_addr_t      dest_ip6_val;
        fal_ip6_addr_t      dest_ip6_mask;

        /* fields of ip rule */
        a_uint8_t          ip_proto_val;
        a_uint8_t          ip_proto_mask;
        a_uint8_t          ip_dscp_val;
        a_uint8_t          ip_dscp_mask;

        /* fields of layer four */
        a_uint16_t         src_l4port_val;
        a_uint16_t         src_l4port_mask;
        fal_acl_field_op_t src_l4port_op;
        a_uint16_t         dest_l4port_val;
        a_uint16_t         dest_l4port_mask;
        fal_acl_field_op_t dest_l4port_op;
        a_uint8_t          icmp_type_val;
        a_uint8_t          icmp_type_mask;
        a_uint8_t          icmp_code_val;
        a_uint8_t          icmp_code_mask;
        a_uint8_t          tcp_flag_val;
        a_uint8_t          tcp_flag_mask;
        a_uint8_t          ripv1_val;
        a_uint8_t          ripv1_mask;
        a_uint8_t          dhcpv4_val;
        a_uint8_t          dhcpv4_mask;
        a_uint8_t          dhcpv6_val;
        a_uint8_t          dhcpv6_mask;

        /* user defined fields */
        fal_acl_udf_type_t udf_type;
        a_uint8_t udf_offset;
        a_uint8_t udf_len;
        a_uint8_t udf_val[FAL_ACL_UDF_MAX_LENGTH];
        a_uint8_t udf_mask[FAL_ACL_UDF_MAX_LENGTH];

        /* fields of action */
        fal_acl_action_map_t  action_flg;
        fal_pbmp_t            ports; /*high 8bits, 00-port bitmap, 01-nexthop, 10-vp*/
        a_uint32_t            match_cnt;
        a_uint16_t            vid;
        a_uint8_t             up;
        a_uint8_t             queue;
        a_uint16_t            stag_vid;
        a_uint8_t             stag_pri;
        a_uint8_t             stag_dei;
        a_uint16_t            ctag_vid;
        a_uint8_t             ctag_pri;
        a_uint8_t             ctag_cfi;
        a_uint16_t            policer_ptr;
        a_uint16_t            arp_ptr;
        a_uint16_t            wcmp_ptr;
        a_uint8_t             dscp;
        a_uint8_t             rsv;
        fal_policy_forward_t  policy_fwd;
        fal_combined_t    combined;

	/*new add match fields for hawkeye*/
        a_uint8_t pri; /*rule priority 0-7*/
        a_bool_t post_routing;
        a_uint8_t acl_pool;

	a_bool_t is_ip_val;
	a_uint8_t is_ip_mask;

	a_bool_t is_ipv6_val;
	a_uint8_t is_ipv6_mask;

	a_bool_t is_fake_mac_header_val;
	a_uint8_t is_fake_mac_header_mask;

	a_bool_t is_snap_val;
	a_uint8_t is_snap_mask;

	a_bool_t is_ethernet_val;
	a_uint8_t is_ethernet_mask;

	a_bool_t is_fragement_val;
	a_uint8_t is_fragement_mask;

	a_bool_t is_ah_header_val;
	a_uint8_t is_ah_header_mask;

	a_bool_t is_esp_header_val;
	a_uint8_t is_esp_header_mask;

	a_bool_t is_mobility_header_val;
	a_uint8_t is_mobility_header_mask;

	a_bool_t is_fragment_header_val;
	a_uint8_t is_fragment_header_mask;

	a_bool_t is_other_header_val;
	a_uint8_t is_other_header_mask;

	a_bool_t is_ipv4_option_val;
	a_uint8_t is_ipv4_option_mask;

	a_bool_t is_first_frag_val;
	a_uint8_t is_first_frag_mask;

	/*fields of VLAN rule*/
        a_bool_t vsi_valid;
        a_uint8_t vsi_valid_mask;
        a_uint8_t vsi; /*0-31*/
        a_uint8_t vsi_mask; /*0-31*/
        /*fields of L2 MISC rule*/
        a_uint16_t pppoe_sessionid;
        a_uint16_t pppoe_sessionid_mask;
        fal_acl_field_op_t icmp_type_code_op;
        /*fields of IP MISC rule*/
        a_uint8_t l3_ttl;
        a_uint8_t l3_ttl_mask;
        fal_acl_field_op_t l3_length_op;
        a_uint16_t l3_length;
        a_uint16_t l3_length_mask;
        a_uint16_t l3_pkt_type;
        a_uint16_t l3_pkt_type_mask;
        /*field of udf*/
        fal_acl_field_op_t udf0_op;
        a_uint16_t udf0_val;
        a_uint16_t udf0_mask;
        fal_acl_field_op_t udf1_op;
        a_uint16_t udf1_val;
        a_uint16_t udf1_mask;
        a_uint16_t udf2_val;
        a_uint16_t udf2_mask;
        a_uint16_t udf3_val;
        a_uint16_t udf3_mask;

        /*new add acl action for hawkeye*/
        a_uint32_t            bypass_bitmap;
        a_uint8_t             enqueue_pri;
        a_uint8_t             stag_fmt;
        a_uint8_t             ctag_fmt;
        a_uint8_t             int_dp;
        a_uint8_t             service_code;
        a_uint8_t             cpu_code;
        a_uint64_t            match_bytes;
        /*new add acl action for IPQ60xx*/
        a_uint8_t             dscp_mask;
        a_uint8_t             qos_res_prec;
    } fal_acl_rule_t;


    /**
    @brief This enum defines the ACL will work on which derection traffic.
    */
    typedef enum
    {
        FAL_ACL_DIREC_IN = 0,   /**<   Acl will work on ingressive traffic */
        FAL_ACL_DIREC_EG,       /**<   Acl will work on egressive traffic */
        FAL_ACL_DIREC_BOTH,     /**<    Acl will work on both ingressive and egressive traffic*/
    } fal_acl_direc_t;


    /**
    @brief This enum defines the ACL will work on which partiualr object.
    */
    typedef enum
    {
        FAL_ACL_BIND_PORT = 0,  /**<   Acl wil work on particular port and virtual port */
        FAL_ACL_BIND_PORTBITMAP = 1,  /**<   Acl wil work on port bitmap */
        FAL_ACL_BIND_SERVICE_CODE = 2,  /**<   Acl wil work on service code */
        FAL_ACL_BIND_L3_IF = 3,  /**<   Acl wil work on l3 interface */
    } fal_acl_bind_obj_t;

enum
{
	/*acl*/
	FUNC_ACL_LIST_CREAT = 0,
	FUNC_ACL_LIST_DESTROY,
	FUNC_ACL_RULE_ADD,
	FUNC_ACL_RULE_DELETE,
	FUNC_ACL_RULE_QUERY,
	FUNC_ACL_RULE_DUMP,
	FUNC_ACL_LIST_BIND,
	FUNC_ACL_LIST_UNBIND,
	FUNC_ACL_LIST_DUMP,
	FUNC_ACL_UDF_PROFILE_SET,
	FUNC_ACL_UDF_PROFILE_GET,
};


sw_error_t
fal_acl_list_creat(a_uint32_t dev_id, a_uint32_t list_id, a_uint32_t list_pri);

sw_error_t
fal_acl_list_destroy(a_uint32_t dev_id, a_uint32_t list_id);

sw_error_t
fal_acl_rule_add(a_uint32_t dev_id, a_uint32_t list_id, a_uint32_t rule_id, a_uint32_t rule_nr, fal_acl_rule_t * rule);

sw_error_t
fal_acl_rule_delete(a_uint32_t dev_id, a_uint32_t list_id, a_uint32_t rule_id, a_uint32_t rule_nr);

sw_error_t
fal_acl_rule_query(a_uint32_t dev_id, a_uint32_t list_id, a_uint32_t rule_id, fal_acl_rule_t * rule);

sw_error_t
fal_acl_list_bind(a_uint32_t dev_id, a_uint32_t list_id, fal_acl_direc_t direc, fal_acl_bind_obj_t obj_t, a_uint32_t obj_idx);

sw_error_t
fal_acl_list_unbind(a_uint32_t dev_id, a_uint32_t list_id, fal_acl_direc_t direc, fal_acl_bind_obj_t obj_t, a_uint32_t obj_idx);

sw_error_t
fal_acl_status_set(a_uint32_t dev_id, a_bool_t enable);

sw_error_t
fal_acl_status_get(a_uint32_t dev_id, a_bool_t * enable);

sw_error_t
fal_acl_list_dump(a_uint32_t dev_id);

sw_error_t
fal_acl_rule_dump(a_uint32_t dev_id);

sw_error_t
fal_acl_port_udf_profile_set(a_uint32_t dev_id, fal_port_t port_id, fal_acl_udf_type_t udf_type, a_uint32_t offset, a_uint32_t length);
sw_error_t
fal_acl_port_udf_profile_get(a_uint32_t dev_id, fal_port_t port_id, fal_acl_udf_type_t udf_type, a_uint32_t * offset, a_uint32_t * length);

sw_error_t
fal_acl_udf_profile_set(a_uint32_t dev_id, fal_acl_udf_pkt_type_t pkt_type,a_uint32_t udf_idx, fal_acl_udf_type_t udf_type, a_uint32_t offset);

sw_error_t
fal_acl_udf_profile_get(a_uint32_t dev_id, fal_acl_udf_pkt_type_t pkt_type,a_uint32_t udf_idx, fal_acl_udf_type_t *udf_type, a_uint32_t *offset);

sw_error_t
fal_acl_rule_active(a_uint32_t dev_id, a_uint32_t list_id, a_uint32_t rule_id, a_uint32_t rule_nr);
sw_error_t
fal_acl_rule_deactive(a_uint32_t dev_id, a_uint32_t list_id, a_uint32_t rule_id, a_uint32_t rule_nr);
sw_error_t
fal_acl_rule_src_filter_sts_set(a_uint32_t dev_id, a_uint32_t rule_id, a_bool_t enable);
sw_error_t
fal_acl_rule_src_filter_sts_get(a_uint32_t dev_id, a_uint32_t rule_id, a_bool_t* enable);


#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif                          /* _FAL_ACL_H_ */
/**
 * @}
 */
