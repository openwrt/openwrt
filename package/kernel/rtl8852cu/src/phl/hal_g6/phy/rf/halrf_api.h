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
#ifndef _HALRF_API_H_
#define _HALRF_API_H_
/*@--------------------------[Define] ---------------------------------------*/
#define HALRF_ABS(a,b) ((a>b) ? (a-b) : (b-a))
/*@--------------------------[Enum]------------------------------------------*/
enum phlrf_lna_set {
	PHLRF_LNA_DISABLE = 0,
	PHLRF_LNA_ENABLE = 1,
};

enum halrf_rfk_type {
	RF_BTC_IQK		= 0,
	RF_BTC_LCK		= 1,
	RF_BTC_DPK		= 2,
	RF_BTC_TXGAPK		= 3,
	RF_BTC_DACK		= 4,
	RF_BTC_RXDCK		= 5,
	RF_BTC_TSSI		= 6,
	RF_BTC_CHLK		= 7
};

enum halrf_rfk_process {
	RFK_STOP		= 0,
	RFK_START		= 1,
	RFK_ONESHOT_START	= 2,
	RFK_ONESHOT_STOP	= 3
};

enum adc_ck {
	ADC_NA	= 0,
	ADC_480M	= 1,
	ADC_960M	= 2,
	ADC_1920M	= 3,
};

enum dac_ck {
	DAC_40M	= 0,
	DAC_80M	= 1,
	DAC_120M	= 2,
	DAC_160M	= 3,
	DAC_240M	= 4,
	DAC_320M	= 5,
	DAC_480M	= 6,
	DAC_960M	= 7,
};

enum halrf_event_idx {
	RF_EVENT_PWR_TRK = 0,
	RF_EVENT_IQK = 1,
	RF_EVENT_DPK = 2,
	RF_EVENT_TXGAPK = 3,
	RF_EVENT_DACK = 4
};

enum halrf_event_func {
	RF_EVENT_OFF = 0,
	RF_EVENT_ON = 1,
	RF_EVENT_TRIGGER = 2
};


/*@--------------------------[Structure]-------------------------------------*/
 
/*@--------------------------[Prptotype]-------------------------------------*/
struct rf_info;

u32 phlrf_psd_log2base(struct rf_info *rf, u32 val);

void phlrf_rf_lna_setting(struct rf_info *rf, enum phlrf_lna_set type);

void halrf_bkp(struct rf_info *rf, u32 *bp_reg, u32 *bp, u32 reg_num);

void halrf_bkprf(struct rf_info *rf, u32 *bp_reg, u32 bp[][4], u32 reg_num, u32 path_num);

void halrf_reload_bkp(struct rf_info *rf, u32 *bp_reg, u32 *bp, u32 reg_num);
	
void halrf_reload_bkprf(struct rf_info *rf,
		       u32 *bp_reg,
		       u32 bp[][4],
		       u32 reg_num,
		       u8 path_num);

u8 halrf_kpath(struct rf_info *rf, enum phl_phy_idx phy_idx);

void halrf_tmac_tx_pause(struct rf_info *rf, enum phl_phy_idx band_idx, bool pause);

void halrf_trigger_thermal(struct rf_info *rf);

u8 halrf_only_get_thermal(struct rf_info *rf, enum rf_path path);

void halrf_thermal_period(struct rf_info *rf);

void halrf_btc_rfk_ntfy(struct rf_info *rf, u8 phy_map, enum halrf_rfk_type type,
			enum halrf_rfk_process process);
void halrf_fcs_init(struct rf_info *rf);
void halrf_fast_chl_sw_backup(struct rf_info *rf, u8 chl_index, u8 t_index);
void halrf_fast_chl_sw_reload(struct rf_info *rf, u8 chl_index, u8 t_index);

/*FW Offload*/
void halrf_write_fwofld_start(struct rf_info *rf);
void halrf_write_fwofld_trigger(struct rf_info *rf);
void halrf_write_fwofld_end(struct rf_info *rf);

void  halrf_quick_check_rf(void *rf_void);

/*MCC function*/
void halrf_mcc_info_init(void *rf_void, enum phl_phy_idx phy);
void halrf_mcc_get_ch_info(void *rf_void, enum phl_phy_idx phy);
void  halrf_watchdog_stop(struct rf_info *rf, bool is_stop);
/*DBCC*/
void halrf_chlk_backup_dbcc(struct rf_info *rf, enum phl_phy_idx phy);
void halrf_chlk_reload_dbcc(struct rf_info *rf, enum phl_phy_idx phy, u8 idx);
bool halrf_chlk_reload_check_dbcc(struct rf_info *rf, enum phl_phy_idx phy);
void halrf_reset_io_count(struct rf_info *rf);
#endif
