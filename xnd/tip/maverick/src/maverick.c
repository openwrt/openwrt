/* SPDX-License-Identifier: BSD-3-Clause */

#include <linux/limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <libubox/uloop.h>
#include <libubox/ulog.h>

static void
maverick_cb(struct uloop_timeout *delay)
{
	ULOG_INFO("triggering maverick check");
	if (system("/usr/libexec/ucentral/maverick.sh"))
		ULOG_ERR("failed to launch Maverick");
	uloop_end();
	return;
}

static struct uloop_timeout maverick = {
	.cb = maverick_cb,
};

int
main(int argc, char **argv)
{
	ulog_open(ULOG_STDIO | ULOG_SYSLOG, LOG_DAEMON, "maverick");

	uloop_init();
	uloop_timeout_set(&maverick, 300 * 1000);
	uloop_run();
	uloop_done();

	return 0;
}
