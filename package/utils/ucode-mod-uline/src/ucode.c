// SPDX-License-Identifier: ISC
/*
 * Copyright (C) 2025 Felix Fietkau <nbd@nbd.name>
 */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <poll.h>

#include <ucode/module.h>
#include <libubox/list.h>
#include <libubox/uloop.h>

#include "uline.h"

static uc_value_t *registry;
static uc_resource_type_t *state_type, *argp_type;

enum {
	STATE_RES,
	STATE_CB,
	STATE_INPUT,
	STATE_OUTPUT,
	STATE_POLL_CB,
};

struct uc_uline_state {
	struct uloop_fd fd;

	struct uline_state s;
	int registry_index;

	uc_vm_t *vm;
	uc_value_t *state, *cb, *res, *poll_cb;

	uc_value_t *line;

	uint32_t input_mask[256 / 32];
};

struct uc_arg_parser {
	char line_sep;
};

static unsigned int
registry_set(uc_vm_t *vm, uc_value_t *val)
{
	uc_value_t *registry;
	size_t i, len;

	registry = uc_vm_registry_get(vm, "uline.registry");
	len = ucv_array_length(registry);
	for (i = 0; i < len; i++)
		if (ucv_array_get(registry, i) == NULL)
			break;

	ucv_array_set(registry, i, ucv_get(val));
	return i;
}

static uc_value_t *
uc_uline_poll(uc_vm_t *vm, size_t nargs)
{
	struct uc_uline_state *us = uc_fn_thisval("uline.state");
	uc_value_t *val;

	if (!us)
		return NULL;

	uline_poll(&us->s);
	val = us->line;
	us->line = NULL;

	return val;
}

static uc_value_t *
uc_uline_poll_key(uc_vm_t *vm, size_t nargs)
{
	struct uc_uline_state *us = uc_fn_thisval("uline.state");
	uc_value_t *timeout_arg = uc_fn_arg(0);
	struct pollfd pfd = {};
	int timeout, len;
	char c;

	if (!us)
		return NULL;

	if (ucv_type(timeout_arg) == UC_INTEGER)
		timeout = ucv_int64_get(timeout_arg);
	else
		timeout = -1;

	pfd.fd = us->s.input;
	pfd.events = POLLIN;
	poll(&pfd, 1, timeout);
	if (!(pfd.revents & POLLIN))
		return NULL;

	do {
		len = read(pfd.fd, &c, 1);
	} while (len < 0 && errno == EINTR);

	if (len != 1)
		return NULL;

	return ucv_string_new_length(&c, 1);
}

static uc_value_t *
uc_uline_poll_stop(uc_vm_t *vm, size_t nargs)
{
	struct uc_uline_state *us = uc_fn_thisval("uline.state");

	if (!us)
		return NULL;

	us->s.stop = true;

	return NULL;
}

static uc_value_t *
uc_uline_get_window(uc_vm_t *vm, size_t nargs)
{
	struct uc_uline_state *us = uc_fn_thisval("uline.state");
	uc_value_t *val;

	if (!us)
		return NULL;

	val = ucv_object_new(vm);
	ucv_object_add(val, "x", ucv_int64_new(us->s.cols));
	ucv_object_add(val, "y", ucv_int64_new(us->s.rows));
	return val;
}

static uc_value_t *
uc_uline_get_line(uc_vm_t *vm, size_t nargs)
{
	struct uc_uline_state *us = uc_fn_thisval("uline.state");
	uc_value_t *line2 = uc_fn_arg(0);
	uc_value_t *state, *val;
	const char *line;
	size_t len;

	if (!us)
		return NULL;

	state = ucv_object_new(vm);
	if (ucv_is_truish(line2))
		uline_get_line2(&us->s, &line, &len);
	else
		uline_get_line(&us->s, &line, &len);
	ucv_object_add(state, "line", ucv_string_new_length(line, len));
	ucv_object_add(state, "pos", ucv_int64_new(us->s.line.pos));

	return state;
}

static uc_value_t *
uc_uline_set_state(uc_vm_t *vm, size_t nargs)
{
	struct uc_uline_state *us = uc_fn_thisval("uline.state");
	uc_value_t *state = uc_fn_arg(0);
	uc_value_t *arg;
	bool found;

	if (!us || ucv_type(state) != UC_OBJECT)
		return NULL;

	if ((arg = ucv_object_get(state, "prompt", NULL)) != NULL) {
	    if (ucv_type(arg) != UC_STRING)
			return NULL;

		uline_set_prompt(&us->s, ucv_string_get(arg));
	}

	if ((arg = ucv_object_get(state, "line", NULL)) != NULL) {
	    if (ucv_type(arg) != UC_STRING)
			return NULL;

		uline_set_line(&us->s, ucv_string_get(arg), ucv_string_length(arg));
	}

	if ((arg = ucv_object_get(state, "pos", NULL)) != NULL) {
		if (ucv_type(arg) != UC_INTEGER)
			return NULL;

		uline_set_cursor(&us->s, ucv_int64_get(arg));
	}

	arg = ucv_object_get(state, "line2_prompt", &found);
	if (found) {
		if (!arg)
			uline_set_line2_prompt(&us->s, NULL);
		else if (ucv_type(arg) == UC_STRING)
			uline_set_line2_prompt(&us->s, ucv_string_get(arg));
		else
			return NULL;
	}

	if ((arg = ucv_object_get(state, "line2", NULL)) != NULL) {
	    if (ucv_type(arg) != UC_STRING)
			return NULL;

		uline_set_line2(&us->s, ucv_string_get(arg), ucv_string_length(arg));
	}

	if ((arg = ucv_object_get(state, "line2_pos", NULL)) != NULL) {
		if (ucv_type(arg) != UC_INTEGER)
			return NULL;

		uline_set_line2_cursor(&us->s, ucv_int64_get(arg));
	}

	return ucv_boolean_new(true);
}

static uc_value_t *
uc_uline_set_hint(uc_vm_t *vm, size_t nargs)
{
	struct uc_uline_state *us = uc_fn_thisval("uline.state");
	uc_value_t *arg = uc_fn_arg(0);

	if (!us || ucv_type(arg) != UC_STRING)
		return NULL;

	uline_set_hint(&us->s, ucv_string_get(arg), ucv_string_length(arg));

	return ucv_boolean_new(true);
}

static uc_value_t *
uc_uline_set_uloop(uc_vm_t *vm, size_t nargs)
{
	struct uc_uline_state *us = uc_fn_thisval("uline.state");
	uc_value_t *cb = uc_fn_arg(0);

	if (!us || (cb && !ucv_is_callable(cb)))
		return NULL;

	us->poll_cb = cb;
	ucv_array_set(us->state, STATE_POLL_CB, ucv_get(cb));
	if (cb) {
		uloop_fd_add(&us->fd, ULOOP_READ);
		us->fd.cb(&us->fd, 0);
	} else {
		uloop_fd_delete(&us->fd);
	}

	return ucv_boolean_new(true);
}

static uc_value_t *
uc_uline_reset_key_input(uc_vm_t *vm, size_t nargs)
{
	struct uc_uline_state *us = uc_fn_thisval("uline.state");

	us->s.repeat_char = 0;

	return ucv_boolean_new(true);
}

static uc_value_t *
uc_uline_hide_prompt(uc_vm_t *vm, size_t nargs)
{
	struct uc_uline_state *us = uc_fn_thisval("uline.state");

	if (!us)
		return NULL;

	uline_hide_prompt(&us->s);

	return ucv_boolean_new(true);
}

static uc_value_t *
uc_uline_refresh_prompt(uc_vm_t *vm, size_t nargs)
{
	struct uc_uline_state *us = uc_fn_thisval("uline.state");

	if (!us)
		return NULL;

	uline_refresh_prompt(&us->s);

	return ucv_boolean_new(true);
}

static bool
cb_prepare(struct uc_uline_state *us, const char *name)
{
	uc_value_t *func;

	func = ucv_object_get(us->cb, name, NULL);
	if (!func)
		return false;

	uc_vm_stack_push(us->vm, ucv_get(us->res));
	uc_vm_stack_push(us->vm, ucv_get(func));
	return true;
}

static uc_value_t *
cb_call_ret(struct uc_uline_state *us, size_t args, ...)
{
	va_list ap;

	va_start(ap, args);
	for (size_t i = 0; i < args; i++)
		uc_vm_stack_push(us->vm, ucv_get(va_arg(ap, void *)));
	va_end(ap);

	if (uc_vm_call(us->vm, true, args) == EXCEPTION_NONE)
		return uc_vm_stack_pop(us->vm);

	return NULL;
}
#define cb_call(...) ucv_put(cb_call_ret(__VA_ARGS__))

static bool
uc_uline_cb_line(struct uline_state *s, const char *str, size_t len)
{
	struct uc_uline_state *us = container_of(s, struct uc_uline_state, s);
	bool complete = true;
	uc_value_t *ret;

	if (cb_prepare(us, "line_check")) {
		ret = cb_call_ret(us, 1, ucv_string_new_length(str, len));
		complete = ucv_is_truish(ret);
		ucv_put(ret);
	}

	s->stop = complete;
	if (complete)
		us->line = ucv_string_new_length(str, len);

	return complete;
}

static void
uc_uline_cb_event(struct uline_state *s, enum uline_event ev)
{
	struct uc_uline_state *us = container_of(s, struct uc_uline_state, s);
	static const char * const ev_types[] = {
		[EDITLINE_EV_CURSOR_UP] = "cursor_up",
		[EDITLINE_EV_CURSOR_DOWN] = "cursor_down",
		[EDITLINE_EV_WINDOW_CHANGED] = "window_changed",
		[EDITLINE_EV_EOF] = "eof",
		[EDITLINE_EV_INTERRUPT] = "interrupt",
	};

	if (ev > ARRAY_SIZE(ev_types) || !ev_types[ev])
		return;

	if (!cb_prepare(us, ev_types[ev]))
		return;

	if (ev == EDITLINE_EV_WINDOW_CHANGED)
		cb_call(us, 2, ucv_int64_new(s->cols), ucv_int64_new(s->rows));
	else
		cb_call(us, 0);
}

static void uc_uline_poll_cb(struct uloop_fd *fd, unsigned int events)
{
	struct uc_uline_state *us = container_of(fd, struct uc_uline_state, fd);
	uc_value_t *val;

	while (!uloop_cancelled && us->poll_cb) {
		uline_poll(&us->s);

		val = us->line;
		if (!val)
			break;

		us->line = NULL;
		if (!ucv_is_callable(us->poll_cb))
			return;

		uc_vm_stack_push(us->vm, ucv_get(us->res));
		uc_vm_stack_push(us->vm, ucv_get(us->poll_cb));
		cb_call(us, 1, val);
	}
}

static bool
uc_uline_cb_key_input(struct uline_state *s, unsigned char c, unsigned int count)
{
	struct uc_uline_state *us = container_of(s, struct uc_uline_state, s);
	uc_value_t *ret;
	bool retval;

	if (!(us->input_mask[c / 32] & (1 << (c % 32))))
		return false;

	if (!cb_prepare(us, "key_input"))
		return false;

	ret = cb_call_ret(us, 2, ucv_string_new_length((char *)&c, 1), ucv_int64_new(count));
	retval = ucv_is_truish(ret);
	ucv_put(ret);

	return retval;
}

static void
uc_uline_cb_line2_update(struct uline_state *s, const char *str, size_t len)
{
	struct uc_uline_state *us = container_of(s, struct uc_uline_state, s);

	if (cb_prepare(us, "line2_update"))
		cb_call(us, 1, ucv_string_new_length(str, len));
}

static bool
uc_uline_cb_line2_cursor(struct uline_state *s)
{
	struct uc_uline_state *us = container_of(s, struct uc_uline_state, s);
	uc_value_t *retval;
	bool ret = true;

	if (cb_prepare(us, "line2_cursor")) {
		retval = cb_call_ret(us, 0);
		ret = ucv_is_truish(retval);
		ucv_put(retval);
	}

	return ret;
}

static bool
uc_uline_cb_line2_newline(struct uline_state *s, const char *str, size_t len)
{
	struct uc_uline_state *us = container_of(s, struct uc_uline_state, s);
	uc_value_t *retval;
	bool ret = false;

	if (cb_prepare(us, "line2_newline")) {
		retval = cb_call_ret(us, 1, ucv_string_new_length(str, len));
		ret = ucv_is_truish(retval);
		ucv_put(retval);
	}

	return ret;
}

static uc_value_t *
uc_uline_new(uc_vm_t *vm, size_t nargs)
{
	static const struct uline_cb uline_cb = {
#define _CB(_type) ._type = uc_uline_cb_##_type
		_CB(key_input),
		_CB(line),
		_CB(event),
		_CB(line2_update),
		_CB(line2_cursor),
		_CB(line2_newline),
#undef _CB
	};
	uc_value_t *data = uc_fn_arg(0);
	struct uc_uline_state *us;
	FILE *input, *output;
	uc_value_t *arg, *cb, *state, *res;

	if (ucv_type(data) != UC_OBJECT)
		return NULL;

	cb = ucv_object_get(data, "cb", NULL);
	if (ucv_type(cb) != UC_OBJECT)
		return NULL;

	state = ucv_array_new(vm);
	ucv_array_set(state, 0, ucv_get(cb));
	if ((arg = ucv_object_get(data, "input", NULL)) != NULL) {
		input = ucv_resource_data(arg, "fs.file");
		ucv_array_set(state, STATE_INPUT, ucv_get(arg));
	} else {
		input = stdin;
	}

	if ((arg = ucv_object_get(data, "output", NULL)) != NULL) {
		output = ucv_resource_data(arg, "fs.file");
		ucv_array_set(state, STATE_OUTPUT, ucv_get(arg));
	} else {
		output = stdout;
	}

	if (!input || !output) {
		input = output = NULL;
		return NULL;
	}

	us = calloc(1, sizeof(*us));
	us->vm = vm;
	us->state = ucv_array_new(vm);
	ucv_array_set(us->state, STATE_CB, ucv_get(cb));
	us->cb = cb;
	us->registry_index = registry_set(vm, state);

	if ((arg = ucv_object_get(data, "key_input_list", NULL)) != NULL) {
		uc_value_t *val;
		size_t len;

		if (ucv_type(arg) != UC_ARRAY)
			goto free;

		len = ucv_array_length(arg);
		for (size_t i = 0; i < len; i++) {
			unsigned char c;

			val = ucv_array_get(arg, i);
			if (ucv_type(val) != UC_STRING || ucv_string_length(val) != 1)
				goto free;

			c = ucv_string_get(val)[0];
			us->input_mask[c / 32] |= 1 << (c % 32);
		}
	}

	res = ucv_resource_new(state_type, us);
	ucv_array_set(us->state, STATE_RES, ucv_get(res));
	us->res = res;
	us->fd.fd = fileno(input);
	us->fd.cb = uc_uline_poll_cb;

	uline_init(&us->s, &uline_cb, us->fd.fd, output, true);

	return res;

free:
	free(us);
	return NULL;
}

static void free_state(void *ptr)
{
	struct uc_uline_state *us = ptr;
	uc_value_t *registry;

	if (!us)
		return;

	registry = uc_vm_registry_get(us->vm, "uline.registry");
	ucv_array_set(registry, us->registry_index, NULL);
	uline_free(&us->s);
	free(us);
}

static uc_value_t *
uc_uline_close(uc_vm_t *vm, size_t nargs)
{
	struct uline_state **s = uc_fn_this("uline.state");

	if (!s || !*s)
		return NULL;

	free_state(*s);
	*s = NULL;

	return NULL;
}

static bool
skip_space(const char **str, const char *end)
{
	while (*str < end && isspace(**str))
		(*str)++;
	return *str < end;
}

static void
add_str(uc_stringbuf_t **buf, const char *str, const char *next)
{
	if (str == next)
		return;

	if (!*buf)
		*buf = ucv_stringbuf_new();
	ucv_stringbuf_addstr(*buf, str, next - str);
}

static void
uc_uline_add_pos(uc_vm_t *vm, uc_value_t *list, ssize_t start, ssize_t end)
{
	uc_value_t *val = ucv_array_new(vm);
	ucv_array_push(val, ucv_int64_new(start));
	ucv_array_push(val, ucv_int64_new(end));
	ucv_array_push(list, val);
}

static uc_value_t *
uc_uline_parse_args(uc_vm_t *vm, size_t nargs, bool check)
{
	struct uc_arg_parser *argp = uc_fn_thisval("uline.argp");
	uc_value_t *list = NULL, *pos_list = NULL;
	uc_value_t *args = NULL, *pos_args = NULL;
	uc_value_t *str_arg = uc_fn_arg(0);
	uc_stringbuf_t *buf = NULL;
	uc_value_t *missing = NULL;
	uc_value_t *ret;
	const char *start, *str, *end;
	ssize_t start_idx = -1, end_idx = 0;
	enum {
		UNQUOTED,
		BACKSLASH,
		SINGLE_QUOTE,
		DOUBLE_QUOTE,
		DOUBLE_QUOTE_BACKSLASH,
	} state = UNQUOTED;
	static const char * const state_str[] = {
		[BACKSLASH] = "\\",
		[SINGLE_QUOTE] = "'",
		[DOUBLE_QUOTE] = "\"",
		[DOUBLE_QUOTE_BACKSLASH] = "\\\"",
	};
#define UNQUOTE_TOKENS " \t\r\n'\"\\"
	char unquote_tok[] = UNQUOTE_TOKENS "\x00";
	unquote_tok[strlen(UNQUOTE_TOKENS)] = argp->line_sep;

	if (!argp || ucv_type(str_arg) != UC_STRING)
		return NULL;

	if (!check) {
		list = ucv_array_new(vm);
		pos_list = ucv_array_new(vm);
		if (argp->line_sep) {
			args = ucv_array_new(vm);
			pos_args = ucv_array_new(vm);
			ucv_array_push(args, list);
			ucv_array_push(pos_args, pos_list);
		} else {
			args = list;
			pos_args = pos_list;
		}
	}

	start = str = ucv_string_get(str_arg);
	end = str + ucv_string_length(str_arg);
	skip_space(&str, end);

	while (*str && str < end) {
		const char *next;

		switch (state) {
		case UNQUOTED:
			if (isspace(*str)) {
				skip_space(&str, end);
				if (!buf)
					continue;

				ucv_array_push(list, ucv_stringbuf_finish(buf));
				uc_uline_add_pos(vm, pos_list, start_idx, end_idx);
				start_idx = -1;
				buf = NULL;
				continue;
			}

			if (start_idx < 0)
				start_idx = str - start;
			next = str + strcspn(str, unquote_tok);
			if (list)
				add_str(&buf, str, next);
			str = next;
			end_idx = str - start;

			switch (*str) {
			case 0:
				continue;
			case '\'':
				state = SINGLE_QUOTE;
				break;
			case '"':
				state = DOUBLE_QUOTE;
				break;
			case '\\':
				state = BACKSLASH;
				break;
			default:
				if (argp->line_sep &&
				    *str == argp->line_sep) {
					str++;
					if (list) {
						if (buf) {
							ucv_array_push(list, ucv_stringbuf_finish(buf));
							uc_uline_add_pos(vm, pos_list, start_idx, end_idx);
							start_idx = -1;
						}

						buf = NULL;
						list = ucv_array_new(vm);
						ucv_array_push(args, list);

						pos_list = ucv_array_new(vm);
						ucv_array_push(pos_args, pos_list);
					}
				}
				continue;
			}
			if (!buf)
				buf = ucv_stringbuf_new();
			str++;
			break;

		case BACKSLASH:
		case DOUBLE_QUOTE_BACKSLASH:
			if (start_idx < 0)
				start_idx = str - start;
			if (list && *str != '\n')
				add_str(&buf, str, str + 1);
			str++;
			state--;
			end_idx = str - start;
			break;

		case SINGLE_QUOTE:
			if (start_idx < 0)
				start_idx = str - start;
			next = str + strcspn(str, "'");
			if (list)
				add_str(&buf, str, next);
			str = next;

			if (*str == '\'') {
				state = UNQUOTED;
				str++;
			}
			end_idx = str - start;
			break;

		case DOUBLE_QUOTE:
			if (start_idx < 0)
				start_idx = str - start;
			next = str + strcspn(str, "\"\\");
			if (list)
				add_str(&buf, str, next);
			str = next;

			if (*str == '"') {
				state = UNQUOTED;
				str++;
			} else if (*str == '\\') {
				state = DOUBLE_QUOTE_BACKSLASH;
				str++;
			}
			end_idx = str - start;
		}
	}

	if (buf) {
		ucv_array_push(list, ucv_stringbuf_finish(buf));
		uc_uline_add_pos(vm, pos_list, start_idx, end_idx);
	}

	if (state_str[state])
		missing = ucv_string_new(state_str[state]);

	if (!list)
		return missing;

	ret = ucv_object_new(vm);
	ucv_object_add(ret, "args", args);
	ucv_object_add(ret, "pos", pos_args);
	if (missing)
		ucv_object_add(ret, "missing", missing);

	return ret;
}

static uc_value_t *
uc_uline_arg_parser(uc_vm_t *vm, size_t nargs)
{
	uc_value_t *opts = uc_fn_arg(0);
	struct uc_arg_parser *argp;
	uc_value_t *a;
	char sep = 0;

	if ((a = ucv_object_get(opts, "line_separator", NULL)) != NULL) {
		if (ucv_type(a) != UC_STRING || ucv_string_length(a) != 1)
			return NULL;

		sep = ucv_string_get(a)[0];
	}

	argp = calloc(1, sizeof(*argp));
	argp->line_sep = sep;

	return ucv_resource_new(argp_type, argp);
}

static uc_value_t *
uc_uline_argp_parse(uc_vm_t *vm, size_t nargs)
{
	return uc_uline_parse_args(vm, nargs, false);
}

static uc_value_t *
uc_uline_argp_check(uc_vm_t *vm, size_t nargs)
{
	return uc_uline_parse_args(vm, nargs, true);
}

static uc_value_t *
uc_uline_argp_escape(uc_vm_t *vm, size_t nargs)
{
	uc_value_t *arg = uc_fn_arg(0);
	uc_value_t *ref_arg = uc_fn_arg(1);
	const char *str, *next;
	uc_stringbuf_t *buf;
	char ref = 0;

	if (ucv_type(arg) != UC_STRING)
		return NULL;

	if (ucv_type(ref_arg) == UC_STRING)
		ref = ucv_string_get(ref_arg)[0];

	str = ucv_string_get(arg);
	if (ref != '"' && ref != '\'') {
		next = str + strcspn(str, "\n\t '\"");
		if (*next)
			ref = '"';
	}
	if (ref != '"' && ref != '\'')
		return ucv_string_new(str);

	buf = ucv_stringbuf_new();
	ucv_stringbuf_addstr(buf, &ref, 1);

	while (*str) {
		next = strchr(str, ref);
		if (!next) {
			ucv_stringbuf_addstr(buf, str, strlen(str));
			break;
		}

		if (next - str)
			ucv_stringbuf_addstr(buf, str, next - str);
		if (ref == '\'')
			ucv_stringbuf_addstr(buf, "'\\''", 4);
		else
			ucv_stringbuf_addstr(buf, "\\\"", 2);
		str = next + 1;
	}

	ucv_stringbuf_addstr(buf, &ref, 1);

	return ucv_stringbuf_finish(buf);
}

static uc_value_t *
uc_uline_getpass(uc_vm_t *vm, size_t nargs)
{
	uc_value_t *prompt = uc_fn_arg(0);
	char *pw;

	if (ucv_type(prompt) != UC_STRING)
		return NULL;

	pw = getpass(ucv_string_get(prompt));
	if (!pw)
		return NULL;

	return ucv_string_new(pw);
}

static const uc_function_list_t argp_fns[] = {
	{ "parse", uc_uline_argp_parse },
	{ "check", uc_uline_argp_check },
	{ "escape", uc_uline_argp_escape },
};

static const uc_function_list_t state_fns[] = {
	{ "close", uc_uline_close },
	{ "poll", uc_uline_poll },
	{ "poll_stop", uc_uline_poll_stop },
	{ "poll_key", uc_uline_poll_key },
	{ "reset_key_input", uc_uline_reset_key_input },
	{ "get_line", uc_uline_get_line },
	{ "get_window", uc_uline_get_window },
	{ "set_hint", uc_uline_set_hint },
	{ "set_state", uc_uline_set_state },
	{ "set_uloop", uc_uline_set_uloop },
	{ "hide_prompt", uc_uline_hide_prompt },
	{ "refresh_prompt", uc_uline_refresh_prompt },
};

static const uc_function_list_t global_fns[] = {
	{ "new", uc_uline_new },
	{ "arg_parser", uc_uline_arg_parser },
	{ "getpass", uc_uline_getpass },
};

void uc_module_init(uc_vm_t *vm, uc_value_t *scope)
{
	uc_function_list_register(scope, global_fns);

	state_type = uc_type_declare(vm, "uline.state", state_fns, free_state);
	argp_type = uc_type_declare(vm, "uline.argp", argp_fns, free);
	registry = ucv_array_new(vm);
	uc_vm_registry_set(vm, "uline.registry", registry);
}
