/*
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * Copyright (c) 2025 Realtek Semiconductor Corp. All rights reserved.
 *
 * This software is a confidential and proprietary property of Realtek
 * Semiconductor Corp. Disclosure, reproduction, redistribution, in
 * whole or in part, of this work and its derivatives without express
 * permission is prohibited.
 *
 * Realtek Semiconductor Corp. reserves the right to update, modify, or
 * discontinue this software at any time without notice. This software is
 * provided as is and any express or implied warranties, including, but
 * not limited to, the implied warranties of merchantability and fitness for
 * a particular purpose are disclaimed. In no event shall Realtek
 * Semiconductor Corp. be liable for any direct, indirect, incidental,
 * special, exemplary, or consequential damages (including, but not limited
 * to, procurement of substitute goods or services; loss of use, data, or
 * profits; or business interruption) however caused and on any theory of
 * liability, whether in contract, strict liability, or tort (including
 * negligence or otherwise) arising in any way out of the use of this software,
 * even if advised of the possibility of such damage.
 */

#ifndef __RTKSW_API_ACL_H__
#define __RTKSW_API_ACL_H__

/*
 * Data Type Declaration
 */
#define RTKSW_FILTER_RAW_FIELD_NUMBER                8

#define ACL_DEFAULT_ABILITY                         0
#define ACL_DEFAULT_UNMATCH_PERMIT                  1

#define ACL_RULE_FREE                               0
#define ACL_RULE_INAVAILABLE                        1
#define ACL_RULE_CARETAG_MASK                       0x1F
#define FILTER_POLICING_MAX                         4
#define FILTER_LOGGING_MAX                          8
#define FILTER_PATTERN_MAX                          4

#define FILTER_ENACT_CVLAN_MASK         0x01
#define FILTER_ENACT_SVLAN_MASK         0x02
#define FILTER_ENACT_PRIORITY_MASK      0x04
#define FILTER_ENACT_POLICING_MASK      0x08
#define FILTER_ENACT_FWD_MASK           0x10
#define FILTER_ENACT_INTGPIO_MASK       0x20
#define FILTER_ENACT_INIT_MASK          0x3F

typedef enum rtksw_filter_act_cactext_e
{
    FILTER_ENACT_CACTEXT_VLANONLY=0,
    FILTER_ENACT_CACTEXT_BOTHVLANTAG,
    FILTER_ENACT_CACTEXT_TAGONLY,
    FILTER_ENACT_CACTEXT_END,


}rtksw_filter_act_cactext_t;

typedef enum rtksw_filter_act_ctagfmt_e
{
    FILTER_CTAGFMT_UNTAG=0,
    FILTER_CTAGFMT_TAG,
    FILTER_CTAGFMT_KEEP,
    FILTER_CTAGFMT_KEEP1PRMK,


}rtksw_filter_act_ctag_t;





#define RTKSW_MAX_NUM_OF_FILTER_TYPE                  5
#define RTKSW_MAX_NUM_OF_FILTER_FIELD                 8

#define RTKSW_DOT_1AS_TIMESTAMP_UNIT_IN_WORD_LENGTH   3UL
#define RTKSW_IPV6_ADDR_WORD_LENGTH                   4UL

#define FILTER_ENACT_CVLAN_TYPE(type)   (type - FILTER_ENACT_CVLAN_INGRESS)
#define FILTER_ENACT_SVLAN_TYPE(type)   (type - FILTER_ENACT_SVLAN_INGRESS)
#define FILTER_ENACT_FWD_TYPE(type)     (type - FILTER_ENACT_ADD_DSTPORT)
#define FILTER_ENACT_PRI_TYPE(type)     (type - FILTER_ENACT_PRIORITY)

#define RTKSW_FILTER_FIELD_USED_MAX                   8
#define RTKSW_FILTER_FIELD_INDEX(template, index)     ((template << 4) + index)


typedef enum rtksw_filter_act_enable_e
{
    /* CVLAN */
    FILTER_ENACT_CVLAN_INGRESS = 0,
    FILTER_ENACT_CVLAN_EGRESS,
    FILTER_ENACT_CVLAN_SVID,
    FILTER_ENACT_POLICING_1,

    /* SVLAN */
    FILTER_ENACT_SVLAN_INGRESS,
    FILTER_ENACT_SVLAN_EGRESS,
    FILTER_ENACT_SVLAN_CVID,
    FILTER_ENACT_POLICING_2,

    /* Policing and Logging */
    FILTER_ENACT_POLICING_0,

    /* Forward */
    FILTER_ENACT_COPY_CPU,
    FILTER_ENACT_DROP,
    FILTER_ENACT_ADD_DSTPORT,
    FILTER_ENACT_REDIRECT,
    FILTER_ENACT_MIRROR,
    FILTER_ENACT_TRAP_CPU,
    FILTER_ENACT_ISOLATION,

    /* QoS */
    FILTER_ENACT_PRIORITY,
    FILTER_ENACT_DSCP_REMARK,
    FILTER_ENACT_1P_REMARK,
    FILTER_ENACT_POLICING_3,

    /* Interrutp and GPO */
    FILTER_ENACT_INTERRUPT,
    FILTER_ENACT_GPO,

    /*VLAN tag*/
    FILTER_ENACT_EGRESSCTAG_UNTAG,
    FILTER_ENACT_EGRESSCTAG_TAG,
    FILTER_ENACT_EGRESSCTAG_KEEP,
    FILTER_ENACT_EGRESSCTAG_KEEPAND1PRMK,

    FILTER_ENACT_END,
} rtksw_filter_act_enable_t;


typedef struct
{
    rtksw_filter_act_enable_t actEnable[FILTER_ENACT_END];

    /* CVLAN acton */
    rtksw_uint32      filterCvlanVid;
    rtksw_uint32      filterCvlanIdx;
    /* SVLAN action */
    rtksw_uint32      filterSvlanVid;
    rtksw_uint32      filterSvlanIdx;

    /* Policing action */
    rtksw_uint32      filterPolicingIdx[FILTER_POLICING_MAX];

    /* Forwarding action */
    rtksw_portmask_t  filterPortmask;

    /* QOS action */
    rtksw_uint32      filterPriority;

    /*GPO*/
    rtksw_uint32      filterPin;

} rtksw_filter_action_t;

typedef struct rtksw_filter_flag_s
{
    rtksw_uint32 value;
    rtksw_uint32 mask;
} rtksw_filter_flag_t;

typedef enum rtksw_filter_care_tag_index_e
{
    CARE_TAG_CTAG = 0,
    CARE_TAG_STAG,
    CARE_TAG_PPPOE,
    CARE_TAG_IPV4,
    CARE_TAG_IPV6,
    CARE_TAG_TCP,
    CARE_TAG_UDP,
    CARE_TAG_ARP,
    CARE_TAG_RSV1,
    CARE_TAG_RSV2,
    CARE_TAG_ICMP,
    CARE_TAG_IGMP,
    CARE_TAG_LLC,
    CARE_TAG_RSV3,
    CARE_TAG_HTTP,
    CARE_TAG_RSV4,
    CARE_TAG_RSV5,
    CARE_TAG_DHCP,
    CARE_TAG_DHCPV6,
    CARE_TAG_SNMP,
    CARE_TAG_OAM,
    CARE_TAG_END,
} rtksw_filter_care_tag_index_t;

typedef struct rtksw_filter_care_tag_s
{
    rtksw_filter_flag_t tagType[CARE_TAG_END];
} rtksw_filter_care_tag_t;

typedef struct rtksw_filter_field rtksw_filter_field_t;

typedef struct
{
    rtksw_uint32 value[RTKSW_DOT_1AS_TIMESTAMP_UNIT_IN_WORD_LENGTH];
} rtksw_filter_dot1as_timestamp_t;

typedef enum rtksw_filter_field_data_type_e
{
    FILTER_FIELD_DATA_MASK = 0,
    FILTER_FIELD_DATA_RANGE,
    FILTER_FIELD_DATA_END ,
} rtksw_filter_field_data_type_t;

typedef struct rtksw_filter_ip_s
{
    rtksw_uint32 dataType;
    rtksw_uint32 rangeStart;
    rtksw_uint32 rangeEnd;
    rtksw_uint32 value;
    rtksw_uint32 mask;
} rtksw_filter_ip_t;

typedef struct rtksw_filter_mac_s
{
    rtksw_uint32 dataType;
    rtksw_mac_t value;
    rtksw_mac_t mask;
    rtksw_mac_t rangeStart;
    rtksw_mac_t rangeEnd;
} rtksw_filter_mac_t;

typedef rtksw_uint32 rtksw_filter_op_t;

typedef struct rtksw_filter_value_s
{
    rtksw_uint32 dataType;
    rtksw_uint32 value;
    rtksw_uint32 mask;
    rtksw_uint32 rangeStart;
    rtksw_uint32 rangeEnd;

} rtksw_filter_value_t;

typedef struct rtksw_filter_activeport_s
{
    rtksw_portmask_t value;
    rtksw_portmask_t mask;

} rtksw_filter_activeport_t;



typedef struct rtksw_filter_tag_s
{
    rtksw_filter_value_t pri;
    rtksw_filter_flag_t cfi;
    rtksw_filter_value_t vid;
} rtksw_filter_tag_t;

typedef struct rtksw_filter_ipFlag_s
{
    rtksw_filter_flag_t xf;
    rtksw_filter_flag_t mf;
    rtksw_filter_flag_t df;
} rtksw_filter_ipFlag_t;

typedef struct
{
    rtksw_uint32 addr[RTKSW_IPV6_ADDR_WORD_LENGTH];
} rtksw_filter_ip6_addr_t;

typedef struct
{
    rtksw_uint32 dataType;
    rtksw_filter_ip6_addr_t value;
    rtksw_filter_ip6_addr_t mask;
    rtksw_filter_ip6_addr_t rangeStart;
    rtksw_filter_ip6_addr_t rangeEnd;
} rtksw_filter_ip6_t;

typedef rtksw_uint32 rtksw_filter_number_t;

typedef struct rtksw_filter_pattern_s
{
    rtksw_uint32 value[FILTER_PATTERN_MAX];
    rtksw_uint32 mask[FILTER_PATTERN_MAX];
} rtksw_filter_pattern_t;

typedef struct rtksw_filter_tcpFlag_s
{
    rtksw_filter_flag_t urg;
    rtksw_filter_flag_t ack;
    rtksw_filter_flag_t psh;
    rtksw_filter_flag_t rst;
    rtksw_filter_flag_t syn;
    rtksw_filter_flag_t fin;
    rtksw_filter_flag_t ns;
    rtksw_filter_flag_t cwr;
    rtksw_filter_flag_t ece;
} rtksw_filter_tcpFlag_t;

typedef rtksw_uint32 rtksw_filter_field_raw_t;

typedef enum rtksw_filter_field_temple_input_e
{
    FILTER_FIELD_TEMPLE_INPUT_TYPE = 0,
    FILTER_FIELD_TEMPLE_INPUT_INDEX,
    FILTER_FIELD_TEMPLE_INPUT_MAX ,
} rtksw_filter_field_temple_input_t;

struct rtksw_filter_field
{
    rtksw_uint32 fieldType;

    union
    {
        /* L2 struct */
        rtksw_filter_mac_t       dmac;
        rtksw_filter_mac_t       smac;
        rtksw_filter_value_t     etherType;
        rtksw_filter_tag_t       ctag;
        rtksw_filter_tag_t       relayCtag;
        rtksw_filter_tag_t       stag;
        rtksw_filter_tag_t       l2tag;
        rtksw_filter_dot1as_timestamp_t dot1asTimeStamp;
        rtksw_filter_mac_t       mac;

        /* L3 struct */
        rtksw_filter_ip_t      sip;
        rtksw_filter_ip_t      dip;
        rtksw_filter_ip_t      ip;
        rtksw_filter_value_t   protocol;
        rtksw_filter_value_t   ipTos;
        rtksw_filter_ipFlag_t  ipFlag;
        rtksw_filter_value_t   ipOffset;
        rtksw_filter_ip6_t     sipv6;
        rtksw_filter_ip6_t     dipv6;
        rtksw_filter_ip6_t     ipv6;
        rtksw_filter_value_t   ipv6TrafficClass;
        rtksw_filter_value_t   ipv6NextHeader;
        rtksw_filter_value_t   flowLabel;

        /* L4 struct */
        rtksw_filter_value_t   tcpSrcPort;
        rtksw_filter_value_t   tcpDstPort;
        rtksw_filter_tcpFlag_t tcpFlag;
        rtksw_filter_value_t   tcpSeqNumber;
        rtksw_filter_value_t   tcpAckNumber;
        rtksw_filter_value_t   udpSrcPort;
        rtksw_filter_value_t   udpDstPort;
        rtksw_filter_value_t   icmpCode;
        rtksw_filter_value_t   icmpType;
        rtksw_filter_value_t   igmpType;

        /* pattern match */
        rtksw_filter_pattern_t pattern;

        rtksw_filter_value_t   inData;

    } filter_pattern_union;

    rtksw_uint32 fieldTemplateNo;
    rtksw_uint32 fieldTemplateIdx[RTKSW_FILTER_FIELD_USED_MAX];

    struct rtksw_filter_field *next;
};

typedef enum rtksw_filter_field_type_e
{
    FILTER_FIELD_DMAC = 0,
    FILTER_FIELD_SMAC,
    FILTER_FIELD_ETHERTYPE,
    FILTER_FIELD_CTAG,
    FILTER_FIELD_STAG,

    FILTER_FIELD_IPV4_SIP,
    FILTER_FIELD_IPV4_DIP,
    FILTER_FIELD_IPV4_TOS,
    FILTER_FIELD_IPV4_PROTOCOL,
    FILTER_FIELD_IPV4_FLAG,
    FILTER_FIELD_IPV4_OFFSET,
    FILTER_FIELD_IPV6_SIPV6,
    FILTER_FIELD_IPV6_DIPV6,
    FILTER_FIELD_IPV6_TRAFFIC_CLASS,
    FILTER_FIELD_IPV6_NEXT_HEADER,

    FILTER_FIELD_TCP_SPORT,
    FILTER_FIELD_TCP_DPORT,
    FILTER_FIELD_TCP_FLAG,
    FILTER_FIELD_UDP_SPORT,
    FILTER_FIELD_UDP_DPORT,
    FILTER_FIELD_ICMP_CODE,
    FILTER_FIELD_ICMP_TYPE,
    FILTER_FIELD_IGMP_TYPE,

    FILTER_FIELD_VID_RANGE,
    FILTER_FIELD_IP_RANGE,
    FILTER_FIELD_PORT_RANGE,

    FILTER_FIELD_USER_DEFINED00,
    FILTER_FIELD_USER_DEFINED01,
    FILTER_FIELD_USER_DEFINED02,
    FILTER_FIELD_USER_DEFINED03,
    FILTER_FIELD_USER_DEFINED04,
    FILTER_FIELD_USER_DEFINED05,
    FILTER_FIELD_USER_DEFINED06,
    FILTER_FIELD_USER_DEFINED07,
    FILTER_FIELD_USER_DEFINED08,
    FILTER_FIELD_USER_DEFINED09,
    FILTER_FIELD_USER_DEFINED10,
    FILTER_FIELD_USER_DEFINED11,
    FILTER_FIELD_USER_DEFINED12,
    FILTER_FIELD_USER_DEFINED13,
    FILTER_FIELD_USER_DEFINED14,
    FILTER_FIELD_USER_DEFINED15,

    FILTER_FIELD_PATTERN_MATCH,

    FILTER_FIELD_END,
} rtksw_filter_field_type_t;


typedef enum rtksw_filter_field_type_raw_e
{
    FILTER_FIELD_RAW_UNUSED = 0,
    FILTER_FIELD_RAW_DMAC_15_0,
    FILTER_FIELD_RAW_DMAC_31_16,
    FILTER_FIELD_RAW_DMAC_47_32,
    FILTER_FIELD_RAW_SMAC_15_0,
    FILTER_FIELD_RAW_SMAC_31_16,
    FILTER_FIELD_RAW_SMAC_47_32,
    FILTER_FIELD_RAW_ETHERTYPE,
    FILTER_FIELD_RAW_STAG,
    FILTER_FIELD_RAW_CTAG,

    FILTER_FIELD_RAW_IPV4_SIP_15_0 = 0x10,
    FILTER_FIELD_RAW_IPV4_SIP_31_16,
    FILTER_FIELD_RAW_IPV4_DIP_15_0,
    FILTER_FIELD_RAW_IPV4_DIP_31_16,


    FILTER_FIELD_RAW_IPV6_SIP_15_0 = 0x20,
    FILTER_FIELD_RAW_IPV6_SIP_31_16,
    FILTER_FIELD_RAW_IPV6_DIP_15_0 = 0x28,
    FILTER_FIELD_RAW_IPV6_DIP_31_16,
    
    FILTER_FIELD_RAW_L4_DPORT = 0x2A,
    FILTER_FIELD_RAW_L4_SPORT,

    FILTER_FIELD_RAW_VIDRANGE = 0x30,
    FILTER_FIELD_RAW_IPRANGE,
    FILTER_FIELD_RAW_PORTRANGE,
    FILTER_FIELD_RAW_FIELD_VALID,

    FILTER_FIELD_RAW_FIELD_SELECT00 = 0x40,
    FILTER_FIELD_RAW_FIELD_SELECT01,
    FILTER_FIELD_RAW_FIELD_SELECT02,
    FILTER_FIELD_RAW_FIELD_SELECT03,
    FILTER_FIELD_RAW_FIELD_SELECT04,
    FILTER_FIELD_RAW_FIELD_SELECT05,
    FILTER_FIELD_RAW_FIELD_SELECT06,
    FILTER_FIELD_RAW_FIELD_SELECT07,
    FILTER_FIELD_RAW_FIELD_SELECT08,
    FILTER_FIELD_RAW_FIELD_SELECT09,
    FILTER_FIELD_RAW_FIELD_SELECT10,
    FILTER_FIELD_RAW_FIELD_SELECT11,
    FILTER_FIELD_RAW_FIELD_SELECT12,
    FILTER_FIELD_RAW_FIELD_SELECT13,
    FILTER_FIELD_RAW_FIELD_SELECT14,
    FILTER_FIELD_RAW_FIELD_SELECT15,

    FILTER_FIELD_RAW_END,
} rtksw_filter_field_type_raw_t;

typedef enum rtksw_filter_flag_care_type_e
{
    FILTER_FLAG_CARE_DONT_CARE = 0,
    FILTER_FLAG_CARE_1,
    FILTER_FLAG_CARE_0,
    FILTER_FLAG_END
} rtksw_filter_flag_care_type_t;

typedef rtksw_uint32  rtksw_filter_id_t;    /* filter id type */

typedef enum rtksw_filter_invert_e
{
    FILTER_INVERT_DISABLE = 0,
    FILTER_INVERT_ENABLE,
    FILTER_INVERT_END,
} rtksw_filter_invert_t;

typedef rtksw_uint32 rtksw_filter_state_t;

typedef rtksw_uint32 rtksw_filter_unmatch_action_t;

typedef enum rtksw_filter_unmatch_action_e
{
    FILTER_UNMATCH_DROP = 0,
    FILTER_UNMATCH_PERMIT,
    FILTER_UNMATCH_END,
} rtksw_filter_unmatch_action_type_t;

typedef struct
{
    rtksw_filter_field_t      *fieldHead;
    rtksw_filter_care_tag_t   careTag;
    rtksw_filter_activeport_t activeport;

    rtksw_filter_invert_t     invert;
} rtksw_filter_cfg_t;

typedef struct
{
    rtksw_filter_field_raw_t      dataFieldRaw[RTKSW_FILTER_RAW_FIELD_NUMBER];
    rtksw_filter_field_raw_t      careFieldRaw[RTKSW_FILTER_RAW_FIELD_NUMBER];
    rtksw_filter_field_type_raw_t fieldRawType[RTKSW_FILTER_RAW_FIELD_NUMBER];
    rtksw_filter_care_tag_t       careTag;
    rtksw_filter_activeport_t     activeport;

    rtksw_filter_invert_t         invert;
    rtksw_enable_t                valid;
} rtksw_filter_cfg_raw_t;

typedef struct
{
    rtksw_uint32 index;
    rtksw_filter_field_type_raw_t fieldType[RTKSW_FILTER_RAW_FIELD_NUMBER];
} rtksw_filter_template_t;

typedef enum rtksw_field_sel_e
{
    FORMAT_DEFAULT = 0,
    FORMAT_RAW,
    FORMAT_LLC,
    FORMAT_IPV4,
    FORMAT_ARP,
    FORMAT_IPV6,
    FORMAT_IPPAYLOAD,
    FORMAT_L4PAYLOAD,
    FORMAT_END
}rtksw_field_sel_t;

typedef enum rtksw_filter_iprange_e
{
    IPRANGE_UNUSED = 0,
    IPRANGE_IPV4_SIP,
    IPRANGE_IPV4_DIP,
    IPRANGE_IPV6_SIP,
    IPRANGE_IPV6_DIP,
    IPRANGE_END
}rtksw_filter_iprange_t;

typedef enum rtksw_filter_vidrange_e
{
    VIDRANGE_UNUSED = 0,
    VIDRANGE_CVID,
    VIDRANGE_SVID,
    VIDRANGE_END
}rtksw_filter_vidrange_t;

typedef enum rtksw_filter_portrange_e
{
    PORTRANGE_UNUSED = 0,
    PORTRANGE_SPORT,
    PORTRANGE_DPORT,
    PORTRANGE_END
}rtksw_filter_portrange_t;

/* Function Name:
 *      rtksw_filter_igrAcl_init
 * Description:
 *      ACL initialization function
 * Input:
 *      unit            - Unit ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_NULL_POINTER - Pointer pFilter_field or pFilter_cfg point to NULL.
 * Note:
 *      This function enable and intialize ACL function
 */
extern rtksw_api_ret_t rtksw_filter_igrAcl_init(rtksw_uint32 unit);

/* Function Name:
 *      rtksw_filter_igrAcl_field_add
 * Description:
 *      Add comparison rule to an ACL configuration
 * Input:
 *      unit            - Unit ID
 *      pFilter_cfg     - The ACL configuration that this function will add comparison rule
 *      pFilter_field   - The comparison rule that will be added.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_NULL_POINTER     - Pointer pFilter_field or pFilter_cfg point to NULL.
 *      RT_ERR_INPUT            - Invalid input parameters.
 * Note:
 *      This function add a comparison rule (*pFilter_field) to an ACL configuration (*pFilter_cfg).
 *      Pointer pFilter_cfg points to an ACL configuration structure, this structure keeps multiple ACL
 *      comparison rules by means of linked list. Pointer pFilter_field will be added to linked
 *      list keeped by structure that pFilter_cfg points to.
 */
extern rtksw_api_ret_t rtksw_filter_igrAcl_field_add(rtksw_uint32 unit, rtksw_filter_cfg_t *pFilter_cfg, rtksw_filter_field_t *pFilter_field);

/* Function Name:
 *      rtksw_filter_igrAcl_cfg_add
 * Description:
 *      Add an ACL configuration to ASIC
 * Input:
 *      unit            - Unit ID
 *      filter_id       - Start index of ACL configuration.
 *      pFilter_cfg     - The ACL configuration that this function will add comparison rule
 *      pFilter_action  - Action(s) of ACL configuration.
 * Output:
 *      ruleNum - number of rules written in acl table
 * Return:
 *      RT_ERR_OK                               - OK
 *      RT_ERR_FAILED                           - Failed
 *      RT_ERR_SMI                              - SMI access error
 *      RT_ERR_NULL_POINTER                     - Pointer pFilter_field or pFilter_cfg point to NULL.
 *      RT_ERR_INPUT                            - Invalid input parameters.
 *      RT_ERR_ENTRY_INDEX                      - Invalid filter_id .
 *      RT_ERR_NULL_POINTER                     - Pointer pFilter_action or pFilter_cfg point to NULL.
 *      RT_ERR_FILTER_INACL_ACT_NOT_SUPPORT     - Action is not supported in this chip.
 *      RT_ERR_FILTER_INACL_RULE_NOT_SUPPORT    - Rule is not supported.
 * Note:
 *      This function store pFilter_cfg, pFilter_action into ASIC. The starting
 *      index(es) is filter_id.
 */
extern rtksw_api_ret_t rtksw_filter_igrAcl_cfg_add(rtksw_uint32 unit, rtksw_filter_id_t filter_id, rtksw_filter_cfg_t *pFilter_cfg, rtksw_filter_action_t *pAction, rtksw_filter_number_t *ruleNum);

/* Function Name:
 *      rtksw_filter_igrAcl_cfg_del
 * Description:
 *      Delete an ACL configuration from ASIC
 * Input:
 *      unit        - Unit ID
 *      filter_id   - Start index of ACL configuration.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_FILTER_ENTRYIDX  - Invalid filter_id.
 * Note:
 *      This function delete a group of ACL rules starting from filter_id.
 */
extern rtksw_api_ret_t rtksw_filter_igrAcl_cfg_del(rtksw_uint32 unit, rtksw_filter_id_t filter_id);

/* Function Name:
 *      rtksw_filter_igrAcl_cfg_delAll
 * Description:
 *      Delete all ACL entries from ASIC
 * Input:
 *      unit            - Unit ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      This function delete all ACL configuration from ASIC.
 */
extern rtksw_api_ret_t rtksw_filter_igrAcl_cfg_delAll(rtksw_uint32 unit);

/* Function Name:
 *      rtksw_filter_igrAcl_cfg_get
 * Description:
 *      Get one ingress acl configuration from ASIC.
 * Input:
 *      unit            - Unit ID
 *      filter_id       - Start index of ACL configuration.
 * Output:
 *      pFilter_cfg     - buffer pointer of ingress acl data
 *      pFilter_action  - buffer pointer of ingress acl action
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_NULL_POINTER     - Pointer pFilter_action or pFilter_cfg point to NULL.
 *      RT_ERR_FILTER_ENTRYIDX  - Invalid entry index.
 * Note:
 *      This function delete all ACL configuration from ASIC.
 */
extern rtksw_api_ret_t rtksw_filter_igrAcl_cfg_get(rtksw_uint32 unit, rtksw_filter_id_t filter_id, rtksw_filter_cfg_raw_t *pFilter_cfg, rtksw_filter_action_t *pAction);

/* Function Name:
 *      rtksw_filter_igrAcl_unmatchAction_set
 * Description:
 *      Set action to packets when no ACL configuration match
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 *      action  - Action.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port id.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This function sets action of packets when no ACL configruation matches.
 */
extern rtksw_api_ret_t rtksw_filter_igrAcl_unmatchAction_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_filter_unmatch_action_t action);

/* Function Name:
 *      rtksw_filter_igrAcl_unmatchAction_get
 * Description:
 *      Get action to packets when no ACL configuration match
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 * Output:
 *      pAction - Action.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port id.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This function gets action of packets when no ACL configruation matches.
 */
extern rtksw_api_ret_t rtksw_filter_igrAcl_unmatchAction_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_filter_unmatch_action_t* action);

/* Function Name:
 *      rtksw_filter_igrAcl_state_set
 * Description:
 *      Set state of ingress ACL.
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 *      state   - Ingress ACL state.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port id.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This function gets action of packets when no ACL configruation matches.
 */
extern rtksw_api_ret_t rtksw_filter_igrAcl_state_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_filter_state_t state);

/* Function Name:
 *      rtksw_filter_igrAcl_state_get
 * Description:
 *      Get state of ingress ACL.
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 * Output:
 *      pState  - Ingress ACL state.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port id.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This function gets action of packets when no ACL configruation matches.
 */
extern rtksw_api_ret_t rtksw_filter_igrAcl_state_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_filter_state_t* state);

/* Function Name:
 *      rtksw_filter_igrAcl_template_set
 * Description:
 *      Set template of ingress ACL.
 * Input:
 *      unit        - Unit ID
 *      template    - Ingress ACL template
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_INPUT           - Invalid input parameters.
 * Note:
 *      This function set ACL template.
 */
extern rtksw_api_ret_t rtksw_filter_igrAcl_template_set(rtksw_uint32 unit, rtksw_filter_template_t *aclTemplate);

/* Function Name:
 *      rtksw_filter_igrAcl_template_get
 * Description:
 *      Get template of ingress ACL.
 * Input:
 *      unit        - Unit ID
 *      template    - Ingress ACL template
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 * Note:
 *      This function gets template of ACL.
 */
extern rtksw_api_ret_t rtksw_filter_igrAcl_template_get(rtksw_uint32 unit, rtksw_filter_template_t *aclTemplate);

/* Function Name:
 *      rtksw_filter_igrAcl_field_sel_set
 * Description:
 *      Set user defined field selectors in HSB
 * Input:
 *      unit        - Unit ID
 *      index       - index of field selector 0-15
 *      format      - Format of field selector
 *      offset      - Retrieving data offset
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 * Note:
 *      System support 16 user defined field selctors.
 *      Each selector can be enabled or disable.
 *      User can defined retrieving 16-bits in many predefiend
 *      standard l2/l3/l4 payload.
 */
extern rtksw_api_ret_t rtksw_filter_igrAcl_field_sel_set(rtksw_uint32 unit, rtksw_uint32 index, rtksw_field_sel_t format, rtksw_uint32 offset);

/* Function Name:
 *      rtksw_filter_igrAcl_field_sel_get
 * Description:
 *      Get user defined field selectors in HSB
 * Input:
 *      unit        - Unit ID
 *      index       - index of field selector 0-15
 * Output:
 *      pFormat     - Format of field selector
 *      pOffset     - Retrieving data offset
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 * Note:
 *      None.
 */
extern rtksw_api_ret_t rtksw_filter_igrAcl_field_sel_get(rtksw_uint32 unit, rtksw_uint32 index, rtksw_field_sel_t *pFormat, rtksw_uint32 *pOffset);

/* Function Name:
 *      rtksw_filter_iprange_set
 * Description:
 *      Set IP Range check
 * Input:
 *      unit        - Unit ID
 *      index       - index of IP Range 0-15
 *      type        - IP Range check type, 0:Delete a entry, 1: IPv4_SIP, 2: IPv4_DIP, 3:IPv6_SIP, 4:IPv6_DIP
 *      upperIp     - The upper bound of IP range
 *      lowerIp     - The lower Bound of IP range
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_OUT_OF_RANGE    - The parameter is out of range
 *      RT_ERR_INPUT           - Input error
 * Note:
 *      upperIp must be larger or equal than lowerIp.
 */
extern rtksw_api_ret_t rtksw_filter_iprange_set(rtksw_uint32 unit, rtksw_uint32 index, rtksw_filter_iprange_t type, ipaddr_t upperIp, ipaddr_t lowerIp);

/* Function Name:
 *      rtksw_filter_iprange_get
 * Description:
 *      Set IP Range check
 * Input:
 *      unit         - Unit ID
 *      index        - index of IP Range 0-15
 * Output:
 *      pType        - IP Range check type, 0:Delete a entry, 1: IPv4_SIP, 2: IPv4_DIP, 3:IPv6_SIP, 4:IPv6_DIP
 *      pUpperIp     - The upper bound of IP range
 *      pLowerIp     - The lower Bound of IP range
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_OUT_OF_RANGE    - The parameter is out of range
 * Note:
 *      upperIp must be larger or equal than lowerIp.
 */
extern rtksw_api_ret_t rtksw_filter_iprange_get(rtksw_uint32 unit, rtksw_uint32 index, rtksw_filter_iprange_t *pType, ipaddr_t *pUpperIp, ipaddr_t *pLowerIp);

/* Function Name:
 *      rtksw_filter_vidrange_set
 * Description:
 *      Set VID Range check
 * Input:
 *      unit        - Unit ID
 *      index       - index of VID Range 0-15
 *      type        - IP Range check type, 0:Delete a entry, 1: CVID, 2: SVID
 *      upperVid    - The upper bound of VID range
 *      lowerVid    - The lower Bound of VID range
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_OUT_OF_RANGE    - The parameter is out of range
 *      RT_ERR_INPUT           - Input error
 * Note:
 *      upperVid must be larger or equal than lowerVid.
 */
extern rtksw_api_ret_t rtksw_filter_vidrange_set(rtksw_uint32 unit, rtksw_uint32 index, rtksw_filter_vidrange_t type, rtksw_uint32 upperVid, rtksw_uint32 lowerVid);

/* Function Name:
 *      rtksw_filter_vidrange_get
 * Description:
 *      Get VID Range check
 * Input:
 *      unit        - Unit ID
 *      index       - index of VID Range 0-15
 * Output:
 *      pType        - IP Range check type, 0:Unused, 1: CVID, 2: SVID
 *      pUpperVid    - The upper bound of VID range
 *      pLowerVid    - The lower Bound of VID range
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_OUT_OF_RANGE    - The parameter is out of range
 * Note:
 *      None.
 */
extern rtksw_api_ret_t rtksw_filter_vidrange_get(rtksw_uint32 unit, rtksw_uint32 index, rtksw_filter_vidrange_t *pType, rtksw_uint32 *pUpperVid, rtksw_uint32 *pLowerVid);

/* Function Name:
 *      rtksw_filter_portrange_set
 * Description:
 *      Set Port Range check
 * Input:
 *      unit        - Unit ID
 *      index       - index of Port Range 0-15
 *      type        - IP Range check type, 0:Delete a entry, 1: Source Port, 2: Destnation Port
 *      upperPort   - The upper bound of Port range
 *      lowerPort   - The lower Bound of Port range
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_OUT_OF_RANGE    - The parameter is out of range
 *      RT_ERR_INPUT           - Input error
 * Note:
 *      upperPort must be larger or equal than lowerPort.
 */
extern rtksw_api_ret_t rtksw_filter_portrange_set(rtksw_uint32 unit, rtksw_uint32 index, rtksw_filter_portrange_t type, rtksw_uint32 upperPort, rtksw_uint32 lowerPort);

/* Function Name:
 *      rtksw_filter_portrange_get
 * Description:
 *      Set Port Range check
 * Input:
 *      unit        - Unit ID
 *      index       - index of Port Range 0-15
 * Output:
 *      pType       - IP Range check type, 0:Delete a entry, 1: Source Port, 2: Destnation Port
 *      pUpperPort  - The upper bound of Port range
 *      pLowerPort  - The lower Bound of Port range
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_OUT_OF_RANGE    - The parameter is out of range
 *      RT_ERR_INPUT           - Input error
 * Note:
 *      None.
 */
extern rtksw_api_ret_t rtksw_filter_portrange_get(rtksw_uint32 unit, rtksw_uint32 index, rtksw_filter_portrange_t *pType, rtksw_uint32 *pUpperPort, rtksw_uint32 *pLowerPort);

/* Function Name:
 *      rtksw_filter_igrAclPolarity_set
 * Description:
 *      Set ACL Goip control palarity
 * Input:
 *      unit        - Unit ID
 *      polarity    - 1: High, 0: Low
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - Success
 *      RT_ERR_SMI      - SMI access error
 * Note:
 *      none
 */
extern rtksw_api_ret_t rtksw_filter_igrAclPolarity_set(rtksw_uint32 unit, rtksw_uint32 polarity);

/* Function Name:
 *      rtksw_filter_igrAclPolarity_get
 * Description:
 *      Get ACL Goip control palarity
 * Input:
 *      unit            - Unit ID
 * Output:
 *      pPolarity       - 1: High, 0: Low
 * Return:
 *      RT_ERR_OK       - Success
 *      RT_ERR_SMI      - SMI access error
 * Note:
 *      none
 */
extern rtksw_api_ret_t rtksw_filter_igrAclPolarity_get(rtksw_uint32 unit, rtksw_uint32* pPolarity);

#if defined(CONFIG_COMPATIBLE_V1_4)
#define rtk_filter_igrAcl_init(void)                                            rtksw_filter_igrAcl_init(0)
#define rtk_filter_igrAcl_field_add(pFilter_cfg, pFilter_field)                 rtksw_filter_igrAcl_field_add(0, pFilter_cfg, pFilter_field)
#define rtk_filter_igrAcl_cfg_add(filter_id, pFilter_cfg, pAction, ruleNum)     rtksw_filter_igrAcl_cfg_add(0, filter_id, pFilter_cfg, pAction, ruleNum)
#define rtk_filter_igrAcl_cfg_del(filter_id)                                    rtksw_filter_igrAcl_cfg_del(0, filter_id)
#define rtk_filter_igrAcl_cfg_delAll(void)                                      rtksw_filter_igrAcl_cfg_delAll(0)
#define rtk_filter_igrAcl_cfg_get(filter_id, pFilter_cfg, pAction)              rtksw_filter_igrAcl_cfg_get(0, filter_id, pFilter_cfg, pAction)
#define rtk_filter_igrAcl_unmatchAction_set(port, action)                       rtksw_filter_igrAcl_unmatchAction_set(0, port, action)
#define rtk_filter_igrAcl_unmatchAction_get(port, action)                       rtksw_filter_igrAcl_unmatchAction_get(0, port, action)
#define rtk_filter_igrAcl_state_set(port, state)                                rtksw_filter_igrAcl_state_set(0, port, state)
#define rtk_filter_igrAcl_state_get(port, state)                                rtksw_filter_igrAcl_state_get(0, port, state)
#define rtk_filter_igrAcl_template_set(aclTemplate)                             rtksw_filter_igrAcl_template_set(0, aclTemplate)
#define rtk_filter_igrAcl_template_get(aclTemplate)                             rtksw_filter_igrAcl_template_get(0, aclTemplate)
#define rtk_filter_igrAcl_field_sel_set(index, format, offset)                  rtksw_filter_igrAcl_field_sel_set(0, index, format, offset)
#define rtk_filter_igrAcl_field_sel_get(index, pFormat, pOffset)                rtksw_filter_igrAcl_field_sel_get(0, index, pFormat, pOffset)
#define rtk_filter_iprange_set(index, type, upperIp, lowerIp)                   rtksw_filter_iprange_set(0, index, type, upperIp, lowerIp)
#define rtk_filter_iprange_get(index, pType, pUpperIp, pLowerIp)                rtksw_filter_iprange_get(0, index, pType, pUpperIp, pLowerIp)
#define rtk_filter_vidrange_set(index, type, upperVid, lowerVid)                rtksw_filter_vidrange_set(0, index, type, upperVid, lowerVid)
#define rtk_filter_vidrange_get(index, pType, pUpperVid, pLowerVid)             rtksw_filter_vidrange_get(0, index, pType, pUpperVid, pLowerVid)
#define rtk_filter_portrange_set(index, type, upperPort, lowerPort)             rtksw_filter_portrange_set(0, index, type, upperPort, lowerPort)
#define rtk_filter_portrange_get(index, pType, pUpperPort, pLowerPort)          rtksw_filter_portrange_get(0, index, pType, pUpperPort, pLowerPort)
#define rtk_filter_igrAclPolarity_set(polarity)                                 rtksw_filter_igrAclPolarity_set(0, polarity)
#define rtk_filter_igrAclPolarity_get(pPolarity)                                rtksw_filter_igrAclPolarity_get(0, pPolarity)

#define rtk_filter_act_cactext_t                rtksw_filter_act_cactext_t
#define rtk_filter_act_ctag_t                   rtksw_filter_act_ctag_t
#define rtk_filter_act_enable_t                 rtksw_filter_act_enable_t
#define rtk_filter_action_t                     rtksw_filter_action_t
#define rtk_filter_flag_t                       rtksw_filter_flag_t
#define rtk_filter_care_tag_index_t             rtksw_filter_care_tag_index_t
#define rtk_filter_care_tag_t                   rtksw_filter_care_tag_t
#define rtk_filter_field_t                      rtksw_filter_field_t
#define rtk_filter_dot1as_timestamp_t           rtksw_filter_dot1as_timestamp_t
#define rtk_filter_field_data_type_t            rtksw_filter_field_data_type_t
#define rtk_filter_ip_t                         rtksw_filter_ip_t
#define rtk_filter_mac_t                        rtksw_filter_mac_t
#define rtk_filter_op_t                         rtksw_filter_op_t
#define rtk_filter_value_t                      rtksw_filter_value_t
#define rtk_filter_activeport_t                 rtksw_filter_activeport_t
#define rtk_filter_tag_t                        rtksw_filter_tag_t
#define rtk_filter_ipFlag_t                     rtksw_filter_ipFlag_t
#define rtk_filter_ip6_addr_t                   rtksw_filter_ip6_addr_t
#define rtk_filter_ip6_t                        rtksw_filter_ip6_t
#define rtk_filter_number_t                     rtksw_filter_number_t
#define rtk_filter_pattern_t                    rtksw_filter_pattern_t
#define rtk_filter_tcpFlag_t                    rtksw_filter_tcpFlag_t
#define rtk_filter_field_raw_t                  rtksw_filter_field_raw_t
#define rtk_filter_field_temple_input_t         rtksw_filter_field_temple_input_t
#define rtk_filter_field_t                      rtksw_filter_field_t
#define rtk_filter_field_type_t                 rtksw_filter_field_type_t
#define rtk_filter_field_type_raw_t             rtksw_filter_field_type_raw_t
#define rtk_filter_flag_care_type_t             rtksw_filter_flag_care_type_t
#define rtk_filter_id_t                         rtksw_filter_id_t
#define rtk_filter_invert_t                     rtksw_filter_invert_t
#define rtk_filter_state_t                      rtksw_filter_state_t
#define rtk_filter_unmatch_action_t             rtksw_filter_unmatch_action_t
#define rtk_filter_unmatch_action_type_t        rtksw_filter_unmatch_action_type_t
#define rtk_filter_cfg_t                        rtksw_filter_cfg_t
#define rtk_filter_cfg_raw_t                    rtksw_filter_cfg_raw_t
#define rtk_filter_template_t                   rtksw_filter_template_t
#define rtk_field_sel_t                         rtksw_field_sel_t
#define rtk_filter_iprange_t                    rtksw_filter_iprange_t
#define rtk_filter_vidrange_t                   rtksw_filter_vidrange_t
#define rtk_filter_portrange_t                  rtksw_filter_portrange_t

#endif /* CONFIG_COMPATIBLE_V1_4 */

#endif /* __RTKSW_API_ACL_H__ */
