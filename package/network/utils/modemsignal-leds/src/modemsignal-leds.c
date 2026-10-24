// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * modemsignal-leds - drive cellular signal-strength LEDs from a modem.
 *
 * Copyright (C) 2026 Carlo Szelinsky <github@szelinsky.de>
 *
 * One process instance drives the LEDs of ONE modem. It periodically runs a
 * backend program that prints a normalized signal quality (0..100, or a
 * negative value = no service) and maps that onto the configured LEDs.
 *
 * The engine is deliberately dumb: config parsing, backend selection and LED
 * discovery all happen in the shell init script, which invokes this binary
 * with resolved arguments (like rssileds).
 *
 * Usage:
 *   modemsignal-leds -i <sec> [-m bars|status]
 *                    -l <name,min,max> [-l ...] -- <backend> [args...]
 *
 * The backend after the options is exec'd directly (no shell). It must print
 * one integer (the quality) on stdout. LED names may contain ':' so the
 * per-LED fields are comma-separated.
 *
 * The sysfs LED base path defaults to /sys/class/leds and can be overridden
 * with $MODEMSIGNAL_LEDS_SYSFS (used for host testing).
 *
 * uloop runs the show: it owns the poll timer, reaps the backend and handles
 * SIGINT/SIGTERM, so the poll period stays the configured one instead of
 * drifting by however long the backend took.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <libubox/ulog.h>
#include <libubox/uloop.h>

enum mode { MODE_BARS, MODE_STATUS };

#define LED_NAME_MAX	64	/* "mdio-bus:08:white:rssi-1" and the like */
#define LED_PATH_MAX	512	/* <base>/<name>/brightness		  */
#define BACKEND_OUT_MAX	64	/* the backend prints one integer	  */

struct led {
	char	 name[LED_NAME_MAX];
	int	 min;		/* quality window: off below min ... */
	int	 max;		/* ... full at/above max            */
	int	 maxbright;	/* from <led>/max_brightness        */
	int	 fd;		/* open <led>/brightness            */
	int	 lit;		/* hysteresis state, -1 = undecided  */
	int	 written;	/* last value written, -1 = unknown  */
	int	 write_failed;	/* so a dead LED is reported once     */
};

/*
 * Quality points a bar has to move past its boundary before it changes state.
 * The LED windows are a few dB wide (5 points is about 2.5 dB of RSRP), and a
 * stationary router sees the level wander by more than that, so without this
 * the topmost lit bar toggles on nearly every poll.
 */
#define HYSTERESIS 5

/*
 * Every number here comes from UCI by way of argv, so it is attacker-grade
 * input as far as this process is concerned: atoi() would turn a typo like
 * "option refresh 2147484" into a negative millisecond timeout, which expires
 * at once and turns the poll loop into a fork/kill spin with a dark panel.
 */
static int parse_num(const char *s, int min, int max, int *out)
{
	char *end;
	long v;

	errno = 0;
	v = strtol(s, &end, 10);
	if (errno || end == s || *end || v < min || v > max)
		return -1;
	*out = (int)v;
	return 0;
}

static const char *sysfs_base(void)
{
	const char *e = getenv("MODEMSIGNAL_LEDS_SYSFS");
	return (e && *e) ? e : "/sys/class/leds";
}

static int read_int_file(const char *path, int dflt)
{
	FILE *f = fopen(path, "r");
	int v = dflt;

	if (f) {
		if (fscanf(f, "%d", &v) != 1)
			v = dflt;
		fclose(f);
	}
	return v;
}

/* take the LED away from any kernel trigger so we own its brightness */
static void led_release_trigger(const char *base, const char *name)
{
	char path[LED_PATH_MAX];
	int fd;

	snprintf(path, sizeof(path), "%s/%s/trigger", base, name);
	fd = open(path, O_WRONLY | O_CLOEXEC);
	if (fd < 0)
		return;		/* no trigger file: nothing to take it from */
	if (write(fd, "none\n", 5) < 0)
		ULOG_WARN("%s: cannot release the LED trigger: %s\n",
			  name, strerror(errno));
	close(fd);
}

static int led_open(struct led *l, const char *base)
{
	char path[LED_PATH_MAX];

	led_release_trigger(base, l->name);

	snprintf(path, sizeof(path), "%s/%s/max_brightness", base, l->name);
	l->maxbright = read_int_file(path, 255);
	if (l->maxbright <= 0)
		l->maxbright = 255;

	l->lit = -1;
	l->written = -1;

	/* O_CLOEXEC: the backend runs with our fds, and a script that writes to
	 * an inherited descriptor would drive the LED behind our back
	 */
	snprintf(path, sizeof(path), "%s/%s/brightness", base, l->name);
	l->fd = open(path, O_WRONLY | O_CLOEXEC);
	if (l->fd < 0) {
		ULOG_ERR("cannot open %s: %s\n", path, strerror(errno));
		return -1;
	}
	return 0;
}

static void led_set(struct led *l, int value)
{
	char buf[16];
	int n;

	if (l->fd < 0)
		return;
	if (value < 0)
		value = 0;
	if (value > l->maxbright)
		value = l->maxbright;
	if (value == l->written)	/* nothing to say */
		return;
	l->written = value;
	n = snprintf(buf, sizeof(buf), "%d\n", value);

	if (lseek(l->fd, 0, SEEK_SET) < 0 || write(l->fd, buf, n) != n) {
		/* a LED that stops accepting writes is worth one line, not one
		 * line per poll
		 */
		if (!l->write_failed)
			ULOG_WARN("%s: cannot set brightness: %s\n",
				  l->name, strerror(errno));
		l->write_failed = 1;
		return;
	}
	l->write_failed = 0;
	if (ftruncate(l->fd, n) < 0) {
		/* sysfs does not need it; the host tests write plain files */
	}
}

/* linear map of quality into this LED's [min,max] window -> brightness */
static int bar_brightness(struct led *l, int quality)
{
	int b, on;

	if (quality < 0) {
		/* Out of service decides nothing: leave the state undecided so
		 * the first sample after it settles without a margin, or the
		 * panel would come back a bar short.
		 */
		l->lit = -1;
		return 0;
	}

	/*
	 * Decide lit/unlit with hysteresis around the window's lower edge, then
	 * fill as before. l->lit is -1 until the first sample, which settles it
	 * without a margin so the panel is right immediately after start.
	 */
	if (l->lit < 0)
		on = quality > l->min;
	else if (l->lit)
		on = quality > l->min - HYSTERESIS;
	else
		on = quality > l->min + HYSTERESIS;
	l->lit = on;

	if (!on)
		return 0;
	if (quality >= l->max)
		return l->maxbright;
	if (quality < l->min)
		/* Held by hysteresis below its window: keep what the LED already
		 * shows. Falling through to the partial fill would compute a
		 * negative fraction and land on the 1-step floor below, which on
		 * a multi-level LED is visually off - the flicker this is meant
		 * to stop.
		 */
		return l->written > 0 ? l->written : 1;

	/*
	 * Partial fill. Never round down to off: on a plain on/off LED
	 * (max_brightness 1) the fraction is always 0, which would shift the
	 * whole graph up by one window and light nothing below the first
	 * boundary. A bar the quality has reached stays lit.
	 */
	b = (long)l->maxbright * (quality - l->min) / (l->max - l->min);

	return b > 0 ? b : 1;
}

/*
 * One poll: run the backend program (no shell), then light the LEDs with the
 * integer it printed.
 *
 * The child gets its own process group and a deadline of one interval. gcom
 * talks to the modem's control tty and can block for good if the modem stops
 * answering, and killing the group takes a stuck gcom with it rather than
 * leaving it holding the tty.
 */
static struct uloop_interval poll_timer;	/* fires once per interval */
static struct uloop_timeout kill_timer;		/* this poll's deadline */
static struct uloop_process backend_proc;
static char *const *backend_argv;
static int backend_out = -1;
static int interval = 10;	/* seconds */
static int interval_ms = 10000;
static enum mode mode = MODE_BARS;
static struct led *leds;
static int nleds;

static void show_quality(int q)
{
	int i;

	if (q > 100)
		q = 100;

	for (i = 0; i < nleds; i++) {
		if (mode == MODE_STATUS)
			/* single indicator: on while in service, off otherwise */
			led_set(&leds[i], q >= 0 ? leds[i].maxbright : 0);
		else
			led_set(&leds[i], bar_brightness(&leds[i], q));
	}
}

static void backend_done(struct uloop_process *p, int ret)
{
	static int exec_failed;	/* report a backend that will not run only once */
	static int bad_output;	/* same for output that is not a number */
	char buf[BACKEND_OUT_MAX];
	int q = -1;
	ssize_t n;

	(void)p;
	uloop_timeout_cancel(&kill_timer);

	/* execvp() in the child exits 127. Without this, a backend that is
	 * missing or not executable looks exactly like a modem with no
	 * service: a dark panel and nothing in the log.
	 */
	if (WIFEXITED(ret) && WEXITSTATUS(ret) == 127) {
		if (!exec_failed)
			ULOG_ERR("cannot run backend %s\n", backend_argv[0]);
		exec_failed = 1;
	} else {
		exec_failed = 0;
	}

	if (backend_out >= 0) {
		size_t len = 0;

		/* the child is gone, so whatever it printed is already here */
		while (len < sizeof(buf) - 1) {
			n = read(backend_out, buf + len, sizeof(buf) - 1 - len);
			if (n <= 0)
				break;
			len += (size_t)n;
		}
		buf[len] = 0;
		close(backend_out);
		backend_out = -1;

		if (len && sscanf(buf, "%d", &q) != 1) {
			q = -1;
			if (!bad_output)
				ULOG_WARN("backend printed no number\n");
			bad_output = 1;
		} else if (len) {
			bad_output = 0;
		}
	}

	show_quality(q);
}

static void backend_timeout(struct uloop_timeout *t)
{
	(void)t;
	if (backend_proc.pending && backend_proc.pid > 0)
		kill(-backend_proc.pid, SIGKILL);
}

static void start_poll(void)
{
	int fds[2];
	pid_t pid;

	if (pipe(fds) < 0) {
		show_quality(-1);
		return;
	}

	pid = fork();
	if (pid < 0) {
		close(fds[0]);
		close(fds[1]);
		show_quality(-1);
		return;
	}
	if (pid == 0) {
		setpgid(0, 0);
		/* uloop ignores SIGPIPE, and that survives exec: the backend's
		 * own "... | head -1" pipelines should see the default
		 */
		signal(SIGPIPE, SIG_DFL);
		dup2(fds[1], STDOUT_FILENO);
		close(fds[0]);
		close(fds[1]);
		execvp(backend_argv[0], backend_argv);
		_exit(127);
	}

	/* the child does this too; whichever loses gets EACCES, which is fine */
	setpgid(pid, pid);
	close(fds[1]);
	fcntl(fds[0], F_SETFL, O_NONBLOCK);
	fcntl(fds[0], F_SETFD, FD_CLOEXEC);
	backend_out = fds[0];

	backend_proc.pid = pid;
	backend_proc.cb = backend_done;
	uloop_process_add(&backend_proc);
	uloop_timeout_set(&kill_timer, interval_ms);
}

/*
 * One tick per interval, from a timerfd: the period stays the configured one
 * however long a poll takes, instead of being it plus the poll. A backend that
 * is slower than the interval simply loses ticks rather than being started
 * again on top of itself.
 */
static void poll_tick(struct uloop_interval *t)
{
	static int complained;

	(void)t;
	if (backend_proc.pending) {
		if (!complained)
			ULOG_WARN("backend is slower than the %d second interval\n",
				  interval);
		complained = 1;
		return;
	}
	complained = 0;
	start_poll();
}

/* parse "name,min,max" without mutating spec (it points into argv) */
static void parse_led(struct led *l, const char *spec)
{
	const char *c1 = strchr(spec, ',');
	size_t nlen = c1 ? (size_t)(c1 - spec) : strlen(spec);

	l->min = 0;
	l->max = 100;
	l->fd = -1;
	if (nlen >= sizeof(l->name)) {
		nlen = sizeof(l->name) - 1;
		ULOG_WARN("LED name too long, truncated: %.*s\n",
			  (int)nlen, spec);
	}
	memcpy(l->name, spec, nlen);	/* name may contain ':' */
	l->name[nlen] = 0;
	if (c1) {
		const char *c2 = strchr(c1 + 1, ',');
		char win[16];
		size_t wlen = c2 ? (size_t)(c2 - c1 - 1) : strlen(c1 + 1);

		if (wlen < sizeof(win)) {
			memcpy(win, c1 + 1, wlen);
			win[wlen] = 0;
			if (parse_num(win, 0, 100, &l->min))
				ULOG_WARN("%s: bad window start '%s', using 0\n",
					  l->name, win);
		}
		if (c2 && parse_num(c2 + 1, 0, 100, &l->max))
			ULOG_WARN("%s: bad window end '%s', using 100\n",
				  l->name, c2 + 1);
	}
	if (l->max <= l->min)
		l->max = l->min < 100 ? l->min + 1 : 100;
}

int main(int argc, char **argv)
{
	int opt, i, opened;
	char **specs = NULL;
	const char *base = sysfs_base();

	ulog_open(ULOG_SYSLOG | ULOG_STDIO, LOG_DAEMON, "modemsignal-leds");

	while ((opt = getopt(argc, argv, "+i:m:l:")) != -1) {
		switch (opt) {
		case 'i':
			if (parse_num(optarg, 1, 86400, &interval)) {
				ULOG_ERR("bad interval '%s'\n", optarg);
				return 2;
			}
			break;
		case 'm':
			if (!strcmp(optarg, "bars")) {
				mode = MODE_BARS;
			} else if (!strcmp(optarg, "status")) {
				mode = MODE_STATUS;
			} else {
				ULOG_ERR("bad mode '%s'\n", optarg);
				return 2;
			}
			break;
		case 'l': {
			char **grown = realloc(specs,
					       (nleds + 1) * sizeof(*specs));

			if (!grown) {
				free(specs);
				return 1;
			}
			specs = grown;
			specs[nleds++] = optarg;
			break;
		}
		default:
			fprintf(stderr,
				"usage: %s -i sec [-m bars|status] -l name,min,max ... backend args\n",
				argv[0]);
			return 2;
		}
	}
	interval_ms = interval * 1000;
	if (nleds < 1 || optind >= argc) {
		ULOG_ERR("need at least one -l LED and a backend\n");
		return 2;
	}
	backend_argv = &argv[optind];

	leds = calloc(nleds, sizeof(*leds));
	if (!leds)
		return 1;
	for (i = 0, opened = 0; i < nleds; i++) {
		parse_led(&leds[i], specs[i]);
		if (!led_open(&leds[i], base))
			opened++;
	}
	if (!opened) {
		ULOG_ERR("no LED could be opened, giving up\n");
		return 1;
	}

	/* uloop_init() ignores SIGPIPE and installs the SIGINT/SIGTERM handlers
	 * that end the loop, unless something already handles them
	 */
	if (uloop_init() < 0) {
		ULOG_ERR("uloop_init: %s\n", strerror(errno));
		return 1;
	}
	poll_timer.cb = poll_tick;
	kill_timer.cb = backend_timeout;
	start_poll();			/* do not wait out the first interval */
	uloop_interval_set(&poll_timer, interval_ms);
	uloop_run();

	if (backend_proc.pending) {
		kill(-backend_proc.pid, SIGKILL);
		uloop_process_delete(&backend_proc);
	}
	uloop_done();

	/* leave the panel dark on exit */
	for (i = 0; i < nleds; i++) {
		led_set(&leds[i], 0);
		if (leds[i].fd >= 0)
			close(leds[i].fd);
	}
	return 0;
}
