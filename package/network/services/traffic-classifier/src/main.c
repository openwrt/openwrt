#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <syslog.h>
#include <getopt.h>

#include <libubox/uloop.h>
#include <libubox/ustream.h>
#include <libubus.h>

#include "flow_table.h"
#include "capture.h"
#include "classifier.h"
#include "sta_tracker.h"
#include "ubus_api.h"

#define DEFAULT_INTERFACE    "br-lan"
#define DEFAULT_MAX_FLOWS    8192
#define DEFAULT_MODEL_PATH   "/etc/traffic-classifier/model.json"
#define CLASSIFY_INTERVAL_MS 5000
#define EXPIRE_INTERVAL_MS   10000
#define STA_REFRESH_MS       15000

struct tc_daemon {
	struct flow_table *ft;
	struct capture_ctx *cap;
	struct classifier_ctx *cls;
	struct sta_tracker *sta;
	struct tc_ubus_ctx ubus_ctx;

	struct uloop_fd capture_fd;
	struct uloop_timeout classify_timer;
	struct uloop_timeout expire_timer;
	struct uloop_timeout sta_timer;
};

static struct tc_daemon daemon_ctx;

static void capture_fd_cb(struct uloop_fd *fd, unsigned int events)
{
	struct tc_daemon *d = container_of(fd, struct tc_daemon, capture_fd);
	capture_process(d->cap);
}

static int classify_flow_cb(struct flow_entry *entry, void *ctx)
{
	struct classifier_ctx *cls = (struct classifier_ctx *)ctx;
	classifier_classify_flow(cls, entry);
	return 0;
}

static void classify_timer_cb(struct uloop_timeout *t)
{
	struct tc_daemon *d = container_of(t, struct tc_daemon, classify_timer);
	flow_table_for_each(d->ft, classify_flow_cb, d->cls);
	uloop_timeout_set(t, CLASSIFY_INTERVAL_MS);
}

static void expire_timer_cb(struct uloop_timeout *t)
{
	struct tc_daemon *d = container_of(t, struct tc_daemon, expire_timer);
	flow_table_expire(d->ft, time(NULL), FLOW_TIMEOUT_SEC);
	uloop_timeout_set(t, EXPIRE_INTERVAL_MS);
}

static void sta_timer_cb(struct uloop_timeout *t)
{
	struct tc_daemon *d = container_of(t, struct tc_daemon, sta_timer);
	sta_tracker_refresh(d->sta);
	uloop_timeout_set(t, STA_REFRESH_MS);
}

static void usage(const char *prog)
{
	fprintf(stderr,
		"Usage: %s [options]\n"
		"  -i <interface>    Capture interface (default: %s)\n"
		"  -m <model_path>   ML model file (default: %s)\n"
		"  -n <max_flows>    Max concurrent flows (default: %d)\n"
		"  -d                Debug mode (foreground, verbose)\n"
		"  -h                Show this help\n",
		prog, DEFAULT_INTERFACE, DEFAULT_MODEL_PATH, DEFAULT_MAX_FLOWS);
}

int main(int argc, char **argv)
{
	const char *ifname = DEFAULT_INTERFACE;
	const char *model_path = DEFAULT_MODEL_PATH;
	int max_flows = DEFAULT_MAX_FLOWS;
	bool debug = false;
	int opt;

	while ((opt = getopt(argc, argv, "i:m:n:dh")) != -1) {
		switch (opt) {
		case 'i': ifname = optarg; break;
		case 'm': model_path = optarg; break;
		case 'n': max_flows = atoi(optarg); break;
		case 'd': debug = true; break;
		case 'h':
		default:
			usage(argv[0]);
			return (opt == 'h') ? 0 : 1;
		}
	}

	openlog("traffic-classifier", LOG_PID | (debug ? LOG_PERROR : 0), LOG_DAEMON);
	syslog(LOG_INFO, "starting (iface=%s, max_flows=%d)", ifname, max_flows);

	uloop_init();

	struct ubus_context *ubus = ubus_connect(NULL);
	if (!ubus) {
		syslog(LOG_ERR, "failed to connect to ubus");
		return 1;
	}
	ubus_add_uloop(ubus);

	struct tc_daemon *d = &daemon_ctx;
	memset(d, 0, sizeof(*d));

	d->ft = flow_table_create(max_flows);
	if (!d->ft) {
		syslog(LOG_ERR, "failed to create flow table");
		return 1;
	}

	d->cls = classifier_init(model_path);
	if (!d->cls) {
		syslog(LOG_ERR, "failed to init classifier");
		return 1;
	}

	d->sta = sta_tracker_init(ubus);
	if (!d->sta) {
		syslog(LOG_ERR, "failed to init sta tracker");
		return 1;
	}

	d->cap = capture_init(ifname, d->ft);
	if (!d->cap) {
		syslog(LOG_ERR, "failed to init capture on %s", ifname);
		return 1;
	}

	d->ubus_ctx.ubus = ubus;
	d->ubus_ctx.ft = d->ft;
	d->ubus_ctx.classifier = d->cls;
	d->ubus_ctx.sta = d->sta;

	if (tc_ubus_init(&d->ubus_ctx) != 0) {
		syslog(LOG_ERR, "failed to register ubus object");
		return 1;
	}

	d->capture_fd.fd = capture_get_fd(d->cap);
	d->capture_fd.cb = capture_fd_cb;
	uloop_fd_add(&d->capture_fd, ULOOP_READ);

	d->classify_timer.cb = classify_timer_cb;
	uloop_timeout_set(&d->classify_timer, CLASSIFY_INTERVAL_MS);

	d->expire_timer.cb = expire_timer_cb;
	uloop_timeout_set(&d->expire_timer, EXPIRE_INTERVAL_MS);

	d->sta_timer.cb = sta_timer_cb;
	uloop_timeout_set(&d->sta_timer, STA_REFRESH_MS);

	sta_tracker_refresh(d->sta);

	syslog(LOG_INFO, "daemon ready, entering main loop");
	uloop_run();

	tc_ubus_cleanup(&d->ubus_ctx);
	capture_destroy(d->cap);
	classifier_destroy(d->cls);
	sta_tracker_destroy(d->sta);
	flow_table_destroy(d->ft);
	ubus_free(ubus);
	uloop_done();

	syslog(LOG_INFO, "shutdown complete");
	closelog();

	return 0;
}
