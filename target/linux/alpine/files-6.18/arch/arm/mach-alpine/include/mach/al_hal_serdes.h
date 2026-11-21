/* SPDX-License-Identifier: GPL-2.0-only OR BSD-2-Clause */
/* Copyright (C) 2013 Annapurna Labs Ltd. */

/**
 * @defgroup group_serdes_api API
 * SerDes HAL driver API
 * @ingroup group_serdes SerDes
 * @{
 *
 * @file   al_hal_serdes.h
 *
 * @brief Header file for the SerDes HAL driver
 *
 */

#ifndef __AL_HAL_SERDES_H__
#define __AL_HAL_SERDES_H__

#include <mach/al_hal_common.h>

struct al_serdes_obj;

enum al_serdes_group {
	AL_SRDS_GRP_A = 0,
	AL_SRDS_GRP_B,
	AL_SRDS_GRP_C,
	AL_SRDS_GRP_D,

	AL_SRDS_NUM_GROUPS
};

struct al_serdes_group_info {
	/*
	 * Group parent object - filled automatically by al_serdes_handle_init
	 */
	struct al_serdes_obj		*pobj;

	/*
	 * Group specific register base - filled automatically by
	 * al_sedres_handle_init
	 */
	struct al_serdes_regs __iomem	*regs_base;
};

struct al_serdes_obj {
	struct al_serdes_group_info	grp_info[AL_SRDS_NUM_GROUPS];
};

enum al_serdes_reg_page {
	AL_SRDS_REG_PAGE_0_LANE_0 = 0,
	AL_SRDS_REG_PAGE_1_LANE_1,
	AL_SRDS_REG_PAGE_2_LANE_2,
	AL_SRDS_REG_PAGE_3_LANE_3,
	AL_SRDS_REG_PAGE_4_COMMON,
	AL_SRDS_REG_PAGE_0123_LANES_0123 = 7,
};

enum al_serdes_reg_type {
	AL_SRDS_REG_TYPE_PMA = 0,
	AL_SRDS_REG_TYPE_PCS,
};

enum al_serdes_lane {
	AL_SRDS_LANE_0 = AL_SRDS_REG_PAGE_0_LANE_0,
	AL_SRDS_LANE_1 = AL_SRDS_REG_PAGE_1_LANE_1,
	AL_SRDS_LANE_2 = AL_SRDS_REG_PAGE_2_LANE_2,
	AL_SRDS_LANE_3 = AL_SRDS_REG_PAGE_3_LANE_3,

	AL_SRDS_NUM_LANES,
	AL_SRDS_LANES_0123 = AL_SRDS_REG_PAGE_0123_LANES_0123,
};

/**
 * Initializes a SERDES object
 *
 * @param  serdes_regs_base
 *             The SERDES register file base pointer
 *
 * @param obj
 *             An allocated, non initialized object context
 *
 *
 * @return 0 if no error found.
 *
 */
int al_serdes_handle_init(
	void __iomem		*serdes_regs_base,
	struct al_serdes_obj	*obj);

/**
 * SERDES register read
 *
 * Reads a SERDES register
 *
 * @param  obj
 *             The object context
 *
 * @param  grp
 *             The SERDES group
 *
 * @param  page
 *             The SERDES register page within the group
 *
 * @param  type
 *             The SERDES register type (PMA /PCS)
 *
 * @param  offset
 *             The SERDES register offset (0 - 4095)
 *
 * @param data
 *             The read data
 *
 *
 * @return 0 if no error found.
 *
 */
int al_serdes_reg_read(
	struct al_serdes_obj	*obj,
	enum al_serdes_group	grp,
	enum al_serdes_reg_page	page,
	enum al_serdes_reg_type	type,
	uint16_t		offset,
	uint8_t			*data);

/**
 * SERDES register write
 *
 * Writes a SERDES register
 *
 * @param  obj
 *             The object context
 *
 * @param  grp
 *             The SERDES group
 *
 * @param  page
 *             The SERDES register page within the group
 *
 * @param  type
 *             The SERDES register type (PMA /PCS)
 *
 * @param  offset
 *             The SERDES register offset (0 - 4095)
 *
 * @param  data
 *             The data to write
 *
 *
 * @return 0 if no error found.
 *
 */
int al_serdes_reg_write(
	struct al_serdes_obj	*obj,
	enum al_serdes_group	grp,
	enum al_serdes_reg_page	page,
	enum al_serdes_reg_type	type,
	uint16_t		offset,
	uint8_t			data);

/**
 * SERDES lane PMA hard reset
 *
 * Controls Serdes lane PMA hard reset
 *
 * @param  obj
 *             The object context
 *
 * @param  grp
 *             The SERDES group
 *
 * @param  lane
 *             The SERDES lane within the group
 *
 * @param  enable
 *             Enable/disable hard reset
 */
void al_serdes_pma_hard_reset_lane(
	struct al_serdes_obj	*obj,
	enum al_serdes_group	grp,
	enum al_serdes_lane	lane,
	al_bool			enable);

/* KR link training */
/**
 * Set the tx de-emphasis to preset values
 *
 * @param obj The object context
 *
 * @param grp The SERDES group
 *
 * @param lane The SERDES lane within the group
 *
 */
void al_serdes_tx_deemph_preset(
		struct al_serdes_obj	*obj,
		enum al_serdes_group	grp,
		enum al_serdes_lane	lane);

/**
 * Tx de-emphasis parameters
 */
enum al_serdes_tx_deemph_param {
	AL_SERDES_TX_DEEMP_C_ZERO,	/*< c(0) */
	AL_SERDES_TX_DEEMP_C_PLUS,	/*< c(1) */
	AL_SERDES_TX_DEEMP_C_MINUS,	/*< c(-1) */
};

/**
 * Increase tx de-emphasis param.
 *
 * @param obj The object context
 *
 * @param grp The SERDES group
 *
 * @param lane The SERDES lane within the group
 *
 * @param param which tx de-emphasis to change
 *
 * @return false in case max is reached. true otherwise.
 */
al_bool al_serdes_tx_deemph_inc(
		struct al_serdes_obj	*obj,
		enum al_serdes_group	grp,
		enum al_serdes_lane	lane,
		enum al_serdes_tx_deemph_param param);

/**
 * Decrease tx de-emphasis param.
 *
 * @param obj The object context
 *
 * @param grp The SERDES group
 *
 * @param lane The SERDES lane within the group
 *
 * @param param which tx de-emphasis to change
 *
 * @return false in case min is reached. true otherwise.
 */
al_bool al_serdes_tx_deemph_dec(
		struct al_serdes_obj	*obj,
		enum al_serdes_group	grp,
		enum al_serdes_lane	lane,
		enum al_serdes_tx_deemph_param param);

/**
 * run Rx eye measurement.
 *
 * @param obj The object context
 *
 * @param grp The SERDES group
 *
 * @param lane The SERDES lane within the group
 *
 * @param timeout timeout in uSec
 *
 * @param value Rx eye measurement value
 *		(0 - completely closed eye, 0xffff - completely open eye).
 *
 * @return 0 if no error found.
 */
int al_serdes_eye_measure_run(
		struct al_serdes_obj	*obj,
		enum al_serdes_group	grp,
		enum al_serdes_lane	lane,
		uint32_t		timeout,
		unsigned int		*value);

/**
 * Check if signal is detected
 *
 * @param obj The object context
 *
 * @param grp The SERDES group
 *
 * @param lane The SERDES lane within the group
 *
 * @return true if signal is detected. false otherwise.
 */
al_bool al_serdes_signal_is_detected(
		struct al_serdes_obj	*obj,
		enum al_serdes_group	grp,
		enum al_serdes_lane	lane);

struct al_serdes_adv_tx_params {
	/*
	 * select the input values location.
	 * When set to true the values will be taken from the internal registers
	 * that will be override with the next following parameters.
	 * When set to false the values will be taken from external pins (the
	 * other parameters in this case is not needed)
	 */
	al_bool				override;
	/*
	 * Transmit Amplitude control signal. Used to define the full-scale
	 * maximum swing of the driver.
	 *	000 - Not Supported
	 *	001 - 952mVdiff-pkpk
	 *	010 - 1024mVdiff-pkpk
	 *	011 - 1094mVdiff-pkpk
	 *	100 - 1163mVdiff-pkpk
	 *	101 - 1227mVdiff-pkpk
	 *	110 - 1283mVdiff-pkpk
	 *	111 - 1331mVdiff-pkpk
	 */
	uint8_t				amp;
	/* Defines the total number of driver units allocated in the driver */
	uint8_t				total_driver_units;
	/* Defines the total number of driver units allocated to the
	 * first post-cursor (C+1) tap. */
	uint8_t				c_plus_1;
	/* Defines the total number of driver units allocated to the
	 * second post-cursor (C+2) tap. */
	uint8_t				c_plus_2;
	/* Defines the total number of driver units allocated to the
	 * first pre-cursor (C-1) tap. */
	uint8_t				c_minus_1;
	/* TX driver Slew Rate control:
	 *	00 - 31ps
	 *	01 - 33ps
	 *	10 - 68ps
	 *	11 - 170ps
	 */
	uint8_t				slew_rate;
};

struct al_serdes_adv_rx_params {
	/*
	 * select the input values location.
	 * When set to true the values will be taken from the internal registers
	 * that will be override with the next following parameters.
	 * When set to false the values will be taken based in the equalization
	 * results (the other parameters in this case is not needed)
	 */
	al_bool				override;
	/* RX agc high frequency dc gain:
	 *	-3'b000: -3dB
	 *	-3'b001: -2.5dB
	 *	-3'b010: -2dB
	 *	-3'b011: -1.5dB
	 *	-3'b100: -1dB
	 *	-3'b101: -0.5dB
	 *	-3'b110: -0dB
	 *	-3'b111: 0.5dB
	 */
	uint8_t				dcgain;
	/* DFE post-shaping tap 3dB frequency
	 *	-3'b000: 684MHz
	 *	-3'b001: 576MHz
	 *	-3'b010: 514MHz
	 *	-3'b011: 435MHz
	 *	-3'b100: 354MHz
	 *	-3'b101: 281MHz
	 *	-3'b110: 199MHz
	 *	-3'b111: 125MHz
	 */
	uint8_t				dfe_3db_freq;
	/* DFE post-shaping tap gain
	 *	0: no pulse shaping tap
	 *	1: -24mVpeak
	 *	2: -45mVpeak
	 *	3: -64mVpeak
	 *	4: -80mVpeak
	 *	5: -93mVpeak
	 *	6: -101mVpeak
	 *	7: -105mVpeak
	 */
	uint8_t				dfe_gain;
	/* DFE first tap gain control
	 *	-4'b0000: +1mVpeak
	 *	-4'b0001: +10mVpeak
	 *	....
	 *	-4'b0110: +55mVpeak
	 *	-4'b0111: +64mVpeak
	 *	-4'b1000: -1mVpeak
	 *	-4'b1001: -10mVpeak
	 *	....
	 *	-4'b1110: -55mVpeak
	 *	-4'b1111: -64mVpeak
	 */
	uint8_t				dfe_first_tap_ctrl;
	/* DFE second tap gain control
	 *	-4'b0000: +0mVpeak
	 *	-4'b0001: +9mVpeak
	 *	....
	 *	-4'b0110: +46mVpeak
	 *	-4'b0111: +53mVpeak
	 *	-4'b1000: -0mVpeak
	 *	-4'b1001: -9mVpeak
	 *	....
	 *	-4'b1110: -46mVpeak
	 *	-4'b1111: -53mVpeak
	 */
	uint8_t				dfe_secound_tap_ctrl;
	/* DFE third tap gain control
	 *	-4'b0000: +0mVpeak
	 *	-4'b0001: +7mVpeak
	 *	....
	 *	-4'b0110: +38mVpeak
	 *	-4'b0111: +44mVpeak
	 *	-4'b1000: -0mVpeak
	 *	-4'b1001: -7mVpeak
	 *	....
	 *	-4'b1110: -38mVpeak
	 *	-4'b1111: -44mVpeak
	 */
	uint8_t				dfe_third_tap_ctrl;
	/* DFE fourth tap gain control
	 *	-4'b0000: +0mVpeak
	 *	-4'b0001: +6mVpeak
	 *	....
	 *	-4'b0110: +29mVpeak
	 *	-4'b0111: +33mVpeak
	 *	-4'b1000: -0mVpeak
	 *	-4'b1001: -6mVpeak
	 *	....
	 *	-4'b1110: -29mVpeak
	 *	-4'b1111: -33mVpeak
	 */
	uint8_t				dfe_fourth_tap_ctrl;
	/* Low frequency agc gain (att) select
	 *	-3'b000: Disconnected
	 *	-3'b001: -18.5dB
	 *	-3'b010: -12.5dB
	 *	-3'b011: -9dB
	 *	-3'b100: -6.5dB
	 *	-3'b101: -4.5dB
	 *	-3'b110: -2.9dB
	 *	-3'b111: -1.6dB
	 */
	uint8_t				low_freq_agc_gain;
	/* Provides a RX Equalizer pre-hint, prior to beginning
	 * adaptive equalization */
	uint8_t				precal_code_sel;
	/* High frequency agc boost control
	 *	Min d0: Boost ~4dB
	 *	Max d31: Boost ~20dB
	 */
	uint8_t				high_freq_agc_boost;
};

/**
 * configure tx advanced parameters
 *
 * @param obj The object context
 *
 * @param grp The SERDES group
 *
 * @param lane The SERDES lane within the group
 *
 * @param params pointer to the tx parameters
 */
void al_serdes_tx_advanced_params_set(struct al_serdes_obj	      	*obj,
				      enum al_serdes_group		grp,
				      enum al_serdes_lane		lane,
				      struct al_serdes_adv_tx_params  *params);

/**
 * read tx advanced parameters
 *
 * @param obj The object context
 *
 * @param grp The SERDES group
 *
 * @param lane The SERDES lane within the group
 *
 * @param params pointer to the tx parameters
 */
void al_serdes_tx_advanced_params_get(struct al_serdes_obj	     	*obj,
				      enum al_serdes_group	      	grp,
				      enum al_serdes_lane		lane,
				      struct al_serdes_adv_tx_params *params);

/**
 * configure rx advanced parameters
 *
 * @param obj The object context
 *
 * @param grp The SERDES group
 *
 * @param lane The SERDES lane within the group
 *
 * @param params pointer to the rx parameters
 */
void al_serdes_rx_advanced_params_set(struct al_serdes_obj	      *obj,
				      enum al_serdes_group	      grp,
				      enum al_serdes_lane	      lane,
				      struct al_serdes_adv_rx_params  *params);

/**
 * read rx advanced parameters
 *
 * @param obj The object context
 *
 * @param grp The SERDES group
 *
 * @param lane The SERDES lane within the group
 *
 * @param params pointer to the rx parameters
 */
void al_serdes_rx_advanced_params_get(struct al_serdes_obj           *obj,
				      enum al_serdes_group	      grp,
				      enum al_serdes_lane	      lane,
				      struct al_serdes_adv_rx_params* params);

/**
 *  Switch entire SerDes group to SGMII mode based on 156.25 Mhz reference clock
 *
 * @param obj The object context
 *
 * @param grp The SERDES group
 */
void al_serdes_mode_set_sgmii(
	struct al_serdes_obj	*obj,
	enum al_serdes_group	grp);

/**
 *  Switch entire SerDes group to KR mode based on 156.25 Mhz reference clock
 *
 * @param obj The object context
 *
 * @param grp The SERDES group
 */
void al_serdes_mode_set_kr(
	struct al_serdes_obj	*obj,
	enum al_serdes_group	grp);

/**
 * performs SerDes HW equalization test and update equalization parameters
 *
 * @param obj the object context
 *
 * @param grp the SERDES group
 *
 * @param lane The SERDES lane within the group
 */
int al_serdes_rx_equalization(
		struct al_serdes_obj	*obj,
		enum al_serdes_group	grp,
		enum al_serdes_lane	lane);

#endif		/* __AL_SRDS__ */

/** @} end of SERDES group */
