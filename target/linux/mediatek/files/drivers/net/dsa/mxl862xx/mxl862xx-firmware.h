/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Firmware loading support for MaxLinear MxL862xx switches
 *
 * Copyright (C) 2024 MaxLinear Inc.
 * Copyright (C) 2025 Chad Monroe <chad@monroe.io>
 */

#ifndef _MXL862XX_FIRMWARE_H
#define _MXL862XX_FIRMWARE_H

struct mxl862xx_priv;

#define MXL862XX_SB_PDI_CTRL		0xE100
#define MXL862XX_SB_PDI_ADDR		0xE101
#define MXL862XX_SB_PDI_DATA		0xE102
#define MXL862XX_SB_PDI_STAT		0xE103

#define MXL862XX_SB_PDI_CTRL_RST	0x00
#define MXL862XX_SB_PDI_CTRL_RD		0x01
#define MXL862XX_SB_PDI_CTRL_WR		0x02

#define MXL862XX_SB1_ADDR		0x7800

#define MXL862XX_FW_DL_RDY_MAGIC	0xC55C
#define MXL862XX_FW_DL_START_MAGIC	0xF48F
#define MXL862XX_FW_DL_END_MAGIC	0x3CC3

#define MXL862XX_FW_HEADER_SIZE		20

#define MXL862XX_SB_WORDS_HALF		16384
#define MXL862XX_SB_WORDS_SLICE		32760

#define MXL862XX_FW_READY_TIMEOUT_MS	3000
#define MXL862XX_FW_ACK_TIMEOUT_MS	5000
#define MXL862XX_FW_ERASE_TIMEOUT_MS	70000
#define MXL862XX_FW_SLICE_TIMEOUT_MS	70000

struct mxl862xx_fw_header {
	__le32 image_type;
	__le32 image_size_1;
	__le32 image_checksum_1;
	__le32 image_size_2;
	__le32 image_checksum_2;
} __packed;

int mxl862xx_firmware_load(struct mxl862xx_priv *priv);

#endif /* _MXL862XX_FIRMWARE_H */
