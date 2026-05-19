/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __PHY_PATCH_RTL826X_H__
#define __PHY_PATCH_RTL826X_H__

#include <linux/phy.h>

int rtl8261n_phy_patch_db_init(struct phy_device *phydev);
int rtl8264b_phy_patch_db_init(struct phy_device *phydev);

#endif
