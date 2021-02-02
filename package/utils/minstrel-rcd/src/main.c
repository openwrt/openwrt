// SPDX-License-Identifier: GPL-2.0
/* Copyright (C) 2021 Felix Fietkau <nbd@nbd.name> */
#include <libubox/usock.h>
#include <libubox/ustream.h>
#include "rcd.h"

int main(int argc, char **argv)
{
	int ch;

	uloop_init();

	while ((ch = getopt(argc, argv, "h:")) != -1) {
		switch (ch) {
		case 'h':
			rcd_server_add(optarg);
			break;
		}
	}

	rcd_phy_init();
	rcd_server_init();
	uloop_run();

	uloop_end();

	return 0;
}
