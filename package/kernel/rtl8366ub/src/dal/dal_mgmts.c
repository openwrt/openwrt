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

#if (!defined(CONFIG_DAL_RTL8367C) && !defined(CONFIG_DAL_RTL8367D) && !defined(CONFIG_DAL_RTL8371C) && !defined(CONFIG_MSSDK))
#define CONFIG_DAL_ALL
#endif

#include <dal/dal_mgmts.h>
#include <dal/dal_mappers.h>
#include <dal/smi.h>
#if defined(CONFIG_DAL_RTL8367C) || defined(CONFIG_DAL_ALL)
#include <dal/rtl8367c/dal_rtl8367c_mapper.h>
#endif
#if defined(CONFIG_DAL_RTL8367D) || defined(CONFIG_DAL_ALL)
#include <dal/rtl8367d/dal_rtl8367d_mapper.h>
#endif
#if defined(CONFIG_DAL_RTL8371C) || defined(CONFIG_DAL_ALL)
#include <dal/rtl8371c/dal_rtl8371c_mapper.h>
#include <dal/reg.h>
extern rtksw_driver_t rtl8371c_driver;
extern rtksw_halreg_t *halRegCtrl[DAL_MGMT_MAX_UNIT];
#endif

dal_mgmt_info_t         Mgmt_node = {0};
dal_mgmt_info_t         *pRtkswMgmt_node = &Mgmt_node;


static dal_mapper_info_t dal_mapper_database[] =
{

    {CHIP_RTL8367C,     NULL},
    {CHIP_RTL8370B,     NULL},
    {CHIP_RTL8364B,     NULL},
    {CHIP_RTL8363SC_VB, NULL},
    {CHIP_RTL8367D,     NULL},
    {CHIP_RTL8370C,     NULL},
	{CHIP_RTL8367E,     NULL},
    {CHIP_RTL8371C,     NULL},
    
};

rtksw_int32 dal_mgmt_attachDevice(rtksw_uint32 unit, switch_chip_t switchChip)
{
    rtksw_uint32  mapper_size = sizeof(dal_mapper_database)/sizeof(dal_mapper_info_t);
    rtksw_uint32  mapper_index;

    /*mapper init*/
    for (mapper_index = 0; mapper_index < mapper_size; mapper_index++)
    {
        if (switchChip == dal_mapper_database[mapper_index].switchChip)
        {
#if defined(CONFIG_DAL_RTL8367C) || defined(CONFIG_DAL_ALL)
            if ((switchChip == CHIP_RTL8367C) || (switchChip == CHIP_RTL8370B) || (switchChip == CHIP_RTL8364B) || (switchChip == CHIP_RTL8363SC_VB)  || (switchChip == CHIP_RTL8370C))
            {
                dal_mapper_database[mapper_index].pMapper = dal_rtl8367c_mapper_get();
                pRtkswMgmt_node->pMapper[unit] = dal_mapper_database[mapper_index].pMapper;
                return RT_ERR_OK;
            }
#endif
#if defined(CONFIG_DAL_RTL8367D) || defined(CONFIG_DAL_ALL)
            if ((switchChip == CHIP_RTL8367D) || (switchChip == CHIP_RTL8367E))
            {
                dal_mapper_database[mapper_index].pMapper = dal_rtl8367d_mapper_get();
                pRtkswMgmt_node->pMapper[unit] = dal_mapper_database[mapper_index].pMapper;
                return RT_ERR_OK;
            }
#endif
#if defined(CONFIG_DAL_RTL8371C) || defined(CONFIG_DAL_ALL)
            if (switchChip == CHIP_RTL8371C)
            {
                dal_mapper_database[mapper_index].pMapper = dal_rtl8371c_mapper_get();
                pRtkswMgmt_node->pMapper[unit] = dal_mapper_database[mapper_index].pMapper;
                halRegCtrl[unit]->pRtl_driver = &rtl8371c_driver;
                return RT_ERR_OK;
            }
#endif
        }
    }

    return RT_ERR_CHIP_NOT_SUPPORTED;
}
