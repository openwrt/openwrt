/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation.
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

#ifndef __HALRF_8192E_H__
#define __HALRF_8192E_H__

/*--------------------------Define Parameters-------------------------------*/
#if (DM_ODM_SUPPORT_TYPE & ODM_CE)
	#define	IQK_DELAY_TIME_92E		15		/* ms */
#else
	#define	IQK_DELAY_TIME_92E		10
#endif

#define	index_mapping_NUM_92E	15
#define AVG_THERMAL_NUM_92E	4
#define	RF_T_METER_92E			0x42

#include "../halphyrf_ap.h"

void configure_txpower_track_8192e(
	struct txpwrtrack_cfg	*config
);

void
get_delta_swing_table_8192e(
	void		*dm_void,
	u8 **temperature_up_a,
	u8 **temperature_down_a,
	u8 **temperature_up_b,
	u8 **temperature_down_b
);

void do_iqk_8192e(
	void		*dm_void,
	u8		delta_thermal_index,
	u8		thermal_value,
	u8		threshold
);

void
odm_tx_pwr_track_set_pwr92_e(
	void		*dm_void,
	enum pwrtrack_method	method,
	u8				rf_path,
	u8				channel_mapped_index
);

/* 1 7.	IQK */

void
phy_iq_calibrate_8192e(
	void		*dm_void,
	boolean	is_recovery);


/*
 * LC calibrate
 *   */
void
phy_lc_calibrate_8192e(
	void		*dm_void
);

/*
 * AP calibrate
 *   */
void
phy_ap_calibrate_8192e(
#if (DM_ODM_SUPPORT_TYPE & ODM_AP)
	struct dm_struct		*dm,
#else
	void	*adapter,
#endif
	s8		delta);


void
_phy_save_adda_registers_92e(
#if (DM_ODM_SUPPORT_TYPE & ODM_AP)
	struct dm_struct		*dm,
#else
	void	*adapter,
#endif
	u32		*adda_reg,
	u32		*adda_backup,
	u32		register_num
);

void
_phy_path_adda_on_92e(
#if (DM_ODM_SUPPORT_TYPE & ODM_AP)
	struct dm_struct		*dm,
#else
	void	*adapter,
#endif
	u32		*adda_reg,
	boolean		is_path_a_on,
	boolean		is2T
);

void
_phy_mac_setting_calibration_92e(
#if (DM_ODM_SUPPORT_TYPE & ODM_AP)
	struct dm_struct		*dm,
#else
	void	*adapter,
#endif
	u32		*mac_reg,
	u32		*mac_backup
);

#if (DM_ODM_SUPPORT_TYPE & ODM_AP)
void
_phy_path_a_stand_by(
	struct dm_struct		*dm
);
#endif


void
halrf_rf_lna_setting_8192e(
	struct dm_struct	*dm,
	enum halrf_lna_set type
);



#endif	/*#ifndef __HALRF_8192E_H__*/
