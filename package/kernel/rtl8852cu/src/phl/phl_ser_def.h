/******************************************************************************
 *
 * Copyright(c) 2019 - 2020 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/
#ifndef __PHL_SER_DEF_H__
#define __PHL_SER_DEF_H__

enum RTW_PHL_SER_CFG_STEP {
	RTW_PHL_SER_M1_PRE_CFG,
	RTW_PHL_SER_M1_POST_CFG,
	RTW_PHL_SER_M5_CFG
};

enum RTW_PHL_SER_NOTIFY_EVENT {
	RTW_PHL_SER_L0_RESET, /* L0 notify only */
	RTW_PHL_SER_PAUSE_TRX, /* M1 */
	RTW_PHL_SER_DO_RECOVERY, /* M3 */
	RTW_PHL_SER_READY, /* M5 */
	RTW_PHL_SER_L2_RESET,/* M9 */
	RTW_PHL_SER_L2_RESET_DONE,
	RTW_PHL_SER_EVENT_CHK,
	RTW_PHL_SER_DUMP_FW_LOG,
	RTW_PHL_SER_LOG_ONLY,
	RTW_PHL_SER_MAX = 9
};

enum RTW_PHL_SER_RCVY_STEP {
	RTW_PHL_SER_L1_DISABLE_EN = 0x0001,
	RTW_PHL_SER_L1_RCVY_EN = 0x0002,
	RTW_PHL_SER_L0_CFG_NOTIFY = 0x0010,
	RTW_PHL_SER_L0_CFG_DIS_NOTIFY = 0x0011,
	RTW_PHL_SER_L0_CFG_HANDSHAKE = 0x0012,
	RTW_PHL_SER_L0_RCVY_EN = 0x0013,
};

#endif /* __PHL_SER_DEF_H__ */
