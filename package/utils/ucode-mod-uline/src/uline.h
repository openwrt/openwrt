// SPDX-License-Identifier: ISC
/*
 * Copyright (C) 2025 Felix Fietkau <nbd@nbd.name>
 */
#ifndef __EDITLINE_H
#define __EDITLINE_H

#include <stdint.h>
#include <stdbool.h>
#include <termios.h>
#include <stdio.h>

#include <libubox/utils.h>

struct uline_state;

struct linebuf {
	char *buf;
	size_t len;
	size_t bufsize;

	char *prompt;
	size_t pos;
	size_t update_pos;
};

struct pos {
	int16_t x;
	int16_t y;
};

enum uline_event {
	EDITLINE_EV_CURSOR_UP,
	EDITLINE_EV_CURSOR_DOWN,

	EDITLINE_EV_WINDOW_CHANGED,
	EDITLINE_EV_LINE_INPUT,

	EDITLINE_EV_INTERRUPT,
	EDITLINE_EV_EOF,
};

struct uline_cb {
	// called on every key input. return true if handled by callback
	bool (*key_input)(struct uline_state *s, unsigned char c, unsigned int count);

	void (*event)(struct uline_state *s, enum uline_event ev);

	// line: called on newline, returns true to accept the line, false to keep
	// editing a multi-line string
	bool (*line)(struct uline_state *s, const char *str, size_t len);

	// called on any changes to the buffer of the secondary line editor
	void (*line2_update)(struct uline_state *s, const char *str, size_t len);

	// called on cursor button press during line2 editing
	// return true to handle in line2, false to handle in primary line
	bool (*line2_cursor)(struct uline_state *s);

	// called on newline on the secondary line editor
	// return true to ignore, false to process as primary line newline event
	bool (*line2_newline)(struct uline_state *s, const char *str, size_t len);
};

struct uline_state {
	const struct uline_cb *cb;

	int input;
	FILE *output;

	int sigwinch_count;

	struct termios orig_termios;
	bool has_termios;

	struct linebuf line;
	struct linebuf *line2;

	unsigned int repeat_count;
	char repeat_char;

	unsigned int rows, cols;
	struct pos cursor_pos;
	struct pos end_pos;
	bool full_update;
	bool stop;

	bool utf8;

	char esc_seq[8];
	int8_t esc_idx;
	uint8_t utf8_cont;
};

void uline_init(struct uline_state *s, const struct uline_cb *cb,
                int in_fd, FILE *out_stream, bool utf8);
void uline_poll(struct uline_state *s);

void uline_set_line(struct uline_state *s, const char *str, size_t len);
void uline_set_prompt(struct uline_state *s, const char *str);
static inline void
uline_set_cursor(struct uline_state *s, size_t pos)
{
	s->line.pos = pos;
	if (s->line.pos > s->line.len)
		s->line.pos = s->line.len;
}
static inline void
uline_get_line(struct uline_state *s, const char **str, size_t *len)
{
	if (s->line.buf) {
		*str = s->line.buf;
		*len = s->line.len;
	} else{
		*str = "";
		*len = 0;
	}
}



void uline_set_line2(struct uline_state *s, const char *str, size_t len);
void uline_set_line2_prompt(struct uline_state *s, const char *str);
static inline void
uline_set_line2_cursor(struct uline_state *s, size_t pos)
{
	if (!s->line2)
		return;

	s->line2->pos = pos;
	if (s->line2->pos > s->line2->len)
		s->line2->pos = s->line2->len;
}
static inline void
uline_get_line2(struct uline_state *s, const char **str, size_t *len)
{
	if (s->line2 && s->line2->buf) {
		*str = s->line2->buf;
		*len = s->line2->len;
	} else{
		*str = "";
		*len = 0;
	}
}

void uline_set_hint(struct uline_state *s, const char *str, size_t len);
void uline_hide_prompt(struct uline_state *s);
void uline_refresh_prompt(struct uline_state *s);
void uline_free(struct uline_state *s);

#endif
