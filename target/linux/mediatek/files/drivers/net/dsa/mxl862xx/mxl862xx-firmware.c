// SPDX-License-Identifier: GPL-2.0
/*
 * Firmware loading support for MaxLinear MxL862xx switches
 *
 * Copyright (C) 2024 MaxLinear Inc.
 * Copyright (C) 2025 Chad Monroe <chad@monroe.io>
 */

#include <linux/firmware.h>
#include <linux/of.h>
#include <linux/delay.h>
#include <linux/phy.h>
#include <net/dsa.h>

#include "mxl862xx.h"
#include "mxl862xx-host.h"
#include "mxl862xx-api.h"
#include "mxl862xx-cmd.h"
#include "mxl862xx-firmware.h"

static void mxl862xx_pdi_reset(struct mxl862xx_priv *priv)
{
	mxl862xx_smdio_write(priv, MXL862XX_SB_PDI_CTRL, MXL862XX_SB_PDI_CTRL_RST);
	mxl862xx_smdio_write(priv, MXL862XX_SB_PDI_ADDR, MXL862XX_SB_PDI_CTRL_RST);
	mxl862xx_smdio_write(priv, MXL862XX_SB_PDI_DATA, MXL862XX_SB_PDI_CTRL_RST);
}

static int mxl862xx_pdi_wait_stat(struct mxl862xx_priv *priv, u16 expected,
				  unsigned long timeout_ms)
{
	unsigned long timeout = jiffies + msecs_to_jiffies(timeout_ms);
	int val;

	do {
		val = mxl862xx_smdio_read(priv, MXL862XX_SB_PDI_STAT);
		if (val < 0)
			return val;
		if ((u16)val == expected)
			return 0;
		usleep_range(10000, 11000);
	} while (time_before(jiffies, timeout));

	return -ETIMEDOUT;
}

static int mxl862xx_fw_version_read(struct mxl862xx_priv *priv,
				    struct mxl862xx_sys_fw_image_version *ver)
{
	return mxl862xx_api_wrap(priv, SYS_MISC_FW_VERSION, ver,
				 sizeof(*ver), true);
}

static bool mxl862xx_fw_version_matches(struct mxl862xx_priv *priv,
					u8 file_major, u8 file_minor,
					u16 file_revision)
{
	struct mxl862xx_sys_fw_image_version ver = {};
	int ret;

	ret = mxl862xx_fw_version_read(priv, &ver);
	if (ret) {
		dev_warn(priv->dev, "cannot read firmware version\n");
		return false;
	}

	dev_dbg(priv->dev, "running firmware %u.%u.%u (build %u)\n",
		 ver.iv_major, ver.iv_minor,
		 le16_to_cpu(ver.iv_revision),
		 le32_to_cpu(ver.iv_build_num));

	return ver.iv_major == file_major &&
	       ver.iv_minor == file_minor &&
	       le16_to_cpu(ver.iv_revision) == file_revision;
}

static int mxl862xx_fw_update_mode(struct mxl862xx_priv *priv)
{
	return mxl862xx_api_wrap(priv, SYS_MISC_FW_UPDATE, NULL, 0, false);
}

static int mxl862xx_fw_download(struct mxl862xx_priv *priv,
				const u8 *data, size_t size)
{
	const struct mxl862xx_fw_header *hdr;
	const u8 *fw_data;
	u32 full_size, idx = 0, data_written = 0;
	u32 word_idx = 0;
	int ret, i;

	if (size < MXL862XX_FW_HEADER_SIZE)
		return -EINVAL;

	hdr = (const struct mxl862xx_fw_header *)data;
	fw_data = data + MXL862XX_FW_HEADER_SIZE;
	full_size = le32_to_cpu(hdr->image_size_1) + le32_to_cpu(hdr->image_size_2);

	dev_dbg(priv->dev, "fw header: type=0x%08x size1=%u csum1=0x%08x size2=%u csum2=0x%08x\n",
		le32_to_cpu(hdr->image_type),
		le32_to_cpu(hdr->image_size_1),
		le32_to_cpu(hdr->image_checksum_1),
		le32_to_cpu(hdr->image_size_2),
		le32_to_cpu(hdr->image_checksum_2));

	if (full_size > size - MXL862XX_FW_HEADER_SIZE) {
		dev_err(priv->dev, "firmware file too small for declared size\n");
		return -EINVAL;
	}

	mutex_lock(&priv->bus->mdio_lock);

	mxl862xx_pdi_reset(priv);

	ret = mxl862xx_pdi_wait_stat(priv, MXL862XX_FW_DL_RDY_MAGIC,
				     MXL862XX_FW_READY_TIMEOUT_MS);
	if (ret) {
		dev_err(priv->dev, "target not ready for firmware download\n");
		goto unlock;
	}

	mxl862xx_smdio_write(priv, MXL862XX_SB_PDI_STAT, MXL862XX_FW_DL_START_MAGIC);

	ret = mxl862xx_pdi_wait_stat(priv, MXL862XX_FW_DL_START_MAGIC + 1,
				     MXL862XX_FW_ACK_TIMEOUT_MS);
	if (ret) {
		dev_err(priv->dev, "no start ACK from target\n");
		goto unlock;
	}

	mxl862xx_smdio_write(priv, MXL862XX_SB_PDI_CTRL, MXL862XX_SB_PDI_CTRL_WR);
	for (i = 0; i < 10; i++) {
		u16 word = data[i * 2] | (data[i * 2 + 1] << 8);

		mxl862xx_smdio_write(priv, MXL862XX_SB_PDI_DATA, word);
	}
	mxl862xx_pdi_reset(priv);
	mxl862xx_smdio_write(priv, MXL862XX_SB_PDI_STAT, MXL862XX_FW_HEADER_SIZE);

	ret = mxl862xx_pdi_wait_stat(priv, MXL862XX_FW_HEADER_SIZE + 1,
				     MXL862XX_FW_ACK_TIMEOUT_MS);
	if (ret) {
		dev_err(priv->dev, "no header ACK from target\n");
		goto unlock;
	}

	dev_info(priv->dev, "erasing flash...\n");
	ret = mxl862xx_pdi_wait_stat(priv, 0, MXL862XX_FW_ERASE_TIMEOUT_MS);
	if (ret) {
		dev_err(priv->dev, "flash erase timeout\n");
		goto unlock;
	}

	dev_info(priv->dev, "programming flash (%u bytes)...\n", full_size);
	mxl862xx_smdio_write(priv, MXL862XX_SB_PDI_CTRL, MXL862XX_SB_PDI_CTRL_WR);

	while (idx < full_size) {
		u16 fdata;

		if (idx + 1 < full_size) {
			fdata = fw_data[idx] | (fw_data[idx + 1] << 8);
			idx += 2;
			data_written += 2;
		} else {
			fdata = fw_data[idx];
			idx++;
			data_written++;
		}

		mxl862xx_smdio_write(priv, MXL862XX_SB_PDI_DATA, fdata);
		word_idx++;

		if (idx >= full_size) {
			mxl862xx_pdi_reset(priv);
			mxl862xx_smdio_write(priv, MXL862XX_SB_PDI_STAT, data_written);
			break;
		}

		if (word_idx == MXL862XX_SB_WORDS_HALF) {
			mxl862xx_smdio_write(priv, MXL862XX_SB_PDI_CTRL,
					     MXL862XX_SB_PDI_CTRL_RST);
			mxl862xx_smdio_write(priv, MXL862XX_SB_PDI_ADDR,
					     MXL862XX_SB1_ADDR);
			mxl862xx_smdio_write(priv, MXL862XX_SB_PDI_CTRL,
					     MXL862XX_SB_PDI_CTRL_WR);
		} else if (word_idx >= MXL862XX_SB_WORDS_SLICE) {
			mxl862xx_pdi_reset(priv);
			dev_dbg(priv->dev, "slice complete: wrote %u bytes, idx=%u\n",
				data_written, idx);
			mxl862xx_smdio_write(priv, MXL862XX_SB_PDI_STAT, data_written);
			word_idx = 0;
			data_written = 0;

			ret = mxl862xx_pdi_wait_stat(priv, 0,
						     MXL862XX_FW_SLICE_TIMEOUT_MS);
			if (ret) {
				dev_err(priv->dev, "slice program timeout\n");
				goto unlock;
			}
			mxl862xx_smdio_write(priv, MXL862XX_SB_PDI_CTRL,
					     MXL862XX_SB_PDI_CTRL_WR);
		}
	}

	dev_dbg(priv->dev, "final chunk: waiting for STAT=0 (wrote %u bytes)\n",
		data_written);
	ret = mxl862xx_pdi_wait_stat(priv, 0, MXL862XX_FW_SLICE_TIMEOUT_MS);
	if (ret) {
		dev_err(priv->dev, "final chunk program timeout\n");
		goto unlock;
	}

	dev_dbg(priv->dev, "sending end magic 0x%04x\n", MXL862XX_FW_DL_END_MAGIC);
	mxl862xx_smdio_write(priv, MXL862XX_SB_PDI_STAT, MXL862XX_FW_DL_END_MAGIC);
	dev_info(priv->dev, "firmware download complete\n");

	mutex_unlock(&priv->bus->mdio_lock);
	return 0;

unlock:
	mxl862xx_pdi_reset(priv);
	mutex_unlock(&priv->bus->mdio_lock);
	return ret;
}

int mxl862xx_firmware_load(struct mxl862xx_priv *priv)
{
	const struct firmware *fw;
	const char *fw_name;
	u32 fw_version[3];
	int ret;

	ret = of_property_read_string(priv->dev->of_node, "firmware-name", &fw_name);
	if (ret) {
		dev_dbg(priv->dev, "no firmware-name property, skipping firmware load\n");
		return 0;
	}

	ret = of_property_read_u32_array(priv->dev->of_node, "firmware-version",
					 fw_version, 3);
	if (ret) {
		dev_warn(priv->dev, "no firmware-version property, skipping firmware load\n");
		return 0;
	}

	dev_info(priv->dev, "expected firmware version: %u.%u.%u\n",
		 fw_version[0], fw_version[1], fw_version[2]);

	if (mxl862xx_fw_version_matches(priv, fw_version[0], fw_version[1],
					fw_version[2])) {
		dev_dbg(priv->dev, "firmware version matches, skipping update\n");
		return 0;
	}

	ret = request_firmware(&fw, fw_name, priv->dev);
	if (ret) {
		dev_err(priv->dev, "failed to load firmware '%s': %d\n",
			fw_name, ret);
		return ret;
	}

	dev_info(priv->dev, "loading firmware '%s' (%zu bytes)\n",
		 fw_name, fw->size);

	dev_info(priv->dev, "entering firmware update mode\n");
	ret = mxl862xx_fw_update_mode(priv);
	if (ret) {
		dev_err(priv->dev, "failed to enter update mode: %d\n", ret);
		release_firmware(fw);
		return ret;
	}

	ret = mxl862xx_fw_download(priv, fw->data, fw->size);
	release_firmware(fw);

	if (ret)
		return ret;

	msleep(5000);

	return 0;
}
