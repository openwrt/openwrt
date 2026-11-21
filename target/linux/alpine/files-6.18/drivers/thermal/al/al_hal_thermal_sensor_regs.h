/* SPDX-License-Identifier: GPL-2.0-only OR BSD-2-Clause */
/* Copyright (C) 2013 Annapurna Labs Ltd. */

#ifndef __AL_THERMAL_SENSE_REGS_H__
#define __AL_THERMAL_SENSE_REGS_H__

/*
* Unit Registers
*/

struct al_thermal_sensor_unit {
	uint32_t static_config;         /* Thermal Sensor Static Configu ... */
	uint32_t threshold_config;      /* Thermal Sensor reporting thre ... */
	uint32_t dynamic_config;        /* Thermal Sensor flow triggers */
	uint32_t status;                /* Thermal Sensor Status and Res ... */
};

struct al_thermal_sensor_regs {
	struct al_thermal_sensor_unit unit;
};

/*
* Registers Fields
*/

/* Power down of the temp sensor */
#define THERMAL_SENSE_UNIT_STATIC_CONFIG_POWER_DOWN (1 << 4)
/* Clock enable. Set onlyonce power-down is cleared. */
#define THERMAL_SENSE_UNIT_STATIC_CONFIG_ENABLE (1 << 5)

/* Run Continuous command. Kept active till cleared by SW. */
#define THERMAL_SENSE_UNIT_DYNAMIC_CONFIG_KEEP_RUNNING (1 << 1)

/**** status register ****/
/* Thermal Sensor read result */
#define THERMAL_SENSE_UNIT_STATUS_T_RESULT_MASK 0x00000FFF
#define THERMAL_SENSE_UNIT_STATUS_T_RESULT_SHIFT 0
/* Temp Result Valid . (Clear On Read) */
#define THERMAL_SENSE_UNIT_STATUS_T_VALID   (1 << 15)
/* Thermal Sensor Init Done. (4uSec  after T_PWR_OK assertion) */
#define THERMAL_SENSE_UNIT_STATUS_T_INIT_DONE (1 << 29)
/* Thermal Sensor powered up (about 50uSec after power up assertion ... */
#define THERMAL_SENSE_UNIT_STATUS_T_PWR_OK  (1 << 30)

#endif
