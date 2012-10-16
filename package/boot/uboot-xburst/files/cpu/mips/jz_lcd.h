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

#ifndef __JZLCD_H__
#define __JZLCD_H__

#include <asm/io.h>
/*
 * change u-boot macro to celinux macro
 */
/* Chip type */
#if defined(CONFIG_JZ4740)
#define CONFIG_MIPS_JZ4740 		1
#endif

/* board type */
#if defined(CONFIG_NANONOTE)
#define CONFIG_MIPS_JZ4740_PI 		1
#endif

#define mdelay(n) 		udelay((n)*1000)

/*
 * change u-boot macro to celinux macro
 */

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

#if defined(CONFIG_JZLCD_FOXCONN_PT035TN01) || defined(CONFIG_JZLCD_INNOLUX_PT035TN01_SERIAL)

#if defined(CONFIG_JZLCD_FOXCONN_PT035TN01) /* board pmp */
#define MODE 0xcd 		/* 24bit parellel RGB */
#endif
#if defined(CONFIG_JZLCD_INNOLUX_PT035TN01_SERIAL)
#define MODE 0xc9		/* 8bit serial RGB */
#endif

#if defined(CONFIG_MIPS_JZ4740_PI) /* board pavo */
	#define SPEN	(32*2+21)       /*LCD_SPL */
	#define SPCK	(32*2+23)       /*LCD_CLS */
	#define SPDA	(32*2+22)       /*LCD_D12 */
	#define LCD_RET (32*3+27) 
#else
#error "cpu/misp/Jzlcd.h, please define SPI pins on your board."
#endif

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

#if defined(CONFIG_NANONOTE)
	#define __lcd_special_on() \
		do { \
		udelay(50);\
		__spi_write_reg1(0x05, 0x16); \
		__spi_write_reg1(0x04, 0x0b); \
		__spi_write_reg1(0x07, 0x8d); \
		__spi_write_reg1(0x01, 0x95); \
		__spi_write_reg1(0x08, 0xc0); \
		__spi_write_reg1(0x03, 0x40); \
		__spi_write_reg1(0x06, 0x15); \
		__spi_write_reg1(0x05, 0xd7); \
		} while (0)	/* reg 0x0a is control the display direction:DB0->horizontal level DB1->vertical level */

	#define __lcd_special_off()				\
			  do {					\
				  __spi_write_reg1(0x05, 0x5e);	\
			  } while (0)
#endif	/* CONFIG_NANONOTE */
#endif	/* CONFIG_JZLCD_FOXCONN_PT035TN01 or CONFIG_JZLCD_INNOLUX_PT035TN01_SERIAL */

#ifndef __lcd_special_pin_init
#define __lcd_special_pin_init()
#endif
#ifndef __lcd_special_on
#define __lcd_special_on()
#endif
#ifndef __lcd_special_off
#define __lcd_special_off()
#endif


/*
 * Platform specific definition
 */

#if defined(CONFIG_MIPS_JZ4740_PI)

	/* 100 level: 0,1,...,100 */
	#define __lcd_set_backlight_level(n)\
	do { \
	__gpio_as_output(32*3+27); \
	__gpio_set_pin(32*3+27); \
	} while (0)

	#define __lcd_close_backlight() \
	do { \
	__gpio_as_output(GPIO_PWM); \
	__gpio_clear_pin(GPIO_PWM); \
	} while (0)

	#define __lcd_display_pin_init() \
	do { \
		__gpio_as_output(GPIO_DISP_OFF_N); \
		__cpm_start_tcu(); \
		__lcd_special_pin_init(); \
	} while (0)
	/*	__lcd_set_backlight_level(100); \*/
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

#endif /* CONFIG_MIPS_JZ4740_PI) */

/*****************************************************************************
 * LCD display pin dummy macros
 *****************************************************************************/
#ifndef __lcd_display_pin_init
#define __lcd_display_pin_init()
#endif
#ifndef __lcd_display_on
#define __lcd_display_on()
#endif
#ifndef __lcd_display_off
#define __lcd_display_off()
#endif
#ifndef __lcd_set_backlight_level
#define __lcd_set_backlight_level(n)
#endif

