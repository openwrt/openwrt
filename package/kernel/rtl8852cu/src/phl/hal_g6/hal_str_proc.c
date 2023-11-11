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

#include "hal_str_proc.h"


bool hal_is_comment_string(
		char			*szStr
)
{
	if (*szStr == '/' && *(szStr + 1) == '/')
		return true;
	else
		return false;
}

bool hal_get_fractionvalue_fromstring(
		char			*szStr,
		u8				*pInteger,
		u8				*pFraction,
		u32			*pu4bMove
)
{
	char	*szScan = szStr;

	/* Initialize output. */
	*pu4bMove = 0;
	*pInteger = 0;
	*pFraction = 0;

	/* Skip leading space. */
	while (*szScan != '\0' &&	(*szScan == ' ' || *szScan == '\t')) {
		++szScan;
		++(*pu4bMove);
	}

	if (*szScan < '0' || *szScan > '9')
		return false;

	/* Parse each digit. */
	do {
		(*pInteger) *= 10;
		*pInteger += (*szScan - '0');

		++szScan;
		++(*pu4bMove);

		if (*szScan == '.') {
			++szScan;
			++(*pu4bMove);

			if (*szScan < '0' || *szScan > '9')
				return false;

			*pFraction += (*szScan - '0') * 10;
			++szScan;
			++(*pu4bMove);

			if (*szScan >= '0' && *szScan <= '9') {
				*pFraction += *szScan - '0';
				++szScan;
				++(*pu4bMove);
			}
			return true;
		}
	} while (*szScan >= '0' && *szScan <= '9');

	return true;
}

bool hal_is_alpha(char ch_tmp)
{
	if ((ch_tmp >= 'a' && ch_tmp <= 'z') ||
		(ch_tmp >= 'A' && ch_tmp <= 'Z'))
		return true;
	else
		return false;
}

bool hal_ishexdigit(char ch_tmp)
{
	if ((ch_tmp >= '0' && ch_tmp <= '9') ||
		(ch_tmp >= 'a' && ch_tmp <= 'f') ||
		(ch_tmp >= 'A' && ch_tmp <= 'F'))
		return true;
	else
		return false;
}

bool hal_get_hexvalue_fromstring(
		char			*szStr,
		u32			*pu4bVal,
		u32			*pu4bMove
)
{
	char		*szScan = szStr;

	/* Check input parameter. */
	if (szStr == NULL || pu4bVal == NULL || pu4bMove == NULL) {
		PHL_INFO("GetHexValueFromString(): Invalid inpur argumetns! szStr: %p, pu4bVal: %p, pu4bMove: %p\n", szStr, pu4bVal, pu4bMove);
		return false;
	}

	/* Initialize output. */
	*pu4bMove = 0;
	*pu4bVal = 0;

	/* Skip leading space. */
	while (*szScan != '\0' &&
		(*szScan == ' ' || *szScan == '\t')) {
		szScan++;
		(*pu4bMove)++;
	}

	/* Skip leading '0x' or '0X'. */
	if (*szScan == '0' && (*(szScan + 1) == 'x' || *(szScan + 1) == 'X')) {
		szScan += 2;
		(*pu4bMove) += 2;
	}

	/* Check if szScan is now pointer to a character for hex digit, */
	/* if not, it means this is not a valid hex number. */
	if (!hal_ishexdigit(*szScan))
		return false;

	/* Parse each digit. */
	do {
		(*pu4bVal) <<= 4;
		*pu4bVal += hal_mapchar_tohexdigit(*szScan);

		szScan++;
		(*pu4bMove)++;
	} while (hal_ishexdigit(*szScan));

	return true;
}

bool hal_is_allspace_tab(
	char	*data,
	u8	size
)
{
	u8	cnt = 0, NumOfSpaceAndTab = 0;

	while (size > cnt) {
		if (data[cnt] == ' ' || data[cnt] == '\t' || data[cnt] == '\0')
			++NumOfSpaceAndTab;

		++cnt;
	}

	return size == NumOfSpaceAndTab;
}

u32 hal_mapchar_tohexdigit(
			char		chTmp
)
{
	if (chTmp >= '0' && chTmp <= '9')
		return chTmp - '0';
	else if (chTmp >= 'a' && chTmp <= 'f')
		return 10 + (chTmp - 'a');
	else if (chTmp >= 'A' && chTmp <= 'F')
		return 10 + (chTmp - 'A');
	else
		return 0;
}


bool hal_parse_fiedstring(char	*in_str, u32	*start, char	*out_str, char lqualifier, char rqualifier)
{
	u32	i = 0, j = 0;
	char	c = in_str[(*start)++];

	if (c != lqualifier)
		return false;

	i = (*start);
	c = in_str[(*start)++];

	while (c != rqualifier && c != '\0')
		c = in_str[(*start)++];

	if (c == '\0')
		return false;

	j = (*start) - 2;
	_os_strncpy((char *)out_str, (const char *)(in_str + i), j - i + 1);

	return true;
}


bool hal_get_u1bint_fromstr_indec(char	*str, u8	*pint)
{
	u16 i = 0;
	*pint = 0;

	while (str[i] != '\0') {
		if (str[i] >= '0' && str[i] <= '9') {
			*pint *= 10;
			*pint += (str[i] - '0');
		} else
			return false;
		++i;
	}

	return true;
}


bool hal_get_s1bint_fromstr_indec(char	*str,	s8 *val)
{
	u8 negative = 0;
	u16 i = 0;

	*val = 0;

	while (str[i] != '\0') {
		if (i == 0 && (str[i] == '+' || str[i] == '-')) {
			if (str[i] == '-')
				negative = 1;
		} else if (str[i] >= '0' && str[i] <= '9') {
			*val *= 10;
			*val += (str[i] - '0');
		} else
			return false;
		++i;
	}

	if (negative)
		*val = -*val;

	return true;
}

