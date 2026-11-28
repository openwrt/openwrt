/* SPDX-License-Identifier: GPL-2.0-only OR BSD-2-Clause */
/* Copyright (C) 2013 Annapurna Labs Ltd. */

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
 *	err = al_thermal_sensor_handle_init(&handle, (void __iomem*)0xfd860a00,
 *		(void __iomem*)0xfd8a8000);
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
	unsigned int dev_id;
	unsigned int offset;
	unsigned int multiplier;
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
 * @param pbs_regs_base
 *		the virtual base address of the PBS registers
 *
 * @return 0 if no error found.
 */
int al_thermal_sensor_handle_init(
	struct al_thermal_sensor_handle	*thermal_sensor_handle,
	void __iomem			*thermal_sensor_reg_base,
	void __iomem			*pbs_regs_base);

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
