// SPDX-License-Identifier: ISC
/*
 * Copyright (C) 2025 Felix Fietkau <nbd@nbd.name>
 */
#ifndef __EDITLINE_PRIVATE_H
#define __EDITLINE_PRIVATE_H

#include <stdio.h>

#define KEY_NUL	0	// ^@ Null character
#define KEY_SOH	1	// ^A Start of heading, = console interrupt
#define KEY_STX	2	// ^B Start of text, maintenance mode on HP console
#define KEY_ETX	3	// ^C End of text
#define KEY_EOT	4	// ^D End of transmission, not the same as ETB
#define KEY_ENQ	5	// ^E Enquiry, goes with ACK; old HP flow control
#define KEY_ACK	6	// ^F Acknowledge, clears ENQ logon hand
#define KEY_BEL	7	// ^G Bell, rings the bell
#define KEY_BS	8	// ^H Backspace, works on HP terminals/computers
#define KEY_HT	9	// ^I Horizontal tab, move to next tab stop
#define KEY_LF	10	// ^J Line Feed
#define KEY_VT	11	// ^K Vertical tab
#define KEY_FF	12	// ^L Form Feed, page eject
#define KEY_CR	13	// ^M Carriage Return
#define KEY_SO	14	// ^N Shift Out, alternate character set
#define KEY_SI	15	// ^O Shift In, resume defaultn character set
#define KEY_DLE	16	// ^P Data link escape
#define KEY_DC1	17	// ^Q XON, with XOFF to pause listings; "okay to send"
#define KEY_DC2	18	// ^R Device control 2, block-mode flow control
#define KEY_DC3	19	// ^S XOFF, with XON is TERM=18 flow control
#define KEY_DC4	20	// ^T Device control 4
#define KEY_NAK	21	// ^U Negative acknowledge
#define KEY_SYN	22	// ^V Synchronous idle
#define KEY_ETB	23	// ^W End transmission block, not the same as EOT
#define KEY_CAN	24	// ^X Cancel line, MPE echoes !!!
#define KEY_EM	25	// ^Y End of medium, Control-Y interrupt
#define KEY_SUB	26	// ^Z Substitute
#define KEY_ESC	27	// ^[ Escape, next character is not echoed
#define KEY_FS	28	// ^\ File separator
#define KEY_GS	29	// ^] Group separator
#define KEY_RS	30	// ^^ Record separator, block-mode terminator
#define KEY_US	31	// ^_ Unit separator
#define KEY_DEL	127	// Delete (not a real control character)

// Types of escape code
enum vt100_escape {
	VT100_INCOMPLETE,
	VT100_UNKNOWN,
	VT100_IGNORE,
	VT100_CURSOR_UP,
	VT100_CURSOR_DOWN,
	VT100_CURSOR_LEFT,
	VT100_CURSOR_WORD_LEFT,
	VT100_CURSOR_RIGHT,
	VT100_CURSOR_WORD_RIGHT,
	VT100_HOME,
	VT100_END,
	VT100_INSERT,
	VT100_DELETE,
	VT100_DELETE_LEFT,
	VT100_DELETE_LEFT_WORD,
	VT100_PAGE_UP,
	VT100_PAGE_DOWN,
};

ssize_t utf8_nsyms(const char *str, size_t len);
enum vt100_escape vt100_esc_decode(const char *str);

// helpers:
void __vt100_csi_num(FILE *out, int num, char code);
void __vt100_csi2(FILE *out, char c1, char c2);
void __vt100_esc(FILE *out, char c);
static inline void __vt100_sgr(FILE *out, int code)
{
	__vt100_csi2(out, code + '0', 'm');
}


static inline void vt100_attr_reset(FILE *out)
{
	__vt100_sgr(out, 0);
}

static inline void vt100_attr_bright(FILE *out)
{
	__vt100_sgr(out, 1);
}

static inline void vt100_attr_dim(FILE *out)
{
	__vt100_sgr(out, 2);
}

static inline void vt100_attr_underscore(FILE *out)
{
	__vt100_sgr(out, 4);
}

static inline void vt100_attr_blink(FILE *out)
{
	__vt100_sgr(out, 5);
}

static inline void vt100_attr_reverse(FILE *out)
{
	__vt100_sgr(out, 7);
}

static inline void vt100_attr_hidden(FILE *out)
{
	__vt100_sgr(out, 8);
}

static inline void vt100_erase_line(FILE *out)
{
	__vt100_csi2(out, '2', 'K');
}

static inline void vt100_clear_screen(FILE *out)
{
	__vt100_csi2(out, '2', 'J');
}

static inline void vt100_cursor_save(FILE *out)
{
	__vt100_esc(out, '7');
}

static inline void vt100_cursor_restore(FILE *out)
{
	__vt100_esc(out, '8');
}

static inline void vt100_scroll_up(FILE *out)
{
	__vt100_esc(out, 'D');
}

static inline void vt100_scroll_down(FILE *out)
{
	__vt100_esc(out, 'M');
}

static inline void vt100_next_line(FILE *out)
{
	__vt100_esc(out, 'E');
}

static inline void vt100_cursor_up(FILE *out, int count)
{
	__vt100_csi_num(out, count, 'A');
}

static inline void vt100_cursor_down(FILE *out, int count)
{
	__vt100_csi_num(out, count, 'B');
}

static inline void vt100_cursor_forward(FILE *out, int count)
{
	__vt100_csi_num(out, count, 'C');
}

static inline void vt100_cursor_back(FILE *out, int count)
{
	__vt100_csi_num(out, count, 'D');
}

static inline void vt100_cursor_home(FILE *out)
{
	__vt100_csi2(out, 'H', 0);
}

static inline void vt100_erase(FILE *out, int count)
{
	__vt100_csi_num(out, count, 'P');
}

static inline void vt100_erase_down(FILE *out)
{
	__vt100_csi2(out, 'J', 0);
}

static inline void vt100_erase_right(FILE *out)
{
	__vt100_csi2(out, 'K', 0);
}

static inline void vt100_ding(FILE *out)
{
	fputc(7, out);
	fflush(out);
}

#endif
