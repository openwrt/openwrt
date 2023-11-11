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

#ifndef _MAC_AX_TRX_DESC_H_
#define _MAC_AX_TRX_DESC_H_

#include "../type.h"
#include "role.h"
#if MAC_AX_8852A_SUPPORT
#include "mac_8852a/trx_desc_8852a.h"
#endif
#if MAC_AX_8852B_SUPPORT
#include "mac_8852b/trx_desc_8852b.h"
#endif
#if MAC_AX_8852C_SUPPORT
#include "mac_8852c/trx_desc_8852c.h"
#endif
#if MAC_AX_8192XB_SUPPORT
#include "mac_8192xb/trx_desc_8192xb.h"
#endif
#if MAC_AX_8851B_SUPPORT
#include "mac_8851b/trx_desc_8851b.h"
#endif
#if MAC_AX_8851E_SUPPORT
#include "mac_8851e/trx_desc_8851e.h"
#endif
#if MAC_AX_8852D_SUPPORT
#include "mac_8852d/trx_desc_8852d.h"
#endif
#if MAC_AX_1115E_SUPPORT
#include "mac_1115e/trx_desc_1115e.h"
#endif

#endif
