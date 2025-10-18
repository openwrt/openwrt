// SPDX-License-Identifier: GPL-2.0

#include "phy_patch.h"

#include <linux/errno.h>
#include <linux/phy.h>

#include "realtek.h"

int rtlgen_phy_patch(struct phy_device *phydev)
{
	struct rtl826x_priv *priv = phydev->priv;
	struct rtlgen_phy_patch_db *patch_db = priv->patch;
	size_t i;
	int ret;

	if (!patch_db || !patch_db->patch_op_exec) {
		phydev_err(phydev, "patch db or exec callback is NULL\n");
		return -EINVAL;
	}

	for (i = 0; i < patch_db->count; i++) {
		ret = patch_db->patch_op_exec(phydev, &patch_db->conf[i]);
		if (ret < 0) {
			phydev_err(phydev, "patch failed! %zu[%u][0x%X][0x%X][0x%X] ret=%d\n",
				   i, patch_db->conf[i].patch_op,
				   le16_to_cpu(patch_db->conf[i].pagemmd),
				   le16_to_cpu(patch_db->conf[i].addr),
				   le16_to_cpu(patch_db->conf[i].data), ret);
			return ret;
		}
	}

	return 0;
}
