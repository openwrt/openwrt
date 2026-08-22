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

enum mode { MODE_BARS, MODE_STATUS };

struct led {
	char	 name[64];
	int	 min;		/* quality window: off below min ... */
	int	 max;		/* ... full at/above max            */
	int	 maxbright;	/* from <led>/max_brightness        */
	int	 fd;		/* open <led>/brightness            */
};

static volatile sig_atomic_t running = 1;
static void on_signal(int s) { (void)s; running = 0; }
/* only needs to interrupt a blocking read, hence no body */
static void on_alarm(int s) { (void)s; }

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
	char path[512];
	int fd;

	snprintf(path, sizeof(path), "%s/%s/trigger", base, name);
	fd = open(path, O_WRONLY);
	if (fd >= 0) {
		if (write(fd, "none\n", 5) < 0) { /* best effort */ }
		close(fd);
	}
}

static int led_open(struct led *l, const char *base)
{
	char path[512];

	led_release_trigger(base, l->name);

	snprintf(path, sizeof(path), "%s/%s/max_brightness", base, l->name);
	l->maxbright = read_int_file(path, 255);
	if (l->maxbright <= 0)
		l->maxbright = 255;

	snprintf(path, sizeof(path), "%s/%s/brightness", base, l->name);
	l->fd = open(path, O_WRONLY);
	if (l->fd < 0) {
		fprintf(stderr, "modemsignal-leds: cannot open %s: %s\n",
			path, strerror(errno));
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
	n = snprintf(buf, sizeof(buf), "%d\n", value);
	if (lseek(l->fd, 0, SEEK_SET) < 0) { /* sysfs, ignore */ }
	if (write(l->fd, buf, n) < 0) { /* best effort */ }
}

/* linear map of quality into this LED's [min,max] window -> brightness */
static int bar_brightness(const struct led *l, int quality)
{
	int b;

	if (quality < 0 || quality <= l->min)
		return 0;
	if (quality >= l->max)
		return l->maxbright;

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
 * Run the backend program (no shell) and read the first integer it prints.
 *
 * The child is bounded by an alarm: gcom talks to the modem's control tty and
 * can block for good if the modem stops answering, which would freeze the LEDs
 * at their last value and make SIGTERM ineffective (the read would restart).
 */
static int run_backend(char *const argv[], int timeout)
{
	int fds[2];
	pid_t pid;
	int q = -1, status;
	FILE *f;

	if (pipe(fds) < 0)
		return -1;

	pid = fork();
	if (pid < 0) {
		close(fds[0]);
		close(fds[1]);
		return -1;
	}
	if (pid == 0) {
		/* own process group, so a stuck gcom under the backend script is
		 * killed with it rather than left holding the modem's tty */
		setpgid(0, 0);
		dup2(fds[1], STDOUT_FILENO);
		close(fds[0]);
		close(fds[1]);
		execvp(argv[0], argv);
		_exit(127);
	}

	close(fds[1]);
	alarm(timeout);
	f = fdopen(fds[0], "r");
	if (f) {
		if (fscanf(f, "%d", &q) != 1)
			q = -1;
		fclose(f);
	} else {
		close(fds[0]);
	}
	alarm(0);

	/* the read returned early or was interrupted: do not wait on the child */
	kill(-pid, SIGKILL);
	while (waitpid(pid, &status, 0) < 0 && errno == EINTR)
		;

	if (q > 100)
		q = 100;
	return q;
}

/* parse "name,min,max" without mutating spec (it points into argv) */
static void parse_led(struct led *l, const char *spec)
{
	const char *c1 = strchr(spec, ',');
	size_t nlen = c1 ? (size_t)(c1 - spec) : strlen(spec);

	l->min = 0;
	l->max = 100;
	l->fd = -1;
	if (nlen >= sizeof(l->name))
		nlen = sizeof(l->name) - 1;
	memcpy(l->name, spec, nlen);	/* name may contain ':' */
	l->name[nlen] = 0;
	if (c1) {			/* atoi stops at the next comma */
		const char *c2 = strchr(c1 + 1, ',');
		l->min = atoi(c1 + 1);
		if (c2)
			l->max = atoi(c2 + 1);
	}
	if (l->max <= l->min)
		l->max = l->min + 1;
}

int main(int argc, char **argv)
{
	int interval = 10, opt, i, nleds = 0;
	enum mode mode = MODE_BARS;
	char **specs = NULL;
	struct led *leds;
	char *const *backend;
	const char *base = sysfs_base();
	struct sigaction sa;

	while ((opt = getopt(argc, argv, "+i:m:l:")) != -1) {
		switch (opt) {
		case 'i':
			interval = atoi(optarg);
			break;
		case 'm':
			mode = strcmp(optarg, "status") ? MODE_BARS : MODE_STATUS;
			break;
		case 'l':
			specs = realloc(specs, (nleds + 1) * sizeof(*specs));
			if (!specs)
				return 1;
			specs[nleds++] = optarg;
			break;
		default:
			fprintf(stderr,
				"usage: %s -i sec [-m bars|status] -l name,min,max ... backend args\n",
				argv[0]);
			return 2;
		}
	}
	if (interval < 1)
		interval = 10;
	if (nleds < 1 || optind >= argc) {
		fprintf(stderr, "modemsignal-leds: need at least one -l LED and a backend\n");
		return 2;
	}
	backend = &argv[optind];

	leds = calloc(nleds, sizeof(*leds));
	if (!leds)
		return 1;
	for (i = 0; i < nleds; i++) {
		parse_led(&leds[i], specs[i]);
		led_open(&leds[i], base);
	}

	/* sigaction() without SA_RESTART: signal() would set it and the blocking
	 * read in run_backend() would simply resume */
	sa.sa_handler = on_signal;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGTERM, &sa, NULL);
	sigaction(SIGINT, &sa, NULL);
	sa.sa_handler = on_alarm;
	sigaction(SIGALRM, &sa, NULL);
	signal(SIGPIPE, SIG_IGN);

	while (running) {
		int q = run_backend(backend, interval);

		if (mode == MODE_STATUS) {
			/* single indicator: on while in service, off otherwise */
			for (i = 0; i < nleds; i++)
				led_set(&leds[i], q >= 0 ? leds[i].maxbright : 0);
		} else {
			for (i = 0; i < nleds; i++)
				led_set(&leds[i], bar_brightness(&leds[i], q));
		}

		for (i = 0; i < interval && running; i++)
			sleep(1);
	}

	/* leave the panel dark on exit */
	for (i = 0; i < nleds; i++) {
		led_set(&leds[i], 0);
		if (leds[i].fd >= 0)
			close(leds[i].fd);
	}
	return 0;
}
