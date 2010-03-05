/*
 *  linux/include/asm-mips/mach-jz4740/board-n516.h
 *
 *  JZ4730-based N516 board definition.
 *
 *  Copyright (C) 2009, Yauhen Kharuzhy <jekhor@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef __ASM_JZ4740_N516_H__
#define __ASM_JZ4740_N516_H__

#include <asm/mach-jz4740/gpio.h>

/*
 * GPIO
 */
#define GPIO_SD_VCC_EN_N	JZ_GPIO_PORTD(17)
#define GPIO_SD_CD_N		JZ_GPIO_PORTD(7)
#define GPIO_SD_WP		JZ_GPIO_PORTD(15)
#define GPIO_USB_DETECT		JZ_GPIO_PORTD(19)
#define GPIO_CHARG_STAT_N	JZ_GPIO_PORTD(16)
#define GPIO_LED_ENABLE       	JZ_GPIO_PORTD(28)
#define GPIO_LPC_INT		JZ_GPIO_PORTD(14)
#define GPIO_HPHONE_DETECT	JZ_GPIO_PORTD(20)
#define GPIO_SPEAKER_ENABLE	JZ_GPIO_PORTD(21)

/* Display */
#define GPIO_DISPLAY_RST_L	JZ_GPIO_PORTB(18)
#define GPIO_DISPLAY_RDY	JZ_GPIO_PORTB(17)
#define GPIO_DISPLAY_STBY	JZ_GPIO_PORTC(22)
#define GPIO_DISPLAY_ERR	JZ_GPIO_PORTC(23)
#define GPIO_DISPLAY_OFF_N	JZ_GPIO_PORTD(1)

#endif /* __ASM_JZ4740_N516_H__ */
