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
#ifndef _PHL_HEADERS_H_
#define _PHL_HEADERS_H_

/*
 * Basic components
 */
#include "rtw_general_def.h"
#include "phl_list.h"
#include "phl_status.h"
#include "pltfm_ops.h"
#include "phl_config.h"
#include "hal_g6/hal_config.h"
#include "phl_types.h"
#include "phl_util.h"
#include "phl_def.h"
#include "hal_g6/mac/mac_exp_def.h"
#include "phl_debug.h"
/*
 * PHL Feature headers start
 */
#include "phl_ie.h"
#include "phl_regulation_def.h"
#include "phl_chnlplan.h"
#include "phl_country.h"
#include "phl_scan_instance.h"
#include "phl_regulation.h"
#include "phl_trx_def.h"
#include "phl_wow_def.h"
#include "phl_btc_def.h"
#include "phl_p2pps_def.h"
#include "phl_cmd_dispatch.h"
#include "phl_watchdog.h"
#include "phl_chan.h"
#include "phl_role.h"
#include "phl_wow.h"
#include "phl_struct.h"
#include "phl_mcc_def.h"
#include "phl_test_def.h"
#include "phl_test.h"
#include "phl_msg_hub.h"
#include "custom/phl_custom_def.h"
#include "custom/phl_custom_api.h"
#include "custom/phl_custom.h"
#include "phl_sw_cap.h"
#include "phl_sta.h"
#include "phl_mr.h"
#include "phl_mr_coex.h"
#include "test/trx_test.h"
#include "test/cmd_disp_test.h"
#include "test/phl_ps_dbg_cmd.h"
#include "test/phl_ser_dbg_cmd.h"
#include "phl_tx.h"
#include "phl_rx_agg.h"
#include "phl_rx.h"

#include "phl_ser_def.h"
#include "phl_sound_cmd.h"
#include "phl_sound.h"

#include "phl_ps.h"
#include "phl_scan.h"

#ifdef CONFIG_FSM
#include "phl_fsm.h"
#include "phl_cmd_fsm.h"
#include "phl_cmd_job.h"
#include "phl_ser_fsm.h"
#endif /*CONFIG_FSM*/

#include "phl_cmd_ps.h"
#include "phl_sound_cmd.h"
#include "phl_cmd_ser.h"

#include "phl_pkt_ofld.h"
#include "test/phl_dbg_cmd.h"
#include "phl_chan.h"
#include "phl_acs_def.h"
#include "phl_acs.h"
#include "phl_led_def.h"
#include "phl_led.h"
#include "phl_trx_mit.h"
#include "phl_dm.h"
#include "phl_notify.h"
#include "phl_cmd_general.h"
#include "phl_p2pps.h"
#include "phl_cmd_btc.h"
#include "phl_twt.h"
#include "phl_ecsa_export.h"
#include "phl_ecsa.h"
#include "phl_thermal.h"
#include "phl_txpwr.h"
#include "phl_ext_tx_pwr_lmt_def.h"
#include "phl_dfs.h"

#ifdef CONFIG_PHL_CHANNEL_INFO
#include "phl_chan_info_def.h"
#include "phl_chan_info.h"
#endif /* CONFIG_PHL_CHANNEL_INFO */

#ifdef CONFIG_PCI_HCI
#include "hci/phl_trx_def_pcie.h"
#include "hci/phl_trx_pcie.h"
#endif
#ifdef CONFIG_USB_HCI
#include "hci/phl_trx_def_usb.h"
#include "hci/phl_trx_usb.h"
#endif
#ifdef CONFIG_SDIO_HCI
#include "hci/phl_trx_def_sdio.h"
#include "hci/phl_trx_sdio.h"
#endif


/******************************************************************************
 * Driver layer shall pass wifi configuration flag to PHL for feature category
 * comment it temporarily.
 *****************************************************************************/
#ifdef CONFIG_WIFI_5
#endif

#ifdef CONFIG_WIFI_6
#include "hal_g6/hal_general_def.h"
#include "hal_g6/hal_def.h"
#include "hal_g6/hal_api.h"
#include "hal_g6/hal_scanofld.h"
#else
/*temporarily*/
#include "hal_g6/hal_general_def.h"
#include "hal_g6/hal_def.h"
#include "hal_g6/hal_api.h"
#include "hal_g6/hal_scanofld.h"
#endif

#ifdef CONFIG_WIFI_7
#endif

#endif /*_PHL_HEADERS_H_*/
