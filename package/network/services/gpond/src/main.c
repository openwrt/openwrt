// SPDX-License-Identifier: GPL-2.0
/*
 * gpond - AN7552/AN7563 GPON daemon.
 *
 * usage: gpond [-s] [-v]
 *   -s  log to stderr as well (foreground debugging)
 *   -v  debug log level
 * SIGHUP re-reads /etc/config/gpon (procd reload).
 */
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "gpond.h"

struct gpon_cfg cfg;
int gpond_log_level = LOG_INFO;
static bool debug;

void gpond_hexdump(const char *what, const void *data, size_t len)
{
	const uint8_t *p = data;
	char line[3 * 48 + 1];
	size_t i, n;

	if (gpond_log_level < LOG_DEBUG)
		return;
	DBG("%s (%zu bytes)", what, len);
	for (i = 0; i < len; i += 48) {
		n = len - i < 48 ? len - i : 48;
		for (size_t j = 0; j < n; j++)
			snprintf(line + 3 * j, 4, "%02x ", p[i + j]);
		DBG("  %04zx: %s", i, line);
	}
}

static struct uloop_timeout nl_retry;
static struct uloop_signal sighup;

static void start(void)
{
	omci_init();
	eth_init();
	apply_config(false);
	monitor_init();
	mcast_gate_init();
	INFO("running in %s mode", config_mode_name(cfg.mode));
}

static void nl_retry_cb(struct uloop_timeout *t)
{
	if (nl_init()) {
		uloop_timeout_set(t, 5000);
		return;
	}
	start();
}

bool gpond_stopping;

/* SIGHUP (procd reload) and "ubus call gpon reload" */
void gpond_reload(void)
{
	if (gpond_stopping)
		return;
	INFO("reloading configuration");
	config_load(&cfg);
	gpond_log_level = debug ? LOG_DEBUG : cfg.log_level;
	apply_config(false);
	omci_config_changed();
}

static void hup_cb(struct uloop_signal *s)
{
	gpond_reload();
}

int main(int argc, char **argv)
{
	int ch, opts = LOG_PID;

	while ((ch = getopt(argc, argv, "sv")) != -1) {
		switch (ch) {
		case 's':
			opts |= LOG_PERROR;
			break;
		case 'v':
			debug = true;
			break;
		default:
			fprintf(stderr, "usage: %s [-s] [-v]\n", argv[0]);
			return 1;
		}
	}

	openlog("gpond", opts, LOG_DAEMON);
	config_load(&cfg);
	gpond_log_level = debug ? LOG_DEBUG : cfg.log_level;

	uloop_init();
	signal(SIGPIPE, SIG_IGN);
	sighup.signo = SIGHUP;
	sighup.cb = hup_cb;
	uloop_signal_add(&sighup);

	if (ubus_init())
		return 1;

	nl_retry.cb = nl_retry_cb;
	if (nl_init()) {
		WARN("waiting for the an7552-gpon driver");
		uloop_timeout_set(&nl_retry, 5000);
	} else {
		start();
	}

	uloop_run();

	ubus_done();
	nl_done();
	uloop_done();
	closelog();

	return 0;
}
