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

#include "mac_priv.h"
#if MAC_AX_8192XB_SUPPORT
#include "mac_8192xb/mac_priv_8192xb.h"
#endif
#if MAC_AX_8852A_SUPPORT
#include "mac_8852a/mac_priv_8852a.h"
#endif
#if MAC_AX_8852B_SUPPORT
#include "mac_8852b/mac_priv_8852b.h"
#endif
#if MAC_AX_8852C_SUPPORT
#include "mac_8852c/mac_priv_8852c.h"
#endif
#if MAC_AX_8851B_SUPPORT
#include "mac_8851b/mac_priv_8851b.h"
#endif
#if MAC_AX_8851E_SUPPORT
#include "mac_8851e/mac_priv_8851e.h"
#endif
#if MAC_AX_8852D_SUPPORT
#include "mac_8852d/mac_priv_8852d.h"
#endif

u32 get_mac_ax_priv_size(void)
{
	return sizeof(struct mac_ax_priv_ops *);
}

struct mac_ax_priv_ops *mac_ax_get_priv_ops(struct mac_ax_adapter *adapter,
					    enum mac_ax_intf intf)
{
	struct mac_ax_priv_ops *priv_ops = NULL;

	switch (adapter->hw_info->chip_id) {
#if MAC_AX_8852A_SUPPORT
	case MAC_AX_CHIP_ID_8852A:
		priv_ops = get_mac_8852a_priv_ops(intf);
		break;
#endif
#if MAC_AX_8852B_SUPPORT
	case MAC_AX_CHIP_ID_8852B:
		priv_ops = get_mac_8852b_priv_ops(intf);
		break;
#endif
#if MAC_AX_8852C_SUPPORT
	case MAC_AX_CHIP_ID_8852C:
		priv_ops = get_mac_8852c_priv_ops(intf);
		break;
#endif
#if MAC_AX_8192XB_SUPPORT
	case MAC_AX_CHIP_ID_8192XB:
		priv_ops = get_mac_8192xb_priv_ops(intf);
		break;
#endif
#if MAC_AX_8851B_SUPPORT
	case MAC_AX_CHIP_ID_8851B:
		priv_ops = get_mac_8851b_priv_ops(intf);
		break;
#endif
#if MAC_AX_8851E_SUPPORT
	case MAC_AX_CHIP_ID_8851E:
		priv_ops = get_mac_8851e_priv_ops(intf);
		break;
#endif
#if MAC_AX_8852D_SUPPORT
	case MAC_AX_CHIP_ID_8852D:
		priv_ops = get_mac_8852d_priv_ops(intf);
		break;
#endif
	default:
		return NULL;
	}

	return priv_ops;
}
