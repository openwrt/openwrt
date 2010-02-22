/*
 * arch/ubicom32/include/asm/ubicom32bl.h
 *   Ubicom32 architecture backlight driver platform data definitions.
 *
 * (C) Copyright 2009, Ubicom, Inc.
 *
 * This file is part of the Ubicom32 Linux Kernel Port.
 *
 * The Ubicom32 Linux Kernel Port is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 *
 * The Ubicom32 Linux Kernel Port is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Ubicom32 Linux Kernel Port.  If not,
 * see <http://www.gnu.org/licenses/>.
 *
 * Ubicom32 implementation derived from (with many thanks):
 *   arch/m68knommu
 *   arch/blackfin
 *   arch/parisc
 */
#ifndef _ASM_UBICOM32_UBICOM32_BL_H
#define _ASM_UBICOM32_UBICOM32_BL_H

/*
 * Different backlight control mechanisms
 */
enum ubicom32bl_pwm_types {
	/*
	 * PWM controlled backlight
	 */
	UBICOM32BL_TYPE_PWM,

	/*
	 * HRT based PWM backlight
	 */
	UBICOM32BL_TYPE_PWM_HRT,

	/*
	 * No dimming, just on or off
	 */
	UBICOM32BL_TYPE_BINARY,
};

struct ubicom32bl_platform_data {
	/*
	 * Default intensity of the backlight 0-255
	 */
	u8_t				default_intensity;

	/*
	 * TRUE if the backlight sense is active low. (inverted)
	 * FALSE if the backlight sense is active high.
	 */
	bool				invert;

	/*
	 * Type of the backlight
	 */
	enum ubicom32bl_pwm_types	type;

	/*
	 * GPIO of the backlight if UBICOM32BL_TYPE_PWM_HRT, UBICOM32BL_TYPE_BINARY
	 */
	unsigned			gpio;

	/*
	 * PWM channel and parameters of the backlight if UBICOM32BL_TYPE_PWM
	 *	pre_scaler: sets the rate at which the PWM timer is clocked. (clk_core / 2^pre_scaler)
	 *	period: sets the period of the timer in timer cycles
	 * The duty cycle will be directly proportional to the brightness setting.
	 */
	u32_t				pwm_channel;
	u8_t				pwm_prescale;
	u16_t				pwm_period;
};

#endif /* _ASM_UBICOM32_UBICOM32_BL_H */
