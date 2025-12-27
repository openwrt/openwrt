#ifndef __PHY_PATCH_H__
#define __PHY_PATCH_H__

#include <linux/types.h>
#include <linux/phy.h>

enum rtk_phypatch_op {
	RTK_PATCH_OP_PHY,
	RTK_PATCH_OP_PHY_WAIT,
	RTK_PATCH_OP_PHY_WAIT_NOT,
	RTK_PATCH_OP_PHYOCP,
	RTK_PATCH_OP_PHYOCP_BC62,
	RTK_PATCH_OP_TOP,
	RTK_PATCH_OP_TOPOCP,
	RTK_PATCH_OP_PSDS0,
	RTK_PATCH_OP_PSDS1,
	RTK_PATCH_OP_MSDS,
	RTK_PATCH_OP_MAC,
	RTK_PATCH_OP_DELAY_MS
};

struct rtk_hwpatch {
	u8 patch_op;
	u8 portmask;
	u16 pagemmd;
	u16 addr;
	u8 msb;
	u8 lsb;
	u16 data;
} __packed;

struct rt_phy_patch_db {
	int (*patch_op)(struct phy_device *phydev, struct rtk_hwpatch *pPatch_data);
	struct rtk_hwpatch *conf;
	u32 size;
};

int phy_patch(struct phy_device *phydev);

#endif
