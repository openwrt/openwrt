/** @file */
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

#ifndef _MAC_AX_SPATIAL_REUSE_H_
#define _MAC_AX_SPATIAL_REUSE_H_

#include "../type.h"

u32 mac_sr_update(struct mac_ax_adapter *adapter,
		  struct mac_ax_sr_info *sr_info,
		  enum mac_ax_band band);

u32 spatial_reuse_init(struct mac_ax_adapter *adapter,
		       enum mac_ax_band band);

#endif
