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

#ifndef _MAC_AX_RX_FILTER_H_
#define _MAC_AX_RX_FILTER_H_
#include "trxcfg.h"
#include "../type.h"

#define RX_FLTR_BCN_CHK_RULE_SH		8
#define RX_FLTR_BCN_CHK_RULE_MSK	0x3
#define RX_FLTR_PKT_LEN_FLTR_SH		16
#define RX_FLTR_PKT_LEN_FLTR_MSK	0x7F
#define RX_FLTR_UNSUPPORT_PKT_FLTR_SH	22
#define RX_FLTR_UNSUPPORT_PKT_FLTR_MSK	0x3
#define RX_FLTR_UID_FLTR_SH		24
#define RX_FLTR_UID_FLTR_MSK		0xFF

#define RX_FLTR_FRAME_DROP	0x00000000
#define RX_FLTR_FRAME_TO_HOST	0x55555555
#define RX_FLTR_FRAME_TO_WLCPU	0xAAAAAAAA

#define RX_FLTR_SUBTYPE_NUM	16
#define ENTRY_MSK	0x3
#define ENTRY_SH(_idx)	(_idx * 2)
#define ENTRY_IDX	1

/**
 * @addtogroup Basic_TRX
 * @{
 * @addtogroup RX_Filter
 * @{
 */

/**
 * @brief mac_get_rx_fltr_opt
 *
 * @param *adapter
 * @param *fltr_opt
 * @param band
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_get_rx_fltr_opt(struct mac_ax_adapter *adapter,
			struct mac_ax_rx_fltr_ctrl_t *fltr_opt,
			enum mac_ax_band band);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Basic_TRX
 * @{
 * @addtogroup RX_Filter
 * @{
 */

/**
 * @brief mac_set_rx_fltr_opt
 *
 * @param *adapter
 * @param *fltr_opt
 * @param *fltr_opt_msk
 * @param band
 * @return Please Place Description here.
 * @retval u32
 */

u32 mac_set_rx_fltr_opt(struct mac_ax_adapter *adapter,
			struct mac_ax_rx_fltr_ctrl_t *fltr_opt,
			struct mac_ax_rx_fltr_ctrl_t *fltr_opt_msk,
			enum mac_ax_band band);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Basic_TRX
 * @{
 * @addtogroup RX_Filter
 * @{
 */

/**
 * @brief mac_set_typ_fltr_opt
 *
 * @param *adapter
 * @param type
 * @param fwd_target
 * @param band
 * @return Please Place Description here.
 * @retval u32
 */

u32 mac_set_typ_fltr_opt(struct mac_ax_adapter *adapter,
			 enum mac_ax_pkt_t type,
			 enum mac_ax_fwd_target fwd_target,
			 enum mac_ax_band band);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Basic_TRX
 * @{
 * @addtogroup RX_Filter
 * @{
 */

/**
 * @brief mac_set_typsbtyp_fltr_opt
 *
 * @param *adapter
 * @param type
 * @param subtype
 * @param fwd_target
 * @param band
 * @return Please Place Description here.
 * @retval u32
 */

u32 mac_set_typsbtyp_fltr_opt(struct mac_ax_adapter *adapter,
			      enum mac_ax_pkt_t type,
			      u8 subtype,
			      enum mac_ax_fwd_target fwd_target,
			      enum mac_ax_band band);
/**
 * @}
 * @}
 */

u32 rx_fltr_init(struct mac_ax_adapter *adapter,
		 enum mac_ax_band band);

/**
 * @brief mac_set_typ_fltr_opt
 *
 * @param *adapter
 * @param type
 * @param fwd_target
 * @param band
 * @return Please Place Description here.
 * @retval u32
 */

u32 mac_set_typsbtyp_fltr_detail(struct mac_ax_adapter *adapter,
				 enum mac_ax_pkt_t type,
				 struct mac_ax_rx_fltr_elem *elem,
				 enum mac_ax_band band);
/**
 * @}
 * @}
 */
#endif
