/*
 * wlc - tool for configuring the Broadcom Wireless Network Adapter
 * Copyright (C) 2005 Felix Fietkau <nbd@vd-s.ath.cx>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * $Id$
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <typedefs.h>
#include <wlutils.h>

int _argc;
char **_argv;

void qtxpwr() {
	int i = 0;
		
	if (_argc > 3) {
		if (_argv[3][0] == '@') {
			i = atoi(_argv[3] + 1);
			i |= 0x80000000;
		} else {
			i = atoi(_argv[3]);
		}
		wl_set_val(_argv[1], "qtxpower", &i, sizeof(i));
	} else {
		wl_get_val(_argv[1], "qtxpower", &i, sizeof(i));
		if ((i & 0x80000000) == 0x80000000)
			printf("@");
		printf("%d\n", i & 0x7ffffff);
	}
}


struct wl_config {
	char *name, *desc;
	enum {
		INT,
		BOOL,
		OTHER
	} type;
	int get, set;
	int r1, r2;
	void *handler;
};

struct wl_config commands[] = {
	{"txpwr", "transmit power (in mW)", INT, WLC_GET_TXPWR, WLC_SET_TXPWR, 0, 255, NULL},
	{"qtxpwr", "\ttransmit power (in qdbm)\n\t\t\t\"@\" before value means regulatory override", OTHER, 0, 0, 0, 0, qtxpwr},
	{"promisc", "promiscuous mode", BOOL, WLC_GET_PROMISC, WLC_SET_PROMISC, 0, 0, NULL},
	{"monitor", "monitor mode", BOOL, WLC_GET_MONITOR, WLC_SET_MONITOR, 0, 0, NULL},
	{"passive", "passive mode", BOOL, WLC_GET_PASSIVE, WLC_SET_PASSIVE, 0, 0, NULL},
	{"ap", "access point mode (0 = STA, 1 = AP)", BOOL, WLC_GET_AP, WLC_SET_AP, 0, 0, NULL},
	{"infra", "infrastructure mode (0 = IBSS, 1 = Infra BSS)", BOOL, WLC_GET_INFRA, WLC_SET_INFRA, 0, 0, NULL},
	{"antdiv", "rx antenna diversity (0 = antenna 0, 1 = antenna 1, 3 = auto select)", INT, WLC_GET_ANTDIV, WLC_SET_ANTDIV, 0, 3, NULL},
	{"txant", "set tx antenna (0 = antenna 0, 1 = antenna 1, 3 = rx antenna)", INT, WLC_GET_TXANT, WLC_SET_TXANT, 0, 3, NULL},
	{"wet", "wireless ethernet bridging mode", BOOL, WLC_GET_WET, WLC_SET_WET, 0, 0, NULL},
	{"channel", "set channel", INT, WLC_GET_CHANNEL, WLC_SET_CHANNEL, 1, 14, NULL},
	{NULL, NULL, 0, 0, 0, 0, 0, NULL}
};

void set_int(char *name, int ioctl, int value, int r1, int r2)
{
	if ((value >= r1) || (value <= r2)) {
		wl_ioctl(name, ioctl, &value, sizeof(value));
	} else
		fprintf(stderr, "invalid value.\n");
}

void get_int(char *name, int ioctl)
{
	int value = 0;
	wl_ioctl(name, ioctl, &value, sizeof(value));
	printf("%d\n", value);
}

int main(int argc, char **argv)
{
	struct wl_config *cmd; 
	int i;

	_argc = argc;
	_argv = argv;

	if (argc < 3) {
		fprintf(stderr, "Usage: %s <interface> <command> [...]\n\n", argv[0]);
		fprintf(stderr, "Commands:\n\n");
		
		cmd = commands;
		while (cmd->name != NULL) {
			fprintf(stderr, "\t%s", cmd->name);
			switch (cmd->type) {
				case INT:
					fprintf(stderr, " [%d-%d]", cmd->r1, cmd->r2);
					break;
				case BOOL:
					fprintf(stderr, " [0|1]");
					cmd->r1 = 0;
					cmd->r2 = 1;
					
			}
			fprintf(stderr, "\t%s\n", cmd->desc);
			cmd++;
		}
		fprintf(stderr, "\n");
		
		return -1;
	}

	if (wl_probe(argv[1]) < 0) {
		fprintf(stderr, "No broadcom extensions detected on interface %s\n", argv[1]);
		return -1;
	}

	if ((argc > 4) && (strcmp(argv[2], "ioctl") == 0)) {
		if (strcmp(argv[3], "int") == 0) {
			int ioctl = atoi(argv[4]);
			if (argc > 5)
				i = atoi(argv[5]);
			else
				i = 0;
			fprintf(stderr, "ioctl = 0x%x (%d)\nold value = 0x%x (%d)\n", ioctl, ioctl, i, i);
			wl_ioctl(argv[1], ioctl, &i, sizeof(i));
			fprintf(stderr, "new value = 0x%x (%d)\n", i, i);
		} else if (strcmp(argv[3], "intval") == 0) {
			i = 0;
			fprintf(stderr, "var = \"%s\"\nold value = 0x%x (%d)\n", argv[4], i, i);
			if (argc > 5) {
				i = atoi(argv[5]);
				wl_set_val(argv[1], argv[4], &i, sizeof(i));
			} else {
				wl_get_val(argv[1], argv[4], &i, sizeof(i));
			}
			fprintf(stderr, "new value = 0x%x (%d)\n", i, i);
		}
		return 0;
	}
	cmd = commands;
	while (cmd->name != NULL) {
		if (strcmp(argv[2], cmd->name) == 0) {
			switch (cmd->type) {
				case INT:
				case BOOL:
					if (argc < 4) {
						get_int(argv[1], cmd->get);
					} else {
						set_int(argv[1], cmd->set, atoi(argv[3]), cmd->r1, cmd->r2);
					}
				break;
				case OTHER: {
					void (*handler)(void) = cmd->handler;
					handler();
				}
				break;
			}
		}
		cmd++;
	}
	
	
	return 0;
}
