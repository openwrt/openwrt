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

#ifndef _MAC_AX_HCI_FC_8852C_H_
#define _MAC_AX_HCI_FC_8852C_H_

#include "../../mac_def.h"
#include "../mac_priv.h"

#if MAC_AX_8852C_SUPPORT

/**
 * @brief set_fc_page_ctrl_reg_8852c
 *
 * @param *adapter
 * @param ch
 * @return Please Place Description here.
 * @retval u32
 */
u32 set_fc_page_ctrl_reg_8852c(struct mac_ax_adapter *adapter, u8 ch);
/**
 * @}
 * @}
 */

/**
 * @brief get_fc_page_info_8852c
 *
 * @param *adapter
 * @param ch
 * @return Please Place Description here.
 * @retval u32
 */
u32 get_fc_page_info_8852c(struct mac_ax_adapter *adapter, u8 ch);
/**
 * @}
 * @}
 */

/**
 * @brief set_fc_pubpg_8852c
 *
 * @param *adapter
 * @return Please Place Description here.
 * @retval u32
 */
u32 set_fc_pubpg_8852c(struct mac_ax_adapter *adapter);
/**
 * @}
 * @}
 */

/**
 * @brief get_fc_mix_info_8852c
 *
 * @param *adapter
 * @return Please Place Description here.
 * @retval u32
 */
u32 get_fc_mix_info_8852c(struct mac_ax_adapter *adapter);
/**
 * @}
 * @}
 */

/**
 * @brief set_fc_h2c_8852c
 *
 * @param *adapter
 * @return Please Place Description here.
 * @retval u32
 */
u32 set_fc_h2c_8852c(struct mac_ax_adapter *adapter);
/**
 * @}
 * @}
 */

/**
 * @brief set_fc_mix_cfg_8852c
 *
 * @param *adapter
 * @return Please Place Description here.
 * @retval u32
 */
u32 set_fc_mix_cfg_8852c(struct mac_ax_adapter *adapter);
/**
 * @}
 * @}
 */

/**
 * @brief set_fc_func_en_8852c
 *
 * @param *adapter
 * @param en
 * @param h2c_en
 * @return Please Place Description here.
 * @retval u32
 */
u32 set_fc_func_en_8852c(struct mac_ax_adapter *adapter, u8 en, u8 h2c_en);
/**
 * @}
 * @}
 */

#endif /* #if MAC_AX_8852C_SUPPORT */
#endif
