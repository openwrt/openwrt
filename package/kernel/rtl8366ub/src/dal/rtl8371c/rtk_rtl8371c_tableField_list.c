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


rtksw_tableField_t RTL8371C_VLAN_TBL_FIELDS[] =
{
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "RSV",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   21,
        /* len */   11,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "IVL_SVL",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   20,
        /* len */   1,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "SVLAN_CHK_IVL_SVL",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   19,
        /* len */   1,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "FID_MSTI",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   16,
        /* len */   3,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "UNTAGSET",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   8,
        /* len */   8,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "MBR",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   0,
        /* len */   8,
    },
};

rtksw_tableField_t RTL8371C_L2_IPMC_FIELDS[] =
{
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "VALID",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   71,
        /* len */   1,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "STATIC",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   70,
        /* len */   1,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "MBR",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   62,
        /* len */   8,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "IVL_SVL",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   61,
        /* len */   1,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "L3LOOKUP",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   60,
        /* len */   1,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "DIP",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   32,
        /* len */   28,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "SIP",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   0,
        /* len */   32,
    },
};

rtksw_tableField_t RTL8371C_L2_MC_FIELDS[] =
{
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "VALID",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   71,
        /* len */   1,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "STATIC",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   70,
        /* len */   1,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "MBR",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   62,
        /* len */   8,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "IVL_SVL",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   61,
        /* len */   1,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "L3LOOKUP",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   60,
        /* len */   1,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "VID_FID",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   48,
        /* len */   12,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "MAC",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   0,
        /* len */   48,
    },
};

rtksw_tableField_t RTL8371C_L2_UC_FIELDS[] =
{
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "VALID",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   71,
        /* len */   1,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "STATIC",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   70,
        /* len */   1,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "AGE",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   65,
        /* len */   3,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "SPA",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   62,
        /* len */   3,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "IVL_SVL",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   61,
        /* len */   1,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "L3LOOKUP",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   60,
        /* len */   1,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "VID_FID",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   48,
        /* len */   12,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "MAC",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   0,
        /* len */   48,
    },
};

rtksw_tableField_t RTL8371C_ACL_ACT_ENTRY_FIELDS[] =
{
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "GPIO_PIN",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   59,
        /* len */   5,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "GPIO_EN",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   58,
        /* len */   1,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "ACL_INT",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   57,
        /* len */   1,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "ACL_PRI",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   51,
        /* len */   6,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "PRI_CAT",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   49,
        /* len */   2,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "FWD_EXT",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   48,
        /* len */   1,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "ACLPMSK",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   40,
        /* len */   8,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "FWD_ACT",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   38,
        /* len */   2,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "METER_LOG_IDX",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   32,
        /* len */   6,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "SVID",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   20,
        /* len */   12,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "SACT",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   18,
        /* len */   2,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "TAG_FMT",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   16,
        /* len */   2,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "CACT_EXT",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   14,
        /* len */   2,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "CVID",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   2,
        /* len */   12,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "CACT",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   0,
        /* len */   2,
    },
};

rtksw_tableField_t RTL8371C_ACL_RULE_DATA_FIELDS[] =
{
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "VALID",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   144,
        /* len */   1,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "FIELD_7",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   128,
        /* len */   16,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "FIELD_6",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   112,
        /* len */   16,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "FIELD_5",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   96,
        /* len */   16,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "FIELD_4",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   80,
        /* len */   16,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "FIELD_3",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   64,
        /* len */   16,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "FIELD_2",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   48,
        /* len */   16,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "FIELD_1",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   32,
        /* len */   16,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "FIELD_0",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   16,
        /* len */   16,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "ACTIVE_PMSK",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   8,
        /* len */   8,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "FMT_TAG",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   3,
        /* len */   5,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "TID",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   0,
        /* len */   3,
    },
};

rtksw_tableField_t RTL8371C_ACL_RULE_MASK_FIELDS[] =
{
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "FIELD_7",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   128,
        /* len */   16,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "FIELD_6",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   112,
        /* len */   16,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "FIELD_5",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   96,
        /* len */   16,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "FIELD_4",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   80,
        /* len */   16,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "FIELD_3",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   64,
        /* len */   16,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "FIELD_2",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   48,
        /* len */   16,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "FIELD_1",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   32,
        /* len */   16,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "FIELD_0",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   16,
        /* len */   16,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "ACTIVE_PMSK",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   8,
        /* len */   8,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "FMT_TAG",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   3,
        /* len */   5,
    },
    {
        #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        /* name */  "TID",
        #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
        /* lsp */   0,
        /* len */   3,
    },
};

