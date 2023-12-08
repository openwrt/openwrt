/******************************************************************************
 *
 * Copyright(c) 2007 - 2020  Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * The full GNU General Public License is included in this distribution in the
 * file called LICENSE.
 *
 * Contact Information:
 * wlanfae <wlanfae@realtek.com>
 * Realtek Corporation, No. 2, Innovation Road II, Hsinchu Science Park,
 * Hsinchu 300, Taiwan.
 *
 * Larry Finger <Larry.Finger@lwfinger.net>
 *
 *****************************************************************************/
#ifndef __HALBB_MATH_LIB_H__
#define __HALBB_MATH_LIB_H__

/*@--------------------------[Define] ---------------------------------------*/
#define	MASKBYTE0		0xff
#define	MASKBYTE1		0xff00
#define	MASKBYTE2		0xff0000
#define	MASKBYTE3		0xff000000
#define	MASKHWORD		0xffff0000
#define	MASKLWORD		0x0000ffff
#define	MASKDWORD		0xffffffff
#define RFREGOFFSETMASK		0xfffff

#define KB_2_MB(val) ((val) >> 10)

#define MAX_2(_x_, _y_)	(((_x_) > (_y_)) ? (_x_) : (_y_))
#define MIN_2(_x_, _y_)	(((_x_) < (_y_)) ? (_x_) : (_y_))
#define DIFF_2(_x_, _y_)	(((_x_) >= (_y_)) ? ((_x_) - (_y_)) : ((_y_) - (_x_)))
#define SUBTRACT_TO_0(_x_, _y_)	(((_x_) >= (_y_)) ? ((_x_) - (_y_)) : 0)

#define IS_GREATER(_x_, _y_)	(((_x_) >= (_y_)) ? true : false)
#define IS_LESS(_x_, _y_)	(((_x_) < (_y_)) ? true : false)
#define NOT_GREATER(_x_, _y_) (((_x_) > (_y_)) ? (_y_) : (_x_))


#define BYTE_DUPLICATE_2_DWORD(B0)	\
	(((B0) << 24) | ((B0) << 16) | ((B0) << 8) | (B0))

#define BYTE_2_DWORD(B3, B2, B1, B0)	\
	(((B3) << 24) | ((B2) << 16) | ((B1) << 8) | (B0))
#define BYTE_2_WORD(B1, B0)	\
	(((B1) << 8) | (B0))
#define BIT_2_BYTE(B3, B2, B1, B0)	\
	(((B3) << 3) | ((B2) << 2) | ((B1) << 1) | (B0))

#define NIBBLE_2_BYTE(B1, B0)	\
	((((B1) & 0xf) << 4) | ((B0) & 0xf))

#define NIBBLE_2_WORD(B3, B2, B1, B0)	\
	((((B3) & 0xf) << 12) | (((B2) & 0xf) << 8) |\
	(((B1) & 0xf) << 4) | ((B0) & 0xf))

#define NIBBLE_2_DWORD(B7, B6, B5, B4, B3, B2, B1, B0)	\
	((((B7) & 0xf) << 28) | (((B6) & 0xf) << 24) | \
	(((B5) & 0xf) << 20) | (((B4) & 0xf) << 16) | \
	(((B3) & 0xf) << 12) | (((B2) & 0xf) << 8) | \
	(((B1) & 0xf) << 4) | ((B0) & 0xf))

#if PLATFOM_IS_LITTLE_ENDIAN
#define SWAP4BYTE(x) ((u32)(x))
#else
#define SWAP4BYTE(x)                                       \
	((u32)((((u32)(x) & (u32)0x000000ff) << 24) |  \
	       (((u32)(x) & (u32)0x0000ff00) << 8) |     \
	       (((u32)(x) & (u32)0x00ff0000) >> 8) |     \
	       (((u32)(x) & (u32)0xff000000) >> 24)))
#endif

#define HALBB_DIV(a, b) ((b) ? ((a) / (b)) : 0)
#define HALBB_DIV_U64(a, b) ((b) ? (_os_division64((u64)(a), (u64)(b))) : 0)

#define HALBB_DIV_ROUND(a, b) ((b) ? (((a) + (b / 2)) / (b)) : 0)
#define HALBB_CEIL(a, b) ((b) ? ((a) / (b) + (a > (a / b) * b)) : 0)
#define ABS_32(X) (((X) & BIT(31)) ? (0 - (X)) : (X))
#define ABS_16(X) (((X) & BIT(15)) ? (0 - (X)) : (X))
#define ABS_8(X) (((X) & BIT(7)) ? (0 - (X)) : (X))
#define DIVIDED_2(X) ((X) >> 1)
#define DIVIDED_3(X) (((X) + ((X) << 1) + ((X) << 3)) >> 5)
#define DIVIDED_4(X) ((X) >> 2)


#define WEIGHTING_AVG_2(v1, w1, v2, w2) \
	(((v1) * (w1) + (v2) * (w2)) / ((w2) + (w1)))
#define MA_ACC(old, new_val, ma, ma_max) ((old) - ((old << (ma_max - ma)) >> (ma_max)) + (new_val << (ma_max - ma)))
#define GET_MA_VAL(val, ma) (((val) + (1 << ((ma) - 1))) >> (ma))

#define FRAC_BITS 3


/*@--------------------------[Enum]------------------------------------------*/
 
/*@--------------------------[Structure]-------------------------------------*/
 
/*@--------------------------[Prptotype]-------------------------------------*/
struct bb_info;
s32 halbb_pwdb_conversion(s32 X, u32 total_bit, u32 decimal_bit);
s32 halbb_sign_conversion(s32 val, u32 total_bit);
u8 halbb_find_intrvl(struct bb_info *bb, u16 val, u8 *threshold, u8 len);
void halbb_seq_sorting(struct bb_info *bb, u32 *val, u32 *idx, u32 *idx_out,
		       u8 seq_length);
u16 halbb_show_fraction_num(u32 frac_val, u8 bit_num);
u16 halbb_ones_num_in_bitmap(u64 val, u8 size);
u64 halbb_gen_mask_from_0(u8 mask_num);
u64 halbb_gen_mask(u8 up_num, u8 low_num);
u32 halbb_cal_bit_shift(u32 bit_mask);
s32 halbb_cnvrt_2_sign(u32 val, u8 bit_num);
s64 halbb_cnvrt_2_sign_64(u64 val, u8 bit_num);
void halbb_print_sign_frac_digit(struct bb_info *bb, u32 val, u8 total_bit_num,
				 u8 frac_bit_num, char *buf, u16 buf_size);
char *halbb_print_sign_frac_digit2(struct bb_info *bb, u32 val, u8 total_bit_num,
				 u8 frac_bit_num);
void halbb_print_buff_64(struct bb_info *bb, u8 *addr, u16 length);
void halbb_print_buff_32(struct bb_info *bb, u8 *addr, u16 length);
void halbb_math_dbg(struct bb_info *bb, char input[][16], u32 *_used,
		    char *output, u32 *_out_len);
#endif
