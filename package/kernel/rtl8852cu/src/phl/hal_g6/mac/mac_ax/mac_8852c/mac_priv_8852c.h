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

#ifndef _MAC_AX_MAC_PRIV_8852C_H_
#define _MAC_AX_MAC_PRIV_8852C_H_

#include "../mac_priv.h"
#if MAC_AX_8852C_SUPPORT

struct mac_ax_priv_ops *get_mac_8852c_priv_ops(enum mac_ax_intf intf);

#endif /* #if MAC_AX_8852C_SUPPORT */
#endif
