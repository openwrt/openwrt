/*
 * Copyright (c) 2014, 2017, The Linux Foundation. All rights reserved.
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
 * @defgroup fal_sec FAL_SEC
 * @{
 */
#ifndef _FAL_SEC_H_
#define _FAL_SEC_H_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

#include "common/sw.h"
#include "fal/fal_type.h"

/* Exception: L2 */
#define FAL_SEC_EXP_UNKNOWN_L2_PROT                  1
#define FAL_SEC_EXP_PPPOE_WRONG_VER_TYPE             2
#define FAL_SEC_EXP_PPPOE_WRONG_CODE                 3
#define FAL_SEC_EXP_PPPOE_UNSUPPORTED_PPP_PROT       4
/* Exception: IPv4 */
#define FAL_SEC_EXP_IPV4_WRONG_VER                   5
#define FAL_SEC_EXP_IPV4_SMALL_IHL                   6
#define FAL_SEC_EXP_IPV4_WITH_OPTION                 7

#define FAL_SEC_EXP_IPV4_HDR_INCOMPLETE              8

#define FAL_SEC_EXP_IPV4_BAD_TOTAL_LEN               9
#define FAL_SEC_EXP_IPV4_DATA_INCOMPLETE             10

#define FAL_SEC_EXP_IPV4_FRAG                        11
#define FAL_SEC_EXP_IPV4_PING_OF_DEATH               12

#define FAL_SEC_EXP_IPV4_SNALL_TTL                   13
#define FAL_SEC_EXP_IPV4_UNK_IP_PROT                 14
#define FAL_SEC_EXP_IPV4_CHECKSUM_ERR                15
#define FAL_SEC_EXP_IPV4_INV_SIP                     16
#define FAL_SEC_EXP_IPV4_INV_DIP                     17
#define FAL_SEC_EXP_IPV4_LAND_ATTACK                 18
#define FAL_SEC_EXP_IPV4_AH_HDR_INCOMPLETE           19
#define FAL_SEC_EXP_IPV4_AH_HDR_CROSS_BORDER         20
#define FAL_SEC_EXP_IPV4_ESP_HDR_INCOMPLETE          21
/* Exception: IPv6 */
#define FAL_SEC_EXP_IPV6_WRONG_VER                   22
#define FAL_SEC_EXP_IPV6_HDR_INCOMPLETE              23
#define FAL_SEC_EXP_IPV6_BAD_PAYLOAD_LEN             24
#define FAL_SEC_EXP_IPV6_DATA_INCOMPLETE             25
#define FAL_SEC_EXP_IPV6_WITH_EXT_HDR                26
#define FAL_SEC_EXP_IPV6_SMALL_HOP_LIMIT             27
#define FAL_SEC_EXP_IPV6_INV_SIP                     28
#define FAL_SEC_EXP_IPV6_INV_DIP                     29
#define FAL_SEC_EXP_IPV6_LAND_ATTACK                 30
#define FAL_SEC_EXP_IPV6_FRAG                        31
#define FAL_SEC_EXP_IPV6_PING_OF_DEATH               32
#define FAL_SEC_EXP_IPV6_WITH_MORE_EXT_HDR           33
#define FAL_SEC_EXP_IPV6_UNK_LAST_NEXT_HDR           34
#define FAL_SEC_EXP_IPV6_MOBILITY_HDR_INCOMPLETE     35
#define FAL_SEC_EXP_IPV6_MOBILITY_HDR_CROSS_BORDER   36
#define FAL_SEC_EXP_IPV6_AH_HDR_INCOMPLETE           37
#define FAL_SEC_EXP_IPV6_AH_HDR_CROSS_BORDER         38
#define FAL_SEC_EXP_IPV6_ESP_HDR_INCOMPLETE          39
#define FAL_SEC_EXP_IPV6_ESP_HDR_CROSS_BORDER        40

#define FAL_SEC_EXP_IPV6_OTHER_EXT_HDR_INCOMPLETE    41
#define FAL_SEC_EXP_IPV6_OTHER_EXT_HDR_CROSS_BORDER  42

/* Exception: L4 */
#define FAL_SEC_EXP_TCP_HDR_INCOMPLETE               43
#define FAL_SEC_EXP_TCP_HDR_CROSS_BORDER             44
#define FAL_SEC_EXP_TCP_SMAE_SP_DP                   45
#define FAL_SEC_EXP_TCP_SMALL_DATA_OFFSET            46
#define FAL_SEC_EXP_TCP_FLAGS_0                      47
#define FAL_SEC_EXP_TCP_FLAGS_1                      48
#define FAL_SEC_EXP_TCP_FLAGS_2                      49
#define FAL_SEC_EXP_TCP_FLAGS_3                      50
#define FAL_SEC_EXP_TCP_FLAGS_4                      51
#define FAL_SEC_EXP_TCP_FLAGS_5                      52
#define FAL_SEC_EXP_TCP_FLAGS_6                      53
#define FAL_SEC_EXP_TCP_FLAGS_7                      54
#define FAL_SEC_EXP_TCP_CHECKSUM_ERR                 55
#define FAL_SEC_EXP_UDP_HDR_INCOMPLETE               56
#define FAL_SEC_EXP_UDP_HDR_CROSS_BORDER             57

#define FAL_SEC_EXP_UDP_SMAE_SP_DP                   58
#define FAL_SEC_EXP_UDP_BAD_LEN                      59
#define FAL_SEC_EXP_UDP_DATA_INCOMPLETE              60


#define FAL_SEC_EXP_UDP_CHECKSUM_ERR                 61
#define FAL_SEC_EXP_UDP_LITE_HDR_INCOMPLETE          62
#define FAL_SEC_EXP_UDP_LITE_HDR_CROSS_BORDER        63
#define FAL_SEC_EXP_UDP_LITE_SMAE_SP_DP              64
/* Other exception  */
#define FAL_SEC_EXP_UDP_LITE_CSM_COV_1_TO_7          65
#define FAL_SEC_EXP_UDP_LITE_CSM_COV_TOO_LONG        66
#define FAL_SEC_EXP_UDP_LITE_CSM_COV_CROSS_BORDER    67
#define FAL_SEC_EXP_UDP_LITE_CHECKSUM_ERR            68

/**/

#define FAL_SEC_EXP_FAKE_L2_PROT_ERR                 69
#define FAL_SEC_EXP_FAKE_MAC_HEADER_ERR              70


    typedef enum {
        /* define MAC layer related normalization items */
        FAL_NORM_MAC_RESV_VID_CMD = 0,
        FAL_NORM_MAC_INVALID_SRC_ADDR_CMD,

        /* define IP layer related normalization items */
        FAL_NORM_IP_INVALID_VER_CMD,
        FAL_NROM_IP_SAME_ADDR_CMD,
        FAL_NROM_IP_TTL_CHANGE_STATUS,
        FAL_NROM_IP_TTL_VALUE,

        /* define IP4 related normalization items */
        FAL_NROM_IP4_INVALID_HL_CMD,
        FAL_NROM_IP4_HDR_OPTIONS_CMD,
        FAL_NROM_IP4_INVALID_DF_CMD,
        FAL_NROM_IP4_FRAG_OFFSET_MIN_LEN_CMD,
        FAL_NROM_IP4_FRAG_OFFSET_MAX_LEN_CMD,
        FAL_NROM_IP4_INVALID_FRAG_OFFSET_CMD,
        FAL_NROM_IP4_INVALID_SIP_CMD,
        FAL_NROM_IP4_INVALID_DIP_CMD,
        FAL_NROM_IP4_INVALID_CHKSUM_CMD,
        FAL_NROM_IP4_INVALID_PL_CMD,
        FAL_NROM_IP4_DF_CLEAR_STATUS,
        FAL_NROM_IP4_IPID_RANDOM_STATUS,
        FAL_NROM_IP4_FRAG_OFFSET_MIN_SIZE,

        /* define IP4 related normalization items */
        FAL_NROM_IP6_INVALID_PL_CMD,
        FAL_NROM_IP6_INVALID_SIP_CMD,
        FAL_NROM_IP6_INVALID_DIP_CMD,

        /* define TCP related normalization items */
        FAL_NROM_TCP_BLAT_CMD,
        FAL_NROM_TCP_INVALID_HL_CMD,
        FAL_NROM_TCP_INVALID_SYN_CMD,
        FAL_NROM_TCP_SU_BLOCK_CMD,
        FAL_NROM_TCP_SP_BLOCK_CMD,
        FAL_NROM_TCP_SAP_BLOCK_CMD,
        FAL_NROM_TCP_XMAS_SCAN_CMD,
        FAL_NROM_TCP_NULL_SCAN_CMD,
        FAL_NROM_TCP_SR_BLOCK_CMD,
        FAL_NROM_TCP_SF_BLOCK_CMD,
        FAL_NROM_TCP_SAR_BLOCK_CMD,
        FAL_NROM_TCP_RST_SCAN_CMD,
        FAL_NROM_TCP_SYN_WITH_DATA_CMD,
        FAL_NROM_TCP_RST_WITH_DATA_CMD,
        FAL_NROM_TCP_FA_BLOCK_CMD,
        FAL_NROM_TCP_PA_BLOCK_CMD,
        FAL_NROM_TCP_UA_BLOCK_CMD,
        FAL_NROM_TCP_INVALID_CHKSUM_CMD,
        FAL_NROM_TCP_INVALID_URGPTR_CMD,
        FAL_NROM_TCP_INVALID_OPTIONS_CMD,
        FAL_NROM_TCP_MIN_HDR_SIZE,

        /* define UDP related normalization items */
        FAL_NROM_UDP_BLAT_CMD,
        FAL_NROM_UDP_INVALID_LEN_CMD,
        FAL_NROM_UDP_INVALID_CHKSUM_CMD,

        /* define ICMP related normalization items */
        FAL_NROM_ICMP4_PING_PL_EXCEED_CMD,
        FAL_NROM_ICMP6_PING_PL_EXCEED_CMD,
        FAL_NROM_ICMP4_PING_FRAG_CMD,
        FAL_NROM_ICMP6_PING_FRAG_CMD,
        FAL_NROM_ICMP4_PING_MAX_PL_VALUE,
        FAL_NROM_ICMP6_PING_MAX_PL_VALUE,
    }
    fal_norm_item_t;

	typedef struct {
		fal_fwd_cmd_t cmd; /* action for the exception */
		a_bool_t deacclr_en; /* 0 for disable and 1 for disable */
		a_bool_t l3route_only_en; /*host/network route 0: disable and 1: enable*/
		a_bool_t l2fwd_only_en; /*l2 forward 0: disable and 1: enable*/
		a_bool_t l3flow_en; /* 0 for disable and 1 for disable */
		a_bool_t l2flow_en; /* 0 for disable and 1 for disable */
		a_bool_t multicast_en; /* 0 for disable and 1 for disable */
	} fal_l3_excep_ctrl_t;

	typedef struct {
		a_uint8_t small_ip4ttl; /* small ttl value checking */
		a_uint8_t small_ip6hoplimit; /*small hoplimit value for check*/
	} fal_l3_excep_parser_ctrl;

#define TCP_FLAGS_MAX	8
	typedef struct {
		a_uint8_t tcp_flags[TCP_FLAGS_MAX]; /*flag for exception*/
		a_uint8_t tcp_flags_mask[TCP_FLAGS_MAX]; /*flag mask*/
	} fal_l4_excep_parser_ctrl;

enum {
	FUNC_SEC_L3_EXCEP_CTRL_SET = 0,
	FUNC_SEC_L3_EXCEP_CTRL_GET,
	FUNC_SEC_L3_EXCEP_PARSER_CTRL_SET,
	FUNC_SEC_L3_EXCEP_PARSER_CTRL_GET,
	FUNC_SEC_L4_EXCEP_PARSER_CTRL_SET,
	FUNC_SEC_L4_EXCEP_PARSER_CTRL_GET,
};

    sw_error_t
    fal_sec_norm_item_set(a_uint32_t dev_id, fal_norm_item_t item, void *value);

    sw_error_t
    fal_sec_norm_item_get(a_uint32_t dev_id, fal_norm_item_t item, void *value);

    sw_error_t
    fal_sec_l3_excep_ctrl_set(a_uint32_t dev_id, a_uint32_t excep_type, fal_l3_excep_ctrl_t *ctrl);

    sw_error_t
    fal_sec_l3_excep_ctrl_get(a_uint32_t dev_id, a_uint32_t excep_type, fal_l3_excep_ctrl_t *ctrl);

    sw_error_t
    fal_sec_l3_excep_parser_ctrl_set(a_uint32_t dev_id, fal_l3_excep_parser_ctrl *ctrl);

    sw_error_t
    fal_sec_l3_excep_parser_ctrl_get(a_uint32_t dev_id, fal_l3_excep_parser_ctrl *ctrl);

    sw_error_t
    fal_sec_l4_excep_parser_ctrl_set(a_uint32_t dev_id, fal_l4_excep_parser_ctrl *ctrl);

    sw_error_t
    fal_sec_l4_excep_parser_ctrl_get(a_uint32_t dev_id, fal_l4_excep_parser_ctrl *ctrl);

#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif                          /* _FAL_SEC_H_ */

/**
 * @}
 */

