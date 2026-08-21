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

#ifndef __DAL_rtl8371C_ACL_H__
#define __DAL_rtl8371C_ACL_H__

#include <acl.h>

#define RTL8371C_ACLRULENO                  64

#define RTL8371C_ACLRULEMAX                 (RTL8371C_ACLRULENO-1)
#define RTL8371C_ACLRULEFIELDNO             8
#define RTL8371C_ACLTEMPLATENO              5
#define RTL8371C_ACLTYPEMAX                 (RTL8371C_ACLTEMPLATENO-1)

#define RTL8371C_ACLRULETBLEN               10
#define RTL8371C_ACLACTTBLEN                4
#define RTL8371C_ACLRULETBADDR(type, rule)  ((type << 6) | rule)
#define RTL8371C_ACLRULETBADDR2(type, rule) ((type << 5) | (rule + 64))

#define ACL_ACT_CVLAN_ENABLE_MASK           0x1
#define ACL_ACT_SVLAN_ENABLE_MASK           0x2
#define ACL_ACT_PRIORITY_ENABLE_MASK        0x4
#define ACL_ACT_POLICING_ENABLE_MASK        0x8
#define ACL_ACT_FWD_ENABLE_MASK             0x10
#define ACL_ACT_INTGPIO_ENABLE_MASK         0x20

#define RTL8371C_ACLRULETAGBITS             5

#define RTL8371C_ACLRANGENO                 8

#define RTL8371C_ACLRANGEMAX                (RTL8371C_ACLRANGENO-1)

#define RTL8371C_ACL_PORTRANGEMAX           (0xFFFF)
#define RTL8371C_ACL_ACT_TABLE_LEN          (4)

#define RTL8371C_FIELDSEL_FORMAT_NUMBER     (8)
#define RTL8371C_FIELDSEL_MAX_OFFSET        (255)

#define RTL8371C_MAX_LOG_CNT_NUM            (16)

#define RTL8371C_RTKSW_IPV6_ADDR_WORD_LENGTH           2UL

#define RTL8371C_ACLGPIOPINNO               25
#define RTL8371C_ACL_PRIMAX                 7
#define RTL8371C_ACL_DSCPMAX                63

typedef struct rtl8371c_acl_entry_s
{
    rtksw_uint32 entry_data[RTL8371C_ACLRULETBLEN];
}rtl8371c_acl_entry_t;

typedef struct rtl8371c_acl_act_entry_s
{
    rtksw_uint32 entry_data[RTL8371C_ACLACTTBLEN];
}rtl8371c_acl_act_entry_t;

enum RTL8371C_FIELDSEL_FORMAT_FORMAT
{
    RTL8371C_FIELDSEL_FORMAT_DEFAULT = 0,
    RTL8371C_FIELDSEL_FORMAT_RAW,
    RTL8371C_FIELDSEL_FORMAT_LLC,
    RTL8371C_FIELDSEL_FORMAT_IPV4,
    RTL8371C_FIELDSEL_FORMAT_ARP,
    RTL8371C_FIELDSEL_FORMAT_IPV6,
    RTL8371C_FIELDSEL_FORMAT_IPPAYLOAD,
    RTL8371C_FIELDSEL_FORMAT_L4PAYLOAD,
    RTL8371C_FIELDSEL_FORMAT_END
};

enum RTL8371C_ACLFIELDTYPES
{
    RTL8371C_ACL_UNUSED,
    RTL8371C_ACL_DMAC0,
    RTL8371C_ACL_DMAC1,
    RTL8371C_ACL_DMAC2,
    RTL8371C_ACL_SMAC0,
    RTL8371C_ACL_SMAC1,
    RTL8371C_ACL_SMAC2,
    RTL8371C_ACL_ETHERTYPE,
    RTL8371C_ACL_STAG,
    RTL8371C_ACL_CTAG,
    RTL8371C_ACL_IP4SIP0 = 0x10,
    RTL8371C_ACL_IP4SIP1,
    RTL8371C_ACL_IP4DIP0,
    RTL8371C_ACL_IP4DIP1,
    RTL8371C_ACL_IP6SIP0WITHIPV4 = 0x20,
    RTL8371C_ACL_IP6SIP1WITHIPV4,
    RTL8371C_ACL_IP6DIP0WITHIPV4 = 0x28,
    RTL8371C_ACL_IP6DIP1WITHIPV4,
    RTL8371C_ACL_L4DPORT = 0x2a,
    RTL8371C_ACL_L4SPORT = 0x2b,
    RTL8371C_ACL_VIDRANGE = 0x30,
    RTL8371C_ACL_IPRANGE = 0x31,
    RTL8371C_ACL_PORTRANGE = 0x32,
    RTL8371C_ACL_FIELD_VALID = 0x33,
    RTL8371C_ACL_FIELD_SELECT00 = 0x40,
    RTL8371C_ACL_FIELD_SELECT01,
    RTL8371C_ACL_FIELD_SELECT02,
    RTL8371C_ACL_FIELD_SELECT03,
    RTL8371C_ACL_FIELD_SELECT04,
    RTL8371C_ACL_FIELD_SELECT05,
    RTL8371C_ACL_FIELD_SELECT06,
    RTL8371C_ACL_FIELD_SELECT07,
    RTL8371C_ACL_TYPE_END
};

enum RTL8371C_ACLTCAMTYPES
{
    RTL8371C_CAREBITS= 0,
    RTL8371C_DATABITS
};

typedef enum rtl8371c_aclFwd
{
    RTL8371C_ACL_FWD_MIRROR = 0,
    RTL8371C_ACL_FWD_REDIRECT,
    RTL8371C_ACL_FWD_MIRRORFUNTION,
    RTL8371C_ACL_FWD_TRAP,
} rtl8371c_aclFwd_t;


struct rtl8371c_acl_rule_smi_st{
    rtksw_uint16 rule_info;
    rtksw_uint16 field[RTL8371C_ACLRULEFIELDNO];
};

struct rtl8371c_acl_rule_smi_ext_st{
    rtksw_uint16 rule_info;
};

typedef struct RTL8371C_ACLRULESMI{
    struct rtl8371c_acl_rule_smi_st  care_bits;
    rtksw_uint32      valid;
    struct rtl8371c_acl_rule_smi_st  data_bits;

}rtl8371c_aclrulesmi;

struct rtl8371c_acl_rule_st{
    rtksw_uint32 active_portmsk;
    rtksw_uint32 type;
    rtksw_uint32 tag_exist;
    rtksw_uint16 field[RTL8371C_ACLRULEFIELDNO];
};

typedef struct RTL8371C_ACLRULE{
    struct rtl8371c_acl_rule_st  data_bits;
    rtksw_uint32      valid;
    struct rtl8371c_acl_rule_st  care_bits;
}rtl8371c_aclrule;

typedef struct rtl8371c_acl_act_s{
    rtksw_uint32 cvidx_cact;
    rtksw_uint32 cact;
    rtksw_uint32 svidx_sact;
    rtksw_uint32 sact;

    rtksw_uint32 aclmeteridx;
    rtksw_uint32 fwdpmask;
    rtksw_uint32 fwdact;

    rtksw_uint32 pridx;
    rtksw_uint32 priact;
    rtksw_uint32 gpio_pin;
    rtksw_uint32 gpio_en;
    rtksw_uint32 aclint;

    rtksw_uint32 cact_ext;
    rtksw_uint32 fwdact_ext;
    rtksw_uint32 tag_fmt;
}rtl8371c_acl_act_t;

typedef struct rtl8371c_acl_rule_union_s
{
    rtl8371c_aclrule aclRule;
    rtl8371c_acl_act_t aclAct;
    rtksw_uint32 aclActCtrl;
    rtksw_uint32 aclNot;
}rtl8371c_acl_rule_union_t;


/* Function Name:
 *      dal_rtl8371c_filter_igrAcl_init
 * Description:
 *      ACL initialization function
 * Input:
 *      unit    - Unit ID
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
extern rtksw_api_ret_t dal_rtl8371c_filter_igrAcl_init(rtksw_uint32 unit);

/* Function Name:
 *      dal_rtl8371c_filter_igrAcl_field_add
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
extern rtksw_api_ret_t dal_rtl8371c_filter_igrAcl_field_add(rtksw_uint32 unit, rtksw_filter_cfg_t *pFilter_cfg, rtksw_filter_field_t *pFilter_field);

/* Function Name:
 *      dal_rtl8371c_filter_igrAcl_cfg_add
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
extern rtksw_api_ret_t dal_rtl8371c_filter_igrAcl_cfg_add(rtksw_uint32 unit, rtksw_filter_id_t filter_id, rtksw_filter_cfg_t *pFilter_cfg, rtksw_filter_action_t *pAction, rtksw_filter_number_t *ruleNum);

/* Function Name:
 *      dal_rtl8371c_filter_igrAcl_cfg_del
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
extern rtksw_api_ret_t dal_rtl8371c_filter_igrAcl_cfg_del(rtksw_uint32 unit, rtksw_filter_id_t filter_id);

/* Function Name:
 *      dal_rtl8371c_filter_igrAcl_cfg_delAll
 * Description:
 *      Delete all ACL entries from ASIC
 * Input:
 *      unit    - Unit ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      This function delete all ACL configuration from ASIC.
 */
extern rtksw_api_ret_t dal_rtl8371c_filter_igrAcl_cfg_delAll(rtksw_uint32 unit);

/* Function Name:
 *      dal_rtl8371c_filter_igrAcl_cfg_get
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
extern rtksw_api_ret_t dal_rtl8371c_filter_igrAcl_cfg_get(rtksw_uint32 unit, rtksw_filter_id_t filter_id, rtksw_filter_cfg_raw_t *pFilter_cfg, rtksw_filter_action_t *pAction);

/* Function Name:
 *      dal_rtl8371c_filter_igrAcl_unmatchAction_set
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
extern rtksw_api_ret_t dal_rtl8371c_filter_igrAcl_unmatchAction_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_filter_unmatch_action_t action);

/* Function Name:
 *      dal_rtl8371c_filter_igrAcl_unmatchAction_get
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
extern rtksw_api_ret_t dal_rtl8371c_filter_igrAcl_unmatchAction_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_filter_unmatch_action_t* action);

/* Function Name:
 *      dal_rtl8371c_filter_igrAcl_state_set
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
extern rtksw_api_ret_t dal_rtl8371c_filter_igrAcl_state_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_filter_state_t state);

/* Function Name:
 *      dal_rtl8371c_filter_igrAcl_state_get
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
extern rtksw_api_ret_t dal_rtl8371c_filter_igrAcl_state_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_filter_state_t* state);

/* Function Name:
 *      dal_rtl8371c_filter_igrAcl_template_set
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
extern rtksw_api_ret_t dal_rtl8371c_filter_igrAcl_template_set(rtksw_uint32 unit, rtksw_filter_template_t *aclTemplate);

/* Function Name:
 *      dal_rtl8371c_filter_igrAcl_template_get
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
extern rtksw_api_ret_t dal_rtl8371c_filter_igrAcl_template_get(rtksw_uint32 unit, rtksw_filter_template_t *aclTemplate);

/* Function Name:
 *      dal_rtl8371c_filter_igrAcl_field_sel_set
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
extern rtksw_api_ret_t dal_rtl8371c_filter_igrAcl_field_sel_set(rtksw_uint32 unit, rtksw_uint32 index, rtksw_field_sel_t format, rtksw_uint32 offset);

/* Function Name:
 *      dal_rtl8371c_filter_igrAcl_field_sel_get
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
extern rtksw_api_ret_t dal_rtl8371c_filter_igrAcl_field_sel_get(rtksw_uint32 unit, rtksw_uint32 index, rtksw_field_sel_t *pFormat, rtksw_uint32 *pOffset);

/* Function Name:
 *      dal_rtl8371c_filter_iprange_set
 * Description:
 *      Set IP Range check
 * Input:
 *      unit        - Unit ID
 *      index       - index of IP Range 0-7
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
extern rtksw_api_ret_t dal_rtl8371c_filter_iprange_set(rtksw_uint32 unit, rtksw_uint32 index, rtksw_filter_iprange_t type, ipaddr_t upperIp, ipaddr_t lowerIp);

/* Function Name:
 *      dal_rtl8371c_filter_iprange_get
 * Description:
 *      Set IP Range check
 * Input:
 *      unit        - Unit ID
 *      index       - index of IP Range 0-7
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
extern rtksw_api_ret_t dal_rtl8371c_filter_iprange_get(rtksw_uint32 unit, rtksw_uint32 index, rtksw_filter_iprange_t *pType, ipaddr_t *pUpperIp, ipaddr_t *pLowerIp);

/* Function Name:
 *      dal_rtl8371c_filter_vidrange_set
 * Description:
 *      Set VID Range check
 * Input:
 *      unit        - Unit ID
 *      index       - index of VID Range 0-7
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
extern rtksw_api_ret_t dal_rtl8371c_filter_vidrange_set(rtksw_uint32 unit, rtksw_uint32 index, rtksw_filter_vidrange_t type, rtksw_uint32 upperVid, rtksw_uint32 lowerVid);

/* Function Name:
 *      dal_rtl8371c_filter_vidrange_get
 * Description:
 *      Get VID Range check
 * Input:
 *      unit        - Unit ID
 *      index       - index of VID Range 0-7
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
extern rtksw_api_ret_t dal_rtl8371c_filter_vidrange_get(rtksw_uint32 unit, rtksw_uint32 index, rtksw_filter_vidrange_t *pType, rtksw_uint32 *pUpperVid, rtksw_uint32 *pLowerVid);

/* Function Name:
 *      dal_rtl8371c_filter_portrange_set
 * Description:
 *      Set Port Range check
 * Input:
 *      unit        - Unit ID
 *      index       - index of Port Range 0-7
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
extern rtksw_api_ret_t dal_rtl8371c_filter_portrange_set(rtksw_uint32 unit, rtksw_uint32 index, rtksw_filter_portrange_t type, rtksw_uint32 upperPort, rtksw_uint32 lowerPort);

/* Function Name:
 *      dal_rtl8371c_filter_portrange_get
 * Description:
 *      Set Port Range check
 * Input:
 *      unit        - Unit ID
 *      index       - index of Port Range 0-7
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
extern rtksw_api_ret_t dal_rtl8371c_filter_portrange_get(rtksw_uint32 unit, rtksw_uint32 index, rtksw_filter_portrange_t *pType, rtksw_uint32 *pUpperPort, rtksw_uint32 *pLowerPort);

/* Function Name:
 *      dal_rtl8371c_filter_igrAclPolarity_set
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
extern rtksw_api_ret_t dal_rtl8371c_filter_igrAclPolarity_set(rtksw_uint32 unit, rtksw_uint32 polarity);

/* Function Name:
 *      dal_rtl8371c_filter_igrAclPolarity_get
 * Description:
 *      Get ACL Goip control palarity
 * Input:
 *      unit        - Unit ID
 *      pPolarity   - 1: High, 0: Low
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - Success
 *      RT_ERR_SMI      - SMI access error
 * Note:
 *      none
 */
extern rtksw_api_ret_t dal_rtl8371c_filter_igrAclPolarity_get(rtksw_uint32 unit, rtksw_uint32* pPolarity);


#endif /* __DAL_rtl8371C_ACL_H__ */
