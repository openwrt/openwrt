/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation.
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
 *****************************************************************************/
#ifndef _HAL_HEADERS_H_
#define _HAL_HEADERS_H_
#include "../rtw_general_def.h"
#include "../phl_list.h"
#include "../phl_status.h"
#include "../pltfm_ops.h"
#include "../phl_config.h"
#include "../phl_types.h"
#include "../phl_util.h"
#include "../phl_def.h"
#include "../phl_debug.h"
#include "../phl_trx_def.h"
#include "../phl_wow_def.h"
#include "../phl_p2pps_def.h"
#include "../phl_btc_def.h"
#include "../phl_test_def.h"
#include "../phl_ser_def.h"
#include "../phl_regulation_def.h"
#include "../phl_led_def.h"
#include "../phl_acs_def.h"
#include "../phl_ext_tx_pwr_lmt_def.h"
#include "../custom/phl_custom_def.h"
#include "../custom/phl_custom_api.h"

#ifdef CONFIG_PCI_HCI
#include "../hci/phl_trx_def_pcie.h"
#endif
#ifdef CONFIG_USB_HCI
#include "../hci/phl_trx_def_usb.h"
#endif
#ifdef CONFIG_SDIO_HCI
#include "../hci/phl_trx_def_sdio.h"
#endif

#include "hal_general_def.h"
#include "hal_config.h"
#include "hal_def.h"
#include "phy/bb/halbb_outsrc_def.h"
#include "mac/mac_exp_def.h"
#include "mac/mac_outsrc_def.h"
#include "../phl_api_drv.h"
#include "hal_struct.h"
#include "hal_io.h"

#ifdef CONFIG_PCI_HCI
#include "hal_pci.h"
#endif

#include "hal_api_mac.h"
#include "hal_api_bb.h"
#include "hal_api_rf.h"
#include "hal_api_btc.h"
#include "hal_api_efuse.h"
#include "hal_str_proc.h"

#include "hal_tx.h"
#include "hal_rx.h"
#include "hal_sta.h"
#include "hal_cap.h"
#include "hal_chan.h"
#ifdef CONFIG_PHL_CHANNEL_INFO
#include "hal_chan_info.h"
#endif /* CONFIG_PHL_CHANNEL_INFO */
#include "hal_fw.h"
#include "hal_wow.h"

#include "hal_csi_buffer.h"
#include "hal_beamform.h"
#include "hal_sound.h"
#include "hal_ser.h"
#include "hal_c2h.h"
#include "hal_acs.h"
#include "hal_mcc_def.h"
#include "hal_ld_file.h"
#include "hal_notify.h"
#include "hal_ps.h"
#include "hal_custom.h"
#include "hal_txpwr.h"
#include "hal_com_i.h"
#include "hal_dfs.h"
#ifdef CONFIG_DBCC_SUPPORT
#include "hal_dbcc.h"
#endif

#define hal_to_drvpriv(_halinfo) (_halinfo->hal_com->drv_priv)

#endif /*_HAL_HEADERS_H_*/
