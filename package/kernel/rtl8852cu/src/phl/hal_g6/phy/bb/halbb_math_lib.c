/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2021, Realtek Semiconductor Corp. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.
 *
 *   * Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   * Neither the name of the Realtek nor the names of its contributors may
 *     be used to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "halbb_precomp.h"

const u32 db_invert_table[12][8] = {
	{10, 13, 16, 20, 25, 32, 40, 50}, /* @U(32,3) */
	{64, 80, 101, 128, 160, 201, 256, 318}, /* @U(32,3) */
	{401, 505, 635, 800, 1007, 1268, 1596, 2010}, /* @U(32,3) */
	{316, 398, 501, 631, 794, 1000, 1259, 1585}, /* @U(32,0) */
	{1995, 2512, 3162, 3981, 5012, 6310, 7943, 10000}, /* @U(32,0) */
	{12589, 15849, 19953, 25119, 31623, 39811, 50119, 63098}, /* @U(32,0) */
	{79433, 100000, 125893, 158489, 199526, 251189, 316228,
	 398107}, /* @U(32,0) */
	{501187, 630957, 794328, 1000000, 1258925, 1584893, 1995262,
	 2511886}, /* @U(32,0) */
	{3162278, 3981072, 5011872, 6309573, 7943282, 1000000, 12589254,
	 15848932}, /* @U(32,0) */
	{19952623, 25118864, 31622777, 39810717, 50118723, 63095734,
	 79432823, 100000000}, /* @U(32,0) */
	{125892541, 158489319, 199526232, 251188643, 316227766, 398107171,
	 501187234, 630957345}, /* @U(32,0) */
	{794328235, 1000000000, 1258925412, 1584893192, 1995262315,
	 2511886432U, 3162277660U, 3981071706U} }; /* @U(32,0) */

/*Y = 10*log(X)*/
s32 halbb_pwdb_conversion(s32 X, u32 total_bit, u32 decimal_bit)
{
	s32 Y, integer = 0, decimal = 0;
	u32 i;

	if (X == 0)
		X = 1; /* @log2(x), x can't be 0 */

	for (i = (total_bit - 1); i > 0; i--) {
		if (X & BIT(i)) {
			integer = i;
			if (i > 0) {
				/*decimal is 0.5dB*3=1.5dB~=2dB */
				decimal = (X & BIT(i - 1)) ? 2 : 0;
			}
			break;
		}
	}

	Y = 3 * (integer - decimal_bit) + decimal; /* @10*log(x)=3*log2(x), */

	return Y;
}

s32 halbb_sign_conversion(s32 val, u32 total_bit)
{
	if (val & BIT(total_bit - 1))
		val -= BIT(total_bit);

	return val;
}

/*threshold must form low to high*/
u8 halbb_find_intrvl(struct bb_info *bb, u16 val, u8 *threshold, u8 len)
{
	u8 i = 0;
	u8 ret_val = 0;
	u8 max_th = threshold[len - 1];

	for (i = 0; i < len; i++) {
		if (val < threshold[i]) {
			ret_val = i;
			break;
		} else if (val >= max_th) {
			ret_val = len;
			break;
		}
	}

	return ret_val;
}

void halbb_seq_sorting(struct bb_info *bb, u32 *val, u32 *idx, u32 *idx_out,
		       u8 seq_length)
{
	u8 i = 0, j = 0, tmp = 0;
	u32 tmp_a, tmp_b;
	u32 tmp_idx_a, tmp_idx_b;

	for (i = 0; i < seq_length; i++)
		idx[i] = i;

	for (i = 0; i < (seq_length - 1); i++) {
		tmp = seq_length - 1 - i; /*to prevent WIN WHQL warning*/
		for (j = 0; j < tmp; j++) {

			tmp_a = val[j];
			tmp_b = val[j + 1];

			tmp_idx_a = idx[j];
			tmp_idx_b = idx[j + 1];

			if (tmp_a < tmp_b) {
				val[j] = tmp_b;
				val[j + 1] = tmp_a;

				idx[j] = tmp_idx_b;
				idx[j + 1] = tmp_idx_a;
			}
		}
	}

	for (i = 0; i < seq_length; i++)
		idx_out[idx[i]] = i + 1;
}

u32 halbb_convert_to_db(u64 val)
{
	u8 i;
	u8 j;
	u32 dB;

	if (val >= db_invert_table[11][7])
		return 96; /* @maximum 96 dB */

	for (i = 0; i < 12; i++) {
		if (i <= 2 && (val << FRAC_BITS) <= db_invert_table[i][7])
			break;
		else if (i > 2 && val <= db_invert_table[i][7])
			break;
	}

	for (j = 0; j < 8; j++) {
		if (i <= 2 && (val << FRAC_BITS) <= db_invert_table[i][j])
			break;
		else if (i > 2 && i < 12 && val <= db_invert_table[i][j])
			break;
	}

	/*special cases*/
	if (j == 0 && i == 0)
		goto end;

	if (i == 3 && j == 0) {
		if (db_invert_table[3][0] - val >
		    val - (db_invert_table[2][7] >> FRAC_BITS)) {
			i = 2;
			j = 7;
		}
		goto end;
	}

	if (i < 3)
		val = val << FRAC_BITS; /*@elements of row 0~2 shift left*/

	/*compare difference to get precise dB*/
	if (j == 0) {
		if (db_invert_table[i][j] - val >
		    val - db_invert_table[i - 1][7]) {
			i = i - 1;
			j = 7;
		}
	} else {
		if (db_invert_table[i][j] - val >
		    val - db_invert_table[i][j - 1]) {
			j = j - 1;
		}
	}
end:
	dB = (i << 3) + j + 1;

	return dB;
}

u64 halbb_db_2_linear(u32 val)
{
	u8 i = 0;
	u8 j = 0;
	u64 linear = 0;

	val = val & 0xFF;

	/* @1dB~96dB */
	if (val > 96) {
		val = 96;
	} else if (val < 1) {
		linear = 1;
		return linear;
	}

	i = (u8)((val - 1) >> 3);
	j = (u8)(val - 1) - (i << 3);

	linear = db_invert_table[i][j];

	if (i > 2)
		linear = linear << FRAC_BITS;

	return linear;
}

u16 halbb_show_fraction_num(u32 frac_val, u8 bit_num)
{
	u8 i = 0;
	u16 val = 0;
	u16 base = 500; /* Fix to 3 digit after the decimal point*/

	for (i = bit_num; i > 0; i--) {
		if (frac_val & BIT(i - 1))
			val += (base >> (bit_num - i));
	}
	return val;
}

u32 halbb_show_fraction_num_opt(u32 frac_val, u8 bit_num, u8 decimal_place)
{
	u8 i = 0;
	u32 val = 0;
	u32 base = 5;

	if (decimal_place == 0)
		return 0;

	if (decimal_place > 9)
		decimal_place = 9;

	for (i = 1; i < decimal_place; i++)
		base *= 10;

	for (i = bit_num; i > 0; i--) {
		if (frac_val & BIT(i - 1))
			val += (base >> (bit_num - i));
	}
	return val;
}

u16 halbb_ones_num_in_bitmap(u64 val, u8 size)
{
	u8 i = 0;
	u8 ones_num = 0;

	for (i = 0; i < size; i++) {
		if (val & BIT(0))
			ones_num++;

		val = val >> 1;
	}

	return ones_num;
}

u64 halbb_gen_mask_from_0(u8 mask_num)
{
	u8 i = 0;
	u64 bitmask = 0;

	if (mask_num > 64 || mask_num == 0)
		return 0;

	for (i = 0; i < mask_num; i++)
		bitmask = (bitmask << 1) | BIT(0);

	return bitmask;
}

u64 halbb_gen_mask(u8 up_num, u8 low_num)
{
	if (up_num < low_num)
		return 0;

	return (halbb_gen_mask_from_0(up_num - low_num + 1) << low_num);
}

u32 halbb_cal_bit_shift(u32 bit_mask)
{
	u32 i;

	for (i = 0; i <= 31; i++) {
		if ((bit_mask >> i) & BIT0)
			break;
	}
	return i;
}

s32 halbb_cnvrt_2_sign(u32 val, u8 bit_num)
{
	if (bit_num > 32)
		return (s32)val;

	if (val & BIT(bit_num - 1)) /*Sign BIT*/
		val -= (1 << bit_num); /*@2's*/

	return val;
}

s64 halbb_cnvrt_2_sign_64(u64 val, u8 bit_num)
{
	u64 one = 1;
	s64 val_sign = (s64)val;

	if (bit_num >= 64)
		return (s64)val;

	if (val & (one << (bit_num - 1))) /*Sign BIT*/
		val_sign = val - (one << bit_num); /*@2's*/

	return val_sign;
}

void halbb_print_sign_frac_digit(struct bb_info *bb, u32 val, u8 total_bit_num,
				 u8 frac_bit_num, char *buf, u16 buf_size)
{
	s32 val_s32 = (s32)val;
	u32 val_abs = 0;
	u32 mask_frac = 0;
	u32 frac_digit = 0;

	val_abs = ABS_32(val_s32);
	mask_frac = (u32)halbb_gen_mask_from_0(frac_bit_num);
	frac_digit = halbb_show_fraction_num(val_abs & mask_frac, frac_bit_num);

	if (frac_bit_num == 1) {
		frac_digit = (val & 0x1) * 5;
		_os_snprintf(buf, buf_size, "%s%d.%d", (val_s32 >= 0) ? "" : "-",
			     val_abs >> frac_bit_num, frac_digit);
	} else if (frac_bit_num == 2) {
		frac_digit = (val & 0x3) * 25;
		_os_snprintf(buf, buf_size, "%s%d.%02d", (val_s32 >= 0) ? "" : "-",
			     val_abs >> frac_bit_num, frac_digit);
	} else if (frac_bit_num == 3) {
		frac_digit = halbb_show_fraction_num_opt(val_abs & mask_frac, frac_bit_num, 3);
		_os_snprintf(buf, buf_size, "%s%d.%03d", (val_s32 >= 0) ? "" : "-",
			     val_abs >> frac_bit_num, frac_digit);
	} else if (frac_bit_num == 4) {
		frac_digit = halbb_show_fraction_num_opt(val_abs & mask_frac, frac_bit_num, 4);
		_os_snprintf(buf, buf_size, "%s%d.%04d", (val_s32 >= 0) ? "" : "-",
			     val_abs >> frac_bit_num, frac_digit);
	} else if (frac_bit_num == 5) {
		frac_digit = halbb_show_fraction_num_opt(val_abs & mask_frac, frac_bit_num, 5);
		_os_snprintf(buf, buf_size, "%s%d.%05d", (val_s32 >= 0) ? "" : "-",
			     val_abs >> frac_bit_num, frac_digit);
	} else {
		frac_digit = halbb_show_fraction_num_opt(val_abs & mask_frac, frac_bit_num, 8);
		_os_snprintf(buf, buf_size, "%s%d.%08d", (val_s32 >= 0) ? "" : "-",
			     val_abs >> frac_bit_num, frac_digit);
	}
}

char *halbb_print_sign_frac_digit2(struct bb_info *bb, u32 val, u8 total_bit_num,
				 u8 frac_bit_num)
{
	char *buf = bb->dbg_buf;
	u16 buf_size = HALBB_SNPRINT_SIZE;
	s32 val_s32 = (s32)val;
	u32 val_abs = 0;
	u32 mask_frac = 0;
	u16 frac_digit = 0;

	val_abs = ABS_32(val_s32);
	mask_frac = (u32)halbb_gen_mask_from_0(frac_bit_num);
	if (frac_bit_num == 1) {
		frac_digit = (val & 0x1) * 5;
		_os_snprintf(buf, buf_size, "%s%d.%d", (val_s32 >= 0) ? "" : "-",
			     val_abs >> frac_bit_num, frac_digit);
	} else if (frac_bit_num == 2) {
		frac_digit = (val & 0x3) * 25;
		_os_snprintf(buf, buf_size, "%s%d.%02d", (val_s32 >= 0) ? "" : "-",
			     val_abs >> frac_bit_num, frac_digit);
	} else {
		frac_digit = halbb_show_fraction_num(val_abs & mask_frac, frac_bit_num);
		_os_snprintf(buf, buf_size, "%s%d.%03d", (val_s32 >= 0) ? "" : "-",
			     val_abs >> frac_bit_num, frac_digit);
	}

	return buf;
}

void halbb_print_buff_64(struct bb_info *bb, u8 *addr, u16 length) /*unit: Byte*/
{
	u64 *buff_tmp = NULL;
	u8 print_len = length >> 3;
	u8 i;

	if (length % 8)
		print_len++;

	buff_tmp = (u64 *)addr;

	for (i = 0; i < print_len; i++) {
		BB_TRACE("[%02d]0x%016llx\n", i, buff_tmp[i]);
	}
}

void halbb_print_buff_32(struct bb_info *bb, u8 *addr, u16 length) /*unit: Byte*/
{
	u32 *buff_tmp = NULL;
	u8 print_len = length >> 2;
	u8 i;

	if (length % 4)
		print_len++;

	buff_tmp = (u32 *)addr;

	for (i = 0; i < print_len; i++) {
		BB_TRACE("0x%08x\n", buff_tmp[i]);
	}
}

void halbb_math_dbg(struct bb_info *bb, char input[][16], u32 *_used,
		    char *output, u32 *_out_len)
{
	s32 var[5] = {0};
	s32 tmp_32 = 0;
	u64 tmp_64 = 0;

	if (_os_strcmp(input[1], "-h") == 0) {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "div {a(hex)} {b{hex}}\n");
		return;
	}
	HALBB_SCAN(input[1], DCMD_DECIMAL, &var[0]);

	if (_os_strcmp(input[1], "div") == 0) {
		HALBB_SCAN(input[2], DCMD_HEX, &var[0]);
		HALBB_SCAN(input[3], DCMD_HEX, &var[1]);

		tmp_32 = HALBB_DIV(var[0], var[1]);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "(%d/%d) = %d\n", var[0], var[1], tmp_32);

		tmp_64 = HALBB_DIV_U64(var[0], var[1]);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "u64 (%d/%d) = %llx\n", var[0], var[1], tmp_64);

	}
}