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

#ifndef __RTKSW_RTL8371C_MACDRV_H__
#define __RTKSW_RTL8371C_MACDRV_H__

#include <rtk_types.h>
#include <rtk_error.h>
#include <dal/reg.h>

#define RTL8371C_TBL_BUSY_CHECK_NO  (100)
#define RTL8371C_TABLE_WORD         (10)

/* Function Name:
 *      rtl8371c_table_write
 * Description:
 *      Write one L2 specified table entry by table index.
 * Input:
 *      unit    - Unit ID
 *      table   - table index
 *      addr    - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
extern rtksw_int32 rtl8371c_table_write(rtksw_uint32 unit, rtksw_uint32 table, rtksw_uint32 addr, rtksw_uint32 *pData);


/* Function Name:
 *      rtl8371c_table_read
 * Description:
 *      Read one L2 specified table entry by table index.
 * Input:
 *      unit    - Unit ID
 *      table   - table index
 *      addr    - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
extern rtksw_int32 rtl8371c_table_read(rtksw_uint32 unit, rtksw_uint32 table, rtksw_uint32 addr, rtksw_uint32 *pData);

#endif /* End of __RTKSW_RTL8371C_MACDRV_H__ */