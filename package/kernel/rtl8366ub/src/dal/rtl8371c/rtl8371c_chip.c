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

#include <reg.h>
#include <rtk_rtl8371c_reg_struct.h>
#include <rtk_rtl8371c_table_struct.h>
#include <rtk_rtl8371c_macdrv.h>

static rtksw_macPpInfo_t rtl8371c_macPpInfo =
{
    0x1FA0, /* lowerbound_addr */
    0x23A0, /* upperbound_addr */
    0x80,  /* interval */
};

static rtksw_macPpInfo_t rtl8371c_macPTPPpInfo =
{
    0x3DB0, /* lowerbound_addr */
    0x3E30, /* upperbound_addr */
    0x10,  /* interval */
};

/* RTL8371C mac driver service APIs */
rtksw_macdrv_t rtl8371c_macdrv =
{
    rtl8371c_table_read,    /* fMdrv_table_read  */
    rtl8371c_table_write,   /* fMdrv_table_write */
}; /* end of rtl8371c_macdrv */

/* Definition RTL8371C major driver */
rtksw_driver_t rtl8371c_driver =
{
    /* register list   */       rtk_rtl8371c_reg_list,
    /* table list      */       rtk_rtl8371c_table_list,
    /* mac driver      */       &rtl8371c_macdrv,
    /* reg index max   */       RTL8371C_REG_LIST_END,
    /* regField index max */    RTL8371C_REGFIELD_LIST_END,
    /* table index max */       RTL8371C_TABLE_LIST_END,
    /* MAC per port info */     &rtl8371c_macPpInfo,
    /* PTP per port info */     &rtl8371c_macPTPPpInfo
}; /* end of rtl8371c_driver */