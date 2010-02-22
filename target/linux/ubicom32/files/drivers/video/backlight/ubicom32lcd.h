/*
 * ubicom32lcd.h
 *	Ubicom32 lcd panel drivers
 *
 * (C) Copyright 2009, Ubicom, Inc.
 *
 * This file is part of the Ubicom32 Linux Kernel Port.
 *
 * This Ubicom32 library is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 *
 * This Ubicom32 library is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Ubicom32 Linux Kernel Port.  If not,
 * see <http://www.gnu.org/licenses/>.
 */

#ifndef _UBICOM32LCD_H_
#define _UBICOM32LCD_H_

enum ubicom32lcd_op {
	/*
	 * Sleep for (data) ms
	 */
	LCD_STEP_SLEEP,

	/*
	 * Execute write of command
	 */
	LCD_STEP_CMD,

	/*
	 * Execute write of data
	 */
	LCD_STEP_DATA,

	/*
	 * Execute write of command/data
	 */
	LCD_STEP_CMD_DATA,

	/*
	 * Script done
	 */
	LCD_STEP_DONE,
};

struct ubicom32lcd_step {
	enum ubicom32lcd_op		op;
	u16				cmd;
	u16				data;
};

struct ubicom32lcd_panel {
	const struct ubicom32lcd_step	*init_seq;
	const char			*desc;

	u32				xres;
	u32				yres;
	u32				stride;
	u32				flags;

	u16				id;
	u16				horz_reg;
	u16				vert_reg;
	u16				gram_reg;
};

#ifdef CONFIG_LCD_UBICOM32_CFAF240320KTTS
static const struct ubicom32lcd_step cfaf240320ktts_init_0[] = {
	{LCD_STEP_CMD_DATA, 0x0001, 0x0000,}, // Driver Output Control Register (R01h)		Page 14, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0002, 0x0700,}, // LCD Driving Waveform Control (R02h)		Page 15, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0003, 0x50A0,}, // Entry Mode (R03h)				0 degrees
	{LCD_STEP_CMD_DATA, 0x0004, 0x0000,}, // Scaling Control register (R04h)		Page 16, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0008, 0x0207,}, // Display Control 2 (R08h)			Page 17, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0009, 0x0000,}, // Display Control 3 (R09h)			Page 18, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x000A, 0x0000,}, // Frame Cycle Control (R0Ah)			Page 19, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x000C, 0x0000,}, // External Display Interface Control 1 (R0Ch)	Page 20, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x000D, 0x0000,}, // Frame Maker Position (R0Dh)			Page 21, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x000F, 0x0000,}, // External Display Interface Control 2 (R0Fh)	Page 21, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0010, 0x0000,}, // Power Control 1 (R10h)				Page 22, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0011, 0x0007,}, // Power Control 2 (R11h)				Page 23, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0012, 0x0000,}, // Power Control 3 (R12h)				Page 24, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0013, 0x0000,}, // Power Control 4 (R13h)				Page 25, SPFD5408B Datasheet
	{LCD_STEP_SLEEP, 0, 200},
	{LCD_STEP_CMD_DATA, 0x0007, 0x0101,}, // Display Control (R07h)				Page 16, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0010, 0x12B0,}, // Power Control 1 (R10h)				Page 22, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0011, 0x0007,}, // Power Control 2 (R11h)				Page 23, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0012, 0x01BB,}, // Power Control 3 (R12h)				Page 24, SPFD5408B Datasheet
	{LCD_STEP_SLEEP, 0, 50},
	{LCD_STEP_CMD_DATA, 0x0013, 0x1300,}, // Power Control 4 (R13h)				Page 25, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0029, 0x0010,}, // NVM read data 2 (R29h)				Page 30, SPFD5408B Datasheet
	{LCD_STEP_SLEEP, 0, 50},
	{LCD_STEP_CMD_DATA, 0x0030, 0x000A,}, // Gamma Control 1       			 	Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0031, 0x1326,}, // Gamma Control 2       			 	Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0032, 0x0A29,}, // Gamma Control 3       			 	Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0033, 0x290A,}, // Gamma Control 4       			 	Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0034, 0x2613,}, // Gamma Control 5       			 	Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0035, 0x0A0A,}, // Gamma Control 6       			 	Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0036, 0x1E03,}, // Gamma Control 7       			 	Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0037, 0x031E,}, // Gamma Control 8       			 	Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0038, 0x0706,}, // Gamma Control 9       			 	Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0039, 0x0303,}, // Gamma Control 10      			 	Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x003A, 0x0E04,}, // Gamma Control 11      			 	Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x003B, 0x0E01,}, // Gamma Control 12      			 	Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x003C, 0x010E,}, // Gamma Control 13      			 	Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x003D, 0x040E,}, // Gamma Control 14      			 	Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x003E, 0x0303,}, // Gamma Control 15      			 	Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x003F, 0x0607,}, // Gamma Control 16      			 	Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0050, 0x0000,}, // Window Horizontal RAM Address Start (R50h)	Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0051, 0x00EF,}, // Window Horizontal RAM Address End (R51h)	Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0052, 0x0000,}, // Window Vertical RAM Address Start (R52h)	Page 33, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0053, 0x013F,}, // Window Vertical RAM Address End (R53h)		Page 33, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0060, 0x2700,}, // Driver Output Control (R60h)			Page 33, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0061, 0x0001,}, // Driver Output Control (R61h)			Page 35, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x006A, 0x0000,}, // Vertical Scroll Control (R6Ah)			Page 35, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0080, 0x0000,}, // Display Position - Partial Display 1 (R80h)	Page 35, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0081, 0x0000,}, // RAM Address Start - Partial Display 1 (R81h)	Page 35, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0082, 0x0000,}, // RAM Address End - Partial Display 1 (R82h)	Page 36, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0083, 0x0000,}, // Display Position - Partial Display 2 (R83h)	Page 36, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0084, 0x0000,}, // RAM Address Start - Partial Display 2 (R84h)	Page 36, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0085, 0x0000,}, // RAM Address End - Partial Display 2 (R85h)	Page 36, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0090, 0x0010,}, // Panel Interface Control 1 (R90h)		Page 36, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0092, 0x0000,}, // Panel Interface Control 2 (R92h)		Page 37, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0093, 0x0103,}, // Panel Interface control 3 (R93h)		Page 38, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0095, 0x0210,}, // Panel Interface control 4 (R95h)		Page 38, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0097, 0x0000,}, // Panel Interface Control 5 (R97h)		Page 40, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0098, 0x0000,}, // Panel Interface Control 6 (R98h)		Page 41, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0007, 0x0173,}, // Display Control (R07h)				Page 16, SPFD5408B Datasheet
	{LCD_STEP_DONE, 0, 0},
};

const struct ubicom32lcd_panel cfaf240320ktts_0 = {
	.desc		= "CFAF240320KTTS",
	.init_seq	= cfaf240320ktts_init_0,
	.horz_reg	= 0x20,
	.vert_reg	= 0x21,
	.gram_reg	= 0x22,
	.xres		= 240,
	.yres		= 320,
	.stride		= 240,
	.id		= 0x5408,
};
#endif

#ifdef CONFIG_LCD_UBICOM32_CFAF240320KTTS_180
static const struct ubicom32lcd_step cfaf240320ktts_init_180[] = {
	{LCD_STEP_CMD_DATA, 0x0001, 0x0000,}, // Driver Output Control Register (R01h)		Page 14, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0002, 0x0700,}, // LCD Driving Waveform Control (R02h)		Page 15, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0003, 0x5000,}, // Entry Mode (R03h)				180 degrees
	{LCD_STEP_CMD_DATA, 0x0004, 0x0000,}, // Scaling Control register (R04h)		Page 16, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0008, 0x0207,}, // Display Control 2 (R08h)			Page 17, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0009, 0x0000,}, // Display Control 3 (R09h)			Page 18, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x000A, 0x0000,}, // Frame Cycle Control (R0Ah)			Page 19, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x000C, 0x0000,}, // External Display Interface Control 1 (R0Ch)	Page 20, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x000D, 0x0000,}, // Frame Maker Position (R0Dh)			Page 21, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x000F, 0x0000,}, // External Display Interface Control 2 (R0Fh)	Page 21, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0010, 0x0000,}, // Power Control 1 (R10h)				Page 22, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0011, 0x0007,}, // Power Control 2 (R11h)				Page 23, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0012, 0x0000,}, // Power Control 3 (R12h)				Page 24, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0013, 0x0000,}, // Power Control 4 (R13h)				Page 25, SPFD5408B Datasheet
	{LCD_STEP_SLEEP, 0, 200},
	{LCD_STEP_CMD_DATA, 0x0007, 0x0101,}, // Display Control (R07h)				Page 16, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0010, 0x12B0,}, // Power Control 1 (R10h)				Page 22, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0011, 0x0007,}, // Power Control 2 (R11h)				Page 23, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0012, 0x01BB,}, // Power Control 3 (R12h)				Page 24, SPFD5408B Datasheet
	{LCD_STEP_SLEEP, 0, 50},
	{LCD_STEP_CMD_DATA, 0x0013, 0x1300,}, // Power Control 4 (R13h)				Page 25, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0029, 0x0010,}, // NVM read data 2 (R29h)				Page 30, SPFD5408B Datasheet
	{LCD_STEP_SLEEP, 0, 50},
	{LCD_STEP_CMD_DATA, 0x0030, 0x000A,}, // Gamma Control 1       			 	Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0031, 0x1326,}, // Gamma Control 2       			 	Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0032, 0x0A29,}, // Gamma Control 3       			 	Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0033, 0x290A,}, // Gamma Control 4       			 	Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0034, 0x2613,}, // Gamma Control 5       			 	Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0035, 0x0A0A,}, // Gamma Control 6       			 	Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0036, 0x1E03,}, // Gamma Control 7       			 	Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0037, 0x031E,}, // Gamma Control 8       			 	Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0038, 0x0706,}, // Gamma Control 9       			 	Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0039, 0x0303,}, // Gamma Control 10      			 	Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x003A, 0x0E04,}, // Gamma Control 11      			 	Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x003B, 0x0E01,}, // Gamma Control 12      			 	Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x003C, 0x010E,}, // Gamma Control 13      			 	Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x003D, 0x040E,}, // Gamma Control 14      			 	Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x003E, 0x0303,}, // Gamma Control 15      			 	Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x003F, 0x0607,}, // Gamma Control 16      			 	Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0050, 0x0000,}, // Window Horizontal RAM Address Start (R50h)	Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0051, 0x00EF,}, // Window Horizontal RAM Address End (R51h)	Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0052, 0x0000,}, // Window Vertical RAM Address Start (R52h)	Page 33, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0053, 0x013F,}, // Window Vertical RAM Address End (R53h)		Page 33, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0060, 0x2700,}, // Driver Output Control (R60h)			Page 33, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0061, 0x0001,}, // Driver Output Control (R61h)			Page 35, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x006A, 0x0000,}, // Vertical Scroll Control (R6Ah)			Page 35, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0080, 0x0000,}, // Display Position - Partial Display 1 (R80h)	Page 35, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0081, 0x0000,}, // RAM Address Start - Partial Display 1 (R81h)	Page 35, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0082, 0x0000,}, // RAM Address End - Partial Display 1 (R82h)	Page 36, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0083, 0x0000,}, // Display Position - Partial Display 2 (R83h)	Page 36, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0084, 0x0000,}, // RAM Address Start - Partial Display 2 (R84h)	Page 36, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0085, 0x0000,}, // RAM Address End - Partial Display 2 (R85h)	Page 36, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0090, 0x0010,}, // Panel Interface Control 1 (R90h)		Page 36, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0092, 0x0000,}, // Panel Interface Control 2 (R92h)		Page 37, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0093, 0x0103,}, // Panel Interface control 3 (R93h)		Page 38, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0095, 0x0210,}, // Panel Interface control 4 (R95h)		Page 38, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0097, 0x0000,}, // Panel Interface Control 5 (R97h)		Page 40, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0098, 0x0000,}, // Panel Interface Control 6 (R98h)		Page 41, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0007, 0x0173,}, // Display Control (R07h)				Page 16, SPFD5408B Datasheet
	{LCD_STEP_DONE, 0, 0},
};

const struct ubicom32lcd_panel cfaf240320ktts_180 = {
	.desc		= "CFAF240320KTTS 180",
	.init_seq	= cfaf240320ktts_init_180,
	.horz_reg	= 0x20,
	.vert_reg	= 0x21,
	.gram_reg	= 0x22,
	.xres		= 240,
	.yres		= 320,
	.stride		= 240,
	.id		= 0x5408,
};
#endif

#ifdef CONFIG_LCD_UBICOM32_TFT2N0369E_P
static const struct ubicom32lcd_step tft2n0369ep_init[] = {
	{LCD_STEP_CMD_DATA, 0x0028, 0x0006},
	{LCD_STEP_CMD_DATA, 0x0000, 0x0001},
	{LCD_STEP_SLEEP, 0, 15},
	{LCD_STEP_CMD_DATA, 0x002B, 0x9532},
	{LCD_STEP_CMD_DATA, 0x0003, 0xAAAC},
	{LCD_STEP_CMD_DATA, 0x000C, 0x0002},
	{LCD_STEP_CMD_DATA, 0x000D, 0x000A},
	{LCD_STEP_CMD_DATA, 0x000E, 0x2C00},
	{LCD_STEP_CMD_DATA, 0x001E, 0x00AA},
	{LCD_STEP_CMD_DATA, 0x0025, 0x8000},
	{LCD_STEP_SLEEP, 0, 15},
	{LCD_STEP_CMD_DATA, 0x0001, 0x2B3F},
	{LCD_STEP_CMD_DATA, 0x0002, 0x0600},
	{LCD_STEP_CMD_DATA, 0x0010, 0x0000},
	{LCD_STEP_CMD_DATA, 0x0011, 0x6030},
	{LCD_STEP_SLEEP, 0, 20},
	{LCD_STEP_CMD_DATA, 0x0005, 0x0000},
	{LCD_STEP_CMD_DATA, 0x0006, 0x0000},
	{LCD_STEP_CMD_DATA, 0x0016, 0xEF1C},
	{LCD_STEP_CMD_DATA, 0x0017, 0x0003},
	{LCD_STEP_CMD_DATA, 0x0007, 0x0233},
	{LCD_STEP_CMD_DATA, 0x000B, 0x5312},
	{LCD_STEP_CMD_DATA, 0x000F, 0x0000},
	{LCD_STEP_SLEEP, 0, 20},
	{LCD_STEP_CMD_DATA, 0x0041, 0x0000},
	{LCD_STEP_CMD_DATA, 0x0042, 0x0000},
	{LCD_STEP_CMD_DATA, 0x0048, 0x0000},
	{LCD_STEP_CMD_DATA, 0x0049, 0x013F},
	{LCD_STEP_CMD_DATA, 0x0044, 0xEF00},
	{LCD_STEP_CMD_DATA, 0x0045, 0x0000},
	{LCD_STEP_CMD_DATA, 0x0046, 0x013F},
	{LCD_STEP_CMD_DATA, 0x004A, 0x0000},
	{LCD_STEP_CMD_DATA, 0x004B, 0x0000},
	{LCD_STEP_SLEEP, 0, 20},
	{LCD_STEP_CMD_DATA, 0x0030, 0x0707},
	{LCD_STEP_CMD_DATA, 0x0031, 0x0704},
	{LCD_STEP_CMD_DATA, 0x0032, 0x0204},
	{LCD_STEP_CMD_DATA, 0x0033, 0x0201},
	{LCD_STEP_CMD_DATA, 0x0034, 0x0203},
	{LCD_STEP_CMD_DATA, 0x0035, 0x0204},
	{LCD_STEP_CMD_DATA, 0x0036, 0x0204},
	{LCD_STEP_CMD_DATA, 0x0037, 0x0502},
	{LCD_STEP_CMD_DATA, 0x003A, 0x0302},
	{LCD_STEP_CMD_DATA, 0x003B, 0x0500},
	{LCD_STEP_SLEEP, 0, 20},
	{LCD_STEP_CMD_DATA, 0x0044, 239 << 8 | 0},
	{LCD_STEP_CMD_DATA, 0x0045, 0x0000},
	{LCD_STEP_CMD_DATA, 0x0046, 319},
	{LCD_STEP_DONE, 0, 0},
};

const struct ubicom32lcd_panel tft2n0369ep = {
	.desc		= "TFT2N0369E-Portrait",
	.init_seq	= tft2n0369ep_init,
	.horz_reg	= 0x4e,
	.vert_reg	= 0x4f,
	.gram_reg	= 0x22,
	.xres		= 240,
	.yres		= 320,
	.stride		= 240,
	.id		= 0x8989,
};
#endif

#ifdef CONFIG_LCD_UBICOM32_TFT2N0369E_L
static const struct ubicom32lcd_step tft2n0369e_init[] = {
	{LCD_STEP_CMD_DATA, 0x0028, 0x0006},
	{LCD_STEP_CMD_DATA, 0x0000, 0x0001},
	{LCD_STEP_SLEEP, 0, 15},
	{LCD_STEP_CMD_DATA, 0x002B, 0x9532},
	{LCD_STEP_CMD_DATA, 0x0003, 0xAAAC},
	{LCD_STEP_CMD_DATA, 0x000C, 0x0002},
	{LCD_STEP_CMD_DATA, 0x000D, 0x000A},
	{LCD_STEP_CMD_DATA, 0x000E, 0x2C00},
	{LCD_STEP_CMD_DATA, 0x001E, 0x00AA},
	{LCD_STEP_CMD_DATA, 0x0025, 0x8000},
	{LCD_STEP_SLEEP, 0, 15},
	{LCD_STEP_CMD_DATA, 0x0001, 0x2B3F},
	{LCD_STEP_CMD_DATA, 0x0002, 0x0600},
	{LCD_STEP_CMD_DATA, 0x0010, 0x0000},
	{LCD_STEP_CMD_DATA, 0x0011, 0x60A8},
	{LCD_STEP_SLEEP, 0, 20},
	{LCD_STEP_CMD_DATA, 0x0005, 0x0000},
	{LCD_STEP_CMD_DATA, 0x0006, 0x0000},
	{LCD_STEP_CMD_DATA, 0x0016, 0xEF1C},
	{LCD_STEP_CMD_DATA, 0x0017, 0x0003},
	{LCD_STEP_CMD_DATA, 0x0007, 0x0233},
	{LCD_STEP_CMD_DATA, 0x000B, 0x5312},
	{LCD_STEP_CMD_DATA, 0x000F, 0x0000},
	{LCD_STEP_SLEEP, 0, 20},
	{LCD_STEP_CMD_DATA, 0x0041, 0x0000},
	{LCD_STEP_CMD_DATA, 0x0042, 0x0000},
	{LCD_STEP_CMD_DATA, 0x0048, 0x0000},
	{LCD_STEP_CMD_DATA, 0x0049, 0x013F},
	{LCD_STEP_CMD_DATA, 0x0044, 0xEF00},
	{LCD_STEP_CMD_DATA, 0x0045, 0x0000},
	{LCD_STEP_CMD_DATA, 0x0046, 0x013F},
	{LCD_STEP_CMD_DATA, 0x004A, 0x0000},
	{LCD_STEP_CMD_DATA, 0x004B, 0x0000},
	{LCD_STEP_SLEEP, 0, 20},
	{LCD_STEP_CMD_DATA, 0x0030, 0x0707},
	{LCD_STEP_CMD_DATA, 0x0031, 0x0704},
	{LCD_STEP_CMD_DATA, 0x0032, 0x0204},
	{LCD_STEP_CMD_DATA, 0x0033, 0x0201},
	{LCD_STEP_CMD_DATA, 0x0034, 0x0203},
	{LCD_STEP_CMD_DATA, 0x0035, 0x0204},
	{LCD_STEP_CMD_DATA, 0x0036, 0x0204},
	{LCD_STEP_CMD_DATA, 0x0037, 0x0502},
	{LCD_STEP_CMD_DATA, 0x003A, 0x0302},
	{LCD_STEP_CMD_DATA, 0x003B, 0x0500},
	{LCD_STEP_SLEEP, 0, 20},
	{LCD_STEP_CMD_DATA, 0x0044, 239 << 8 | 0},
	{LCD_STEP_CMD_DATA, 0x0045, 0x0000},
	{LCD_STEP_CMD_DATA, 0x0046, 319},
	{LCD_STEP_DONE, 0, 0},
};

const struct ubicom32lcd_panel tft2n0369e = {
	.desc		= "TFT2N0369E-Landscape",
	.init_seq	= tft2n0369e_init,
	.horz_reg	= 0x4e,
	.vert_reg	= 0x4f,
	.gram_reg	= 0x22,
	.xres		= 320,
	.yres		= 240,
	.stride		= 320,
	.id		= 0x8989,
};
#endif

#ifdef CONFIG_LCD_UBICOM32_CFAF240400D
static const struct ubicom32lcd_step cfaf240400d_init[] = {
	{LCD_STEP_CMD_DATA, 0x0606, 0x0000},	// Pin Control (R606h)		// Page 41 of SPFD5420A Datasheet
	{LCD_STEP_SLEEP, 0, 50},
	{LCD_STEP_CMD_DATA, 0x0007, 0x0001},	// Display Control 1 (R007h)	// Page 16 of SPFD5420A Datasheet
	{LCD_STEP_SLEEP, 0, 50},
	{LCD_STEP_CMD_DATA, 0x0110, 0x0001},	// Power Control 6(R110h)	// Page 30 of SPFD5420A Datasheet
	{LCD_STEP_SLEEP, 0, 50},
	{LCD_STEP_CMD_DATA, 0x0100, 0x17B0},	// Power Control 1 (R100h)	// Page 26 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0101, 0x0147},	// Power Control 2 (R101h)	// Page 27 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0102, 0x019D},	// Power Control 3 (R102h)	// Page 28 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0103, 0x3600},	// Power Control 4 (R103h)	// Page 29 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0281, 0x0010},	// NVM read data 2 (R281h)  	// Page 34 of SPFD5420A Datasheet
	{LCD_STEP_SLEEP, 0, 50},
	{LCD_STEP_CMD_DATA, 0x0102, 0x01BD},	// Power Control 3 (R102h)	// Page 28 of SPFD5420A Datasheet
	{LCD_STEP_SLEEP, 0, 50},

	//--------------- Power control 1~6 ---------------//
	{LCD_STEP_CMD_DATA, 0x0100, 0x16B0},	// Power Control 1 (R100h)	// Page 26 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0101, 0x0147},	// Power Control 2 (R101h)	// Page 27 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0102, 0x01BD},	// Power Control 3 (R102h)	// Page 28 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0103, 0x2d00},	// Power Control 4 (R103h)	// Page 29 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0107, 0x0000},	// Power Control 5 (R107h)	// Page 30 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0110, 0x0001},	// Power Control 6(R110h)	// Page 30 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0280, 0x0000},	// NVM read data 1 (R280h)	// Page 33 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0281, 0x0006},	// NVM read data 2 (R281h)	// Page 34 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0282, 0x0000},	// NVM read data 3 (R282h)	// Page 34 of SPFD5420A Datasheet

	//------- Gamma 2.2 control (R300h to R30Fh) ------//
	{LCD_STEP_CMD_DATA, 0x0300, 0x0101},
	{LCD_STEP_CMD_DATA, 0x0301, 0x0b27},
	{LCD_STEP_CMD_DATA, 0x0302, 0x132a},
	{LCD_STEP_CMD_DATA, 0x0303, 0x2a13},
	{LCD_STEP_CMD_DATA, 0x0304, 0x270b},
	{LCD_STEP_CMD_DATA, 0x0305, 0x0101},
	{LCD_STEP_CMD_DATA, 0x0306, 0x1205},
	{LCD_STEP_CMD_DATA, 0x0307, 0x0512},
	{LCD_STEP_CMD_DATA, 0x0308, 0x0005},
	{LCD_STEP_CMD_DATA, 0x0309, 0x0003},
	{LCD_STEP_CMD_DATA, 0x030A, 0x0f04},
	{LCD_STEP_CMD_DATA, 0x030B, 0x0f00},
	{LCD_STEP_CMD_DATA, 0x030C, 0x000f},
	{LCD_STEP_CMD_DATA, 0x030D, 0x040f},
	{LCD_STEP_CMD_DATA, 0x030E, 0x0300},
	{LCD_STEP_CMD_DATA, 0x030F, 0x0500},

	{LCD_STEP_CMD_DATA, 0x0400, 0x3500},	// Base Image Number of Line (R400h)		// Page 36 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0401, 0x0001},	// Base Image Display Control (R401h)		// Page 39 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0404, 0x0000},	// Based Image Vertical Scroll Control (R404h)	// Page 40 of SPFD5420A Datasheet

	//--------------- Normal set ---------------//
	{LCD_STEP_CMD_DATA, 0x0000, 0x0000},	// ID Read Register (R000h)			// Page 13 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0001, 0x0100},	// Driver Output Control Register (R001h)	// Page 14 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0002, 0x0100},	// LCD Driving Waveform Control (R002h)		// Page 14 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0003, 0x1030},	// Entry Mode (R003h)				// Page 15 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0006, 0x0000},	// Display Control 1 (R007h)			// Page 16 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0008, 0x0808},	// Display Control 2 (R008h)			// Page 17 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0009, 0x0001},	// Display Control 3 (R009h)			// Page 18 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x000B, 0x0010},	// Low Power Control (R00Bh)			// Page 19 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x000C, 0x0000},	// External Display Interface Control 1 (R00Ch)	// Page 19 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x000F, 0x0000},	// External Display Interface Control 2 (R00Fh)	// Page 20 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0007, 0x0001},	// Display Control 1 (R007h)			// Page 16 of SPFD5420A Datasheet

	//--------------- Panel interface control 1~6 ---------------//
	{LCD_STEP_CMD_DATA, 0x0010, 0x0012},	// Panel Interface Control 1 (R010h)		// Page 20 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0011, 0x0202},	// Panel Interface Control 2 (R011h)		// Page 21 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0012, 0x0300},	// Panel Interface control 3 (R012h)		// Page 22 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0020, 0x021E},	// Panel Interface control 4 (R020h)		// Page 22 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0021, 0x0202},	// Panel Interface Control 5 (021Rh)		// Page 24 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0022, 0x0100},	// Panel Interface Control 6 (R022h)		// Page 25 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0090, 0x8000},	// Frame Marker Control (R090h)			// Page 25 of SPFD5420A Datasheet

	//--------------- Partial display ---------------//
	{LCD_STEP_CMD_DATA, 0x0210, 0x0000},	// Window Horizontal RAM Address Start (R210h)	// Page 35 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0211, 0x00EF},	// Window Horziontal RAM Address End (R211h)	// Page 35 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0212, 0x0000},	// Window Vertical RAM Address Start (R212h)	// Page 35 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0213, 0x018F},	// Window Vertical RAM Address End (R213h)	// Page 35 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0500, 0x0000},	// Display Position - Partial Display 1 (R500h)	// Page 40 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0501, 0x0000},	// RAM Address Start - Partial Display 1 (R501h)// Page 40 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0502, 0x0000},	// RAM Address End - Partail Display 1 (R502h)	// Page 40 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0503, 0x0000},	// Display Position - Partial Display 2 (R503h)	// Page 40 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0504, 0x0000},	// RAM Address Start . Partial Display 2 (R504h)// Page 41 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0505, 0x0000},	// RAM Address End . Partial Display 2 (R505h)	// Page 41 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0606, 0x0000},	// Pin Control (R606h)				// Page 41 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x06F0, 0x0000},	// NVM Access Control (R6F0h)			// Page 41 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0007, 0x0173},	// Display Control 1 (R007h)			// Page 16 of SPFD5420A Datasheet
	{LCD_STEP_SLEEP, 0, 50},
	{LCD_STEP_CMD_DATA, 0x0007, 0x0171},	// Display Control 1 (R007h)			// Page 16 of SPFD5420A Datasheet
	{LCD_STEP_SLEEP, 0, 10},
	{LCD_STEP_CMD_DATA, 0x0007, 0x0173},	// Display Control 1 (R007h)			// Page 16 of SPFD5420A Datasheet
	{LCD_STEP_DONE, 0, 0},
};

const struct ubicom32lcd_panel cfaf240400d = {
	.desc		= "CFAF240400D",
	.init_seq	= cfaf240400d_init,
	.horz_reg	= 0x0200,
	.vert_reg	= 0x0201,
	.gram_reg	= 0x0202,
	.xres		= 240,
	.yres		= 400,
	.stride		= 240,
	.id		= 0x5420,
};
#endif

#ifdef CONFIG_LCD_UBICOM32_CFAF240400F
static const struct ubicom32lcd_step cfaf320240f_init[] = {
	{LCD_STEP_CMD_DATA, 0x0028, 0x0006},	// VCOM OTP			 	Page 55-56 of SSD2119 datasheet
	{LCD_STEP_CMD_DATA, 0x0000, 0x0001},	// start Oscillator		 	Page 36 of SSD2119 datasheet
	{LCD_STEP_CMD_DATA, 0x0010, 0x0000},	// Sleep mode			 	Page 49 of SSD2119 datasheet
	{LCD_STEP_CMD_DATA, 0x0001, 0x32EF},	// Driver Output Control	 	Page 36-39 of SSD2119 datasheet
	{LCD_STEP_CMD_DATA, 0x0002, 0x0600},	// LCD Driving Waveform Control	 	Page 40-42 of SSD2119 datasheet
	{LCD_STEP_CMD_DATA, 0x0003, 0x6A38},	// Power Control 1		 	Page 43-44 of SSD2119 datasheet
	{LCD_STEP_CMD_DATA, 0x0011, 0x6870},	// Entry Mode			 	Page 50-52 of SSD2119 datasheet
	{LCD_STEP_CMD_DATA, 0X000F, 0x0000},	// Gate Scan Position		 	Page 49 of SSD2119 datasheet
	{LCD_STEP_CMD_DATA, 0X000B, 0x5308},	// Frame Cycle Control		 	Page 45 of SSD2119 datasheet
	{LCD_STEP_CMD_DATA, 0x000C, 0x0003},	// Power Control 2		 	Page 47 of SSD2119 datasheet
	{LCD_STEP_CMD_DATA, 0x000D, 0x000A},	// Power Control 3		 	Page 48 of SSD2119 datasheet
	{LCD_STEP_CMD_DATA, 0x000E, 0x2E00},	// Power Control 4		 	Page 48 of SSD2119 datasheet
	{LCD_STEP_CMD_DATA, 0x001E, 0x00BE},	// Power Control 5		 	Page 53 of SSD2119 datasheet
	{LCD_STEP_CMD_DATA, 0x0025, 0x8000},	// Frame Frequency Control	 	Page 53 of SSD2119 datasheet
	{LCD_STEP_CMD_DATA, 0x0026, 0x7800},	// Analog setting		 	Page 54 of SSD2119 datasheet
	{LCD_STEP_CMD_DATA, 0x004E, 0x0000},	// Ram Address Set		 	Page 58 of SSD2119 datasheet
	{LCD_STEP_CMD_DATA, 0x004F, 0x0000},	// Ram Address Set		 	Page 58 of SSD2119 datasheet
	{LCD_STEP_CMD_DATA, 0x0012, 0x08D9},	// Sleep mode			 	Page 49 of SSD2119 datasheet

	// Gamma Control (R30h to R3Bh) -- Page 56 of SSD2119 datasheet
	{LCD_STEP_CMD_DATA, 0x0030, 0x0000},
	{LCD_STEP_CMD_DATA, 0x0031, 0x0104},
	{LCD_STEP_CMD_DATA, 0x0032, 0x0100},
	{LCD_STEP_CMD_DATA, 0x0033, 0x0305},
	{LCD_STEP_CMD_DATA, 0x0034, 0x0505},
	{LCD_STEP_CMD_DATA, 0x0035, 0x0305},
	{LCD_STEP_CMD_DATA, 0x0036, 0x0707},
	{LCD_STEP_CMD_DATA, 0x0037, 0x0300},
	{LCD_STEP_CMD_DATA, 0x003A, 0x1200},
	{LCD_STEP_CMD_DATA, 0x003B, 0x0800},

	{LCD_STEP_CMD_DATA, 0x0007, 0x0033},	// Display Control 			 Page 45 of SSD2119 datasheet

	{LCD_STEP_CMD_DATA, 0x0044, 0xEF00},	// Vertical RAM address position	 Page 57 of SSD2119 datasheet
	{LCD_STEP_CMD_DATA, 0x0045, 0x0000},	// Horizontal RAM address position 	 Page 57 of SSD2119 datasheet
	{LCD_STEP_CMD_DATA, 0x0046, 0x013F},	// Horizontal RAM address position	 Page 57 of SSD2119 datasheet

	{LCD_STEP_SLEEP, 0, 150},

	{LCD_STEP_DONE, 0, 0},
};

const struct ubicom32lcd_panel cfaf320240f = {
	.desc		= "CFAF320240F",
	.init_seq	= cfaf320240f_init,
	.horz_reg	= 0x4e,
	.vert_reg	= 0x4f,
	.gram_reg	= 0x22,
	.xres		= 320,
	.yres		= 240,
	.stride		= 320,
	.id		= 0x9919,
};
#endif

const struct ubicom32lcd_panel *ubicom32lcd_panels[] = {
#ifdef CONFIG_LCD_UBICOM32_CFAF240400KTTS_180
	&cfaf240320ktts_180,
#endif
#ifdef CONFIG_LCD_UBICOM32_CFAF240400KTTS
	&cfaf240320ktts_0,
#endif
#ifdef CONFIG_LCD_UBICOM32_CFAF240400D
	&cfaf240400d,
#endif
#ifdef CONFIG_LCD_UBICOM32_TFT2N0369E_P
	&tft2n0369ep,
#endif
#ifdef CONFIG_LCD_UBICOM32_TFT2N0369E_L
	&tft2n0369e,
#endif
#ifdef CONFIG_LCD_UBICOM32_CFAF240400F
	&cfaf320240f,
#endif
	NULL,
};

#endif
