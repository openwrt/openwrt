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

#ifndef __RTK_RTL8371C_TABLE_STRUCT_H__
#define __RTK_RTL8371C_TABLE_STRUCT_H__

/*
 * Include Files
 */
#include <reg.h>

/* Superset Table Enum */
typedef enum rtk_rtl8371c_table_list_e
{
    RTL8371C_VLAN_TBLt,
    RTL8371C_L2_IPMCt,
    RTL8371C_L2_MCt,
    RTL8371C_L2_UCt,
    RTL8371C_ACL_ACT_ENTRYt,
    RTL8371C_ACL_RULE_DATAt,
    RTL8371C_ACL_RULE_MASKt,
    RTL8371C_TABLE_LIST_END,
} rtk_rtl8371c_table_list_t;


/* Internal Table Enum */
typedef enum rtk_int_rtl8371c_table_list_e
{
    INT_RTL8371C_VLAN_TBL_RTL8371C,
    INT_RTL8371C_L2_IPMC_RTL8371C,
    INT_RTL8371C_L2_MC_RTL8371C,
    INT_RTL8371C_L2_UC_RTL8371C,
    INT_RTL8371C_ACL_ACT_ENTRY_RTL8371C,
    INT_RTL8371C_ACL_RULE_DATA_RTL8371C,
    INT_RTL8371C_ACL_RULE_MASK_RTL8371C,
    INT_RTL8371C_TABLE_LIST_END,
} rtk_int_rtl8371c_table_list_t;

typedef enum rtk_rtl8371c_vlan_tblField_list_e
{
    RTL8371C_VLAN_TBL_RSVtf,
    RTL8371C_VLAN_TBL_IVL_SVLtf,
    RTL8371C_VLAN_TBL_SVLAN_CHK_IVL_SVLtf,
    RTL8371C_VLAN_TBL_FID_MSTItf,
    RTL8371C_VLAN_TBL_UNTAGSETtf,
    RTL8371C_VLAN_TBL_MBRtf,
    RTL8371C_VLAN_TBLFIELD_LIST_END,
} rtk_rtl8371c_vlan_tblField_list_t;

typedef enum rtk_rtl8371c_l2_ipmcField_list_e
{
    RTL8371C_L2_IPMC_VALIDtf,
    RTL8371C_L2_IPMC_STATICtf,
    RTL8371C_L2_IPMC_MBRtf,
    RTL8371C_L2_IPMC_IVL_SVLtf,
    RTL8371C_L2_IPMC_L3LOOKUPtf,
    RTL8371C_L2_IPMC_DIPtf,
    RTL8371C_L2_IPMC_SIPtf,
    RTL8371C_L2_IPMCFIELD_LIST_END,
} rtk_rtl8371c_l2_ipmcField_list_t;

typedef enum rtk_rtl8371c_l2_mcField_list_e
{
    RTL8371C_L2_MC_VALIDtf,
    RTL8371C_L2_MC_STATICtf,
    RTL8371C_L2_MC_MBRtf,
    RTL8371C_L2_MC_IVL_SVLtf,
    RTL8371C_L2_MC_L3LOOKUPtf,
    RTL8371C_L2_MC_VID_FIDtf,
    RTL8371C_L2_MC_MACtf,
    RTL8371C_L2_MCFIELD_LIST_END,
} rtk_rtl8371c_l2_mcField_list_t;

typedef enum rtk_rtl8371c_l2_ucField_list_e
{
    RTL8371C_L2_UC_VALIDtf,
    RTL8371C_L2_UC_STATICtf,
    RTL8371C_L2_UC_AGEtf,
    RTL8371C_L2_UC_SPAtf,
    RTL8371C_L2_UC_IVL_SVLtf,
    RTL8371C_L2_UC_L3LOOKUPtf,
    RTL8371C_L2_UC_VID_FIDtf,
    RTL8371C_L2_UC_MACtf,
    RTL8371C_L2_UCFIELD_LIST_END,
} rtk_rtl8371c_l2_ucField_list_t;

typedef enum rtk_rtl8371c_acl_act_entryField_list_e
{
    RTL8371C_ACL_ACT_ENTRY_GPIO_PINtf,
    RTL8371C_ACL_ACT_ENTRY_GPIO_ENtf,
    RTL8371C_ACL_ACT_ENTRY_ACL_INTtf,
    RTL8371C_ACL_ACT_ENTRY_ACL_PRItf,
    RTL8371C_ACL_ACT_ENTRY_PRI_CATtf,
    RTL8371C_ACL_ACT_ENTRY_FWD_EXTtf,
    RTL8371C_ACL_ACT_ENTRY_ACLPMSKtf,
    RTL8371C_ACL_ACT_ENTRY_FWD_ACTtf,
    RTL8371C_ACL_ACT_ENTRY_METER_LOG_IDXtf,
    RTL8371C_ACL_ACT_ENTRY_SVIDtf,
    RTL8371C_ACL_ACT_ENTRY_SACTtf,
    RTL8371C_ACL_ACT_ENTRY_TAG_FMTtf,
    RTL8371C_ACL_ACT_ENTRY_CACT_EXTtf,
    RTL8371C_ACL_ACT_ENTRY_CVIDtf,
    RTL8371C_ACL_ACT_ENTRY_CACTtf,
    RTL8371C_ACL_ACT_ENTRYFIELD_LIST_END,
} rtk_rtl8371c_acl_act_entryField_list_t;

typedef enum rtk_rtl8371c_acl_rule_dataField_list_e
{
    RTL8371C_ACL_RULE_DATA_VALIDtf,
    RTL8371C_ACL_RULE_DATA_FIELD_7tf,
    RTL8371C_ACL_RULE_DATA_FIELD_6tf,
    RTL8371C_ACL_RULE_DATA_FIELD_5tf,
    RTL8371C_ACL_RULE_DATA_FIELD_4tf,
    RTL8371C_ACL_RULE_DATA_FIELD_3tf,
    RTL8371C_ACL_RULE_DATA_FIELD_2tf,
    RTL8371C_ACL_RULE_DATA_FIELD_1tf,
    RTL8371C_ACL_RULE_DATA_FIELD_0tf,
    RTL8371C_ACL_RULE_DATA_ACTIVE_PMSKtf,
    RTL8371C_ACL_RULE_DATA_FMT_TAGtf,
    RTL8371C_ACL_RULE_DATA_TIDtf,
    RTL8371C_ACL_RULE_DATAFIELD_LIST_END,
} rtk_rtl8371c_acl_rule_dataField_list_t;

typedef enum rtk_rtl8371c_acl_rule_maskField_list_e
{
    RTL8371C_ACL_RULE_MASK_FIELD_7tf,
    RTL8371C_ACL_RULE_MASK_FIELD_6tf,
    RTL8371C_ACL_RULE_MASK_FIELD_5tf,
    RTL8371C_ACL_RULE_MASK_FIELD_4tf,
    RTL8371C_ACL_RULE_MASK_FIELD_3tf,
    RTL8371C_ACL_RULE_MASK_FIELD_2tf,
    RTL8371C_ACL_RULE_MASK_FIELD_1tf,
    RTL8371C_ACL_RULE_MASK_FIELD_0tf,
    RTL8371C_ACL_RULE_MASK_ACTIVE_PMSKtf,
    RTL8371C_ACL_RULE_MASK_FMT_TAGtf,
    RTL8371C_ACL_RULE_MASK_TIDtf,
    RTL8371C_ACL_RULE_MASKFIELD_LIST_END,
} rtk_rtl8371c_acl_rule_maskField_list_t;

extern rtksw_table_t rtk_rtl8371c_table_list[];


#endif    /* __RTK_RTL8371C_TABLE_STRUCT_H__ */
