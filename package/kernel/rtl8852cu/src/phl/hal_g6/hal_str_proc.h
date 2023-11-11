/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/
#include "hal_headers.h"


#define hal_getLinefrombuffer(buffer)	_os_strsep(&buffer, "\r\n")

bool hal_is_comment_string(char *szStr);

bool hal_is_alpha(char ch_tmp);

bool hal_ishexdigit(char chTmp);

bool hal_get_hexvalue_fromstring(char	*szStr, u32 *pu4bVal, u32 *pu4bMove);

bool hal_get_fractionvalue_fromstring(
		char			*szStr,
		u8				*pInteger,
		u8				*pFraction,
		u32			*pu4bMove);

bool hal_is_allspace_tab(char	*data, u8	size);

u32 hal_mapchar_tohexdigit(char 		chTmp);

bool hal_parse_fiedstring(char	*in_str, u32	*start, char	*out_str, char lqualifier, char rqualifier);

bool hal_get_u1bint_fromstr_indec(char	*str, u8	*pint);

bool hal_get_s1bint_fromstr_indec(char	*str,	s8 *val);

