/*******************************************************************************
Copyright (C) 2013 Annapurna Labs Ltd.

This file may be licensed under the terms of the Annapurna Labs Commercial
License Agreement.

Alternatively, this file can be distributed under the terms of the GNU General
Public License V2 as published by the Free Software Foundation and can be
found at http://www.gnu.org/licenses/gpl-2.0.html

Alternatively, redistribution and use in source and binary forms, with or
without modification, are permitted provided that the following conditions are
met:

    *     Redistributions of source code must retain the above copyright notice,
	  this list of conditions and the following disclaimer.

    *     Redistributions in binary form must reproduce the above copyright
	  notice, this list of conditions and the following disclaimer in
	  the documentation and/or other materials provided with the
	  distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

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

/* *INDENT-OFF* */
#ifdef __cplusplus
extern "C" {
#endif
/* *INDENT-ON* */

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

/** Serdes loopback mode */
enum al_serdes_lb_mode {
	/** No loopback */
	AL_SRDS_LB_MODE_OFF,

	/**
	 * Transmits the untimed, partial equalized RX signal out the transmit
	 * IO pins.
	 * No clock used (untimed)
	 */
	AL_SRDS_LB_MODE_PMA_IO_UN_TIMED_RX_TO_TX,

	/**
	 * Loops back the TX serializer output into the CDR.
	 * CDR recovered bit clock used (without attenuation)
	 */
	AL_SRDS_LB_MODE_PMA_INTERNALLY_BUFFERED_SERIAL_TX_TO_RX,

	/**
	 * Loops back the TX driver IO signal to the RX IO pins
	 * CDR recovered bit clock used (only through IO)
	 */
	AL_SRDS_LB_MODE_PMA_SERIAL_TX_IO_TO_RX_IO,

	/**
	 * Parallel loopback from the PMA receive lane data ports, to the
	 * transmit lane data ports
	 * CDR recovered bit clock used
	 */
	AL_SRDS_LB_MODE_PMA_PARALLEL_RX_TO_TX,

	/** Loops received data after elastic buffer to transmit path */
	AL_SRDS_LB_MODE_PCS_PIPE,

	/** Loops TX data (to PMA) to RX path (instead of PMA data) */
	AL_SRDS_LB_MODE_PCS_NEAR_END,

	/** Loops receive data prior to interface block to transmit path */
	AL_SRDS_LB_MODE_PCS_FAR_END,
};

/** Serdes BIST pattern */
enum al_serdes_bist_pattern {
	AL_SRDS_BIST_PATTERN_USER,
	AL_SRDS_BIST_PATTERN_PRBS7,
	AL_SRDS_BIST_PATTERN_PRBS23,
	AL_SRDS_BIST_PATTERN_PRBS31,
	AL_SRDS_BIST_PATTERN_CLK1010,
};

/** SerDes group rate */
enum al_serdes_rate {
	AL_SRDS_RATE_1_8,
	AL_SRDS_RATE_1_4,
	AL_SRDS_RATE_1_2,
	AL_SRDS_RATE_FULL,
};

/** SerDes power mode */
enum al_serdes_pm {
	AL_SRDS_PM_PD,
	AL_SRDS_PM_P2,
	AL_SRDS_PM_P1,
	AL_SRDS_PM_P0S,
	AL_SRDS_PM_P0,
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
 * Enable BIST required overrides
 *
 * @param	obj
 *		The object context
 * @param	grp
 *		The SERDES group
 * @param	rate
 *		The required speed rate
 */
void al_serdes_bist_overrides_enable(
	struct al_serdes_obj	*obj,
	enum al_serdes_group	grp,
	enum al_serdes_rate	rate);

/**
 * SERDES group power mode control
 *
 * @param	obj
 *		The object context
 * @param	grp
 *		The SERDES group
 * @param	pm
 *		The required power mode
 */
void al_serdes_group_pm_set(
	struct al_serdes_obj	*obj,
	enum al_serdes_group	grp,
	enum al_serdes_pm	pm);

/**
 * SERDES lane power mode control
 *
 * @param	obj
 *		The object context
 * @param	grp
 *		The SERDES group
 * @param	lane
 *		The SERDES lane within the group
 * @param	rx_pm
 *		The required RX power mode
 * @param	tx_pm
 *		The required TX power mode
 */
void al_serdes_lane_pm_set(
	struct al_serdes_obj	*obj,
	enum al_serdes_group	grp,
	enum al_serdes_lane	lane,
	enum al_serdes_pm	rx_pm,
	enum al_serdes_pm	tx_pm);

/**
 * SERDES group PMA hard reset
 *
 * Controls Serdes group PMA hard reset
 *
 * @param  obj
 *             The object context
 *
 * @param  grp
 *             The SERDES group
 *
 * @param  enable
 *             Enable/disable hard reset
 */
void al_serdes_pma_hard_reset_group(
	struct al_serdes_obj	*obj,
	enum al_serdes_group	grp,
	al_bool			enable);

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

/**
 * SERDES loopback control
 *
 * Controls the loopback
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
 * @param  mode
 *             The requested loopback mode
 *
 */
void al_serdes_loopback_control(
	struct al_serdes_obj	*obj,
	enum al_serdes_group	grp,
	enum al_serdes_lane	lane,
	enum al_serdes_lb_mode	mode);

/**
 * SERDES BIST pattern selection
 *
 * Selects the BIST pattern to be used
 *
 * @param  obj
 *             The object context
 *
 * @param  grp
 *             The SERDES group
 *
 * @param  pattern
 *             The pattern to set
 *
 * @param  user_data
 *             The pattern user data (when pattern == AL_SRDS_BIST_PATTERN_USER)
 *             80 bits (8 bytes array)
 *
 */
void al_serdes_bist_pattern_select(
	struct al_serdes_obj		*obj,
	enum al_serdes_group		grp,
	enum al_serdes_bist_pattern	pattern,
	uint8_t				*user_data);

/**
 * SERDES BIST TX Enable
 *
 * Enables/disables TX BIST per lane
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
 *             Enable or disable TX BIST
 */
void al_serdes_bist_tx_enable(
	struct al_serdes_obj	*obj,
	enum al_serdes_group	grp,
	enum al_serdes_lane	lane,
	al_bool			enable);

/**
 * SERDES BIST TX single bit error injection
 *
 * Injects single bit error during a TX BIST
 *
 * @param  obj
 *             The object context
 *
 * @param  grp
 *             The SERDES group
 */
void al_serdes_bist_tx_err_inject(
	struct al_serdes_obj	*obj,
	enum al_serdes_group	grp);

/**
 * SERDES BIST RX Enable
 *
 * Enables/disables RX BIST per lane
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
 *             Enable or disable TX BIST
 */
void al_serdes_bist_rx_enable(
	struct al_serdes_obj	*obj,
	enum al_serdes_group	grp,
	enum al_serdes_lane	lane,
	al_bool			enable);

/**
 * SERDES BIST RX status
 *
 * Checks the RX BIST status for a specific SERDES lane
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
 * @param is_locked
 *             An indication whether RX BIST is locked
 *
 * @param err_cnt_overflow
 *             An indication whether error count overflow occured
 *
 * @param err_cnt
 *             Current bit error count
 */
void al_serdes_bist_rx_status(
	struct al_serdes_obj	*obj,
	enum al_serdes_group	grp,
	enum al_serdes_lane	lane,
	al_bool			*is_locked,
	al_bool			*err_cnt_overflow,
	uint16_t		*err_cnt);

/**
 * SERDES Digital Test Bus
 *
 * Samples the digital test bus of a specific SERDES lane
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
 * @param  sel
 *             The selected sampling group (0 - 31)
 *
 * @param sampled_data
 *             The sampled data (5 bytes array)
 *
 *
 * @return 0 if no error found.
 *
 */
int al_serdes_digital_test_bus(
	struct al_serdes_obj	*obj,
	enum al_serdes_group	grp,
	enum al_serdes_lane	lane,
	uint8_t			sel,
	uint8_t			*sampled_data);


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
 * Eye diagram single sampling
 *
 * @param obj The object context
 *
 * @param grp The SERDES group
 *
 * @param lane The SERDES lane within the group
 *
 * @param x Sampling X position (0 - 63 --> -1.00 UI ... 1.00 UI)
 *
 * @param y Sampling Y position (0 - 62 --> 500mV ... -500mV)
 *
 * @param timeout timeout in uSec
 *
 * @param value Eye diagram sample value (BER - 0x0000 - 0xffff)
 *
 * @return 0 if no error found.
 */
int al_serdes_eye_diag_sample(
		struct al_serdes_obj	*obj,
		enum al_serdes_group	grp,
		enum al_serdes_lane	lane,
		unsigned int		x,
		int			y,
		unsigned int		timeout,
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

/**
 * performs Rx equalization and compute the width and height of the eye
 *
 * @param obj the object context
 *
 * @param grp the SERDES group
 *
 * @param lane The SERDES lane within the group
 *
 * @param width the output width of the eye
 *
 * @param height the output height of the eye
 */
int al_serdes_calc_eye_size(
		struct al_serdes_obj *obj,
		enum al_serdes_group grp,
		enum al_serdes_lane  lane,
		int*                 width,
		int*                 height);

/* *INDENT-OFF* */
#ifdef __cplusplus
}
#endif

/* *INDENT-ON* */
#endif		/* __AL_SRDS__ */

/** @} end of SERDES group */

