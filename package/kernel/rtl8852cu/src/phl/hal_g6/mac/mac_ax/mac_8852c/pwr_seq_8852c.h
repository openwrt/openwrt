/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation. All rights reserved.
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
 ******************************************************************************/

#ifndef _MAC_AX_PWR_SEQ_8852C_H_
#define _MAC_AX_PWR_SEQ_8852C_H_

#include "../../mac_def.h"
#if MAC_AX_8852C_SUPPORT
extern struct mac_pwr_cfg *pwr_on_seq_8852c[];
extern struct mac_pwr_cfg *pwr_off_seq_8852c[];
#if MAC_AX_FEATURE_HV
extern struct mac_pwr_cfg *card_disable_seq_8852c[];
extern struct mac_pwr_cfg *enter_lps_seq_8852c[];
extern struct mac_pwr_cfg *leave_lps_seq_8852c[];
extern struct mac_pwr_cfg *ips_seq_8852c[];
#endif
#endif /* #if MAC_AX_8852C_SUPPORT */
#endif
