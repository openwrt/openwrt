/*
 * JzRISC lcd controller
 *
 * xiangfu liu <xiangfu.z@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __QI_LB60_GPM940B0_H__
#define __QI_LB60_GPM940B0_H__

#include <asm/io.h>

#define mdelay(n)	udelay((n)*1000)

#define NR_PALETTE	256

struct lcd_desc{
	unsigned int next_desc; /* LCDDAx */
	unsigned int databuf;   /* LCDSAx */
	unsigned int frame_id;  /* LCDFIDx */ 
	unsigned int cmd;       /* LCDCMDx */
};

#define MODE_MASK		0x0f
#define MODE_TFT_GEN		0x00
#define MODE_TFT_SHARP		0x01
#define MODE_TFT_CASIO		0x02
#define MODE_TFT_SAMSUNG	0x03
#define MODE_CCIR656_NONINT	0x04
#define MODE_CCIR656_INT	0x05
#define MODE_STN_COLOR_SINGLE	0x08
#define MODE_STN_MONO_SINGLE	0x09
#define MODE_STN_COLOR_DUAL	0x0a
#define MODE_STN_MONO_DUAL	0x0b
#define MODE_8BIT_SERIAL_TFT    0x0c

#define MODE_TFT_18BIT          (1<<7)

#define STN_DAT_PIN1	(0x00 << 4)
#define STN_DAT_PIN2	(0x01 << 4)
#define STN_DAT_PIN4	(0x02 << 4)
#define STN_DAT_PIN8	(0x03 << 4)
#define STN_DAT_PINMASK	STN_DAT_PIN8

#define STFT_PSHI	(1 << 15)
#define STFT_CLSHI	(1 << 14)
#define STFT_SPLHI	(1 << 13)
#define STFT_REVHI	(1 << 12)

#define SYNC_MASTER	(0 << 16)
#define SYNC_SLAVE	(1 << 16)

#define DE_P		(0 << 9)
#define DE_N		(1 << 9)

#define PCLK_P		(0 << 10)
#define PCLK_N		(1 << 10)

#define HSYNC_P		(0 << 11)
#define HSYNC_N		(1 << 11)

#define VSYNC_P		(0 << 8)
#define VSYNC_N		(1 << 8)

#define DATA_NORMAL	(0 << 17)
#define DATA_INVERSE	(1 << 17)


/* Jz LCDFB supported I/O controls. */
#define FBIOSETBACKLIGHT	0x4688
#define FBIODISPON		0x4689
#define FBIODISPOFF		0x468a
#define FBIORESET		0x468b
#define FBIOPRINT_REG		0x468c

/*
 * LCD panel specific definition
 */
#define MODE	0xc9		/* 8bit serial RGB */
#define SPEN	(32*2+21)       /*LCD_SPL */
#define SPCK	(32*2+23)       /*LCD_CLS */
#define SPDA	(32*2+22)       /*LCD_D12 */
#define LCD_RET (32*3+27) 

#define __spi_write_reg1(reg, val) \
do { \
	unsigned char no;\
	unsigned short value;\
	unsigned char a=0;\
	unsigned char b=0;\
	a=reg;\
	b=val;\
	__gpio_set_pin(SPEN);\
	__gpio_set_pin(SPCK);\
	__gpio_clear_pin(SPDA);\
	__gpio_clear_pin(SPEN);\
	udelay(25);\
	value=((a<<8)|(b&0xFF));\
	for(no=0;no<16;no++)\
	{\
		__gpio_clear_pin(SPCK);\
		if((value&0x8000)==0x8000)\
		__gpio_set_pin(SPDA);\
		else\
		__gpio_clear_pin(SPDA);\
		udelay(25);\
		__gpio_set_pin(SPCK);\
		value=(value<<1); \
		udelay(25);\
	 }\
	__gpio_set_pin(SPEN);\
	udelay(100);\
} while (0)

#define __spi_write_reg(reg, val) \
do {\
	__spi_write_reg1((reg<<2|2), val);\
	udelay(100); \
}while(0)

#define __lcd_special_pin_init() \
do { \
	__gpio_as_output(SPEN); /* use SPDA */\
	__gpio_as_output(SPCK); /* use SPCK */\
	__gpio_as_output(SPDA); /* use SPDA */\
	__gpio_as_output(LCD_RET);\
} while (0)

#define __lcd_special_on() \
do { \
	__spi_write_reg1(0x05, 0x1e); \
	udelay(50);\
	__spi_write_reg1(0x05, 0x5d); \
	__spi_write_reg1(0x0B, 0x81); \
	__spi_write_reg1(0x01, 0x95); \
	__spi_write_reg1(0x00, 0x07); \
	__spi_write_reg1(0x06, 0x15); \
	__spi_write_reg1(0x07, 0x8d); \
	__spi_write_reg1(0x04, 0x0f); \
	__spi_write_reg1(0x0d, 0x3d); \
	__spi_write_reg1(0x10, 0x42); \
	__spi_write_reg1(0x11, 0x3a); \
	__spi_write_reg1(0x05, 0x5f); \
} while (0)

#define __lcd_special_off() \
do {					\
	__spi_write_reg1(0x05, 0x5e);	\
} while (0)

#define __lcd_display_pin_init() \
do { \
	__lcd_special_pin_init();\
	__gpio_as_pwm();\
	__lcd_set_backlight_level(8);\
} while (0)

#define __lcd_display_on() \
do { \
	__lcd_set_backlight_level(8); \
	__lcd_special_on();\
} while (0)

#define __lcd_display_off() \
do { \
	__lcd_set_backlight_level(0); \
	__lcd_special_off();\
} while (0)

#define __lcd_set_backlight_level(n)\
do { \
	__gpio_as_output(LCD_RET); \
	__gpio_set_pin(LCD_RET); \
} while (0)

#if defined(CONFIG_SAKC)
#define __lcd_close_backlight() \
do { \
	__gpio_as_output(GPIO_PWM); \
	__gpio_clear_pin(GPIO_PWM); \
} while (0)
#endif

#if defined(CONFIG_SAKC)
#define __lcd_display_pin_init() \
do { \
	__cpm_start_tcu(); \
	__lcd_special_pin_init(); \
} while (0)

#define __lcd_display_on() \
do { \
	__lcd_special_on(); \
} while (0)

#define __lcd_display_off() \
do { \
	__lcd_special_off(); \
} while (0)
#else
#define __lcd_display_pin_init() \
do { \
	__cpm_start_tcu(); \
	__lcd_special_pin_init(); \
} while (0)

#define __lcd_display_on() \
do { \
	__gpio_set_pin(GPIO_DISP_OFF_N); \
	__lcd_special_on(); \
} while (0)

#define __lcd_display_off() \
do { \
	__lcd_special_off(); \
	__gpio_clear_pin(GPIO_DISP_OFF_N); \
} while (0)
#endif

#endif /* __QI_LB60_GPM940B0_H__ */
