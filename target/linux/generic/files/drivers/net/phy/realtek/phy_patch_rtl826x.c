// SPDX-License-Identifier: GPL-2.0

#include "phy_patch_rtl826x.h"

#include <linux/bitops.h>
#include <linux/bits.h>
#include <linux/byteorder/generic.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/firmware.h>
#include <linux/gfp.h>
#include <linux/ktime.h>
#include <linux/mdio.h>
#include <linux/module.h>
#include <linux/overflow.h>
#include <linux/phy.h>
#include <linux/slab.h>
#include <linux/stddef.h>
#include <linux/string.h>
#include <linux/types.h>

#include "phy_patch.h"
#include "realtek.h"

#define PHY_PATCH_WAIT_TIMEOUT	 10000000

static u16 rtl826x_mmd_convert(u16 page, u16 addr)
{
	if (addr < 16)
		return 0xA400 + (page * 2);

	if (addr < 24)
		return (16 * page) + ((addr - 16) * 2);

	return 0xA430 + ((addr - 24) * 2);
}

static int rtl826x_phy_patch_wait(struct phy_device *phydev, u32 mmd_addr, u32 mmd_reg,
				  u16 data, u16 mask, bool wait_equal)
{
	s64 us_diff = 0;
	ktime_t start;
	ktime_t now;
	bool equal;
	int ret;

	start = ktime_get();

	do {
		ret = phy_read_mmd(phydev, mmd_addr, mmd_reg);
		if (ret < 0)
			return ret;

		equal = (ret & mask) == data;
		if (equal == wait_equal)
			break;

		msleep(1);

		now = ktime_get();
		us_diff = ktime_to_us(ktime_sub(now, start));
	} while (us_diff < PHY_PATCH_WAIT_TIMEOUT);

	if (us_diff >= PHY_PATCH_WAIT_TIMEOUT) {
		phydev_err(phydev, "826xb patch wait[%u,0x%X,0x%X,0x%X]:0x%X\n",
			   mmd_addr, mmd_reg, mask, data, ret);
		return -ETIME;
	}

	return 0;
}

static int rtl826x_phy_patch_sds_get(struct phy_device *phydev, u32 sds_page, u32 sds_reg)
{
	u32 sds_addr = 0x8000 + (sds_reg << 6) + sds_page;
	int ret;

	ret = phy_write_mmd(phydev, MDIO_MMD_VEND1, 0x143, sds_addr);
	if (ret < 0)
		return ret;

	ret = rtl826x_phy_patch_wait(phydev, MDIO_MMD_VEND1, 0x143, 0, BIT(15), true);
	if (ret < 0)
		return ret;

	return phy_read_mmd(phydev, MDIO_MMD_VEND1, 0x142);
}

static int rtl826x_phy_patch_sds_set(struct phy_device *phydev, u32 sds_page, u32 sds_reg,
				     u32 data)
{
	u32 sds_addr = 0x8800 + (sds_reg << 6) + sds_page;
	int ret;

	ret = phy_write_mmd(phydev, MDIO_MMD_VEND1, 0x141, data);
	if (ret < 0)
		return ret;

	ret = phy_write_mmd(phydev, MDIO_MMD_VEND1, 0x143, sds_addr);
	if (ret < 0)
		return ret;

	return rtl826x_phy_patch_wait(phydev, MDIO_MMD_VEND1, 0x143, 0, BIT(15), true);
}

static int rtl826x_phy_patch_sds_modify(struct phy_device *phydev, u32 sds_page, u32 sds_reg,
					u32 mask, u32 data)
{
	int ret = 0;

	if (mask != GENMASK(15, 0)) {
		ret = rtl826x_phy_patch_sds_get(phydev, sds_page, sds_reg);
		if (ret < 0)
			return ret;
	}

	return rtl826x_phy_patch_sds_set(phydev, sds_page, sds_reg, (ret & ~mask) | data);
}

static int rtl826x_phy_patch_op(struct phy_device *phydev, const struct rtlgen_hwpatch *patch)
{
	u32 mask = GENMASK(patch->msb, patch->lsb);
	u16 pagemmd = le16_to_cpu(patch->pagemmd);
	u16 addr = le16_to_cpu(patch->addr);
	u16 data = le16_to_cpu(patch->data);
	bool wait_equal;
	int ret = 0;
	int val;
	int cnt;
	u16 reg;

	phydev_dbg(phydev, "patch: op: %u pm: %x mmd: %x addr: %x mask: [%u, %u] data: %x\n",
		   patch->patch_op, patch->portmask, pagemmd, addr,
		   patch->msb, patch->lsb, data);

	switch (patch->patch_op) {
	case RTK_PATCH_OP_PHY:
		reg = rtl826x_mmd_convert(pagemmd, addr);
		if (mask != GENMASK(15, 0)) {
			ret = phy_modify_mmd(phydev, MDIO_MMD_VEND2, reg, mask,
					     data << patch->lsb);
			if (ret < 0)
				return ret;
		} else {
			ret = phy_write_mmd(phydev, MDIO_MMD_VEND2, reg,
					    data << patch->lsb);
			if (ret < 0)
				return ret;
		}
		break;

	case RTK_PATCH_OP_PHY_WAIT:
	case RTK_PATCH_OP_PHY_WAIT_NOT:
		wait_equal = patch->patch_op == RTK_PATCH_OP_PHY_WAIT;
		reg = rtl826x_mmd_convert(pagemmd, addr);
		ret = rtl826x_phy_patch_wait(phydev, MDIO_MMD_VEND2, reg,
					     data << patch->lsb, mask, wait_equal);
		if (ret < 0)
			return ret;
		break;

	case RTK_PATCH_OP_PHYOCP:
		if (mask != GENMASK(15, 0)) {
			ret = phy_modify_mmd(phydev, MDIO_MMD_VEND2, addr, mask,
					     data << patch->lsb);
			if (ret < 0)
				return ret;
		} else {
			ret = phy_write_mmd(phydev, MDIO_MMD_VEND2, addr,
					    data << patch->lsb);
			if (ret < 0)
				return ret;
		}
		break;

	case RTK_PATCH_OP_PHYOCP_BC62:
		val = phy_read_mmd(phydev, MDIO_MMD_VEND2, 0xbc62);
		if (val < 0)
			return val;

		val = (val >> 8) & 0x1f;
		for (cnt = 0; cnt <= val; cnt++) {
			ret = phy_modify_mmd(phydev, MDIO_MMD_VEND2, 0xbc62,
					     GENMASK(12, 8), cnt << 8);
			if (ret < 0)
				return ret;
		}
		break;

	case RTK_PATCH_OP_TOP:
		if (addr < 16)
			return -EINVAL;

		reg = (pagemmd * 8) + (addr - 16);
		if (mask != GENMASK(15, 0)) {
			ret = phy_modify_mmd(phydev, MDIO_MMD_VEND1, reg, mask,
					     data << patch->lsb);
			if (ret < 0)
				return ret;
		} else {
			ret = phy_write_mmd(phydev, MDIO_MMD_VEND1, reg, data << patch->lsb);
			if (ret < 0)
				return ret;
		}
		break;

	case RTK_PATCH_OP_PSDS0:
		ret = rtl826x_phy_patch_sds_modify(phydev, pagemmd, addr,
						   mask, data << patch->lsb);
		if (ret < 0)
			return ret;
		break;

	case RTK_PATCH_OP_DELAY_MS:
		msleep(data);
		break;

	default:
		phydev_err(phydev, "%s: %u not implemented yet!\n", __func__, patch->patch_op);
		return -EINVAL;
	}

	return ret;
}

static int rtl826x_phy_patch_db_init(struct phy_device *phydev, const char *fw_name)
{
	struct rtl826x_priv *priv = phydev->priv;
	struct device *dev = &phydev->mdio.dev;
	struct rtlgen_phy_patch_db *patch_db;
	struct rtlgen_hwpatch *patches;
	const struct firmware *fw;
	size_t count;
	int ret;

	ret = request_firmware_direct(&fw, fw_name, dev);
	if (ret) {
		if (ret == -ENOENT) {
			phydev_dbg(phydev, "Failed to request %s: defer probing\n", fw_name);
			return -EPROBE_DEFER;
		} else {
			phydev_err(phydev, "Failed to request %s: %d\n", fw_name, ret);
			return ret;
		}
	}

	if (!fw->size || fw->size % sizeof(*patches) != 0) {
		phydev_err(phydev, "Invalid firmware size %zu for %s\n",
			   fw->size, fw_name);
		ret = -EINVAL;
		goto out;
	}

	patches = (struct rtlgen_hwpatch *)fw->data;
	count = fw->size / sizeof(*patches);

	patch_db = devm_kzalloc(dev, struct_size(patch_db, conf, count), GFP_KERNEL);
	if (!patch_db) {
		ret = -ENOMEM;
		goto out;
	}

	patch_db->count = count;
	patch_db->patch_op_exec = rtl826x_phy_patch_op;

	memcpy(patch_db->conf, patches, flex_array_size(patch_db, conf, count));

	priv->patch = patch_db;

out:
	release_firmware(fw);

	return ret;
}

int rtl8261n_phy_patch_db_init(struct phy_device *phydev)
{
	return rtl826x_phy_patch_db_init(phydev, "rtl8261n.bin");
}

int rtl8264b_phy_patch_db_init(struct phy_device *phydev)
{
	return rtl826x_phy_patch_db_init(phydev, "rtl8264b.bin");
}

MODULE_FIRMWARE("rtl8261n.bin");
MODULE_FIRMWARE("rtl8264b.bin");
