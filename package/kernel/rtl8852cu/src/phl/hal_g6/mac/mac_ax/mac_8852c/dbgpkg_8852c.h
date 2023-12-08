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

#ifndef _MAC_AX_DBGPKG_8852C_H_
#define _MAC_AX_DBGPKG_8852C_H_

#include "../../mac_def.h"

#if MAC_AX_8852C_SUPPORT

/**
 * @brief tx_dbg_dump_8852c
 * mac tx_dbg_dump_8852c
 * @param *adapter
 * @return Please Place Description here.
 * @retval u32
 */
u32 tx_dbg_dump_8852c(struct mac_ax_adapter *adapter);
/**
 * @}
 * @}
 */

/**
 * @brief crit_dbg_dump_8852c
 * mac crit_dbg_dump_8852c
 * @param *adapter
 * @return Please Place Description here.
 * @retval u32
 */
u32 crit_dbg_dump_8852c(struct mac_ax_adapter *adapter);
/**
 * @}
 * @}
 */

/**
 * @brief cmac_dbg_dump_8852c
 * mac cmac_dbg_dump_8852c
 * @param *adapter
 * @param *band
 * @return Please Place Description here.
 * @retval u32
 */
u32 cmac_dbg_dump_8852c(struct mac_ax_adapter *adapter, enum mac_ax_band band);
/**
 * @}
 * @}
 */

/**
 * @brief dmac_dbg_dump_8852c
 * mac dmac_dbg_dump_8852c
 * @param *adapter
 * @return Please Place Description here.
 * @retval u32
 */
u32 dmac_dbg_dump_8852c(struct mac_ax_adapter *adapter);
/**
 * @}
 * @}
 */

/**
 * @brief dbg_port_sel_8852c
 * mac debug port sel 8852c
 * @param *adapter
 * @param *mac_ax_dbg_port_info
 * @param *sel
 * @return Please Place Description here.
 * @retval u32
 */
u32 dbg_port_sel_8852c(struct mac_ax_adapter *adapter,
		       struct mac_ax_dbg_port_info **info, u32 sel);
/**
 * @}
 * @}
 */

/**
 * @brief dbg_port_sel_rst_8852c
 * mac debug port sel rst 8852c
 * @param *adapter
 * @param sel
 * @return Please Place Description here.
 * @retval u32
 */
u32 dbg_port_sel_rst_8852c(struct mac_ax_adapter *adapter, u32 sel);
/**
 * @}
 * @}
 */

/**
 * @brief tx_flow_ptcl_dbg_port_8852c
 * mac tx_flow_ptcl_dbg_port_8852c
 * @param *adapter
 * @param u8 band
 * @return Please Place Description here.
 * @retval u32
 */
u32 tx_flow_ptcl_dbg_port_8852c(struct mac_ax_adapter *adapter, u8 band);
/**
 * @}
 * @}
 */

/**
 * @brief tx_flow_sch_dbg_port_8852c
 * mac tx_flow_sch_dbg_port_8852c
 * @param *adapter
 * @param u8 band
 * @return Please Place Description here.
 * @retval u32
 */
u32 tx_flow_sch_dbg_port_8852c(struct mac_ax_adapter *adapter, u8 band);
/**
 * @}
 * @}
 */

/**
 * @brief ss_stat_chk_8852c
 * mac ss_stat_chk
 * @param *adapter
 * @return Please Place Description here.
 * @retval u32
 */
u32 ss_stat_chk_8852c(struct mac_ax_adapter *adapter);
/**
 * @}
 * @}
 */

/**
 * @brief is_dbg_port_not_valid_8852c
 *
 * @param *adapter
 * @param dbg_sel
 * @return Please Place Description here.
 * @retval u8
 */
u8 is_dbg_port_not_valid_8852c(struct mac_ax_adapter *adapter, u32 dbg_sel);
/**
 * @}
 * @}
 */

#endif /* MAC_AX_8852C_SUPPORT */
#endif /* _MAC_AX_DBGPKG_8852C_H_ */
