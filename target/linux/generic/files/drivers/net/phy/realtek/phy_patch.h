/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __PHY_PATCH_H__
#define __PHY_PATCH_H__

#include <linux/phy.h>
#include <linux/types.h>

enum rtlgen_phypatch_op {
	RTK_PATCH_OP_PHY,
	RTK_PATCH_OP_PHY_WAIT,
	RTK_PATCH_OP_PHY_WAIT_NOT,
	RTK_PATCH_OP_PHYOCP,
	RTK_PATCH_OP_PHYOCP_BC62,
	RTK_PATCH_OP_TOP,
	RTK_PATCH_OP_TOPOCP, // TODO unimplemented
	RTK_PATCH_OP_PSDS0,
	RTK_PATCH_OP_PSDS1, // TODO unimplemented
	RTK_PATCH_OP_MSDS, // TODO unimplemented
	RTK_PATCH_OP_MAC, // TODO unimplemented
	RTK_PATCH_OP_DELAY_MS
};

struct rtlgen_hwpatch {
	u8 patch_op;
	u8 portmask;
	__le16 pagemmd;
	__le16 addr;
	u8 msb;
	u8 lsb;
	__le16 data;
} __packed;

struct rtlgen_phy_patch_db {
	int (*patch_op_exec)(struct phy_device *phydev, const struct rtlgen_hwpatch *patch);
	size_t count;
	struct rtlgen_hwpatch conf[] __counted_by(count);
};

int rtlgen_phy_patch(struct phy_device *phydev);

#endif
