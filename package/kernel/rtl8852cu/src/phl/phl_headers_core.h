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
#ifndef _PHL_HEADERS_CORE_H_
#define _PHL_HEADERS_CORE_H_

#ifndef PHL_PLATFORM_LINUX
/* Common definition from PHL */
#include "rtw_general_def.h"
#include "phl_config.h"
#include "phl_list.h"
#include "phl_status.h"
#include "phl_types.h"
#include "pltfm_ops.h"
#else
#include "phl_types.h"
#include "phl_config.h"
#include "phl_status.h"
#endif

/* Exported structure/definition from PHL */
#include "phl_util.h"
#include "phl_regulation_def.h"
#include "phl_chnlplan.h"
#include "phl_country.h"
#include "phl_scan_instance.h"
#include "phl_def.h"
#include "hal_g6/mac/mac_exp_def.h"
#include "phl_trx_def.h"
#include "phl_wow_def.h"
#include "phl_ie.h"
#ifdef CONFIG_PHL_CHANNEL_INFO
#include "phl_chan_info_def.h"
#endif

#include "phl_btc_def.h"
#include "phl_test_def.h"
#include "test/trx_test.h"
#include "test/cmd_disp_test.h"
#include "phl_led_def.h"
#include "phl_acs_def.h"
#include "custom/phl_custom_def.h"
#include "phl_ext_tx_pwr_lmt_def.h"

/* Exported APIs from PHL */
#include "phl_api.h"
#include "phl_scan.h"
#ifdef CONFIG_FSM
#include "phl_cmd_job.h"
#endif
#include "phl_connect.h"
#include "phl_ecsa_export.h"
#include "custom/phl_custom_api.h"

#endif /*_PHL_HEADERS_CORE_H_*/
