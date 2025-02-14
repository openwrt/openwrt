// SPDX-License-Identifier: ISC
/*
 * Copyright (C) 2025 Felix Fietkau <nbd@nbd.name>
 */
#include <string.h>
#include <stdlib.h>
#include "uline.h"
#include "private.h"

enum vt100_escape vt100_esc_decode(const char *str)
{
	unsigned long code;
	size_t idx;

	switch (*(str++)) {
	case 0:
		return VT100_INCOMPLETE;
	case '[':
	case 'O':
		switch (*(str++)) {
		case 0:
			return VT100_INCOMPLETE;
		case 'A':
			return VT100_CURSOR_UP;
		case 'B':
			return VT100_CURSOR_DOWN;
		case 'C':
			return VT100_CURSOR_RIGHT;
		case 'D':
			return VT100_CURSOR_LEFT;
		case 'F':
			return VT100_END;
		case 'H':
			return VT100_HOME;
		case '5':
			switch (*str) {
			case 'C':
				return VT100_CURSOR_WORD_RIGHT;
			case 'D':
				return VT100_CURSOR_WORD_LEFT;
			default:
				break;
			}
			/* fallthrough */
		case '0' ... '4':
		case '6' ... '9':
			str--;
			idx = strspn(str, "0123456789");
			if (!str[idx])
				return VT100_INCOMPLETE;
			if (str[idx] != '~')
				return VT100_UNKNOWN;
			code = strtoul(str, NULL, 10);
			switch (code) {
			case 1:
				return VT100_HOME;
			case 3:
				return VT100_DELETE;
			case 4:
				return VT100_END;
			case 200:
			case 201:
				// paste start/end
				return VT100_IGNORE;
			default:
				return VT100_UNKNOWN;
			}
		default:
			return VT100_UNKNOWN;
		}
	default:
		return VT100_UNKNOWN;
	}
}

void __vt100_csi_num(FILE *out, int num, char code)
{
	fprintf(out, "\e[%d%c", num, code);
}

void __vt100_esc(FILE *out, char c)
{
	char seq[] = "\eX";
	seq[1] = c;
	fputs(seq, out);
}

void __vt100_csi2(FILE *out, char c1, char c2)
{
	char seq[] = "\e[XX";

	seq[2] = c1;
	seq[3] = c2;
	fputs(seq, out);
}
