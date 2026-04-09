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
 * @defgroup group_thermal_sensor Thermal Sensor
 * @ingroup group_sys_services
 *  @{
 *	Common Operation Example:
 * @code
 * void main()
 * {
 *	struct al_thermal_sensor_handle handle;
 *	int err;
 *
 *	err = al_thermal_sensor_handle_init(&handle, (void __iomem*)0xfd860a00);
 *	if (err)
 *		return err;
 *
 *	al_thermal_sensor_enable_set(&handle, 1);
 *
 *	while (!al_thermal_sensor_is_ready(&handle))
 *		udelay(1);
 *
 *	al_thermal_sensor_trigger_once(&handle);
 *
 *	while (!al_thermal_sensor_readout_is_valid(&handle))
 *		udelay(1);
 *
 *	printf("temprature: %d degrees\n",
 *		al_thermal_sensor_readout_get(&handle));
 *
 *	al_thermal_sensor_enable_set(&handle, 0);
 * }
 * @endcode
 *      @file  al_hal_thermal_sensor.h
 *      @brief HAL Driver Header for the Annapurna Labs thermal sensor unit
 *
 */

#ifndef _AL_HAL_THERMAL_SENSE_H_
#define _AL_HAL_THERMAL_SENSE_H_

#include <mach/al_hal_common.h>

/******************************* Constants ***********************************/
/** Value to be used for disabling the low temperature threshold */
#define AL_THERMAL_SENSOR_LOW_THRESHOLD_DISABLE		-1000
/** Value to be used for disabling the high temperature threshold */
#define AL_THERMAL_SENSOR_HIGH_THRESHOLD_DISABLE	1000

/*********************** Data Structures and Types ***************************/

/**
 * al_thermal_sensor_handle: data structure used by the HAL to handle thermal
 * sensor functionality. this structure is allocated and set to zeros by the
 * upper layer, then it is initialized by the al_thermal_sensor_init() that
 * should be called before any other function of this API. later, this handle
 * passed to the API functions.
 */
struct al_thermal_sensor_handle {
	struct al_thermal_sensor_regs __iomem	*regs;
	uint32_t readout_raw;
};

/************************************* API ***********************************/

/**
 * @brief Initialize a thermal sensor handle structure
 *
 * @param thermal_sensor_handle
 *		an allocated, non-initialized instance.
 * @param thermal_sensor_reg_base
 *		the virtual base address of the thermal sensor internal
 *		registers
 *
 * @return 0 if no error found.
 */
int al_thermal_sensor_handle_init(
	struct al_thermal_sensor_handle	*thermal_sensor_handle,
	void __iomem			*thermal_sensor_reg_base);

/**
 * @brief Set SW digital trimming
 *
 * @param thermal_sensor_handle
 *		thermal_sensor handle
 * @param trim
 *		the required auxiliary trimming (process dependent, usually
 *		obtained from eFuse, default value is 8)
 */
void al_thermal_sensor_trim_set(
	struct al_thermal_sensor_handle *thermal_sensor_handle,
	unsigned int			trim);

/**
 * @brief Set low and high temperature thresholds for interrupt generation
 *
 * @param thermal_sensor_handle
 *		thermal_sensor handle
 * @param low_temp_threshold
 *		low temperature threshold in celsius degrees
 *		a value of AL_THERMAL_SENSOR_LOW_THRESHOLD_DISABLE disables
 *		the threshold
 * @param high_temp_threshold
 *		high temperature threshold in celsius degrees
 *		a value of AL_THERMAL_SENSOR_HIGH_THRESHOLD_DISABLE disables
 *		the threshold
 */
void al_thermal_sensor_threshold_config(
	struct al_thermal_sensor_handle *thermal_sensor_handle,
	int				low_temp_threshold,
	int				high_temp_threshold);

/**
 * @brief Power up/down and enable/disable the thermal sensor unit
 *
 * @param thermal_sensor_handle
 *		thermal_sensor handle
 * @param enable
 *		Whether to power-up and enable the thermal sensor or to disable
 *		it and power it down
 */
void al_thermal_sensor_enable_set(
	struct al_thermal_sensor_handle	*thermal_sensor_handle,
	int				enable);

/**
 * @brief Determine if the thermal sensor is ready
 *
 * @param thermal_sensor_handle
 *		thermal_sensor handle
 *
 * @return non zero if the thermal sensor is ready
 */
int al_thermal_sensor_is_ready(
	struct al_thermal_sensor_handle	*thermal_sensor_handle);

/**
 * @brief Trigger single sample
 *
 * @param thermal_sensor_handle
 *		thermal_sensor handle
 */
void al_thermal_sensor_trigger_once(
	struct al_thermal_sensor_handle	*thermal_sensor_handle);

/**
 * @brief Trigger continuous sampling
 *
 * @param thermal_sensor_handle
 *		thermal_sensor handle
 */
void al_thermal_sensor_trigger_continuous(
	struct al_thermal_sensor_handle	*thermal_sensor_handle);

/**
 * @brief Determine if the thermal sensor readout is valid
 *
 * @param thermal_sensor_handle
 *		thermal_sensor handle
 *
 * @return non zero if the thermal sensor readout is valid
 */
int al_thermal_sensor_readout_is_valid(
	struct al_thermal_sensor_handle	*thermal_sensor_handle);

/**
 * @brief Get the thermal sensor current readout - assuming valid
 *
 * @param thermal_sensor_handle
 *		thermal_sensor handle
 *
 * @return current thermal sensor readout [degrees celcius]
 */
int al_thermal_sensor_readout_get(
	struct al_thermal_sensor_handle	*thermal_sensor_handle);

#endif
/** @} end of groupthermal_sensor group */

