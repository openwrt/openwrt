// SPDX-License-Identifier: ISC
/*
 * Copyright (C) 2025 Felix Fietkau <nbd@nbd.name>
 */
#include <sys/types.h>
#include <sys/ioctl.h>

#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>

#include <libubox/list.h>

#include "uline.h"
#include "private.h"

#define LINEBUF_CHUNK 64

static int sigwinch_count;

static size_t
nsyms(struct uline_state *s, const char *buf, size_t len)
{
	if (!s->utf8)
		return len;
	return utf8_nsyms(buf, len);
}

static inline bool
is_utf8_cont(unsigned char c)
{
	return (c & 0xc0) == 0x80;
}

static size_t
utf8_move_left(const char *line, size_t pos)
{
	if (!pos)
		return 0;
	do {
		pos--;
	} while (pos > 0 && is_utf8_cont(line[pos]));

	return pos;
}

static size_t
utf8_move_right(const char *line, size_t pos, size_t len)
{
	if (pos == len)
		return pos;

	do {
		pos++;
	} while (pos < len && is_utf8_cont(line[pos]));

	return pos;
}

static char *
linebuf_extend(struct linebuf *l, size_t size)
{
	size_t tailroom = l->bufsize - l->len;
	char *buf;

	if (l->buf && tailroom > size)
		goto out;

	size -= tailroom;
	size += LINEBUF_CHUNK - 1;
	size -= size % LINEBUF_CHUNK;

	buf = realloc(l->buf, l->bufsize + size);
	if (!buf)
		return NULL;

	l->buf = buf;
	l->bufsize += size;

out:
	return l->buf + l->len;
}

static void
linebuf_free(struct linebuf *line)
{
	free(line->buf);
	free(line->prompt);
}

static void
update_window_size(struct uline_state *s, bool init)
{
	unsigned int cols = 80, rows = 25;
#ifdef TIOCGWINSZ
	struct winsize ws = {};

	if (s->ioctl_winsize &&
	    !ioctl(fileno(s->output), TIOCGWINSZ, &ws) &&
	    ws.ws_col && ws.ws_row) {
		cols = ws.ws_col;
		rows = ws.ws_row;
	} else
#endif
	{
		s->ioctl_winsize = false;
	}

	s->sigwinch_count = sigwinch_count;
	if (s->cols == cols && s->rows == rows)
		return;

	s->cols = cols;
	s->rows = rows;
	s->full_update = true;
	s->cb->event(s, EDITLINE_EV_WINDOW_CHANGED);
}

static void
handle_sigwinch(int signal)
{
	sigwinch_count++;
}

static void
reset_input_state(struct uline_state *s)
{
	s->utf8_cont = 0;
	s->esc_idx = -1;
}

static void
termios_set_native_mode(struct uline_state *s)
{
	struct termios t = s->orig_termios;

	if (!s->has_termios)
		return;

	t.c_iflag = 0;
	t.c_oflag = OPOST | ONLCR;
	t.c_lflag = 0;
	t.c_cc[VMIN] = 1;
	t.c_cc[VTIME] = 0;

	tcsetattr(s->input, TCSADRAIN, &t);
}

static void
termios_set_orig_mode(struct uline_state *s)
{
	if (!s->has_termios)
		return;

	tcsetattr(s->input, TCSADRAIN, &s->orig_termios);
}

static bool
check_utf8(struct uline_state *s, unsigned char c)
{
	if (!s->utf8)
		return false;
	if (s->utf8_cont)
		return true;
	return (c & 0xc0) == 0xc0;
}

static bool
handle_utf8(struct uline_state *s, unsigned char c)
{
	if (!s->utf8)
		return false;

	if (!s->utf8_cont) {
		if ((c & 0xc0) != 0xc0)
			return false;

		c &= 0xf0;
		c <<= 1;
		while (c & 0x80) {
			c <<= 1;
			s->utf8_cont++;
		}

		return true;
	}

	if ((c & 0xc0) != 0x80) {
		// invalid utf-8
		s->utf8_cont = 0;
		return false;
	}

	s->utf8_cont--;

	return s->utf8_cont;
}

static bool
linebuf_insert(struct linebuf *line, char *c, size_t len)
{
	char *dest;
	ssize_t tail;

	if (!linebuf_extend(line, len + 1))
		return false;

	dest = &line->buf[line->pos];
	tail = line->len - line->pos;
	if (tail > 0)
		memmove(dest + len, dest, tail);
	else
		dest[len] = 0;

	if (line->update_pos > line->pos)
		line->update_pos = line->pos;

	memcpy(dest, c, len);
	line->len += len;
	line->pos += len;
	line->buf[line->len] = 0;

	return true;
}

static void
linebuf_delete(struct linebuf *line, size_t len)
{
	char *dest = &line->buf[line->pos];
	ssize_t tail = line->len - line->pos;
	size_t max_len = line->len - line->pos;

	if (line->update_pos > line->pos)
		line->update_pos = line->pos;

	if (len > max_len)
		len = max_len;

	memmove(dest, dest + len, tail + 1);
	line->len -= len;
}

static struct pos
pos_convert(struct uline_state *s, ssize_t offset)
{
	struct pos pos;
	pos.y = offset / s->cols;
	pos.x = offset - (pos.y * s->cols);
	return pos;
}

static void
pos_add(struct uline_state *s, struct pos *pos, struct pos add)
{
	pos->x += add.x;
	pos->y += add.y;
	if (pos->x >= (int16_t)s->cols) {
		pos->x -= s->cols;
		pos->y++;
	}
	if (pos->x < 0) {
		pos->x += s->cols;
		pos->y--;
	}
	if (pos->y < 0)
		pos->y = 0;
}

static void
pos_add_ofs(struct uline_state *s, struct pos *pos, size_t offset)
{
	pos_add(s, pos, pos_convert(s, offset));
}

static void
pos_add_newline(struct uline_state *s, struct pos *pos)
{
	pos->x = 0;
	pos->y++;
}

static void
__pos_add_string(struct uline_state *s, struct pos *pos, const char *str, size_t len)
{
	const char *next;

	while ((next = memchr(str, KEY_ESC, len)) != NULL) {
		size_t cur_len = next - str;

		pos_add_ofs(s, pos, nsyms(s, str, cur_len));
		next++;

		if (*next == '[' || *next == 'O') {
			next++;
			while (*next <= 63)
				next++;
		}
		next++;
		len -= next - str;
		str = next;
	}

	pos_add_ofs(s, pos, nsyms(s, str, len));
}

static void
pos_add_string(struct uline_state *s, struct pos *pos, const char *str, size_t len)
{
	const char *next;

	if (!len)
		return;

	while ((next = memchr(str, '\n', len)) != NULL) {
		size_t cur_len = next - str;
		if (cur_len)
			__pos_add_string(s, pos, str, cur_len);
		pos_add_newline(s, pos);
		len -= cur_len + 1;
		str = next + 1;
	}

	if (len)
		__pos_add_string(s, pos, str, len);
}

static struct pos
pos_diff(struct pos start, struct pos end)
{
	struct pos diff = {
		.x = end.x - start.x,
		.y = end.y - start.y
	};

	return diff;
}

static void
set_cursor(struct uline_state *s, struct pos pos)
{
	struct pos diff = pos_diff(s->cursor_pos, pos);

	if (diff.x > 0)
		vt100_cursor_forward(s->output, diff.x);
	else if (diff.x < 0)
		vt100_cursor_back(s->output, -diff.x);

	if (diff.y > 0)
		vt100_cursor_down(s->output, diff.y);
	else if (diff.y < 0)
		vt100_cursor_up(s->output, -diff.y);

	s->cursor_pos = pos;
}

static void
display_output_string(struct uline_state *s, const char *str,
		      size_t len)
{
	fwrite(str, len, 1, s->output);
	pos_add_string(s, &s->cursor_pos, str, len);
}

static void
display_update_line(struct uline_state *s, struct linebuf *line,
		    struct pos *pos)
{
	char *start = line->buf;
	char *end = line->buf + line->len;
	struct pos update_pos;
	size_t prompt_len = 0;

	if (line->prompt)
		prompt_len = strlen(line->prompt);

	if (s->full_update) {
		display_output_string(s, line->prompt, prompt_len);
		*pos = s->cursor_pos;
		line->update_pos = 0;
	} else {
		pos_add_string(s, pos, line->prompt, prompt_len);
	}

	update_pos = *pos;
	if (line->update_pos) {
		start += line->update_pos;
		pos_add_string(s, &update_pos, line->buf, line->update_pos);
	}
	set_cursor(s, update_pos);
	vt100_erase_right(s->output);
	line->update_pos = line->len;

	if (end - start <= 0)
		return;

	display_output_string(s, start, end - start);
	if (s->cursor_pos.x == 0 && end[-1] != '\n')
		vt100_next_line(s->output);
}

static void
display_update(struct uline_state *s)
{
	struct pos edit_pos, end_diff;
	struct pos base_pos = {};
	struct linebuf *line = &s->line;

	if (s->full_update) {
		set_cursor(s, (struct pos){});
		fputc(KEY_CR, s->output);
		vt100_erase_down(s->output);
	}

	display_update_line(s, line, &base_pos);

	if (s->line2) {
		line = s->line2;

		if (s->cursor_pos.x != 0) {
			vt100_next_line(s->output);
			pos_add_newline(s, &s->cursor_pos);
		}

		base_pos = s->cursor_pos;
		display_update_line(s, s->line2, &base_pos);
	}

	edit_pos = base_pos;
	pos_add_string(s, &edit_pos, line->buf, line->pos);

	end_diff = pos_diff(s->end_pos, s->cursor_pos);
	s->end_pos = s->cursor_pos;

	if (end_diff.y != 0)
		vt100_erase_down(s->output);
	else
		vt100_erase_right(s->output);

	set_cursor(s, edit_pos);
	fflush(s->output);

	s->full_update = false;
}

static bool
delete_symbol(struct uline_state *s, struct linebuf *line)
{
	size_t len = 1;

	if (line->pos == line->len)
		return false;

	if (s->utf8) {
		len = utf8_move_right(line->buf, line->pos, line->len);
		len -= line->pos;
	}

	linebuf_delete(line, len);
	return true;
}

static bool
move_left(struct uline_state *s, struct linebuf *line)
{
	if (!line->pos)
		return false;
	if (s->utf8)
		line->pos = utf8_move_left(line->buf, line->pos);
	else
		line->pos--;
	return true;
}

static bool
move_word_left(struct uline_state *s, struct linebuf *line)
{
	char *buf = line->buf;
	size_t pos;

	if (!move_left(s, line))
		return false;

	pos = line->pos;
	// remove trailing spaces
	while (pos > 0 && isspace(buf[pos]))
		pos--;

	// skip word
	while (pos > 0 && !isspace(buf[pos]))
		pos--;
	if (isspace(buf[pos]))
		pos++;

	line->pos = pos;

	return true;
}

static bool
move_right(struct uline_state *s, struct linebuf *line)
{
	if (line->pos >= line->len)
		return false;
	if (s->utf8)
		line->pos = utf8_move_right(line->buf, line->pos, line->len);
	else
		line->pos++;
	return true;
}

static bool
move_word_right(struct uline_state *s, struct linebuf *line)
{
	char *buf = line->buf;
	size_t pos = line->pos;

	if (pos == line->len)
		return false;

	// skip word
	while (!isspace(buf[pos]) && pos < line->len)
		pos++;

	// skip trailing whitespace
	while (isspace(buf[pos]) && pos < line->len)
		pos++;

	line->pos = pos;

	return true;
}

static bool
process_esc(struct uline_state *s, enum vt100_escape esc, uint32_t data)
{
	struct linebuf *line = &s->line;

	if (s->line2 &&
	    (esc == VT100_DELETE ||
	     (s->cb->line2_cursor && s->cb->line2_cursor(s))))
		line = s->line2;

	switch (esc) {
	case VT100_CURSOR_LEFT:
		return move_left(s, line);
	case VT100_CURSOR_WORD_LEFT:
		return move_word_left(s, line);
	case VT100_CURSOR_RIGHT:
		return move_right(s, line);
	case VT100_CURSOR_WORD_RIGHT:
		return move_word_right(s, line);
	case VT100_CURSOR_POS:
		if (s->rows == (data & 0xffff) &&
		    s->cols == data >> 16)
			return false;
		s->rows = data & 0xffff;
		s->cols = data >> 16;
	    s->full_update = true;
		s->cb->event(s, EDITLINE_EV_WINDOW_CHANGED);
		return true;
	case VT100_HOME:
		line->pos = 0;
		return true;
	case VT100_END:
		line->pos = line->len;
		return true;
	case VT100_CURSOR_UP:
		s->cb->event(s, EDITLINE_EV_CURSOR_UP);
		return true;
	case VT100_CURSOR_DOWN:
		s->cb->event(s, EDITLINE_EV_CURSOR_DOWN);
		return true;
	case VT100_DELETE:
		return delete_symbol(s, line);
	default:
		vt100_ding(s->output);
		return false;
	}
}

static bool
process_backword(struct uline_state *s, struct linebuf *line)
{
	size_t pos, len;

	pos = line->pos - 1;
	if (!move_word_left(s, line))
		return false;

	len = pos + 1 - line->pos;
	linebuf_delete(line, len);

	return true;
}

static void
linebuf_reset(struct linebuf *line)
{
	line->pos = 0;
	line->len = 0;
	line->buf[0] = 0;
	line->update_pos = 0;
}

static void
free_line2(struct uline_state *s)
{
	if (!s->line2)
		return;

	linebuf_free(s->line2);
	free(s->line2);
	s->line2 = NULL;
}

static bool
process_newline(struct uline_state *s, bool drop)
{
	bool ret;

	if (drop)
		goto reset;

	termios_set_orig_mode(s);
	if (s->line2 && s->cb->line2_newline &&
	    s->cb->line2_newline(s, s->line2->buf, s->line2->len)) {
		termios_set_native_mode(s);
		return true;
	}

	free_line2(s);
	ret = s->cb->line(s, s->line.buf, s->line.len);
	termios_set_native_mode(s);
	if (!ret) {
		linebuf_insert(&s->line, "\n", 1);
		return true;
	}

reset:
	vt100_next_line(s->output);
	vt100_erase_down(s->output);
	s->cursor_pos = (struct pos) {};
	s->full_update = true;
	fflush(s->output);
	if (!s->line.len)
		return true;

	linebuf_reset(&s->line);

	return true;
}

static bool
process_ctrl(struct uline_state *s, char c)
{
	struct linebuf *line = s->line2 ? s->line2 : &s->line;

	switch (c) {
	case KEY_LF:
	case KEY_CR:
		return process_newline(s, false);
	case KEY_ETX:
		s->cb->event(s, EDITLINE_EV_INTERRUPT);
		process_newline(s, true);
		s->stop = true;
		return true;
	case KEY_EOT:
		if (s->line.len)
			return false;
		s->cb->event(s, EDITLINE_EV_EOF);
		s->stop = true;
		return true;
	case KEY_BS:
	case KEY_DEL:
		if (!move_left(s, line))
			return false;

		delete_symbol(s, line);
		if (s->line2 && s->cb->line2_update)
			s->cb->line2_update(s, line->buf, line->len);
		return true;
	case KEY_FF:
		vt100_cursor_home(s->output);
		vt100_erase_down(s->output);
		s->full_update = true;
		return true;
	case KEY_NAK:
		linebuf_reset(line);
		return true;
	case KEY_SOH:
		return process_esc(s, VT100_HOME, 0);
	case KEY_ENQ:
		return process_esc(s, VT100_END, 0);
	case KEY_VT:
		// TODO: kill
		return false;
	case KEY_EM:
		// TODO: yank
		return false;
	case KEY_ETB:
		return process_backword(s, line);
	case KEY_ESC:
		s->esc_idx = 0;
		return false;
	case KEY_SUB:
		kill(getpid(), SIGTSTP);
		return false;
	default:
		return false;
	}
}

static void
check_key_repeat(struct uline_state *s, char c)
{
	if (s->repeat_char != c)
		s->repeat_count = 0;

	s->repeat_char = c;
	s->repeat_count++;
}

static void
process_char(struct uline_state *s, char c)
{
	enum vt100_escape esc;
	uint32_t data = 0;

	check_key_repeat(s, c);
	if (s->esc_idx >= 0) {
		s->esc_seq[s->esc_idx++] = c;
		s->esc_seq[s->esc_idx] = 0;
		esc = vt100_esc_decode(s->esc_seq, &data);
		if (esc == VT100_INCOMPLETE &&
		    s->esc_idx < (int)sizeof(s->esc_seq) - 1)
			return;

		s->esc_idx = -1;
		if (!process_esc(s, esc, data))
			return;
	} else if (s->cb->key_input &&
	           !check_utf8(s, (unsigned char )c) &&
	           s->cb->key_input(s, c, s->repeat_count)) {
		goto out;
	} else if ((unsigned char)c < 32 || c == 127) {
		if (!process_ctrl(s, c))
			return;
	} else {
		struct linebuf *line = s->line2 ? s->line2 : &s->line;

		if (!linebuf_insert(line, &c, 1) ||
		    handle_utf8(s, (unsigned char )c))
			return;

		if (s->line2 && s->cb->line2_update)
			s->cb->line2_update(s, line->buf, line->len);
	}

out:
	if (s->stop)
		return;

	display_update(s);
}

void uline_poll(struct uline_state *s)
{
	int ret;
	char c;

	uline_refresh_prompt(s);
	s->stop = false;
	while (!s->stop) {
		ret = read(s->input, &c, 1);
		if (ret < 0) {
			if (errno == EINTR)
				continue;
			if (errno == EAGAIN)
				return;
			ret = 0;
		}

		if (!ret) {
			s->cb->event(s, EDITLINE_EV_EOF);
			termios_set_orig_mode(s);
			return;
		}

		if (s->sigwinch_count != sigwinch_count)
			update_window_size(s, false);

		process_char(s, c);
	}
}

void uline_set_prompt(struct uline_state *s, const char *str)
{
	if (s->line.prompt && !strcmp(s->line.prompt, str))
		return;

	free(s->line.prompt);
	s->line.prompt = strdup(str);
	s->full_update = true;
}

void uline_set_line2_prompt(struct uline_state *s, const char *str)
{
	if (!!str != !!s->line2) {
		if (!str)
			free_line2(s);
		else
			s->line2 = calloc(1, sizeof(*s->line2));
	}

	if (!str || (s->line2->prompt && !strcmp(s->line2->prompt, str)))
		return;

	free(s->line2->prompt);
	s->line2->prompt = strdup(str);
	s->full_update = true;
}

static void
__uline_set_line(struct uline_state *s, struct linebuf *line, const char *str, size_t len)
{
	size_t i, prev_len = line->len;

	line->len = 0;
		linebuf_extend(line, len);
	for (i = 0; i < prev_len && i < len; i++) {
		if (line->buf[i] != str[i])
			break;
	}
	if (i > prev_len)
		i--;
	if (s->utf8) {
		// move back to the beginning of the utf-8 symbol
		while (i > 0 && (str[i] & 0xc0) == 0x80)
			i--;
	}
	line->update_pos = i;

	memcpy(line->buf, str, len);
	line->len = len;
	if (line->pos > line->len)
		line->pos = line->len;
}

void uline_set_line(struct uline_state *s, const char *str, size_t len)
{
	__uline_set_line(s, &s->line, str, len);
}

void uline_set_line2(struct uline_state *s, const char *str, size_t len)
{
	if (!s->line2)
		return;
	__uline_set_line(s, s->line2, str, len);
}

void uline_hide_prompt(struct uline_state *s)
{
	set_cursor(s, (struct pos){});
	vt100_erase_down(s->output);
	s->full_update = true;
	fflush(s->output);
}

void uline_refresh_prompt(struct uline_state *s)
{
	termios_set_native_mode(s);
	display_update(s);
}

void uline_set_hint(struct uline_state *s, const char *str, size_t len)
{
	struct pos prev_pos = s->cursor_pos;

	if (len) {
		vt100_next_line(s->output);
		pos_add_newline(s, &s->cursor_pos);
	}
	vt100_erase_down(s->output);

	if (len) {
		fwrite(str, len, 1, s->output);
		pos_add_string(s, &s->cursor_pos, str, len);
	}

	if (s->cursor_pos.y >= s->rows) {
		if (s->cursor_pos.x > 0)
			vt100_next_line(s->output);
		s->cursor_pos = (struct pos){};
		s->full_update = true;
	} else {
		set_cursor(s, prev_pos);
	}
	fflush(s->output);
}

void uline_init(struct uline_state *s, const struct uline_cb *cb,
                int in_fd, FILE *out_stream, bool utf8)
{
	struct sigaction sa = {
		.sa_handler = handle_sigwinch,
	};
	s->cb = cb;
	s->utf8 = utf8;
	s->input = in_fd;
	s->output = out_stream;
	s->ioctl_winsize = true;
	reset_input_state(s);

#ifdef USE_SYSTEM_WCHAR
	if (utf8)
		setlocale(LC_CTYPE, "C.UTF-8");
#endif

	sigaction(SIGWINCH, &sa, NULL);
	s->full_update = true;

	if (!tcgetattr(s->input, &s->orig_termios)) {
		s->has_termios = true;
		termios_set_native_mode(s);
	}

	update_window_size(s, true);
	if (!s->ioctl_winsize) {
		vt100_request_window_size(s->output);
		fflush(s->output);
	}
}

void uline_free(struct uline_state *s)
{
	free_line2(s);
	termios_set_orig_mode(s);
	linebuf_free(&s->line);
}
