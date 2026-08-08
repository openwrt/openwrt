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

#include <rtk_error.h>
#include <chip.h>
#include <dal/dal_mgmts.h>

#if defined(CONFIG_DAL_RTL8367C) || defined(CONFIG_DAL_ALL)
#include <dal/rtl8367c/rtl8367c_asicdrv.h>
#endif

#if defined(CONFIG_DAL_RTL8367D) || defined(CONFIG_DAL_ALL)
#include <dal/rtl8367d/rtl8367d_asicdrv.h>
#endif

#if defined(CONFIG_DAL_RTL8371C) || defined(CONFIG_DAL_ALL)
#include <dal/rtl8371c/rtk_rtl8371c_reg_struct.h>
#endif

#if (!defined(FORCE_PROBE_RTL8367C) && !defined(FORCE_PROBE_RTL8370B) && !defined(FORCE_PROBE_RTL8364B) && !defined(FORCE_PROBE_RTL8363SC_VB) && !defined(FORCE_PROBE_RTL8367D) && !defined(FORCE_PROBE_RTL8370C) && !defined(FORCE_PROBE_RTL8367E) && !defined(FORCE_PROBE_RTL8371C))
#define AUTO_PROBE 1
#else
#define AUTO_PROBE 0
#endif

#define RTKSW_CHAR(__char__)    ((rtksw_uint32)__char__ + 1 - (rtksw_uint32)'A')

#if defined(CONFIG_DAL_RTL8367C) || defined(CONFIG_DAL_ALL)
#if (AUTO_PROBE || defined(FORCE_PROBE_RTL8367C))
static rtksw_switch_halCtrl_t rtl8367c_hal_Ctrl =
{
    /* Switch Chip */
    CHIP_RTL8367C,

    /* Logical to Physical */
    {0, 1, 2, 3, 4, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
     6, 7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },

    /* Physical to Logical */
    {UTP_PORT0, UTP_PORT1, UTP_PORT2, UTP_PORT3, UTP_PORT4, UNDEFINE_PORT, EXT_PORT0, EXT_PORT1,
     UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT,
     UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT,
     UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT},

    /* Port Type */
    {UTP_PORT, UTP_PORT, UTP_PORT, UTP_PORT, UTP_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT,
     UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT,
     EXT_PORT, EXT_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT,
     UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT},

    /* PTP port */
    {1, 1, 1, 1, 1, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0 },

    /* Valid port mask */
    ( (0x1 << UTP_PORT0) | (0x1 << UTP_PORT1) | (0x1 << UTP_PORT2) | (0x1 << UTP_PORT3) | (0x1 << UTP_PORT4) | (0x1 << EXT_PORT0) | (0x1 << EXT_PORT1) ),

    /* Valid UTP port mask */
    ( (0x1 << UTP_PORT0) | (0x1 << UTP_PORT1) | (0x1 << UTP_PORT2) | (0x1 << UTP_PORT3) | (0x1 << UTP_PORT4) ),

    /* Valid EXT port mask */
    ( (0x1 << EXT_PORT0) | (0x1 << EXT_PORT1) ),

    /* Valid CPU port mask */
    0x00,

    /* Minimum physical port number */
    0,

    /* Maxmum physical port number */
    7,

    /* Physical port mask */
    0xDF,

    /* Combo Logical port ID */
    4,

    /* USXGMII Logical portmask */
    0x0,

    /* HSG Logical portmask */
    (0x1 << EXT_PORT0),

    /* SGMII Logical portmask */
    (0x1 << EXT_PORT0),

    /* UTP 2.5G Logical portmask */
    0x00,

    /* Max Meter ID */
    31,

    /* MAX LUT Address Number */
    2112,

    /* Trunk Group Mask */
    0x03,

    /* Packet buffer page number */
    1024
};
#endif
#endif

#if defined(CONFIG_DAL_RTL8367C) || defined(CONFIG_DAL_ALL)
#if (AUTO_PROBE || defined(FORCE_PROBE_RTL8370B))
static rtksw_switch_halCtrl_t rtl8370b_hal_Ctrl =
{
    /* Switch Chip */
    CHIP_RTL8370B,

    /* Logical to Physical */
    {0, 1, 2, 3, 4, 5, 6, 7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
     8, 9, 10, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },

    /* Physical to Logical */
    {UTP_PORT0, UTP_PORT1, UTP_PORT2, UTP_PORT3, UTP_PORT4, UTP_PORT5, UTP_PORT6, UTP_PORT7,
     EXT_PORT0, EXT_PORT1, EXT_PORT2, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT,
     UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT,
     UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT},

    /* Port Type */
    {UTP_PORT, UTP_PORT, UTP_PORT, UTP_PORT, UTP_PORT, UTP_PORT, UTP_PORT, UTP_PORT,
     UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT,
     EXT_PORT, EXT_PORT, EXT_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT,
     UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT},

    /* PTP port */
    {1, 1, 1, 1, 1, 1, 1, 1,
     0, 0, 0, 0, 0, 0, 0, 0,
     1, 1, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0 },

    /* Valid port mask */
    ( (0x1 << UTP_PORT0) | (0x1 << UTP_PORT1) | (0x1 << UTP_PORT2) | (0x1 << UTP_PORT3) | (0x1 << UTP_PORT4) | (0x1 << UTP_PORT5) | (0x1 << UTP_PORT6) | (0x1 << UTP_PORT7) | (0x1 << EXT_PORT0) | (0x1 << EXT_PORT1) | (0x1 << EXT_PORT2) ),

    /* Valid UTP port mask */
    ( (0x1 << UTP_PORT0) | (0x1 << UTP_PORT1) | (0x1 << UTP_PORT2) | (0x1 << UTP_PORT3) | (0x1 << UTP_PORT4) | (0x1 << UTP_PORT5) | (0x1 << UTP_PORT6) | (0x1 << UTP_PORT7) ),

    /* Valid EXT port mask */
    ( (0x1 << EXT_PORT0) | (0x1 << EXT_PORT1) | (0x1 << EXT_PORT2) ),

    /* Valid CPU port mask */
    (0x1 << EXT_PORT2),

    /* Minimum physical port number */
    0,

    /* Maxmum physical port number */
    10,

    /* Physical port mask */
    0x7FF,

    /* Combo Logical port ID */
    7,

    /* USXGMII Logical portmask */
    0x0,

    /* HSG Logical portmask */
    (0x1 << EXT_PORT1),

    /* SGMII Logical portmask */
    ( (0x1 << EXT_PORT0) | (0x1 << EXT_PORT1) ),

    /* UTP 2.5G Logical portmask */
    0x00,

    /* Max Meter ID */
    63,

    /* MAX LUT Address Number 4096 + 64*/
    4160,

    /* Trunk Group Mask */
    0x07,

    /* Packet buffer page number */
    1536
};
#endif
#endif

#if defined(CONFIG_DAL_RTL8367C) || defined(CONFIG_DAL_ALL)
#if (AUTO_PROBE || defined(FORCE_PROBE_RTL8364B))
static rtksw_switch_halCtrl_t rtl8364b_hal_Ctrl =
{
    /* Switch Chip */
    CHIP_RTL8364B,

    /* Logical to Physical */
    {0xFF, 1, 0xFF, 3, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
     6, 7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },

    /* Physical to Logical */
    {UNDEFINE_PORT, UTP_PORT1, UNDEFINE_PORT, UTP_PORT3, UNDEFINE_PORT, UNDEFINE_PORT, EXT_PORT0, EXT_PORT1,
     UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT,
     UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT,
     UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT},

    /* Port Type */
    {UNKNOWN_PORT, UTP_PORT, UNKNOWN_PORT, UTP_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT,
     UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT,
     EXT_PORT, EXT_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT,
     UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT},

    /* PTP port */
    {0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0 },

    /* Valid port mask */
    ( (0x1 << UTP_PORT1) | (0x1 << UTP_PORT3) | (0x1 << EXT_PORT0) | (0x1 << EXT_PORT1) ),

    /* Valid UTP port mask */
    ( (0x1 << UTP_PORT1) | (0x1 << UTP_PORT3) ),

    /* Valid EXT port mask */
    ( (0x1 << EXT_PORT0) | (0x1 << EXT_PORT1) ),

    /* Valid CPU port mask */
    0x00,

    /* Minimum physical port number */
    0,

    /* Maxmum physical port number */
    7,

    /* Physical port mask */
    0xCA,

    /* Combo Logical port ID */
    4,

    /* USXGMII Logical portmask */
    0x0,

    /* HSG Logical portmask */
    (0x1 << EXT_PORT0),

    /* SGMII Logical portmask */
    ( (0x1 << EXT_PORT0) | (0x1 << EXT_PORT1) ),

    /* UTP 2.5G Logical portmask */
    0x00,

    /* Max Meter ID */
    32,

    /* MAX LUT Address Number */
    2112,

    /* Trunk Group Mask */
    0x01,

    /* Packet buffer page number */
    512
};
#endif
#endif

#if defined(CONFIG_DAL_RTL8367C) || defined(CONFIG_DAL_ALL)
#if (AUTO_PROBE || defined(FORCE_PROBE_RTL8363SC_VB))
static rtksw_switch_halCtrl_t rtl8363sc_vb_hal_Ctrl =
{
    /* Switch Chip */
    CHIP_RTL8363SC_VB,

    /* Logical to Physical */
    {0xFF, 0xFF, 1, 3, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
     6, 7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },

    /* Physical to Logical */
    {UNDEFINE_PORT, UTP_PORT2, UNDEFINE_PORT, UTP_PORT3, UNDEFINE_PORT, UNDEFINE_PORT, EXT_PORT0, EXT_PORT1,
     UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT,
     UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT,
     UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT},

    /* Port Type */
    {UNKNOWN_PORT, UNKNOWN_PORT, UTP_PORT, UTP_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT,
     UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT,
     EXT_PORT, EXT_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT,
     UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT},

    /* PTP port */
    {0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0 },

    /* Valid port mask */
    ( (0x1 << UTP_PORT2) | (0x1 << UTP_PORT3) | (0x1 << EXT_PORT0) | (0x1 << EXT_PORT1) ),

    /* Valid UTP port mask */
    ( (0x1 << UTP_PORT2) | (0x1 << UTP_PORT3) ),

    /* Valid EXT port mask */
    ( (0x1 << EXT_PORT0) | (0x1 << EXT_PORT1) ),

    /* Valid CPU port mask */
    0x00,

    /* Minimum physical port number */
    0,

    /* Maxmum physical port number */
    7,

    /* Physical port mask */
    0xCA,

    /* Combo Logical port ID */
    4,

    /* USXGMII Logical portmask */
    0x0,

    /* HSG Logical portmask */
    (0x1 << EXT_PORT0),

    /* SGMII Logical portmask */
    ( (0x1 << EXT_PORT0) | (0x1 << EXT_PORT1) ),

    /* UTP 2.5G Logical portmask */
    0x00,

    /* Max Meter ID */
    32,

    /* MAX LUT Address Number */
    2112,

    /* Trunk Group Mask */
    0x01,

    /* Packet buffer page number */
    512
};
#endif
#endif

#if defined(CONFIG_DAL_RTL8367D) || defined(CONFIG_DAL_ALL)
#if (AUTO_PROBE || defined(FORCE_PROBE_RTL8367D))
static rtksw_switch_halCtrl_t rtl8367d_hal_Ctrl =
{
    /* Switch Chip */
    CHIP_RTL8367D,

    /* Logical to Physical */
    {0, 1, 2, 3, 4, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
     6, 7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },

    /* Physical to Logical */
    {UTP_PORT0, UTP_PORT1, UTP_PORT2, UTP_PORT3, UTP_PORT4, UNDEFINE_PORT, EXT_PORT0, EXT_PORT1,
     UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT,
     UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT,
     UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT},

    /* Port Type */
    {UTP_PORT, UTP_PORT, UTP_PORT, UTP_PORT, UTP_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT,
     UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT,
     EXT_PORT, EXT_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT,
     UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT},

    /* PTP port */
    {0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0 },

    /* Valid port mask */
    ( (0x1 << UTP_PORT0) | (0x1 << UTP_PORT1) | (0x1 << UTP_PORT2) | (0x1 << UTP_PORT3) | (0x1 << UTP_PORT4) | (0x1 << EXT_PORT0) | (0x1 << EXT_PORT1) ),

    /* Valid UTP port mask */
    ( (0x1 << UTP_PORT0) | (0x1 << UTP_PORT1) | (0x1 << UTP_PORT2) | (0x1 << UTP_PORT3) | (0x1 << UTP_PORT4) ),

    /* Valid EXT port mask */
    ( (0x1 << EXT_PORT0) | (0x1 << EXT_PORT1) ),

    /* Valid CPU port mask */
    0x00,

    /* Minimum physical port number */
    0,

    /* Maxmum physical port number */
    7,

    /* Physical port mask */
    0xFF,

    /* Combo Logical port ID */
    4,

    /* USXGMII Logical portmask */
    0x0,

    /* HSG Logical portmask */
    ((0x1 << EXT_PORT0) |  (0x1 << EXT_PORT1)),

    /* SGMII Logical portmask */
    ((0x1 << EXT_PORT0) |  (0x1 << EXT_PORT1)),

    /* UTP 2.5G Logical portmask */
    0x00,

    /* Max Meter ID */
    39,

    /* MAX LUT Address Number */
    2056,

    /* Trunk Group Mask */
    0x03,

    /* Packet buffer page number */
    2048
};
#endif
#endif

#if defined(CONFIG_DAL_RTL8367D) || defined(CONFIG_DAL_ALL)
#if (AUTO_PROBE || defined(FORCE_PROBE_RTL8367E))
static rtksw_switch_halCtrl_t rtl8367e_hal_Ctrl =
{
    /* Switch Chip */
    CHIP_RTL8367E,

    /* Logical to Physical */
    {0, 1, 2, 3, 4, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
     6, 7, 5, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },

    /* Physical to Logical */
    {UTP_PORT0, UTP_PORT1, UTP_PORT2, UTP_PORT3, UTP_PORT4, EXT_PORT2, EXT_PORT0, EXT_PORT1,
     UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT,
     UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT,
     UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT},

    /* Port Type */
    {UTP_PORT, UTP_PORT, UTP_PORT, UTP_PORT, UTP_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT,
     UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT,
     EXT_PORT, EXT_PORT, EXT_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT,
     UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT},

    /* PTP port */
    {0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0 },

    /* Valid port mask */
    ( (0x1 << UTP_PORT0) | (0x1 << UTP_PORT1) | (0x1 << UTP_PORT2) | (0x1 << UTP_PORT3) | (0x1 << UTP_PORT4) | (0x1 << EXT_PORT0) | (0x1 << EXT_PORT1) | (0x1 << EXT_PORT2) ),

    /* Valid UTP port mask */
    ( (0x1 << UTP_PORT0) | (0x1 << UTP_PORT1) | (0x1 << UTP_PORT2) | (0x1 << UTP_PORT3) | (0x1 << UTP_PORT4) ),

    /* Valid EXT port mask */
    ( (0x1 << EXT_PORT0) | (0x1 << EXT_PORT1) | (0x1 << EXT_PORT2) ),

    /* Valid CPU port mask */
    (0x1 << EXT_PORT2),

    /* Minimum physical port number */
    0,

    /* Maxmum physical port number */
    7,

    /* Physical port mask */
    0xFF,

    /* Combo Logical port ID */
    4,

    /* USXGMII Logical portmask */
    0x0,

    /* HSG Logical portmask */
    ((0x1 << EXT_PORT0) |  (0x1 << EXT_PORT1)),

    /* SGMII Logical portmask */
    ((0x1 << EXT_PORT0) |  (0x1 << EXT_PORT1)),

    /* UTP 2.5G Logical portmask */
    0x00,

    /* Max Meter ID */
    39,

    /* MAX LUT Address Number */
    2056,

    /* Trunk Group Mask */
    0x03,

    /* Packet buffer page number */
    2048
};
#endif
#endif

#if defined(CONFIG_DAL_RTL8371C) || defined(CONFIG_DAL_ALL)
#if (AUTO_PROBE || defined(FORCE_PROBE_RTL8371C))
static rtksw_switch_halCtrl_t rtl8371c_hal_Ctrl =
{
    /* Switch Chip */
    CHIP_RTL8371C,

    /* Logical to Physical */
    {0, 1, 2, 3, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
     6, 7, 5, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },

    /* Physical to Logical */
    {UTP_PORT0, UTP_PORT1, UTP_PORT2, UTP_PORT3, UNDEFINE_PORT, EXT_PORT2, EXT_PORT0, EXT_PORT1,
     UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT,
     UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT,
     UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT},

    /* Port Type */
    {UTP_PORT, UTP_PORT, UTP_PORT, UTP_PORT, UTP_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT,
     UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT,
     EXT_PORT, EXT_PORT, EXT_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT,
     UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT},

    /* PTP port */
    {1, 1, 1, 1, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0,
     1, 1, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0 },

    /* Valid port mask */
    ( (0x1 << UTP_PORT0) | (0x1 << UTP_PORT1) | (0x1 << UTP_PORT2) | (0x1 << UTP_PORT3) | (0x1 << EXT_PORT0) | (0x1 << EXT_PORT1) | (0x1 << EXT_PORT2) ),

    /* Valid UTP port mask */
    ( (0x1 << UTP_PORT0) | (0x1 << UTP_PORT1) | (0x1 << UTP_PORT2) | (0x1 << UTP_PORT3) ),

    /* Valid EXT port mask */
    ( (0x1 << EXT_PORT0) | (0x1 << EXT_PORT1) | (0x1 << EXT_PORT2) ),

    /* Valid CPU port mask */
    0x00,

    /* Minimum physical port number */
    0,

    /* Maxmum physical port number */
    7,

    /* Physical port mask */
    0xFF,

    /* Combo Logical port ID */
    0,

    /* USXGMII Logical portmask */
    (0x1 << EXT_PORT1),

    /* HSG Logical portmask */
    ((0x1 << EXT_PORT0) |  (0x1 << EXT_PORT1)),

    /* SGMII Logical portmask */
    ((0x1 << EXT_PORT0) |  (0x1 << EXT_PORT1)),

    /* UTP 2.5G Logical portmask */
    ((0x1 << UTP_PORT0) |  (0x1 << UTP_PORT1)),

    /* Max Meter ID */
    39,

    /* MAX LUT Address Number */
    2056,

    /* Trunk Group Mask */
    0x03,

    /* Packet buffer page number */
    4096
};
#endif
#endif

#if defined(CONFIG_DAL_RTL8367C) || defined(CONFIG_DAL_ALL)
#if (AUTO_PROBE || defined(FORCE_PROBE_RTL8370C))
static rtksw_switch_halCtrl_t rtl8370c_hal_Ctrl =
{
    /* Switch Chip */
    CHIP_RTL8370C,

    /* Logical to Physical */
    {0, 1, 2, 3, 4, 5, 6, 7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
     8, 9, 10, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },

    /* Physical to Logical */
    {UTP_PORT0, UTP_PORT1, UTP_PORT2, UTP_PORT3, UTP_PORT4, UTP_PORT5, UTP_PORT6, UTP_PORT7,
     EXT_PORT0, EXT_PORT1, EXT_PORT2, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT,
     UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT,
     UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT},

    /* Port Type */
    {UTP_PORT, UTP_PORT, UTP_PORT, UTP_PORT, UTP_PORT, UTP_PORT, UTP_PORT, UTP_PORT,
     UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT,
     EXT_PORT, EXT_PORT, EXT_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT,
     UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT},

    /* PTP port */
    {1, 1, 1, 1, 1, 1, 1, 1,
     0, 0, 0, 0, 0, 0, 0, 0,
     1, 1, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0 },

    /* Valid port mask */
    ( (0x1 << UTP_PORT0) | (0x1 << UTP_PORT1) | (0x1 << UTP_PORT2) | (0x1 << UTP_PORT3) | (0x1 << UTP_PORT4) | (0x1 << UTP_PORT5) | (0x1 << UTP_PORT6) | (0x1 << UTP_PORT7) | (0x1 << EXT_PORT0) | (0x1 << EXT_PORT1) | (0x1 << EXT_PORT2) ),

    /* Valid UTP port mask */
    ( (0x1 << UTP_PORT0) | (0x1 << UTP_PORT1) | (0x1 << UTP_PORT2) | (0x1 << UTP_PORT3) | (0x1 << UTP_PORT4) | (0x1 << UTP_PORT5) | (0x1 << UTP_PORT6) | (0x1 << UTP_PORT7) ),

    /* Valid EXT port mask */
    ( (0x1 << EXT_PORT0) | (0x1 << EXT_PORT1) | (0x1 << EXT_PORT2) ),

    /* Valid CPU port mask */
    (0x1 << EXT_PORT2),

    /* Minimum physical port number */
    0,

    /* Maxmum physical port number */
    10,

    /* Physical port mask */
    0x7FF,

    /* Combo Logical port ID */
    0xFF,

    /* USXGMII Logical portmask */
    0x0,
    
    /* HSG Logical portmask */
    ( (0x1 << EXT_PORT0) | (0x1 << EXT_PORT1) ),

    /* SGMII Logical portmask */
    ( (0x1 << EXT_PORT0) | (0x1 << EXT_PORT1) ),
    
    /* UTP 2.5G Logical portmask */
    0x00,
    
    /* Max Meter ID */
    63,

    /* MAX LUT Address Number 4096 + 64*/
    4160,

    /* Trunk Group Mask */
    0x07,

    /* Packet buffer page number */
    1536
};
#endif
#endif

/* Function Name:
 *      switch_probe
 * Description:
 *      Probe switch
 * Input:
 *      unit            - Unit ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - Switch probed
 *      RT_ERR_FAILED   - Switch Unprobed.
 * Note:
 *
 */
rtksw_api_ret_t switch_probe(rtksw_uint32 unit, switch_chip_t *pSwitchChip)
{
#if defined(FORCE_PROBE_RTL8367C)

    *pSwitchChip = CHIP_RTL8367C;
    return RT_ERR_OK;

#elif defined(FORCE_PROBE_RTL8370B)

    *pSwitchChip = CHIP_RTL8370B;
    return RT_ERR_OK;

#elif defined(FORCE_PROBE_RTL8364B)

    *pSwitchChip = CHIP_RTL8364B;
    return RT_ERR_OK;

#elif defined(FORCE_PROBE_RTL8363SC_VB)

    *pSwitchChip = CHIP_RTL8363SC_VB;
    return RT_ERR_OK;

#elif defined(FORCE_PROBE_RTL8367D)

    *pSwitchChip = CHIP_RTL8367D;
    return RT_ERR_OK;
    
#elif defined(FORCE_PROBE_RTL8370C)

    *pSwitchChip = CHIP_RTL8370C;
    return RT_ERR_OK;
    
#elif defined(FORCE_PROBE_RTL8367E)

    *pSwitchChip = CHIP_RTL8367E;
    return RT_ERR_OK;

#elif defined(FORCE_PROBE_RTL8371C)

    *pSwitchChip = CHIP_RTL8371C;
    return RT_ERR_OK;

#else

    rtksw_uint32 retVal;
#if defined(CONFIG_DAL_RTL8367C) || defined(CONFIG_DAL_RTL8367D) || defined(CONFIG_DAL_ALL)
    rtksw_uint32 data;
    rtksw_uint32 regValue;
#endif
#if defined(CONFIG_DAL_RTL8371C) || defined(CONFIG_DAL_ALL)
    rtksw_uint32 rtl_ID;
    rtksw_uint32 regData;
    rtksw_uint32 char1;
    rtksw_uint32 char2;
#endif

    if (retVal) {} /* Skip warning */

#if defined(CONFIG_DAL_RTL8367C) || defined(CONFIG_DAL_ALL)
    if((retVal = rtl8367c_setAsicReg(unit, 0x13C2, 0x0249)) != RT_ERR_OK)
        return retVal;

    if((retVal = rtl8367c_getAsicReg(unit, 0x1300, &data)) != RT_ERR_OK)
        return retVal;

    if((retVal = rtl8367c_getAsicReg(unit, 0x1301, &regValue)) != RT_ERR_OK)
        return retVal;

    if((retVal = rtl8367c_setAsicReg(unit, 0x13C2, 0x0000)) != RT_ERR_OK)
        return retVal;

    switch (data)
    {
        case 0x0276:
        case 0x0597:
        case 0x6367:
            *pSwitchChip = CHIP_RTL8367C;
            return RT_ERR_OK;
        case 0x0652:
        case 0x6368:
            *pSwitchChip = CHIP_RTL8370B;
            return RT_ERR_OK;
        case 0x6871:
            *pSwitchChip = CHIP_RTL8370C;
            return RT_ERR_OK;
        case 0x0801:
        case 0x6511:
            if( (regValue & 0x00F0) == 0x0080)
            {
                *pSwitchChip = CHIP_RTL8363SC_VB;
                return RT_ERR_OK;
            }
            else
            {
                *pSwitchChip = CHIP_RTL8364B;
                return RT_ERR_OK;
            }
        default:
            break;
    }
#endif

#if defined(CONFIG_DAL_RTL8367D) || defined(CONFIG_DAL_ALL)
    if((retVal = rtl8367d_setAsicReg(unit, 0x13C2, 0x0249)) != RT_ERR_OK)
        return retVal;

    if((retVal = rtl8367d_getAsicReg(unit, 0x1300, &data)) != RT_ERR_OK)
        return retVal;

    if((retVal = rtl8367d_getAsicReg(unit, 0x1301, &regValue)) != RT_ERR_OK)
        return retVal;

    if((retVal = rtl8367d_setAsicReg(unit, 0x13C2, 0x0000)) != RT_ERR_OK)
        return retVal;

    switch (data)
    {
        case 0x6642:
            *pSwitchChip = CHIP_RTL8367D;
            return RT_ERR_OK;
        case 0x1181:
        case 0x6980:
            *pSwitchChip = CHIP_RTL8367E;
            return RT_ERR_OK;
        default:
            break;
     }
#endif

#if defined(CONFIG_DAL_RTL8371C) || defined(CONFIG_DAL_ALL)
    if ((retVal = hal_getAsicReg(unit, 0x0004, &rtl_ID)) != RT_ERR_OK)
        return retVal;

    if ((retVal = hal_getAsicReg(unit, 0x0006, &regData)) != RT_ERR_OK)
        return retVal;

    char1 = (regData >> 9) & 0x1F;
    char2 = (regData >> 4) & 0x1F;

    if ((rtl_ID == 0x8366) && (char1 == RTKSW_CHAR('U')) && (char2 == RTKSW_CHAR('B'))) /* RTL8366UB */
    {
        *pSwitchChip = CHIP_RTL8371C;
        return RT_ERR_OK;
    }
    else if (rtl_ID == 0x8371) /* RTL8371C */
    {
        *pSwitchChip = CHIP_RTL8371C;
        return RT_ERR_OK;
    }

#endif
#endif

    return RT_ERR_CHIP_NOT_FOUND;
}

/* Function Name:
 *      rtksw_hal_find_device
 * Description:
 *      Find the mac chip from SDK supported mac device lists.
 * Input:
 *      unit        - Unit ID
 * Output:
 *      None
 * Return:
 *      NULL        - Not found
 *      Otherwise   - Pointer of mac chip HAL structure that found
 * Note:
 *
 */
rtksw_switch_halCtrl_t *rtksw_hal_find_device(rtksw_uint32 unit)
{
    switch_chip_t switchChip;
    rtksw_uint32  retVal;

    /* probe switch */
    if((retVal = switch_probe(unit, &switchChip)) != RT_ERR_OK)
        return NULL;

    switch (switchChip)
    {
#if defined(CONFIG_DAL_RTL8367C) || defined(CONFIG_DAL_ALL)
#if (AUTO_PROBE || defined(FORCE_PROBE_RTL8367C))
        case CHIP_RTL8367C:
            return &rtl8367c_hal_Ctrl;
#endif
#if (AUTO_PROBE || defined(FORCE_PROBE_RTL8370B))
        case CHIP_RTL8370B:
            return &rtl8370b_hal_Ctrl;
#endif
#if (AUTO_PROBE || defined(FORCE_PROBE_RTL8364B))
        case CHIP_RTL8364B:
            return &rtl8364b_hal_Ctrl;
#endif
#if (AUTO_PROBE || defined(FORCE_PROBE_RTL8363SC_VB))
        case CHIP_RTL8363SC_VB:
            return &rtl8363sc_vb_hal_Ctrl;
#endif
#if (AUTO_PROBE || defined(FORCE_PROBE_RTL8370C))
        case CHIP_RTL8370C:
            return &rtl8370c_hal_Ctrl;
#endif
#endif

#if defined(CONFIG_DAL_RTL8367D) || defined(CONFIG_DAL_ALL)
#if (AUTO_PROBE || defined(FORCE_PROBE_RTL8367D))
        case CHIP_RTL8367D:
            return &rtl8367d_hal_Ctrl;
#endif
#if (AUTO_PROBE || defined(FORCE_PROBE_RTL8367E))
        case CHIP_RTL8367E:
            return &rtl8367e_hal_Ctrl;
#endif
#endif

#if defined(CONFIG_DAL_RTL8371C) || defined(CONFIG_DAL_ALL)
#if (AUTO_PROBE || defined(FORCE_PROBE_RTL8371C))
        case CHIP_RTL8371C:
            return &rtl8371c_hal_Ctrl;
#endif
#endif
        default:
            return NULL;
    }

    /* Not found */
    return NULL;
}
