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

#ifndef __RTK_RTL8371C_TABLEFIELD_LIST_H__
#define __RTK_RTL8371C_TABLEFIELD_LIST_H__

/*
 * Include Files
 */
#include <reg.h>


extern rtksw_tableField_t RTL8371C_VLAN_TBL_FIELDS[];

extern rtksw_tableField_t RTL8371C_L2_IPMC_FIELDS[];

extern rtksw_tableField_t RTL8371C_L2_MC_FIELDS[];

extern rtksw_tableField_t RTL8371C_L2_UC_FIELDS[];

extern rtksw_tableField_t RTL8371C_ACL_ACT_ENTRY_FIELDS[];

extern rtksw_tableField_t RTL8371C_ACL_RULE_DATA_FIELDS[];

extern rtksw_tableField_t RTL8371C_ACL_RULE_MASK_FIELDS[];



#endif    /* __RTK_RTL8371C_TABLEFIELD_LIST_H__ */
