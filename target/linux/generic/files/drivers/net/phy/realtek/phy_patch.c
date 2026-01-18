#include "realtek.h"

int phy_patch(struct phy_device *phydev)
{
	struct rtl826x_priv *priv = phydev->priv;
	struct rt_phy_patch_db *patch_db = priv->patch;
	int ret = 0, i = 0;

	if (patch_db == NULL || patch_db->patch_op == NULL) {
		phydev_err(phydev, "db is NULL\n");
		return -EINVAL;
	}

	for (i = 0; i < patch_db->size / sizeof(struct rtk_hwpatch); i++) {
		ret = patch_db->patch_op(phydev, &patch_db->conf[i]);
		if (ret < 0) {
			phydev_err(phydev, "patch failed! %u[%u][0x%X][0x%X][0x%X] ret=0x%X\n",
				   i + 1, patch_db->conf[i].patch_op, patch_db->conf[i].pagemmd,
				   patch_db->conf[i].addr, patch_db->conf[i].data, ret);
			return ret;
		}
	}

	return 0;
}
