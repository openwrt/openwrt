// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2022 Felix Fietkau <nbd@nbd.name>
 */
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include "dhcpsnoop.h"

int dhcpsnoop_run_cmd(char *cmd, bool ignore_error)
{
	char *argv[] = { "sh", "-c", cmd, NULL };
	bool first = true;
	int status = -1;
	char buf[512];
	int fds[2];
	FILE *f;
	int pid;

	if (pipe(fds))
		return -1;

	pid = fork();
	if (!pid) {
		close(fds[0]);
		if (fds[1] != STDOUT_FILENO)
			dup2(fds[1], STDOUT_FILENO);
		if (fds[1] != STDERR_FILENO)
			dup2(fds[1], STDERR_FILENO);
		if (fds[1] > STDERR_FILENO)
			close(fds[1]);
		execv("/bin/sh", argv);
		exit(1);
	}

	if (pid < 0)
		return -1;

	close(fds[1]);
	f = fdopen(fds[0], "r");
	if (!f) {
		close(fds[0]);
		goto out;
	}

	while (fgets(buf, sizeof(buf), f) != NULL) {
		if (!strlen(buf))
			break;
		if (ignore_error)
			continue;
		if (first) {
			ULOG_WARN("Command: %s\n", cmd);
			first = false;
		}
		ULOG_WARN("%s%s", buf, strchr(buf, '\n') ? "" : "\n");
	}

	fclose(f);

out:
	while (waitpid(pid, &status, 0) < 0)
		if (errno != EINTR)
			break;

	return status;
}

int main(int argc, char **argv)
{
	ulog_open(ULOG_STDIO | ULOG_SYSLOG, LOG_DAEMON, "udhcpsnoop");
	uloop_init();
	dhcpsnoop_ubus_init();
	dhcpsnoop_dev_init();

	ulog_threshold(LOG_INFO);
	uloop_run();

	dhcpsnoop_ubus_done();
	dhcpsnoop_dev_done();
	uloop_done();

	return 0;
}

