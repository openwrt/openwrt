/******************************************************************************
 *
 * Copyright(c) 2022 Realtek Corporation.
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
#ifndef _PHL_SCANOFLD_H_
#define _PHL_SCANOFLD_H_
#include "phl_status.h"
#ifdef CONFIG_PHL_SCANOFLD
void phl_cmd_scanofld_end(void *drv, void *param, u8 band_idx);
enum phl_mdl_ret_code
phl_cmd_scanofld_hdl_internal_evt(void *dispr, void *priv, void *msg);
#endif
#endif /* _PHL_SCANOFLD_H_ */
