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

#ifndef __DAL_MGMT_H__
#define __DAL_MGMT_H__

/*
 * Include Files
 */
#include <rtk_types.h>
#include <rtk_error.h>
#include <dal/dal_mappers.h>
#if defined(CONFIG_MSSDK)
#include <common/rt_autoconf.h>
#endif
/*
 * Symbol Definition
 */
#if defined(RTK_X86_CLE)
#define DAL_MGMT_MAX_UNIT   4
#elif defined(CONFIG_MSSDK)
#define  DAL_MGMT_MAX_UNIT  (CONFIG_SDK_MAX_UNIT_MS + CONFIG_SDK_MAX_UNIT_US)
#else
#define DAL_MGMT_MAX_UNIT   1
#endif

typedef struct dal_mgmt_info_s
{
    dal_mapper_t    *pMapper[DAL_MGMT_MAX_UNIT];
} dal_mgmt_info_t;

typedef struct dal_mapper_info_s
{
    switch_chip_t       switchChip;
    dal_mapper_t        *pMapper;
} dal_mapper_info_t;

/*
 * Data Declaration
 */
extern dal_mgmt_info_t      *pRtkswMgmt_node;

/*
 * Macro Definition
 */
#define RT_MGMT       pRtkswMgmt_node
#define RT_MAPPER     RT_MGMT->pMapper[unit]


/*
 * Function Declaration
 */

/* Module Name : */

/* Function Name:
 *      dal_mgmt_attachDevice
 * Description:
 *      Attach device(semaphore, database clear)
 * Input:
 *      unit            - unit ID
 *      switchChip      - switch type
 * Output:
 *      None
 * Return:
 *      RT_ERR_FAILED   - initialize fail
 *      RT_ERR_OK       - initialize success
 * Note:
 *      RTK must call this function before do other kind of action.
 */
extern rtksw_int32
dal_mgmt_attachDevice(rtksw_uint32 unit, switch_chip_t switchChip);

#endif /* __DAL_MGMT_H__ */


