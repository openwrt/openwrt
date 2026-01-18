#include "realtek.h"
#include <linux/firmware.h>
#include <linux/byteorder/generic.h>

#define PHY_PATCH_WAIT_TIMEOUT	 10000000

static u16 _phy_rtl826x_mmd_convert(u16 page, u16 addr)
{
	if (addr < 16)
		return 0xA400 + (page * 2);

	if (addr < 24)
		return (16 * page) + ((addr - 16) * 2);

	return 0xA430 + ((addr - 24) * 2);
}

static int _phy_patch_rtl826x_wait(struct phy_device *phydev, u32 mmd_addr, u32 mmd_reg, u16 data, u16 mask)
{
	int ret = 0, us_diff;
	struct timespec64 start, now;

	ktime_get_real_ts64(&start);

	do {
		ret = phy_read_mmd(phydev, mmd_addr, mmd_reg);
		if (ret < 0)
			return ret;

		if ((ret & mask) == data)
			break;

		mdelay(1);

		ktime_get_real_ts64(&now);
		us_diff = (now.tv_sec - start.tv_sec) * USEC_PER_SEC +
				  (now.tv_nsec - start.tv_nsec) / NSEC_PER_USEC;
	} while (us_diff < PHY_PATCH_WAIT_TIMEOUT);

	if (us_diff > PHY_PATCH_WAIT_TIMEOUT) {
		phydev_err(phydev, "826XB patch wait[%u,0x%X,0x%X,0x%X]:0x%X\n", mmd_addr, mmd_reg, mask, data, ret);
		return -ETIME;
	}

	return 0;
}

static int _phy_patch_rtl826x_wait_not_equal(struct phy_device *phydev, u32 mmd_addr, u32 mmd_reg, u16 data, u16 mask)
{
	int ret = 0, us_diff;
	struct timespec64 start, now;

	ktime_get_real_ts64(&start);

	do {
		ret = phy_read_mmd(phydev, mmd_addr, mmd_reg);
		if (ret < 0)
			return ret;

		if ((ret & mask) != data)
			break;

		mdelay(1);

		ktime_get_real_ts64(&now);
		us_diff = (now.tv_sec - start.tv_sec) * USEC_PER_SEC +
				  (now.tv_nsec - start.tv_nsec) / NSEC_PER_USEC;
	} while (us_diff < PHY_PATCH_WAIT_TIMEOUT);

	if (us_diff > PHY_PATCH_WAIT_TIMEOUT) {
		phydev_err(phydev, "826xb patch wait[%u,0x%X,0x%X,0x%X]:0x%X\n", mmd_addr, mmd_reg, mask, data, ret);
		return -ETIME;
	}

	return 0;
}

static int _phy_patch_rtl826x_sds_get(struct phy_device *phydev, u32 sds_page, u32 sds_reg)
{
	int ret = 0;
	u32 sds_addr = 0x8000 + (sds_reg << 6) + sds_page;

	ret = phy_write_mmd(phydev, MDIO_MMD_VEND1, 0x143, sds_addr);
	if (ret < 0)
		return ret;

	ret = _phy_patch_rtl826x_wait(phydev, MDIO_MMD_VEND1, 0x143, 0, BIT(15));
	if (ret < 0)
		return ret;

	return phy_read_mmd(phydev, MDIO_MMD_VEND1, 0x142);
}

static int _phy_patch_rtl826x_sds_set(struct phy_device *phydev, u32 sds_page, u32 sds_reg, u32 data)
{
	int ret = 0;
	u32 sds_addr = 0x8800 + (sds_reg << 6) + sds_page;

	ret = phy_write_mmd(phydev, MDIO_MMD_VEND1, 0x141, data);
	if (ret < 0)
		return ret;

	ret = phy_write_mmd(phydev, MDIO_MMD_VEND1, 0x143, sds_addr);
	if (ret < 0)
		return ret;

	return _phy_patch_rtl826x_wait(phydev, MDIO_MMD_VEND1, 0x143, 0, BIT(15));
}

static int _phy_patch_rtl826x_sds_modify(struct phy_device *phydev, u32 sds_page, u32 sds_reg, u32 mask, u32 data)
{
	int ret = 0;

	if (mask != GENMASK(15, 0)) {
		ret = _phy_patch_rtl826x_sds_get(phydev, sds_page, sds_reg);
		if (ret < 0)
			return ret;
	}

	return _phy_patch_rtl826x_sds_set(phydev, sds_page, sds_reg, (ret & ~mask) | data);
}

static int phy_patch_rtl826x_op(struct phy_device *phydev, struct rtk_hwpatch *patch)
{
	int ret = 0, data, cnt;
	u16 reg = 0;
	u32 mask = GENMASK(patch->msb, patch->lsb);

	phydev_dbg(phydev, "patch: op: %u pm: %x mmd: %x addr: %x mask: [%u, %u] data: %x\n",
			   patch->patch_op, patch->portmask, patch->pagemmd, patch->addr, patch->msb,
			   patch->lsb, patch->data);

	switch (patch->patch_op) {
	case RTK_PATCH_OP_PHY:
		reg = _phy_rtl826x_mmd_convert(patch->pagemmd, patch->addr);
		if (mask != GENMASK(15, 0)) {
			ret = phy_modify_mmd(phydev, MDIO_MMD_VEND2, reg, mask, patch->data << patch->lsb);
			if (ret < 0)
				return ret;
		} else {
			ret = phy_write_mmd(phydev, MDIO_MMD_VEND2, reg, patch->data << patch->lsb);
			if (ret < 0)
				return ret;
		}
		break;

	case RTK_PATCH_OP_PHY_WAIT:
		reg = _phy_rtl826x_mmd_convert(patch->pagemmd, patch->addr);
		ret = _phy_patch_rtl826x_wait(phydev, MDIO_MMD_VEND2, reg, patch->data << patch->lsb, mask);
		if (ret < 0)
			return ret;
		break;

	case RTK_PATCH_OP_PHY_WAIT_NOT:
		reg = _phy_rtl826x_mmd_convert(patch->pagemmd, patch->addr);
		ret = _phy_patch_rtl826x_wait_not_equal(phydev, MDIO_MMD_VEND2, reg, patch->data << patch->lsb, mask);
		if (ret < 0)
			return ret;
		break;

	case RTK_PATCH_OP_PHYOCP:
		if (mask != GENMASK(15, 0)) {
			ret = phy_modify_mmd(phydev, MDIO_MMD_VEND2, patch->addr, mask, patch->data << patch->lsb);
			if (ret < 0)
				return ret;
		} else {
			ret = phy_write_mmd(phydev, MDIO_MMD_VEND2, patch->addr, patch->data << patch->lsb);
			if (ret < 0)
				return ret;
		}
		break;

	case RTK_PATCH_OP_PHYOCP_BC62:
		data = phy_read_mmd(phydev, MDIO_MMD_VEND2, 0xbc62);
		if (data < 0)
			return data;

		data = (data >> 8) & 0x1f;
		for (cnt = 0; cnt <= data; cnt++) {
			ret = phy_modify_mmd(phydev, MDIO_MMD_VEND2, 0xbc62, GENMASK(12, 8), cnt << 8);
			if (ret < 0)
				return ret;
		}
		break;

	case RTK_PATCH_OP_TOP:
		reg = (patch->pagemmd * 8) + (patch->addr - 16);
		if (mask != GENMASK(15, 0)) {
			ret = phy_modify_mmd(phydev, MDIO_MMD_VEND1, reg, mask, patch->data << patch->lsb);
			if (ret < 0)
				return ret;
		} else {
			ret = phy_write_mmd(phydev, MDIO_MMD_VEND1, reg, patch->data << patch->lsb);
			if (ret < 0)
				return ret;
		}
		break;

	case RTK_PATCH_OP_PSDS0:
		ret = _phy_patch_rtl826x_sds_modify(phydev, patch->pagemmd, patch->addr, mask, patch->data << patch->lsb);
		if (ret < 0)
			return ret;
		break;

	case RTK_PATCH_OP_DELAY_MS:
		mdelay(patch->data);
		break;

	default:
		phydev_err(phydev, "%s: %u not implemented yet!\n", __func__, patch->patch_op);
		return -EINVAL;
	}

	return ret;
}

int phy_patch_rtl8261n_db_init(struct phy_device *phydev)
{
	int ret = 0, i;
	struct device *dev = &phydev->mdio.dev;
	struct rtl826x_priv *priv = phydev->priv;
	struct rt_phy_patch_db *patch_db = NULL;
	const struct firmware *fw = NULL;
	struct rtk_hwpatch *patches;

	ret = request_firmware_direct(&fw, "rtl8261n.bin", dev);
	if (ret) {
		phydev_err(phydev, "Failed to request rtl8261n.bin\n");
		goto out;
	}
	patches = (struct rtk_hwpatch *) fw->data;

	patch_db = kmalloc(sizeof(struct rt_phy_patch_db), GFP_ATOMIC);
	if (!patch_db) {
		ret = -ENOMEM;
		goto out;
	}

	memset(patch_db, 0x0, sizeof(struct rt_phy_patch_db));

	patch_db->patch_op = phy_patch_rtl826x_op;

	patch_db->size = fw->size;
	if (priv->patch_rtk_serdes)
		patch_db->size += 2 * sizeof(struct rtk_hwpatch);

	patch_db->conf = kmalloc(patch_db->size, GFP_ATOMIC);
	if (!patch_db->conf) {
		ret = -ENOMEM;
		goto out;
	}

	for (i = 0; i < fw->size / sizeof(struct rtk_hwpatch); i++) {
		patch_db->conf[i].patch_op = patches[i].patch_op;
		patch_db->conf[i].portmask = patches[i].portmask;
		patch_db->conf[i].pagemmd = le16_to_cpu(patches[i].pagemmd);
		patch_db->conf[i].addr = le16_to_cpu(patches[i].addr);
		patch_db->conf[i].msb = patches[i].msb;
		patch_db->conf[i].lsb = patches[i].lsb;
		patch_db->conf[i].data = le16_to_cpu(patches[i].data);
	}

	if (priv->patch_rtk_serdes) {
		patch_db->conf[fw->size].patch_op = RTK_PATCH_OP_PSDS0;
		patch_db->conf[fw->size].portmask = 0xff;
		patch_db->conf[fw->size].pagemmd = 0x07;
		patch_db->conf[fw->size].addr = 0x10;
		patch_db->conf[fw->size].msb = 15;
		patch_db->conf[fw->size].lsb = 0;
		patch_db->conf[fw->size].data = 0x80aa;

		patch_db->conf[fw->size + 1].patch_op = RTK_PATCH_OP_PSDS0;
		patch_db->conf[fw->size + 1].portmask = 0xff;
		patch_db->conf[fw->size + 1].pagemmd = 0x06;
		patch_db->conf[fw->size + 1].addr = 0x12;
		patch_db->conf[fw->size + 1].msb = 15;
		patch_db->conf[fw->size + 1].lsb = 0;
		patch_db->conf[fw->size + 1].data = 0x5078;
	}

	priv->patch = patch_db;

out:
	if (fw && !IS_ERR(fw))
		release_firmware(fw);

	return ret;
}

int phy_patch_rtl8264b_db_init(struct phy_device *phydev)
{
	int ret = 0, i;
	struct device *dev = &phydev->mdio.dev;
	struct rtl826x_priv *priv = phydev->priv;
	struct rt_phy_patch_db *patch_db = NULL;
	const struct firmware *fw = NULL;
	struct rtk_hwpatch *patches;

	ret = request_firmware_direct(&fw, "rtl8264b.bin", dev);
	if (ret) {
		phydev_err(phydev, "Failed to request rtl8264b.bin\n");
		goto out;
	}
	patches = (struct rtk_hwpatch *) fw->data;

	patch_db = kmalloc(sizeof(struct rt_phy_patch_db), GFP_ATOMIC);
	if (!patch_db) {
		ret = -ENOMEM;
		goto out;
	}

	memset(patch_db, 0x0, sizeof(struct rt_phy_patch_db));

	patch_db->patch_op = phy_patch_rtl826x_op;

	patch_db->size = fw->size;
	if (priv->patch_rtk_serdes)
		patch_db->size += 2 * sizeof(struct rtk_hwpatch);

	patch_db->conf = kmalloc(patch_db->size, GFP_ATOMIC);
	if (!patch_db->conf) {
		ret = -ENOMEM;
		goto out;
	}

	for (i = 0; i < fw->size / sizeof(struct rtk_hwpatch); i++) {
		patch_db->conf[i].patch_op = patches[i].patch_op;
		patch_db->conf[i].portmask = patches[i].portmask;
		patch_db->conf[i].pagemmd = le16_to_cpu(patches[i].pagemmd);
		patch_db->conf[i].addr = le16_to_cpu(patches[i].addr);
		patch_db->conf[i].msb = patches[i].msb;
		patch_db->conf[i].lsb = patches[i].lsb;
		patch_db->conf[i].data = le16_to_cpu(patches[i].data);
	}

	if (priv->patch_rtk_serdes) {
		patch_db->conf[fw->size].patch_op = RTK_PATCH_OP_PSDS0;
		patch_db->conf[fw->size].portmask = 0xff;
		patch_db->conf[fw->size].pagemmd = 0x07;
		patch_db->conf[fw->size].addr = 0x10;
		patch_db->conf[fw->size].msb = 15;
		patch_db->conf[fw->size].lsb = 0;
		patch_db->conf[fw->size].data = 0x80aa;

		patch_db->conf[fw->size + 1].patch_op = RTK_PATCH_OP_PSDS0;
		patch_db->conf[fw->size + 1].portmask = 0xff;
		patch_db->conf[fw->size + 1].pagemmd = 0x06;
		patch_db->conf[fw->size + 1].addr = 0x12;
		patch_db->conf[fw->size + 1].msb = 15;
		patch_db->conf[fw->size + 1].lsb = 0;
		patch_db->conf[fw->size + 1].data = 0x5078;
	}

	priv->patch = patch_db;

out:
	if (fw && !IS_ERR(fw))
		release_firmware(fw);

	return ret;
}

MODULE_FIRMWARE("rtl8261n.bin");
MODULE_FIRMWARE("rtl8264b.bin");