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

#include <rtk_rtl8371c_table_struct.h>
#include <rtk_rtl8371c_tableField_list.h>

rtksw_table_t rtk_rtl8371c_table_list[] =
{

    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* table name       */      "VLAN_TBL",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* access table type */     3,
        /* table size */            4096,
        /* total data registers */  2,
        /* total field numbers */   RTL8371C_VLAN_TBLFIELD_LIST_END,
        /* table fields */          RTL8371C_VLAN_TBL_FIELDS
    },

    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* table name       */      "L2_IPMC",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* access table type */     4,
        /* table size */            2048,
        /* total data registers */  5,
        /* total field numbers */   RTL8371C_L2_IPMCFIELD_LIST_END,
        /* table fields */          RTL8371C_L2_IPMC_FIELDS
    },

    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* table name       */      "L2_MC",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* access table type */     4,
        /* table size */            2048,
        /* total data registers */  5,
        /* total field numbers */   RTL8371C_L2_MCFIELD_LIST_END,
        /* table fields */          RTL8371C_L2_MC_FIELDS
    },

    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* table name       */      "L2_UC",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* access table type */     4,
        /* table size */            2048,
        /* total data registers */  5,
        /* total field numbers */   RTL8371C_L2_UCFIELD_LIST_END,
        /* table fields */          RTL8371C_L2_UC_FIELDS
    },

    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* table name       */      "ACL_ACT_ENTRY",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* access table type */     2,
        /* table size */            64,
        /* total data registers */  4,
        /* total field numbers */   RTL8371C_ACL_ACT_ENTRYFIELD_LIST_END,
        /* table fields */          RTL8371C_ACL_ACT_ENTRY_FIELDS
    },

    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* table name       */      "ACL_RULE_DATA",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* access table type */     1,
        /* table size */            64,
        /* total data registers */  10,
        /* total field numbers */   RTL8371C_ACL_RULE_DATAFIELD_LIST_END,
        /* table fields */          RTL8371C_ACL_RULE_DATA_FIELDS
    },

    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* table name       */      "ACL_RULE_MASK",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* access table type */     1,
        /* table size */            64,
        /* total data registers */  9,
        /* total field numbers */   RTL8371C_ACL_RULE_MASKFIELD_LIST_END,
        /* table fields */          RTL8371C_ACL_RULE_MASK_FIELDS
    },
};

