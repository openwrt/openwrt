/*
 *  Copyright (C) 2009, Lars-Peter Clausen <lars@metafoo.de>
 *
 *  This program is free software; you can redistribute	 it and/or modify it
 *  under  the terms of	 the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the	License, or (at your
 *  option) any later version.
 *
 *  You should have received a copy of the  GNU General Public License along
 *  with this program; if not, write  to the Free Software Foundation, Inc.,
 *  675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifndef __LINUX_JZ4740_FB_H
#define __LINUX_JZ4740_FB_H

#include <linux/fb.h>

enum jz4740_fb_lcd_type {
	JZ_LCD_TYPE_GENERIC_16_18_BIT = 0,
	JZ_LCD_TYPE_SPECIAL_TFT_1 = 1,
	JZ_LCD_TYPE_SPECIAL_TFT_2 = 2,
	JZ_LCD_TYPE_SPECIAL_TFT_3 = 3,
	JZ_LCD_TYPE_NON_INTERLACED_CCIR656 = 5,
	JZ_LCD_TYPE_INTERLACED_CCIR656 = 7,
	JZ_LCD_TYPE_SINGLE_COLOR_STN = 8,
	JZ_LCD_TYPE_SINGLE_MONOCHROME_STN = 9,
	JZ_LCD_TYPE_DUAL_COLOR_STN = 10,
	JZ_LCD_TYPE_8BIT_SERIAL = 11,
};

/*
* width: width of the lcd display in mm
* height: height of the lcd display in mm
* num_modes: size of modes
* modes: list of valid video modes
* bpp: bits per pixel for the lcd
* lcd_type: lcd type
*/

struct jz4740_fb_platform_data {
	unsigned int width;
	unsigned int height;

	size_t num_modes;
	struct fb_videomode *modes;
	int bpp;
    enum jz4740_fb_lcd_type lcd_type;
};

#endif
