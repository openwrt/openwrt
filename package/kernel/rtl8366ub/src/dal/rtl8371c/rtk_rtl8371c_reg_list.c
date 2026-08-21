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

#include <rtk_rtl8371c_reg_struct.h>
#include <rtk_rtl8371c_regField_list.h>

rtksw_reg_t rtk_rtl8371c_reg_list[] =
{

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MODEL_NAME_ID",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MODEL_NAME_ID_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MODEL_NAME_INFO",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x6,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MODEL_NAME_INFO_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CHIP_MODE_INFO",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x8,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CHIP_MODE_INFO_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CHIP_INFO",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0xA,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CHIP_INFO_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CHIP_RL_INFO",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0xC,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CHIP_RL_INFO_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_EN_DIS",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0xE,
        /* field numbers */     16,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_EN_DIS_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CHIP_INFO_PROTECT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x10,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CHIP_INFO_PROTECT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CHIP_RST",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x14,
        /* field numbers */     9,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CHIP_RST_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "HSA_DBG_REG1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2E40,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   HSA_DBG_REG1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "HSA_DBG_REG2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2E42,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   HSA_DBG_REG2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "HSA_DBG_REG3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2E44,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   HSA_DBG_REG3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "HSA_DBG_REG4",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2E46,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   HSA_DBG_REG4_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "HSA_DBG_REG5",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2E48,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   HSA_DBG_REG5_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "HSA_DBG_REG6",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2E4A,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   HSA_DBG_REG6_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "HSA_DBG_REG7",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2E4C,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   HSA_DBG_REG7_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "HSA_DBG_REG8",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2E4E,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   HSA_DBG_REG8_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "HSA_DBG_REG9",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2E50,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   HSA_DBG_REG9_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "HSA_DBG_REG10",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2E52,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   HSA_DBG_REG10_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "HSA_DBG_REG11",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2E54,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   HSA_DBG_REG11_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "HSA_DBG_REG12",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2E56,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   HSA_DBG_REG12_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "HSA_DBG_REG13",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2E58,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   HSA_DBG_REG13_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "HSA_DBG_REG14",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2E5A,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   HSA_DBG_REG14_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "HSA_DBG_REG15",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2E5C,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   HSA_DBG_REG15_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "HAS_DBG_REG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2E5E,
        /* field numbers */     6,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   HAS_DBG_REG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "BIST_FAIL_ADDR0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3EF0,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   BIST_FAIL_ADDR0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "BIST_FAIL_ADDR1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3EF2,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   BIST_FAIL_ADDR1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "BIST_FAIL_ADDR2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3EF4,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   BIST_FAIL_ADDR2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "BIST_FAIL_ADDR3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3EF6,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   BIST_FAIL_ADDR3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACCESS_ADDR0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3EF8,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACCESS_ADDR0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACCESS_ADDR1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3EFA,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACCESS_ADDR1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACCESS_ADDR2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3EFC,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACCESS_ADDR2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACCESS_ADDR3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3EFE,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACCESS_ADDR3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACCESS_ADDR4",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3F00,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACCESS_ADDR4_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACCESS_ADDR5",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3F02,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACCESS_ADDR5_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACCESS_ADDR6",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3F04,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACCESS_ADDR6_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACCESS_ADDR7",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3F06,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACCESS_ADDR7_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PKB_ACC_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3F08,
        /* field numbers */     9,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PKB_ACC_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PKB_RD_DATA_ACC",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3F0A,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PKB_RD_DATA_ACC_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "HT_WR_DATA",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3F0C,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   HT_WR_DATA_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACC_RD_DATA",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3F0E,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACC_RD_DATA_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "HT_ACC_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3F10,
        /* field numbers */     5,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   HT_ACC_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PKB_ACC_CTRL1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3F12,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PKB_ACC_CTRL1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DUMY_REG0_CTRLCKT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3F14,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   DUMY_REG0_CTRLCKT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DUMY_REG1_CTRLCKT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3F16,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   DUMY_REG1_CTRLCKT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DUMY_REG2_CTRLCKT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3F18,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   DUMY_REG2_CTRLCKT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NRESTORE_MAGIC_NUM",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x16,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NRESTORE_MAGIC_NUM_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NRESTORE_MISC",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x18,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NRESTORE_MISC_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBIST_GLB_RESULT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3B80,
        /* field numbers */     8,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MBIST_GLB_RESULT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBIST_GLB_RESULT1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3B82,
        /* field numbers */     8,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MBIST_GLB_RESULT1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBISD_GLB_CFG_RESULT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3B84,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MBISD_GLB_CFG_RESULT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBISD_GLB_DATA_RESULT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3B86,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 18,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   MBISD_GLB_DATA_RESULT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBISD_GLB_CFG_RESULT1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3BAC,
        /* field numbers */     6,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MBISD_GLB_CFG_RESULT1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBIST_GLB_RESULT2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3BAE,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MBIST_GLB_RESULT2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBIST_GLB_RESULT3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3BB0,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MBIST_GLB_RESULT3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBIST_GLB_RESULT4",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3BB2,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MBIST_GLB_RESULT4_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBISD_CAM_CFG_RESULT2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3BB4,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MBISD_CAM_CFG_RESULT2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBISD_CAM_RESULT0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3BB6,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MBISD_CAM_RESULT0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBISD_CAM_RESULT1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3BB8,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 8,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   MBISD_CAM_RESULT1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBISD_CAM_RESULT2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3BCA,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 8,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   MBISD_CAM_RESULT2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBISD_CAM_RESULT3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3BDC,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 3,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   MBISD_CAM_RESULT3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBIST_MIB_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x520,
        /* field numbers */     8,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MBIST_MIB_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBIST_MIB_RESULT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x522,
        /* field numbers */     6,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MBIST_MIB_RESULT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MIB_MEM_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x524,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MIB_MEM_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBIST_CVLANRAM_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3A60,
        /* field numbers */     8,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MBIST_CVLANRAM_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBIST_CVLANRAM_RESULT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3A62,
        /* field numbers */     6,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MBIST_CVLANRAM_RESULT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CVLANRAM_MEM_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3A64,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CVLANRAM_MEM_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBIST_L2RAM_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3A66,
        /* field numbers */     9,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MBIST_L2RAM_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBIST_L2RAM_RESULT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3A68,
        /* field numbers */     6,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MBIST_L2RAM_RESULT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "L2_MEM_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3A6A,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   L2_MEM_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "L2_MEM_CFG1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3A6C,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   L2_MEM_CFG1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBIST_ACTRAM_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3A6E,
        /* field numbers */     8,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MBIST_ACTRAM_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBIST_ACTRAM_RESULT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3A70,
        /* field numbers */     6,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MBIST_ACTRAM_RESULT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACTRAM_MEM_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3A72,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACTRAM_MEM_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBIST_TCAM_RESULT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3A74,
        /* field numbers */     6,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MBIST_TCAM_RESULT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBIST_TCAM_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3A76,
        /* field numbers */     6,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MBIST_TCAM_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBIST_OUTQ_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x28A0,
        /* field numbers */     13,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MBIST_OUTQ_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBIST_OUTQ_CFG1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x28A2,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MBIST_OUTQ_CFG1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBIST_OUTQ_RESULT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x28A4,
        /* field numbers */     7,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MBIST_OUTQ_RESULT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBIST_OUTQ_RESULT1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x28A6,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MBIST_OUTQ_RESULT1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "OUTQRAM_MEM_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x28A8,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   OUTQRAM_MEM_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "OUTQRAM_MEM_CFG1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x28AA,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   OUTQRAM_MEM_CFG1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "OUTQRAM_MEM_CFG2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x28AC,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   OUTQRAM_MEM_CFG2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBIST_INQ_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x28AE,
        /* field numbers */     9,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MBIST_INQ_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBIST_INQ_RESULT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x28B0,
        /* field numbers */     6,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MBIST_INQ_RESULT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "INQ_MEM_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x28B2,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   INQ_MEM_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBIST_HSA_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x28B4,
        /* field numbers */     8,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MBIST_HSA_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBIST_HSA_RESULT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x28B6,
        /* field numbers */     6,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MBIST_HSA_RESULT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "HSA_MEM_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x28B8,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   HSA_MEM_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBIST_NIC_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C30,
        /* field numbers */     8,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MBIST_NIC_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBIST_NIC_RESULT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C32,
        /* field numbers */     6,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MBIST_NIC_RESULT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NIC_MEM_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C34,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NIC_MEM_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBIST_ERAM_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C00,
        /* field numbers */     8,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MBIST_ERAM_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBIST_ERAM_CFG1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C02,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MBIST_ERAM_CFG1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBIST_ERAM_RESULT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C04,
        /* field numbers */     6,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MBIST_ERAM_RESULT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ERAM_MEM_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C06,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ERAM_MEM_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ERAM_MEM_CFG1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C08,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ERAM_MEM_CFG1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ERAM_MEM_CFG2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C0A,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ERAM_MEM_CFG2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBIST_IRAM_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C0C,
        /* field numbers */     8,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MBIST_IRAM_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBIST_IRAM_RESULT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C0E,
        /* field numbers */     6,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MBIST_IRAM_RESULT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "IRAM_MEM_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C10,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   IRAM_MEM_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBIST_IROM_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C12,
        /* field numbers */     8,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MBIST_IROM_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBIST_IROM_RESULT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C14,
        /* field numbers */     6,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MBIST_IROM_RESULT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "IROM_MEM_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C16,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   IROM_MEM_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBIST_PB_CFG_LOW",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3F1A,
        /* field numbers */     8,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MBIST_PB_CFG_LOW_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBIST_PB_CFG1_LOW",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3F1C,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MBIST_PB_CFG1_LOW_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBIST_PB_RESULT_LOW",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3F1E,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MBIST_PB_RESULT_LOW_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBIST_PB_RESULT1_LOW",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3F20,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MBIST_PB_RESULT1_LOW_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBIST_PB_CFG_HIGH",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3F22,
        /* field numbers */     8,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MBIST_PB_CFG_HIGH_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBIST_PB_CFG1_HIGH",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3F24,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MBIST_PB_CFG1_HIGH_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBIST_PB_RESULT_HIGH",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3F26,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MBIST_PB_RESULT_HIGH_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBIST_PB_RESULT1_HIGH",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3F28,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MBIST_PB_RESULT1_HIGH_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PB_MEM_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3F2A,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PB_MEM_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PB_MEM_CFG1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3F2C,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PB_MEM_CFG1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PB_MEM_CFG2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3F2E,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PB_MEM_CFG2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBIST_HT_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3F30,
        /* field numbers */     7,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MBIST_HT_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBIST_HT_RESULT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3F32,
        /* field numbers */     6,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MBIST_HT_RESULT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "HT_MEM_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3F34,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   HT_MEM_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBIST_NCTLRAM_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4110,
        /* field numbers */     8,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MBIST_NCTLRAM_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBIST_NCTLRAM_RESULT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4112,
        /* field numbers */     6,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MBIST_NCTLRAM_RESULT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NCTLRAM_MEM_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4114,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NCTLRAM_MEM_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBIST_NCTLROM_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4116,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MBIST_NCTLROM_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBIST_NCTLROM_RESULT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4118,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MBIST_NCTLROM_RESULT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBIST_NCTLROM_RESULT1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x411A,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MBIST_NCTLROM_RESULT1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBIST_NCTLROM_RESULT2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x411C,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MBIST_NCTLROM_RESULT2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NCTLROM_MEM_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x411E,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NCTLROM_MEM_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBISTWRAP_DUMMY_REG1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x6F0,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MBISTWRAP_DUMMY_REG1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBISTWRAP_DUMMY_REG2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x6F2,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MBISTWRAP_DUMMY_REG2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBISTWRAP_DUMMY_REG3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x6F4,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MBISTWRAP_DUMMY_REG3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBISTWRAP_DUMMY_REG4",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x6F6,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MBISTWRAP_DUMMY_REG4_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MBISTWRAP_DUMMY_REG5",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x6F8,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MBISTWRAP_DUMMY_REG5_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RTL_OUI_CFG0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1E,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RTL_OUI_CFG0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RTL_OUI_CFG1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x20,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RTL_OUI_CFG1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PCSXF_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x22,
        /* field numbers */     5,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PCSXF_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHYID_CFG0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x24,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PHYID_CFG0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "GPHY_ACS_MISC",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x26,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   GPHY_ACS_MISC_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "GPHY_OCP_MSB_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x28,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   GPHY_OCP_MSB_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "GPHY_OCP_MSB_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2A,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   GPHY_OCP_MSB_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "GPHY_OCP_MSB_2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2C,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   GPHY_OCP_MSB_2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "WGPHY_MISC_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2E,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   WGPHY_MISC_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "WGPHY_MISC_2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x30,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   WGPHY_MISC_2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "IO_MISC_FUNC",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x32,
        /* field numbers */     6,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   IO_MISC_FUNC_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EF_SLV_CTRL_3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x34,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EF_SLV_CTRL_3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "INBAND_14C",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x36,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   INBAND_14C_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "BTCAM_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x38,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   BTCAM_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "BOND4READ_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3A,
        /* field numbers */     15,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   BOND4READ_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "BOND4READ_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   BOND4READ_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "REG_TO_ECO0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3E,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   REG_TO_ECO0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "REG_TO_ECO1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x40,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   REG_TO_ECO1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "REG_TO_ECO4",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x42,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   REG_TO_ECO4_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHYSTS_CTRL0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x27A0,
        /* field numbers */     5,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PHYSTS_CTRL0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "BUFF_RST_CTRL0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x44,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   BUFF_RST_CTRL0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "BUFF_RST_CTRL1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x46,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   BUFF_RST_CTRL1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "BUFF_RST_CTRL2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x48,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   BUFF_RST_CTRL2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "BUFF_RST_CTRL3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4A,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   BUFF_RST_CTRL3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "BUFF_RST_CTRL4",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4C,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   BUFF_RST_CTRL4_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "BUFF_RST_CTRL5",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4E,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   BUFF_RST_CTRL5_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "BUFF_RST_CTRL6",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x50,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   BUFF_RST_CTRL6_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "BUFF_RST_CTRL7",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x52,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   BUFF_RST_CTRL7_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "BUFF_RST_CTRL8",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x54,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   BUFF_RST_CTRL8_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "BUFF_RST_CTRL9",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x56,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   BUFF_RST_CTRL9_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "BUFF_RST_CTRL10",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x58,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   BUFF_RST_CTRL10_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "BUFF_RST_CTRL11",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x5A,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   BUFF_RST_CTRL11_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "TOP_CON0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x5C,
        /* field numbers */     5,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   TOP_CON0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "TOP_CON1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x5E,
        /* field numbers */     8,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   TOP_CON1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SWR_FPWM",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x60,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SWR_FPWM_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CHIP_70B_DUMMY_4",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x62,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CHIP_70B_DUMMY_4_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SHORT_PRMB",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x64,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SHORT_PRMB_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_INDSC_THR_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x66,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_INDSC_THR_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_INDSC_THR_CTRL_TG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x68,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_INDSC_THR_CTRL_TG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "BUFF_RST_CTRL12",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x6A,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   BUFF_RST_CTRL12_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "BUFF_RST_CTRL13",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x6C,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   BUFF_RST_CTRL13_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "BUFF_RST_CTRL14",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x6E,
        /* field numbers */     5,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   BUFF_RST_CTRL14_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "BUFF_RST_CTRL15",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x70,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   BUFF_RST_CTRL15_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "BUFF_RST_CTRL16",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x72,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   BUFF_RST_CTRL16_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "BUFF_RST_CTRL17",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x74,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   BUFF_RST_CTRL17_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "BUFF_RST_CTRL18",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x76,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   BUFF_RST_CTRL18_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "BUFF_RST_CTRL19",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x78,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   BUFF_RST_CTRL19_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "BUFF_RST_CTRL20",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x7A,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   BUFF_RST_CTRL20_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "BUFF_RST_CTRL21",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x7C,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   BUFF_RST_CTRL21_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "BUFF_RST_CTRL22",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x7E,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   BUFF_RST_CTRL22_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "BUFF_RST_CTRL23",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x80,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   BUFF_RST_CTRL23_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "BUFF_RST_CTRL24",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x82,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   BUFF_RST_CTRL24_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "BUFF_RST_CTRL25",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x84,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   BUFF_RST_CTRL25_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "BUFF_RST_CTRL26",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x86,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   BUFF_RST_CTRL26_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "BUFF_RST_CTRL27",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x88,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   BUFF_RST_CTRL27_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "BUFF_RST_CTRL28",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x8A,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   BUFF_RST_CTRL28_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "BUFF_RST_CTRL29",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x8C,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   BUFF_RST_CTRL29_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "BUFF_RST_CTRL30",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x8E,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   BUFF_RST_CTRL30_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "BUFF_RST_CTRL31",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x90,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   BUFF_RST_CTRL31_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_M_I2C_CTL_STA_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x92,
        /* field numbers */     6,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_M_I2C_CTL_STA_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_DUMB_RW_ADDR_0_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x94,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_DUMB_RW_ADDR_0_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_DUMB_RW_ADDR_1_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x96,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_DUMB_RW_ADDR_1_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_DUMB_RW_DATA_0_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x98,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_DUMB_RW_DATA_0_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_DUMB_RW_DATA_1_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x9A,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_DUMB_RW_DATA_1_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_DUMB_RW_CTL_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x9C,
        /* field numbers */     6,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_DUMB_RW_CTL_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_M_I2C_SYS_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x9E,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_M_I2C_SYS_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_M_I2C_CTL_STA_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0xA0,
        /* field numbers */     6,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_M_I2C_CTL_STA_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_DUMB_RW_ADDR_0_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0xA2,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_DUMB_RW_ADDR_0_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_DUMB_RW_ADDR_1_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0xA4,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_DUMB_RW_ADDR_1_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_DUMB_RW_DATA_0_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0xA6,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_DUMB_RW_DATA_0_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_DUMB_RW_DATA_1_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0xA8,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_DUMB_RW_DATA_1_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_DUMB_RW_CTL_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0xAA,
        /* field numbers */     6,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_DUMB_RW_CTL_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_M_I2C_SYS_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0xAC,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_M_I2C_SYS_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SET_SGMII_PHY_SIDE",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0xAE,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SET_SGMII_PHY_SIDE_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_MODE_CTRL_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0xB0,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SDS_MODE_CTRL_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "L2_MISC_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0xB2,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   L2_MISC_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "BUF_RST_CNT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0xB4,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   BUF_RST_CNT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "TM_CTRL_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0xB6,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   TM_CTRL_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "TM_CTRL_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0xB8,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   TM_CTRL_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "TM_CTRL_2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0xBA,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   TM_CTRL_2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "TM_CTRL_3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0xBC,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   TM_CTRL_3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "TM_CTRL_4",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0xBE,
        /* field numbers */     10,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   TM_CTRL_4_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "TM_CTRL_5",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0xC0,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   TM_CTRL_5_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "TM_CTRL_6",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0xC2,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   TM_CTRL_6_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "TM_RESULT_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0xC4,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   TM_RESULT_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MAC_TXILDE",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0xC6,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MAC_TXILDE_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "TM_RESULT_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0xC8,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   TM_RESULT_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "TM_RESULT_2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0xCA,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   TM_RESULT_2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "HT_PB_IND_ACS_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0xCC,
        /* field numbers */     5,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   HT_PB_IND_ACS_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "HT_PB_IND_ACS_ADDR",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0xCE,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   HT_PB_IND_ACS_ADDR_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "HT_PB_IND_ACS_DIN0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0xD0,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   HT_PB_IND_ACS_DIN0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "HT_PB_IND_ACS_DIN1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0xD2,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   HT_PB_IND_ACS_DIN1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "HT_PB_IND_ACS_DOUT0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0xD4,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   HT_PB_IND_ACS_DOUT0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "HT_PB_IND_ACS_DOUT1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0xD6,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   HT_PB_IND_ACS_DOUT1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "TM_RESULT_3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0xD8,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   TM_RESULT_3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "TM_RESULT_4",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0xDA,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   TM_RESULT_4_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "TM_RESULT_5",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0xDC,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   TM_RESULT_5_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "TM_RESULT_6",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0xDE,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   TM_RESULT_6_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "TM_RESULT_7",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0xE0,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   TM_RESULT_7_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "TM_RESULT_8",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0xE2,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   TM_RESULT_8_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "TM_RESULT_8A",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0xE4,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   TM_RESULT_8A_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FIBER_CFG_5_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0xE6,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FIBER_CFG_5_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY_STAT_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0xE8,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PHY_STAT_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY_STAT_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0xEA,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PHY_STAT_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY_STAT_2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0xEC,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PHY_STAT_2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY_STAT_3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0xEE,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PHY_STAT_3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY_STAT_4",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0xF0,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PHY_STAT_4_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_STAT_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0xF2,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SDS_STAT_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_STAT_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0xF4,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SDS_STAT_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MAC_LINK_STAT_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0xF6,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MAC_LINK_STAT_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_GPHY_RESET",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0xF8,
        /* field numbers */     7,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_GPHY_RESET_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHYSTS_CTRL0_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x27A2,
        /* field numbers */     8,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PHYSTS_CTRL0_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "BOND_DBG_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0xFA,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   BOND_DBG_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CHIP_DUMMY8",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0xFC,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CHIP_DUMMY8_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "STRP_DBG_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0xFE,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   STRP_DBG_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "STRP_DBG_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x100,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   STRP_DBG_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "STRP_DBG_2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x102,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   STRP_DBG_2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "STRP_DBG_3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x104,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   STRP_DBG_3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EE_IMAGE_ID_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x106,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EE_IMAGE_ID_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NCTL_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x108,
        /* field numbers */     5,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NCTL_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NCTL_P00_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x10A,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NCTL_P00_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NCTL_P00_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x10C,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NCTL_P00_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NCTL_P01_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x10E,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NCTL_P01_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NCTL_P01_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x110,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NCTL_P01_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NCTL_P02_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x112,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NCTL_P02_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NCTL_P02_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x114,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NCTL_P02_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NCTL_P03_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x116,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NCTL_P03_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NCTL_P03_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x118,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NCTL_P03_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NCTL_P04_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x11A,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NCTL_P04_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NCTL_P04_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x11C,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NCTL_P04_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NCTL_P05_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x11E,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NCTL_P05_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NCTL_P05_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x120,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NCTL_P05_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NCTL_P06_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x122,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NCTL_P06_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NCTL_P06_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x124,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NCTL_P06_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NCTL_P07_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x126,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NCTL_P07_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NCTL_P07_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x128,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NCTL_P07_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NCTL_P08_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x12A,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NCTL_P08_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NCTL_P08_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x12C,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NCTL_P08_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NCTL_P09_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x12E,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NCTL_P09_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NCTL_P09_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x130,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NCTL_P09_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NCTL_P10_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x132,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NCTL_P10_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NCTL_P10_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x134,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NCTL_P10_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NCTL_P11_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x136,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NCTL_P11_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NCTL_P11_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x138,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NCTL_P11_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NCTL_P12_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x13A,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NCTL_P12_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NCTL_P12_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x13C,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NCTL_P12_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NCTL_P13_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x13E,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NCTL_P13_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NCTL_P13_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x140,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NCTL_P13_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NCTL_P14_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x142,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NCTL_P14_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NCTL_P14_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x144,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NCTL_P14_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NCTL_P15_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x146,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NCTL_P15_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NCTL_P15_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x148,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NCTL_P15_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NCTL_SRCA_VAL0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x14A,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NCTL_SRCA_VAL0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NCTL_SRCA_VAL1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x14C,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NCTL_SRCA_VAL1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NCTL_SRCA_VAL2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x14E,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NCTL_SRCA_VAL2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NCTL_SRCB_VAL0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x150,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NCTL_SRCB_VAL0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NCTL_SRCB_VAL1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x152,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NCTL_SRCB_VAL1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NCTL_SRCB_VAL2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x154,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NCTL_SRCB_VAL2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NCTL_CIO_SEL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x156,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NCTL_CIO_SEL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NCTL_CO_MON",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x158,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NCTL_CO_MON_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NCTL_CI_MSB_MON",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x15A,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NCTL_CI_MSB_MON_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NCTL_PC_MON",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x15C,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NCTL_PC_MON_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NCTL_SRAM_IN",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x15E,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NCTL_SRAM_IN_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NCTL_EN",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x160,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NCTL_EN_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NCTL_CI_LSB_MON",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x162,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NCTL_CI_LSB_MON_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NCTL_RAM_INDACS_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x164,
        /* field numbers */     5,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NCTL_RAM_INDACS_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NCTL_RAM_INDACS_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x166,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NCTL_RAM_INDACS_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NCTL_RAM_INDACS_2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x168,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NCTL_RAM_INDACS_2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_NCTL_LINK_SRC",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x16A,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_NCTL_LINK_SRC_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DBGO_L",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x16C,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   DBGO_L_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DBGO_H",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x16E,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   DBGO_H_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CHIP_DUMMY0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x170,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CHIP_DUMMY0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CHIP_DUMMY1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x172,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CHIP_DUMMY1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CHIP_DUMMY2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x174,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CHIP_DUMMY2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CHIP_DUMMY3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x176,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CHIP_DUMMY3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CHIP_DUMMY4",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x178,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CHIP_DUMMY4_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CHIP_DUMMY5",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x17A,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CHIP_DUMMY5_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CHIP_DUMMY6",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x17C,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CHIP_DUMMY6_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CHIP_DUMMY7",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x17E,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CHIP_DUMMY7_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CHIP_DUMMY10",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x180,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CHIP_DUMMY10_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CHIP_DUMMY11",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x182,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CHIP_DUMMY11_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CHIP_DUMMY12",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x184,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CHIP_DUMMY12_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CHIP_DUMMY13",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x186,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CHIP_DUMMY13_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CHIP_DUMMY14",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x188,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CHIP_DUMMY14_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CHIP_DUMMY15",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x18A,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CHIP_DUMMY15_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CHIP_DUMMY16",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x18C,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CHIP_DUMMY16_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CHIP_DUMMY17",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x18E,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CHIP_DUMMY17_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CHIP_DUMMY18",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x190,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CHIP_DUMMY18_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CHIP_DUMMY19",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x192,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CHIP_DUMMY19_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CHIP_DUMMY20",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x194,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CHIP_DUMMY20_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SW_PLL_CTRL_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4C0,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SW_PLL_CTRL_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SW_PLL_CTRL_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4C2,
        /* field numbers */     5,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SW_PLL_CTRL_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SW_PLL_SSC_CTRL_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4C4,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SW_PLL_SSC_CTRL_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SW_PLL_SSC_CTRL_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4C6,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SW_PLL_SSC_CTRL_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SW_PLL_MISC_CTRL_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4C8,
        /* field numbers */     9,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SW_PLL_MISC_CTRL_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SW_PLL_MISC_CTRL_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4CA,
        /* field numbers */     11,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SW_PLL_MISC_CTRL_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SW_PLL_MISC_CTRL_2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4CC,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SW_PLL_MISC_CTRL_2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "XTAL_CTRL_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4CE,
        /* field numbers */     6,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   XTAL_CTRL_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PLL_TOP_CTRL_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4D0,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PLL_TOP_CTRL_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PLL_TOP_CTRL_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4D2,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PLL_TOP_CTRL_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PLL_TOP_CTRL_2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4D4,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PLL_TOP_CTRL_2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "UTP_FIB_DET",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x196,
        /* field numbers */     9,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   UTP_FIB_DET_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "UTP_FIB_DET_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x198,
        /* field numbers */     8,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   UTP_FIB_DET_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "UTP_FIB_DET_2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x19A,
        /* field numbers */     6,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   UTP_FIB_DET_2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "UTP_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x19C,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   UTP_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MC_SDS_MODE_CONFIG1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x19E,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MC_SDS_MODE_CONFIG1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MC_SDS_MODE_CONFIG2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1A0,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MC_SDS_MODE_CONFIG2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MC_SDS_MODE_CONFIG3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1A2,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MC_SDS_MODE_CONFIG3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MC_SDS_MODE_CONFIG4",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1A4,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MC_SDS_MODE_CONFIG4_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MC_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1A6,
        /* field numbers */     9,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MC_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ESD_SIG_CTRL_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1A8,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ESD_SIG_CTRL_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ESD_SIG_CTRL_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1AA,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ESD_SIG_CTRL_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_PARTNER_ABLTY_FRC_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1AC,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SDS_PARTNER_ABLTY_FRC_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_MDX_ABLTY_FRC_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1AE,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SDS_MDX_ABLTY_FRC_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_PARTNER_ABLTY_FRC_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1B0,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SDS_PARTNER_ABLTY_FRC_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_MDX_ABLTY_FRC_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1B2,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SDS_MDX_ABLTY_FRC_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHYAD",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1B4,
        /* field numbers */     8,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PHYAD_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "INTERNAL_PHY_MDC_DRV",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1B6,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   INTERNAL_PHY_MDC_DRV_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "LINK_DOWN_CHK",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1B8,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   LINK_DOWN_CHK_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHYACK_TO",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1BA,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PHYACK_TO_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MDXACK_TO",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1BC,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MDXACK_TO_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MDX_PHY_REG0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1BE,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MDX_PHY_REG0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SMI_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x27A4,
        /* field numbers */     11,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SMI_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SMI_PORT6_7_ADDR_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x27A6,
        /* field numbers */     6,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SMI_PORT6_7_ADDR_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MDIO_FREE_CNT_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x27A8,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MDIO_FREE_CNT_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SMI_10GPHY_POLL_SEL_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x27AA,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SMI_10GPHY_POLL_SEL_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SMI_10GPHY_POLL_SEL_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x27AC,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SMI_10GPHY_POLL_SEL_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SMI_10GPHY_POLL_SEL_2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x27AE,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SMI_10GPHY_POLL_SEL_2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RLFD_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x27B0,
        /* field numbers */     6,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RLFD_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RLFD_POLL_REG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x27B2,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RLFD_POLL_REG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "TG_POLL_REG0_CFG_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x27B4,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   TG_POLL_REG0_CFG_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "TG_POLL_REG0_CFG_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x27B6,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   TG_POLL_REG0_CFG_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "TG_POLL_REG9_CFG_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x27B8,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   TG_POLL_REG9_CFG_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "TG_POLL_REG9_CFG_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x27BA,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   TG_POLL_REG9_CFG_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "TG_POLL_REG10_CFG_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x27BC,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   TG_POLL_REG10_CFG_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "TG_POLL_REG10_CFG_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x27BE,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   TG_POLL_REG10_CFG_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SMI_IND_ACC_PHY_CTRL_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x27C0,
        /* field numbers */     7,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SMI_IND_ACC_PHY_CTRL_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SMI_IND_ACC_PHY_CTRL_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x27C2,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SMI_IND_ACC_PHY_CTRL_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SMI_IND_ACC_PHY_CTRL_2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x27C4,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SMI_IND_ACC_PHY_CTRL_2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SMI_IND_ACC_PHY_CTRL_3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x27C6,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SMI_IND_ACC_PHY_CTRL_3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SMI_IND_ACC_PHY_CTRL_4",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x27C8,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SMI_IND_ACC_PHY_CTRL_4_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SMI_IND_ACC_PHY_CTRL_5",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x27CA,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SMI_IND_ACC_PHY_CTRL_5_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SMI_REG_CHK1_CTRL0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x27CC,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SMI_REG_CHK1_CTRL0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SMI_REG_CHK1_CTRL1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x27CE,
        /* field numbers */     5,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SMI_REG_CHK1_CTRL1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SMI_REG_CHK1_CTRL2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x27D0,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SMI_REG_CHK1_CTRL2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SMI_REG_CHK1_CTRL3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x27D2,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SMI_REG_CHK1_CTRL3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SMI_REG_CHK1_CTRL4",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x27D4,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SMI_REG_CHK1_CTRL4_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SMI_REG_CHK1_CTRL5",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x27D6,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SMI_REG_CHK1_CTRL5_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SMI_REG_CHK1_CTRL6",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x27D8,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SMI_REG_CHK1_CTRL6_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SMI_REG_CHK2_CTRL0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x27DA,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SMI_REG_CHK2_CTRL0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SMI_REG_CHK2_CTRL1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x27DC,
        /* field numbers */     5,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SMI_REG_CHK2_CTRL1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SMI_REG_CHK2_CTRL2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x27DE,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SMI_REG_CHK2_CTRL2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SMI_REG_CHK2_CTRL3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x27E0,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SMI_REG_CHK2_CTRL3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SMI_REG_CHK2_CTRL4",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x27E2,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SMI_REG_CHK2_CTRL4_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SMI_REG_CHK2_CTRL5",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x27E4,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SMI_REG_CHK2_CTRL5_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SMI_REG_CHK2_CTRL6",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x27E6,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SMI_REG_CHK2_CTRL6_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SMI_REG_CHK3_CTRL0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x27E8,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SMI_REG_CHK3_CTRL0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SMI_REG_CHK3_CTRL1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x27EA,
        /* field numbers */     5,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SMI_REG_CHK3_CTRL1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SMI_REG_CHK3_CTRL2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x27EC,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SMI_REG_CHK3_CTRL2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SMI_REG_CHK3_CTRL3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x27EE,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SMI_REG_CHK3_CTRL3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SMI_REG_CHK3_CTRL4",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x27F0,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SMI_REG_CHK3_CTRL4_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SMI_REG_CHK3_CTRL5",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x27F2,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SMI_REG_CHK3_CTRL5_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SMI_REG_CHK3_CTRL6",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x27F4,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SMI_REG_CHK3_CTRL6_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SMI_REG_CHK4_CTRL0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x27F6,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SMI_REG_CHK4_CTRL0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SMI_REG_CHK4_CTRL1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x27F8,
        /* field numbers */     5,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SMI_REG_CHK4_CTRL1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SMI_REG_CHK4_CTRL2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x27FA,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SMI_REG_CHK4_CTRL2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SMI_REG_CHK4_CTRL3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x27FC,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SMI_REG_CHK4_CTRL3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SMI_REG_CHK4_CTRL4",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x27FE,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SMI_REG_CHK4_CTRL4_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SMI_REG_CHK4_CTRL5",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2800,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SMI_REG_CHK4_CTRL5_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SMI_REG_CHK4_CTRL6",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2802,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SMI_REG_CHK4_CTRL6_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SMI_REG_CHK5_CTRL0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2804,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SMI_REG_CHK5_CTRL0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SMI_REG_CHK5_CTRL1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2806,
        /* field numbers */     5,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SMI_REG_CHK5_CTRL1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SMI_REG_CHK5_CTRL2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2808,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SMI_REG_CHK5_CTRL2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SMI_REG_CHK5_CTRL3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x280A,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SMI_REG_CHK5_CTRL3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SMI_REG_CHK5_CTRL4",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x280C,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SMI_REG_CHK5_CTRL4_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SMI_REG_CHK5_CTRL5",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x280E,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SMI_REG_CHK5_CTRL5_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SMI_REG_CHK5_CTRL6",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2810,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SMI_REG_CHK5_CTRL6_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SMI_REG_CHK_RESULT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2812,
        /* field numbers */     6,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SMI_REG_CHK_RESULT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SMI_GLB_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2814,
        /* field numbers */     6,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SMI_GLB_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SMI_PHT_STS_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2816,
        /* field numbers */     5,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   SMI_PHT_STS_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "LINK_DOWN_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2826,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   LINK_DOWN_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "LINK_DELAY_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2828,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   LINK_DELAY_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "LINK_DELAY_CTRL_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x282A,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   LINK_DELAY_CTRL_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MAC_FORCE_MODE_CTRL0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x282C,
        /* field numbers */     10,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   MAC_FORCE_MODE_CTRL0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MAC_FORCE_MODE_CTRL1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x283C,
        /* field numbers */     10,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   MAC_FORCE_MODE_CTRL1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MAC_STATUS",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x284C,
        /* field numbers */     13,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   MAC_STATUS_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY_STATUS",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x285C,
        /* field numbers */     13,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   PHY_STATUS_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY_STS_CTRL_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x286C,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PHY_STS_CTRL_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY_STS_CTRL_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x286E,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PHY_STS_CTRL_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY_STS_CTRL_2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2870,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PHY_STS_CTRL_2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY_STS_CTRL_3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2872,
        /* field numbers */     3,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   PHY_STS_CTRL_3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_MODE_ADJ_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2882,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SDS_MODE_ADJ_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SMI_DUMMY_REG_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2884,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SMI_DUMMY_REG_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SMI_DUMMY_REG_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2886,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SMI_DUMMY_REG_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SMI_DUMMY_REG_2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2888,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SMI_DUMMY_REG_2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SMI_CTRL1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x288A,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SMI_CTRL1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SYNCE_CTRL_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3ED0,
        /* field numbers */     12,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SYNCE_CTRL_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SYNCE_CTRL_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3ED2,
        /* field numbers */     12,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SYNCE_CTRL_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SYNCE_CTRL_2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3ED4,
        /* field numbers */     13,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SYNCE_CTRL_2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SYNCE_CTRL_3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3ED6,
        /* field numbers */     10,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SYNCE_CTRL_3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_LOCK_SRC_SEL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3ED8,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_LOCK_SRC_SEL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SYNCE_DUMMY_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3EDA,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SYNCE_DUMMY_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SYNCE_DUMMY_2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3EDC,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SYNCE_DUMMY_2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SYNCE_DUMMY_3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3EDE,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SYNCE_DUMMY_3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SMI_SYNCE_MISC",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3FB0,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SMI_SYNCE_MISC_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EFUSE_ACCESS_EN0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4030,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EFUSE_ACCESS_EN0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "OTP_IND_CMD0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4032,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   OTP_IND_CMD0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "OTP_IND_CMD1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4034,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   OTP_IND_CMD1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EFUSE_DUMMY_REG1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4036,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EFUSE_DUMMY_REG1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "OTP_IND_WD",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4038,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   OTP_IND_WD_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "OTP_IND_RD",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x403A,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   OTP_IND_RD_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "OTP_CP_MISC",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x403C,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   OTP_CP_MISC_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CON_RD_ERR_CNT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x403E,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CON_RD_ERR_CNT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "OTP_INI_MARG_RD_ERR_DAT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4040,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   OTP_INI_MARG_RD_ERR_DAT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "OTP_INI_MARG_RD_ERR_ADR",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4042,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   OTP_INI_MARG_RD_ERR_ADR_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EFUSE_DUMMY_REG2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4044,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EFUSE_DUMMY_REG2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EFUSE_DUMMY_REG3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4046,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EFUSE_DUMMY_REG3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EFUSE_DUMMY_REG4",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4048,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EFUSE_DUMMY_REG4_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "OTP_PCK_MODE_SET",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x404A,
        /* field numbers */     8,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   OTP_PCK_MODE_SET_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "OTP_PCK_MODE_STATE",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x404C,
        /* field numbers */     8,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   OTP_PCK_MODE_STATE_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EFUSE_AUTOLOAD",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x404E,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EFUSE_AUTOLOAD_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EF_CHK_STAGE1_RD",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4050,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EF_CHK_STAGE1_RD_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EF_CHK_STAGE2_RD",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4052,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EF_CHK_STAGE2_RD_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EF_CHK_STAGE3_RD",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4054,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EF_CHK_STAGE3_RD_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CAL_CTR_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4056,
        /* field numbers */     5,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CAL_CTR_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CAL_CTR_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4058,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CAL_CTR_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CAL_CTR_2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x405A,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CAL_CTR_2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EFUSE_MISC_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x405C,
        /* field numbers */     11,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EFUSE_MISC_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EFUSE_MISC_2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x405E,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EFUSE_MISC_2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EFUSE_MISC_3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4060,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EFUSE_MISC_3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EFUSE_MISC_4",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4062,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EFUSE_MISC_4_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EFUSE_MISC_5",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4064,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EFUSE_MISC_5_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EFUSE_MISC_6",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4066,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EFUSE_MISC_6_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EFUSE_MISC_7",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4068,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EFUSE_MISC_7_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EFUSE_MISC_8",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x406A,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EFUSE_MISC_8_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "UNIQUE_ID_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x406C,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   UNIQUE_ID_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "UNIQUE_ID_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x406E,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   UNIQUE_ID_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "UNIQUE_ID_2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4070,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   UNIQUE_ID_2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "UNIQUE_ID_3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4072,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   UNIQUE_ID_3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "UNIQUE_ID_4",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4074,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   UNIQUE_ID_4_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "UNIQUE_ID_5",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4076,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   UNIQUE_ID_5_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "UNIQUE_ID_6",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4078,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   UNIQUE_ID_6_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "UNIQUE_ID_7",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x407A,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   UNIQUE_ID_7_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EFUSE_MISC_9",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x407C,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EFUSE_MISC_9_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PGMCHK_ERROR_CNT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x407E,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PGMCHK_ERROR_CNT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EFUSE_MISC_10",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4080,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EFUSE_MISC_10_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_EEE_FLG_DLY",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x6B0,
        /* field numbers */     8,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   CFG_EEE_FLG_DLY_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_PHY_MDI_REVERSE",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x6FA,
        /* field numbers */     6,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_PHY_MDI_REVERSE_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_PHY_TX_POLARITY_SWAP",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x6FC,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_PHY_TX_POLARITY_SWAP_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_PHY_OCP_TIMEOUT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x6FE,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_PHY_OCP_TIMEOUT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_PHY_PCSXF_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x700,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_PHY_PCSXF_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_PHY_PCSXF_2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x702,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_PHY_PCSXF_2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_PHY_G2XG_IPG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x704,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_PHY_G2XG_IPG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_PHY_G2XG_FIFO_THR",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x706,
        /* field numbers */     7,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_PHY_G2XG_FIFO_THR_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_PHY_G2XG_AUTORST",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x708,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_PHY_G2XG_AUTORST_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_PHY_G2XG_MISC",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x70A,
        /* field numbers */     5,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_PHY_G2XG_MISC_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_PHY_G2XG_MODULE_RST",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x70C,
        /* field numbers */     7,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_PHY_G2XG_MODULE_RST_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "P0_PHY_G2XG_BCH_ERR_FLAG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x70E,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   P0_PHY_G2XG_BCH_ERR_FLAG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "P1_PHY_G2XG_BCH_ERR_FLAG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x710,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   P1_PHY_G2XG_BCH_ERR_FLAG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_PHYMD_MISC",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x712,
        /* field numbers */     11,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   CFG_PHYMD_MISC_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY_LINK_FAULT_STS",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x716,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PHY_LINK_FAULT_STS_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_PHY_BRD",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x718,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_PHY_BRD_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_PHY_INI",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x71A,
        /* field numbers */     7,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   CFG_PHY_INI_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_PHY_POLL_CMD1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x71E,
        /* field numbers */     5,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_PHY_POLL_CMD1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_PHY_POLL_CMD2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x720,
        /* field numbers */     5,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_PHY_POLL_CMD2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_PHY_HOTCMD1_ADR",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x722,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_PHY_HOTCMD1_ADR_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_PHY_HOTCMD1_DAT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x724,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_PHY_HOTCMD1_DAT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "P0_XG2XG_IPG_DBG_INFO",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x6B4,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   P0_XG2XG_IPG_DBG_INFO_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "P0_XG2XG_PRMB_DBG_INFO",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x6B6,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   P0_XG2XG_PRMB_DBG_INFO_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "P0_XG2XG_THR_DBG_INFO",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x6B8,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   P0_XG2XG_THR_DBG_INFO_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_PHY_POLL_ADR0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x726,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_PHY_POLL_ADR0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_PHY_POLL_ADR1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x728,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_PHY_POLL_ADR1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_PHY_POLL_ADR2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x72A,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_PHY_POLL_ADR2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_PHY_POLL_ADR3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x72C,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_PHY_POLL_ADR3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_PHY_POLL_INV0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x72E,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_PHY_POLL_INV0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_PHY_POLL_INV1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x730,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_PHY_POLL_INV1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_PHY_POLL_INV2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x732,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_PHY_POLL_INV2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_PHY_POLL_INV3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x734,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_PHY_POLL_INV3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_PHY_POLL_WD0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x736,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_PHY_POLL_WD0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_PHY_POLL_WD1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x738,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_PHY_POLL_WD1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_PHY_POLL_WD2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x73A,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_PHY_POLL_WD2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_PHY_POLL_WD3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x73C,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_PHY_POLL_WD3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY_SDET_STATUS",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x73E,
        /* field numbers */     6,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PHY_SDET_STATUS_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "P0_PHY_POLL_CMD0_RDAT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x740,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   P0_PHY_POLL_CMD0_RDAT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "P0_PHY_POLL_CMD1_RDAT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x742,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   P0_PHY_POLL_CMD1_RDAT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "P0_PHY_POLL_CMD2_RDAT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x744,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   P0_PHY_POLL_CMD2_RDAT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "P0_PHY_POLL_CMD3_RDAT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x746,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   P0_PHY_POLL_CMD3_RDAT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "P1_PHY_POLL_CMD0_RDAT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x748,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   P1_PHY_POLL_CMD0_RDAT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "P1_PHY_POLL_CMD1_RDAT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x74A,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   P1_PHY_POLL_CMD1_RDAT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "P1_PHY_POLL_CMD2_RDAT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x74C,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   P1_PHY_POLL_CMD2_RDAT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "P1_PHY_POLL_CMD3_RDAT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x74E,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   P1_PHY_POLL_CMD3_RDAT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "P2_PHY_POLL_CMD0_RDAT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x750,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   P2_PHY_POLL_CMD0_RDAT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "P2_PHY_POLL_CMD1_RDAT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x752,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   P2_PHY_POLL_CMD1_RDAT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "P2_PHY_POLL_CMD2_RDAT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x754,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   P2_PHY_POLL_CMD2_RDAT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "P2_PHY_POLL_CMD3_RDAT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x756,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   P2_PHY_POLL_CMD3_RDAT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "P3_PHY_POLL_CMD0_RDAT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x758,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   P3_PHY_POLL_CMD0_RDAT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "P3_PHY_POLL_CMD1_RDAT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x75A,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   P3_PHY_POLL_CMD1_RDAT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "P3_PHY_POLL_CMD2_RDAT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x75C,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   P3_PHY_POLL_CMD2_RDAT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "P3_PHY_POLL_CMD3_RDAT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x75E,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   P3_PHY_POLL_CMD3_RDAT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "P4_PHY_POLL_CMD0_RDAT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x760,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   P4_PHY_POLL_CMD0_RDAT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "P4_PHY_POLL_CMD1_RDAT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x762,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   P4_PHY_POLL_CMD1_RDAT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "P4_PHY_POLL_CMD2_RDAT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x764,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   P4_PHY_POLL_CMD2_RDAT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "P4_PHY_POLL_CMD3_RDAT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x766,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   P4_PHY_POLL_CMD3_RDAT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "POWCTRL_BIT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x768,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   POWCTRL_BIT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY_POLL_CFG15",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x76A,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PHY_POLL_CFG15_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY_POLL_CFG16",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x76C,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PHY_POLL_CFG16_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY_POLL_CFG17",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x76E,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PHY_POLL_CFG17_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY_POLL_CFG18",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x770,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PHY_POLL_CFG18_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY_POLL_CFG19",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x772,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PHY_POLL_CFG19_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY_ABLTY_RESOLUTION_FRC_MODE",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x774,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PHY_ABLTY_RESOLUTION_FRC_MODE_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "P0_PHY_ABLTY_RESOLUTION_FORCE",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x776,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   P0_PHY_ABLTY_RESOLUTION_FORCE_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "P1_PHY_ABLTY_RESOLUTION_FORCE",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x778,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   P1_PHY_ABLTY_RESOLUTION_FORCE_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "P2_PHY_ABLTY_RESOLUTION_FORCE",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x77A,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   P2_PHY_ABLTY_RESOLUTION_FORCE_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "P3_PHY_ABLTY_RESOLUTION_FORCE",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x77C,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   P3_PHY_ABLTY_RESOLUTION_FORCE_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "POWCTRL_ADR_PHYMD",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x77E,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   POWCTRL_ADR_PHYMD_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "POWCTRL0_BIT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x780,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   POWCTRL0_BIT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RS_LAYER_CONFIG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x782,
        /* field numbers */     9,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RS_LAYER_CONFIG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY0_RD_PCS_ABILITY",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x784,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PHY0_RD_PCS_ABILITY_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY1_RD_PCS_ABILITY",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x786,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PHY1_RD_PCS_ABILITY_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY2_RD_PCS_ABILITY",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x788,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PHY2_RD_PCS_ABILITY_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY3_RD_PCS_ABILITY",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x78A,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PHY3_RD_PCS_ABILITY_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY4_RD_PCS_ABILITY",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x78C,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PHY4_RD_PCS_ABILITY_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_PHY_XG2G_G_MISC",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x78E,
        /* field numbers */     12,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_PHY_XG2G_G_MISC_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "P1_XG2XG_IPG_DBG_INFO",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x6BA,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   P1_XG2XG_IPG_DBG_INFO_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "P1_XG2XG_PRMB_DBG_INFO",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x6BC,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   P1_XG2XG_PRMB_DBG_INFO_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "P1_XG2XG_THR_DBG_INFO",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x6BE,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   P1_XG2XG_THR_DBG_INFO_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RANDOM_UPD_PERIOD",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x660,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RANDOM_UPD_PERIOD_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RANDOM_UPD_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x662,
        /* field numbers */     10,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RANDOM_UPD_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RG_RDM_SEED_2P5G_SRC_ADDR",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x664,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RG_RDM_SEED_2P5G_SRC_ADDR_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RG_RDM_SEED_1G_SRC_ADDR",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x666,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RG_RDM_SEED_1G_SRC_ADDR_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RING_RATE_REGADDR",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x668,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RING_RATE_REGADDR_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RING_RATE_SEL_MASK_L",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x66A,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RING_RATE_SEL_MASK_L_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RING_RATE_SEL_MASK_H",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x66C,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RING_RATE_SEL_MASK_H_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RING_RATE_FRC_VALUE_H",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x66E,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RING_RATE_FRC_VALUE_H_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RING_RATE_FRC_VALUE_L",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x670,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RING_RATE_FRC_VALUE_L_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "LFSR_INIT_SEED_FRC_VALUE",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x672,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   LFSR_INIT_SEED_FRC_VALUE_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RING_RATE_RD_VALUE_H",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x674,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RING_RATE_RD_VALUE_H_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RING_RATE_RD_VALUE_L",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x676,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RING_RATE_RD_VALUE_L_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "LFSR_INIT_SEED_RD_VALUE",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x678,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   LFSR_INIT_SEED_RD_VALUE_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "P0_G2XG_CFG_CLR_ERR_FLAG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x790,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   P0_G2XG_CFG_CLR_ERR_FLAG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "P1_G2XG_CFG_CLR_ERR_FLAG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x792,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   P1_G2XG_CFG_CLR_ERR_FLAG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "G2XG_FIFO_CLR_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x794,
        /* field numbers */     16,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   G2XG_FIFO_CLR_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "G2G_WATER_LEVEL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x6C0,
        /* field numbers */     5,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   G2G_WATER_LEVEL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "G2G_MISC_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x6C2,
        /* field numbers */     10,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   G2G_MISC_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "G2G_ERR_CNT_01",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x6C4,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   G2G_ERR_CNT_01_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "G2G_ERR_CNT_23",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x6C6,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   G2G_ERR_CNT_23_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "XG2XG_WATER_LEVEL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x6C8,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   XG2XG_WATER_LEVEL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "XG2XG_MISC_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x6CA,
        /* field numbers */     13,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   XG2XG_MISC_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "XG2XG_ERR_STATUS",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x6CC,
        /* field numbers */     8,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   XG2XG_ERR_STATUS_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_LPI_DLY_CYCLE",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x6CE,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_LPI_DLY_CYCLE_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PREAMBLE_RECOVERY_CRTL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x6D0,
        /* field numbers */     5,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PREAMBLE_RECOVERY_CRTL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "G2G_FIFO_CLR_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x6D2,
        /* field numbers */     16,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   G2G_FIFO_CLR_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "XG2XG_FIFO_CLR_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x6D4,
        /* field numbers */     12,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   XG2XG_FIFO_CLR_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PKTGEN_GLOBAL_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x5A0,
        /* field numbers */     6,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PKTGEN_GLOBAL_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PKTGEN_PAYLOAD_IND_ACCESS_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x5A2,
        /* field numbers */     6,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PKTGEN_PAYLOAD_IND_ACCESS_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PKTGEN_G2XG_FIFO_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x5A6,
        /* field numbers */     8,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PKTGEN_G2XG_FIFO_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PKTGEN0_CTRL0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x5AA,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PKTGEN0_CTRL0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PKTGEN0_CTRL1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x5AE,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PKTGEN0_CTRL1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PKTGEN0_CTRL2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x5B2,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PKTGEN0_CTRL2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PKTGEN0_CTRL3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x5B4,
        /* field numbers */     5,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PKTGEN0_CTRL3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PKTGEN0_CTRL4",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x5B8,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PKTGEN0_CTRL4_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PKTGEN0_CTRL5",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x5BC,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PKTGEN0_CTRL5_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PKTGEN0_CTRL6",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x5C0,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PKTGEN0_CTRL6_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PKTGEN0_CTRL7",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x5C4,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PKTGEN0_CTRL7_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PKTGEN1_CTRL0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x5C8,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PKTGEN1_CTRL0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PKTGEN1_CTRL1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x5CC,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PKTGEN1_CTRL1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PKTGEN1_CTRL2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x5D0,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PKTGEN1_CTRL2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PKTGEN1_CTRL3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x5D2,
        /* field numbers */     5,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PKTGEN1_CTRL3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PKTGEN1_CTRL4",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x5D6,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PKTGEN1_CTRL4_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PKTGEN1_CTRL5",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x5DA,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PKTGEN1_CTRL5_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PKTGEN1_CTRL6",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x5DE,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PKTGEN1_CTRL6_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PKTGEN1_CTRL7",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x5E2,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PKTGEN1_CTRL7_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PKTGEN2_CTRL0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x5E6,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PKTGEN2_CTRL0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PKTGEN2_CTRL1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x5EA,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PKTGEN2_CTRL1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PKTGEN2_CTRL2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x5EE,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PKTGEN2_CTRL2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PKTGEN2_CTRL3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x5F0,
        /* field numbers */     5,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PKTGEN2_CTRL3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PKTGEN2_CTRL4",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x5F4,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PKTGEN2_CTRL4_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PKTGEN2_CTRL5",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x5F8,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PKTGEN2_CTRL5_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PKTGEN2_CTRL6",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x5FC,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PKTGEN2_CTRL6_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PKTGEN2_CTRL7",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x600,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PKTGEN2_CTRL7_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PKTGEN3_CTRL0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x604,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PKTGEN3_CTRL0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PKTGEN3_CTRL1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x608,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PKTGEN3_CTRL1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PKTGEN3_CTRL2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x60C,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PKTGEN3_CTRL2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PKTGEN3_CTRL3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x60E,
        /* field numbers */     5,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PKTGEN3_CTRL3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PKTGEN3_CTRL4",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x612,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PKTGEN3_CTRL4_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PKTGEN3_CTRL5",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x616,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PKTGEN3_CTRL5_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PKTGEN3_CTRL6",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x61A,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PKTGEN3_CTRL6_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PKTGEN3_CTRL7",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x61E,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PKTGEN3_CTRL7_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PKTGEN0_CTRL8",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x622,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PKTGEN0_CTRL8_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PKTGEN1_CTRL8",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x624,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PKTGEN1_CTRL8_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PKTGEN2_CTRL8",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x626,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PKTGEN2_CTRL8_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PKTGEN3_CTRL8",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x628,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PKTGEN3_CTRL8_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PKTGEN0_CTRL15",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x62A,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PKTGEN0_CTRL15_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PKTGEN1_CTRL15",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x62E,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PKTGEN1_CTRL15_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PKTGEN2_CTRL15",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x632,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PKTGEN2_CTRL15_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PKTGEN3_CTRL15",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x636,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PKTGEN3_CTRL15_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY_MODE_SEL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x796,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PHY_MODE_SEL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY_FIFO_PREMB_RCVY_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x6D6,
        /* field numbers */     8,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PHY_FIFO_PREMB_RCVY_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FIBER_CFG_0_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1C0,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FIBER_CFG_0_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FIBER_CFG_1_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1C2,
        /* field numbers */     10,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FIBER_CFG_1_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FIBER_CFG_2_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1C4,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FIBER_CFG_2_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FIBER_CFG_3_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1C6,
        /* field numbers */     9,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FIBER_CFG_3_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FIBER_CFG_4_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1C8,
        /* field numbers */     11,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FIBER_CFG_4_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_MISC_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1CA,
        /* field numbers */     10,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SDS_MISC_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FIBER_CFG_1_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1CC,
        /* field numbers */     8,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FIBER_CFG_1_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FIBER_CFG_2_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1CE,
        /* field numbers */     11,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FIBER_CFG_2_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FIBER_CFG_3_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1D0,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FIBER_CFG_3_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FIBER_CFG_4_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1D2,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FIBER_CFG_4_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SERDES_RESULT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1D4,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SERDES_RESULT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_MISC",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1D6,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SDS_MISC_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_MISC_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1D8,
        /* field numbers */     6,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SDS_MISC_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_MISC_2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1DA,
        /* field numbers */     10,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SDS_MISC_2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MMDRDBUS_S0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1DC,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MMDRDBUS_S0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_MISC_3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1DE,
        /* field numbers */     13,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SDS_MISC_3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FIB0_REG0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x6200,
        /* field numbers */     12,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FIB0_REG0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FIB0_REG1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x6202,
        /* field numbers */     7,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FIB0_REG1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FIB0_REG2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x6204,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FIB0_REG2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FIB0_REG3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x6206,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FIB0_REG3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FIB0_REG4",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x6208,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FIB0_REG4_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FIB0_REG5",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x620A,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FIB0_REG5_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FIB0_REG6",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x620C,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FIB0_REG6_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FIB0_REG7",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x620E,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FIB0_REG7_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FIB0_REG8",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x6210,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FIB0_REG8_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FIB0_REG9",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x6212,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FIB0_REG9_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FIB0_REG10",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x6214,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FIB0_REG10_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FIB0_REG11",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x6216,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FIB0_REG11_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FIB0_REG12",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x6218,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FIB0_REG12_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FIB0_REG13",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x621A,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FIB0_REG13_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FIB0_REG14",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x621C,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FIB0_REG14_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FIB0_REG15",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x621E,
        /* field numbers */     5,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FIB0_REG15_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FIB1_REG0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x6220,
        /* field numbers */     12,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FIB1_REG0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FIB1_REG1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x6222,
        /* field numbers */     7,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FIB1_REG1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FIB1_REG2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x6224,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FIB1_REG2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FIB1_REG3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x6226,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FIB1_REG3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FIB1_REG4",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x6228,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FIB1_REG4_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FIB1_REG5",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x622A,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FIB1_REG5_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FIB1_REG6",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x622C,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FIB1_REG6_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FIB1_REG7",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x622E,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FIB1_REG7_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FIB1_REG8",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x6230,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FIB1_REG8_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FIB1_REG9",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x6232,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FIB1_REG9_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FIB1_REG10",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x6234,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FIB1_REG10_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FIB1_REG11",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x6236,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FIB1_REG11_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FIB1_REG12",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x6238,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FIB1_REG12_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FIB1_REG13",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x623A,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FIB1_REG13_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FIB1_REG14",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x623C,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FIB1_REG14_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FIB1_REG15",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x623E,
        /* field numbers */     5,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FIB1_REG15_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_INDACS_CMD",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x6600,
        /* field numbers */     6,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SDS_INDACS_CMD_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_INDACS_ADR",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x6602,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SDS_INDACS_ADR_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_INDACS_DATA",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x6604,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SDS_INDACS_DATA_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SYS_LED_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3A80,
        /* field numbers */     15,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SYS_LED_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SYS_LED_CFG2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3A82,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SYS_LED_CFG2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "LED_MODE",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3A84,
        /* field numbers */     10,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   LED_MODE_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DATA_LED_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3A86,
        /* field numbers */     6,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   DATA_LED_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RTCT_RESULTS_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3A88,
        /* field numbers */     16,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RTCT_RESULTS_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RTCT_LED_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3A8A,
        /* field numbers */     5,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RTCT_LED_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CPU_FORCE_LED_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3A8C,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CPU_FORCE_LED_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CPU_FORCE_LED0_CFG0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3A8E,
        /* field numbers */     8,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CPU_FORCE_LED0_CFG0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CPU_FORCE_LED1_CFG0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3A90,
        /* field numbers */     8,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CPU_FORCE_LED1_CFG0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CPU_FORCE_LED2_CFG0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3A92,
        /* field numbers */     8,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CPU_FORCE_LED2_CFG0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_RTCT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3A98,
        /* field numbers */     9,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_RTCT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_RTCT_PX_EN",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3A9A,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_RTCT_PX_EN_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RTCT_TIMEOUT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3A9C,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RTCT_TIMEOUT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SCAN0_LED_IO_EN",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3AA2,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SCAN0_LED_IO_EN_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "LPI_LED_OPTION1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3AA4,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   LPI_LED_OPTION1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "LPI_LED_OPTION2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3AA6,
        /* field numbers */     11,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   LPI_LED_OPTION2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "LPI_LED_OPTION3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3AA8,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   LPI_LED_OPTION3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "LED0_DATA_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3ABA,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   LED0_DATA_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "LED0_DATA_CTRL_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3ABC,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   LED0_DATA_CTRL_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "LED1_DATA_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3ABE,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   LED1_DATA_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "LED1_DATA_CTRL_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3AC0,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   LED1_DATA_CTRL_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "LED2_DATA_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3AC2,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   LED2_DATA_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "LED2_DATA_CTRL_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3AC4,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   LED2_DATA_CTRL_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SERIAL_LED_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3AC6,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SERIAL_LED_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RTCT_CHA_BASE_ADR",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3AC8,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RTCT_CHA_BASE_ADR_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RTCT_CHB_BASE_ADR",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3ACA,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RTCT_CHB_BASE_ADR_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RTCT_CHC_BASE_ADR",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3ACC,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RTCT_CHC_BASE_ADR_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RTCT_CHD_BASE_ADR",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3ACE,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RTCT_CHD_BASE_ADR_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RTCT_CHA_XG_BASE_ADR",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3AD0,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RTCT_CHA_XG_BASE_ADR_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RTCT_CHB_XG_BASE_ADR",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3AD2,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RTCT_CHB_XG_BASE_ADR_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RTCT_CHC_XG_BASE_ADR",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3AD4,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RTCT_CHC_XG_BASE_ADR_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RTCT_CHD_XG_BASE_ADR",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3AD6,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RTCT_CHD_XG_BASE_ADR_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY_MODE_LED_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3AD8,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PHY_MODE_LED_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "LED_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3ADA,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   LED_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RTCT_FORCE_REG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3ADC,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RTCT_FORCE_REG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "LED_ACTIVE_LOW_CFG0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3ADE,
        /* field numbers */     16,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   LED_ACTIVE_LOW_CFG0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "LED_ACTIVE_LOW_CFG1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3AE0,
        /* field numbers */     6,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   LED_ACTIVE_LOW_CFG1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "LED_IO_MAPPING0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3AE2,
        /* field numbers */     16,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   LED_IO_MAPPING0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "LED_IO_MAPPING1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3AE4,
        /* field numbers */     6,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   LED_IO_MAPPING1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "LED_IO_MAPPING2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3AE6,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   LED_IO_MAPPING2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "LED_IO_MAPPING3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3AE8,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   LED_IO_MAPPING3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "LED_IO_MAPPING4",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3AEA,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   LED_IO_MAPPING4_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "LED_IO_MAPPING5",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3AEC,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   LED_IO_MAPPING5_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "LED_IO_MAPPING6",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3AEE,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   LED_IO_MAPPING6_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "LED_IO_MAPPING7",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3AF0,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   LED_IO_MAPPING7_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "LED_IO_MAPPING8",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3AF2,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   LED_IO_MAPPING8_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SERIAL_LED_CTRL0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3AF4,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SERIAL_LED_CTRL0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SERIAL_LED_CTRL1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3AF6,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SERIAL_LED_CTRL1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_RTCT_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x25E0,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_RTCT_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "I2C_SCK_DIV",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1E0,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   I2C_SCK_DIV_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MDX_MDC_DIV",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1E2,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MDX_MDC_DIV_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "IIC_NOACK",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1E4,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   IIC_NOACK_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_GPIO_INPUT0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1E6,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_GPIO_INPUT0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_GPIO_INPUT1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1E8,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_GPIO_INPUT1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_GPIO_OUTPUT0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1EA,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_GPIO_OUTPUT0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_GPIO_OUTPUT1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1EC,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_GPIO_OUTPUT1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_GPIO_OUTPUT_EN0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1EE,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_GPIO_OUTPUT_EN0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_GPIO_OUTPUT_EN1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1F0,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_GPIO_OUTPUT_EN1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_GPIO_SOURCE0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1F2,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_GPIO_SOURCE0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_GPIO_SOURCE1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1F4,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_GPIO_SOURCE1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_1588_TIMER_EN_GPIO",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1F6,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_1588_TIMER_EN_GPIO_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEPROM_CONFIG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1F8,
        /* field numbers */     7,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEPROM_CONFIG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "INTRPT_POLARITY",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2720,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   INTRPT_POLARITY_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "IMR_RLFD",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2722,
        /* field numbers */     12,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   IMR_RLFD_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "IMS_RLFD",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2724,
        /* field numbers */     12,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   IMS_RLFD_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "IMR",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2726,
        /* field numbers */     16,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   IMR_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "IMS",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2728,
        /* field numbers */     16,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   IMS_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EVT_L2_LEARN_OVER",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x272A,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EVT_L2_LEARN_OVER_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EVT_SPEED_CHG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x272C,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EVT_SPEED_CHG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EVT_SPECIAL_CGST",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x272E,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EVT_SPECIAL_CGST_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EVT_PLINKDOWN",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2730,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EVT_PLINKDOWN_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EVT_PLINUP",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2732,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EVT_PLINUP_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EVT_SYS_LEARN_OVER",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2734,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EVT_SYS_LEARN_OVER_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EVT_SILENT_START",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2736,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EVT_SILENT_START_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "IMR_RLFD_8051",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2738,
        /* field numbers */     12,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   IMR_RLFD_8051_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "IMS_RLFD_8051",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x273A,
        /* field numbers */     12,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   IMS_RLFD_8051_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "IMR_8051",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x273C,
        /* field numbers */     16,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   IMR_8051_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "IMS_8051",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x273E,
        /* field numbers */     16,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   IMS_8051_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CMD_FR_8051",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2740,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CMD_FR_8051_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EVT_L2_LRN_OVER_8051",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2742,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EVT_L2_LRN_OVER_8051_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EVT_SPD_CHG_8051",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2744,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EVT_SPD_CHG_8051_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EVT_SPC_CGST_8051",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2746,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EVT_SPC_CGST_8051_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EVT_SPC_PLINKDOWN_8051",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2748,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EVT_SPC_PLINKDOWN_8051_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EVT_SPC_PLINUP_8051",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x274A,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EVT_SPC_PLINUP_8051_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EVT_SILENT_START_8051",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x274C,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EVT_SILENT_START_8051_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MIB_RMON_LEN_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x25E2,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MIB_RMON_LEN_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY_MIB_GLOBAL_CONFIG_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x7F0,
        /* field numbers */     6,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PHY_MIB_GLOBAL_CONFIG_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY_MIB_GLOBAL_CONFIG_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x7F2,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PHY_MIB_GLOBAL_CONFIG_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MIB_DATA_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4E0,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MIB_DATA_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MIB_DATA_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4E2,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MIB_DATA_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MIB_DATA_2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4E4,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MIB_DATA_2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MIB_DATA_3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4E6,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MIB_DATA_3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MIB_ADR",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4E8,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MIB_ADR_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MIB_CMD_REG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4EA,
        /* field numbers */     14,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MIB_CMD_REG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY0_RX_MIB_CNTR0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x810,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PHY0_RX_MIB_CNTR0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY0_RX_MIB_CNTR1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x814,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PHY0_RX_MIB_CNTR1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY0_RX_MIB_CNTR2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x818,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PHY0_RX_MIB_CNTR2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY0_RX_MIB_CNTR3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x81C,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PHY0_RX_MIB_CNTR3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY0_RX_MIB_CNTR4",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x820,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PHY0_RX_MIB_CNTR4_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY0_RX_MIB_CNTR5",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x824,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PHY0_RX_MIB_CNTR5_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY0_TX_MIB_CNTR0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x830,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PHY0_TX_MIB_CNTR0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY0_TX_MIB_CNTR1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x834,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PHY0_TX_MIB_CNTR1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY0_TX_MIB_CNTR2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x838,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PHY0_TX_MIB_CNTR2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY0_TX_MIB_CNTR3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x83C,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PHY0_TX_MIB_CNTR3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY0_TX_MIB_CNTR4",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x83E,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PHY0_TX_MIB_CNTR4_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY0_TX_MIB_CNTR5",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x842,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PHY0_TX_MIB_CNTR5_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY0_TX_MIB_CNTR6",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x844,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PHY0_TX_MIB_CNTR6_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY1_RX_MIB_CNTR0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x850,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PHY1_RX_MIB_CNTR0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY1_RX_MIB_CNTR1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x854,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PHY1_RX_MIB_CNTR1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY1_RX_MIB_CNTR2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x858,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PHY1_RX_MIB_CNTR2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY1_RX_MIB_CNTR3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x85C,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PHY1_RX_MIB_CNTR3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY1_RX_MIB_CNTR4",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x860,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PHY1_RX_MIB_CNTR4_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY1_RX_MIB_CNTR5",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x864,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PHY1_RX_MIB_CNTR5_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY1_TX_MIB_CNTR0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x870,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PHY1_TX_MIB_CNTR0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY1_TX_MIB_CNTR1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x874,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PHY1_TX_MIB_CNTR1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY1_TX_MIB_CNTR2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x878,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PHY1_TX_MIB_CNTR2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY1_TX_MIB_CNTR3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x87C,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PHY1_TX_MIB_CNTR3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY1_TX_MIB_CNTR4",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x87E,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PHY1_TX_MIB_CNTR4_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY1_TX_MIB_CNTR5",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x882,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PHY1_TX_MIB_CNTR5_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY1_TX_MIB_CNTR6",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x884,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PHY1_TX_MIB_CNTR6_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY2_RX_MIB_CNTR0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x890,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PHY2_RX_MIB_CNTR0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY2_RX_MIB_CNTR1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x894,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PHY2_RX_MIB_CNTR1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY2_RX_MIB_CNTR2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x898,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PHY2_RX_MIB_CNTR2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY2_RX_MIB_CNTR3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x89C,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PHY2_RX_MIB_CNTR3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY2_RX_MIB_CNTR4",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x8A0,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PHY2_RX_MIB_CNTR4_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY2_RX_MIB_CNTR5",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x8A4,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PHY2_RX_MIB_CNTR5_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY2_TX_MIB_CNTR0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x8B0,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PHY2_TX_MIB_CNTR0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY2_TX_MIB_CNTR1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x8B4,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PHY2_TX_MIB_CNTR1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY2_TX_MIB_CNTR2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x8B8,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PHY2_TX_MIB_CNTR2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY2_TX_MIB_CNTR3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x8BC,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PHY2_TX_MIB_CNTR3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY2_TX_MIB_CNTR4",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x8BE,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PHY2_TX_MIB_CNTR4_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY2_TX_MIB_CNTR5",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x8C2,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PHY2_TX_MIB_CNTR5_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY2_TX_MIB_CNTR6",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x8C4,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PHY2_TX_MIB_CNTR6_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY3_RX_MIB_CNTR0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x8D0,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PHY3_RX_MIB_CNTR0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY3_RX_MIB_CNTR1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x8D4,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PHY3_RX_MIB_CNTR1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY3_RX_MIB_CNTR2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x8D8,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PHY3_RX_MIB_CNTR2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY3_RX_MIB_CNTR3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x8DC,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PHY3_RX_MIB_CNTR3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY3_RX_MIB_CNTR4",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x8E0,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PHY3_RX_MIB_CNTR4_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY3_RX_MIB_CNTR5",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x8E4,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PHY3_RX_MIB_CNTR5_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY3_TX_MIB_CNTR0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x8F0,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PHY3_TX_MIB_CNTR0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY3_TX_MIB_CNTR1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x8F4,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PHY3_TX_MIB_CNTR1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY3_TX_MIB_CNTR2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x8F8,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PHY3_TX_MIB_CNTR2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY3_TX_MIB_CNTR3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x8FC,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PHY3_TX_MIB_CNTR3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY3_TX_MIB_CNTR4",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x8FE,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PHY3_TX_MIB_CNTR4_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY3_TX_MIB_CNTR5",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x902,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PHY3_TX_MIB_CNTR5_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY3_TX_MIB_CNTR6",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x904,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PHY3_TX_MIB_CNTR6_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_CH0_RX_MIB_CNTR0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x910,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   SDS_CH0_RX_MIB_CNTR0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_CH0_RX_MIB_CNTR1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x914,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   SDS_CH0_RX_MIB_CNTR1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_CH0_RX_MIB_CNTR2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x918,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   SDS_CH0_RX_MIB_CNTR2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_CH0_RX_MIB_CNTR3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x91C,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   SDS_CH0_RX_MIB_CNTR3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_CH0_RX_MIB_CNTR4",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x920,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   SDS_CH0_RX_MIB_CNTR4_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_CH0_RX_MIB_CNTR5",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x924,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SDS_CH0_RX_MIB_CNTR5_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_CH0_TX_MIB_CNTR0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x930,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   SDS_CH0_TX_MIB_CNTR0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_CH0_TX_MIB_CNTR1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x934,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   SDS_CH0_TX_MIB_CNTR1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_CH0_TX_MIB_CNTR2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x938,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   SDS_CH0_TX_MIB_CNTR2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_CH0_TX_MIB_CNTR3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x93C,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SDS_CH0_TX_MIB_CNTR3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_CH0_TX_MIB_CNTR4",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x93E,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   SDS_CH0_TX_MIB_CNTR4_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_CH0_TX_MIB_CNTR5",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x942,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SDS_CH0_TX_MIB_CNTR5_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_CH0_TX_MIB_CNTR6",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x944,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SDS_CH0_TX_MIB_CNTR6_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_CH1_RX_MIB_CNTR0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x950,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   SDS_CH1_RX_MIB_CNTR0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_CH1_RX_MIB_CNTR1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x954,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   SDS_CH1_RX_MIB_CNTR1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_CH1_RX_MIB_CNTR2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x958,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   SDS_CH1_RX_MIB_CNTR2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_CH1_RX_MIB_CNTR3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x95C,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   SDS_CH1_RX_MIB_CNTR3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_CH1_RX_MIB_CNTR4",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x960,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   SDS_CH1_RX_MIB_CNTR4_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_CH1_RX_MIB_CNTR5",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x964,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SDS_CH1_RX_MIB_CNTR5_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_CH1_TX_MIB_CNTR0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x970,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   SDS_CH1_TX_MIB_CNTR0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_CH1_TX_MIB_CNTR1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x974,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   SDS_CH1_TX_MIB_CNTR1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_CH1_TX_MIB_CNTR2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x978,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   SDS_CH1_TX_MIB_CNTR2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_CH1_TX_MIB_CNTR3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x97C,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SDS_CH1_TX_MIB_CNTR3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_CH1_TX_MIB_CNTR4",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x97E,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   SDS_CH1_TX_MIB_CNTR4_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_CH1_TX_MIB_CNTR5",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x982,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SDS_CH1_TX_MIB_CNTR5_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_CH1_TX_MIB_CNTR6",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x984,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SDS_CH1_TX_MIB_CNTR6_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_CH2_RX_MIB_CNTR0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x990,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   SDS_CH2_RX_MIB_CNTR0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_CH2_RX_MIB_CNTR1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x994,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   SDS_CH2_RX_MIB_CNTR1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_CH2_RX_MIB_CNTR2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x998,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   SDS_CH2_RX_MIB_CNTR2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_CH2_RX_MIB_CNTR3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x99C,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   SDS_CH2_RX_MIB_CNTR3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_CH2_RX_MIB_CNTR4",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x9A0,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   SDS_CH2_RX_MIB_CNTR4_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_CH2_RX_MIB_CNTR5",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x9A4,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SDS_CH2_RX_MIB_CNTR5_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_CH2_TX_MIB_CNTR0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x9B0,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   SDS_CH2_TX_MIB_CNTR0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_CH2_TX_MIB_CNTR1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x9B4,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   SDS_CH2_TX_MIB_CNTR1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_CH2_TX_MIB_CNTR2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x9B8,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   SDS_CH2_TX_MIB_CNTR2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_CH2_TX_MIB_CNTR3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x9BC,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SDS_CH2_TX_MIB_CNTR3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_CH2_TX_MIB_CNTR4",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x9BE,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   SDS_CH2_TX_MIB_CNTR4_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_CH2_TX_MIB_CNTR5",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x9C2,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SDS_CH2_TX_MIB_CNTR5_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_CH2_TX_MIB_CNTR6",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x9C4,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SDS_CH2_TX_MIB_CNTR6_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_CH3_RX_MIB_CNTR0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x9D0,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   SDS_CH3_RX_MIB_CNTR0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_CH3_RX_MIB_CNTR1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x9D4,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   SDS_CH3_RX_MIB_CNTR1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_CH3_RX_MIB_CNTR2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x9D8,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   SDS_CH3_RX_MIB_CNTR2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_CH3_RX_MIB_CNTR3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x9DC,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   SDS_CH3_RX_MIB_CNTR3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_CH3_RX_MIB_CNTR4",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x9E0,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   SDS_CH3_RX_MIB_CNTR4_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_CH3_RX_MIB_CNTR5",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x9E4,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SDS_CH3_RX_MIB_CNTR5_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_CH3_TX_MIB_CNTR0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x9F0,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   SDS_CH3_TX_MIB_CNTR0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_CH3_TX_MIB_CNTR1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x9F4,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   SDS_CH3_TX_MIB_CNTR1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_CH3_TX_MIB_CNTR2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x9F8,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   SDS_CH3_TX_MIB_CNTR2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_CH3_TX_MIB_CNTR3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x9FC,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SDS_CH3_TX_MIB_CNTR3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_CH3_TX_MIB_CNTR4",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x9FE,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   SDS_CH3_TX_MIB_CNTR4_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_CH3_TX_MIB_CNTR5",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0xA02,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SDS_CH3_TX_MIB_CNTR5_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SDS_CH3_TX_MIB_CNTR6",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0xA04,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SDS_CH3_TX_MIB_CNTR6_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MAC_TX_MISC",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1FA0,
        /* field numbers */     7,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   MAC_TX_MISC_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FORCE_RATA_H",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1FA2,
        /* field numbers */     1,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   FORCE_RATA_H_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FORCE_RATA_L",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1FA4,
        /* field numbers */     1,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   FORCE_RATA_L_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MAC_RX_MISC",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1FA6,
        /* field numbers */     3,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   MAC_RX_MISC_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_FRC_RATE",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1FA8,
        /* field numbers */     2,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   CFG_FRC_RATE_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CUR_RATE",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1FAC,
        /* field numbers */     2,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   CUR_RATE_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PKTMAXLEN",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1FB0,
        /* field numbers */     2,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   PKTMAXLEN_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MAC_MISC",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1FB2,
        /* field numbers */     4,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   MAC_MISC_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RG00",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x25E4,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RG00_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RG01",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x25E6,
        /* field numbers */     7,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RG01_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RG02",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x25E8,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RG02_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RG04",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x25EA,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RG04_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "INBW_LBOUND",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x25EC,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   INBW_LBOUND_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "INBW_HBOUND",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x25EE,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   INBW_HBOUND_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RG_48PASS1_DROP",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x25F0,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RG_48PASS1_DROP_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RG07",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x25F2,
        /* field numbers */     9,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RG07_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RG08",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x25F4,
        /* field numbers */     8,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RG08_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MAC_L2_ADDR_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x25F6,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        48,
        /* register fields */   MAC_L2_ADDR_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MAC_IPG_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x25FC,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MAC_IPG_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RG13",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x25FE,
        /* field numbers */     6,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RG13_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "INGRESSBW_BYPASS_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2600,
        /* field numbers */     6,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   INGRESSBW_BYPASS_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RG15",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2602,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RG15_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RG16",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2604,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RG16_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RG2B",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2606,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RG2B_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CHG_DUP_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2608,
        /* field numbers */     6,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CHG_DUP_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "BACK_PRESSURE_IPG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x260A,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   BACK_PRESSURE_IPG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "TX_ESD_LVL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x260C,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   TX_ESD_LVL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "WOL_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x260E,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   WOL_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "WOL_MAC",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2610,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        48,
        /* register fields */   WOL_MAC_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PAR_FIELD00",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2616,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 7,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   PAR_FIELD00_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "HWPKT_GEN_MISC",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2626,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   HWPKT_GEN_MISC_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FLOWCTRL_CTRL0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2628,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FLOWCTRL_CTRL0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "TX_IDLE_STYLE",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x262A,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   TX_IDLE_STYLE_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SW_DUMMYREG_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x262C,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SW_DUMMYREG_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SW_DUMMYREG_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x262E,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SW_DUMMYREG_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SW_DUMMYREG_2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2630,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SW_DUMMYREG_2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MAC_ACTIVE",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1FA,
        /* field numbers */     5,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MAC_ACTIVE_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "INDACC_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1F00,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   INDACC_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "INDACC_STATUS",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1F02,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   INDACC_STATUS_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "IND_ACC_ADDR",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1F04,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   IND_ACC_ADDR_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "IND_ACC_WDATA",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1F06,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   IND_ACC_WDATA_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "IND_ACC_RDATA",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1F08,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   IND_ACC_RDATA_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "GPHY_MISC_RST",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4D6,
        /* field numbers */     7,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   GPHY_MISC_RST_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FT_SCAN",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4D8,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FT_SCAN_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "INT_PHY_OCP_INDR_ACC_CTRL_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x798,
        /* field numbers */     7,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   INT_PHY_OCP_INDR_ACC_CTRL_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "INT_PHY_OCP_INDR_ACC_CTRL_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x79C,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   INT_PHY_OCP_INDR_ACC_CTRL_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "INT_PHY_OCP_INDR_ACC_CTRL_2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x79E,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   INT_PHY_OCP_INDR_ACC_CTRL_2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MAC_L2_TGPORT_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1F20,
        /* field numbers */     16,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MAC_L2_TGPORT_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MAC_L2_TGPORT_PRMB_DBG0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1F22,
        /* field numbers */     5,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MAC_L2_TGPORT_PRMB_DBG0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MAC_L2_TGPORT_PRMB_DBG1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1F24,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MAC_L2_TGPORT_PRMB_DBG1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MAC_L2_TGPORT_PRMB_DBG2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1F26,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MAC_L2_TGPORT_PRMB_DBG2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CUR_RATE_H_TG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1F28,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CUR_RATE_H_TG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CUR_RATE_L_TG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1F2A,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CUR_RATE_L_TG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MISC_TG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1F2C,
        /* field numbers */     5,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MISC_TG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PGMETER_H_TG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1F2E,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PGMETER_H_TG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PGMETER_L_TG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1F30,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PGMETER_L_TG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MAC_L2_TGPORT_STATUS",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1F32,
        /* field numbers */     7,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MAC_L2_TGPORT_STATUS_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MAC_L2_TGPORT_IPG_SHORT_CNT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1F34,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MAC_L2_TGPORT_IPG_SHORT_CNT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MAC_L2_TGPORT_DUMMY_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1F36,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MAC_L2_TGPORT_DUMMY_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MAC_L2_TGPORT_DUMMY_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1F38,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MAC_L2_TGPORT_DUMMY_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MAC_L2_TGPORT_TXCRC",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1F3A,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MAC_L2_TGPORT_TXCRC_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MAC_L2_TGPORT_INFO_FIFO_STS",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1F3C,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MAC_L2_TGPORT_INFO_FIFO_STS_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MAC_L2_TGPORT_TRX_STS",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1F3E,
        /* field numbers */     10,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MAC_L2_TGPORT_TRX_STS_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MAC_L2_TGPORT_CTRL_2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1F40,
        /* field numbers */     8,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MAC_L2_TGPORT_CTRL_2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CTRLCKT_DSC_OVFLAG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3F36,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CTRLCKT_DSC_OVFLAG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DSCNT_MISMATCH_STS_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1F42,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   DSCNT_MISMATCH_STS_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DSCNT_MISMATCH_CFG_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1F44,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   DSCNT_MISMATCH_CFG_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MAC_L2_TGPORT_TXPLA_STS",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1F46,
        /* field numbers */     5,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MAC_L2_TGPORT_TXPLA_STS_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MAC_L2_TGPORT_TX_SPDCHG_CNT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1F48,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MAC_L2_TGPORT_TX_SPDCHG_CNT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEECFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1FB4,
        /* field numbers */     12,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   EEECFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEETXMTR",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1FB6,
        /* field numbers */     1,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   EEETXMTR_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEERXMTR",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1FB8,
        /* field numbers */     1,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   EEERXMTR_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "TX_RATE_EEE_GIGA_500M",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2632,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   TX_RATE_EEE_GIGA_500M_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "TX_RATE_EEE_100M",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2634,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   TX_RATE_EEE_100M_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_MISC",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2636,
        /* field numbers */     8,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_MISC_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_GIGA0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2638,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_GIGA0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_GIGA1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x263A,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_GIGA1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_100M0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x263C,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_100M0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_100M1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x263E,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_100M1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_BURSTSIZE",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2640,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_BURSTSIZE_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_UPS_H",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1FC,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_UPS_H_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_GATING_CLOCK_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1FE,
        /* field numbers */     13,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_GATING_CLOCK_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "WAKELPI_SLOT_PRD",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x200,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   WAKELPI_SLOT_PRD_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "WAKELPI_SLOT_PG0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x202,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   WAKELPI_SLOT_PG0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "WAKELPI_SLOT_PG1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x204,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   WAKELPI_SLOT_PG1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "WAKELPI_SLOT_PG2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x206,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   WAKELPI_SLOT_PG2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "WAKELPI_SLOT_PG3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x208,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   WAKELPI_SLOT_PG3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DD_EEE_MISC",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x24A0,
        /* field numbers */     5,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   DD_EEE_MISC_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PORTMISC",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1FBA,
        /* field numbers */     4,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   PORTMISC_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "TXERRCNT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1FBC,
        /* field numbers */     2,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   TXERRCNT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_500M0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x24A2,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_500M0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RXIDLE_G",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x24A4,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RXIDLE_G_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RXIDLE_GLITE",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x24A6,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RXIDLE_GLITE_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_NEW_CTRL0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x24A8,
        /* field numbers */     5,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_NEW_CTRL0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_MINIPG_100M",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x24AA,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_MINIPG_100M_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_MINIPG_500M",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x24AC,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_MINIPG_500M_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_MINIPG_GIGA",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x24AE,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_MINIPG_GIGA_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_MINIPG_2P5G",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x24B0,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_MINIPG_2P5G_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_MINIPG_5G",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x24B2,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_MINIPG_5G_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_MINIPG_TG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x24B4,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_MINIPG_TG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_TD_CTRL_H",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x24B6,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_TD_CTRL_H_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P00_08",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2510,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P00_08_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P00_07",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2512,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P00_07_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P00_06",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2514,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P00_06_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P00_05",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2516,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P00_05_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P00_04",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2518,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P00_04_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P00_03",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x251A,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P00_03_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P00_02",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x251C,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P00_02_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P00_01",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x251E,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P00_01_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P00_00",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2520,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P00_00_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P01_08",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2522,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P01_08_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P01_07",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2524,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P01_07_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P01_06",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2526,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P01_06_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P01_05",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2528,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P01_05_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P01_04",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x252A,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P01_04_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P01_03",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x252C,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P01_03_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P01_02",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x252E,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P01_02_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P01_01",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2530,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P01_01_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P01_00",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2532,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P01_00_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P02_08",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2534,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P02_08_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P02_07",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2536,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P02_07_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P02_06",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2538,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P02_06_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P02_05",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x253A,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P02_05_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P02_04",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x253C,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P02_04_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P02_03",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x253E,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P02_03_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P02_02",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2540,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P02_02_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P02_01",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2542,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P02_01_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P02_00",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2544,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P02_00_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P03_08",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2546,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P03_08_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P03_07",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2548,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P03_07_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P03_06",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x254A,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P03_06_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P03_05",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x254C,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P03_05_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P03_04",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x254E,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P03_04_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P03_03",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2550,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P03_03_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P03_02",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2552,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P03_02_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P03_01",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2554,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P03_01_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P03_00",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2556,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P03_00_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P04_08",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2558,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P04_08_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P04_07",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x255A,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P04_07_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P04_06",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x255C,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P04_06_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P04_05",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x255E,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P04_05_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P04_04",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2560,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P04_04_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P04_03",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2562,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P04_03_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P04_02",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2564,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P04_02_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P04_01",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2566,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P04_01_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P04_00",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2568,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P04_00_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P05_08",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x256A,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P05_08_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P05_07",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x256C,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P05_07_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P05_06",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x256E,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P05_06_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P05_05",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2570,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P05_05_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P05_04",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2572,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P05_04_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P05_03",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2574,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P05_03_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P05_02",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2576,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P05_02_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P05_01",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2578,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P05_01_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P05_00",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x257A,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P05_00_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P06_08",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x257C,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P06_08_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P06_07",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x257E,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P06_07_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P06_06",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2580,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P06_06_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P06_05",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2582,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P06_05_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P06_04",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2584,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P06_04_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P06_03",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2586,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P06_03_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P06_02",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2588,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P06_02_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P06_01",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x258A,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P06_01_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P06_00",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x258C,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P06_00_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P07_08",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x258E,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P07_08_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P07_07",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2590,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P07_07_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P07_06",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2592,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P07_06_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P07_05",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2594,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P07_05_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P07_04",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2596,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P07_04_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P07_03",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2598,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P07_03_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P07_02",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x259A,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P07_02_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P07_01",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x259C,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P07_01_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_PKTL_P07_00",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x259E,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_PKTL_P07_00_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_RX_MASK",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x25A0,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_RX_MASK_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_REG0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x25A2,
        /* field numbers */     5,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_REG0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_TD_500M",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x24B8,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_TD_500M_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_TD_2P5G",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x24BA,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_TD_2P5G_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_TD_5G",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x24BC,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_TD_5G_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_TD_TG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x24BE,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_TD_TG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_TW_2P5G",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x24C0,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_TW_2P5G_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_TW_5G",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x24C2,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_TW_5G_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_TW_TG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x24C4,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_TW_TG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_TW_2P5GL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x24C6,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_TW_2P5GL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_LONGIDLE_100M",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x24C8,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_LONGIDLE_100M_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_LONGIDLE_500M",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x24CA,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_LONGIDLE_500M_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_LONGIDLE_GIGA",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x24CC,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_LONGIDLE_GIGA_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_LONGIDLE_2P5G",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x24CE,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_LONGIDLE_2P5G_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_LONGIDLE_2P5GL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x24D0,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_LONGIDLE_2P5GL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_LONGIDLE_5G",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x24D2,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_LONGIDLE_5G_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_LONGIDLE_TG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x24D4,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_LONGIDLE_TG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_DECISON_100M",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x24D6,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_DECISON_100M_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_DECISON_500M",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x24D8,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_DECISON_500M_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_DECISON_G",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x24DA,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_DECISON_G_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_DECISON_2P5G",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x24DC,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_DECISON_2P5G_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_DECISON_2P5GL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x24DE,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_DECISON_2P5GL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_DECISON_5G",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x24E0,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_DECISON_5G_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_DECISON_TG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x24E2,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_DECISON_TG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_SLOW_CLK_MASK",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x20A,
        /* field numbers */     9,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_SLOW_CLK_MASK_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_LONGIDLE_DECICION_SEL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x24E4,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_LONGIDLE_DECICION_SEL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_MULTI_WAKEUP_CONTROL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x24E6,
        /* field numbers */     6,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_MULTI_WAKEUP_CONTROL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PAUSE_WAKE_SEL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x24E8,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PAUSE_WAKE_SEL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_NIC_RXRD",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C36,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_NIC_RXRD_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_NIC_TXASR",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C38,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_NIC_TXASR_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NIC_RXCMD",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C3A,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NIC_RXCMD_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NIC_TXCMD",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C3C,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NIC_TXCMD_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NIC_IMS",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C3E,
        /* field numbers */     9,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NIC_IMS_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NIC_IMR",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C40,
        /* field numbers */     9,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NIC_IMR_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NIC_RXCR0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C42,
        /* field numbers */     8,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NIC_RXCR0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NIC_RXCR1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C44,
        /* field numbers */     6,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NIC_RXCR1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NIC_TXCR",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C46,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NIC_TXCR_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NIC_GCR",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C48,
        /* field numbers */     5,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NIC_GCR_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NIC_MHR0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C4A,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NIC_MHR0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NIC_MHR1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C4C,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NIC_MHR1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NIC_MHR2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C4E,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NIC_MHR2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NIC_MHR3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C50,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NIC_MHR3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NIC_MHR4",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C52,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NIC_MHR4_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NIC_MHR5",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C54,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NIC_MHR5_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NIC_MHR6",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C56,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NIC_MHR6_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NIC_MHR7",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C58,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NIC_MHR7_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NIC_PAHR0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C5A,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NIC_PAHR0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NIC_PAHR1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C5C,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NIC_PAHR1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NIC_PAHR2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C5E,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NIC_PAHR2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NIC_PAHR3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C60,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NIC_PAHR3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NIC_PAHR4",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C62,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NIC_PAHR4_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NIC_PAHR5",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C64,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NIC_PAHR5_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NIC_PAHR6",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C66,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NIC_PAHR6_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NIC_PAHR7",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C68,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NIC_PAHR7_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NIC_TXSTOPRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C6A,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NIC_TXSTOPRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NIC_TXSTOPRH",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C6C,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NIC_TXSTOPRH_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NIC_RXSTOPRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C6E,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NIC_RXSTOPRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NIC_RXSTOPRH",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C70,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NIC_RXSTOPRH_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NIC_RXFSTR",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C72,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NIC_RXFSTR_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NIC_RXMBTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C74,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NIC_RXMBTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NIC_RXMBTRH",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C76,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NIC_RXMBTRH_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NIC_RXMPTR",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C78,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NIC_RXMPTR_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NIC_T0TR",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C7A,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NIC_T0TR_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NIC_CRXCPR",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C7C,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NIC_CRXCPR_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NIC_CTXCPR",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C7E,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NIC_CTXCPR_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NIC_SRXCURPKTR",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C80,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NIC_SRXCURPKTR_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NIC_STXCURPKTR",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C82,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NIC_STXCURPKTR_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NIC_STXPKTLENR",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C84,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NIC_STXPKTLENR_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NIC_STXCURUNITR",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C86,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NIC_STXCURUNITR_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NIC_RX_DROP_MODE_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C88,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NIC_RX_DROP_MODE_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "NIC_URGENT_THD",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C8A,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   NIC_URGENT_THD_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ISA_WR_PI",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3C8C,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ISA_WR_PI_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CPU_TAG_AWARE_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2642,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CPU_TAG_AWARE_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CPU_TAG_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2644,
        /* field numbers */     9,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CPU_TAG_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FPGA_PUS",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2646,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FPGA_PUS_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_IFG_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2648,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_IFG_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EEE_MSK_DUP",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x264A,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EEE_MSK_DUP_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MIB_MISC1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x264C,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MIB_MISC1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACS_CMD",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x39E0,
        /* field numbers */     6,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACS_CMD_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACS_ADR",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x39E2,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACS_ADR_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACS_L2_ADR",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x39E4,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACS_L2_ADR_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_HSBA_LATCH",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x39E6,
        /* field numbers */     8,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_HSBA_LATCH_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACCESS_DATA_R00",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x39E8,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACCESS_DATA_R00_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACCESS_DATA_R01",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x39EA,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACCESS_DATA_R01_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACCESS_DATA_R02",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x39EC,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACCESS_DATA_R02_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACCESS_DATA_R03",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x39EE,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACCESS_DATA_R03_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACCESS_DATA_R04",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x39F0,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACCESS_DATA_R04_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACCESS_DATA_R05",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x39F2,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACCESS_DATA_R05_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACCESS_DATA_R06",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x39F4,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACCESS_DATA_R06_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACCESS_DATA_R07",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x39F6,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACCESS_DATA_R07_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACCESS_DATA_R08",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x39F8,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACCESS_DATA_R08_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACCESS_DATA_R09",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x39FA,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACCESS_DATA_R09_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACCESS_DATA_W00",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x39FC,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACCESS_DATA_W00_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACCESS_DATA_W01",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x39FE,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACCESS_DATA_W01_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACCESS_DATA_W02",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3A00,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACCESS_DATA_W02_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACCESS_DATA_W03",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3A02,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACCESS_DATA_W03_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACCESS_DATA_W04",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3A04,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACCESS_DATA_W04_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACCESS_DATA_W05",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3A06,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACCESS_DATA_W05_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACCESS_DATA_W06",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3A08,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACCESS_DATA_W06_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACCESS_DATA_W07",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3A0A,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACCESS_DATA_W07_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACCESS_DATA_W08",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3A0C,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACCESS_DATA_W08_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACCESS_DATA_W09",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3A0E,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACCESS_DATA_W09_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "TABLE_ACC_TIMEOUT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3A10,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   TABLE_ACC_TIMEOUT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACS_CMD_8051",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3A12,
        /* field numbers */     6,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACS_CMD_8051_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACS_ADR_8051",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3A14,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACS_ADR_8051_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACS_L2_ADR_8051",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3A16,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACS_L2_ADR_8051_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACCESS_DATA_8051_R00",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3A18,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACCESS_DATA_8051_R00_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACCESS_DATA_8051_R01",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3A1A,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACCESS_DATA_8051_R01_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACCESS_DATA_8051_R02",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3A1C,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACCESS_DATA_8051_R02_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACCESS_DATA_8051_R03",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3A1E,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACCESS_DATA_8051_R03_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACCESS_DATA_8051_R04",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3A20,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACCESS_DATA_8051_R04_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACCESS_DATA_8051_R05",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3A22,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACCESS_DATA_8051_R05_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACCESS_DATA_8051_R06",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3A24,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACCESS_DATA_8051_R06_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACCESS_DATA_8051_R07",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3A26,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACCESS_DATA_8051_R07_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACCESS_DATA_8051_R08",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3A28,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACCESS_DATA_8051_R08_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACCESS_DATA_8051_R09",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3A2A,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACCESS_DATA_8051_R09_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACCESS_DATA_8051_W00",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3A2C,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACCESS_DATA_8051_W00_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACCESS_DATA_8051_W01",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3A2E,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACCESS_DATA_8051_W01_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACCESS_DATA_8051_W02",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3A30,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACCESS_DATA_8051_W02_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACCESS_DATA_8051_W03",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3A32,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACCESS_DATA_8051_W03_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACCESS_DATA_8051_W04",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3A34,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACCESS_DATA_8051_W04_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACCESS_DATA_8051_W05",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3A36,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACCESS_DATA_8051_W05_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACCESS_DATA_8051_W06",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3A38,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACCESS_DATA_8051_W06_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACCESS_DATA_8051_W07",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3A3A,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACCESS_DATA_8051_W07_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACCESS_DATA_8051_W08",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3A3C,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACCESS_DATA_8051_W08_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACCESS_DATA_8051_W09",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3A3E,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACCESS_DATA_8051_W09_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DW8051_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4130,
        /* field numbers */     9,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   DW8051_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DW8051_PRO_RG0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4132,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   DW8051_PRO_RG0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DW8051_PRO_RG1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4134,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   DW8051_PRO_RG1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DW8051_PRO_RG2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4136,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   DW8051_PRO_RG2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DW8051_PRO_RG3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4138,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   DW8051_PRO_RG3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DW8051_PRO_RG4",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x413A,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   DW8051_PRO_RG4_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DW8051_PRO_RG5",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x413C,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   DW8051_PRO_RG5_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DW8051_PRO_RG6",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x413E,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   DW8051_PRO_RG6_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DW8051_PRO_RG7",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4140,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   DW8051_PRO_RG7_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "VLAN_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3080,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   VLAN_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CVLAN_INGRESS_FILTER",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3082,
        /* field numbers */     1,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        1,
        /* register fields */   CVLAN_INGRESS_FILTER_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "VLAN_PORT_EGR_TAG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1FBE,
        /* field numbers */     2,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   VLAN_PORT_EGR_TAG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "VLAN_PORT_PB_VLAN",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3084,
        /* field numbers */     2,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   VLAN_PORT_PB_VLAN_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "VLAN_PORT_AFT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3094,
        /* field numbers */     1,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        2,
        /* register fields */   VLAN_PORT_AFT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PORT_BASED_FID_EN",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3096,
        /* field numbers */     1,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        1,
        /* register fields */   PORT_BASED_FID_EN_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PORT_BASED_FID",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3098,
        /* field numbers */     2,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   PORT_BASED_FID_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "VLAN_TAG_PRI_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2AC0,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   VLAN_TAG_PRI_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "VLAN_RESET_TABLE",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x30A8,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   VLAN_RESET_TABLE_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "VLAN_PORT_EGR_KEEP",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2AC2,
        /* field numbers */     2,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   VLAN_PORT_EGR_KEEP_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "VLAN_PORT_EGR_TRANS",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3140,
        /* field numbers */     2,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   VLAN_PORT_EGR_TRANS_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "VS_UPLINK_PORT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x264E,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   VS_UPLINK_PORT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "VS_PORT_DFLT_SVID",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x37E0,
        /* field numbers */     2,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   VS_PORT_DFLT_SVID_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "VS_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x37F0,
        /* field numbers */     5,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   VS_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "VS_UN_IDX",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x37F2,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   VS_UN_IDX_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "VS_SP2C",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2AD2,
        /* field numbers */     6,
        /* port index */        0, 0,
        /* array index */       0, 63,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   VS_SP2C_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "VLAN_C2S_ENTRY",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x37F4,
        /* field numbers */     6,
        /* port index */        0, 0,
        /* array index */       0, 31,
        /* portlist index */    0,
        /* bit offset */        48,
        /* register fields */   VLAN_C2S_ENTRY_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RMA_ETY00",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3150,
        /* field numbers */     7,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RMA_ETY00_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RMA_ETY01",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3152,
        /* field numbers */     7,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RMA_ETY01_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RMA_ETY02",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3154,
        /* field numbers */     7,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RMA_ETY02_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RMA_ETY03",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3156,
        /* field numbers */     7,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RMA_ETY03_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RMA_ETY04",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3158,
        /* field numbers */     7,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RMA_ETY04_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RMA_ETY08",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x315A,
        /* field numbers */     7,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RMA_ETY08_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RMA_ETY0D",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x315C,
        /* field numbers */     7,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RMA_ETY0D_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RMA_ETY0E",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x315E,
        /* field numbers */     7,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RMA_ETY0E_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RMA_ETY10",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3160,
        /* field numbers */     7,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RMA_ETY10_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RMA_ETY11",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3162,
        /* field numbers */     7,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RMA_ETY11_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RMA_ETY12",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3164,
        /* field numbers */     7,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RMA_ETY12_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RMA_ETY13",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3166,
        /* field numbers */     7,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RMA_ETY13_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RMA_ETY18",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3168,
        /* field numbers */     7,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RMA_ETY18_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RMA_ETY1A",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x316A,
        /* field numbers */     7,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RMA_ETY1A_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RMA_ETY20",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x316C,
        /* field numbers */     7,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RMA_ETY20_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RMA_ETY21",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x316E,
        /* field numbers */     7,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RMA_ETY21_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RMA_ETY22",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3170,
        /* field numbers */     7,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RMA_ETY22_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RMA_ETY30",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3172,
        /* field numbers */     7,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RMA_ETY30_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RMA_ETY31",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3174,
        /* field numbers */     7,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RMA_ETY31_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RMA_ETY32",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3176,
        /* field numbers */     7,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RMA_ETY32_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RMA_LLDP_EN",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3178,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RMA_LLDP_EN_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RMA_PORT_PTP_ETH2_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x317A,
        /* field numbers */     1,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        2,
        /* register fields */   RMA_PORT_PTP_ETH2_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RMA_PORT_PTP_UDP_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x317C,
        /* field numbers */     1,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        2,
        /* register fields */   RMA_PORT_PTP_UDP_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RMA_PORT_PTP_DELAY_CARE_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x317E,
        /* field numbers */     1,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        1,
        /* register fields */   RMA_PORT_PTP_DELAY_CARE_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RMA_PORT_PTP_PDELAY_CARE_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3180,
        /* field numbers */     1,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        1,
        /* register fields */   RMA_PORT_PTP_PDELAY_CARE_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RMA_PORT_PTP_ASM_CARE_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3182,
        /* field numbers */     1,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        1,
        /* register fields */   RMA_PORT_PTP_ASM_CARE_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RMA_PTP_TRAP_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3184,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RMA_PTP_TRAP_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EPMSK_EMPTY",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2C40,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   EPMSK_EMPTY_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "TRUNK_DROP_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3186,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   TRUNK_DROP_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "TRK_HASH_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3188,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 1,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   TRK_HASH_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "TRK_MBR_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x318C,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 1,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   TRK_MBR_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "TRK_FC",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3190,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   TRK_FC_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "TRK_HASH",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3192,
        /* field numbers */     20,
        /* port index */        0, 0,
        /* array index */       0, 1,
        /* portlist index */    0,
        /* bit offset */        64,
        /* register fields */   TRK_HASH_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "TRK_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x31A2,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 1,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   TRK_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MSTP_STATE",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x35E0,
        /* field numbers */     8,
        /* port index */        0, 0,
        /* array index */       0, 7,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   MSTP_STATE_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "UNKN_UC_FLD_PMSK",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x31A6,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   UNKN_UC_FLD_PMSK_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "UNKN_MC_FLD_PMSK",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x31A8,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   UNKN_MC_FLD_PMSK_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "UNKN_BC_FLD_PMSK",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x31AA,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   UNKN_BC_FLD_PMSK_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "L2_NEWSA_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x31AC,
        /* field numbers */     1,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        2,
        /* register fields */   L2_NEWSA_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "L2_UNMATCH_SA_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x31AE,
        /* field numbers */     1,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        2,
        /* register fields */   L2_UNMATCH_SA_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "L2_UNKNOWN_DA_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x31B0,
        /* field numbers */     1,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        2,
        /* register fields */   L2_UNKNOWN_DA_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SOURCE_PORT_PERMIT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x31B2,
        /* field numbers */     1,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        1,
        /* register fields */   SOURCE_PORT_PERMIT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "IPMUL_NO_VLAN_EGRESS",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x31B4,
        /* field numbers */     1,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        1,
        /* register fields */   IPMUL_NO_VLAN_EGRESS_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "IPMUL_NO_PORTISO",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x31B6,
        /* field numbers */     1,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        1,
        /* register fields */   IPMUL_NO_PORTISO_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "L2_LRN_PORT_CONSTRT_ACT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x31B8,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   L2_LRN_PORT_CONSTRT_ACT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "L2_PORT_MC_LM_ACT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x31BA,
        /* field numbers */     1,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        2,
        /* register fields */   L2_PORT_MC_LM_ACT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "IPV4_PORT_MC_LM_ACT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x31BC,
        /* field numbers */     1,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        2,
        /* register fields */   IPV4_PORT_MC_LM_ACT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "IPV6_PORT_MC_LM_ACT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x31BE,
        /* field numbers */     1,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        2,
        /* register fields */   IPV6_PORT_MC_LM_ACT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "L2_FORCE_MODE",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x31C0,
        /* field numbers */     1,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        1,
        /* register fields */   L2_FORCE_MODE_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "L2_FORCE_DPM_PORT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x31C2,
        /* field numbers */     2,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   L2_FORCE_DPM_PORT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "IPMC_GROUP_ETY",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x31D2,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 63,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   IPMC_GROUP_ETY_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "IPMC_GROUP_PMSK",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x35F0,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 63,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   IPMC_GROUP_PMSK_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "IPMC_GROUP_VALID",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x38B4,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 63,
        /* portlist index */    0,
        /* bit offset */        1,
        /* register fields */   IPMC_GROUP_VALID_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "OD_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x32D2,
        /* field numbers */     6,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   OD_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_LOOKUP_HIT_ISO_ACT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3670,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_LOOKUP_HIT_ISO_ACT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "L2_SRAM_DEBUG_MDOE",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3672,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   L2_SRAM_DEBUG_MDOE_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "L2_LRN_PORT_LMT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3674,
        /* field numbers */     2,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   L2_LRN_PORT_LMT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "L2_LRN_PORT_CONSTRT_CNT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3684,
        /* field numbers */     2,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   L2_LRN_PORT_CONSTRT_CNT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "LRN_SYS_LMT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3694,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   LRN_SYS_LMT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "LRN_SYS_LMT_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3696,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   LRN_SYS_LMT_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "LUT_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3698,
        /* field numbers */     6,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   LUT_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DIS_AGE",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x369A,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   DIS_AGE_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "AGE_TIMER_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x369C,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   AGE_TIMER_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "L2_FLUSH_CMD",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x369E,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   L2_FLUSH_CMD_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "L2_FLUSH_XID",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x36A0,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   L2_FLUSH_XID_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "L2_FLUSH_MODE",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x36A2,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   L2_FLUSH_MODE_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "L2_FLUSH_ALL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x36A4,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   L2_FLUSH_ALL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "L2_FLUSH_STATUS",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x36A6,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   L2_FLUSH_STATUS_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "LRN_CNT_UNDER_STS",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x36A8,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   LRN_CNT_UNDER_STS_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SA_MOVE_PMASK",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x36AA,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SA_MOVE_PMASK_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "IGMP_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x35A0,
        /* field numbers */     8,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   IGMP_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "IGMP_PORT_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x35A2,
        /* field numbers */     6,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   IGMP_PORT_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PORT_ISO_PORT_PMSK",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x32D4,
        /* field numbers */     2,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   PORT_ISO_PORT_PMSK_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MIRROR_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x32E4,
        /* field numbers */     9,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MIRROR_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MIRROR_SET_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2650,
        /* field numbers */     5,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MIRROR_SET_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MIRROR_SRC_PMASK",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x32E6,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MIRROR_SRC_PMASK_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MIRROR_SAMPLE_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x32E8,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MIRROR_SAMPLE_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MIRROR_SAMPLE_CNT0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x32EA,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MIRROR_SAMPLE_CNT0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MIRROR_SAMPLE_CNT1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x32EC,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MIRROR_SAMPLE_CNT1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACL_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2E80,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACL_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACL_PORT_EN",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2E82,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACL_PORT_EN_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACL_PORT_UNMATCH_PERMIT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2E84,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACL_PORT_UNMATCH_PERMIT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACL_TEMPLATE_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2E86,
        /* field numbers */     16,
        /* port index */        0, 0,
        /* array index */       0, 4,
        /* portlist index */    0,
        /* bit offset */        64,
        /* register fields */   ACL_TEMPLATE_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACL_ACT_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2EAE,
        /* field numbers */     8,
        /* port index */        0, 0,
        /* array index */       0, 63,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   ACL_ACT_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACL_HIT_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2F2E,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACL_HIT_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACL_HIT_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2F30,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACL_HIT_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACL_HIT_2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2F32,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACL_HIT_2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACL_GPIO_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x20C,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACL_GPIO_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACL_GPIO_CFG2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x20E,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACL_GPIO_CFG2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "ACL_GPIO_POLAR",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x210,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   ACL_GPIO_POLAR_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RNG_CHK_PORT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2F34,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 7,
        /* portlist index */    0,
        /* bit offset */        48,
        /* register fields */   RNG_CHK_PORT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RNG_CHK_VID",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2F64,
        /* field numbers */     5,
        /* port index */        0, 0,
        /* array index */       0, 7,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   RNG_CHK_VID_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RNG_CHK_IP",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2F84,
        /* field numbers */     6,
        /* port index */        0, 0,
        /* array index */       0, 7,
        /* portlist index */    0,
        /* bit offset */        80,
        /* register fields */   RNG_CHK_IP_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MIB_RESET_CMD",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4EC,
        /* field numbers */     16,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MIB_RESET_CMD_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MIB_ACL_CNTSEL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4EE,
        /* field numbers */     9,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MIB_ACL_CNTSEL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MIB_TIMER_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4F0,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MIB_TIMER_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MIB_ACL_TYPESEL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4F2,
        /* field numbers */     9,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MIB_ACL_TYPESEL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MIB_DUMMY0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4F4,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MIB_DUMMY0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MIB_DUMMY1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4F6,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MIB_DUMMY1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_TIME_TOD_DELAY",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3D30,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_TIME_TOD_DELAY_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_TIME_OP_CHECK",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3D32,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_TIME_OP_CHECK_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_OTAG_CONFIG0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3D34,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_OTAG_CONFIG0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_DUMMY_RG03",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3D36,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_DUMMY_RG03_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_DUMMY_RG04",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3D38,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_DUMMY_RG04_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_DUMMY_RG05",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3D3A,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_DUMMY_RG05_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_ITAG_CONFIG0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3D3C,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_ITAG_CONFIG0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_DUMMY_RG07",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3D3E,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_DUMMY_RG07_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_DUMMY_RG08",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3D40,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_DUMMY_RG08_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_DUMMY_RG09",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3D42,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_DUMMY_RG09_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_DUMMY_RG10",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3D44,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_DUMMY_RG10_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_APPLY_FREQ",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3D46,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_APPLY_FREQ_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_TIME_FREQ0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3D48,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_TIME_FREQ0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_TIME_FREQ1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3D4A,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_TIME_FREQ1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_CUR_TIME_FREQ0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3D4C,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_CUR_TIME_FREQ0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_CUR_TIME_FREQ1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3D4E,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_CUR_TIME_FREQ1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_TIME_NSEC0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3D50,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_TIME_NSEC0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_TIME_NSEC1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3D52,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_TIME_NSEC1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_TIME_SEC0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3D54,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_TIME_SEC0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_TIME_SEC1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3D56,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_TIME_SEC1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_TIME_SEC2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3D58,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_TIME_SEC2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_TIME_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3D5A,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_TIME_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_TIME_NSEC_RD0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3D5C,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_TIME_NSEC_RD0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_TIME_NSEC_RD1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3D5E,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_TIME_NSEC_RD1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_TIME_SEC_RD0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3D60,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_TIME_SEC_RD0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_TIME_SEC_RD1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3D62,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_TIME_SEC_RD1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_TIME_SEC_RD2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3D64,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_TIME_SEC_RD2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_CLKOUT_NSEC0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3D66,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_CLKOUT_NSEC0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_CLKOUT_NSEC1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3D68,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_CLKOUT_NSEC1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_CLKOUT_SEC0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3D6A,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_CLKOUT_SEC0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_CLKOUT_SEC1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3D6C,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_CLKOUT_SEC1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_CLKOUT_SEC2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3D6E,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_CLKOUT_SEC2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_CLKOUT_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3D70,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_CLKOUT_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_CLKOUT_HALF_PERD_NS_L",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3D72,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_CLKOUT_HALF_PERD_NS_L_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_CLKOUT_HALF_PERD_NS_H",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3D74,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_CLKOUT_HALF_PERD_NS_H_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_TIME_OP_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3D76,
        /* field numbers */     5,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_TIME_OP_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_PPS_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3D78,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_PPS_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_TX_TIMESTAMP_RD0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3D7A,
        /* field numbers */     5,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_TX_TIMESTAMP_RD0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_TX_TIMESTAMP_RD1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3D7C,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_TX_TIMESTAMP_RD1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_TX_TIMESTAMP_RD2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3D7E,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_TX_TIMESTAMP_RD2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_TX_TIMESTAMP_RD3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3D80,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_TX_TIMESTAMP_RD3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_MIB_INTR",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3D82,
        /* field numbers */     7,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_MIB_INTR_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_GLOBAL_DBG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3D84,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_GLOBAL_DBG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_CLK_SRC_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3D86,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_CLK_SRC_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_CLKOUT_HALF_PERD_FS_L",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3D88,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_CLKOUT_HALF_PERD_FS_L_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_CLKOUT_HALF_PERD_FS_H",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3D8A,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_CLKOUT_HALF_PERD_FS_H_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_DUMMY_RG46",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3D8C,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_DUMMY_RG46_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_DUMMY_RG47",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3D8E,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_DUMMY_RG47_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PPS_IN_LATCH_TIME_NSEC_L",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3D90,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PPS_IN_LATCH_TIME_NSEC_L_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PPS_IN_LATCH_TIME_NSEC_H",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3D92,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PPS_IN_LATCH_TIME_NSEC_H_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PPS_IN_LATCH_TIME_SEC_L",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3D94,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PPS_IN_LATCH_TIME_SEC_L_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PPS_IN_LATCH_TIME_SEC_M",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3D96,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PPS_IN_LATCH_TIME_SEC_M_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PPS_IN_LATCH_TIME_SEC_H",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3D98,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PPS_IN_LATCH_TIME_SEC_H_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_DEBUG_TOP_STS_H",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3D9A,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_DEBUG_TOP_STS_H_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_DEBUG_TOP_STS_L",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3D9C,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_DEBUG_TOP_STS_L_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_TIMESTAMP_FIFO_STS",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3D9E,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_TIMESTAMP_FIFO_STS_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_DUMMY_STS1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3DA0,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_DUMMY_STS1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_DUMMY_STS2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3DA2,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_DUMMY_STS2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_DUMMY_RG58",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3DA4,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_DUMMY_RG58_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_DUMMY_RG59",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3DA6,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_DUMMY_RG59_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_DUMMY_RG60",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3DA8,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_DUMMY_RG60_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_DUMMY_RG61",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3DAA,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_DUMMY_RG61_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_TIME_SPEED_UP",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3DAC,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_TIME_SPEED_UP_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_VERSION",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3DAE,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PTP_VERSION_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_PORT_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3DB0,
        /* field numbers */     6,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   PTP_PORT_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTPLINK_DELAY_H",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3DB2,
        /* field numbers */     1,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   PTPLINK_DELAY_H_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_MISC_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3DB4,
        /* field numbers */     6,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   PTP_MISC_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_TX_IMBAL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3DB6,
        /* field numbers */     2,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   PTP_TX_IMBAL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_RX_IMBAL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3DB8,
        /* field numbers */     2,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   PTP_RX_IMBAL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PTP_SPD_ST",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3DBA,
        /* field numbers */     2,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   PTP_SPD_ST_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DBG_PTP_CTRL_TX",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3DBC,
        /* field numbers */     1,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   DBG_PTP_CTRL_TX_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DBG_PTP_CTRL_RX",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3DBE,
        /* field numbers */     1,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   DBG_PTP_CTRL_RX_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "TOD_OUT_DATA_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3E50,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 15,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   TOD_OUT_DATA_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "TOD_OUT_CTRL0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3E70,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   TOD_OUT_CTRL0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "TOD_OUT_CTRL1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3E72,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   TOD_OUT_CTRL1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "TOD_SARP_GPS_WEEK",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3E74,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   TOD_SARP_GPS_WEEK_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "TOD_SARP_GPS_SEC_L",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3E76,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   TOD_SARP_GPS_SEC_L_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "TOD_SARP_GPS_SEC_H",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3E78,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   TOD_SARP_GPS_SEC_H_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "TOD_UART_SETTING",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3E7A,
        /* field numbers */     9,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   TOD_UART_SETTING_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "TOD_INTR",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3E7C,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   TOD_INTR_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "TOD_DBG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3E7E,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   TOD_DBG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RX_STORM_BCAST_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x36AC,
        /* field numbers */     1,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        1,
        /* register fields */   RX_STORM_BCAST_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RX_STORM_MCAST_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x36AE,
        /* field numbers */     1,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        1,
        /* register fields */   RX_STORM_MCAST_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RX_STORM_UNUCAST_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x36B0,
        /* field numbers */     1,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        1,
        /* register fields */   RX_STORM_UNUCAST_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RX_STORM_UNMCAST_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x36B2,
        /* field numbers */     1,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        1,
        /* register fields */   RX_STORM_UNMCAST_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RX_STORM_BCAST_METER",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x36B4,
        /* field numbers */     2,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   RX_STORM_BCAST_METER_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RX_STORM_MCAST_METER",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x36C4,
        /* field numbers */     2,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   RX_STORM_MCAST_METER_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RX_STORM_UNUCAST_METER",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x36D4,
        /* field numbers */     2,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   RX_STORM_UNUCAST_METER_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RX_STORM_UNMCAST_METER",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x36E4,
        /* field numbers */     2,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   RX_STORM_UNMCAST_METER_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_STORM_EXT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x36F4,
        /* field numbers */     6,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_STORM_EXT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "STORM_EXT_MTRIDX_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x36F6,
        /* field numbers */     8,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   STORM_EXT_MTRIDX_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "IGBW_PORT_FC_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1FC0,
        /* field numbers */     3,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   IGBW_PORT_FC_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "IGBW_PORT_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1FC2,
        /* field numbers */     3,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   IGBW_PORT_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "EGRESS_RATE",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2D00,
        /* field numbers */     3,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   EGRESS_RATE_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "APR_METER",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2D20,
        /* field numbers */     10,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   APR_METER_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "BYPASS_LINE_RATE",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2D40,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   BYPASS_LINE_RATE_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "LINE_RATE_1G",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2D42,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   LINE_RATE_1G_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "LINE_RATE_100",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2D46,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   LINE_RATE_100_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "LINE_RATE_10",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2D4A,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   LINE_RATE_10_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "LINE_RATE_500",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2D4E,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   LINE_RATE_500_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "LINE_RATE_HSG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2D52,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   LINE_RATE_HSG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "LINE_RATE_10G",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2D56,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   LINE_RATE_10G_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "P_EN_TX_MISC",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2D5A,
        /* field numbers */     9,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   P_EN_TX_MISC_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SCH_WRR_OPT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2D5C,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SCH_WRR_OPT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SCH_DUMMY0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2D5E,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   SCH_DUMMY0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MTR_PKT_MODE_TOKEN",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x28C0,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MTR_PKT_MODE_TOKEN_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_METER_RATE",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x28C2,
        /* field numbers */     6,
        /* port index */        0, 0,
        /* array index */       0, 39,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   CFG_METER_RATE_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_METER_BUCKET",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2962,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 39,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   CFG_METER_BUCKET_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_MTR_CTRL0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x29B2,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_MTR_CTRL0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_MTR_CTRL1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x29B4,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_MTR_CTRL1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MTR_OVER_RG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x29B6,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 39,
        /* portlist index */    0,
        /* bit offset */        1,
        /* register fields */   MTR_OVER_RG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MTR_8051_OVER_RG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x29BC,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 39,
        /* portlist index */    0,
        /* bit offset */        1,
        /* register fields */   MTR_8051_OVER_RG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MTR_MISC",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x29C2,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MTR_MISC_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "INQ_OVER",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2BD2,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   INQ_OVER_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PKTCNT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1FC6,
        /* field numbers */     2,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   PKTCNT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PGMETER",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1FC8,
        /* field numbers */     2,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   PGMETER_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PKT_TXCOM",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1FCC,
        /* field numbers */     3,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   PKT_TXCOM_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_GAP_EP",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2C42,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_GAP_EP_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_GAP_OQ",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2C44,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_GAP_OQ_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFGTH_QDRP",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2C46,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 7,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   CFGTH_QDRP_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFGTH_PDRP",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2C56,
        /* field numbers */     2,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   CFGTH_PDRP_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PRT_SEL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2C66,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PRT_SEL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "OUTQ_CLR_CNT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2C68,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   OUTQ_CLR_CNT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "OQ0_CUR_PGECNT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2C6A,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   OQ0_CUR_PGECNT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "OQ1_CUR_PGECNT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2C6C,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   OQ1_CUR_PGECNT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "OQ2_CUR_PGECNT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2C6E,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   OQ2_CUR_PGECNT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "OQ3_CUR_PGECNT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2C70,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   OQ3_CUR_PGECNT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "OQ4_CUR_PGECNT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2C72,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   OQ4_CUR_PGECNT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "OQ5_CUR_PGECNT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2C74,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   OQ5_CUR_PGECNT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "OQ6_CUR_PGECNT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2C76,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   OQ6_CUR_PGECNT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "OQ7_CUR_PGECNT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2C78,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   OQ7_CUR_PGECNT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PRT_CUR_PGECNT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2C7A,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PRT_CUR_PGECNT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "OQ0_MAX_PGECNT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2C7C,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   OQ0_MAX_PGECNT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "OQ1_MAX_PGECNT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2C7E,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   OQ1_MAX_PGECNT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "OQ2_MAX_PGECNT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2C80,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   OQ2_MAX_PGECNT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "OQ3_MAX_PGECNT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2C82,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   OQ3_MAX_PGECNT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "OQ4_MAX_PGECNT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2C84,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   OQ4_MAX_PGECNT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "OQ5_MAX_PGECNT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2C86,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   OQ5_MAX_PGECNT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "OQ6_MAX_PGECNT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2C88,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   OQ6_MAX_PGECNT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "OQ7_MAX_PGECNT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2C8A,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   OQ7_MAX_PGECNT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PRT_MAX_PGECNT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2C8C,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PRT_MAX_PGECNT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "TOTAL_PKT_CNT_REG2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2C8E,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   TOTAL_PKT_CNT_REG2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "HQ_MSK_PG0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2C90,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   HQ_MSK_PG0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "HQ_MSK_PG1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2C92,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   HQ_MSK_PG1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "HQ_MSK_PG2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2C94,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   HQ_MSK_PG2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "HQ_MSK_PG3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2C96,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   HQ_MSK_PG3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "TH_LQOVER",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2C98,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   TH_LQOVER_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "TH_TX_PREFET",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2C9A,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   TH_TX_PREFET_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "Q_TXPKT_CNT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2C9C,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 7,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   Q_TXPKT_CNT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "Q_TXPKT_CNT_CTL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2CBC,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   Q_TXPKT_CNT_CTL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FC_PORT_Q_EGR_FC",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x32EE,
        /* field numbers */     1,
        /* port index */        0, 7,
        /* array index */       0, 7,
        /* portlist index */    0,
        /* bit offset */        1,
        /* register fields */   FC_PORT_Q_EGR_FC_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FC_GLB_DROP_ALL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2652,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FC_GLB_DROP_ALL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FC_GLB_PAUSE_ALL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2654,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FC_GLB_PAUSE_ALL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FC_GLB_HI_ON",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2656,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FC_GLB_HI_ON_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FC_GLB_HI_OFF",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2658,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FC_GLB_HI_OFF_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FC_GLB_LOW_ON",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x265A,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FC_GLB_LOW_ON_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FC_GLB_LOW_OFF",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x265C,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FC_GLB_LOW_OFF_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FC_GLB_FCOFF_HI_ON",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x265E,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FC_GLB_FCOFF_HI_ON_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FC_GLB_FCOFF_HI_OFF",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2660,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FC_GLB_FCOFF_HI_OFF_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FC_GLB_FCOFF_LOW_ON",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2662,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FC_GLB_FCOFF_LOW_ON_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FC_GLB_FCOFF_LOW_OFF",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2664,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FC_GLB_FCOFF_LOW_OFF_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FC_PORT_HI_ON",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2666,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FC_PORT_HI_ON_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FC_PORT_HI_OFF",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2668,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FC_PORT_HI_OFF_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FC_PORT_LOW_ON",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x266A,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FC_PORT_LOW_ON_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FC_PORT_LOW_OFF",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x266C,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FC_PORT_LOW_OFF_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FC_PORT_FCOFF_HI_ON",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x266E,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FC_PORT_FCOFF_HI_ON_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FC_PORT_FCOFF_HI_OFF",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2670,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FC_PORT_FCOFF_HI_OFF_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FC_PORT_FCOFF_LOW_ON",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2672,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FC_PORT_FCOFF_LOW_ON_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FC_PORT_FCOFF_LOW_OFF",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2674,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FC_PORT_FCOFF_LOW_OFF_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FC_JUMBO_GLB_HI_ON",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2676,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FC_JUMBO_GLB_HI_ON_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FC_JUMBO_GLB_HI_OFF",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2678,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FC_JUMBO_GLB_HI_OFF_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FC_JUMBO_GLB_LOW_ON",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x267A,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FC_JUMBO_GLB_LOW_ON_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FC_JUMBO_GLB_LOW_OFF",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x267C,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FC_JUMBO_GLB_LOW_OFF_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FC_JUMBO_PORT_HI_ON",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x267E,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FC_JUMBO_PORT_HI_ON_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FC_JUMBO_PORT_HI_OFF",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2680,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FC_JUMBO_PORT_HI_OFF_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FC_JUMBO_PORT_LOW_ON",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2682,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FC_JUMBO_PORT_LOW_ON_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FC_JUMBO_PORT_LOW_OFF",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2684,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FC_JUMBO_PORT_LOW_OFF_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FC_JUMBO_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2686,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FC_JUMBO_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FC_TOTAL_PAGE",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2688,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FC_TOTAL_PAGE_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FC_PUBLIC_PAGE",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x268A,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FC_PUBLIC_PAGE_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FC_MAX_TOTAL_PAGE",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x268C,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FC_MAX_TOTAL_PAGE_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FC_MAX_PUBLIC_PAGE",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x268E,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FC_MAX_PUBLIC_PAGE_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FC_PORT_PAGE",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2690,
        /* field numbers */     2,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   FC_PORT_PAGE_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FC_MAX_PORT_PAGE",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x26A0,
        /* field numbers */     2,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   FC_MAX_PORT_PAGE_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FC_FCOFF_PUBLIC_PAGE",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x26B0,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FC_FCOFF_PUBLIC_PAGE_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FC_JUMBO_PUBLIC_PAGE",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x26B2,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FC_JUMBO_PUBLIC_PAGE_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FC_MAX_FCOFF_PUBLIC_PAGE",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x26B4,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FC_MAX_FCOFF_PUBLIC_PAGE_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FC_MAX_JUMBO_PUBLIC_PAGE",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x26B6,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FC_MAX_JUMBO_PUBLIC_PAGE_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FC_CLR_MAX",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x26B8,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FC_CLR_MAX_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FC_FCDROP_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x26BA,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FC_FCDROP_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "MAX_FIFO_SIZE",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x26BC,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   MAX_FIFO_SIZE_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_SDF_TO_SDF_BAPSS",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x26BE,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_SDF_TO_SDF_BAPSS_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CGSTTIMER_HALF",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1FCE,
        /* field numbers */     5,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   CGSTTIMER_HALF_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CGSTTIMER",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x1FD0,
        /* field numbers */     3,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   CGSTTIMER_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PORT_PRI",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x32FE,
        /* field numbers */     1,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        3,
        /* register fields */   PORT_PRI_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DOT1Q_PRI_REMAP_W0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3302,
        /* field numbers */     8,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   DOT1Q_PRI_REMAP_W0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DOT1Q_PRI_REMAP_W1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3304,
        /* field numbers */     8,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   DOT1Q_PRI_REMAP_W1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PRI_SEL_REMAP_DSCP",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3306,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 63,
        /* portlist index */    0,
        /* bit offset */        3,
        /* register fields */   PRI_SEL_REMAP_DSCP_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PORT_WEIGHT_SEL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3320,
        /* field numbers */     1,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        1,
        /* register fields */   PORT_WEIGHT_SEL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PRI_WEIGHT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3322,
        /* field numbers */     10,
        /* port index */        0, 0,
        /* array index */       0, 1,
        /* portlist index */    0,
        /* bit offset */        48,
        /* register fields */   PRI_WEIGHT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PRI_TO_CPU_W0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x332E,
        /* field numbers */     8,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PRI_TO_CPU_W0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PRI_TO_CPU_W1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3330,
        /* field numbers */     8,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PRI_TO_CPU_W1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PRI_TRAP",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3332,
        /* field numbers */     6,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PRI_TRAP_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PRI_TRAP2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3334,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PRI_TRAP2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "WFQ_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2D60,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   WFQ_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "WFQ_BURSIZE",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2D62,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   WFQ_BURSIZE_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SCHED_PORT_Q_CTRL_SET",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2D64,
        /* field numbers */     1,
        /* port index */        0, 7,
        /* array index */       0, 7,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   SCHED_PORT_Q_CTRL_SET_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "SCHED_PORT_WFQ_CTRL_SET",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2DE4,
        /* field numbers */     9,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   SCHED_PORT_WFQ_CTRL_SET_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "APREN",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2DF4,
        /* field numbers */     1,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        1,
        /* register fields */   APREN_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_QNUM",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3336,
        /* field numbers */     1,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        3,
        /* register fields */   CFG_QNUM_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_QID_ASSIGN",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x333A,
        /* field numbers */     16,
        /* port index */        0, 0,
        /* array index */       0, 7,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   CFG_QID_ASSIGN_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RMK_PORT_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2BD4,
        /* field numbers */     3,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        16,
        /* register fields */   RMK_PORT_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RMK_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2BE4,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RMK_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RMK_INTPRI2DSCP_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2BE6,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 7,
        /* portlist index */    0,
        /* bit offset */        6,
        /* register fields */   RMK_INTPRI2DSCP_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RMK_INTPRI2IPRI_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2BEE,
        /* field numbers */     16,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        32,
        /* register fields */   RMK_INTPRI2IPRI_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DOT1X_PORT_EN",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x36FA,
        /* field numbers */     1,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        1,
        /* register fields */   DOT1X_PORT_EN_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DOT1X_PORT_AUTH",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x36FC,
        /* field numbers */     1,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        1,
        /* register fields */   DOT1X_PORT_AUTH_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DOT1X_PORT_DIR",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x36FE,
        /* field numbers */     1,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        1,
        /* register fields */   DOT1X_PORT_DIR_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DOT1X_UNAUTH_ACT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3700,
        /* field numbers */     1,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        1,
        /* register fields */   DOT1X_UNAUTH_ACT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DOS_CFG",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x335A,
        /* field numbers */     11,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   DOS_CFG_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY_WOL_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x7A0,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   PHY_WOL_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "PHY_WOL_MAC",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x7A2,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        48,
        /* register fields */   PHY_WOL_MAC_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DEBUG_TRAP_DROP0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x335C,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   DEBUG_TRAP_DROP0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DEBUG_TRAP_DROP1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x335E,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   DEBUG_TRAP_DROP1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DEBUG_TRAP_DROP2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3360,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   DEBUG_TRAP_DROP2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DEBUG_TRAP_DROP3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3362,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   DEBUG_TRAP_DROP3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DEBUG_TRAP_DROP_IND",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3364,
        /* field numbers */     8,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   DEBUG_TRAP_DROP_IND_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DBG_CTRL_ADR0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4090,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   DBG_CTRL_ADR0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DBG_CTRL_ADR1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4092,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   DBG_CTRL_ADR1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DBG_CTRL_ADR2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4094,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   DBG_CTRL_ADR2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DBG_CTRL_ADR3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4096,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   DBG_CTRL_ADR3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DBG_CTRL_SEL0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x4098,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   DBG_CTRL_SEL0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DBG_CTRL_SEL1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x409A,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   DBG_CTRL_SEL1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DBG_CTRL_SEL2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x409C,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   DBG_CTRL_SEL2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DBG_CTRL_SEL3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x409E,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   DBG_CTRL_SEL3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DBG_CTRL_VAL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x40A0,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   DBG_CTRL_VAL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DBG_BIT_SEL0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x40A2,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   DBG_BIT_SEL0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DBG_BIT_SEL1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x40A4,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   DBG_BIT_SEL1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DBG_BIT_SEL2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x40A6,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   DBG_BIT_SEL2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DBG_BIT_SEL3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x40A8,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   DBG_BIT_SEL3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DBG_BIT_SEL4",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x40AA,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   DBG_BIT_SEL4_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DBG_BIT_SEL5",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x40AC,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   DBG_BIT_SEL5_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DBG_BIT_SEL6",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x40AE,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   DBG_BIT_SEL6_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DBG_BIT_SEL7",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x40B0,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   DBG_BIT_SEL7_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DBG_BIT_SEL8",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x40B2,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   DBG_BIT_SEL8_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DBG_BIT_SEL9",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x40B4,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   DBG_BIT_SEL9_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DBG_BIT_SEL10",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x40B6,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   DBG_BIT_SEL10_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DBG_BIT_SEL11",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x40B8,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   DBG_BIT_SEL11_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DBG_BIT_SEL12",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x40BA,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   DBG_BIT_SEL12_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DBG_BIT_SEL13",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x40BC,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   DBG_BIT_SEL13_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DBG_BIT_SEL14",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x40BE,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   DBG_BIT_SEL14_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DBG_BIT_SEL15",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x40C0,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   DBG_BIT_SEL15_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_DBG_MSK",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x40C2,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_DBG_MSK_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_DBG_CMP_VAL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x40C4,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_DBG_CMP_VAL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_DBG_TRIG_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x40C6,
        /* field numbers */     3,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_DBG_TRIG_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RLDP_RLPP_CTRL",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2760,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RLDP_RLPP_CTRL_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RLDP_REG0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2762,
        /* field numbers */     8,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RLDP_REG0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RLDP_REG1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2764,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RLDP_REG1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RLDP_REG2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2766,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RLDP_REG2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RLDP_REG3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2768,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RLDP_REG3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RLDP_REG4",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x276A,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RLDP_REG4_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RLDP_RAND_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x276C,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RLDP_RAND_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RLDP_RAND_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x276E,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RLDP_RAND_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RLDP_RAND_2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2770,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RLDP_RAND_2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RLDP_SEED_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2772,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RLDP_SEED_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RLDP_SEED_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2774,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RLDP_SEED_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RLDP_SEED_2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2776,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RLDP_SEED_2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RLDP_LOOP_STS0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2778,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RLDP_LOOP_STS0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RLDP_LOOP_STS2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x277A,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RLDP_LOOP_STS2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RLDP_LOOP_PORT",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x277C,
        /* field numbers */     1,
        /* port index */        0, 7,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        3,
        /* register fields */   RLDP_LOOP_PORT_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "RLDP_LOOP_STS1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x2780,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   RLDP_LOOP_STS1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "IO_CTRL_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3FB2,
        /* field numbers */     4,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   IO_CTRL_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "IO_CTRL_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3FB4,
        /* field numbers */     7,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   IO_CTRL_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "IO_CTRL_2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3FB6,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   IO_CTRL_2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "IO_CTRL_4",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3FB8,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   IO_CTRL_4_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DBG_PAD_CTRL_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3FBA,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   DBG_PAD_CTRL_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "DBG_PAD_CTRL_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3FBC,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   DBG_PAD_CTRL_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "GPIO_MODE_X0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3FBE,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   GPIO_MODE_X0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "GPIO_MODE_X1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3FC0,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   GPIO_MODE_X1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_PAD_DRI_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3FC2,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_PAD_DRI_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_PAD_DRI_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3FC4,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_PAD_DRI_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_PAD_DRI_2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3FC6,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_PAD_DRI_2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_PAD_DRI_3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3FC8,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_PAD_DRI_3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_PAD_DRI_4",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3FCA,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_PAD_DRI_4_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_PAD_SLEW_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3FCC,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_PAD_SLEW_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_PAD_SLEW_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3FCE,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_PAD_SLEW_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_PAD_SLEW_2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3FD0,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_PAD_SLEW_2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_PAD_SLEW_3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3FD2,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_PAD_SLEW_3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_PAD_SLEW_4",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3FD4,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_PAD_SLEW_4_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_PAD_SMT_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3FD6,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_PAD_SMT_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_PAD_SMT_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3FD8,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_PAD_SMT_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_PAD_SMT_2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3FDA,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_PAD_SMT_2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_PAD_SMT_3",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3FDC,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_PAD_SMT_3_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "CFG_PAD_SMT_4",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3FDE,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   CFG_PAD_SMT_4_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FORCE_PAD_PU_PD_EN_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3FE0,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FORCE_PAD_PU_PD_EN_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FORCE_PAD_PU_PD_EN_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3FE2,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FORCE_PAD_PU_PD_EN_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FORCE_PAD_PU_PD_EN_2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3FE4,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FORCE_PAD_PU_PD_EN_2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FORCE_PAD_PU_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3FE6,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FORCE_PAD_PU_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FORCE_PAD_PU_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3FE8,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FORCE_PAD_PU_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FORCE_PAD_PU_2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3FEA,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FORCE_PAD_PU_2_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FORCE_PAD_PD_0",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3FEC,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FORCE_PAD_PD_0_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FORCE_PAD_PD_1",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3FEE,
        /* field numbers */     1,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FORCE_PAD_PD_1_RTL8371C_FIELDS,
    },

    {
        #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
        /* register name  */    "FORCE_PAD_PD_2",
        #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
        /* offset address */    0x3FF0,
        /* field numbers */     2,
        /* port index */        0, 0,
        /* array index */       0, 0,
        /* portlist index */    0,
        /* bit offset */        0,
        /* register fields */   FORCE_PAD_PD_2_RTL8371C_FIELDS,
    },
};

