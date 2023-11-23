/** @file */
/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 ******************************************************************************/
#ifndef _MAC_AX_HDR_CONV_RX_8852C_H_
#define _MAC_AX_HDR_CONV_RX_8852C_H_

#include "../../type.h"
#include "../../mac_ax.h"
#include "../../mac_def.h"

/**
 * @brief enable/disable RX MAC Header Conversion
 *
 * @param adapter pointer of mac ax adapter.
 * @param cfg confihuration parameter.
 * @return result of configuration.
 * @retval u32
 */
u32 mac_hdr_conv_rx_en_8852c(struct mac_ax_adapter *adapter,
			     struct mac_ax_rx_hdr_conv_cfg *cfg);

/**
 * @brief enable/disable driver info of RX MAC Header Conversion
 *
 * @param adapter pointer of mac ax adapter.
 * @param cfg confihuration parameter.
 * @return result of configuration.
 * @retval u32
 */
u32 mac_hdr_conv_rx_en_driv_info_hdr_8852c(struct mac_ax_adapter *adapter,
					   struct mac_ax_rx_driv_info_hdr_cfg *cfg);
#endif
