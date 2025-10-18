/*
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Copyright (c) 2023 Realtek Semiconductor Corp. All rights reserved.
 */

#ifndef __HAL_PHY_PHY_PATCH_RTL826XB_H__
#define __HAL_PHY_PHY_PATCH_RTL826XB_H__

/*
 * Include Files
 */
#include "phy_patch.h"

int phy_patch_rtl8264b_db_init(struct phy_device *phydev, rt_phy_patch_db_t **pPhy_patchDb);
int phy_patch_rtl8261n_db_init(struct phy_device *phydev, rt_phy_patch_db_t **pPhy_patchDb);

#endif /* __HAL_PHY_PHY_PATCH_RTL826XB_H__ */
