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


#ifndef __AL_THERMAL_SENSE_REGS_H__
#define __AL_THERMAL_SENSE_REGS_H__

#ifdef __cplusplus
extern "C" {
#endif
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


/**** static_config register ****/
/* SW digital trimming offset */
#define THERMAL_SENSE_UNIT_STATIC_CONFIG_TRIM_MASK 0x0000000F
#define THERMAL_SENSE_UNIT_STATIC_CONFIG_TRIM_SHIFT 0
/* Power down of the temp sensor */
#define THERMAL_SENSE_UNIT_STATIC_CONFIG_POWER_DOWN (1 << 4)
/* Clock enable. Set onlyonce power-down is cleared. */
#define THERMAL_SENSE_UNIT_STATIC_CONFIG_ENABLE (1 << 5)
/* Analog vtrim enabling */
#define THERMAL_SENSE_UNIT_STATIC_CONFIG_CAL (1 << 6)
/* Reserved */
#define THERMAL_SENSE_UNIT_STATIC_CONFIG_RSRVD_7 (1 << 7)
/* Combined digital TRIM in case fuses are used to override the  ... */
#define THERMAL_SENSE_UNIT_STATIC_CONFIG_CTRIM_MASK 0x00000F00
#define THERMAL_SENSE_UNIT_STATIC_CONFIG_CTRIM_SHIFT 8
/* Clock divide ratio for 1uSec local clock needed by the sensor ... */
#define THERMAL_SENSE_UNIT_STATIC_CONFIG_CLK_RATIO_MASK 0x000FF000
#define THERMAL_SENSE_UNIT_STATIC_CONFIG_CLK_RATIO_SHIFT 12
/* PowerUp Time Period before Reset Release */
#define THERMAL_SENSE_UNIT_STATIC_CONFIG_POWER_UP_COUNT_MASK 0x0FF00000
#define THERMAL_SENSE_UNIT_STATIC_CONFIG_POWER_UP_COUNT_SHIFT 20
/* Init Time Period */
#define THERMAL_SENSE_UNIT_STATIC_CONFIG_INIT_COUNT_MASK 0xF0000000
#define THERMAL_SENSE_UNIT_STATIC_CONFIG_INIT_COUNT_SHIFT 28

/**** threshold_config register ****/
/* Threshold for Low-Temp indication and Interrupt assertion */
#define THERMAL_SENSE_UNIT_THRESHOLD_CONFIG_LOW_TEMP_THRESHOLD_MASK 0x00000FFF
#define THERMAL_SENSE_UNIT_THRESHOLD_CONFIG_LOW_TEMP_THRESHOLD_SHIFT 0
/* Reserved */
#define THERMAL_SENSE_UNIT_THRESHOLD_CONFIG_RSRVD_15_12_MASK 0x0000F000
#define THERMAL_SENSE_UNIT_THRESHOLD_CONFIG_RSRVD_15_12_SHIFT 12
/* Threshold for High-Temp indication and Interrupt assertion */
#define THERMAL_SENSE_UNIT_THRESHOLD_CONFIG_HIGH_TEMP_THRESHOLD_MASK 0x0FFF0000
#define THERMAL_SENSE_UNIT_THRESHOLD_CONFIG_HIGH_TEMP_THRESHOLD_SHIFT 16
/* Reserved */
#define THERMAL_SENSE_UNIT_THRESHOLD_CONFIG_RSRVD_31_28_MASK 0xF0000000
#define THERMAL_SENSE_UNIT_THRESHOLD_CONFIG_RSRVD_31_28_SHIFT 28

/**** dynamic_config register ****/
/* Run Once command */
#define THERMAL_SENSE_UNIT_DYNAMIC_CONFIG_RUN_ONCE (1 << 0)
/* Run Continuous command. Kept active till cleared by SW. */
#define THERMAL_SENSE_UNIT_DYNAMIC_CONFIG_KEEP_RUNNING (1 << 1)
/* Reserved */
#define THERMAL_SENSE_UNIT_DYNAMIC_CONFIG_RSRVD_31_2_MASK 0xFFFFFFFC
#define THERMAL_SENSE_UNIT_DYNAMIC_CONFIG_RSRVD_31_2_SHIFT 2

/**** status register ****/
/* Thermal Sensor read result */
#define THERMAL_SENSE_UNIT_STATUS_T_RESULT_MASK 0x00000FFF
#define THERMAL_SENSE_UNIT_STATUS_T_RESULT_SHIFT 0
/* Temp is below Min. (Clear On Read) */
#define THERMAL_SENSE_UNIT_STATUS_T_LOW     (1 << 13)
/* Temp is above Max. (Clear On Read) */
#define THERMAL_SENSE_UNIT_STATUS_T_HIGH    (1 << 14)
/* Temp Result Valid . (Clear On Read) */
#define THERMAL_SENSE_UNIT_STATUS_T_VALID   (1 << 15)
/* Thermal Sensor Init Done. (4uSec  after T_PWR_OK assertion) */
#define THERMAL_SENSE_UNIT_STATUS_T_INIT_DONE (1 << 29)
/* Thermal Sensor powered up (about 50uSec after power up assertion ... */
#define THERMAL_SENSE_UNIT_STATUS_T_PWR_OK  (1 << 30)
/* Thermal Sensor is busy (in the middle of conversion sequence) */
#define THERMAL_SENSE_UNIT_STATUS_T_BUSY    (1 << 31)

#ifdef __cplusplus
}
#endif

#endif

